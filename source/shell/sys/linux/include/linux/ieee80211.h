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
} __packed __aligned(2);

struct ieee80211_hdr_3addr {
    __le16 frame_control;
    __le16 duration_id;
    u8 addr1[ETH_ALEN];
    u8 addr2[ETH_ALEN];
    u8 addr3[ETH_ALEN];
    __le16 seq_ctrl;
} __packed __aligned(2);

struct ieee80211_qos_hdr {
    __le16 frame_control;
    __le16 duration_id;
    u8 addr1[ETH_ALEN];
    u8 addr2[ETH_ALEN];
    u8 addr3[ETH_ALEN];
    __le16 seq_ctrl;
    __le16 qos_ctrl;
} __packed __aligned(2);

#define WLAN_SA_QUERY_TR_ID_LEN         2
#define WLAN_MEMBERSHIP_LEN             8
#define WLAN_USER_POSITION_LEN          16

struct ieee80211_quiet_ie {
    u8 count;
    u8 period;
    __le16 duration;
    __le16 offset;
} __packed;

struct ieee80211_msrment_ie {
    u8 token;
    u8 mode;
    u8 type;
    u8 request[];
} __packed;

struct ieee80211_channel_sw_ie {
    u8 mode;
    u8 new_ch_num;
    u8 count;
} __packed;

struct ieee80211_ext_chansw_ie {
    u8 mode;
    u8 new_operating_class;
    u8 new_ch_num;
    u8 count;
} __packed;

struct ieee80211_sec_chan_offs_ie {
    u8 sec_chan_offs;
} __packed;

struct ieee80211_mesh_chansw_params_ie {
    u8 mesh_ttl;
    u8 mesh_flags;
    __le16 mesh_reason;
    __le16 mesh_pre_value;
} __packed;

struct ieee80211_wide_bw_chansw_ie {
    u8 new_channel_width;
    u8 new_center_freq_seg0, new_center_freq_seg1;
} __packed;

struct ieee80211_tim_ie {
    u8 dtim_count;
    u8 dtim_period;
    u8 bitmap_ctrl;
    /* variable size: 1 - 251 bytes */
    u8 virtual_map[1];
} __packed;

struct ieee80211_meshconf_ie {
    u8 meshconf_psel;
    u8 meshconf_pmetric;
    u8 meshconf_congest;
    u8 meshconf_synch;
    u8 meshconf_auth;
    u8 meshconf_form;
    u8 meshconf_cap;
} __packed;

struct ieee80211_tpc_report_ie {
    u8 tx_power;
    u8 link_margin;
} __packed;

struct ieee80211_addba_ext_ie {
    u8 data;
} __packed;

struct ieee80211_s1g_bcn_compat_ie {
    __le16 compat_info;
    __le16 beacon_int;
    __le32 tsf_completion;
} __packed;

struct ieee80211_s1g_oper_ie {
    u8 ch_width;
    u8 oper_class;
    u8 primary_ch;
    u8 oper_ch;
    __le16 basic_mcs_nss;
} __packed;

struct ieee80211_aid_response_ie {
    __le16 aid;
    u8 switch_count;
    __le16 response_int;
} __packed;

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
            u8 variable[];
        } __packed auth;
        struct {
            __le16 reason_code;
        } __packed deauth;
        struct {
            __le16 capab_info;
            __le16 listen_interval;
            /* followed by SSID and Supported rates */
            u8 variable[];
        } __packed assoc_req;
        struct {
            __le16 capab_info;
            __le16 status_code;
            __le16 aid;
            /* followed by Supported rates */
            u8 variable[];
        } __packed assoc_resp, reassoc_resp;
        struct {
            __le16 capab_info;
            __le16 status_code;
            u8 variable[];
        } __packed s1g_assoc_resp, s1g_reassoc_resp;
        struct {
            __le16 capab_info;
            __le16 listen_interval;
            u8 current_ap[ETH_ALEN];
            /* followed by SSID and Supported rates */
            u8 variable[];
        } __packed reassoc_req;
        struct {
            __le16 reason_code;
        } __packed disassoc;
        struct {
            __le64 timestamp;
            __le16 beacon_int;
            __le16 capab_info;
            /* followed by some of SSID, Supported rates,
             * FH Params, DS Params, CF Params, IBSS Params, TIM */
            u8 variable[];
        } __packed beacon;
        struct {
            /* only variable items: SSID, Supported rates */
            u8 variable[];
        } __packed probe_req;
        struct {
            __le64 timestamp;
            __le16 beacon_int;
            __le16 capab_info;
            /* followed by some of SSID, Supported rates,
             * FH Params, DS Params, CF Params, IBSS Params */
            u8 variable[];
        } __packed probe_resp;
        struct {
            u8 category;
            union {
                struct {
                    u8 action_code;
                    u8 dialog_token;
                    u8 status_code;
                    u8 variable[];
                } __packed wme_action;
                struct{
                    u8 action_code;
                    u8 variable[];
                } __packed chan_switch;
                struct{
                    u8 action_code;
                    struct ieee80211_ext_chansw_ie data;
                    u8 variable[];
                } __packed ext_chan_switch;
                struct{
                    u8 action_code;
                    u8 dialog_token;
                    u8 element_id;
                    u8 length;
                    struct ieee80211_msrment_ie msr_elem;
                } __packed measurement;
                struct{
                    u8 action_code;
                    u8 dialog_token;
                    __le16 capab;
                    __le16 timeout;
                    __le16 start_seq_num;
                    /* followed by BA Extension */
                    u8 variable[];
                } __packed addba_req;
                struct{
                    u8 action_code;
                    u8 dialog_token;
                    __le16 status;
                    __le16 capab;
                    __le16 timeout;
                } __packed addba_resp;
                struct{
                    u8 action_code;
                    __le16 params;
                    __le16 reason_code;
                } __packed delba;
                struct {
                    u8 action_code;
                    u8 variable[];
                } __packed self_prot;
                struct{
                    u8 action_code;
                    u8 variable[];
                } __packed mesh_action;
                struct {
                    u8 action;
                    u8 trans_id[WLAN_SA_QUERY_TR_ID_LEN];
                } __packed sa_query;
                struct {
                    u8 action;
                    u8 smps_control;
                } __packed ht_smps;
                struct {
                    u8 action_code;
                    u8 chanwidth;
                } __packed ht_notify_cw;
                struct {
                    u8 action_code;
                    u8 dialog_token;
                    __le16 capability;
                    u8 variable[0];
                } __packed tdls_discover_resp;
                struct {
                    u8 action_code;
                    u8 operating_mode;
                } __packed vht_opmode_notif;
                struct {
                    u8 action_code;
                    u8 membership[WLAN_MEMBERSHIP_LEN];
                    u8 position[WLAN_USER_POSITION_LEN];
                } __packed vht_group_notif;
                struct {
                    u8 action_code;
                    u8 dialog_token;
                    u8 tpc_elem_id;
                    u8 tpc_elem_length;
                    struct ieee80211_tpc_report_ie tpc;
                } __packed tpc_report;
                struct {
                    u8 action_code;
                    u8 dialog_token;
                    u8 follow_up;
                    u8 tod[6];
                    u8 toa[6];
                    __le16 tod_error;
                    __le16 toa_error;
                    u8 variable[];
                } __packed ftm;
                struct {
                    u8 action_code;
                    u8 variable[];
                } __packed s1g;
                struct {
                    u8 action_code;
                    u8 dialog_token;
                    u8 follow_up;
                    u32 tod;
                    u32 toa;
                    u8 max_tod_error;
                    u8 max_toa_error;
                } __packed wnm_timing_msr;
            } u;
        } __packed action;
    } u;
} __packed __aligned(2);

#ifdef __cplusplus
}
#endif

#endif
