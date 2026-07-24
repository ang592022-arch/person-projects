import { findRegion } from '../data/regions/index';
import type { RegionLevel } from '../types/region';

export interface RegionHierarchy {
  countryCode?: string;
  provinceCode?: string;
  cityCode?: string;
  countyCode?: string;
}

export interface RegionCountSummary {
  countryCount: number;
  provinceCount: number;
  cityCount: number;
  countyCount: number;
}

const trackedLevels: RegionLevel[] = ['country', 'province', 'city', 'county'];

export function resolveRegionHierarchy(regionCode: string): RegionHierarchy {
  const hierarchy: RegionHierarchy = {};
  const visited = new Set<string>();
  let currentCode = regionCode;

  while (currentCode && !visited.has(currentCode)) {
    visited.add(currentCode);
    const region = findRegion(currentCode);
    if (!region) break;

    if (trackedLevels.includes(region.level)) {
      const key = `${region.level}Code` as keyof RegionHierarchy;
      hierarchy[key] = region.regionCode;
    }

    currentCode = region.parentCode || '';
  }

  return hierarchy;
}

export function summarizeRegionCodes(regionCodes: string[]): RegionCountSummary {
  const countries = new Set<string>();
  const provinces = new Set<string>();
  const cities = new Set<string>();
  const counties = new Set<string>();

  regionCodes.forEach((regionCode) => {
    const hierarchy = resolveRegionHierarchy(regionCode);
    if (hierarchy.countryCode) countries.add(hierarchy.countryCode);
    if (hierarchy.provinceCode) provinces.add(hierarchy.provinceCode);
    if (hierarchy.cityCode) cities.add(hierarchy.cityCode);
    if (hierarchy.countyCode) counties.add(hierarchy.countyCode);
  });

  return {
    countryCount: countries.size,
    provinceCount: provinces.size,
    cityCount: cities.size,
    countyCount: counties.size,
  };
}
