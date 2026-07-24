import { allCityMaps, findRegion, worldMap } from '../../data/regions/index';
import { worldCityRegions } from '../../data/regions/world-cities';
import {
  getCustomWorldCityRegions,
  syncCustomWorldCitiesFromCloud,
  upsertCustomWorldCity,
} from '../../services/custom-world-city-store';
import {
  imageUrl,
  isCloudMediaEnabled,
  prepareTravelImages,
  rollbackPreparedImages,
  removeTemporaryLocalImages,
} from '../../services/travel-media.service';
import { ensurePrivacyAuthorization } from '../../services/privacy.service';
import { saveTravelRecord, setRegionLit } from '../../services/travel-store.service';
import {
  parseTravelImportText,
  recognizeTravelTextFromImage,
  type ParsedTravelImport,
  type ParsedTravelPlace,
} from '../../services/ocr-import.service';
import { getActiveTheme } from '../../services/theme-store';
import type { TravelImage } from '../../types/travel-record';
import { shadowClass, themeClass, themeToCssVars } from '../../utils/theme-style';

interface DisplayTravelPlace extends ParsedTravelPlace {
  selected: boolean;
  confidenceText: string;
  statusText: string;
}

const DEFAULT_COUNTRY_INDEX = Math.max(
  0,
  worldMap.regions.findIndex((country) => country.regionCode === 'country:cn'),
);

function normalizedCityParts(name: string): string[] {
  return name
    .toLowerCase()
    .split(/[ /·・\-]+/)
    .map((part) => part.replace(/[^a-z0-9\u4e00-\u9fa5]/g, ''))
    .filter((part) => part.length >= 2);
}

function findKnownCityByName(name: string) {
  const targets = normalizedCityParts(name);
  if (!targets.length) return undefined;

  return [
    ...allCityMaps.flatMap((map) => map.regions),
    ...worldCityRegions,
    ...getCustomWorldCityRegions(),
  ].find((city) => {
    const cityParts = normalizedCityParts(city.name);
    return targets.some((target) => cityParts.includes(target));
  });
}

function today(): string {
  return new Date().toISOString().slice(0, 10);
}

function now(): string {
  return new Date().toISOString();
}

function persistTemporaryImage(tempFilePath: string): Promise<string> {
  const fileSystem = wx.getFileSystemManager() as any;
  if (typeof fileSystem.saveFile !== 'function') return Promise.resolve(tempFilePath);

  return new Promise((resolve, reject) => {
    fileSystem.saveFile({
      tempFilePath,
      success: (result: { savedFilePath: string }) => resolve(result.savedFilePath),
      fail: reject,
    });
  });
}

function ocrErrorMessage(error: unknown): string {
  const code = String((error as any)?.code || (error as any)?.message || '');
  if (code.includes('OCR_NOT_CONFIGURED')) return 'OCR 服务尚未配置，请先填写云函数凭证。';
  if (/FailedOperation\.UnOpenError|UnOpenError/i.test(code)) return 'OCR 服务尚未开通，请先在腾讯云控制台开通文字识别。';
  if (/ResourceUnavailable\.ResourcePackageRunOut|ResourcePackageRunOut/i.test(code)) return '免费识别额度尚未到账或已用完，请稍后再试。';
  if (/AuthFailure|UnauthorizedOperation|InvalidCredential/i.test(code)) return 'OCR 服务没有调用权限，请检查云账号授权。';
  if (code.includes('OCR_DAILY_LIMIT')) return '今天的识别次数已用完，请明天再试。';
  if (code.includes('OCR_MONTHLY_LIMIT')) return '本月识别额度已用完，下个月会自动恢复。';
  if (code.includes('OCR_CLOUD_UNAVAILABLE')) return '当前版本未启用云开发，暂时不能识别截图。';
  if (/FUNCTION_NOT_FOUND|FunctionName/i.test(code)) return 'OCR 云函数尚未部署，请先完成云端部署。';
  if (/UPLOAD|STORAGE|downloadFile|uploadFile/i.test(code)) return '截图上传失败，请检查网络和云存储权限。';
  return '识别暂时失败，可先粘贴行程文字。';
}

function emptyParsed(): ParsedTravelImport {
  return {
    visitDate: today(),
    title: '截图导入的旅行记录',
    content: '',
    tags: [],
    tickets: [],
    detectedPlaces: [],
  };
}

function confidenceText(confidence: number): string {
  if (confidence >= 0.9) return '高';
  if (confidence >= 0.75) return '中';
  return '需确认';
}

function statusText(place: ParsedTravelPlace): string {
  if (place.regionCode) return '已匹配地图';
  if (place.countryCode) return place.needsReview ? '将创建国外城市，请核对' : '将创建国外城市';
  return '缺少国家，请手动选择';
}

function decoratePlaces(places: ParsedTravelPlace[], selectedIds: string[]): DisplayTravelPlace[] {
  return places.map((place) => ({
    ...place,
    selected: selectedIds.includes(place.id),
    confidenceText: confidenceText(place.confidence),
    statusText: statusText(place),
  }));
}

Page({
  data: {
    screenshot: null,
    screenshotUrl: '',
    ocrText: '',
    parsed: emptyParsed(),
    parsedTagsText: '',
    detectedPlaces: [] as DisplayTravelPlace[],
    selectedPlaceIds: [] as string[],
    countries: worldMap.regions,
    countryIndex: DEFAULT_COUNTRY_INDEX,
    manualCityName: '',
    recognizing: false,
    ocrError: '',
    parsing: false,
    saving: false,
    imported: false,
    selectedPlaceCount: 0,
    themeStyle: '',
    themeClass: '',
    shadowClass: '',
  },

  onShow() {
    const theme = getActiveTheme();
    this.setData({
      themeStyle: themeToCssVars(theme),
      themeClass: themeClass(theme),
      shadowClass: shadowClass(theme),
    });
  },

  async chooseScreenshot() {
    try {
      await ensurePrivacyAuthorization();
      const result = await wx.chooseMedia({
        count: 1,
        mediaType: ['image'],
        sourceType: ['album', 'camera'],
        sizeType: ['compressed'],
      });
      const file = result.tempFiles?.[0];
      if (!file) return;

      let localPath = file.tempFilePath;
      try {
        localPath = await persistTemporaryImage(file.tempFilePath);
      } catch {
        localPath = file.tempFilePath;
      }

      const screenshot: TravelImage = {
        fileID: localPath,
        tempPath: file.tempFilePath,
        localPath,
        mediaType: 'image',
        size: file.size,
        uploadedAt: now(),
      };

      this.setData({
        screenshot,
        screenshotUrl: imageUrl(screenshot),
        ocrError: '',
      });
    } catch {
      wx.showToast({ title: '没有选择截图', icon: 'none' });
    }
  },

  async recognizeScreenshot() {
    if (this.data.recognizing || this.data.saving) return;
    const screenshot = this.data.screenshot as any as TravelImage | null;
    if (!screenshot?.localPath && !screenshot?.tempPath) {
      wx.showToast({ title: '先选择截图', icon: 'none' });
      return;
    }

    this.setData({ recognizing: true, ocrError: '' });
    try {
      const text = await recognizeTravelTextFromImage(screenshot.localPath || screenshot.tempPath || screenshot.fileID);
      this.setData({ recognizing: false });

      if (!text) {
        wx.showToast({ title: '未配置 OCR 云函数，可先粘贴文字', icon: 'none' });
        return;
      }

      this.setData({ ocrText: text });
      this.parseText();
    } catch (error) {
      const message = ocrErrorMessage(error);
      this.setData({ recognizing: false, ocrError: message });
      wx.showToast({ title: message, icon: 'none', duration: 3000 });
    }
  },

  handleTextInput(event: any) {
    this.setData({ ocrText: event.detail.value });
  },

  handleCityInput(event: any) {
    this.setData({ manualCityName: event.detail.value });
  },

  handleCountryChange(event: any) {
    this.setData({ countryIndex: Number(event.detail.value) });
  },

  parseText() {
    if (this.data.parsing || this.data.saving) return;
    if (!String(this.data.ocrText || '').trim()) {
      wx.showToast({ title: '请先粘贴或识别文字', icon: 'none' });
      return;
    }
    this.parseTextAsync();
  },

  async parseTextAsync() {
    this.setData({ parsing: true });
    try {
      await syncCustomWorldCitiesFromCloud().catch(() => undefined);
      const parsed = parseTravelImportText(String(this.data.ocrText || ''));
      const selectedPlaceIds = parsed.detectedPlaces.map((place) => place.id);
      const countryIndex = parsed.countryCode
        ? (this.data.countries as any[]).findIndex((country) => country.regionCode === parsed.countryCode)
        : this.data.countryIndex;

      this.setData({
        parsed,
        parsedTagsText: parsed.tags.join(' '),
        detectedPlaces: decoratePlaces(parsed.detectedPlaces, selectedPlaceIds),
        selectedPlaceIds,
        selectedPlaceCount: selectedPlaceIds.length,
        manualCityName: parsed.cityName || parsed.regionName || this.data.manualCityName,
        countryIndex: countryIndex >= 0 ? countryIndex : this.data.countryIndex,
        parsing: false,
      });
    } catch {
      this.setData({ parsing: false });
      wx.showToast({ title: '解析失败，请检查文字内容', icon: 'none' });
    }
  },

  togglePlaceSelection(event: any) {
    const id = String(event.currentTarget.dataset.id || '');
    const selectedPlaceIds = this.data.selectedPlaceIds as any as string[];
    const nextIds = selectedPlaceIds.includes(id)
      ? selectedPlaceIds.filter((item) => item !== id)
      : [...selectedPlaceIds, id];

    this.setData({
      selectedPlaceIds: nextIds,
      selectedPlaceCount: nextIds.length,
      detectedPlaces: decoratePlaces((this.data.parsed as any as ParsedTravelImport).detectedPlaces, nextIds),
    });
  },

  handleTitleInput(event: any) {
    this.setData({ parsed: { ...(this.data.parsed as any), title: event.detail.value } });
  },

  handleDateChange(event: any) {
    this.setData({ parsed: { ...(this.data.parsed as any), visitDate: event.detail.value } });
  },

  handleContentInput(event: any) {
    this.setData({ parsed: { ...(this.data.parsed as any), content: event.detail.value } });
  },

  handleTagsInput(event: any) {
    this.setData({ parsedTagsText: event.detail.value });
  },

  async saveImport() {
    if (this.data.saving || this.data.recognizing || this.data.parsing) return;
    const parsed = this.data.parsed as any as ParsedTravelImport;
    const primaryPlace = parsed.detectedPlaces.find((place) => place.id === (this.data.selectedPlaceIds as any as string[])[0]);
    const regionCode = await this.resolveRegionCode(primaryPlace);
    if (!regionCode) {
      wx.showToast({ title: '请填写城市或选择国家', icon: 'none' });
      return;
    }

    this.setData({ saving: true });
    let preparedImages: TravelImage[] = [];
    const sourceImages = (this.data.screenshot ? [this.data.screenshot] : []) as TravelImage[];
    try {
      const screenshot = this.data.screenshot as any as TravelImage | null;
      preparedImages = screenshot ? await prepareTravelImages(regionCode, [screenshot]) : [];
      await saveTravelRecord({
        regionCode,
        visitDate: parsed.visitDate || today(),
        title: parsed.title || '截图导入的旅行记录',
        content: parsed.content || String(this.data.ocrText || '由截图导入生成。'),
        tags: this.readTags(),
        images: preparedImages,
        tickets: parsed.tickets,
      });
      await setRegionLit(regionCode, true).catch(() => undefined);
      if (screenshot && isCloudMediaEnabled()) removeTemporaryLocalImages([screenshot]);
      this.setData({ saving: false, imported: true });
      wx.showToast({ title: '已导入', icon: 'success' });
      wx.navigateTo({
        url: `/pages/region-detail/index?regionCode=${encodeURIComponent(regionCode)}`,
      });
    } catch {
      if (preparedImages.length) {
        await rollbackPreparedImages(sourceImages, preparedImages).catch(() => undefined);
      }
      this.setData({ saving: false });
      wx.showToast({ title: '导入失败，请稍后重试', icon: 'none' });
    }
  },

  async saveSelectedPlaces() {
    if (this.data.saving || this.data.recognizing || this.data.parsing) return;
    const parsed = this.data.parsed as any as ParsedTravelImport;
    const selectedIds = this.data.selectedPlaceIds as any as string[];
    const selectedPlaces = parsed.detectedPlaces.filter((place) => selectedIds.includes(place.id));
    if (!selectedPlaces.length) {
      wx.showToast({ title: '请先选择要导入的足迹', icon: 'none' });
      return;
    }

    this.setData({ saving: true });
    let successCount = 0;

    try {
      for (const [index, place] of selectedPlaces.entries()) {
        const regionCode = await this.resolveRegionCode(place);
        if (!regionCode) continue;

        await saveTravelRecord({
          regionCode,
          visitDate: parsed.visitDate || today(),
          title: `${place.cityName}旅行足迹`,
          content: parsed.content || String(this.data.ocrText || '由截图导入生成。'),
          tags: Array.from(new Set([...this.readTags(), '截图导入'])),
          images: [],
          tickets: index === 0 ? parsed.tickets : [],
        });
        await setRegionLit(regionCode, true);
        successCount += 1;
      }
    } catch {
      this.setData({ saving: false });
      wx.showToast({ title: '批量导入失败，请稍后重试', icon: 'none' });
      return;
    }

    this.setData({ saving: false, imported: successCount > 0 });
    if (!successCount) {
      wx.showToast({ title: '没有可导入的地点', icon: 'none' });
      return;
    }

    const screenshot = this.data.screenshot as any as TravelImage | null;
    if (screenshot) {
      removeTemporaryLocalImages([screenshot]);
      this.setData({ screenshot: null, screenshotUrl: '' });
    }

    wx.showToast({ title: `已导入${successCount}个足迹`, icon: 'success' });
  },

  readTags(): string[] {
    return String(this.data.parsedTagsText || '')
      .split(/[,，\s]+/)
      .map((tag) => tag.trim())
      .filter(Boolean);
  },

  async resolveRegionCode(place?: ParsedTravelPlace): Promise<string> {
    const parsed = this.data.parsed as any as ParsedTravelImport;
    if (place?.regionCode && findRegion(place.regionCode)) return place.regionCode;
    if (parsed.regionCode && findRegion(parsed.regionCode)) return parsed.regionCode;

    const cityName = String(place?.cityName || this.data.manualCityName || parsed.cityName || '').trim();
    const knownCity = findKnownCityByName(cityName);
    if (knownCity) return knownCity.regionCode;

    const countryFromPlace = place?.countryCode
      ? { regionCode: place.countryCode, name: place.countryName || place.countryCode }
      : undefined;
    const country = countryFromPlace || (this.data.countries as any[])[this.data.countryIndex];
    if (!cityName || !country?.regionCode) return '';

    if (country.regionCode === 'country:cn') {
      return place?.regionCode || parsed.regionCode || '';
    }

    const city = await upsertCustomWorldCity({
      countryCode: country.regionCode,
      countryName: country.name,
      name: cityName,
      lonLat: place?.lonLat,
    });
    return city.regionCode;
  },

  onUnload() {
    const screenshot = this.data.screenshot as any as TravelImage | null;
    if (!screenshot) return;
    if (!this.data.imported) removeTemporaryLocalImages([screenshot]);
  },
});
