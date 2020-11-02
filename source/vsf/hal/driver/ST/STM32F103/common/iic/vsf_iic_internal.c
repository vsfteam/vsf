#include "./vsf_iic_internal.h"

/*****************************master_evt_handler******************************/

void vsf_iic_m_w_evt_sb_handler(vsf_iic_t *iic_obj)
{
    iic_obj->iic_reg->DR = iic_obj->slave_address;
}

void vsf_iic_m_w_evt_addr_handler(vsf_iic_t *iic_obj)
{
    reg32_t sr2_reg;
    sr2_reg = iic_obj->iic_reg->SR2;
}

void vsf_iic_m_w_evt_txe_btf_handler(vsf_iic_t *iic_obj)
{
    if(!iic_obj->no_stop) {
        iic_obj->iic_reg->CR1 |= IIC_STOP;
        iic_obj->iic_reg->CR2 &= ~(IIC_IT_EVT_EN | IIC_IT_BUF_EN | IIC_IT_ERR_EN);
        iic_obj->iic_reg->CR1 &= ~IIC_EN;
        iic_obj->iic_is_busy  = false;
        iic_obj->no_stop      = false;
    } else {
        if(iic_obj->out_count < iic_obj->out_size) {
            reg32_t sr1_reg;
            sr1_reg = iic_obj->iic_reg->SR1;
            iic_obj->iic_reg->DR = iic_obj->output[iic_obj->out_count++];
        }
    }
    
    if(iic_obj->callback_handler != NULL) {
        iic_obj->callback_handler(iic_obj);
    }
    
}

void vsf_iic_m_w_evt_txe_handler(vsf_iic_t *iic_obj)
{
    if(iic_obj->out_count < iic_obj->out_size) {
        iic_obj->iic_reg->DR = iic_obj->output[iic_obj->out_count++];
    }
}

void vsf_iic_m_r_evt_sb_handler(vsf_iic_t *iic_obj)
{
    iic_obj->iic_reg->DR = iic_obj->slave_address;
}

void vsf_iic_m_r_evt_addr_handler(vsf_iic_t *iic_obj)
{
    reg32_t sr2_reg;
    sr2_reg = iic_obj->iic_reg->SR2;
}

void vsf_iic_m_r_evt_rxne_handler(vsf_iic_t *iic_obj)
{
    if(iic_obj->in_count < iic_obj->in_size) {
                    
        if(iic_obj->in_count == (iic_obj->in_size -2)) {
            iic_obj->iic_reg->CR1 &= ~IIC_ACK_EN;
            iic_obj->iic_reg->CR1 |= IIC_STOP;
        }
        
        if(iic_obj->in_count == (iic_obj->in_size - 1)) {
            iic_obj->iic_reg->CR2 &= ~(IIC_IT_EVT_EN | IIC_IT_BUF_EN);
            iic_obj->iic_reg->CR1 &= ~IIC_EN;
            iic_obj->iic_is_busy  = false;
            
            if(iic_obj->callback_handler != NULL) {
                iic_obj->callback_handler(iic_obj);
            }
        }
        
        iic_obj->input[iic_obj->in_count++] = iic_obj->iic_reg->DR;
        
    } 
}

/******************************slave_evt_handler******************************/
void vsf_iic_s_w_evt_addr_handler(vsf_iic_t *iic_obj)
{
    reg32_t sr2_reg;
    sr2_reg = iic_obj->iic_reg->SR2;
}

void vsf_iic_s_w_evt_txe_handler(vsf_iic_t *iic_obj)
{
    if(iic_obj->out_count < iic_obj->out_size) {
                    
        iic_obj->iic_reg->DR = iic_obj->output[iic_obj->out_count++];
        
    }
}

void vsf_iic_s_r_evt_addr_handler(vsf_iic_t *iic_obj)
{
    reg32_t sr2_reg;
    sr2_reg = iic_obj->iic_reg->SR2;
}

void vsf_iic_s_r_evt_stopf_handler(vsf_iic_t *iic_obj)
{
    iic_obj->iic_reg->CR2 &= ~(IIC_IT_EVT_EN | IIC_IT_BUF_EN | IIC_IT_ERR_EN);
    iic_obj->iic_reg->CR1 &= ~IIC_EN;
    iic_obj->iic_is_busy  = false;
}

void vsf_iic_s_r_evt_rxne_handler(vsf_iic_t *iic_obj)
{
    if(iic_obj->in_count < iic_obj->in_size) {
        
        iic_obj->input[iic_obj->in_count++] = iic_obj->iic_reg->DR;
        
    }
}

/*****************************master_err_handler******************************/
void vsf_iic_m_w_err_arlo_handler(vsf_iic_t *iic_obj)
{
    iic_obj->is_arlo   = true;
    iic_obj->is_master = false;
    iic_obj->is_write  = false;
    iic_obj->input     = iic_obj->arlo_buffer;
    iic_obj->in_size   = iic_obj->arlo_size;
    
    iic_obj->iic_reg->SR1 &= ~IIC_ARLO_SET;
    
    iic_obj->iic_reg->CR1 |= IIC_EN;
    iic_obj->iic_reg->CR1 &= ~IIC_POS_EN;
    iic_obj->iic_reg->CR1 |= IIC_ACK_EN;
    iic_obj->iic_reg->CR2 |= IIC_IT_EVT_EN | IIC_IT_BUF_EN;
}

/******************************slave_err_handler******************************/
void vsf_iic_s_r_err_af_handler(vsf_iic_t *iic_obj)
{
    iic_obj->iic_reg->SR1 &= ~IIC_AF_SET; 
    iic_obj->iic_reg->CR1 &= ~IIC_ACK_EN;
    iic_obj->iic_reg->CR2 &= ~(IIC_IT_EVT_EN | IIC_IT_BUF_EN | IIC_IT_ERR_EN);
    iic_obj->iic_reg->CR1 &= ~IIC_EN;
    iic_obj->iic_is_busy  = false;
}

/***************************vsf_iic_master_init*******************************/
bool vsf_iic_master_init(vsf_iic_t *iic_obj, iic_cfg_t *cfg_obj)
{
    if((NULL == iic_obj) || (NULL == cfg_obj)) {
        return false;
    }
    
    uint8_t frequency;
    
    {
        iic_obj->master_evt_handler->m_w_err_arlo    = &vsf_iic_m_w_err_arlo_handler;
        
        iic_obj->master_evt_handler->m_w_evt_addr    = &vsf_iic_m_w_evt_addr_handler;
        iic_obj->master_evt_handler->m_w_evt_sb      = &vsf_iic_m_w_evt_sb_handler;
        iic_obj->master_evt_handler->m_w_evt_txe     = &vsf_iic_m_w_evt_txe_handler;
        iic_obj->master_evt_handler->m_w_evt_txe_btf = &vsf_iic_m_w_evt_txe_btf_handler;
        
        iic_obj->master_evt_handler->m_r_evt_addr    = &vsf_iic_m_r_evt_addr_handler;
        iic_obj->master_evt_handler->m_r_evt_rxne    = &vsf_iic_m_r_evt_rxne_handler;
        iic_obj->master_evt_handler->m_r_evt_sb      = &vsf_iic_m_r_evt_sb_handler;
    }
    
    {
        iic_obj->slave_evt_handler->s_r_err_af       = &vsf_iic_s_r_err_af_handler;
        iic_obj->slave_evt_handler->s_r_evt_addr     = &vsf_iic_s_r_evt_addr_handler;
        iic_obj->slave_evt_handler->s_r_evt_rxne     = &vsf_iic_s_r_evt_rxne_handler;
        iic_obj->slave_evt_handler->s_r_evt_stopf    = &vsf_iic_s_r_evt_stopf_handler;
        
        iic_obj->slave_evt_handler->s_w_evt_addr     = &vsf_iic_s_w_evt_addr_handler;
        iic_obj->slave_evt_handler->s_w_evt_txe      = &vsf_iic_s_w_evt_txe_handler;
    }
    
    vsf_interrupt_safe() {
    
        if(I2C1 == iic_obj->iic_reg) {
            RCC->APB2ENR |= IIC1_GPIO_CLK_EN;
            RCC->APB1ENR |= IIC1_CLK_EN;
            
            GPIOB->CRL |= IIC1_GPIO_MODE;         
            
            HAL_NVIC_SetPriority(I2C1_EV_IRQn, 0, 0);
            HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
            HAL_NVIC_SetPriority(I2C1_ER_IRQn, 0, 0);
            HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);
        }
        
        if(I2C2 == iic_obj->iic_reg) {
            RCC->APB2ENR |= IIC2_GPIO_CLK_EN;
            RCC->APB1ENR |= IIC2_CLK_EN;
            
            GPIOB->CRH |= IIC2_GPIO_MODE;

            HAL_NVIC_SetPriority(I2C2_EV_IRQn, 0, 0);
            HAL_NVIC_EnableIRQ(I2C2_EV_IRQn);
            HAL_NVIC_SetPriority(I2C2_ER_IRQn, 0, 0);
            HAL_NVIC_EnableIRQ(I2C2_ER_IRQn);
        }
        
        frequency = PCLK1 / 1000000;
        
        iic_obj->iic_reg->CR1   &= ~IIC_EN;
        
        iic_obj->iic_reg->CR2   |= (0x3F & frequency);

        iic_obj->iic_reg->TRISE &= 0x00;
        iic_obj->iic_reg->TRISE |= (frequency + 1);

        iic_obj->iic_reg->CCR   |= (0x0FFF & (((500000000U / cfg_obj->clock_speed) / ((972) / frequency)) - 6));

        iic_obj->iic_reg->CR1   |= cfg_obj->general_call_mode | cfg_obj->no_stretch_mode;

        iic_obj->iic_reg->OAR1  |= cfg_obj->addressing_mode | cfg_obj->own_address1;

        iic_obj->iic_reg->OAR2  |= cfg_obj->dual_address_mode | cfg_obj->own_address2;

        iic_obj->iic_reg->CR1   |= IIC_EN;
    }
    
    iic_obj->is_master = true;
    
    return true;
}

/****************************vsf_iic_slave_init*******************************/
bool vsf_iic_slave_init(vsf_iic_t *iic_obj, iic_cfg_t *cfg_obj)
{
    if((NULL == iic_obj) || (NULL == cfg_obj)) {
        return false;
    }
    
    uint8_t frequency;
    
    iic_obj->slave_evt_handler->s_r_err_af     = &vsf_iic_s_r_err_af_handler;
    iic_obj->slave_evt_handler->s_r_evt_addr   = &vsf_iic_s_r_evt_addr_handler;
    iic_obj->slave_evt_handler->s_r_evt_rxne   = &vsf_iic_s_r_evt_rxne_handler;
    iic_obj->slave_evt_handler->s_r_evt_stopf  = &vsf_iic_s_r_evt_stopf_handler;
    
    iic_obj->slave_evt_handler->s_w_evt_addr   = &vsf_iic_s_w_evt_addr_handler;
    iic_obj->slave_evt_handler->s_w_evt_txe    = &vsf_iic_s_w_evt_txe_handler;
    
    vsf_interrupt_safe() {
    
        if(I2C1 == iic_obj->iic_reg) {
            RCC->APB2ENR |= IIC1_GPIO_CLK_EN;
            
            GPIOB->CRL |= IIC1_GPIO_MODE;
            
            RCC->APB1ENR |= IIC1_CLK_EN;

            HAL_NVIC_SetPriority(I2C1_EV_IRQn, 0, 0);
            HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
            HAL_NVIC_SetPriority(I2C1_ER_IRQn, 0, 0);
            HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);
        }
        
        if(I2C2 == iic_obj->iic_reg) {
            RCC->APB2ENR |= IIC2_GPIO_CLK_EN;

            GPIOB->CRH |= IIC2_GPIO_MODE;

            RCC->APB1ENR |= IIC2_CLK_EN;

            HAL_NVIC_SetPriority(I2C2_EV_IRQn, 0, 0);
            HAL_NVIC_EnableIRQ(I2C2_EV_IRQn);
            HAL_NVIC_SetPriority(I2C2_ER_IRQn, 0, 0);
            HAL_NVIC_EnableIRQ(I2C2_ER_IRQn);
        }
        
        frequency = PCLK1 / 1000000;
        
        iic_obj->iic_reg->CR1   &= ~IIC_EN;
        
        iic_obj->iic_reg->CR2   |= (0x3F & frequency);

        iic_obj->iic_reg->TRISE &= 0x00;
        iic_obj->iic_reg->TRISE |= (frequency + 1);

        iic_obj->iic_reg->CCR   |= (0x0FFF & ((500000000U / cfg_obj->clock_speed) / ((972) / frequency)));

        iic_obj->iic_reg->CR1   |= cfg_obj->general_call_mode | cfg_obj->no_stretch_mode;

        iic_obj->iic_reg->OAR1  |= cfg_obj->addressing_mode | cfg_obj->own_address1;

        iic_obj->iic_reg->OAR2  |= cfg_obj->dual_address_mode | cfg_obj->own_address2;

        iic_obj->iic_reg->CR1   |= IIC_EN;
    }
    
    iic_obj->is_master = false;
    
    return true;
}

/****************************vsf_iic_master_write*****************************/
bool vsf_iic_master_write(vsf_iic_t *iic_obj, uint8_t slave_address, uint8_t *output, uint16_t out_size, uint8_t *arlo_buffer, uint16_t arlo_size, uint8_t Process_sele)
{
    if((NULL == iic_obj) || (NULL == output) || (0 == out_size) || (NULL == arlo_buffer) || (0 == arlo_size)) {
        return false;
    }
    
    if(iic_obj->iic_is_busy != false) {
        return false;
    }
    
    iic_obj->slave_address = slave_address;
    iic_obj->output        = output;
    iic_obj->out_size      = out_size;
    iic_obj->arlo_buffer   = arlo_buffer;
    iic_obj->arlo_size     = arlo_size;
    iic_obj->out_count     = 0;
    iic_obj->iic_is_busy   = true;
    iic_obj->is_write      = true;
    iic_obj->is_af_back    = false;
    
    if(Process_sele & NO_ADDR) {
        iic_obj->no_addr = true;
    }
    
    if(Process_sele & NO_STOP) {
        iic_obj->no_stop = true;
    }
    
    vsf_interrupt_safe() {
        iic_obj->iic_reg->CR1 |= IIC_EN;
        iic_obj->iic_reg->CR1 &= ~IIC_POS_EN;
        
        if(!(Process_sele & NO_START)) {
            iic_obj->iic_reg->CR1 |= IIC_START;
        }
        
        iic_obj->iic_reg->CR2 |= IIC_IT_EVT_EN | IIC_IT_BUF_EN | IIC_IT_ERR_EN;
    }
    
    return true;
}

/*****************************vsf_iic_master_read*****************************/
bool vsf_iic_master_read(vsf_iic_t *iic_obj, uint8_t slave_address, uint8_t *input, uint16_t size)
{
    if((NULL == iic_obj) || (NULL == input) || (0 == size)) {
        return false;
    }
    
    if(iic_obj->iic_is_busy != false) {
        return false;
    }
    
    iic_obj->slave_address = slave_address | IIC_MASTER_READ;
    iic_obj->input         = input;
    iic_obj->in_size       = size;
    iic_obj->in_count      = 0;
    iic_obj->iic_is_busy   = true;
    iic_obj->is_write      = false;
    
    vsf_interrupt_safe() {
        iic_obj->iic_reg->CR1 |= IIC_EN;
        iic_obj->iic_reg->CR1 &= ~IIC_POS_EN;
        iic_obj->iic_reg->CR1 |= IIC_ACK_EN;
        iic_obj->iic_reg->CR1 |= IIC_START;
        
        iic_obj->iic_reg->CR2 |= IIC_IT_EVT_EN | IIC_IT_BUF_EN;
    }
    
    return true;
}

/******************************vsf_iic_slave_write****************************/
bool vsf_iic_slave_write(vsf_iic_t *iic_obj, uint8_t *output, uint16_t size)
{
    if((NULL == iic_obj) || (NULL == output) || (0 == size)) {
        return false;
    }
    
    if(iic_obj->iic_is_busy != false) {
        return false;
    }
    
    iic_obj->output      = output;
    iic_obj->out_size    = size;
    iic_obj->out_count   = 0;
    iic_obj->iic_is_busy = true;
    iic_obj->is_write    = true;
    
    vsf_interrupt_safe() {
        iic_obj->iic_reg->CR1 |= IIC_EN;
        iic_obj->iic_reg->CR1 &= ~IIC_POS_EN;
        iic_obj->iic_reg->CR1 |= IIC_ACK_EN;
        
        iic_obj->iic_reg->CR2 |= IIC_IT_EVT_EN | IIC_IT_BUF_EN | IIC_IT_ERR_EN;
    }
    
    return true;
}

/******************************vsf_iic_slave_read*****************************/
bool vsf_iic_slave_read(vsf_iic_t *iic_obj, uint8_t *input, uint16_t size)
{
    if((NULL == iic_obj) || (NULL == input) || (0 == size)) {
        return false;
    }
    
    if(iic_obj->iic_is_busy != false) {
        return false;
    }
    
    iic_obj->input       = input;
    iic_obj->in_size     = size;
    iic_obj->in_count    = 0;
    iic_obj->iic_is_busy = true;
    iic_obj->is_write    = false;
    
    vsf_interrupt_safe() {
        iic_obj->iic_reg->CR1 |= IIC_EN;
        iic_obj->iic_reg->CR1 &= ~IIC_POS_EN;
        iic_obj->iic_reg->CR1 |= IIC_ACK_EN;
        
        iic_obj->iic_reg->CR2 |= IIC_IT_EVT_EN | IIC_IT_BUF_EN;
    }
    
    return true;
}

/*************************vsf_iic_get_request_state***************************/
em_vsf_iic_request_state_t vsf_iic_get_request_state(vsf_iic_t *iic_obj)
{
    ASSERT(iic_obj != NULL);
    
    if(iic_obj->is_arlo != false) {
        return IIC_REQ_ARLO;
    } else if(false == iic_obj->iic_is_busy) {
        return IIC_REQ_SUCCESS;
    }
    
    return IIC_REQ_ON_GOING;
}

/************************vsf_iic_master_register_event************************/
bool vsf_iic_master_register_event( vsf_iic_t *iic_obj, vsf_iic_m_evt_interface_t *evt_obj)
{
    if((NULL == iic_obj) || (NULL == evt_obj)) {
        return false;
    }
    
    iic_obj->master_evt_handler->m_r_evt_addr    = evt_obj->m_r_evt_addr;
    iic_obj->master_evt_handler->m_r_evt_rxne    = evt_obj->m_r_evt_rxne;
    iic_obj->master_evt_handler->m_r_evt_sb      = evt_obj->m_r_evt_sb;
    iic_obj->master_evt_handler->m_w_err_arlo    = evt_obj->m_w_err_arlo;
    iic_obj->master_evt_handler->m_w_evt_addr    = evt_obj->m_w_evt_addr;
    iic_obj->master_evt_handler->m_w_evt_sb      = evt_obj->m_w_evt_sb;
    iic_obj->master_evt_handler->m_w_evt_txe     = evt_obj->m_w_evt_txe;
    iic_obj->master_evt_handler->m_w_evt_txe_btf = evt_obj->m_w_evt_txe_btf;
    
    return true;
}

/************************vsf_iic_slave_register_event*************************/
bool vsf_iic_slave_register_event(vsf_iic_t *iic_obj, vsf_iic_s_evt_interface_t *evt_obj)
{
    if((NULL == iic_obj) || (NULL == evt_obj)) {
        return false;
    }
    
    iic_obj->slave_evt_handler->s_r_err_af    = evt_obj->s_r_err_af;
    iic_obj->slave_evt_handler->s_r_evt_addr  = evt_obj->s_r_evt_addr;
    iic_obj->slave_evt_handler->s_r_evt_rxne  = evt_obj->s_r_evt_rxne;
    iic_obj->slave_evt_handler->s_r_evt_stopf = evt_obj->s_r_evt_stopf;
    iic_obj->slave_evt_handler->s_w_evt_addr  = evt_obj->s_w_evt_addr;
    iic_obj->slave_evt_handler->s_w_evt_txe   = evt_obj->s_w_evt_txe;
    
    return true;
}

/*********************************vsf_iic_task********************************/
fsm_rt_t vsf_iic_task(vsf_iic_t *iic_obj)
{
    if(NULL == iic_obj) {
        return fsm_rt_err;
    }
    
    //master_mode
    if(iic_obj->is_master != false) {
        //master_write
        if(iic_obj->is_write != false) {
            
            reg32_t sr1_reg = iic_obj->iic_reg->SR1;
            
            if(sr1_reg & IIC_SB_SET) {
     
                iic_obj->master_evt_handler->m_w_evt_sb(iic_obj);
                
            } else if(sr1_reg & IIC_ARLO_SET) {
                
                iic_obj->master_evt_handler->m_w_err_arlo(iic_obj);
              
            } else if(sr1_reg & IIC_ADDR_SET) {
                
                iic_obj->master_evt_handler->m_w_evt_addr(iic_obj);
                
            }  else if((sr1_reg & IIC_TXE_SET) && (sr1_reg & IIC_BTF_SET)) {
master_write_end:            
                iic_obj->master_evt_handler->m_w_evt_txe_btf(iic_obj);
                
            } else if(sr1_reg & IIC_TXE_SET) {
                
                iic_obj->master_evt_handler->m_w_evt_txe(iic_obj);
  
            } else if(sr1_reg & IIC_AF_SET) {
                
                if(iic_obj->is_af_back != false) {
                    goto master_write_end;
                }
                
                iic_obj->is_af_back = true;
                iic_obj->iic_reg->SR1 &= ~IIC_AF_SET;
                iic_obj->iic_reg->CR1 |= IIC_START;
                
            }
        //master_read
        } else {
            reg32_t sr1_reg = iic_obj->iic_reg->SR1;
            
            if(sr1_reg & IIC_SB_SET) {
     
                iic_obj->master_evt_handler->m_r_evt_sb(iic_obj);
                
            } else if(sr1_reg & IIC_ADDR_SET) {
                
                iic_obj->master_evt_handler->m_r_evt_addr(iic_obj);
                
            } else if(sr1_reg & IIC_RXNE_SET) {
                
                iic_obj->master_evt_handler->m_r_evt_rxne(iic_obj);
                
            }
            
        }
    //slave_mode
    } else {
        //slave_write
        if(iic_obj->is_write != false) {
            
            reg32_t sr1_reg = iic_obj->iic_reg->SR1;
            
            if(sr1_reg & IIC_ADDR_SET) {
                
                iic_obj->slave_evt_handler->s_w_evt_addr(iic_obj);
                
            } else if(sr1_reg & IIC_TXE_SET) {
                
                iic_obj->slave_evt_handler->s_w_evt_txe(iic_obj);
                
            } else if(sr1_reg & IIC_AF_SET) {
                
                iic_obj->slave_evt_handler->s_r_err_af(iic_obj);
                
            }
            
        //slave_read
        } else {
            reg32_t sr1_reg = iic_obj->iic_reg->SR1;
            
            if(sr1_reg & IIC_ADDR_SET) {
                
                iic_obj->slave_evt_handler->s_r_evt_addr(iic_obj);
                
            } else if(sr1_reg & IIC_STOPF_SET) {
                
                iic_obj->slave_evt_handler->s_r_evt_stopf(iic_obj);
                
            } else if(sr1_reg & IIC_RXNE_SET) {
                
                iic_obj->slave_evt_handler->s_r_evt_rxne(iic_obj);
                
            }        
        }
    }
    
    return fsm_rt_cpl;
}