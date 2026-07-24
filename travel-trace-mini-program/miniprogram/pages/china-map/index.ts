import { chinaProvinceMap } from '../../data/regions/china-provinces';
import { chinaProvinceGeoJson } from '../../data/geojson/china-provinces';
import { getMapComplianceNote } from '../../config/map-compliance';
import { getCityMapForProvince, getProvinceHighlightStates } from '../../data/regions/province-city-maps';
import { hasAmapBoundaryConfig, requestAmapDistrictBoundary } from '../../services/amap-boundary.service';
import { getAllRegionStates } from '../../services/travel-store.service';
import { getActiveTheme } from '../../services/theme-store';
import { toBoundaryRegionViewItems } from '../../utils/geojson-boundary';
import { themeClass, themeToCssVars } from '../../utils/theme-style';

Page({
  data: {
    regions: [],
    litProvinceCount: 0,
    litCityCount: 0,
    totalProvinceCount: 0,
    loading: true,
    theme: null,
    themeStyle: '',
    themeClass: '',
    mapComplianceNote: getMapComplianceNote('china'),
  },

  async onShow() {
    const theme = getActiveTheme();
    this.setData({
      loading: true,
      theme,
      themeStyle: themeToCssVars(theme),
      themeClass: themeClass(theme),
    });
    const states = await getAllRegionStates();
    const provinceStates = getProvinceHighlightStates(states);
    const boundaryGeoJson = await this.loadBoundaryGeoJson();

    this.setData({
      regions: toBoundaryRegionViewItems(chinaProvinceMap, boundaryGeoJson, provinceStates),
      litProvinceCount: Object.values(provinceStates).filter((state) => state.lit).length,
      litCityCount: Object.values(states).filter((state) => state.lit && state.regionCode.includes('-city-')).length,
      totalProvinceCount: chinaProvinceMap.regions.length,
      theme,
      themeStyle: themeToCssVars(theme),
      themeClass: themeClass(theme),
      loading: false,
    });
  },

  async loadBoundaryGeoJson() {
    if (chinaProvinceGeoJson.features.length || !hasAmapBoundaryConfig()) {
      return chinaProvinceGeoJson;
    }

    try {
      return await requestAmapDistrictBoundary({
        keywords: '中国',
        subdistrict: 1,
      });
    } catch (error) {
      wx.showToast({ title: '高德边界加载失败', icon: 'none' });
      return chinaProvinceGeoJson;
    }
  },

  handleRegionTap(event: any) {
    const region = event.detail;
    if (!getCityMapForProvince(region.regionCode)) {
      wx.showToast({ title: '暂未开放城市地图', icon: 'none' });
      return;
    }

    wx.navigateTo({
      url: `/packages/map/pages/city-map/index?provinceCode=${encodeURIComponent(region.regionCode)}`,
    });
  },
});
