import { pointInBoundary } from '../../utils/geojson-boundary';

Component({
  data: {
    hasBoundaryData: false,
  },

  properties: {
    title: String,
    subtitle: String,
    missingText: String,
    showLabels: {
      type: Boolean,
      value: true,
    },
    showRegionList: {
      type: Boolean,
      value: true,
    },
    compact: {
      type: Boolean,
      value: false,
    },
    regions: {
      type: Array,
      value: [],
      observer(this: any) {
        this.updateBoundaryState(() => this.drawBoundaryMap());
      },
    },
    markers: {
      type: Array,
      value: [],
    },
    theme: {
      type: Object,
      value: null,
      observer(this: any) {
        this.drawBoundaryMap();
      },
    },
  },

  lifetimes: {
    ready(this: any) {
      this.updateBoundaryState(() => this.drawBoundaryMap());
    },
  },

  pageLifetimes: {
    show(this: any) {
      this.updateBoundaryState(() => {
        setTimeout(() => this.drawBoundaryMap(), 80);
      });
    },
  },

  methods: {
    updateBoundaryState(this: any, callback?: () => void) {
      const regions = this.data.regions as Array<any>;
      this.setData({
        hasBoundaryData: regions.some((region) => region.polygons?.length),
      }, callback);
    },

    drawBoundaryMap(this: any, retries = 0) {
      if (!this.data.hasBoundaryData) return;

      const query = this.createSelectorQuery();
      query
        .select('#boundaryCanvas')
        .fields({ node: true, size: true })
        .exec((result: any[]) => {
          const canvas = result?.[0]?.node;
          const width = result?.[0]?.width || 0;
          const height = result?.[0]?.height || 0;
          if (!canvas || !width || !height) {
            if (retries < 4) setTimeout(() => this.drawBoundaryMap(retries + 1), 80);
            return;
          }

          const dpr = wx.getWindowInfo().pixelRatio || 1;
          canvas.width = width * dpr;
          canvas.height = height * dpr;

          const context = canvas.getContext('2d');
          const theme = this.data.theme as any;
          const colors = theme?.colors || {};
          context.scale(dpr, dpr);
          context.clearRect(0, 0, width, height);
          context.fillStyle = colors.mapBackground || '#E4ECE7';
          context.fillRect(0, 0, width, height);

          const scaleX = width / 1000;
          const scaleY = height / 620;
          const regions = this.data.regions as Array<any>;

          regions.forEach((region) => {
            if (!region.polygons?.length) return;
            context.beginPath();
            region.polygons.forEach((ring: number[][]) => {
              ring.forEach(([x, y], index) => {
                const px = x * scaleX;
                const py = y * scaleY;
                if (index === 0) context.moveTo(px, py);
                else context.lineTo(px, py);
              });
              context.closePath();
            });
            context.fillStyle = region.lit
              ? (region.visitCount > 1 ? colors.multiVisitRegion : colors.visitedRegion) || '#65c97a'
              : colors.unvisitedRegion || '#DDD8CF';
            context.strokeStyle = region.lit ? colors.selectedRegion || '#304D40' : colors.mapBorder || '#AAB5AD';
            context.lineWidth = region.lit ? 1.6 : 1;
            context.fill('evenodd');
            context.stroke();
          });
        });
    },

    handleCanvasTap(this: any, event: any) {
      if (!this.data.hasBoundaryData) return;

      const touch = event.touches?.[0] || event.changedTouches?.[0];
      if (!touch) return;

      const query = this.createSelectorQuery();
      query
        .select('#boundaryCanvas')
        .boundingClientRect()
        .exec((result: any[]) => {
          const rect = result?.[0];
          if (!rect) return;

          const point = [
            ((touch.clientX - rect.left) / rect.width) * 1000,
            ((touch.clientY - rect.top) / rect.height) * 620,
          ];
          const region = [...(this.data.regions as Array<any>)]
            .reverse()
            .find((item) => pointInBoundary(point, item.polygons || []));

          if (region) this.triggerEvent('regiontap', region);
        });
    },

    handleRegionTap(this: any, event: any) {
      const index = event.currentTarget.dataset.index;
      const region = this.data.regions[index];
      this.triggerEvent('regiontap', region);
    },

    handleMarkerTap(this: any, event: any) {
      const index = event.currentTarget.dataset.index;
      const marker = this.data.markers[index];
      this.triggerEvent('markertap', marker);
    },
  },
});
