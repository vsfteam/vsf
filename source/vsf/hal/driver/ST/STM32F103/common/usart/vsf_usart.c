#include "./vsf_usart.h"

/**********************************define tEvtMask*******************************/
usart_evt_status_t tEvtMask = 0x00;

/***********************************usart_init***********************************/
vsf_err_t vsf_usart_init(vsf_usart_t *pUsart, usart_cfg_t *cfg_ptr)
{
    ASSERT(pUsart->pBase != NULL);
    
    float fUsartdtiv;
	uint16_t hwIntegerPart;
	uint16_t hwDecimalPart;	   

/*USART1_CLK_GPIO_NVIC*/    
    if(USART1 == pUsart->pBase) {
        RCC->APB2ENR |= USART1_CLK_EN|GPIOA_CLK_EN; 
        
        GPIOA->CRH &= USART1_GPIO_MODE_CLEAR;
        GPIOA->CRH |= USART1_GPIO_MODE; 

        fUsartdtiv = (float)PCLK2/(cfg_ptr->baudrate*16);
        
        /* USART1 interrupt Init */
        HAL_NVIC_SetPriority(USART1_IRQn, 1, 1);
        HAL_NVIC_EnableIRQ(USART1_IRQn);
    }
    
    
/*USART2_CLK_GPIO_NVIC*/  
    if(USART2 == pUsart->pBase) {
        RCC->APB1ENR |= USART2_CLK_EN;
        RCC->APB2ENR |= GPIOA_CLK_EN; 
        
        GPIOA->CRL &= USART2_GPIO_MODE_CLEAR;
        GPIOA->CRL |= USART2_GPIO_MODE; 

        fUsartdtiv = (float)PCLK1/(cfg_ptr->baudrate*16);
        
        /* USART2 interrupt Init */
        HAL_NVIC_SetPriority(USART2_IRQn, 1, 1);
        HAL_NVIC_EnableIRQ(USART2_IRQn);
    }
    
    
/*USART3_CLK_GPIO_NVIC*/ 
    if(USART3 == pUsart->pBase) {
        RCC->APB1ENR |= USART3_CLK_EN;
        RCC->APB2ENR |= GPIOB_CLK_EN; 
        
        GPIOB->CRH &= USART3_GPIO_MODE_CLEAR;
        GPIOB->CRH |= USART3_GPIO_MODE; 

        fUsartdtiv = (float)PCLK1/(cfg_ptr->baudrate*16);

        HAL_NVIC_SetPriority(USART3_IRQn, 1, 1);
        HAL_NVIC_EnableIRQ(USART3_IRQn);
    }
      
    hwIntegerPart = fUsartdtiv;
    hwDecimalPart = (fUsartdtiv - hwIntegerPart) * 16;	 
    hwIntegerPart <<= 4;
    hwIntegerPart += hwDecimalPart;
    
    pUsart->pBase->BRR  = hwIntegerPart;
    pUsart->pBase->CR1 |= cfg_ptr->mode;
    pUsart->pBase->CR1 |= USART_EN;
    
    return VSF_ERR_NONE;
}

/**********************************usart_enable**********************************/
fsm_rt_t vsf_usart_enable(vsf_usart_t *pUsart)
{
    ASSERT(pUsart->pBase != NULL);
    
    pUsart->pBase->CR1 |= USART_EN;
    return fsm_rt_cpl;
}

/**********************************usart_disable*********************************/
fsm_rt_t vsf_usart_disable(vsf_usart_t *pUsart)
{
    ASSERT(pUsart->pBase != NULL);
    
    pUsart->pBase->CR1 |= USART_DISEN;
    return fsm_rt_cpl;
}

/***********************************usart_status*********************************/
usart_status_t vsf_usart_status(vsf_usart_t *pUsart)
{
    ASSERT(pUsart->pBase != NULL);
    
    usart_status_t state_break = {
        .bIsBusy = false
    };
    
    if(pUsart->pBase->SR & USART_SR_IDLE_FREE) { 
        state_break.bIsBusy = true;
    }
    
    return state_break;
}

/*********************************usart_read_byte********************************/
bool vsf_usart_read_byte(vsf_usart_t *pUsart, uint8_t *byte_ptr) 
{
    ASSERT(pUsart->pBase != NULL);
    
    if(pUsart->pBase->SR & USART_SR_RXNE_FULL) {
        *byte_ptr = pUsart->pBase->DR;
        
        pUsart->tEvtStatus.chEvtStatus |= VSF_USART_EVT_RX;
        if((pUsart->tEvtRx.handler_fn != NULL) && (tEvtMask & VSF_USART_EVT_RX)) {
            pUsart->tEvtRx.handler_fn(pUsart->tEvtRx.target_ptr, pUsart, pUsart->tEvtStatus);
        }
        
        return true;
    }
    return false;
}

/*********************************usart_write_byte*******************************/
bool vsf_usart_write_byte(vsf_usart_t *pUsart, uint_fast8_t chByte) 
{
    ASSERT(pUsart->pBase != NULL);
    
    uint8_t chTimer = 100;
    
    if(pUsart->pBase->SR & USART_SR_TXE_TRUE) {
        pUsart->pBase->DR = chByte;
    } else {
        return false;
    }
    
    while(chTimer--){
        if(USART_SR_TC_TRUE == (pUsart->pBase->SR & USART_SR_TC_TRUE)) {
            
            pUsart->tEvtStatus.chEvtStatus |= VSF_USART_EVT_TX;
            if((pUsart->tEvtTx.handler_fn != NULL) && (tEvtMask & VSF_USART_EVT_TX)) {
                pUsart->tEvtTx.handler_fn(pUsart->tEvtTx.target_ptr, pUsart, pUsart->tEvtStatus);
            }
            
            return true;
        }
    }
    return false;
}

/********************************usart_request_read******************************/
fsm_rt_t vsf_usart_request_read(vsf_usart_t *pUsart, uint8_t *buffer_ptr, uint_fast32_t u32_size)
{
    ASSERT(pUsart->pBase != NULL);
    ASSERT(buffer_ptr != NULL);
    ASSERT(u32_size != 0);
    
    if(false == pUsart->bIsLoading) {
        pUsart->bIsLoading     = true;
        pUsart->pchReadBuffer  = buffer_ptr;
        pUsart->wReadSize      = u32_size - 1;
        pUsart->wReadSizeTimer = 0; 
        pUsart->pBase->CR1 |= USART_CR1_RXNEIE_EN;
    }
    
    if(pUsart->wReadSizeTimer > pUsart->wReadSize) {
        pUsart->bIsLoading = false;
        
        pUsart->tEvtStatus.chEvtStatus |= VSF_USART_EVT_RCV_BLK_CPL;
        if((pUsart->tEvtRcv.handler_fn != NULL) && (tEvtMask & VSF_USART_EVT_RCV_BLK_CPL)) {
            pUsart->tEvtRcv.handler_fn(pUsart->tEvtRcv.target_ptr, pUsart, pUsart->tEvtStatus);
        }
        
        return fsm_rt_cpl;
    }
    
    return fsm_rt_asyn;
}

/*******************************usart_request_write******************************/
fsm_rt_t vsf_usart_request_write(vsf_usart_t *pUsart, uint8_t *buffer_ptr, uint_fast32_t u32_size)
{
    ASSERT(pUsart->pBase != NULL);
    ASSERT(buffer_ptr != NULL);
    ASSERT(u32_size != 0);
        
    if(false == pUsart->bIsWriting) {
        pUsart->bIsWriting      = true;
        pUsart->pchWriteBuffer  = buffer_ptr;
        pUsart->wWriteSize      = u32_size - 1;
        pUsart->wWriteSizeTimer = 0;
        pUsart->pBase->CR1 |= USART_CR1_TXEIE_EN;
    }
    
    if(pUsart->wWriteSizeTimer >= pUsart->wWriteSize) {
        pUsart->bIsWriting = false;
        
        pUsart->tEvtStatus.chEvtStatus |= VSF_USART_EVT_SND_BLK_CPL;
        if((pUsart->tEvtSnd.handler_fn != NULL) && (tEvtMask & VSF_USART_EVT_SND_BLK_CPL)) {
            pUsart->tEvtSnd.handler_fn(pUsart->tEvtSnd.target_ptr, pUsart, pUsart->tEvtStatus);
        }
        
        return fsm_rt_cpl;
    }
        
    return fsm_rt_asyn;
}

/*******************************vsf_usart_riqhandler*****************************/
void vsf_usart_irqhandler(vsf_usart_t *pUsart)
{
//    SAFE_ATOM_CODE() {
        if(pUsart->pBase->SR & USART_SR_RXNE_FULL) {
            pUsart->pchReadBuffer[pUsart->wReadSizeTimer] = pUsart->pBase->DR;
            if((pUsart->wReadSizeTimer++) >= pUsart->wReadSize) {
                pUsart->pBase->CR1 &= USART_CR1_RXNEIE_DISEN;
            }
        } else {
            pUsart->pBase->DR = pUsart->pchWriteBuffer[pUsart->wWriteSizeTimer];
            if((pUsart->wWriteSizeTimer++) >= pUsart->wWriteSize) {
                pUsart->pBase->CR1 &= USART_CR1_TXEIE_DISEN;
            }
        }
//    }
}

/*********************************usart_huart_arrcy******************************/
#define __USART_HUART(__N, __VALUE)                                              \
                                                                                 \
        {                                                                        \
            .pBase  = USART##__N                                               \
        },                             

static vsf_usart_t usart_huart[] = {
    REPEAT_MACRO(USART_COUNT, __USART_HUART, NULL)
};

/*******************************vsf_evt_usart_register***************************/
void vsf_usart_evt_register(vsf_usart_evt_type_t tType, vsf_usart_evt_t tEvent)
{
    if(VSF_USART_EVT_RX == tType) {
        for(uint8_t i =1; i < USART_COUNT; i++) {
            usart_huart[i].tEvtRx = tEvent;
        }
    }
    
    if(VSF_USART_EVT_TX == tType) {
        for(uint8_t i =1; i < USART_COUNT; i++) {
            usart_huart[i].tEvtTx = tEvent;
        }
    }
    
    if(VSF_USART_EVT_RCV_BLK_CPL == tType) {
        for(uint8_t i =1; i < USART_COUNT; i++) {
            usart_huart[i].tEvtRcv = tEvent;
        }
    }
    
    if(VSF_USART_EVT_SND_BLK_CPL == tType) {
        for(uint8_t i =1; i < USART_COUNT; i++) {
            usart_huart[i].tEvtSnd = tEvent;
        }
    }
}

/*******************************vsf_usart_evt_enable*****************************/
usart_evt_status_t vsf_usart_evt_enable(usart_evt_status_t tEventMask)
{
    if(tEvtMask != 0xFF) {
        tEvtMask |= tEventMask;
    }
    return tEvtMask;
}

/*******************************vsf_usart_evt_disable****************************/
usart_evt_status_t vsf_usart_evt_disable(usart_evt_status_t tEventMask)
{
    if(tEvtMask != 0x00) {
        tEvtMask &= (~tEventMask);
    }
    return tEvtMask;
}

/*******************************vsf_usart_evt_resume*****************************/
void vsf_usart_evt_resume(usart_evt_status_t tEventStatus)
{
    tEvtMask = tEventStatus;
}

/********************************VSF_USART_FUNC_BODY*****************************/
#define __VSF_USART_FUNC_BODY(__N, __VALUE)                                      \
                                                                                 \
/*usart_enable*/                                                                 \
vsf_err_t vsf_usart##__N##_init(usart_cfg_t *usart_cfg)                          \
{                                                                                \
    return vsf_usart_init(&usart_huart[__N], usart_cfg);                         \
}                                                                                \
                                                                                 \
/*usart_enable*/                                                                 \
fsm_rt_t vsf_usart##__N##_enable(void)                                           \
{                                                                                \
    return vsf_usart_enable(&usart_huart[__N]);                                  \
}                                                                                \
                                                                                 \
/*usart_disable*/                                                                \
fsm_rt_t vsf_usart##__N##_disable(void)                                          \
{                                                                                \
    return vsf_usart_disable(&usart_huart[__N]);                                 \
}                                                                                \
                                                                                 \
/*usart_status*/                                                                 \
usart_status_t vsf_usart##__N##_status(void)                                     \
{                                                                                \
    return vsf_usart_status(&usart_huart[__N]);                                  \
}                                                                                \
                                                                                 \
/*usart_read_byte*/                                                              \
bool vsf_usart##__N##_read_byte(uint8_t *byte_ptr)                                \
{                                                                                \
    return vsf_usart_read_byte(&usart_huart[__N], byte_ptr);                      \
}                                                                                \
                                                                                 \
/*usart_write_byte*/                                                             \
bool vsf_usart##__N##_write_byte(uint_fast8_t chByte)                            \
{                                                                                \
    return vsf_usart_write_byte(&usart_huart[__N], chByte);                      \
}                                                                                \
                                                                                 \
/*usart_request_read*/                                                           \
fsm_rt_t vsf_usart##__N##_request_read(uint8_t *buffer_ptr, uint_fast32_t u32_size)  \
{                                                                                \
    return vsf_usart_request_read(&usart_huart[__N], buffer_ptr, u32_size);          \
}                                                                                \
                                                                                 \
/*usart_request_write*/                                                          \
fsm_rt_t vsf_usart##__N##_request_write(uint8_t *buffer_ptr, uint_fast32_t u32_size) \
{                                                                                \
    return vsf_usart_request_write(&usart_huart[__N], buffer_ptr, u32_size);         \
}                                                                                \
                                                                                 \
/*usart_irqhandler*/                                                             \
void USART##__N##_IRQHandler(void)                                               \
{                                                                                \
    vsf_usart_irqhandler(&usart_huart[__N]);                                     \
}

/********************************VSF_USART_FUNC_BODY*****************************/
#define __VSF_USART_INTERFACE_DEF(__N, __VALUE)                                  \
    {                                                                            \
        .Init         = &vsf_usart##__N##_init,                                  \
        .Enable       = &vsf_usart##__N##_enable,                                \
        .Disable      = &vsf_usart##__N##_disable,                               \
        .Status       = (peripheral_status_t (*)(void))vsf_usart##__N##_status,  \
        .ReadByte     = &vsf_usart##__N##_read_byte,                             \
        .WriteByte    = &vsf_usart##__N##_write_byte,                            \
        .Block = {                                                               \
            .Read.Request  = &vsf_usart##__N##_request_read,                     \
            .Write.Request = &vsf_usart##__N##_request_write,                    \
        },                                                                       \
        .Event = {                                                               \
            .Register     = &vsf_usart_evt_register,                             \
            .Enable       = &vsf_usart_evt_enable,                               \
            .Disable      = &vsf_usart_evt_disable,                              \
            .Resume       = &vsf_usart_evt_resume,                               \
        },                                                                       \
    },                                                                           
                                                                                 
/********************************VSF_USART_INTERFACE*****************************/
#define __VSF_USART_INTERFACE()          const i_usart_t VSF_USART[USART_COUNT]
    
/**********************************REPEAT_MACRO**********************************/
REPEAT_MACRO(USART_COUNT, __VSF_USART_FUNC_BODY, NULL);

__VSF_USART_INTERFACE() = {
    REPEAT_MACRO(USART_COUNT, __VSF_USART_INTERFACE_DEF, NULL)
};

