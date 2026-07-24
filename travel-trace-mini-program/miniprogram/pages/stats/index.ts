import { canMigrateLocalDataToCloud, migrateLocalDataToCloud } from '../../services/data-migration.service';
import { calculateEstimatedDistance, formatDistanceKm, getHomePlaces } from '../../services/home-place.service';
import { getAllRegionStates, getTravelRecords } from '../../services/travel-store.service';
import { getActiveTheme } from '../../services/theme-store';
import { summarizeRegionCodes } from '../../utils/region-statistics';
import { shadowClass, themeClass, themeToCssVars } from '../../utils/theme-style';

Page({
  data: {
    recordCount: 0,
    cityCount: 0,
    provinceCount: 0,
    countryCount: 0,
    countyCount: 0,
    photoCount: 0,
    distanceText: '设置常住地后计算',
    homeCount: 0,
    recentRecords: [],
    canMigrate: false,
    migrating: false,
    migrationSummary: '',
    themeStyle: '',
    themeClass: '',
    shadowClass: '',
  },

  async onShow() {
    const theme = getActiveTheme();
    const states = Object.values(await getAllRegionStates());
    const allRecords = await getTravelRecords();
    const records = allRecords.slice().sort((a, b) => b.visitDate.localeCompare(a.visitDate));
    const visitedCodes = Array.from(new Set([
      ...states.filter((state) => state.lit).map((state) => state.regionCode),
      ...records.map((record) => record.regionCode),
    ]));
    const regionSummary = summarizeRegionCodes(visitedCodes);
    const homePlaces = getHomePlaces();
    const distance = calculateEstimatedDistance(allRecords, homePlaces);
    this.setData({
      recordCount: records.length,
      cityCount: regionSummary.cityCount,
      provinceCount: regionSummary.provinceCount,
      countryCount: regionSummary.countryCount,
      countyCount: regionSummary.countyCount,
      photoCount: records.reduce((sum, record) => sum + record.images.length, 0),
      distanceText: homePlaces.length ? formatDistanceKm(distance.totalKm) : '设置常住地后计算',
      homeCount: homePlaces.length,
      recentRecords: records.slice(0, 5),
      canMigrate: canMigrateLocalDataToCloud(),
      themeStyle: themeToCssVars(theme),
      themeClass: themeClass(theme),
      shadowClass: shadowClass(theme),
    });
  },

  goAnnualReport() {
    wx.navigateTo({
      url: '/pages/annual-report/index',
      fail: () => wx.redirectTo({ url: '/pages/annual-report/index' }),
    });
  },

  async migrateToCloud() {
    if (this.data.migrating) return;

    if (!canMigrateLocalDataToCloud()) {
      wx.showToast({ title: '请先配置云环境并切换 cloud 模式', icon: 'none' });
      return;
    }

    const confirmed = await new Promise<boolean>((resolve) => {
      wx.showModal({
        title: '迁移本地数据到云端？',
        content: '会上传本地旅行记录、点亮状态、媒体文件、自定义国外城市和主题偏好。已有相同记录会跳过。',
        confirmColor: '#496B5C',
        success: (result) => resolve(Boolean(result.confirm)),
        fail: () => resolve(false),
      });
    });
    if (!confirmed) return;

    this.setData({ migrating: true, migrationSummary: '正在迁移，请保持小程序打开。' });
    const report = await migrateLocalDataToCloud();
    const summary = [
      `记录 ${report.travelRecords} 条`,
      `跳过 ${report.skippedRecords} 条`,
      `点亮状态 ${report.regionStates} 个`,
      `媒体 ${report.mediaFiles} 个`,
      `国外城市 ${report.customWorldCities} 个`,
      report.failedMediaFiles ? `媒体失败 ${report.failedMediaFiles} 个` : '',
      report.errors.length ? `错误：${report.errors.join('；')}` : '',
    ].filter(Boolean).join('，');

    this.setData({
      migrating: false,
      migrationSummary: summary,
    });
    wx.showToast({ title: report.ok ? '迁移完成' : '迁移完成但有提示', icon: 'none' });
    await this.onShow();
  },
});
