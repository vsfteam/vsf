print('vsfvm demo started...\r\n');
libusb_on_evt(evt, libusb_dev dev) {
    if ((USB_EVT_ON_ARRIVED == evt) && (0 == dev.ifs())) {
        print('libusb_dev_on_arrived: ', dev);

        var desc = buffer_create(18);
        var result;
        dev.transfer(0, @result, 0x80, 0x06, 0x0100, 0x0000, 18, desc);
        print(desc, '\r\n');
    }
}
libusb_start(libusb_on_evt);

var heartbeat = 0;
while (1) {
    delay_ms(1000);
    print('heart beat ', heartbeat, '\r\n');
    heartbeat = heartbeat + 1;
}
