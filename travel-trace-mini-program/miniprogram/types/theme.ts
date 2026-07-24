export type ThemeMode = 'light' | 'dark';
export type ThemeSource = 'system-default' | 'preset' | 'custom';
export type ThemeHighlightStyle = 'solid' | 'gradient' | 'glow' | 'stars' | 'stamp' | 'footprint';
export type JournalLayout = 'timeline' | 'cards' | 'gallery' | 'list';
export type ShadowLevel = 'none' | 'light' | 'medium';

export interface ThemeConfig {
  id: string;
  name: string;
  source: ThemeSource;
  version: number;
  mode: ThemeMode;
  colors: {
    primary: string;
    primaryLight: string;
    primaryDark: string;
    secondary: string;
    accent: string;
    accentLight: string;
    background: string;
    pageBackground: string;
    cardBackground: string;
    elevatedBackground: string;
    textPrimary: string;
    textSecondary: string;
    textMuted: string;
    border: string;
    divider: string;
    visitedRegion: string;
    unvisitedRegion: string;
    wishlistRegion: string;
    selectedRegion: string;
    multiVisitRegion: string;
    mapBorder: string;
    mapBackground: string;
    buttonBackground: string;
    buttonText: string;
    success: string;
    warning: string;
    danger: string;
  };
  card: {
    borderRadius: number;
    shadowLevel: ShadowLevel;
  };
  map: {
    highlightStyle: ThemeHighlightStyle;
    showTexture: boolean;
    showRoutes: boolean;
    showAnimation: boolean;
  };
  journal: {
    layout: JournalLayout;
  };
  assets: {
    backgroundImage?: string;
    homeCoverImage?: string;
    avatarImage?: string;
  };
  updatedAt: string;
}

export interface UserPreferences {
  userId?: string;
  activeThemeId: string;
  customTheme?: ThemeConfig;
  journalLayout: JournalLayout;
  updatedAt: string;
}
