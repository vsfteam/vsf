/*
 * Copyright (c) 2018-2019
 * Jianjia Ma, Wearable Bio-Robotics Group (WBR)
 * majianjia@live.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-02-05     Jianjia Ma   The first version
 */

#ifndef __NNOM_PORT_H__
#define __NNOM_PORT_H__

#include "service/vsf_service.h"
#include "utilities/vsf_utilities.h"
#include "kernel/vsf_kernel.h"

//#include <stdint.h>
//#include <string.h>
//#include <stdbool.h>
//#include <stdlib.h>
//#include <stdio.h>

// memory interfaces
#define nnom_malloc(n)      vsf_heap_malloc(n) 
#define nnom_free(p)        vsf_heap_free(p)
#define nnom_memset(p,v,s)  memset(p,v,s)

// runtime & debuges
#define nnom_us_get()       vsf_systimer_get_us()
#define nnom_ms_get()       vsf_systimer_get_ms()
#define NNOM_LOG(...)       vsf_trace(VSF_TRACE_INFO, __VA_ARGS__)

// NNoM configuration
#define NNOM_BLOCK_NUM  	(8)		// maximum number of memory block  
#define DENSE_WEIGHT_OPT 	(1)		// if used fully connected layer optimized weights. 

// Backend format configuration
#ifdef __ARM_ARCH_PROFILE
// use CMSIS-NN for optimation 
#   define NNOM_USING_CMSIS_NN
#else
// use CHW format. otherwise using default HWC format.
// Notes, CHW is incompatible with CMSIS-NN. 
// CHW must be used when using hardware accelerator such as KPU in K210 chip
#   define NNOM_USING_CHW
#endif


#endif



