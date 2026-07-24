import type { GeoJsonFeatureCollection } from '../../../../types/geojson';

const emptyFeatureCollection: GeoJsonFeatureCollection = {
  type: 'FeatureCollection',
  features: [],
};

// City boundary coordinates are intentionally excluded from the public source
// package because the original dataset provenance could not be verified.
export function getCityBoundaryGeoJson(_provinceCode: string): GeoJsonFeatureCollection {
  return emptyFeatureCollection;
}

export function clearCityBoundaryGeoJsonCache(): void {
  // No bundled boundary cache in the open-source template.
}
