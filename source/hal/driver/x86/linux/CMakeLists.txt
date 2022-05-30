# CMakeLists head

target_sources(${VSF_HOST_SYSTEM_LIB_NAME} PUBLIC
    driver.c
)
add_subdirectory(rtc)
