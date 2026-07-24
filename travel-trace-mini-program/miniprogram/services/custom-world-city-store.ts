import type { WorldCityRegion } from '../data/regions/world-cities';
import { cloudConfig } from '../config/env';
import { assertCloudRuntimeReady, isCloudMode, isCloudRuntimeReady } from './cloud-runtime.service';

const CUSTOM_WORLD_CITY_KEY = 'travel-map:custom-world-cities';

export interface CustomWorldCityInput {
  countryCode: string;
  countryName: string;
  name: string;
  lonLat?: [number, number];
}

function readCities(): WorldCityRegion[] {
  try {
    return wx.getStorageSync(CUSTOM_WORLD_CITY_KEY) || [];
  } catch {
    return [];
  }
}

function writeCities(cities: WorldCityRegion[]): void {
  wx.setStorageSync(CUSTOM_WORLD_CITY_KEY, cities);
}

function isCloudEnabled(): boolean {
  return isCloudRuntimeReady();
}

function db() {
  if (!wx.cloud || !cloudConfig.envId) throw new Error('请先配置云开发环境 ID');
  return wx.cloud.database();
}

function mergeCities(localCities: WorldCityRegion[], cloudCities: WorldCityRegion[]): WorldCityRegion[] {
  const byCode = new Map<string, WorldCityRegion>();
  [...localCities, ...cloudCities].forEach((city) => byCode.set(city.regionCode, city));
  return Array.from(byCode.values());
}

async function fetchCloudCustomWorldCities(): Promise<WorldCityRegion[]> {
  if (!isCloudEnabled()) return [];

  const pageSize = 100;
  let offset = 0;
  const cities: WorldCityRegion[] = [];
  const collection = db().collection(cloudConfig.collections.customWorldCities);

  while (true) {
    const result = await collection.skip(offset).limit(pageSize).get();
    const data = (result.data || []) as Array<WorldCityRegion & { _id?: string; _openid?: string }>;
    cities.push(...data.map(({ _id, _openid, ...city }) => city));
    if (data.length < pageSize) break;
    offset += pageSize;
  }

  return cities;
}

async function upsertCloudCustomWorldCity(city: WorldCityRegion): Promise<void> {
  if (!isCloudEnabled()) return;

  const collection = db().collection(cloudConfig.collections.customWorldCities);
  const existing = await collection
    .where({ regionCode: city.regionCode })
    .limit(1)
    .get();

  if (existing.data?.[0]?._id) {
    await collection.doc(existing.data[0]._id).update({ data: city });
    return;
  }

  await collection.add({ data: city });
}

function slugify(value: string): string {
  return encodeURIComponent(value.trim().toLowerCase().replace(/\s+/g, '-'));
}

export function getCustomWorldCityRegions(): WorldCityRegion[] {
  return readCities();
}

export function getCustomWorldCitiesForCountry(countryCode: string): WorldCityRegion[] {
  return readCities().filter((city) => city.countryCode === countryCode);
}

export function findCustomWorldCity(regionCode: string): WorldCityRegion | undefined {
  return readCities().find((city) => city.regionCode === regionCode);
}

export async function upsertCustomWorldCity(input: CustomWorldCityInput): Promise<WorldCityRegion> {
  if (isCloudMode()) assertCloudRuntimeReady();

  const cityName = input.name.trim();
  const countrySlug = input.countryCode.replace(/^country:/, '');
  const cityCode = `world-city-custom:${countrySlug}:${slugify(cityName)}`;
  const cities = readCities();
  const existing = cities.find((city) => city.regionCode === cityCode);
  const next: WorldCityRegion = {
    regionCode: cityCode,
    name: cityName,
    level: 'city',
    parentCode: input.countryCode,
    countryCode: input.countryCode,
    countryName: input.countryName,
    lonLat: input.lonLat,
    center: [500, 310],
    shape: { type: 'rect', x: 0, y: 0, width: 1, height: 1 },
  };

  if (isCloudMode()) await upsertCloudCustomWorldCity(next);

  const updatedCities = existing
    ? cities.map((city) => city.regionCode === cityCode ? next : city)
    : [...cities, next];
  writeCities(updatedCities);
  return next;
}

export async function syncCustomWorldCitiesFromCloud(): Promise<WorldCityRegion[]> {
  if (!isCloudMode()) return readCities();
  assertCloudRuntimeReady();

  const localCities = readCities();
  const cloudCities = await fetchCloudCustomWorldCities();
  const merged = mergeCities(localCities, cloudCities);
  writeCities(merged);
  await Promise.all(localCities.map((city) => upsertCloudCustomWorldCity(city)));
  return merged;
}
