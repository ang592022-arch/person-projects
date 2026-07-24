import { getAllRegionStates, getTravelRecords } from '../../services/travel-store.service';
import { getActiveTheme } from '../../services/theme-store';
import { summarizeRegionCodes } from '../../utils/region-statistics';
import { shadowClass, themeClass, themeToCssVars } from '../../utils/theme-style';

interface MineEntry {
  title: string;
  subtitle: string;
  action: string;
}

function daysSince(dateText: string): number {
  if (!dateText) return 1;
  const start = new Date(dateText).getTime();
  if (Number.isNaN(start)) return 1;
  const diff = Date.now() - start;
  return Math.max(1, Math.ceil(diff / 86400000));
}

Page({
  data: {
    themeStyle: '',
    themeClass: '',
    shadowClass: '',
    joinedDays: 1,
    litCount: 0,
    recordCount: 0,
    journalCount: 0,
    mediaCount: 0,
    cityCount: 0,
    countryCount: 0,
    entries: [
      { title: '个性化主题', subtitle: '调整地图、手账和颜色风格', action: 'theme' },
      { title: '截图导入', subtitle: '识别携程、高德、机票和车票足迹', action: 'import' },
      { title: '我的想去清单', subtitle: '整理下一次出发的目的地', action: 'wishlist' },
      { title: '旅行数据', subtitle: '查看点亮地区、日志和影像统计', action: 'stats' },
      { title: '年度旅行报告', subtitle: '按年份回顾走过的国家、省份与城市', action: 'annualReport' },
      { title: '国外城市', subtitle: '维护识别后新增的海外城市', action: 'foreignCities' },
      { title: '数据备份', subtitle: '配置云开发后迁移本地数据', action: 'backup' },
      { title: '隐私设置', subtitle: '相册、截图识别和旅行数据说明', action: 'privacy' },
      { title: '关于漫迹', subtitle: 'Travel Trace 版本和项目说明', action: 'about' },
    ] as MineEntry[],
  },

  async onShow() {
    const theme = getActiveTheme();
    const states = Object.values(await getAllRegionStates());
    const records = await getTravelRecords();
    const visitedCodes = Array.from(new Set([
      ...states.filter((state) => state.lit).map((state) => state.regionCode),
      ...records.map((record) => record.regionCode),
    ]));
    const regionSummary = summarizeRegionCodes(visitedCodes);

    const firstRecordDate = records
      .map((record) => record.createdAt || record.visitDate)
      .sort((a, b) => a.localeCompare(b))[0];

    this.setData({
      themeStyle: themeToCssVars(theme),
      themeClass: themeClass(theme),
      shadowClass: shadowClass(theme),
      joinedDays: daysSince(firstRecordDate),
      litCount: states.filter((state) => state.lit).length,
      recordCount: records.length,
      journalCount: records.filter((record) => record.content || record.title).length,
      mediaCount: records.reduce((sum, record) => sum + record.images.length, 0),
      cityCount: regionSummary.cityCount,
      countryCount: regionSummary.countryCount,
    });
  },

  handleEntryTap(event: any) {
    const action = event.currentTarget.dataset.action;
    const routes: Record<string, string> = {
      theme: '/pages/theme-settings/index',
      import: '/pages/import-assistant/index',
      stats: '/pages/stats/index',
      annualReport: '/pages/annual-report/index',
      foreignCities: '/pages/foreign-city-list/index',
      backup: '/pages/stats/index',
    };

    if (routes[action]) {
      wx.navigateTo({ url: routes[action] });
      return;
    }

    if (action === 'privacy' && typeof (wx as any).openPrivacyContract === 'function') {
      (wx as any).openPrivacyContract({
        fail: () => {
          wx.showModal({
            title: '隐私说明',
            content: '隐私保护指引暂时无法打开，请稍后重试或联系开发者。',
            showCancel: false,
            confirmColor: '#496B5C',
          });
        },
      });
      return;
    }

    if (action === 'privacy') {
      wx.showModal({
        title: '隐私说明',
        content: '漫迹会在本机或你的云开发账号中保存旅行记录、图片视频、主题偏好和截图识别结果。旅行日记默认为私密，只有你主动分享的摘要会出现在分享页。',
        showCancel: false,
        confirmColor: '#496B5C',
      });
      return;
    }

    if (action === 'about') {
      wx.showModal({
        title: '漫迹 · Travel Trace',
        content: '由个人开发者独立制作的旅行地图学习作品，当前为非公开体验版本。功能聚焦地图点亮、旅行日志、图片视频、主题和截图导入。',
        showCancel: false,
        confirmColor: '#496B5C',
      });
      return;
    }

    wx.showToast({ title: '这个入口正在完善中', icon: 'none' });
  },
});
