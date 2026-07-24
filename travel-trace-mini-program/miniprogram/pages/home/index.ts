import { chinaProvinceMap, findRegion } from '../../data/regions/index';
import { chinaProvinceGeoJson } from '../../data/geojson/china-provinces';
import { getProvinceHighlightStates } from '../../data/regions/province-city-maps';
import {
  calculateEstimatedDistance,
  formatDistanceKm,
  getHomePlaces,
  type HomePlace,
} from '../../services/home-place.service';
import { getAllRegionStates, getTravelRecords } from '../../services/travel-store.service';
import { imageUrl } from '../../services/travel-media.service';
import { getActiveTheme } from '../../services/theme-store';
import type { TravelRecord } from '../../types/travel-record';
import { toBoundaryRegionViewItems } from '../../utils/geojson-boundary';
import { shadowClass, themeClass, themeToCssVars } from '../../utils/theme-style';

interface TimelineYearView {
  label: string;
  count: number;
  active: boolean;
}

interface RecentRecordView {
  id: string;
  regionCode: string;
  year: string;
  title: string;
  place: string;
  dateText: string;
  summary: string;
  imageCount: number;
  coverUrl: string;
  toneClass: string;
}

const RECENT_RECORD_LIMIT = 3;
const DEFAULT_HOME_COVER = '/assets/home-cover-mountain.jpg';

function buildThemeViewData() {
  const theme = getActiveTheme();
  return {
    theme,
    themeStyle: themeToCssVars(theme),
    themeClass: themeClass(theme),
    shadowClass: shadowClass(theme),
    homeCoverUrl: theme.assets.homeCoverImage || DEFAULT_HOME_COVER,
  };
}

function buildHomeMapRegions(stateMap = {}) {
  return toBoundaryRegionViewItems(
    chinaProvinceMap,
    chinaProvinceGeoJson,
    getProvinceHighlightStates(stateMap),
  );
}

function openPage(url: string) {
  wx.navigateTo({
    url,
    fail: () => {
      wx.redirectTo({
        url,
        fail: () => wx.showToast({ title: '页面打开失败，请重试', icon: 'none' }),
      });
    },
  });
}

function recordYear(record: TravelRecord): string {
  const match = /^(\d{4})/.exec(record.visitDate || '');
  return match ? match[1] : '未知';
}

function formatDateText(date: string): string {
  const match = /^(\d{4})-(\d{2})-(\d{2})/.exec(date || '');
  if (!match) return date || '未记录日期';
  return `${match[1]}年${Number(match[2])}月${Number(match[3])}日`;
}

function isCityRegionCode(regionCode: string): boolean {
  const region = findRegion(regionCode);
  return region?.level === 'city' || regionCode.indexOf('world-city:') === 0;
}

function regionPlaceText(regionCode: string): string {
  const region = findRegion(regionCode);
  if (!region) return '自定义地点';

  const parent = region.parentCode ? findRegion(region.parentCode) : undefined;
  return parent ? `${region.name} · ${parent.name}` : region.name;
}

function fallbackTitle(record: TravelRecord): string {
  const region = findRegion(record.regionCode);
  return record.title || (region ? `${region.name}旅行` : '旅行记录');
}

function buildTimelineYears(records: TravelRecord[]): TimelineYearView[] {
  const years = records.reduce<Record<string, number>>((result, record) => {
    const year = recordYear(record);
    if (year === '未知') return result;
    result[year] = (result[year] || 0) + 1;
    return result;
  }, {});

  const labels = Object.keys(years).sort((a, b) => Number(a) - Number(b));
  const visibleLabels = labels.length > 0
    ? labels.slice(-4)
    : Array.from({ length: 4 }, (_, index) => String(new Date().getFullYear() - 3 + index));

  return visibleLabels.map((label) => ({
    label,
    count: years[label] || 0,
    active: Boolean(years[label]),
  }));
}

function buildRecentRecords(records: TravelRecord[]): RecentRecordView[] {
  return records
    .slice()
    .sort((a, b) => (b.visitDate || '').localeCompare(a.visitDate || ''))
    .slice(0, RECENT_RECORD_LIMIT)
    .map((record, index) => {
      const cover = record.images.find((item) => item.mediaType !== 'video') || record.images[0];
      return {
        id: record.id,
        regionCode: record.regionCode,
        year: recordYear(record),
        title: fallbackTitle(record),
        place: regionPlaceText(record.regionCode),
        dateText: formatDateText(record.visitDate),
        summary: record.content || '这次旅行还没有写下感受。',
        imageCount: record.images.length,
        coverUrl: cover ? imageUrl(cover) : '',
        toneClass: index === 1 ? 'palace' : index === 2 ? 'mountain' : '',
      };
    });
}

Page({
  data: {
    litCount: 0,
    recordCount: 0,
    ticks: Array.from({ length: 24 }, (_, index) => index),
    timelineYears: [] as TimelineYearView[],
    recentRecords: [] as RecentRecordView[],
    hasRecentRecords: false,
    homePlaces: [] as HomePlace[],
    homeCount: 0,
    distanceText: '设置常住地后计算',
    distanceHint: '按最近常住地往返估算',
    homeMapRegions: [],
    theme: null,
    themeStyle: '',
    themeClass: '',
    shadowClass: '',
    homeCoverUrl: DEFAULT_HOME_COVER,
  },

  async onShow() {
    const themeViewData = buildThemeViewData();

    // Keep the primary map and navigation usable even when a stored record is malformed.
    this.setData({
      ...themeViewData,
      homeMapRegions: buildHomeMapRegions(),
    });

    try {
      const [stateMap, records] = await Promise.all([
        getAllRegionStates(),
        getTravelRecords(),
      ]);
      const states = Object.values(stateMap);
      const homePlaces = getHomePlaces();
      const distance = calculateEstimatedDistance(records, homePlaces);

      this.setData({
        litCount: states.filter((state) => state.lit && isCityRegionCode(state.regionCode)).length,
        recordCount: records.length,
        timelineYears: buildTimelineYears(records),
        recentRecords: buildRecentRecords(records),
        hasRecentRecords: records.length > 0,
        homePlaces,
        homeCount: homePlaces.length,
        distanceText: homePlaces.length ? formatDistanceKm(distance.totalKm) : '设置常住地后计算',
        distanceHint: homePlaces.length
          ? (distance.calculatedRecordCount ? `已按 ${distance.calculatedRecordCount} 条日志估算` : '常住地或日志缺少坐标')
          : '最多可设置 5 个常住地',
        homeMapRegions: buildHomeMapRegions(stateMap),
      });
    } catch {
      wx.showToast({ title: '旅行数据加载失败，请重试', icon: 'none' });
    }
  },

  goWorldMap() {
    openPage('/packages/map/pages/world-map/index');
  },

  goChinaMap() {
    openPage('/pages/china-map/index');
  },

  goStats() {
    wx.navigateTo({ url: '/pages/stats/index' });
  },

  openHomePlace(event: { currentTarget: { dataset: { regionCode?: string } } }) {
    const regionCode = event.currentTarget.dataset.regionCode as string | undefined;
    if (!regionCode) return;
    wx.navigateTo({ url: `/pages/region-detail/index?regionCode=${encodeURIComponent(regionCode)}` });
  },

  openRegion(event: { currentTarget: { dataset: { regionCode?: string } } }) {
    const regionCode = event.currentTarget.dataset.regionCode as string | undefined;
    if (!regionCode) return;
    wx.navigateTo({ url: `/pages/region-detail/index?regionCode=${encodeURIComponent(regionCode)}` });
  },

  goAddFootprint() {
    openPage('/pages/city-picker/index');
  },

  handleCoverError() {
    if (this.data.homeCoverUrl === DEFAULT_HOME_COVER) return;
    this.setData({ homeCoverUrl: DEFAULT_HOME_COVER });
  },

  handleRecentCoverError(event: { currentTarget: { dataset: { recordId?: string } } }) {
    const recordId = event.currentTarget.dataset.recordId;
    if (!recordId) return;
    this.setData({
      recentRecords: this.data.recentRecords.map((record: RecentRecordView) => (
        record.id === recordId ? { ...record, coverUrl: '' } : record
      )),
    });
  },
});
