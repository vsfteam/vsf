/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software       *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

/*============================ INCLUDES ======================================*/

#include "./vsf_wifi_mt76.h"

#if VSF_USE_WIFI == ENABLED && VSF_WIFI_USE_MT76 == ENABLED

#include "../../vsf_wifi_priv.h"

/*============================ MACROS ========================================*/

#define MT76_VEND_TYPE_CFG                  ((uint32_t)1 << 30)

#define MT76_ASIC_VERSION                   0x0000
#define MT76_USB_U3DMA_CFG                  (MT76_VEND_TYPE_CFG | 0x9018)
#define MT76_WPDMA_GLO_CFG                  0x0208
#define MT76_MAC_SYS_CTRL                   0x1004
#define MT76_RX_FILTR_CFG                   0x1400

#define MT76_FCE_DMA_ADDR                   0x0230
#define MT76_FCE_DMA_LEN                    0x0234
#define MT76_TX_CPU_FROM_FCE_CPU_DESC_IDX   0x09A8
#define MT76_TX_CPU_FROM_FCE_BASE_PTR       0x09A0
#define MT76_TX_CPU_FROM_FCE_MAX_COUNT      0x09A4
#define MT76_FCE_PSE_CTRL                   0x0800
#define MT76_FCE_PDMA_GLOBAL_CONF           0x09C4
#define MT76_FCE_SKIP_FS                    0x0A6C

#define MT76_MCU_COM_REG0                   0x0730
#define MT76_MCU_CLOCK_CTL                  0x0708
#define MT76_MCU_RESET_CTL                  0x070C
#define MT76_MCU_SEMAPHORE_03               0x07BC

#define MT76_REV_E3                         0x22

#define MT76_CHIP_ID_7612                   0x7612
#define MT76_CHIP_ID_7632                   0x7632
#define MT76_CHIP_ID_7662                   0x7662
#define MT76_CHIP_ID_7602                   0x7602

#define MT76_PATCH_HDR_SIZE                 30
#define MT76_FW_HDR_SIZE                    32
#define MT76_ROM_PATCH_OFFSET               0x00090000
#define MT76_ILM_OFFSET                     0x00080000
#define MT76_DLM_OFFSET                     0x00110000
#define MT76_DLM_OFFSET_E3_EXTRA            0x00000800

#define MT76_USB_DMA_CFG_VAL                0x00C00020

#define MT76_WLAN_FUN_CTRL                  0x0080
#define MT76_WLAN_FUN_CTRL_WLAN_EN          (1U << 0)
#define MT76_WLAN_FUN_CTRL_WLAN_CLK_EN      (1U << 1)
#define MT76_WLAN_FUN_CTRL_WLAN_RESET_RF    (1U << 2)
#define MT76_WLAN_FUN_CTRL_FRC_WL_ANT_SEL   (1U << 5)

#define MT76_WLAN_MTC_CTRL                  (0x40000000 | 0x148)
#define MT76_WLAN_MTC_CTRL_MTCMOS_PWR_UP    (1U << 0)
#define MT76_WLAN_MTC_CTRL_PWR_ACK          (1U << 12)
#define MT76_WLAN_MTC_CTRL_PWR_ACK_S        (1U << 13)
#define MT76_WLAN_MTC_CTRL_STATE_UP         (1U << 28)

#define MT76_MAC_CSR0                       0x1000

#define MT76_MCU_MSG_TYPE_CMD               ((uint32_t)1 << 30)
#define MT76_MCU_MSG_CMD_TYPE_SHIFT         20
#define MT76_MCU_MSG_CMD_SEQ_SHIFT          16
#define MT76_MCU_MSG_PORT_SHIFT             27
#define MT76_MCU_MSG_LEN_MASK               0xFFFF

#define MT76_CPU_TX_PORT                    2
#define MT76_WLAN_PORT                      0

#define MT76_VEND_DEV_MODE                  0x01

#define MT76_FW_PATCH_CHUNK_PAYLOAD         (2048 - 8)
#define MT76_FW_FW_CHUNK_PAYLOAD            (MT76_TX_URB_SIZE - 8)

#define MT76_CMD_FUN_SET_OP                 1
#define MT76_CMD_POWER_SAVING_OP            20
#define MT76_FUN_Q_SELECT                   1
#define MT76_PWR_RADIO_ON                   0x31
#define MT76_PWR_RADIO_OFF                  0x30

#define MT_EE_READ                          0
#define MT76_EE_MAC_ADDR                    0x004

#define __mt76_put_le32(_p, _v)             do { \
        (_p)[0] = (uint8_t)(_v);                \
        (_p)[1] = (uint8_t)((_v) >> 8);         \
        (_p)[2] = (uint8_t)((_v) >> 16);        \
        (_p)[3] = (uint8_t)((_v) >> 24);        \
    } while (0)
#define __mt76_get_le32(_p)                 \
    (   (uint32_t)(_p)[0]                   \
      | ((uint32_t)(_p)[1] << 8)            \
      | ((uint32_t)(_p)[2] << 16)           \
      | ((uint32_t)(_p)[3] << 24))
#define __mt76_get_le16(_p)                 \
    (   (uint16_t)(_p)[0]                   \
      | ((uint16_t)(_p)[1] << 8))
#define __mt76_round_up(_x, _n)             (((_x) + ((_n) - 1)) & ~((_n) - 1))
#define __mt76_min(_a, _b)                  ((_a) < (_b) ? (_a) : (_b))
#define __mt76_rev(_priv)                   ((uint16_t)((_priv)->asic_rev & 0xFFFF))

/*============================ TYPES =========================================*/

typedef enum {
    MT76_STATE_IDLE = 0,
    MT76_STATE_INIT_READ_ASIC,
    MT76_STATE_INIT_MCU_Q_SELECT,
    MT76_STATE_INIT_MCU_RADIO_ON,
    MT76_STATE_INIT_EEPROM_LOAD,
    MT76_STATE_INIT_MAC_ADDR,
    MT76_STATE_INIT_READY,
} mt76_state_t;

typedef enum {
    MT76_FW_STATE_RESET_WLAN = 0,
    MT76_FW_STATE_RESET_WLAN_RF,
    MT76_FW_STATE_RESET_WLAN_EN,
    MT76_FW_STATE_POWER_ON_MTCMOS,
    MT76_FW_STATE_POWER_ON_MTCMOS_POLL,
    MT76_FW_STATE_POWER_ON_MTCMOS_CLEAR1,
    MT76_FW_STATE_POWER_ON_MTCMOS_CLEAR2,
    MT76_FW_STATE_POWER_ON_MTCMOS_SET2,
    MT76_FW_STATE_POWER_ON_MTCMOS_CLEAR3,
    MT76_FW_STATE_POWER_ON_AD_DA,
    MT76_FW_STATE_POWER_ON_AD_DA_WRITE,
    MT76_FW_STATE_POWER_ON_WLAN_EN,
    MT76_FW_STATE_POWER_ON_WLAN_EN_WRITE,
    MT76_FW_STATE_POWER_ON_BBP_RST,
    MT76_FW_STATE_POWER_ON_BBP_RST_WRITE,
    MT76_FW_STATE_POWER_ON_RF,
    MT76_FW_STATE_WAIT_MAC,
    MT76_FW_STATE_READ_ASIC_REV,
    MT76_FW_STATE_CHECK_PATCH,
    MT76_FW_STATE_ACQUIRE_SEM,
    MT76_FW_STATE_ENABLE_USB_DMA,
    MT76_FW_STATE_FCE_PSE,
    MT76_FW_STATE_FCE_BASE,
    MT76_FW_STATE_FCE_MAX,
    MT76_FW_STATE_FCE_PDMA,
    MT76_FW_STATE_FCE_SKIP,
    MT76_FW_STATE_UPLOAD,
    MT76_FW_STATE_ENABLE_PATCH,
    MT76_FW_STATE_RESET_WMT,
    MT76_FW_STATE_WAIT_WMT,
    MT76_FW_STATE_POLL_PATCH,
    MT76_FW_STATE_RELEASE_SEM,
    MT76_FW_STATE_PARSE_FW_HEADER,
    MT76_FW_STATE_LOAD_IVB,
    MT76_FW_STATE_POLL_FW_READY,
    MT76_FW_STATE_SET_COMREG,
    MT76_FW_STATE_REENABLE_FCE,
    MT76_FW_STATE_DONE,
} mt76_fw_state_t;

typedef enum {
    MT76_FW_STAGE_PATCH = 0,
    MT76_FW_STAGE_ILM,
    MT76_FW_STAGE_DLM,
} mt76_fw_stage_t;

typedef enum {
    MT76_FW_SEND_IDLE = 0,
    MT76_FW_SEND_CHUNK,
    MT76_FW_SEND_FCE_LEN,
    MT76_FW_SEND_BULK,
    MT76_FW_SEND_IDX_READ,
    MT76_FW_SEND_IDX_WRITE,
} mt76_fw_send_state_t;

/*============================ LOCAL VARIABLES ===============================*/

/*============================ FORWARD DECLARATIONS ==========================*/

static mt76_wifi_priv_t *__mt76_priv(vsf_wifi_t *wifi);
static const vsf_wifi_mt76_bus_ops_t *__mt76_bus_ops(vsf_wifi_t *wifi);

static void __mt76_init_next(vsf_wifi_t *wifi, vsf_err_t err);

static void __mt76_fw_next(vsf_wifi_t *wifi, vsf_err_t err);
static void __mt76_fw_finish(vsf_wifi_t *wifi, vsf_err_t err);
static void __mt76_fw_reset_wlan_done(vsf_wifi_t *wifi, vsf_err_t err);
static void __mt76_fw_power_on_done(vsf_wifi_t *wifi, vsf_err_t err);
static void __mt76_fw_wait_mac_done(vsf_wifi_t *wifi, vsf_err_t err);
static void __mt76_fw_power_on_rf_continue(vsf_wifi_t *wifi, vsf_err_t err);
static void __mt76_fw_power_on_rf_step(vsf_wifi_t *wifi);
static void __mt76_fw_read_asic_done(vsf_wifi_t *wifi, vsf_err_t err);
static void __mt76_fw_check_patch_done(vsf_wifi_t *wifi, vsf_err_t err);
static void __mt76_fw_acquire_sem_start(vsf_wifi_t *wifi);
static void __mt76_fw_acquire_sem_done(vsf_wifi_t *wifi, vsf_err_t err);
static void __mt76_fw_release_sem(vsf_wifi_t *wifi);
static void __mt76_fw_send_start(vsf_wifi_t *wifi);
static void __mt76_fw_send_chunk(vsf_wifi_t *wifi);
static void __mt76_fw_send_fce_addr_done(vsf_wifi_t *wifi, vsf_err_t err);
static void __mt76_fw_send_fce_len_done(vsf_wifi_t *wifi, vsf_err_t err);
static void __mt76_fw_send_bulk_done(vsf_wifi_t *wifi, vsf_err_t err);
static void __mt76_fw_send_idx_read_done(vsf_wifi_t *wifi, vsf_err_t err);
static void __mt76_fw_send_idx_write_done(vsf_wifi_t *wifi, vsf_err_t err);
static void __mt76_fw_poll_patch_start(vsf_wifi_t *wifi);
static void __mt76_fw_poll_patch_done(vsf_wifi_t *wifi, vsf_err_t err);
static void __mt76_fw_poll_fw_ready_start(vsf_wifi_t *wifi);
static void __mt76_fw_poll_fw_ready_done(vsf_wifi_t *wifi, vsf_err_t err);

#if VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER == ENABLED
static void __mt76_fw_timer(vsf_callback_timer_t *timer);
static void __mt76_fw_timer_start(vsf_wifi_t *wifi, uint32_t ms);
#endif

static vsf_err_t __mt76_mcu_msg_send(vsf_wifi_t *wifi, uint8_t cmd,
    const uint8_t *payload, uint16_t payload_len, vsf_wifi_done_t done);
static void __mt76_on_rx(vsf_wifi_t *wifi, uint8_t *buf, uint16_t len);

/*============================ PROTOTYPES ====================================*/

extern vsf_err_t __mt76_firmware_load(vsf_wifi_t *wifi, vsf_wifi_done_t done);
extern vsf_err_t __mt76_init(vsf_wifi_t *wifi, vsf_wifi_done_t done);
extern void __mt76_fini(vsf_wifi_t *wifi);
extern vsf_err_t __mt76_eeprom_load(vsf_wifi_t *wifi, vsf_wifi_done_t done);
extern const uint8_t *__mt76_eeprom_get_mac(vsf_wifi_t *wifi);
extern vsf_err_t __mt76_set_channel(vsf_wifi_t *wifi, uint8_t channel,
                                    vsf_wifi_done_t done);
extern vsf_err_t __mt76_set_rx_filter(vsf_wifi_t *wifi, uint32_t mask,
                                      vsf_wifi_done_t done);
extern vsf_err_t __mt76_set_mac_addr(vsf_wifi_t *wifi, const uint8_t mac[6],
                                     vsf_wifi_done_t done);
extern vsf_err_t __mt76_set_bssid(vsf_wifi_t *wifi, const uint8_t bssid[6],
                                  vsf_wifi_done_t done);
extern vsf_err_t __mt76_set_auth_mode(vsf_wifi_t *wifi,
                                      const vsf_wifi_auth_cfg_t *cfg,
                                      vsf_wifi_done_t done);
extern vsf_err_t __mt76_connect(vsf_wifi_t *wifi,
                                const uint8_t bssid[6], const uint8_t *ssid,
                                uint8_t ssid_len, uint8_t channel,
                                vsf_wifi_done_t done);
extern vsf_err_t __mt76_disconnect(vsf_wifi_t *wifi, vsf_wifi_done_t done);
extern vsf_err_t __mt76_get_link_info(vsf_wifi_t *wifi,
                                      vsf_wifi_link_info_t *info);
extern void __mt76_parse_rx(vsf_wifi_t *wifi, uint8_t *frame, uint16_t len);
extern vsf_err_t __mt76_tx(vsf_wifi_t *wifi, const uint8_t *frame, uint16_t len);

/*============================ IMPLEMENTATION ================================*/

static mt76_wifi_priv_t *__mt76_priv(vsf_wifi_t *wifi)
{
    return (mt76_wifi_priv_t *)wifi->chip_priv;
}

static bool __mt76_chip_id_valid(uint32_t asic_rev);

static const vsf_wifi_mt76_bus_ops_t *__mt76_bus_ops(vsf_wifi_t *wifi)
{
    VSF_WIFI_ASSERT(wifi->bus_ops != NULL);
    return (const vsf_wifi_mt76_bus_ops_t *)wifi->bus_ops;
}

#define __mt76_cfg_read(__wifi, ...)      __mt76_bus_ops(__wifi)->cfg_read(__wifi, __VA_ARGS__)
#define __mt76_cfg_write(__wifi, ...)     __mt76_bus_ops(__wifi)->cfg_write(__wifi, __VA_ARGS__)
#define __mt76_fce_write(__wifi, ...)     __mt76_bus_ops(__wifi)->fce_write(__wifi, __VA_ARGS__)
#define __mt76_dev_cmd(__wifi, ...)       __mt76_bus_ops(__wifi)->dev_cmd(__wifi, __VA_ARGS__)
#define __mt76_dev_class_cmd(__wifi, ...) __mt76_bus_ops(__wifi)->dev_class_cmd(__wifi, __VA_ARGS__)
#define __mt76_mcu_cmd(__wifi, ...)       __mt76_bus_ops(__wifi)->mcu_cmd(__wifi, __VA_ARGS__)
#define __mt76_tx_frame(__wifi, ...)      __mt76_bus_ops(__wifi)->tx_frame(__wifi, __VA_ARGS__)
#define __mt76_rx_submit(__wifi, ...)     __mt76_bus_ops(__wifi)->rx_submit(__wifi, __VA_ARGS__)

/*============================ MCU helpers ===================================*/

static vsf_err_t __mt76_mcu_msg_send(vsf_wifi_t *wifi, uint8_t cmd,
    const uint8_t *payload, uint16_t payload_len, vsf_wifi_done_t done)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    uint16_t padded = (payload_len + 3) & ~3;
    uint16_t total  = 4 + padded + 4;

    if (total > sizeof(priv->tx_buf)) {
        return VSF_ERR_NOT_ENOUGH_RESOURCES;
    }

    uint8_t seq = ++priv->mcu_seq;
    if (seq == 0) {
        seq = ++priv->mcu_seq;
    }
    seq &= 0x0F;

    uint32_t info = MT76_MCU_MSG_TYPE_CMD
                  | (((uint32_t)cmd & 0x7F) << MT76_MCU_MSG_CMD_TYPE_SHIFT)
                  | (((uint32_t)seq & 0x0F) << MT76_MCU_MSG_CMD_SEQ_SHIFT)
                  | (((uint32_t)MT76_CPU_TX_PORT & 0x07) << MT76_MCU_MSG_PORT_SHIFT)
                  | ((uint32_t)payload_len & MT76_MCU_MSG_LEN_MASK);

    uint8_t *buf = priv->tx_buf;
    __mt76_put_le32(buf, info);
    if (payload_len > 0) {
        memcpy(buf + 4, payload, payload_len);
    }
    memset(buf + 4 + payload_len, 0, (padded - payload_len) + 4);

    return __mt76_mcu_cmd(wifi, priv->tx_buf, total, done);
}

/*============================ Firmware load =================================*/

#if VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER == ENABLED
static void __mt76_fw_timer(vsf_callback_timer_t *timer)
{
    mt76_wifi_priv_t *priv = vsf_container_of(timer, mt76_wifi_priv_t, fw_timer);
    __mt76_fw_next(priv->wifi, VSF_ERR_NONE);
}

static void __mt76_fw_timer_start(vsf_wifi_t *wifi, uint32_t ms)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    vsf_callback_timer_init(&priv->fw_timer);
    priv->fw_timer.on_timer = __mt76_fw_timer;
    vsf_callback_timer_add_ms(&priv->fw_timer, ms);
}
#else
static void __mt76_fw_timer_start(vsf_wifi_t *wifi, uint32_t ms)
{
    (void)wifi; (void)ms;
}
#endif

static void __mt76_fw_finish(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    vsf_wifi_done_t done = priv->pending_done;

    priv->pending_done = NULL;
    priv->fw_state     = MT76_FW_STATE_DONE;
    priv->fw_send_state= MT76_FW_SEND_IDLE;

    if (err != VSF_ERR_NONE) {
        vsf_wifi_chip_mt76_trace_error(
            "mt76: firmware load failed (%d)" VSF_TRACE_CFG_LINEEND, err);
    } else {
        vsf_wifi_chip_mt76_trace_info(
            "mt76: firmware load completed" VSF_TRACE_CFG_LINEEND);
    }

    if (done != NULL) {
        done(wifi, err);
    }
}

static bool __mt76_chip_id_valid(uint32_t asic_rev)
{
    uint16_t chip_id = (uint16_t)(asic_rev >> 16);
    return (chip_id == MT76_CHIP_ID_7612) ||
           (chip_id == MT76_CHIP_ID_7632) ||
           (chip_id == MT76_CHIP_ID_7662) ||
           (chip_id == MT76_CHIP_ID_7602);
}

static void __mt76_fw_reset_wlan_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);

    if (err != VSF_ERR_NONE) {
        __mt76_fw_finish(wifi, err);
        return;
    }

    uint32_t val = priv->fw_idx;
    val &= ~MT76_WLAN_FUN_CTRL_FRC_WL_ANT_SEL;

    if (val & MT76_WLAN_FUN_CTRL_WLAN_EN) {
        val |= MT76_WLAN_FUN_CTRL_WLAN_RESET_RF;
        priv->fw_state = MT76_FW_STATE_RESET_WLAN_RF;
        err = __mt76_cfg_write(wifi, MT76_WLAN_FUN_CTRL, val,
                               __mt76_fw_next);
    } else {
        priv->fw_state = MT76_FW_STATE_RESET_WLAN_EN;
        err = __mt76_cfg_write(wifi, MT76_WLAN_FUN_CTRL, val,
                               __mt76_fw_next);
    }

    if (err != VSF_ERR_NONE) {
        __mt76_fw_finish(wifi, err);
    }
}

static void __mt76_fw_power_on_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);

    if (err != VSF_ERR_NONE) {
        __mt76_fw_finish(wifi, err);
        return;
    }

    uint32_t val = priv->fw_idx;
    uint32_t expected = MT76_WLAN_MTC_CTRL_MTCMOS_PWR_UP |
                        MT76_WLAN_MTC_CTRL_PWR_ACK |
                        MT76_WLAN_MTC_CTRL_PWR_ACK_S |
                        MT76_WLAN_MTC_CTRL_STATE_UP;

    if ((val & expected) == expected) {
        priv->fw_state = MT76_FW_STATE_POWER_ON_MTCMOS_CLEAR1;
        __mt76_fw_next(wifi, VSF_ERR_NONE);
    } else {
        /* Retry after 1 ms. */
        __mt76_fw_timer_start(wifi, 1);
    }
}

static void __mt76_fw_wait_mac_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);

    if (err != VSF_ERR_NONE) {
        __mt76_fw_finish(wifi, err);
        return;
    }

    uint32_t val = priv->fw_idx;
    if (val != 0 && val != 0xFFFFFFFF) {
        priv->fw_state = MT76_FW_STATE_READ_ASIC_REV;
        __mt76_fw_next(wifi, VSF_ERR_NONE);
    } else {
        if (priv->fw_poll_ms == 0) {
            vsf_wifi_chip_mt76_trace_error(
                "mt76: MAC did not become ready" VSF_TRACE_CFG_LINEEND);
            __mt76_fw_finish(wifi, VSF_ERR_FAIL);
            return;
        }
        priv->fw_poll_ms--;
        __mt76_fw_timer_start(wifi, 5);
    }
}

static void __mt76_fw_power_on_rf_continue(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (err != VSF_ERR_NONE) {
        __mt76_fw_finish(wifi, err);
        return;
    }
    __mt76_fw_power_on_rf_step(wifi);
}

static void __mt76_fw_power_on_rf_step(vsf_wifi_t *wifi)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    uint32_t shift = priv->fw_rf_unit ? 8 : 0;
    vsf_err_t err = VSF_ERR_NONE;

    vsf_wifi_chip_mt76_trace_info(
        "mt76: power_on_rf unit=%u step=%u" VSF_TRACE_CFG_LINEEND,
        priv->fw_rf_unit, priv->fw_rf_step);

    switch (priv->fw_rf_step) {
    case 0:
        priv->fw_rf_step = 1;
        err = __mt76_cfg_read(wifi, 0x40000130, &priv->fw_idx,
                              __mt76_fw_power_on_rf_continue);
        break;
    case 1:
        priv->fw_idx |= (1U << 0) << shift;
        priv->fw_rf_step = 2;
        err = __mt76_cfg_write(wifi, 0x40000130, priv->fw_idx,
                               __mt76_fw_power_on_rf_continue);
        break;
    case 2:
        priv->fw_rf_step = 3;
        __mt76_fw_timer_start(wifi, 1);
        break;
    case 3:
        priv->fw_rf_step = 4;
        err = __mt76_cfg_read(wifi, 0x40000130, &priv->fw_idx,
                              __mt76_fw_power_on_rf_continue);
        break;
    case 4: {
        uint32_t val = ((1U << 1) | (1U << 3) | (1U << 4) | (1U << 5)) << shift;
        priv->fw_idx |= val;
        priv->fw_rf_step = 5;
        err = __mt76_cfg_write(wifi, 0x40000130, priv->fw_idx,
                               __mt76_fw_power_on_rf_continue);
        break;
    }
    case 5:
        priv->fw_rf_step = 6;
        __mt76_fw_timer_start(wifi, 1);
        break;
    case 6:
        priv->fw_rf_step = 7;
        err = __mt76_cfg_read(wifi, 0x40000130, &priv->fw_idx,
                              __mt76_fw_power_on_rf_continue);
        break;
    case 7:
        priv->fw_idx &= ~((1U << 2) << shift);
        priv->fw_rf_step = 8;
        err = __mt76_cfg_write(wifi, 0x40000130, priv->fw_idx,
                               __mt76_fw_power_on_rf_continue);
        break;
    case 8:
        priv->fw_rf_step = 9;
        __mt76_fw_timer_start(wifi, 1);
        break;
    case 9:
        priv->fw_rf_step = 10;
        err = __mt76_cfg_read(wifi, 0x40000130, &priv->fw_idx,
                              __mt76_fw_power_on_rf_continue);
        break;
    case 10:
        priv->fw_idx |= (1U << 0) | (1U << 16);
        priv->fw_rf_step = 11;
        err = __mt76_cfg_write(wifi, 0x40000130, priv->fw_idx,
                               __mt76_fw_power_on_rf_continue);
        break;
    case 11:
        priv->fw_rf_step = 12;
        __mt76_fw_timer_start(wifi, 1);
        break;
    case 12:
        priv->fw_rf_step = 13;
        err = __mt76_cfg_read(wifi, 0x4000001c, &priv->fw_idx,
                              __mt76_fw_power_on_rf_continue);
        break;
    case 13: {
        uint32_t val = priv->fw_idx & ~0xffU;
        val |= 0x30;
        priv->fw_rf_step = 14;
        err = __mt76_cfg_write(wifi, 0x4000001c, val,
                               __mt76_fw_power_on_rf_continue);
        break;
    }
    case 14:
        priv->fw_rf_step = 15;
        err = __mt76_cfg_write(wifi, 0x40000014, 0x484f,
                               __mt76_fw_power_on_rf_continue);
        break;
    case 15:
        priv->fw_rf_step = 16;
        __mt76_fw_timer_start(wifi, 1);
        break;
    case 16:
        priv->fw_rf_step = 17;
        err = __mt76_cfg_read(wifi, 0x40000130, &priv->fw_idx,
                              __mt76_fw_power_on_rf_continue);
        break;
    case 17:
        priv->fw_idx |= (1U << 17);
        priv->fw_rf_step = 18;
        err = __mt76_cfg_write(wifi, 0x40000130, priv->fw_idx,
                               __mt76_fw_power_on_rf_continue);
        break;
    case 18:
        priv->fw_rf_step = 19;
        __mt76_fw_timer_start(wifi, 1);
        break;
    case 19:
        priv->fw_rf_step = 20;
        err = __mt76_cfg_read(wifi, 0x40000130, &priv->fw_idx,
                              __mt76_fw_power_on_rf_continue);
        break;
    case 20:
        priv->fw_idx &= ~(1U << 16);
        priv->fw_rf_step = 21;
        err = __mt76_cfg_write(wifi, 0x40000130, priv->fw_idx,
                               __mt76_fw_power_on_rf_continue);
        break;
    case 21:
        priv->fw_rf_step = 22;
        __mt76_fw_timer_start(wifi, 1);
        break;
    case 22:
        priv->fw_rf_step = 23;
        err = __mt76_cfg_read(wifi, 0x4000014c, &priv->fw_idx,
                              __mt76_fw_power_on_rf_continue);
        break;
    case 23: {
        uint32_t val = priv->fw_idx | (1U << 19) | (1U << 20);
        priv->fw_rf_step = 24;
        err = __mt76_cfg_write(wifi, 0x4000014c, val,
                               __mt76_fw_power_on_rf_continue);
        break;
    }
    case 24:
        priv->fw_rf_step = 25;
        err = __mt76_cfg_read(wifi, 0x0530, &priv->fw_idx,
                              __mt76_fw_power_on_rf_continue);
        break;
    case 25: {
        uint32_t val = priv->fw_idx | 0xf;
        priv->fw_rf_step = 26;
        err = __mt76_cfg_write(wifi, 0x0530, val,
                               __mt76_fw_power_on_rf_continue);
        break;
    }
    case 26:
        if (priv->fw_rf_unit == 0) {
            priv->fw_rf_unit = 1;
            priv->fw_rf_step = 0;
            __mt76_fw_power_on_rf_step(wifi);
        } else {
            priv->fw_state = MT76_FW_STATE_WAIT_MAC;
            __mt76_fw_next(wifi, VSF_ERR_NONE);
        }
        return;
    default:
        err = VSF_ERR_BUG;
        break;
    }

    if (err != VSF_ERR_NONE) {
        __mt76_fw_finish(wifi, err);
    }
}

static void __mt76_fw_read_asic_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);

    if (err != VSF_ERR_NONE) {
        __mt76_fw_finish(wifi, err);
        return;
    }

    priv->asic_rev = priv->fw_idx;
    if (!__mt76_chip_id_valid(priv->asic_rev)) {
        vsf_wifi_chip_mt76_trace_error(
            "mt76: unsupported chip 0x%04X" VSF_TRACE_CFG_LINEEND,
            (unsigned)(priv->asic_rev >> 16));
        __mt76_fw_finish(wifi, VSF_ERR_NOT_SUPPORT);
        return;
    }

    vsf_wifi_chip_mt76_trace_info(
        "mt76: ASIC version = 0x%08X" VSF_TRACE_CFG_LINEEND,
        (unsigned)priv->asic_rev);

    /* Patch applied flag location depends on revision. */
    if (__mt76_rev(priv) >= MT76_REV_E3) {
        priv->fw_patch_reg  = MT76_MCU_CLOCK_CTL;
        priv->fw_patch_mask = 0x00000001;
    } else {
        priv->fw_patch_reg  = MT76_MCU_COM_REG0;
        priv->fw_patch_mask = 0x00000002;
    }

    priv->fw_state = MT76_FW_STATE_CHECK_PATCH;
    __mt76_fw_next(wifi, VSF_ERR_NONE);
}

static void __mt76_fw_check_patch_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);

    if (err != VSF_ERR_NONE) {
        __mt76_fw_finish(wifi, err);
        return;
    }

    if (priv->fw_idx & priv->fw_patch_mask) {
        vsf_wifi_chip_mt76_trace_info(
            "mt76: ROM patch already applied" VSF_TRACE_CFG_LINEEND);
        priv->fw_stage = MT76_FW_STAGE_ILM;
    } else {
        priv->fw_stage = MT76_FW_STAGE_PATCH;
    }

    if (((uint16_t)(priv->asic_rev >> 16)) == MT76_CHIP_ID_7612) {
        priv->fw_state = MT76_FW_STATE_ENABLE_USB_DMA;
    } else {
        priv->fw_state = MT76_FW_STATE_ACQUIRE_SEM;
        priv->fw_sem_ms = 600;
    }
    __mt76_fw_next(wifi, VSF_ERR_NONE);
}

static void __mt76_fw_acquire_sem_start(vsf_wifi_t *wifi)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);

    if (priv->fw_sem_ms == 0) {
        vsf_wifi_chip_mt76_trace_error(
            "mt76: ROM patch semaphore timeout" VSF_TRACE_CFG_LINEEND);
        __mt76_fw_finish(wifi, VSF_ERR_FAIL);
        return;
    }

    vsf_err_t err = __mt76_cfg_read(wifi, MT76_MCU_SEMAPHORE_03,
                                     &priv->fw_idx, __mt76_fw_acquire_sem_done);
    if (err != VSF_ERR_NONE) {
        __mt76_fw_finish(wifi, err);
    }
}

static void __mt76_fw_acquire_sem_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    if (err != VSF_ERR_NONE) { __mt76_fw_finish(wifi, err); return; }

    if (priv->fw_idx & 0x00000001) {
        if (priv->fw_stage == MT76_FW_STAGE_ILM) {
            priv->fw_state = MT76_FW_STATE_RELEASE_SEM;
        } else {
            priv->fw_state = MT76_FW_STATE_ENABLE_USB_DMA;
        }
        __mt76_fw_next(wifi, VSF_ERR_NONE);
        return;
    }

    if (priv->fw_sem_ms > 0) {
        priv->fw_sem_ms--;
    }
    if (priv->fw_sem_ms == 0) {
        vsf_wifi_chip_mt76_trace_error(
            "mt76: ROM patch semaphore timeout" VSF_TRACE_CFG_LINEEND);
        __mt76_fw_finish(wifi, VSF_ERR_FAIL);
        return;
    }

    __mt76_fw_timer_start(wifi, 1);
}

static void __mt76_fw_release_sem(vsf_wifi_t *wifi)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);

    priv->fw_state = MT76_FW_STATE_PARSE_FW_HEADER;
    vsf_err_t err = __mt76_cfg_write(wifi, MT76_MCU_SEMAPHORE_03, 1,
                                      __mt76_fw_next);
    if (err != VSF_ERR_NONE) {
        __mt76_fw_finish(wifi, err);
    }
}

static void __mt76_fw_parse_header(vsf_wifi_t *wifi)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    const uint8_t *hdr = __mt76_main_firmware_data;
    uint32_t total = __mt76_main_firmware_size;
    uint32_t ilm_len, dlm_len;

    if (total < MT76_FW_HDR_SIZE) {
        __mt76_fw_finish(wifi, VSF_ERR_FAIL);
        return;
    }

    ilm_len = __mt76_get_le32(hdr + 0);
    dlm_len = __mt76_get_le32(hdr + 4);

    if ((MT76_FW_HDR_SIZE + ilm_len + dlm_len) != total) {
        vsf_wifi_chip_mt76_trace_error(
            "mt76: firmware size mismatch" VSF_TRACE_CFG_LINEEND);
        __mt76_fw_finish(wifi, VSF_ERR_FAIL);
        return;
    }

    priv->fw_ilm_len = ilm_len;
    priv->fw_dlm_len = dlm_len;
    priv->fw_stage   = MT76_FW_STAGE_ILM;
    priv->fw_state   = MT76_FW_STATE_ENABLE_USB_DMA;

    vsf_wifi_chip_mt76_trace_info(
        "mt76: firmware ILM=%lu DLM=%lu" VSF_TRACE_CFG_LINEEND,
        (unsigned long)ilm_len, (unsigned long)dlm_len);

    __mt76_fw_next(wifi, VSF_ERR_NONE);
}

static void __mt76_fw_send_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);

    if (err != VSF_ERR_NONE) {
        __mt76_fw_finish(wifi, err);
        return;
    }

    priv->fw_state = priv->fw_send_next_state;
    __mt76_fw_next(wifi, VSF_ERR_NONE);
}

static void __mt76_fw_send_start(vsf_wifi_t *wifi)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    const uint8_t *data = NULL;
    uint32_t len = 0;
    uint32_t dst = 0;

    switch (priv->fw_stage) {
    case MT76_FW_STAGE_PATCH:
        if (__mt76_rom_patch_firmware_size <= MT76_PATCH_HDR_SIZE) {
            __mt76_fw_finish(wifi, VSF_ERR_FAIL);
            return;
        }
        data = __mt76_rom_patch_firmware_data + MT76_PATCH_HDR_SIZE;
        len  = __mt76_rom_patch_firmware_size - MT76_PATCH_HDR_SIZE;
        dst  = MT76_ROM_PATCH_OFFSET;
        priv->fw_send_next_state = MT76_FW_STATE_ENABLE_PATCH;
        break;
    case MT76_FW_STAGE_ILM:
        data = __mt76_main_firmware_data + MT76_FW_HDR_SIZE;
        len  = priv->fw_ilm_len;
        dst  = MT76_ILM_OFFSET;
        priv->fw_send_next_state = MT76_FW_STATE_UPLOAD;
        priv->fw_stage           = MT76_FW_STAGE_DLM;
        break;
    case MT76_FW_STAGE_DLM:
        data = __mt76_main_firmware_data + MT76_FW_HDR_SIZE + priv->fw_ilm_len;
        len  = priv->fw_dlm_len;
        dst  = MT76_DLM_OFFSET;
        if (__mt76_rev(priv) >= MT76_REV_E3) {
            dst += MT76_DLM_OFFSET_E3_EXTRA;
        }
        priv->fw_send_next_state = MT76_FW_STATE_LOAD_IVB;
        break;
    default:
        __mt76_fw_finish(wifi, VSF_ERR_BUG);
        return;
    }

    priv->fw_data        = data;
    priv->fw_len         = len;
    priv->fw_pos         = 0;
    priv->fw_dst_offset  = dst;
    priv->fw_max_payload = (priv->fw_stage == MT76_FW_STAGE_PATCH)
                         ? MT76_FW_PATCH_CHUNK_PAYLOAD
                         : MT76_FW_FW_CHUNK_PAYLOAD;
    priv->fw_send_state  = MT76_FW_SEND_CHUNK;

    __mt76_fw_send_chunk(wifi);
}

static void __mt76_fw_send_chunk(vsf_wifi_t *wifi)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    uint32_t left   = priv->fw_len - priv->fw_pos;
    uint32_t chunk  = __mt76_min(left, priv->fw_max_payload);
    uint32_t rounded= __mt76_round_up(chunk, 4);
    uint32_t total  = 4 + rounded + 4;
    uint8_t *buf    = priv->tx_buf;

    VSF_WIFI_ASSERT(total <= sizeof(priv->tx_buf));

    uint32_t info = MT76_MCU_MSG_TYPE_CMD
                  | (((uint32_t)MT76_CPU_TX_PORT & 0x07) << MT76_MCU_MSG_PORT_SHIFT)
                  | (chunk & MT76_MCU_MSG_LEN_MASK);

    __mt76_put_le32(buf, info);
    memcpy(buf + 4, priv->fw_data + priv->fw_pos, chunk);
    memset(buf + 4 + chunk, 0, (rounded - chunk) + 4);

    priv->fw_send_state = MT76_FW_SEND_CHUNK;

    vsf_err_t err = __mt76_fce_write(wifi, MT76_FCE_DMA_ADDR,
                                     priv->fw_dst_offset + priv->fw_pos,
                                     __mt76_fw_send_fce_addr_done);
    if (err != VSF_ERR_NONE) {
        __mt76_fw_finish(wifi, err);
    }
}

static void __mt76_fw_send_fce_addr_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    if (err != VSF_ERR_NONE) { __mt76_fw_finish(wifi, err); return; }

    uint32_t left    = priv->fw_len - priv->fw_pos;
    uint32_t chunk   = __mt76_min(left, priv->fw_max_payload);
    uint32_t rounded = __mt76_round_up(chunk, 4);

    priv->fw_send_state = MT76_FW_SEND_FCE_LEN;
    err = __mt76_fce_write(wifi, MT76_FCE_DMA_LEN, rounded << 16,
                           __mt76_fw_send_fce_len_done);
    if (err != VSF_ERR_NONE) {
        __mt76_fw_finish(wifi, err);
    }
}

static void __mt76_fw_send_fce_len_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    if (err != VSF_ERR_NONE) { __mt76_fw_finish(wifi, err); return; }

    uint32_t left    = priv->fw_len - priv->fw_pos;
    uint32_t chunk   = __mt76_min(left, priv->fw_max_payload);
    uint32_t rounded = __mt76_round_up(chunk, 4);
    uint32_t total   = 4 + rounded + 4;

    priv->fw_send_state = MT76_FW_SEND_BULK;
    err = __mt76_tx_frame(wifi, priv->tx_buf, (uint16_t)total, 0,
                          __mt76_fw_send_bulk_done);
    if (err != VSF_ERR_NONE) {
        __mt76_fw_finish(wifi, err);
    }
}

static void __mt76_fw_send_bulk_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    if (err != VSF_ERR_NONE) { __mt76_fw_finish(wifi, err); return; }

    priv->fw_send_state = MT76_FW_SEND_IDX_READ;
    err = __mt76_cfg_read(wifi, MT76_TX_CPU_FROM_FCE_CPU_DESC_IDX,
                          &priv->fw_idx, __mt76_fw_send_idx_read_done);
    if (err != VSF_ERR_NONE) {
        __mt76_fw_finish(wifi, err);
    }
}

static void __mt76_fw_send_idx_read_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    if (err != VSF_ERR_NONE) { __mt76_fw_finish(wifi, err); return; }

    priv->fw_idx++;
    priv->fw_send_state = MT76_FW_SEND_IDX_WRITE;
    err = __mt76_cfg_write(wifi, MT76_TX_CPU_FROM_FCE_CPU_DESC_IDX,
                           priv->fw_idx, __mt76_fw_send_idx_write_done);
    if (err != VSF_ERR_NONE) {
        __mt76_fw_finish(wifi, err);
    }
}

static void __mt76_fw_send_idx_write_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    if (err != VSF_ERR_NONE) { __mt76_fw_finish(wifi, err); return; }

    uint32_t left   = priv->fw_len - priv->fw_pos;
    uint32_t chunk  = __mt76_min(left, priv->fw_max_payload);
    priv->fw_pos   += chunk;

    if (priv->fw_pos < priv->fw_len) {
        __mt76_fw_send_chunk(wifi);
    } else {
        priv->fw_send_state = MT76_FW_SEND_IDLE;
        __mt76_fw_send_done(wifi, VSF_ERR_NONE);
    }
}

static void __mt76_fw_poll_patch_start(vsf_wifi_t *wifi)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);

    if (priv->fw_poll_ms == 0) {
        vsf_wifi_chip_mt76_trace_error(
            "mt76: ROM patch did not apply" VSF_TRACE_CFG_LINEEND);
        __mt76_fw_finish(wifi, VSF_ERR_FAIL);
        return;
    }

    vsf_err_t err = __mt76_cfg_read(wifi, priv->fw_patch_reg,
                                     &priv->fw_idx, __mt76_fw_poll_patch_done);
    if (err != VSF_ERR_NONE) {
        __mt76_fw_finish(wifi, err);
    }
}

static void __mt76_fw_poll_patch_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    if (err != VSF_ERR_NONE) { __mt76_fw_finish(wifi, err); return; }

    if (priv->fw_idx & priv->fw_patch_mask) {
        priv->fw_state = MT76_FW_STATE_RELEASE_SEM;
        __mt76_fw_next(wifi, VSF_ERR_NONE);
        return;
    }

    if (priv->fw_poll_ms > 0) {
        priv->fw_poll_ms--;
    }
    if (priv->fw_poll_ms == 0) {
        vsf_wifi_chip_mt76_trace_error(
            "mt76: ROM patch apply timeout" VSF_TRACE_CFG_LINEEND);
        __mt76_fw_finish(wifi, VSF_ERR_FAIL);
        return;
    }

    __mt76_fw_timer_start(wifi, 1);
}

static void __mt76_fw_poll_fw_ready_start(vsf_wifi_t *wifi)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);

    if (priv->fw_poll_ms == 0) {
        vsf_wifi_chip_mt76_trace_error(
            "mt76: firmware did not start" VSF_TRACE_CFG_LINEEND);
        __mt76_fw_finish(wifi, VSF_ERR_FAIL);
        return;
    }

    vsf_err_t err = __mt76_cfg_read(wifi, MT76_MCU_COM_REG0,
                                     &priv->fw_idx, __mt76_fw_poll_fw_ready_done);
    if (err != VSF_ERR_NONE) {
        __mt76_fw_finish(wifi, err);
    }
}

static void __mt76_fw_poll_fw_ready_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    if (err != VSF_ERR_NONE) { __mt76_fw_finish(wifi, err); return; }

    if (priv->fw_idx & 0x00000001) {
        priv->fw_state = MT76_FW_STATE_SET_COMREG;
        __mt76_fw_next(wifi, VSF_ERR_NONE);
        return;
    }

    if (priv->fw_poll_ms > 0) {
        priv->fw_poll_ms--;
    }
    if (priv->fw_poll_ms == 0) {
        vsf_wifi_chip_mt76_trace_error(
            "mt76: firmware start timeout" VSF_TRACE_CFG_LINEEND);
        __mt76_fw_finish(wifi, VSF_ERR_FAIL);
        return;
    }

    __mt76_fw_timer_start(wifi, 1);
}

static void __mt76_fw_next(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    vsf_err_t step_err;

    if (err != VSF_ERR_NONE) {
        __mt76_fw_finish(wifi, err);
        return;
    }

    vsf_wifi_chip_mt76_trace_info(
        "mt76: fw_state=%d" VSF_TRACE_CFG_LINEEND, priv->fw_state);

    switch (priv->fw_state) {
    case MT76_FW_STATE_RESET_WLAN:
        step_err = __mt76_cfg_read(wifi, MT76_WLAN_FUN_CTRL,
                                   &priv->fw_idx, __mt76_fw_reset_wlan_done);
        break;

    case MT76_FW_STATE_RESET_WLAN_RF:
        priv->fw_state = MT76_FW_STATE_RESET_WLAN_EN;
        priv->fw_idx &= ~MT76_WLAN_FUN_CTRL_WLAN_RESET_RF;
        step_err = __mt76_cfg_write(wifi, MT76_WLAN_FUN_CTRL, priv->fw_idx,
                                    __mt76_fw_next);
        break;

    case MT76_FW_STATE_RESET_WLAN_EN: {
        uint32_t val = priv->fw_idx | MT76_WLAN_FUN_CTRL_WLAN_EN |
                       MT76_WLAN_FUN_CTRL_WLAN_CLK_EN;
        priv->fw_state = MT76_FW_STATE_POWER_ON_MTCMOS;
        step_err = __mt76_cfg_write(wifi, MT76_WLAN_FUN_CTRL, val,
                                    __mt76_fw_next);
        break;
    }

    case MT76_FW_STATE_POWER_ON_MTCMOS:
        priv->fw_state = MT76_FW_STATE_POWER_ON_MTCMOS_POLL;
        step_err = __mt76_cfg_write(wifi, MT76_WLAN_MTC_CTRL,
                                    MT76_WLAN_MTC_CTRL_MTCMOS_PWR_UP,
                                    __mt76_fw_next);
        break;

    case MT76_FW_STATE_POWER_ON_MTCMOS_POLL:
        step_err = __mt76_cfg_read(wifi, MT76_WLAN_MTC_CTRL,
                                   &priv->fw_idx, __mt76_fw_power_on_done);
        break;

    case MT76_FW_STATE_POWER_ON_MTCMOS_CLEAR1:
        priv->fw_state = MT76_FW_STATE_POWER_ON_MTCMOS_CLEAR2;
        priv->fw_idx &= ~(0x7fU << 16);
        step_err = __mt76_cfg_write(wifi, MT76_WLAN_MTC_CTRL, priv->fw_idx,
                                    __mt76_fw_next);
        break;

    case MT76_FW_STATE_POWER_ON_MTCMOS_CLEAR2:
        priv->fw_state = MT76_FW_STATE_POWER_ON_MTCMOS_SET2;
        priv->fw_idx &= ~(0xfU << 24);
        step_err = __mt76_cfg_write(wifi, MT76_WLAN_MTC_CTRL, priv->fw_idx,
                                    __mt76_fw_next);
        break;

    case MT76_FW_STATE_POWER_ON_MTCMOS_SET2:
        priv->fw_state = MT76_FW_STATE_POWER_ON_MTCMOS_CLEAR3;
        priv->fw_idx |= (0xfU << 24);
        step_err = __mt76_cfg_write(wifi, MT76_WLAN_MTC_CTRL, priv->fw_idx,
                                    __mt76_fw_next);
        break;

    case MT76_FW_STATE_POWER_ON_MTCMOS_CLEAR3:
        priv->fw_state = MT76_FW_STATE_POWER_ON_AD_DA;
        priv->fw_idx &= 0xfffff000U;
        step_err = __mt76_cfg_write(wifi, MT76_WLAN_MTC_CTRL, priv->fw_idx,
                                    __mt76_fw_next);
        break;

    case MT76_FW_STATE_POWER_ON_AD_DA:
        priv->fw_state = MT76_FW_STATE_POWER_ON_AD_DA_WRITE;
        step_err = __mt76_cfg_read(wifi, 0x40001204,
                                   &priv->fw_idx, __mt76_fw_next);
        break;

    case MT76_FW_STATE_POWER_ON_AD_DA_WRITE:
        priv->fw_state = MT76_FW_STATE_POWER_ON_WLAN_EN;
        step_err = __mt76_cfg_write(wifi, 0x40001204,
                                    priv->fw_idx & ~(1U << 3),
                                    __mt76_fw_next);
        break;

    case MT76_FW_STATE_POWER_ON_WLAN_EN:
        priv->fw_state = MT76_FW_STATE_POWER_ON_WLAN_EN_WRITE;
        step_err = __mt76_cfg_read(wifi, 0x40000080,
                                   &priv->fw_idx, __mt76_fw_next);
        break;

    case MT76_FW_STATE_POWER_ON_WLAN_EN_WRITE:
        priv->fw_state = MT76_FW_STATE_POWER_ON_BBP_RST;
        step_err = __mt76_cfg_write(wifi, 0x40000080,
                                    priv->fw_idx | (1U << 0),
                                    __mt76_fw_next);
        break;

    case MT76_FW_STATE_POWER_ON_BBP_RST:
        priv->fw_state = MT76_FW_STATE_POWER_ON_BBP_RST_WRITE;
        step_err = __mt76_cfg_read(wifi, 0x40000064,
                                   &priv->fw_idx, __mt76_fw_next);
        break;

    case MT76_FW_STATE_POWER_ON_BBP_RST_WRITE:
        priv->fw_state = MT76_FW_STATE_POWER_ON_RF;
        priv->fw_rf_unit = 0;
        priv->fw_rf_step = 0;
        step_err = VSF_ERR_NONE;
        __mt76_fw_power_on_rf_step(wifi);
        break;

    case MT76_FW_STATE_POWER_ON_RF:
        /* Continuation handled by __mt76_fw_power_on_rf_step. */
        __mt76_fw_power_on_rf_step(wifi);
        return;

    case MT76_FW_STATE_WAIT_MAC:
        priv->fw_poll_ms = 100;
        step_err = __mt76_cfg_read(wifi, MT76_MAC_CSR0,
                                   &priv->fw_idx, __mt76_fw_wait_mac_done);
        break;

    case MT76_FW_STATE_READ_ASIC_REV:
        step_err = __mt76_cfg_read(wifi, MT76_ASIC_VERSION,
                                   &priv->fw_idx, __mt76_fw_read_asic_done);
        break;

    case MT76_FW_STATE_CHECK_PATCH:
        step_err = __mt76_cfg_read(wifi, priv->fw_patch_reg,
                                   &priv->fw_idx, __mt76_fw_check_patch_done);
        break;

    case MT76_FW_STATE_ACQUIRE_SEM:
        priv->fw_sem_ms = 600;
        step_err = VSF_ERR_NONE;
        __mt76_fw_acquire_sem_start(wifi);
        break;

    case MT76_FW_STATE_ENABLE_USB_DMA:
        /* Windows driver does not issue MT_VEND_DEV_MODE reset before FCE
         * setup; skip it to match the successful Windows enumeration sequence. */
        priv->fw_state = MT76_FW_STATE_FCE_PSE;
        step_err = __mt76_cfg_write(wifi, MT76_USB_U3DMA_CFG,
                                    MT76_USB_DMA_CFG_VAL, __mt76_fw_next);
        break;

    case MT76_FW_STATE_FCE_PSE:
        priv->fw_state = MT76_FW_STATE_FCE_BASE;
        step_err = __mt76_cfg_write(wifi, MT76_FCE_PSE_CTRL, 1, __mt76_fw_next);
        break;

    case MT76_FW_STATE_FCE_BASE:
        priv->fw_state = MT76_FW_STATE_FCE_MAX;
        step_err = __mt76_cfg_write(wifi, MT76_TX_CPU_FROM_FCE_BASE_PTR,
                                    0x400230, __mt76_fw_next);
        break;

    case MT76_FW_STATE_FCE_MAX:
        priv->fw_state = MT76_FW_STATE_FCE_PDMA;
        step_err = __mt76_cfg_write(wifi, MT76_TX_CPU_FROM_FCE_MAX_COUNT,
                                    1, __mt76_fw_next);
        break;

    case MT76_FW_STATE_FCE_PDMA:
        priv->fw_state = MT76_FW_STATE_FCE_SKIP;
        step_err = __mt76_cfg_write(wifi, MT76_FCE_PDMA_GLOBAL_CONF,
                                    0x44, __mt76_fw_next);
        break;

    case MT76_FW_STATE_FCE_SKIP:
        priv->fw_state = MT76_FW_STATE_UPLOAD;
        step_err = VSF_ERR_NONE;
        __mt76_fw_send_start(wifi);
        break;

    case MT76_FW_STATE_UPLOAD:
        step_err = VSF_ERR_NONE;
        __mt76_fw_send_start(wifi);
        break;

    case MT76_FW_STATE_ENABLE_PATCH: {
        static const uint8_t enable_patch[] = {
            0x6f, 0xfc, 0x08, 0x01,
            0x20, 0x04, 0x00, 0x00,
            0x00, 0x09, 0x00
        };
        priv->fw_state = MT76_FW_STATE_RESET_WMT;
        step_err = __mt76_dev_class_cmd(wifi, MT76_VEND_DEV_MODE, 0x12, 0,
                    enable_patch, sizeof(enable_patch), __mt76_fw_next);
        break;
    }

    case MT76_FW_STATE_RESET_WMT: {
        static const uint8_t reset_wmt[] = {
            0x6f, 0xfc, 0x05, 0x01,
            0x07, 0x01, 0x00, 0x04
        };
        priv->fw_state = MT76_FW_STATE_WAIT_WMT;
        step_err = __mt76_dev_class_cmd(wifi, MT76_VEND_DEV_MODE, 0x12, 0,
                    reset_wmt, sizeof(reset_wmt), __mt76_fw_next);
        break;
    }

    case MT76_FW_STATE_WAIT_WMT:
        priv->fw_state = MT76_FW_STATE_POLL_PATCH;
        priv->fw_poll_ms = 100;
        __mt76_fw_timer_start(wifi, 20);
        step_err = VSF_ERR_NONE;
        break;

    case MT76_FW_STATE_POLL_PATCH:
        step_err = VSF_ERR_NONE;
        __mt76_fw_poll_patch_start(wifi);
        break;

    case MT76_FW_STATE_RELEASE_SEM:
        step_err = VSF_ERR_NONE;
        __mt76_fw_release_sem(wifi);
        break;

    case MT76_FW_STATE_PARSE_FW_HEADER:
        step_err = VSF_ERR_NONE;
        __mt76_fw_parse_header(wifi);
        break;

    case MT76_FW_STATE_LOAD_IVB:
        priv->fw_state = MT76_FW_STATE_POLL_FW_READY;
        priv->fw_poll_ms = 100;
        step_err = __mt76_dev_cmd(wifi, MT76_VEND_DEV_MODE, 0x12, 0,
                                  __mt76_fw_next);
        break;

    case MT76_FW_STATE_POLL_FW_READY:
        step_err = VSF_ERR_NONE;
        __mt76_fw_poll_fw_ready_start(wifi);
        break;

    case MT76_FW_STATE_SET_COMREG:
        priv->fw_state = MT76_FW_STATE_REENABLE_FCE;
        step_err = __mt76_cfg_write(wifi, MT76_MCU_COM_REG0, 0x02,
                                    __mt76_fw_next);
        break;

    case MT76_FW_STATE_REENABLE_FCE:
        priv->fw_state = MT76_FW_STATE_DONE;
        step_err = __mt76_cfg_write(wifi, MT76_FCE_PSE_CTRL, 1,
                                    __mt76_fw_next);
        break;

    case MT76_FW_STATE_DONE:
        __mt76_fw_finish(wifi, VSF_ERR_NONE);
        return;

    default:
        step_err = VSF_ERR_BUG;
        break;
    }

    if (step_err != VSF_ERR_NONE) {
        __mt76_fw_finish(wifi, step_err);
    }
}

vsf_err_t __mt76_firmware_load(vsf_wifi_t *wifi, vsf_wifi_done_t done)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);

    priv->pending_done = done;
    priv->fw_state     = MT76_FW_STATE_RESET_WLAN;
    priv->fw_send_state= MT76_FW_SEND_IDLE;
    priv->mcu_seq      = 0;

    vsf_wifi_chip_mt76_trace_info(
        "mt76: loading firmware" VSF_TRACE_CFG_LINEEND);

    __mt76_fw_next(wifi, VSF_ERR_NONE);
    return VSF_ERR_NONE;
}

/*============================ EEPROM / MAC address ==========================*/

static void __mt76_eeprom_load_continue(vsf_wifi_t *wifi, vsf_err_t err);

vsf_err_t __mt76_eeprom_load_start(vsf_wifi_t *wifi, vsf_wifi_done_t done)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);

    priv->eeprom_offset = 0;
    priv->eeprom_step   = 0;
    priv->eeprom_done   = done;

    uint32_t addr = priv->eeprom_offset & ~0xF;
    uint32_t val  = MT_EFUSE_CTRL_KICK |
                    ((uint32_t)MT_EE_READ << 6) |
                    (addr << 16);

    vsf_err_t step_err = __mt76_cfg_write(wifi, MT_EFUSE_CTRL, val,
                                          __mt76_eeprom_load_continue);
    if (step_err != VSF_ERR_NONE) {
        priv->eeprom_done = NULL;
        if (done != NULL) done(wifi, step_err);
    }
    return step_err;
}

static void __mt76_eeprom_load_continue(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    vsf_wifi_done_t done = priv->eeprom_done;

    if (err != VSF_ERR_NONE) {
        priv->eeprom_done = NULL;
        if (done != NULL) done(wifi, err);
        return;
    }

    switch (priv->eeprom_step) {
    case 0:
        priv->eeprom_step = 1;
        err = __mt76_cfg_read(wifi, MT_EFUSE_DATA(0), &priv->eeprom_data[0],
                              __mt76_eeprom_load_continue);
        break;
    case 1:
        priv->eeprom_step = 2;
        err = __mt76_cfg_read(wifi, MT_EFUSE_DATA(1), &priv->eeprom_data[1],
                              __mt76_eeprom_load_continue);
        break;
    case 2:
        priv->eeprom_step = 3;
        err = __mt76_cfg_read(wifi, MT_EFUSE_DATA(2), &priv->eeprom_data[2],
                              __mt76_eeprom_load_continue);
        break;
    case 3:
        priv->eeprom_step = 4;
        err = __mt76_cfg_read(wifi, MT_EFUSE_DATA(3), &priv->eeprom_data[3],
                              __mt76_eeprom_load_continue);
        break;
    case 4: {
        uint8_t *dst = &priv->eeprom[priv->eeprom_offset];
        for (int i = 0; i < 4; i++) {
            uint32_t val = priv->eeprom_data[i];
            dst[4 * i + 0] = (uint8_t)(val >> 0);
            dst[4 * i + 1] = (uint8_t)(val >> 8);
            dst[4 * i + 2] = (uint8_t)(val >> 16);
            dst[4 * i + 3] = (uint8_t)(val >> 24);
        }

        priv->eeprom_offset += 16;
        if (priv->eeprom_offset >= MT76_EEPROM_SIZE) {
            priv->eeprom_done = NULL;
            if (done != NULL) done(wifi, VSF_ERR_NONE);
            return;
        }

        priv->eeprom_step = 0;
        uint32_t addr = priv->eeprom_offset & ~0xF;
        uint32_t val  = MT_EFUSE_CTRL_KICK |
                        ((uint32_t)MT_EE_READ << 6) |
                        (addr << 16);
        err = __mt76_cfg_write(wifi, MT_EFUSE_CTRL, val,
                               __mt76_eeprom_load_continue);
        break;
    }
    default:
        err = VSF_ERR_BUG;
        break;
    }

    if (err != VSF_ERR_NONE) {
        priv->eeprom_done = NULL;
        if (done != NULL) done(wifi, err);
    }
}

static void __mt76_mac_addr_program_continue(vsf_wifi_t *wifi, vsf_err_t err);

static vsf_err_t __mt76_mac_addr_program_start(vsf_wifi_t *wifi,
                                               const uint8_t mac[6],
                                               vsf_wifi_done_t done)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);

    memcpy(priv->mac_addr, mac, 6);
    priv->mac_addr_step = 0;
    priv->mac_addr_done = done;

    uint32_t dw0 = ((uint32_t)mac[0] << 0) |
                   ((uint32_t)mac[1] << 8) |
                   ((uint32_t)mac[2] << 16) |
                   ((uint32_t)mac[3] << 24);

    vsf_err_t step_err = __mt76_cfg_write(wifi, MT_MAC_ADDR_DW0, dw0,
                                          __mt76_mac_addr_program_continue);
    if (step_err != VSF_ERR_NONE) {
        priv->mac_addr_done = NULL;
        return step_err;
    }
    return VSF_ERR_NONE;
}

static void __mt76_mac_addr_program_continue(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    vsf_wifi_done_t done = priv->mac_addr_done;

    if (err != VSF_ERR_NONE) {
        priv->mac_addr_done = NULL;
        if (done != NULL) done(wifi, err);
        return;
    }

    const uint8_t *mac = priv->mac_addr;
    uint32_t dw1;
    vsf_err_t step_err = VSF_ERR_NONE;

    switch (priv->mac_addr_step) {
    case 0:
        dw1 = ((uint32_t)mac[4] << 0) |
              ((uint32_t)mac[5] << 8) |
              MT_MAC_ADDR_DW1_U2ME_MASK;
        priv->mac_addr_step = 1;
        step_err = __mt76_cfg_write(wifi, MT_MAC_ADDR_DW1, dw1,
                                    __mt76_mac_addr_program_continue);
        break;
    case 1:
        dw1 = ((uint32_t)mac[0] << 0) |
              ((uint32_t)mac[1] << 8) |
              ((uint32_t)mac[2] << 16) |
              ((uint32_t)mac[3] << 24);
        priv->mac_addr_step = 2;
        step_err = __mt76_cfg_write(wifi, MT_MAC_BSSID_DW0, dw1,
                                    __mt76_mac_addr_program_continue);
        break;
    case 2:
        dw1 = ((uint32_t)mac[4] << 0) |
              ((uint32_t)mac[5] << 8) |
              MT_MAC_BSSID_DW1_MBSS_MODE |
              MT_MAC_BSSID_DW1_MBSS_LOCAL_BIT |
              (7U << MT_MAC_BSSID_DW1_MBEACON_N_SHIFT);
        priv->mac_addr_step = 3;
        step_err = __mt76_cfg_write(wifi, MT_MAC_BSSID_DW1, dw1,
                                    __mt76_mac_addr_program_continue);
        break;
    case 3:
        priv->mac_addr_done = NULL;
        if (done != NULL) done(wifi, VSF_ERR_NONE);
        return;
    default:
        step_err = VSF_ERR_BUG;
        break;
    }

    if (step_err != VSF_ERR_NONE) {
        priv->mac_addr_done = NULL;
        if (done != NULL) done(wifi, step_err);
    }
}

vsf_err_t __mt76_eeprom_load(vsf_wifi_t *wifi, vsf_wifi_done_t done)
{
    return __mt76_eeprom_load_start(wifi, done);
}

void __mt76_eeprom_parse_hw_cap(vsf_wifi_t *wifi)
{
    /* TODO: parse TX/RX path, PA/LNA, etc. */
    (void)wifi;
}

const uint8_t *__mt76_eeprom_get_mac(vsf_wifi_t *wifi)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    return &priv->eeprom[MT76_EE_MAC_ADDR];
}

/*============================ Initialization ================================*/

static void __mt76_init_mcu_q_select_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    if (err != VSF_ERR_NONE) {
        __mt76_init_next(wifi, err);
        return;
    }
    priv->state = MT76_STATE_INIT_MCU_RADIO_ON;
    __mt76_init_next(wifi, VSF_ERR_NONE);
}

static void __mt76_init_ready(vsf_wifi_t *wifi)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    vsf_wifi_done_t done = priv->pending_done;

    priv->state = MT76_STATE_INIT_READY;
    priv->on_rx = __mt76_on_rx;
    /* Notify the bus driver that the data path can be enabled. */
    __mt76_bus_ops(wifi)->on_ready(wifi);
    priv->pending_done = NULL;
    if (done != NULL) done(wifi, VSF_ERR_NONE);
}

static void __mt76_eeprom_load_done(vsf_wifi_t *wifi, vsf_err_t err);
static void __mt76_mac_addr_done(vsf_wifi_t *wifi, vsf_err_t err);

static void __mt76_init_mcu_radio_on_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    vsf_wifi_done_t done = priv->pending_done;

    if (err != VSF_ERR_NONE) {
        priv->pending_done = NULL;
        if (done != NULL) done(wifi, err);
        return;
    }

    priv->state = MT76_STATE_INIT_EEPROM_LOAD;
    vsf_err_t step_err = __mt76_eeprom_load(wifi, __mt76_eeprom_load_done);
    if (step_err != VSF_ERR_NONE) {
        priv->pending_done = NULL;
        if (done != NULL) done(wifi, step_err);
    }
}

static void __mt76_eeprom_load_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    vsf_wifi_done_t init_done = priv->pending_done;

    if (err != VSF_ERR_NONE) {
        vsf_wifi_chip_mt76_trace_error(
            "mt76: failed to load EEPROM" VSF_TRACE_CFG_LINEEND);
        priv->pending_done = NULL;
        if (init_done != NULL) init_done(wifi, err);
        return;
    }

    const uint8_t *mac = __mt76_eeprom_get_mac(wifi);
    vsf_wifi_chip_mt76_trace_info(
        "mt76: EEPROM loaded, MAC=%02X:%02X:%02X:%02X:%02X:%02X" VSF_TRACE_CFG_LINEEND,
        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    memcpy(wifi->mac, mac, 6);

    priv->state = MT76_STATE_INIT_MAC_ADDR;
    vsf_err_t step_err = __mt76_set_mac_addr(wifi, mac, __mt76_mac_addr_done);
    if (step_err != VSF_ERR_NONE) {
        priv->pending_done = NULL;
        if (init_done != NULL) init_done(wifi, step_err);
    }
}

static void __mt76_mac_addr_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);

    if (err != VSF_ERR_NONE) {
        vsf_wifi_chip_mt76_trace_error(
            "mt76: failed to set MAC address" VSF_TRACE_CFG_LINEEND);
        vsf_wifi_done_t init_done = priv->pending_done;
        priv->pending_done = NULL;
        if (init_done != NULL) init_done(wifi, err);
        return;
    }

    __mt76_init_ready(wifi);
}

static void __mt76_read_asic_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    if (err != VSF_ERR_NONE) {
        vsf_wifi_chip_mt76_trace_error(
            "mt76: failed to read ASIC version" VSF_TRACE_CFG_LINEEND);
        __mt76_init_next(wifi, err);
        return;
    }

    if (!__mt76_chip_id_valid(priv->asic_rev)) {
        vsf_wifi_chip_mt76_trace_error(
            "mt76: unsupported chip 0x%04X" VSF_TRACE_CFG_LINEEND,
            (unsigned)(priv->asic_rev >> 16));
        __mt76_init_next(wifi, VSF_ERR_NOT_SUPPORT);
        return;
    }

    vsf_wifi_chip_mt76_trace_info(
        "mt76: ASIC version = 0x%08X" VSF_TRACE_CFG_LINEEND,
        (unsigned)priv->asic_rev);

    priv->state = MT76_STATE_INIT_MCU_Q_SELECT;
    __mt76_init_next(wifi, VSF_ERR_NONE);
}

vsf_err_t __mt76_init(vsf_wifi_t *wifi, vsf_wifi_done_t done)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    priv->pending_done = done;
    priv->state        = MT76_STATE_INIT_READ_ASIC;
    return __mt76_cfg_read(wifi, MT76_ASIC_VERSION,
                           &priv->asic_rev, __mt76_read_asic_done);
}

static void __mt76_init_next(vsf_wifi_t *wifi, vsf_err_t err)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    vsf_wifi_done_t done = priv->pending_done;

    if (err != VSF_ERR_NONE) {
        priv->pending_done = NULL;
        if (done != NULL) done(wifi, err);
        return;
    }

    switch (priv->state) {
    case MT76_STATE_INIT_READ_ASIC: {
        uint8_t payload[8];
        __mt76_put_le32(payload + 0, MT76_FUN_Q_SELECT);
        __mt76_put_le32(payload + 4, 1);
        priv->state = MT76_STATE_INIT_MCU_Q_SELECT;
        vsf_err_t step_err = __mt76_mcu_msg_send(wifi, MT76_CMD_FUN_SET_OP,
                                    payload, sizeof(payload),
                                    __mt76_init_mcu_q_select_done);
        if (step_err != VSF_ERR_NONE) {
            __mt76_init_next(wifi, step_err);
        }
        break;
    }

    case MT76_STATE_INIT_MCU_Q_SELECT: {
        uint8_t payload[8];
        __mt76_put_le32(payload + 0, MT76_PWR_RADIO_ON);
        __mt76_put_le32(payload + 4, 0);
        priv->state = MT76_STATE_INIT_MCU_RADIO_ON;
        vsf_err_t step_err = __mt76_mcu_msg_send(wifi, MT76_CMD_POWER_SAVING_OP,
                                    payload, sizeof(payload),
                                    __mt76_init_mcu_radio_on_done);
        if (step_err != VSF_ERR_NONE) {
            __mt76_init_next(wifi, step_err);
        }
        break;
    }

    default:
        priv->pending_done = NULL;
        if (done != NULL) done(wifi, VSF_ERR_BUG);
        break;
    }
}

static void __mt76_on_rx(vsf_wifi_t *wifi, uint8_t *buf, uint16_t len)
{
    mt76_wifi_priv_t *priv = __mt76_priv(wifi);
    (void)priv;

    if (len < 4) return;

    uint32_t info = __mt76_get_le32(buf);
#if VSF_WIFI_CFG_CHIP_MT76_LOG_LEVEL >= 4
    uint8_t evt = (uint8_t)((info >> 20) & 0x7F);
    uint8_t seq = (uint8_t)((info >> 16) & 0x0F);

    vsf_wifi_chip_mt76_trace_debug(
        "mt76: rx evt=0x%02X seq=%u len=%u" VSF_TRACE_CFG_LINEEND,
        evt, seq, len);
#else
    (void)info;
#endif

    /* TODO: parse CMD_RESP / PKT_RX and dispatch to scan/connect/rate/etc. */
}

void __mt76_fini(vsf_wifi_t *wifi)
{
    /* TODO: stop TX/RX, power down */
    (void)wifi;
}

/*============================ WiFi hooks ====================================*/

vsf_err_t __mt76_set_channel(vsf_wifi_t *wifi, uint8_t channel,
                             vsf_wifi_done_t done)
{
    (void)wifi; (void)channel; (void)done;
    /* TODO: CMD_SWITCH_CHANNEL_OP via mcu_msg_send() */
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t __mt76_set_rx_filter(vsf_wifi_t *wifi, uint32_t mask,
                               vsf_wifi_done_t done)
{
    (void)wifi; (void)mask;
    /* STUB: allow VSF WiFi attach to complete.
     * TODO: write MT_RX_FILTR_CFG via cfg_write(). */
    if (done != NULL) done(wifi, VSF_ERR_NONE);
    return VSF_ERR_NONE;
}

vsf_err_t __mt76_set_mac_addr(vsf_wifi_t *wifi, const uint8_t mac[6],
                              vsf_wifi_done_t done)
{
    return __mt76_mac_addr_program_start(wifi, mac, done);
}

vsf_err_t __mt76_set_bssid(vsf_wifi_t *wifi, const uint8_t bssid[6],
                           vsf_wifi_done_t done)
{
    (void)wifi; (void)bssid; (void)done;
    /* TODO: program BSSID via cfg_write()/mcu_cmd() */
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t __mt76_set_auth_mode(vsf_wifi_t *wifi,
                               const vsf_wifi_auth_cfg_t *cfg,
                               vsf_wifi_done_t done)
{
    (void)wifi; (void)cfg; (void)done;
    /* TODO: store auth config for connect() */
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t __mt76_connect(vsf_wifi_t *wifi,
                         const uint8_t bssid[6], const uint8_t *ssid,
                         uint8_t ssid_len, uint8_t channel,
                         vsf_wifi_done_t done)
{
    (void)wifi; (void)bssid; (void)ssid; (void)ssid_len; (void)channel; (void)done;
    /* TODO: set BSSID/WCID/key via mcu_cmd, start rx_submit */
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t __mt76_disconnect(vsf_wifi_t *wifi, vsf_wifi_done_t done)
{
    /* TODO: clear BSSID/WCID via mcu_cmd, disable TX/RX */
    if (done != NULL) done(wifi, VSF_ERR_NOT_SUPPORT);
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t __mt76_get_link_info(vsf_wifi_t *wifi, vsf_wifi_link_info_t *info)
{
    (void)info;
    /* TODO: read RSSI/rate */
    return VSF_ERR_NOT_SUPPORT;
}

void __mt76_parse_rx(vsf_wifi_t *wifi, uint8_t *frame, uint16_t len)
{
    /* TODO: parse beacon/probe response during scan */
    (void)wifi; (void)frame; (void)len;
}

vsf_err_t __mt76_tx(vsf_wifi_t *wifi, const uint8_t *frame, uint16_t len)
{
    (void)wifi; (void)frame; (void)len;
    /* TODO: build TXINFO+TXWI and submit tx_frame(..., MT76_EP_OUT_AC_BE, NULL) */
    return VSF_ERR_NOT_SUPPORT;
}

const vsf_wifi_chip_drv_t vsf_wifi_mt76_drv = {
    .name           = "mt7612u",
    .firmware_load  = __mt76_firmware_load,
    .init           = __mt76_init,
    .fini           = __mt76_fini,
    .set_channel    = __mt76_set_channel,
    .set_rx_filter  = __mt76_set_rx_filter,
    .set_mac_addr   = __mt76_set_mac_addr,
    .set_bssid      = __mt76_set_bssid,
    .set_auth_mode  = __mt76_set_auth_mode,
    .connect        = __mt76_connect,
    .disconnect     = __mt76_disconnect,
    .get_link_info  = __mt76_get_link_info,
    .parse_rx       = __mt76_parse_rx,
    .build_tx       = NULL,
    .tx             = __mt76_tx,
};

#endif      /* VSF_USE_WIFI == ENABLED && VSF_WIFI_USE_MT76 == ENABLED */
