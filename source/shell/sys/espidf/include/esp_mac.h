#ifndef __ESP_MAC_H__
#define __ESP_MAC_H__

#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ESP_MAC_WIFI_STA,
    ESP_MAC_WIFI_SOFTAP,
    ESP_MAC_BT,
    ESP_MAC_ETH,
    ESP_MAC_IEEE802154,
    ESP_MAC_BASE,
} esp_mac_type_t;

esp_err_t esp_read_mac(uint8_t *mac, esp_mac_type_t type);
esp_err_t esp_derive_local_mac(uint8_t *local_mac, const uint8_t *universal_mac);
esp_err_t esp_efuse_mac_get_default(uint8_t *mac);
esp_err_t esp_efuse_mac_get_custom(uint8_t *mac);
esp_err_t esp_base_mac_addr_get(uint8_t *mac);
esp_err_t esp_base_mac_addr_set(const uint8_t *mac);

#ifdef __cplusplus
}
#endif

#endif
