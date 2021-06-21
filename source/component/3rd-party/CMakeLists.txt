# CMakeLists head

add_subdirectory_ifdef(VSF_USE_LVGL         lvgl)
add_subdirectory_ifdef(VSF_USE_FREETYPE     freetype)
add_subdirectory_ifdef(VSF_USE_LWIP         lwip)
