# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    driver.c
    $<$<OR:
        $<COMPILE_LANG_AND_ID:C,IAR>,
        $<COMPILE_LANG_AND_ID:CXX,IAR>>
    :startup_stm32f103xe.s>
)
