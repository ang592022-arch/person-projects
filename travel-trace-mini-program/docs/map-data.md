# 地图数据接入

## 为什么仓库没有中国省市边界

私人原型曾使用一份缺少完整来源记录的边界数据。公开源码意味着他人可以下载和再分发，因此该数据没有进入本仓库。

公开模板保留了以下稳定接口：

- `miniprogram/data/geojson/china-provinces.ts`：省级 `FeatureCollection`；
- `miniprogram/packages/map/data/geojson/province-cities.ts`：按省加载市级 `FeatureCollection`；
- `miniprogram/services/amap-boundary.service.ts`：可选的高德行政区查询适配器；
- `miniprogram/utils/geojson-boundary.ts`：GeoJSON 投影和页面视图转换。

页面只依赖这些接口，不需要为不同数据提供方复制页面代码。

## 推荐方式：运行时请求

1. 申请自己的地图服务 Key；
2. 复制 `miniprogram/config/env.example.ts` 为被忽略的 `env.ts`；
3. 在 `amapConfig.key` 中填写私人 Key；
4. 在微信公众平台配置服务商要求的合法请求域名；
5. 核对服务条款、调用配额、缓存限制、署名和地图审核要求。

当本地 `FeatureCollection` 为空且配置了 Key 时，中国地图页和城市地图页会尝试通过边界服务加载数据。

## 接入静态 GeoJSON

静态数据必须至少记录：

- 数据提供方和原始链接；
- 数据版本与更新时间；
- 明确的再分发许可证；
- 是否要求署名或相同方式共享；
- 发布地适用的地图审核和审图号要求；
- 原始文件及生成文件的 SHA-256。

GeoJSON 结构应为标准 `FeatureCollection`，每个要素至少包含 `properties.name` 和 Polygon 或 MultiPolygon 几何。

未经确认的数据不要提交到公共仓库。

## 已附带的世界数据

世界国家边界和国外城市点位来自 Natural Earth，属于公共领域。来源与条款记录在根目录的 `THIRD_PARTY_NOTICES.md`。
