export type RegionLevel = 'world' | 'continent' | 'country' | 'province' | 'city' | 'county';

export interface RegionShape {
  type: 'rect' | 'polygon';
  points?: number[][];
  x?: number;
  y?: number;
  width?: number;
  height?: number;
}

export interface Region {
  regionCode: string;
  name: string;
  level: RegionLevel;
  parentCode?: string;
  childrenMapCode?: string;
  center: [number, number];
  shape: RegionShape;
  nextPage?: string;
  geoJsonAliases?: string[];
}

export interface RegionMap {
  mapCode: string;
  name: string;
  level: RegionLevel;
  parentCode?: string;
  source?: 'lite-shape' | 'geojson';
  minCreatableLevel?: RegionLevel;
  width: number;
  height: number;
  regions: Region[];
}

export interface UserRegionState {
  regionCode: string;
  lit: boolean;
  firstVisitDate?: string;
  lastVisitDate?: string;
  visitCount: number;
  tags: string[];
  coverImage?: string;
  updatedAt: string;
}
