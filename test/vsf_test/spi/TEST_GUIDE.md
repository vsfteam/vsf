# VSF SPI 测试编写指南（vsf_test）

面向对象：为基于 `source/hal/driver/common/template/vsf_template_spi.h` 的 SPI 实现编写测试，测试由 `vsf_test` 模块运行，覆盖单元测试 / 功能测试 / 集成测试，并按测试目的进行规划。

## 1. 测试目标与分层

- **单元测试（Unit）**：不依赖或最小依赖硬件，验证接口输入、参数合法性、状态转换和工具函数（如 `vsf_spi_mode_to_data_bits()`）。
- **功能测试（Functional）**：在真实硬件上验证核心 SPI 行为，覆盖模式/位宽/时钟、片选、中断、DMA（若支持）等。
- **集成测试（Integration）**：跨模块协同，验证 SPI 与 DMA、Distbus、外设链路或多设备场景的系统级稳定性。
- **按目的规划**：为每一类测试标记“目的”标签，便于选择性运行，例如 `purpose=config`, `purpose=data-path`, `purpose=robustness`, `purpose=perf`。

## 2. 目录与命名约定

建议在 `test/vsf_test/spi/` 下组织：
```
unit/          # 纯 API & 工具函数验证（尽量脱离硬件）
functional/    # 需要 SPI 外设，验证传输、模式、CS、IRQ、DMA
integration/   # 跨模块/跨设备场景
test_spi_main.c
test_spi_common.h
```

命名规范（函数名与注册名保持一致、便于筛选）：
- `__test_spi_<area>_<scenario>()`
- 注册字符串：`"spi_<area>_<scenario> purpose=<tag> hw_req=<tag>"`
  示例：`"spi_config_invalid purpose=config hw_req=none"`

硬件标记建议：`none` / `spi` / `spi+loopback` / `spi+assist` / `spi+multi_dev` / `spi+dma`.

## 3. 规划参考（覆盖表）

| 目的           | 级别       | 重点 API/行为                              | 典型用例                                         | 硬件需求        |
|----------------|------------|--------------------------------------------|--------------------------------------------------|-----------------|
| `config`       | Unit       | `init/fini/get_configuration`              | 空指针/重复 init/非法模式/频率边界               | none            |
| `config`       | Functional | `capability` 与实测对比                    | 读取 `vsf_spi_capability` 与真实能力比对         | spi             |
| `data-path`    | Functional | `fifo_transfer/request_transfer/cancel`    | TX-only/RX-only/全双工，小包/大包，取消后计数    | spi+assist      |
| `mode`         | Functional | `mode`、位宽、CPOL/CPHA、LSB/MSB           | MODE0/1/2/3 回环，8/16/32 位，bit-order 切换      | spi+loopback    |
| `cs-timing`    | Functional | `cs_active/cs_inactive`                    | 多片选切换、不同 `auto_cs_index` 时序             | spi+multi_dev   |
| `irq`          | Functional | `irq_enable/irq_disable/irq_clear`         | 传输完成/错误中断，屏蔽后无唤醒，清除后恢复       | spi             |
| `dma`          | Integration| `fifo_transfer/request_transfer` + DMA     | 长包 DMA 传输、DMA 错误注入、重复启动             | spi+dma         |
| `robustness`   | Integration| 复位/超时/异常路径                         | 看门狗复位后恢复、错误码检查、硬件拔插（如有）    | spi             |
| `multi-device` | Integration| 多设备/多实例                              | 不同 `vsf_spi_t` 配置切换、不同 CS 同时验证       | spi+multi_dev   |

## 4. vsf_test 接入步骤

1. **创建测试文件**：按分层放置，如 `unit/test_spi_init.c`、`functional/test_spi_transfer.c`。
2. **引入公共头**：在每个文件包含 `test_spi_common.h`，其中集中放 `vsf_test_t *test`、默认配置、辅助设备钩子。
3. **编写用例函数**：满足签名 `static void __test_spi_xxx(void)`，使用 `VSF_TEST_ASSERT(vsf_test, condition)` 断言。
4. **注册用例**：在 `test_spi_main.c` 的用例表中添加
   ```c
   VSF_TEST_ADD(__test_spi_init_null_ptr,
                "spi_init_null_ptr purpose=config hw_req=none", 0),
   ```
   通过 `purpose`/`hw_req` 字段筛选运行。
5. **能力检查**：在功能/集成用例入口先读取 `vsf_spi_capability()`，不支持的特性使用 `VSF_TEST_INCONCLUSIVE`/跳过。
6. **资源清理**：每个用例结束前关闭 CS、禁用 IRQ/模块，确保下一个用例独立。

## 5. 用例模板

### 单元测试示例（配置合法性）
```c
static void __test_spi_init_invalid_mode(void)
{
    vsf_spi_t *spi = NULL;
    vsf_spi_cfg_t cfg = {
        .mode = (vsf_spi_mode_t)0xFFFFFFFF,   // 非法模式
    };
    vsf_err_t err = vsf_spi_init(spi, &cfg);
    VSF_TEST_ASSERT(vsf_test, err != VSF_ERR_NONE);
}
```

### 功能测试示例（基础回环）
```c
static void __test_spi_loopback_mode0_8bit(void)
{
    vsf_spi_t *spi = &vsf_spi[0];
    vsf_spi_cfg_t cfg = {
        .mode     = VSF_SPI_MODE_0 | VSF_SPI_DATASIZE_8,
        .clock_hz = 1000000,
    };
    VSF_TEST_ASSERT(vsf_test, VSF_ERR_NONE == vsf_spi_init(spi, &cfg));
    VSF_TEST_ASSERT(vsf_test, fsm_rt_cpl == vsf_spi_enable(spi));

    uint8_t tx[] = {0x11, 0x22, 0x33, 0x44};
    uint8_t rx[dimof(tx)] = {0};
    VSF_TEST_ASSERT(vsf_test, VSF_ERR_NONE == vsf_spi_request_transfer(spi, tx, rx, sizeof(tx)));
    VSF_TEST_ASSERT(vsf_test, 0 == memcmp(tx, rx, sizeof(tx)));
}
```

### 集成测试示例（SPI + DMA）
```c
static void __test_spi_dma_long_packet(void)
{
    // 前置：确认 vsf_spi_capability().is_dma 支持
    // 步骤：配置 DMA 通道 -> SPI DMA 传输 -> 校验计数与数据一致
    // 断言：传输完成，`vsf_spi_get_transferred_count` 返回长度一致。
}
```

## 6. 执行与筛选

- 构建：在对应目标上编译 `vsf_test`（参考板级 CMake 目标选择 `-DVSF_USE_TEST=ON` 或当前工程已有开关）。
- 运行：烧录后，`vsf_test` 会按注册顺序执行；通过 `purpose`/`hw_req` 字符串在辅助端选择子集运行。
- 报告：`vsf_test` 串口输出/辅助脚本收集结果，失败项包含文件、行号与断言描述。

## 7. 编写检查清单

- [ ] 用例按 `purpose`、`hw_req` 标签归类。
- [ ] 单元测试尽量脱离硬件；功能/集成用例显式声明所需连线。
- [ ] 覆盖 `vsf_spi_init/fini/enable/disable/capability/status/cs/irq/transfer/cancel/ctrl` 等主要 API。
- [ ] 处理异常路径：空指针、无效参数、超时/取消、硬件不支持的模式。
- [ ] 每个用例完成后释放资源，确保下个用例可重复。
