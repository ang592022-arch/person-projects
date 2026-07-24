import type { Region, RegionMap } from '../../types/region';
import { generatedWorldCityData } from './world-cities.generated';

export interface WorldCityRegion extends Region {
  countryCode: string;
  countryName: string;
  lonLat?: [number, number];
}

function city(
  countrySlug: string,
  countryName: string,
  slug: string,
  name: string,
  lonLat: [number, number],
): WorldCityRegion {
  return {
    regionCode: `world-city:${countrySlug}:${slug}`,
    name,
    level: 'city',
    parentCode: `country:${countrySlug}`,
    countryCode: `country:${countrySlug}`,
    countryName,
    lonLat,
    center: [500, 310],
    shape: { type: 'rect', x: 0, y: 0, width: 1, height: 1 },
  };
}

const coreWorldCityRegions: WorldCityRegion[] = [
  city('japan', 'Japan', 'tokyo', '东京 Tokyo', [139.6917, 35.6895]),
  city('japan', 'Japan', 'osaka', '大阪 Osaka', [135.5023, 34.6937]),
  city('japan', 'Japan', 'kyoto', '京都 Kyoto', [135.7681, 35.0116]),
  city('japan', 'Japan', 'sapporo', '札幌 Sapporo', [141.3545, 43.0618]),
  city('south-korea', 'South Korea', 'seoul', '首尔 Seoul', [126.978, 37.5665]),
  city('south-korea', 'South Korea', 'busan', '釜山 Busan', [129.0756, 35.1796]),
  city('thailand', 'Thailand', 'bangkok', '曼谷 Bangkok', [100.5018, 13.7563]),
  city('thailand', 'Thailand', 'chiang-mai', '清迈 Chiang Mai', [98.9853, 18.7883]),
  city('singapore', 'Singapore', 'singapore', '新加坡 Singapore', [103.8198, 1.3521]),
  city('malaysia', 'Malaysia', 'kuala-lumpur', '吉隆坡 Kuala Lumpur', [101.6869, 3.139]),
  city('malaysia', 'Malaysia', 'penang', '槟城 Penang', [100.3288, 5.4141]),
  city('vietnam', 'Vietnam', 'hanoi', '河内 Hanoi', [105.8342, 21.0278]),
  city('vietnam', 'Vietnam', 'ho-chi-minh-city', '胡志明市 Ho Chi Minh City', [106.6297, 10.8231]),
  city('indonesia', 'Indonesia', 'jakarta', '雅加达 Jakarta', [106.8456, -6.2088]),
  city('indonesia', 'Indonesia', 'bali', '巴厘岛 Bali', [115.1889, -8.4095]),
  city('philippines', 'Philippines', 'manila', '马尼拉 Manila', [120.9842, 14.5995]),
  city('india', 'India', 'delhi', '德里 Delhi', [77.1025, 28.7041]),
  city('india', 'India', 'mumbai', '孟买 Mumbai', [72.8777, 19.076]),
  city('united-arab-emirates', 'United Arab Emirates', 'dubai', '迪拜 Dubai', [55.2708, 25.2048]),
  city('united-arab-emirates', 'United Arab Emirates', 'abu-dhabi', '阿布扎比 Abu Dhabi', [54.3773, 24.4539]),
  city('qatar', 'Qatar', 'doha', '多哈 Doha', [51.531, 25.2854]),
  city('turkey', 'Turkey', 'istanbul', '伊斯坦布尔 Istanbul', [28.9784, 41.0082]),
  city('egypt', 'Egypt', 'cairo', '开罗 Cairo', [31.2357, 30.0444]),
  city('france', 'France', 'paris', '巴黎 Paris', [2.3522, 48.8566]),
  city('france', 'France', 'nice', '尼斯 Nice', [7.2619, 43.7102]),
  city('united-kingdom', 'United Kingdom', 'london', '伦敦 London', [-0.1276, 51.5072]),
  city('united-kingdom', 'United Kingdom', 'edinburgh', '爱丁堡 Edinburgh', [-3.1883, 55.9533]),
  city('italy', 'Italy', 'rome', '罗马 Rome', [12.4964, 41.9028]),
  city('italy', 'Italy', 'milan', '米兰 Milan', [9.19, 45.4642]),
  city('italy', 'Italy', 'venice', '威尼斯 Venice', [12.3155, 45.4408]),
  city('spain', 'Spain', 'madrid', '马德里 Madrid', [-3.7038, 40.4168]),
  city('spain', 'Spain', 'barcelona', '巴塞罗那 Barcelona', [2.1734, 41.3851]),
  city('germany', 'Germany', 'berlin', '柏林 Berlin', [13.405, 52.52]),
  city('germany', 'Germany', 'munich', '慕尼黑 Munich', [11.582, 48.1351]),
  city('netherlands', 'Netherlands', 'amsterdam', '阿姆斯特丹 Amsterdam', [4.9041, 52.3676]),
  city('belgium', 'Belgium', 'brussels', '布鲁塞尔 Brussels', [4.3517, 50.8503]),
  city('switzerland', 'Switzerland', 'zurich', '苏黎世 Zurich', [8.5417, 47.3769]),
  city('austria', 'Austria', 'vienna', '维也纳 Vienna', [16.3738, 48.2082]),
  city('czech-republic', 'Czech Republic', 'prague', '布拉格 Prague', [14.4378, 50.0755]),
  city('greece', 'Greece', 'athens', '雅典 Athens', [23.7275, 37.9838]),
  city('russia', 'Russia', 'moscow', '莫斯科 Moscow', [37.6173, 55.7558]),
  city('russia', 'Russia', 'saint-petersburg', '圣彼得堡 Saint Petersburg', [30.3351, 59.9343]),
  city('united-states-of-america', 'United States of America', 'new-york', '纽约 New York', [-74.006, 40.7128]),
  city('united-states-of-america', 'United States of America', 'los-angeles', '洛杉矶 Los Angeles', [-118.2437, 34.0522]),
  city('united-states-of-america', 'United States of America', 'san-francisco', '旧金山 San Francisco', [-122.4194, 37.7749]),
  city('united-states-of-america', 'United States of America', 'chicago', '芝加哥 Chicago', [-87.6298, 41.8781]),
  city('united-states-of-america', 'United States of America', 'las-vegas', '拉斯维加斯 Las Vegas', [-115.1398, 36.1699]),
  city('canada', 'Canada', 'toronto', '多伦多 Toronto', [-79.3832, 43.6532]),
  city('canada', 'Canada', 'vancouver', '温哥华 Vancouver', [-123.1207, 49.2827]),
  city('canada', 'Canada', 'montreal', '蒙特利尔 Montreal', [-73.5673, 45.5017]),
  city('mexico', 'Mexico', 'mexico-city', '墨西哥城 Mexico City', [-99.1332, 19.4326]),
  city('brazil', 'Brazil', 'rio-de-janeiro', '里约热内卢 Rio de Janeiro', [-43.1729, -22.9068]),
  city('brazil', 'Brazil', 'sao-paulo', '圣保罗 Sao Paulo', [-46.6333, -23.5505]),
  city('argentina', 'Argentina', 'buenos-aires', '布宜诺斯艾利斯 Buenos Aires', [-58.3816, -34.6037]),
  city('peru', 'Peru', 'lima', '利马 Lima', [-77.0428, -12.0464]),
  city('chile', 'Chile', 'santiago', '圣地亚哥 Santiago', [-70.6693, -33.4489]),
  city('australia', 'Australia', 'sydney', '悉尼 Sydney', [151.2093, -33.8688]),
  city('australia', 'Australia', 'melbourne', '墨尔本 Melbourne', [144.9631, -37.8136]),
  city('australia', 'Australia', 'brisbane', '布里斯班 Brisbane', [153.0251, -27.4698]),
  city('new-zealand', 'New Zealand', 'auckland', '奥克兰 Auckland', [174.7633, -36.8485]),
  city('new-zealand', 'New Zealand', 'queenstown', '皇后镇 Queenstown', [168.6626, -45.0312]),
  city('south-africa', 'South Africa', 'cape-town', '开普敦 Cape Town', [18.4241, -33.9249]),
  city('south-africa', 'South Africa', 'johannesburg', '约翰内斯堡 Johannesburg', [28.0473, -26.2041]),
  city('kenya', 'Kenya', 'nairobi', '内罗毕 Nairobi', [36.8219, -1.2921]),
  city('morocco', 'Morocco', 'marrakesh', '马拉喀什 Marrakesh', [-7.9811, 31.6295]),
];

const coreWorldCityCodes = new Set(coreWorldCityRegions.map((cityItem) => cityItem.regionCode));
const expandedWorldCityRegions = generatedWorldCityData
  .map(([countrySlug, countryName, citySlug, name, longitude, latitude]) => (
    city(countrySlug, countryName, citySlug, name, [longitude, latitude])
  ))
  .filter((cityItem) => !coreWorldCityCodes.has(cityItem.regionCode));

export const worldCityRegions: WorldCityRegion[] = [
  ...coreWorldCityRegions,
  ...expandedWorldCityRegions,
];

export const worldCityMap: RegionMap = {
  mapCode: 'world-cities',
  name: '国外城市点位',
  level: 'world',
  source: 'lite-shape',
  minCreatableLevel: 'city',
  width: 1000,
  height: 620,
  regions: worldCityRegions,
};

export function getWorldCitiesForCountry(countryCode: string): WorldCityRegion[] {
  return worldCityRegions.filter((cityItem) => cityItem.countryCode === countryCode);
}

export function getWorldCityCountry(regionCode: string): WorldCityRegion | undefined {
  return worldCityRegions.find((cityItem) => cityItem.regionCode === regionCode);
}
