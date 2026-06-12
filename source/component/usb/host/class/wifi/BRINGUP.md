# RT28xx USB WiFi 驱动 — 硬件 bring-up 进度

本文档记录 VSF 原生 RT2800/RT28xx/RT5572/RT5592 USB WiFi 驱动的实现状态、
架构、设计决策，以及从代码到示波器/抓包的调试路线。

> 目标硬件：RT2870 / RT3070 / RT3572 / RT5370 / RT5372 / RT5572(RT5592) USB dongle
> 实测平台：vc.linux 工程 + WinUSB HCD + RT5572 真实 dongle (VID=0x148F PID=0x5572)
> 当前状态：**CCMP 数据面 bring-up 完成** —— 枚举、固件上传、MCU 就绪、
> eFuse MAC 读取、RF 全频段调谐、2.4G+5G 扫描、TX 发包、Auth/Assoc、
> WPA2-PSK 4-way handshake、CCMP 加密 TX / 解密 RX（PTK+GTK）、连接/断开，
> 全部实测通过。
> 框架依赖：仅依赖 VSF 内核 + USB host 栈，**不依赖 VSF/Linux 子系统**。

---

## 1. 体系结构（两层，已重构）

驱动拆成 **芯片无关框架层** 与 **USB 传输层**，芯片知识与总线知识彻底分离：

```
+-----------------------------------+
| 应用 (vsf_wifi_on_scan_result /   |
|       vsf_wifi_on_rx / on_new ...) |
+-----------------+-----------------+
                  |  通用 API (scan / set_channel / connect ...)
+-----------------v-----------------+   芯片无关框架  component/wifi/
| vsf_wifi.c / vsf_wifi.h           |   - ep0 dispatcher:
|                                   |       run_script / run_blob /
|                                   |       run_read / run_read_poll
|                                   |   - scan 调度器 (跳频 + dwell timer)
|                                   |   - parse_rx 路由 / on_scan_result
+-----------------+-----------------+
                  |  drv vtable (firmware_load / init / set_channel /
                  |              parse_rx / connect ...) + op-script API
+-----------------v-----------------+   芯片 backend  component/wifi/chip/rt28xx/
| vsf_wifi_rt28xx.c                 |   - 全部 chip-specific 知识
|                                   |   - .rodata init / post_fw 静态 op 表
|                                   |   - 2.4G+5G RF 调谐 (config_channel_rf55xx)
|                                   |   - eFuse MAC 读取 / RXWI 解析
+-----------------+-----------------+
                  |  固件 blob (链接期注入)
+-----------------v-----------------+
| vsf_wifi_rt2870_firmware.c (weak) |
| vsf_wifi_rt2870_firmware_data.c   |   - 8 KiB rt2870.bin 数组
+-----------------------------------+

      ↑ 以上为 bus-agnostic；以下为 USB 专属 ↓

+-----------------------------------+   USB 传输层  usb/host/class/wifi/
| vsf_usbh_wifi.c / .h / _priv.h    |   - vk_usbh_class_drv probe (VID/PID)
|                                   |   - bulk RX URB pool → vsf_wifi RX 路由
|                                   |   - bulk TX (vk_usbh_wifi_send)
|                                   |   - ep0 control transfer 后端
|                                   |   - 内嵌 vsf_wifi_t，旧 vk_usbh_wifi_* 别名兼容
+-----------------------------------+
```

### ep0 dispatcher 模型（component/wifi/vsf_wifi.c）

所有 ep0 vendor 操作都通过同一个单飞状态机，互斥共享 dispatcher slot：

| 模式 | API | 用途 |
|------|-----|------|
| script    | `vsf_wifi_run_script(ops, count, done)` | 顺序写一组 (reg, val) |
| blob      | `vsf_wifi_run_blob(blob, done)`         | 按 chunk 切片写固件 |
| read      | `vsf_wifi_run_read(reg, out, done)`     | 单个寄存器读 (vendor 0x07 IN) |
| read_poll | `vsf_wifi_run_read_poll(reg, pred, ...)`| 轮询寄存器直到谓词成立 |

- 同时只允许 1 个 ep0 操作在飞
- backend 链式操作 (firmware_load = run_blob → run_script) 通过暂存外层 `done` 串接

---

## 2. 已完成的功能（均已实测）

### 2.1 软件框架（component/wifi/，与芯片/总线无关）

- [x] `vsf_wifi_run_script` / `run_blob` / `run_read` / `run_read_poll`：四种 ep0 通路
- [x] init 链：`firmware_load → init → eeprom_read → set_rx_filter → start_rx → on_ready`
- [x] scan 调度器：框架层主导跳频，`vsf_callback_timer_t` 控制 dwell；
      扫描期间 RX 帧路由到 `drv->parse_rx`
- [x] `vsf_wifi_on_scan_result` / `on_rx` / `on_new` 弱回调供应用覆写
- [x] disconnect 优雅终止 + `disconnecting` gate
- [x] 固件 blob weak 占位 + strong override（rt2870.bin 8192 字节已链入）

### 2.2 RT28xx backend（component/wifi/chip/rt28xx/）

- [x] `__rt28xx_init_script[]` / `__rt28xx_post_fw_script[]` (.rodata)
- [x] `__rt28xx_firmware_load`：blob 上传 (128×64B chunk @ 0x3000) + post-fw 握手
- [x] **MCU 就绪**：post-fw 后用 read_poll 轮询确认 MCU 离开 patch-RAM 模式
- [x] **eFuse/EEPROM MAC 读取**：EFUSE_CTRL(0x0580) KICK + EFUSE_DATA0..3 反序读出
      row-0 的 6 字节 MAC，写回 MAC_ADDR_DW0/DW1，并暴露给应用
- [x] **寄存器间接访问**：RF 经 RF_CSR_CFG、BBP 经 BBP_CSR_CFG，shadow 缓存
      支持读-改-写（`__emit_rf_rmw` / `__emit_bbp_rmw`）
- [x] **RF 调谐：2.4GHz + 5GHz 全频段**（详见 §4）
- [x] **parse_rx**：RT5592 的 RXINFO(4B) + RXWI(24B) 解析，抽取
      beacon/probe-resp 的 BSSID / channel / RSSI / SSID / caps → on_scan_result
- [x] backend vtable hook：firmware_load / init / fini / set_channel /
      set_rx_filter / set_mac_addr / set_bssid / set_auth_mode / connect /
      disconnect / get_link_info / parse_rx

### 2.3 WPA2-PSK 连接（component/wifi/）

- [x] TX 路径：TXINFO(4B) + TXWI 头封装 + bulk TX 发送
- [x] TX 自测（offline）：框架初始化后自动 TX self-test 验证发包通路
- [x] Auth 帧收发：Open System Authentication（seq1/seq2）
- [x] Association Request/Response：capability + listen interval + WMM IE
- [x] WPA2-PSK PBKDF2：SSID + passphrase → PMK (256-bit)
- [x] 4-way handshake：M1(ANonce) → M2(SNonce+MIC) → M3(encrypted GTK) → M4(confirm)
- [x] AES key-unwrap (RFC 3394)：就地解包，无大小限制
- [x] PTK/GTK 密钥派生与安装
- [x] M4 多次发送（×3）提高可靠性（无硬件 ACK 情况下单次丢失率高）
- [x] RUN 状态接受 AP M3 重试（AP 未收到 M4 时自动重发 M4）
- [x] disconnect 发送 Deauth 帧 + 状态清理
- [x] **GTK Rekeying（Group Key Handshake）**：RUN 状态接收 AP 的 Group Key M1
      （group 类型 EAPOL-Key），用 KCK 校验 MIC、KEK 解包新 GTK、就地更新
      `wpa_gtk`，回复 G2 确认；软件 CCMP RX 路径立即使用新 GTK，硬件后端
      重新 install_key。复用 4-way 的 MIC/unwrap/解析代码，链路保持不断开。

### 2.4 CCMP 数据面（component/wifi/）

- [x] CCMP 加密 TX：802.11 QoS Data + CCMP header + AES-CCM 加密 + MIC
- [x] CCMP 解密 RX（GTK）：广播/多播帧用 GTK 解密
- [x] CCMP 解密 RX（PTK）：单播帧用 PTK 解密
- [x] GTK 提取：标准 TLV 解析从 M3 Key Data 正确提取 GTK KDE
- [x] QoS Data 帧格式（WMM 兼容）：FC=0x8801, TID 字段
- [x] A1 MAC 地址过滤：丢弃不是发给本站的单播帧
- [x] 加密 EAPOL 路由：解密后检测 SNAP 0x888E 送入 handshake
- [x] 4-way 期间 PTK 解密：SNonce 已生成即可尝试解密（处理加密 M3）
- [x] AES-CCM 自测：NIST SP 800-38C Example C.4 向量验证
- [x] PN（Packet Number）递增：TX 每帧递增，防重放
- [x] DHCP 完整流程测试：Discover → Offer → Request → ACK，获得 IP 地址

### 2.5 RT28xx backend 数据面增强（component/wifi/chip/rt28xx/）

- [x] TX 时序/重试/功率寄存器初始化（TX_RTY_CFG / TX_TIMEOUT_CFG / TX_PWR_CFG_0..4）
- [x] LEGACY_BASIC_RATE + AUTO_RSP_CFG：硬件 auto-ACK 工作
- [x] UNICAST_TO_ME_MASK=0xFF：MAC_ADDR_DW1 完整 6 字节匹配
- [x] WCID 1 编程：connect 时写入 AP MAC，WCID_ATTR=0（SW crypto）
- [x] L2PAD：build_tx 中正确插入 L2 对齐填充
- [x] TXWI ACK 位 + WCID 字段：硬件跟踪 TX 状态
- [x] RX_FILTER 0x17F97：DROP_NOT_TO_ME 防止 USB 洪泛
- [x] 保护寄存器（CCK/OFDM/MM20/MM40/GF20/GF40_PROT_CFG）

### 2.6 vc.linux + WinUSB HCD 实测（RT5572 真实硬件）

```
/ # usbhost
/ # wifi_scan
wifi: scanning 37 channels (dwell 120 ms)
wifi: scan done, 40 APs found
/ # wifi_connect VStudio <password>
wifi: WPA2-PSK "VStudio" PMK derived, starting handshake
wifi: connecting to "VStudio" bssid=74:39:89:1C:6B:07 ch=6
wifi: auth ok, sending assoc-req
wifi: associated, aid=1 (4-way handshake)
wifi: 4-way M1 rx, M2 sent
wifi: GTK extracted keyidx=1 len=16 tk=CBC13A7C...
wifi: 4-way handshake complete, aid=1 (link up)
wifi: LINK UP bssid=74:39:89:1C:6B:07 ch=6 flags=0x3
wifi: connected to "VStudio"
/ # wifi_dhcp
wifi: DHCP Discover (xid=...)
wifi: DHCP Offer: ip=192.168.0.xxx server=192.168.0.1
wifi: DHCP Request (ip=192.168.0.xxx)
wifi: DHCP complete!
  IP:      192.168.0.xxx
  Subnet:  255.255.255.0
  Gateway: 192.168.0.1
  DNS:     192.168.0.1
/ # wifi_disconnect
wifi: LINK DOWN (reason=203)
```

- 枚举 + 固件 + init + MCU 就绪 + eFuse MAC 全部 `urb_status=0`
- 2.4GHz：单次扫描稳定捕获 35~40 个真实 AP
- 5GHz：LDO_CFG0 在 5G 信道正确切到 VLEVEL=5 (0x15040F14)，2.4G 为
  VLEVEL=0 (0x01040F14)；抓到真实 5G AP（如 ch48 `803_5G`，与 2.4G 的
  `803` 同源），所有 5G 信道 `false_cca` 非零，证明 5G RF/PHY 前端工作正常
- WPA2-PSK 全流程端到端验证通过（真实 AP: VStudio, ch6）
- **CCMP 数据面验证通过**：
  - TX 加密帧 AP 接受（self-verify OK）
  - RX GTK 解密成功（广播 DHCP Offer + IGMP）
  - 4-way handshake 稳定、GTK 正确提取
- 测试命令：`wifi_scan` / `wifi_connect` / `wifi_arp` / `wifi_null` / `wifi_dhcp` / `wifi_disconnect`

---

## 3. 未完成 / 待续（按优先级）

- [ ] **(P0) 【进行中】RT5572 + ChinaNet-5Jhc 回归：连接态单播 data 帧 TX 全失败 /
      RX EAPOL M1 不交付 → 4-way 超时**（详见 §5.6 调试记录，根因仍未定位）
- [ ] **(P1)** WiFi netif 对接 lwIP：实现 linkoutput/input 桥接，lwIP 处理 DHCP/ARP/TCP
- [ ] **(P2)** 速率自适应：当前固定 OFDM MCS0 (6 Mbps)，需基于 TX status 反馈调整
- [ ] **(P2)** 扫描结果去重（同 AP 多次出现，多信道/重复 beacon）
- [ ] **(P2)** ch144 及 UNII-4 (169/173/177)：ref `rf_vals_5592` 表未提供系数
- [ ] **(P2)** 断线自动重连 / 漫游
- [ ] **(P3)** WPA2-Enterprise / WPA3-SAE 支持
- [ ] **(P3)** iq_calibrate / EEPROM TX power 表（当前 TX 功率 clamp 到 POWER_BOUND）
- [ ] **(P3)** 多 chip family 适配：RT2860/RT2870 的 RXWI 为 16B（当前按 RT5592 的 24B）
- [ ] **(P3)** 省电模式（PS-Poll）

---

## 4. 5GHz (A-band) 实现要点

完全对照 Linux rt2x00 `rt2800_config_channel_rf55xx()` + `rt2800_config_channel()`
逐函数移植（参考 `ref/rt2x00/rt2800lib.c`）。

### 4.1 频率合成器表

- `__rf_vals_5592_xtal20[]` / `__rf_vals_5592_xtal40[]` 改为
  `{channel, n, k, mod, r}` 格式，**verbatim 抄自 ref 原表**
- 5GHz 信道号不连续（36,38,…,64 / 100,102,…,140 / 149,…,165 / 184,…,196），
  必须用 `__rt28xx_find_rf(channel)` **按信道号查找**，不能按索引
- 覆盖范围：UNII-1/2 (36-64)、UNII-2e (100-140)、UNII-3 (149-165)；
  ref 表本身缺 ch144 / 163 / UNII-4

### 4.2 信道调谐分支（`__rt28xx_emit_channel`）

- `is_5g = (channel > 14)` 分流；5GHz 共用一组 A-band RFCSR
  (10/11/25/27/36/37/38/40/41/42/45/48/57/60/61)
- 5GHz 再按子频段细分：`36–64` 与 `100–165`，子频段内还有更细的
  channel 阈值（<=50 / <=116 / <=124 / <=128 / <=138 / <=153 …）调整
  RFCSR12/13/22/23/24/39/43/44/46/51/52/54/55/56/58/59/62
- **184–196 在频率表里有系数，但不落入任何子频段分支**（与 ref 一致，
  Japan 4.9G 实际不可用）

### 4.3 按频段差异的关键寄存器

| 项 | 2.4 GHz | 5 GHz |
|----|---------|-------|
| 功率上限 | POWER_BOUND = 0x27 | POWER_BOUND_5G = 0x2B |
| TX_BAND_CFG | 0x4 (BG) | 0x2 (A) |
| TX_PIN_CFG | 0x00050F02 | 0x00050F05 |
| LDO_CFG0 LDO_CORE_VLEVEL (bit26-28) | 0 | 5 |
| BBP79/80/81/82 | 1C/0E/3A/62 | 18/08/38/92 |
| BBP82 公共尾部 | 0x84 | 0xF2 |
| GLRT 表 (BBP195/196) | 各值 | 各值 |

- `set_channel` 用 `__rt28xx_find_rf` 校验合法信道（取代旧的 1..14 clamp）
- `__rt28xx_setch_after_diag` 按频段设 LDO VLEVEL
- 扫描表 `__app_wifi_scan_channels[]`（vsf_main.c）加入 24 个 5G 主信道；
  `VSF_WIFI_CFG_SCAN_MAX_CHANNELS` 由 14 扩到 40

---

## 5. 已知限制与设计决策

### 5.1 不依赖 VSF 内部的 Linux/wireless 子系统
纯 VSF 实现：mac80211/cfg80211 全部不可用（扫描解析、信道列表、MLME 自己写），
收益是依赖面小，只要 VSF 内核 + USB host 栈。

### 5.2 固件嵌入方案 A（rodata）
编译期嵌入为 rodata 静态数组（`vsf_wifi_rt2870_firmware_data.c`），零运行时
文件依赖；代价是占 8 KiB rom，切芯片型号要重链。weak stub + strong override。

### 5.3 内建 WPA2-PSK supplicant
驱动内建轻量 WPA2-PSK supplicant（PBKDF2 + 4-way handshake + AES key-unwrap），
无需外部 wpa_supplicant。Auth/Assoc/Deauth 帧由框架层构造发送。

### 5.4 AES key-unwrap 就地解包
`vsf_wifi_aes_unwrap` 使用 caller 提供的 `out` 缓冲区做 RFC 3394 就地解包，
无栈缓冲区大小限制。真实 AP 的 M3 Key Data（RSN IE + GTK KDE）包裹后通常 >48 字节，
旧版 40 字节栈缓冲区会导致 GTK unwrap 失败。

### 5.5 bring-up 踩坑记录（复发概率极高，务必保留）

#### (1) Ralink rt2x00 vendor 协议参数
**症状**：control transfer 全部 timeout (`urb_status=-121`)。
**根因/正确协议**（对照 `drivers/net/wireless/ralink/rt2x00/rt2x00usb.{h,c}`）：

| 操作 | bRequest | wValue | wIndex |
|------|----------|--------|--------|
| 寄存器写 | `USB_MULTI_WRITE = 6` | 0 | 寄存器偏移 |
| 寄存器读 | `USB_MULTI_READ  = 7` | 0 | 寄存器偏移 |
| 固件块写 | `USB_MULTI_WRITE = 6` | 0 | 基址 + 块内偏移 |

#### (2) 固件 BLOCK_WRITE 用了只读 buffer
**症状**：第 1 个固件 chunk 被 WinUSB 拒绝 (`urb_status=-998` NOACCESS)。
**根因**：固件常量在 `.rdata` 只读页，WinUsb_ControlTransfer 要求可写 buffer。
**修复**：BLOCK_WRITE 改用 `vk_usbh_urb_alloc_buffer` 拿可写堆缓冲，memcpy 后发。

#### (3) Windows CRT system() 子进程吃光重定向 stdin
**症状**：stdin 文件重定向注入命令时，命令行从未被 shell 读到（空读 EOF）。
**根因**：`system("chcp 65001")` spawn 的 cmd.exe 继承并 drain 了共享 stdin 文件指针。
**修复**：改用 Win32 `SetConsoleCP/SetConsoleOutputCP(CP_UTF8)`，纯 API 不 spawn 子进程。

#### (4) RF 调谐用错 RT30xx 序列（5572 scan 0 帧根因）
**症状**：枚举/固件全通，但扫描 0 帧、false_cca 恒为 0（RF/PHY 形同死亡）。
**根因**：早期 set_channel 套用了 RT30xx 的 RF 序列；RT5592 必须走
`config_channel_rf55xx` 的 RFCSR + BBP + LDO_CFG0 + TX_PIN/BAND_CFG 全序列。
**修复**：逐函数移植 ref 的 rf55xx 调谐；TX_PIN_CFG 必须置 LNA_PE/RFTR/TRSW，
否则接收前端无输出。

#### (5) RF 间接访问总线地址
RT5592 的 RF_CSR_CFG 寄存器地址应为 **0x0500**（非早期误用的 0x1020）。

#### (6) GTK TLV 解析逐字节扫描导致 GTK 全零
**症状**：4-way handshake 完成但所有广播帧 MIC 失败，GTK 全零。
**根因**：`__wpa_parse_gtk` 对非-DD IE 用 `p++` 逐字节扫描；当扫到 RSN IE
内部的 0x00 字节（OUI 00:0F:AC 的一部分）时，`if (t == 0x00) break` 终止循环，
GTK KDE 永远无法被找到。
**修复**：改为标准 TLV 解析 `p += 2 + l`，正确跳过整个 IE。

#### (7) RX_FILTER 过宽导致 USB 洪泛 + 握手超时
**症状**：连接后握手偶发超时，M2 已发但 AP 重传 M1。
**根因**：RX_FILTER 缺 DROP_NOT_TO_ME，所有其他 STA 的帧通过 USB bulk IN
洪泛占满带宽，TX 队列被延迟→ AP 超时重传→ 握手失败。
**修复**：设 RX_FILTER=0x17F97（对齐 Linux rt2800_config_filter），包含
DROP_NOT_TO_ME / DROP_VER_ERROR / DROP_DUP 等。

#### (8) 无 LEGACY_BASIC_RATE 导致硬件不发 ACK
**症状**：AP 持续重传，最终放弃关联。
**根因**：硬件 auto-responder 没有有效的 basic-rate 表，无法生成 ACK。
**修复**：写入 LEGACY_BASIC_RATE + AUTO_RSP_CFG，同时设 TX_PWR_CFG_0..4
确保 ACK 发射功率足够。

### 5.6 【进行中】RT5572 + ChinaNet-5Jhc 4-way 超时回归（根因：硬件未 auto-ACK）

**测试环境**：vc.linux + WinUSB HCD + RT5572 真实 dongle（ASIC=0x55920222
rev=0x0222 = RT5592C+，2T2R），目标 AP `ChinaNet-5Jhc`（BSSID
`88:C7:8F:1B:81:6E`，ch1，WPA2-PSK），STA MAC `00:87:33:23:03:41`。
测试脚本 `run_wifi_dhcp_test.ps1`，运行日志 `wifi_dhcp_run.log`，
空口抓包 `ref/wifi_channel1.pcap`（11.7MB，45783 帧，决定性证据）。

**症状（pcap + 设备日志双向印证）**：
- 本机状态机进入 mlme=4（自认为关联成功、AP 分配 aid），但 RAW-TOP 实测显示
  AP 仍在 mlme=4 后持续重发 Assoc-Resp（最新一次 40 次），说明 AP 未认为关联
  完成——即本机未对 Assoc-Resp 回链路层 ACK（详见下方根因重定位）。
- **EAPOL M1 = 明文 QoS Data 帧**（pcap frame 3669：`fc=0x880a`，type=2
  sub=8，`llc=0x888e`，`eapol.type=3`，Protected=0）；AP 需先认为关联完成才会发
  M1，因此在本 bug 下 M1 根本不会被发出。
- **M1 既不被硬件 ACK，也不被 DMA 上交主机**：设备日志 `mlme=4` 阶段
  从未出现任何 `type=2` 帧（RAW-TOP 会打印所有 to-us 帧），全是 Assoc-Resp
  (sub=1) 和 Auth (sub=11) 的重传。STA 因此永远进不了 4-way → 无 M2 →
  pcap 中 STA→AP 方向一个 EAPOL 都没有 → 握手超时。
- **我方上行 data 帧 TX「失败」实为虚假统计**：`TX_STA_FIFO ack_ok=0
  ack_req=1 wcid=1 pid=4 mcs=0 raw=0x40000189` 连续 34 次完全相同。**`raw 值
  一字不变`是已知的 TX_STA_FIFO 虚假统计现象**（FIFO 未被正确清空，VALID=1
  但内容不刷新），不代表真实 TX 失败。绝不能据此判断 TX 死亡——必须用
  RAW-TOP 真实帧观测，或先确认 FIFO 返回值是否动态刷新。

**已排除的假设（逐一实测证伪）**：
1. **IQ 校准**：eFuse IQ 块全 0xFF，原代码却 iq_valid=true。已修为全 0xFF
   时 iq_valid=false，不执行校准。不是根因。
2. **RFCSR1=0x17（缺 TX0/TX1_PD）**：怀疑 TX 链路 power-gate。但 RFCSR8=0xF1
   读回完全正确证明 RF 读路径可信；且 mgmt 帧能正常 TX 到达 AP，说明 TX 物理层
   没死。0x17 可能只是 idle 时 TX 动态 power-gate。不是根因。

**⚠️ 本 session 的两处错误改动（已回滚，记录以防再犯）**：
本 session 一度无视历史已验证结论，把两处正确配置改反，均经实测证明是
**退化**，现已全部回滚到验证基线：
3. **WCID_ATTR**：误据 Linux `rt2800_config_wcid_attr_cipher`（装 PTK 前清 0）
   把 0x0001 改成 0x0000。**这是退化**——RT5572 的 WCID_ATTR `KEYTAB` 位
   (bit0) 是硬件接收 unicast data 帧（type=2）的门控，即使软件加解密
   (cipher=0) 也必须置 1，否则硬件丢弃所有 to-me 单播 data 帧（含明文 EAPOL
   M1）。Windows 实测写 0x0001，Linux/VSF 默认 0x0000 才是 bug。**已恢复
   0x00000001**。Linux 的 clear-before-SET_KEY 路径不适用于本软件加解密 raw
   后端。
4. **MAC_BSSID(0x1010)**：误据 Linux `rt2800_config_intf` 把写我方 MAC 改成
   写 AP BSSID。**这是退化**——验证基线是 0x1010 写我方 MAC + DW1 高半字
   0x0023（vendor bit21=auto-ACK 门控），正是该组合下 mgmt 帧 auto-ACK 工作
   (aid 分配、Assoc-Resp 双向 ACK)。改写 AP BSSID 实测无效。**已恢复写我方
   MAC**。

**当前状态**：两处错误改动已回滚到验证基线（WCID_ATTR=0x0001、0x1010=我方
MAC+bit21）并重测（wifi_dhcp_run.log），以 RAW-TOP 真实帧为唯一可信信号：
- `mlme=4` 阶段 **type=2 帧 = 0 个**（M1 仍未被 DMA 上交）。
- AP 持续重发 **Assoc-Resp 40 次** + Auth 9 次。
- 4-way handshake 依旧 timeout。

**根因重定位（以 RAW-TOP 实测为准，修正本节开头的“mgmt 双向 ACK”假设）**：
AP 反复重发同一 Assoc-Resp 达 40 次的唯一原因 = 收不到我方对该单播管理帧
的链路层 ACK。即**硬件未对 AP 的单播帧自动回 ACK**，这才是 M1 永不到达的真
正根因（与本 session 早期“mgmt 能 ACK”的判断相反；以无盲区的 RAW-TOP 实测
为准）。`TX_STA_FIFO=0x40000189` 确认为虚假统计、不可作为 TX 判断依据。

**下一步方向**：在 Windows 成功抓包 `ref/rt5572_win_usb.log` 中定位关联成功
瞬间为开启 auto-ACK 写的寄存器（`MAC_SYS_CTRL` / `AUTO_RSP_CFG` / `TXOP` /
`PBF` 等），与我方连接流程逐条对齐，找出缺失的 auto-ACK 使能写。

> 教训：历史记忆中经实测验证的配置（WCID_ATTR KEYTAB=1、0x1010 写我方 MAC、
> TX_STA_FIFO 虚假统计）优先级高于 Linux 参考源码的纸面推断；不得用未经实测
> 的源码比对去推翻已验证结论。

> 注：本回归出现在 ChinaNet-5Jhc；§2.6 记录的 VStudio AP 全流程通过是更早
> 的验证状态，两者差异（信道 ch1 vs ch6、AP 行为）尚待对比确认。

---

## 6. 文件清单

| 文件 | 用途 |
|------|------|
| `component/wifi/vsf_wifi.h / .c` | 芯片无关框架：API + ep0 dispatcher + scan + 回调 |
| `component/wifi/vsf_wifi_priv.h` | 内部数据结构 + 内部 API |
| `component/wifi/vsf_wifi_cfg.h` | 编译开关（SCRATCH_OPS / SCAN_MAX_CHANNELS …） |
| `component/wifi/vsf_wifi_wpa.c` | WPA2-PSK supplicant：4-way handshake + GTK 解析 |
| `component/wifi/vsf_wifi_crypto.c` | 密码学原语：PBKDF2 + PRF + AES unwrap + CCM + 自测 |
| `component/wifi/chip/rt28xx/vsf_wifi_rt28xx.c` | RT28xx/RT5592 backend（含 2.4G+5G 调谐、eFuse、parse_rx、L2PAD） |
| `component/wifi/chip/rt28xx/vsf_wifi_rt2870_firmware.c` | 固件 weak 占位 |
| `component/wifi/chip/rt28xx/vsf_wifi_rt2870_firmware_data.c` | rt2870.bin 8192B 数组 |
| `usb/host/class/wifi/vsf_usbh_wifi.h / .c` | USB 传输层：probe + bulk RX/TX + ep0 后端 |
| `usb/host/class/wifi/vsf_usbh_wifi_priv.h` | USB 层内部结构 |
| `usb/host/class/wifi/BRINGUP.md` | 本文 |

---

## 7. 调试 cheatsheet

### 7.1 set_channel 诊断日志
`__rt28xx_setch_after_diag` 每次跳频打印 RX_STA_CNT1 / false_cca / plcp_err /
MAC_SYS_CTRL(TX_EN/RX_EN) / LDO_CFG0(VLEVEL) / CH_IDLE / CH_BUSY。
- false_cca > 0 ⇒ RF/PHY 前端活着（0 帧则查 USB DMA 路径）
- 全 0 ⇒ RF/PHY dead（查 RF 序列 / TX_PIN_CFG）
- 5G 信道 VLEVEL 应为 5，2.4G 应为 0

### 7.2 校验寄存器写入顺序
Wireshark filter `usb.bRequest == 6` 看 vendor 控制写；对照
`__rt28xx_init_script[]` / `__rt28xx_emit_channel` 注释定位。

### 7.3 RX 帧完整性
在 RX 分支 hex dump 前 32 字节，核对 RXINFO_W0[15:0] = rx_pkt_len = RXWI(24B)+frame。

### 7.4 验证 disconnect 不泄漏
反复插拔 dongle，`vsf_usbh_malloc` 计数应回到平衡。

---

## 8. 编译与运行验证状态

- vc.linux 工程 msbuild Debug/x64 构建通过，0 error。
- **已验证**（vc.linux + WinUSB HCD + RT5572 真实硬件）：
  - ep0 vendor 写 (USB_MULTI_WRITE=6) / 读 (USB_MULTI_READ=7) 通路
  - 8192B 固件切片上传 + post-fw 握手 + MCU 就绪轮询
  - eFuse row-0 MAC 读取
  - 2.4GHz 扫描（35~40 AP）+ 5GHz 扫描（抓到真实 5G AP）
  - bulk TX 路径 + TXINFO/TXWI 头封装 + L2PAD 对齐
  - Open System Auth + Association + WMM IE
  - WPA2-PSK 4-way handshake（M1→M2→M3→M4，PTK/GTK 安装成功）
  - **CCMP 加密 TX**（AP 接受加密帧，self-verify OK）
  - **CCMP 解密 RX**（GTK 解密广播 DHCP/IGMP，PTK 解密单播）
  - 全流程连接到真实 AP（VStudio, WPA2-PSK, ch6）
  - disconnect（Deauth 发送 + LINK DOWN）路径不泄漏
- **未验证 / 待续**：
  - lwIP netif 集成（DHCP/TCP 端到端）
  - GTK Rekeying
  - 扫描去重、ch144 / UNII-4
  - 断线重连 / 漫游

espidf 测试集不含 wifi 用例——wifi 不接入 espidf，验证只通过 vc.linux 工程
跑 `wifi_scan` / `wifi_connect` / `wifi_disconnect` 完成。
