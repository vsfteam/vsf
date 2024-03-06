#ifndef __VSF_LINUX_IEEE80211_H__
#define __VSF_LINUX_IEEE80211_H__

#include <linux/types.h>
#include <linux/if_ether.h>
#include <linux/etherdevice.h>
#include <linux/bitfield.h>

#ifdef __cplusplus
extern "C" {
#endif

#define IEEE80211_FCTL_VERS             0x0003
#define IEEE80211_FCTL_FTYPE            0x000c
#define IEEE80211_FCTL_STYPE            0x00f0
#define IEEE80211_FCTL_TODS             0x0100
#define IEEE80211_FCTL_FROMDS           0x0200
#define IEEE80211_FCTL_MOREFRAGS        0x0400
#define IEEE80211_FCTL_RETRY            0x0800
#define IEEE80211_FCTL_PM               0x1000
#define IEEE80211_FCTL_MOREDATA         0x2000
#define IEEE80211_FCTL_PROTECTED        0x4000
#define IEEE80211_FCTL_ORDER            0x8000
#define IEEE80211_FCTL_CTL_EXT          0x0f00

#define IEEE80211_SCTL_FRAG             0x000F
#define IEEE80211_SCTL_SEQ              0xFFF0

#define IEEE80211_FTYPE_MGMT            0x0000
#define IEEE80211_FTYPE_CTL             0x0004
#define IEEE80211_FTYPE_DATA            0x0008
#define IEEE80211_FTYPE_EXT             0x000c

#define IEEE80211_STYPE_ASSOC_REQ       0x0000
#define IEEE80211_STYPE_ASSOC_RESP      0x0010
#define IEEE80211_STYPE_REASSOC_REQ     0x0020
#define IEEE80211_STYPE_REASSOC_RESP    0x0030
#define IEEE80211_STYPE_PROBE_REQ       0x0040
#define IEEE80211_STYPE_PROBE_RESP      0x0050
#define IEEE80211_STYPE_BEACON          0x0080
#define IEEE80211_STYPE_ATIM            0x0090
#define IEEE80211_STYPE_DISASSOC        0x00A0
#define IEEE80211_STYPE_AUTH            0x00B0
#define IEEE80211_STYPE_DEAUTH          0x00C0
#define IEEE80211_STYPE_ACTION          0x00D0

#define IEEE80211_STYPE_TRIGGER         0x0020
#define IEEE80211_STYPE_CTL_EXT         0x0060
#define IEEE80211_STYPE_BACK_REQ        0x0080
#define IEEE80211_STYPE_BACK            0x0090
#define IEEE80211_STYPE_PSPOLL          0x00A0
#define IEEE80211_STYPE_RTS             0x00B0
#define IEEE80211_STYPE_CTS             0x00C0
#define IEEE80211_STYPE_ACK             0x00D0
#define IEEE80211_STYPE_CFEND           0x00E0
#define IEEE80211_STYPE_CFENDACK        0x00F0

#define IEEE80211_STYPE_DATA            0x0000
#define IEEE80211_STYPE_DATA_CFACK      0x0010
#define IEEE80211_STYPE_DATA_CFPOLL     0x0020
#define IEEE80211_STYPE_DATA_CFACKPOLL  0x0030
#define IEEE80211_STYPE_NULLFUNC        0x0040
#define IEEE80211_STYPE_CFACK           0x0050
#define IEEE80211_STYPE_CFPOLL          0x0060
#define IEEE80211_STYPE_CFACKPOLL       0x0070
#define IEEE80211_STYPE_QOS_DATA        0x0080
#define IEEE80211_STYPE_QOS_DATA_CFACK  0x0090
#define IEEE80211_STYPE_QOS_DATA_CFPOLL 0x00A0
#define IEEE80211_STYPE_QOS_DATA_CFACKPOLL  0x00B0
#define IEEE80211_STYPE_QOS_NULLFUNC    0x00C0
#define IEEE80211_STYPE_QOS_CFACK       0x00D0
#define IEEE80211_STYPE_QOS_CFPOLL      0x00E0
#define IEEE80211_STYPE_QOS_CFACKPOLL   0x00F0

#define IEEE80211_STYPE_DMG_BEACON      0x0000
#define IEEE80211_STYPE_S1G_BEACON      0x0010

#define IEEE80211_S1G_BCN_NEXT_TBTT     0x100

#define IEEE80211_QOS_CTL_LEN           2

#define IEEE80211_HT_CTL_LEN            4

enum ieee80211_min_mpdu_spacing {
    IEEE80211_HT_MPDU_DENSITY_NONE      = 0,    /* No restriction */
    IEEE80211_HT_MPDU_DENSITY_0_25      = 1,    /* 1/4 usec */
    IEEE80211_HT_MPDU_DENSITY_0_5       = 2,    /* 1/2 usec */
    IEEE80211_HT_MPDU_DENSITY_1         = 3,    /* 1 usec */
    IEEE80211_HT_MPDU_DENSITY_2         = 4,    /* 2 usec */
    IEEE80211_HT_MPDU_DENSITY_4         = 5,    /* 4 usec */
    IEEE80211_HT_MPDU_DENSITY_8         = 6,    /* 8 usec */
    IEEE80211_HT_MPDU_DENSITY_16        = 7,    /* 16 usec */
};

enum ieee80211_s1g_chanwidth {
    IEEE80211_S1G_CHANWIDTH_1MHZ        = 0,
    IEEE80211_S1G_CHANWIDTH_2MHZ        = 1,
    IEEE80211_S1G_CHANWIDTH_4MHZ        = 3,
    IEEE80211_S1G_CHANWIDTH_8MHZ        = 7,
    IEEE80211_S1G_CHANWIDTH_16MHZ       = 15,
};

struct ieee80211_hdr {
    __le16 frame_control;
    __le16 duration_id;
    struct_group(addrs,
        u8 addr1[ETH_ALEN];
        u8 addr2[ETH_ALEN];
        u8 addr3[ETH_ALEN];
    );
    __le16 seq_ctrl;
    u8 addr4[ETH_ALEN];
} VSF_CAL_PACKED VSF_CAL_ALIGN(2);

struct ieee80211_hdr_3addr {
    __le16 frame_control;
    __le16 duration_id;
    u8 addr1[ETH_ALEN];
    u8 addr2[ETH_ALEN];
    u8 addr3[ETH_ALEN];
    __le16 seq_ctrl;
} VSF_CAL_PACKED VSF_CAL_ALIGN(2);

struct ieee80211_qos_hdr {
    __le16 frame_control;
    __le16 duration_id;
    u8 addr1[ETH_ALEN];
    u8 addr2[ETH_ALEN];
    u8 addr3[ETH_ALEN];
    __le16 seq_ctrl;
    __le16 qos_ctrl;
} VSF_CAL_PACKED VSF_CAL_ALIGN(2);

#define WLAN_SA_QUERY_TR_ID_LEN         2
#define WLAN_MEMBERSHIP_LEN             8
#define WLAN_USER_POSITION_LEN          16

struct ieee80211_quiet_ie {
    u8 count;
    u8 period;
    __le16 duration;
    __le16 offset;
} VSF_CAL_PACKED;

struct ieee80211_msrment_ie {
    u8 token;
    u8 mode;
    u8 type;
    u8 request[0];
} VSF_CAL_PACKED;

struct ieee80211_channel_sw_ie {
    u8 mode;
    u8 new_ch_num;
    u8 count;
} VSF_CAL_PACKED;

struct ieee80211_ext_chansw_ie {
    u8 mode;
    u8 new_operating_class;
    u8 new_ch_num;
    u8 count;
} VSF_CAL_PACKED;

struct ieee80211_sec_chan_offs_ie {
    u8 sec_chan_offs;
} VSF_CAL_PACKED;

struct ieee80211_mesh_chansw_params_ie {
    u8 mesh_ttl;
    u8 mesh_flags;
    __le16 mesh_reason;
    __le16 mesh_pre_value;
} VSF_CAL_PACKED;

struct ieee80211_wide_bw_chansw_ie {
    u8 new_channel_width;
    u8 new_center_freq_seg0, new_center_freq_seg1;
} VSF_CAL_PACKED;

struct ieee80211_tim_ie {
    u8 dtim_count;
    u8 dtim_period;
    u8 bitmap_ctrl;
    /* variable size: 1 - 251 bytes */
    u8 virtual_map[1];
} VSF_CAL_PACKED;

struct ieee80211_meshconf_ie {
    u8 meshconf_psel;
    u8 meshconf_pmetric;
    u8 meshconf_congest;
    u8 meshconf_synch;
    u8 meshconf_auth;
    u8 meshconf_form;
    u8 meshconf_cap;
} VSF_CAL_PACKED;

struct ieee80211_tpc_report_ie {
    u8 tx_power;
    u8 link_margin;
} VSF_CAL_PACKED;

struct ieee80211_addba_ext_ie {
    u8 data;
} VSF_CAL_PACKED;

struct ieee80211_s1g_bcn_compat_ie {
    __le16 compat_info;
    __le16 beacon_int;
    __le32 tsf_completion;
} VSF_CAL_PACKED;

struct ieee80211_s1g_oper_ie {
    u8 ch_width;
    u8 oper_class;
    u8 primary_ch;
    u8 oper_ch;
    __le16 basic_mcs_nss;
} VSF_CAL_PACKED;

struct ieee80211_aid_response_ie {
    __le16 aid;
    u8 switch_count;
    __le16 response_int;
} VSF_CAL_PACKED;

struct ieee80211_mmie {
    u8 element_id;
    u8 length;
    __le16 key_id;
    u8 sequence_number[6];
    u8 mic[8];
} VSF_CAL_PACKED;

struct ieee80211_mmie_16 {
    u8 element_id;
    u8 length;
    __le16 key_id;
    u8 sequence_number[6];
    u8 mic[16];
} VSF_CAL_PACKED;

struct ieee80211_vendor_ie {
    u8 element_id;
    u8 len;
    u8 oui[3];
    u8 oui_type;
} VSF_CAL_PACKED;

struct ieee80211_wmm_ac_param {
    u8 aci_aifsn;
    u8 cw;
    __le16 txop_limit;
} VSF_CAL_PACKED;

struct ieee80211_wmm_param_ie {
    u8 element_id;
    u8 len;
    u8 oui[3];
    u8 oui_type;
    u8 oui_subtype;
    u8 version;
    u8 qos_info;
    u8 reserved;
    struct ieee80211_wmm_ac_param ac[4];
} VSF_CAL_PACKED;

struct ieee80211_rts {
    __le16 frame_control;
    __le16 duration;
    u8 ra[ETH_ALEN];
    u8 ta[ETH_ALEN];
} VSF_CAL_PACKED VSF_CAL_ALIGN(2);

struct ieee80211_cts {
    __le16 frame_control;
    __le16 duration;
    u8 ra[ETH_ALEN];
} VSF_CAL_PACKED VSF_CAL_ALIGN(2);

struct ieee80211_pspoll {
    __le16 frame_control;
    __le16 aid;
    u8 bssid[ETH_ALEN];
    u8 ta[ETH_ALEN];
} VSF_CAL_PACKED VSF_CAL_ALIGN(2);

/* Information Element IDs */
enum ieee80211_eid {
    WLAN_EID_SSID = 0,
    WLAN_EID_SUPP_RATES = 1,
    WLAN_EID_FH_PARAMS = 2, /* reserved now */
    WLAN_EID_DS_PARAMS = 3,
    WLAN_EID_CF_PARAMS = 4,
    WLAN_EID_TIM = 5,
    WLAN_EID_IBSS_PARAMS = 6,
    WLAN_EID_COUNTRY = 7,
    /* 8, 9 reserved */
    WLAN_EID_REQUEST = 10,
    WLAN_EID_QBSS_LOAD = 11,
    WLAN_EID_EDCA_PARAM_SET = 12,
    WLAN_EID_TSPEC = 13,
    WLAN_EID_TCLAS = 14,
    WLAN_EID_SCHEDULE = 15,
    WLAN_EID_CHALLENGE = 16,
    /* 17-31 reserved for challenge text extension */
    WLAN_EID_PWR_CONSTRAINT = 32,
    WLAN_EID_PWR_CAPABILITY = 33,
    WLAN_EID_TPC_REQUEST = 34,
    WLAN_EID_TPC_REPORT = 35,
    WLAN_EID_SUPPORTED_CHANNELS = 36,
    WLAN_EID_CHANNEL_SWITCH = 37,
    WLAN_EID_MEASURE_REQUEST = 38,
    WLAN_EID_MEASURE_REPORT = 39,
    WLAN_EID_QUIET = 40,
    WLAN_EID_IBSS_DFS = 41,
    WLAN_EID_ERP_INFO = 42,
    WLAN_EID_TS_DELAY = 43,
    WLAN_EID_TCLAS_PROCESSING = 44,
    WLAN_EID_HT_CAPABILITY = 45,
    WLAN_EID_QOS_CAPA = 46,
    /* 47 reserved for Broadcom */
    WLAN_EID_RSN = 48,
    WLAN_EID_802_15_COEX = 49,
    WLAN_EID_EXT_SUPP_RATES = 50,
    WLAN_EID_AP_CHAN_REPORT = 51,
    WLAN_EID_NEIGHBOR_REPORT = 52,
    WLAN_EID_RCPI = 53,
    WLAN_EID_MOBILITY_DOMAIN = 54,
    WLAN_EID_FAST_BSS_TRANSITION = 55,
    WLAN_EID_TIMEOUT_INTERVAL = 56,
    WLAN_EID_RIC_DATA = 57,
    WLAN_EID_DSE_REGISTERED_LOCATION = 58,
    WLAN_EID_SUPPORTED_REGULATORY_CLASSES = 59,
    WLAN_EID_EXT_CHANSWITCH_ANN = 60,
    WLAN_EID_HT_OPERATION = 61,
    WLAN_EID_SECONDARY_CHANNEL_OFFSET = 62,
    WLAN_EID_BSS_AVG_ACCESS_DELAY = 63,
    WLAN_EID_ANTENNA_INFO = 64,
    WLAN_EID_RSNI = 65,
    WLAN_EID_MEASUREMENT_PILOT_TX_INFO = 66,
    WLAN_EID_BSS_AVAILABLE_CAPACITY = 67,
    WLAN_EID_BSS_AC_ACCESS_DELAY = 68,
    WLAN_EID_TIME_ADVERTISEMENT = 69,
    WLAN_EID_RRM_ENABLED_CAPABILITIES = 70,
    WLAN_EID_MULTIPLE_BSSID = 71,
    WLAN_EID_BSS_COEX_2040 = 72,
    WLAN_EID_BSS_INTOLERANT_CHL_REPORT = 73,
    WLAN_EID_OVERLAP_BSS_SCAN_PARAM = 74,
    WLAN_EID_RIC_DESCRIPTOR = 75,
    WLAN_EID_MMIE = 76,
    WLAN_EID_ASSOC_COMEBACK_TIME = 77,
    WLAN_EID_EVENT_REQUEST = 78,
    WLAN_EID_EVENT_REPORT = 79,
    WLAN_EID_DIAGNOSTIC_REQUEST = 80,
    WLAN_EID_DIAGNOSTIC_REPORT = 81,
    WLAN_EID_LOCATION_PARAMS = 82,
    WLAN_EID_NON_TX_BSSID_CAP =  83,
    WLAN_EID_SSID_LIST = 84,
    WLAN_EID_MULTI_BSSID_IDX = 85,
    WLAN_EID_FMS_DESCRIPTOR = 86,
    WLAN_EID_FMS_REQUEST = 87,
    WLAN_EID_FMS_RESPONSE = 88,
    WLAN_EID_QOS_TRAFFIC_CAPA = 89,
    WLAN_EID_BSS_MAX_IDLE_PERIOD = 90,
    WLAN_EID_TSF_REQUEST = 91,
    WLAN_EID_TSF_RESPOSNE = 92,
    WLAN_EID_WNM_SLEEP_MODE = 93,
    WLAN_EID_TIM_BCAST_REQ = 94,
    WLAN_EID_TIM_BCAST_RESP = 95,
    WLAN_EID_COLL_IF_REPORT = 96,
    WLAN_EID_CHANNEL_USAGE = 97,
    WLAN_EID_TIME_ZONE = 98,
    WLAN_EID_DMS_REQUEST = 99,
    WLAN_EID_DMS_RESPONSE = 100,
    WLAN_EID_LINK_ID = 101,
    WLAN_EID_WAKEUP_SCHEDUL = 102,
    /* 103 reserved */
    WLAN_EID_CHAN_SWITCH_TIMING = 104,
    WLAN_EID_PTI_CONTROL = 105,
    WLAN_EID_PU_BUFFER_STATUS = 106,
    WLAN_EID_INTERWORKING = 107,
    WLAN_EID_ADVERTISEMENT_PROTOCOL = 108,
    WLAN_EID_EXPEDITED_BW_REQ = 109,
    WLAN_EID_QOS_MAP_SET = 110,
    WLAN_EID_ROAMING_CONSORTIUM = 111,
    WLAN_EID_EMERGENCY_ALERT = 112,
    WLAN_EID_MESH_CONFIG = 113,
    WLAN_EID_MESH_ID = 114,
    WLAN_EID_LINK_METRIC_REPORT = 115,
    WLAN_EID_CONGESTION_NOTIFICATION = 116,
    WLAN_EID_PEER_MGMT = 117,
    WLAN_EID_CHAN_SWITCH_PARAM = 118,
    WLAN_EID_MESH_AWAKE_WINDOW = 119,
    WLAN_EID_BEACON_TIMING = 120,
    WLAN_EID_MCCAOP_SETUP_REQ = 121,
    WLAN_EID_MCCAOP_SETUP_RESP = 122,
    WLAN_EID_MCCAOP_ADVERT = 123,
    WLAN_EID_MCCAOP_TEARDOWN = 124,
    WLAN_EID_GANN = 125,
    WLAN_EID_RANN = 126,
    WLAN_EID_EXT_CAPABILITY = 127,
    /* 128, 129 reserved for Agere */
    WLAN_EID_PREQ = 130,
    WLAN_EID_PREP = 131,
    WLAN_EID_PERR = 132,
    /* 133-136 reserved for Cisco */
    WLAN_EID_PXU = 137,
    WLAN_EID_PXUC = 138,
    WLAN_EID_AUTH_MESH_PEER_EXCH = 139,
    WLAN_EID_MIC = 140,
    WLAN_EID_DESTINATION_URI = 141,
    WLAN_EID_UAPSD_COEX = 142,
    WLAN_EID_WAKEUP_SCHEDULE = 143,
    WLAN_EID_EXT_SCHEDULE = 144,
    WLAN_EID_STA_AVAILABILITY = 145,
    WLAN_EID_DMG_TSPEC = 146,
    WLAN_EID_DMG_AT = 147,
    WLAN_EID_DMG_CAP = 148,
    /* 149 reserved for Cisco */
    WLAN_EID_CISCO_VENDOR_SPECIFIC = 150,
    WLAN_EID_DMG_OPERATION = 151,
    WLAN_EID_DMG_BSS_PARAM_CHANGE = 152,
    WLAN_EID_DMG_BEAM_REFINEMENT = 153,
    WLAN_EID_CHANNEL_MEASURE_FEEDBACK = 154,
    /* 155-156 reserved for Cisco */
    WLAN_EID_AWAKE_WINDOW = 157,
    WLAN_EID_MULTI_BAND = 158,
    WLAN_EID_ADDBA_EXT = 159,
    WLAN_EID_NEXT_PCP_LIST = 160,
    WLAN_EID_PCP_HANDOVER = 161,
    WLAN_EID_DMG_LINK_MARGIN = 162,
    WLAN_EID_SWITCHING_STREAM = 163,
    WLAN_EID_SESSION_TRANSITION = 164,
    WLAN_EID_DYN_TONE_PAIRING_REPORT = 165,
    WLAN_EID_CLUSTER_REPORT = 166,
    WLAN_EID_RELAY_CAP = 167,
    WLAN_EID_RELAY_XFER_PARAM_SET = 168,
    WLAN_EID_BEAM_LINK_MAINT = 169,
    WLAN_EID_MULTIPLE_MAC_ADDR = 170,
    WLAN_EID_U_PID = 171,
    WLAN_EID_DMG_LINK_ADAPT_ACK = 172,
    /* 173 reserved for Symbol */
    WLAN_EID_MCCAOP_ADV_OVERVIEW = 174,
    WLAN_EID_QUIET_PERIOD_REQ = 175,
    /* 176 reserved for Symbol */
    WLAN_EID_QUIET_PERIOD_RESP = 177,
    /* 178-179 reserved for Symbol */
    /* 180 reserved for ISO/IEC 20011 */
    WLAN_EID_EPAC_POLICY = 182,
    WLAN_EID_CLISTER_TIME_OFF = 183,
    WLAN_EID_INTER_AC_PRIO = 184,
    WLAN_EID_SCS_DESCRIPTOR = 185,
    WLAN_EID_QLOAD_REPORT = 186,
    WLAN_EID_HCCA_TXOP_UPDATE_COUNT = 187,
    WLAN_EID_HL_STREAM_ID = 188,
    WLAN_EID_GCR_GROUP_ADDR = 189,
    WLAN_EID_ANTENNA_SECTOR_ID_PATTERN = 190,
    WLAN_EID_VHT_CAPABILITY = 191,
    WLAN_EID_VHT_OPERATION = 192,
    WLAN_EID_EXTENDED_BSS_LOAD = 193,
    WLAN_EID_WIDE_BW_CHANNEL_SWITCH = 194,
    WLAN_EID_TX_POWER_ENVELOPE = 195,
    WLAN_EID_CHANNEL_SWITCH_WRAPPER = 196,
    WLAN_EID_AID = 197,
    WLAN_EID_QUIET_CHANNEL = 198,
    WLAN_EID_OPMODE_NOTIF = 199,

    WLAN_EID_REDUCED_NEIGHBOR_REPORT = 201,

    WLAN_EID_AID_REQUEST = 210,
    WLAN_EID_AID_RESPONSE = 211,
    WLAN_EID_S1G_BCN_COMPAT = 213,
    WLAN_EID_S1G_SHORT_BCN_INTERVAL = 214,
    WLAN_EID_S1G_TWT = 216,
    WLAN_EID_S1G_CAPABILITIES = 217,
    WLAN_EID_VENDOR_SPECIFIC = 221,
    WLAN_EID_QOS_PARAMETER = 222,
    WLAN_EID_S1G_OPERATION = 232,
    WLAN_EID_CAG_NUMBER = 237,
    WLAN_EID_AP_CSN = 239,
    WLAN_EID_FILS_INDICATION = 240,
    WLAN_EID_DILS = 241,
    WLAN_EID_FRAGMENT = 242,
    WLAN_EID_RSNX = 244,
    WLAN_EID_EXTENSION = 255
};

/* Element ID Extensions for Element ID 255 */
enum ieee80211_eid_ext {
    WLAN_EID_EXT_ASSOC_DELAY_INFO = 1,
    WLAN_EID_EXT_FILS_REQ_PARAMS = 2,
    WLAN_EID_EXT_FILS_KEY_CONFIRM = 3,
    WLAN_EID_EXT_FILS_SESSION = 4,
    WLAN_EID_EXT_FILS_HLP_CONTAINER = 5,
    WLAN_EID_EXT_FILS_IP_ADDR_ASSIGN = 6,
    WLAN_EID_EXT_KEY_DELIVERY = 7,
    WLAN_EID_EXT_FILS_WRAPPED_DATA = 8,
    WLAN_EID_EXT_FILS_PUBLIC_KEY = 12,
    WLAN_EID_EXT_FILS_NONCE = 13,
    WLAN_EID_EXT_FUTURE_CHAN_GUIDANCE = 14,
    WLAN_EID_EXT_HE_CAPABILITY = 35,
    WLAN_EID_EXT_HE_OPERATION = 36,
    WLAN_EID_EXT_UORA = 37,
    WLAN_EID_EXT_HE_MU_EDCA = 38,
    WLAN_EID_EXT_HE_SPR = 39,
    WLAN_EID_EXT_NDP_FEEDBACK_REPORT_PARAMSET = 41,
    WLAN_EID_EXT_BSS_COLOR_CHG_ANN = 42,
    WLAN_EID_EXT_QUIET_TIME_PERIOD_SETUP = 43,
    WLAN_EID_EXT_ESS_REPORT = 45,
    WLAN_EID_EXT_OPS = 46,
    WLAN_EID_EXT_HE_BSS_LOAD = 47,
    WLAN_EID_EXT_MAX_CHANNEL_SWITCH_TIME = 52,
    WLAN_EID_EXT_MULTIPLE_BSSID_CONFIGURATION = 55,
    WLAN_EID_EXT_NON_INHERITANCE = 56,
    WLAN_EID_EXT_KNOWN_BSSID = 57,
    WLAN_EID_EXT_SHORT_SSID_LIST = 58,
    WLAN_EID_EXT_HE_6GHZ_CAPA = 59,
    WLAN_EID_EXT_UL_MU_POWER_CAPA = 60,
    WLAN_EID_EXT_EHT_OPERATION = 106,
    WLAN_EID_EXT_EHT_MULTI_LINK = 107,
    WLAN_EID_EXT_EHT_CAPABILITY = 108,
};

#define WLAN_OUI_WFA                0x506f9a
#define WLAN_OUI_TYPE_WFA_P2P       9
#define WLAN_OUI_TYPE_WFA_DPP       0x1A
#define WLAN_OUI_MICROSOFT          0x0050f2
#define WLAN_OUI_TYPE_MICROSOFT_WPA 1
#define WLAN_OUI_TYPE_MICROSOFT_WMM 2
#define WLAN_OUI_TYPE_MICROSOFT_WPS 4
#define WLAN_OUI_TYPE_MICROSOFT_TPC 8

struct ieee80211_mgmt {
    __le16 frame_control;
    __le16 duration;
    u8 da[ETH_ALEN];
    u8 sa[ETH_ALEN];
    u8 bssid[ETH_ALEN];
    __le16 seq_ctrl;
    union {
        struct {
            __le16 auth_alg;
            __le16 auth_transaction;
            __le16 status_code;
            /* possibly followed by Challenge text */
            u8 variable[0];
        } VSF_CAL_PACKED auth;
        struct {
            __le16 reason_code;
        } VSF_CAL_PACKED deauth;
        struct {
            __le16 capab_info;
            __le16 listen_interval;
            /* followed by SSID and Supported rates */
            u8 variable[0];
        } VSF_CAL_PACKED assoc_req;
        struct {
            __le16 capab_info;
            __le16 status_code;
            __le16 aid;
            /* followed by Supported rates */
            u8 variable[0];
        } VSF_CAL_PACKED assoc_resp, reassoc_resp;
        struct {
            __le16 capab_info;
            __le16 status_code;
            u8 variable[0];
        } VSF_CAL_PACKED s1g_assoc_resp, s1g_reassoc_resp;
        struct {
            __le16 capab_info;
            __le16 listen_interval;
            u8 current_ap[ETH_ALEN];
            /* followed by SSID and Supported rates */
            u8 variable[0];
        } VSF_CAL_PACKED reassoc_req;
        struct {
            __le16 reason_code;
        } VSF_CAL_PACKED disassoc;
        struct {
            __le64 timestamp;
            __le16 beacon_int;
            __le16 capab_info;
            /* followed by some of SSID, Supported rates,
             * FH Params, DS Params, CF Params, IBSS Params, TIM */
            u8 variable[0];
        } VSF_CAL_PACKED beacon;
        struct {
            /* only variable items: SSID, Supported rates */
            u8 variable[1];
        } VSF_CAL_PACKED probe_req;
        struct {
            __le64 timestamp;
            __le16 beacon_int;
            __le16 capab_info;
            /* followed by some of SSID, Supported rates,
             * FH Params, DS Params, CF Params, IBSS Params */
            u8 variable[0];
        } VSF_CAL_PACKED probe_resp;
        struct {
            u8 category;
            union {
                struct {
                    u8 action_code;
                    u8 dialog_token;
                    u8 status_code;
                    u8 variable[0];
                } VSF_CAL_PACKED wme_action;
                struct{
                    u8 action_code;
                    u8 variable[0];
                } VSF_CAL_PACKED chan_switch;
                struct{
                    u8 action_code;
                    struct ieee80211_ext_chansw_ie data;
                    u8 variable[0];
                } VSF_CAL_PACKED ext_chan_switch;
                struct{
                    u8 action_code;
                    u8 dialog_token;
                    u8 element_id;
                    u8 length;
                    struct ieee80211_msrment_ie msr_elem;
                } VSF_CAL_PACKED measurement;
                struct{
                    u8 action_code;
                    u8 dialog_token;
                    __le16 capab;
                    __le16 timeout;
                    __le16 start_seq_num;
                    /* followed by BA Extension */
                    u8 variable[0];
                } VSF_CAL_PACKED addba_req;
                struct{
                    u8 action_code;
                    u8 dialog_token;
                    __le16 status;
                    __le16 capab;
                    __le16 timeout;
                } VSF_CAL_PACKED addba_resp;
                struct{
                    u8 action_code;
                    __le16 params;
                    __le16 reason_code;
                } VSF_CAL_PACKED delba;
                struct {
                    u8 action_code;
                    u8 variable[0];
                } VSF_CAL_PACKED self_prot;
                struct{
                    u8 action_code;
                    u8 variable[0];
                } VSF_CAL_PACKED mesh_action;
                struct {
                    u8 action;
                    u8 trans_id[WLAN_SA_QUERY_TR_ID_LEN];
                } VSF_CAL_PACKED sa_query;
                struct {
                    u8 action;
                    u8 smps_control;
                } VSF_CAL_PACKED ht_smps;
                struct {
                    u8 action_code;
                    u8 chanwidth;
                } VSF_CAL_PACKED ht_notify_cw;
                struct {
                    u8 action_code;
                    u8 dialog_token;
                    __le16 capability;
                    u8 variable[0];
                } VSF_CAL_PACKED tdls_discover_resp;
                struct {
                    u8 action_code;
                    u8 operating_mode;
                } VSF_CAL_PACKED vht_opmode_notif;
                struct {
                    u8 action_code;
                    u8 membership[WLAN_MEMBERSHIP_LEN];
                    u8 position[WLAN_USER_POSITION_LEN];
                } VSF_CAL_PACKED vht_group_notif;
                struct {
                    u8 action_code;
                    u8 dialog_token;
                    u8 tpc_elem_id;
                    u8 tpc_elem_length;
                    struct ieee80211_tpc_report_ie tpc;
                } VSF_CAL_PACKED tpc_report;
                struct {
                    u8 action_code;
                    u8 dialog_token;
                    u8 follow_up;
                    u8 tod[6];
                    u8 toa[6];
                    __le16 tod_error;
                    __le16 toa_error;
                    u8 variable[0];
                } VSF_CAL_PACKED ftm;
                struct {
                    u8 action_code;
                    u8 variable[0];
                } VSF_CAL_PACKED s1g;
                struct {
                    u8 action_code;
                    u8 dialog_token;
                    u8 follow_up;
                    u32 tod;
                    u32 toa;
                    u8 max_tod_error;
                    u8 max_toa_error;
                } VSF_CAL_PACKED wnm_timing_msr;
            } u;
        } VSF_CAL_PACKED action;
    } u;
} VSF_CAL_PACKED VSF_CAL_ALIGN(2);

static inline bool ieee80211_has_tods(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_TODS)) != 0;
}
static inline bool ieee80211_has_fromds(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_FROMDS)) != 0;
}
static inline bool ieee80211_has_a4(__le16 fc)
{
    __le16 tmp = cpu_to_le16(IEEE80211_FCTL_TODS | IEEE80211_FCTL_FROMDS);
    return (fc & tmp) == tmp;
}
static inline bool ieee80211_has_morefrags(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_MOREFRAGS)) != 0;
}
static inline bool ieee80211_has_retry(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_RETRY)) != 0;
}
static inline bool ieee80211_has_pm(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_PM)) != 0;
}
static inline bool ieee80211_has_moredata(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_MOREDATA)) != 0;
}
static inline bool ieee80211_has_protected(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_PROTECTED)) != 0;
}
static inline bool ieee80211_has_order(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_ORDER)) != 0;
}
static inline bool ieee80211_is_mgmt(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE)) == cpu_to_le16(IEEE80211_FTYPE_MGMT);
}
static inline bool ieee80211_is_ctl(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE)) == cpu_to_le16(IEEE80211_FTYPE_CTL);
}
static inline bool ieee80211_is_data(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE)) == cpu_to_le16(IEEE80211_FTYPE_DATA);
}
static inline bool ieee80211_is_ext(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE)) == cpu_to_le16(IEEE80211_FTYPE_EXT);
}
static inline bool ieee80211_is_data_qos(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_STYPE_QOS_DATA)) ==
            cpu_to_le16(IEEE80211_FTYPE_DATA | IEEE80211_STYPE_QOS_DATA);
}
static inline bool ieee80211_is_data_present(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | 0x40)) == cpu_to_le16(IEEE80211_FTYPE_DATA);
}
static inline bool ieee80211_is_assoc_req(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
            cpu_to_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_ASSOC_REQ);
}
static inline bool ieee80211_is_assoc_resp(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
            cpu_to_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_ASSOC_RESP);
}
static inline bool ieee80211_is_reassoc_req(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
            cpu_to_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_REASSOC_REQ);
}
static inline bool ieee80211_is_reassoc_resp(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
            cpu_to_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_REASSOC_RESP);
}
static inline bool ieee80211_is_probe_req(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
            cpu_to_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_PROBE_REQ);
}
static inline bool ieee80211_is_probe_resp(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
            cpu_to_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_PROBE_RESP);
}
static inline bool ieee80211_is_beacon(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
            cpu_to_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_BEACON);
}
static inline bool ieee80211_is_s1g_beacon(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
            cpu_to_le16(IEEE80211_FTYPE_EXT | IEEE80211_STYPE_S1G_BEACON);
}
static inline bool ieee80211_next_tbtt_present(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
            cpu_to_le16(IEEE80211_FTYPE_EXT | IEEE80211_STYPE_S1G_BEACON) &&
            fc & cpu_to_le16(IEEE80211_S1G_BCN_NEXT_TBTT);
}
static inline bool ieee80211_is_s1g_short_beacon(__le16 fc)
{
    return ieee80211_is_s1g_beacon(fc) && ieee80211_next_tbtt_present(fc);
}
static inline bool ieee80211_is_atim(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
            cpu_to_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_ATIM);
}
static inline bool ieee80211_is_disassoc(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
            cpu_to_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_DISASSOC);
}
static inline bool ieee80211_is_auth(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
            cpu_to_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_AUTH);
}
static inline bool ieee80211_is_deauth(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
            cpu_to_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_DEAUTH);
}
static inline bool ieee80211_is_action(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
            cpu_to_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_ACTION);
}
static inline bool ieee80211_is_back_req(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
            cpu_to_le16(IEEE80211_FTYPE_CTL | IEEE80211_STYPE_BACK_REQ);
}
static inline bool ieee80211_is_back(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
            cpu_to_le16(IEEE80211_FTYPE_CTL | IEEE80211_STYPE_BACK);
}
static inline bool ieee80211_is_pspoll(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
            cpu_to_le16(IEEE80211_FTYPE_CTL | IEEE80211_STYPE_PSPOLL);
}
static inline bool ieee80211_is_rts(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
            cpu_to_le16(IEEE80211_FTYPE_CTL | IEEE80211_STYPE_RTS);
}
static inline bool ieee80211_is_cts(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
            cpu_to_le16(IEEE80211_FTYPE_CTL | IEEE80211_STYPE_CTS);
}
static inline bool ieee80211_is_ack(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
            cpu_to_le16(IEEE80211_FTYPE_CTL | IEEE80211_STYPE_ACK);
}
static inline bool ieee80211_is_cfend(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
            cpu_to_le16(IEEE80211_FTYPE_CTL | IEEE80211_STYPE_CFEND);
}
static inline bool ieee80211_is_cfendack(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
            cpu_to_le16(IEEE80211_FTYPE_CTL | IEEE80211_STYPE_CFENDACK);
}
static inline bool ieee80211_is_nullfunc(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
            cpu_to_le16(IEEE80211_FTYPE_DATA | IEEE80211_STYPE_NULLFUNC);
}
static inline bool ieee80211_is_qos_nullfunc(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
            cpu_to_le16(IEEE80211_FTYPE_DATA | IEEE80211_STYPE_QOS_NULLFUNC);
}
static inline bool ieee80211_is_trigger(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
            cpu_to_le16(IEEE80211_FTYPE_CTL | IEEE80211_STYPE_TRIGGER);
}
static inline bool ieee80211_is_any_nullfunc(__le16 fc)
{
    return (ieee80211_is_nullfunc(fc) || ieee80211_is_qos_nullfunc(fc));
}
static inline bool ieee80211_is_bufferable_mmpdu(__le16 fc)
{
    return ieee80211_is_mgmt(fc) && (ieee80211_is_action(fc) || ieee80211_is_disassoc(fc) || ieee80211_is_deauth(fc));
}
static inline bool ieee80211_is_first_frag(__le16 seq_ctrl)
{
    return (seq_ctrl & cpu_to_le16(IEEE80211_SCTL_FRAG)) == 0;
}

extern unsigned int ieee80211_get_hdrlen_from_skb(const struct sk_buff *skb);

#ifdef __cplusplus
}
#endif

#endif
