#include "./vsf_spi.h"

vsf_spi_t vsf_spi[SPI_COUNT] = {
    [1] = {
        .hspi = SPI1,
    },
    
    [2] = {
        .hspi = SPI2,
    },
};

spi_status_t vsf_spi_get_status(vsf_spi_t *spi_obj)
{
    ASSERT(spi_obj != NULL);
    
    spi_status_t spi_status;
    
    spi_status.SPI_RXNE   = spi_obj->hspi->SR & 0x01; 
    spi_status.SPI_TXE    = spi_obj->hspi->SR & 0x02; 
    spi_status.SPI_CHSIDE = spi_obj->hspi->SR & 0x04; 
    spi_status.SPI_UDR    = spi_obj->hspi->SR & 0x08; 
    spi_status.SPI_CRCERR = spi_obj->hspi->SR & 0x10; 
    spi_status.SPI_MODF   = spi_obj->hspi->SR & 0x20; 
    spi_status.SPI_OVR    = spi_obj->hspi->SR & 0x40; 
    spi_status.SPI_BSY    = spi_obj->hspi->SR & 0x80; 
        
    return spi_status;
}

spi_capability_t vsf_spi_get_capability(void)
{
    spi_capability_t spi_capability;
    
    spi_capability.Read.can_read_block   = true;
    spi_capability.Read.can_read_byte    = true;
    spi_capability.Read.random_access    = false;
    spi_capability.Read.support_dma      = true;
    spi_capability.Read.support_fifo     = false;
    spi_capability.Read.support_isr      = true;
    
    spi_capability.Write.can_write_block = true;
    spi_capability.Write.can_write_byte  = true;
    spi_capability.Write.random_access   = false;
    spi_capability.Write.support_dma     = true;
    spi_capability.Write.support_dma     = false;
    spi_capability.Write.support_isr     = true;
    
    spi_capability.Feature.u3_data_type_size = 0x03;
    spi_capability.Feature.u5_block_size     = 0xFF;
    
    return spi_capability;
}

vsf_err_t vsf_spi_init(vsf_spi_t *spi_obj, spi_cfg_t *cfg_ptr)
{
    ASSERT((spi_obj != NULL) && (cfg_ptr != NULL));
    
    if(SPI1 == spi_obj->hspi) {        
        RCC->APB2ENR |= SPI1_CLK_EN | SPI1_GPIO_CLK_EN;

        GPIOA->CRL &= SPI1_GPIO_CLEAR;
        GPIOA->CRL |= SPI1_GPIO_MODE;
        
        HAL_NVIC_SetPriority(SPI1_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(SPI1_IRQn);
    }
    
    if(SPI2 == spi_obj->hspi) {
        RCC->APB1ENR |= SPI2_CLK_EN;
        RCC->APB2ENR |= SPI2_GPIO_CLK_EN;

        GPIOB->CRH &= SPI2_GPIO_CLEAR;
        GPIOB->CRH |= SPI2_GPIO_MODE;
        
        HAL_NVIC_SetPriority(SPI2_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(SPI2_IRQn);
    }
    
    spi_obj->hspi->CR1 &= ~SPI_EN;
    
    if(SPI1 == spi_obj->hspi) {
        spi_obj->hspi->CR1 |= SPI_NSS_SET;
    }
    
    spi_obj->hspi->CR1 |= cfg_ptr->wMode;
    spi_obj->hspi->CR1 |= cfg_ptr->hwClockDiv;
    spi_obj->hspi->CR1 |= cfg_ptr->chDataSize;
    
    return VSF_ERR_NONE;
}

uintalu_t vsf_spi_polarity_set(vsf_spi_t *spi_obj, uintalu_t polarity)
{
    ASSERT(spi_obj != NULL);
    
    uintalu_t uintalu;
    
    vsf_interrupt_safe() {
        uintalu = spi_obj->hspi->CR1 & 0x0003;
        
        spi_obj->hspi->CR1 &= 0xFFFB;
        spi_obj->hspi->CR1 |= polarity;
    }
    return uintalu;
}

uintalu_t vsf_spi_polarity_get(vsf_spi_t *spi_obj)
{
    ASSERT(spi_obj != NULL);
    
    uintalu_t uintalu;
    
    vsf_interrupt_safe() {
        uintalu = spi_obj->hspi->CR1 & 0x0003;
    }
    return uintalu;
}

fsm_rt_t vsf_spi_exchange(vsf_spi_t *spi_obj, uintalu_t output, void *input)
{
    ASSERT((spi_obj != NULL) && (input != NULL));
            
    bool wait_for_read = false;
    bool exchange_cpl  = false;
    
    vsf_interrupt_safe() {
    
        spi_obj->hspi->CR1 |= SPI_EN;
        
        while(!exchange_cpl) {        
                    
            if((spi_obj->hspi->SR & SPI_SR_TXE_IS_TRUE) && (false == wait_for_read)) {
                *(__IO uint8_t *)&spi_obj->hspi->DR = output;
                wait_for_read = true;
            }
            
            if(spi_obj->hspi->SR & SPI_SR_RXNE_IS_TRUE) {
                *(uint8_t *)input = spi_obj->hspi->DR;
                exchange_cpl = true;
            }
        }
        
        spi_obj->hspi->CR1 &= (~SPI_EN);
    }
    
    return fsm_rt_cpl;
}

fsm_rt_t vsf_spi_request_exchange(vsf_spi_t *spi_obj, void *output, void *input, uint_fast32_t size)
{
    ASSERT((spi_obj != NULL) && (output != NULL) && (input != NULL) && (size != 0));
    
    vsf_interrupt_safe() {
        if(false == spi_obj->data_exchange) {
            spi_obj->output           = output;
            spi_obj->input            = input;
            spi_obj->data_size        = size;
            
            spi_obj->rx_count         = 0;
            spi_obj->tx_count         = 0;
            spi_obj->data_exchange    = true;
            
            spi_obj->hspi->CR2 |= SPI_IT_TXE | SPI_IT_RXNE;
            spi_obj->hspi->CR1 |= SPI_EN;
        }
        
    }
        
    if((spi_obj->rx_count >= spi_obj->data_size) && (spi_obj->tx_count >= spi_obj->data_size)) {
        vsf_interrupt_safe() {
            spi_obj->hspi->CR1 &= ~SPI_EN;
            spi_obj->data_exchange = false;
        }

        return fsm_rt_cpl;
    }      
    
    return fsm_rt_asyn;
}

static void vsf_spi_irqhandler(vsf_spi_t *spi_obj)
{
        uint32_t spi_sr  = spi_obj->hspi->SR;
        uint32_t spi_cr2 = spi_obj->hspi->CR2;
    
        if((spi_sr & SPI_SR_TXE_IS_TRUE) && (spi_cr2 & SPI_IT_TXE)) {
            *(__IO uint8_t *)&spi_obj->hspi->DR = spi_obj->output[spi_obj->tx_count++];
            
            if(spi_obj->tx_count >= spi_obj->data_size) {
                spi_obj->hspi->CR2 &= ~SPI_IT_TXE;
            }
        }

        if((spi_sr & SPI_SR_RXNE_IS_TRUE) && (spi_cr2 & SPI_IT_RXNE)) {
            spi_obj->input[spi_obj->rx_count++] = *(__IO uint8_t *)&spi_obj->hspi->DR;
            
            if(spi_obj->rx_count >= spi_obj->data_size) {
                spi_obj->hspi->CR2 &= ~SPI_IT_RXNE;
            }
        }
}

#define __VSF_SPI_FUNC_BODY(__N, __VALUE)                                    \
spi_status_t vsf_spi##__N##_get_status(void)                                 \
{                                                                            \
    return vsf_spi_get_status(&vsf_spi[__N]);                                \
}                                                                            \
                                                                             \
vsf_err_t vsf_spi##__N##_init(spi_cfg_t *cfg_ptr)                            \
{                                                                            \
    return vsf_spi_init(&vsf_spi[__N], cfg_ptr);                             \
}                                                                            \
                                                                             \
uintalu_t vsf_spi##__N##_polarity_set(uintalu_t polarity)                    \
{                                                                            \
    return vsf_spi_polarity_set(&vsf_spi[__N], polarity);                    \
}                                                                            \
                                                                             \
uintalu_t vsf_spi##__N##_polarity_get(void)                                  \
{                                                                            \
    return vsf_spi_polarity_get(&vsf_spi[__N]);                              \
}                                                                            \
                                                                             \
fsm_rt_t vsf_spi##__N##_exchange(uintalu_t output, void *input)              \
{                                                                            \
    return vsf_spi_exchange(&vsf_spi[__N], output, input);                   \
}                                                                            \
                                                                             \
fsm_rt_t vsf_spi##__N##_request_exchange(void *output, void *input,          \
                                         uint_fast32_t size)                 \
{                                                                            \
    return vsf_spi_request_exchange(&vsf_spi[__N], output, input, size);     \
}                                                                            \
                                                                             \
void SPI##__N##_IRQHandler(void)                                             \
{                                                                            \
    vsf_spi_irqhandler(&vsf_spi[__N]);                                       \
}                           

#define __VSF_SPI_INTERFACE_DEF(__N, __VALUE)                                \
    {                                                                        \
        .SPI = {                                                             \
            .Status     = &vsf_spi##__N##_get_status,                        \
            .Capability = &vsf_spi_get_capability,                           \
        },                                                                   \
        .Init = &vsf_spi##__N##_init,                                        \
        .Polarity = {                                                        \
            .Set = &vsf_spi##__N##_polarity_set,                             \
            .Get = &vsf_spi##__N##_polarity_get,                             \
        },                                                                   \
        .SingleExchange = &vsf_spi##__N##_exchange,                          \
        .Block = {                                                           \
            .RequestExchange = &vsf_spi##__N##_request_exchange,             \
        }                                                                    \
    },
    
#define __VSF_SPI_INTERFACE()          const i_spi_t VSF_SPI[SPI_COUNT]
    
REPEAT_MACRO(SPI_COUNT, __VSF_SPI_FUNC_BODY, NULL)

__VSF_SPI_INTERFACE() = {
    REPEAT_MACRO(SPI_COUNT, __VSF_SPI_INTERFACE_DEF, NULL)
};