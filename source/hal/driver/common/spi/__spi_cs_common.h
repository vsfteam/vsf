/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

#ifndef __HAL_DRIVER_COMMON_SPI_CS_H__
#define __HAL_DRIVER_COMMON_SPI_CS_H__

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

#ifndef VSF_MULTI_SPI_MAX_CS
#   define VSF_MULTI_SPI_MAX_CS         8
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define __MULTI_SPI_DEF(__N, __MSPI)                                            \
    extern const i_spi_t __MSPI##__N;

/*============================ TYPES =========================================*/

typedef struct vsf_spi_cs_request_t {
    vsf_dlist_node_t        node;

    int8_t                  cs;
    void                   *out_buffer_ptr;
    void                   *in_buffer_ptr;
    uint_fast32_t           count;
} vsf_spi_cs_request_t;

typedef struct vsf_spi_cs_one_t {
    int_fast32_t            transfered_count;
    em_spi_irq_mask_t       irq_mask;
    spi_cfg_t               spi_cfg;

    vsf_spi_cs_request_t    request;
} vsf_spi_cs_one_t;

typedef struct vsf_spi_cs_t {
    vsf_spi_t        *spi;

    uint8_t           init_mask;
    uint8_t           en_mask;
    uint8_t           cs_mask;

    int8_t            cur_cs;
    em_spi_irq_mask_t irq_mask;     // All CS IRQ Mask Wire-OR

    vsf_dlist_t       list;

    vsf_spi_cs_one_t  cfgs[0];

} vsf_spi_cs_t;


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/


#if SPI_MAX_PORT >= 0 && VSF_HAL_USE_SPI0 == ENABLED && (SPI_PORT_MASK & (1 << 0))
#   ifndef VSF_HAL_SPI0_CS_CNT
#       define VSF_HAL_SPI0_CS_CNT 2
#   endif
VSF_MREPEAT(VSF_HAL_SPI0_CS_CNT, __MULTI_SPI_DEF, VSF_SPI0_CS)
#endif

#if SPI_MAX_PORT >= 1 && VSF_HAL_USE_SPI1 == ENABLED && (SPI_PORT_MASK & (1 << 1))
#   ifndef VSF_HAL_SPI1_CS_CNT
#       define VSF_HAL_SPI1_CS_CNT 1
#   endif
VSF_MREPEAT(VSF_HAL_SPI1_CS_CNT, __MULTI_SPI_DEF, VSF_SPI1_CS)
#endif

#if SPI_MAX_PORT >= 2 && VSF_HAL_USE_SPI2 == ENABLED && (SPI_PORT_MASK & (1 << 2))
#   ifndef VSF_HAL_SPI2_CS_CNT
#       define VSF_HAL_SPI2_CS_CNT 1
#   endif
VSF_MREPEAT(VSF_HAL_SPI2_CS_CNT, __MULTI_SPI_DEF, VSF_SPI2_CS)
#endif

#if SPI_MAX_PORT >= 3 && VSF_HAL_USE_SPI3 == ENABLED && (SPI_PORT_MASK & (1 << 3))
#   ifndef VSF_HAL_SPI3_CS_CNT
#       define VSF_HAL_SPI3_CS_CNT 1
#   endif
VSF_MREPEAT(VSF_HAL_SPI3_CS_CNT, __MULTI_SPI_DEF, VSF_SPI3_CS)
#endif

#if SPI_MAX_PORT >= 4 && VSF_HAL_USE_SPI4 == ENABLED && (SPI_PORT_MASK & (1 << 4))
#   ifndef VSF_HAL_SPI4_CS_CNT
#       define VSF_HAL_SPI4_CS_CNT 1
#   endif
VSF_MREPEAT(VSF_HAL_SPI4_CS_CNT, __MULTI_SPI_DEF, VSF_SPI4_CS)
#endif

#if SPI_MAX_PORT >= 5 && VSF_HAL_USE_SPI5 == ENABLED && (SPI_PORT_MASK & (1 << 5))
#   ifndef VSF_HAL_SPI5_CS_CNT
#       define VSF_HAL_SPI5_CS_CNT 1
#   endif
VSF_MREPEAT(VSF_HAL_SPI5_CS_CNT, V__MULTI_SPI_DEF, SF_SPI5_CS)
#endif

#if SPI_MAX_PORT >= 6 && VSF_HAL_USE_SPI6 == ENABLED && (SPI_PORT_MASK & (1 << 6))
#   ifndef VSF_HAL_SPI6_CS_CNT
#       define VSF_HAL_SPI6_CS_CNT 1
#   endif
VSF_MREPEAT(VSF_HAL_SPI6_CS_CNT, __MULTI_SPI_DEF, VSF_SPI6_CS)
#endif

#if SPI_MAX_PORT >= 7 && VSF_HAL_USE_SPI7 == ENABLED && (SPI_PORT_MASK & (1 << 7))
#   ifndef VSF_HAL_SPI7_CS_CNT
#       define VSF_HAL_SPI7_CS_CNT 1
#   endif
VSF_MREPEAT(VSF_HAL_SPI7_CS_CNT, __MULTI_SPI_DEF, VSF_SPI7_CS)
#endif

#endif
