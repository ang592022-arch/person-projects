import { findRegion } from '../../data/regions/index';
import {
  chooseTravelMedia,
  imageUrl,
  isCloudMediaEnabled,
  prepareTravelImages,
  rollbackPreparedImages,
  removeTemporaryLocalImages,
  removeTravelImages,
} from '../../services/travel-media.service';
import { deleteTravelRecord, getTravelRecord, saveTravelRecord, setRegionLit } from '../../services/travel-store.service';
import { getActiveTheme } from '../../services/theme-store';
import type { TravelImage, TravelTicket, TravelTicketType } from '../../types/travel-record';
import { shadowClass, themeClass, themeToCssVars } from '../../utils/theme-style';

const MAX_MEDIA_COUNT = 300;

function today(): string {
  return new Date().toISOString().slice(0, 10);
}

function splitTags(value: string): string[] {
  return value
    .split(/[,，\s]+/)
    .map((tag) => tag.trim())
    .filter(Boolean);
}

function imageViews(images: TravelImage[]) {
  return images.map((image) => ({
    ...image,
    url: imageUrl(image),
    mediaType: image.mediaType || 'image',
  }));
}

function emptyTicket(): TravelTicket {
  return {
    id: `ticket-${Date.now()}`,
    type: 'flight',
    title: '',
    departure: '',
    arrival: '',
  };
}

Page({
  data: {
    regionCode: '',
    recordId: '',
    regionName: '',
    visitDate: today(),
    title: '',
    content: '',
    tagsText: '',
    companionsText: '',
    visibility: 'private',
    images: [],
    maxMediaCount: MAX_MEDIA_COUNT,
    ticketTypeLabels: ['机票', '火车票', '高铁票'],
    pendingImages: [],
    removedImages: [],
    tickets: [],
    saved: false,
    saving: false,
    deleting: false,
    themeStyle: '',
    themeClass: '',
    shadowClass: '',
  },

  onLoad(options: any) {
    this.init(options);
  },

  async init(options: any) {
    const regionCode = decodeURIComponent(options.regionCode || '');
    const recordId = decodeURIComponent(options.recordId || '');
    const region = findRegion(regionCode);
    const record = recordId ? await getTravelRecord(recordId) : undefined;
    const theme = getActiveTheme();

    if (!region || region.level !== 'city') {
      wx.showToast({ title: '请先选择城市', icon: 'none' });
      wx.navigateBack();
      return;
    }

    this.setData({
      regionCode,
      recordId,
      regionName: region?.name || '旅行地区',
      visitDate: record?.visitDate || today(),
      title: record?.title || '',
      content: record?.content || '',
      tagsText: record?.tags?.join(' ') || '',
      companionsText: record?.companions?.join('、') || '',
      visibility: record?.visibility || 'private',
      images: imageViews(record?.images || []),
      tickets: record?.tickets?.length ? record.tickets : [],
      pendingImages: [],
      removedImages: [],
      saved: false,
      themeStyle: themeToCssVars(theme),
      themeClass: themeClass(theme),
      shadowClass: shadowClass(theme),
    });
  },

  onUnload() {
    if (this.data.saved) return;
    removeTemporaryLocalImages(this.data.pendingImages as TravelImage[]);
  },

  handleDateChange(event: any) {
    this.setData({ visitDate: event.detail.value });
  },

  handleTitleInput(event: any) {
    this.setData({ title: event.detail.value });
  },

  handleContentInput(event: any) {
    this.setData({ content: event.detail.value });
  },

  handleTagsInput(event: any) {
    this.setData({ tagsText: event.detail.value });
  },

  handleCompanionsInput(event: any) {
    this.setData({ companionsText: event.detail.value });
  },

  toggleVisibility() {
    this.setData({
      visibility: this.data.visibility === 'shareable' ? 'private' : 'shareable',
    });
  },

  async chooseImages() {
    if (this.data.saving) return;
    const currentImages = this.data.images as Array<TravelImage & { url: string }>;
    const remain = Math.max(0, MAX_MEDIA_COUNT - currentImages.length);
    if (remain === 0) {
      wx.showToast({ title: `最多保存 ${MAX_MEDIA_COUNT} 个媒体`, icon: 'none' });
      return;
    }

    try {
      const selected = await chooseTravelMedia(remain);
      const pendingImages = this.data.pendingImages as TravelImage[];
      this.setData({
        images: imageViews([...currentImages, ...selected]),
        pendingImages: [...pendingImages, ...selected],
      });
    } catch {
      wx.showToast({ title: '没有选择图片或视频', icon: 'none' });
    }
  },

  async removeImage(event: any) {
    const index = event.currentTarget.dataset.index;
    const images = [...(this.data.images as Array<TravelImage & { url: string }>)];
    const removed = images.splice(index, 1);
    const pendingImages = [...(this.data.pendingImages as TravelImage[])];
    const removedImages = [...(this.data.removedImages as TravelImage[])];
    const pendingKeys = new Set(pendingImages.map((image) => image.fileID));

    if (pendingKeys.has(removed[0]?.fileID)) {
      removeTemporaryLocalImages(removed);
      this.setData({
        images,
        pendingImages: pendingImages.filter((image) => image.fileID !== removed[0]?.fileID),
      });
      return;
    }

    this.setData({
      images,
      removedImages: [...removedImages, ...removed],
    });
  },

  previewImage(event: any) {
    const index = event.currentTarget.dataset.index;
    const media = (this.data.images as Array<TravelImage & { url: string }>).map((image) => ({
      url: image.url,
      type: image.mediaType === 'video' ? 'video' : 'image',
    }));
    if (media.length === 0) return;

    if (wx.previewMedia) {
      wx.previewMedia({
        sources: media,
        current: index,
      });
      return;
    }

    const imageUrls = media.filter((item) => item.type === 'image').map((item) => item.url);
    if (imageUrls.length) wx.previewImage({ urls: imageUrls, current: imageUrls[0] });
  },

  addTicket() {
    const tickets = this.data.tickets as TravelTicket[];
    this.setData({ tickets: [...tickets, emptyTicket()] });
  },

  removeTicket(event: any) {
    const index = event.currentTarget.dataset.index;
    const tickets = [...(this.data.tickets as TravelTicket[])];
    tickets.splice(index, 1);
    this.setData({ tickets });
  },

  handleTicketTypeChange(event: any) {
    const index = event.currentTarget.dataset.index;
    const value = Number(event.detail.value);
    const types: TravelTicketType[] = ['flight', 'train', 'highSpeedRail'];
    this.updateTicket(index, 'type', types[value] || 'flight');
  },

  handleTicketInput(event: any) {
    const { index, field } = event.currentTarget.dataset;
    this.updateTicket(index, field, event.detail.value);
  },

  updateTicket(index: number, field: keyof TravelTicket, value: string) {
    const tickets = [...(this.data.tickets as TravelTicket[])];
    if (!tickets[index]) return;
    tickets[index] = {
      ...tickets[index],
      [field]: value,
    };
    this.setData({ tickets });
  },

  async saveRecord() {
    if (this.data.saving) return;
    const { regionCode, recordId, visitDate, title, content, tagsText, companionsText, visibility } = this.data;
    const rawImages = (this.data.images as Array<TravelImage & { url: string }>).map(({ url, ...image }) => image);
    const tickets = (this.data.tickets as TravelTicket[]).filter((ticket) => (
      ticket.title.trim() || ticket.departure.trim() || ticket.arrival.trim() || ticket.ticketNo?.trim()
    ));

    if (!visitDate) {
      wx.showToast({ title: '请选择到访日期', icon: 'none' });
      return;
    }

    if (!String(content).trim() && rawImages.length === 0) {
      wx.showToast({ title: '写点感受或添加照片吧', icon: 'none' });
      return;
    }

    this.setData({ saving: true });
    let preparedImages: TravelImage[] = [];
    try {
      preparedImages = await prepareTravelImages(String(regionCode), rawImages);
      await saveTravelRecord({
        id: recordId || undefined,
        regionCode,
        visitDate,
        title: String(title).trim(),
        content: String(content).trim(),
        tags: splitTags(String(tagsText)),
        companions: splitTags(String(companionsText)),
        visibility: visibility === 'shareable' ? 'shareable' : 'private',
        images: preparedImages,
        tickets,
      });
      await setRegionLit(regionCode, true).catch(() => undefined);
      await removeTravelImages(this.data.removedImages as TravelImage[]).catch(() => undefined);
      if (isCloudMediaEnabled()) removeTemporaryLocalImages(this.data.pendingImages as TravelImage[]);

      this.setData({ saving: false, saved: true, pendingImages: [], removedImages: [] });
      wx.showToast({ title: '已保存', icon: 'success' });
      setTimeout(() => wx.navigateBack(), 500);
    } catch (error) {
      if (preparedImages.length) {
        await rollbackPreparedImages(rawImages, preparedImages).catch(() => undefined);
      }
      this.setData({ saving: false });
      wx.showToast({ title: '保存失败，请稍后重试', icon: 'none' });
    }
  },

  deleteRecord() {
    const { recordId, deleting, saving } = this.data;
    if (deleting || saving) return;
    if (!recordId) return;

    wx.showModal({
      title: '删除这条日志？',
      content: '删除后会同步移除这条日志里的媒体文件引用。',
      confirmColor: '#B86B63',
      success: async (result: any) => {
        if (!result.confirm) return;
        this.setData({ deleting: true });
        try {
          const deleted = await deleteTravelRecord(recordId);
          if (deleted) await removeTravelImages(deleted.images).catch(() => undefined);
          removeTemporaryLocalImages(this.data.pendingImages as TravelImage[]);
          this.setData({ deleting: false, saved: true, pendingImages: [], removedImages: [] });
          wx.showToast({ title: '已删除', icon: 'none' });
          setTimeout(() => wx.navigateBack(), 500);
        } catch {
          this.setData({ deleting: false });
          wx.showToast({ title: '删除失败，请稍后重试', icon: 'none' });
        }
      },
    });
  },
});
