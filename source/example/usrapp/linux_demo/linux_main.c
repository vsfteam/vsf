#include <unistd.h>
#include <stdio.h>

#if VSF_LINUX_USE_LIBUSB == ENABLED
#include <libusb.h>

int lsusb_main(int argc, char *argv[])
{
    bool verbose = (argc == 2) && !strcmp(argv[1], "-v");

    if (libusb_init(NULL) < 0) {
        printf("fail to initialize libusb\r\n");
        return -1;
    }

    libusb_device **devs;
    ssize_t cnt = libusb_get_device_list(NULL, &devs);
    if (cnt < 0) {
        printf("fail to get device list\r\n");
        libusb_exit(NULL);
        return -1;
    }
    if (!cnt) {
        printf("no usb device connected\r\n");
        libusb_exit(NULL);
        return -1;
    }

    struct libusb_device_descriptor desc;
    libusb_device_handle *handle = NULL;
    char description[260];
    char string[256];
    for (int i = 0; devs[i]; i++) {
        if (libusb_get_device_descriptor(devs[i], &desc) < 0) {
            printf("fail to get device descriptor\r\n");
            continue;
        }

        if (LIBUSB_SUCCESS == libusb_open(devs[i], &handle)) {
            if (desc.iManufacturer) {
                if (libusb_get_string_descriptor_ascii(handle, desc.iManufacturer, (unsigned char *)string, sizeof(string)) > 0) {
                    snprintf(description, sizeof(description), "%s - ", string);
                } else {
                    snprintf(description, sizeof(description), "%04X - ", desc.idVendor);
                }
            } else {
                snprintf(description, sizeof(description), "%04X - ", desc.idVendor);
            }

            if (desc.iProduct) {
                if (libusb_get_string_descriptor_ascii(handle, desc.iProduct, (unsigned char *)string, sizeof(string)) > 0) {
                    snprintf(description + strlen(description), sizeof(description) - strlen(description), "%s", string);
                } else {
                    snprintf(description + strlen(description), sizeof(description) - strlen(description), "%04X", desc.idProduct);
                }
            } else {
                snprintf(description + strlen(description), sizeof(description) - strlen(description), "%04X", desc.idProduct);
            }

            if (desc.iSerialNumber) {
                if (libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, (unsigned char *)string, sizeof(string)) > 0) {
                    snprintf(description + strlen(description), sizeof(description) - strlen(description), " - %s", string);
                }
            }

            printf("Dev (bus %d, device %d): %s\n", libusb_get_bus_number(devs[i]), libusb_get_device_address(devs[i]), description);
            if (!verbose) { continue; }

            struct libusb_config_descriptor *config_desc;
            struct libusb_interface_descriptor *interface_desc;
            struct libusb_endpoint_descriptor *endpoint_desc;
            struct libusb_interface *interface;
            for (int i = 0; i < desc.bNumConfigurations; i++) {
                if (LIBUSB_SUCCESS != libusb_get_config_descriptor(devs[i], i, &config_desc)) {
                    continue;
                }

                printf("  Configuration:\n");
                printf("    wTotalLength:         %d\n",    config_desc->wTotalLength);
                printf("    bNumInterfaces:       %d\n",    config_desc->bNumInterfaces);
                printf("    bConfigurationValue:  %d\n",    config_desc->bConfigurationValue);
                printf("    iConfiguration:       %d\n",    config_desc->iConfiguration);
                printf("    bmAttributes:         %02xh\n", config_desc->bmAttributes);
                printf("    bMaxPower:            %d\n",    config_desc->bMaxPower);

                for (int j = 0; j < config_desc->bNumInterfaces; j++) {
                    interface = (struct libusb_interface *)&config_desc->interface[j];
                    for (int k = 0; k < interface->num_altsetting; k++) {
                        interface_desc = (struct libusb_interface_descriptor *)&interface->altsetting[k];

                        printf("    Interface:\n");
                        printf("      bInterfaceNumber:   %d\n", interface_desc->bInterfaceNumber);
                        printf("      bAlternateSetting:  %d\n", interface_desc->bAlternateSetting);
                        printf("      bNumEndpoints:      %d\n", interface_desc->bNumEndpoints);
                        printf("      bInterfaceClass:    %d\n", interface_desc->bInterfaceClass);
                        printf("      bInterfaceSubClass: %d\n", interface_desc->bInterfaceSubClass);
                        printf("      bInterfaceProtocol: %d\n", interface_desc->bInterfaceProtocol);
                        printf("      iInterface:         %d\n", interface_desc->iInterface);

                        for (int l = 0; l < interface_desc->bNumEndpoints; l++) {
                            endpoint_desc = (struct libusb_endpoint_descriptor *)&interface_desc->endpoint[l];
                            printf("      Endpoint:\n");
                            printf("        bEndpointAddress: %02xh\n", endpoint_desc->bEndpointAddress);
                            printf("        bmAttributes:     %02xh\n", endpoint_desc->bmAttributes);
                            printf("        wMaxPacketSize:   %d\n",    endpoint_desc->wMaxPacketSize);
                            printf("        bInterval:        %d\n",    endpoint_desc->bInterval);
                            printf("        bRefresh:         %d\n",    endpoint_desc->bRefresh);
                            printf("        bSynchAddress:    %d\n",    endpoint_desc->bSynchAddress);
                        }
                    }
                }

                libusb_free_config_descriptor(config_desc);
            }

            libusb_close(handle);
        }
    }
    libusb_free_device_list(devs, 1);
    libusb_exit(NULL);
    return 0;
}
#endif
