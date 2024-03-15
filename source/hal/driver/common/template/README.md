# VSF HAL Template

这份文档主要介绍 HAL 模板的设计思路和目前已知缺陷

## 文件介绍

`vsf_template.h` 是对外的头文件入口，由上层头文件 `hal/driver/common/common.h` 包含
`vsf_template_instance_declaration.h` 为具体外设生成实例的声明，用户通常不需要直接使用
`vsf_template_instance_implementation.h` 为具体外设生成实例的声明，用户通常不需要直接使用
`vsf_template_xxx.h` 主要是xxx外设的枚举/结构体定义和函数声明, 以及一些辅助宏。

## 已知问题

### 外设定义的枚举类型扩展困难

如果给某个外设的枚举添加新的定义，那么会影响所有现存的驱动。
例如 `vsf_adc_mode_t` 添加一个新的模式 `VSF_ADC_REF_VDD_1_5`，那么现有的所有 ADC 驱动都需要添加这个模式的处理。
目前的 workaround 办法仅仅给某个外设的驱动添加。

