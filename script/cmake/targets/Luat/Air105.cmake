set(VSF_HAL_CHIP_VENDOR     Luat)
set(VSF_HAL_CHIP_SERIES     Air105)
set(VSF_HAL_CHIP_NAME       Air105)

set(VSF_TARGET_DEFINITIONS
    "__Luat__"
    "__Air105__"

    ${VSF_TARGET_DEFINITIONS}
)

include(${VSF_CMAKE_ROOT}/targets/arm/__cortex_m4.cmake)

set(VSF_TARGET_PATH ${VSF_SRC_PATH}/hal/driver/${VSF_HAL_CHIP_VENDOR}/${VSF_HAL_CHIP_SERIES})
