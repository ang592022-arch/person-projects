import { amapConfig } from '../config/env';
import type { GeoJsonFeature, GeoJsonFeatureCollection, GeoJsonPosition } from '../types/geojson';

interface AmapDistrict {
  adcode?: string;
  name?: string;
  level?: string;
  center?: string;
  polyline?: string;
  districts?: AmapDistrict[];
}

interface AmapDistrictResponse {
  status: string;
  info?: string;
  districts?: AmapDistrict[];
}

export interface AmapBoundaryRequestOptions {
  keywords: string;
  subdistrict?: number;
  key?: string;
  serviceBaseUrl?: string;
}

export function hasAmapBoundaryConfig(): boolean {
  return Boolean(amapConfig.key);
}

function parseCoordinatePair(pair: string): GeoJsonPosition | undefined {
  const [lngText, latText] = pair.split(',');
  const lng = Number(lngText);
  const lat = Number(latText);

  if (!Number.isFinite(lng) || !Number.isFinite(lat)) return undefined;
  return [lng, lat];
}

function closeRing(points: GeoJsonPosition[]): GeoJsonPosition[] {
  if (points.length < 3) return points;

  const first = points[0];
  const last = points[points.length - 1];
  if (first[0] === last[0] && first[1] === last[1]) return points;
  return [...points, first];
}

export function amapPolylineToMultiPolygon(polyline = ''): GeoJsonPosition[][][] {
  return polyline
    .split('|')
    .map((polygonText) => polygonText
      .split(';')
      .map(parseCoordinatePair)
      .filter((point): point is GeoJsonPosition => Boolean(point)))
    .filter((ring) => ring.length >= 3)
    .map((ring) => [closeRing(ring)]);
}

function districtToFeature(district: AmapDistrict): GeoJsonFeature | undefined {
  const coordinates = amapPolylineToMultiPolygon(district.polyline);
  if (coordinates.length === 0) return undefined;

  return {
    type: 'Feature',
    properties: {
      adcode: district.adcode,
      name: district.name,
      level: district.level,
      center: district.center,
    },
    geometry: {
      type: 'MultiPolygon',
      coordinates,
    },
  };
}

export function amapDistrictsToGeoJson(districts: AmapDistrict[] = []): GeoJsonFeatureCollection {
  return {
    type: 'FeatureCollection',
    features: districts
      .map(districtToFeature)
      .filter((feature): feature is GeoJsonFeature => Boolean(feature)),
  };
}

function resolveRenderableDistricts(districts: AmapDistrict[] = []): AmapDistrict[] {
  if (districts.length === 1 && districts[0].districts?.length) {
    return districts[0].districts;
  }

  return districts;
}

export function requestAmapDistrictBoundary(options: AmapBoundaryRequestOptions): Promise<GeoJsonFeatureCollection> {
  const key = options.key || amapConfig.key;
  const serviceBaseUrl = options.serviceBaseUrl || amapConfig.serviceBaseUrl;

  if (!key) {
    return Promise.reject(new Error('请先在 miniprogram/config/env.ts 中填写高德 Web 服务 Key'));
  }

  return new Promise((resolve, reject) => {
    wx.request({
      url: `${serviceBaseUrl}/v3/config/district`,
      data: {
        key,
        keywords: options.keywords,
        subdistrict: options.subdistrict ?? 0,
        extensions: 'all',
      },
      success(response: { data: AmapDistrictResponse }) {
        const data = response.data;
        if (data.status !== '1') {
          reject(new Error(data.info || '高德行政区边界请求失败'));
          return;
        }

        resolve(amapDistrictsToGeoJson(resolveRenderableDistricts(data.districts)));
      },
      fail(error) {
        reject(error);
      },
    });
  });
}
