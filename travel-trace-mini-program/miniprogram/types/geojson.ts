export type GeoJsonPosition = [number, number];

export interface GeoJsonGeometry {
  type: 'Polygon' | 'MultiPolygon';
  coordinates: GeoJsonPosition[][] | GeoJsonPosition[][][];
}

export interface GeoJsonFeature {
  type: 'Feature';
  properties: Record<string, string | number | undefined>;
  geometry: GeoJsonGeometry;
}

export interface GeoJsonFeatureCollection {
  type: 'FeatureCollection';
  features: GeoJsonFeature[];
}

export interface BoundaryRegionViewItem {
  regionCode: string;
  name: string;
  level: string;
  nextPage?: string;
  childrenMapCode?: string;
  lit: boolean;
  polygons: number[][][];
  labelStyle: string;
}

export interface BoundaryMapMarker {
  id: string;
  regionCode: string;
  name: string;
  x: number;
  y: number;
  lit: boolean;
  visitCount?: number;
  style: string;
}
