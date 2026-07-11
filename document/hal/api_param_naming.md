# VSF HAL API 参数命名规范 (Parameter Naming Standard)

本规范约束 `source/hal/driver/common/template/vsf_template_*.h` 中所有 HAL API 的**形参命名**,目的是消除歧义、保证一致,避免反复返工。

> 适用范围:**新增 / 修改的 API 必须遵守**;存量 API **逐步对齐**(不强制一次性全量重命名)。`vsf_template_usb.h` 使用另一套旧模板宏风格,不在本规范内(另案处理)。

---

## 一、核心原则

**参数名表达"角色"(数据缓冲 / 配置 / 数量 / 通道 / 掩码 …),同一角色在全 HAL 使用同一规范名;参数承载的具体内容语义写进 doxygen 注释;只有当同一函数内"同一角色出现多个"时,才通过前缀 / 方向来消歧。**

即:
- 名字回答"这是什么**角色**",内容由注释回答"具体是什么"。
- 消歧只针对"角色重复",不针对"内容差异"。

---

## 二、规则

1. **三处一致(硬性)**:同一函数的形参名,必须在以下三处**完全相同**,任一处不同即为 bug:
   - `VSF_xxx_APIS` 宏(生成函数指针类型 / 驱动声明)
   - `extern` 原型声明
   - doxygen `@param`(以及 `@note` 正文中引用该参数处)

2. **指针一律 `_ptr` 结尾**。唯一例外:`ctrl` 系列的不透明参数 `void *param`(保留惯用名,不加 `_ptr`)。

3. **明确 > 简短**:
   - 禁止无语境缩写:`cnt` / `num` → `count`;`buf` → `buffer`。
   - 裸 `cfg_ptr` **仅限**"整个外设配置"(见词汇表);任何子配置必须带语境前缀。

4. **消歧靠前缀 / 方向(仅当同一角色重复时)**:
   - 子配置:`channel_cfg_ptr`(单)、`channel_cfgs_ptr`(数组)、`irq_cfg_ptr`。
   - 数组用复数(`cfgs`)+ `_ptr`。
   - 收发双缓冲:`in_buffer_ptr` / `out_buffer_ptr`。
   - 多数量:`sg_count` 等(默认单数量用 `count`)。

---

## 三、角色词汇表 (Canonical Names)

| 角色 | 规范名 | 说明 |
|---|---|---|
| 外设实例句柄 | `<periph>_ptr` | `adc_ptr`、`i2c_ptr` … |
| 整个外设配置 | `cfg_ptr` | 仅 `init` / `get_configuration` 等整体配置。**唯一允许的裸 cfg** |
| 通道配置(单 / 数组) | `channel_cfg_ptr` / `channel_cfgs_ptr` | 单复数区分,均带 `_ptr` |
| 中断配置 | `irq_cfg_ptr` | |
| 通用数据缓冲 | `buffer_ptr` | 承载传输数据的内存(采样/收发/读写),内容语义写注释 |
| 收发双缓冲 | `in_buffer_ptr` / `out_buffer_ptr` | SPI 全双工等 |
| scatter-gather 缓冲 | `sg_buf_ptr` | 传的是缓冲(如 eth) |
| scatter-gather 描述符 | `sg_desc_ptr` | 传的是描述符(如 dma) |
| 数量 / 元素个数 | `count` | 禁用 `cnt`/`num`;多数量时加前缀如 `sg_count` |
| 通道编号 | `channel` | 值类型,非指针 |
| 中断掩码 | `irq_mask` | 值类型 |
| 偏移 | `offset_ptr` / `offset_of_bytes` | 指针 / 字节数 |
| 请求描述符 | `req_ptr` | 如 `vsf_sdio_host_request(sdio_ptr, req_ptr)` |
| ctrl 不透明参数 | `param` | **例外**:指针但不加 `_ptr` |

> `sg_buf_ptr`(缓冲)与 `sg_desc_ptr`(描述符)按"传的到底是缓冲还是描述符"选择,不可混用。

---

## 四、校验

- 改 / 加 API 时,三处同步命名。
- 定期扫描不一致:逐函数比对 `VSF_xxx_APIS` 宏形参名 vs `extern` 原型形参名 vs doxygen `@param`。
- 提交前用 `git diff` 审查:命名类改动应是"纯改名"(增删行数平衡、无签名/类型/逻辑变化)。

---

## 五、示例(修订前 → 修订后)

| 位置 | 修订前 | 修订后 |
|---|---|---|
| `adc/dac channel_config` | `cfgs_ptr`, `cnt` | `channel_cfgs_ptr`, `count` |
| `adc/dac channel_request_once` | `channel_cfg` | `channel_cfg_ptr` |
| `gpio exti_irq_config/get` | `cfg_ptr` | `irq_cfg_ptr` |
| `sdio host_request` | `req` | `req_ptr` |
| `rtc get/set` | `rtc_tm` | `rtc_tm_ptr` |
| `eth send_sg/recv_sg` | `sg_ptr` / `buf_ptr` | `sg_buf_ptr` |
| `dma channel_sg_config_desc` | `scatter_gather_cfg` | `sg_desc_ptr` |
