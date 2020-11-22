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
#include "../compiler/compiler.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#define __when(__cond)                          (__cond)
#define when(__cond)                            __when(__cond)
#else
#define __when(...)                             (__VA_ARGS__)
#define when(...)                               __when(__VA_ARGS__)
#endif

#define __vsf_list_tmp_name(__head)             CONNECT2(__head, __LINE__)

/*-----------------------------------------------------------------------------*
 * Single Chain List internal macro                                            *
 *-----------------------------------------------------------------------------*/

#define __vsf_slist_is_empty(__list_ptr)                                        \
            (NULL == ((vsf_slist_t *)(__list_ptr))->head)

#define __vsf_slist_set_next(__node_ptr, __item_next_ptr)                       \
    do {                                                                        \
        ((vsf_slist_node_t *)(__node_ptr))->next = (__item_next_ptr);           \
    } while (0)

#define __vsf_slist_ref_next(__node_ptr, __item_ref_ptr)                        \
    do {                                                                        \
        (*(void **)&(__item_ref_ptr)) =                                         \
            (((vsf_slist_node_t *)(__node_ptr))->next);                         \
    } while (0)

#define __vsf_slist_insert_next(__host_type, __member, __node_ptr, __item_ptr)  \
    do {                                                                        \
        __vsf_slist_set_next(&((__item_ptr)->__member), ((__node_ptr)->next));  \
        __vsf_slist_set_next((__node_ptr), (__item_ptr));                       \
    } while (0)

#define __vsf_slist_remove_next_unsafe( __host_type,                            \
                                        __member,                               \
                                        __node_ptr,                             \
                                        __item_ref_ptr)                         \
    do {                                                                        \
        __vsf_slist_ref_next((__node_ptr), (__item_ref_ptr));                   \
        __vsf_slist_set_next((__node_ptr),                                      \
                ((__host_type *)(__item_ref_ptr))->__member.next);              \
    } while (0)

#define __vsf_slist_insert_after(   __host_type,/* type of the host object */   \
                                    __member,   /* the name of the list */      \
                                    __item_ptr, /* current item address */      \
                                    __item_new_ptr)/* new item address */       \
    do {                                                                        \
        vsf_slist_node_t *__vsf_list_tmp_name(node) = &((__item_ptr)->__member);\
        __vsf_slist_insert_next(__host_type, __member,                          \
                __vsf_list_tmp_name(node), (__item_new_ptr));                   \
    } while (0)

#define __vsf_slist_remove_after(   __host_type, /* type of the host object */  \
                                    __member,    /* the name of the list */     \
                                    __item_ptr,  /* current item address */     \
                                    __item_ref_ptr) /* pointer of host type*/   \
    do {                                                                        \
        vsf_slist_node_t *__vsf_list_tmp_name(node) = &((__item_ptr)->__member);\
        __vsf_slist_ref_next(__vsf_list_tmp_name(node), (__item_ref_ptr));      \
        if (NULL != __vsf_list_tmp_name(node)->next) {                          \
            __vsf_slist_set_next(__vsf_list_tmp_name(node),                     \
                ((__host_type *)(__vsf_list_tmp_name(node)->next))->__member.next);\
            __vsf_slist_set_next(&((__item_ref_ptr)->__member), NULL);          \
        }                                                                       \
    } while (0)


#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#   define __vsf_slist_insert(  __host_type,  /* type of the host object */     \
                                __member,     /* the name of the list */        \
                                __list_ptr,   /* the address of the list */     \
                                __item_ptr,   /* the address of the new item */ \
                                __cond)       /* how to find insert point */    \
    do {                                                                        \
        vsf_slist_node_t *__vsf_list_tmp_name(node) =                           \
                    (vsf_slist_node_t *)(__list_ptr);                           \
        vsf_slist_init_node(__host_type, __member, __item_ptr);                 \
        for (; __vsf_list_tmp_name(node)->next != NULL;){                       \
            const __host_type * const _ =                                       \
                    (__host_type *)(__vsf_list_tmp_name(node)->next);           \
            /* using __VA_ARGS__ so ',' operation could be supported */         \
            if (__cond) {                                                       \
                __vsf_slist_insert_next(__host_type, __member,                  \
                    __vsf_list_tmp_name(node), (__item_ptr));                   \
                break;                                                          \
            }                                                                   \
            __vsf_list_tmp_name(node) = (vsf_slist_node_t *)&(_->__member);     \
        }                                                                       \
        if (NULL == __vsf_list_tmp_name(node)->next) {                          \
            __vsf_slist_insert_next(__host_type, __member,                      \
                    __vsf_list_tmp_name(node), (__item_ptr));                   \
            break;                                                              \
        }                                                                       \
    } while (0)
#else
#   define __vsf_slist_insert(  __host_type,  /* type of the host object */     \
                                __member,     /* the name of the list */        \
                                __list_ptr,   /* the address of the list */     \
                                __item_ptr,   /* the address of the new item */ \
                                ...)          /* how to find insert point */    \
    do {                                                                        \
        vsf_slist_init_node(__host_type, __member, __item_ptr);                 \
        vsf_slist_node_t *__vsf_list_tmp_name(node) =                           \
                    (vsf_slist_node_t *)(__list_ptr);                           \
        for (; __vsf_list_tmp_name(node)->next != NULL;){                       \
            const __host_type * const _ =                                       \
                    (__host_type *)(__vsf_list_tmp_name(node)->next);           \
            /* using __VA_ARGS__ so ',' operation could be supported */         \
            if (__VA_ARGS__) {                                                  \
                __vsf_slist_insert_next(__host_type, __member,                  \
                    __vsf_list_tmp_name(node), (__item_ptr));                   \
                break;                                                          \
            }                                                                   \
            __vsf_list_tmp_name(node) = (vsf_slist_node_t *)&(_->__member);     \
        }                                                                       \
        if (NULL == __vsf_list_tmp_name(node)->next) {                          \
            __vsf_slist_insert_next(__host_type, __member,                      \
                    __vsf_list_tmp_name(node), (__item_ptr));                   \
            break;                                                              \
        }                                                                       \
    } while (0)
#endif

#define __vsf_slist_foreach_unsafe(                                             \
                            __host_type,    /* the type of the host type */     \
                            __member,       /* the member name of the list */   \
                            __list_ptr)     /* the address of the list */       \
    for (   __host_type *_ = (__host_type *)((__list_ptr)->head);               \
            _ != NULL;                                                          \
            _ = (__host_type *)(_->__member.next))

#define __vsf_slist_foreach_next_unsafe(                                        \
                            __host_type,    /* the type of the host type */     \
                            __member,       /* the member name of the list */   \
                            __list_ptr)     /* the address of the list */       \
    for (   __host_type *_ = (__host_type *)((__list_ptr)->head),               \
                *__ = _ ? (__host_type *)(_->__member.next) : NULL;             \
            _ != NULL;                                                          \
            _ = __, __ = _ ? (__host_type *)(_->__member.next) : NULL)


#define __vsf_slist_foreach(__host_type,    /* the type of the host type */     \
                            __member,       /* the member name of the list */   \
                            __list_ptr,     /* the address of the list */       \
                            __item_ptr)     /* the pointer name, e.g. ptarget */\
    for (__host_type                        /* loop initialisation */           \
            *__item_ptr = (__host_type *)(__list_ptr)->head,                    \
            *_ = __item_ptr,                                                    \
            *next = __item_ptr                                                  \
                ?   (__host_type *)                                             \
                        ((__host_type *)((__list_ptr)->head))->__member.next    \
                :   NULL;                                                       \
            __item_ptr != NULL;             /* loop condition */                \
            _ = __item_ptr = next, next = __item_ptr                            \
                ?   (__host_type *)                                             \
                       ((__host_type *)((__list_ptr)->head))->__member.next     \
                :   NULL)                   /* prepare for next iteration */

#define __vsf_slist_discard_head(__host_type, __member, __list_ptr)             \
            if (NULL != (__list_ptr)->head) {                                   \
                __host_type *item = (__host_type *)((__list_ptr)->head);        \
                __vsf_slist_set_next(                                           \
                        (__list_ptr),                                           \
                        ((__host_type *)((__list_ptr)->head))->__member.next);  \
                item->__member.next = NULL;                                     \
            }


#define __vsf_slist_queue_init(__queue_ptr)                                     \
    do {                                                                        \
        __vsf_slist_set_next(&((__queue_ptr)->head), NULL);                     \
        __vsf_slist_set_next(&((__queue_ptr)->tail), NULL);                     \
    } while (0)

#define __vsf_slist_queue_enqueue(__host_type, __member, __queue_ptr, __item_ptr)\
    do {                                                                        \
        __host_type * __vsf_list_tmp_name(host_tail) =                          \
                (__host_type *)((__queue_ptr)->tail.next);                      \
        __vsf_slist_set_next(&((__queue_ptr)->tail), (__item_ptr));             \
        if (__vsf_list_tmp_name(host_tail) != NULL)                             \
            __vsf_slist_set_next(&__vsf_list_tmp_name(host_tail)->__member, (__item_ptr));\
        else                                                                    \
            __vsf_slist_set_next(&((__queue_ptr)->head), (__item_ptr));         \
    } while (0)

#define __vsf_slist_queue_dequeue(  __host_type,                                \
                                    __member,                                   \
                                    __queue_ptr,                                \
                                    __item_ref_ptr)                             \
    do {                                                                        \
        __vsf_slist_ref_next(&((__queue_ptr)->head), (__item_ref_ptr));         \
        if (NULL != (__item_ref_ptr)) {                                         \
            __vsf_slist_set_next(                                               \
                &((__queue_ptr)->head),                                         \
                (__item_ref_ptr)->__member.next);                               \
            if (__vsf_slist_is_empty(&((__queue_ptr)->head)))                   \
                __vsf_slist_set_next(&((__queue_ptr)->tail), NULL);             \
            __vsf_slist_set_next(&((__item_ref_ptr)->__member), NULL);          \
        }                                                                       \
    } while (0)

#define __vsf_slist_queue_peek( __host_type,                                    \
                                __member,                                       \
                                __queue_ptr,                                    \
                                __item_ref_ptr)                                 \
        __vsf_slist_ref_next(&((__queue_ptr)->head), (__item_ref_ptr));


#define __vsf_slist_queue_add_to_head(  __host_type,                            \
                                        __member,                               \
                                        __queue_ptr,                            \
                                        __item_ptr)                             \
    do {                                                                        \
        __host_type * __vsf_list_tmp_name(host_head) =                          \
                (__host_type *)((__queue_ptr)->head.next);                      \
        __vsf_slist_set_next(&(__item_ptr)->__member,                           \
                __vsf_list_tmp_name(host_head));                                \
        __vsf_slist_set_next(&((__queue_ptr)->head), (__item_ptr));             \
        if (NULL == __vsf_list_tmp_name(host_head))                             \
            __vsf_slist_set_next(&((__queue_ptr)->tail), (__item_ptr));         \
    } while (0)

/*-----------------------------------------------------------------------------*
 * Double Chain List intenral macro                                            *
 *-----------------------------------------------------------------------------*/

#define __vsf_dlist_init(__list_ptr)                                            \
    do {                                                                        \
        (__list_ptr)->head = (__list_ptr)->tail = NULL;                         \
    } while (0)

#define __vsf_dlist_init_node(__node_ptr)                                       \
    do {                                                                        \
        (__node_ptr)->next = (__node_ptr)->prev = NULL;                         \
    } while (0)

#define __vsf_dlist_get_host(__host_type, __member, __node_ptr)                 \
            container_of((__node_ptr), __host_type, __member)

#define __vsf_dlist_get_host_safe(__host_type, __member, __node_ptr)            \
            safe_container_of((__node_ptr), __host_type, __member)

#define __vsf_dlist_is_empty(__list_ptr)       ((__list_ptr)->head == NULL)

#define __vsf_dlist_is_in(__list_ptr, __node_ptr)                               \
            __vsf_dlist_is_in_imp((__list_ptr), (__node_ptr))

#define __vsf_dlist_ref(__host_type, __member, __node_ptr, __item_ref_ptr)      \
    do {                                                                        \
        (*(__host_type **)&(__item_ref_ptr)) =                                  \
                __vsf_dlist_get_host(__host_type, __member, __node_ptr);        \
    } while (0)

#define __vsf_dlist_ref_safe(__host_type, __member, __node_ptr, __item_ref_ptr) \
    do {                                                                        \
        (*(__host_type **)&(__item_ref_ptr)) =                                  \
                __vsf_dlist_get_host_safe(__host_type, __member, __node_ptr);   \
    } while (0)

#define __vsf_dlist_add_to_head(__host_type, __member, __list_ptr, __item_ptr)  \
            __vsf_dlist_add_to_head_imp((__list_ptr), &((__item_ptr)->__member))

#define __vsf_dlist_add_to_tail(__host_type, __member, __list_ptr, __item_ptr)  \
            __vsf_dlist_add_to_tail_imp((__list_ptr), &((__item_ptr)->__member))

#define __vsf_dlist_peek_next(__host_type, __member, __item_ptr, __item_ref_ptr)\
    do {                                                                        \
        vsf_dlist_node_t *__vsf_list_tmp_name(node) = (__item_ptr)->__member.next;\
        __vsf_dlist_ref_safe(__host_type, __member,                             \
                __vsf_list_tmp_name(node), (__item_ref_ptr));                   \
    } while (0)

#define __vsf_dlist_peek_head(__host_type, __member, __list_ptr, __item_ref_ptr)\
    do {                                                                        \
        vsf_dlist_node_t *__vsf_list_tmp_name(node) = (__list_ptr)->head;       \
        __vsf_dlist_ref_safe(__host_type, __member,                             \
                __vsf_list_tmp_name(node), (__item_ref_ptr));                   \
    } while (0)

#define __vsf_dlist_remove_head(__host_type,                                    \
                                __member,                                       \
                                __list_ptr,                                     \
                                __item_ref_ptr)                                 \
    do {                                                                        \
        vsf_dlist_node_t *__vsf_list_tmp_name(node) =                           \
                __vsf_dlist_remove_head_imp(__list_ptr);                        \
        __vsf_dlist_ref_safe(__host_type, __member,                             \
                __vsf_list_tmp_name(node), (__item_ref_ptr));                   \
    } while (0)

#define __vsf_dlist_remove_tail(__host_type,                                    \
                                __member,                                       \
                                __list_ptr,                                     \
                                __item_ref_ptr)                                 \
    do {                                                                        \
        vsf_dlist_t *__vsf_list_tmp_name(node) =                                \
                __vsf_dlist_remove_tail_imp(__list_ptr);                        \
        __vsf_dlist_ref_safe(__host_type, __member,                             \
                __vsf_list_tmp_name(node), (__item_ref_ptr));                   \
    } while (0)

#define __vsf_dlist_insert_after(                                               \
                __host_type, __member, __list_ptr, __item_prv_ptr, __item_ptr)  \
            __vsf_dlist_insert_after_imp(                                       \
                (__list_ptr),                                                   \
                &((__item_prv_ptr)->__member),                                  \
                &((__item_ptr)->__member))

#define __vsf_dlist_insert_before(                                              \
                __host_type, __member, __list_ptr, __item_next_ptr, __item_ptr) \
            __vsf_dlist_insert_before_imp(                                      \
                (__list_ptr),                                                   \
                &((__item_next_ptr)->__member),                                 \
                &((__item_ptr)->__member))

#define __vsf_dlist_remove(__host_type, __member, __list_ptr, __item_ptr)       \
            __vsf_dlist_remove_imp((__list_ptr), &((__item_ptr)->__member))

#define __vsf_dlist_remove_after(                                               \
                __host_type, __member, __list_ptr, __node_ptr, __item_ref_ptr)  \
    do {                                                                        \
        vsf_dlist_node_t *__vsf_list_tmp_name(node_nxt) = (__node_ptr)->next;   \
        if (__vsf_list_tmp_name(node_nxt) != NULL) {                            \
            __vsf_dlist_remove(__host_type, __member, __list_ptr,               \
                    __vsf_list_tmp_name(node_nxt));                             \
            __vsf_dlist_ref(__host_type, __member,                              \
                    __vsf_list_tmp_name(node_nxt), (__item_ref_ptr));           \
        } else {                                                                \
            (*(__host_type **)&(__item_ref_ptr)) = NULL;                        \
        }                                                                       \
    } while (0)

#define __vsf_dlist_remove_before(                                              \
                __host_type, __member, __list_ptr, __node_ptr, __item_ref_ptr)  \
    do {                                                                        \
        vsf_dlist_node_t *__vsf_list_tmp_name(node_prv) = (__node_ptr)->prev;   \
        if (__vsf_list_tmp_name(node_prv) != NULL) {                            \
            __vsf_dlist_remove(__host_type, __member, __list_ptr,               \
                    __vsf_list_tmp_name(node_prv));                             \
            __vsf_dlist_ref(__host_type, __member,                              \
                    __vsf_list_tmp_name(node_prv), (__item_ref_ptr));           \
        } else {                                                                \
            (*(__host_type **)&(__item_ref_ptr)) = NULL;                        \
        }                                                                       \
    } while (0)

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#   define __vsf_dlist_insert(  __host_type,/* type of the host object */       \
                                __member,   /* the name of the list */          \
                                __list_ptr, /* the address of the list */       \
                                __item_ptr, /* the address of the new item */   \
                                __cond)     /* how to find insert point */      \
    do {                                                                        \
        vsf_dlist_node_t *__vsf_list_tmp_name(node) =                           \
                (vsf_dlist_node_t *)(__list_ptr);                               \
        vsf_dlist_init_node(__host_type, __member, __item_ptr);                 \
        for (; __vsf_list_tmp_name(node)->next != NULL;){                       \
            const __host_type * const _ = __vsf_dlist_get_host(__host_type,     \
                __member, __vsf_list_tmp_name(node)->next);                     \
            /* using __VA_ARGS__ so ',' operation could be supported */         \
            if (__cond) {                                                       \
                __vsf_dlist_insert_before_imp((__list_ptr),                     \
                    __vsf_list_tmp_name(node)->next, &((__item_ptr)->__member));\
                break;                                                          \
            }                                                                   \
            __vsf_list_tmp_name(node) = __vsf_list_tmp_name(node)->next;        \
        }                                                                       \
        if (NULL == __vsf_list_tmp_name(node)->next) {                          \
            __vsf_dlist_add_to_tail(                                            \
                    __host_type, __member, __list_ptr, (__item_ptr));           \
        }                                                                       \
    } while (0)
#else
#   define __vsf_dlist_insert(     __host_type,/* type of the host object */    \
                                __member,   /* the name of the list */          \
                                __list_ptr, /* the address of the list */       \
                                __item_ptr, /* the address of the new item */   \
                                ...)        /* how to find insert point */      \
    do {                                                                        \
        vsf_dlist_init_node(__host_type, __member, __item_ptr);                 \
        vsf_dlist_node_t *__vsf_list_tmp_name(node) =                           \
                (vsf_dlist_node_t *)(__list_ptr);                               \
        for (; __vsf_list_tmp_name(node)->next != NULL;){                       \
            const __host_type * const _ = __vsf_dlist_get_host(__host_type,     \
                __member, __vsf_list_tmp_name(node)->next);                     \
            /* using __VA_ARGS__ so ',' operation could be supported */         \
            if (__VA_ARGS__) {                                                  \
                __vsf_dlist_insert_before_imp((__list_ptr),                     \
                    __vsf_list_tmp_name(node)->next, &((__item_ptr)->__member));\
                break;                                                          \
            }                                                                   \
            __vsf_list_tmp_name(node) = __vsf_list_tmp_name(node)->next;        \
        }                                                                       \
        if (NULL == __vsf_list_tmp_name(node)->next) {                          \
            __vsf_dlist_add_to_tail(                                            \
                    __host_type, __member, __list_ptr, (__item_ptr));           \
        }                                                                       \
    } while (0)
#endif

#define __vsf_dlist_foreach_unsafe(                                             \
                            __host_type,    /* the type of the host type */     \
                            __member,       /* the member name of the list */   \
                            __list_ptr)     /* the address of the list */       \
    for (   __host_type *_ = NULL == (__list_ptr)->head ? NULL :                \
                __vsf_dlist_get_host(__host_type, __member, (__list_ptr)->head);\
            _ != NULL;                                                          \
            _ = NULL == _->__member.next ? NULL:                                \
                __vsf_dlist_get_host(__host_type, __member, _->__member.next))


/*-----------------------------------------------------------------------------*
 * Single Chain List                                                           *
 *-----------------------------------------------------------------------------*/

//! \name list normal access
//! @{

#define vsf_slist_init(__list_ptr)          /* the address of the list */       \
            __vsf_slist_set_next((__list_ptr), NULL)

#define vsf_slist_is_empty(__list_ptr)      /* the address of the list */       \
            __vsf_slist_is_empty(__list_ptr)

#define vsf_slist_set_next( __host_type,    /* the type of the host type */     \
                            __member,       /* the member name of the list */   \
                            __node_ptr,     /* the reference node */            \
                            __item_ptr)     /* the address of the target item */\
            __vsf_slist_set_next((__node_ptr), (__item_ptr))

#define vsf_slist_get_length(   __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list */   \
                                __list_ptr) /* the address of the list */       \
            __vsf_slist_get_length_imp(                                         \
                (__list_ptr), offset_of(__host_type, __member))

#define vsf_slist_remove_tail(  __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list */   \
                                __list_ptr) /* the address of the list */       \
            __vsf_slist_remove_tail_imp(                                        \
                (__list_ptr), offset_of(__host_type, __member))

#define vsf_slist_get_item_by_index(                                            \
                                __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list */   \
                                __list_ptr, /* the address of the list */       \
                                __index)    /* index of the item in list */     \
            __vsf_slist_get_item_by_index_imp(                                  \
                (__list_ptr), (__index), offset_of(__host_type, __member))

#define vsf_slist_get_index(    __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list */   \
                                __list_ptr, /* the address of the list */       \
                                __item_ptr) /* the address of the target item */\
            __vsf_slist_get_index_imp(                                          \
                (__list_ptr), (__item_ptr), offset_of(__host_type, __member))

#define vsf_slist_append(   __host_type,    /* the type of the host type */     \
                            __member,       /* the member name of the list */   \
                            __list_ptr,     /* the address of the list */       \
                            __item_ptr)     /* the address of the target item */\
            __vsf_slist_append_imp(                                             \
                (__list_ptr), (__item_ptr), offset_of(__host_type, __member))

#define vsf_slist_is_in(    __host_type,    /* the type of the host type */     \
                            __member,       /* the member name of the list */   \
                            __list_ptr,     /* the address of the list */       \
                            __item_ptr)     /* the address of the target item */\
        (   vsf_slist_get_index(                                                \
                __host_type, __member, (__list_ptr), (__item_ptr))              \
        >=  0)

#define vsf_slist_init_node(__host_type,    /* the type of the host type */     \
                            __member,       /* the member name of the list */   \
                            __item_ptr)     /* the address of the target item */\
            __vsf_slist_set_next(&((__item_ptr)->__member), NULL)

#define vsf_slist_remove(   __host_type,    /* the type of the host type */     \
                            __member,       /* the member name of the list*/    \
                            __list_ptr,     /* the address of the list */       \
                            __item_ptr)     /* the address of the target item */\
            __vsf_slist_remove_imp(                                             \
                (__list_ptr), (__item_ptr), offset_of(__host_type, __member))

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#   define vsf_slist_insert(__host_type,    /* the type of the host type */     \
                            __member,       /* the member name of the list */   \
                            __list_ptr,     /* the address of the list */       \
                            __item_ptr,     /* the address of the target item */\
                            __cond)         /* when( condition expression ) */  \
            __vsf_slist_insert(                                                 \
                __host_type, __member, (__list_ptr), (__item_ptr), (__cond))
#else
#   define vsf_slist_insert(__host_type,    /* the type of the host type */     \
                            __member,       /* the member name of the list */   \
                            __list_ptr,     /* the address of the list */       \
                            __item_ptr,     /* the address of the target item */\
                            ...)            /* when( condition expression ) */  \
            __vsf_slist_insert(                                                 \
                __host_type, __member, (__list_ptr), (__item_ptr), __VA_ARGS__)
#endif

#define vsf_slist_remove_after( __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list*/    \
                                __item_ptr, /* the address of current item */   \
                                __item_ref_ptr)/* the pointer of host type */   \
            __vsf_slist_remove_after(                                           \
                __host_type, __member, (__item_ptr), (__item_ref_ptr))

#define vsf_slist_insert_after( __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list*/    \
                                __item_ptr, /* the address of current item */   \
                                __item_new_ptr)/* the address of new item */    \
            __vsf_slist_insert_after(                                           \
                __host_type, __member, (__item_ptr), (__item_new_ptr))

//! @}


//! \name stack / stack-like operations using list
//£¡ @{
#define vsf_slist_stack_init(__list_ptr)    vsf_slist_init_node(__list_ptr)

#define vsf_slist_stack_pop(__host_type,    /* the type of the host type */     \
                            __member,       /* the member name of the list */   \
                            __list_ptr,     /* the address of the list */       \
                            __item_ref_ptr) /* the pointer of host type */      \
            vsf_slist_remove_from_head(                                         \
                __host_type, __member, (__list_ptr), (__item_ref_ptr))


#define vsf_slist_stack_push(   __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list */   \
                                __list_ptr, /* the address of the list */       \
                                __item_ptr) /* the address of the target item */\
            vsf_slist_add_to_head(                                              \
                __host_type, __member, (__list_ptr), (__item_ptr))


#define vsf_slist_add_to_head(  __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list */   \
                                __list_ptr, /* the address of the list */       \
                                __item_ptr) /* the address of the target item */\
            __vsf_slist_insert_next(                                            \
                __host_type,                                                    \
                __member,                                                       \
                (vsf_slist_node_t *)(__list_ptr),                               \
                (__item_ptr))

#define vsf_slist_discard_head( __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list */   \
                                __list_ptr) /* the address of the list */       \
            __vsf_slist_discard_head(__host_type, __member, (__list_ptr))

#define vsf_slist_remove_from_head_unsafe(                                      \
                                __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list*/    \
                                __list_ptr, /* the address of the list */       \
                                __item_ref_ptr)/* the pointer of host type */   \
            __vsf_slist_remove_next_unsafe(                                     \
                __host_type,                                                    \
                __member,                                                       \
                (vsf_slist_node_t *)(__list_ptr),                               \
                (__item_ref_ptr))

#define vsf_slist_remove_from_head(                                             \
                                __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list*/    \
                                __list_ptr, /* the address of the list */       \
                                __item_ref_ptr)/* the pointer of host type */   \
    do {                                                                        \
        vsf_slist_peek_next(__host_type, __member, (__list_ptr), (__item_ref_ptr));\
        vsf_slist_discard_head(__host_type, __member, (__list_ptr));            \
    } while (0)

#define vsf_slist_peek_next(__host_type,    /* the type of the host type */     \
                            __member,       /* the member name of the list */   \
                            __node_ptr,     /* the reference node */            \
                            __item_ref_ptr) /* the pointer of host type */      \
            __vsf_slist_ref_next((__node_ptr), (__item_ref_ptr))
//! @}

//! \name list enumeration access
//! @{
#define vsf_slist_foreach(  __host_type,    /* the type of the host type */     \
                            __member,       /* the member name of the list */   \
                            __list_ptr,     /* the address of the list */       \
                            __item_ptr)     /* the pointer name, e.g. ptarget */\
            __vsf_slist_foreach(__host_type, __member, (__list_ptr), __item_ptr)
//! @}


//! \name queue / queue-like operations using slist
//£¡ @{
#define vsf_slist_queue_is_empty(__queue_ptr)                                   \
            vsf_slist_is_empty(&(__queue_ptr)->head)

#define vsf_slist_queue_init(__queue_ptr)                                       \
            __vsf_slist_queue_init((__queue_ptr))/* the address of the queue */

#define vsf_slist_queue_enqueue(__host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list*/    \
                                __queue_ptr,/* the address of the queue */      \
                                __item_ptr) /* the address of the target item */\
            __vsf_slist_queue_enqueue(                                          \
                    __host_type, __member, (__queue_ptr), (__item_ptr))

#define vsf_slist_queue_dequeue(__host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list*/    \
                                __queue_ptr,/* the address of the queue */      \
                                __item_ref_ptr)/* the pointer of host type */   \
            __vsf_slist_queue_dequeue(                                          \
                    __host_type, __member, (__queue_ptr), (__item_ref_ptr))

#define vsf_slist_queue_peek(   __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list*/    \
                                __queue_ptr,/* the address of the queue */      \
                                __item_ref_ptr)/* the pointer of host type */   \
            __vsf_slist_queue_peek(                                             \
                    __host_type, __member, (__queue_ptr), (__item_ref_ptr))

#define vsf_slist_queue_add_to_head(                                            \
                                __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list*/    \
                                __queue_ptr,/* the address of the queue */      \
                                __item_ptr) /* the address of the target item */\
            __vsf_slist_queue_add_to_head(                                      \
                    __host_type, __member, (__queue_ptr), (__item_ptr))
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

#define vsf_dlist_init(__list_ptr)          /* the address of the list */       \
            __vsf_dlist_init(__list_ptr)

#define vsf_dlist_is_empty(__list_ptr)      /* the address of the list */       \
            __vsf_dlist_is_empty(__list_ptr)

#define vsf_dlist_is_in(        __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list */   \
                                __list_ptr, /* the address of the list */       \
                                __item_ptr) /* the address of the target item */\
            __vsf_dlist_is_in((__list_ptr), &(__item_ptr)->__member)

#define vsf_dlist_init_node(    __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list */   \
                                __item_ptr) /* the address of the target item */\
            __vsf_dlist_init_node(&((__item_ptr)->__member))

#define vsf_dlist_add_to_head(  __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list */   \
                                __list_ptr, /* the address of the list */       \
                                __item_ptr) /* the address of the target item */\
            __vsf_dlist_add_to_head(__host_type,                                \
                                    __member,                                   \
                                    (__list_ptr),                               \
                                    (__item_ptr))

#define vsf_dlist_add_to_tail(  __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list */   \
                                __list_ptr, /* the address of the list */       \
                                __item_ptr) /* the address of the target item */\
            __vsf_dlist_add_to_tail(__host_type,                                \
                                    __member,                                   \
                                    (__list_ptr),                               \
                                    (__item_ptr))

#define vsf_dlist_peek_head(    __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list */   \
                                __list_ptr, /* the address of the list */       \
                                __item_ref_ptr)/* the pointer of host type */   \
            __vsf_dlist_peek_head(                                              \
                    __host_type, __member, (__list_ptr), (__item_ref_ptr))

#define vsf_dlist_peek_next(    __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list */   \
                                __item_ptr, /* the address of the target item */\
                                __item_ref_ptr)/* the pointer of host type */   \
            __vsf_dlist_peek_next(                                              \
                    __host_type, __member, (__item_ptr), (__item_ref_ptr))

#define vsf_dlist_remove_head(  __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list */   \
                                __list_ptr, /* the address of the list */       \
                                __item_ref_ptr)/* the pointer of host type */   \
            __vsf_dlist_remove_head(                                            \
                    __host_type, __member, (__list_ptr), (__item_ref_ptr))

#define vsf_dlist_remove_tail(  __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list */   \
                                __list_ptr, /* the address of the list */       \
                                __item_ref_ptr)/* the pointer of host type */   \
            __vsf_dlist_remove_tail(                                            \
                    __host_type, __member, (__list_ptr), (__item_ref_ptr))

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#   define vsf_dlist_insert(    __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list */   \
                                __list_ptr, /* the address of the list */       \
                                __item_ptr, /* the address of the target item */\
                                __cond)     /* when( condition expression ) */  \
            __vsf_dlist_insert(                                                 \
                __host_type, __member, (__list_ptr), (__item_ptr), (__cond))
#else
#   define vsf_dlist_insert(    __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list */   \
                                __list_ptr, /* the address of the list */       \
                                __item_ptr, /* the address of the target item */\
                                ...)        /* when( condition expression ) */  \
            __vsf_dlist_insert(                                                 \
                __host_type, __member, (__list_ptr), (__item_ptr), __VA_ARGS__)
#endif

#define vsf_dlist_insert_after( __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list*/    \
                                __list_ptr, /* the address of the list */       \
                                __item_prv_ptr,/* the address of the prev item */\
                                __item_ptr) /* the address of the target item */\
            __vsf_dlist_insert_after(                                           \
                __host_type,                                                    \
                __member,                                                       \
                (__list_ptr),                                                   \
                (__item_prv_ptr),                                               \
                (__item_ptr))

#define vsf_dlist_insert_before(__host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list*/    \
                                __list_ptr, /* the address of the list */       \
                                __item_next_ptr,/* the address of the next item */\
                                __item_ptr) /* the address of the target item */\
            __vsf_dlist_insert_before(                                          \
                __host_type,                                                    \
                __member,                                                       \
                (__list_ptr),                                                   \
                (__item_next_ptr),                                              \
                (__item_ptr))

#define vsf_dlist_remove(       __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list*/    \
                                __list_ptr, /* the address of the list */       \
                                __item_ptr) /* the address of the target item */\
            __vsf_dlist_remove(__host_type, __member, (__list_ptr), (__item_ptr))

#define vsf_dlist_remove_after( __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list*/    \
                                __list_ptr, /* the address of the list */       \
                                __node_ptr, /* the reference node */            \
                                __item_ref_ptr)/* the pointer of host type */   \
            __vsf_dlist_remove_after(                                           \
                __host_type,                                                    \
                __member,                                                       \
                (__list_ptr),                                                   \
                (__node_ptr),                                                   \
                (__item_ref_ptr))

#define vsf_dlist_remove_before(__host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list*/    \
                                __list_ptr, /* the address of the list */       \
                                __node_ptr, /* the reference node */            \
                                __item_ref_ptr)/* the pointer of host type */   \
            __vsf_dlist_remove_before(                                          \
                __host_type,                                                    \
                __member,                                                       \
                (__list_ptr),                                                   \
                (__node_ptr),                                                   \
                (__item_ref_ptr))

//! @}

//! \name queue operation using dual-way linked list
//! @{

#define vsf_dlist_queue_peek(   __host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list */   \
                                __list_ptr, /* the address of the list */       \
                                __item_ref_ptr)/* the pointer of host type */   \
            __vsf_dlist_ref_safe(                                               \
                    __host_type, __member, (__list_ptr)->head, (__item_ref_ptr))

#define vsf_dlist_queue_enqueue(__host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list*/    \
                                __list_ptr, /* the address of the list */       \
                                __item_ptr) /* the address of the target item */\
            vsf_dlist_add_to_tail(                                              \
                    __host_type, __member, (__list_ptr), (__item_ptr))

#define vsf_dlist_queue_dequeue(__host_type,/* the type of the host type */     \
                                __member,   /* the member name of the list*/    \
                                __list_ptr, /* the address of the list */       \
                                __item_ref_ptr)/* the address of the target item */\
            vsf_dlist_remove_head(                                              \
                    __host_type, __member, (__list_ptr), (__item_ref_ptr))

//! @}

/*============================ TYPES =========================================*/

//! \name single list item
//! @{
typedef struct vsf_slist_t {
    void *head;                             /*!< pointer for the next */
} vsf_slist_t;

typedef struct vsf_slist_node_t {
    void *next;                             /*!< pointer for the next */
} vsf_slist_node_t;
//! @}

//! \name dual-way linked list item
//! @{
typedef struct vsf_dlist_t {
    struct vsf_dlist_node_t *head;          /*!< pointer for the head */
    struct vsf_dlist_node_t *tail;          /*!< pointer for the tail */
} vsf_dlist_t;

typedef struct vsf_dlist_node_t {
    struct vsf_dlist_node_t *next;          /*!< slist node in forward list */
    struct vsf_dlist_node_t *prev;          /*!< slist node in backward list */
} vsf_dlist_node_t;
//! @}

//! \name single list queue header
//! @{
typedef struct vsf_slist_queue_t {
    vsf_slist_node_t head;                  /*!< pointer for the queue head */
    vsf_slist_node_t tail;                  /*!< pointer for the queue tail */
} vsf_slist_queue_t;
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

/*! \brief calculate the length of the target list
 *! \param this_ptr        address of the target list
 *! \param list_offset  the list offset in byte within a host type
 *! \return the length of the list, -1 means illegal input
 */
extern uint_fast16_t __vsf_slist_get_length_imp(
                                    vsf_slist_t *this_ptr,
                                    size_t list_offset);

/*! \brief get the specified item with a given index from the target list
 *! \param this_ptr        address of the target list
 *! \param index        index number of the wanted item
 *! \param list_offset  the list offset in byte within a host type
 *! \retval NULL        Illegal input
 *! \retval !NULL       the address of the wanted item
 */
extern void * __vsf_slist_get_item_by_index_imp(
                                    vsf_slist_t *this_ptr,
                                    uint_fast16_t index,
                                    size_t list_offset);

/*! \brief find the index number of a given item from the target list
 *! \param this_ptr        address of the target list
 *! \param item         address of the target item
 *! \param list_offset  the list offset in byte within a host type
 *! \return the index number, (-1) means illegal input or the item doesn't exist
 */
extern int_fast16_t __vsf_slist_get_index_imp(  vsf_slist_t *this_ptr,
                                                void *item,
                                                size_t list_offset);

/*! \brief remove a item from the target list
 *! \param this_ptr        address of the target list
 *! \param item         address of the target item
 *! \param list_offset  the list offset in byte within a host type
 *! \retval NULL        Illegal input
 *! \retval !NULL       the address of the removed item
 */
extern void * __vsf_slist_remove_imp(   vsf_slist_t *this_ptr,
                                        void *item,
                                        size_t list_offset);

/*! \brief append a item from the target list
 *! \param this_ptr        address of the target list
 *! \param item         address of the target item
 *! \param list_offset  the list offset in byte within a host type
 *! \retval NULL        Illegal input
 *! \retval !NULL       the address of the appended item
 */
extern void * __vsf_slist_append_imp(   vsf_slist_t *this_ptr,
                                        void *item,
                                        size_t list_offset);

extern void * __vsf_slist_remove_tail_imp(  vsf_slist_t *this_ptr,
                                            size_t list_offset);

extern bool __vsf_dlist_is_in_imp(  vsf_dlist_t *this_ptr,
                                    vsf_dlist_node_t *node);

extern void __vsf_dlist_add_to_head_imp(vsf_dlist_t *this_ptr,
                                        vsf_dlist_node_t *node);

extern void __vsf_dlist_add_to_tail_imp(vsf_dlist_t *this_ptr,
                                        vsf_dlist_node_t *node);

extern vsf_dlist_node_t *__vsf_dlist_remove_head_imp(vsf_dlist_t *this_ptr);

extern vsf_dlist_node_t *__vsf_dlist_remove_tail_imp(vsf_dlist_t *this_ptr);

extern void __vsf_dlist_insert_after_imp(   vsf_dlist_t *this_ptr,
                                            vsf_dlist_node_t *node_prv,
                                            vsf_dlist_node_t *node);

extern void __vsf_dlist_insert_before_imp(  vsf_dlist_t *this_ptr,
                                            vsf_dlist_node_t *node_nxt,
                                            vsf_dlist_node_t *node);

extern void __vsf_dlist_remove_imp(vsf_dlist_t *this_ptr, vsf_dlist_node_t *node);

#ifdef __cplusplus
}
#endif

#endif
