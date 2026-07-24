import { getWorldCitiesForCountry } from '../../data/regions/world-cities';
import { getCustomWorldCitiesForCountry, syncCustomWorldCitiesFromCloud, upsertCustomWorldCity } from '../../services/custom-world-city-store';
import { getAllRegionStates } from '../../services/travel-store.service';
import { getActiveTheme } from '../../services/theme-store';
import { shadowClass, themeClass, themeToCssVars } from '../../utils/theme-style';

Page({
  data: {
    countryCode: '',
    countryName: '',
    newCityName: '',
    cities: [],
    addingCity: false,
    themeStyle: '',
    themeClass: '',
    shadowClass: '',
  },

  onLoad(options: any) {
    const countryCode = decodeURIComponent(options.countryCode || '');
    const countryName = decodeURIComponent(options.countryName || '国外城市');
    wx.setNavigationBarTitle({ title: countryName });
    this.setData({ countryCode, countryName });
  },

  async onShow() {
    await syncCustomWorldCitiesFromCloud().catch(() => {
      wx.showToast({ title: '云端城市同步失败，已显示本地数据', icon: 'none' });
    });
    const theme = getActiveTheme();
    const states = await getAllRegionStates();
    const cities = [
      ...getWorldCitiesForCountry(this.data.countryCode),
      ...getCustomWorldCitiesForCountry(this.data.countryCode),
    ].map((city) => ({
      ...city,
      lit: Boolean(states[city.regionCode]?.lit),
      visitCount: states[city.regionCode]?.visitCount || 0,
    }));

    this.setData({
      cities,
      themeStyle: themeToCssVars(theme),
      themeClass: themeClass(theme),
      shadowClass: shadowClass(theme),
    });
  },

  handleCityTap(event: any) {
    const regionCode = event.currentTarget.dataset.code;
    wx.navigateTo({
      url: `/pages/region-detail/index?regionCode=${encodeURIComponent(regionCode)}`,
    });
  },

  handleCityNameInput(event: any) {
    this.setData({ newCityName: event.detail.value });
  },

  async addCustomCity() {
    if (this.data.addingCity) return;
    const name = String(this.data.newCityName || '').trim();
    if (!name) {
      wx.showToast({ title: '先输入城市名', icon: 'none' });
      return;
    }

    this.setData({ addingCity: true });
    try {
      const city = await upsertCustomWorldCity({
        countryCode: this.data.countryCode,
        countryName: this.data.countryName,
        name,
      });
      this.setData({ newCityName: '' });
      await this.onShow();
      wx.showToast({ title: '已添加城市', icon: 'none' });
      wx.navigateTo({
        url: `/pages/region-detail/index?regionCode=${encodeURIComponent(city.regionCode)}`,
      });
    } catch {
      wx.showToast({ title: '城市保存失败，请检查网络后重试', icon: 'none' });
    } finally {
      this.setData({ addingCity: false });
    }
  },
});
