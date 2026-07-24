const cloud = require('wx-server-sdk');
const crypto = require('crypto');
const https = require('https');

cloud.init({
  env: cloud.DYNAMIC_CURRENT_ENV,
});

const OCR_HOST = 'ocr.tencentcloudapi.com';
const OCR_SERVICE = 'ocr';
const OCR_VERSION = '2018-11-19';
const DEFAULT_ACTION = 'GeneralAccurateOCR';
const MAX_SOURCE_BYTES = 7.5 * 1024 * 1024;
const DEFAULT_DAILY_LIMIT = 30;
const DEFAULT_MONTHLY_LIMIT = 900;

function sha256(value) {
  return crypto.createHash('sha256').update(value).digest('hex');
}

function hmac(key, value, encoding) {
  return crypto.createHmac('sha256', key).update(value).digest(encoding);
}

function utcDate(timestamp) {
  return new Date(timestamp * 1000).toISOString().slice(0, 10);
}

function credentials() {
  const configured = {
    secretId: process.env.OCR_SECRET_ID || '',
    secretKey: process.env.OCR_SECRET_KEY || '',
    token: process.env.OCR_SESSION_TOKEN || '',
  };
  if (configured.secretId || configured.secretKey || configured.token) {
    return configured;
  }

  return {
    secretId: process.env.TENCENTCLOUD_SECRETID || '',
    secretKey: process.env.TENCENTCLOUD_SECRETKEY || '',
    token: process.env.TENCENTCLOUD_SESSIONTOKEN || '',
  };
}

function createAuthorization(payload, action, timestamp, credential) {
  const date = utcDate(timestamp);
  const canonicalHeaders = [
    'content-type:application/json; charset=utf-8',
    `host:${OCR_HOST}`,
    `x-tc-action:${action.toLowerCase()}`,
    '',
  ].join('\n');
  const signedHeaders = 'content-type;host;x-tc-action';
  const canonicalRequest = [
    'POST',
    '/',
    '',
    canonicalHeaders,
    signedHeaders,
    sha256(payload),
  ].join('\n');
  const credentialScope = `${date}/${OCR_SERVICE}/tc3_request`;
  const stringToSign = [
    'TC3-HMAC-SHA256',
    timestamp,
    credentialScope,
    sha256(canonicalRequest),
  ].join('\n');
  const secretDate = hmac(`TC3${credential.secretKey}`, date);
  const secretService = hmac(secretDate, OCR_SERVICE);
  const secretSigning = hmac(secretService, 'tc3_request');
  const signature = hmac(secretSigning, stringToSign, 'hex');

  return [
    'TC3-HMAC-SHA256',
    `Credential=${credential.secretId}/${credentialScope},`,
    `SignedHeaders=${signedHeaders},`,
    `Signature=${signature}`,
  ].join(' ');
}

function callTencentOcr(imageBase64) {
  const credential = credentials();
  if (!credential.secretId || !credential.secretKey) {
    const error = new Error('OCR credentials are not configured.');
    error.code = 'OCR_NOT_CONFIGURED';
    throw error;
  }

  const action = process.env.OCR_ACTION || DEFAULT_ACTION;
  const timestamp = Math.floor(Date.now() / 1000);
  const payload = JSON.stringify({ ImageBase64: imageBase64 });
  const headers = {
    Authorization: createAuthorization(payload, action, timestamp, credential),
    'Content-Type': 'application/json; charset=utf-8',
    Host: OCR_HOST,
    'X-TC-Action': action,
    'X-TC-Timestamp': String(timestamp),
    'X-TC-Version': OCR_VERSION,
  };
  if (credential.token) headers['X-TC-Token'] = credential.token;

  return new Promise((resolve, reject) => {
    const request = https.request({
      hostname: OCR_HOST,
      method: 'POST',
      path: '/',
      headers: {
        ...headers,
        'Content-Length': Buffer.byteLength(payload),
      },
      timeout: 15000,
    }, (response) => {
      const chunks = [];
      response.on('data', (chunk) => chunks.push(chunk));
      response.on('end', () => {
        try {
          const body = JSON.parse(Buffer.concat(chunks).toString('utf8'));
          const result = body.Response || {};
          if (result.Error) {
            const error = new Error(result.Error.Message || result.Error.Code);
            error.code = result.Error.Code || 'OCR_PROVIDER_ERROR';
            reject(error);
            return;
          }
          resolve(result);
        } catch (error) {
          reject(error);
        }
      });
    });
    request.on('timeout', () => request.destroy(new Error('OCR_TIMEOUT')));
    request.on('error', reject);
    request.end(payload);
  });
}

function positiveInteger(value, fallback) {
  const parsed = Number(value);
  return Number.isFinite(parsed) && parsed > 0 ? Math.floor(parsed) : fallback;
}

async function readQuotaDocument(document) {
  try {
    return (await document.get()).data || undefined;
  } catch (error) {
    if (String(error?.message || error).includes('does not exist')) return undefined;
    throw error;
  }
}

function quotaError(code, message) {
  const error = new Error(message);
  error.code = code;
  return error;
}

async function consumeOcrQuota(openid) {
  if (!openid) return;

  const dailyLimit = positiveInteger(process.env.OCR_DAILY_LIMIT, DEFAULT_DAILY_LIMIT);
  const monthlyLimit = positiveInteger(process.env.OCR_MONTHLY_LIMIT, DEFAULT_MONTHLY_LIMIT);
  const date = new Date().toISOString().slice(0, 10);
  const month = date.slice(0, 7);
  const database = cloud.database();
  const outcome = await database.runTransaction(async (transaction) => {
    const collection = transaction.collection('ocr_usage');
    const dailyDocument = collection.doc(`${date}_${openid}`);
    const monthlyDocument = collection.doc(`global_${month}`);
    const daily = await readQuotaDocument(dailyDocument);
    const monthly = await readQuotaDocument(monthlyDocument);
    const dailyCount = Number(daily?.count || 0);
    const monthlyCount = Number(monthly?.count || 0);

    if (dailyCount >= dailyLimit) return { error: 'OCR_DAILY_LIMIT' };
    if (monthlyCount >= monthlyLimit) return { error: 'OCR_MONTHLY_LIMIT' };

    const updatedAt = new Date();
    await dailyDocument.set({
      data: {
        openid,
        date,
        scope: 'user-day',
        count: dailyCount + 1,
        updatedAt,
      },
    });
    await monthlyDocument.set({
      data: {
        month,
        scope: 'global-month',
        count: monthlyCount + 1,
        updatedAt,
      },
    });
    return { ok: true };
  });

  if (outcome?.error === 'OCR_DAILY_LIMIT') {
    throw quotaError('OCR_DAILY_LIMIT', 'Daily OCR limit reached.');
  }
  if (outcome?.error === 'OCR_MONTHLY_LIMIT') {
    throw quotaError('OCR_MONTHLY_LIMIT', 'Monthly OCR budget limit reached.');
  }
}

async function removeSource(fileID) {
  if (!fileID || process.env.OCR_KEEP_SOURCE === 'true') return;
  await cloud.deleteFile({ fileList: [fileID] }).catch(() => undefined);
}

exports.main = async (event) => {
  const context = cloud.getWXContext();
  const openid = context.OPENID;
  if (!openid) {
    return { error: 'MISSING_OPENID', message: 'A WeChat user identity is required.' };
  }

  const uploadPrefix = `ocr-import/${openid}/`;
  const mediaUploadPrefix = `travel-images/${openid}/`;
  if (event && event.action === 'prepare') {
    return { uploadPrefix };
  }

  if (event && event.action === 'prepareMedia') {
    return { uploadPrefix: mediaUploadPrefix };
  }

  if (event && event.action === 'deleteMedia') {
    const fileList = Array.isArray(event.fileList) ? event.fileList : [];
    if (fileList.length === 0 || fileList.length > 100) {
      return { error: 'INVALID_FILE_LIST', message: 'Provide between 1 and 100 media file IDs.' };
    }
    if (fileList.some((fileID) => (
      typeof fileID !== 'string'
      || !fileID.startsWith('cloud://')
      || !fileID.includes(`/${mediaUploadPrefix}`)
    ))) {
      return { error: 'FILE_NOT_OWNED', message: 'Every media file must belong to the current user.' };
    }

    try {
      const deleted = await cloud.deleteFile({ fileList });
      const failedFileIDs = (deleted.fileList || [])
        .filter((item) => typeof item.status === 'number' && item.status !== 0)
        .map((item) => item.fileID)
        .filter(Boolean);
      return { failedFileIDs };
    } catch (error) {
      return {
        error: error.code || 'MEDIA_DELETE_FAILED',
        message: error.message || 'Media deletion failed.',
      };
    }
  }

  const fileID = event && event.fileID;
  if (!fileID || typeof fileID !== 'string' || !fileID.startsWith('cloud://')) {
    return { error: 'INVALID_FILE_ID', message: 'A CloudBase fileID is required.' };
  }
  if (!fileID.includes(`/${uploadPrefix}`)) {
    return { error: 'FILE_NOT_OWNED', message: 'The OCR source does not belong to the current user.' };
  }

  try {
    const downloaded = await cloud.downloadFile({ fileID });
    const source = downloaded.fileContent;
    if (!source || source.length === 0) {
      return { error: 'EMPTY_IMAGE', message: 'The uploaded image is empty.' };
    }
    if (source.length > MAX_SOURCE_BYTES) {
      return { error: 'IMAGE_TOO_LARGE', message: 'The image must be smaller than 7.5 MB.' };
    }

    await consumeOcrQuota(openid);
    const response = await callTencentOcr(source.toString('base64'));
    const lines = Array.isArray(response.TextDetections)
      ? response.TextDetections.map((item) => String(item.DetectedText || '').trim()).filter(Boolean)
      : [];
    const text = lines.join('\n');

    return {
      text,
      fullText: text,
      lineCount: lines.length,
      provider: 'tencent-cloud-ocr',
      requestId: response.RequestId || '',
    };
  } catch (error) {
    return {
      error: error.code || 'OCR_FAILED',
      message: error.message || 'OCR failed.',
    };
  } finally {
    await removeSource(fileID);
  }
};
