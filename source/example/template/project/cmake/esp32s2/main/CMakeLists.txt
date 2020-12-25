set(vsf_path "../../../../../../vsf/")
set(vsf_template_path "../../../../")
set(vsf_template_srcs
    "${vsf_template_path}demo/common/usrapp_common.c"
    "${vsf_template_path}demo/common/usrapp_usbd_common.c"
    "${vsf_template_path}demo/common/usrapp_usbh_common.c"
    "${vsf_template_path}demo/linux_demo/linux_demo.c"
    "${vsf_template_path}demo/linux_demo/libusb_demo.c"
    "${vsf_template_path}demo/scsi_demo/scsi_demo.c"
    "${vsf_template_path}demo/btstack_demo/btstack_demo.c"
    "${vsf_template_path}demo/usbh_demo/usbh_demo.c"
    "${vsf_template_path}demo/usbd_demo/usbd_demo_cdc.c"
    "${vsf_template_path}demo/usbd_demo/usbd_demo_msc.c"
    "${vsf_template_path}demo/usbd_demo/usbd_demo_uac.c"
    "${vsf_template_path}demo/fakefat32.c"

# 3rd-party: btstack port
    "${vsf_path}component/3rd-party/btstack/port/btstack_run_loop_vsf.c"
    "${vsf_path}component/3rd-party/btstack/port/hci_dump_vsf.c"
    "${vsf_path}component/3rd-party/btstack/port/hci_transport_h2_vsf.c"
# 3rd-party: btstack firmware
    "${vsf_path}component/3rd-party/btstack/firmware/bcm/bcm20702/bt_firmware_image.c"
# 3rd-party: btstack chipset
    "${vsf_path}component/3rd-party/btstack/raw/chipset/bcm/btstack_chipset_bcm.c"
    "${vsf_path}component/3rd-party/btstack/raw/chipset/bcm/btstack_chipset_bcm_download_firmware.c"
    "${vsf_path}component/3rd-party/btstack/raw/chipset/csr/btstack_chipset_csr.c"
# 3rd-party: btstack
    "${vsf_path}component/3rd-party/btstack/raw/src/classic/rfcomm.c"
    "${vsf_path}component/3rd-party/btstack/raw/src/classic/sdp_server.c"
    "${vsf_path}component/3rd-party/btstack/raw/src/classic/sdp_util.c"
    "${vsf_path}component/3rd-party/btstack/raw/src/classic/spp_server.c"
    "${vsf_path}component/3rd-party/btstack/raw/src/ad_parser.c"
    "${vsf_path}component/3rd-party/btstack/raw/src/btstack_linked_list.c"
    "${vsf_path}component/3rd-party/btstack/raw/src/btstack_memory.c"
    "${vsf_path}component/3rd-party/btstack/raw/src/btstack_memory_pool.c"
    "${vsf_path}component/3rd-party/btstack/raw/src/btstack_run_loop.c"
    "${vsf_path}component/3rd-party/btstack/raw/src/btstack_util.c"
    "${vsf_path}component/3rd-party/btstack/raw/src/hci.c"
    "${vsf_path}component/3rd-party/btstack/raw/src/hci_cmd.c"
    "${vsf_path}component/3rd-party/btstack/raw/src/l2cap.c"
    "${vsf_path}component/3rd-party/btstack/raw/src/l2cap_signaling.c"
)
set(vsf_template_include_dirs
    "${vsf_template_path}config"

# 3rd-party: btstack
    "${vsf_path}component/3rd-party/btstack/raw/src"
    "${vsf_path}component/3rd-party/btstack/raw/chipset"
)

idf_component_register(
    SRCS "usr_dbg_stream.c" "app_main.c" ${vsf_template_srcs}
    INCLUDE_DIRS ${vsf_template_include_dirs})

idf_component_set_property(main ORIG_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
