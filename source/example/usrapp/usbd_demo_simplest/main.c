#include "vsf.h"

describe_usbd(user_usbd, 0x2348, 0xA7A8, USB_DC_SPEED_HIGH)
    usbd_common_desc(user_usbd, u"VSF-USBD-Simplest", u"SimonQian", u"1.0.0", 64, USB_DESC_CDC_ACM_IAD_LEN, USB_CDC_ACM_IFS_NUM, USB_CONFIG_ATT_WAKEUP, 100)
        cdc_acm_desc(user_usbd, 0, 0, 1, 2, 2, 512, 16)
    usbd_func_desc(user_usbd)
        usbd_func_str_desc(user_usbd, 0, u"VSF-CDC")
    usbd_std_desc_table(user_usbd)
        usbd_func_str_desc_table(user_usbd, 0)
    usbd_func(user_usbd)
        cdc_acm_func(user_usbd, 0, 1, 2, 2, NULL, NULL, USB_CDC_ACM_LINECODE(115200, 8, USB_CDC_ACM_PARITY_NONE, USB_CDC_ACM_STOPBIT_1))
    usbd_ifs(user_usbd)
        cdc_acm_ifs(user_usbd, 0)
end_describe_usbd(user_usbd, VSF_USB_DC0)

void main(void)
{
    vk_usbd_init(&user_usbd);
    vk_usbd_connect(&user_usbd);
}
