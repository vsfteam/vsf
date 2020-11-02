# CMakeLists head

target_sources(vsf 
    INTERFACE driver.c
    INTERFACE startup_M484.c
    INTERFACE interrupt_swi.c
)
