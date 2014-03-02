/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 * 
 * \brief  Single linked list implementation
 */

/********************************************************************************
 *
 * Include files
 *
 ********************************************************************************/
#include <stdlib.h>
#include <assert.h>
#include "mpl_list.h"

/********************************************************************************
 *
 * Defines & Type definitions
 *
 ********************************************************************************/
#define DBG_ASSERT(EXPR) assert(EXPR)

/********************************************************************************
 *
 * Local variables
 *
 ********************************************************************************/

/********************************************************************************
 *
 * Private function prototypes
 *
 ********************************************************************************/

/*******************************************************************************
 *
 * Public Functions
 *
 *******************************************************************************/

/**
 * mpl_list_add
 */
void mpl_list_add( mpl_list_t **list_pp, mpl_list_t *entry_p )
{
  DBG_ASSERT( entry_p != NULL );
  
  entry_p->next_p = *list_pp;
  *list_pp = entry_p;
}

/**
 * mpl_list_remove
 */
mpl_list_t *mpl_list_remove( mpl_list_t **list_pp, mpl_list_t *entry_p )
{
  if (entry_p == NULL)
  {
    /* Remove first */
    entry_p = *list_pp;
  }

  while (*list_pp)
  {
    if (*list_pp == entry_p)
    {
      *list_pp = entry_p->next_p;
      entry_p->next_p = NULL;
      return entry_p;
    }
    list_pp = &(*list_pp)->next_p;
  }
  
  return NULL;
}
                                                


/**
 * mpl_list_len
 */
size_t mpl_list_len(const mpl_list_t *list_p)
{
  const mpl_list_t *tmp_p = list_p;
  size_t len = 0;
  
  MPL_LIST_FOR_EACH(list_p,tmp_p)
  {
    len++;
  }
  return len;
}


/**
 * mpl_is_on_list
 */
//lint -e818 Ignore lint's const recomandations
bool mpl_is_on_list(const mpl_list_t *list_p, const mpl_list_t *entry_p)
{
  const mpl_list_t *tmp_p = list_p;

  if (entry_p == NULL)
    return false;
  
  MPL_LIST_FOR_EACH(list_p,tmp_p)
  {
    if (tmp_p == entry_p)
      return true;
  }
  return false;
}


