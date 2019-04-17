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

#ifndef __VSF_LIST_H__
#define __VSF_LIST_H__

/*============================ INCLUDES ======================================*/
#include <stdint.h>

/*============================ MACROS ========================================*/



#define __when(...)                             (__VA_ARGS__)
#define when(...)                               __when(__VA_ARGS__)

/*-----------------------------------------------------------------------------*
 * Single Chain List internal macro                                            *
 *-----------------------------------------------------------------------------*/

#define __vsf_slist_is_empty(plist)             (NULL == (plist)->pnext)

#define __vsf_slist_set_next(__plist, __pnode_nxt)                              \
    do {                                                                        \
        ((vsf_slist_t *)(__plist))->pnext = (vsf_slist_t *)(__pnode_nxt);       \
    } while (0)

#define __vsf_slist_ref_next(__plist, __pitem_ref)                              \
    do {                                                                        \
        (*(vsf_slist_t **)&(__pitem_ref)) = ((vsf_slist_t *)(__plist))->pnext;  \
    } while (0)

#define __vsf_slist_insert_after(__pnode, __pitem, __member)                    \
    do {                                                                        \
        vsf_slist_t *plist = &((__pnode)->__member);                            \
        __vsf_slist_set_next(&((__pitem)->__member), ((plist)->pnext));         \
        __vsf_slist_set_next(plist, (__pitem));                                 \
    } while (0)

#define __vsf_slist_remove_after(   __host_type, /* type of the host object */  \
                                    __member,    /* the name of the list */     \
                                    __pnode,     /* current node address */     \
                                    __pitem_ref) /* pointer of host type*/      \
    do {                                                                        \
        vsf_slist_t *plist = &((__pnode)->__member);                            \
        __vsf_slist_ref_next(plist, (__pitem_ref));                             \
        if (NULL != plist->pnext) {                                             \
            __vsf_slist_set_next(                                               \
                plist,                                                          \
                ((__host_type *)(plist->pnext))->__member.pnext);               \
            __vsf_slist_set_next(&((__pitem_ref)->__member), NULL);             \
        }                                                                       \
    } while (0)



#define __vsf_slist_insert(     __host_type,  /* type of the host object */     \
                                __member,     /* the name of the list */        \
                                __plist,      /* the address of the list */     \
                                __pitem,      /* the address of the new item */ \
                                ...)          /* how to find insert point */    \
    do {                                                                        \
        vsf_slist_init_node(__host_type, __member, __pitem);                    \
        vsf_slist_t *plist_item = (__plist);                                    \
        for (; plist_item->pnext != NULL;){                                     \
            __host_type *_ = (__host_type *)(plist_item->pnext);                \
            /* ptarget might be modified by user, so save a copy */             \
            __host_type *pnode_tmp = ptarget;                                   \
            /* using __VA_ARGS__ so ',' operation could be supported */         \
            if (__VA_ARGS__) {                                                  \
                __vsf_slist_add_item(                                           \
                    __host_type, __member, plist_item, (__pitem));              \
                break;                                                          \
            }                                                                   \
            plist_item = &(pnode_tmp->__member);                                \
        }                                                                       \
        if (NULL == plist_item->pnext) {                                        \
            __vsf_slist_add_item(                                               \
                    __host_type, __member, plist_item, (__pitem));              \
            break;                                                              \
        }                                                                       \
    } while (0)

#define __vsf_slist_foreach(__host_type,/* the type of the host type */         \
                            __member,   /* the member name of the list */       \
                            __plist,    /* the address of the list */           \
                            __pitem)    /* the pointer name, e.g. ptarget */    \
    for (__host_type                    /* loop initialisation */               \
            *__pitem = (__host_type *)(__plist)->pnext,                         \
            *_ = __pitem,                                                       \
            *pnext = __pitem                                                    \
                ?   (__host_type *)                                             \
                        ((__host_type *)((__plist)->pnext))->__member.pnext     \
                :   NULL;                                                       \
            __pitem != NULL;            /* loop condition */                    \
            _ = __pitem = pnext, pnext = __pitem                                \
                ?   (__host_type *)                                             \
                        ((__host_type *)((__plist)->pnext))->__member.pnext     \
                :   NULL)               /* prepare for next iteration */

#define __vsf_slist_discard_head(__host_type, __member, __plist)                \
            if (NULL != (__plist)->pnext) {                                     \
                __host_type *pitem = (__host_type *)((__plist)->pnext);         \
                __vsf_slist_set_next(                                           \
                        (__plist),                                              \
                        ((__host_type *)((__plist)->pnext))->__member.pnext);   \
                pitem->__member.pnext = NULL;                                   \
            }

#define __vsf_slist_add_item(__host_type, __member, __plist, __pitem)           \
    do {                                                                        \
        __vsf_slist_set_next(&((__pitem)->__member), ((__plist)->pnext));       \
        __vsf_slist_set_next((__plist), (__pitem));                             \
    } while(0)



#define __vsf_slist_queue_init(__pqueue)                                        \
    do {                                                                        \
        __vsf_slist_set_next(&((__pqueue)->head), NULL);                        \
        __vsf_slist_set_next(&((__pqueue)->tail), NULL);                        \
    } while (0)

#define __vsf_slist_queue_enqueue(__host_type, __member, __pqueue, __pitem)     \
    do {                                                                        \
        __host_type * __pitem_tail = (__host_type *)((__pqueue)->tail.pnext);   \
        __vsf_slist_set_next(&((__pqueue)->tail), (__pitem));                   \
        if (__pitem_tail != NULL)                                               \
            __vsf_slist_set_next(&__pitem_tail->__member, (__pitem));           \
        else                                                                    \
            __vsf_slist_set_next(&((__pqueue)->head), (__pitem));               \
    } while (0)

#define __vsf_slist_queue_dequeue(__host_type, __member, __pqueue, __pitem_ref) \
    do {                                                                        \
        __vsf_slist_ref_next(&((__pqueue)->head), (__pitem_ref));               \
        if (NULL != (__pitem_ref)) {                                            \
            __vsf_slist_set_next(                                               \
                &((__pqueue)->head),                                            \
                (__pitem_ref)->__member.pnext);                                 \
            if (__vsf_slist_is_empty(&((__pqueue)->head)))                      \
                __vsf_slist_set_next(&((__pqueue)->tail), NULL);                \
            __vsf_slist_set_next(&((__pitem_ref)->__member), NULL);             \
        }                                                                       \
    } while (0)

/*-----------------------------------------------------------------------------*
 * Double Chain List intenral macro                                            *
 *-----------------------------------------------------------------------------*/

#define __vsf_dlist_init(__plist)                                               \
    do {                                                                        \
        (__plist)->phead = (__plist)->ptail = NULL;                             \
    } while (0)

#define __vsf_dlist_init_node(__pnode)                                          \
            __vsf_dlist_init(__pnode)

#define __vsf_dlist_get_host(__host_type, __member, __pnode)                    \
            container_of((__pnode), __host_type, __member)

#define __vsf_dlist_get_host_safe(__host_type, __member, __pnode)               \
            safe_container_of((__pnode), __host_type, __member)

#define __vsf_dlist_is_empty(__plist)       ((__plist)->pnext == NULL)

#define __vsf_dlist_is_in(__plist, __pnode)                                     \
            __vsf_dlist_is_in_internal((__plist), (__pnode))

#define __vsf_dlist_ref(__host_type, __member, __pnode, __pitem_ref)            \
    do {                                                                        \
        (*(__host_type **)&(__pitem_ref)) =                                     \
                __vsf_dlist_get_host(__host_type, __member, __pnode);           \
    } while (0)

#define __vsf_dlist_ref_safe(__host_type, __member, __pnode, __pitem_ref)       \
    do {                                                                        \
        (*(__host_type **)&(__pitem_ref)) =                                     \
                __vsf_dlist_get_host_safe(__host_type, __member, __pnode);      \
    } while (0)

#define __vsf_dlist_add_to_head(__host_type, __member, __plist, __pitem)        \
            __vsf_dlist_add_to_head_internal((__plist), &((__pitem)->__member))

#define __vsf_dlist_add_to_tail(__host_type, __member, __plist, __pitem)        \
            __vsf_dlist_add_to_tail_internal((__plist), &((__pitem)->__member))

#define __vsf_dlist_peek_next(__host_type, __member, __pitem, __pitem_ref)      \
    do {                                                                        \
        vsf_dlist_t *pnode = (__pitem)->__member.pnext;                         \
        __vsf_dlist_ref_safe(__host_type, __member, (pnode), (__pitem_ref));    \
    } while (0)

#define __vsf_dlist_peek_head(__host_type, __member, __plist, __pitem_ref)      \
    do {                                                                        \
        vsf_dlist_t *pnode = (__plist)->pnext;                                  \
        __vsf_dlist_ref_safe(__host_type, __member, (pnode), (__pitem_ref));    \
    } while (0)

#define __vsf_dlist_remove_head(__host_type, __member, __plist, __pitem_ref)    \
    do {                                                                        \
        vsf_dlist_t *pnode = __vsf_dlist_remove_head_internal(__plist);         \
        __vsf_dlist_ref_safe(__host_type, __member, (pnode), (__pitem_ref));    \
    } while (0)

#define __vsf_dlist_remove_tail(__host_type, __member, __plist, __pitem_ref)    \
    do {                                                                        \
        vsf_dlist_t *pnode = __vsf_dlist_remove_tail_internal(__plist);         \
        __vsf_dlist_ref_safe(__host_type, __member, (pnode), (__pitem_ref));    \
    } while (0)

#define __vsf_dlist_insert_after(                                               \
                __host_type, __member, __plist, __pitem_prv, __pitem)           \
            __vsf_dlist_insert_after_internal(                                  \
                (__plist), &((__pitem_prv)->__member), &((__pitem)->__member))

#define __vsf_dlist_insert_before(                                              \
                __host_type, __member, __plist, __pitem_nxt, __pitem)           \
            __vsf_dlist_insert_before_internal(                                 \
                (__plist), &((__pitem_nxt)->__member), &((__pitem)->__member))

#define __vsf_dlist_remove(__host_type, __member, __plist, __pitem)             \
            __vsf_dlist_remove_internal((__plist), &((__pitem)->__member))

#define __vsf_dlist_remove_after(                                               \
                __host_type, __member, __plist, __pnode, __pitem_ref)           \
    do {                                                                        \
        vsf_dlist_t *__pnode_nxt = (__pnode)->pnext;                            \
        if (__pnode_nxt != NULL) {                                              \
            __vsf_dlist_remove(__host_type, __member, __plist, __pnode_nxt);    \
            __vsf_dlist_ref(__host_type, __member, (__pnode_nxt), (__pitem_ref));\
        } else {                                                                \
            (*(__host_type **)&(__pitem_ref)) = NULL;                           \
        }                                                                       \
    } while (0)

#define __vsf_dlist_remove_before(                                              \
                __host_type, __member, __plist, __pnode, __pitem_ref)           \
    do {                                                                        \
        vsf_dlist_t *__pnode_prv = (__pnode)->pprev;                            \
        if (__pitem_prv != NULL) {                                              \
            __vsf_dlist_remove(__host_type, __member, __plist, __pitem_prv);    \
            __vsf_dlist_ref(__host_type, __member, (__pnode_prv), (__pitem_ref));\
        } else {                                                                \
            (*(__host_type **)&(__pitem_ref)) = NULL;                           \
        }                                                                       \
    } while (0)

#define __vsf_dlist_insert(     __host_type,  /* type of the host object */     \
                                __member,     /* the name of the list */        \
                                __plist,      /* the address of the list */     \
                                __pitem,      /* the address of the new item */ \
                                ...)          /* how to find insert point */    \
    do {                                                                        \
        vsf_dlist_init_node(__host_type, __member, __pitem);                    \
        vsf_dlist_t *plist_item = (__plist);                                    \
        for (; plist_item->pnext != NULL;){                                     \
            __host_type *ptarget =                                              \
                __vsf_dlist_get_host(__host_type, __member, plist_item->pnext); \
            /* using __VA_ARGS__ so ',' operation could be supported */         \
            if (__VA_ARGS__) {                                                  \
                __vsf_dlist_insert_before_internal(                             \
                    (__plist), plist_item->pnext, &((__pitem)->__member));      \
                break;                                                          \
            }                                                                   \
            plist_item = plist_item->pnext;                                     \
        }                                                                       \
        if (NULL == plist_item->pnext) {                                        \
            __vsf_dlist_add_to_tail(                                            \
                    __host_type, __member, __plist, (__pitem));                 \
        }                                                                       \
    } while (0)


/*-----------------------------------------------------------------------------*
 * Single Chain List                                                           *
 *-----------------------------------------------------------------------------*/

//! \name list normal access
//! @{

#define vsf_slist_init(__plist)             /* the address of the list */       \
            __vsf_slist_set_next((__plist), NULL)

#define vsf_slist_is_empty(__plist)         /* the address of the list */       \
            __vsf_slist_is_empty(__plist)

#define vsf_slist_is_in(    __host_type,/* the type of the host type */         \
                            __member,   /* the member name of the list */       \
                            __plist,    /* the address of the list */           \
                            __pitem)    /* the address of the target item */    \
        (   vsf_slist_get_idx(                                                  \
                (__plist), (__pitem), offset_of(__host_type, __member))         \
        >   0)

#define vsf_slist_init_node(__host_type,    /* the type of the host type */     \
                            __member,       /* the member name of the list */   \
                            __pitem)        /* the address of the target item */\
            __vsf_slist_set_next(&((__pitem)->__member), NULL)

#define vsf_slist_add_item( __host_type,    /* the type of the host type */     \
                            __member,       /* the member name of the list */   \
                            __plist,        /* the address of the list */       \
                            __pitem)        /* the address of the target item */\
            __vsf_slist_add_item(__host_type, __member, (__plist), (__pitem))

#define vsf_slist_remove_current_item(                                          \
                                __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list*/    \
                                __plist,    /* the address of the list */       \
                                __pitem_ref)/* the pointer of host type */      \
                vsf_slist_remove_head(                                          \
                    __host_type, __member, __plist, __pitem_ref)

#define vsf_slist_insert(       __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list */   \
                                __plist,    /* the address of the list */       \
                                __pitem,    /* the address of the target item */\
                                ...)        /* when( condition expression ) */  \
            __vsf_slist_insert(                                                 \
                __host_type, __member, (__plist), (__pitem), __VA_ARGS__)

#define vsf_slist_remove_after( __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list*/    \
                                __plist,    /* the address of the list */       \
                                __pitem_ref)/* the pointer of host type */      \
            __vsf_slist_remove_after(                                           \
                __host_type, __member, (__pnode), (__pitem_ref))

#define vsf_slist_insert_after( __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list*/    \
                                __pnode,    /* the reference node */            \
                                __pitem)    /* the address of the target item */\
            __vsf_slist_insert_after((__pnode), (__pitem), __member)

//! @}


//! \name stack / stack-like operations using list
//£¡ @{
#define vsf_slist_stack_init(__plist)            vsf_slist_init_node(__plist)

#define vsf_slist_stack_pop(__host_type,    /* the type of the host type */     \
                            __member,       /* the member name of the list */   \
                            __plist,        /* the address of the list */       \
                            __pitem_ref)    /* the pointer of host type */      \
            vsf_slist_remove_from_head(                                         \
                __host_type, __member, (__plist), (__pitem_ref))


#define vsf_slist_stack_push(   __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list */   \
                                __plist,    /* the address of the list */       \
                                __pitem)    /* the address of the target item */\
            vsf_slist_add_to_head(                                              \
                __host_type, __member, (__plist), (__pitem))


#define vsf_slist_add_to_head(  __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list */   \
                                __plist,    /* the address of the list */       \
                                __pitem)    /* the address of the target item */\
            __vsf_slist_add_item(__host_type, __member, (__plist), (__pitem))

#define vsf_slist_discard_head( __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list */   \
                                __plist)    /* the address of the list */       \
            __vsf_slist_discard_head(__host_type, __member, (__plist))

#define vsf_slist_remove_from_head( __host_type,/* the type of the host type */ \
                                    __member,   /* the member name of the list*/\
                                    __plist,    /* the address of the list */   \
                                    __pitem_ref)/* the pointer of host type */  \
    do {                                                                        \
        vsf_slist_peek_next(__host_type, __member, (__plist), (__pitem_ref));   \
        vsf_slist_discard_head(__host_type, __member, (__plist));               \
    } while(0)

#define vsf_slist_peek_next(__host_type,    /* the type of the host type */     \
                            __member,       /* the member name of the list */   \
                            __plist,        /* the address of the list */       \
                            __pitem_ref)    /* the pointer of host type */      \
            __vsf_slist_ref_next((__plist), (__pitem_ref))
//! @}

//! \name list enumeration access
//! @{
#define vsf_slist_foreach(  __host_type,/* the type of the host type */         \
                            __member,   /* the member name of the list */       \
                            __plist,    /* the address of the list */           \
                            __pitem)    /* the pointer name, e.g. ptarget */    \
            __vsf_slist_foreach(__host_type, __member, (__plist), __pitem )
//! @}


//! \name queue / queue-like operations using slist
//£¡ @{
#define vsf_slist_queue_is_empty(__pqueue)                                      \
            vsf_slist_is_empty(&(__pqueue)->head)

#define vsf_slist_queue_init(__pqueue)                                          \
            __vsf_slist_queue_init((__pqueue))

#define vsf_slist_queue_enqueue(__host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list*/    \
                                __pqueue,   /* the address of the queue */      \
                                __pitem)    /* the address of the target item */\
            __vsf_slist_queue_enqueue(                                          \
                    __host_type, __member, (__pqueue), (__pitem))

#define vsf_slist_queue_dequeue(    __host_type,/* the type of the host type */ \
                                    __member,   /* the member name of the list*/\
                                    __pqueue,   /* the address of the queue */  \
                                    __pitem_ref)/* the pointer of host type */  \
            __vsf_slist_queue_dequeue(                                          \
                    __host_type, __member, (__pqueue), (__pitem_ref))
//! @}

/*-----------------------------------------------------------------------------*
 * Double Chain List                                                           *
 *-----------------------------------------------------------------------------*/

/* memory layout for dlist
      dlist:------------------
|-----------|head        tail|--------------|
|           ------------------              |
|  head:---------- tail:----------          |
------->|forward |----->|forward |--->NULL  |
NULL<---|backward|<-----|backward|<----------
        ----------      ----------
*/

//! \name dual-way linked list normal access
//! @{

#define vsf_dlist_init(__plist)             /* the address of the list */       \
            __vsf_dlist_init(__plist)

#define vsf_dlist_is_empty(__plist)         /* the address of the list */       \
            __vsf_dlist_is_empty(__plist)

#define vsf_dlist_is_in(        __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list */   \
                                __plist,    /* the address of the list */       \
                                __pitem)    /* the address of the target item */\
            __vsf_dlist_is_in((__plist), &(__pitem)->__member)

#define vsf_dlist_init_node(    __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list */   \
                                __pitem)    /* the address of the target item */\
            __vsf_dlist_init_node(&((__pitem)->__member))

#define vsf_dlist_add_to_head(  __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list */   \
                                __plist,    /* the address of the list */       \
                                __pitem)    /* the address of the target item */\
            __vsf_dlist_add_to_head(__host_type, __member, (__plist), (__pitem))

#define vsf_dlist_add_to_tail(  __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list */   \
                                __plist,    /* the address of the list */       \
                                __pitem)    /* the address of the target item */\
            __vsf_dlist_add_to_tail(__host_type, __member, (__plist), (__pitem))

#define vsf_dlist_peek_head(    __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list */   \
                                __plist,    /* the address of the list */       \
                                __pitem_ref)/* the pointer of host type */      \
            __vsf_dlist_peek_head(                                              \
                    __host_type, __member, (__plist), (__pitem_ref))

#define vsf_dlist_peek_next(  __host_type,/* the type of the host type */       \
                                __member,   /* the member name of the list */   \
                                __pitem,    /* the address of the target item */\
                                __pitem_ref)/* the pointer of host type */      \
            __vsf_dlist_peek_next(                                              \
                    __host_type, __member, (__pitem), (__pitem_ref))

#define vsf_dlist_remove_head(  __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list */   \
                                __plist,    /* the address of the list */       \
                                __pitem_ref)/* the pointer of host type */      \
            __vsf_dlist_remove_head(                                            \
                    __host_type, __member, (__plist), (__pitem_ref))

#define vsf_dlist_remove_tail(  __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list */   \
                                __plist,    /* the address of the list */       \
                                __pitem_ref)/* the pointer of host type */      \
            __vsf_dlist_remove_tail(                                            \
                    __host_type, __member, (__plist), (__pitem_ref))

#define vsf_dlist_insert(       __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list */   \
                                __plist,    /* the address of the list */       \
                                __pitem,    /* the address of the target item */\
                                ...)        /* when( condition expression ) */  \
            __vsf_dlist_insert(                                                 \
                __host_type, __member, (__plist), (__pitem), __VA_ARGS__)

#define vsf_dlist_insert_after( __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list*/    \
                                __plist,    /* the address of the list */       \
                                __pitem_prv,/* the address of the prev item */  \
                                __pitem)    /* the address of the target item */\
            __vsf_dlist_insert_after(                                           \
                    __host_type, __member, (__plist), (__pitem_prv), (__pitem))

#define vsf_dlist_insert_before(__host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list*/    \
                                __plist,    /* the address of the list */       \
                                __pitem_nxt,/* the address of the next item */  \
                                __pitem)    /* the address of the target item */\
            __vsf_dlist_insert_before(                                          \
                    __host_type, __member, (__plist), (__pitem_nxt), (__pitem))

#define vsf_dlist_remove(       __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list*/    \
                                __plist,    /* the address of the list */       \
                                __pitem)    /* the address of the target item */\
            __vsf_dlist_remove(__host_type, __member, (__plist), (__pitem))

#define vsf_dlist_remove_after( __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list*/    \
                                __plist,    /* the address of the list */       \
                                __pnode,    /* the reference node */            \
                                __pitem_ref)/* the pointer of host type */      \
            __vsf_dlist_remove_after(                                           \
                    __host_type, __member, (__plist), (__pnode), (__pitem_ref))

#define vsf_dlist_remove_before(__host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list*/    \
                                __plist,    /* the address of the list */       \
                                __pnode,    /* the reference node */            \
                                __pitem_ref)/* the pointer of host type */      \
            __vsf_dlist_remove_before(                                          \
                    __host_type, __member, (__plist), (__pnode), (__pitem_ref))

//! @}

//! \name queue operation using dual-way linked list
//! @{

#define vsf_dlist_queue_peek(   __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list */   \
                                __plist,    /* the address of the list */       \
                                __pitem_ref)/* the pointer of host type */      \
            __vsf_dlist_ref_safe(                                               \
                    __host_type, __member, (__plist)->pnext, (__pitem_ref))

#define vsf_dlist_queue_enqueue(__host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list*/    \
                                __plist,    /* the address of the list */       \
                                __pitem)    /* the address of the target item */\
            vsf_dlist_add_to_tail(                                              \
                    __host_type, __member, (__plist), (__pitem))

#define vsf_dlist_queue_dequeue(__host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list*/    \
                                __plist,    /* the address of the list */       \
                                __pitem_ref)/* the address of the target item */\
            vsf_dlist_remove_head(                                             \
                    __host_type, __member, (__plist), (__pitem_ref))

//! @}

/*============================ TYPES =========================================*/

//! \name single list item
//! @{
struct vsf_slist_t {
    void *pnext;                        /*!< pointer for the next */
};
typedef struct vsf_slist_t vsf_slist_t;
//! @}

//! \name dual-way linked list item
//! @{
struct vsf_dlist_t {
    union {
        struct vsf_dlist_t *pnext;      /*!< slist node in forward list */
        struct vsf_dlist_t *phead;      /*!< pointer for the head */
    };
    union {
        struct vsf_dlist_t *pprev;      /*!< slist node in backward list */
        struct vsf_dlist_t *ptail;      /*!< pointer for the tail */
    };
};
typedef struct vsf_dlist_t vsf_dlist_t;
typedef struct vsf_dlist_t vsf_dlist_node_t;
//! @}

//! \name single list queue header
//! @{
struct vsf_slist_queue_t {
    vsf_slist_t head;                   /*!< pointer for the queue head */
    vsf_slist_t tail;                   /*!< pointer for the queue tail */
};
typedef struct vsf_slist_queue_t vsf_slist_queue_t;
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

/*! \brief calculate the length of the target list 
 *! \param pthis        address of the target list
 *! \param list_offset  the list offset in byte within a host type
 *! \return the length of the list, -1 means illegal input
 */
extern int_fast32_t vsf_slist_get_length(vsf_slist_t *pthis, size_t list_offset);

/*! \brief get the specified node with a given index from the target list
 *! \param pthis        address of the target list
 *! \param index        index number of the wanted node
 *! \param list_offset  the list offset in byte within a host type
 *! \retval NULL        Illegal input 
 *! \retval !NULL       the address of the wanted node
 */
extern void * vsf_slist_get_node(   vsf_slist_t *pthis, 
                                    int_fast32_t index, 
                                    size_t list_offset);

/*! \brief find the index number of a given node from the target list 
 *! \param pthis        address of the target list
 *! \param pnode        address of the target node
 *! \param list_offset  the list offset in byte within a host type
 *! \return the index number, (-1) means illegal input or the node doesn't exist
 */
extern int_fast32_t vsf_slist_get_index(    vsf_slist_t *pthis, 
                                            void *pnode,
                                            size_t list_offset);

/*! \brief remove a node from the target list
 *! \param pthis        address of the target list
 *! \param pnode        address of the target node
 *! \param list_offset  the list offset in byte within a host type
 *! \retval NULL        Illegal input 
 *! \retval !NULL       the address of the removed node
 */
extern void * vsf_slist_remove( vsf_slist_t *pthis, 
                                void *pnode, 
                                size_t list_offset);

/*! \brief remove a node from the target list
 *! \param pthis        address of the target list
 *! \param pnode        address of the target node
 *! \param list_offset  the list offset in byte within a host type
 *! \retval NULL        Illegal input 
 *! \retval !NULL       the address of the appended node
 */
extern void * vsf_slist_append(   vsf_slist_t *pthis, 
                                void *pnode, 
                                size_t list_offset);

extern bool __vsf_dlist_is_in_internal(         vsf_dlist_t *plist,
                                                vsf_dlist_node_t *pnode);

extern void __vsf_dlist_add_to_head_internal(   vsf_dlist_t *plist, 
                                                vsf_dlist_node_t *pnode);
                                                
extern void __vsf_dlist_add_to_tail_internal(   vsf_dlist_t *plist, 
                                                vsf_dlist_node_t *pnode);
                                                
extern vsf_dlist_node_t *__vsf_dlist_remove_head_internal(vsf_dlist_t *plist);

extern vsf_dlist_node_t *__vsf_dlist_remove_tail_internal(vsf_dlist_t *plist);

extern void __vsf_dlist_insert_after_internal(  vsf_dlist_t *plist, 
                                                vsf_dlist_node_t *pnode_prv, 
                                                vsf_dlist_node_t *pnode);
                                                
extern void __vsf_dlist_insert_before_internal( vsf_dlist_t *plist, 
                                                vsf_dlist_node_t *pnode_nxt, 
                                                vsf_dlist_node_t *pnode);
                                                
extern void __vsf_dlist_remove_internal(        vsf_dlist_t *plist, 
                                                vsf_dlist_node_t *pnode);

#endif
