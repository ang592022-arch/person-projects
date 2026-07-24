import { cloudConfig } from '../config/env';
import type { UserRegionState } from '../types/region';
import type { TravelRecord, TravelRecordInput } from '../types/travel-record';

type CloudRecord<T> = T & { _id: string; _openid?: string };

const PENDING_REGION_REFRESH_KEY = 'travel-map:pending-cloud-region-refresh';

function db() {
  if (!wx.cloud || !cloudConfig.envId) {
    throw new Error('请先在 miniprogram/config/env.ts 中配置云开发环境 ID');
  }

  return wx.cloud.database();
}

function now(): string {
  return new Date().toISOString();
}

function readPendingRegionRefreshes(): string[] {
  try {
    const value = wx.getStorageSync(PENDING_REGION_REFRESH_KEY);
    return Array.isArray(value) ? value.filter((item): item is string => typeof item === 'string' && Boolean(item)) : [];
  } catch {
    return [];
  }
}

function writePendingRegionRefreshes(regionCodes: string[]): void {
  const uniqueRegionCodes = Array.from(new Set(regionCodes));
  if (uniqueRegionCodes.length === 0) {
    wx.removeStorageSync(PENDING_REGION_REFRESH_KEY);
    return;
  }
  wx.setStorageSync(PENDING_REGION_REFRESH_KEY, uniqueRegionCodes);
}

function enqueueRegionRefresh(regionCode: string): void {
  try {
    writePendingRegionRefreshes([...readPendingRegionRefreshes(), regionCode]);
  } catch {
    // Keep the completed record operation successful even if local retry metadata cannot be written.
  }
}

async function refreshRegionStateEventually(regionCode: string): Promise<void> {
  try {
    await refreshCloudRegionStateFromRecords(regionCode);
  } catch {
    enqueueRegionRefresh(regionCode);
  }
}

function recordFromCloud(record: CloudRecord<TravelRecord>): TravelRecord {
  return {
    ...record,
    id: record.id || record._id,
    tickets: record.tickets || [],
    companions: record.companions || [],
    visibility: record.visibility || 'private',
  };
}

async function fetchAll<T>(query: any): Promise<T[]> {
  const pageSize = 100;
  let offset = 0;
  const all: T[] = [];

  while (true) {
    const result = await query.skip(offset).limit(pageSize).get();
    const data = (result.data || []) as T[];
    all.push(...data);
    if (data.length < pageSize) break;
    offset += pageSize;
  }

  return all;
}

export async function getCloudRegionStates(): Promise<Record<string, UserRegionState>> {
  const rows = await fetchAll<CloudRecord<UserRegionState>>(db().collection(cloudConfig.collections.regionStates));
  const states: Record<string, UserRegionState> = {};

  rows.forEach((state) => {
    states[state.regionCode] = state;
  });

  return states;
}

export async function getCloudRegionState(regionCode: string): Promise<UserRegionState | undefined> {
  const result = await db()
    .collection(cloudConfig.collections.regionStates)
    .where({ regionCode })
    .limit(1)
    .get();

  return (result.data as UserRegionState[])[0];
}

export async function setCloudRegionLit(regionCode: string, lit: boolean): Promise<UserRegionState> {
  const collection = db().collection(cloudConfig.collections.regionStates);
  const existing = await getCloudRegionState(regionCode);
  const next: UserRegionState = {
    regionCode,
    lit,
    firstVisitDate: existing?.firstVisitDate,
    lastVisitDate: existing?.lastVisitDate,
    visitCount: existing?.visitCount || 0,
    tags: existing?.tags || [],
    coverImage: existing?.coverImage,
    updatedAt: now(),
  };

  if (existing) {
    await collection.doc((existing as CloudRecord<UserRegionState>)._id).update({ data: next });
  } else {
    await collection.add({ data: next });
  }

  return next;
}

export async function getCloudTravelRecords(regionCode?: string): Promise<TravelRecord[]> {
  const collection = db().collection(cloudConfig.collections.travelRecords);
  const query = regionCode ? collection.where({ regionCode }) : collection;
  const rows = await fetchAll<CloudRecord<TravelRecord>>(query);

  return rows
    .map(recordFromCloud)
    .sort((a, b) => b.visitDate.localeCompare(a.visitDate));
}

export async function getCloudTravelRecord(recordId: string): Promise<TravelRecord | undefined> {
  const result = await db()
    .collection(cloudConfig.collections.travelRecords)
    .doc(recordId)
    .get();

  return result.data ? recordFromCloud(result.data as CloudRecord<TravelRecord>) : undefined;
}

export async function saveCloudTravelRecord(input: TravelRecordInput): Promise<TravelRecord> {
  const collection = db().collection(cloudConfig.collections.travelRecords);
  const timestamp = now();
  const existing = input.id ? await getCloudTravelRecord(input.id) : undefined;
  const record: TravelRecord = {
    id: input.id || '',
    regionCode: input.regionCode,
    visitDate: input.visitDate,
    title: input.title || '',
    content: input.content,
    tags: input.tags,
    companions: input.companions || existing?.companions || [],
    visibility: input.visibility || existing?.visibility || 'private',
    images: input.images,
    tickets: input.tickets || existing?.tickets || [],
    createdAt: existing?.createdAt || timestamp,
    updatedAt: timestamp,
  };

  if (input.id && existing) {
    await collection.doc(input.id).update({ data: record });
    await refreshRegionStateEventually(input.regionCode);
    return record;
  }

  const added = await collection.add({ data: record });
  const next = {
    ...record,
    id: added._id,
  };
  await refreshRegionStateEventually(input.regionCode);
  return next;
}

export async function deleteCloudTravelRecord(recordId: string): Promise<TravelRecord | undefined> {
  const existing = await getCloudTravelRecord(recordId);
  if (!existing) return undefined;

  await db()
    .collection(cloudConfig.collections.travelRecords)
    .doc(recordId)
    .remove();
  await refreshRegionStateEventually(existing.regionCode);
  return existing;
}

export async function retryPendingCloudRegionRefreshes(): Promise<number> {
  const pendingRegionCodes = readPendingRegionRefreshes();
  if (pendingRegionCodes.length === 0) return 0;

  const failedRegionCodes: string[] = [];
  let refreshed = 0;
  for (const regionCode of pendingRegionCodes) {
    try {
      await refreshCloudRegionStateFromRecords(regionCode);
      refreshed += 1;
    } catch {
      failedRegionCodes.push(regionCode);
    }
  }

  writePendingRegionRefreshes(failedRegionCodes);
  return refreshed;
}

export async function refreshCloudRegionStateFromRecords(regionCode: string): Promise<void> {
  const records = (await getCloudTravelRecords(regionCode)).sort((a, b) => a.visitDate.localeCompare(b.visitDate));
  const existing = await getCloudRegionState(regionCode);
  const collection = db().collection(cloudConfig.collections.regionStates);
  const next: UserRegionState = {
    regionCode,
    lit: existing?.lit || records.length > 0,
    firstVisitDate: records[0]?.visitDate,
    lastVisitDate: records[records.length - 1]?.visitDate,
    visitCount: records.length,
    tags: existing?.tags || [],
    coverImage: existing?.coverImage,
    updatedAt: now(),
  };

  if (existing) {
    await collection.doc((existing as CloudRecord<UserRegionState>)._id).update({ data: next });
  } else {
    await collection.add({ data: next });
  }
}
