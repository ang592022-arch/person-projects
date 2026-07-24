import { cloudConfig } from '../config/env';

export interface CloudRuntimeStatus {
  mode: 'local' | 'cloud';
  ready: boolean;
  reason: 'LOCAL_MODE' | 'READY' | 'MISSING_ENV_ID' | 'CLOUD_API_UNAVAILABLE';
}

export function getCloudRuntimeStatus(): CloudRuntimeStatus {
  if (cloudConfig.storageMode !== 'cloud') {
    return { mode: 'local', ready: false, reason: 'LOCAL_MODE' };
  }

  if (!cloudConfig.envId.trim()) {
    return { mode: 'cloud', ready: false, reason: 'MISSING_ENV_ID' };
  }

  if (!wx.cloud) {
    return { mode: 'cloud', ready: false, reason: 'CLOUD_API_UNAVAILABLE' };
  }

  return { mode: 'cloud', ready: true, reason: 'READY' };
}

export function isCloudMode(): boolean {
  return cloudConfig.storageMode === 'cloud';
}

export function isCloudRuntimeReady(): boolean {
  return getCloudRuntimeStatus().ready;
}

export function assertCloudRuntimeReady(): void {
  const status = getCloudRuntimeStatus();
  if (status.ready) return;

  if (status.reason === 'MISSING_ENV_ID') {
    throw new Error('CLOUD_ENV_ID_MISSING');
  }

  if (status.reason === 'CLOUD_API_UNAVAILABLE') {
    throw new Error('CLOUD_API_UNAVAILABLE');
  }

  throw new Error('CLOUD_MODE_DISABLED');
}
