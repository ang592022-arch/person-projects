import { findRegion } from '../../data/regions/index';
import { getTravelRecords } from '../../services/travel-store.service';
import { getActiveTheme } from '../../services/theme-store';
import type { TravelRecord } from '../../types/travel-record';
import { summarizeRegionCodes } from '../../utils/region-statistics';
import { shadowClass, themeClass, themeToCssVars } from '../../utils/theme-style';

interface AnnualRecordView {
  id: string;
  regionName: string;
  date: string;
  title: string;
}

function recordYear(record: TravelRecord): string {
  return /^\d{4}/.test(record.visitDate || '') ? record.visitDate.slice(0, 4) : '';
}

function mostVisitedRegion(records: TravelRecord[]): string {
  const counts = new Map<string, number>();
  records.forEach((record) => counts.set(record.regionCode, (counts.get(record.regionCode) || 0) + 1));
  const top = [...counts.entries()].sort((a, b) => b[1] - a[1])[0];
  return top ? findRegion(top[0])?.name || '自定义地点' : '还没有出发';
}

function activeMonthCount(records: TravelRecord[]): number {
  return new Set(records.map((record) => record.visitDate.slice(0, 7)).filter((value) => /^\d{4}-\d{2}$/.test(value))).size;
}

Page({
  data: {
    years: [] as string[],
    yearIndex: 0,
    selectedYear: String(new Date().getFullYear()),
    tripCount: 0,
    cityCount: 0,
    provinceCount: 0,
    countryCount: 0,
    countyCount: 0,
    mediaCount: 0,
    activeMonthCount: 0,
    topRegionName: '还没有出发',
    records: [] as AnnualRecordView[],
    hasRecords: false,
    themeStyle: '',
    themeClass: '',
    shadowClass: '',
  },

  async onShow() {
    const records = await getTravelRecords();
    const currentYear = String(new Date().getFullYear());
    const years = Array.from(new Set(records.map(recordYear).filter(Boolean))).sort((a, b) => Number(b) - Number(a));
    if (!years.includes(currentYear)) years.push(currentYear);
    years.sort((a, b) => Number(b) - Number(a));

    const selectedYear = years.includes(this.data.selectedYear) ? this.data.selectedYear : years[0];
    const theme = getActiveTheme();
    this.setData({
      years,
      yearIndex: years.indexOf(selectedYear),
      selectedYear,
      themeStyle: themeToCssVars(theme),
      themeClass: themeClass(theme),
      shadowClass: shadowClass(theme),
    });
    this.buildReport(records, selectedYear);
  },

  async handleYearChange(event: any) {
    const yearIndex = Number(event.detail.value);
    const selectedYear = (this.data.years as string[])[yearIndex];
    const records = await getTravelRecords();
    this.setData({ yearIndex, selectedYear });
    this.buildReport(records, selectedYear);
  },

  buildReport(allRecords: TravelRecord[], year: string) {
    const records = allRecords
      .filter((record) => recordYear(record) === year)
      .sort((a, b) => b.visitDate.localeCompare(a.visitDate));
    const summary = summarizeRegionCodes(records.map((record) => record.regionCode));

    this.setData({
      tripCount: records.length,
      cityCount: summary.cityCount,
      provinceCount: summary.provinceCount,
      countryCount: summary.countryCount,
      countyCount: summary.countyCount,
      mediaCount: records.reduce((sum, record) => sum + record.images.length, 0),
      activeMonthCount: activeMonthCount(records),
      topRegionName: mostVisitedRegion(records),
      hasRecords: records.length > 0,
      records: records.slice(0, 6).map((record) => ({
        id: record.id,
        regionName: findRegion(record.regionCode)?.name || '自定义地点',
        date: record.visitDate,
        title: record.title || '未命名旅行',
      })),
    });
  },

  goAddFootprint() {
    wx.navigateTo({
      url: '/pages/city-picker/index',
      fail: () => wx.redirectTo({ url: '/pages/city-picker/index' }),
    });
  },
});
