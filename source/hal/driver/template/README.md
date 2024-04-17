# HAL TEMPLATE GUIDE

Template in current directory can be used for:
1. Add a driver for peripherals of DEVICE in a SERIES from VENDOR.

These drivers are called HW drivers in VSF, codes between "HW" - "HW end" comments sections in the source code of the template are for these drivers.
It's recommended to use blackbox mode for peripheral drivers, which hide everything in peripheral header files(eg adc.h), but REIMPLEMENT part(reimplement some structure/emun according to the register) MUST be open.
Here VENDOR/SERIES/DEVICE will be referenced in the source code of the template, please replace them with the actual name.

2. Implement an IPCore driver XXX_IP from VENDOR.

User of IPCore drivers is peripheral driver.
codes between "IPCore" - "IPCore end" comments sections in the source code of the template are for these drivers.
IPCore drivers are actually provided to peripheral drivers, so information in IPCore drivers should be open.
Here VENDOR/XXX_IP will be referenced in the source code of the template, please replace them with the actual name.

3. Implement a emulated driver as if it's an specified peripheral.

User of emulated drivers is application developer.
In this case, it's similar to IPCore case, so the source code of the template will take it as IPCore usage.
But VENDOR is not used, XXX_IP should be the name of the emulated peripheral driver.
Usage case example:
  - gpio_i2c: gpio emulated i2c driver which has the same API as vsf_i2c_t driver.
  - spi_flash: spi flash driver can provide 2 interfaces. One is mal which is block interface, another is vsf_flash_t driver interface.
  - usbh_uart: usb host driver for CDCACM class, which will implement a UART interface.

---

本目录下的模板可用于：

1. 增加一个 VENDOR 厂家的 SERIES 系列中的 DEVICE 芯片的外设驱动。

这种具体芯片的外设驱动在 VSF 中称为 HW 驱动。在 "HW" 到 "HW end" 注释段之间的代码，就是用于这种驱动的。
实现外设驱动的时候，推荐使用黑盒模式，外设驱动的头文件不提供具体的信息，但是需要提供 REIMPLEMENT 的内容，用于驱动的简化（相当于按照寄存器的未定义，重新实现一些数据结构或者枚举）。
VENDOR/SERIES/DEVICE 会出现在模板代码中，请替换为实际的名字。

2. 实现一个 VENDOR 厂家的 XXX_IP 的 IP 核驱动。

IP 核驱动的用户是芯片外设驱动，所以相关的信息都需要对用户可见。
在 "IPCore" 到 "IPCore end" 注释段之间的代码，就是用于这种驱动的。
VENDOR/XXX_IP 会出现在模板代码中，请替换为实际的名字。

3. 实现一个模拟某种芯片外设接口的驱动。

模拟驱动的用户是应用开发者，所以相关的信息都需要对用户可见，但是不能 REIMPLEMENT ，因为这种驱动是没有寄存器的。
这种情况下，和 IP 核驱动类似，模板代码将这种使用场景等同于 IP 核驱动的场景， VENDOR 不使用， XXX_IP 为模拟驱动的名字。
使用用例：
  - gpio_i2c: GPIO 模拟 I2C ，使用和硬件 I2C 一样的接口。
  - spi_flash: spi flash 可以提供 2 种接口，一种是块设备接口，另一种是和硬件 flash 一样的接口。
  - usbh_uart: CDCACM 类的 USB 主机驱动，实现一个 UART 的接口。
