#ifndef __VSF_IIC_INTERNAL_H__
#define __VSF_IIC_INTERNAL_H__

#include "hal/vsf_hal_cfg.h"
#include "../../__device.h"

//! \brief i2c address configuration structure
typedef struct {
    uint32_t clock_speed;       

    uint32_t duty_cycle;              

    uint32_t addressing_mode;   

    uint32_t own_address1;

    uint32_t dual_address_mode;  

    uint32_t own_address2;      

    uint32_t general_call_mode;  

    uint32_t no_stretch_mode;    
}iic_cfg_t;

//! \name i2c send request erro state
//! @{
typedef enum {
    IIC_REQ_ON_GOING                    = 0,
    IIC_REQ_SUCCESS                     = 1,
    IIC_REQ_FAILED                      = 2,
    IIC_REQ_BUS_ERROR                   = 3,
    IIC_REQ_CANCEL                      = 4,
    IIC_REQ_ARLO                        = 5
}em_vsf_iic_request_state_t;

typedef enum {    
    IIC_MASTER_READ                     = 0x00000001U,
    
    IIC_DUTYCYCLE_2                     = 0x00000000U,
    IIC_DUTYCYCLE_16_9                  = 0x00004000U,
    
    IIC_ADDRESSINGMODE_7BIT             = 0x00004000U,
    IIC_ADDRESSINGMODE_10BIT            = 0x0000C000U,
    
    IIC_DUALADDRESS_DISABLE             = 0x00000000U,
    IIC_DUALADDRESS_ENABLE              = 0x00000001U,
    
    IIC_GENERALCALL_DISABLE             = 0x00000000U,
    IIC_GENERALCALL_ENABLE              = 0x00000040U,
    
    IIC_NOSTRETCH_DISABLE               = 0x00000000U,
    IIC_NOSTRETCH_ENABLE                = 0x00000080U,
} em_iic_mode_t;

typedef enum {
    IIC1_GPIO_CLK_EN                    = 0x00000008U,
    IIC1_CLK_EN                         = 0x00200000U,
    IIC1_GPIO_MODE                      = 0xFF000000U,
    
    IIC2_GPIO_CLK_EN                    = 0x00000008U,
    IIC2_CLK_EN                         = 0x00400000U,
    IIC2_GPIO_MODE                      = 0x0000FF00U,
} em_iic_low_hardware_cfg_t;

typedef enum  {
    IIC_EN                              = 0x00000001U,
    IIC_START                           = 0x00000100U,
    IIC_STOP                            = 0x00000200U,
    IIC_ACK_EN                          = 0x00000400U,
    IIC_POS_EN                          = 0x00000800U,
} em_iic_cr1_reg_t;

typedef enum  {
    IIC_IT_ERR_EN                       = 0x00000100U,
    IIC_IT_EVT_EN                       = 0x00000200U,
    IIC_IT_BUF_EN                       = 0x00000400U,
} em_iic_it_t;

typedef enum  {
    IIC_SB_SET                          = 0x00000001U,
    IIC_ADDR_SET                        = 0x00000002U,
    IIC_BTF_SET                         = 0x00000004U,
    IIC_STOPF_SET                       = 0x00000010U,
    IIC_RXNE_SET                        = 0x00000040U,
    IIC_TXE_SET                         = 0x00000080U,
    IIC_ARLO_SET                        = 0x00000200U,
    IIC_AF_SET                          = 0x00000400U,
    
    IIC_BUSY                            = 0x00000002U,
} em_iic_status_t;

typedef enum  {
    NO_START                            = 0x00000001U,
    NO_ADDR                             = 0x00000002U,
    NO_STOP                             = 0x00000004U,
} em_process_sele_t;

typedef struct vsf_iic_m_evt_interface_t vsf_iic_m_evt_interface_t;
typedef struct vsf_iic_s_evt_interface_t vsf_iic_s_evt_interface_t;
typedef struct vsf_iic_t vsf_iic_t;

typedef struct vsf_iic_t{
    I2C_TypeDef                      *iic_reg;
    bool                             is_master;
    bool                             is_write;
    bool                             iic_is_busy;
    bool                             is_arlo;
    bool                             is_af_back;
    bool                             no_addr;
    bool                             no_stop;
    uint8_t                          slave_address;
    uint8_t                          *input;
    uint8_t                          *output;
    uint8_t                          *arlo_buffer;  
    uint32_t                         arlo_size;
    uint32_t                         in_size;
    uint32_t                         in_count;
    uint32_t                         out_size;
    uint32_t                         out_count;
    void                             (*callback_handler)(vsf_iic_t *iic_obj);
    vsf_iic_m_evt_interface_t        *master_evt_handler;
    vsf_iic_s_evt_interface_t        *slave_evt_handler;
} vsf_iic_t;

struct vsf_iic_m_evt_interface_t{
    void (*m_w_evt_sb)(vsf_iic_t *iic_obj);
    void (*m_w_evt_addr)(vsf_iic_t *iic_obj);
    void (*m_w_evt_txe_btf)(vsf_iic_t *iic_obj);
    void (*m_w_evt_txe)(vsf_iic_t *iic_obj);
    
    void (*m_r_evt_sb)(vsf_iic_t *iic_obj);
    void (*m_r_evt_addr)(vsf_iic_t *iic_obj);
    void (*m_r_evt_rxne)(vsf_iic_t *iic_obj);
    
    void (*m_w_err_arlo)(vsf_iic_t *iic_obj);
};

struct vsf_iic_s_evt_interface_t{
    void (*s_w_evt_addr)(vsf_iic_t *iic_obj);
    void (*s_w_evt_txe)(vsf_iic_t *iic_obj);
    
    void (*s_r_evt_addr)(vsf_iic_t *iic_obj);
    void (*s_r_evt_stopf)(vsf_iic_t *iic_obj);
    void (*s_r_evt_rxne)(vsf_iic_t *iic_obj);
    
    void (*s_r_err_af)(vsf_iic_t *iic_obj);
};

extern bool vsf_iic_master_init(vsf_iic_t *iic_obj, iic_cfg_t *cfg_obj);

extern bool vsf_iic_master_read(vsf_iic_t *iic_obj, uint8_t slave_address, uint8_t *input, uint16_t size);

extern bool vsf_iic_master_write(vsf_iic_t *iic_obj, uint8_t slave_address, uint8_t *output, uint16_t out_size, uint8_t *arlo_buffer, uint16_t arlo_size, uint8_t Process_sele);

//extern bool vsf_iic_cancel(vsf_iic_t *iic_obj);

extern bool vsf_iic_master_register_event( vsf_iic_t *iic_obj, vsf_iic_m_evt_interface_t *evt_obj);

extern em_vsf_iic_request_state_t vsf_iic_get_request_state(vsf_iic_t *iic_obj);

extern bool vsf_iic_slave_init(vsf_iic_t *iic_obj, iic_cfg_t *cfg_obj);

extern bool vsf_iic_slave_read(vsf_iic_t *iic_obj, uint8_t *input, uint16_t size);

extern bool vsf_iic_slave_write(vsf_iic_t *iic_obj, uint8_t *output, uint16_t size);

//extern bool vsf_iic_set_address(vsf_iic_t *iic_obj, vsf_iic_addr_cfg_t *address);

//extern vsf_iic_addr_cfg_t vsf_iic_get_address(vsf_iic_t *iic_obj, uint8_t index);

//extern bool vsf_iic_slave_enable(vsf_iic_t *iic_obj);

//extern bool vsf_iic_slave_disable(vsf_iic_t *iic_obj);

extern bool vsf_iic_slave_register_event(vsf_iic_t *iic_obj, vsf_iic_s_evt_interface_t *evt_obj);

extern fsm_rt_t vsf_iic_task(vsf_iic_t *iic_obj);

//extern bool vsf_iic_feed_data(vsf_iic_t *iic_obj, uint8_t data);

#endif