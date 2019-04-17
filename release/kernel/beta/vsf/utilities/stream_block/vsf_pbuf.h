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

#ifndef __VSF_PBUF_H__
#define __VSF_PBUF_H__

/*============================ INCLUDES ======================================*/

#include "../ooc.h"
#include "../communicate.h"
#include "../preprocessor/mrepeat.h"

#include "./__class_pbuf.h"
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
#define __BLOCK_FREE_TO(__N, __NUM)            VSF_PBUF_FREE_TO_##__N = __N+1,
 
/*============================ TYPES =========================================*/

enum {
    VSF_PBUF_FREE_TO_ANY = 0,
    MREPEAT(254, __BLOCK_FREE_TO, NULL)
    VSF_PBUF_NO_FREE     = 255
};

//! \name special methods for accessing blocks
//! \note the instance of i_block_methods_t should stored in ROM
//! @{
def_interface(i_pbuf_methods_t)

    void (*Free)(void *, vsf_pbuf_t *ptBlock);         //!< User specified free
    
    //! interface for reading target memory
    int_fast32_t (*Read)(   void *ptarget, 
                            void *pbuf_addr_ptr,
                            void *pbuffer,            
                            int_fast32_t size, 
                            uint_fast32_t offset);
                            
    //! interface for write target memory
    int_fast32_t (*Write)(  void *ptarget, 
                            void *pbuf_addr_ptr,
                            const void *pbuffer, 
                            int_fast32_t size, 
                            uint_fast32_t offset);

end_def_interface(i_pbuf_methods_t)
//! @}

struct vsf_pbuf_adapter_t {
    void *ptarget;                              //!< user defined target obj
    uint_fast8_t id;                            //!< Array index
    const i_pbuf_methods_t *pmethods;           //!< block methods
};
typedef struct vsf_pbuf_adapter_t vsf_pbuf_adapter_t;

typedef struct {
    void *          pbuf;
    union {
        struct {
            uint32_t    pbuf_size           : 24;
            uint32_t    is_no_write         : 1;
            uint32_t    is_no_read          : 1;
            uint32_t    is_no_direct_access : 1;
            uint32_t                        : 5;
        };
        uint32_t        capability;
    };
    uint_fast8_t    adapter_id;
} vsf_pbuf_cfg_t;


def_interface(i_pbuf_t)
    
    vsf_pbuf_t *       (*Init)(vsf_pbuf_t *, vsf_pbuf_cfg_t *);
    
    struct {
        vsf_pbuf_t *   (*Free)(vsf_pbuf_t *);
        struct {
             void   (*Get) (void *ptarget, 
                            void *pitem, 
                            uint_fast32_t size);
        }ItemInitEventHandler;
    } PoolHelper;
    
    struct {
        //! method to register an pbuf adapter array, size < 255
        void            (*Register)(vsf_pbuf_adapter_t *padapter,
                                    uint_fast8_t size);
    } Adapter;
   
    struct {
        uint_fast16_t   (*Get)(vsf_pbuf_t *);
        void            (*Set)(vsf_pbuf_t *, uint_fast16_t);
        void            (*Reset)(vsf_pbuf_t *);
        uint_fast16_t   (*Capacity)(vsf_pbuf_t *);
    } Size;
    struct {
        void *          (*Get)(vsf_pbuf_t *);
        bool            (*Write)   (vsf_pbuf_t *pobj, 
                                    const void *psrc, 
                                    int_fast32_t size, 
                                    uint_fast32_t offsite);
        vsf_mem_t       (*Read)    (vsf_pbuf_t *pobj, 
                                    void *psrc, 
                                    int_fast32_t size, 
                                    uint_fast32_t offsite);
    } Buffer;
    
end_def_interface(i_pbuf_t)

/*============================ GLOBAL VARIABLES ==============================*/

extern const i_pbuf_t VSF_PBUF;

/*============================ PROTOTYPES ====================================*/
extern vsf_pbuf_t * vsf_pbuf_free(vsf_pbuf_t *pbuf);
extern vsf_pbuf_t *vsf_pbuf_init(vsf_pbuf_t *ptBlock, vsf_pbuf_cfg_t *pcfg);
extern void vsf_adapter_register(   vsf_pbuf_adapter_t *ptAdaptors, 
                                    uint_fast8_t chSize);

extern void vsf_pbuf_size_reset(vsf_pbuf_t *pobj);
extern void *vsf_pbuf_buffer_get(vsf_pbuf_t *pobj);
extern void vsf_pbuf_size_set(vsf_pbuf_t *pobj, uint_fast16_t size);

extern uint_fast16_t vsf_pbuf_size_get(vsf_pbuf_t *pobj);
extern uint_fast16_t vsf_pbuf_capacity_get(vsf_pbuf_t *pobj);

extern bool vsf_pbuf_buffer_write(  vsf_pbuf_t *pobj, 
                                    const void *psrc, 
                                    int_fast32_t size, 
                                    uint_fast32_t offsite);
extern vsf_mem_t vsf_pbuf_buffer_read(  vsf_pbuf_t *pobj, 
                                        void *psrc, 
                                        int_fast32_t size, 
                                        uint_fast32_t offsite);
extern void vsf_pbuf_pool_item_init_event_handler(  void *ptarget, 
                                                    void *pitem, 
                                                    uint_fast32_t item_size);
                                    
/*============================ INCLUDES ======================================*/


#endif
/* EOF */
