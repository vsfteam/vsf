/****************************************************************************
 * @file     NuConsole_Config.h
 * @version  V1.00
 * $Revision: 0 $
 * $Date: 16/07/06 0:00p $
 * @brief    
 *
 * @note
 * Copyright (C) 2016 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

#ifndef __NUCONSOLE_CONFIG_H__
#define __NUCONSOLE_CONFIG_H__

#include "component/vsf_component_cfg.h"

#if VSF_DEBUGGER_CFG_CONSOLE == VSF_DEBUGGER_CFG_CONSOLE_NULINK_NUCONSOLE

#define NUCOSOLE_TX_BUFFER_SIZE    1024
#define NUCOSOLE_RX_BUFFER_SIZE    8

#define NUCONSOLE_FIFO_DEFAULT_MODE    NUCONSOLE_FIFO_MODE_NON_BLOCKING

#endif
#endif
