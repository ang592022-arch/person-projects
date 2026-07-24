Component({
  properties: {
    active: {
      type: String,
      value: 'map',
    },
    items: {
      type: Array,
      value: [
        { key: 'map', label: '地图', icon: '⌖', url: '/pages/home/index' },
        { key: 'footprints', label: '足迹', icon: '◇', url: '/pages/stats/index' },
        { key: 'add', label: '添加', icon: '+', url: '/pages/city-picker/index', featured: true },
        { key: 'time', label: '时光', icon: '—', url: '/pages/timeline/index' },
        { key: 'mine', label: '我的', icon: '○', url: '/pages/mine/index' },
      ],
    },
  },

  methods: {
    handleTap(this: any, event: any) {
      const item = this.data.items[event.currentTarget.dataset.index];
      this.triggerEvent('change', item);
      if (!item?.url || item.key === this.data.active) return;
      if (item.key === 'add') {
        wx.navigateTo({
          url: item.url,
          fail: () => {
            wx.redirectTo({
              url: item.url,
              fail: () => wx.showToast({ title: '页面打开失败，请重试', icon: 'none' }),
            });
          },
        });
        return;
      }
      wx.redirectTo({ url: item.url });
    },
  },
});
