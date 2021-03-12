#ifndef __VSF_IIC_H__
#define __VSF_IIC_H__

#include "./vsf_iic_internal.h"

typedef struct {
    struct {
        bool (*init)(iic_cfg_t *cfg_obj);
        bool (*write)(uint8_t slave_address, uint8_t *output, uint16_t out_size, uint8_t *arlo_buffer, uint16_t arlo_size);
        bool (*read)(uint8_t slave_address, uint8_t *input, uint16_t size);
    }master;
    
    struct {
        bool (*init)(iic_cfg_t *cfg_obj);
        bool (*write)(uint8_t *output, uint16_t size);
        bool (*read)(uint8_t *input, uint16_t size);
    }slave;
    
    em_vsf_iic_request_state_t (*get_state)(void);
    
}i_iic_t;

extern vsf_iic_t IIC_OBJ[IIC_COUNT];
extern const i_iic_t VSF_IIC[IIC_COUNT];

#endif