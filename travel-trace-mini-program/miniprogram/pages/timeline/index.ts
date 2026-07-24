import { findRegion } from '../../data/regions/index';
import { imageUrl } from '../../services/travel-media.service';
import { getTravelRecords } from '../../services/travel-store.service';
import { getActiveTheme } from '../../services/theme-store';
import type { TravelRecord } from '../../types/travel-record';
import { shadowClass, themeClass, themeToCssVars } from '../../utils/theme-style';

interface TimelineRecordView {
  id: string;
  regionCode: string;
  regionName: string;
  parentName: string;
  visitDate: string;
  title: string;
  content: string;
  tags: string[];
  initial: string;
  coverUrl: string;
  mediaCount: number;
}

interface TimelineYearGroup {
  year: string;
  summary: string;
  records: TimelineRecordView[];
}

function regionLabel(regionCode: string) {
  const region = findRegion(regionCode);
  const parent = region?.parentCode ? findRegion(region.parentCode) : undefined;
  return {
    regionName: region?.name || '未知地点',
    parentName: parent?.name || '',
  };
}

function toRecordView(record: TravelRecord): TimelineRecordView {
  const labels = regionLabel(record.regionCode);
  const cover = record.images[0];
  return {
    id: record.id,
    regionCode: record.regionCode,
    regionName: labels.regionName,
    parentName: labels.parentName,
    visitDate: record.visitDate,
    title: record.title || `${labels.regionName}旅行`,
    content: record.content || '这段旅程还没有写下详细故事。',
    tags: record.tags || [],
    initial: labels.regionName.slice(0, 1),
    coverUrl: cover ? imageUrl(cover) : '',
    mediaCount: record.images.length,
  };
}

function groupByYear(records: TravelRecord[]): TimelineYearGroup[] {
  const groups = new Map<string, TimelineRecordView[]>();
  records
    .slice()
    .sort((a, b) => b.visitDate.localeCompare(a.visitDate))
    .forEach((record) => {
      const year = record.visitDate.slice(0, 4) || '未知年份';
      const list = groups.get(year) || [];
      list.push(toRecordView(record));
      groups.set(year, list);
    });

  return Array.from(groups.entries()).map(([year, list]) => {
    const cities = new Set(list.map((item) => item.regionCode)).size;
    return {
      year,
      summary: `这一年，你去了 ${cities} 个地方，留下 ${list.length} 段故事。`,
      records: list,
    };
  });
}

Page({
  data: {
    themeStyle: '',
    themeClass: '',
    shadowClass: '',
    loading: true,
    totalRecords: 0,
    totalCities: 0,
    yearGroups: [] as TimelineYearGroup[],
  },

  async onShow() {
    const theme = getActiveTheme();
    this.setData({
      themeStyle: themeToCssVars(theme),
      themeClass: themeClass(theme),
      shadowClass: shadowClass(theme),
      loading: true,
    });

    const records = await getTravelRecords();
    this.setData({
      yearGroups: groupByYear(records),
      totalRecords: records.length,
      totalCities: new Set(records.map((record) => record.regionCode)).size,
      loading: false,
    });
  },

  goAddCity() {
    wx.navigateTo({ url: '/pages/city-picker/index' });
  },

  openRecord(event: any) {
    const regionCode = event.currentTarget.dataset.regionCode;
    if (!regionCode) return;
    wx.navigateTo({
      url: `/pages/region-detail/index?regionCode=${encodeURIComponent(regionCode)}`,
    });
  },
});
