/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \brief  PS Bearer implementation for MAL
 */

/********************************************************************************
 *
 * Include files
 *
 ********************************************************************************/

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "pscc_bearer.h"
#include "pscc_netdev.h"
#include "pscc_utils.h"
#include "psccd.h"
#include "pscc_qpc_mal.h"
#include "mal_gpds.h"
#include "shm_netlnk.h"

/********************************************************************************
 *
 * Defines & Type definitions
 *
 ********************************************************************************/
#ifndef MAX_UINT8
#define MAX_UINT8 255
#endif
#define PSCC_BEARER_MAL_STATES                     \
  PSCC_BEARER_MAL_STATE_ELEMENT(idle)              \
  PSCC_BEARER_MAL_STATE_ELEMENT(activating)        \
  PSCC_BEARER_MAL_STATE_ELEMENT(activated)         \
  PSCC_BEARER_MAL_STATE_ELEMENT(deactivating)

#define maximum_uplink_speed ((uint16_t)8640)
#define maximum_downlink_speed ((uint16_t)16000)
/* hardcoded value approx the size of an ip packet */
#define maximum_uplink_packet_size ((uint32_t)1500)

#define PSCC_BEARER_MAL_STATE_ELEMENT(STATE) pscc_bearer_mal_##STATE,
typedef enum {
  PSCC_BEARER_MAL_STATES
pscc_bearer_mal_number_of_states} pscc_bearer_mal_state_t;
#undef PSCC_BEARER_MAL_STATE_ELEMENT

#define PSCC_BEARER_MAL_STATE_ELEMENT(STATE) #STATE,
static const char* pscc_bearer_mal_state_names[] = { PSCC_BEARER_MAL_STATES};
#undef PSCC_BEARER_MAL_STATE_ELEMENT

#define pscc_bearer_change_state(obj_p,new_state)                     \
  do                                                                  \
  {                                                                   \
    assert((new_state) < pscc_bearer_mal_number_of_states);           \
    assert((obj_p) != NULL);                                          \
    assert((obj_p)->state < pscc_bearer_mal_number_of_states);        \
    PSCC_DBG_TRACE(LOG_DEBUG, "state change: %s->%s\n",               \
                   pscc_bearer_mal_state_names[(obj_p)->state],       \
                   pscc_bearer_mal_state_names[(new_state)]);         \
    (obj_p)->state = new_state;                                       \
  } while(0)

typedef struct pscc_bearer {
  int parent;
  int handle;
  uint8_t mal_cid;
  pscc_bearer_mal_state_t state;
  int mal_pdp_type;
  pscc_get_param_fp_t get_param_cb;
  pscc_set_param_fp_t set_param_cb;
  pscc_delete_param_fp_t delete_param_cb;
  pscc_bearer_event_fp_t event_cb;
  pscc_reason_t reason;
  int cause;
  uint32_t tx_data_count_offset_lo;
  uint32_t tx_data_count_offset_hi;
  uint32_t rx_data_count_offset_lo;
  uint32_t rx_data_count_offset_hi;
  pscc_data_counter_state_t data_counter_state;
  mpl_list_t list_entry;
} pscc_bearer_t;

static mpl_list_t *pscc_bearer_list_p = NULL;

#define PSCC_COMPILE_CHECK_ALL_REL97_QOS_VALUES()        \
PSCC_COMPILE_TIME_ASSERT(MAL_GPDS_QOS_PRECEDENCE_CLASS_0 == pscc_qos_PRECEDENCE_CLASS_UNSPECIFIED); \
PSCC_COMPILE_TIME_ASSERT(MAL_GPDS_QOS_PRECEDENCE_CLASS_1 == pscc_qos_PRECEDENCE_CLASS_HIGH); \
PSCC_COMPILE_TIME_ASSERT(MAL_GPDS_QOS_PRECEDENCE_CLASS_2 == pscc_qos_PRECEDENCE_CLASS_NORMAL); \
PSCC_COMPILE_TIME_ASSERT(MAL_GPDS_QOS_PRECEDENCE_CLASS_3 == pscc_qos_PRECEDENCE_CLASS_LOW); \
PSCC_COMPILE_TIME_ASSERT(MAL_GPDS_QOS_DELAY_CLASS_0 == pscc_qos_DELAY_CLASS_UNSPECIFIED); \
PSCC_COMPILE_TIME_ASSERT(MAL_GPDS_QOS_DELAY_CLASS_1 == pscc_qos_DELAY_CLASS_LEVEL_1); \
PSCC_COMPILE_TIME_ASSERT(MAL_GPDS_QOS_DELAY_CLASS_2 == pscc_qos_DELAY_CLASS_LEVEL_2); \
PSCC_COMPILE_TIME_ASSERT(MAL_GPDS_QOS_DELAY_CLASS_3 == pscc_qos_DELAY_CLASS_LEVEL_3); \
PSCC_COMPILE_TIME_ASSERT(MAL_GPDS_QOS_DELAY_CLASS_4 == pscc_qos_DELAY_CLASS_LEVEL_4); \
PSCC_COMPILE_TIME_ASSERT(MAL_GPDS_QOS_RELIABILITY_CLASS_0 == pscc_qos_RELIABILITY_CLASS_UNSPECIFIED); \
PSCC_COMPILE_TIME_ASSERT(MAL_GPDS_QOS_RELIABILITY_CLASS_1 == pscc_qos_RELIABILITY_CLASS_LEVEL_1); \
PSCC_COMPILE_TIME_ASSERT(MAL_GPDS_QOS_RELIABILITY_CLASS_2 == pscc_qos_RELIABILITY_CLASS_LEVEL_2); \
PSCC_COMPILE_TIME_ASSERT(MAL_GPDS_QOS_RELIABILITY_CLASS_3 == pscc_qos_RELIABILITY_CLASS_LEVEL_3); \
PSCC_COMPILE_TIME_ASSERT(MAL_GPDS_QOS_RELIABILITY_CLASS_4 == pscc_qos_RELIABILITY_CLASS_LEVEL_4); \
PSCC_COMPILE_TIME_ASSERT(MAL_GPDS_QOS_RELIABILITY_CLASS_5 == pscc_qos_RELIABILITY_CLASS_LEVEL_5); \
PSCC_COMPILE_TIME_ASSERT(MAL_GPDS_QOS_PEAK_THROUGHPUT_CLASS_SUBSCRIBED == pscc_qos_PEAK_THROUGHPUT_CLASS_UNSPECIFIED); \
PSCC_COMPILE_TIME_ASSERT(MAL_GPDS_QOS_PEAK_THROUGHPUT_CLASS_8000 == pscc_qos_PEAK_THROUGHPUT_CLASS_LEVEL_1); \
PSCC_COMPILE_TIME_ASSERT(MAL_GPDS_QOS_PEAK_THROUGHPUT_CLASS_16000 == pscc_qos_PEAK_THROUGHPUT_CLASS_LEVEL_2); \
PSCC_COMPILE_TIME_ASSERT(MAL_GPDS_QOS_PEAK_THROUGHPUT_CLASS_32000 == pscc_qos_PEAK_THROUGHPUT_CLASS_LEVEL_4); \
PSCC_COMPILE_TIME_ASSERT(MAL_GPDS_QOS_PEAK_THROUGHPUT_CLASS_64000 == pscc_qos_PEAK_THROUGHPUT_CLASS_LEVEL_8); \
PSCC_COMPILE_TIME_ASSERT(MAL_GPDS_QOS_PEAK_THROUGHPUT_CLASS_128000 == pscc_qos_PEAK_THROUGHPUT_CLASS_LEVEL_16); \
PSCC_COMPILE_TIME_ASSERT(MAL_GPDS_QOS_PEAK_THROUGHPUT_CLASS_256000 == pscc_qos_PEAK_THROUGHPUT_CLASS_LEVEL_32); \
PSCC_COMPILE_TIME_ASSERT(MAL_GPDS_QOS_PEAK_THROUGHPUT_CLASS_512000 == pscc_qos_PEAK_THROUGHPUT_CLASS_LEVEL_64); \
PSCC_COMPILE_TIME_ASSERT(MAL_GPDS_QOS_PEAK_THROUGHPUT_CLASS_1024000 == pscc_qos_PEAK_THROUGHPUT_CLASS_LEVEL_128); \
PSCC_COMPILE_TIME_ASSERT(MAL_GPDS_QOS_PEAK_THROUGHPUT_CLASS_2048000 == pscc_qos_PEAK_THROUGHPUT_CLASS_LEVEL_256); \


#define PSCC_COMPILE_CHECK_ALL_REL99_QOS_VALUES()        \
PSCC_COMPILE_TIME_ASSERT(MAL_GPDS_QOS_TRAFFICCLASS_SUBSCRIBED == pscc_qos_TRAFFIC_CLASS_UNSPECIFIED); \
PSCC_COMPILE_TIME_ASSERT(MAL_GPDS_QOS_TRAFFICCLASS_CONVERSATIONAL == pscc_qos_TRAFFIC_CLASS_CONVERSATIONAL); \
PSCC_COMPILE_TIME_ASSERT(MAL_GPDS_QOS_TRAFFICCLASS_STREAMING == pscc_qos_TRAFFIC_CLASS_STREAMING); \
PSCC_COMPILE_TIME_ASSERT(MAL_GPDS_QOS_TRAFFICCLASS_INTERACTIVE == pscc_qos_TRAFFIC_CLASS_INTERACTIVE); \
PSCC_COMPILE_TIME_ASSERT(MAL_GPDS_QOS_TRAFFICCLASS_BACKGROUND == pscc_qos_TRAFFIC_CLASS_BACKGROUND); \
PSCC_COMPILE_TIME_ASSERT(MAL_GPDS_QOS_DELIVERY_ORDER_SUBSCRIBED == pscc_qos_DELIVERY_ORDER_UNSPECIFIED); \
PSCC_COMPILE_TIME_ASSERT(MAL_GPDS_QOS_DELIVERY_ORDER_YES == pscc_qos_DELIVERY_ORDER_YES); \
PSCC_COMPILE_TIME_ASSERT(MAL_GPDS_QOS_DELIVERY_ORDER_NO == pscc_qos_DELIVERY_ORDER_NO); \
PSCC_COMPILE_TIME_ASSERT(MAL_GPDS_QOS_DELIVERY_ERROR_SDU_SUBSCRIBED == pscc_qos_DELIVERY_ERRONEOUS_SDU_UNSPECIFIED); \
PSCC_COMPILE_TIME_ASSERT(MAL_GPDS_QOS_DELIVERY_ERROR_SDU_NO_DETECT == pscc_qos_DELIVERY_ERRONEOUS_SDU_NO_DETECT); \
PSCC_COMPILE_TIME_ASSERT(MAL_GPDS_QOS_DELIVERY_ERROR_SDU_YES == pscc_qos_DELIVERY_ERRONEOUS_SDU_YES); \
PSCC_COMPILE_TIME_ASSERT(MAL_GPDS_QOS_DELIVERY_ERROR_SDU_NO == pscc_qos_DELIVERY_ERRONEOUS_SDU_NO); \
PSCC_COMPILE_TIME_ASSERT(MAL_GPDS_QOS_TRAFFIC_HANDLING_PRIO_SUBSCRIBED == pscc_qos_TRAFFIC_HANDLING_PRIORITY_UNSPECIFIED); \
PSCC_COMPILE_TIME_ASSERT(MAL_GPDS_QOS_TRAFFIC_HANDLING_PRIO_1 == pscc_qos_TRAFFIC_HANDLING_PRIORITY_LEVEL_1); \
PSCC_COMPILE_TIME_ASSERT(MAL_GPDS_QOS_TRAFFIC_HANDLING_PRIO_2 == pscc_qos_TRAFFIC_HANDLING_PRIORITY_LEVEL_2); \
PSCC_COMPILE_TIME_ASSERT(MAL_GPDS_QOS_TRAFFIC_HANDLING_PRIO_3 == pscc_qos_TRAFFIC_HANDLING_PRIORITY_LEVEL_3); \
PSCC_COMPILE_TIME_ASSERT(MAL_GPDS_QOS_NOT_OPT_SGN == pscc_qos_SIGNALLING_INDICATION_NOT_OPTIMIZED_FOR_SIGNALLING_TRAFFIC); \
PSCC_COMPILE_TIME_ASSERT(MAL_GPDS_QOS_OPT_SGN == pscc_qos_SIGNALLING_INDICATION_OPTIMIZED_FOR_SIGNALLING_TRAFFIC); \
PSCC_COMPILE_TIME_ASSERT(MAL_GPDS_QOS_SRC_STAT_DESC_UNKNOWN == pscc_qos_SOURCE_STATISTICS_DESCRIPTOR_UNKNOWN); \
PSCC_COMPILE_TIME_ASSERT(MAL_GPDS_QOS_SRC_STAT_DESC_SPEECH == pscc_qos_SOURCE_STATISTICS_DESCRIPTOR_SPEECH); \


/********************************************************************************
 *
 * Local variables
 *
 ********************************************************************************/
static int pscc_mal_gpds_fd = -1;
static int pscc_mal_pipe_fd[2] = {-1,-1};
static int pscc_netlnk_fd = -1;
static bool reset_ind_set = false;
static bool pscc_mal_initialised = false;
static pscc_attach_status_t pscc_mal_attach_status = pscc_attach_status_detached;
static pscc_bearer_attach_event_fp_t pscc_bearer_attach_event_cb = NULL;
static pscc_bearer_ping_block_mode_event_fp_t pscc_bearer_ping_mode_event_cb = NULL;
static pscc_bearer_rc_event_fp_t pscc_bearer_rc_event_cb = NULL;
static uint32_t attach_request_ct = 0;
static bool attach_ct_set = false;
static void* config_source_func = NULL;

/* if handler wants to set aol mode we need to wait for its successful response */
static pscc_aol_conf_state_t pending_aol_mode = pscc_aol_conf_state_idle;
static mal_gpds_aol_context_mode_t expected_aol_mode = MAL_GPDS_AOL_CONTEXT_DEFAULT;
static mal_gpds_icmp_drop_mode_t expected_ping_block_mode = MAL_GPDS_ICMP_DROP_MODE_KEEP_EXISTING;
static mal_gpds_ps_attach_mode_t expected_attach_mode = MAL_GPDS_PS_ATTACH_DEFAULT;

/********************************************************************************
 *
 * Private function prototypes
 *
 ********************************************************************************/
static void reset_bearer(pscc_bearer_t *obj_p, bool send_event_deactivated);
static pscc_bearer_t* find_instance_by_handle(int handle);
static pscc_bearer_t *find_instance_by_mal_cid(uint8_t mal_cid);
static pscc_bearer_t *find_instance_by_connid(int connid);
static void mal_event_cb(uint8_t conn_id, mal_gpds_event_ids_t event_id, void *params);
static void set_apn(pscc_bearer_t *obj_p);
static void set_static_ip(pscc_bearer_t *obj_p);
static void set_hcmp(pscc_bearer_t *obj_p);
static void set_dcmp(pscc_bearer_t *obj_p);
static void set_pdptype(pscc_bearer_t *obj_p);
static void set_auth(pscc_bearer_t *obj_p);
static void set_qos(pscc_bearer_t *obj_p);
static pscc_qos_type_t get_qos_type(pscc_bearer_t *obj_p);
static void qos_mpl_to_mal(pscc_bearer_t *obj_p, mal_gpds_qos_profile_t* qos_p, bool req);
static void qos_mal_to_mpl(pscc_bearer_t *obj_p, mal_gpds_qos_profile_t* qos_p);
static void handle_event_activating(pscc_bearer_t *obj_p, void *params);
static void handle_event_activated(pscc_bearer_t *obj_p, void *params);
static void handle_event_activation_failed(pscc_bearer_t *obj_p, void *params);
static void handle_event_deactivated(pscc_bearer_t *obj_p, void *params);
static void handle_event_data_counter(pscc_bearer_t *obj_p, void *params);
static void handle_event_context_modified(pscc_bearer_t *obj_p, void *params);
static int handle_resource_control_event (mal_gpds_event_ids_t event_id, void *params);
static int handle_attach_event (mal_gpds_event_ids_t event_id, void *params);
static int deactivate_pdp(pscc_bearer_t *obj_p);
static uint8_t alloc_mal_connid(void);
static int convert_ip_to_string(ip_address_t ip, int pdp_type, char* ip_str);
static uint16_t return_least(uint16_t a, uint16_t b);
static void qos_dbg_print_mal_profile(mal_gpds_qos_profile_t* qos_p);

/*******************************************************************************
 *
 * Public Functions
 *
 *******************************************************************************/
/**
 * pscc_bearer_init()
 **/
int pscc_bearer_init(void)
{
  if (!pscc_mal_initialised) {
    if (mal_gpds_init(&pscc_mal_gpds_fd, &pscc_netlnk_fd, &pscc_mal_pipe_fd[0]) < 0) {
      PSCC_DBG_TRACE(LOG_ERR, "mal_gpds_init() failed\n");
      return (-1);
    }
    pscc_mal_initialised = true;

    /* register callback */
    mal_gpds_register_callback(mal_event_cb);
  }
  return (pscc_mal_gpds_fd);
}

/**
 * pscc_bearer_deinit()
 **/
void pscc_bearer_deinit(void)
{
  mpl_list_t *obj_p, *tmp_p;
  pscc_bearer_t *bearer_p;

  PSCC_DBG_TRACE(LOG_DEBUG, "pscc_bearer_deinit\n");

  /*  initiate deactivation of all active PDP contexts*/
  MPL_LIST_FOR_EACH_SAFE(pscc_bearer_list_p, obj_p, tmp_p) {
    bearer_p = MPL_LIST_CONTAINER(obj_p, pscc_bearer_t, list_entry);
    switch (bearer_p->state) {
    case pscc_bearer_mal_activating:
    case pscc_bearer_mal_activated:
      pscc_bearer_change_state(bearer_p,pscc_bearer_mal_deactivating);
      if(!reset_ind_set)
        (void) deactivate_pdp(bearer_p);
      else
        bearer_p->reason = pscc_reason_pdp_context_nw_deactivated_reset;
      /* fallthrough */
    case pscc_bearer_mal_deactivating:
      reset_bearer(bearer_p, true);
      (void) mal_gpds_request(bearer_p->mal_cid, MAL_GPDS_REQ_PSCONN_RELEASE, NULL);
      break;
    default:
      break;
    }
    (void) mpl_list_remove(&pscc_bearer_list_p, &bearer_p->list_entry);
    free(bearer_p);
  }

  /* close mal session */
  if (pscc_mal_initialised)
    (void) mal_gpds_deinit();
}

/**
 * pscc_bearer_alloc - allocate a bearer object
 **/
int pscc_bearer_alloc(int connid)
{
  pscc_bearer_t *obj_p = malloc(sizeof(pscc_bearer_t));
  static int pscc_bearer_handle = 0;

  if (NULL == obj_p) {
    PSCC_DBG_TRACE(LOG_ERR, "failed allocating memory\n");
    return (-1);
  }
  memset(obj_p, 0, sizeof(pscc_bearer_t));
  obj_p->handle = ++pscc_bearer_handle;
  obj_p->parent = connid;

  obj_p->mal_cid = alloc_mal_connid();
  if (obj_p->mal_cid == 0) {
    PSCC_DBG_TRACE(LOG_ERR, "to many mal connections\n");
    free(obj_p);
    return (-1);
  }

  /* create MAL PS connection */
  if (mal_gpds_request(obj_p->mal_cid, MAL_GPDS_REQ_PSCONN_CREATE, NULL) < 0) {
    PSCC_DBG_TRACE(LOG_ERR, "mal_gpds_request failed\n");
    free(obj_p);
    return (-1);
  }

  /* reset the data counter handling */
  obj_p->rx_data_count_offset_lo = 0;
  obj_p->rx_data_count_offset_hi = 0;
  obj_p->tx_data_count_offset_lo = 0;
  obj_p->tx_data_count_offset_hi = 0;
  obj_p->data_counter_state = pscc_data_counter_state_idle;

  pscc_bearer_change_state(obj_p,pscc_bearer_mal_idle);
  mpl_list_add(&pscc_bearer_list_p, &obj_p->list_entry);
  PSCC_DBG_TRACE(LOG_DEBUG, "bearer allocated (handle=%d, mal_cid=%d)\n",obj_p->handle,obj_p->mal_cid);
  return (obj_p->handle);
}

/**
 * pscc_bearer_free
 **/
int pscc_bearer_free(int handle)
{
  pscc_bearer_t *obj_p;
  PSCC_DBG_TRACE(LOG_DEBUG, "pscc_bearer_free()\n");
  obj_p = find_instance_by_handle(handle);
  if (NULL == obj_p) {
    PSCC_DBG_TRACE(LOG_ERR, "invalid bearer obj specified\n");
    return (-1);
  }

  switch (obj_p->state) {
  case pscc_bearer_mal_activating:
  case pscc_bearer_mal_activated:
    PSCC_DBG_TRACE(LOG_DEBUG, "destroy called on active pdp\n");
    pscc_bearer_change_state(obj_p,pscc_bearer_mal_deactivating);
    (void) deactivate_pdp(obj_p);
    /* fallthrough */
  case pscc_bearer_mal_deactivating: /* already deactivating */
    reset_bearer(obj_p, true);
    break;
  default:
    break;
  }

  if (mal_gpds_request(obj_p->mal_cid, MAL_GPDS_REQ_PSCONN_RELEASE, NULL) < 0) {
    PSCC_DBG_TRACE(LOG_ERR, "mal_gpds_request failed\n");
  }
  mpl_list_remove(&pscc_bearer_list_p, &obj_p->list_entry);
  free(obj_p);
  PSCC_DBG_TRACE(LOG_DEBUG, "bearer freed\n");
  return (0);
}

/**
 * pscc_bearer_activate
 **/
int pscc_bearer_activate(int handle, pscc_get_param_fp_t get_param_cb, pscc_set_param_fp_t set_param_cb,
    pscc_delete_param_fp_t delete_param_cb, pscc_bearer_event_fp_t event_cb)
{
  pscc_bearer_t *obj_p;
  static const int pscc_context_type = MAL_PDP_CONTEXT_TYPE_PRIMARY;
  void *data = NULL;
  mal_gpds_phonet_conf_t phonet_mode = MAL_GPDS_CONFIG_PHONET_NORMAL;
  mpl_param_element_t* param_p;

  PSCC_DBG_TRACE(LOG_DEBUG, "inside pscc_bearer_activate\n");

  /* if we want h3g to be configured, but is not or if the configuration failed before this activation, we warn */
  if (pending_aol_mode != pscc_aol_conf_state_idle)
  {
    PSCC_DBG_TRACE(LOG_WARNING, "no successful response of aol configuration has been received, so aol might not work!\n");
  }

  assert(get_param_cb != NULL);
  assert(set_param_cb != NULL);
  assert(delete_param_cb != NULL);
  assert(event_cb != NULL);

  obj_p = find_instance_by_handle(handle);
  if (NULL == obj_p) {
    PSCC_DBG_TRACE(LOG_ERR, "invalid bearer handle\n");
    return (-1);
  }

  if (pscc_bearer_mal_idle != obj_p->state) {
    PSCC_DBG_TRACE(LOG_ERR, "invalid bearer obj state\n");
    return (-1);
  }

  /* store callbacks */
  obj_p->set_param_cb = set_param_cb;
  obj_p->get_param_cb = get_param_cb;
  obj_p->delete_param_cb = delete_param_cb;
  obj_p->event_cb = event_cb;

  /* set parameters*/
  set_apn(obj_p);
  set_static_ip(obj_p);
  set_dcmp(obj_p);
  set_hcmp(obj_p);
  set_pdptype(obj_p);
  set_auth(obj_p);
  set_qos(obj_p);

  if (mal_gpds_set_param(obj_p->mal_cid, MAL_PARAMS_PDP_CONTEXT_TYPE_ID, (void*) &pscc_context_type) < 0)
    PSCC_DBG_TRACE(LOG_ERR, "mal_gpds_set_param(CONTEXT_TYPE)\n");

  /* special case for generation of uplink data */
  param_p = obj_p->get_param_cb(obj_p->parent, pscc_paramid_pdp_type);
  if ((NULL != param_p) && (NULL != param_p->value_p) && *((uint32_t*) param_p->value_p) == pscc_pdp_type_test)
  {
    PSCC_DBG_TRACE(LOG_DEBUG, "activate request done in test mode\n");
    phonet_mode = MAL_GPDS_CONFIG_PHONET_TEST;
  }
  data = (void*) &phonet_mode;

  /* activate primary PDP context */
  if (mal_gpds_request(obj_p->mal_cid, MAL_GPDS_REQ_PSCONN_ACTIVATE, data) < 0) {
    PSCC_DBG_TRACE(LOG_ERR, "mal_gpds_request(ACTIVATE) failed!\n");
    reset_bearer(obj_p, false);
    return (-1);
  }
  pscc_bearer_change_state(obj_p,pscc_bearer_mal_activating);
  return (0);
}

/**
 * pscc_bearer_deactivate
 **/
int pscc_bearer_deactivate(int handle)
{
  pscc_bearer_t *obj_p;

  PSCC_DBG_TRACE(LOG_DEBUG, "inside pscc_bearer_deactivate\n");

  obj_p = find_instance_by_handle(handle);
  if (NULL == obj_p) {
    PSCC_DBG_TRACE(LOG_ERR, "invalid bearer handle\n");
    return (-1);
  }

  switch (obj_p->state) {
  case pscc_bearer_mal_activating:
  case pscc_bearer_mal_activated:
    pscc_bearer_change_state(obj_p,pscc_bearer_mal_deactivating);
    if (deactivate_pdp(obj_p) < 0) {
      reset_bearer(obj_p, true);
      return (-1);
    }
    break;
  case pscc_bearer_mal_deactivating: /* already deactivating */
    break;
  case pscc_bearer_mal_idle: /* unexpected state */
  default:
    return (-1);
  }
  return (0);
}

/**
 * Reads the qos parameters from mpl and attempts to set them on the MAL interface.
 *
 * Returns -1 if something does not work according to plan.
 **/
int pscc_bearer_modify(int handle)
{
  pscc_bearer_t *obj_p;
  obj_p = find_instance_by_handle(handle);

  if (NULL == obj_p) {
    PSCC_DBG_TRACE(LOG_ERR, "invalid bearer handle\n");
    return (-1);
  }

  if (pscc_bearer_mal_activated != obj_p->state) {
    PSCC_DBG_TRACE(LOG_ERR, "invalid MAL bearer obj state\n");
    return (-1);
  }

  // set the qos parameters:
  set_qos(obj_p);

  if (mal_gpds_request(obj_p->mal_cid, MAL_GPDS_REQ_PSCONN_MODIFY, NULL) < 0) {
    PSCC_DBG_TRACE(LOG_ERR, "mal_gpds_request(MAL_GPDS_REQ_PSCONN_MODIFY)\n");
    return (-1);
  }

  return (0);
}

/**
 * pscc_bearer_handler()
 **/
int pscc_bearer_handler(int fd)
{
  if (fd != pscc_mal_gpds_fd)
    return (-1);

  mal_gpds_response_handler(pscc_mal_gpds_fd);
  return (0);
}

/**
 * pscc_bearer_attach()
 **/
int pscc_bearer_attach (pscc_bearer_attach_event_fp_t event_cb, uint32_t ct)
{
  assert(event_cb != NULL);

  /* if we have pending attach/detach we dont let this attach pass */
  if (attach_ct_set == true)
  {
    PSCC_IDENTIFIER_NOT_USED(ct);
    PSCC_DBG_TRACE(LOG_ERR, "pending attach/detach request already in progress\n");
    return (-1);
  }

  pscc_bearer_attach_event_cb = event_cb;

  if (pscc_mal_attach_status == pscc_attach_status_attached)
  {
    return (-1);
  }
  /* do ps attach */
  if (mal_gpds_request(0, MAL_GPDS_REQ_PS_ATTACH, NULL) < 0) {
    PSCC_DBG_TRACE(LOG_ERR, "mal_gpds_request(ATTACH) failed!\n");
    return (-1);
  }

  /* we await event about attach/detach */
  attach_request_ct = ct;
  attach_ct_set = true;

  return (0);
}

/**
 * pscc_bearer_detach()
 **/
int pscc_bearer_detach (pscc_bearer_attach_event_fp_t event_cb, uint32_t ct)
{
  assert(event_cb != NULL);

  /* if we have pending attach/detach we dont let this attach pass */
  if (attach_ct_set == true)
  {
    PSCC_IDENTIFIER_NOT_USED(ct);
    PSCC_DBG_TRACE(LOG_ERR, "pending attach/detach request already in progress\n");
    return (-1);
  }

  pscc_bearer_attach_event_cb = event_cb;

  if (pscc_mal_attach_status == pscc_attach_status_detached)
  {
    return (-1);
  }
  /* do ps detach */
  if (mal_gpds_request(0, MAL_GPDS_REQ_PS_DETACH, NULL) < 0) {
    PSCC_DBG_TRACE(LOG_ERR, "mal_gpds_request(DETACH) failed!\n");
    return (-1);
  }

  /* we await event about attach/detach */
  attach_request_ct = ct;
  attach_ct_set = true;

  return (0);
}

/**
 * pscc_bearer_attach_status()
 **/
int pscc_bearer_attach_status (pscc_attach_status_t *attach_status_p)
{
  assert(attach_status_p != NULL);
  *attach_status_p = pscc_mal_attach_status;
  return (0);
}

/* query ps attach status */
int pscc_bearer_query_attach_status(pscc_bearer_attach_event_fp_t event_cb, uint32_t ct)
{
  assert(event_cb != NULL);
  pscc_bearer_attach_event_cb = event_cb;

  /* if we have pending attach request we dont let this pass */
  if (attach_ct_set == true)
  {
    PSCC_IDENTIFIER_NOT_USED(ct);
    PSCC_DBG_TRACE(LOG_ERR, "pending attach request already in progress\n");
    return (-1);
  }

  if (mal_gpds_request(0, MAL_GPDS_REQ_PS_ATTACH_STATUS, NULL) < 0) {
    PSCC_DBG_TRACE(LOG_ERR, "mal_gpds_request(PS_ATTACH_STATUS) failed!\n");
    return (-1);
  }

    /* we await event about attach/detach */
    attach_request_ct = ct;
    attach_ct_set = true;
    return (0);
}

/**
 * pscc_bearer_query_attach_mode()
 **/
int pscc_bearer_query_attach_mode (pscc_bearer_attach_event_fp_t event_cb, uint32_t ct)
{
  assert(event_cb != NULL);

  /*
   * Certain bearer operations can not be performed concurrently with this operation.
   * If any such operation is pending, then we do not let this operation start.
   */
  if (attach_ct_set == true)
  {
    PSCC_IDENTIFIER_NOT_USED(ct);
    PSCC_DBG_TRACE(LOG_ERR, "%s: failure due to other pending bearer operation\n", __func__);
    return (-1);
  }

  pscc_bearer_attach_event_cb = event_cb;

  /* query ps attach mode */
  if (mal_gpds_request(0, MAL_GPDS_REQ_QUERY_CONFIG, NULL) < 0) {
    PSCC_DBG_TRACE(LOG_ERR, "mal_gpds_request(QUERY_CONFIG) failed!\n");
    return (-1);
  }

  /*
   * Set the flag to block certain other bearer operations.
   */
  attach_request_ct = ct;
  attach_ct_set = true;
  config_source_func = pscc_bearer_query_attach_mode;

  return (0);
}

/**
 * pscc_bearer_query_ping_block_mode()
 **/
int pscc_bearer_query_ping_block_mode (pscc_bearer_ping_block_mode_event_fp_t event_cb, uint32_t ct)
{
  assert(event_cb != NULL);

  /*
   * Certain bearer operations can not be performed concurrently with this operation.
   * If any such operation is pending, then we do not let this operation start.
   */
  if (attach_ct_set)
  {
    PSCC_DBG_TRACE(LOG_ERR, "%s: failure due to other pending bearer operation\n", __func__);
    return (-1);
  }

    pscc_bearer_ping_mode_event_cb = event_cb;

    /* query ping mode */
    if (mal_gpds_request(0, MAL_GPDS_REQ_QUERY_CONFIG, NULL) < 0) {
      PSCC_DBG_TRACE(LOG_ERR, "mal_gpds_request(QUERY_CONFIG) failed!\n");
      return (-1);
    }

    /*
     * Set the flag to block certain other bearer operations.
     */
    attach_request_ct = ct;
    attach_ct_set = true;
    config_source_func = pscc_bearer_query_ping_block_mode;

    return (0);
}

/**
 * pscc_bearer_configure_attach_mode()
 **/
int pscc_bearer_configure_attach_mode (pscc_bearer_attach_event_fp_t event_cb, pscc_attach_mode_t attach_mode, uint32_t ct)
{
  mal_gpds_config_t mal_gpds_config;

  assert(event_cb != NULL);

  /*
   * Certain bearer operations can not be performed concurrently with this operation.
   * If any such operation is pending, then we do not let this operation start.
   */
  if (attach_ct_set == true)
  {
    PSCC_IDENTIFIER_NOT_USED(ct);
    PSCC_DBG_TRACE(LOG_ERR, "%s: failure due to other pending bearer operation\n", __func__);
    return (-1);
  }

  pscc_bearer_attach_event_cb = event_cb;
  if (attach_mode == pscc_attach_mode_automatic) {
      mal_gpds_config.ps_attach_mode = MAL_GPDS_PS_ATTACH_AUTOMATIC;
  } else {
      mal_gpds_config.ps_attach_mode = MAL_GPDS_PS_ATTACH_MANUAL;
  }

  expected_attach_mode = mal_gpds_config.ps_attach_mode;

  mal_gpds_config.nwi_act_mode = MAL_GPDS_NWI_CONTEXT_DEFAULT;
  mal_gpds_config.aol_mode = MAL_GPDS_AOL_CONTEXT_DEFAULT;
  mal_gpds_config.drop_icmp_mode = MAL_GPDS_ICMP_DROP_MODE_KEEP_EXISTING;

  /* configure ps attach mode */
  if (mal_gpds_request(0, MAL_GPDS_REQ_CONFIG_CHANGE, &mal_gpds_config) < 0) {
    PSCC_DBG_TRACE(LOG_ERR, "mal_gpds_request(CONFIG_CHANGE) failed!\n");
    return (-1);
  }

  /*
   * Set the flag to block certain other bearer operations.
   */
  attach_request_ct = ct;
  attach_ct_set = true;
  config_source_func = pscc_bearer_configure_attach_mode;

  return (0);
}

/**
 * pscc_bearer_configure_ping_block_mode()
 **/
int pscc_bearer_configure_ping_block_mode(pscc_bearer_ping_block_mode_event_fp_t event_cb,
                                          pscc_ping_block_mode_t ping_mode, uint32_t ct)
{
  mal_gpds_config_t mal_gpds_config;

  assert(event_cb != NULL);

  /*
   * Certain bearer operations can not be performed concurrently with this operation.
   * If any such operation is pending, then we do not let this operation start.
   */
  if (attach_ct_set == true)
  {
    PSCC_IDENTIFIER_NOT_USED(ct);
    PSCC_DBG_TRACE(LOG_ERR, "%s: failure due to other pending bearer operation\n", __func__);
    return (-1);
  }

  pscc_bearer_ping_mode_event_cb = event_cb;

  if (ping_mode == pscc_ping_block_mode_enabled) {
    mal_gpds_config.drop_icmp_mode = MAL_GPDS_ICMP_DROP_MODE_ENABLED;
  } else {
      mal_gpds_config.drop_icmp_mode = MAL_GPDS_ICMP_DROP_MODE_DISABLED;
  }

  expected_ping_block_mode = mal_gpds_config.drop_icmp_mode;

  mal_gpds_config.ps_attach_mode = MAL_GPDS_PS_ATTACH_DEFAULT;
  mal_gpds_config.nwi_act_mode = MAL_GPDS_NWI_CONTEXT_DEFAULT;
  mal_gpds_config.aol_mode = MAL_GPDS_AOL_CONTEXT_DEFAULT;

  /* configure ping mode */
  if (mal_gpds_request(0, MAL_GPDS_REQ_CONFIG_CHANGE, &mal_gpds_config) < 0) {
    PSCC_DBG_TRACE(LOG_ERR, "mal_gpds_request(CONFIG_CHANGE) failed!\n");
    return (-1);
  }

  /*
   * Set the flag to block certain other bearer operations.
   */
  attach_request_ct = ct;
  attach_ct_set = true;
  config_source_func = pscc_bearer_configure_ping_block_mode;

  return (0);
}

/**
 * pscc_bearer_configure_aol_mode()
 **/
int pscc_bearer_configure_aol_mode (bool enable)
{
  mal_gpds_config_t mal_gpds_config;

  if (pending_aol_mode == pscc_aol_conf_state_pending) {
    PSCC_DBG_TRACE(LOG_WARNING, "pscc_bearer_configure_aol_mode already pending!\n");
  }

  mal_gpds_config.nwi_act_mode = MAL_GPDS_NWI_CONTEXT_DEFAULT;
  mal_gpds_config.ps_attach_mode = MAL_GPDS_PS_ATTACH_DEFAULT;
  mal_gpds_config.drop_icmp_mode = MAL_GPDS_ICMP_DROP_MODE_KEEP_EXISTING;
  mal_gpds_config.aol_mode = (enable) ? MAL_GPDS_AOL_CONTEXT_ENABLE : MAL_GPDS_AOL_CONTEXT_DISABLE;

  expected_aol_mode = mal_gpds_config.aol_mode; /* update what we expect to get */

  /* configure ps attach mode */
  if (mal_gpds_request(0, MAL_GPDS_REQ_CONFIG_CHANGE, &mal_gpds_config) < 0) {
    PSCC_DBG_TRACE(LOG_ERR, "mal_gpds_request(CONFIG_CHANGE) failed!\n");
    pending_aol_mode = pscc_aol_conf_state_failed; /* indicate failure */
    return (-1);
  }

  pending_aol_mode = pscc_aol_conf_state_pending; /* indicate pending/waiting */
  return (0);
}

/**
 * pscc_bearer_generate_uplink_data()
 **/
int pscc_bearer_generate_uplink_data (int handle)
{
  pscc_bearer_t *obj_p;
  int mal_fd;
  uint32_t uplink_size = 0;
  uint32_t max_data_size = maximum_uplink_packet_size;
  ssize_t data_sent = 0;
  mpl_param_element_t* param_p;
  static struct iovec iov[1];
  static struct msghdr send_msg;

  PSCC_DBG_TRACE(LOG_DEBUG, "pscc_bearer_generate_uplink_data()\n");

  memset(&iov, 0, sizeof(struct iovec));
  memset(&send_msg, 0, sizeof(struct msghdr));

  obj_p = find_instance_by_handle(handle);
  if (NULL == obj_p)
  {
    PSCC_DBG_TRACE(LOG_ERR, "invalid bearer obj specified\n");
    return (-1);
  }

  /* fetch the filedescriptor from mal */
  if (mal_gpds_get_param(obj_p->mal_cid, MAL_PARAMS_PIPEFD_ID, &mal_fd) < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "mal_gpds_get_param(fd)\n");
    return (-1);
  }

  if (mal_fd<=0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "invalid mal_fd received, fd=%d\n", mal_fd);
    return (-1);
  }

  assert(NULL != obj_p->get_param_cb);
  param_p = obj_p->get_param_cb(obj_p->parent, pscc_paramid_uplink_data_size);
  uplink_size = *(uint32_t*) param_p->value_p;

  PSCC_DBG_TRACE(LOG_DEBUG, "Sending %d bytes of data, max packet size:%d\n", uplink_size, max_data_size);
  for(; uplink_size >0; uplink_size -= data_sent)
  {
    /* Compute number of bytes to send in this packet */
    ssize_t data_send_size = max_data_size>uplink_size ? uplink_size : max_data_size;
    uint8_t data_send_buf[data_send_size];

    PSCC_DBG_TRACE(LOG_DEBUG, "Packet contains: %d bytes of data. %d bytes left to send.\n", data_send_size, max_data_size>uplink_size ? 0 : uplink_size-data_send_size);

    /* Fill the buffer with 0x7e data */
    memset(&data_send_buf, 0x7e, data_send_size);

    iov[0].iov_base=(unsigned char *)data_send_buf;
    iov[0].iov_len = data_send_size;
    send_msg.msg_iov = iov;
    send_msg.msg_iovlen = 1;
    send_msg.msg_control = NULL;
    send_msg.msg_controllen = 0;
    send_msg.msg_flags = 0;

    /* Send message to socket */
    data_sent = sendmsg(mal_fd, &send_msg, MSG_EOR);

    if (data_sent < 0)
    {
        PSCC_DBG_TRACE(LOG_ERR, "Writing to filedescriptor failed, returnvalue was: %d. errno:%x errnostr:%s", data_sent,  errno, strerror(errno));
        return (-1);
    }
    else if (0 == data_sent)
   {
        /* Stop here to avoid any possible neverending loops */
        PSCC_DBG_TRACE(LOG_ERR, "Writing to filedescriptor succeded, but wrote zero bytes.");
        return (-1);
    }
    else
    {
        PSCC_DBG_TRACE(LOG_DEBUG, "Successflully wrote %d bytes to mal filedescriptor", data_sent);
    }
  }

  return (0);
}

/**
 * pscc_bearer_fetch_data_counters()
 **/
int pscc_bearer_data_counters (int handle, bool fetch)
{
  pscc_bearer_t *obj_p;

  PSCC_DBG_TRACE(LOG_DEBUG, "pscc_bearer_data_counters(), fetch=%x\n", fetch);

  obj_p = find_instance_by_handle(handle);
  if (NULL == obj_p)
  {
    PSCC_DBG_TRACE(LOG_ERR, "invalid bearer obj specified\n");
    return (-1);
  }

  if (pscc_bearer_mal_activated != obj_p->state) {
    PSCC_DBG_TRACE(LOG_ERR, "invalid MAL bearer obj state\n");
    return (-1);
  }

  if (pscc_data_counter_state_idle != obj_p->data_counter_state)
  {
    PSCC_DBG_TRACE(LOG_ERR, "already pending data counter request on this pdp context\n");
    return (-1);
  }

  if (fetch)
  {
    obj_p->data_counter_state = pscc_data_counter_state_fetching;
  }
  else
  {
    obj_p->data_counter_state = pscc_data_counter_state_resetting;
  }

  if (mal_gpds_request(obj_p->mal_cid, MAL_GPDS_REQ_DATA_COUNTER, NULL) < 0) {
    PSCC_DBG_TRACE(LOG_ERR, "mal_gpds_request(MAL_GPDS_REQ_DATA_COUNTER)\n");
    return (-1);
  }

  return (0);
}

/**
 * pscc_bearer_respond_resource_control()
 **/
int pscc_bearer_respond_resource_control (pscc_rc_t* pscc_rc_p, bool accepted, bool modified)
{
  mal_gpds_resource_control_req_info_t mal_gpds_resource_control_req_info;
  assert(pscc_rc_p != NULL);

  PSCC_DBG_TRACE(LOG_DEBUG, "pscc_bearer_respond_resource_control(), seq_id=%d, accepted=%x, modified=%x\n", pscc_rc_p->seq_id, accepted, modified);

  memset(&mal_gpds_resource_control_req_info, 0, sizeof(mal_gpds_resource_control_req_info));
  mal_gpds_resource_control_req_info.seq_id = pscc_rc_p->seq_id;

  /* set call control result */
  if (modified && accepted) /* modified */
  {
    mal_gpds_resource_control_req_info.cc_result = MAL_GPDS_RESOURCE_MODIFIED;
    mal_gpds_resource_control_req_info.resource_control_data.data = pscc_rc_p->data_p;
    mal_gpds_resource_control_req_info.resource_control_data.data_length = pscc_rc_p->length;
    /* we are not sure what the values that has been modified, but no one should read these changed values */
  }
  else if(accepted) /* accepted */
  {
    mal_gpds_resource_control_req_info.cc_result = MAL_GPDS_RESOURCE_ALLOWED;
  }
  else /* rejected */
  {
    /* modem will say activation failed if we response this so we dont need to do anything else */
    mal_gpds_resource_control_req_info.cc_result = MAL_GPDS_RESOURCE_REJECTED;
  }

  if (mal_gpds_request(0, MAL_GPDS_REQ_RESOURCE_CONTROL, &mal_gpds_resource_control_req_info) < 0) {
    PSCC_DBG_TRACE(LOG_ERR, "mal_gpds_request(MAL_GPDS_REQ_RESOURCE_CONTROL)\n");
    return (-1);
  }

  return (0);
}

/**
 * pscc_bearer_configure_rc()
 **/
int pscc_bearer_configure_rc (pscc_bearer_rc_event_fp_t rc_event_cb)
{
  assert(rc_event_cb != NULL);
  pscc_bearer_rc_event_cb = rc_event_cb;

  if (mal_gpds_request(0, MAL_GPDS_REQ_RESOURCE_CONFIGURE, &gprs_res_ctrl) < 0) {
    PSCC_DBG_TRACE(LOG_ERR, "mal_gpds_request(MAL_GPDS_REQ_RESOURCE_CONFIGURE)\n");
    return (-1);
  }
  return (0);
}

/**
 * net link functions
 **/

/**
 * pscc_bearer_netlnk_init()
 **/
int pscc_bearer_netlnk_init()
{
    return pscc_netlnk_fd;
}

/**
 * pscc_bearer_netlnk_handle_message()
 **/
int pscc_bearer_netlnk_handle_message(int fd)
{
    int netlnk_msg = -1;

    if (fd != pscc_netlnk_fd) {
        PSCC_DBG_TRACE(LOG_ERR, "invalid netlnk file descriptor!!\n");
        return (-1);
    }

    if( netlnk_socket_recv(fd, &netlnk_msg) < 0 ){
        PSCC_DBG_TRACE(LOG_ERR, "Err netlnk_socket_recv\n");
        return (-1);
    }

    if (netlnk_msg == MODEM_RESET_IND) {
        PSCC_DBG_TRACE(LOG_ERR, "received modem reset indication!!\n");
        reset_ind_set = true;
        return 0;
    }

    return (-1);
}


/**
 * net dev functions
 **/
/**
 * pscc_netdev_init()
 **/
int pscc_netdev_init(void)
{
  if (!pscc_mal_initialised) {
    if (mal_gpds_init(&pscc_mal_gpds_fd, &pscc_netlnk_fd, &pscc_mal_pipe_fd[0]) < 0)
      return (-1);
    pscc_mal_initialised = true;
  }

  /* before kernel 3.0 upgrade of mal, the pipe_fd is set to -1 and we need to create
   * a dead end pipe */
  if (pscc_mal_pipe_fd[0] < 0)
  {
    PSCC_DBG_TRACE(LOG_INFO, "received negative pipe fd from mal, assuming pre-kernel upgrade.\n");
    if(pipe(pscc_mal_pipe_fd) < 0)
    {
       PSCC_DBG_TRACE(LOG_ERR, "failed to create a unidirectional pipe\n");
       return -1;
    }
  }

  /* register callback */
  mal_gpds_register_callback(mal_event_cb);
  /* return the read end of the pipe */
  return (pscc_mal_pipe_fd[0]);
}

/**
 * pscc_netdev_shutdown()
 **/
void pscc_netdev_shutdown(void)
{
  /* close pipe fd created for psccd */
  if (pscc_mal_pipe_fd[0] > -1)
    close(pscc_mal_pipe_fd[0]);
  if (pscc_mal_pipe_fd[1] > -1)
    close(pscc_mal_pipe_fd[1]);

  pscc_mal_pipe_fd[0] = -1;
  pscc_mal_pipe_fd[1] = -1;
}

/**
 * pscc_netdev_handle_message()
 **/
int pscc_netdev_handle_message(int fd)
{
  if (fd != pscc_mal_pipe_fd[0])
    return (-1);

  /* call gpds response handle */
  (void) mal_gpds_response_handler(fd);
  return 0;
}

/**
 * pscc_netdev_create()
 **/
int pscc_netdev_create(int connid, pscc_get_param_fp_t param_get_cb, pscc_set_param_fp_t param_set_cb,
    pscc_delete_param_fp_t param_delete_cb, pscc_netdev_destroyed_fp_t netdev_destroyed_cb)
{
  pscc_bearer_t *obj_p;
  PSCC_IDENTIFIER_NOT_USED(param_get_cb);
  PSCC_IDENTIFIER_NOT_USED(param_set_cb);
  PSCC_IDENTIFIER_NOT_USED(param_delete_cb);
  PSCC_IDENTIFIER_NOT_USED(netdev_destroyed_cb);

  obj_p = find_instance_by_connid(connid);
  if (NULL == obj_p)
    return (-1);
  return (obj_p->handle);
}

/**
 *  pscc_netdev_destroy()
 **/
int pscc_netdev_destroy(int dev_handle)
{
  /* do nothing, always success */
  PSCC_IDENTIFIER_NOT_USED(dev_handle);
  return 0;
}
/*******************************************************************************
 *
 * Private Functions
 *
 *******************************************************************************/

/**
 * mal_event_cb
 **/
static void mal_event_cb(uint8_t conn_id, mal_gpds_event_ids_t event_id, void *params)
{
  pscc_bearer_t *obj_p;

  PSCC_DBG_TRACE(LOG_DEBUG, "inside mal_event_cb(%d)\n",conn_id);

  if (handle_attach_event(event_id, params) == 0)
  {
    PSCC_DBG_TRACE(LOG_DEBUG, "mal event handled by handle_attach_event\n");
    return;
  }

  if (handle_resource_control_event(event_id, params) == 0)
  {
    PSCC_DBG_TRACE(LOG_DEBUG, "mal event handled by handle_resource_configure_event\n");
    return;
  }

  obj_p = find_instance_by_mal_cid(conn_id);
  if (NULL == obj_p) {
    PSCC_DBG_TRACE(LOG_ERR, "bearer obj not found\n");
    return;
  }
  switch (event_id) {
  case MAL_GPDS_EVENT_ID_PDP_ACTIVATING:
    PSCC_DBG_TRACE(LOG_DEBUG, "MAL_GPDS_EVENT_ID_PDP_ACTIVATING()\n");
    handle_event_activating(obj_p, params);
    break;
  case MAL_GPDS_EVENT_ID_PDP_ACTIVE:
    PSCC_DBG_TRACE(LOG_DEBUG, "MAL_GPDS_EVENT_ID_PDP_ACTIVE()\n");
    handle_event_activated(obj_p, params);
    break;
  case MAL_GPDS_EVENT_ID_PDP_ACTIVATION_FAILED:
    PSCC_DBG_TRACE(LOG_DEBUG, "MAL_GPDS_EVENT_ID_PDP_ACTIVATION_FAILED()\n");
    handle_event_activation_failed(obj_p, params);
    break;
  case MAL_GPDS_EVENT_ID_PDP_DEACTIVATED:
    PSCC_DBG_TRACE(LOG_DEBUG, "MAL_GPDS_EVENT_ID_PDP_DEACTIVATED()\n");
    handle_event_deactivated(obj_p, params);
    break;
  case MAL_GPDS_EVENT_ID_DATA_COUNTER:
    PSCC_DBG_TRACE(LOG_DEBUG, "MAL_GPDS_EVENT_ID_DATA_COUNTER()\n");
    handle_event_data_counter(obj_p, params);
    break;
  case MAL_GPDS_EVENT_ID_PDP_SUSPENDED:
    PSCC_DBG_TRACE(LOG_DEBUG, "MAL_GPDS_EVENT_ID_PDP_SUSPENDED()\n");
    break;
  case MAL_GPDS_EVENT_ID_NWI_PDP_REQUESTED:
    PSCC_DBG_TRACE(LOG_DEBUG, "MAL_GPDS_EVENT_ID_NWI_PDP_REQUESTED()\n");
    break;
  case MAL_GPDS_EVENT_ID_CONTEXT_MODIFIED:
    PSCC_DBG_TRACE(LOG_DEBUG, "MAL_GPDS_EVENT_ID_CONTEXT_MODIFIED()\n");
    handle_event_context_modified(obj_p, params);
    break;
  case MAL_GPDS_EVENT_ID_CONTEXT_MODIFICATION_FAILED:
    PSCC_DBG_TRACE(LOG_ERR, " MAL_GPDS_EVENT_ID_CONTEXT_MODIFICATION_FAILED\n");
    obj_p->event_cb(obj_p->parent, pscc_bearer_event_modify_failed, 0, 0);
    break;
  default:
    break;
  }
}

/**
 * reset_bearer
 **/
static void reset_bearer(pscc_bearer_t *obj_p, bool send_event_deactivated)
{
  assert(obj_p != NULL);

  pscc_bearer_change_state(obj_p,pscc_bearer_mal_idle);

  if (send_event_deactivated && (NULL != obj_p->event_cb))
    obj_p->event_cb(obj_p->parent, pscc_bearer_event_deactivated, obj_p->reason, obj_p->cause);

  /* delete all parameters potentially set by the bearer*/
  if (NULL != obj_p->delete_param_cb) {
    obj_p->delete_param_cb(obj_p->parent, pscc_paramid_own_ip_address);
    obj_p->delete_param_cb(obj_p->parent, pscc_paramid_own_ipv6_address);
    obj_p->delete_param_cb(obj_p->parent, pscc_paramid_gw_address);
    obj_p->delete_param_cb(obj_p->parent, pscc_paramid_dns_address);
    obj_p->delete_param_cb(obj_p->parent, pscc_paramid_ipv6_dns_address);
    obj_p->delete_param_cb(obj_p->parent, pscc_paramid_secondary_dns_address);
    obj_p->delete_param_cb(obj_p->parent, pscc_paramid_ipv6_secondary_dns_address);
    obj_p->delete_param_cb(obj_p->parent, pscc_paramid_netdev_name);
  }

  obj_p->event_cb = NULL;
  obj_p->set_param_cb = NULL;
  obj_p->get_param_cb = NULL;
  obj_p->delete_param_cb = NULL;
  obj_p->reason = pscc_reason_pdp_context_ms_deactivated;
  obj_p->cause = 0;
}

/**
 * find_instance_by_handle()
 **/
static pscc_bearer_t *find_instance_by_handle(int handle)
{
  mpl_list_t *obj_p;
  pscc_bearer_t *bearer_p;

  MPL_LIST_FOR_EACH(pscc_bearer_list_p, obj_p) {
    bearer_p = MPL_LIST_CONTAINER(obj_p, pscc_bearer_t, list_entry);
    if ((NULL != bearer_p) && (bearer_p->handle == handle))
      return (bearer_p);
  }
  return NULL;
}

/**
 * find_instance_by_mal_cid()
 **/
static pscc_bearer_t *find_instance_by_mal_cid(uint8_t mal_cid)
{
  mpl_list_t *obj_p;
  pscc_bearer_t *bearer_p;

  MPL_LIST_FOR_EACH(pscc_bearer_list_p, obj_p) {
    bearer_p = MPL_LIST_CONTAINER(obj_p, pscc_bearer_t, list_entry);
    if ((NULL != bearer_p) && (bearer_p->mal_cid == mal_cid))
      return (bearer_p);
  }
  return NULL;
}

/**
 * find_instance_by_connid()
 **/
static pscc_bearer_t *find_instance_by_connid(int connid)
{
  mpl_list_t *obj_p;
  pscc_bearer_t *bearer_p;

  MPL_LIST_FOR_EACH(pscc_bearer_list_p, obj_p) {
    bearer_p = MPL_LIST_CONTAINER(obj_p, pscc_bearer_t, list_entry);
    if ((NULL != bearer_p) && (bearer_p->parent == connid))
      return (bearer_p);
  }
  return NULL;
}

/*
 * set_apn
 */
static void set_apn(pscc_bearer_t *obj_p)
{
  static const uint8_t apn[] = "";
  uint8_t *apn_p = (uint8_t*) &apn;
  mpl_param_element_t* param_p;

  assert(NULL != obj_p);
  assert(NULL != obj_p->get_param_cb);

  param_p = obj_p->get_param_cb(obj_p->parent, pscc_paramid_apn);
  if (NULL != param_p && NULL != param_p->value_p)
    apn_p = (uint8_t*) param_p->value_p;

  PSCC_DBG_TRACE(LOG_DEBUG, "using apn %s\n",(char*)apn_p);

  if (mal_gpds_set_param(obj_p->mal_cid, MAL_PARAMS_APN_ID, apn_p) < 0)
    PSCC_DBG_TRACE(LOG_ERR, "mal_gpds_set_param(APN)\n");
}

/*
 * set_static_ip
 */
static void set_static_ip(pscc_bearer_t *obj_p)
{
  uint8_t *static_ip_p = NULL;
  ip_address_t ip_address;
  int ip_family = AF_INET;
  mpl_param_element_t* param_p;

  assert(NULL != obj_p);
  assert(NULL != obj_p->get_param_cb);

  memset(&ip_address, 0, sizeof(ip_address));

  param_p = obj_p->get_param_cb(obj_p->parent, pscc_paramid_static_ip_address);
  if (NULL != param_p && NULL != param_p->value_p) {
    static_ip_p = (uint8_t*) param_p->value_p;
  }
  else {
    PSCC_DBG_TRACE(LOG_DEBUG, "no static ip found\n");
    return;
  }

  param_p = obj_p->get_param_cb(obj_p->parent, pscc_paramid_pdp_type);
  if ((NULL != param_p) && (NULL != param_p->value_p)) {
    switch (*((pscc_pdp_type_t*) param_p->value_p)) {
    case pscc_pdp_type_ipv4:
    case pscc_pdp_type_test: /* special case for uplink data generation */
      ip_family = AF_INET;
      break;
    case pscc_pdp_type_ipv6:
      ip_family = AF_INET6;
      break;
    default:
      PSCC_DBG_TRACE(LOG_ERR, "unsupported pdp type\n");
      return;
    }
  }

  if (inet_pton(ip_family, (char*) static_ip_p, &ip_address) <= 0) {
    PSCC_DBG_TRACE(LOG_ERR, "inet_pton failed\n");
    return;
  }

  PSCC_DBG_TRACE(LOG_DEBUG, "using static ip %s\n", (char*) static_ip_p);

  if (mal_gpds_set_param(obj_p->mal_cid, MAL_PARAMS_IP_ID, &ip_address) < 0)
    PSCC_DBG_TRACE(LOG_ERR, "mal_gpds_set_param(IP)\n");
}


/**
 * set_hcmp
 **/
static void set_hcmp(pscc_bearer_t *obj_p)
{
  mpl_param_element_t* param_p;
  int compression = MAL_PDP_HCMP_OFF;

  assert(NULL != obj_p);
  assert(NULL != obj_p->get_param_cb);

  param_p = obj_p->get_param_cb(obj_p->parent, pscc_paramid_hcmp);
  if ((NULL != param_p) && (NULL != param_p->value_p) && *((bool*) param_p->value_p))
    compression = MAL_PDP_HCMP_ON;

  if (mal_gpds_set_param(obj_p->mal_cid, MAL_PARAMS_HCMP_ID, &compression) < 0)
    PSCC_DBG_TRACE(LOG_ERR, "mal_gpds_set_param(APN)\n");
}

/**
 * set_dcmp
 **/
static void set_dcmp(pscc_bearer_t *obj_p)
{
  mpl_param_element_t* param_p;
  int compression = MAL_PDP_DCMP_OFF;

  assert(NULL != obj_p);
  assert(NULL != obj_p->get_param_cb);

  param_p = obj_p->get_param_cb(obj_p->parent, pscc_paramid_dcmp);
  if ((NULL != param_p) && (NULL != param_p->value_p) && *((bool*) param_p->value_p))
    compression = MAL_PDP_DCMP_ON;

  if (mal_gpds_set_param(obj_p->mal_cid, MAL_PARAMS_DCMP_ID, &compression) < 0)
    PSCC_DBG_TRACE(LOG_ERR, "mal_gpds_set_param(DCMP)\n");
}

/**
 * set_pdptype
 **/
static void set_pdptype(pscc_bearer_t *obj_p)
{
  mpl_param_element_t* param_p;
  int pdp_type = MAL_PDP_TYPE_IPV4;

  assert(NULL != obj_p);
  assert(NULL != obj_p->get_param_cb);

  param_p = obj_p->get_param_cb(obj_p->parent, pscc_paramid_pdp_type);
  if ((NULL != param_p) && (NULL != param_p->value_p)) {
    switch (*((pscc_pdp_type_t*) param_p->value_p)) {
    case pscc_pdp_type_ipv4:
    case pscc_pdp_type_test: /* special case for uplink data generation */
      pdp_type = (MAL_PDP_TYPE_IPV4);
      break;
    case pscc_pdp_type_ipv4v6:
      pdp_type = (MAL_PDP_TYPE_IPV4V6);
      break;
    case pscc_pdp_type_ipv6:
      pdp_type = (MAL_PDP_TYPE_IPV6);
      break;
    default:
      PSCC_DBG_TRACE(LOG_ERR, "unsupported pdp type, using default\n");
      break;
    }
  }
  obj_p->mal_pdp_type = pdp_type;
  if (mal_gpds_set_param(obj_p->mal_cid, MAL_PARAMS_PDP_TYPE_ID, &pdp_type) < 0)
    PSCC_DBG_TRACE(LOG_ERR, "mal_gpds_set_param(PDP_TYPE)\n");
}

/**
 * set_auth
 **/
static void set_auth(pscc_bearer_t *obj_p)
{
  static uint8_t pscc_chap_id = 1;
  static uint8_t pscc_pap_id = 1;
  mpl_param_element_t* param_p;
  pscc_auth_method_t pscc_auth_method = pscc_auth_method_none;
  int mal_auth_method = MAL_AUTH_METHOD_NONE;
  /* PAP and CHAP parameters */
  static const uint8_t uid[] = "";
  uint8_t *uid_p = (uint8_t*)&uid;
  static const uint8_t pwd[] = "";
  uint8_t *pwd_p = (uint8_t*)&pwd;
  uint8_t auth_id=0;
  /* CHAP parameters */
  uint8_t *challenge_p=NULL;
  uint8_t *response_p=NULL;
  size_t responselen=0;
  size_t challengelen=0;
  bool free_response = true;
  bool free_challenge = true;

  assert(NULL != obj_p);
  assert(NULL != obj_p->get_param_cb);

  param_p = obj_p->get_param_cb(obj_p->parent,pscc_paramid_auth_method);
  if(NULL != param_p)
    pscc_auth_method = *((pscc_auth_method_t*)param_p->value_p);

  param_p = obj_p->get_param_cb(obj_p->parent,pscc_paramid_auth_uid);
  if(NULL != param_p)
    uid_p = (uint8_t*)param_p->value_p;

  param_p = obj_p->get_param_cb(obj_p->parent,pscc_paramid_auth_pwd);
  if(NULL != param_p)
    pwd_p = (uint8_t*)param_p->value_p;

  PSCC_DBG_TRACE(LOG_DEBUG, "using auth_method %d, uid=%s, pwd=%s\n",pscc_auth_method,(char*)  uid_p,(char*) pwd_p);
  /* Create challenge */
  switch(pscc_auth_method)
  {
  case pscc_auth_method_chap:
  {
    mal_gpds_chap_auth_data_t challenge;
    mal_gpds_chap_auth_data_t response;
    memset(&challenge, '\0', sizeof(mal_gpds_chap_auth_data_t));
    memset(&response, '\0', sizeof(mal_gpds_chap_auth_data_t));

    mal_auth_method = MAL_AUTH_METHOD_SECURE;

    /* chap id */
    param_p = obj_p->get_param_cb(obj_p->parent,pscc_paramid_auth_chap_id);
    if(NULL != param_p)
      auth_id = (*(uint8_t*)param_p->value_p);
    else
      auth_id = (++pscc_chap_id);

    if(mal_gpds_set_param(obj_p->mal_cid,MAL_PARAMS_CHAP_IDENTIFIER_ID,&auth_id) < 0)
      PSCC_DBG_TRACE(LOG_ERR, "mal_gpds_set_param(CHAP_IDENTIFIER)\n");

    PSCC_DBG_TRACE(LOG_DEBUG, "auth_id 0x%x\n", auth_id);

    /* set username */
    if(mal_gpds_set_param(obj_p->mal_cid,MAL_PARAMS_USERNAME_ID,uid_p) < 0)
      PSCC_DBG_TRACE(LOG_ERR, "mal_gpds_set_param(USERNAME)\n");

    /* chap challenge */
    param_p = obj_p->get_param_cb(obj_p->parent,pscc_paramid_auth_chap_challenge);
    if(NULL != param_p)
    {
      PSCC_DBG_TRACE(LOG_DEBUG, "challange data from param\n");
      challengelen = ((mpl_uint8_array_t*)param_p->value_p)->len;
      challenge_p = ((mpl_uint8_array_t*)param_p->value_p)->arr_p;
      free_challenge = false;
    }
    else
      challengelen = pscc_utils_generate_chap_challenge(&challenge_p, MAL_GPDS_MAX_CHALLENGE_SIZE);

    challenge.length = challengelen;
    challenge.data = challenge_p;

    if(challengelen > 0 && mal_gpds_set_param(obj_p->mal_cid,MAL_PARAMS_CHALLENGE_DATA_ID,&challenge) < 0)
      PSCC_DBG_TRACE(LOG_ERR, "mal_gpds_set_param(CHALLENGE_DATA)\n");

    /* chap response */
    param_p = obj_p->get_param_cb(obj_p->parent,pscc_paramid_auth_chap_response);
    if(NULL != param_p)
    {
      PSCC_DBG_TRACE(LOG_DEBUG, "response data from param\n");
      responselen = ((mpl_uint8_array_t*)param_p->value_p)->len;
      response_p = ((mpl_uint8_array_t*)param_p->value_p)->arr_p;
      free_response = false;
    }
    else
      responselen = pscc_utils_calculate_chap_response(auth_id,
                                                     challenge_p,
                                                     challengelen,
                                                     (char*) pwd_p,
                                                     strlen((char*) pwd_p),
                                                     &response_p);

    response.length = responselen;
    response.data =  response_p;

    PSCC_DBG_TRACE(LOG_DEBUG, "chap challenge length 0x%x, response length 0x%x\n", challengelen, responselen);

    if(responselen > 0 && mal_gpds_set_param(obj_p->mal_cid,MAL_PARAMS_RESPONSE_DATA_ID,&response) < 0)
      PSCC_DBG_TRACE(LOG_ERR, "mal_gpds_set_param(RESPONSE_DATA)\n");

  }
    break;
  case pscc_auth_method_pap:
    mal_auth_method = MAL_AUTH_METHOD_NORMAL;
    auth_id = (++pscc_pap_id);

    /* set pap id */
    if(mal_gpds_set_param(obj_p->mal_cid,MAL_PARAMS_CHAP_IDENTIFIER_ID,&auth_id) < 0)
      PSCC_DBG_TRACE(LOG_ERR, "mal_gpds_set_param(CHAP_IDENTIFIER)\n");

    /* set username */
    if(mal_gpds_set_param(obj_p->mal_cid,MAL_PARAMS_USERNAME_ID,uid_p) < 0)
      PSCC_DBG_TRACE(LOG_ERR, "mal_gpds_set_param(USERNAME)\n");

    /* set password */
    if(mal_gpds_set_param(obj_p->mal_cid,MAL_PARAMS_PASSWORD_ID,pwd_p) < 0)
      PSCC_DBG_TRACE(LOG_ERR, "mal_gpds_set_param(PWD)\n");
    break;

  case pscc_auth_method_none:
  default:
    break;
  }

  if(mal_gpds_set_param(obj_p->mal_cid,MAL_PARAMS_AUTH_METHOD_ID,&mal_auth_method) < 0)
    PSCC_DBG_TRACE(LOG_ERR, "mal_gpds_set_param(AUTH_METHOD)\n");


  if(free_challenge && NULL != challenge_p)
    free(challenge_p);

  if(free_response && NULL != response_p)
    free(response_p);
}

/*
 *  Set all QoS parameters
 */
static void set_qos(pscc_bearer_t *obj_p)
{
  mal_gpds_qos_profile_t qos_p;
  assert(NULL != obj_p);

  /* set values for requested qos: */
  qos_mpl_to_mal(obj_p, &qos_p, true);

  if (mal_gpds_set_param(obj_p->mal_cid, MAL_PARAMS_QOS_REQ_ID, &qos_p) < 0) {
    PSCC_DBG_TRACE(LOG_ERR, "mal_gpds_set_param(MAL_PARAMS_QOS_REQ_ID)\n");
  }
  else {
    PSCC_DBG_TRACE(LOG_DEBUG, "Setting MAL_PARAMS_QOS_REQ_ID:\n");
    qos_dbg_print_mal_profile(&qos_p);
  }

  /* set values for rel5 qos: */
  if (get_qos_type(obj_p) == pscc_qos_type_rel99) {
    qos_p.qos_type = MAL_GPDS_QOS_REL5;
    if (mal_gpds_set_param(obj_p->mal_cid, MAL_PARAMS_QOS_REL5_ID, &qos_p) < 0) {
      PSCC_DBG_TRACE(LOG_ERR, "mal_gpds_set_param(MAL_PARAMS_QOS_REL5_ID)\n");
    }
    else {
      PSCC_DBG_TRACE(LOG_DEBUG, "Setting MAL_PARAMS_QOS_REL5_ID:\n");
      qos_dbg_print_mal_profile(&qos_p);
    }
  }

  /* set values for minimum qos: */
  qos_mpl_to_mal(obj_p, &qos_p, false);

  if (mal_gpds_set_param(obj_p->mal_cid, MAL_PARAMS_QOS_MIN_ID, &qos_p) < 0) {
    PSCC_DBG_TRACE(LOG_ERR, "mal_gpds_set_param(MAL_PARAMS_QOS_MIN_ID)\n");
  }
  else {
    PSCC_DBG_TRACE(LOG_DEBUG, "Setting MAL_PARAMS_QOS_MIN_ID:\n");
    qos_dbg_print_mal_profile(&qos_p);
  }
}

/*
 *  Get QoS type
 */
static pscc_qos_type_t get_qos_type(pscc_bearer_t *obj_p)
{
  mpl_param_element_t* param_p;
  pscc_qos_type_t qos_type = pscc_qos_type_rel99;

  assert(NULL != obj_p);

  /* get qos type */
  param_p = obj_p->get_param_cb(obj_p->parent, pscc_paramid_qos_type);
  if (NULL != param_p)
    qos_type = *((pscc_qos_type_t*) param_p->value_p);

  return qos_type;
}

/**
 * handle_event_activating
 **/
static void handle_event_activating(pscc_bearer_t *obj_p, void *params)
{
  assert(NULL != obj_p);
  PSCC_IDENTIFIER_NOT_USED(params);

  switch (obj_p->state) {
  case pscc_bearer_mal_activating:
    if (NULL != obj_p->event_cb)
      obj_p->event_cb(obj_p->parent, pscc_bearer_event_activating, 0, 0);
    break;
  default:
    PSCC_DBG_TRACE(LOG_ERR, "unexpected state\n");
    break;
  }
}

static int store_address(pscc_bearer_t *obj_p,
                         mal_gpds_params_id_t mal_param_id,
                         mpl_param_element_id_t mpl_param_id)
{
  char ip_address_str[sizeof("0102:0304:0506:0708:0910:1112:1314:1516")];
  ip_address_t ip_address;
  mpl_param_element_t *param_p;
  int pdp_type = MAL_PDP_TYPE_IPV4;

  /* Memset all addresses to zero */
  memset(&ip_address_str, 0, sizeof(ip_address_str));
  memset(&ip_address, 0, sizeof(ip_address));

  switch(mal_param_id)
  {
    case MAL_PARAMS_IP_ID:
    case MAL_PARAMS_PDNS_ID:
    case MAL_PARAMS_SDNS_ID:
       pdp_type = MAL_PDP_TYPE_IPV4;
       break;
    case MAL_PARAMS_IPV6_ID:
    case MAL_PARAMS_IPV6_PDNS_ID:
    case MAL_PARAMS_IPV6_SDNS_ID:
       pdp_type = MAL_PDP_TYPE_IPV6;
       break;
    default:
       PSCC_DBG_TRACE(LOG_ERR, "Invalid value of param id : %d\n",mal_param_id);
       return (-1);
  }

  if (mal_gpds_get_param(obj_p->mal_cid, mal_param_id, &ip_address) < 0) {
    PSCC_DBG_TRACE(LOG_ERR, "mal_gpds_get_param  param_id (%d)\n", mal_param_id);
    return (-1);
  }

  if (convert_ip_to_string(ip_address, pdp_type, ip_address_str) < 0) {
    PSCC_DBG_TRACE(LOG_ERR, "convert_ip_to_string param_id(%d)\n", mal_param_id);
    return (-1);
  }

  param_p = mpl_param_element_create_stringn(mpl_param_id, ip_address_str, strlen(ip_address_str));
  if (NULL == param_p) {
    PSCC_DBG_TRACE(LOG_ERR, "failed to create parameter for param_id(%d)\n", mal_param_id);
    return (-1);
  }

  if (obj_p->set_param_cb(obj_p->parent, param_p) < 0) {
    PSCC_DBG_TRACE(LOG_ERR, "not able to store MAL_PARAM_ID parameter %d \n", mal_param_id);
    mpl_param_element_destroy(param_p);
    return (-1);
  }
  PSCC_DBG_TRACE(LOG_INFO, "Address is : %s\n", ip_address_str);

  return 0;
}


/**
 * handle_event_activated
 **/
static void handle_event_activated(pscc_bearer_t *obj_p, void *params)
{
  char value[50]; // Max not defined by MAL
  mpl_param_element_t *param_p;
  mal_gpds_qos_profile_t qos_param;
  int ip_map = 0;

  assert(NULL != obj_p);

  switch (obj_p->state) {
  case pscc_bearer_mal_activating:
    break;
  default:
    PSCC_DBG_TRACE(LOG_ERR, "activated in unexpected state\n");
    return;
  }

  /* fetch ip_map to determine which address(es) are available for reading */
  if (mal_gpds_get_param(obj_p->mal_cid, MAL_PARAMS_IP_MAP_ID, &ip_map) < 0) {
    PSCC_DBG_TRACE(LOG_ERR, "mal_gpds_get_param(IP MAP)\n");
    goto error_return;
  }

  param_p = mpl_param_element_create(pscc_paramid_ip_map, &ip_map);
  if (NULL == param_p) {
    PSCC_DBG_TRACE(LOG_ERR, "failed to create parameter for ip map\n");
    goto error_return;
  }

  if (obj_p->set_param_cb(obj_p->parent, param_p) < 0) {
    PSCC_DBG_TRACE(LOG_ERR, "not able to store ip map\n");
    mpl_param_element_destroy(param_p);
    goto error_return;
  }
  PSCC_DBG_TRACE(LOG_INFO, "IP MAP is : %d\n", ip_map);

  if ((ip_map & MAL_GPDS_IPV4_ADDRESS_PRESENT) == MAL_GPDS_IPV4_ADDRESS_PRESENT) {
      PSCC_DBG_TRACE(LOG_INFO, "pscc_paramid_own_ip_address is present\n");
      store_address(obj_p, MAL_PARAMS_IP_ID, pscc_paramid_own_ip_address);
  } else {
      PSCC_DBG_TRACE(LOG_INFO, "pscc_paramid_own_ip_address is not present\n");
  }

  if ((ip_map & MAL_GPDS_IPV6_ADDRESS_PRESENT) == MAL_GPDS_IPV6_ADDRESS_PRESENT) {
      PSCC_DBG_TRACE(LOG_INFO, "pscc_paramid_own_ipv6_address is present\n");
      store_address(obj_p, MAL_PARAMS_IPV6_ID, pscc_paramid_own_ipv6_address);
  } else {
      PSCC_DBG_TRACE(LOG_INFO, "pscc_paramid_own_ipv6_address is not present\n");
  }

  if ((ip_map & MAL_GPDS_IPV4_PDNS_PRESENT) == MAL_GPDS_IPV4_PDNS_PRESENT) {
      PSCC_DBG_TRACE(LOG_INFO, "pscc_paramid_dns_address is present\n");
      store_address(obj_p, MAL_PARAMS_PDNS_ID, pscc_paramid_dns_address);
  } else {
      PSCC_DBG_TRACE(LOG_INFO, "pscc_paramid_dns_address is not present\n");
  }

  if ((ip_map & MAL_GPDS_IPV6_PDNS_PRESENT) == MAL_GPDS_IPV6_PDNS_PRESENT) {
      PSCC_DBG_TRACE(LOG_INFO, "pscc_paramid_ipv6_dns_address is present\n");
      store_address(obj_p, MAL_PARAMS_IPV6_PDNS_ID, pscc_paramid_ipv6_dns_address);
  } else {
      PSCC_DBG_TRACE(LOG_INFO, "pscc_paramid_ipv6_dns_address is not present\n");
  }

  if ((ip_map & MAL_GPDS_IPV4_SDNS_PRESENT) == MAL_GPDS_IPV4_SDNS_PRESENT) {
      PSCC_DBG_TRACE(LOG_INFO, "pscc_paramid_secondary_dns_address is present\n");
      store_address(obj_p, MAL_PARAMS_SDNS_ID, pscc_paramid_secondary_dns_address);
  } else {
      PSCC_DBG_TRACE(LOG_INFO, "pscc_paramid_secondary_dns_address is not present\n");
  }

  if ((ip_map & MAL_GPDS_IPV6_SDNS_PRESENT) == MAL_GPDS_IPV6_SDNS_PRESENT) {
      PSCC_DBG_TRACE(LOG_INFO, "pscc_paramid_ipv6_secondary_dns_address is present\n");
      store_address(obj_p, MAL_PARAMS_IPV6_SDNS_ID, pscc_paramid_ipv6_secondary_dns_address);
  } else {
      PSCC_DBG_TRACE(LOG_INFO, "pscc_paramid_ipv6_secondary_dns_address is not present\n");
  }

  /* Interface name */
  memset(&value, 0, sizeof(value));
  if (mal_gpds_get_param(obj_p->mal_cid, MAL_PARAMS_IF_NAME_ID, &value) < 0) {
    PSCC_DBG_TRACE(LOG_ERR, "mal_gpds_get_param(IF_NAME)\n");
    goto error_return;
  }

  param_p = mpl_param_element_create_stringn(pscc_paramid_netdev_name, value, strlen(value));
  if (NULL == param_p) {
    PSCC_DBG_TRACE(LOG_ERR, "failed to create parameter netdev_name address\n");
    goto error_return;
  }

  if (obj_p->set_param_cb(obj_p->parent, param_p) < 0) {
    PSCC_DBG_TRACE(LOG_ERR, "not able to store netdev_name parameter\n");
    mpl_param_element_destroy(param_p);
    goto error_return;
  }
  PSCC_DBG_TRACE(LOG_INFO, "netdev_name: %s\n",value);

  // Read out the qos params from the data object and set them to the mpl instance
  if (mal_gpds_get_param(obj_p->mal_cid, MAL_PARAMS_QOS_NEG_ID, (void*) &qos_param) < 0) {
    PSCC_DBG_TRACE(LOG_ERR, "failed to get negotiated QoS parameters\n");
    goto error_return;
  }

  PSCC_DBG_TRACE(LOG_INFO, "Setting qos parameters...\n");
  qos_mal_to_mpl(obj_p, (mal_gpds_qos_profile_t*) &qos_param);

  // All parameters read and set. Send back message to user of pscc:


  if (NULL == obj_p->event_cb) {
    PSCC_DBG_TRACE(LOG_INFO, "handle_event_activated called with event_cb NULL\n");
    goto error_return;
  }
  pscc_bearer_change_state(obj_p,pscc_bearer_mal_activated);
  PSCC_DBG_TRACE(LOG_INFO, "Activated, calling callback.\n");
  obj_p->event_cb(obj_p->parent, pscc_bearer_event_activated, 0, (params != NULL ? (int) (*(uint8_t*) params) : 0));
  return;

  error_return:
  pscc_bearer_change_state(obj_p,pscc_bearer_mal_deactivating);
  if (NULL != obj_p->event_cb)
    obj_p->event_cb(obj_p->parent, pscc_bearer_event_deactivating, 0, 0);

  if (deactivate_pdp(obj_p) < 0) {
    reset_bearer(obj_p, true);
  }
}

/**
 * handle_event_activation_failed
 **/
static void handle_event_activation_failed(pscc_bearer_t *obj_p, void *params)
{
  assert(NULL != obj_p);

  if (params != NULL)
    PSCC_DBG_TRACE(LOG_DEBUG, "cause %d\n",*(uint8_t*)params);

  switch (obj_p->state) {
  case pscc_bearer_mal_activating:
    pscc_bearer_change_state(obj_p,pscc_bearer_mal_idle);
    if (NULL != obj_p->event_cb)
      obj_p->event_cb(obj_p->parent, pscc_bearer_event_deactivated, pscc_reason_pdp_context_activation_failed,
          (params != NULL ? (int) (*(uint8_t*) params) : 0));
    break;
  default:
    PSCC_DBG_TRACE(LOG_ERR, "unexpected state\n");
    return;
  }
}

/**
 * handle_event_deactivated
 **/
static void handle_event_deactivated(pscc_bearer_t *obj_p, void *params)
{
  assert(NULL != obj_p);

  switch (obj_p->state) {
  case pscc_bearer_mal_activating:
  case pscc_bearer_mal_activated:
    obj_p->reason = pscc_reason_pdp_context_nw_deactivated;
    obj_p->cause = (params != NULL ? (int) (*(uint8_t*) params) : 0);
    reset_bearer(obj_p, true);
    break;
  case pscc_bearer_mal_deactivating:
    obj_p->reason = pscc_reason_pdp_context_ms_deactivated;
    obj_p->cause = (params != NULL ? (int) (*(uint8_t*) params) : 0);
    reset_bearer(obj_p, true);
    break;
  default:
    PSCC_DBG_TRACE(LOG_ERR, "unexpected state\n");
    break;
  }
}

/**
 * handle_event_data_counter
 **/
static void handle_event_data_counter(pscc_bearer_t *obj_p, void *params)
{
  assert(NULL != obj_p);
  mpl_param_element_t* param_p;
  mal_gpds_data_counter_info_t *mal_gpds_data_counter_info_p;
  mal_gpds_data_counter_info_p = (mal_gpds_data_counter_info_t*) params;
  uint64_t rx = 0;
  uint64_t tx = 0;
  uint32_t temp;
  uint64_t pow8 = 256; //2^8
  uint64_t pow32 = 4294967296LLU; //2^32
  int i;
  uint64_t multiplier = 1;

  /* convert to 64 bit values */
  for (i = 7; i>-1 ; i--) {
    rx+=mal_gpds_data_counter_info_p->rx_byte_count[i]*multiplier;
    tx+=mal_gpds_data_counter_info_p->tx_byte_count[i]*multiplier;
    multiplier *= pow8;
  }
  PSCC_DBG_TRACE(LOG_DEBUG, "handle_event_data_counter counters fetched: rx: %llu tx: %llu\n", rx, tx);

  /* check that we are in correct state */
  switch (obj_p->state) {
  case pscc_bearer_mal_activated:
    break;
  default:
    PSCC_DBG_TRACE(LOG_ERR, "unexpected state\n");
    return;
  }

  if (NULL == obj_p->event_cb) {
    PSCC_DBG_TRACE(LOG_INFO, "handle_event_data_counter called with event_cb NULL\n");
    return;
  }

  /* if fetching we need to calculate the offset and set the rx/tx datacounter in the handler */
  if (pscc_data_counter_state_fetching == obj_p->data_counter_state)
  {
    PSCC_DBG_TRACE(LOG_DEBUG, "data counter fetching event.\n");

    /* calculate the value since the last reset */
    rx -= (obj_p->rx_data_count_offset_lo + obj_p->rx_data_count_offset_hi * (pow32));
    tx -= (obj_p->tx_data_count_offset_lo + obj_p->tx_data_count_offset_hi * (pow32));

    PSCC_DBG_TRACE(LOG_DEBUG, "handle_event_data_counter counters after offset: rx: %llu tx: %llu\n", rx, tx);
    /* store the values */
    temp = rx%(pow32);
    param_p = mpl_param_element_create(pscc_paramid_rx_data_count_lo, &temp);
    if (obj_p->set_param_cb(obj_p->parent, param_p) < 0) {
      PSCC_DBG_TRACE(LOG_ERR, "not able to store rx data count lo\n");
      mpl_param_element_destroy(param_p);
    }
    temp = rx/(pow32);
    param_p = mpl_param_element_create(pscc_paramid_rx_data_count_hi, &temp);
    if (obj_p->set_param_cb(obj_p->parent, param_p) < 0) {
      PSCC_DBG_TRACE(LOG_ERR, "not able to store rx data count hi\n");
      mpl_param_element_destroy(param_p);
    }
    temp = tx%(pow32);
    param_p = mpl_param_element_create(pscc_paramid_tx_data_count_lo, &temp);
    if (obj_p->set_param_cb(obj_p->parent, param_p) < 0) {
      PSCC_DBG_TRACE(LOG_ERR, "not able to store tx data count lo\n");
      mpl_param_element_destroy(param_p);
    }
    temp = tx/(pow32);
    param_p = mpl_param_element_create(pscc_paramid_tx_data_count_hi, &temp);
    if (obj_p->set_param_cb(obj_p->parent, param_p) < 0) {
      PSCC_DBG_TRACE(LOG_ERR, "not able to store tx data count hi\n");
      mpl_param_element_destroy(param_p);
    }
    /* update the data counter state */
    obj_p->data_counter_state = pscc_data_counter_state_idle;
    /* send the callback to the handler */
    obj_p->event_cb(obj_p->parent, pscc_bearer_event_data_counters_fetched, 0, 0);
  }

  else if (pscc_data_counter_state_resetting == obj_p->data_counter_state)
  {
    PSCC_DBG_TRACE(LOG_DEBUG, "data counter resetting event.\n");
    /* update the data counter reset offset */
    obj_p->rx_data_count_offset_lo = (uint32_t) (rx%(pow32));
    obj_p->rx_data_count_offset_hi = (uint32_t) (rx/(pow32));
    obj_p->tx_data_count_offset_lo = (uint32_t) (tx%(pow32));
    obj_p->tx_data_count_offset_hi = (uint32_t) (tx/(pow32));
    PSCC_DBG_TRACE(LOG_DEBUG, "data counter offset: rx_lo: %d, rx_hi: %d, tx_lo: %d, tx_hi: %d\n", obj_p->rx_data_count_offset_lo, obj_p->rx_data_count_offset_hi, obj_p->tx_data_count_offset_lo, obj_p->tx_data_count_offset_hi);
    /* update the data counter state */
    obj_p->data_counter_state = pscc_data_counter_state_idle;
    /* send the callback to the handler */
    obj_p->event_cb(obj_p->parent, pscc_bearer_event_data_counters_reset, 0, 0);
  }

  else {
    PSCC_DBG_TRACE(LOG_ERR, "bearer object was not waiting for an data counter event?\n");
  }

  return;
}

/*
 * Takes the QoS parameters in qos_p and puts them into the mpl object in obj_p.
 */
static void qos_mal_to_mpl(pscc_bearer_t *obj_p, mal_gpds_qos_profile_t* qos_p)
{
  mpl_param_element_t* param_p;
  uint8_t bitrate_base = 0;
  uint8_t bitrate_ext = 0;

  // Temporary variables used when converting parameters.
  pscc_qos_residual_ber_t res_ber = 0;
  pscc_qos_sdu_error_ratio_t sdu_err = 0;
  uint8_t mpl_int = 0;

  assert(NULL != obj_p);
  assert(NULL != obj_p->set_param_cb);
  assert(NULL != obj_p->delete_param_cb);
  assert(qos_p != NULL);

  //Delete the old parameters to avoid memory leakage:
  obj_p->delete_param_cb(obj_p->parent, pscc_paramid_neg_qos_traffic_class);
  obj_p->delete_param_cb(obj_p->parent, pscc_paramid_neg_qos_delivery_order);
  obj_p->delete_param_cb(obj_p->parent, pscc_paramid_neg_qos_delivery_erroneous_sdu);
  obj_p->delete_param_cb(obj_p->parent, pscc_paramid_neg_qos_max_sdu_size);
  obj_p->delete_param_cb(obj_p->parent, pscc_paramid_neg_qos_max_bitrate_uplink);
  obj_p->delete_param_cb(obj_p->parent, pscc_paramid_neg_qos_max_bitrate_downlink);
  obj_p->delete_param_cb(obj_p->parent, pscc_paramid_neg_qos_residual_ber);
  obj_p->delete_param_cb(obj_p->parent, pscc_paramid_neg_qos_sdu_error_ratio);
  obj_p->delete_param_cb(obj_p->parent, pscc_paramid_neg_qos_transfer_delay);
  obj_p->delete_param_cb(obj_p->parent, pscc_paramid_neg_qos_traffic_handling_priority);
  obj_p->delete_param_cb(obj_p->parent, pscc_paramid_neg_qos_guaranteed_bitrate_uplink);
  obj_p->delete_param_cb(obj_p->parent, pscc_paramid_neg_qos_guaranteed_bitrate_downlink);
  obj_p->delete_param_cb(obj_p->parent, pscc_paramid_neg_qos_signalling_indication);
  obj_p->delete_param_cb(obj_p->parent, pscc_paramid_neg_qos_source_statistics_descriptor);
  obj_p->delete_param_cb(obj_p->parent, pscc_paramid_neg_qos_extended_max_bitrate_downlink);
  obj_p->delete_param_cb(obj_p->parent, pscc_paramid_neg_qos_extended_guaranteed_bitrate_downlink);
  obj_p->delete_param_cb(obj_p->parent, pscc_paramid_neg_qos_extended_max_bitrate_uplink);
  obj_p->delete_param_cb(obj_p->parent, pscc_paramid_neg_qos_extended_guaranteed_bitrate_uplink);

  param_p = mpl_param_element_create(pscc_paramid_neg_qos_traffic_class, &qos_p->traffic_class);
  if (obj_p->set_param_cb(obj_p->parent, param_p) < 0) {
    PSCC_DBG_TRACE(LOG_ERR, "not able to store neg_qos_traffic_class\n");
    mpl_param_element_destroy(param_p);
  }
  param_p = mpl_param_element_create(pscc_paramid_neg_qos_delivery_order, &qos_p->delivery_order);
  if (obj_p->set_param_cb(obj_p->parent, param_p) < 0) {
    PSCC_DBG_TRACE(LOG_ERR, "not able to store neg_qos_delivery_order\n");
    mpl_param_element_destroy(param_p);
  }
  param_p = mpl_param_element_create(pscc_paramid_neg_qos_delivery_erroneous_sdu, &qos_p->delivery_of_error_sdu);
  if (obj_p->set_param_cb(obj_p->parent, param_p) < 0) {
    PSCC_DBG_TRACE(LOG_ERR, "not able to store neg_qos_delivery_erroneous_sdu\n");
    mpl_param_element_destroy(param_p);
  }

  res_ber = pscc_mqpc_res_ber_to_mpl(qos_p->residual_ber);
  param_p = mpl_param_element_create(pscc_paramid_neg_qos_residual_ber, &res_ber);
  if (obj_p->set_param_cb(obj_p->parent, param_p) < 0) {
    PSCC_DBG_TRACE(LOG_ERR, "not able to store neg_qos_residual_ber\n");
    mpl_param_element_destroy(param_p);
  }

  sdu_err = pscc_mqpc_sdu_err_ratio_to_mpl(qos_p->sdu_error_ratio);
  param_p = mpl_param_element_create(pscc_paramid_neg_qos_sdu_error_ratio, &sdu_err);
  if (obj_p->set_param_cb(obj_p->parent, param_p) < 0) {
    PSCC_DBG_TRACE(LOG_ERR, "not able to store neg_qos_sdu_error_ratio\n");
    mpl_param_element_destroy(param_p);
  }
  param_p = mpl_param_element_create(pscc_paramid_neg_qos_traffic_handling_priority, &qos_p->trafic_handling_priority);
  if (obj_p->set_param_cb(obj_p->parent, param_p) < 0) {
    PSCC_DBG_TRACE(LOG_ERR, "not able to store neg_qos_traffic_handling_priority\n");
    mpl_param_element_destroy(param_p);
  }

  mpl_int = pscc_mqpc_transfer_delay_to_mpl(qos_p->transfer_delay);
  param_p = mpl_param_element_create(pscc_paramid_neg_qos_transfer_delay, &mpl_int);
  if (obj_p->set_param_cb(obj_p->parent, param_p) < 0) {
    PSCC_DBG_TRACE(LOG_ERR, "not able to store neg_qos_transfer_delay\n");
    mpl_param_element_destroy(param_p);
  }

  mpl_int = pscc_mqpc_max_sdu_to_mpl(qos_p->maximum_sdu_size);
  param_p = mpl_param_element_create(pscc_paramid_neg_qos_max_sdu_size, &mpl_int);
  if (obj_p->set_param_cb(obj_p->parent, param_p) < 0) {
    PSCC_DBG_TRACE(LOG_ERR, "not able to store neg_qos_max_sdu_size\n");
    mpl_param_element_destroy(param_p);
  }

  pscc_mqpc_bitrate_to_mpl(qos_p->maximum_bitrate_ul, &bitrate_base, &bitrate_ext);
  param_p = mpl_param_element_create(pscc_paramid_neg_qos_max_bitrate_uplink, &bitrate_base);
  if (obj_p->set_param_cb(obj_p->parent, param_p) < 0) {
    PSCC_DBG_TRACE(LOG_ERR, "not able to store neg_qos_max_bitrate_uplink\n");
    mpl_param_element_destroy(param_p);
  }
  param_p = mpl_param_element_create(pscc_paramid_neg_qos_extended_max_bitrate_uplink, &bitrate_ext);
  if (obj_p->set_param_cb(obj_p->parent, param_p) < 0) {
    PSCC_DBG_TRACE(LOG_ERR, "not able to store neg_qos_extended_max_bitrate_uplink\n");
    mpl_param_element_destroy(param_p);
  }

  pscc_mqpc_bitrate_to_mpl(qos_p->maximum_bitrate_dl, &bitrate_base, &bitrate_ext);
  param_p = mpl_param_element_create(pscc_paramid_neg_qos_max_bitrate_downlink, &bitrate_base);
  if (obj_p->set_param_cb(obj_p->parent, param_p) < 0) {
    PSCC_DBG_TRACE(LOG_ERR, "not able to store neg_qos_max_bitrate_downlink\n");
    mpl_param_element_destroy(param_p);
  }
  param_p = mpl_param_element_create(pscc_paramid_neg_qos_extended_max_bitrate_downlink, &bitrate_ext);
  if (obj_p->set_param_cb(obj_p->parent, param_p) < 0) {
    PSCC_DBG_TRACE(LOG_ERR, "not able to store neg_qos_extended_max_bitrate_downlink\n");
    mpl_param_element_destroy(param_p);
  }

  pscc_mqpc_bitrate_to_mpl(qos_p->guaranteed_bitrate_ul, &bitrate_base, &bitrate_ext);
  param_p = mpl_param_element_create(pscc_paramid_neg_qos_guaranteed_bitrate_uplink, &bitrate_base);
  if (obj_p->set_param_cb(obj_p->parent, param_p) < 0) {
    PSCC_DBG_TRACE(LOG_ERR, "not able to store neg_qos_guaranteed_bitrate_uplink\n");
    mpl_param_element_destroy(param_p);
  }
  param_p = mpl_param_element_create(pscc_paramid_neg_qos_extended_guaranteed_bitrate_uplink, &bitrate_ext);
  if (obj_p->set_param_cb(obj_p->parent, param_p) < 0) {
    PSCC_DBG_TRACE(LOG_ERR, "not able to store neg_qos_extended_guaranteed_bitrate_uplink\n");
    mpl_param_element_destroy(param_p);
  }

  pscc_mqpc_bitrate_to_mpl(qos_p->guaranteed_bitrate_dl, &bitrate_base, &bitrate_ext);
  param_p = mpl_param_element_create(pscc_paramid_neg_qos_guaranteed_bitrate_downlink, &bitrate_base);
  if (obj_p->set_param_cb(obj_p->parent, param_p) < 0) {
    PSCC_DBG_TRACE(LOG_ERR, "not able to store neg_qos_guaranteed_bitrate_downlink\n");
    mpl_param_element_destroy(param_p);
  }
  param_p = mpl_param_element_create(pscc_paramid_neg_qos_extended_guaranteed_bitrate_downlink, &bitrate_ext);
  if (obj_p->set_param_cb(obj_p->parent, param_p) < 0) {
    PSCC_DBG_TRACE(LOG_ERR, "not able to store neg_qos_extended_guaranteed_bitrate_downlink\n");
    mpl_param_element_destroy(param_p);
  }

  param_p = mpl_param_element_create(pscc_paramid_neg_qos_signalling_indication, &qos_p->sgn_ind);
  if (obj_p->set_param_cb(obj_p->parent, param_p) < 0) {
    PSCC_DBG_TRACE(LOG_ERR, "not able to store neg_qos_signalling_indication\n");
    mpl_param_element_destroy(param_p);
  }
  param_p = mpl_param_element_create(pscc_paramid_neg_qos_source_statistics_descriptor, &qos_p->src_stat_desc);
  if (obj_p->set_param_cb(obj_p->parent, param_p) < 0) {
    PSCC_DBG_TRACE(LOG_ERR, "not able to store neg_qos_source_statistics_descriptor\n");
    mpl_param_element_destroy(param_p);
  }
}

/*
 * Reads the QoS parameters from mpl (obj_p) and puts them into a MAL-formatted qos object.
 * If req is true, req-parameters will be used. If false, min-parameters will be used.
 */
static void qos_mpl_to_mal(pscc_bearer_t *obj_p, mal_gpds_qos_profile_t* qos_p, bool req)
{
  mpl_param_element_t* param_p;
  uint8_t linkspeed_base = 0;
  uint8_t linkspeed_extended = 0;
  pscc_qos_type_t qos_type = get_qos_type(obj_p);

  /* empty the mal qos struct: */
  memset(qos_p, 0, sizeof(mal_gpds_qos_profile_t));

  if (qos_type == pscc_qos_type_rel97) {
    PSCC_COMPILE_CHECK_ALL_REL97_QOS_VALUES();

    /* Use Rel97 params. */
    qos_p->qos_type = MAL_GPDS_QOS_REL97_98;

    /* Precedence class */
    param_p = obj_p->get_param_cb(obj_p->parent, req ? pscc_paramid_req_qos_precedence_class : pscc_paramid_min_qos_precedence_class);
    if (NULL != param_p)
      qos_p->precedence_class = *((mal_gpds_qos_precedence_class_t*) param_p->value_p);

    /* Delay class */
    param_p = obj_p->get_param_cb(obj_p->parent, req ? pscc_paramid_req_qos_delay_class : pscc_paramid_min_qos_delay_class);
    if (NULL != param_p)
      qos_p->delay_class = *((mal_gpds_qos_delay_class_t*) param_p->value_p);

    /* Reliability class */
    param_p = obj_p->get_param_cb(obj_p->parent, req ? pscc_paramid_req_qos_reliability_class : pscc_paramid_min_qos_reliability_class);
    if (NULL != param_p)
      qos_p->reliability_class = *((mal_gpds_qos_reliability_class_t*) param_p->value_p);

    /* Peak Throughput class */
    param_p = obj_p->get_param_cb(obj_p->parent, req ? pscc_paramid_req_qos_peak_throughput_class : pscc_paramid_min_qos_peak_throughput_class);
    if (NULL != param_p)
      qos_p->peak_throughput_class = *((mal_gpds_qos_peak_throughput_class_t*) param_p->value_p);

    /* Mean Throughput class */
    param_p = obj_p->get_param_cb(obj_p->parent, req ? pscc_paramid_req_qos_mean_throughput_class : pscc_paramid_min_qos_mean_throughput_class);
    if (NULL != param_p)
     qos_p->mean_throughput_class = pscc_mqpc_mean_throughput_class_to_mal(*((pscc_qos_mean_throughput_class_t*) (param_p->value_p)));

  }
  else {
    PSCC_COMPILE_CHECK_ALL_REL99_QOS_VALUES();

    /* Only Release 99 of the specification will be used for MAL. */
    qos_p->qos_type = MAL_GPDS_QOS_REL99;

    /* read out every qos parameter and put into the profile struct: */
    param_p = obj_p->get_param_cb(obj_p->parent, req ? pscc_paramid_req_qos_traffic_class : pscc_paramid_min_qos_traffic_class);
    if (NULL != param_p)
      qos_p->traffic_class = *((mal_gpds_qos_traffic_class_t*) param_p->value_p);

    param_p = obj_p->get_param_cb(obj_p->parent, req ? pscc_paramid_req_qos_delivery_order : pscc_paramid_min_qos_delivery_order);
    if (NULL != param_p)
      qos_p->delivery_order = *((mal_gpds_qos_delivery_order_t*) param_p->value_p);

    param_p = obj_p->get_param_cb(obj_p->parent, req ? pscc_paramid_req_qos_delivery_erroneous_sdu
        : pscc_paramid_min_qos_delivery_erroneous_sdu);
    if (NULL != param_p)
      qos_p->delivery_of_error_sdu = *((mal_gpds_qos_delivery_of_error_sdu_t*) param_p->value_p);

    param_p = obj_p->get_param_cb(obj_p->parent, req ? pscc_paramid_req_qos_residual_ber : pscc_paramid_min_qos_residual_ber);
    if (NULL != param_p)
      qos_p->residual_ber = pscc_mqpc_res_ber_to_mal(*((pscc_qos_residual_ber_t*) param_p->value_p));

    param_p = obj_p->get_param_cb(obj_p->parent, req ? pscc_paramid_req_qos_sdu_error_ratio : pscc_paramid_min_qos_sdu_error_ratio);
    if (NULL != param_p)
     qos_p->sdu_error_ratio = pscc_mqpc_sdu_err_ratio_to_mal(*((pscc_qos_sdu_error_ratio_t*) (param_p->value_p)));

    param_p = obj_p->get_param_cb(obj_p->parent, req ? pscc_paramid_req_qos_traffic_handling_priority
       : pscc_paramid_min_qos_traffic_handling_priority);
    if (NULL != param_p)
      qos_p->trafic_handling_priority = *((mal_gpds_qos_trafic_handling_priority_t*) param_p->value_p);

    param_p = obj_p->get_param_cb(obj_p->parent, req ? pscc_paramid_req_qos_transfer_delay : pscc_paramid_min_qos_transfer_delay);
    if (NULL != param_p)
      qos_p->transfer_delay = pscc_mqpc_transfer_delay_to_mal(*((uint8_t*) param_p->value_p));

    param_p = obj_p->get_param_cb(obj_p->parent, req ? pscc_paramid_req_qos_max_sdu_size : pscc_paramid_min_qos_max_sdu_size);
    if (NULL != param_p)
      qos_p->maximum_sdu_size = pscc_mqpc_max_sdu_to_mal(*((uint8_t*) param_p->value_p));

    /* OK */
    param_p = obj_p->get_param_cb(obj_p->parent, req ? pscc_paramid_req_qos_max_bitrate_uplink : pscc_paramid_min_qos_max_bitrate_uplink);
   if (NULL != param_p) {
      linkspeed_base = *((uint8_t*) param_p->value_p);
      param_p = obj_p->get_param_cb(obj_p->parent, req ? pscc_paramid_req_qos_extended_max_bitrate_uplink
          : pscc_paramid_min_qos_extended_max_bitrate_uplink);
      if (NULL != param_p)
        linkspeed_extended = *((uint8_t*) param_p->value_p);
      else
        /* if linkspeed is not set in pscc, default to 0 */
        linkspeed_extended = 0;

      qos_p->maximum_bitrate_ul = return_least(pscc_mqpc_bitrate_to_mal(linkspeed_base, linkspeed_extended), maximum_uplink_speed);
    }

    /* OK */
    param_p = obj_p->get_param_cb(obj_p->parent, req ? pscc_paramid_req_qos_max_bitrate_downlink : pscc_paramid_min_qos_max_bitrate_downlink);
    if (NULL != param_p) {
      linkspeed_base = *((uint8_t*) param_p->value_p);
      param_p = obj_p->get_param_cb(obj_p->parent, req ? pscc_paramid_req_qos_extended_max_bitrate_downlink
          : pscc_paramid_min_qos_extended_max_bitrate_downlink);
      if (NULL != param_p)
        linkspeed_extended = *((uint8_t*) param_p->value_p);
      else
        /* if linkspeed is not set in pscc, default to 0 */
        linkspeed_extended = 0;

      qos_p->maximum_bitrate_dl = return_least(pscc_mqpc_bitrate_to_mal(linkspeed_base, linkspeed_extended), maximum_downlink_speed);
    }

    /* OK */
    param_p = obj_p->get_param_cb(obj_p->parent, req ? pscc_paramid_req_qos_guaranteed_bitrate_uplink
        : pscc_paramid_min_qos_guaranteed_bitrate_uplink);
   if (NULL != param_p) {
      linkspeed_base = *((uint8_t*) param_p->value_p);
      param_p = obj_p->get_param_cb(obj_p->parent, req ? pscc_paramid_req_qos_extended_guaranteed_bitrate_uplink
          : pscc_paramid_min_qos_extended_guaranteed_bitrate_uplink);
      if (NULL != param_p)
        linkspeed_extended = *((uint8_t*) param_p->value_p);
      else
        /* if linkspeed is not set in pscc, default to 0 */
        linkspeed_extended = 0;

      qos_p->guaranteed_bitrate_ul = return_least(pscc_mqpc_bitrate_to_mal(linkspeed_base, linkspeed_extended), maximum_uplink_speed);
    }

    param_p = obj_p->get_param_cb(obj_p->parent, req ? pscc_paramid_req_qos_guaranteed_bitrate_downlink
        : pscc_paramid_min_qos_guaranteed_bitrate_downlink);
    if (NULL != param_p) {
      linkspeed_base = *((uint8_t*) param_p->value_p);
      param_p = obj_p->get_param_cb(obj_p->parent, req ? pscc_paramid_req_qos_extended_guaranteed_bitrate_downlink
          : pscc_paramid_min_qos_extended_guaranteed_bitrate_downlink);
      if (NULL != param_p)
        linkspeed_extended = *((uint8_t*) param_p->value_p);
      else
        /* if linkspeed is not set in pscc, default to 0 */
        linkspeed_extended = 0;

      qos_p->guaranteed_bitrate_dl = return_least(pscc_mqpc_bitrate_to_mal(linkspeed_base, linkspeed_extended), maximum_downlink_speed);
    }

    param_p = obj_p->get_param_cb(obj_p->parent, req ? pscc_paramid_req_qos_signalling_indication
        : pscc_paramid_min_qos_signalling_indication);
    if (NULL != param_p)
      qos_p->sgn_ind = *((mal_gpds_qos_sgn_indication_t *) param_p->value_p);

    param_p = obj_p->get_param_cb(obj_p->parent, req ? pscc_paramid_req_qos_source_statistics_descriptor
        : pscc_paramid_min_qos_source_statistics_descriptor);
    if (NULL != param_p)
      qos_p->src_stat_desc = *((mal_gpds_qos_src_stat_descriptor_t*) param_p->value_p);
  }
}

/**
 * Handles the event context modified
 *
 * If the state is active, it means that the parameters have been renegotiated or returned, parse and pass them to the user of pscc.
 */
static void handle_event_context_modified(pscc_bearer_t *obj_p, void *params)
{
  mal_gpds_qos_profile_t qos_param;

  assert(NULL != obj_p);
  PSCC_IDENTIFIER_NOT_USED(params);

  if (NULL == obj_p->event_cb) {
    PSCC_DBG_TRACE(LOG_ERR, "No event callback set, cannot handle_event_context_modified\n");
    return;
  }

  // Read out the qos params from the data object and set them to the mpl instance
  if (mal_gpds_get_param(obj_p->mal_cid, MAL_PARAMS_QOS_NEG_ID, (void*) &qos_param) < 0) {
    PSCC_DBG_TRACE(LOG_ERR, "failed to get negotiated QoS parameters\n");
    return;
  }

  PSCC_DBG_TRACE(LOG_INFO, "Setting qos parameters...\n");
  qos_mal_to_mpl(obj_p, (mal_gpds_qos_profile_t*) &qos_param);

  if (pscc_bearer_mal_activated == obj_p->state) {
    PSCC_DBG_TRACE(LOG_DEBUG, " handle_event_context_modified received while in state pscc_bearer_mal_activated\n");
    obj_p->event_cb(obj_p->parent, pscc_bearer_event_modified, 0, 0);
  } else {
    PSCC_DBG_TRACE(LOG_ERR, " handle_event_context_modified received while in illegal state: %d\n", obj_p->state);
  }

  return;
}

/**
 * Handles the resource control events
 *
 */
static int handle_resource_control_event (mal_gpds_event_ids_t event_id, void *params)
{
  switch (event_id)
  {
  /* event sent either when gpds is ready for rc configuration or when it has been reconfigured */
  case MAL_GPDS_EVENT_ID_RESOURCE_CONFIGURED:
  {
    mal_gpds_configuration_status_t *status_p = (mal_gpds_configuration_status_t*) params;
    PSCC_DBG_TRACE(LOG_DEBUG, "MAL_GPDS_EVENT_ID_RESOURCE_CONFIGURED(): %d\n", *status_p);
    if (MAL_GPDS_RESOURCE_CONF_READY == *status_p)
    {
      /* if this event comes, we try to do a resource control enabling */
      if (pscc_bearer_configure_rc(pscc_bearer_rc_event_cb) < 0)
        PSCC_DBG_TRACE(LOG_ERR, "Failed to configure resource control\n");
    }
    else
    {
      PSCC_DBG_TRACE(LOG_DEBUG, "Resource configuration not ready at GPDS start up\n");
    }
  }
    break;

  /* event sent if the rc configuration failed */
  case MAL_GPDS_EVENT_ID_RESOURCE_CONF_FAILED:
    PSCC_DBG_TRACE(LOG_DEBUG, "MAL_GPDS_EVENT_ID_RESOURCE_CONF_FAILED()\n");
    PSCC_DBG_TRACE(LOG_ERR, "Failed to configure resource control\n");
    break;

  /* event sent when gpds wants us to perform resource control */
  case MAL_GPDS_EVENT_ID_RESOURCE_CONTROL_REQUESTED:
  {
    mal_gpds_resource_control_info_t *resource_control_info_p;
    pscc_rc_t pscc_rc;

    PSCC_DBG_TRACE(LOG_DEBUG, "MAL_GPDS_EVENT_ID_RESOURCE_CONTROL_REQUESTED()\n");

    memset(&pscc_rc, 0, sizeof(pscc_rc_t));
    resource_control_info_p = (mal_gpds_resource_control_info_t*) params;

    /* store data pointer, data lenght and rc sequence id */
    pscc_rc.data_p = resource_control_info_p->resource_control_data.data;
    pscc_rc.length = resource_control_info_p->resource_control_data.data_length;
    pscc_rc.seq_id = resource_control_info_p->seq_id;

    if (pscc_bearer_rc_event_cb == NULL)
      PSCC_DBG_TRACE(LOG_ERR, "handle_event_resource_control_request received when callback pointer hasnt been registered\n");
    else
      pscc_bearer_rc_event_cb(pscc_bearer_rc_event_requested, &pscc_rc);
  }
    break;

  /* event sent when gpds has accepted the resource control */
  case MAL_GPDS_EVENT_ID_RESOURCE_CONTROL_DONE:
    PSCC_DBG_TRACE(LOG_DEBUG, "MAL_GPDS_EVENT_ID_RESOURCE_CONTROL_DONE()\n");
    pscc_bearer_rc_event_cb(pscc_bearer_rc_event_done, NULL);
    break;

  /* event sent when gpds has rejected the resource control */
  case MAL_GPDS_EVENT_ID_RESOURCE_CONTROL_FAILED:
    PSCC_DBG_TRACE(LOG_DEBUG, "MAL_GPDS_EVENT_ID_RESOURCE_CONTROL_FAILED()\n");
    pscc_bearer_rc_event_cb(pscc_bearer_rc_event_failed, NULL);
    break;

  default:
    return (-1);
    break;
  }

  return (0);
}

/**
 * Handles the gprs attach related events
 *
 */
static int handle_attach_event (mal_gpds_event_ids_t event_id, void *params)
{
  pscc_bearer_attach_event_t attach_event;
  pscc_attach_event_params_t attach_event_params;
  /* if extra parameters is provided pass it along as cause, otherwise pass 0 */
  int cause = (params != NULL ? (int) (*(uint8_t*) params) : 0);

  memset(&attach_event_params, 0, sizeof(pscc_attach_event_params_t));

  switch (event_id)
  {
  case MAL_GPDS_EVENT_ID_PS_ATTACHED:
    PSCC_DBG_TRACE(LOG_DEBUG, "MAL_GPDS_EVENT_ID_PS_ATTACHED()\n");
    attach_event = pscc_bearer_attach_event_attached;
    pscc_mal_attach_status = pscc_attach_status_attached;
    break;
  case MAL_GPDS_EVENT_ID_PS_ATTACH_FAILED:
    PSCC_DBG_TRACE(LOG_DEBUG, "MAL_GPDS_EVENT_ID_PS_ATTACH_FAILED()\n");
    attach_event = pscc_bearer_attach_event_attach_failed;
    pscc_mal_attach_status = pscc_attach_status_detached;
    break;
  case MAL_GPDS_EVENT_ID_PS_DETACHED:
    PSCC_DBG_TRACE(LOG_DEBUG, "MAL_GPDS_EVENT_ID_PS_DETACHED()\n");
    attach_event = pscc_bearer_attach_event_detached;
    pscc_mal_attach_status = pscc_attach_status_detached;
    break;
  case MAL_GPDS_EVENT_ID_PS_NWI_DETACHED:
    PSCC_DBG_TRACE(LOG_DEBUG, "MAL_GPDS_EVENT_ID_PS_NWI_DETACHED()\n");
    attach_event = pscc_bearer_attach_event_nwi_detached;
    pscc_mal_attach_status = pscc_attach_status_detached;
    /* Since detached cause is passed through a struct here we need to extract it */
    if (params != NULL)
    {
      cause = ((mal_gpds_detach_info_t*) params)->detach_cause;
    }
    break;
  case MAL_GPDS_EVENT_ID_PS_DETACH_FAILED:
    PSCC_DBG_TRACE(LOG_DEBUG, "MAL_GPDS_EVENT_ID_PS_DETACH_FAILED()\n");
    attach_event = pscc_bearer_attach_event_detach_failed;
    pscc_mal_attach_status = pscc_attach_status_attached;
    break;

  case MAL_GPDS_EVENT_ID_CONFIG_QUERIED:
  {
    mal_gpds_config_t* param_p = (mal_gpds_config_t*) params;
    void* source_func = config_source_func;
    config_source_func = NULL;

    PSCC_DBG_TRACE(LOG_DEBUG, "MAL_GPDS_EVENT_ID_CONFIG_QUERIED()\n");
    if (!param_p) {
        PSCC_DBG_TRACE(LOG_ERR, "MAL_GPDS_EVENT_ID_CONFIG_QUERIED with params == NULL\n");
    }

    if (source_func == pscc_bearer_query_attach_mode) {
      PSCC_DBG_TRACE(LOG_DEBUG, "attach mode queried (%d)\n", ((param_p) ? (int)param_p->ps_attach_mode : -1));

      attach_event = pscc_bearer_attach_event_mode_queried;
      if (param_p && param_p->ps_attach_mode == MAL_GPDS_PS_ATTACH_AUTOMATIC) {
        attach_event_params.attach_mode = pscc_attach_mode_automatic;
      } else {
        /*
         *  Attach mode is either manual or unknown due to error in underlying layers,
         *  in which case we assume it is manual. In any case we have to send an event in order
         *  to prevent our client from hanging.
         */
        attach_event_params.attach_mode = pscc_attach_mode_manual;
      }

    } else if (source_func == pscc_bearer_query_ping_block_mode) {
      PSCC_DBG_TRACE(LOG_DEBUG, "icmp drop mode queried (%d)\n", ((param_p) ? (int)param_p->drop_icmp_mode : -1));

      if (pscc_bearer_ping_mode_event_cb) {
        /*
         *  If param_p is NULL, then assume drop mode is disabled, otherwise set according to flag.
         */
        pscc_ping_block_mode_t mode = pscc_ping_block_mode_disabled;
        if (param_p && param_p->drop_icmp_mode == MAL_GPDS_ICMP_DROP_MODE_ENABLED) {
          mode = pscc_ping_block_mode_enabled;
        }
        pscc_bearer_ping_mode_event_cb(pscc_bearer_ping_block_mode_queried_event, mode, attach_request_ct);
      } else {
        PSCC_DBG_TRACE(LOG_ERR, "Can't send pscc_bearer_ping_block_mode_queried_event when callback is NULL\n");
      }
      attach_ct_set = false;
      attach_request_ct = 0;
      pscc_bearer_ping_mode_event_cb = NULL;
      return 0;

    } else {
        PSCC_DBG_TRACE(LOG_DEBUG, "MAL_GPDS_EVENT_ID_CONFIG_QUERIED caused by unkown source\n");
        return -1; /* Event not handled by this function, return -1 to allow other handler to have a go at it*/
    }
    break;
  }
  case MAL_GPDS_EVENT_ID_CONFIGURED:
  {
    mal_gpds_config_t* param_p = (mal_gpds_config_t*) params;
    void* source_func = config_source_func;
    config_source_func = NULL;

    PSCC_DBG_TRACE(LOG_DEBUG, "MAL_GPDS_EVENT_ID_CONFIGURED()\n");
    if (!param_p) {
      PSCC_DBG_TRACE(LOG_ERR, "MAL_GPDS_EVENT_ID_CONFIGURED with params == NULL\n");
    }

    /*
     * Check if AOL configuration has been requested (at least once)
     */
    if (expected_aol_mode != MAL_GPDS_AOL_CONTEXT_DEFAULT) {
      pscc_aol_conf_state_t old_state = pending_aol_mode;
      mal_gpds_aol_context_mode_t mode = (mal_gpds_aol_context_mode_t)-1;
      if (param_p) {
        mode = param_p->aol_mode;
        if (mode != MAL_GPDS_AOL_CONTEXT_DEFAULT) {
          if (mode == expected_aol_mode) {
            pending_aol_mode = pscc_aol_conf_state_idle;
          } else {
            pending_aol_mode = pscc_aol_conf_state_failed;
          }
        }
      }
      if (pending_aol_mode != old_state) {
          PSCC_DBG_TRACE(LOG_DEBUG, "configure aol mode change: state %d, mode %d)\n", pending_aol_mode, mode);
      }
    }

    if (source_func == pscc_bearer_configure_attach_mode) {
      PSCC_DBG_TRACE(LOG_DEBUG, "configure attach mode (%d)\n", ((param_p) ? (int)param_p->ps_attach_mode : -1));
      if (param_p && param_p->ps_attach_mode == expected_attach_mode) {
        attach_event = pscc_bearer_attach_event_mode_configured;
      } else {
        PSCC_DBG_TRACE(LOG_ERR, "Received configured attach mode was not the expected (%d)\n", expected_attach_mode);
        attach_event = pscc_bearer_attach_event_mode_configure_failed;
      }
      expected_attach_mode = MAL_GPDS_PS_ATTACH_DEFAULT;
    } else if (source_func == pscc_bearer_configure_ping_block_mode) {
      PSCC_DBG_TRACE(LOG_DEBUG, "configure icmp drop mode (%d)\n", ((param_p) ? (int)param_p->drop_icmp_mode : -1));
      if (pscc_bearer_ping_mode_event_cb) {
        pscc_ping_block_mode_t mode = pscc_ping_block_mode_disabled;
        if (param_p && param_p->drop_icmp_mode == expected_ping_block_mode) {
          if (param_p->drop_icmp_mode == MAL_GPDS_ICMP_DROP_MODE_ENABLED) {
            mode = pscc_ping_block_mode_enabled;
          }
          pscc_bearer_ping_mode_event_cb(pscc_bearer_ping_block_mode_configured_event, mode, attach_request_ct);
        } else {
          PSCC_DBG_TRACE(LOG_ERR, "Received configured icmp drop mode was not the expected (%d)\n", expected_ping_block_mode);
          pscc_bearer_ping_mode_event_cb(pscc_bearer_ping_block_mode_configure_failed_event, mode, attach_request_ct);
        }
      } else {
        PSCC_DBG_TRACE(LOG_ERR, "Can't send pscc_bearer_ping_block_mode_configured_event when callback is NULL\n");
      }
      attach_ct_set = false;
      attach_request_ct = 0;
      pscc_bearer_ping_mode_event_cb = NULL;
      expected_ping_block_mode = MAL_GPDS_ICMP_DROP_MODE_KEEP_EXISTING;
      return 0;

    } else {
      PSCC_DBG_TRACE(LOG_DEBUG, "MAL_GPDS_EVENT_ID_CONFIGURED caused by unkown source\n");
      return -1; /* Event not handled by this function, return -1 to allow other handler to have a go at it*/
    }
    break;
  }
  case MAL_GPDS_EVENT_ID_CONFIGURATION_FAILED:
  {
    void* source_func = config_source_func;
    config_source_func = NULL;

    PSCC_DBG_TRACE(LOG_DEBUG, "MAL_GPDS_EVENT_ID_CONFIGURATION_FAILED()\n");

    if (pending_aol_mode  == pscc_aol_conf_state_pending ) {
      PSCC_DBG_TRACE(LOG_ERR, "MAL_GPDS_AOL_CONTEXT_ENABLE configuration returned failed\n");
      pending_aol_mode = pscc_aol_conf_state_failed;
    }

    if (source_func == pscc_bearer_configure_attach_mode) {
      PSCC_DBG_TRACE(LOG_DEBUG, "configure attach mode failed\n");
      attach_event = pscc_bearer_attach_event_mode_configure_failed;

    } else if (source_func == pscc_bearer_configure_ping_block_mode) {
      PSCC_DBG_TRACE(LOG_DEBUG, "configure icmp drop mode failed\n");
      if (pscc_bearer_ping_mode_event_cb) {
        pscc_bearer_ping_mode_event_cb(pscc_bearer_ping_block_mode_configure_failed_event,
                                       pscc_ping_block_mode_disabled, attach_request_ct);
      } else {
        PSCC_DBG_TRACE(LOG_ERR, "Can't send pscc_bearer_ping_block_mode_configure_failed_event when callback is NULL\n");
      }
      attach_ct_set = false;
      attach_request_ct = 0;
      pscc_bearer_ping_mode_event_cb = NULL;
      expected_ping_block_mode = MAL_GPDS_ICMP_DROP_MODE_KEEP_EXISTING;
      return 0;

    } else {
      PSCC_DBG_TRACE(LOG_DEBUG, "MAL_GPDS_EVENT_ID_CONFIGURATION_FAILED caused by unkown source\n");
      return -1; /* Event not handled by this function, return -1 to allow other handler to have a go at it*/
    }
    break;
  }
  case MAL_GPDS_EVENT_ID_PS_ATTACH_STATUS:
    if (params != NULL) {
      if (*(mal_gpds_ps_attach_status_t*) params == MAL_GPDS_PS_STATUS_ATTACHED) {
        PSCC_DBG_TRACE(LOG_DEBUG, "MAL_GPDS_EVENT_ID_PS_ATTACH_STATUS, attached\n");
        pscc_mal_attach_status = pscc_attach_status_attached;
        attach_event_params.attach_status = pscc_attach_status_attached;
      } else {
        PSCC_DBG_TRACE(LOG_DEBUG, "MAL_GPDS_EVENT_ID_PS_ATTACH_STATUS, detached\n");
        pscc_mal_attach_status = pscc_attach_status_detached;
        attach_event_params.attach_status = pscc_attach_status_detached;
      }
    } else {
      PSCC_DBG_TRACE(LOG_ERR, "MAL_GPDS_EVENT_ID_PS_ATTACH_STATUS with params == NULL\n");
    }

    attach_event = pscc_bearer_attach_event_attach_status;
    break;
  default:
    return (-1);
    break;
  }

  if (pscc_bearer_attach_event_cb != NULL) {
    pscc_bearer_attach_event_cb (attach_event, attach_event_params, cause, attach_request_ct);
  }

  attach_ct_set = false;
  attach_request_ct = 0;
  return (0);
}

/**
 * deactivate_pdp
 **/
static int deactivate_pdp(pscc_bearer_t *obj_p)
{
  assert(NULL != obj_p);

  if (mal_gpds_request(obj_p->mal_cid, MAL_GPDS_REQ_PSCONN_DEACTIVATE, NULL) < 0) {
    PSCC_DBG_TRACE(LOG_ERR, "mal_gpds_request(DEACTIVATE)\n");
    return (-1);
  }
  return (0);
}

/**
 * alloc_mal_connid
 **/
static uint8_t alloc_mal_connid(void)
{
  mpl_list_t *obj_p;
  pscc_bearer_t *bearer_p;
  int pscc_mal_connid = 1;

  /* loop through all bearer objects and see if cid is in use */
  obj_p = pscc_bearer_list_p;
  while (obj_p != NULL) {
    bearer_p = MPL_LIST_CONTAINER(obj_p, pscc_bearer_t, list_entry);
    if (bearer_p->mal_cid == (uint8_t) pscc_mal_connid) {
      /* restart loop */
      obj_p = pscc_bearer_list_p;
      pscc_mal_connid++;
      if (pscc_mal_connid > MAX_UINT8)
        return 0;
    } else
      obj_p = obj_p->next_p;
  }
  return (uint8_t) pscc_mal_connid;
}

/**
 * convert_ip_to_string
 **/
static int convert_ip_to_string(ip_address_t ip, int pdp_type, char *ip_str)
{
  assert(NULL != ip_str);

  switch (pdp_type) {
  case MAL_PDP_TYPE_IPV4:
    PSCC_DBG_TRACE(LOG_DEBUG, "Received pdp type ipv4\n");
    sprintf(ip_str, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
    break;
  case MAL_PDP_TYPE_IPV6:
    PSCC_DBG_TRACE(LOG_DEBUG, "Received pdp type ipv6\n");
    sprintf(ip_str, "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
               ip[0], ip[1], ip[2], ip[3], ip[4], ip[5], ip[6], ip[7], ip[8], ip[9], ip[10], ip[11], ip[12], ip[13], ip[14], ip[15]);
    break;
  default:
    PSCC_DBG_TRACE(LOG_ERR, "mal returned unmapped type: %x\n",pdp_type);
    return (-1);
    break;
  }

  return (0);
}

static uint16_t return_least(uint16_t a, uint16_t b)
{
  return (a < b) ? a : b;
}

/*
 * Print the MAL QoS profile.
 */
static void qos_dbg_print_mal_profile(mal_gpds_qos_profile_t* qos_p)
{
  assert(NULL != qos_p);

  PSCC_DBG_TRACE(LOG_DEBUG, "qos_type                 : %d\n", qos_p->qos_type);

  switch (qos_p->qos_type) {

  case MAL_GPDS_QOS_REL97_98:
    PSCC_DBG_TRACE(LOG_DEBUG, "precedence_class         : %d\n", qos_p->precedence_class);
    PSCC_DBG_TRACE(LOG_DEBUG, "delay_class              : %d\n", qos_p->delay_class);
    PSCC_DBG_TRACE(LOG_DEBUG, "reliability_class        : %d\n", qos_p->reliability_class);
    PSCC_DBG_TRACE(LOG_DEBUG, "peak_throughput_class    : %d\n", qos_p->peak_throughput_class);
    PSCC_DBG_TRACE(LOG_DEBUG, "mean_throughput_class    : %d\n", qos_p->mean_throughput_class);
    break;

  case MAL_GPDS_QOS_REL99:
    PSCC_DBG_TRACE(LOG_DEBUG, "traffic_class            : %d\n", qos_p->traffic_class);
    PSCC_DBG_TRACE(LOG_DEBUG, "delivery_order           : %d\n", qos_p->delivery_order);
    PSCC_DBG_TRACE(LOG_DEBUG, "delivery_of_error_sdu    : %d\n", qos_p->delivery_of_error_sdu);
    PSCC_DBG_TRACE(LOG_DEBUG, "residual_ber             : %d\n", qos_p->residual_ber);
    PSCC_DBG_TRACE(LOG_DEBUG, "sdu_error_ratio          : %d\n", qos_p->sdu_error_ratio);
    PSCC_DBG_TRACE(LOG_DEBUG, "trafic_handling_priority : %d\n", qos_p->trafic_handling_priority);
    PSCC_DBG_TRACE(LOG_DEBUG, "transfer_delay           : %u\n", qos_p->transfer_delay);
    PSCC_DBG_TRACE(LOG_DEBUG, "maximum_sdu_size         : %u\n", qos_p->maximum_sdu_size);
    PSCC_DBG_TRACE(LOG_DEBUG, "maximum_bitrate_ul       : %u\n", qos_p->maximum_bitrate_ul);
    PSCC_DBG_TRACE(LOG_DEBUG, "maximum_bitrate_dl       : %u\n", qos_p->maximum_bitrate_dl);
    PSCC_DBG_TRACE(LOG_DEBUG, "guaranteed_bitrate_ul    : %u\n", qos_p->guaranteed_bitrate_ul);
    PSCC_DBG_TRACE(LOG_DEBUG, "guaranteed_bitrate_dl    : %u\n", qos_p->guaranteed_bitrate_dl);
    break;

  case MAL_GPDS_QOS_REL5:
    PSCC_DBG_TRACE(LOG_DEBUG, "sgn_ind                  : %d\n", qos_p->sgn_ind);
    PSCC_DBG_TRACE(LOG_DEBUG, "src_stat_desc            : %d\n", qos_p->src_stat_desc);
    break;

  default:
    break;
  }
}

