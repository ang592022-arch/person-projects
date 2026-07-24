import { assertCloudRuntimeReady, isCloudMode, isCloudRuntimeReady } from './cloud-runtime.service';
import type { UserRegionState } from '../types/region';
import type { TravelRecord, TravelRecordInput } from '../types/travel-record';
import {
  deleteTravelRecord as deleteLocalTravelRecord,
  getAllRegionStates as getLocalAllRegionStates,
  getRegionState as getLocalRegionState,
  getTravelRecord as getLocalTravelRecord,
  getTravelRecords as getLocalTravelRecords,
  saveTravelRecord as saveLocalTravelRecord,
  setRegionLit as setLocalRegionLit,
} from './local-travel-store';
import {
  deleteCloudTravelRecord,
  getCloudRegionState,
  getCloudRegionStates,
  getCloudTravelRecord,
  getCloudTravelRecords,
  saveCloudTravelRecord,
  setCloudRegionLit,
} from './cloud-travel-store';

function recordSignature(record: Pick<TravelRecord, 'regionCode' | 'visitDate' | 'title' | 'content'>): string {
  return [record.regionCode, record.visitDate, record.title || '', record.content || ''].join('|');
}

function mergeTravelRecords(localRecords: TravelRecord[], cloudRecords: TravelRecord[]): TravelRecord[] {
  const merged = new Map<string, TravelRecord>();
  const signatures = new Set<string>();

  cloudRecords.forEach((record) => {
    merged.set(record.id, record);
    signatures.add(recordSignature(record));
  });

  localRecords.forEach((record) => {
    if (merged.has(record.id) || signatures.has(recordSignature(record))) return;
    merged.set(record.id, record);
  });

  return Array.from(merged.values()).sort((a, b) => b.visitDate.localeCompare(a.visitDate));
}

function bestEffortLocalMirror(action: () => void): void {
  try {
    action();
  } catch {
    // The cloud write is authoritative; a cache failure must not turn it into a false failure.
  }
}

function mergeRegionState(
  localState?: UserRegionState,
  cloudState?: UserRegionState,
): UserRegionState | undefined {
  if (!localState) return cloudState;
  if (!cloudState) return localState;

  const firstVisitDates = [localState.firstVisitDate, cloudState.firstVisitDate].filter(Boolean) as string[];
  const lastVisitDates = [localState.lastVisitDate, cloudState.lastVisitDate].filter(Boolean) as string[];
  return {
    ...localState,
    ...cloudState,
    regionCode: cloudState.regionCode || localState.regionCode,
    lit: localState.lit || cloudState.lit,
    firstVisitDate: firstVisitDates.sort()[0],
    lastVisitDate: lastVisitDates.sort().reverse()[0],
    visitCount: Math.max(localState.visitCount || 0, cloudState.visitCount || 0),
    tags: Array.from(new Set([...(localState.tags || []), ...(cloudState.tags || [])])),
    coverImage: cloudState.coverImage || localState.coverImage,
    updatedAt: cloudState.updatedAt || localState.updatedAt,
  };
}

export function isCloudStorageEnabled(): boolean {
  return isCloudRuntimeReady();
}

function useCloudStorage(): boolean {
  if (!isCloudMode()) return false;
  assertCloudRuntimeReady();
  return true;
}

export async function getAllRegionStates(): Promise<Record<string, UserRegionState>> {
  if (!useCloudStorage()) return getLocalAllRegionStates();

  const localStates = getLocalAllRegionStates();
  try {
    const cloudStates = await getCloudRegionStates();
    const regionCodes = new Set([...Object.keys(localStates), ...Object.keys(cloudStates)]);
    const merged: Record<string, UserRegionState> = {};
    regionCodes.forEach((regionCode) => {
      const state = mergeRegionState(localStates[regionCode], cloudStates[regionCode]);
      if (state) merged[regionCode] = state;
    });
    return merged;
  } catch {
    return localStates;
  }
}

export async function getRegionState(regionCode: string): Promise<UserRegionState | undefined> {
  const localState = getLocalRegionState(regionCode);
  if (!useCloudStorage()) return localState;

  try {
    return mergeRegionState(localState, await getCloudRegionState(regionCode));
  } catch {
    return localState;
  }
}

export async function setRegionLit(regionCode: string, lit: boolean): Promise<UserRegionState> {
  if (!useCloudStorage()) return setLocalRegionLit(regionCode, lit);

  const cloudState = await setCloudRegionLit(regionCode, lit);
  bestEffortLocalMirror(() => setLocalRegionLit(regionCode, lit));
  return cloudState;
}

export async function getTravelRecords(regionCode?: string): Promise<TravelRecord[]> {
  const localRecords = getLocalTravelRecords(regionCode);
  if (!useCloudStorage()) return localRecords;

  try {
    return mergeTravelRecords(localRecords, await getCloudTravelRecords(regionCode));
  } catch {
    return localRecords;
  }
}

export async function getTravelRecord(recordId: string): Promise<TravelRecord | undefined> {
  const localRecord = getLocalTravelRecord(recordId);
  if (!useCloudStorage() || recordId.startsWith('local-')) return localRecord;

  try {
    return await getCloudTravelRecord(recordId) || localRecord;
  } catch {
    return localRecord;
  }
}

export async function saveTravelRecord(input: TravelRecordInput): Promise<TravelRecord> {
  if (!useCloudStorage()) return saveLocalTravelRecord(input);

  const legacyLocalId = input.id?.startsWith('local-') ? input.id : undefined;
  const cloudRecord = await saveCloudTravelRecord({
    ...input,
    id: legacyLocalId ? undefined : input.id,
  });
  if (legacyLocalId) {
    bestEffortLocalMirror(() => deleteLocalTravelRecord(legacyLocalId));
  }
  bestEffortLocalMirror(() => saveLocalTravelRecord({ ...cloudRecord, id: cloudRecord.id }));
  return cloudRecord;
}

export async function deleteTravelRecord(recordId: string): Promise<TravelRecord | undefined> {
  if (!useCloudStorage() || recordId.startsWith('local-')) return deleteLocalTravelRecord(recordId);

  const deleted = await deleteCloudTravelRecord(recordId);
  if (deleted) bestEffortLocalMirror(() => deleteLocalTravelRecord(recordId));
  return deleted;
}
