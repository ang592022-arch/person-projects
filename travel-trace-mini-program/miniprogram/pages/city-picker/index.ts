import { chinaProvinceMap, getCityMapForProvince } from '../../data/regions/index';
import { getAllRegionStates, setRegionLit } from '../../services/travel-store.service';
import { getActiveTheme } from '../../services/theme-store';
import { shadowClass, themeClass, themeToCssVars } from '../../utils/theme-style';

interface ProvinceItem {
  regionCode: string;
  name: string;
  initial: string;
  cityCount: number;
}

interface ProvinceGroup {
  initial: string;
  items: ProvinceItem[];
}

interface CityItem {
  regionCode: string;
  name: string;
  provinceCode: string;
  provinceName: string;
  visited: boolean;
}

interface AlphabetItem {
  letter: string;
  enabled: boolean;
}

const PROVINCE_INITIALS: Record<string, string> = {
  'cn-anhui': 'A',
  'cn-aomen': 'A',
  'cn-beijing': 'B',
  'cn-chongqing': 'C',
  'cn-fujian': 'F',
  'cn-gansu': 'G',
  'cn-guangdong': 'G',
  'cn-guangxi': 'G',
  'cn-guizhou': 'G',
  'cn-hainan': 'H',
  'cn-hebei': 'H',
  'cn-heilongjiang': 'H',
  'cn-henan': 'H',
  'cn-hubei': 'H',
  'cn-hunan': 'H',
  'cn-jiangsu': 'J',
  'cn-jiangxi': 'J',
  'cn-jilin': 'J',
  'cn-liaoning': 'L',
  'cn-neimenggu': 'N',
  'cn-ningxia': 'N',
  'cn-qinghai': 'Q',
  'cn-shaanxi': 'S',
  'cn-shandong': 'S',
  'cn-shanghai': 'S',
  'cn-shanxi': 'S',
  'cn-sichuan': 'S',
  'cn-taiwan': 'T',
  'cn-tianjin': 'T',
  'cn-xianggang': 'X',
  'cn-xinjiang': 'X',
  'cn-xizang': 'X',
  'cn-yunnan': 'Y',
  'cn-zhejiang': 'Z',
};

const ALPHABET = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ'.split('');

function buildProvinceGroups(): ProvinceGroup[] {
  const groups = new Map<string, ProvinceItem[]>();

  chinaProvinceMap.regions.forEach((province) => {
    const initial = PROVINCE_INITIALS[province.regionCode] || '#';
    const cityCount = getCityMapForProvince(province.regionCode)?.regions.length || 0;
    const item: ProvinceItem = {
      regionCode: province.regionCode,
      name: province.name,
      initial,
      cityCount,
    };
    groups.set(initial, [...(groups.get(initial) || []), item]);
  });

  return Array.from(groups.entries())
    .sort(([a], [b]) => a.localeCompare(b))
    .map(([initial, items]) => ({ initial, items }));
}

const PROVINCE_GROUPS = buildProvinceGroups();
const AVAILABLE_INITIALS = new Set(PROVINCE_GROUPS.map((group) => group.initial));
const ALPHABET_ITEMS: AlphabetItem[] = ALPHABET.map((letter) => ({
  letter,
  enabled: AVAILABLE_INITIALS.has(letter),
}));

function cityItemsForProvince(provinceCode: string, visitedCodes: string[]): CityItem[] {
  const province = chinaProvinceMap.regions.find((item) => item.regionCode === provinceCode);
  const cityMap = getCityMapForProvince(provinceCode);
  const visited = new Set(visitedCodes);

  if (!province || !cityMap) return [];
  return cityMap.regions.map((city) => ({
    regionCode: city.regionCode,
    name: city.name,
    provinceCode,
    provinceName: province.name,
    visited: visited.has(city.regionCode),
  }));
}

function allCityItems(visitedCodes: string[]): CityItem[] {
  return chinaProvinceMap.regions.flatMap((province) => (
    cityItemsForProvince(province.regionCode, visitedCodes)
  ));
}

function searchCityItems(query: string, visitedCodes: string[]): CityItem[] {
  const keyword = query.trim();
  if (!keyword) return [];
  const upperKeyword = keyword.toUpperCase();

  return allCityItems(visitedCodes)
    .filter((city) => {
      const initial = PROVINCE_INITIALS[city.provinceCode] || '';
      return city.name.includes(keyword)
        || city.provinceName.includes(keyword)
        || (keyword.length === 1 && initial === upperKeyword);
    })
    .slice(0, 80);
}

function themeViewData() {
  const theme = getActiveTheme();
  return {
    themeStyle: themeToCssVars(theme),
    themeClass: themeClass(theme),
    shadowClass: shadowClass(theme),
  };
}

Page({
  data: {
    ...themeViewData(),
    query: '',
    provinceGroups: PROVINCE_GROUPS,
    alphabet: ALPHABET_ITEMS,
    scrollTarget: '',
    activeProvinceCode: '',
    activeProvinceName: '',
    cities: [] as CityItem[],
    searchResults: [] as CityItem[],
    visitedCodes: [] as string[],
    visitedCount: 0,
    savingRegionCode: '',
  },

  async onShow() {
    this.setData(themeViewData());
    try {
      const stateMap = await getAllRegionStates();
      const visitedCodes = Object.values(stateMap)
        .filter((state) => state.lit)
        .map((state) => state.regionCode);
      this.refreshLists(visitedCodes);
    } catch {
      wx.showToast({ title: '城市状态加载失败，请重试', icon: 'none' });
    }
  },

  refreshLists(visitedCodes: string[]) {
    const activeProvinceCode = this.data.activeProvinceCode;
    this.setData({
      visitedCodes,
      visitedCount: allCityItems(visitedCodes).filter((city) => city.visited).length,
      cities: activeProvinceCode ? cityItemsForProvince(activeProvinceCode, visitedCodes) : [],
      searchResults: searchCityItems(this.data.query, visitedCodes),
    });
  },

  handleSearchInput(event: { detail: { value: string } }) {
    const query = event.detail.value || '';
    this.setData({
      query,
      searchResults: searchCityItems(query, this.data.visitedCodes),
    });
  },

  clearSearch() {
    this.setData({ query: '', searchResults: [] });
  },

  jumpToInitial(event: { currentTarget: { dataset: { letter?: string; enabled?: boolean } } }) {
    const { letter, enabled } = event.currentTarget.dataset;
    if (!letter || !enabled) return;
    const scrollTarget = `province-${letter}`;
    if (this.data.scrollTarget !== scrollTarget) {
      this.setData({ scrollTarget });
      return;
    }
    this.setData({ scrollTarget: '' }, () => {
      wx.nextTick(() => this.setData({ scrollTarget }));
    });
  },

  selectProvince(event: { currentTarget: { dataset: { regionCode?: string; name?: string } } }) {
    const { regionCode, name } = event.currentTarget.dataset;
    if (!regionCode || !name) return;
    this.setData({
      activeProvinceCode: regionCode,
      activeProvinceName: name,
      cities: cityItemsForProvince(regionCode, this.data.visitedCodes),
      query: '',
      searchResults: [],
    });
  },

  chooseAnotherProvince() {
    this.setData({
      activeProvinceCode: '',
      activeProvinceName: '',
      cities: [],
    });
  },

  async addCity(event: { currentTarget: { dataset: { regionCode?: string; name?: string } } }) {
    const { regionCode, name } = event.currentTarget.dataset;
    if (!regionCode || !name || this.data.savingRegionCode) return;
    if (this.data.visitedCodes.includes(regionCode)) {
      wx.showToast({ title: `${name}已经点亮`, icon: 'none' });
      return;
    }

    this.setData({ savingRegionCode: regionCode });
    try {
      await setRegionLit(regionCode, true);
      const visitedCodes = [...this.data.visitedCodes, regionCode];
      this.setData({ savingRegionCode: '' });
      this.refreshLists(visitedCodes);
      wx.showToast({ title: `已点亮${name}`, icon: 'success' });
    } catch {
      this.setData({ savingRegionCode: '' });
      wx.showToast({ title: '添加失败，请重试', icon: 'none' });
    }
  },

  finish() {
    wx.navigateBack({
      delta: 1,
      fail: () => wx.redirectTo({ url: '/pages/home/index' }),
    });
  },
});
