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
#include "service/vsf_service_cfg.h"
#if VSF_USE_PBUF == ENABLED

/*! \NOTE: Make sure #include "utilities/ooc_class.h" is close to the class
 *!        definition and there is NO ANY OTHER module-interface-header file 
 *!        included in this file
 */
#define __PLOOC_CLASS_USE_STRICT_TEMPLATE__
   
#if     defined(__VSF_PBUF_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT
#   undef __VSF_PBUF_CLASS_IMPLEMENT
#elif   defined(__VSF_PBUF_CLASS_INHERIT)
#   define __PLOOC_CLASS_INHERIT
#   undef __VSF_PBUF_CLASS_INHERIT
#endif   

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/
#if     defined(VSF_POOL_CFG_SUPPORT_USER_OBJECT) \
    &&  VSF_POOL_CFG_SUPPORT_USER_OBJECT != ENABLED
#   error vsf_pbuf_t depends on a feature from vsf_pool_t which is not enabled. \
1) if vsf_pool_t is compiled with the source code, please set macro \
VSF_POOL_CFG_SUPPORT_USER_OBJECT to ENABLED.\
2) if vsf_pool_t is includeded in some library, please add the source code of \
vsf_pool_t into your project and follow the step 1). 
#endif


/*============================ MACROFIED FUNCTIONS ===========================*/
#define __BLOCK_FREE_TO(__N, __NUM)            VSF_PBUF_FREE_TO_##__N = __N+1,
 
/*============================ TYPES =========================================*/

enum {
    VSF_PBUF_FREE_TO_ANY = 0,
    MREPEAT(254, __BLOCK_FREE_TO, NULL)
    VSF_PBUF_NO_FREE     = 255
};

declare_class(vsf_pbuf_t)

/*! \name prototype of pbuf request event handler
 *! \param ptTarget target object address
 *! \param nNoLessThan Specify the smallest available size of the pbuf, 
 *!        (-1) means don't care.
 *! \param nBestSize Specify the most desired size of the pbuf, 
 *!        (-1) means don't care.
 *! \return the address of the allocated pbuf, (NULL) means allocation is failed.
 */
typedef vsf_pbuf_t *req_pbuf_evt_handler(   void *pTarget, 
                                            int_fast32_t nNoLessThan,
                                            int_fast32_t nBestSize,
                                            uint_fast16_t hwReserve);

typedef struct req_pbuf_evt_t req_pbuf_evt_t;
struct req_pbuf_evt_t {
    req_pbuf_evt_handler    *fnHandler;
    void                    *pTarget;
};

//! \brief fixed memory block used as stream buffer
//! @{
typedef union {
    struct {
        uint32_t    u24BlockSize        : 24;
        uint32_t    isNoWrite           : 1;
        uint32_t    isNoRead            : 1;
        uint32_t    isNoDirectAccess    : 1;
        uint32_t    u5REFCount          : 5;
    };
    uint32_t        Capability;
}vsf_pbuf_capability_t;

def_class(vsf_pbuf_t, 
    which(
        implement(vsf_slist_node_t)
    ),

    private_member(
        implement_ex(vsf_pbuf_capability_t, tFeature)
        
        uint8_t            *pchBuffer;                                         
       
        uint32_t            u24Size             : 24;
        uint32_t            u8AdapterID           : 8;
    )
)
end_def_class(vsf_pbuf_t)
//! @}

//! \name special methods for accessing blocks
//! \note the instance of i_block_methods_t should stored in ROM
//! @{
def_interface(i_pbuf_methods_t)

    /*! \retval NULL    target pbuf has been free-ed
     *! \retval !NULL   Only reference number decreased.
     */
    vsf_pbuf_t * (*Free)(void *, vsf_pbuf_t *ptBlock);         //!< User specified free

#if VSF_PBUF_CFG_INDIRECT_RW_SUPPORT == ENABLED
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
#endif
end_def_interface(i_pbuf_methods_t)
//! @}

struct vsf_pbuf_adapter_t {
    void *ptTarget;                             //!< user defined target obj
    uint_fast8_t ID;                            //!< Array index
    const i_pbuf_methods_t *piMethods;          //!< block methods
};
typedef struct vsf_pbuf_adapter_t vsf_pbuf_adapter_t;



typedef struct {
    void *              pBuffer;
    vsf_pbuf_capability_t;
    uint_fast8_t        AdapterID;
} vsf_pbuf_cfg_t;


def_interface(i_pbuf_t)
    
    vsf_pbuf_t *       (*Init)(vsf_pbuf_t *, vsf_pbuf_cfg_t *);

    struct {
        vsf_pbuf_t *   (*Free)(vsf_pbuf_t *);
        struct {
            void        (*Get) (void *ptarget, 
                                void *pitem, 
                                uint_fast32_t size);
        }ItemInitEventHandler;
    } PoolHelper;
    
    struct {
        //! method to register an pbuf adapter array, size < 255
        void            (*Register)(const vsf_pbuf_adapter_t *padapter,
                                    uint_fast8_t size);
        const vsf_pbuf_adapter_t *
                        (*Get)(uint_fast8_t chID);
    } Adapter;

    struct {
        vsf_pbuf_capability_t (*Get)(vsf_pbuf_t *);
    } Capability;

    struct {
        int_fast32_t    (*Get)(vsf_pbuf_t *);
        void            (*Set)(vsf_pbuf_t *, int_fast32_t);
        void            (*Reset)(vsf_pbuf_t *);
        int_fast32_t    (*Capacity)(vsf_pbuf_t *);
    } Size;

    struct {
        void *          (*Get)(vsf_pbuf_t *);
        int_fast32_t    (*Write)   (vsf_pbuf_t *pobj, 
                                    const void *psrc, 
                                    int_fast32_t size, 
                                    uint_fast32_t offsite);
        int_fast32_t       (*Read)    (vsf_pbuf_t *pobj, 
                                    void *psrc, 
                                    int_fast32_t size, 
                                    uint_fast32_t offsite);
    } Buffer;

#if VSF_PBUF_CFG_SUPPORT_REF_COUNTING == ENABLED
    struct {
        void           (*Increase)(vsf_pbuf_t *);
        vsf_pbuf_t *   (*Decrease)(vsf_pbuf_t *);
    }REF;
#endif
end_def_interface(i_pbuf_t)

/*============================ GLOBAL VARIABLES ==============================*/

extern const i_pbuf_t VSF_PBUF;

/*============================ PROTOTYPES ====================================*/
extern vsf_pbuf_t *vsf_pbuf_free(vsf_pbuf_t *pbuf);
extern vsf_pbuf_t *vsf_pbuf_init(vsf_pbuf_t *ptBlock, vsf_pbuf_cfg_t *pcfg);
extern void vsf_adapter_register(   const vsf_pbuf_adapter_t *ptAdaptors, 
                                    uint_fast8_t chSize);
extern const vsf_pbuf_adapter_t *vsf_pbuf_adapter_get(uint_fast8_t chID);
extern vsf_pbuf_capability_t vsf_pbuf_capability_get(vsf_pbuf_t *ptObj);
extern void vsf_pbuf_size_reset(vsf_pbuf_t *pobj);
extern void *vsf_pbuf_buffer_get(vsf_pbuf_t *pobj);
extern void vsf_pbuf_size_set(vsf_pbuf_t *pobj, int_fast32_t size);

extern int_fast32_t vsf_pbuf_size_get(vsf_pbuf_t *pobj);
extern int_fast32_t vsf_pbuf_capacity_get(vsf_pbuf_t *pobj);

extern int_fast32_t vsf_pbuf_buffer_write( vsf_pbuf_t *pobj, 
                                            const void *psrc, 
                                            int_fast32_t size, 
                                            uint_fast32_t offsite);
extern int_fast32_t vsf_pbuf_buffer_read(  vsf_pbuf_t *pobj, 
                                        void *psrc, 
                                        int_fast32_t size, 
                                        uint_fast32_t offsite);
extern void vsf_pbuf_pool_item_init_event_handler(  void *ptarget, 
                                                    void *pitem, 
                                                    uint_fast32_t item_size);
#if VSF_PBUF_CFG_SUPPORT_REF_COUNTING == ENABLED
extern void vsf_pbuf_ref_increase(vsf_pbuf_t *pbuf);
#endif
/*============================ INCLUDES ======================================*/

#endif
#endif
/* EOF */
