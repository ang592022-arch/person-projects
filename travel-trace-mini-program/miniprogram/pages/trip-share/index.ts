import { getActiveTheme } from '../../services/theme-store';
import { shadowClass, themeClass, themeToCssVars } from '../../utils/theme-style';

function readOption(value?: string): string {
  if (!value) return '';
  try {
    return decodeURIComponent(value);
  } catch {
    return value;
  }
}

Page({
  data: {
    place: '一次旅行',
    date: '',
    title: '旅行片段',
    summary: '这段旅程没有填写公开摘要。',
    sharePath: '/pages/trip-share/index',
    themeStyle: '',
    themeClass: '',
    shadowClass: '',
  },

  onLoad(options: any) {
    const place = (readOption(options.place) || '一次旅行').slice(0, 24);
    const date = readOption(options.date).slice(0, 20);
    const title = (readOption(options.title) || '旅行片段').slice(0, 40);
    const summary = (readOption(options.summary) || '这段旅程没有填写公开摘要。').slice(0, 60);
    const theme = getActiveTheme();
    const query = [
      ['place', place],
      ['date', date],
      ['title', title],
      ['summary', summary],
    ].map(([key, value]) => `${key}=${encodeURIComponent(value)}`).join('&');

    this.setData({
      place,
      date,
      title,
      summary,
      sharePath: `/pages/trip-share/index?${query}`,
      themeStyle: themeToCssVars(theme),
      themeClass: themeClass(theme),
      shadowClass: shadowClass(theme),
    });
  },

  onShareAppMessage() {
    return {
      title: `${this.data.place} · ${this.data.title}`,
      path: this.data.sharePath,
    };
  },
});
