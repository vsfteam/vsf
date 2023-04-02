# CMakeLists head

target_sources(${CMAKE_PROJECT_NAME} PUBLIC
    linux_demo.c
    libusb_demo.c
    mount_demo.c
    httpd_demo.c
    tty_demo.c
    elfloader_demo.c
    ntpdate_demo.c
    telnetd_demo.c
)
