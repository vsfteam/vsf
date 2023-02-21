# CMakeLists head

vsf_add_sources(
    driver.c
)

add_subdirectory(rng)
add_subdirectory(rtc)
add_subdirectory(uart)