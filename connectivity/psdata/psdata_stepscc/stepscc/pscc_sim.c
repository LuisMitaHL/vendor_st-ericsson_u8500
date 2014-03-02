/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \brief  PS Sim implementation for MAL
 */

/********************************************************************************
 *
 * Include files
 *
 ********************************************************************************/

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "psccd.h"
#include "pscc_sim.h"
#include "sim.h"
/********************************************************************************
 *
 * Defines & Type definitions
 *
 ********************************************************************************/

typedef struct {
  int fd;
  ste_sim_t *ste_sim_p;
  ste_sim_closure_t ste_sim_closure;
} pscc_sim_client_t;

typedef struct pscc_sim {
  int parent;
  int handle;
  pscc_get_param_fp_t get_param_cb;
  pscc_set_param_fp_t set_param_cb;
  pscc_delete_param_fp_t delete_param_cb;
  mpl_list_t list_entry;
} pscc_sim_t;

static mpl_list_t *pscc_sim_list_p = NULL;
pscc_sim_client_t* pscc_sim_client_p = NULL;
int init_handle = 0;
/* store inside this class since this is general setting for pscc sim */
static pscc_cc_gprs_availability_t pscc_cc_gprs_availability = pscc_cc_gprs_availability_unknown;
static pscc_sim_event_fp_t sim_event_cb;

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

static void pscc_sim_callback(int cause, uintptr_t client_tag, void *data,
    void *user_data);
static pscc_sim_t *find_instance_by_handle(int handle);

/*******************************************************************************
 *
 * Public Functions
 *
 *******************************************************************************/

/**
 * pscc_sim_init()
 **/
int pscc_sim_init(void) {
  int result = 0;

  pscc_sim_client_p = (pscc_sim_client_t*) calloc(1, sizeof(pscc_sim_client_t));

  if (!pscc_sim_client_p) {
    PSCC_DBG_TRACE(LOG_ERR, "failed to allocate sim client\n");
    goto error_return;
  }

  pscc_sim_client_p->fd = -1;
  pscc_sim_client_p->ste_sim_closure.func = pscc_sim_callback;
  pscc_sim_client_p->ste_sim_closure.user_data = pscc_sim_client_p;

  pscc_sim_client_p->ste_sim_p = ste_sim_new_st(
      &(pscc_sim_client_p->ste_sim_closure));
  if (!pscc_sim_client_p->ste_sim_p) {
    PSCC_DBG_TRACE(LOG_ERR, "ste_sim_new_st failed\n");
    goto error_new_st;
  }

  result = ste_sim_connect(pscc_sim_client_p->ste_sim_p, init_handle);
  if (STE_SIM_SUCCESS != result) {
    PSCC_DBG_TRACE(LOG_ERR, "ste_sim_connect failed\n");
    goto error_connect;
  }

  pscc_sim_client_p->fd = ste_sim_fd(pscc_sim_client_p->ste_sim_p);
  if (pscc_sim_client_p->fd < 0) {
    PSCC_DBG_TRACE(LOG_ERR, "ste_sim_fd failed\n");
    goto error_sim_fd;
  }

  PSCC_DBG_TRACE(LOG_DEBUG, "pscc_sim_init successful, sim handle: %d\n", init_handle);
  return pscc_sim_client_p->fd;

error_sim_fd:
  result = ste_sim_disconnect(pscc_sim_client_p->ste_sim_p, init_handle);

  if (result < 0) {
    PSCC_DBG_TRACE(LOG_ERR, "ste_sim_disconnect failed\n");
  }

error_connect:
  (void) ste_sim_delete(pscc_sim_client_p->ste_sim_p,  init_handle);

error_new_st:
  free(pscc_sim_client_p);

error_return:
  return -1;
}

/**
 * pscc_sim_deinit()
 **/
void pscc_sim_deinit(void) {
  int result;

  PSCC_DBG_TRACE(LOG_DEBUG, "pscc_sim_deinit\n");

  if (NULL != pscc_sim_client_p) {
    result = ste_sim_disconnect(pscc_sim_client_p->ste_sim_p, init_handle);
    if (result < 0) {
      PSCC_DBG_TRACE(LOG_ERR, "ste_sim_disconnect failed\n");
      free(pscc_sim_client_p);
      pscc_sim_client_p = NULL;
      return;
    }

    (void) ste_sim_delete(pscc_sim_client_p->ste_sim_p, init_handle);

    free(pscc_sim_client_p);
    pscc_sim_client_p = NULL;
  }
}

/**
 * pscc_sim_alloc()
 **/
int pscc_sim_alloc(int connid) {
  static int pscc_sim_handle = 0;
  pscc_sim_t* obj_p;
  PSCC_DBG_TRACE(LOG_DEBUG, "pscc_sim_alloc()\n");

  obj_p = calloc(1, sizeof(pscc_sim_t));

  if (NULL == obj_p) {
    PSCC_DBG_TRACE(LOG_ERR, "failed allocating memory\n");
    return (-1);
  }

  obj_p->handle = ++pscc_sim_handle;
  obj_p->parent = connid;

  mpl_list_add(&pscc_sim_list_p, &obj_p->list_entry);
  PSCC_DBG_TRACE(LOG_DEBUG, "sim object allocated (handle=%d)\n",obj_p->handle);

  return (obj_p->handle);
}

/**
 * pscc_sim_free()
 **/
int pscc_sim_free(int handle) {
  pscc_sim_t *obj_p;
  PSCC_DBG_TRACE(LOG_DEBUG, "pscc_sim_free() (handle=%d)\n", handle);

  obj_p = find_instance_by_handle(handle);
  if (NULL == obj_p) {
    PSCC_DBG_TRACE(LOG_ERR, "invalid sim obj specified\n");
    return (-1);
  }

  mpl_list_remove(&pscc_sim_list_p, &obj_p->list_entry);
  free(obj_p);
  PSCC_DBG_TRACE(LOG_DEBUG, "sim object freed\n");
  return (0);
}

/**
 * pscc_sim_handler()
 **/
int pscc_sim_handler(int fd)
{
  if (NULL == pscc_sim_client_p)
  {
    PSCC_DBG_TRACE(LOG_ERR, "pscc_sim_client_p == NULL, pscc sim not initiated?\n");
    return (-1);
  }
  if (fd != pscc_sim_client_p->fd)
  {
    PSCC_DBG_TRACE(LOG_ERR, "fd received != pscc_sim_client_p->fd\n");
    return (-1);
  }

  return ste_sim_read(pscc_sim_client_p->ste_sim_p);
}

/**
 * pscc_sim_cc_gprs_availability()
 **/
int pscc_sim_cc_gprs_availability(pscc_cc_gprs_availability_t* pscc_cc_gprs_availability_p)
{
  uicc_request_status_t result;
  if (!pscc_sim_client_p || !pscc_sim_client_p->ste_sim_p) {
    PSCC_DBG_TRACE(LOG_ERR, "PSCC Sim client not initialized!\n");
    return -1;
  }

  if (pscc_cc_gprs_availability_p == NULL) {
    PSCC_DBG_TRACE(LOG_ERR, "pscc_cc_gprs_availability_p was NULL!\n");
    return -1;
  }

  *pscc_cc_gprs_availability_p = pscc_cc_gprs_availability;

  /* only request availability check if status is unknown */
  if (pscc_cc_gprs_availability_unknown == pscc_cc_gprs_availability) {
    result = ste_uicc_get_service_availability(pscc_sim_client_p->ste_sim_p,
        (uintptr_t) init_handle, SIM_SERVICE_TYPE_CALL_CONTROL_ON_GPRS);

    if ((UICC_REQUEST_STATUS_PENDING != result) && (UICC_REQUEST_STATUS_OK
        != result)) {
      PSCC_DBG_TRACE(LOG_ERR, "SIM call ste_uicc_get_service_availability failed!\n");
      return -1;
    }
  }

  return 0;
}

/**
 * pscc_sim_perform_cc()
 **/
int pscc_sim_perform_cc(pscc_rc_t *pscc_rc_p, pscc_sim_event_fp_t event_cb)
{
  uicc_request_status_t result;
  ste_cat_call_control_t cc_config;
  ste_cat_cc_pdp_t ste_cat_cc_pdp;

  assert(pscc_rc_p != NULL);
  assert(event_cb != NULL);

  PSCC_DBG_TRACE(LOG_DEBUG, "inside pscc_sim_perform_cc, seq_id:%d\n", pscc_rc_p->seq_id);

  if (!pscc_sim_client_p || !pscc_sim_client_p->ste_sim_p) {
    PSCC_DBG_TRACE(LOG_ERR, "PSCC Sim client not initialized!\n");
    return -1;
  }

  /* store callback */
  sim_event_cb = event_cb;

  /* perform call control */
  cc_config.cc_data.pdp_p = &ste_cat_cc_pdp;

  /* set rc parameters */
  ste_cat_cc_pdp.pdp_context.str_p = pscc_rc_p->data_p;
  ste_cat_cc_pdp.pdp_context.no_of_bytes = pscc_rc_p->length;
  ste_cat_cc_pdp.pdp_context.text_coding = STE_SIM_CODING_UNKNOWN;
  cc_config.cc_type = STE_CAT_CC_PDP;

  /* use sequence id as clienttag */
  result = ste_cat_call_control(pscc_sim_client_p->ste_sim_p,
      pscc_rc_p->seq_id, &cc_config);

  if ((UICC_REQUEST_STATUS_PENDING != result) && (UICC_REQUEST_STATUS_OK
      != result)) {
    PSCC_DBG_TRACE(LOG_ERR, "SIM call ste_cat_call_control failed!\n");
    return -1;
  }

  return 0;
}

/*******************************************************************************
 *
 * Private Functions
 *
 *******************************************************************************/

void pscc_sim_callback(int cause, uintptr_t client_tag, void *data,
    void *user_data) {

  PSCC_IDENTIFIER_NOT_USED(user_data);

  switch (cause) {
  case STE_SIM_CAUSE_CONNECT:
    PSCC_DBG_TRACE(LOG_INFO, "STE_SIM_CAUSE_CONNECT\n");
  break;

  case STE_SIM_CAUSE_DISCONNECT:
    PSCC_DBG_TRACE(LOG_INFO, "STE_SIM_CAUSE_DISCONNECT\n");
  break;

  case STE_UICC_CAUSE_SIM_STATE_CHANGED:
  {
    ste_uicc_sim_state_changed_t *state_change;
    PSCC_DBG_TRACE(LOG_INFO, "STE_UICC_CAUSE_SIM_STATE_CHANGED\n");
    if (NULL == data)
    {
      PSCC_DBG_TRACE(LOG_DEBUG, "data was NULL\n");
      return;
    }

    if (!pscc_sim_client_p || !pscc_sim_client_p->ste_sim_p) {
      PSCC_DBG_TRACE(LOG_ERR, "PSCC Sim client not initialized!\n");
      return;
    }

    state_change = (ste_uicc_sim_state_changed_t *) data;

    /* when sim becomes ready we perform a service availability check */
    if (state_change && state_change->state == SIM_STATE_READY)
    {
      uicc_request_status_t result;
      PSCC_DBG_TRACE(LOG_INFO, "SIM_STATE_READY\n");
      result = ste_uicc_get_service_availability(pscc_sim_client_p->ste_sim_p,
          (uintptr_t) init_handle, SIM_SERVICE_TYPE_CALL_CONTROL_ON_GPRS);

      if ((UICC_REQUEST_STATUS_PENDING != result) && (UICC_REQUEST_STATUS_OK
          != result)) {
        PSCC_DBG_TRACE(LOG_ERR, "SIM call ste_uicc_get_service_availability failed!\n");
        return;
      }
    }
  }
  break;

  case STE_UICC_CAUSE_REQ_GET_SERVICE_AVAILABILITY:
  {
    ste_uicc_get_service_availability_response_t *response_p;
    PSCC_DBG_TRACE(LOG_INFO, "STE_UICC_CAUSE_REQ_GET_SERVICE_AVAILABILITY\n");
    if (NULL == data)
    {
      PSCC_DBG_TRACE(LOG_DEBUG, "data was NULL\n");
      return;
    }
    response_p = (ste_uicc_get_service_availability_response_t *) data;

    if (STE_UICC_STATUS_CODE_OK != response_p->uicc_status_code) {
      PSCC_DBG_TRACE(LOG_ERR, "STE_UICC_CAUSE_REQ_GET_SERVICE_AVAILABILITY returned with error code: %d\n", response_p->uicc_status_code);
      /* set availability as unavailable */
      pscc_cc_gprs_availability = pscc_cc_gprs_availability_unavailable;
    }
    else {
      /* update availability */
      pscc_cc_gprs_availability = (STE_UICC_SERVICE_AVAILABLE == response_p->service_availability ? pscc_cc_gprs_availability_available
              : pscc_cc_gprs_availability_unavailable);
    }

    PSCC_DBG_TRACE(LOG_DEBUG, "cc_gprs_availability: %d\n",pscc_cc_gprs_availability);
  }
  break;

  case STE_CAT_CAUSE_SIM_EC_CALL_CONTROL:
  {
    pscc_rc_t pscc_rc;
    ste_cat_call_control_response_t *response_p;

    PSCC_DBG_TRACE(LOG_DEBUG, "STE_CAT_CAUSE_SIM_EC_CALL_CONTROL\n");

    if (NULL == data)
    {
      PSCC_DBG_TRACE(LOG_DEBUG, "data was NULL\n");
      return;
    }

    response_p = (ste_cat_call_control_response_t *) data;

    memset(&pscc_rc, 0, sizeof(pscc_rc_t));
    /* we used clienttag as sequence id, for accept and reject we only need this */
    pscc_rc.seq_id = client_tag;

    switch (response_p->cc_result) {
    case STE_CAT_CC_ALLOWED_NO_MODIFICATION:
      PSCC_DBG_TRACE(LOG_INFO, "STE_CAT_CC_ALLOWED_NO_MODIFICATION seq_id:%d\n", pscc_rc.seq_id);
      sim_event_cb(pscc_sim_event_cc_accept, &pscc_rc);
      break;
    case STE_CAT_CC_NOT_ALLOWED:
      PSCC_DBG_TRACE(LOG_INFO, "STE_CAT_CC_NOT_ALLOWED seq_id:%d\n", pscc_rc.seq_id);
      sim_event_cb(pscc_sim_event_cc_reject, &pscc_rc);
      break;
    case STE_CAT_CC_ALLOWED_WITH_MODIFICATIONS:
    {
      ste_cat_cc_pdp_t * ste_cat_cc_pdp_p;
      PSCC_DBG_TRACE(LOG_INFO, "STE_CAT_CC_ALLOWED_WITH_MODIFICATIONS seq_id:%d\n", pscc_rc.seq_id);
      ste_cat_cc_pdp_p = response_p->cc_info.cc_data.pdp_p;
      /* we get new call control message */
      pscc_rc.data_p = ste_cat_cc_pdp_p->pdp_context.str_p;
      pscc_rc.length = ste_cat_cc_pdp_p->pdp_context.no_of_bytes;
      sim_event_cb(pscc_sim_event_cc_modified, &pscc_rc);
    }
      break;
    default:
      PSCC_DBG_TRACE(LOG_ERR, "unknown result code: %d", response_p->cc_result);
      break;
    }
  }
    break;

  default:
    /* ignore any other events */
    break;
  }

  return;

}

/**
 * find_instance_by_handle()
 **/
static pscc_sim_t *find_instance_by_handle(int handle) {
  mpl_list_t *obj_p;
  pscc_sim_t *sim_p;

  MPL_LIST_FOR_EACH(pscc_sim_list_p, obj_p) {
    sim_p = MPL_LIST_CONTAINER(obj_p, pscc_sim_t, list_entry);
    if ((NULL != sim_p) && (sim_p->handle == handle))
      return (sim_p);
  }
  return NULL;
}
