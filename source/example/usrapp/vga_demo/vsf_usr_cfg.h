#ifndef __TOP_APP_CFG_H__
#define __TOP_APP_CFG_H__

#define ASSERT(...)                     if (!(__VA_ARGS__)) {while(1);};
//#define ASSERT(...)

#define M480_PLL_FREQ_HZ                (288 * 1000 * 1000)
#define M480_HCLK_FREQ_HZ               (288 * 1000 * 1000)
#define M480_HXT_FREQ_HZ                (12 * 1000 * 1000)
#define M480_PCLK0_FREQ_HZ              (72 * 1000 * 1000)
#define M480_PCLK1_FREQ_HZ              (72 * 1000 * 1000)

#define VSF_SYSTIMER_FREQ               M480_HCLK_FREQ_HZ

#define VSF_HEAP_SIZE                   (16 * 1024)

#endif
