import type { GeoJsonFeatureCollection } from '../../types/geojson';

// China boundary coordinates are intentionally not distributed in the public
// repository. Supply an authorized dataset at build time or configure the
// optional boundary provider described in docs/map-data.md.
export const chinaProvinceGeoJson: GeoJsonFeatureCollection = {
  type: 'FeatureCollection',
  features: [],
};
