import type { TravelImage } from '../types/travel-record';
import { ensurePrivacyAuthorization } from './privacy.service';

const MAX_IMAGE_BYTES = 20 * 1024 * 1024;
const MAX_VIDEO_BYTES = 100 * 1024 * 1024;

function now(): string {
  return new Date().toISOString();
}

export function imageUrl(image: TravelImage): string {
  return image.localPath || image.tempPath || image.fileID;
}

export async function chooseLocalMedia(limit: number): Promise<TravelImage[]> {
  await ensurePrivacyAuthorization();
  const result = await wx.chooseMedia({
    count: Math.min(limit, 9),
    mediaType: ['image', 'video'],
    sourceType: ['album', 'camera'],
    sizeType: ['compressed'],
  });

  const files = result.tempFiles || [];
  const images: TravelImage[] = [];

  for (const file of files) {
    const isVideo = file.fileType === 'video';
    const maxBytes = isVideo ? MAX_VIDEO_BYTES : MAX_IMAGE_BYTES;
    if (file.size > maxBytes) {
      throw new Error(isVideo ? 'VIDEO_TOO_LARGE' : 'IMAGE_TOO_LARGE');
    }

    const tempPath = file.tempFilePath;
    let localPath = tempPath;

    try {
      const saved = await wx.saveFile({ tempFilePath: tempPath });
      localPath = saved.savedFilePath;
    } catch {
      localPath = tempPath;
    }

    images.push({
      fileID: localPath,
      tempPath,
      localPath,
      mediaType: isVideo ? 'video' : 'image',
      size: file.size,
      duration: file.duration,
      uploadedAt: now(),
    });
  }

  return images;
}

export async function chooseLocalImages(limit: number): Promise<TravelImage[]> {
  return chooseLocalMedia(limit);
}

export function removeSavedLocalFiles(images: TravelImage[]): void {
  const manager = wx.getFileSystemManager ? wx.getFileSystemManager() : undefined;
  if (!manager) return;

  images.forEach((image) => {
    const path = image.localPath;
    if (!path) return;
    try {
      manager.unlinkSync(path);
    } catch {
      // Local file cleanup is best effort. The record itself has already been removed.
    }
  });
}
