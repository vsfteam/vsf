#include "./vsf_usart.h"

/**********************************define tEvtMask*******************************/
usart_evt_status_t evt_mask = 0x00;

/***********************************usart_init***********************************/
vsf_err_t vsf_usart_init(vsf_usart_t *usart_ptr, usart_cfg_t *cfg_ptr)
{
    ASSERT(usart_ptr->obj_ptr != NULL);
    
    float usart_div;
	uint16_t integer_part;
	uint16_t decimal_part;	   

/*USART1_CLK_GPIO_NVIC*/    
    if(USART1 == usart_ptr->obj_ptr) {
        RCC->APB2ENR |= USART1_CLK_EN|GPIOA_CLK_EN; 
        
        GPIOA->CRH &= USART1_GPIO_MODE_CLEAR;
        GPIOA->CRH |= USART1_GPIO_MODE; 

        usart_div = (float)PCLK2/(cfg_ptr->baudrate*16);
        
        /* USART1 interrupt Init */
        HAL_NVIC_SetPriority(USART1_IRQn, 1, 1);
        HAL_NVIC_EnableIRQ(USART1_IRQn);
    }
    
    
/*USART2_CLK_GPIO_NVIC*/  
    if(USART2 == usart_ptr->obj_ptr) {
        RCC->APB1ENR |= USART2_CLK_EN;
        RCC->APB2ENR |= GPIOA_CLK_EN; 
        
        GPIOA->CRL &= USART2_GPIO_MODE_CLEAR;
        GPIOA->CRL |= USART2_GPIO_MODE; 

        usart_div = (float)PCLK1/(cfg_ptr->baudrate*16);
        
        /* USART2 interrupt Init */
        HAL_NVIC_SetPriority(USART2_IRQn, 1, 1);
        HAL_NVIC_EnableIRQ(USART2_IRQn);
    }
    
    
/*USART3_CLK_GPIO_NVIC*/ 
    if(USART3 == usart_ptr->obj_ptr) {
        RCC->APB1ENR |= USART3_CLK_EN;
        RCC->APB2ENR |= GPIOB_CLK_EN; 
        
        GPIOB->CRH &= USART3_GPIO_MODE_CLEAR;
        GPIOB->CRH |= USART3_GPIO_MODE; 

        usart_div = (float)PCLK1/(cfg_ptr->baudrate*16);

        HAL_NVIC_SetPriority(USART3_IRQn, 1, 1);
        HAL_NVIC_EnableIRQ(USART3_IRQn);
    }
      
    integer_part = usart_div;
    decimal_part = (usart_div - integer_part) * 16;	 
    integer_part <<= 4;
    integer_part += decimal_part;
    
    usart_ptr->obj_ptr->BRR  = integer_part;
    usart_ptr->obj_ptr->CR1 |= cfg_ptr->mode;
    usart_ptr->obj_ptr->CR1 |= USART_EN;
    
    return VSF_ERR_NONE;
}

/**********************************usart_enable**********************************/
fsm_rt_t vsf_usart_enable(vsf_usart_t *usart_ptr)
{
    ASSERT(usart_ptr->obj_ptr != NULL);
    
    usart_ptr->obj_ptr->CR1 |= USART_EN;
    return fsm_rt_cpl;
}

/**********************************usart_disable*********************************/
fsm_rt_t vsf_usart_disable(vsf_usart_t *usart_ptr)
{
    ASSERT(usart_ptr->obj_ptr != NULL);
    
    usart_ptr->obj_ptr->CR1 |= USART_DISEN;
    return fsm_rt_cpl;
}

/***********************************usart_status*********************************/
usart_status_t vsf_usart_status(vsf_usart_t *usart_ptr)
{
    ASSERT(usart_ptr->obj_ptr != NULL);
    
    usart_status_t state_break = {
        .is_busy = false
    };
    
    if(usart_ptr->obj_ptr->SR & USART_SR_IDLE_FREE) { 
        state_break.is_busy = true;
    }
    
    return state_break;
}

/*********************************usart_read_byte********************************/
bool vsf_usart_read_byte(vsf_usart_t *usart_ptr, uint8_t *byte_ptr) 
{
    ASSERT(usart_ptr->obj_ptr != NULL);
    
    SAFE_ATOM_CODE() {
        if(usart_ptr->obj_ptr->SR & USART_SR_RXNE_FULL) {
            *byte_ptr = usart_ptr->obj_ptr->DR;
            
            usart_ptr->evt_status.evt_status |= VSF_USART_EVT_RX;
            if((usart_ptr->evt_rx.handler_fn != NULL) && (evt_mask & VSF_USART_EVT_RX)) {
                usart_ptr->evt_rx.handler_fn(usart_ptr->evt_rx.target_ptr, usart_ptr, usart_ptr->evt_status);
            }
            
            return true;
        }
    }
    return false;
}

/*********************************usart_write_byte*******************************/
bool vsf_usart_write_byte(vsf_usart_t *usart_ptr, uint_fast8_t byte) 
{
    ASSERT(usart_ptr->obj_ptr != NULL);
    
    uint8_t timer = 100;
    
    SAFE_ATOM_CODE() {
        if(usart_ptr->obj_ptr->SR & USART_SR_TXE_TRUE) {
            usart_ptr->obj_ptr->DR = byte;
        } else {
            return false;
        }
        
        while(timer--){
            if(USART_SR_TC_TRUE == (usart_ptr->obj_ptr->SR & USART_SR_TC_TRUE)) {
                
                usart_ptr->evt_status.evt_status |= VSF_USART_EVT_TX;
                if((usart_ptr->evt_tx.handler_fn != NULL) && (evt_mask & VSF_USART_EVT_TX)) {
                    usart_ptr->evt_tx.handler_fn(usart_ptr->evt_tx.target_ptr, usart_ptr, usart_ptr->evt_status);
                }
                
                return true;
            }
        }
    }
    return false;
}

/********************************usart_request_read******************************/
fsm_rt_t vsf_usart_request_read(vsf_usart_t *usart_ptr, uint8_t *buffer_ptr, uint_fast32_t size)
{
    ASSERT(usart_ptr->obj_ptr != NULL);
    ASSERT(buffer_ptr != NULL);
    ASSERT(size != 0);
    
    SAFE_ATOM_CODE() {
        if(false == usart_ptr->is_loading) {
            usart_ptr->is_loading       = true;
            usart_ptr->read_buffer_ptr  = buffer_ptr;
            usart_ptr->read_size        = size - 1;
            usart_ptr->read_sizecounter = 0; 
            usart_ptr->obj_ptr->CR1     |= USART_CR1_RXNEIE_EN;
        }
    }
    
    if(usart_ptr->read_sizecounter > usart_ptr->read_size) {
        usart_ptr->is_loading = false;
        
        usart_ptr->evt_status.evt_status |= VSF_USART_EVT_RCV_BLK_CPL;
        if((usart_ptr->evt_rcv.handler_fn != NULL) && (evt_mask & VSF_USART_EVT_RCV_BLK_CPL)) {
            usart_ptr->evt_rcv.handler_fn(usart_ptr->evt_rcv.target_ptr, usart_ptr, usart_ptr->evt_status);
        }
        
        return fsm_rt_cpl;
    }
    
    return fsm_rt_asyn;
}

/*******************************usart_request_write******************************/
fsm_rt_t vsf_usart_request_write(vsf_usart_t *usart_ptr, uint8_t *buffer_ptr, uint_fast32_t size)
{
    ASSERT(usart_ptr->obj_ptr != NULL);
    ASSERT(buffer_ptr != NULL);
    ASSERT(size != 0);
        
    SAFE_ATOM_CODE() {
        if(false == usart_ptr->is_writing) {
            usart_ptr->is_writing        = true;
            usart_ptr->write_buffer_ptr  = buffer_ptr;
            usart_ptr->write_size        = size - 1;
            usart_ptr->write_sizecounter = 0;
            usart_ptr->obj_ptr->CR1      |= USART_CR1_TXEIE_EN;
        }
    }
    
    if(usart_ptr->write_sizecounter >= usart_ptr->write_size) {
        usart_ptr->is_writing = false;
        
        usart_ptr->evt_status.evt_status |= VSF_USART_EVT_SND_BLK_CPL;
        if((usart_ptr->evt_send.handler_fn != NULL) && (evt_mask & VSF_USART_EVT_SND_BLK_CPL)) {
            usart_ptr->evt_send.handler_fn(usart_ptr->evt_send.target_ptr, usart_ptr, usart_ptr->evt_status);
        }
        
        return fsm_rt_cpl;
    }
        
    return fsm_rt_asyn;
}

/*******************************vsf_usart_riqhandler*****************************/
void vsf_usart_irqhandler(vsf_usart_t *usart_ptr)
{
//    SAFE_ATOM_CODE() {
        if(usart_ptr->obj_ptr->SR & USART_SR_RXNE_FULL) {
            usart_ptr->read_buffer_ptr[usart_ptr->read_sizecounter] = usart_ptr->obj_ptr->DR;
            if((usart_ptr->read_sizecounter++) >= usart_ptr->read_size) {
                usart_ptr->obj_ptr->CR1 &= USART_CR1_RXNEIE_DISEN;
            }
        } else {
            usart_ptr->obj_ptr->DR = usart_ptr->write_buffer_ptr[usart_ptr->write_sizecounter];
            if((usart_ptr->write_sizecounter++) >= usart_ptr->write_size) {
                usart_ptr->obj_ptr->CR1 &= USART_CR1_TXEIE_DISEN;
            }
        }
//    }
}

/*********************************usart_huart_arrcy******************************/
#define __USART_HUART(__N, __VALUE)                                              \
                                                                                 \
        {                                                                        \
            .obj_ptr = USART##__N                                                \
        },                             

static vsf_usart_t usart_huart[] = {
    REPEAT_MACRO(USART_COUNT, __USART_HUART, NULL)
};

/*******************************vsf_evt_usart_register***************************/
void vsf_usart_evt_register(vsf_usart_evt_type_t tType, vsf_usart_evt_t event)
{
    if(VSF_USART_EVT_RX == tType) {
        for(uint8_t i =1; i < USART_COUNT; i++) {
            usart_huart[i].evt_rx = event;
        }
    }
    
    if(VSF_USART_EVT_TX == tType) {
        for(uint8_t i =1; i < USART_COUNT; i++) {
            usart_huart[i].evt_tx = event;
        }
    }
    
    if(VSF_USART_EVT_RCV_BLK_CPL == tType) {
        for(uint8_t i =1; i < USART_COUNT; i++) {
            usart_huart[i].evt_rcv = event;
        }
    }
    
    if(VSF_USART_EVT_SND_BLK_CPL == tType) {
        for(uint8_t i =1; i < USART_COUNT; i++) {
            usart_huart[i].evt_send = event;
        }
    }
}

/*******************************vsf_usart_evt_enable*****************************/
usart_evt_status_t vsf_usart_evt_enable(usart_evt_status_t event_mask)
{
    if(evt_mask != 0xFF) {
        evt_mask |= event_mask;
    }
    return evt_mask;
}

/*******************************vsf_usart_evt_disable****************************/
usart_evt_status_t vsf_usart_evt_disable(usart_evt_status_t event_mask)
{
    if(evt_mask != 0x00) {
        evt_mask &= (~event_mask);
    }
    return evt_mask;
}

/*******************************vsf_usart_evt_resume*****************************/
void vsf_usart_evt_resume(usart_evt_status_t tEventStatus)
{
    evt_mask = tEventStatus;
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
bool vsf_usart##__N##_read_byte(uint8_t *byte_ptr)                               \
{                                                                                \
    return vsf_usart_read_byte(&usart_huart[__N], byte_ptr);                     \
}                                                                                \
                                                                                 \
/*usart_write_byte*/                                                             \
bool vsf_usart##__N##_write_byte(uint_fast8_t byte)                              \
{                                                                                \
    return vsf_usart_write_byte(&usart_huart[__N], byte);                        \
}                                                                                \
                                                                                 \
/*usart_request_read*/                                                           \
fsm_rt_t vsf_usart##__N##_request_read(uint8_t *buffer_ptr, uint_fast32_t size)  \
{                                                                                \
    return vsf_usart_request_read(&usart_huart[__N], buffer_ptr, size);          \
}                                                                                \
                                                                                 \
/*usart_request_write*/                                                          \
fsm_rt_t vsf_usart##__N##_request_write(uint8_t *buffer_ptr, uint_fast32_t size) \
{                                                                                \
    return vsf_usart_request_write(&usart_huart[__N], buffer_ptr, size);         \
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

