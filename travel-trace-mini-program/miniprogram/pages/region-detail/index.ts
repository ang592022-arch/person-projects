import { findRegion } from '../../data/regions/index';
import { getProvinceForCity } from '../../data/regions/province-city-maps';
import { getWorldCityCountry } from '../../data/regions/world-cities';
import { addHomePlace, isHomePlace, MAX_HOME_PLACES, removeHomePlace } from '../../services/home-place.service';
import { imageUrl, removeTravelImages } from '../../services/travel-media.service';
import { getActiveTheme } from '../../services/theme-store';
import {
  deleteTravelRecord,
  getRegionState,
  getTravelRecords,
  setRegionLit,
} from '../../services/travel-store.service';
import type { TravelRecord } from '../../types/travel-record';
import { shadowClass, themeClass, themeToCssVars } from '../../utils/theme-style';

function ticketTypeLabel(type: string): string {
  if (type === 'flight') return '机票';
  if (type === 'train') return '火车票';
  return '高铁票';
}

function recordViews(records: TravelRecord[]) {
  return records.map((record) => ({
    ...record,
    tagText: record.tags.join(' · '),
    companionText: (record.companions || []).join('、'),
    isShareable: record.visibility === 'shareable',
    privacyText: record.visibility === 'shareable' ? '可分享摘要' : '仅自己可见',
    mediaItems: record.images.map((image) => ({
      ...image,
      url: imageUrl(image),
      mediaType: image.mediaType || 'image',
    })),
    imageCount: record.images.length,
    ticketCount: record.tickets?.length || 0,
    ticketSummary: (record.tickets || [])
      .map((ticket) => `${ticketTypeLabel(ticket.type)} ${ticket.departure || '?'}-${ticket.arrival || '?'}`)
      .join(' · '),
  }));
}

function monthText(dateText: string): string {
  if (!dateText) return '未记录';
  const [year, month] = dateText.split('-');
  if (!year || !month) return dateText;
  return `${year}年${Number(month)}月`;
}

Page({
  data: {
    regionCode: '',
    regionName: '',
    parentName: '',
    levelText: '',
    lit: false,
    visitCount: 0,
    firstVisitDate: '',
    recentVisitDate: '',
    recentVisitMonth: '未记录',
    photoCount: 0,
    coverUrl: '',
    records: [],
    isHomePlace: false,
    theme: null,
    themeStyle: '',
    themeClass: '',
    shadowClass: '',
    journalLayout: 'timeline',
    journalTitle: '旅行时间线',
    togglingLit: false,
    deletingRecordId: '',
  },

  onLoad(options: any) {
    const regionCode = decodeURIComponent(options.regionCode || '');
    const region = findRegion(regionCode);

    if (!region) {
      wx.showToast({ title: '没有找到这个地区', icon: 'none' });
      wx.navigateBack();
      return;
    }

    if (region.level === 'province') {
      wx.redirectTo({
        url: `/packages/map/pages/city-map/index?provinceCode=${encodeURIComponent(region.regionCode)}`,
      });
      return;
    }

    this.setData({
      regionCode,
      regionName: region.name,
      parentName: getProvinceForCity(regionCode)?.name || getWorldCityCountry(regionCode)?.countryName || '',
      levelText: region.level === 'city' ? '城市旅行空间' : '地区旅行空间',
    });
  },

  onShow() {
    this.refresh();
  },

  async refresh() {
    const { regionCode } = this.data;
    if (!regionCode) return;

    const state = await getRegionState(regionCode);
    const records = (await getTravelRecords(regionCode))
      .slice()
      .sort((a, b) => b.visitDate.localeCompare(a.visitDate));
    const theme = getActiveTheme();
    const journalLayout = theme.journal.layout;
    const firstRecord = records[records.length - 1];
    const recentRecord = records[0];
    const coverImage = records.find((record) => record.images.length > 0)?.images[0];
    const firstVisitDate = state?.firstVisitDate || firstRecord?.visitDate || '';
    const recentVisitDate = state?.lastVisitDate || recentRecord?.visitDate || '';

    this.setData({
      lit: Boolean(state?.lit),
      visitCount: state?.visitCount || records.length,
      firstVisitDate,
      recentVisitDate,
      recentVisitMonth: monthText(recentVisitDate),
      photoCount: records.reduce((sum, record) => sum + record.images.length, 0),
      coverUrl: coverImage ? imageUrl(coverImage) : '',
      records: recordViews(records),
      isHomePlace: isHomePlace(regionCode),
      theme,
      themeStyle: themeToCssVars(theme),
      themeClass: themeClass(theme),
      shadowClass: shadowClass(theme),
      journalLayout,
      journalTitle: journalLayout === 'cards' ? '手账卡片' : journalLayout === 'gallery' ? '照片列表' : '旅行时间线',
    });
  },

  goAddFirstRecord() {
    this.goEdit();
  },

  async toggleLit() {
    if (this.data.togglingLit) return;
    const { regionCode, lit } = this.data;
    this.setData({ togglingLit: true });
    try {
      await setRegionLit(regionCode, !lit);
      await this.refresh();
      wx.showToast({
        title: !lit ? '已经点亮' : '已经取消点亮',
        icon: 'none',
      });
    } catch {
      wx.showToast({ title: '点亮状态保存失败，请重试', icon: 'none' });
    } finally {
      this.setData({ togglingLit: false });
    }
  },

  toggleHomePlace() {
    const { regionCode, isHomePlace: currentIsHomePlace } = this.data;
    if (currentIsHomePlace) {
      removeHomePlace(regionCode);
      this.setData({ isHomePlace: false });
      wx.showToast({ title: '已取消常住地', icon: 'none' });
      return;
    }

    const result = addHomePlace(regionCode);
    if (!result.ok && result.reason === 'limit-reached') {
      wx.showToast({ title: `最多设置 ${MAX_HOME_PLACES} 个常住地`, icon: 'none' });
      return;
    }

    if (!result.ok) {
      wx.showToast({ title: '这个地区暂时不能设为家', icon: 'none' });
      return;
    }

    this.setData({ isHomePlace: true });
    wx.showToast({ title: '已设为常住地', icon: 'none' });
  },

  goEdit() {
    wx.navigateTo({
      url: `/pages/record-edit/index?regionCode=${encodeURIComponent(this.data.regionCode)}`,
    });
  },

  editRecord(event: any) {
    const recordId = event.currentTarget.dataset.id;
    wx.navigateTo({
      url: `/pages/record-edit/index?regionCode=${encodeURIComponent(this.data.regionCode)}&recordId=${encodeURIComponent(recordId)}`,
    });
  },

  previewRecordImages(event: any) {
    const recordIndex = event.currentTarget.dataset.recordIndex;
    const imageIndex = event.currentTarget.dataset.imageIndex;
    const record = (this.data.records as Array<any>)[recordIndex];
    if (!record?.mediaItems?.length) return;

    const sources = record.mediaItems.map((item: any) => ({
      url: item.url,
      type: item.mediaType === 'video' ? 'video' : 'image',
    }));

    if (wx.previewMedia) {
      wx.previewMedia({
        sources,
        current: imageIndex,
      });
      return;
    }

    const imageUrls = sources.filter((item: any) => item.type === 'image').map((item: any) => item.url);
    if (imageUrls.length) wx.previewImage({ urls: imageUrls, current: imageUrls[0] });
  },

  deleteRecord(event: any) {
    const recordId = event.currentTarget.dataset.id;
    if (!recordId || this.data.deletingRecordId) return;
    wx.showModal({
      title: '删除这条日志？',
      content: '删除后会更新这个地区的到访次数，并移除这条日志中的媒体文件。',
      confirmColor: '#B86B63',
      success: async (result: any) => {
        if (!result.confirm) return;
        this.setData({ deletingRecordId: recordId });
        try {
          const deleted = await deleteTravelRecord(recordId);
          if (deleted) await removeTravelImages(deleted.images).catch(() => undefined);
          await this.refresh();
          wx.showToast({ title: '已删除', icon: 'none' });
        } catch {
          wx.showToast({ title: '删除失败，请稍后重试', icon: 'none' });
        } finally {
          this.setData({ deletingRecordId: '' });
        }
      },
    });
  },

  onShareAppMessage(event: any) {
    const recordId = event?.target?.dataset?.id;
    const record = (this.data.records as Array<any>).find((item) => item.id === recordId && item.isShareable);
    if (!record) {
      return {
        title: `${this.data.regionName}旅行空间`,
        path: '/pages/home/index',
      };
    }

    const summary = String(record.content || '这段旅程没有填写公开摘要。').trim().slice(0, 60);
    const shareTitle = String(record.title || `${this.data.regionName}旅行`).slice(0, 40);
    const sharePlace = String(this.data.regionName || '一次旅行').slice(0, 24);
    const query = [
      ['place', sharePlace],
      ['date', record.visitDate || ''],
      ['title', shareTitle],
      ['summary', summary],
    ].map(([key, value]) => `${key}=${encodeURIComponent(value)}`).join('&');

    return {
      title: `${sharePlace} · ${shareTitle}`,
      path: `/pages/trip-share/index?${query}`,
    };
  },
});
