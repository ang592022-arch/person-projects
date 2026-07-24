Component({
  properties: {
    loading: {
      type: Boolean,
      value: false,
    },
    disabled: {
      type: Boolean,
      value: false,
    },
  },

  methods: {
    handleTap(this: any) {
      if (this.data.loading || this.data.disabled) return;
      this.triggerEvent('tap');
    },
  },
});
