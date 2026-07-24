# 个性化主题系统设计

## 本阶段范围

本阶段实现数据配置驱动的主题基础能力，不为每种风格复制页面代码。

- 已实现：主题类型、6 套系统预设、当前主题本地存储、个性化设置页、首页背景图、地图颜色、卡片圆角、暗色模式、日志布局切换。
- 暂不实现：云端同步、渐变/发光/印章点亮效果、复杂手账模板、背景图云存储删除同步。

## 核心文件

```text
miniprogram/types/theme.ts             ThemeConfig 和 UserPreferences 类型
miniprogram/data/themes/presets.ts     系统默认主题和 6 套预设主题
miniprogram/services/theme-store.ts    当前主题读写、本地存储、预设复制为自定义主题
miniprogram/utils/theme-style.ts       ThemeConfig 转 CSS 变量和主题类名
miniprogram/pages/theme-settings/      个性化设置页
```

## 数据模型

`ThemeConfig` 区分系统默认主题、系统预设主题和用户自定义主题：

- `source: 'system-default'`：系统默认主题。
- `source: 'preset'`：系统预设主题。
- `source: 'custom'`：用户从预设复制后修改得到的自定义主题。

用户修改预设颜色、圆角、背景图或日志布局时，不直接改预设原始数据，而是保存为 `customTheme`。

`UserPreferences` 当前先写入微信本地存储：

```ts
interface UserPreferences {
  userId?: string;
  activeThemeId: string;
  customTheme?: ThemeConfig;
  journalLayout: JournalLayout;
  updatedAt: string;
}
```

## 渲染方案

原生微信小程序 WXSS 支持 CSS 变量，因此本项目采用：

1. 页面 `onShow` 读取 `getActiveTheme()`。
2. 使用 `themeToCssVars(theme)` 生成 `--primary`、`--page-background`、`--visited-region` 等变量。
3. 页面根节点绑定 `style="{{themeStyle}}"`。
4. 普通页面通过 WXSS `var(--text-primary)` 等变量渲染。
5. Canvas 地图无法直接继承 WXSS 变量，因此通过 `theme="{{theme}}"` 传入 `boundary-map-board`，绘制时读取 `theme.colors`。

## 日志布局

地区详情页读取 `theme.journal.layout`：

- `timeline`：时间线模式，日志卡片左侧突出主色。
- `cards`：手账卡片模式，卡片留白更明显。
- `gallery`：照片列表模式，图片区域更突出。

后续可以继续扩展 `list` 或更复杂的模板，但仍应只新增布局配置和少量样式，不复制页面。

## 后续阶段

1. 将 `UserPreferences` 同步到云数据库，按 `_openid` 隔离。
2. 首页背景图切换为云存储 `fileID`，删除背景图时同步删除云文件。
3. 增加 `highlightStyle` 的渐变、发光、印章、足迹效果。
4. 增加更多日志模板，并保持 `ThemeConfig` 向后兼容。
