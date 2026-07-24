import { defaultTheme } from '../../data/themes/presets';
import { ensurePrivacyAuthorization } from '../../services/privacy.service';
import {
  isCloudMediaEnabled,
  prepareTravelImages,
  removeTemporaryLocalImages,
  removeTravelImages,
  rollbackPreparedImages,
} from '../../services/travel-media.service';
import { getActiveTheme, getPresetThemes, saveCustomThemeFromPreset, setActivePresetTheme, syncThemePreferencesFromCloud } from '../../services/theme-store';
import type { JournalLayout, ThemeConfig } from '../../types/theme';
import type { TravelImage } from '../../types/travel-record';
import { shadowClass, themeClass, themeToCssVars } from '../../utils/theme-style';

const colorOptions = [
  '#3f8f63',
  '#1d84b5',
  '#9b6740',
  '#222222',
  '#8fe0a2',
  '#ff8fab',
  '#f4f7ed',
  '#080d0b',
  '#74c47b',
  '#d9dfd2',
  '#f0bd64',
  '#ffffff',
];

const journalLayouts: Array<{ label: string; value: JournalLayout }> = [
  { label: '时间线', value: 'timeline' },
  { label: '手账卡片', value: 'cards' },
  { label: '照片列表', value: 'gallery' },
];

function cloneTheme(theme: ThemeConfig): ThemeConfig {
  return JSON.parse(JSON.stringify(theme));
}

function buildPreviewStyle(theme: ThemeConfig) {
  return {
    theme,
    themeStyle: themeToCssVars(theme),
    themeClass: themeClass(theme),
    shadowClass: shadowClass(theme),
    homeCoverStyle: theme.assets.homeCoverImage ? `background-image:url(${theme.assets.homeCoverImage})` : '',
    darkMode: theme.mode === 'dark',
    selectedLayout: theme.journal.layout,
  };
}

function normalizePreviewTheme(theme: ThemeConfig): ThemeConfig {
  return {
    ...theme,
    source: 'custom',
    updatedAt: new Date().toISOString(),
  };
}

function coverMedia(path: string): TravelImage {
  return {
    fileID: path,
    localPath: path.startsWith('cloud://') ? undefined : path,
    mediaType: 'image',
    uploadedAt: new Date().toISOString(),
  };
}

function relativeLuminance(hex: string): number | undefined {
  const normalized = hex.trim().replace(/^#/, '');
  const expanded = normalized.length === 3
    ? normalized.split('').map((value) => `${value}${value}`).join('')
    : normalized;
  if (!/^[0-9a-fA-F]{6}$/.test(expanded)) return undefined;

  const channels = [0, 2, 4].map((offset) => Number.parseInt(expanded.slice(offset, offset + 2), 16) / 255)
    .map((value) => value <= 0.03928 ? value / 12.92 : ((value + 0.055) / 1.055) ** 2.4);
  return channels[0] * 0.2126 + channels[1] * 0.7152 + channels[2] * 0.0722;
}

function contrastRatio(first: string, second: string): number {
  const firstLuminance = relativeLuminance(first);
  const secondLuminance = relativeLuminance(second);
  if (firstLuminance === undefined || secondLuminance === undefined) return 1;
  const lighter = Math.max(firstLuminance, secondLuminance);
  const darker = Math.min(firstLuminance, secondLuminance);
  return (lighter + 0.05) / (darker + 0.05);
}

Page({
  data: {
    presets: [],
    colorOptions,
    journalLayouts,
    theme: defaultTheme,
    themeStyle: themeToCssVars(defaultTheme),
    themeClass: themeClass(defaultTheme),
    shadowClass: shadowClass(defaultTheme),
    homeCoverStyle: '',
    darkMode: false,
    selectedLayout: 'timeline',
    dirty: false,
    warning: '',
    pendingCoverPath: '',
    saving: false,
  },

  async onLoad() {
    await syncThemePreferencesFromCloud().catch(() => {
      wx.showToast({ title: '云端主题同步失败，已显示本地设置', icon: 'none' });
    });
    const theme = getActiveTheme();
    this.setData({
      presets: getPresetThemes(),
      ...buildPreviewStyle(theme),
      dirty: false,
      warning: '',
    });
  },

  applyPreview(theme: ThemeConfig, dirty = true) {
    const warning = this.hasWeakContrast(theme)
      ? '当前文字和背景对比度偏低，建议换深一点或浅一点的颜色。'
      : '';
    this.setData({
      ...buildPreviewStyle(theme),
      dirty,
      warning,
    });
  },

  handlePresetTap(event: any) {
    const preset = (this.data.presets as ThemeConfig[]).find((item) => item.id === event.currentTarget.dataset.id);
    if (!preset) return;
    this.applyPreview(cloneTheme(preset));
  },

  handleDarkModeChange(event: any) {
    const checked = Boolean(event.detail.value);
    const theme = cloneTheme(this.data.theme as any);
    const nightTheme = (this.data.presets as ThemeConfig[]).find((item) => item.id === 'night-travel');
    const lightTheme = (this.data.presets as ThemeConfig[]).find((item) => item.id === 'fresh-nature');

    if (checked && nightTheme) {
      this.applyPreview(cloneTheme(nightTheme));
      return;
    }

    if (!checked && theme.id === 'night-travel' && lightTheme) {
      this.applyPreview(cloneTheme(lightTheme));
      return;
    }

    theme.mode = checked ? 'dark' : 'light';
    this.applyPreview(normalizePreviewTheme(theme));
  },

  handleColorTap(event: any) {
    const field = event.currentTarget.dataset.field;
    const value = event.currentTarget.dataset.value;
    const theme = cloneTheme(this.data.theme as any);
    (theme.colors as Record<string, string>)[field] = value;
    this.applyPreview(normalizePreviewTheme(theme));
  },

  handleRadiusChange(event: any) {
    const theme = cloneTheme(this.data.theme as any);
    theme.card.borderRadius = Number(event.detail.value);
    this.applyPreview(normalizePreviewTheme(theme));
  },

  handleLayoutTap(event: any) {
    const theme = cloneTheme(this.data.theme as any);
    theme.journal.layout = event.currentTarget.dataset.value;
    this.applyPreview(normalizePreviewTheme(theme));
  },

  async chooseHomeCover() {
    try {
      await ensurePrivacyAuthorization();
    } catch {
      wx.showToast({ title: '请先同意隐私保护指引', icon: 'none' });
      return;
    }

    wx.chooseMedia({
      count: 1,
      mediaType: ['image'],
      sourceType: ['album', 'camera'],
      success: (result) => {
        const file = result.tempFiles[0];
        if (!file) return;

        if (file.size > 2 * 1024 * 1024) {
          wx.showToast({ title: '图片请控制在 2MB 内', icon: 'none' });
          return;
        }

        wx.saveFile({
          tempFilePath: file.tempFilePath,
          success: (saveResult) => {
            const previousPendingPath = String(this.data.pendingCoverPath || '');
            if (previousPendingPath && previousPendingPath !== saveResult.savedFilePath) {
              removeTemporaryLocalImages([coverMedia(previousPendingPath)]);
            }
            const theme = cloneTheme(this.data.theme as any);
            theme.assets.homeCoverImage = saveResult.savedFilePath;
            this.setData({ pendingCoverPath: saveResult.savedFilePath });
            this.applyPreview(normalizePreviewTheme(theme));
          },
          fail: () => {
            wx.showToast({ title: '背景图保存失败', icon: 'none' });
          },
        });
      },
    });
  },

  removeHomeCover() {
    const pendingCoverPath = String(this.data.pendingCoverPath || '');
    if (pendingCoverPath) removeTemporaryLocalImages([coverMedia(pendingCoverPath)]);
    const theme = cloneTheme(this.data.theme as any);
    theme.assets.homeCoverImage = '';
    this.setData({ pendingCoverPath: '' });
    this.applyPreview(normalizePreviewTheme(theme));
  },

  resetTheme() {
    const theme = (this.data.presets as ThemeConfig[]).find((item) => item.id === 'mountain-journal');
    if (!theme) return;
    this.applyPreview(cloneTheme(theme));
    wx.showToast({ title: '已预览默认主题', icon: 'none' });
  },

  async saveTheme() {
    if (this.data.saving) return;
    const theme = cloneTheme(this.data.theme as any);
    if (this.hasWeakContrast(theme)) {
      wx.showToast({ title: '文字与背景对比度不足', icon: 'none' });
      return;
    }

    const previousTheme = getActiveTheme();
    const originalCover = String(theme.assets.homeCoverImage || '');
    const originalCoverMedia = originalCover ? coverMedia(originalCover) : undefined;
    let preparedCover: TravelImage[] = [];
    this.setData({ saving: true });

    try {
      if (originalCoverMedia && isCloudMediaEnabled() && !originalCover.startsWith('cloud://')) {
        preparedCover = await prepareTravelImages('theme-cover', [originalCoverMedia]);
        if (!preparedCover[0]?.fileID?.startsWith('cloud://')) throw new Error('THEME_COVER_UPLOAD_FAILED');
        theme.assets.homeCoverImage = preparedCover[0].fileID;
      }

      const preset = (this.data.presets as ThemeConfig[]).find((item) => item.id === theme.id);
      const savedTheme = preset && theme.source !== 'custom'
        ? setActivePresetTheme(theme.id)
        : saveCustomThemeFromPreset(theme);
      const previousCover = String(previousTheme.assets.homeCoverImage || '');
      const nextCover = String(savedTheme.assets.homeCoverImage || '');
      if (previousCover && previousCover !== nextCover) {
        await removeTravelImages([coverMedia(previousCover)]).catch(() => undefined);
      }
      if (originalCoverMedia && preparedCover.length) {
        removeTemporaryLocalImages([originalCoverMedia]);
      }

      this.setData({
        ...buildPreviewStyle(savedTheme),
        dirty: false,
        pendingCoverPath: '',
        saving: false,
      });
      wx.showToast({ title: '已保存主题', icon: 'success' });
    } catch {
      if (originalCoverMedia && preparedCover.length) {
        await rollbackPreparedImages([originalCoverMedia], preparedCover).catch(() => undefined);
      }
      this.setData({ saving: false });
      wx.showToast({ title: '主题保存失败，请检查网络', icon: 'none' });
    }
  },

  hasWeakContrast(theme: ThemeConfig): boolean {
    return contrastRatio(theme.colors.background, theme.colors.textPrimary) < 4.5
      || contrastRatio(theme.colors.cardBackground, theme.colors.textPrimary) < 4.5;
  },

  onUnload() {
    const pendingCoverPath = String(this.data.pendingCoverPath || '');
    if (pendingCoverPath) removeTemporaryLocalImages([coverMedia(pendingCoverPath)]);
  },
});
