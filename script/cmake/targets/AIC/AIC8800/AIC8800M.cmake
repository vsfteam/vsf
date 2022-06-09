set(VSF_HAL_CHIP_VENDOR     AIC)
set(VSF_HAL_CHIP_SERIES     AIC8800)
set(VSF_HAL_CHIP_NAME       AIC8800M)

set(VSF_TARGET_DEFINITIONS
    "__AIC__"
    "__AIC8800__"

    ${VSF_TARGET_DEFINITIONS}
)

include(${VSF_CMAKE_ROOT}/targets/arm/__cortex_m4.cmake)

set(VSF_TARGET_PATH ${VSF_SRC_PATH}/hal/driver/${VSF_HAL_CHIP_VENDOR}/${VSF_HAL_CHIP_SERIES})
list(APPEND VSF_TARGET_INCLUDE_DIRECTORIES
    ${VSF_TARGET_PATH}/port
    ${VSF_TARGET_PATH}/port/arch/ll
    ${VSF_TARGET_PATH}/port/rtos

    ${VSF_TARGET_PATH}/vendor/plf/aic8800/src/arch
    ${VSF_TARGET_PATH}/vendor/plf/aic8800/src/arch/boot
    ${VSF_TARGET_PATH}/vendor/plf/aic8800/src/arch/cmsis

    ${VSF_TARGET_PATH}/vendor/plf/aic8800/src/driver
    ${VSF_TARGET_PATH}/vendor/plf/aic8800/src/driver/common
    ${VSF_TARGET_PATH}/vendor/plf/aic8800/src/driver/reg
    ${VSF_TARGET_PATH}/vendor/plf/aic8800/src/driver/ipc
    ${VSF_TARGET_PATH}/vendor/plf/aic8800/src/driver/stdio_uart
    ${VSF_TARGET_PATH}/vendor/plf/aic8800/src/driver/iomux
    ${VSF_TARGET_PATH}/vendor/plf/aic8800/src/driver/dma
    ${VSF_TARGET_PATH}/vendor/plf/aic8800/src/driver/ticker
    ${VSF_TARGET_PATH}/vendor/plf/aic8800/src/driver/flash
    ${VSF_TARGET_PATH}/vendor/plf/aic8800/src/driver/psram
    ${VSF_TARGET_PATH}/vendor/plf/aic8800/src/driver/pmic
    ${VSF_TARGET_PATH}/vendor/plf/aic8800/src/driver/sleep
    ${VSF_TARGET_PATH}/vendor/plf/aic8800/src/driver/aic1000lite_regs
    ${VSF_TARGET_PATH}/vendor/plf/aic8800/src/driver/sysctrl
    ${VSF_TARGET_PATH}/vendor/plf/aic8800/src/driver/asdma_mst
    ${VSF_TARGET_PATH}/vendor/plf/aic8800/src/driver/trng
    ${VSF_TARGET_PATH}/vendor/plf/aic8800/src/driver/time
    ${VSF_TARGET_PATH}/vendor/plf/aic8800/src/driver/bt
    ${VSF_TARGET_PATH}/vendor/plf/aic8800/src/driver/bt/patch/mcu

    ${VSF_TARGET_PATH}/vendor/modules/common/api
    ${VSF_TARGET_PATH}/vendor/modules/dbg/api
)