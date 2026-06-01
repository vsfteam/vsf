#ifndef __VSF_TEST_I2C_SLAVE_FIFO_H__
#define __VSF_TEST_I2C_SLAVE_FIFO_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"
#   include "component/test/vsf_test/vsf_test.h"
#   include "test_params_generated.h"
#ifndef VSF_TEST_I2C_SLAVE_FIFO_MASTER_BUF_SIZE
#   define VSF_TEST_I2C_SLAVE_FIFO_MASTER_BUF_SIZE        16
#endif
#ifndef VSF_TEST_I2C_SLAVE_FIFO_SLAVE_BUF_SIZE
#   define VSF_TEST_I2C_SLAVE_FIFO_SLAVE_BUF_SIZE        16
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_I2C_SLAVE_FIFO_CASE_COUNT
#   define VSF_TEST_I2C_SLAVE_FIFO_CASE_COUNT     1
#endif
#ifndef VSF_TEST_I2C_SLAVE_FIFO_CLOCK_HZ
#   define VSF_TEST_I2C_SLAVE_FIFO_CLOCK_HZ                        100000
#endif

#ifndef VSF_TEST_I2C_SLAVE_FIFO_TIMEOUT_MS
#   define VSF_TEST_I2C_SLAVE_FIFO_TIMEOUT_MS                      2000
#endif

#ifndef VSF_TEST_I2C_SLAVE_FIFO_ADDR
#   define VSF_TEST_I2C_SLAVE_FIFO_ADDR                            0x50
#endif

#ifndef VSF_TEST_I2C_SLAVE_FIFO_WRITE_CMD
#   define VSF_TEST_I2C_SLAVE_FIFO_WRITE_CMD                       (VSF_I2C_CMD_START|VSF_I2C_CMD_STOP|VSF_I2C_CMD_WRITE|VSF_I2C_CMD_7_BITS)
#endif

#ifndef VSF_TEST_I2C_SLAVE_FIFO_PATTERN_BYTE
#   define VSF_TEST_I2C_SLAVE_FIFO_PATTERN_BYTE               0xA0
#endif

#ifndef VSF_TEST_I2C_SLAVE_FIFO_PRIO
#   define VSF_TEST_I2C_SLAVE_FIFO_PRIO             vsf_arch_prio_1
#endif

#ifndef VSF_TEST_I2C_SLAVE_FIFO_MODE
#   define VSF_TEST_I2C_SLAVE_FIFO_MODE             (VSF_I2C_MODE_SLAVE | VSF_I2C_ADDR_7_BITS | VSF_I2C_SPEED_STANDARD_MODE)
#endif

#ifndef VSF_TEST_I2C_SLAVE_FIFO_CHANNEL_MODE
#   define VSF_TEST_I2C_SLAVE_FIFO_CHANNEL_MODE     (VSF_I2C_MODE_MASTER | VSF_I2C_ADDR_7_BITS | VSF_I2C_SPEED_STANDARD_MODE)
#endif



/*============================ TYPES =========================================*/

#if VSF_TEST_I2C_SLAVE_FIFO_ENABLE == ENABLED
typedef struct {
    volatile vsf_i2c_irq_mask_t master_irq_mask;
    volatile vsf_i2c_irq_mask_t slave_irq_mask;
    uint8_t master_buf[VSF_TEST_I2C_SLAVE_FIFO_MASTER_BUF_SIZE];
    uint8_t slave_buf[VSF_TEST_I2C_SLAVE_FIFO_SLAVE_BUF_SIZE];
    volatile uint_fast16_t slave_rx_offset;
    volatile bool master_done;
    volatile bool slave_complete;
} vsf_test_i2c_slave_fifo_data_t;
#endif

/*============================ PROTOTYPES ====================================*/

void vsf_test_i2c_slave_fifo_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst);

typedef struct {
    uint8_t  idx;
    uint8_t  master_i2c_idx;
    uint8_t  slave_i2c_idx;
} vsf_test_i2c_slave_fifo_params_t;

struct vsf_test_i2c_slave_fifo_s;
extern const struct vsf_test_i2c_slave_fifo_s vsf_test_i2c_slave_fifo;

#endif /* __VSF_TEST_I2C_SLAVE_FIFO_H__ */
/* EOF */
