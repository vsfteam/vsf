# CMakeLists head

target_sources(${CMAKE_PROJECT_NAME} PUBLIC
    usbd_demo_cdc.c
    usbd_demo_msc.c
    usbd_demo_uac.c
    usbd_demo_user.c
    usbd_demo_uvc.c
)