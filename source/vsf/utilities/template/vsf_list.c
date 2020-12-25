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

/*============================ INCLUDES ======================================*/
#include "utilities/vsf_utilities_cfg.h"
#include "vsf_list.h"

/*============================ MACROS ========================================*/
#if __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#endif

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define __vsf_get_slist_address(__node_ptr, __offset)                              \
            ((vsf_slist_t *)(((uint8_t *)(__node_ptr)) + (__offset)))

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/



/*! \brief calculate the length of the target list 
 *! \param this_ptr        address of the target list
 *! \param list_offset  the list offset in byte within a host type
 *! \return the length of the list
 */
uint_fast16_t __vsf_slist_get_length_imp(vsf_slist_t *this_ptr, size_t list_offset)
{
    uint_fast16_t length = 0;
    ASSERT(NULL != this_ptr);

    while (NULL != this_ptr->head) {
        length++;
        this_ptr = __vsf_get_slist_address(this_ptr->head, list_offset);
    }

    return length;
}

/*! \brief get the specified item with a given index from the target list
 *! \param this_ptr        address of the target list
 *! \param index        index number of the wanted item
 *! \param list_offset  the list offset in byte within a host type
 *! \retval NULL        Illegal input 
 *! \retval !NULL       the address of the wanted item
 */
void * __vsf_slist_get_item_by_index_imp(
                            vsf_slist_t *this_ptr,
                            uint_fast16_t index,
                            size_t list_offset)
{
    void *item = NULL;
    ASSERT(NULL != this_ptr);

    do {
        if (NULL == this_ptr->head) {
            break;
        }
        if (0 == index--) {
            item = this_ptr->head;
        }
        this_ptr = __vsf_get_slist_address(this_ptr->head, list_offset);
    } while (index);

    return item;
}

/*! \brief find the index number of a given item from the target list 
 *! \param this_ptr        address of the target list
 *! \param item         address of the target item
 *! \param list_offset  the list offset in byte within a host type
 *! \return the index number, (-1) means illegal input or the item doesn't exist
 */
int_fast16_t __vsf_slist_get_index_imp( vsf_slist_t *this_ptr, 
                                        void *item, 
                                        size_t list_offset)
{
    int_fast16_t index = -1, n = 0;
    ASSERT((NULL != this_ptr) && (NULL != item));

    while (NULL != this_ptr->head) {
        if (this_ptr->head == item) {
            index = n;
            break;
        }
        n++;
        this_ptr = __vsf_get_slist_address(this_ptr->head, list_offset);
    }

    return index;
}

/*! \brief remove a item from the target list
 *! \param this_ptr        address of the target list
 *! \param item         address of the target item
 *! \param list_offset  the list offset in byte within a host type
 *! \retval NULL        Illegal input 
 *! \retval !NULL       the address of the removed item
 */
void * __vsf_slist_remove_imp(vsf_slist_t *this_ptr, void *item, size_t list_offset)
{
    void *item_tmp = NULL;
    ASSERT((NULL != this_ptr) && (NULL != item));

    while (NULL != this_ptr->head) {
        if ((void *)(this_ptr->head) == item) {
            //! remove the target item
            this_ptr->head = __vsf_get_slist_address(item, list_offset)->head;
            //! cut the item off from the list
            __vsf_get_slist_address(item, list_offset)->head = NULL;
            item_tmp = item;
            break;
        }
        this_ptr = __vsf_get_slist_address(this_ptr->head, list_offset);
    }

    return item_tmp;
}

/*! \brief append a item to the target list
 *! \param this_ptr        address of the target list
 *! \param item         address of the target item
 *! \param list_offset  the list offset in byte within a host type
 *! \retval NULL        Illegal input 
 *! \retval !NULL       the address of the appended item
 */
void * __vsf_slist_append_imp(vsf_slist_t *this_ptr, void *item, size_t list_offset)
{
    ASSERT((NULL != this_ptr) && (NULL != item));

    while (NULL != this_ptr->head) {
        this_ptr = __vsf_get_slist_address(this_ptr->head, list_offset);
    }
    this_ptr->head = item;                   //!< append the item
    __vsf_get_slist_address(item, list_offset)->head = NULL;

    return item;
}

void * __vsf_slist_remove_tail_imp(vsf_slist_t *this_ptr, size_t list_offset)
{
    void *item = this_ptr->head;

    while ( (item != NULL)
        &&  (__vsf_get_slist_address(item, list_offset)->head != NULL)) {
        this_ptr = __vsf_get_slist_address(item, list_offset);
        item = this_ptr->head;
    }
    this_ptr->head = NULL;
    return item;
}

bool __vsf_dlist_is_in_imp(vsf_dlist_t *this_ptr, vsf_dlist_node_t *node)
{
    return (node->next != NULL) || (node->prev != NULL) || (this_ptr->head == node);
}

void __vsf_dlist_add_to_head_imp(vsf_dlist_t *this_ptr, vsf_dlist_node_t *node)
{
    vsf_dlist_node_t *node_nxt = this_ptr->head;

    node->next = node_nxt;
    node->prev = NULL;
    this_ptr->head = node;
    if (node_nxt != NULL) {
        node_nxt->prev = node;
    } else {
        this_ptr->tail = node;
    }
}

void __vsf_dlist_add_to_tail_imp(vsf_dlist_t *this_ptr, vsf_dlist_node_t *node)
{
    vsf_dlist_node_t *node_prv = this_ptr->tail;

    node->prev = node_prv;
    node->next = NULL;
    this_ptr->tail = node;
    if (node_prv != NULL) {
        node_prv->next = node;
    } else {
        this_ptr->head = node;
    }
}

vsf_dlist_node_t *__vsf_dlist_remove_head_imp(vsf_dlist_t *this_ptr)
{
    vsf_dlist_node_t *node = this_ptr->head;

    if (node != NULL) {
        vsf_dlist_node_t *node_nxt = node->next;
        node->next = NULL;
        this_ptr->head = node_nxt;
        if (node_nxt != NULL) {
            node_nxt->prev = NULL;
        } else {
            this_ptr->tail = NULL;
        }
    }
    return node;
}

vsf_dlist_node_t *__vsf_dlist_remove_tail_imp(vsf_dlist_t *this_ptr)
{
    vsf_dlist_node_t *node = this_ptr->tail;

    if (node != NULL) {
        vsf_dlist_node_t *node_prv = node->prev;
        node->prev = NULL;
        this_ptr->tail = node_prv;
        if (node_prv != NULL) {
            node_prv->next = NULL;
        } else {
            this_ptr->head = NULL;
        }
    }
    return node;
}

void __vsf_dlist_insert_after_imp(  vsf_dlist_t *this_ptr,
                                    vsf_dlist_node_t *node_prv,
                                    vsf_dlist_node_t *node)
{
    vsf_dlist_node_t *node_nxt = node_prv->next;

    node->next = node_nxt;
    node->prev = node_prv;
    node_prv->next = node;
    if (node_nxt != NULL) {
        node_nxt->prev = node;
    } else {
        this_ptr->tail = node;
    }
}

void __vsf_dlist_insert_before_imp( vsf_dlist_t *this_ptr,
                                    vsf_dlist_node_t *node_nxt,
                                    vsf_dlist_node_t *node)
{
    vsf_dlist_node_t *node_prv = node_nxt->prev;

    node->next = node_nxt;
    node->prev = node_prv;
    node_nxt->prev = node;
    if (node_prv != NULL) {
        node_prv->next = node;
    } else {
        this_ptr->head = node;
    }
}

void __vsf_dlist_remove_imp(vsf_dlist_t *this_ptr, vsf_dlist_node_t *node)
{
    vsf_dlist_node_t *node_nxt = node->next, *node_prv = node->prev;

    node->next = node->prev = NULL;
    if (node_prv != NULL) {
        node_prv->next = node_nxt;
    } else {
        this_ptr->head = node_nxt;
    }
    if (node_nxt != NULL) {
        node_nxt->prev = node_prv;
    } else {
        this_ptr->tail = node_prv;
    }
}

#if __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic pop
#endif

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif
