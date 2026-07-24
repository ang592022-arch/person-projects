import { chinaProvinceMap } from './china-provinces';
import { allCityMaps } from './province-city-maps';
import { worldCityMap } from './world-cities';
import { worldMap } from './world';
import { findCustomWorldCity, getCustomWorldCityRegions } from '../../services/custom-world-city-store';
import type { Region, RegionMap } from '../../types/region';

const maps: Record<string, RegionMap> = {
  [worldMap.mapCode]: worldMap,
  [worldCityMap.mapCode]: worldCityMap,
  [chinaProvinceMap.mapCode]: chinaProvinceMap,
  ...Object.fromEntries(allCityMaps.map((map) => [map.mapCode, map])),
};

export function getRegionMap(mapCode: string): RegionMap {
  return maps[mapCode] || worldMap;
}

export function findRegion(regionCode: string): Region | undefined {
  const staticRegion = Object.values(maps)
    .flatMap((map) => map.regions)
    .find((region) => region.regionCode === regionCode);
  return staticRegion || findCustomWorldCity(regionCode);
}

export { chinaProvinceMap, worldMap };
export { allCityMaps, getCityMapForProvince, getProvinceForCity, getProvinceHighlightStates } from './province-city-maps';
export { getWorldCitiesForCountry, getWorldCityCountry, worldCityMap, worldCityRegions } from './world-cities';
export { getCustomWorldCityRegions };
