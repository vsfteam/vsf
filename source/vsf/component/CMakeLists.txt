# CMakeLists head

target_sources(vsf INTERFACE
    vsf_component.c
)

add_subdirectory(3rd-party)
add_subdirectory(av)
add_subdirectory(crypto)
add_subdirectory(debugger)
add_subdirectory(fs)
add_subdirectory(input)
add_subdirectory(mal)
add_subdirectory(scsi)
add_subdirectory(tcpip)
add_subdirectory(ui)
add_subdirectory(usb)
