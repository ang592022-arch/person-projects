# 漫迹 · Travel Trace

一个使用原生微信小程序、TypeScript 和配置驱动主题系统实现的私人旅行地图手账。

项目提供地图浏览、旅行地点点亮、日志与媒体记录、旅行时间线、主题设置、年度统计和截图 OCR 导入的完整应用结构。默认使用本地存储，无需云环境即可预览主要页面。

> 这是源码模板，不是已经完成地图审核的线上产品。仓库不包含中国省、市、县行政区边界坐标，使用者需要自行取得合法数据源并完成适用于其发布场景的许可、署名和地图审核要求。

## 功能

- 世界地图与国外城市足迹
- 中国省、市旅行空间的数据结构和按需加载接口
- 旅行日期、标题、感受、标签、同行人、照片、视频和票据信息
- 地区点亮、想去、多次到访和常住地状态
- 时间线、手账卡片和照片列表布局
- 六套配置驱动主题和个性化设置
- 本地存储与微信云开发数据服务
- 腾讯云 OCR 截图导入云函数示例
- TypeScript、源码合规扫描和 GitHub Actions 检查

## 开源版与私人版的区别

公开仓库保留了业务代码、页面、组件、主题、世界地图和国外城市数据，但做了以下清理：

- 删除来源记录无法确认的中国省市 GeoJSON；
- 删除由该数据派生的中国城市坐标；
- 删除真实 AppID、云环境 ID 和本机配置；
- 不包含任何真实旅行记录、OpenID、云文件地址或用户照片；
- 默认以本地存储模式运行。

中国地图页在没有边界配置时会显示“边界数据未导入”。这属于预期状态，不是构建错误。

## 快速开始

### 1. 准备环境

- 安装微信开发者工具；
- 安装 Node.js 20 或更高版本；
- 克隆本仓库。

### 2. 创建本地配置

Windows PowerShell：

```powershell
Copy-Item miniprogram/config/env.example.ts miniprogram/config/env.ts
```

macOS 或 Linux：

```bash
cp miniprogram/config/env.example.ts miniprogram/config/env.ts
```

示例配置使用 `local` 存储且不包含密钥。`env.ts` 已被 `.gitignore` 忽略。

### 3. 导入微信开发者工具

1. 选择“导入项目”；
2. 选择仓库根目录；
3. 使用测试号，或者在 `project.config.json` 中填写自己的 AppID；
4. 点击“编译”。

常用页面：

- 首页：`pages/home/index`
- 中国地图：`pages/china-map/index`
- 世界地图：`packages/map/pages/world-map/index`
- 截图导入：`pages/import-assistant/index`
- 主题设置：`pages/theme-settings/index`

## 中国边界数据

开源仓库不会提供来源不明的边界文件。推荐在私人配置中填写高德 Web Service Key，由现有边界服务在运行时加载；也可以接入其他拥有明确授权的数据提供方。

请勿把接口返回结果直接提交回仓库，除非相应许可明确允许再分发。详细接口和替换位置见 [docs/map-data.md](docs/map-data.md)。

## 云开发与 OCR

本地模式不需要云环境。需要跨设备同步、云存储和 OCR 时：

1. 创建自己的微信云开发环境；
2. 在私有 `miniprogram/config/env.ts` 中填写环境 ID并改为 `cloud`；
3. 按 [docs/cloud-development.md](docs/cloud-development.md) 创建集合和部署云函数；
4. 通过云函数运行身份或环境变量提供 OCR 凭证；
5. 不要把 AppSecret、SecretId、SecretKey 或云端用户数据提交到 GitHub。

## 检查

```bash
pnpm install
pnpm run check
```

检查包含：

- TypeScript 类型检查；
- 中国边界数据和真实项目 ID泄漏扫描；
- OCR 文本解析回归测试。

## 项目结构

```text
miniprogram/
  components/       公共组件
  config/           环境和地图合规配置
  data/             地区元数据、主题与公开地图数据
  pages/            主包页面
  packages/map/     地图分包
  services/         数据、媒体、OCR 和主题服务
  styles/           全局设计变量
  types/            TypeScript 类型
cloudfunctions/
  ocrImport/        OCR 云函数
docs/               数据模型、云开发和地图接入文档
scripts/            自动检查
```

## 隐私与安全

旅行日志和照片默认属于私人数据。公开部署前应配置微信隐私保护指引、数据库按 `_openid` 隔离、云存储路径隔离，并使用两个真实微信账号完成越权测试。安全注意事项见 [SECURITY.md](SECURITY.md)。

## 许可证

项目源码使用 [MIT License](LICENSE)。Natural Earth 等第三方数据和服务不自动转为 MIT，详情见 [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md)。
