export const cloudConfig = {
  // Copy this file to env.ts, then fill your WeChat CloudBase environment ID.
  // Keep it empty for local preview.
  envId: '',

  // local: store records and media on the current device, suitable for development preview.
  // cloud: reserved for the next migration step, using cloud database and cloud storage.
  storageMode: 'local' as 'local' | 'cloud',

  collections: {
    users: 'users',
    regionStates: 'user_region_states',
    travelRecords: 'travel_records',
    userPreferences: 'user_preferences',
    customWorldCities: 'custom_world_cities',
    ocrUsage: 'ocr_usage',
  },
};

export const amapConfig = {
  // Optional: fill a Gaode Web Service key only in your private env.ts.
  // Do not commit real keys to a public repository.
  key: '',
  serviceBaseUrl: 'https://restapi.amap.com',
};
