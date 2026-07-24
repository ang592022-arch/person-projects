import type { TravelRecord, TravelRecordInput } from '../types/travel-record';
import type { UserRegionState } from '../types/region';
import { getCityMapForProvince } from '../data/regions/province-city-maps';
import { worldCityRegions } from '../data/regions/world-cities';
import { getCustomWorldCityRegions } from './custom-world-city-store';

const STATE_KEY = 'travel-map:user-region-state';
const RECORD_KEY = 'travel-map:travel-records';
const MIGRATION_KEY = 'travel-map:migration:city-level-v1';
const WORLD_CITY_MIGRATION_KEY = 'travel-map:migration:known-world-city-v1';

function normalizedCityParts(name: string): string[] {
  return name
    .toLowerCase()
    .split(/[ /·・\-]+/)
    .map((part) => part.replace(/[^a-z0-9\u4e00-\u9fa5]/g, ''))
    .filter((part) => part.length >= 2);
}

function findKnownWorldCity(name: string) {
  const targets = normalizedCityParts(name);
  if (!targets.length) return undefined;
  return worldCityRegions.find((city) => {
    const cityParts = normalizedCityParts(city.name);
    return targets.some((target) => cityParts.includes(target));
  });
}

function cityNameFromRegionCode(regionCode: string): string {
  const encodedName = regionCode.split(':').pop() || '';
  try {
    return decodeURIComponent(encodedName).replace(/-/g, ' ');
  } catch {
    return encodedName.replace(/-/g, ' ');
  }
}

function readJson<T>(key: string, fallback: T): T {
  try {
    const value = wx.getStorageSync(key);
    return value || fallback;
  } catch {
    return fallback;
  }
}

function writeJson<T>(key: string, value: T): void {
  wx.setStorageSync(key, value);
}

function now(): string {
  return new Date().toISOString();
}

export function getAllRegionStates(): Record<string, UserRegionState> {
  return readJson<Record<string, UserRegionState>>(STATE_KEY, {});
}

export function getRegionState(regionCode: string): UserRegionState | undefined {
  return getAllRegionStates()[regionCode];
}

export function setRegionLit(regionCode: string, lit: boolean): UserRegionState {
  const states = getAllRegionStates();
  const current = states[regionCode];
  const next: UserRegionState = {
    regionCode,
    lit,
    firstVisitDate: current?.firstVisitDate,
    lastVisitDate: current?.lastVisitDate,
    visitCount: current?.visitCount || 0,
    tags: current?.tags || [],
    coverImage: current?.coverImage,
    updatedAt: now(),
  };
  states[regionCode] = next;
  writeJson(STATE_KEY, states);
  return next;
}

export function getTravelRecords(regionCode?: string): TravelRecord[] {
  const records = readJson<TravelRecord[]>(RECORD_KEY, []).map((record) => ({
    ...record,
    tickets: record.tickets || [],
    companions: record.companions || [],
    visibility: record.visibility || 'private',
  }));
  if (!regionCode) return records;
  return records
    .filter((record) => record.regionCode === regionCode)
    .sort((a, b) => b.visitDate.localeCompare(a.visitDate));
}

export function getTravelRecord(recordId: string): TravelRecord | undefined {
  const record = readJson<TravelRecord[]>(RECORD_KEY, []).find((item) => item.id === recordId);
  return record ? {
    ...record,
    tickets: record.tickets || [],
    companions: record.companions || [],
    visibility: record.visibility || 'private',
  } : undefined;
}

export function saveTravelRecord(input: TravelRecordInput): TravelRecord {
  const records = readJson<TravelRecord[]>(RECORD_KEY, []);
  const existing = input.id ? records.find((record) => record.id === input.id) : undefined;
  const timestamp = now();
  const record: TravelRecord = {
    id: existing?.id || input.id || `local-${Date.now()}`,
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
  const nextRecords = existing
    ? records.map((item) => (item.id === record.id ? record : item))
    : [record, ...records];

  writeJson(RECORD_KEY, nextRecords);
  refreshRegionStateFromRecords(record.regionCode, nextRecords);
  return record;
}

export function deleteTravelRecord(recordId: string): TravelRecord | undefined {
  const records = readJson<TravelRecord[]>(RECORD_KEY, []);
  const target = records.find((record) => record.id === recordId);
  if (!target) return undefined;

  const nextRecords = records.filter((record) => record.id !== recordId);
  writeJson(RECORD_KEY, nextRecords);
  refreshRegionStateFromRecords(target.regionCode, nextRecords);
  return target;
}

export function migrateKnownWorldCityData(): void {
  if (wx.getStorageSync(WORLD_CITY_MIGRATION_KEY)) return;

  const records = readJson<TravelRecord[]>(RECORD_KEY, []);
  const states = getAllRegionStates();
  const staticCodes = new Set(worldCityRegions.map((city) => city.regionCode));
  const customCities = getCustomWorldCityRegions();
  const mappings = new Map<string, string>();

  const candidateCodes = new Set([
    ...records.map((record) => record.regionCode),
    ...Object.keys(states),
    ...customCities.map((city) => city.regionCode),
  ]);

  candidateCodes.forEach((regionCode) => {
    if (!regionCode.startsWith('world-city') || staticCodes.has(regionCode)) return;
    const customCity = customCities.find((city) => city.regionCode === regionCode);
    const knownCity = findKnownWorldCity(customCity?.name || cityNameFromRegionCode(regionCode));
    if (knownCity && knownCity.regionCode !== regionCode) {
      mappings.set(regionCode, knownCity.regionCode);
    }
  });

  if (mappings.size > 0) {
    const migratedRecords = records.map((record) => ({
      ...record,
      regionCode: mappings.get(record.regionCode) || record.regionCode,
      updatedAt: mappings.has(record.regionCode) ? now() : record.updatedAt,
    }));

    mappings.forEach((targetCode, sourceCode) => {
      const source = states[sourceCode];
      if (!source) return;
      const target = states[targetCode];
      states[targetCode] = {
        regionCode: targetCode,
        lit: Boolean(source.lit || target?.lit),
        firstVisitDate: [source.firstVisitDate, target?.firstVisitDate].filter(Boolean).sort()[0],
        lastVisitDate: [source.lastVisitDate, target?.lastVisitDate].filter(Boolean).sort().reverse()[0],
        visitCount: (source.visitCount || 0) + (target?.visitCount || 0),
        tags: Array.from(new Set([...(source.tags || []), ...(target?.tags || [])])),
        coverImage: target?.coverImage || source.coverImage,
        updatedAt: now(),
      };
      delete states[sourceCode];
    });

    writeJson(RECORD_KEY, migratedRecords);
    writeJson(STATE_KEY, states);
    Array.from(new Set(mappings.values())).forEach((regionCode) => {
      refreshRegionStateFromRecords(regionCode, migratedRecords);
    });
  }

  wx.setStorageSync(WORLD_CITY_MIGRATION_KEY, true);
}

export function migrateLegacyProvinceDataToCities(): void {
  if (wx.getStorageSync(MIGRATION_KEY)) return;

  const states = getAllRegionStates();
  const records = readJson<TravelRecord[]>(RECORD_KEY, []);
  let changed = false;

  const migratedRecords = records.map((record) => {
    const cityMap = getCityMapForProvince(record.regionCode);
    const targetCity = cityMap?.regions[0];
    if (!targetCity) return record;

    changed = true;
    return {
      ...record,
      regionCode: targetCity.regionCode,
      updatedAt: now(),
    };
  });

  const affectedCityCodes = new Set(
    migratedRecords.map((record) => record.regionCode).filter((regionCode) => regionCode.includes('-city-')),
  );

  Object.keys(states).forEach((regionCode) => {
    const cityMap = getCityMapForProvince(regionCode);
    const targetCity = cityMap?.regions[0];
    if (!targetCity) return;

    const legacyState = states[regionCode];
    const existingCityState = states[targetCity.regionCode];

    if (legacyState.lit || existingCityState) {
      states[targetCity.regionCode] = {
        regionCode: targetCity.regionCode,
        lit: Boolean(existingCityState?.lit || legacyState.lit),
        firstVisitDate: existingCityState?.firstVisitDate || legacyState.firstVisitDate,
        lastVisitDate: existingCityState?.lastVisitDate || legacyState.lastVisitDate,
        visitCount: existingCityState?.visitCount || legacyState.visitCount || 0,
        tags: Array.from(new Set([...(existingCityState?.tags || []), ...(legacyState.tags || [])])),
        coverImage: existingCityState?.coverImage || legacyState.coverImage,
        updatedAt: now(),
      };
      affectedCityCodes.add(targetCity.regionCode);
    }

    delete states[regionCode];
    changed = true;
  });

  if (changed) {
    writeJson(RECORD_KEY, migratedRecords);
    writeJson(STATE_KEY, states);
    affectedCityCodes.forEach((regionCode) => {
      refreshRegionStateFromRecords(regionCode, migratedRecords);
    });
  }

  wx.setStorageSync(MIGRATION_KEY, true);
}

export function refreshRegionStateFromRecords(regionCode: string, allRecords = getTravelRecords()): void {
  const regionRecords = allRecords
    .filter((record) => record.regionCode === regionCode)
    .sort((a, b) => a.visitDate.localeCompare(b.visitDate));
  const states = getAllRegionStates();
  const current = states[regionCode];

  states[regionCode] = {
    regionCode,
    lit: current?.lit || regionRecords.length > 0,
    firstVisitDate: regionRecords[0]?.visitDate,
    lastVisitDate: regionRecords[regionRecords.length - 1]?.visitDate,
    visitCount: regionRecords.length,
    tags: current?.tags || [],
    coverImage: current?.coverImage,
    updatedAt: now(),
  };

  writeJson(STATE_KEY, states);
}
