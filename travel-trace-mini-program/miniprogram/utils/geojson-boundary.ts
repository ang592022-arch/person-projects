import type { BoundaryMapMarker, BoundaryRegionViewItem, GeoJsonFeature, GeoJsonFeatureCollection, GeoJsonPosition } from '../types/geojson';
import type { Region, RegionMap, UserRegionState } from '../types/region';
import { labelToStyle } from './shape';

export interface Bounds {
  minLng: number;
  maxLng: number;
  minLat: number;
  maxLat: number;
}

export function normalizeGeoJsonName(name: string): string {
  return name
    .replace(/特别行政区|维吾尔自治区|壮族自治区|回族自治区|自治区|省|市|地区|盟|自治州/g, '')
    .trim();
}

function featureName(feature: GeoJsonFeature): string {
  return String(feature.properties.name || feature.properties.fullname || '');
}

function featureMatchesRegion(feature: GeoJsonFeature, region: Region): boolean {
  const name = normalizeGeoJsonName(featureName(feature));
  const regionName = normalizeGeoJsonName(region.name);
  return name === regionName || name.includes(regionName) || regionName.includes(name);
}

export function findGeoJsonFeatureByName(
  geoJson: GeoJsonFeatureCollection,
  name: string,
): GeoJsonFeature | undefined {
  const targetName = normalizeGeoJsonName(name);
  return geoJson.features.find((feature) => {
    const currentName = normalizeGeoJsonName(featureName(feature));
    return currentName === targetName || currentName.includes(targetName) || targetName.includes(currentName);
  });
}

function ringsFromGeometry(feature: GeoJsonFeature): GeoJsonPosition[][] {
  if (feature.geometry.type === 'Polygon') {
    return feature.geometry.coordinates as GeoJsonPosition[][];
  }

  return (feature.geometry.coordinates as GeoJsonPosition[][][]).flatMap((polygon) => polygon);
}

function getBounds(features: GeoJsonFeature[]): Bounds | undefined {
  const points = features.flatMap((feature) => ringsFromGeometry(feature).flat());
  if (points.length === 0) return undefined;

  const lngs = points.map(([lng]) => lng);
  const lats = points.map(([, lat]) => lat);

  return {
    minLng: Math.min(...lngs),
    maxLng: Math.max(...lngs),
    minLat: Math.min(...lats),
    maxLat: Math.max(...lats),
  };
}

export function getGeoJsonBounds(geoJson: GeoJsonFeatureCollection): Bounds | undefined {
  return getBounds(geoJson.features);
}

function projectPoint(point: GeoJsonPosition, bounds: Bounds, width: number, height: number): number[] {
  const [lng, lat] = point;
  const lngSpan = bounds.maxLng - bounds.minLng || 1;
  const latSpan = bounds.maxLat - bounds.minLat || 1;
  const padding = 36;
  const drawableWidth = width - padding * 2;
  const drawableHeight = height - padding * 2;
  const scale = Math.min(drawableWidth / lngSpan, drawableHeight / latSpan);
  const projectedWidth = lngSpan * scale;
  const projectedHeight = latSpan * scale;
  const offsetX = padding + (drawableWidth - projectedWidth) / 2;
  const offsetY = padding + (drawableHeight - projectedHeight) / 2;

  return [
    offsetX + (lng - bounds.minLng) * scale,
    offsetY + (bounds.maxLat - lat) * scale,
  ];
}

export function projectGeoJsonPoint(
  point: GeoJsonPosition,
  bounds: Bounds,
  width: number,
  height: number,
): number[] {
  return projectPoint(point, bounds, width, height);
}

export function featureCenter(feature: GeoJsonFeature): GeoJsonPosition | undefined {
  const points = ringsFromGeometry(feature).flat();
  if (points.length === 0) return undefined;

  const lngs = points.map(([lng]) => lng);
  const lats = points.map(([, lat]) => lat);

  return [
    (Math.min(...lngs) + Math.max(...lngs)) / 2,
    (Math.min(...lats) + Math.max(...lats)) / 2,
  ];
}

function projectFeature(feature: GeoJsonFeature, bounds: Bounds, width: number, height: number): number[][][] {
  return ringsFromGeometry(feature).map((ring) => ring.map((point) => projectPoint(point, bounds, width, height)));
}

function labelStyleFromPolygons(polygons: number[][][]): string | undefined {
  const points = polygons.flat();
  if (points.length === 0) return undefined;

  const xs = points.map(([x]) => x);
  const ys = points.map(([, y]) => y);
  const center: [number, number] = [
    (Math.min(...xs) + Math.max(...xs)) / 2,
    (Math.min(...ys) + Math.max(...ys)) / 2,
  ];

  return labelToStyle(center, 1000, 620);
}

function pointInRing(point: number[], ring: number[][]): boolean {
  const [x, y] = point;
  let inside = false;

  for (let index = 0, previous = ring.length - 1; index < ring.length; previous = index++) {
    const [xi, yi] = ring[index];
    const [xj, yj] = ring[previous];
    const intersects = yi > y !== yj > y && x < ((xj - xi) * (y - yi)) / (yj - yi || 1) + xi;
    if (intersects) inside = !inside;
  }

  return inside;
}

export function pointInBoundary(point: number[], polygons: number[][][]): boolean {
  return polygons.some((ring) => pointInRing(point, ring));
}

export function toBoundaryRegionViewItems(
  map: RegionMap,
  geoJson: GeoJsonFeatureCollection,
  states: Record<string, UserRegionState>,
): BoundaryRegionViewItem[] {
  const bounds = getBounds(geoJson.features);

  return map.regions.map((region) => {
    const aliasNames = region.geoJsonAliases || [];
    const features = geoJson.features.filter((item) => (
      featureMatchesRegion(item, region)
      || aliasNames.some((aliasName) => findGeoJsonFeatureByName({ type: 'FeatureCollection', features: [item] }, aliasName))
    ));
    const polygons = bounds
      ? features.flatMap((feature) => projectFeature(feature, bounds, map.width, map.height))
      : [];

    return {
      regionCode: region.regionCode,
      name: region.name,
      level: region.level,
      nextPage: region.nextPage,
      childrenMapCode: region.childrenMapCode,
      lit: Boolean(states[region.regionCode]?.lit),
      polygons,
      labelStyle: labelStyleFromPolygons(polygons) || labelToStyle(region.center, map.width, map.height),
    };
  });
}

export function createBoundaryMapMarker(
  id: string,
  regionCode: string,
  name: string,
  point: number[],
  lit: boolean,
  visitCount?: number,
): BoundaryMapMarker {
  const [x, y] = point;

  return {
    id,
    regionCode,
    name,
    x,
    y,
    lit,
    visitCount,
    style: labelToStyle([x, y], 1000, 620),
  };
}
