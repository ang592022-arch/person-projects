import { findRegion } from '../../../../data/regions/index';
import { getMapComplianceNote } from '../../../../config/map-compliance';
import { getCityBoundaryGeoJson } from '../../data/geojson/province-cities';
import { getCityMapForProvince } from '../../../../data/regions/province-city-maps';
import { hasAmapBoundaryConfig, requestAmapDistrictBoundary } from '../../../../services/amap-boundary.service';
import { getAllRegionStates } from '../../../../services/travel-store.service';
import { getActiveTheme } from '../../../../services/theme-store';
import { toBoundaryRegionViewItems } from '../../../../utils/geojson-boundary';
import { themeClass, themeToCssVars } from '../../../../utils/theme-style';

Page({
  data: {
    provinceCode: '',
    provinceName: '',
    regions: [],
    litCityCount: 0,
    totalCityCount: 0,
    progressPercent: 0,
    hasPreciseBoundary: false,
    boundarySourceNote: '',
    loading: true,
    theme: null,
    themeStyle: '',
    themeClass: '',
  },

  onLoad(options: any) {
    const provinceCode = decodeURIComponent(options.provinceCode || '');
    const province = findRegion(provinceCode);

    if (!province) {
      wx.showToast({ title: '没有找到这个省份', icon: 'none' });
      wx.navigateBack();
      return;
    }

    wx.setNavigationBarTitle({ title: `${province.name}城市地图` });
    this.setData({
      provinceCode,
      provinceName: province.name,
      boundarySourceNote: provinceCode === 'cn-taiwan'
        ? getMapComplianceNote('taiwan')
        : getMapComplianceNote('china'),
    });
  },

  onShow() {
    this.refresh();
  },

  async refresh() {
    const { provinceCode, provinceName } = this.data;
    const cityMap = getCityMapForProvince(provinceCode);
    if (!cityMap) return;

    const theme = getActiveTheme();
    this.setData({
      loading: true,
      theme,
      themeStyle: themeToCssVars(theme),
      themeClass: themeClass(theme),
    });
    const states = await getAllRegionStates();
    const litCityCount = cityMap.regions.filter((city) => states[city.regionCode]?.lit).length;
    const cityGeoJson = await this.loadBoundaryGeoJson(provinceCode, provinceName);
    const progressPercent = cityMap.regions.length
      ? Math.round((litCityCount / cityMap.regions.length) * 100)
      : 0;

    this.setData({
      regions: toBoundaryRegionViewItems(cityMap, cityGeoJson, states),
      hasPreciseBoundary: cityGeoJson.features.length > 0,
      litCityCount,
      totalCityCount: cityMap.regions.length,
      progressPercent,
      theme,
      themeStyle: themeToCssVars(theme),
      themeClass: themeClass(theme),
      loading: false,
    });
  },

  async loadBoundaryGeoJson(provinceCode: string, provinceName: string) {
    const localGeoJson = getCityBoundaryGeoJson(provinceCode);
    if (localGeoJson.features.length || !hasAmapBoundaryConfig()) {
      return localGeoJson;
    }

    try {
      return await requestAmapDistrictBoundary({
        keywords: provinceName,
        subdistrict: 1,
      });
    } catch (error) {
      wx.showToast({ title: '高德边界加载失败', icon: 'none' });
      return localGeoJson;
    }
  },

  handleRegionTap(event: any) {
    const city = event.detail;
    wx.navigateTo({
      url: `/pages/region-detail/index?regionCode=${encodeURIComponent(city.regionCode)}`,
    });
  },
});
