# CMakeLists head

vsf_add_sources(
    driver.c
    $<$<OR:
        $<COMPILE_LANG_AND_ID:C,IAR>,
        $<COMPILE_LANG_AND_ID:CXX,IAR>>
    :startup_stm32f103xe.s>
)
