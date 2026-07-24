Component({
  properties: {
    title: String,
    description: String,
    actionText: String,
  },

  methods: {
    handleAction(this: any) {
      this.triggerEvent('action');
    },
  },
});
