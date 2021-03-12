# CMakeLists head

target_sources( vsf
    INTERFACE gd32vf103_adc.c
    INTERFACE gd32vf103_bkp.c
    INTERFACE gd32vf103_can.c
    INTERFACE gd32vf103_crc.c
    INTERFACE gd32vf103_dac.c
    INTERFACE gd32vf103_dbg.c
    INTERFACE gd32vf103_dma.c
    INTERFACE gd32vf103_eclic.c
    INTERFACE gd32vf103_exmc.c
    INTERFACE gd32vf103_fmc.c
    INTERFACE gd32vf103_fwdgt.c
    INTERFACE gd32vf103_gpio.c
    INTERFACE gd32vf103_i2c.c
    INTERFACE gd32vf103_pmu.c
    INTERFACE gd32vf103_rcu.c
    INTERFACE gd32vf103_rtc.c
    INTERFACE gd32vf103_spi.c
    INTERFACE gd32vf103_timer.c
    INTERFACE gd32vf103_usart.c
    INTERFACE gd32vf103_wwdgt.c
)
