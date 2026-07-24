export class PrivacyAuthorizationError extends Error {
  code = 'PRIVACY_NOT_AUTHORIZED';

  constructor(message = 'PRIVACY_NOT_AUTHORIZED') {
    super(message);
    this.name = 'PrivacyAuthorizationError';
  }
}

function getPrivacySetting(): Promise<any> {
  if (typeof wx.getPrivacySetting !== 'function') {
    return Promise.resolve({ needAuthorization: false });
  }

  return new Promise((resolve, reject) => {
    wx.getPrivacySetting({
      success: resolve,
      fail: reject,
    });
  });
}

function requestPrivacyAuthorization(): Promise<void> {
  if (typeof wx.requirePrivacyAuthorize !== 'function') {
    // Older base libraries display the platform privacy dialog when the
    // protected API is called. Keep that compatible fallback intact.
    return Promise.resolve();
  }

  return new Promise((resolve, reject) => {
    wx.requirePrivacyAuthorize({
      success: () => resolve(),
      fail: (error: any) => reject(new PrivacyAuthorizationError(error?.errMsg)),
    });
  });
}

export async function ensurePrivacyAuthorization(): Promise<void> {
  let setting: any;
  try {
    setting = await getPrivacySetting();
  } catch (error) {
    throw new PrivacyAuthorizationError((error as any)?.errMsg);
  }

  if (!setting?.needAuthorization) return;
  await requestPrivacyAuthorization();
}

export function isPrivacyAuthorizationError(error: unknown): boolean {
  return error instanceof PrivacyAuthorizationError
    || (error as any)?.code === 'PRIVACY_NOT_AUTHORIZED';
}
