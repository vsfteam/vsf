set(VSF_HAL_CHIP_VENDOR     AIC)
set(VSF_HAL_CHIP_SERIES     AIC8800)
set(VSF_HAL_CHIP_NAME       AIC8800M)
set(VSF_ARCH_SERIES         arm)
set(VSF_ARCH_NAME           cortex-m4)

set(VSF_TARGET_DEFINITIONS
    "__AIC__"
    "__AIC8800__"

#   hw related definitions for aic8800m required by sdk
    "CFG_HW_PLATFORM=2"
    "CFG_RF_MODE=0"
    "CFG_PMIC"
    "CFG_PMIC_VER_LITE"
    "CFG_PMIC_ITF_ASDMA"
    "CFG_WIFI_STACK"
    "CFG_RSSI_DATAPKT"
    "CFG_DBG"
    "CFG_IPERF"
    "CFG_WIFI_ROM_VER=2"
#   move lwip configurations to a right place
#    "LWIP_NO_STDINT_H=1"
#    "LWIP_SO_RCVBUF=1"
)

include(${VSF_CMAKE_ROOT}/targets/arm/__cortex_m4.cmake)
