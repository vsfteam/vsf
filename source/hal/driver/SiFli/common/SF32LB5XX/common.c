#include "bf0_hal.h"

uint32_t flash_get_freq(int clk_module, uint16_t clk_div, uint8_t hcpu)
{
    int src;
    uint32_t freq;

    if (clk_div <= 0)
        return 0;

    if (hcpu == 0)
    {
        freq = HAL_RCC_GetSysCLKFreq(CORE_ID_LCPU);
        return freq / clk_div;
    }

    src = HAL_RCC_HCPU_GetClockSrc(clk_module);
#ifdef SOC_SF32LB52X
    if (RCC_CLK_FLASH_DLL2 == src)
    {
        freq = HAL_RCC_HCPU_GetDLL2Freq();
    }
    else if (RCC_CLK_SRC_DLL1 == src)
    {
        freq = HAL_RCC_HCPU_GetDLL1Freq();
    }
    else if (3 == src)  // DBL96
    {
        freq = 96000000;
    }
    else    // CLK_PERI, RC48/XTAL48
    {
        freq = 48000000;
    }
#else
    if (RCC_CLK_FLASH_DLL2 == src)
    {
        freq = HAL_RCC_HCPU_GetDLL2Freq();
    }
    else if (RCC_CLK_FLASH_DLL3 == src)
    {
        freq = HAL_RCC_HCPU_GetDLL3Freq();
    }
    else
    {
        freq = HAL_RCC_GetSysCLKFreq(CORE_ID_HCPU);
    }
#endif
    return freq / clk_div;;
}