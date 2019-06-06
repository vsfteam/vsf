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

#define __vsf_slist_is_empty(__plist)                                           \
            (NULL == ((vsf_slist_t *)(__plist))->head)

#define __vsf_slist_set_next(__pnode, __pitem_nxt)                              \
    do {                                                                        \
        ((vsf_slist_node_t *)(__pnode))->next = (__pitem_nxt);                  \
    } while (0)

#define __vsf_slist_ref_next(__pnode, __pitem_ref)                              \
    do {                                                                        \
        (*(void **)&(__pitem_ref)) = (((vsf_slist_node_t *)(__pnode))->next);   \
    } while (0)

#define __vsf_slist_insert_next(__host_type, __member, __pnode, __pitem)        \
    do {                                                                        \
        __vsf_slist_set_next(&((__pitem)->__member), ((__pnode)->next));        \
        __vsf_slist_set_next((__pnode), (__pitem));                             \
    } while (0)

#define __vsf_slist_remove_next_unsafe(__host_type, __member, __pnode, __pitem_ref)\
    do {                                                                        \
        __vsf_slist_ref_next((__pnode), (__pitem_ref));                         \
        __vsf_slist_set_next((__pnode),                                         \
                ((__host_type *)(__pitem_ref))->__member.next);                 \
    } while (0)

#define __vsf_slist_insert_after(   __host_type,/* type of the host object */   \
                                    __member,   /* the name of the list */      \
                                    __pitem,    /* current item address */      \
                                    __pitem_new)/* new item address */          \
    do {                                                                        \
        vsf_slist_node_t *node = &((__pitem)->__member);                        \
        __vsf_slist_insert_next(__host_type, __member, node, (__pitem_new));    \
    } while (0)

#define __vsf_slist_remove_after(   __host_type, /* type of the host object */  \
                                    __member,    /* the name of the list */     \
                                    __pitem,     /* current item address */     \
                                    __pitem_ref) /* pointer of host type*/      \
    do {                                                                        \
        vsf_slist_node_t *node = &((__pitem)->__member);                        \
        __vsf_slist_ref_next(node, (__pitem_ref));                              \
        if (NULL != node->next) {                                               \
            __vsf_slist_set_next(                                               \
                node, ((__host_type *)(node->next))->__member.next);            \
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
        vsf_slist_node_t *__ = (vsf_slist_node_t *)(__plist);                   \
        for (; __->next != NULL;){                                              \
            const __host_type * const _ = (__host_type *)(__->next);            \
            /* using __VA_ARGS__ so ',' operation could be supported */         \
            if (__VA_ARGS__) {                                                  \
                __vsf_slist_insert_next(                                        \
                    __host_type, __member, __, (__pitem));                      \
                break;                                                          \
            }                                                                   \
            __ = (vsf_slist_node_t *)&(_->__member);                            \
        }                                                                       \
        if (NULL == __->next) {                                                 \
            __vsf_slist_insert_next(                                            \
                    __host_type, __member, __, (__pitem));                      \
            break;                                                              \
        }                                                                       \
    } while (0)

#define __vsf_slist_foreach_unsafe(                                             \
                            __host_type,/* the type of the host type */         \
                            __member,   /* the member name of the list */       \
                            __plist)    /* the address of the list */           \
    for (__host_type *_ = (__plist)->head; _ != NULL; _ = _->__member.next)

#define __vsf_slist_foreach_next_unsafe(                                        \
                            __host_type,/* the type of the host type */         \
                            __member,   /* the member name of the list */       \
                            __plist)    /* the address of the list */           \
    for (__host_type *_ = (__plist)->head, *__ = _ ? _->__member.next : NULL;   \
            _ != NULL;                                                          \
            _ = __, __ = _ ? _->__member.next : NULL)
            

#define __vsf_slist_foreach(__host_type,/* the type of the host type */         \
                            __member,   /* the member name of the list */       \
                            __plist,    /* the address of the list */           \
                            __pitem)    /* the pointer name, e.g. ptarget */    \
    for (__host_type                    /* loop initialisation */               \
            *__pitem = (__host_type *)(__plist)->head,                          \
            *_ = __pitem,                                                       \
            *next = __pitem                                                     \
                ?   (__host_type *)                                             \
                        ((__host_type *)((__plist)->head))->__member.next       \
                :   NULL;                                                       \
            __pitem != NULL;            /* loop condition */                    \
            _ = __pitem = next, next = __pitem                                  \
                ?   (__host_type *)                                             \
                       ((__host_type *)((__plist)->head))->__member.next        \
                :   NULL)               /* prepare for next iteration */

#define __vsf_slist_discard_head(__host_type, __member, __plist)                \
            if (NULL != (__plist)->head) {                                      \
                __host_type *item = (__host_type *)((__plist)->head);           \
                __vsf_slist_set_next(                                           \
                        (__plist),                                              \
                        ((__host_type *)((__plist)->head))->__member.next);     \
                item->__member.next = NULL;                                     \
            }


#define __vsf_slist_queue_init(__pqueue)                                        \
    do {                                                                        \
        __vsf_slist_set_next(&((__pqueue)->head), NULL);                        \
        __vsf_slist_set_next(&((__pqueue)->tail), NULL);                        \
    } while (0)

#define __vsf_slist_queue_enqueue(__host_type, __member, __pqueue, __pitem)     \
    do {                                                                        \
        __host_type * item_tail = (__host_type *)((__pqueue)->tail.next);       \
        __vsf_slist_set_next(&((__pqueue)->tail), (__pitem));                   \
        if (item_tail != NULL)                                                  \
            __vsf_slist_set_next(&item_tail->__member, (__pitem));              \
        else                                                                    \
            __vsf_slist_set_next(&((__pqueue)->head), (__pitem));               \
    } while (0)

#define __vsf_slist_queue_dequeue(__host_type, __member, __pqueue, __pitem_ref) \
    do {                                                                        \
        __vsf_slist_ref_next(&((__pqueue)->head), (__pitem_ref));               \
        if (NULL != (__pitem_ref)) {                                            \
            __vsf_slist_set_next(                                               \
                &((__pqueue)->head),                                            \
                (__pitem_ref)->__member.next);                                  \
            if (__vsf_slist_is_empty(&((__pqueue)->head)))                      \
                __vsf_slist_set_next(&((__pqueue)->tail), NULL);                \
            __vsf_slist_set_next(&((__pitem_ref)->__member), NULL);             \
        }                                                                       \
    } while (0)

#define __vsf_slist_queue_peek(__host_type, __member, __pqueue, __pitem_ref)    \
        __vsf_slist_ref_next(&((__pqueue)->head), (__pitem_ref));               


#define __vsf_slist_queue_add_to_head(__host_type, __member, __pqueue, __pitem) \
    do {                                                                        \
        __host_type * item_head = (__host_type *)((__pqueue)->head.next);       \
        __vsf_slist_set_next(&(__pitem)->__member, item_head);                  \
        __vsf_slist_set_next(&((__pqueue)->head), (__pitem));                   \
        if (NULL == item_head)                                                  \
            __vsf_slist_set_next(&((__pqueue)->tail), (__pitem));               \
    } while (0)

/*-----------------------------------------------------------------------------*
 * Double Chain List intenral macro                                            *
 *-----------------------------------------------------------------------------*/

#define __vsf_dlist_init(__plist)                                               \
    do {                                                                        \
        (__plist)->head = (__plist)->tail = NULL;                               \
    } while (0)

#define __vsf_dlist_init_node(__pnode)                                          \
    do {                                                                        \
        (__pnode)->next = (__pnode)->prev = NULL;                               \
    } while (0)

#define __vsf_dlist_get_host(__host_type, __member, __pnode)                    \
            container_of((__pnode), __host_type, __member)

#define __vsf_dlist_get_host_safe(__host_type, __member, __pnode)               \
            safe_container_of((__pnode), __host_type, __member)

#define __vsf_dlist_is_empty(__plist)       ((__plist)->head == NULL)

#define __vsf_dlist_is_in(__plist, __pnode)                                     \
            __vsf_dlist_is_in_imp((__plist), (__pnode))

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
            __vsf_dlist_add_to_head_imp((__plist), &((__pitem)->__member))

#define __vsf_dlist_add_to_tail(__host_type, __member, __plist, __pitem)        \
            __vsf_dlist_add_to_tail_imp((__plist), &((__pitem)->__member))

#define __vsf_dlist_peek_next(__host_type, __member, __pitem, __pitem_ref)      \
    do {                                                                        \
        vsf_dlist_node_t *node = (__pitem)->__member.next;                      \
        __vsf_dlist_ref_safe(__host_type, __member, node, (__pitem_ref));       \
    } while (0)

#define __vsf_dlist_peek_head(__host_type, __member, __plist, __pitem_ref)      \
    do {                                                                        \
        vsf_dlist_node_t *node = (__plist)->head;                               \
        __vsf_dlist_ref_safe(__host_type, __member, node, (__pitem_ref));       \
    } while (0)

#define __vsf_dlist_remove_head(__host_type, __member, __plist, __pitem_ref)    \
    do {                                                                        \
        vsf_dlist_node_t *node = __vsf_dlist_remove_head_imp(__plist);          \
        __vsf_dlist_ref_safe(__host_type, __member, node, (__pitem_ref));       \
    } while (0)

#define __vsf_dlist_remove_tail(__host_type, __member, __plist, __pitem_ref)    \
    do {                                                                        \
        vsf_dlist_t *node = __vsf_dlist_remove_tail_imp(__plist);               \
        __vsf_dlist_ref_safe(__host_type, __member, node, (__pitem_ref));       \
    } while (0)

#define __vsf_dlist_insert_after(                                               \
                __host_type, __member, __plist, __pitem_prv, __pitem)           \
            __vsf_dlist_insert_after_imp(                                       \
                (__plist), &((__pitem_prv)->__member), &((__pitem)->__member))

#define __vsf_dlist_insert_before(                                              \
                __host_type, __member, __plist, __pitem_nxt, __pitem)           \
            __vsf_dlist_insert_before_imp(                                      \
                (__plist), &((__pitem_nxt)->__member), &((__pitem)->__member))

#define __vsf_dlist_remove(__host_type, __member, __plist, __pitem)             \
            __vsf_dlist_remove_imp((__plist), &((__pitem)->__member))

#define __vsf_dlist_remove_after(                                               \
                __host_type, __member, __plist, __pnode, __pitem_ref)           \
    do {                                                                        \
        vsf_dlist_node_t *node_nxt = (__pnode)->next;                           \
        if (node_nxt != NULL) {                                                 \
            __vsf_dlist_remove(__host_type, __member, __plist, node_nxt);       \
            __vsf_dlist_ref(__host_type, __member, node_nxt, (__pitem_ref));    \
        } else {                                                                \
            (*(__host_type **)&(__pitem_ref)) = NULL;                           \
        }                                                                       \
    } while (0)

#define __vsf_dlist_remove_before(                                              \
                __host_type, __member, __plist, __pnode, __pitem_ref)           \
    do {                                                                        \
        vsf_dlist_node_t *node_prv = (__pnode)->prev;                           \
        if (node_prv != NULL) {                                                 \
            __vsf_dlist_remove(__host_type, __member, __plist, node_prv);       \
            __vsf_dlist_ref(__host_type, __member, (node_prv), (__pitem_ref));  \
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
        vsf_dlist_node_t *__ = (vsf_dlist_node_t *)(__plist);                   \
        for (; __->next != NULL;){                                              \
            const __host_type * const _ =                                       \
                __vsf_dlist_get_host(__host_type, __member, __->next);          \
            /* using __VA_ARGS__ so ',' operation could be supported */         \
            if (__VA_ARGS__) {                                                  \
                __vsf_dlist_insert_before_imp(                                  \
                    (__plist), __->next, &((__pitem)->__member));               \
                break;                                                          \
            }                                                                   \
            __ = __->next;                                                      \
        }                                                                       \
        if (NULL == __->next) {                                                 \
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

#define vsf_slist_set_next( __host_type,/* the type of the host type */         \
                            __member,   /* the member name of the list */       \
                            __pnode,    /* the reference node */                \
                            __pitem)    /* the address of the target item */    \
            __vsf_slist_set_next((__pnode), (__pitem))

#define vsf_slist_get_length(   __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list */   \
                                __plist)    /* the address of the list */       \
            __vsf_slist_get_length_imp(                                         \
                (__plist), offset_of(__host_type, __member))

#define vsf_slist_remove_tail(  __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list */   \
                                __plist)    /* the address of the list */       \
            __vsf_slist_remove_tail_imp(                                        \
                (__plist), offset_of(__host_type, __member))

#define vsf_slist_get_item_by_index(                                            \
                                __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list */   \
                                __plist,    /* the address of the list */       \
                                __index)    /* index of the item in list */     \
            __vsf_slist_get_item_by_index_imp(                                  \
                (__plist), (__index), offset_of(__host_type, __member))

#define vsf_slist_get_index(    __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list */   \
                                __plist,    /* the address of the list */       \
                                __pitem)    /* the address of the target item */\
            __vsf_slist_get_index_imp(                                          \
                (__plist), (__pitem), offset_of(__host_type, __member))

#define vsf_slist_append(   __host_type,/* the type of the host type */         \
                            __member,   /* the member name of the list */       \
                            __plist,    /* the address of the list */           \
                            __pitem)    /* the address of the target item */    \
            __vsf_slist_append_imp(                                             \
                (__plist), (__pitem), offset_of(__host_type, __member))

#define vsf_slist_get_index(__host_type,/* the type of the host type */         \
                            __member,   /* the member name of the list */       \
                            __plist,    /* the address of the list */           \
                            __pitem)    /* the address of the target item */    \
            __vsf_slist_get_index_imp(                                          \
                (__plist), (__pitem), offset_of(__host_type, __member))

#define vsf_slist_is_in(    __host_type,/* the type of the host type */         \
                            __member,   /* the member name of the list */       \
                            __plist,    /* the address of the list */           \
                            __pitem)    /* the address of the target item */    \
        (   vsf_slist_get_index(                                                \
                (__plist), (__pitem), offset_of(__host_type, __member))         \
        >   0)

#define vsf_slist_init_node(__host_type,/* the type of the host type */         \
                            __member,   /* the member name of the list */       \
                            __pitem)    /* the address of the target item */    \
            __vsf_slist_set_next(&((__pitem)->__member), NULL)

#define vsf_slist_remove(   __host_type,/* the type of the host type */         \
                            __member,   /* the member name of the list*/        \
                            __plist,    /* the address of the list */           \
                            __pitem)    /* the address of the target item */    \
            __vsf_slist_remove_imp(                                             \
                (__plist), (__pitem), offset_of(__host_type, __member))

#define vsf_slist_insert(   __host_type,/* the type of the host type */         \
                            __member,   /* the member name of the list */       \
                            __plist,    /* the address of the list */           \
                            __pitem,    /* the address of the target item */    \
                            ...)        /* when( condition expression ) */      \
            __vsf_slist_insert(                                                 \
                __host_type, __member, (__plist), (__pitem), __VA_ARGS__)

#define vsf_slist_remove_after( __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list*/    \
                                __pitem,    /* the address of current item */   \
                                __pitem_ref)/* the pointer of host type */      \
            __vsf_slist_remove_after(                                           \
                __host_type, __member, (__pitem), (__pitem_ref))

#define vsf_slist_insert_after( __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list*/    \
                                __pitem,    /* the address of current item */   \
                                __pitem_new)/* the address of new item */       \
            __vsf_slist_insert_after(                                           \
                __host_type, __member, (__pitem), (__pitem_new))

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
            __vsf_slist_insert_next(                                            \
                __host_type, __member, (vsf_slist_node_t *)(__plist), (__pitem))

#define vsf_slist_discard_head( __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list */   \
                                __plist)    /* the address of the list */       \
            __vsf_slist_discard_head(__host_type, __member, (__plist))

#define vsf_slist_remove_from_head_unsafe(                                      \
                                    __host_type,/* the type of the host type */ \
                                    __member,   /* the member name of the list*/\
                                    __plist,    /* the address of the list */   \
                                    __pitem_ref)/* the pointer of host type */  \
            __vsf_slist_remove_next_unsafe(                                     \
                __host_type, __member, (vsf_slist_node_t *)(__plist), (__pitem_ref))

#define vsf_slist_remove_from_head( __host_type,/* the type of the host type */ \
                                    __member,   /* the member name of the list*/\
                                    __plist,    /* the address of the list */   \
                                    __pitem_ref)/* the pointer of host type */  \
    do {                                                                        \
        vsf_slist_peek_next(__host_type, __member, (__plist), (__pitem_ref));   \
        vsf_slist_discard_head(__host_type, __member, (__plist));               \
    } while (0)

#define vsf_slist_peek_next(__host_type,    /* the type of the host type */     \
                            __member,       /* the member name of the list */   \
                            __pnode,        /* the reference node */            \
                            __pitem_ref)    /* the pointer of host type */      \
            __vsf_slist_ref_next((__pnode), (__pitem_ref))
//! @}

//! \name list enumeration access
//! @{
#define vsf_slist_foreach(  __host_type,/* the type of the host type */         \
                            __member,   /* the member name of the list */       \
                            __plist,    /* the address of the list */           \
                            __pitem)    /* the pointer name, e.g. ptarget */    \
            __vsf_slist_foreach(__host_type, __member, (__plist), __pitem)
//! @}


//! \name queue / queue-like operations using slist
//£¡ @{
#define vsf_slist_queue_is_empty(__pqueue)                                      \
            vsf_slist_is_empty(&(__pqueue)->head)

#define vsf_slist_queue_init(__pqueue)                                          \
            __vsf_slist_queue_init((__pqueue))

#define vsf_slist_queue_enqueue(    __host_type,/* the type of the host type */ \
                                    __member,   /* the member name of the list*/\
                                    __pqueue,   /* the address of the queue */  \
                                    __pitem)    /* the address of the target item */\
            __vsf_slist_queue_enqueue(                                          \
                    __host_type, __member, (__pqueue), (__pitem))

#define vsf_slist_queue_dequeue(    __host_type,/* the type of the host type */ \
                                    __member,   /* the member name of the list*/\
                                    __pqueue,   /* the address of the queue */  \
                                    __pitem_ref)/* the pointer of host type */  \
            __vsf_slist_queue_dequeue(                                          \
                    __host_type, __member, (__pqueue), (__pitem_ref))

#define vsf_slist_queue_peek(       __host_type,/* the type of the host type */ \
                                    __member,   /* the member name of the list*/\
                                    __pqueue,   /* the address of the queue */  \
                                    __pitem_ref)/* the pointer of host type */  \
            __vsf_slist_queue_peek(                                             \
                    __host_type, __member, (__pqueue), (__pitem_ref))

#define vsf_slist_queue_add_to_head(__host_type,/* the type of the host type */ \
                                    __member,   /* the member name of the list*/\
                                    __pqueue,   /* the address of the queue */  \
                                    __pitem)    /* the address of the target item */\
            __vsf_slist_queue_add_to_head(                                      \
                    __host_type, __member, (__pqueue), (__pitem))
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
                    __host_type, __member, (__plist)->head, (__pitem_ref))

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
    void *head;                         /*!< pointer for the next */
};
typedef struct vsf_slist_t vsf_slist_t;

struct vsf_slist_node_t {
    void *next;                         /*!< pointer for the next */
};
typedef struct vsf_slist_node_t vsf_slist_node_t;
//! @}

//! \name dual-way linked list item
//! @{
struct vsf_dlist_t {
    struct vsf_dlist_node_t *head;      /*!< pointer for the head */
    struct vsf_dlist_node_t *tail;      /*!< pointer for the tail */
};
typedef struct vsf_dlist_t vsf_dlist_t;

struct vsf_dlist_node_t {
    struct vsf_dlist_node_t *next;      /*!< slist node in forward list */
    struct vsf_dlist_node_t *prev;      /*!< slist node in backward list */
};
typedef struct vsf_dlist_node_t vsf_dlist_node_t;
//! @}

//! \name single list queue header
//! @{
struct vsf_slist_queue_t {
    vsf_slist_node_t head;              /*!< pointer for the queue head */
    vsf_slist_node_t tail;              /*!< pointer for the queue tail */
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
extern uint_fast16_t __vsf_slist_get_length_imp(
                                    vsf_slist_t *pthis,
                                    size_t list_offset);

/*! \brief get the specified item with a given index from the target list
 *! \param pthis        address of the target list
 *! \param index        index number of the wanted item
 *! \param list_offset  the list offset in byte within a host type
 *! \retval NULL        Illegal input 
 *! \retval !NULL       the address of the wanted item
 */
extern void * __vsf_slist_get_item_by_index_imp(
                                    vsf_slist_t *pthis, 
                                    uint_fast16_t index, 
                                    size_t list_offset);

/*! \brief find the index number of a given item from the target list 
 *! \param pthis        address of the target list
 *! \param item         address of the target item
 *! \param list_offset  the list offset in byte within a host type
 *! \return the index number, (-1) means illegal input or the item doesn't exist
 */
extern int_fast16_t __vsf_slist_get_index_imp(  vsf_slist_t *pthis,
                                                void *item,
                                                size_t list_offset);

/*! \brief remove a item from the target list
 *! \param pthis        address of the target list
 *! \param item         address of the target item
 *! \param list_offset  the list offset in byte within a host type
 *! \retval NULL        Illegal input 
 *! \retval !NULL       the address of the removed item
 */
extern void * __vsf_slist_remove_imp(   vsf_slist_t *pthis,
                                        void *item,
                                        size_t list_offset);

/*! \brief append a item from the target list
 *! \param pthis        address of the target list
 *! \param item         address of the target item
 *! \param list_offset  the list offset in byte within a host type
 *! \retval NULL        Illegal input 
 *! \retval !NULL       the address of the appended item
 */
extern void * __vsf_slist_append_imp(   vsf_slist_t *pthis,
                                        void *item,
                                        size_t list_offset);

extern void * __vsf_slist_remove_tail_imp(  vsf_slist_t *pthis,
                                            size_t list_offset);

extern bool __vsf_dlist_is_in_imp(vsf_dlist_t *pthis, vsf_dlist_node_t *node);

extern void __vsf_dlist_add_to_head_imp(vsf_dlist_t *pthis, vsf_dlist_node_t *node);
                                                
extern void __vsf_dlist_add_to_tail_imp(vsf_dlist_t *pthis, vsf_dlist_node_t *node);

extern vsf_dlist_node_t *__vsf_dlist_remove_head_imp(vsf_dlist_t *pthis);

extern vsf_dlist_node_t *__vsf_dlist_remove_tail_imp(vsf_dlist_t *pthis);

extern void __vsf_dlist_insert_after_imp(   vsf_dlist_t *pthis,
                                            vsf_dlist_node_t *node_prv,
                                            vsf_dlist_node_t *node);
                                                
extern void __vsf_dlist_insert_before_imp(  vsf_dlist_t *pthis,
                                            vsf_dlist_node_t *node_nxt,
                                            vsf_dlist_node_t *node);
                                                
extern void __vsf_dlist_remove_imp(vsf_dlist_t *pthis, vsf_dlist_node_t *node);

#endif
