# 云数据库权限配置建议

这份文档用于上线前配置微信云开发数据库权限。最终界面和规则语法以微信开发者工具或云开发控制台当前版本为准；本项目的目标是所有旅行数据都按当前微信用户隔离，普通用户不能读取或修改其他用户的数据。

## 需要创建的集合

- `users`
- `user_region_states`
- `travel_records`
- `user_preferences`
- `custom_world_cities`
- `ocr_usage`

## 推荐权限目标

| 集合 | 数据内容 | 推荐权限 |
| --- | --- | --- |
| `users` | 用户资料 | 仅创建者可读写 |
| `user_region_states` | 地区点亮状态 | 仅创建者可读写 |
| `travel_records` | 旅行日志、票据、媒体引用 | 仅创建者可读写 |
| `user_preferences` | 主题偏好 | 仅创建者可读写 |
| `custom_world_cities` | 用户自定义国外城市 | 仅创建者可读写 |
| `ocr_usage` | OCR 日调用计数 | 仅云函数读写，客户端不可访问 |

如果控制台提供“仅创建者可读写”模板，优先使用这个模板。云开发会自动给客户端新增的数据写入 `_openid`，后续读取、更新、删除都应限制在同一 `_openid` 下。

## 安全规则草案

如果你的控制台使用表达式规则，可以按控制台变量名调整为等价规则：

```text
read:  当前用户 openid 等于文档 _openid
write: 当前用户 openid 等于文档 _openid，或当前请求是创建新文档
```

不要把这些集合配置成“所有用户可读”或“所有用户可写”。旅行日志、照片、出行票据、主题偏好都属于用户私有数据。

## 字段检查

上线前在云数据库里随机打开几条数据，确认：

- 每条用户私有数据都有 `_openid`。
- `travel_records.images` 中保存的是云存储 `fileID`，不是本地临时路径。
- `travel_records.tickets` 不包含身份证号、手机号等不必要敏感信息。
- `user_region_states.regionCode` 和 `travel_records.regionCode` 能对应到地图数据。
- `custom_world_cities.countryCode` 是 `country:*` 格式。

## 建议索引

数据量上来后建议给这些字段建索引。启用“仅创建者可读写”后，控制台可能把 `_openid` 自动加入查询条件，因此优先建立以下复合索引（均为升序）：

- `travel_records`: `_openid`, `regionCode`
- `travel_records`: `_openid`, `visitDate`
- `user_region_states`: `_openid`, `regionCode`
- `custom_world_cities`: `_openid`, `countryCode`
- `custom_world_cities`: `_openid`, `regionCode`

索引不是第一天必须，但会影响地图、详情页和统计页的加载速度。

## 云存储权限

当前媒体文件路径包括：

- `travel-images/{openid}/{regionCode}/...`
- `ocr-import/{openid}/...`

上线前至少确认：

- 普通用户只能上传自己的旅行媒体。
- 删除日志时能删除对应云存储文件。
- OCR 临时截图不要长期无清理地堆积。

OCR 和旅行图片均通过云函数生成用户专属前缀；旅行图片删除也由云函数校验文件路径归属：

```text
travel-images/{openid}/{regionCode}/...
```

这样更容易做用户级隔离和批量清理。正式验收仍需在云存储控制台确认普通用户不能管理其他创建者的文件。

## 验收步骤

1. 使用账号 A 新增一条旅行记录和一张图片。
2. 使用账号 B 登录体验版。
3. 账号 B 不能看到账号 A 的记录、点亮状态、主题偏好和自定义城市。
4. 账号 A 删除日志后，云数据库记录消失，关联媒体也被清理。
5. 清空账号 A 本地缓存后重新进入，云端数据能恢复。
