import { chinaProvinceMap } from './china-provinces';
import type { Region, RegionMap, UserRegionState } from '../../types/region';

const cityNamesByProvince: Record<string, string[]> = {
  'cn-xinjiang': ['乌鲁木齐', '克拉玛依', '吐鲁番', '哈密', '昌吉', '博尔塔拉', '巴音郭楞', '阿克苏', '克孜勒苏', '喀什', '和田', '伊犁', '塔城', '阿勒泰', '石河子', '阿拉尔', '图木舒克', '五家渠', '北屯', '铁门关', '双河', '可克达拉', '昆玉', '胡杨河'],
  'cn-xizang': ['拉萨', '日喀则', '昌都', '林芝', '山南', '那曲', '阿里'],
  'cn-qinghai': ['西宁', '海东', '海北', '黄南', '海南', '果洛', '玉树', '海西'],
  'cn-gansu': ['兰州', '嘉峪关', '金昌', '白银', '天水', '武威', '张掖', '平凉', '酒泉', '庆阳', '定西', '陇南', '临夏', '甘南'],
  'cn-neimenggu': ['呼和浩特', '包头', '乌海', '赤峰', '通辽', '鄂尔多斯', '呼伦贝尔', '巴彦淖尔', '乌兰察布', '兴安盟', '锡林郭勒', '阿拉善'],
  'cn-ningxia': ['银川', '石嘴山', '吴忠', '固原', '中卫'],
  'cn-shaanxi': ['西安', '铜川', '宝鸡', '咸阳', '渭南', '延安', '汉中', '榆林', '安康', '商洛'],
  'cn-shanxi': ['太原', '大同', '阳泉', '长治', '晋城', '朔州', '晋中', '运城', '忻州', '临汾', '吕梁'],
  'cn-hebei': ['石家庄', '唐山', '秦皇岛', '邯郸', '邢台', '保定', '张家口', '承德', '沧州', '廊坊', '衡水'],
  'cn-beijing': ['北京'],
  'cn-tianjin': ['天津'],
  'cn-liaoning': ['沈阳', '大连', '鞍山', '抚顺', '本溪', '丹东', '锦州', '营口', '阜新', '辽阳', '盘锦', '铁岭', '朝阳', '葫芦岛'],
  'cn-jilin': ['长春', '吉林', '四平', '辽源', '通化', '白山', '松原', '白城', '延边'],
  'cn-heilongjiang': ['哈尔滨', '齐齐哈尔', '鸡西', '鹤岗', '双鸭山', '大庆', '伊春', '佳木斯', '七台河', '牡丹江', '黑河', '绥化', '大兴安岭'],
  'cn-henan': ['郑州', '开封', '洛阳', '平顶山', '安阳', '鹤壁', '新乡', '焦作', '濮阳', '许昌', '漯河', '三门峡', '南阳', '商丘', '信阳', '周口', '驻马店', '济源'],
  'cn-shandong': ['济南', '青岛', '淄博', '枣庄', '东营', '烟台', '潍坊', '济宁', '泰安', '威海', '日照', '临沂', '德州', '聊城', '滨州', '菏泽'],
  'cn-sichuan': ['成都', '自贡', '攀枝花', '泸州', '德阳', '绵阳', '广元', '遂宁', '内江', '乐山', '南充', '眉山', '宜宾', '广安', '达州', '雅安', '巴中', '资阳', '阿坝', '甘孜', '凉山'],
  'cn-chongqing': ['重庆'],
  'cn-hubei': ['武汉', '黄石', '十堰', '宜昌', '襄阳', '鄂州', '荆门', '孝感', '荆州', '黄冈', '咸宁', '随州', '恩施', '仙桃', '潜江', '天门', '神农架'],
  'cn-anhui': ['合肥', '芜湖', '蚌埠', '淮南', '马鞍山', '淮北', '铜陵', '安庆', '黄山', '滁州', '阜阳', '宿州', '六安', '亳州', '池州', '宣城'],
  'cn-jiangsu': ['南京', '无锡', '徐州', '常州', '苏州', '南通', '连云港', '淮安', '盐城', '扬州', '镇江', '泰州', '宿迁'],
  'cn-shanghai': ['上海'],
  'cn-zhejiang': ['杭州', '宁波', '温州', '嘉兴', '湖州', '绍兴', '金华', '衢州', '舟山', '台州', '丽水'],
  'cn-jiangxi': ['南昌', '景德镇', '萍乡', '九江', '新余', '鹰潭', '赣州', '吉安', '宜春', '抚州', '上饶'],
  'cn-hunan': ['长沙', '株洲', '湘潭', '衡阳', '邵阳', '岳阳', '常德', '张家界', '益阳', '郴州', '永州', '怀化', '娄底', '湘西'],
  'cn-guizhou': ['贵阳', '六盘水', '遵义', '安顺', '毕节', '铜仁', '黔西南', '黔东南', '黔南'],
  'cn-yunnan': ['昆明', '曲靖', '玉溪', '保山', '昭通', '丽江', '普洱', '临沧', '楚雄', '红河', '文山', '西双版纳', '大理', '德宏', '怒江', '迪庆'],
  'cn-guangxi': ['南宁', '柳州', '桂林', '梧州', '北海', '防城港', '钦州', '贵港', '玉林', '百色', '贺州', '河池', '来宾', '崇左'],
  'cn-guangdong': ['广州', '韶关', '深圳', '珠海', '汕头', '佛山', '江门', '湛江', '茂名', '肇庆', '惠州', '梅州', '汕尾', '河源', '阳江', '清远', '东莞', '中山', '潮州', '揭阳', '云浮'],
  'cn-fujian': ['福州', '厦门', '莆田', '三明', '泉州', '漳州', '南平', '龙岩', '宁德'],
  'cn-hainan': ['海口', '三亚', '三沙', '儋州', '五指山', '琼海', '文昌', '万宁', '东方', '定安', '屯昌', '澄迈', '临高', '白沙', '昌江', '乐东', '陵水', '保亭', '琼中'],
  'cn-taiwan': ['台北', '新北', '桃园', '台中', '台南', '高雄', '基隆', '新竹', '嘉义', '新竹县', '苗栗县', '彰化县', '南投县', '云林县', '嘉义县', '屏东县', '宜兰县', '花莲县', '台东县', '澎湖县', '金门县', '连江县'],
  'cn-xianggang': ['香港'],
  'cn-aomen': ['澳门'],
};

function createCityRegions(provinceCode: string, names: string[]): Region[] {
  const columns = names.length <= 6 ? 2 : names.length <= 12 ? 3 : 4;
  const gap = 18;
  const left = 64;
  const top = 64;
  const width = (1000 - left * 2 - gap * (columns - 1)) / columns;
  const rows = Math.ceil(names.length / columns);
  const height = Math.min(104, (620 - top * 2 - gap * (rows - 1)) / rows);

  return names.map((name, index) => {
    const column = index % columns;
    const row = Math.floor(index / columns);
    const x = left + column * (width + gap);
    const y = top + row * (height + gap);

    return {
      regionCode: `${provinceCode}-city-${String(index + 1).padStart(2, '0')}`,
      name,
      level: 'city',
      parentCode: provinceCode,
      center: [x + width / 2, y + height / 2],
      shape: {
        type: 'rect',
        x,
        y,
        width,
        height,
      },
    };
  });
}

export const provinceCityMaps: Record<string, RegionMap> = chinaProvinceMap.regions.reduce(
  (maps, province) => {
    const names = cityNamesByProvince[province.regionCode] || [province.name];
    const mapCode = `${province.regionCode}:cities`;

    maps[province.regionCode] = {
      mapCode,
      name: `${province.name}城市地图`,
      level: 'province',
      parentCode: province.regionCode,
      source: 'geojson',
      minCreatableLevel: 'city',
      width: 1000,
      height: 620,
      regions: createCityRegions(province.regionCode, names),
    };

    return maps;
  },
  {} as Record<string, RegionMap>,
);

export const allCityMaps = Object.values(provinceCityMaps);

export function getCityMapForProvince(provinceCode: string): RegionMap | undefined {
  return provinceCityMaps[provinceCode];
}

export function getProvinceForCity(regionCode: string): Region | undefined {
  const cityMap = allCityMaps.find((map) => map.regions.some((region) => region.regionCode === regionCode));
  if (!cityMap?.parentCode) return undefined;
  return chinaProvinceMap.regions.find((province) => province.regionCode === cityMap.parentCode);
}

export function getProvinceHighlightStates(
  states: Record<string, UserRegionState>,
): Record<string, UserRegionState> {
  const provinceStates: Record<string, UserRegionState> = {};

  chinaProvinceMap.regions.forEach((province) => {
    const cityMap = provinceCityMaps[province.regionCode];
    const litCities = cityMap.regions.filter((city) => states[city.regionCode]?.lit);
    const lit = litCities.length > 0;

    if (lit) {
      const firstVisitDates = litCities
        .map((city) => states[city.regionCode]?.firstVisitDate)
        .filter(Boolean)
        .sort();
      const lastVisitDates = litCities
        .map((city) => states[city.regionCode]?.lastVisitDate)
        .filter(Boolean)
        .sort()
        .reverse();

      provinceStates[province.regionCode] = {
        regionCode: province.regionCode,
        lit,
        firstVisitDate: firstVisitDates[0],
        lastVisitDate: lastVisitDates[0],
        visitCount: litCities.reduce((sum, city) => sum + (states[city.regionCode]?.visitCount || 0), 0),
        tags: Array.from(new Set(litCities.flatMap((city) => states[city.regionCode]?.tags || []))),
        coverImage: litCities.map((city) => states[city.regionCode]?.coverImage).find(Boolean),
        updatedAt: new Date().toISOString(),
      };
    }
  });

  return provinceStates;
}
