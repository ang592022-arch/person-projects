import { cloudConfig } from '../config/env';
import { defaultTheme, findPresetTheme, presetThemes } from '../data/themes/presets';
import type { JournalLayout, ThemeConfig, UserPreferences } from '../types/theme';
import { assertCloudRuntimeReady, isCloudMode, isCloudRuntimeReady } from './cloud-runtime.service';

const PREFERENCES_KEY = 'travel-map:user-preferences';
const THEME_VERSION = 1;

function now(): string {
  return new Date().toISOString();
}

function updatedAtValue(value?: string): number {
  const timestamp = value ? new Date(value).getTime() : 0;
  return Number.isFinite(timestamp) ? timestamp : 0;
}

function cloneTheme(theme: ThemeConfig): ThemeConfig {
  return JSON.parse(JSON.stringify(theme));
}

function isCloudEnabled(): boolean {
  return isCloudRuntimeReady();
}

function db() {
  if (!wx.cloud || !cloudConfig.envId) throw new Error('请先配置云开发环境 ID');
  return wx.cloud.database();
}

function readPreferences(): UserPreferences | undefined {
  try {
    return wx.getStorageSync(PREFERENCES_KEY) || undefined;
  } catch {
    return undefined;
  }
}

function writePreferences(preferences: UserPreferences, syncCloud = true): void {
  if (syncCloud && isCloudMode()) assertCloudRuntimeReady();
  wx.setStorageSync(PREFERENCES_KEY, preferences);
  if (syncCloud) {
    savePreferencesToCloud(preferences).catch(() => undefined);
  }
}

async function savePreferencesToCloud(preferences: UserPreferences): Promise<void> {
  if (!isCloudEnabled()) return;

  const collection = db().collection(cloudConfig.collections.userPreferences);
  const existing = await collection.limit(1).get();
  const data = {
    ...preferences,
    updatedAt: preferences.updatedAt || now(),
  };

  if (existing.data?.[0]?._id) {
    await collection.doc(existing.data[0]._id).update({ data });
    return;
  }

  await collection.add({ data });
}

export function getPresetThemes(): ThemeConfig[] {
  return presetThemes.map(cloneTheme);
}

export function mergeTheme(partial?: ThemeConfig): ThemeConfig {
  const source = partial || defaultTheme;
  return {
    ...cloneTheme(defaultTheme),
    ...source,
    version: source.version || THEME_VERSION,
    colors: {
      ...defaultTheme.colors,
      ...(source.colors || {}),
    },
    card: {
      ...defaultTheme.card,
      ...(source.card || {}),
    },
    map: {
      ...defaultTheme.map,
      ...(source.map || {}),
    },
    journal: {
      ...defaultTheme.journal,
      ...(source.journal || {}),
    },
    assets: {
      ...defaultTheme.assets,
      ...(source.assets || {}),
    },
    updatedAt: source.updatedAt || now(),
  };
}

export function getUserPreferences(): UserPreferences {
  const preferences = readPreferences();
  if (!preferences) {
    return {
      activeThemeId: defaultTheme.id,
      journalLayout: defaultTheme.journal.layout,
      updatedAt: now(),
    };
  }

  return {
    activeThemeId: preferences.activeThemeId || defaultTheme.id,
    customTheme: preferences.customTheme ? mergeTheme(preferences.customTheme) : undefined,
    journalLayout: preferences.journalLayout || preferences.customTheme?.journal.layout || defaultTheme.journal.layout,
    updatedAt: preferences.updatedAt || now(),
  };
}

export function getActiveTheme(): ThemeConfig {
  const preferences = getUserPreferences();
  if (preferences.customTheme) {
    return mergeTheme(preferences.customTheme);
  }

  return mergeTheme(findPresetTheme(preferences.activeThemeId) || defaultTheme);
}

export function saveActiveTheme(theme: ThemeConfig): void {
  const nextTheme = mergeTheme({
    ...theme,
    source: theme.source === 'preset' || theme.source === 'system-default' ? theme.source : 'custom',
    updatedAt: now(),
  });

  writePreferences({
    activeThemeId: nextTheme.id,
    customTheme: nextTheme.source === 'custom' ? nextTheme : undefined,
    journalLayout: nextTheme.journal.layout,
    updatedAt: now(),
  });
}

export function saveCustomThemeFromPreset(theme: ThemeConfig): ThemeConfig {
  const customTheme = mergeTheme({
    ...theme,
    id: 'custom-theme',
    name: '我的自定义主题',
    source: 'custom',
    updatedAt: now(),
  });
  saveActiveTheme(customTheme);
  return customTheme;
}

export function setActivePresetTheme(themeId: string): ThemeConfig {
  const theme = mergeTheme(findPresetTheme(themeId) || defaultTheme);
  writePreferences({
    activeThemeId: theme.id,
    journalLayout: theme.journal.layout,
    updatedAt: now(),
  });
  return theme;
}

export function setJournalLayout(layout: JournalLayout): ThemeConfig {
  const theme = getActiveTheme();
  const customTheme = saveCustomThemeFromPreset({
    ...theme,
    journal: {
      ...theme.journal,
      layout,
    },
  });
  return customTheme;
}

export function resetThemePreferences(): ThemeConfig {
  writePreferences({
    activeThemeId: defaultTheme.id,
    journalLayout: defaultTheme.journal.layout,
    updatedAt: now(),
  });
  return mergeTheme(defaultTheme);
}

export async function syncThemePreferencesFromCloud(): Promise<UserPreferences> {
  if (!isCloudMode()) return getUserPreferences();
  assertCloudRuntimeReady();

  const result = await db()
    .collection(cloudConfig.collections.userPreferences)
    .limit(1)
    .get();
  const cloudPreferences = result.data?.[0] as UserPreferences | undefined;
  const localPreferences = getUserPreferences();

  if (!cloudPreferences) {
    await savePreferencesToCloud(localPreferences);
    return localPreferences;
  }

  if (updatedAtValue(localPreferences.updatedAt) > updatedAtValue(cloudPreferences.updatedAt)) {
    await savePreferencesToCloud(localPreferences);
    return localPreferences;
  }

  const normalized: UserPreferences = {
    activeThemeId: cloudPreferences.activeThemeId || defaultTheme.id,
    customTheme: cloudPreferences.customTheme ? mergeTheme(cloudPreferences.customTheme) : undefined,
    journalLayout: cloudPreferences.journalLayout || cloudPreferences.customTheme?.journal.layout || defaultTheme.journal.layout,
    updatedAt: cloudPreferences.updatedAt || now(),
  };
  writePreferences(normalized, false);
  return normalized;
}
