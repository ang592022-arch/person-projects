import type { TravelImage } from '../types/travel-record';

const PENDING_CLOUD_IMAGE_CLEANUP_KEY = 'travel-map:pending-cloud-image-cleanup';

interface MediaFunctionResult {
  uploadPrefix?: string;
  failedFileIDs?: string[];
  error?: string;
}

function now(): string {
  return new Date().toISOString();
}

function fileNameFromPath(path: string): string {
  return path.split('/').pop() || `${Date.now()}.jpg`;
}

function safePathSegment(value: string): string {
  return value.replace(/[^a-zA-Z0-9._-]/g, '_').slice(0, 80) || 'unknown';
}

function readPendingCleanup(): string[] {
  try {
    const value = wx.getStorageSync(PENDING_CLOUD_IMAGE_CLEANUP_KEY);
    return Array.isArray(value)
      ? value.filter((fileID): fileID is string => typeof fileID === 'string' && fileID.startsWith('cloud://'))
      : [];
  } catch {
    return [];
  }
}

function writePendingCleanup(fileIDs: string[]): void {
  const uniqueFileIDs = Array.from(new Set(fileIDs));
  if (uniqueFileIDs.length === 0) {
    wx.removeStorageSync(PENDING_CLOUD_IMAGE_CLEANUP_KEY);
    return;
  }
  wx.setStorageSync(PENDING_CLOUD_IMAGE_CLEANUP_KEY, uniqueFileIDs);
}

function enqueuePendingCleanup(fileIDs: string[]): void {
  try {
    writePendingCleanup([...readPendingCleanup(), ...fileIDs]);
  } catch {
    // A failed local queue write must not hide the original cloud deletion error.
  }
}

async function deleteCloudFileIDs(fileList: string[]): Promise<string[]> {
  const result = await wx.cloud.callFunction({
    name: 'ocrImport',
    data: { action: 'deleteMedia', fileList },
  });
  const data = result.result as MediaFunctionResult;
  if (data?.error) throw new Error(data.error);
  return Array.isArray(data?.failedFileIDs) ? data.failedFileIDs : [];
}

async function getMediaUploadPrefix(): Promise<string> {
  const result = await wx.cloud.callFunction({
    name: 'ocrImport',
    data: { action: 'prepareMedia' },
  });
  const data = result.result as MediaFunctionResult;
  const uploadPrefix = String(data?.uploadPrefix || '');
  if (data?.error || !/^travel-images\/[A-Za-z0-9_-]+\/$/.test(uploadPrefix)) {
    throw new Error(data?.error || 'MEDIA_UPLOAD_PREFIX_INVALID');
  }
  return uploadPrefix;
}

export async function uploadTravelImages(regionCode: string, images: TravelImage[]): Promise<TravelImage[]> {
  if (!wx.cloud) {
    throw new Error('当前环境没有启用微信云开发');
  }

  const uploaded: TravelImage[] = [];
  const newlyUploaded: TravelImage[] = [];
  const needsUpload = images.some((image) => !image.fileID || !image.fileID.startsWith('cloud://'));
  const uploadPrefix = needsUpload ? await getMediaUploadPrefix() : '';

  try {
    for (const image of images) {
      if (image.fileID && image.fileID.startsWith('cloud://')) {
        uploaded.push(image);
        continue;
      }

      const localPath = image.localPath || image.tempPath || image.fileID;
      const cloudPath = [
        uploadPrefix.replace(/\/$/, ''),
        safePathSegment(regionCode),
        `${Date.now()}-${Math.random().toString(36).slice(2)}-${safePathSegment(fileNameFromPath(localPath))}`,
      ].join('/');
      const result = await wx.cloud.uploadFile({
        cloudPath,
        filePath: localPath,
      });

      const cloudImage = {
        ...image,
        fileID: result.fileID,
        cloudPath,
        tempPath: undefined,
        localPath: undefined,
        uploadedAt: image.uploadedAt || now(),
      };
      uploaded.push(cloudImage);
      newlyUploaded.push(cloudImage);
    }
  } catch (error) {
    await deleteCloudImages(newlyUploaded).catch(() => undefined);
    throw error;
  }

  return uploaded;
}

export async function deleteCloudImages(images: TravelImage[]): Promise<void> {
  if (!wx.cloud) return;

  const fileList = images
    .map((image) => image.fileID)
    .filter((fileID) => fileID && fileID.startsWith('cloud://'));

  if (fileList.length === 0) return;

  let failedFileIDs: string[];
  try {
    failedFileIDs = await deleteCloudFileIDs(fileList);
  } catch (error) {
    enqueuePendingCleanup(fileList);
    throw error;
  }

  if (failedFileIDs.length > 0) {
    enqueuePendingCleanup(failedFileIDs);
    throw new Error('CLOUD_IMAGE_DELETE_PARTIAL_FAILURE');
  }
}

export async function retryPendingCloudImageCleanup(): Promise<number> {
  if (!wx.cloud) return 0;

  const pendingFileIDs = readPendingCleanup();
  if (pendingFileIDs.length === 0) return 0;

  let failedFileIDs: string[];
  try {
    failedFileIDs = await deleteCloudFileIDs(pendingFileIDs);
  } catch (error) {
    enqueuePendingCleanup(pendingFileIDs);
    throw error;
  }

  writePendingCleanup(failedFileIDs);
  return pendingFileIDs.length - failedFileIDs.length;
}
