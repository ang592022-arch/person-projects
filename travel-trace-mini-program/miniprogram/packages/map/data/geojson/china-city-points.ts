export interface ChinaCityPoint {
  name: string;
  lonLat: [number, number];
}

// Derived China city coordinates are omitted together with the unverified
// boundary dataset. Foreign city points still use Natural Earth data.
export const cityPointsByProvince: Record<string, ChinaCityPoint[]> = {};

export function findChinaCityPoint(
  _provinceCode: string,
  _cityName: string,
): [number, number] | undefined {
  return undefined;
}
