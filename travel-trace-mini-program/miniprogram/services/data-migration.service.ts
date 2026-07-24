import { cloudConfig } from '../config/env';
import { getCustomWorldCityRegions } from './custom-world-city-store';
import { getAllRegionStates as getLocalAllRegionStates, getTravelRecords as getLocalTravelRecords } from './local-travel-store';
import { getUserPreferences } from './theme-store';
import { prepareTravelImages, rollbackPreparedImages } from './travel-media.service';
import { getCloudTravelRecords, saveCloudTravelRecord, setCloudRegionLit } from './cloud-travel-store';
import type { UserPreferences } from '../types/theme';
import type { TravelImage, TravelRecord } from '../types/travel-record';
import type { WorldCityRegion } from '../data/regions/world-cities';

export interface MigrationReport {
  ok: boolean;
  regionStates: number;
  travelRecords: number;
  skippedRecords: number;
  mediaFiles: number;
  failedMediaFiles: number;
  customWorldCities: number;
  preferences: number;
  errors: string[];
}

function emptyReport(): MigrationReport {
  return {
    ok: false,
    regionStates: 0,
    travelRecords: 0,
    skippedRecords: 0,
    mediaFiles: 0,
    failedMediaFiles: 0,
    customWorldCities: 0,
    preferences: 0,
    errors: [],
  };
}

function db() {
  if (!wx.cloud || !cloudConfig.envId) {
    throw new Error('请先配置云开发环境 ID，并把 storageMode 切换为 cloud');
  }

  return wx.cloud.database();
}

function isCloudReady(): boolean {
  return cloudConfig.storageMode === 'cloud' && Boolean(cloudConfig.envId) && Boolean(wx.cloud);
}

function recordSignature(record: Pick<TravelRecord, 'regionCode' | 'visitDate' | 'title' | 'content'>): string {
  return [
    record.regionCode,
    record.visitDate,
    record.title || '',
    record.content || '',
  ].join('|');
}

async function upsertCustomWorldCity(city: WorldCityRegion): Promise<boolean> {
  const collection = db().collection(cloudConfig.collections.customWorldCities);
  const existing = await collection
    .where({ regionCode: city.regionCode })
    .limit(1)
    .get();

  if (existing.data?.[0]?._id) {
    await collection.doc(existing.data[0]._id).update({ data: city });
    return false;
  }

  await collection.add({ data: city });
  return true;
}

async function upsertUserPreferences(preferences: UserPreferences): Promise<boolean> {
  const collection = db().collection(cloudConfig.collections.userPreferences);
  const existing = await collection
    .limit(1)
    .get();
  const data = {
    ...preferences,
    updatedAt: new Date().toISOString(),
  };

  if (existing.data?.[0]?._id) {
    await collection.doc(existing.data[0]._id).update({ data });
    return false;
  }

  await collection.add({ data });
  return true;
}

async function uploadRecordImages(
  record: TravelRecord,
  report: MigrationReport,
): Promise<TravelImage[] | undefined> {
  if (!record.images.length) return [];

  try {
    return await prepareTravelImages(record.regionCode, record.images);
  } catch (error) {
    report.failedMediaFiles += record.images.length;
    report.errors.push(`${record.title || record.regionCode} 的媒体上传失败`);
    return undefined;
  }
}

export function canMigrateLocalDataToCloud(): boolean {
  return isCloudReady();
}

export async function migrateLocalDataToCloud(): Promise<MigrationReport> {
  const report = emptyReport();
  if (!isCloudReady()) {
    report.errors.push('请先在 env.ts 填写 envId，并把 storageMode 改成 cloud');
    return report;
  }

  try {
    const localStates = Object.values(getLocalAllRegionStates());
    const localRecords = getLocalTravelRecords();
    const existingCloudRecords = await getCloudTravelRecords();
    const existingSignatures = new Set(existingCloudRecords.map(recordSignature));

    for (const record of localRecords) {
      const signature = recordSignature(record);
      if (existingSignatures.has(signature)) {
        report.skippedRecords += 1;
        continue;
      }

      const images = await uploadRecordImages(record, report);
      if (!images) continue;

      try {
        await saveCloudTravelRecord({
          regionCode: record.regionCode,
          visitDate: record.visitDate,
          title: record.title,
          content: record.content,
          tags: record.tags,
          companions: record.companions || [],
          visibility: record.visibility || 'private',
          images,
          tickets: record.tickets || [],
        });
      } catch (error) {
        await rollbackPreparedImages(record.images, images).catch(() => undefined);
        report.errors.push(`${record.title || record.regionCode} 的云端记录保存失败`);
        continue;
      }

      report.mediaFiles += images.filter((image) => image.fileID?.startsWith('cloud://')).length;
      existingSignatures.add(signature);
      report.travelRecords += 1;
    }

    for (const state of localStates.filter((item) => item.lit)) {
      await setCloudRegionLit(state.regionCode, true);
      report.regionStates += 1;
    }

    for (const city of getCustomWorldCityRegions()) {
      const inserted = await upsertCustomWorldCity(city);
      if (inserted) report.customWorldCities += 1;
    }

    const preferencesInserted = await upsertUserPreferences(getUserPreferences());
    report.preferences = preferencesInserted ? 1 : 0;
    report.ok = report.errors.length === 0;
    return report;
  } catch (error) {
    report.errors.push(error instanceof Error ? error.message : '迁移失败，请检查云环境和数据库权限');
    return report;
  }
}
