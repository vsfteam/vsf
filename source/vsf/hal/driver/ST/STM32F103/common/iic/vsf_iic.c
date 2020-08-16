#include "./vsf_iic.h"

static vsf_iic_m_evt_interface_t master_evt_1;
static vsf_iic_s_evt_interface_t slave_evt_1;
static vsf_iic_m_evt_interface_t master_evt_2;
static vsf_iic_s_evt_interface_t slave_evt_2;

vsf_iic_t IIC_OBJ[IIC_COUNT] = {
    [1] = {
        .iic_reg = I2C1,
        .master_evt_handler = &master_evt_1,
        .slave_evt_handler  = &slave_evt_1
    },
    
    [2] = {
        .iic_reg = I2C2,
        .master_evt_handler = &master_evt_2,
        .slave_evt_handler  = &slave_evt_2
    },
};

/********************************VSF_IIC_FUNC_BODY*****************************/
#define __VSF_IIC_FUNC_BODY(__N, __VALUE)                                      \
                                                                               \
bool vsf_iic##__N##_master_init(iic_cfg_t *cfg_obj)                            \
{                                                                              \
    return vsf_iic_master_init(&IIC_OBJ[__N], cfg_obj);                        \
}                                                                              \
                                                                               \
bool vsf_iic##__N##_slave_init(iic_cfg_t *cfg_obj)                             \
{                                                                              \
    return vsf_iic_slave_init(&IIC_OBJ[__N], cfg_obj);                         \
}                                                                              \
                                                                               \
bool vsf_iic##__N##_master_write(uint8_t slave_address,                        \
                                  uint8_t *output,                             \
                                  uint16_t out_size,                           \
                                  uint8_t *arlo_buffer,                        \
                                  uint16_t arlo_size)                          \
{                                                                              \
    return vsf_iic_master_write(&IIC_OBJ[__N],                                 \
                                slave_address,                                 \
                                output,                                        \
                                out_size,                                      \
                                arlo_buffer,                                   \
                                arlo_size,                                     \
                                NULL);                                         \
}                                                                              \
                                                                               \
bool vsf_iic##__N##_master_read(uint8_t slave_address,                         \
                                 uint8_t *input,                               \
                                 uint16_t size)                                \
{                                                                              \
    return vsf_iic_master_read(&IIC_OBJ[__N],                                  \
                               slave_address,                                  \
                               input,                                          \
                               size);                                          \
}                                                                              \
                                                                               \
bool vsf_iic##__N##_slave_write(uint8_t *output, uint16_t size)                \
{                                                                              \
    return vsf_iic_slave_write(&IIC_OBJ[__N], output, size);                   \
}                                                                              \
                                                                               \
bool vsf_iic##__N##_slave_read(uint8_t *input, uint16_t size)                  \
{                                                                              \
    return vsf_iic_slave_read(&IIC_OBJ[__N], input, size);                     \
}                                                                              \
                                                                               \
em_vsf_iic_request_state_t vsf_iic##__N##_get_request_state(void)              \
{                                                                              \
    return vsf_iic_get_request_state(&IIC_OBJ[__N]);                           \
}                                                                              \
                                                                               \
void I2C##__N##_EV_IRQHandler(void)                                            \
{                                                                              \
    vsf_iic_task(&IIC_OBJ[__N]);                                               \
}                                                                              \
                                                                               \
void I2C##__N##_ER_IRQHandler(void)                                            \
{                                                                              \
    vsf_iic_task(&IIC_OBJ[__N]);                                               \
}

/********************************VSF_IIC_FUNC_BODY*****************************/
#define __VSF_IIC_INTERFACE_DEF(__N, __VALUE)                                  \
    {                                                                          \
        .master = {                                                            \
            .init  = &vsf_iic##__N##_master_init,                              \
            .write = &vsf_iic##__N##_master_write,                             \
            .read  = &vsf_iic##__N##_master_read,                              \
        },                                                                     \
                                                                               \
        .slave = {                                                             \
            .init  = &vsf_iic##__N##_slave_init,                               \
            .write = &vsf_iic##__N##_slave_write,                              \
            .read  = &vsf_iic##__N##_slave_read,                               \
        },                                                                     \
                                                                               \
        .get_state = &vsf_iic##__N##_get_request_state,                        \
    },

/********************************VSF_iic_INTERFACE****************************/
#define __VSF_IIC_INTERFACE()          const i_iic_t VSF_IIC[IIC_COUNT]
    
/**********************************REPEAT_MACRO**********************************/
REPEAT_MACRO(IIC_COUNT, __VSF_IIC_FUNC_BODY, NULL)

__VSF_IIC_INTERFACE() = {
    REPEAT_MACRO(IIC_COUNT, __VSF_IIC_INTERFACE_DEF, NULL)
};