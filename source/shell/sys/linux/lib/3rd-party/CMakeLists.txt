# CMakeLists head

add_subdirectory_ifdef(VSF_LINUX_USE_LIBTUV         libtuv)

add_subdirectory(getopt)
add_subdirectory(regex)
add_subdirectory(fnmatch)
