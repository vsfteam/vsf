# CMakeLists head

target_sources(vsf 
    INTERFACE driver.c
)

add_subdirectory(
    Vendor
    $<$<OR: $<COMPILE_LANG_AND_ID:C,GNU>, 
            $<COMPILE_LANG_AND_ID:CXX,GNU>>
    : startup_gcc>
)
