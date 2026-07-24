import { worldGeoJson } from '../../data/geojson/world';
import { findChinaCityPoint } from '../../data/geojson/china-city-points';
import { getMapComplianceNote } from '../../../../config/map-compliance';
import { worldMap } from '../../../../data/regions/world';
import { allCityMaps } from '../../../../data/regions/province-city-maps';
import { worldCityRegions } from '../../../../data/regions/world-cities';
import { getCustomWorldCityRegions, syncCustomWorldCitiesFromCloud } from '../../../../services/custom-world-city-store';
import { getAllRegionStates } from '../../../../services/travel-store.service';
import { getActiveTheme } from '../../../../services/theme-store';
import type { BoundaryMapMarker } from '../../../../types/geojson';
import type { Region, UserRegionState } from '../../../../types/region';
import {
  createBoundaryMapMarker,
  featureCenter,
  findGeoJsonFeatureByName,
  getGeoJsonBounds,
  projectGeoJsonPoint,
  toBoundaryRegionViewItems,
} from '../../../../utils/geojson-boundary';
import { themeClass, themeToCssVars } from '../../../../utils/theme-style';

Page({
  data: {
    regions: [],
    markers: [],
    litCountryCount: 0,
    markerCount: 0,
    loading: true,
    theme: null,
    themeStyle: '',
    themeClass: '',
    mapComplianceNote: getMapComplianceNote('world'),
  },

  async onShow() {
    await syncCustomWorldCitiesFromCloud().catch(() => {
      wx.showToast({ title: '云端城市同步失败，已显示本地数据', icon: 'none' });
    });
    const theme = getActiveTheme();
    this.setData({
      loading: true,
      theme,
      themeStyle: themeToCssVars(theme),
      themeClass: themeClass(theme),
    });
    const states = await getAllRegionStates();
    const markers = this.createWorldCityMarkers(states);
    const worldStates = this.createWorldRegionStates(states, markers);
    const litCountryCount = worldMap.regions.filter((region) => worldStates[region.regionCode]?.lit).length;

    this.setData({
      regions: toBoundaryRegionViewItems(worldMap, worldGeoJson, worldStates),
      markers,
      litCountryCount,
      markerCount: markers.length,
      theme,
      themeStyle: themeToCssVars(theme),
      themeClass: themeClass(theme),
      loading: false,
    });
  },

  createWorldCityMarkers(states: Record<string, UserRegionState>): BoundaryMapMarker[] {
    const worldBounds = getGeoJsonBounds(worldGeoJson);
    if (!worldBounds) return [];

    const cities = allCityMaps.flatMap((map) => map.regions);
    const foreignCities = [...worldCityRegions, ...getCustomWorldCityRegions()];

    const chinaMarkers = Object.values(states)
      .filter((state) => state.lit && state.regionCode.includes('-city-'))
      .map((state) => {
        const city = cities.find((item) => item.regionCode === state.regionCode);
        if (!city?.parentCode) return undefined;

        const lonLat = findChinaCityPoint(city.parentCode, city.name);
        if (!lonLat) return undefined;

        const point = projectGeoJsonPoint(lonLat, worldBounds, worldMap.width, worldMap.height);
        return createBoundaryMapMarker(
          `world-marker-${city.regionCode}`,
          city.regionCode,
          city.name,
          point,
          true,
          state.visitCount,
        );
      })
      .filter((marker): marker is BoundaryMapMarker => Boolean(marker));

    const foreignMarkers = Object.values(states)
      .filter((state) => state.lit && state.regionCode.startsWith('world-city:'))
      .map((state) => {
        const city = foreignCities.find((item) => item.regionCode === state.regionCode);
        if (!city) return undefined;

        const countryFeature = findGeoJsonFeatureByName(worldGeoJson, city.countryName);
        const fallbackCenter = countryFeature ? featureCenter(countryFeature) : undefined;
        const lonLat = city.lonLat || fallbackCenter;
        if (!lonLat) return undefined;

        const point = projectGeoJsonPoint(lonLat, worldBounds, worldMap.width, worldMap.height);
        return createBoundaryMapMarker(
          `world-marker-${city.regionCode}`,
          city.regionCode,
          city.name,
          point,
          true,
          state.visitCount,
        );
      })
      .filter((marker): marker is BoundaryMapMarker => Boolean(marker));

    return [...chinaMarkers, ...foreignMarkers];
  },

  createWorldRegionStates(
    states: Record<string, UserRegionState>,
    markers: BoundaryMapMarker[],
  ): Record<string, UserRegionState> {
    const worldStates: Record<string, UserRegionState> = { ...states };
    const chinaMarkers = markers.filter((marker) => marker.regionCode.includes('-city-'));
    const chinaVisitCount = chinaMarkers.reduce((sum, marker) => sum + (marker.visitCount || 0), 0);

    if (chinaMarkers.length > 0) {
      worldStates['country:cn'] = {
        regionCode: 'country:cn',
        lit: true,
        visitCount: chinaVisitCount,
        tags: [],
        updatedAt: new Date().toISOString(),
      };
    }

    [...worldCityRegions, ...getCustomWorldCityRegions()].forEach((city) => {
      const state = states[city.regionCode];
      if (!state?.lit) return;

      const current = worldStates[city.countryCode];
      worldStates[city.countryCode] = {
        regionCode: city.countryCode,
        lit: true,
        visitCount: (current?.visitCount || 0) + (state.visitCount || 0),
        tags: current?.tags || [],
        updatedAt: new Date().toISOString(),
      };
    });

    return worldStates;
  },

  handleRegionTap(event: any) {
    const region = event.detail as Region;
    if (region.nextPage) {
      wx.navigateTo({ url: region.nextPage });
      return;
    }

    wx.navigateTo({
      url: `/pages/foreign-city-list/index?countryCode=${encodeURIComponent(region.regionCode)}&countryName=${encodeURIComponent(region.name)}`,
    });
  },

  handleMarkerTap(event: any) {
    const marker = event.detail as BoundaryMapMarker;
    wx.navigateTo({
      url: `/pages/region-detail/index?regionCode=${encodeURIComponent(marker.regionCode)}`,
    });
  },
});
