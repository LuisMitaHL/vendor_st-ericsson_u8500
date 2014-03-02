/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 * 
 * \brief  PS Connection Control object
 */

#ifndef PSCC_OBJECT_H
#define PSCC_OBJECT_H

/********************************************************************************
 *
 * Include files
 *
 ********************************************************************************/
#include "pscc_bearer.h"
#include "pscc_msg.h"
#include "mpl_list.h"

/********************************************************************************
 *
 * Defines & Type definitions
 *
 ********************************************************************************/
typedef struct pscc_object
{
  int                          connid;
  pscc_connection_status_t     state;
  mpl_list_t                  *param_list_p;
  int                          bearer_handle;
  int                          sim_handle;
  mpl_list_t                   list_entry;
  int                          netdev_handle;
} pscc_object_t;

extern const char* pscc_names_connection_status[];


/*******************************************************************************
 *
 * Functions and macros
 *
 *******************************************************************************/

/**
 * pscc_obj_add - add ps connection control object
 *
 * @connid   connection id
 *
 * Returns: pointer to object on success, (NULL) on error
 *
 **/
pscc_object_t *pscc_obj_add(int connid);

/**
 * pscc_obj_delete - delete ps connection control object
 *
 * @connid   connection id
 *
 * Returns: (0) on success, (-1) on error
 *
 **/
int pscc_obj_delete(int connid);

/**
 * pscc_obj_get - get ps connection control object
 *
 * @connid   connection id
 *
 * Returns: Pointer to  pscc object, NULL if not found or error
 *
 **/
pscc_object_t *pscc_obj_get(int connid);

/**
 * pscc_obj_get_all_connid - get all ps connections' ID
 *
 * @list_pp     a list pointer contains all connection ids
 *
 * Returns: (0) on success, (-1) on error
 *
 **/
int pscc_obj_get_all_connid(mpl_list_t** list_pp);

/**
 * pscc_obj_get_connid - read connection id from ps object
 *
 * @self      pointer to obj
 *
 * Returns: connection id 
 *
 **/
#define pscc_obj_get_connid(self) (self)->connid

/**
 * pscc_obj_get_netdev_handle - read netdev handler from ps object
 *
 * @self      pointer to obj
 *
 * Returns: net device handler 
 *
 **/
#define pscc_obj_get_netdev_handle(self) (self)->netdev_handle

/**
 * pscc_obj_get_state - read state id from ps object
 *
 * @self      pointer to obj
 *
 * Returns: state
 *
 **/
#define pscc_obj_get_state(self) (self)->state

/**
 * pscc_obj_set_state - set state of a ps object
 *
 * @self      pointer to obj
 *
 * Returns: state
 *
 **/
#define pscc_obj_set_state(self,newstate)                           \
  do                                                                \
  {                                                                 \
    assert((newstate) < pscc_number_of_connection_status);          \
    assert((self)->state < pscc_number_of_connection_status);       \
    assert((self) != NULL);                                         \
    PSCC_DBG_TRACE(LOG_INFO, "state change: %s->%s\n",              \
                   pscc_names_connection_status[(self)->state],     \
                   pscc_names_connection_status[(newstate)]);       \
    (self)->state = newstate;                                       \
    pscc_runscript_call(self);                                      \
  } while(0)


/**
 * pscc_obj_set_bearer_handler - store bearer handler in object
 *
 * @self      pointer to obj
 * @handle    bearer handle
 *
 * Returns: -
 *
 **/
#define pscc_obj_set_bearer_handle(self,handle) (self)->bearer_handle = (handle)

/**
 * pscc_obj_set_sim_handle - store sim handler in object
 *
 * @self      pointer to obj
 * @handle    sim handle
 *
 * Returns: -
 *
 **/
#define pscc_obj_set_sim_handle(self,handle) (self)->sim_handle = (handle)
/**
 * pscc_obj_get_bearer_handle - read bearer handle from object
 *
 * @self  pointer to obj
 *
 * Returns: pscc bearer handler
 *
 **/
#define pscc_obj_get_bearer_handle(self) (self)->bearer_handle

/**
 * pscc_obj_get_sim_handle - read sim handle from object
 *
 * @self  pointer to obj
 *
 * Returns: pscc sim handler
 *
 **/
#define pscc_obj_get_sim_handle(self) (self)->sim_handle

/**
 * pscc_obj_get_param_list_ref - fetch param list ference from ps object
 *
 * @self  pointer to obj
 *
 * Returns: Pointer to parameter list, NULL if not found or error
 *
 **/
#define pscc_obj_get_param_list_ref(self) &(self)->param_list_p

/**
 * pscc_obj_set_netdev_handle - set netdev handle in object
 *
 * @self      pointer to obj
 * @handler   net device handle
 *
 *
 **/
#define pscc_obj_set_netdev_handle(self,handle) (self)->netdev_handle = (handle)

#endif
