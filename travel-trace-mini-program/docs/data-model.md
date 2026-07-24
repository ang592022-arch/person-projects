# 数据模型设计

这个项目把“地区目录”和“用户旅行数据”分开保存。当前版本的创作最小单位是城市，省份只负责进入城市地图和汇总点亮状态。

地区目录是公共数据，例如中国、省份、市县的名称、层级、父级编码和地图边界。用户旅行数据是私有数据，例如某个用户是否点亮某个省份、写了哪些日志、上传了哪些照片。

这样做的好处是：

- 同一个地区可以被很多用户使用，但每个用户的点亮状态互不影响。
- 以后增加市级、县级数据时，只需要扩展地区目录。
- 云数据库查询可以始终按 `_openid` 隔离用户数据。

## RegionCatalog

第一阶段先放在 `miniprogram/data/regions/`，未来数据量变大后可以迁移到分包、云存储或云数据库。

```ts
interface Region {
  regionCode: string
  name: string
  level: 'world' | 'continent' | 'country' | 'province' | 'city' | 'county'
  parentCode?: string
  center: [number, number]
  geoJsonPath?: string
}
```

当前有三类地图：

- `worldMap`：世界入口地图。
- `chinaProvinceMap`：中国省级地图。
- `provinceCityMaps`：每个省一个城市目录，边界由对应省份的市级 GeoJSON 提供。

## UserRegionState

用于快速渲染地图高亮。它是用户私有数据。

```ts
interface UserRegionState {
  _openid: string
  regionCode: string
  lit: boolean
  firstVisitDate?: string
  lastVisitDate?: string
  visitCount: number
  tags: string[]
  coverImage?: string
  updatedAt: Date
}
```

`visitCount` 可以由 `TravelRecord` 计算得到，但地图页需要频繁读取高亮和统计，所以保留一个缓存字段。每次新增、编辑、删除日志后刷新它。

中国地图上的省份高亮由城市状态汇总得到：只要某个省下面有一个城市被点亮，这个省就在中国地图上高亮。

从旧版本升级到城市级模型时，历史省份级本地数据会自动迁移到该省城市列表的第一个城市。这样可以避免旧日志丢失，但具体城市可能需要用户之后手动调整。

## TravelRecord

一个地区可以有多条旅行日志。

```ts
interface TravelRecord {
  _openid: string
  regionCode: string
  visitDate: string
  title?: string
  content: string
  tags: string[]
  images: TravelImage[]
  tickets: TravelTicket[]
  createdAt: Date
  updatedAt: Date
}
```

当前版本要求 `TravelRecord.regionCode` 指向城市级 `Region`。如果以后扩展到县级，可以继续让日志指向县级，并由县级向城市、省份汇总。

## TravelImage / TravelMedia

```ts
interface TravelImage {
  fileID: string
  cloudPath?: string
  tempPath?: string
  localPath?: string
  mediaType?: 'image' | 'video'
  size?: number
  duration?: number
  thumbFileID?: string
  uploadedAt: Date
}
```

当前第二版先使用本地图片/视频路径，删除日志时会尽力删除本地保存文件。正式接入云存储后，删除日志时需要同时删除它关联的云存储文件。如果文件删除失败，不应直接丢失数据库信息，后续可以增加一个待清理队列或云函数重试。

字段名暂时仍叫 `images`，是为了兼容旧代码和旧本地数据；实际已经可以通过 `mediaType` 保存图片或视频。

## TravelTicket

```ts
interface TravelTicket {
  id: string
  type: 'flight' | 'train' | 'highSpeedRail'
  title: string
  departure: string
  arrival: string
  departAt?: string
  arriveAt?: string
  carrier?: string
  ticketNo?: string
  seat?: string
  note?: string
}
```

票据当前随 `TravelRecord` 一起保存，适合记录“这次旅行用了哪张机票/火车票/高铁票”。后续如果要做票据图片识别、按行程聚合、多段交通串联，可以把它拆成独立集合 `travel_tickets`，用 `recordId` 或 `tripId` 关联。

## 云开发集合

建议集合名称：

- `users`：用户基础资料。
- `user_region_states`：用户私有点亮状态和统计缓存。
- `travel_records`：用户私有旅行日志。

集合权限建议使用“仅创建者可读写”。云数据库会自动写入 `_openid`，页面查询时不需要把 openid 明文写进前端。
