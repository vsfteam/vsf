# HAL TEMPLATE GUIDE

Template in current directory can be used for:
1. Add a driver for peripherials of specific ${DEVICE} of a specific ${SERIES} from a specific ${VENDOR}.
These drivers are called HW drivers in VSF, codes between "HW" - "HW end" comments sections in the source code of the template are for these drivers.
It's recommended to use blackbox mode for peripherial drivers, which hide everything in peripherial header files(eg adc.h).
Here ${VENDOR}/${SERIES}/${DEVICE} will be referenced in the source code of the template, please replace them with the actual name.

2. Implement an IPCore driver ${XXX_IP} from ${VENDOR}.
codes between "IPCore" - "IPCore end" comments sections in the source code of the template are for these drivers.
IPCore drivers are actually provided to peripherial drivers, so information in IPCore drivers should be open.
Here ${VENDOR}/${XXX_IP} will be referenced in the source code of the template, please replace them with the actual name.

3. Implement a emulated driver as if it's an specified peripherial.
In this case, it's similar to IPCore case, so the source code of the template will take it as IPCore usage.
But ${VENDOR} is not used, ${XXX_IP} should be the name of the emulated peripherial driver.
Usage case example:
  1. gpio_i2c: gpio emulated i2c driver which has the same API as vsf_i2c_t driver.
  2. spi_flash: spi flash driver can provide 2 interfaces. One is mal which is block interface, another is vsf_flash_t driver interface.
  3. usbh_uart: usb host driver for CDCACM class, which will implement a UART interface.

本目录下的模板可用于：
1. 增加一个 ${VENDOR} 厂家的 ${SERIES} 系列忠的 ${DEVICE} 芯片的外设驱动。
这种具体芯片的外设驱动在 VSF 中称为 HW 驱动。在 "H#" 到 "HW end" 注释段之间的代码，就是用于这种驱动的。
实现外设驱动的时候，推荐使用黑盒模式，外设驱动的头文件不提供具体的信息。
${VENDOR}/${SERIES}/${DEVICE} 会出现在模板代码中，请替换为实际的名字。

2. 实现一个 ${VENDOR} 厂家的 ${XXX_IP} 的 IP 核驱动。
在 "IPCore" 到 "IPCore end" 注释段之间的代码，就是用于这种驱动的。
IP 核驱动实际上是提供给具体芯片外设驱动使用，所以相关的信息都需要对高层可见。
${VENDOR}/${XXX_IP} 会出现在模板代码中，请替换为实际的名字。

3. 实现一个模拟某种芯片外设接口的驱动。
这种情况下，和 IP 核驱动类似，模板代码将这种使用场景等同于 IP 核驱动的场景，${VENDOR} 不使用， ${XXX_IP} 为模拟驱动的名字。
使用用例：
  1. gpio_i2c: GPIO 模拟 I2C ，使用和硬件 I2C 一样的接口。
  2. spi_flash: spi flash 可以提供 2 种接口，一种是块设备接口，另一种是和硬件 flash 一样的接口。
  3. usbh_uart: CDCACM 类的 USB 主机驱动，实现一个 UART 的接口。
