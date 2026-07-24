import { findRegion } from '../data/regions/index';
import { regionLonLatMap } from '../data/region-lonlat';
import type { Region } from '../types/region';
import type { TravelRecord } from '../types/travel-record';

const HOME_PLACES_KEY = 'travel-map:home-places';
export const MAX_HOME_PLACES = 5;

export interface HomePlace {
  regionCode: string;
  name: string;
  parentName: string;
  lonLat?: [number, number];
  addedAt: string;
}

export interface DistanceSummary {
  homeCount: number;
  totalKm: number;
  calculatedRecordCount: number;
  skippedRecordCount: number;
}

function readHomePlaces(): HomePlace[] {
  try {
    return wx.getStorageSync(HOME_PLACES_KEY) || [];
  } catch {
    return [];
  }
}

function writeHomePlaces(places: HomePlace[]): void {
  wx.setStorageSync(HOME_PLACES_KEY, places.slice(0, MAX_HOME_PLACES));
}

function parentName(region: Region): string {
  const parent = region.parentCode ? findRegion(region.parentCode) : undefined;
  return parent?.name || '';
}

function regionLonLat(region: Region): [number, number] | undefined {
  const regionWithLonLat = region as Region & { lonLat?: [number, number] };
  if (regionWithLonLat.lonLat) return regionWithLonLat.lonLat;
  return regionLonLatMap[region.regionCode];
}

export function getRegionLonLat(regionCode: string): [number, number] | undefined {
  const region = findRegion(regionCode);
  return region ? regionLonLat(region) : undefined;
}

export function getHomePlaces(): HomePlace[] {
  return readHomePlaces();
}

export function isHomePlace(regionCode: string): boolean {
  return readHomePlaces().some((place) => place.regionCode === regionCode);
}

export function addHomePlace(regionCode: string): { ok: boolean; reason?: 'missing-region' | 'limit-reached'; places: HomePlace[] } {
  const places = readHomePlaces();
  if (places.some((place) => place.regionCode === regionCode)) {
    return { ok: true, places };
  }

  if (places.length >= MAX_HOME_PLACES) {
    return { ok: false, reason: 'limit-reached', places };
  }

  const region = findRegion(regionCode);
  if (!region) {
    return { ok: false, reason: 'missing-region', places };
  }

  const nextPlaces = [
    ...places,
    {
      regionCode,
      name: region.name,
      parentName: parentName(region),
      lonLat: getRegionLonLat(regionCode),
      addedAt: new Date().toISOString(),
    },
  ];
  writeHomePlaces(nextPlaces);
  return { ok: true, places: nextPlaces };
}

export function removeHomePlace(regionCode: string): HomePlace[] {
  const nextPlaces = readHomePlaces().filter((place) => place.regionCode !== regionCode);
  writeHomePlaces(nextPlaces);
  return nextPlaces;
}

function distanceKm(from: [number, number], to: [number, number]): number {
  const earthRadiusKm = 6371;
  const toRadians = (value: number) => (value * Math.PI) / 180;
  const [fromLng, fromLat] = from;
  const [toLng, toLat] = to;
  const latDelta = toRadians(toLat - fromLat);
  const lngDelta = toRadians(toLng - fromLng);
  const startLat = toRadians(fromLat);
  const endLat = toRadians(toLat);
  const a = Math.sin(latDelta / 2) ** 2
    + Math.cos(startLat) * Math.cos(endLat) * Math.sin(lngDelta / 2) ** 2;

  return earthRadiusKm * 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1 - a));
}

export function calculateEstimatedDistance(records: TravelRecord[], homePlaces = readHomePlaces()): DistanceSummary {
  const usableHomes = homePlaces.filter((place) => place.lonLat);
  if (!usableHomes.length) {
    return {
      homeCount: homePlaces.length,
      totalKm: 0,
      calculatedRecordCount: 0,
      skippedRecordCount: records.length,
    };
  }

  return records.reduce<DistanceSummary>((summary, record) => {
    const destination = getRegionLonLat(record.regionCode);
    if (!destination) {
      summary.skippedRecordCount += 1;
      return summary;
    }

    const nearestOneWayKm = Math.min(...usableHomes.map((home) => distanceKm(home.lonLat as [number, number], destination)));
    summary.totalKm += nearestOneWayKm < 1 ? 0 : nearestOneWayKm * 2;
    summary.calculatedRecordCount += 1;
    return summary;
  }, {
    homeCount: homePlaces.length,
    totalKm: 0,
    calculatedRecordCount: 0,
    skippedRecordCount: 0,
  });
}

export function formatDistanceKm(km: number): string {
  if (km <= 0) return '0 公里';
  if (km >= 10000) return `${(km / 10000).toFixed(km >= 100000 ? 0 : 1)} 万公里`;
  return `${Math.round(km).toLocaleString()} 公里`;
}
