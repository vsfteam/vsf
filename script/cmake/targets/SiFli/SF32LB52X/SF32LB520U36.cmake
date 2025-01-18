set(VSF_HAL_CHIP_VENDOR     SiFli)
set(VSF_HAL_CHIP_SERIES     SF32LB52X)
set(VSF_HAL_CHIP_NAME       SF32LB520U36)

set(VSF_TARGET_DEFINITIONS
    "__SiFli__"
    "__SF32LB520U36__"

    ${VSF_TARGET_DEFINITIONS}
)

include(${VSF_CMAKE_ROOT}/targets/arm/__cortex_m33.cmake)

# TODO: add vendor source/include directory/macro etc
#set(VSF_TARGET_PATH ${VSF_SRC_PATH}/hal/driver/${VSF_HAL_CHIP_VENDOR}/${VSF_HAL_CHIP_SERIES})
#list(APPEND VSF_TARGET_INCLUDE_DIRECTORIES
#    ${VSF_TARGET_PATH}/port
#)