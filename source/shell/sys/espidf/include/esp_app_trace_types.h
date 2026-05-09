#ifndef ESP_APP_TRACE_TYPES_H_
#define ESP_APP_TRACE_TYPES_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int64_t   start;
    int64_t   tmo;
    int64_t   elapsed;
} esp_apptrace_tmo_t;

typedef struct {
    uint8_t *data;
    volatile uint32_t size;
    volatile uint32_t cur_size;
    volatile uint32_t rd;
    volatile uint32_t wr;
} esp_apptrace_rb_t;

typedef enum {
    ESP_APPTRACE_DEST_JTAG,
    ESP_APPTRACE_DEST_UART,
} esp_apptrace_dest_t;

typedef enum {
    ESP_APPTRACE_HEADER_SIZE_16 = 2,
    ESP_APPTRACE_HEADER_SIZE_32 = 4,
} esp_apptrace_header_size_t;

typedef struct {
    int         uart_num;
    int         tx_pin_num;
    int         rx_pin_num;
    int         baud_rate;
    uint32_t    tx_buff_size;
    uint32_t    tx_msg_size;
} esp_apptrace_uart_config_t;

typedef struct {
    esp_apptrace_dest_t dest;
    union {
        esp_apptrace_uart_config_t uart;
        struct {
            uint8_t _unused;
        } jtag;
    } dest_cfg;
    uint32_t    flush_tmo;
    uint32_t    flush_thresh;
} esp_apptrace_config_t;

#ifdef __cplusplus
}
#endif

#endif
