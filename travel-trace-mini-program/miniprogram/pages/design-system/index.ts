import { getActiveTheme } from '../../services/theme-store';
import { themeClass, themeToCssVars } from '../../utils/theme-style';

Page({
  data: {
    themeStyle: '',
    themeClass: '',
    stats: [
      { label: '城市', value: '18', helper: '已点亮' },
      { label: '旅行', value: '42', helper: '次记录' },
      { label: '照片', value: '286', helper: '张回忆' },
    ],
  },

  onShow() {
    const theme = getActiveTheme();
    this.setData({
      themeStyle: themeToCssVars(theme),
      themeClass: themeClass(theme),
    });
  },

  showToast() {
    wx.showToast({ title: '组件反馈正常', icon: 'none' });
  },
});
