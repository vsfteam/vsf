#ifndef __BTSTACK_CONFIG
#define __BTSTACK_CONFIG

// BTstack features that can be enabled
#define ENABLE_BLE
//#define ENABLE_LE_PERIPHERAL
#define ENABLE_LE_CENTRAL
//#define ENABLE_LE_DATA_LENGTH_EXTENSION
#define ENABLE_CLASSIC
//#define ENABLE_LOG_INFO
//#define ENABLE_LOG_ERROR
//#define ENABLE_LOG_DEBUG

// ENABLE_L2CAP_ENHANCED_RETRANSMISSION_MODE will be confilict with btstack_host_hid etc.
//#define ENABLE_L2CAP_ENHANCED_RETRANSMISSION_MODE

// BTstack configuration. buffers, sizes, ...
#define MAX_NR_WHITELIST_ENTRIES 1
#define MAX_NR_HCI_CONNECTIONS 3
#define MAX_NR_SM_LOOKUP_ENTRIES 3
#define MAX_NR_L2CAP_SERVICES 5
#define MAX_NR_L2CAP_CHANNELS 6
#define MAX_NR_GATT_CLIENTS 1

#define MAX_NR_RFCOMM_MULTIPLEXERS 0
#define MAX_NR_RFCOMM_SERVICES 0
#define MAX_NR_RFCOMM_CHANNELS 0
#define MAX_NR_HFP_CONNECTIONS 0
#define MAX_NR_BTSTACK_LINK_KEY_DB_MEMORY_ENTRIES 3
#define MAX_NR_BNEP_SERVICES 0
#define MAX_NR_BNEP_CHANNELS 0
#define MAX_NR_SERVICE_RECORD_ITEMS 3
#define MAX_NR_LE_DEVICE_DB_ENTRIES 1

#define MAX_ATT_DB_SIZE 1

#define HCI_ACL_PAYLOAD_SIZE                                    1024

#endif

