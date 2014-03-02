/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \brief  PS Connection Control object, implements a single linked list to store PS
 *      connection control objects
 */

/********************************************************************************
 *
 * Include files
 *
 ********************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>
#include "pscc_object.h"
#include "psccd.h"
#include "mpl_list.h"
#include "pscc_msg.h"

/********************************************************************************
 *
 * Defines & Type definitions
 *
 ********************************************************************************/

/********************************************************************************
 *
 * Global variables
 *
 ********************************************************************************/


/********************************************************************************
 *
 * Local variables
 *
 ********************************************************************************/

static mpl_list_t *objects_list = NULL;

/*******************************************************************************
 *
 * Private Functions
 *
 *******************************************************************************/


/*******************************************************************************
 *
 * Public Functions
 *
 *******************************************************************************/

/**
 * pscc_obj_add - add ps connection control object
 **/
pscc_object_t *pscc_obj_add(int connid)
{
  pscc_object_t *temp;

  temp = malloc(sizeof(pscc_object_t));
  if(temp==NULL)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed allocating memory\n");
    return (NULL);
  }
  
  temp->connid = connid;
  temp->state = pscc_connection_status_disconnected;
  temp->param_list_p = NULL;
  mpl_list_add(&objects_list,&temp->list_entry);

  return (temp);
}


/**
 * pscc_obj_delete - delete ps connection control object
 **/
int pscc_obj_delete(int connid)
{
  pscc_object_t* cur;
  cur = pscc_obj_get(connid);
  if (cur==NULL)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed finding object for connid=%d\n", connid);    
    return (-1);
  }
  
  mpl_list_remove(&objects_list,&cur->list_entry);
  mpl_param_list_destroy(&cur->param_list_p);
  free(cur);
  return (0);
}


/**
 * pscc_obj_get - get ps connection control object
 **/
pscc_object_t *pscc_obj_get(int connid)
{
  mpl_list_t *obj_p;
  pscc_object_t *res;

  MPL_LIST_FOR_EACH(objects_list, obj_p)
  {
    res = MPL_LIST_CONTAINER(obj_p, pscc_object_t, list_entry);
    if (res->connid == connid)
      return res;
  }
  PSCC_DBG_TRACE(LOG_DEBUG, "pscc object with conn id %d not found\n",connid);
  return NULL;
}

/**
 * pscc_obj_get_all_connid - get all ps connections' id
 **/
int pscc_obj_get_all_connid(mpl_list_t** list_pp)
{
  mpl_list_t *tmp_list_p = NULL;
  mpl_param_element_t * elem_p = NULL;
  pscc_object_t *tmp = NULL;
  MPL_LIST_FOR_EACH(objects_list,tmp_list_p)
  {
    tmp = MPL_LIST_CONTAINER(tmp_list_p, pscc_object_t, list_entry);
    if (NULL == tmp)
    {
      PSCC_DBG_TRACE(LOG_DEBUG, "Can't fetch the container when get all connid\n");
      mpl_param_list_destroy(list_pp);
      return (-1);
    }
    elem_p = mpl_param_element_create(pscc_paramid_connid, &tmp->connid);
    if (elem_p == NULL)
    {
      PSCC_DBG_TRACE(LOG_DEBUG, "Can't create param elem when get all connid\n");
      mpl_param_list_destroy(list_pp);
      return (-1);
    }
    mpl_list_add(list_pp,&(elem_p->list_entry));
  }
  return (0);
}
