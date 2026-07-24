import { allCityMaps, worldMap } from '../data/regions/index';
import { countryAliases, travelPlaceAliases, type CountryAlias, type TravelPlaceAlias } from '../data/regions/travel-place-aliases';
import { worldCityRegions, type WorldCityRegion } from '../data/regions/world-cities';
import { getCustomWorldCityRegions } from './custom-world-city-store';
import type { Region } from '../types/region';
import type { TravelTicket, TravelTicketType } from '../types/travel-record';

export interface ParsedTravelPlace {
  id: string;
  cityName: string;
  countryCode?: string;
  countryName?: string;
  regionCode?: string;
  regionName?: string;
  lonLat?: [number, number];
  sourceText: string;
  confidence: number;
  isKnownRegion: boolean;
  isCustomCandidate: boolean;
  needsReview?: boolean;
}

export interface ParsedTravelImport {
  regionCode?: string;
  regionName?: string;
  countryCode?: string;
  countryName?: string;
  cityName?: string;
  visitDate: string;
  title: string;
  content: string;
  tags: string[];
  tickets: TravelTicket[];
  detectedPlaces: ParsedTravelPlace[];
}

function today(): string {
  return new Date().toISOString().slice(0, 10);
}

function normalize(value: string): string {
  return value
    .toLowerCase()
    .replace(/[（）()【】\[\]{}《》<>「」『』"'“”‘’]/g, '')
    .replace(/[·・.。,:：;；、，\s_\-—–/\\|]+/g, '')
    .replace(/市|省|县|区|特别行政区|国家|地区|景区|机场|火车站|高铁站|国际机场/g, '');
}

function lineTokens(line: string): string[] {
  return line
    .split(/[,\s，、/|｜·・\-—–:：>＞]+/)
    .map((token) => token.trim())
    .filter(Boolean);
}

function allCities(): Region[] {
  return [
    ...allCityMaps.flatMap((map) => map.regions),
    ...worldCityRegions,
    ...getCustomWorldCityRegions(),
  ];
}

function cityNameParts(name: string): string[] {
  return name
    .split(/[ /\-·・]+/)
    .map((part) => part.trim())
    .filter((part) => part.length >= 2);
}

function placeKey(place: ParsedTravelPlace): string {
  return place.regionCode || `${place.countryCode || 'unknown'}:${normalize(place.cityName)}`;
}

function cityMatchKeys(name: string): Set<string> {
  return new Set(
    [name, ...cityNameParts(name)]
      .map(normalize)
      .filter((part) => part.length >= 2),
  );
}

function placesReferToSameCity(left: ParsedTravelPlace, right: ParsedTravelPlace): boolean {
  if (placeKey(left) === placeKey(right)) return true;
  if (left.countryCode && right.countryCode && left.countryCode !== right.countryCode) return false;

  const leftKeys = cityMatchKeys(left.cityName);
  return Array.from(cityMatchKeys(right.cityName)).some((key) => leftKeys.has(key));
}

function createPlaceId(place: Pick<ParsedTravelPlace, 'cityName' | 'countryCode' | 'regionCode'>): string {
  return encodeURIComponent(place.regionCode || `${place.countryCode || 'unknown'}:${place.cityName}`);
}

function toPlaceFromAlias(alias: TravelPlaceAlias, sourceText: string, confidence = 0.94): ParsedTravelPlace {
  return {
    id: createPlaceId({ cityName: alias.cityName, countryCode: alias.countryCode, regionCode: alias.regionCode }),
    cityName: alias.cityName,
    countryCode: alias.countryCode,
    countryName: alias.countryName,
    regionCode: alias.regionCode,
    regionName: alias.regionName || alias.cityName,
    lonLat: alias.lonLat,
    sourceText,
    confidence,
    isKnownRegion: Boolean(alias.regionCode),
    isCustomCandidate: !alias.regionCode,
  };
}

function toPlaceFromWorldCity(city: Region, sourceText: string): ParsedTravelPlace {
  const worldCity = city as WorldCityRegion;
  const countryCode = worldCity.countryCode || (city.parentCode?.startsWith('country:') ? city.parentCode : undefined);
  const countryName = worldCity.countryName || worldMap.regions.find((country) => country.regionCode === countryCode)?.name;

  return {
    id: createPlaceId({ cityName: city.name, countryCode, regionCode: city.regionCode }),
    cityName: city.name,
    countryCode,
    countryName,
    regionCode: city.regionCode,
    regionName: city.name,
    lonLat: worldCity.lonLat,
    sourceText,
    confidence: 0.86,
    isKnownRegion: true,
    isCustomCandidate: false,
  };
}

function dedupePlaces(places: ParsedTravelPlace[]): ParsedTravelPlace[] {
  const deduped: ParsedTravelPlace[] = [];

  places.forEach((place) => {
    const index = deduped.findIndex((existing) => placesReferToSameCity(existing, place));
    if (index < 0) {
      deduped.push(place);
      return;
    }

    const existing = deduped[index];
    const preferred = (!existing.regionCode && place.regionCode) || place.confidence > existing.confidence
      ? place
      : existing;
    const alternate = preferred === place ? existing : place;
    const regionCode = preferred.regionCode || alternate.regionCode;
    deduped[index] = {
      ...alternate,
      ...preferred,
      regionCode,
      regionName: preferred.regionName || alternate.regionName,
      countryCode: preferred.countryCode || alternate.countryCode,
      countryName: preferred.countryName || alternate.countryName,
      lonLat: preferred.lonLat || alternate.lonLat,
      confidence: Math.max(existing.confidence, place.confidence),
      isKnownRegion: Boolean(regionCode || existing.isKnownRegion || place.isKnownRegion),
      isCustomCandidate: !regionCode,
      needsReview: regionCode ? false : Boolean(existing.needsReview || place.needsReview),
    };
  });

  return deduped.sort((a, b) => b.confidence - a.confidence);
}

function findCountryByAlias(source: string): CountryAlias | undefined {
  const normalizedSource = normalize(source);
  const fromConfigured = countryAliases.find((country) =>
    country.aliases.some((alias) => {
      const normalizedAlias = normalize(alias);
      return normalizedAlias.length >= 2 && normalizedSource.includes(normalizedAlias);
    }),
  );
  if (fromConfigured) return fromConfigured;

  const fromWorld = worldMap.regions.find((country) => {
    const normalizedName = normalize(country.name);
    return normalizedName.length >= 3 && normalizedSource.includes(normalizedName);
  });
  return fromWorld
    ? {
        countryCode: fromWorld.regionCode,
        countryName: fromWorld.name,
        aliases: [fromWorld.name],
      }
    : undefined;
}

function detectAliasPlaces(source: string): ParsedTravelPlace[] {
  const normalizedSource = normalize(source);

  return travelPlaceAliases
    .map((alias) => {
      const matchedAlias = alias.aliases.find((item) => {
        const normalizedAlias = normalize(item);
        return normalizedAlias.length >= 2 && normalizedSource.includes(normalizedAlias);
      });
      return matchedAlias ? toPlaceFromAlias(alias, matchedAlias) : undefined;
    })
    .filter(Boolean) as ParsedTravelPlace[];
}

function detectKnownRegionPlaces(source: string): ParsedTravelPlace[] {
  const normalizedSource = normalize(source);

  return allCities()
    .map((city) => {
      const parts = [city.name, ...cityNameParts(city.name)]
        .map(normalize)
        .filter((part) => part.length >= 2);
      const matched = parts.find((part) => normalizedSource.includes(part));
      return matched ? toPlaceFromWorldCity(city, city.name) : undefined;
    })
    .filter(Boolean) as ParsedTravelPlace[];
}

function isUsefulCityCandidate(token: string): boolean {
  const trimmed = token.trim();
  if (trimmed.length < 2 || trimmed.length > 28) return false;
  if (/^\d+$/.test(trimmed)) return false;
  if (/[票价航班车次座位订单手机号身份证入住退订支付里程足迹国家地区世界全部添加识别导入截图携程高德飞猪去哪儿]/.test(trimmed)) return false;
  if (/我|已|去过|到过|总|累计|全部|最近|最远|会员|达人|公里|个/.test(trimmed)) return false;
  return /^[A-Za-z\u4e00-\u9fa5.'’\- ]+$/.test(trimmed);
}

function detectCountryContextPlaces(lines: string[]): ParsedTravelPlace[] {
  const places: ParsedTravelPlace[] = [];

  lines.forEach((line) => {
    const tokens = lineTokens(line);
    if (tokens.length < 2) return;

    tokens.forEach((token, index) => {
      const configuredCountry = countryAliases.find((item) =>
        item.aliases.some((alias) => normalize(alias) === normalize(token)),
      );
      const worldCountry = worldMap.regions.find((item) => normalize(item.name) === normalize(token));
      const country = configuredCountry || (worldCountry ? {
        countryCode: worldCountry.regionCode,
        countryName: worldCountry.name,
        aliases: [worldCountry.name],
      } : undefined);
      if (!country || country.countryCode === 'country:cn') return;

      const neighbors = [tokens[index - 1], tokens[index + 1]].filter(Boolean) as string[];
      neighbors.forEach((candidate) => {
        if (!isUsefulCityCandidate(candidate)) return;
        const normalizedCandidate = normalize(candidate);
        const isCountryName = countryAliases.some((item) =>
          item.aliases.some((alias) => normalize(alias) === normalizedCandidate),
        );
        if (isCountryName) return;

        places.push({
          id: createPlaceId({ cityName: candidate, countryCode: country.countryCode }),
          cityName: candidate,
          countryCode: country.countryCode,
          countryName: country.countryName,
          sourceText: line,
          confidence: 0.68,
          isKnownRegion: false,
          isCustomCandidate: true,
          needsReview: true,
        });
      });
    });
  });

  return places;
}

function parseDestinationCity(source: string): string {
  const destination = source.match(/(?:目的地|到达|抵达|前往|城市|景点)[:：\s]*([A-Za-z\u4e00-\u9fa5\s.'’\-]{2,28})/);
  if (destination?.[1]) return destination[1].trim();

  const arrowTrip = source.match(/([A-Za-z\u4e00-\u9fa5]{2,18})\s*(?:-|—|–|→|至|到)\s*([A-Za-z\u4e00-\u9fa5]{2,18})/);
  return arrowTrip?.[2]?.trim() || '';
}

function parseDate(source: string): string {
  const fullDate = source.match(/(20\d{2})[年\-.\/](\d{1,2})[月\-.\/](\d{1,2})/);
  if (fullDate) {
    const [, year, month, day] = fullDate;
    return `${year}-${month.padStart(2, '0')}-${day.padStart(2, '0')}`;
  }

  const yearMonth = source.match(/(20\d{2})[年\-.\/](\d{1,2})月?/);
  if (yearMonth) {
    const [, year, month] = yearMonth;
    return `${year}-${month.padStart(2, '0')}-01`;
  }

  return today();
}

function detectTicketType(source: string): TravelTicketType | undefined {
  if (/航班|机票|登机|机场|boarding|flight/i.test(source)) return 'flight';
  if (/高铁|动车|G\d{1,5}|D\d{1,5}|C\d{1,5}/i.test(source)) return 'highSpeedRail';
  if (/火车|列车|车票|铁路|Z\d{1,5}|T\d{1,5}|K\d{1,5}/i.test(source)) return 'train';
  return undefined;
}

function parseTicket(source: string): TravelTicket | undefined {
  const ticketType = detectTicketType(source);
  if (!ticketType) return undefined;

  const route = source.match(/([A-Za-z\u4e00-\u9fa5]{2,18})\s*(?:-|—|–|→|至|到)\s*([A-Za-z\u4e00-\u9fa5]{2,18})/);
  const ticketNo = source.match(/\b([A-Z]{2}\d{3,4}|[GDCZT]\d{1,5})\b/i)?.[1]?.toUpperCase();
  const carrier = source.match(/(?:航空|航司|承运|车站|铁路)[:：\s]*([A-Za-z\u4e00-\u9fa5]{2,20})/)?.[1];
  const seat = source.match(/(?:座位|座号|席位|车厢)[:：\s]*([A-Za-z0-9\u4e00-\u9fa5\-]{1,20})/)?.[1];

  return {
    id: `ticket-${Date.now()}`,
    type: ticketType,
    title: ticketType === 'flight' ? '截图导入机票' : ticketType === 'highSpeedRail' ? '截图导入高铁票' : '截图导入火车票',
    departure: route?.[1] || '',
    arrival: route?.[2] || '',
    carrier,
    ticketNo,
    seat,
    note: '由截图文字解析生成，请核对后保存。',
  };
}

export async function recognizeTravelTextFromImage(filePath: string): Promise<string> {
  if (!wx.cloud) throw new Error('OCR_CLOUD_UNAVAILABLE');

  let uploadedFileID = '';
  try {
    const prepared = await wx.cloud.callFunction({
      name: 'ocrImport',
      data: { action: 'prepare' },
    });
    const uploadPrefix = String((prepared.result as any)?.uploadPrefix || '');
    if (!/^ocr-import\/[A-Za-z0-9_-]+\/$/.test(uploadPrefix)) {
      throw new Error('OCR_UPLOAD_PREFIX_INVALID');
    }
    const sourceName = (filePath.split('/').pop() || 'source.jpg')
      .replace(/[^a-zA-Z0-9._-]/g, '_')
      .slice(-80);
    const uploaded = await wx.cloud.uploadFile({
      cloudPath: `${uploadPrefix}${Date.now()}-${Math.random().toString(36).slice(2)}-${sourceName}`,
      filePath,
    });
    uploadedFileID = uploaded.fileID;
    const result = await wx.cloud.callFunction({
      name: 'ocrImport',
      data: {
        action: 'recognize',
        fileID: uploaded.fileID,
      },
    });
    const data = result.result as any;
    if (data?.error) {
      const error = new Error(String(data.error));
      (error as any).code = String(data.error);
      throw error;
    }
    return String(data?.text || data?.fullText || '');
  } finally {
    if (uploadedFileID) {
      await wx.cloud.deleteFile({ fileList: [uploadedFileID] }).catch(() => undefined);
    }
  }
}

export function parseTravelImportText(text: string): ParsedTravelImport {
  const cleaned = text.replace(/\r/g, '\n').trim();
  const lines = cleaned
    .split(/\n+/)
    .map((line) => line.trim())
    .filter(Boolean);
  const places = dedupePlaces([
    ...detectAliasPlaces(cleaned),
    ...detectKnownRegionPlaces(cleaned),
    ...detectCountryContextPlaces(lines),
  ]);
  const country = places[0]?.countryCode ? undefined : findCountryByAlias(cleaned);
  const ticket = parseTicket(cleaned);
  const visitDate = parseDate(cleaned);
  const titleSource = lines.find((line) => line.length >= 4 && line.length <= 28 && !/订单|票号|身份证|手机号/.test(line));
  const primary = places[0];
  const title = titleSource || ticket?.title || (primary ? `${primary.cityName}旅行足迹` : '截图导入的旅行记录');
  const tags = Array.from(new Set([
    /携程|Trip|Ctrip/i.test(cleaned) ? '携程足迹' : '',
    /高德|Amap/i.test(cleaned) ? '高德足迹' : '',
    ticket ? ticket.type === 'flight' ? '机票' : ticket.type === 'highSpeedRail' ? '高铁票' : '火车票' : '',
    /景点|门票|游玩|攻略|行程/.test(cleaned) ? '景点' : '',
    /酒店|民宿|入住/.test(cleaned) ? '住宿' : '',
    places.length > 1 ? '批量导入' : '',
  ].filter(Boolean)));

  return {
    regionCode: primary?.regionCode,
    regionName: primary?.regionName,
    countryCode: primary?.countryCode || country?.countryCode,
    countryName: primary?.countryName || country?.countryName,
    cityName: primary?.cityName || parseDestinationCity(cleaned),
    visitDate,
    title,
    content: cleaned || '由截图导入生成。',
    tags,
    tickets: ticket ? [ticket] : [],
    detectedPlaces: places,
  };
}
