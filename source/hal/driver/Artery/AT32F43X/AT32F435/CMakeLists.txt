# CMakeLists head

target_sources(vsf 
    INTERFACE driver.c
    INTERFACE startup_APM32F407.c
    INTERFACE interrupt_swi.c
)
