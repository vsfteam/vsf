# RT28xx USB WiFi 驱动 — 硬件 bring-up 进度

本文档记录 `vsf/source/component/usb/host/class/wifi/` 下 RT2800/RT28xx/RT5572
USB WiFi 驱动当前的实现状态、设计决策、待硬件到位后必须验证/补全的事项，以及
从代码到示波器/抓包的调试路线。

> 目标硬件：RT2870 / RT3070 / RT3572 / RT5370 / RT5372 / RT5572 系列 USB dongle
> 当前状态：**阶段1 已在 vc.linux + WinUSB HCD 上 bring-up 通过** —— 枚举、
> 33 条 init script、8 KiB 固件上传 (128×64B chunk) + post-fw 握手、MCU 就绪
> 全部走通；阶段 C/D/E (EEPROM / parse_rx / TX 头封装) 待续
> 框架依赖：仅依赖 VSF 内核 + USB host 栈，**不依赖 VSF/Linux 子系统**

---

## 1. 体系结构

```
+---------------------------+
| 应用 (vsf_usbh_wifi_on_*)|
+-------------+-------------+
              |
+-------------v-------------+   公共 API
| vsf_usbh_wifi.h           |   vk_usbh_wifi_send / set_channel /
|                           |   scan / connect / disconnect ...
+-------------+-------------+
              |
+-------------v-------------+   类驱动 (chip-agnostic)
| vsf_usbh_wifi.c           |   - ep0 dispatcher (run_script / run_blob)
|                           |   - 状态机 (init chain, scan scheduler,
|                           |     disconnect graceful teardown)
|                           |   - bulk RX/TX URB pool
+-------------+-------------+
              | drv vtable (firmware_load / init / set_channel /
              |              parse_rx ...) + 内部 op-script API
+-------------v-------------+   芯片 backend
| vsf_usbh_wifi_rt28xx.c    |   - 全部 chip-specific 知识
|                           |   - .rodata 静态 op 表 (init / post_fw)
|                           |   - 参数化 op 拼装 (set_channel ...)
+---------------------------+
              |
+-------------v-------------+   固件 blob (链接期注入)
| vsf_usbh_wifi_rt2870_     |   - weak 占位符号 (size = 0)
|   firmware.c              |   - 集成方提供 strong override
+---------------------------+
```

### ep0 dispatcher 模型

类驱动里所有 ep0 vendor 0x01 写入都通过同一个状态机：

| 模式 | API | 用途 |
|------|-----|------|
| script | `vk_usbh_wifi_run_script(ops, count, done)` | 顺序写一组 (reg, val) |
| blob   | `vk_usbh_wifi_run_blob(blob, done)`         | 按 chunk 切片写大块连续数据 (固件) |

- 同时只允许 1 个 ep0 操作在飞，由 `wifi->script_busy` + `dev->ep0.crit` 互斥
- ep0 完成在 `__vk_usbh_wifi_evthandler` 的 `VSF_EVT_MESSAGE` 分支推进
- backend 链式 (firmware_load = run_blob → run_script) 通过
  `wifi->backend_chain_done` 暂存外层 `done`

---

## 2. 已完成的功能

### 2.1 软件框架 (与硬件无关，已实现)

- [x] `vk_usbh_wifi_run_script`：异步寄存器写脚本
- [x] `vk_usbh_wifi_run_blob`：异步固件 blob 上传 (按 chunk 切片)
- [x] init 链：`firmware_load → init → set_rx_filter → start_rx → on_ready`
- [x] scan 调度器：class 层主导跳频，使用 `vsf_callback_timer_t` dwell；
      对扫描期间的 RX 帧路由到 `drv->parse_rx`
- [x] disconnect 优雅终止：5 步流水线 (gate flag → cancel timer → on_del →
      drv->fini → free URB → eda_fini)，evthandler 入口的 `disconnecting` 守卫
- [x] send 失败回滚 `is_busy` (避免 TX slot 永久占用)
- [x] 固件 blob 占位 (`__rt2870_firmware_data` / `__rt2870_firmware_size`)
      — 默认 0 字节 weak stub，集成方 strong override

### 2.2 RT28xx backend (已实现，阶段1 已在 vc.linux 上验证)

- [x] `__rt28xx_init_script[]` (.rodata)：MAC reset / USB_DMA_CFG /
      WPDMA_GLO_CFG / MAC defaults / BBP defaults / RF defaults / TX+RX enable
- [x] `__rt28xx_post_fw_script[]` (.rodata)：固件上传后 H2M mailbox 清零 +
      PBF_SYS_CTRL 释放 patch-RAM 模式
- [x] `__rt28xx_firmware_load`：blob 上传 + post-fw 握手链
- [x] `__rt28xx_channel_table[1..14]`：每个信道的 TX_PIN/BAND/SW_CFG + RF1..4
- [x] `__rt28xx_emit_channel` / `__rt28xx_emit_bssid` 参数化拼装
- [x] backend vtable 全 11 个 hook：firmware_load/init/fini/set_channel/
      set_rx_filter/set_mac_addr/set_bssid/set_auth_mode/connect/disconnect/
      get_link_info

### 2.3 vc.linux + WinUSB HCD bring-up (阶段1 完成)

在 RT5572 (VID=0x148F PID=0x5572) 真实 dongle 上，通过 vc.linux 工程的
winusb_hcd 走通了完整 bring-up 链：

```
/ # usbhost wifi
Device path: \\?\usb#vid_148f&pid_5572#1.0#{a5dcbf10-...}
usbh: new high speed device connected
wifi_usb: probe matched vid=0x148F pid=0x5572 chip=rt28xx
wifi: start: chip=rt28xx, firmware=yes
wifi: uploading firmware ...
wifi: firmware uploaded, running init script ...
wifi: MCU ready, chip is up
```

- 128 个 64-byte 固件 chunk + post-fw 握手 + init script 全部 `urb_status=0`
- 当前的 "MCU ready" 是 init script 全条 ACK 后由 class 层抛出的等价信号；
  真正的 `PBF_SYS_CTRL.MCU_READY` 寄存器轮询仍在 P1 list
- 验证脚本：`project/vc.linux/_run_wifi.bat`，注入文件 `wifi_in.txt` 包含
  `usbhost wifi`，输出落在 `run_wifi.log`

---

## 3. 硬件到位后必须做的事 (按顺序)

### 3.1 先决：拿到 rt2870.bin

- 来源：Ralink/MediaTek 官方 release 或主流 Linux 发行版
  `linux-firmware/rt2870.bin` (8192 字节，校验该字节数)
- 集成方式：写一个 user 文件，例如 `application/.../rt2870_blob.c`：
  ```c
  #include <stdint.h>
  // 用 `xxd -i rt2870.bin` 生成下面的数组
  const uint8_t  __rt2870_firmware_data[] = { 0x01, 0x23, /* ... 8192 bytes */ };
  const uint32_t __rt2870_firmware_size   = sizeof(__rt2870_firmware_data);
  ```
  链入工程后，weak stub 自动让位。`__rt28xx_firmware_load` 会检查
  `size == 8192`，否则报 `VSF_ERR_INVALID_PARAMETER`。

### 3.2 阶段 A — ep0 register-write 通路 ✅ 已完成

**目标：确认 USB 枚举成功 + ep0 vendor 写入被设备 ACK。**

不需要固件。验证步骤：

1. 插入 dongle，确认 `__vk_usbh_wifi_probe` 命中正确的 VID/PID 并分配 `wifi`
   实例 (VID `0x148F` 是 Ralink 主标识)。
2. 在 `__vk_usbh_wifi_on_init_done` 设断点：若 `__rt28xx_init_script` 全部
   ACK，说明 33 条 init 写入 (含 BBP/RF) 都通过。
3. 抓包 (Wireshark + USBPcap)：每条都是 `bRequest=0x01, wValue=<reg>`，
   wLength=4，data=LE32(val)，主机收到 IN STATUS phase。
4. **故障定位**：若 init 中途失败，看 `wifi->s.script.idx` 落在哪一条。
   对照 `__rt28xx_init_script[]` 的注释定位。常见原因：
   - USB_DMA_CFG (0x02A0) 写入超时 → 检查 USB endpoint 配置
   - BBP/RF 写入超时 → BBP_CSR_CFG/RF_CSR_CFG (0x101C/0x1020)，需要确认
     之前 MAC reset 真的成功了 (寄存器 self-clearing)

### 3.3 阶段 B — 固件上传 ✅ 已完成 (MCU_READY 轮询 deferred)

**目标：确认 8 KiB 固件能被切片送到 0x3000 起始的指令 RAM，然后
post-fw 握手让 MCU 跳出 patch-RAM 模式。**

1. 链入 rt2870.bin 后，让 `__rt28xx_firmware_load` 走完整路径。
2. 在 `vk_usbh_wifi_run_blob` 进入循环时，检查每个 chunk 的：
   - `wValue` 应从 `0x3000` 起步，每次递增 `chunk_size` (默认 64)
   - 共应有 `8192 / 64 = 128` 个 chunk
   - 全部 ACK 后切到 `__rt28xx_post_fw_script` (5 条 reg write)
3. **关键观测**：post-fw 握手最后一条 `PBF_SYS_CTRL = 0` 之后，原始 rt2x00
   驱动会**轮询** `PBF_SYS_CTRL.MCU_READY` 位。当前实现暂未轮询 (read 通路尚
   未接入)，硬件到位后必须补全：
   ```c
   // 伪代码 — 在 post_fw_script 之后插入
   poll until (read32(PBF_SYS_CTRL) & MCU_READY)
   ```
   这需要先实现 `vk_usbh_wifi_run_read_script` (vendor 0x07 IN)。

### 3.4 阶段 C — EEPROM 读取 (第 2-3 天)

**目标：拿到设备真实 MAC、RF chip type、TX power 表。**

1. RT28xx 的 EEPROM 经 ep0 vendor 请求 0x07 读出 (256 字节)
   - `bRequestType = USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_DIR_IN`
   - `bRequest = 0x07`
   - `wValue = offset`, `wIndex = 0`, `wLength = chunk`
2. 关键 offset：
   - 0x04..0x09 — MAC address (6 bytes, LE)
   - 0x1A — RF chip type (区分 RT2820/RT3070/RT5370/...)
   - 0x35..0x6A — TX power tables (per channel, per rate)
3. 当前 `set_mac_addr` 接口要求**外部传入** MAC；EEPROM 方案应在
   `firmware_load done` 之后插一步 `eeprom_read`，把读出的 MAC 通过
   `on_ready` 回调暴露给应用，或在 backend 内部直接 `set_mac_addr(self_mac)`。

### 3.5 阶段 D — RX 帧解析 (第 3-4 天)

**目标：让 `vsf_usbh_wifi_on_rx` 收到去掉 RXWI/RXINFO 包头的 802.11 帧。**

1. RT28xx bulk-IN 帧格式 (datasheet §RXINFO/RXWI)：
   ```
   +-----------+-----------+--------------+----------+
   | RXINFO 4B | RXWI ~16B | 802.11 frame | TKIP MIC |
   +-----------+-----------+--------------+----------+
   ```
   - RXWI 大小因芯片而异：RT28xx/RT30xx 为 16B；RT5572/RT55xx 为 20B
   - RXINFO.bit[7..0] = MPDU_LEN (含 802.11)
2. 当前 `parse_rx` 钩子留为 `NULL`，需要在 `vsf_usbh_wifi_rt28xx.c` 实现：
   ```c
   static void __rt28xx_parse_rx(vk_usbh_wifi_t *wifi,
           uint8_t *frame, uint16_t len) {
       // 解 RXINFO -> 跳过 RXWI -> 取出 802.11 头 ->
       // 若是 beacon/probe-resp 且正在扫描 -> 抽 SSID/BSSID/channel/RSSI
       // 调用 vsf_usbh_wifi_on_scan_result(wifi, &result)
   }
   ```
3. 验证手段：在已知 AP 的环境下扫描，对比 `iw scan` 抓到的 BSS 列表。

### 3.6 阶段 E — TX 路径 (第 4-5 天)

**目标：发出 802.11 frame，被监听口抓到。**

1. RT28xx bulk-OUT 帧需要前缀 TXINFO + TXWI (合计 ~16-24 字节)，
   再附原始 802.11 frame，最后 4B padding (DMA done marker)。
2. 当前 `vk_usbh_wifi_send` 直接写 frame — 必须改为：
   - 先在 `wifi_usb_eval` 描述的格式上封 TXINFO/TXWI
   - 计算 wireless protection / rate / power 字段
3. RT2870 与 RT5572 的 TXWI 布局有差异，应该用 vtable 钩子：
   ```c
   uint16_t (*build_tx_header)(vk_usbh_wifi_t *wifi,
           uint8_t *out, const uint8_t *frame, uint16_t len);
   ```
   现在 vtable 没有这个 hook，硬件到位后再加。

### 3.7 阶段 F — MLME / WPA (长期)

- 当前 `set_auth_mode` backend 是 stub，原文档 (`wifi_usb_eval.txt`) 说明
  这是 raw-protocol 设计 — 关联/认证由上层应用直接发裸帧完成。
- 若要加 host-driven WPA2 4-way handshake，需另起 `wpa_supplicant` 风格的
  状态机，**不属于本驱动职责**。

---

## 4. 已知限制与设计决策

### 4.1 不依赖 VSF 内部的 Linux/wireless 子系统

- 用户决策 (本次会话)：**纯 VSF 实现，不引 VSF/Linux**
- 后果：mac80211/cfg80211 那一套全部不可用 (扫描结果解析、信道列表、
  MLME 都得自己写)
- 收益：依赖面小，只要 VSF 内核 + USB host 栈即可，符合 ESP-IDF 子系统
  独立性约束

### 4.2 固件嵌入方案 A (rodata)

- 用户决策：**编译期嵌入为 rodata 静态数组**（不走文件系统）
- 优点：零运行时依赖，无 fopen/lseek
- 缺点：固件占 8 KiB rom；切换芯片型号要重链
- 实现：`vsf_usbh_wifi_rt2870_firmware.c` 提供 weak 0 字节 stub；集成方写
  strong override 文件

### 4.3 ep0 dispatcher 单转单飞

- script 与 blob 共用一个 `script_busy` 标志
- 任何外部 API (set_channel / set_mac / connect / scan ...) 在
  `script_busy` 时会得到 `VSF_ERR_NOT_AVAILABLE`
- 应用层应该在 `on_ready` 后再发起业务命令；scan 期间禁用其它命令

### 4.4 scratch_ops 默认 16 entries

- 单次 backend 操作最坏 11 ops (connect: 2 BSSID + 8 channel + 1 RX filter)
- 若新增 backend 序列超过 16，调 `VSF_USBH_WIFI_CFG_SCRATCH_OPS`

### 4.5 chunk_size 默认 64

- 对应 rt2x00 reference driver 行为
- 真实硬件可调到 256/512 减少 round-trip
- 通过 `VSF_USBH_WIFI_CFG_FW_CHUNK_SIZE` 改

### 4.6 阶段1 bring-up 踩坑记录 (vc.linux + WinUSB)

实测过程中遇到 3 个本质性问题，全部已修复并加注释；后续 bring-up 其它
rt2x00 类芯片或迁移到别的 HCD 时，复发概率极高，列在这里：

#### (1) Ralink rt2x00 vendor 协议参数装错

**症状**：`urb_status=-121` (ERROR_SEM_TIMEOUT)，每次 control transfer 设备
直接超时不响应，所有 ep0 操作 timeout。

**根因**：早期 setup packet 用了 `bRequest=0x01`，那其实是 `USB_DEVICE_MODE`；
并且把寄存器地址塞进了 `wValue`、`wIndex` 填 0。

**正确协议**（对照 Linux kernel `drivers/net/wireless/ralink/rt2x00/rt2x00usb.{h,c}`）：

| 操作 | bRequest | wValue | wIndex |
|------|----------|--------|--------|
| 寄存器写 | `USB_MULTI_WRITE = 6` | `0` | 寄存器偏移 |
| 寄存器读 | `USB_MULTI_READ  = 7` | `0` | 寄存器偏移 |
| 固件块写 | `USB_MULTI_WRITE = 6` | `0` | 基址 + 块内偏移 |

常量已定义在 `vsf_usbh_wifi.c`：`VSF_USBH_WIFI_RT_REQ_MULTI_WRITE/READ`。

#### (2) 固件 BLOCK_WRITE 用了只读 buffer

**症状**：`urb_status=-998` (ERROR_NOACCESS)，第 1 个固件 chunk 就被 WinUSB
拒绝。

**根因**：固件常量数组 `__rt2870_firmware_data` 链入 `.rdata` 段（只读页），
之前用 `vk_usbh_urb_set_buffer` 直接把 urb buffer 指向它。WinUsb_ControlTransfer
要求 buffer 可写（DMA bounce 需求），对只读页指针返回 998。

**修复**：BLOCK_WRITE 路径改用 `vk_usbh_urb_alloc_buffer` 拿一个 HCD 托管
的可写堆缓冲，`memcpy` 进去再发；每个 chunk 完成后配套 `free_buffer`。
REG_WRITE 路径本来就走的是 alloc_buffer，不受影响。

#### (3) Windows CRT system() 子进程吃光重定向 stdin

**症状**：vc.linux 用文件重定向注入命令时，`usbhost wifi` 这一行从未被
shell 看到，加 `rxdbg` 后看到 `ReadFile ok=1 rsize=0`（空读 EOF）。

**根因**：`vsf/source/hal/driver/x86/win/driver.c` 启动时调了
`system("chcp 65001")`，CRT system() 会 spawn cmd.exe 子进程并继承
 stdin 句柄；当 stdin 被重定向到磁盘文件时，父子进程**共享文件指针**，
cmd.exe 在退出前把整个 14 字节注入文件 drain 到 EOF，主进程随后读不到
任何数据。

**修复**：用 Win32 `SetConsoleCP(CP_UTF8) + SetConsoleOutputCP(CP_UTF8)`
替代 `system("chcp ...")`，纯 API 调用不 spawn 任何子进程，stdin 文件
指针保持不动。

---

## 5. 调试 cheatsheet

### 5.1 看 ep0 script 卡住

```c
// 在 evthandler VSF_EVT_MESSAGE 进入处加 trace
if (urb_hcd == dev->ep0.urb.urb_hcd) {
    printf("ep0 done idx=%u/%u status=%d\n",
            wifi->script_is_blob ? wifi->s.blob.offset / wifi->s.blob.chunk_size
                                 : wifi->s.script.idx,
            wifi->script_is_blob ? wifi->s.blob.len / wifi->s.blob.chunk_size
                                 : wifi->s.script.count,
            vk_usbh_urb_get_status(&dev->ep0.urb));
}
```

### 5.2 校验 init script 寄存器写入顺序

参考 `__rt28xx_init_script[]` 注释，每条配 (offset, value)。Wireshark
filter `usb.bRequest == 0x01` 只看 vendor 控制写。

### 5.3 RX 数据完整性

```c
// 在 evthandler RX 分支加 hex dump
if (iocb->is_rx && len > 0) {
    printf("rx %u bytes: ", len);
    for (uint32_t i = 0; i < (len > 32 ? 32 : len); i++)
        printf("%02x ", frame[i]);
    printf("\n");
}
```

### 5.4 验证 disconnect 不泄漏

不停插拔 dongle，检查 `vsf_usbh_malloc` 计数应回到平衡。`__vk_usbh_wifi_free_all`
基于 `vk_usbh_urb_is_alloced`，只释放真分配过的，不会 double-free。

---

## 6. 文件清单

| 文件 | 行数 | 用途 |
|------|------|------|
| `vsf_usbh_wifi.h` | 195 | 公共 API + backend vtable typedef |
| `vsf_usbh_wifi.c` | 882 | 类驱动 (ep0 dispatcher / 状态机 / scan / disconnect) |
| `vsf_usbh_wifi_priv.h` | 233 | 内部数据结构 + 内部 API |
| `vsf_usbh_wifi_rt28xx.c` | 487 | RT28xx backend |
| `vsf_usbh_wifi_rt2870_firmware.c` | 84 | 固件 blob weak 占位 |
| `wifi_usb_eval.txt` | 41 | 原始设计笔记 |
| `BRINGUP.md` | 本文 | 硬件 bring-up 进度 |

---

## 7. 下一步 checklist (硬件到位后)

按优先级排序：

- [x] **(P0)** 拿到 rt2870.bin (8192 字节)，写一个 strong override
      blob 文件链入工程
- [x] **(P0)** 阶段 A：跑通 init script，确认所有 33 条 ep0 写入 ACK
- [x] **(P0)** 阶段 B：固件上传 + post-fw 握手 ACK
- [ ] **(P1)** 加 `vk_usbh_wifi_run_read_script` (vendor `USB_MULTI_READ=7` IN)
- [ ] **(P1)** 用 read_script 实现 `PBF_SYS_CTRL.MCU_READY` 寄存器级轮询
      (取代当前的 init-script-done 隐式信号)
- [ ] **(P1)** 阶段 C：EEPROM 读取 → 自动 set_mac_addr
- [ ] **(P2)** 阶段 D：parse_rx (RXINFO/RXWI 解析)
- [ ] **(P2)** 加 `build_tx_header` vtable hook，重写 `vk_usbh_wifi_send`
- [ ] **(P3)** 阶段 E：TX 路径联调 (按 vsf_usr_cfg 抓包确认)
- [ ] **(P3)** scan beacon 解析 → on_scan_result 触发
- [ ] **(P4)** 多 chip family 适配 (RT5572 RXWI 大小差异等)

---

## 8. 重构历史

本驱动经历了 3 轮迭代，留作复盘：

1. **初版 (review 之前)**：原始的"逐个 API 各写各的 ep0"实现，存在
   异步未等待、ep0 临界区误用、send 失败不回滚等 14 个 bug。
2. **第二轮 (op-script 重构)**：引入 `vk_usbh_wifi_op_t` + `run_script`，
   把所有 ep0 写收口到一个状态机。修了第 1 轮列出的所有 bug。
3. **本轮 (firmware/scan/disconnect)**：
   - 新增 `vk_usbh_wifi_run_blob` 处理大块固件
   - 新增 backend chain (firmware_load = blob → script)
   - class 层主导 scan 跳频调度 (callback timer + parse_rx 钩子)
   - disconnect 5 步优雅终止 + `disconnecting` gate
   - 固件 blob weak 占位文件 (方案 A)

## 9. 编译与运行验证状态

wifi 三个 .c 已加入 `project/vc.linux/vsf_demo.vcxproj`，msbuild Debug/x64
构建通过，0 error / 0 warning。

**已验证**（vc.linux + WinUSB HCD + RT5572 真实硬件）：
- ep0 vendor `USB_MULTI_WRITE=6` 控制传输 — 33 条 init script + 5 条 post-fw
  + 128 个 64B 固件 chunk 全部 `urb_status=0`
- `__rt2870_firmware_data` 8192 字节 blob 切片上传到基址 0x3000，全部 ACK
- post-fw 握手让 MCU 离开 patch-RAM 模式后回报就绪
- disconnect 路径 (拔出 dongle) 不泄漏

**未验证**（待续阶段）：
- vendor `USB_MULTI_READ=7` 读通路（EEPROM / 寄存器读 / MCU_READY 轮询）
- bulk RX 帧解析（RXINFO / RXWI）
- bulk TX 路径（TXINFO / TXWI 头封装）
- scan 跳频 + beacon 解析 → `on_scan_result`
- station 关联 / 数据收发

espidf 测试集 (`vsf_espidf_test.c`) **不包含 wifi 用例**——wifi 不接入
espidf，验证只通过 vc.linux 工程跑 `usbhost wifi` 命令完成。
