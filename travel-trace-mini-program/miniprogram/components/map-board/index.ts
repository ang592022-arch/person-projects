Component({
  properties: {
    title: String,
    subtitle: String,
    regions: {
      type: Array,
      value: [],
    },
  },

  methods: {
    handleRegionTap(this: any, event: any) {
      const index = event.currentTarget.dataset.index;
      const region = this.data.regions[index];
      this.triggerEvent('regiontap', region);
    },
  },
});
