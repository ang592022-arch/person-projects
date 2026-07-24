import type { RegionMap, UserRegionState } from '../types/region';
import { labelToStyle, shapeToStyle } from './shape';

export interface RegionViewItem {
  regionCode: string;
  name: string;
  level: string;
  nextPage?: string;
  lit: boolean;
  style: string;
  labelStyle: string;
}

export function toRegionViewItems(
  map: RegionMap,
  states: Record<string, UserRegionState>,
): RegionViewItem[] {
  return map.regions.map((region) => ({
    regionCode: region.regionCode,
    name: region.name,
    level: region.level,
    nextPage: region.nextPage,
    lit: Boolean(states[region.regionCode]?.lit),
    style: shapeToStyle(region.shape, map.width, map.height),
    labelStyle: labelToStyle(region.center, map.width, map.height),
  }));
}
