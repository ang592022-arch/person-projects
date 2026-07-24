import type { ThemeConfig } from '../types/theme';

export function themeToCssVars(theme: ThemeConfig): string {
  const colors = theme.colors;
  return [
    `--color-primary:${colors.primary}`,
    `--color-primary-light:${colors.primaryLight}`,
    `--color-primary-dark:${colors.primaryDark}`,
    `--color-accent:${colors.accent}`,
    `--color-accent-light:${colors.accentLight}`,
    `--color-page:${colors.pageBackground || colors.background}`,
    `--color-card:${colors.cardBackground}`,
    `--color-elevated:${colors.elevatedBackground}`,
    `--color-text:${colors.textPrimary}`,
    `--color-text-secondary:${colors.textSecondary}`,
    `--color-text-muted:${colors.textMuted}`,
    `--color-border:${colors.border}`,
    `--color-divider:${colors.divider}`,
    `--color-success:${colors.success}`,
    `--color-warning:${colors.warning}`,
    `--color-danger:${colors.danger}`,
    `--primary:${colors.primary}`,
    `--secondary:${colors.secondary}`,
    `--page-background:${colors.pageBackground || colors.background}`,
    `--card-background:${colors.cardBackground}`,
    `--text-primary:${colors.textPrimary}`,
    `--text-secondary:${colors.textSecondary}`,
    `--visited-region:${colors.visitedRegion}`,
    `--unvisited-region:${colors.unvisitedRegion}`,
    `--wishlist-region:${colors.wishlistRegion}`,
    `--selected-region:${colors.selectedRegion}`,
    `--multi-visit-region:${colors.multiVisitRegion}`,
    `--map-border:${colors.mapBorder}`,
    `--map-background:${colors.mapBackground}`,
    `--button-background:${colors.buttonBackground}`,
    `--button-text:${colors.buttonText}`,
    `--danger:${colors.danger}`,
    `--card-radius:${theme.card.borderRadius}rpx`,
  ].join(';');
}

export function themeClass(theme: ThemeConfig): string {
  return `theme-${theme.mode}`;
}

export function shadowClass(theme: ThemeConfig): string {
  return `theme-shadow-${theme.card.shadowLevel}`;
}
