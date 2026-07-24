import { cloudConfig } from './config/env';
import { retryPendingCloudImageCleanup } from './services/cloud-image.service';
import { retryPendingCloudRegionRefreshes } from './services/cloud-travel-store';
import { getCloudRuntimeStatus } from './services/cloud-runtime.service';
import { syncCustomWorldCitiesFromCloud } from './services/custom-world-city-store';
import { migrateKnownWorldCityData, migrateLegacyProvinceDataToCities } from './services/local-travel-store';
import { syncThemePreferencesFromCloud } from './services/theme-store';

App({
  globalData: {
    cloudReady: false,
    cloudConfigurationError: '',
  },

  onLaunch() {
    migrateKnownWorldCityData();
    migrateLegacyProvinceDataToCities();

    const cloudStatus = getCloudRuntimeStatus();
    if (cloudStatus.ready) {
      wx.cloud.init({
        env: cloudConfig.envId,
        traceUser: true,
      });
      this.globalData.cloudReady = true;
      retryPendingCloudImageCleanup().catch(() => undefined);
      retryPendingCloudRegionRefreshes().catch(() => undefined);
      syncThemePreferencesFromCloud().catch(() => undefined);
      syncCustomWorldCitiesFromCloud().catch(() => undefined);
      return;
    }

    if (cloudStatus.mode === 'cloud') {
      this.globalData.cloudConfigurationError = cloudStatus.reason;
      wx.showModal({
        title: '云开发尚未配置完成',
        content: cloudStatus.reason === 'MISSING_ENV_ID'
          ? '当前已选择云端模式，但没有填写云环境 ID。为避免旅行数据误存到本机，保存功能已停止。'
          : '当前微信基础库无法使用云开发，请升级微信后重试。',
        showCancel: false,
      });
    }
  },
});
