import type { TravelImage } from '../types/travel-record';
import { assertCloudRuntimeReady, isCloudMode, isCloudRuntimeReady } from './cloud-runtime.service';
import { deleteCloudImages, uploadTravelImages } from './cloud-image.service';
import { chooseLocalMedia, imageUrl, removeSavedLocalFiles } from './local-image.service';

export { imageUrl };

export function isCloudMediaEnabled(): boolean {
  return isCloudRuntimeReady();
}

function useCloudMedia(): boolean {
  if (!isCloudMode()) return false;
  assertCloudRuntimeReady();
  return true;
}

export async function chooseTravelMedia(limit: number): Promise<TravelImage[]> {
  return chooseLocalMedia(limit);
}

export async function prepareTravelImages(regionCode: string, images: TravelImage[]): Promise<TravelImage[]> {
  if (!useCloudMedia()) return images;
  return uploadTravelImages(regionCode, images);
}

export async function removeTravelImages(images: TravelImage[]): Promise<void> {
  try {
    if (useCloudMedia()) {
      await deleteCloudImages(images);
    }
  } finally {
    removeSavedLocalFiles(images);
  }
}

export async function rollbackPreparedImages(
  originalImages: TravelImage[],
  preparedImages: TravelImage[],
): Promise<void> {
  if (!isCloudRuntimeReady()) return;

  const originalFileIds = new Set(originalImages.map((image) => image.fileID));
  const newlyUploaded = preparedImages.filter((image) => (
    image.fileID?.startsWith('cloud://') && !originalFileIds.has(image.fileID)
  ));
  if (!newlyUploaded.length) return;

  await deleteCloudImages(newlyUploaded);
}

export function removeTemporaryLocalImages(images: TravelImage[]): void {
  removeSavedLocalFiles(images);
}
