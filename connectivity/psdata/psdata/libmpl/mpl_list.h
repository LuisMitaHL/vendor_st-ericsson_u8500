/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 * 
 * \brief  Single linked list AP
 */

#ifndef _MPL_LIST_H
#define _MPL_LIST_H

/**************************************************************************
 * Includes
 *************************************************************************/
#include <stdbool.h>
#include <stddef.h>

#ifdef  __cplusplus
extern "C" {
#endif


/********************************************************************************
 *
 * Defines & Type definitions
 *
 ********************************************************************************/

/**
 * mpl_list_t
 *
 * Structure for linking containers into a list.
 *
 * The struct mpl_list_t is meant to be a member of the nodes that are to be linked. This makes
 * the list implementation generic, i.e. all kinds of nodes (containers) can be linked in lists
 * (although one particular list will normally link only one type of container).
 * A restriction is that one list entry in a container can only make the container member of one list at
 * a time. If one wants a container to be member of several lists at the same time, several list entries
 * must exist in the container.
 *
 * container (node):
 * +-----------------------------+
 * | contents                    |
 * | .....                       |
 * | ...                         |
 * | mpl_list_t list_entry;  |
 * +-----------------------------+
 *
 * list:         container                container                container
 *               +--------------------+   +--------------------+   +--------------------+
 * list_p --+    |  contents          |   |  contents          |   |  contents          |
 *          |    |  .....             |   |  .....             |   |  .....             |
 *          |    |  ...               |   |  ...               |   |  ...               |
 *          +----|->list_entry.next_p-----|->list_entry.next_p-----|->list_entry.next_p-----+
 *               +--------------------+   +--------------------+   +--------------------+   |
 *                                                                                         ---
 *
 * The container itself is obtained by using the macro MPL_LIST_CONTAINER() on the list pointer.
 *
 *
 */
struct mpl_list
{
  struct mpl_list *next_p;
};
typedef struct mpl_list mpl_list_t;


/*******************************************************************************
 *
 * Functions and macros
 *
 *******************************************************************************/

/**
 * mpl_list_add
 * 
 * Description: Add entry to list.
 *
 * Parameters:
 *     list_pp:   address of list pointer
 *     new_p:     pointer to list structure to be added (at the beginning)
 *
 * Return Values : -
 *
 */
void mpl_list_add( mpl_list_t **list_pp, mpl_list_t * new_p );


/**
 * mpl_list_remove
 * 
 * Description: Remove entry from list.
 *
 * Parameters:
 *     list_pp:   address of list pointer
 *     entry_p:   pointer to list structure to be removed (NULL means remove first entry)
 *
 * Return Values : The list structure that was removed
 *
 */
mpl_list_t *mpl_list_remove( mpl_list_t **list_pp, mpl_list_t *entry_p );


/**
 * MPL_LIST_CONTAINER
 * 
 * Description: Return pointer to container (node) where list structure is member.
 *
 * Parameters:
 *     list_p:    pointer to list struct inside the container
 *     type:      data type of the container (a struct)
 *     member:    name of the list member of the container
 *
 * Return Values : Pointer to container
 *
 */
#define MPL_LIST_CONTAINER(list_p,type,member) ((type*)(((char*)list_p) - offsetof(type,member))/*lint -e826 suspicisous pointer, but ok*/)


/**
 * MPL_LIST_FOR_EACH
 * 
 * Description: Loop through list (use this as loop head, a la while)
 *
 * Parameters:
 *     list_p:    pointer to list
 *     tmp_p:     pointer that will point to each entry in the list while looping
 *
 */
#define MPL_LIST_FOR_EACH(list_p,tmp_p) for((tmp_p) = (list_p);(tmp_p)!=NULL;(tmp_p)=(tmp_p)->next_p)


/**
 * MPL_LIST_FOR_EACH_SAFE
 * 
 * Description: Loop through list (use this as loop head, a la while)
 *              This loop allows the current object to be deleted in the loop block.
 *
 * Parameters:
 *     list_p:    pointer to list (type mpl_list_p *)
 *     tmp_p:     pointer that will point to each entry in the list while looping (type mpl_list_p *)
 *     tmp2_p:    pointer that is used internally in the macro (type mpl_list_p *)
 *
 */
#define MPL_LIST_FOR_EACH_SAFE(list_p,tmp_p,tmp2_p) for((tmp_p) = (list_p),(tmp2_p) = (list_p) ? (list_p)->next_p : NULL;(tmp_p)!=NULL;(tmp_p) = (tmp2_p), (tmp2_p) = (tmp2_p) ? (tmp2_p)->next_p : NULL)


/**
 * mpl_list_len
 * 
 * Description: Return length of list.
 *
 * Parameters:
 *     list_p: pointer to list
 *
 * Return Values : Number of elements in list.
 *
 */
size_t mpl_list_len(const mpl_list_t *list_p);

/**
 * mpl_is_on_list
 * 
 * Description: Return TRUE if entry_p is member of list list_p.
 *
 * Parameters:
 *     list_p:   pointer to list
 *     entry_p:  pointer to list entry
 *
 * Return Values : true if the entry is member of the list
 *
 */
bool mpl_is_on_list(const mpl_list_t *list_p, const mpl_list_t *entry_p);


#ifdef  __cplusplus
}
#endif
#endif
