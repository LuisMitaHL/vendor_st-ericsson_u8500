/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 * 
 * \brief  PS Bearer implementation for MFL
 */



/********************************************************************************
 *
 * Include files
 *
 ********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "pscc_bearer.h"
#include "pscc_pco.h"
#include "pscc_utils.h"
#include "psccd.h"
#include "mfl.h"
#include "r_sys.h"
#include "r_pss.h"

/********************************************************************************
 *
 * Defines & Type definitions
 *
 ********************************************************************************/
#define PSCC_BEARER_MFL_MAGIC 0xBEADBABE

#define PSCC_BEARER_MFL_STATES                    \
  PSCC_BEARER_MFL_STATE_ELEMENT(idle)              \
  PSCC_BEARER_MFL_STATE_ELEMENT(pending_nsapi)     \
  PSCC_BEARER_MFL_STATE_ELEMENT(activating)        \
  PSCC_BEARER_MFL_STATE_ELEMENT(activated)         \
  PSCC_BEARER_MFL_STATE_ELEMENT(deactivating)      \
  PSCC_BEARER_MFL_STATE_ELEMENT(pending_destroy)


#define PSCC_BEARER_MFL_STATE_ELEMENT(STATE) pscc_bearer_mfl_##STATE,
typedef enum
{
  PSCC_BEARER_MFL_STATES
  pscc_bearer_mfl_number_of_states
} pscc_bearer_mfl_state_t;
#undef PSCC_BEARER_MFL_STATE_ELEMENT

#define PSCC_BEARER_MFL_STATE_ELEMENT(STATE) #STATE,
static const char* pscc_bearer_mfl_state_names[] =
{
  PSCC_BEARER_MFL_STATES
};
#undef PSCC_BEARER_MFL_STATE_ELEMENT

#define pscc_bearer_change_state(obj_p,new_state)                     \
  do                                                                  \
  {                                                                   \
    assert((new_state) < pscc_bearer_mfl_number_of_states);           \
    assert((obj_p)->state < pscc_bearer_mfl_number_of_states);        \
    assert((obj_p) != NULL);                                          \
    PSCC_DBG_TRACE(LOG_DEBUG, "state change: %s->%s\n",               \
                   pscc_bearer_mfl_state_names[(obj_p)->state],       \
                   pscc_bearer_mfl_state_names[(new_state)]);         \
    (obj_p)->state = new_state;                                       \
  } while(0)


typedef struct pscc_bearer
{
  int handle;
  int connid;
  pscc_bearer_mfl_state_t state;
  int nsapi;
  pscc_get_param_fp_t get_param_cb;
  pscc_set_param_fp_t set_param_cb;
  pscc_delete_param_fp_t delete_param_cb;
  pscc_bearer_event_fp_t event_cb;
  pscc_reason_t reason;
  int cause;
  mpl_list_t list_entry;
}pscc_bearer_t;

mpl_list_t *pscc_bearer_list_p=NULL;

#ifdef PSCC_MODULE_TEST_ENABLED
  /*only used with MFL simulator */
  #define PSCC_MFL_PATH "/tmp/ecaif_chnl1"
#endif

#define get_primitive(signal) \
  ((SigselectWithClientTag_t*)(signal))->Primitive

#define PSCC_COMPILE_CHECK_ALL_QOS_VAULES()                             \
  PSCC_COMPILE_TIME_ASSERT(PS_TRAFFIC_CLASS_UNSPECIFIED == pscc_qos_TRAFFIC_CLASS_UNSPECIFIED); \
  PSCC_COMPILE_TIME_ASSERT(PS_TRAFFIC_CLASS_CONVERSATIONAL == pscc_qos_TRAFFIC_CLASS_CONVERSATIONAL); \
  PSCC_COMPILE_TIME_ASSERT(PS_TRAFFIC_CLASS_STREAMING == pscc_qos_TRAFFIC_CLASS_STREAMING); \
  PSCC_COMPILE_TIME_ASSERT(PS_TRAFFIC_CLASS_INTERACTIVE == pscc_qos_TRAFFIC_CLASS_INTERACTIVE); \
  PSCC_COMPILE_TIME_ASSERT(PS_TRAFFIC_CLASS_BACKGROUND == pscc_qos_TRAFFIC_CLASS_BACKGROUND); \
  PSCC_COMPILE_TIME_ASSERT(PS_DELIVERY_ORDER_UNSPECIFIED == pscc_qos_DELIVERY_ORDER_UNSPECIFIED); \
  PSCC_COMPILE_TIME_ASSERT(PS_DELIVERY_ORDER_YES == pscc_qos_DELIVERY_ORDER_YES); \
  PSCC_COMPILE_TIME_ASSERT(PS_DELIVERY_ORDER_NO == pscc_qos_DELIVERY_ORDER_NO); \
  PSCC_COMPILE_TIME_ASSERT(PS_DELIVERY_ERRONEOUS_SDU_UNSPECIFIED == pscc_qos_DELIVERY_ERRONEOUS_SDU_UNSPECIFIED); \
  PSCC_COMPILE_TIME_ASSERT(PS_DELIVERY_ERRONEOUS_SDU_NO_DETECT == pscc_qos_DELIVERY_ERRONEOUS_SDU_NO_DETECT); \
  PSCC_COMPILE_TIME_ASSERT(PS_DELIVERY_ERRONEOUS_SDU_YES == pscc_qos_DELIVERY_ERRONEOUS_SDU_YES); \
  PSCC_COMPILE_TIME_ASSERT(PS_DELIVERY_ERRONEOUS_SDU_NO == pscc_qos_DELIVERY_ERRONEOUS_SDU_NO); \
  PSCC_COMPILE_TIME_ASSERT(PS_RESIDUAL_BER_UNSPECIFIED == pscc_qos_RESIDUAL_BER_UNSPECIFIED); \
  PSCC_COMPILE_TIME_ASSERT(PS_RESIDUAL_BER_5_E_NEG2 == pscc_qos_RESIDUAL_BER_5_E_NEG2); \
  PSCC_COMPILE_TIME_ASSERT(PS_RESIDUAL_BER_1_E_NEG2 == pscc_qos_RESIDUAL_BER_1_E_NEG2); \
  PSCC_COMPILE_TIME_ASSERT(PS_RESIDUAL_BER_5_E_NEG3 == pscc_qos_RESIDUAL_BER_5_E_NEG3); \
  PSCC_COMPILE_TIME_ASSERT(PS_RESIDUAL_BER_4_E_NEG3 == pscc_qos_RESIDUAL_BER_4_E_NEG3); \
  PSCC_COMPILE_TIME_ASSERT(PS_RESIDUAL_BER_1_E_NEG3 == pscc_qos_RESIDUAL_BER_1_E_NEG3); \
  PSCC_COMPILE_TIME_ASSERT(PS_RESIDUAL_BER_1_E_NEG4 == pscc_qos_RESIDUAL_BER_1_E_NEG4); \
  PSCC_COMPILE_TIME_ASSERT(PS_RESIDUAL_BER_1_E_NEG5 == pscc_qos_RESIDUAL_BER_1_E_NEG5); \
  PSCC_COMPILE_TIME_ASSERT(PS_RESIDUAL_BER_1_E_NEG6 == pscc_qos_RESIDUAL_BER_1_E_NEG6); \
  PSCC_COMPILE_TIME_ASSERT(PS_RESIDUAL_BER_6_E_NEG8 == pscc_qos_RESIDUAL_BER_6_E_NEG8); \
  PSCC_COMPILE_TIME_ASSERT(PS_SDU_ERROR_RATIO_UNSPECIFIED == pscc_qos_SDU_ERROR_RATIO_UNSPECIFIED); \
  PSCC_COMPILE_TIME_ASSERT(PS_SDU_ERROR_RATIO_1_E_NEG2 == pscc_qos_SDU_ERROR_RATIO_1_E_NEG2); \
  PSCC_COMPILE_TIME_ASSERT(PS_SDU_ERROR_RATIO_7_E_NEG3 == pscc_qos_SDU_ERROR_RATIO_7_E_NEG3); \
  PSCC_COMPILE_TIME_ASSERT(PS_SDU_ERROR_RATIO_1_E_NEG3 == pscc_qos_SDU_ERROR_RATIO_1_E_NEG3); \
  PSCC_COMPILE_TIME_ASSERT(PS_SDU_ERROR_RATIO_1_E_NEG4 == pscc_qos_SDU_ERROR_RATIO_1_E_NEG4); \
  PSCC_COMPILE_TIME_ASSERT(PS_SDU_ERROR_RATIO_1_E_NEG5 == pscc_qos_SDU_ERROR_RATIO_1_E_NEG5); \
  PSCC_COMPILE_TIME_ASSERT(PS_SDU_ERROR_RATIO_1_E_NEG6 == pscc_qos_SDU_ERROR_RATIO_1_E_NEG6); \
  PSCC_COMPILE_TIME_ASSERT(PS_SDU_ERROR_RATIO_1_E_NEG1 == pscc_qos_SDU_ERROR_RATIO_1_E_NEG1); \
  PSCC_COMPILE_TIME_ASSERT(PS_TRAFFIC_HANDLING_PRIORITY_UNSPECIFIED == pscc_qos_TRAFFIC_HANDLING_PRIORITY_UNSPECIFIED); \
  PSCC_COMPILE_TIME_ASSERT(PS_TRAFFIC_HANDLING_PRIORITY_LEVEL_1 == pscc_qos_TRAFFIC_HANDLING_PRIORITY_LEVEL_1); \
  PSCC_COMPILE_TIME_ASSERT(PS_TRAFFIC_HANDLING_PRIORITY_LEVEL_2 == pscc_qos_TRAFFIC_HANDLING_PRIORITY_LEVEL_2); \
  PSCC_COMPILE_TIME_ASSERT(PS_TRAFFIC_HANDLING_PRIORITY_LEVEL_3 == pscc_qos_TRAFFIC_HANDLING_PRIORITY_LEVEL_3); \
  PSCC_COMPILE_TIME_ASSERT(PS_SIGNALLING_INDICATION_NOT_OPTIMIZED_FOR_SIGNALLING_TRAFFIC == pscc_qos_SIGNALLING_INDICATION_NOT_OPTIMIZED_FOR_SIGNALLING_TRAFFIC); \
  PSCC_COMPILE_TIME_ASSERT(PS_SIGNALLING_INDICATION_OPTIMIZED_FOR_SIGNALLING_TRAFFIC == pscc_qos_SIGNALLING_INDICATION_OPTIMIZED_FOR_SIGNALLING_TRAFFIC); \
  PSCC_COMPILE_TIME_ASSERT(PS_SOURCE_STATISTICS_DESCRIPTOR_UNKNOWN == pscc_qos_SOURCE_STATISTICS_DESCRIPTOR_UNKNOWN); \
  PSCC_COMPILE_TIME_ASSERT(PS_SOURCE_STATISTICS_DESCRIPTOR_SPEECH == pscc_qos_SOURCE_STATISTICS_DESCRIPTOR_SPEECH); \



/********************************************************************************
 *
 * Local variables
 *
 ********************************************************************************/

static modem_session_t *mfl_session_p = NULL;

/********************************************************************************
 *
 * Private function prototypes
 *
 ********************************************************************************/
static void handle_response_contextactivate(pscc_bearer_t *obj_p,void *signal_p);
static void handle_response_contextdeactivate(pscc_bearer_t *obj_p,void *signal_p);
static void handle_event_contextstatus(pscc_bearer_t *obj_p,void *signal_p);
static void handle_event_contextdeactivated(pscc_bearer_t *obj_p,void *signal_p);
static void handle_response_contextmodify(pscc_bearer_t *obj_p, void *signal_p);
static void handle_response_qos_profile_read(pscc_bearer_t *obj_p, void *signal_p);

static const uint8_t* get_apn_ref(pscc_bearer_t *obj_p);
static PS_QOS_Type_t get_qostype(pscc_bearer_t *obj_p);
static void get_qos(pscc_bearer_t *obj_p,PS_QOS_Profile_t *qos_p,boolean req);
static void set_qos(pscc_bearer_t *obj_p,PS_QOS_Profile_t *qos_p);
static PS_HeaderCompression_t get_hcmp(pscc_bearer_t *obj_p);
static PS_DataCompression_t get_dcmp(pscc_bearer_t *obj_p);
static PS_PDP_Type_t get_pdptype(pscc_bearer_t *obj_p);
static size_t create_pco(pscc_bearer_t *obj_p,uint8_t *pco_p, size_t pcolen);
static const PS_PDP_Address_t *get_pdpaddr_ref(pscc_bearer_t *obj_p);
static PS_Priority_t get_ps_prio(pscc_bearer_t *obj_p);
static const PS_UserPlaneSettings_t *get_userplane_setting(pscc_bearer_t *obj_p);
static void handle_context_activated(pscc_bearer_t *obj_p,
                                     PS_PDP_Type_t pdptype,
                                     PS_PDP_Address_t *pdpaddr_p,
                                     uint8_t *pco_p,
                                     uint8_t pcolen);
static void reset_bearer(pscc_bearer_t *obj_p, bool send_event);
static int deactivate_pdp(pscc_bearer_t *obj_p);
static pscc_bearer_t *find_bearer_by_handle(int handle);


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
  int mfl_fd;

#ifdef PSCC_MODULE_TEST_ENABLED
  //only used with MFL simulator
  mfl_session_p =  mfl_session_begin_named_socket(PSCC_MFL_PATH);
#else
  //use default mfl path for target
  mfl_session_p = mfl_session_begin();
#endif

  if(NULL == mfl_session_p)
  {
    PSCC_DBG_TRACE(LOG_ERR, "failed to open MFL session\n");
    return (-1);
  }

  mfl_session_get_file_descriptor(mfl_session_p,&mfl_fd);

  return (mfl_fd);
}

/**
 * pscc_bearer_deinit()
 **/
void pscc_bearer_deinit(void)
{
  mpl_list_t *obj_p, *tmp_p;
  pscc_bearer_t *bearer_p;

  /*  initiate deactivation of all active PDP contexts*/
  MPL_LIST_FOR_EACH_SAFE(pscc_bearer_list_p, obj_p, tmp_p)
  {
    bearer_p = MPL_LIST_CONTAINER(obj_p, pscc_bearer_t, list_entry);
    assert(NULL != bearer_p);
    switch(bearer_p->state)
    {
    case pscc_bearer_mfl_activating:
    case pscc_bearer_mfl_activated:
      (void)deactivate_pdp(bearer_p);
      /* fallthrough */
    case pscc_bearer_mfl_pending_nsapi: /* wait for activate response before start deactivate */
      if(NULL != bearer_p->event_cb)
        bearer_p->event_cb(bearer_p->connid,
                           pscc_bearer_event_deactivated,
                           pscc_reason_pdp_context_ms_deactivated,0);
      /*fallthrough*/
    case pscc_bearer_mfl_pending_destroy:
      break;
    default: /* unexpected states */
      PSCC_DBG_TRACE(LOG_ERR, "unexpected state %d\n",bearer_p->state);
      break;
    }
    (void)mpl_list_remove(&pscc_bearer_list_p, &bearer_p->list_entry);
    free(bearer_p);
  }

  if(NULL != mfl_session_p)
    mfl_session_end(mfl_session_p);
}

/**
 * pscc_bearer_alloc - allocate a bearer object
 **/
int pscc_bearer_alloc(int connid)
{
  static int pscc_bearer_handle=0;
  pscc_bearer_t *obj_p = malloc(sizeof(pscc_bearer_t));

  if(NULL == obj_p)
  {
    PSCC_DBG_TRACE(LOG_ERR, "failed allocating memory\n");
    return (-1);
  }

  memset(obj_p,0,sizeof(pscc_bearer_t));
  obj_p->handle = ++pscc_bearer_handle;
  obj_p->connid = connid;
  pscc_bearer_change_state(obj_p,pscc_bearer_mfl_idle);
  mpl_list_add(&pscc_bearer_list_p,&obj_p->list_entry);
  PSCC_DBG_TRACE(LOG_DEBUG, "bearer allocated\n");
  return (obj_p->handle);
}


/**
 * pscc_bearer_free
 **/
int pscc_bearer_free(int handle)
{
  pscc_bearer_t *obj_p;
  obj_p = find_bearer_by_handle(handle);
  
  if(NULL == obj_p)
  {
    PSCC_DBG_TRACE(LOG_ERR, "invalid bearer obj specified\n");
    return (-1);
  }

  switch(obj_p->state)
  {
  case pscc_bearer_mfl_idle:
    break;
  case pscc_bearer_mfl_pending_nsapi:
    /*We are waiting for a response. Cannot send destroy before we have received it*/
    /*Set state to pending_destroy and report deactivated to the handler*/
    /*The bearer will remain in a "zombie" state until NS has responded or until deinit are called*/
    if (NULL != obj_p->event_cb)
      obj_p->event_cb(obj_p->connid,
                      pscc_bearer_event_deactivated,
                      pscc_reason_pdp_context_ms_deactivated,0);
    pscc_bearer_change_state(obj_p,pscc_bearer_mfl_pending_destroy);
    return (0);
  case pscc_bearer_mfl_activating:
  case pscc_bearer_mfl_activated:
    (void)deactivate_pdp(obj_p);
    /* fallthrough */
  case pscc_bearer_mfl_deactivating:
    reset_bearer(obj_p,true);
    break;
  case pscc_bearer_mfl_pending_destroy:
    reset_bearer(obj_p,false);
    break;
  default: /* unexpected states */
    PSCC_DBG_TRACE(LOG_ERR, "unexpected state %d\n",obj_p->state);
    break;
  }

  mpl_list_remove(&pscc_bearer_list_p,&obj_p->list_entry);
  free(obj_p);
  PSCC_DBG_TRACE(LOG_DEBUG, "bearer freed\n");
  return (0);
}

/**
 * pscc_bearer_activate
 **/
int pscc_bearer_activate
  (
   int handle,
   pscc_get_param_fp_t get_param_cb,
   pscc_set_param_fp_t set_param_cb,
   pscc_delete_param_fp_t delete_param_cb,
   pscc_bearer_event_fp_t event_cb
  )
{
  pscc_bearer_t *obj_p;
  RequestControlBlock_t* rcb_p;
  uint8_t *pco_p=NULL,pcolen=0;
  PS_QOS_Profile_t minqos,reqqos;

  assert(mfl_session_p != NULL);

  PSCC_DBG_TRACE(LOG_DEBUG, "inside pscc_bearer_activate\n");

  obj_p = find_bearer_by_handle(handle);
  if(NULL == obj_p)
  {
    PSCC_DBG_TRACE(LOG_ERR, "invalid bearer obj\n");
    return (-1);
  }

  if(pscc_bearer_mfl_idle != obj_p->state)
  {
    PSCC_DBG_TRACE(LOG_ERR, "invalid bearer obj state\n");
    return (-1);
  }

  /* store callbacks */
  obj_p->set_param_cb = set_param_cb;
  obj_p->get_param_cb = get_param_cb;
  obj_p->delete_param_cb = delete_param_cb;
  obj_p->event_cb = event_cb;

  /* call request to activate a PDP context */
  rcb_p = mfl_request_control_block_alloc(mfl_session_p, (ClientTag_t)obj_p->handle);
  if(NULL == rcb_p)
  {
    PSCC_DBG_TRACE(LOG_ERR, "failed to allocate rcb\n");
    return (-1);
  }

  pco_p = malloc(PS_MAX_PROTOCOL_OPTIONS_LENGTH);
  if(NULL != pco_p)
  {
    pcolen = (uint8_t)create_pco(obj_p,pco_p,PS_MAX_PROTOCOL_OPTIONS_LENGTH);
  }
  else
    PSCC_DBG_TRACE(LOG_ERR, "failed allocating memory\n");

  get_qos(obj_p,&minqos,FALSE);
  get_qos(obj_p,&reqqos,TRUE);

  PSCC_DBG_TRACE_DEFINE(({
                            int i;
                            PSCC_DBG_TRACE(LOG_INFO, "sending pco(%d): ",pcolen);
                            for(i=0;i<pcolen;i++)
                              PSCC_DBG_TRACE_NO_INFO("%x ",pco_p[i]);
                            PSCC_DBG_TRACE_NO_INFO("\n");
                          }));

  PSCC_DBG_TRACE(LOG_DEBUG, "calling Request_PS_PDP_Context_Activate\n");
  if(Request_PS_PDP_Context_Activate_V3((RequestControlBlock_t *)rcb_p,
                                        get_apn_ref(obj_p),
                                        get_hcmp(obj_p),
                                        get_dcmp(obj_p),
                                        get_qostype(obj_p),
                                        FALSE,  // QoS reverted
                                        &minqos,
                                        &reqqos,
                                        get_pdptype(obj_p),
                                        get_pdpaddr_ref(obj_p),
                                        pco_p,
                                        pcolen,
                                        get_ps_prio(obj_p),
                                        get_userplane_setting(obj_p),
                                        NULL,
                                        NULL) != REQUEST_PENDING)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Request_PS_PDP_Context_Activate_V3 failed\n");

    if(NULL != pco_p)
      free(pco_p);

    mfl_request_control_block_free(rcb_p);
    return (-1);
  }

  if(NULL != pco_p)
    free(pco_p);
  mfl_request_control_block_free(rcb_p);
  pscc_bearer_change_state(obj_p,pscc_bearer_mfl_pending_nsapi);
  PSCC_DBG_TRACE(LOG_DEBUG, "waiting for Response_PS_PDP_Context_Activate\n");
  return (0);
}


/**
 * pscc_bearer_deactivate
 **/
int pscc_bearer_deactivate(int handle)
{
  pscc_bearer_t *obj_p;

  PSCC_DBG_TRACE(LOG_DEBUG, "inside pscc_bearer_deactivate\n");

  obj_p = find_bearer_by_handle(handle);
  if(NULL == obj_p)
    return (-1);
  switch(obj_p->state)
  {
  case pscc_bearer_mfl_pending_nsapi: /* wait for activate response before start deactivate */
    PSCC_DBG_TRACE(LOG_DEBUG, "pending nsapi\n");
    pscc_bearer_change_state(obj_p,pscc_bearer_mfl_deactivating);
    return (0);

  case pscc_bearer_mfl_activating:
  case pscc_bearer_mfl_activated:
    /* start deactivating PDP context */
    break;

  default: /* unexpected states */
    PSCC_DBG_TRACE(LOG_ERR, "unexpected state %d\n",obj_p->state);
    return (-1);
  }

  if(deactivate_pdp(obj_p) < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "deactivate pdp failed\n");
    return (-1);
  }

  pscc_bearer_change_state(obj_p,pscc_bearer_mfl_deactivating);
  PSCC_DBG_TRACE(LOG_DEBUG, "waiting for Response_PS_PDP_Context_Deactivate\n");
  return (0);
}


/**
 *
 **/
int pscc_bearer_modify(int handle)
{
  pscc_bearer_t *obj_p;
  RequestControlBlock_t* rcb_p;
  uint8_t *pco_p=NULL,pcolen=0;
  PS_QOS_Profile_t minqos,reqqos;
  PS_CauseCode_t Cause;
  PS_GeneralResult_t result=0;

  assert(mfl_session_p != NULL);

  PSCC_DBG_TRACE(LOG_DEBUG, "inside pscc_bearer_modify\n");

  obj_p = find_bearer_by_handle(handle);
  if(NULL == obj_p)
  {
    PSCC_DBG_TRACE(LOG_ERR, "invalid bearer obj\n");
    return (-1);
  }

  if(pscc_bearer_mfl_activated != obj_p->state)
  {
    PSCC_DBG_TRACE(LOG_ERR, "invalid bearer obj state\n");
    return (-1);
  }

  /* call request to activate a PDP context */
  rcb_p = mfl_request_control_block_alloc(mfl_session_p, (ClientTag_t)obj_p->handle);
  if(NULL == rcb_p)
  {
    PSCC_DBG_TRACE(LOG_ERR, "failed to allocate rcb\n");
    return (-1);
  }

  get_qos(obj_p,&minqos,FALSE);
  get_qos(obj_p,&reqqos,TRUE);

  PSCC_DBG_TRACE(LOG_DEBUG, "calling Request_PS_PDP_Context_Modify_V3\n");
  if(Request_PS_PDP_Context_Modify_V3((RequestControlBlock_t *)rcb_p,
                                      obj_p->nsapi,
                                      get_qostype(obj_p),
                                      &minqos,
                                      &reqqos,
                                      PS_TFT_OPERATION_CODE_NO_OPERATION,
                                      0,
                                      NULL,
                                      NULL,
                                      NULL,
                                      &pcolen,
                                      &pco_p,
                                      &Cause,
                                      &result) != REQUEST_PENDING)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Request_PS_PDP_Context_Modify_V3 failed\n");
    mfl_request_control_block_free(rcb_p);
    return (-1);
  }

  mfl_request_control_block_free(rcb_p);
  PSCC_DBG_TRACE(LOG_DEBUG, "waiting for Response_PS_PDP_Context_Modify\n");
  return (0);
}

/**
 * pscc_bearer_attach()
 **/
int pscc_bearer_attach (pscc_bearer_attach_event_fp_t event_cb, uint32_t ct)
{
  assert(event_cb != NULL);
  PSCC_IDENTIFIER_NOT_USED(ct);
  PSCC_DBG_TRACE(LOG_ERR, "pscc_bearer_attach not supported in mfl\n");
  return (-1);
}

/**
 * pscc_bearer_detach()
 **/
int pscc_bearer_detach (pscc_bearer_attach_event_fp_t event_cb, uint32_t ct)
{
  assert(event_cb != NULL);
  PSCC_IDENTIFIER_NOT_USED(ct);
  PSCC_DBG_TRACE(LOG_ERR, "pscc_bearer_detach not supported in mfl\n");
  return (-1);
}

/**
 * pscc_bearer_attach_status()
 **/
int pscc_bearer_attach_status (pscc_attach_status_t *attach_status_p)
{
  assert(attach_status_p != NULL);
  PSCC_DBG_TRACE(LOG_ERR, "pscc_bearer_attach_status not supported in mfl\n");
  return (-1);
}

/* query ps attach status */
int pscc_bearer_query_attach_status(pscc_bearer_attach_event_fp_t event_cb, uint32_t ct)
{
  PSCC_IDENTIFIER_NOT_USED(ct);
  PSCC_IDENTIFIER_NOT_USED(event_cb);
  PSCC_DBG_TRACE(LOG_ERR, "pscc_bearer_attach_status not supported in mfl\n");
  return (-1);
}

/**
 * pscc_bearer_query_attach_mode()
 **/
int pscc_bearer_query_attach_mode (pscc_bearer_attach_event_fp_t event_cb, uint32_t ct)
{
  assert(event_cb != NULL);
  PSCC_IDENTIFIER_NOT_USED(ct);
  PSCC_DBG_TRACE(LOG_ERR, "pscc_bearer_query_attach_mode not supported in mfl\n");
  return (-1);

}

/**
 * pscc_bearer_configure_attach_mode()
 **/
int pscc_bearer_configure_attach_mode (pscc_bearer_attach_event_fp_t event_cb, pscc_attach_mode_t attach_mode, uint32_t ct)
{
  assert(event_cb != NULL);
  PSCC_IDENTIFIER_NOT_USED(ct);
  PSCC_DBG_TRACE(LOG_ERR, "pscc_bearer_configure_attach_mode not supported in mfl\n");
  return (-1);

}

/**
 * pscc_bearer_query_ping_block_mode()
 **/
int pscc_bearer_query_ping_block_mode (pscc_bearer_ping_block_mode_event_fp_t event_cb, uint32_t ct)
{
  assert(event_cb != NULL);
  PSCC_IDENTIFIER_NOT_USED(ct);
  PSCC_DBG_TRACE(LOG_ERR, "pscc_bearer_query_ping_block_mode not supported in mfl\n");
  return (-1);

}

/**
 * pscc_bearer_configure_ping_block_mode()
 **/
int pscc_bearer_configure_ping_block_mode(pscc_bearer_ping_block_mode_event_fp_t event_cb,
                                                pscc_ping_block_mode_t ping_mode, uint32_t ct)
{
  assert(event_cb != NULL);
  PSCC_IDENTIFIER_NOT_USED(ping_mode);
  PSCC_IDENTIFIER_NOT_USED(ct);
  PSCC_DBG_TRACE(LOG_ERR, "pscc_bearer_configure_ping_block_mode not supported in mfl\n");
  return (-1);
}

/**
 * pscc_bearer_configure_aol_mode()
 **/
int pscc_bearer_configure_aol_mode (bool enable)
{
  PSCC_IDENTIFIER_NOT_USED(enable);
  PSCC_DBG_TRACE(LOG_ERR, "pscc_bearer_configure_aol_mode not supported in mfl\n");
  return (-1);
}

/**
 * pscc_bearer_data_counters()
 **/
int pscc_bearer_data_counters(int handle, bool fetch)
{
  PSCC_IDENTIFIER_NOT_USED(handle);
  PSCC_DBG_TRACE(LOG_ERR, "pscc_bearer_data_counters not supported in mfl\n");
  return (-1);
}

/**
 * pscc_bearer_respond_resource_control()
 **/
int pscc_bearer_respond_resource_control (pscc_rc_t* pscc_rc_p, bool accepted, bool modified)
{
  PSCC_IDENTIFIER_NOT_USED(pscc_rc_p);
  PSCC_IDENTIFIER_NOT_USED(accepted);
  PSCC_IDENTIFIER_NOT_USED(modified);
  PSCC_DBG_TRACE(LOG_ERR, "pscc_bearer_respond_resource_control not supported in mfl\n");
  return (-1);
}

/**
 * pscc_bearer_configure_rc()
 **/
int pscc_bearer_configure_rc (pscc_bearer_rc_event_fp_t rc_event_cb)
{
  PSCC_IDENTIFIER_NOT_USED(rc_event_cb);
  PSCC_DBG_TRACE(LOG_ERR, "pscc_bearer_configure_rc not supported in mfl\n");
  return (-1);
}

/**
 * pscc_bearer_generate_uplink_data()
 **/
int pscc_bearer_generate_uplink_data (int handle)
{
  PSCC_IDENTIFIER_NOT_USED(handle);
  PSCC_DBG_TRACE(LOG_ERR, "pscc_bearer_generate_uplink_data not supported in mfl\n");
  return (-1);
}

/**
 * pscc_bearer_handler()
 **/
int pscc_bearer_handler(int fd)
{
  int mfl_fd;
  void *signal_p;
  pscc_bearer_t *obj_p;
  ClientTag_t ct;

  PSCC_DBG_TRACE(LOG_DEBUG, "inside pscc_bearer_handler()\n");

  assert(mfl_session_p != NULL);
  mfl_session_get_file_descriptor(mfl_session_p,&mfl_fd);
  if(fd != mfl_fd)
  {
    PSCC_DBG_TRACE(LOG_ERR, "invalid file dscriptor\n");
    return (-1);
  }

  /* receive the signal */
  signal_p = mfl_session_get_signal(mfl_session_p);
  if(NULL == signal_p)  {
    PSCC_DBG_TRACE(LOG_ERR, "no signal returned\n");
    return (-1);
  }

  /* fetch client tag from signal */
  ct = mfl_get_client_tag(signal_p);
  obj_p = find_bearer_by_handle((int)ct);

  if(NULL == obj_p)
  {
    PSCC_DBG_TRACE(LOG_DEBUG, "bearer obj not found, probably already deleted ignore\n");
    mfl_signal_free(&signal_p);
    return (-1);
  }

  switch(get_primitive(signal_p))
  {
  case RESPONSE_PS_PDP_CONTEXT_ACTIVATE_V2:
    PSCC_DBG_TRACE(LOG_DEBUG, "RESPONSE_PS_PDP_CONTEXT_ACTIVATE_V2 received\n");
    handle_response_contextactivate(obj_p,signal_p);
    break;
  case RESPONSE_PS_PDP_CONTEXT_DEACTIVATE_V2:
    PSCC_DBG_TRACE(LOG_DEBUG, "RESPONSE_PS_PDP_CONTEXT_DEACTIVATE_V2 received\n");
    handle_response_contextdeactivate(obj_p,signal_p);
    break;
  case RESPONSE_PS_PDP_CONTEXT_MODIFY_V3:
    PSCC_DBG_TRACE(LOG_DEBUG, "RESPONSE_PS_PDP_CONTEXT_MODIFY_V3 received\n");
    handle_response_contextmodify(obj_p,signal_p);
    break;
  case RESPONSE_PS_QOS_PROFILE_READ:
    PSCC_DBG_TRACE(LOG_DEBUG, "RESPONSE_PS_QOS_PROFILE_READ received\n");
    handle_response_qos_profile_read(obj_p,signal_p);
    break;
  case EVENT_PS_PDP_CONTEXTSTATUS:
    PSCC_DBG_TRACE(LOG_DEBUG, "EVENT_PS_PDP_CONTEXTSTATUS received\n");
    handle_event_contextstatus(obj_p,signal_p);
    break;
  case EVENT_PS_PDP_CONTEXTDEACTIVATE_V2:
    PSCC_DBG_TRACE(LOG_DEBUG, "EVENT_PS_PDP_CONTEXTDEACTIVATE_V2 received\n");
    handle_event_contextdeactivated(obj_p,signal_p);
    break;
  default:
    PSCC_DBG_TRACE(LOG_DEBUG, "unknown signal received %d\n",get_primitive(signal_p));
    break;
  }
  mfl_signal_free(&signal_p);

  return (0);
}

/**
 * net link stubbed functions
 **/

/**
 * pscc_bearer_netlnk_init()
 **/
int pscc_bearer_netlnk_init()
{
    return -1;
}

/**
 * pscc_bearer_netlnk_handle_message()
 **/
int pscc_bearer_netlnk_handle_message(int fd)
{
    PSCC_IDENTIFIER_NOT_USED(fd);
    return -1;
}


/*******************************************************************************
 *
 * Private Functions
 *
 *******************************************************************************/

static void handle_response_contextactivate(pscc_bearer_t *obj_p,void *signal_p)
{
  PS_NSAPI_t nsapi;
  RequestStatus_t status;
  PS_PDP_ContextActivateResult_t result=0;
  mpl_param_element_t *param_p;

  assert(NULL != obj_p);
  status = Response_PS_PDP_Context_Activate_V2(signal_p,&nsapi,&result);

  if((REQUEST_OK != status) || (PS_PDP_CONTEXT_ACTIVATE_RESULT_IN_PROGRESS != result))
  {
    PSCC_DBG_TRACE(LOG_ERR, "Response_PS_PDP_Context_Activate_V2 failed %d:%d\n",status,result);
    if (obj_p->state == pscc_bearer_mfl_pending_destroy)
    {
      reset_bearer(obj_p,false);
      mpl_list_remove(&pscc_bearer_list_p,&obj_p->list_entry);
      free(obj_p);
    }
    else
    {
      obj_p->reason = pscc_reason_pdp_context_activation_failed;
      obj_p->cause = 0;
      reset_bearer(obj_p,true);
    }
    return;
  }

  switch(obj_p->state)
  {
  case pscc_bearer_mfl_pending_nsapi:
    PSCC_DBG_TRACE(LOG_DEBUG, "got nsapi %d\n",nsapi);
    obj_p->nsapi = nsapi;
    pscc_bearer_change_state(obj_p,pscc_bearer_mfl_activating); /* wait for context status event */
    /* store the nsapi. If it fails, netdev_caif will return error later. So we don't catch error here */
    param_p = mpl_param_element_create(pscc_paramid_nsapi, &obj_p->nsapi);
    if((NULL != param_p) && (NULL != obj_p->set_param_cb))
    {
      if (obj_p->set_param_cb(obj_p->connid,param_p) < 0)
      {
        PSCC_DBG_TRACE(LOG_ERR, "not able to store nsapi parameter\n");
        mpl_param_element_destroy(param_p);
      }
    }

    if(NULL != obj_p->event_cb)
      obj_p->event_cb(obj_p->connid,pscc_bearer_event_activating,0,0);
    break;
  case pscc_bearer_mfl_deactivating:
    /* initiate deactivate of PDP context */
    obj_p->nsapi = nsapi;
    if(deactivate_pdp(obj_p) < 0)
    {
      PSCC_DBG_TRACE(LOG_ERR, "deactivate pdp failed\n");
      obj_p->reason = pscc_reason_pdp_context_ms_deactivated;
      obj_p->cause = 0;
      reset_bearer(obj_p,true);
      return;
    }
    break;
  case pscc_bearer_mfl_pending_destroy:
    /*Send deactivate and clean up*/
    obj_p->nsapi = nsapi;
    (void)deactivate_pdp(obj_p);
    reset_bearer(obj_p,false);
    mpl_list_remove(&pscc_bearer_list_p,&obj_p->list_entry);
    free(obj_p);
    break;

  default: /* unexpected states */
    PSCC_DBG_TRACE(LOG_ERR, "unexpected state %d\n",obj_p->state);
    break;
  }
}

static void handle_response_contextdeactivate(pscc_bearer_t *obj_p, void *signal_p)
{
  RequestStatus_t status;
  PS_GeneralResult_t result=0;

  assert(NULL!= obj_p);
  status = Response_PS_PDP_Context_Deactivate_V2(signal_p,&result);

  if((REQUEST_OK != status) || (PS_GENERAL_RESULT_OK != result))
  {
    PSCC_DBG_TRACE(LOG_ERR, "Response_PS_PDP_Context_Deactivate failed %d:%d\n",status,result);
    return;
  }

  if(pscc_bearer_mfl_deactivating != obj_p->state)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Response_PS_PDP_Context_Deactivate received in unexpected state %d\n",obj_p->state);
    return;
  }

  if(NULL != obj_p->event_cb)
    obj_p->event_cb(obj_p->connid,pscc_bearer_event_deactivating,0,0);
  /* wait for deactivate event */
}

static void handle_response_contextmodify(pscc_bearer_t *obj_p, void *signal_p)
{
  RequestStatus_t status;
  PS_GeneralResult_t result=0;
  PS_CauseCode_t cause=0;
  uint8 pcolen=0;
  uint8 *pco_p = NULL;
  RequestControlBlock_t* rcb_p;
  PS_QOS_Profile_t QOS_Profile;

  assert(NULL!= obj_p);
  status = Response_PS_PDP_Context_Modify_V3(signal_p,&pcolen,&pco_p,&cause,&result);

  if(NULL != obj_p->event_cb)
  {
    if((REQUEST_OK != status) || (PS_GENERAL_RESULT_OK != result))
    {
      PSCC_DBG_TRACE(LOG_ERR, "Response_PS_PDP_Context_Modify_V3 failed %d:%d\n",status,result);
      goto error_report_and_return;
    }
    free(pco_p);
    /*Get negotiated QOS values*/
    rcb_p = mfl_request_control_block_alloc(mfl_session_p, (ClientTag_t)obj_p->handle);
    if(NULL == rcb_p)
    {
      PSCC_DBG_TRACE(LOG_ERR, "failed to allocate rcb\n");
      goto error_report_and_return;
    }

    PSCC_DBG_TRACE(LOG_DEBUG, "calling Request_PS_QOS_Profile_Read\n");
    if(Request_PS_QOS_Profile_Read((RequestControlBlock_t *)rcb_p,
                                   obj_p->nsapi,
                                   &QOS_Profile,
                                   &result) != REQUEST_PENDING)
    {
      PSCC_DBG_TRACE(LOG_ERR, "Request_PS_QOS_Profile_Read failed\n");
      mfl_request_control_block_free(rcb_p);
      goto error_report_and_return;
    }

    mfl_request_control_block_free(rcb_p);
    PSCC_DBG_TRACE(LOG_DEBUG, "waiting for Response_PS_QOS_Profile_Read\n");
  }
  
  return;

  error_report_and_return:
  if(NULL != obj_p->event_cb)
    obj_p->event_cb(obj_p->connid,pscc_bearer_event_modify_failed,result,cause);
  
}

static void handle_response_qos_profile_read(pscc_bearer_t *obj_p, void *signal_p)
{
  RequestStatus_t status;
  PS_GeneralResult_t result=0;
  PS_QOS_Profile_t QOS_Profile;

  assert(NULL!= obj_p);
  status = Response_PS_QOS_Profile_Read(signal_p, &QOS_Profile, &result);

  if(NULL == obj_p->event_cb)
    return;
  
  if((REQUEST_OK != status) || (PS_GENERAL_RESULT_OK != result))
  {
    PSCC_DBG_TRACE(LOG_ERR, "Response_PS_QOS_Profile_Read failed %d:%d\n",status,result);
    goto error_report_and_return;
  }


  /*Set qos values*/
  set_qos(obj_p, &QOS_Profile);
  
  if(pscc_bearer_mfl_activated == obj_p->state)
  {
    PSCC_DBG_TRACE(LOG_DEBUG, " Response_PS_QOS_Profile_Read success %d:%d\n",status,result);
    obj_p->event_cb(obj_p->connid,pscc_bearer_event_modified,0,0);
  }
  else if (pscc_bearer_mfl_activating == obj_p->state)
  {
    PSCC_DBG_TRACE(LOG_DEBUG, " Response_PS_QOS_Profile_Read success %d:%d\n",status,result);
    pscc_bearer_change_state(obj_p,pscc_bearer_mfl_activated);
    obj_p->event_cb(obj_p->connid,pscc_bearer_event_activated,0,0);   
  }

  return;
  
  error_report_and_return:
  if(pscc_bearer_mfl_activated == obj_p->state)
  {
    PSCC_DBG_TRACE(LOG_ERR, " Response_PS_QOS_Profile_Read %d:%d\n",status,result);
    obj_p->event_cb(obj_p->connid,pscc_bearer_event_modify_failed,result,0);
  }
  else if (pscc_bearer_mfl_activating == obj_p->state)
  {
    /*Deactivate??*/
    if(deactivate_pdp(obj_p) < 0)
    {
      PSCC_DBG_TRACE(LOG_ERR, "deactivate pdp failed\n");
      obj_p->reason = pscc_reason_pdp_context_activation_failed;
      obj_p->cause = 0;
      reset_bearer(obj_p,true);
    }
    pscc_bearer_change_state(obj_p,pscc_bearer_mfl_deactivating);
  }
}


static void handle_event_contextstatus(pscc_bearer_t *obj_p, void *signal_p)
{
  EventStatus_t status;
  PS_PDP_Type_t pdptype;
  PS_PDP_Address_t pdpaddr;
  uint8_t *pco_p=NULL;
  uint8_t pcolen;
  PS_NSAPI_t nsapi;
  PS_CauseCode_t cause;
  PS_PDP_ContextEvent_t contextevent;

  assert(NULL != obj_p);

  PSCC_DBG_TRACE(LOG_DEBUG, "inside handle_event_contextstatus()\n");

  status = Event_PS_PDP_ContextStatus(signal_p,&pdptype,&pdpaddr,&pco_p,&pcolen,&nsapi,&cause,&contextevent);
  if(GS_EVENT_OK != status)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Event_PS_PDP_ContextStatus failed %d\n",status);
    return;
  }

  if((nsapi != obj_p->nsapi) && (pscc_bearer_mfl_idle != obj_p->state))
  {
    PSCC_DBG_TRACE(LOG_ERR, "mismatch between NSAPI and client tag\n");
    // TODO: handle this case when Network initiated secondary PDP is added
    return;
  }

  switch(contextevent)
  {
  case PS_PDP_CONTEXT_EVENT_MS_ACTIVATED:
    PSCC_DBG_TRACE(LOG_DEBUG, "context activated, nsapi=%d\n",nsapi);
    handle_context_activated(obj_p,pdptype,&pdpaddr,pco_p,pcolen);
    break;
  case PS_PDP_CONTEXT_EVENT_MS_ACTIVATION_FAILED:
    PSCC_DBG_TRACE(LOG_DEBUG, "context activation failed, nsapi=%d\n",nsapi);
    obj_p->reason = pscc_reason_pdp_context_activation_failed;
    obj_p->cause = cause;
    reset_bearer(obj_p,true);
    break;
  case PS_PDP_CONTEXT_EVENT_MS_ACTIVATION_REJECTED:
    PSCC_DBG_TRACE(LOG_DEBUG, "context activation rejected, nsapi=%d\n",nsapi);
    obj_p->reason = pscc_reason_pdp_context_activation_rejected;
    obj_p->cause = cause;
    reset_bearer(obj_p,true);
    break;
  case PS_PDP_CONTEXT_EVENT_NW_DEACTIVATED:
    PSCC_DBG_TRACE(LOG_DEBUG, "context activation nw deactivated, nsapi=%d\n",nsapi);
    obj_p->reason = pscc_reason_pdp_context_nw_deactivated;
    obj_p->cause = cause;
    reset_bearer(obj_p,true);
    break;

  case PS_PDP_CONTEXT_EVENT_NETWORK_MODIFICATION:
  {
    PS_GeneralResult_t result=0;
    RequestControlBlock_t* rcb_p;
    PS_QOS_Profile_t QOS_Profile;
    
    PSCC_DBG_TRACE(LOG_DEBUG, "context modified by NW, nsapi=%d\n",nsapi);
    /*Read QOS*/
    /*Get negotiated QOS values*/
    rcb_p = mfl_request_control_block_alloc(mfl_session_p, (ClientTag_t)obj_p->handle);
    if(NULL == rcb_p)
    {
      PSCC_DBG_TRACE(LOG_ERR, "failed to allocate rcb\n");
      /*Send event modification failed??*/
      break;
    }
  
    PSCC_DBG_TRACE(LOG_DEBUG, "calling Request_PS_QOS_Profile_Read\n");
    if(Request_PS_QOS_Profile_Read((RequestControlBlock_t *)rcb_p,
                                   obj_p->nsapi,
                                   &QOS_Profile,
                                   &result) != REQUEST_PENDING)
    {
      PSCC_DBG_TRACE(LOG_ERR, "Request_PS_QOS_Profile_Read failed\n");
      mfl_request_control_block_free(rcb_p);
      /*Send event modification failed??*/
      break;
    }

    mfl_request_control_block_free(rcb_p);
    PSCC_DBG_TRACE(LOG_DEBUG, "waiting for Response_PS_QOS_Profile_Read\n");
  }
  
  break;

  case PS_PDP_CONTEXT_EVENT_MS_DEACTIVATED:
    PSCC_DBG_TRACE(LOG_DEBUG, "context ms deactivated, nsapi=%d\n",nsapi);
    obj_p->reason = pscc_reason_pdp_context_ms_deactivated;
    obj_p->cause = cause;
    /* no action here, wait for context deactivated event*/
    break;

  case PS_PDP_CONTEXT_EVENT_MS_DEACTIVATE_STARTED:
  case PS_PDP_CONTEXT_EVENT_NW_ACTIVATION_STARTED:
  case PS_PDP_CONTEXT_EVENT_NW_ACTIVATION_FAILED:
  case PS_PDP_CONTEXT_EVENT_NW_ACTIVATED:
  case PS_PDP_CONTEXT_EVENT_NW_ACTIVATION_REJECTED:
  case PS_PDP_CONTEXT_EVENT_MS_ACTIVATION_STARTED:
  default:
    PSCC_DBG_TRACE(LOG_DEBUG, "PDP context status received (Event=%d, NSAPI=%d, ConnId=%d) - No action\n",
                   contextevent,nsapi,obj_p->connid);
    break;
  }
}

static void handle_event_contextdeactivated(pscc_bearer_t *obj_p, void *signal_p)
{
  EventStatus_t status;
  PS_NSAPI_t nsapi;
  PS_TimeAndDataVolume_t timeanddatavolume;
  uint8_t *pco_p=NULL;
  uint8_t pcolen;
  PS_CauseCode_t cause;

  PSCC_DBG_TRACE(LOG_DEBUG, "inside handle_event_contextdeactivated()\n");
  assert(NULL != obj_p);

  status = Event_PS_PDP_ContextDeactivate_V2(signal_p,&nsapi,&timeanddatavolume,&pco_p,&pcolen,&cause);

  if(GS_EVENT_OK != status)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Event_PS_PDP_ContextStatus failed %d\n",status);
    return;
  }

  if(nsapi != obj_p->nsapi)
  {
    PSCC_DBG_TRACE(LOG_ERR, "nsapi mismatch\n");
    return;
  }

  switch(obj_p->state)
  {
  case pscc_bearer_mfl_deactivating:
  case pscc_bearer_mfl_activating:
  case pscc_bearer_mfl_activated:
    break;
  default:
    PSCC_DBG_TRACE(LOG_ERR, "unexpected state %d\n",obj_p->state);
    return;
  }

  /* TODO: do we need to report data counters and duration? */

  if(0 != obj_p->cause)
    obj_p->cause = cause;

  reset_bearer(obj_p,true);
}

static const uint8_t* get_apn_ref(pscc_bearer_t *obj_p)
{
  static const uint8_t apn[] = "";
  mpl_param_element_t* param_p;

  assert(NULL != obj_p);
  assert(NULL != obj_p->get_param_cb);

  param_p = obj_p->get_param_cb(obj_p->connid,pscc_paramid_apn);
  if(NULL != param_p)
  {
    PSCC_DBG_TRACE(LOG_DEBUG, "using apn %s\n",(uint8_t*)param_p->value_p);
    return ((const uint8_t*)param_p->value_p);
  }

  /* failed to get APN parameter, use empty */
  return (apn);
}

static void get_qos(pscc_bearer_t *obj_p,PS_QOS_Profile_t *qos_p,boolean req)
{
  mpl_param_element_t* param_p;

  PSCC_COMPILE_CHECK_ALL_QOS_VAULES();

  assert(NULL != obj_p);
  assert(NULL != obj_p->get_param_cb);
  assert(qos_p != NULL);

  memset(qos_p,0,sizeof(PS_QOS_Profile_t));

  param_p = obj_p->get_param_cb(obj_p->connid,req?pscc_paramid_req_qos_traffic_class:pscc_paramid_min_qos_traffic_class);
  if(NULL != param_p)
    qos_p->TrafficClass = *((PS_TrafficClass_t*)param_p->value_p);

  param_p = obj_p->get_param_cb(obj_p->connid,req?pscc_paramid_req_qos_delivery_order:pscc_paramid_min_qos_delivery_order);
  if(NULL != param_p)
    qos_p->DeliveryOrder = *((PS_DeliveryOrder_t*)param_p->value_p);

  param_p = obj_p->get_param_cb(obj_p->connid,req?pscc_paramid_req_qos_delivery_erroneous_sdu:pscc_paramid_min_qos_delivery_erroneous_sdu);
  if(NULL != param_p)
    qos_p->DeliveryErroneousSDU = *((PS_DeliveryErroneousSDU_t*)param_p->value_p);

  param_p = obj_p->get_param_cb(obj_p->connid,req?pscc_paramid_req_qos_max_sdu_size:pscc_paramid_min_qos_max_sdu_size);
  if(NULL != param_p)
    qos_p->MaxSDU_Size = *((PS_MaxSDU_Size_t*)param_p->value_p);

  param_p = obj_p->get_param_cb(obj_p->connid,req?pscc_paramid_req_qos_max_bitrate_uplink:pscc_paramid_min_qos_max_bitrate_uplink);
  if(NULL != param_p)
    qos_p->MaxBitRateUplink = *((PS_MaxBitRateUplink_t*)param_p->value_p);

  param_p = obj_p->get_param_cb(obj_p->connid,req?pscc_paramid_req_qos_max_bitrate_downlink:pscc_paramid_min_qos_max_bitrate_downlink);
  if(NULL != param_p)
    qos_p->MaxBitRateDownlink = *((PS_MaxBitRateDownlink_t*)param_p->value_p);

  param_p = obj_p->get_param_cb(obj_p->connid,req?pscc_paramid_req_qos_residual_ber:pscc_paramid_min_qos_residual_ber);
  if(NULL != param_p)
    qos_p->ResidualBER = *((PS_ResidualBER_t*)param_p->value_p);

  param_p = obj_p->get_param_cb(obj_p->connid,req?pscc_paramid_req_qos_sdu_error_ratio:pscc_paramid_min_qos_sdu_error_ratio);
  if(NULL != param_p)
    qos_p->SDU_ErrorRatio = *((PS_SDU_ErrorRatio_t*)param_p->value_p);

  param_p = obj_p->get_param_cb(obj_p->connid,req?pscc_paramid_req_qos_transfer_delay:pscc_paramid_min_qos_transfer_delay);
  if(NULL != param_p)
    qos_p->TransferDelay = *((PS_TransferDelay_t*)param_p->value_p);

  param_p = obj_p->get_param_cb(obj_p->connid,req?pscc_paramid_req_qos_traffic_handling_priority:pscc_paramid_min_qos_traffic_handling_priority);
  if(NULL != param_p)
    qos_p->TrafficHandlingPriority = *((PS_TrafficHandlingPriority_t*)param_p->value_p);

  param_p = obj_p->get_param_cb(obj_p->connid,req?pscc_paramid_req_qos_guaranteed_bitrate_uplink:pscc_paramid_min_qos_guaranteed_bitrate_uplink);
  if(NULL != param_p)
    qos_p->GuaranteedBitRateUplink = *((PS_GuaranteedBitRateUplink_t*)param_p->value_p);

  param_p = obj_p->get_param_cb(obj_p->connid,req?pscc_paramid_req_qos_guaranteed_bitrate_downlink:pscc_paramid_min_qos_guaranteed_bitrate_downlink);
  if(NULL != param_p)
    qos_p->GuaranteedBitRateDownlink = *((PS_GuaranteedBitRateDownlink_t*)param_p->value_p);

  param_p = obj_p->get_param_cb(obj_p->connid,req?pscc_paramid_req_qos_signalling_indication:pscc_paramid_min_qos_signalling_indication);
  if(NULL != param_p)
    qos_p->SignallingIndication = *((PS_SignallingIndication_t*)param_p->value_p);

  param_p = obj_p->get_param_cb(obj_p->connid,req?pscc_paramid_req_qos_source_statistics_descriptor:pscc_paramid_min_qos_source_statistics_descriptor);
  if(NULL != param_p)
    qos_p->SourceStatisticsDescriptor = *((PS_SourceStatisticsDescriptor_t*)param_p->value_p);

  param_p = obj_p->get_param_cb(obj_p->connid,req?pscc_paramid_req_qos_extended_max_bitrate_downlink:pscc_paramid_min_qos_extended_max_bitrate_downlink);
  if(NULL != param_p)
    qos_p->ExtendedMaxBitRateDownlink = *((PS_ExtendedMaxBitRateDownlink_t*)param_p->value_p);

  param_p = obj_p->get_param_cb(obj_p->connid,req?pscc_paramid_req_qos_extended_guaranteed_bitrate_downlink:pscc_paramid_min_qos_extended_guaranteed_bitrate_downlink);
  if(NULL != param_p)
    qos_p->ExtendedGuaranteedBitRateDownlink = *((PS_ExtendedMaxBitRateDownlink_t*)param_p->value_p);

#if 0 // TODO: add when available
  param_p = obj_p->get_param_cb(obj_p->connid,req?pscc_paramid_req_qos_extended_max_bitrate_uplink:pscc_paramid_min_qos_extended_max_bitrate_uplink);
  if(NULL != param_p)
    qos_p->ExtendedMaxBitRateUplink = *((PS_ExtendedMaxBitRateUplink_t*)param_p->value_p);

  param_p = obj_p->get_param_cb(obj_p->connid,req?pscc_paramid_req_qos_extended_guaranteed_bitrate_uplink:pscc_paramid_min_qos_extended_guaranteed_bitrate_uplink);
  if(NULL != param_p)
    qos_p->ExtendedGuaranteedBitRateUplink = *((PS_ExtendedMaxBitRateUplink_t*)param_p->value_p);
#endif
}

static void set_qos(pscc_bearer_t *obj_p,PS_QOS_Profile_t *qos_p)
{
  mpl_param_element_t* param_p;

  PSCC_COMPILE_CHECK_ALL_QOS_VAULES();

  assert(NULL != obj_p);
  assert(NULL != obj_p->set_param_cb);
  assert(NULL != obj_p->delete_param_cb);
  assert(qos_p != NULL);

  //Delete the old parameters
  obj_p->delete_param_cb(obj_p->connid,pscc_paramid_neg_qos_traffic_class);
  obj_p->delete_param_cb(obj_p->connid,pscc_paramid_neg_qos_delivery_order);
  obj_p->delete_param_cb(obj_p->connid,pscc_paramid_neg_qos_delivery_erroneous_sdu);
  obj_p->delete_param_cb(obj_p->connid,pscc_paramid_neg_qos_max_sdu_size);
  obj_p->delete_param_cb(obj_p->connid,pscc_paramid_neg_qos_max_bitrate_uplink);
  obj_p->delete_param_cb(obj_p->connid,pscc_paramid_neg_qos_max_bitrate_downlink);
  obj_p->delete_param_cb(obj_p->connid,pscc_paramid_neg_qos_residual_ber);
  obj_p->delete_param_cb(obj_p->connid,pscc_paramid_neg_qos_sdu_error_ratio);
  obj_p->delete_param_cb(obj_p->connid,pscc_paramid_neg_qos_transfer_delay);
  obj_p->delete_param_cb(obj_p->connid,pscc_paramid_neg_qos_traffic_handling_priority);
  obj_p->delete_param_cb(obj_p->connid,pscc_paramid_neg_qos_guaranteed_bitrate_uplink);
  obj_p->delete_param_cb(obj_p->connid,pscc_paramid_neg_qos_guaranteed_bitrate_downlink);
  obj_p->delete_param_cb(obj_p->connid,pscc_paramid_neg_qos_signalling_indication);
  obj_p->delete_param_cb(obj_p->connid,pscc_paramid_neg_qos_source_statistics_descriptor);
  obj_p->delete_param_cb(obj_p->connid,pscc_paramid_neg_qos_extended_max_bitrate_downlink);
  obj_p->delete_param_cb(obj_p->connid,pscc_paramid_neg_qos_extended_guaranteed_bitrate_downlink);
#if 0 // TODO: add when available
  obj_p->delete_param_cb(obj_p->connid,pscc_paramid_neg_qos_extended_max_bitrate_uplink);
  obj_p->delete_param_cb(obj_p->connid,pscc_paramid_neg_qos_extended_guaranteed_bitrate_uplink);
#endif
  //Set the new parameters       

  param_p = mpl_param_element_create(pscc_paramid_neg_qos_traffic_class,&qos_p->TrafficClass);
  if(obj_p->set_param_cb(obj_p->connid,param_p) < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "not able to store neg_qos_traffic_class\n");
    mpl_param_element_destroy(param_p);
  }
  param_p = mpl_param_element_create(pscc_paramid_neg_qos_delivery_order,&qos_p->DeliveryOrder);
  if(obj_p->set_param_cb(obj_p->connid,param_p) < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "not able to store neg_qos_delivery_order\n");
    mpl_param_element_destroy(param_p);
  }
  param_p = mpl_param_element_create(pscc_paramid_neg_qos_delivery_erroneous_sdu,&qos_p->DeliveryErroneousSDU);
  if(obj_p->set_param_cb(obj_p->connid,param_p) < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "not able to store neg_qos_delivery_erroneous_sdu\n");
    mpl_param_element_destroy(param_p);
  }
  param_p = mpl_param_element_create(pscc_paramid_neg_qos_max_sdu_size,&qos_p->MaxSDU_Size);
  if(obj_p->set_param_cb(obj_p->connid,param_p) < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "not able to store neg_qos_max_sdu_size\n");
    mpl_param_element_destroy(param_p);
  }
  param_p = mpl_param_element_create(pscc_paramid_neg_qos_max_bitrate_uplink,&qos_p->MaxBitRateUplink);
  if(obj_p->set_param_cb(obj_p->connid,param_p) < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "not able to store neg_qos_max_bitrate_uplink\n");
    mpl_param_element_destroy(param_p);
  }
  param_p = mpl_param_element_create(pscc_paramid_neg_qos_max_bitrate_downlink,&qos_p->MaxBitRateDownlink);
  if(obj_p->set_param_cb(obj_p->connid,param_p) < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "not able to store neg_qos_max_bitrate_downlink\n");
    mpl_param_element_destroy(param_p);
  }
  param_p = mpl_param_element_create(pscc_paramid_neg_qos_residual_ber,&qos_p->ResidualBER);
  if(obj_p->set_param_cb(obj_p->connid,param_p) < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "not able to store neg_qos_residual_ber\n");
    mpl_param_element_destroy(param_p);
  }
  param_p = mpl_param_element_create(pscc_paramid_neg_qos_sdu_error_ratio,&qos_p->SDU_ErrorRatio);
  if(obj_p->set_param_cb(obj_p->connid,param_p) < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "not able to store neg_qos_sdu_error_ratio\n");
    mpl_param_element_destroy(param_p);
  }
  param_p = mpl_param_element_create(pscc_paramid_neg_qos_transfer_delay,&qos_p->TransferDelay);
  if(obj_p->set_param_cb(obj_p->connid,param_p) < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "not able to store neg_qos_transfer_delay\n");
    mpl_param_element_destroy(param_p);
  }
  param_p = mpl_param_element_create(pscc_paramid_neg_qos_traffic_handling_priority,&qos_p->TrafficHandlingPriority);
  if(obj_p->set_param_cb(obj_p->connid,param_p) < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "not able to store neg_qos_traffic_handling_priority\n");
    mpl_param_element_destroy(param_p);
  }
  param_p = mpl_param_element_create(pscc_paramid_neg_qos_guaranteed_bitrate_uplink,&qos_p->GuaranteedBitRateUplink);
  if(obj_p->set_param_cb(obj_p->connid,param_p) < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "not able to store neg_qos_guaranteed_bitrate_uplink\n");
    mpl_param_element_destroy(param_p);
  }
  param_p = mpl_param_element_create(pscc_paramid_neg_qos_guaranteed_bitrate_downlink,&qos_p->GuaranteedBitRateDownlink);
  if(obj_p->set_param_cb(obj_p->connid,param_p) < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "not able to store neg_qos_guaranteed_bitrate_downlink\n");
    mpl_param_element_destroy(param_p);
  }
  param_p = mpl_param_element_create(pscc_paramid_neg_qos_signalling_indication,&qos_p->SignallingIndication);
  if(obj_p->set_param_cb(obj_p->connid,param_p) < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "not able to store neg_qos_signalling_indication\n");
    mpl_param_element_destroy(param_p);
  }
  param_p = mpl_param_element_create(pscc_paramid_neg_qos_source_statistics_descriptor,&qos_p->SourceStatisticsDescriptor);
  if(obj_p->set_param_cb(obj_p->connid,param_p) < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "not able to store neg_qos_source_statistics_descriptor\n");
    mpl_param_element_destroy(param_p);
  }
  param_p = mpl_param_element_create(pscc_paramid_neg_qos_extended_max_bitrate_downlink,&qos_p->ExtendedMaxBitRateDownlink);
  if(obj_p->set_param_cb(obj_p->connid,param_p) < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "not able to store neg_qos_extended_max_bitrate_downlink\n");
    mpl_param_element_destroy(param_p);
  }
  param_p = mpl_param_element_create(pscc_paramid_neg_qos_extended_guaranteed_bitrate_downlink,&qos_p->ExtendedGuaranteedBitRateDownlink);
  if(obj_p->set_param_cb(obj_p->connid,param_p) < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "not able to store neg_qos_extended_guaranteed_bitrate_downlink\n");
    mpl_param_element_destroy(param_p);
  }
#if 0 // TODO: add when available
  param_p = mpl_param_element_create(pscc_paramid_neg_qos_extended_max_bitrate_uplink,&qos_p->ExtendedMaxBitRateUplink);
  if(obj_p->set_param_cb(obj_p->connid,param_p) < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "not able to store neg_qos_extended_max_bitrate_uplink\n");
    mpl_param_element_destroy(param_p);
  }
  param_p = mpl_param_element_create(pscc_paramid_neg_qos_extended_guaranteed_bitrate_uplink,&qos_p->ExtendedGuaranteedBitRateUplink);
  if(obj_p->set_param_cb(obj_p->connid,param_p) < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "not able to store neg_qos_extended_guaranteed_bitrate_uplink\n");
    mpl_param_element_destroy(param_p);
  }
#endif

}

static const PS_PDP_Address_t *get_pdpaddr_ref(pscc_bearer_t *obj_p)
{
  static PS_PDP_Address_t pdpaddr;
  mpl_param_element_t* param_p;

  assert(NULL != obj_p);
  assert(NULL != obj_p->get_param_cb);

  memset(&pdpaddr,0,sizeof(pdpaddr));

  param_p = obj_p->get_param_cb(obj_p->connid,pscc_paramid_static_ip_address);
  if(NULL != param_p)
  {
    /* only valid for IPv4 */
    if(inet_pton(AF_INET,param_p->value_p,&pdpaddr) <= 0)
      /* inet_pton failed */
      memset(&pdpaddr,0,sizeof(pdpaddr));
  }

  return (const PS_PDP_Address_t *)&pdpaddr;
}

static PS_HeaderCompression_t get_hcmp(pscc_bearer_t *obj_p)
{
  mpl_param_element_t* param_p;

  assert(NULL != obj_p);
  assert(NULL != obj_p->get_param_cb);

  param_p = obj_p->get_param_cb(obj_p->connid,pscc_paramid_hcmp);
  if((NULL != param_p) && *((bool*)param_p->value_p))
    return (PS_HEADER_COMPRESSION_REQUESTED);

  return (PS_HEADER_COMPRESSION_NO_COMPRESSION);
}

static PS_DataCompression_t get_dcmp(pscc_bearer_t *obj_p)
{
  mpl_param_element_t* param_p;

  assert(NULL != obj_p);
  assert(NULL != obj_p->get_param_cb);

  param_p = obj_p->get_param_cb(obj_p->connid,pscc_paramid_dcmp);
  if((NULL != param_p) && *((bool*)param_p->value_p))
    return (PS_DATA_COMPRESSION_REQUESTED);

  return (PS_DATA_COMPRESSION_NO_COMPRESSION);
}

static PS_PDP_Type_t get_pdptype(pscc_bearer_t *obj_p)
{
  mpl_param_element_t* param_p;

  assert(NULL != obj_p);
  assert(NULL != obj_p->get_param_cb);

  param_p = obj_p->get_param_cb(obj_p->connid,pscc_paramid_pdp_type);
  if(NULL != param_p)
  {
    switch(*((pscc_pdp_type_t*)param_p->value_p))
    {
    case pscc_pdp_type_ipv4:
      return (PS_PDP_TYPE_IP_V4);
    case pscc_pdp_type_ipv6:
      PSCC_DBG_TRACE(LOG_ERR, "Ipv6 is not supported yet, defaulting to ipv4\n");
      return (PS_PDP_TYPE_IP_V4);
    default:
      PSCC_DBG_TRACE(LOG_ERR, "unsupported pdp type\n");
      break;
    }
  }
  return (PS_PDP_TYPE_IP_V4);
}

static PS_QOS_Type_t get_qostype(pscc_bearer_t *obj_p)
{
  PSCC_IDENTIFIER_NOT_USED(obj_p);
  /* we only support release 99 parameter set */
  return (PS_QOS_TYPE_RELEASE_99);
}

static PS_Priority_t get_ps_prio(pscc_bearer_t *obj_p)
{
  mpl_param_element_t* param_p;
  assert(NULL != obj_p);
  assert(NULL != obj_p->get_param_cb);

  param_p = obj_p->get_param_cb(obj_p->connid,pscc_paramid_pref_service);
  if((NULL != param_p) &&
     (pscc_preferred_service_ps_only == *((pscc_preferred_service_t*)param_p->value_p)))
    return (PS_PRIORITY_REQUESTED);

  return (PS_PRIORITY_NOT_REQUESTED);
}

static const PS_UserPlaneSettings_t *get_userplane_setting(pscc_bearer_t *obj_p)
{
  static PS_UserPlaneSettings_t settings;
  mpl_param_element_t* param_p;

  assert(NULL != obj_p);
  assert(NULL != obj_p->get_param_cb);

  memset(&settings,0,sizeof(settings));

  param_p = obj_p->get_param_cb(obj_p->connid,pscc_paramid_aqm);
  if(NULL != param_p)
    settings.EnableAQM = (boolean)(*((bool*)param_p->value_p));

  param_p = obj_p->get_param_cb(obj_p->connid,pscc_paramid_packet_priority);
  if(NULL != param_p)
    settings.SDU_Prioritization = (PS_SDU_TypeBitField_t)(*((uint32_t*)param_p->value_p));

  return (const PS_UserPlaneSettings_t *)&settings;
}


static size_t create_pco(pscc_bearer_t *obj_p,uint8_t *pco_p, size_t pcolen)
{
  static const char empty_string[] = "";
  static uint8_t pscc_chap_id = 1,pscc_pap_id=1;
  mpl_param_element_t* param_p;
  pscc_auth_method_t auth_method = pscc_auth_method_none;
  char *uid=(char*)empty_string,*pwd=(char*)empty_string;
  size_t responselen=0,challengelen=0;
  uint8_t *challenge_p=NULL,*response_p=NULL;
  uint8_t auth_id=0;
  pscc_pdp_type_t pdp_type=pscc_pdp_type_ipv4;
  uint32_t static_ip = 0;
  size_t len=0;
  int tmplen;
  bool free_response = true;
  bool free_challenge = true;

  assert(NULL != obj_p);
  assert(NULL != obj_p->get_param_cb);

  /* fetch authentication parameters */
  param_p = obj_p->get_param_cb(obj_p->connid,pscc_paramid_auth_method);
  if(NULL != param_p)
    auth_method = *((pscc_auth_method_t*)param_p->value_p);

  param_p = obj_p->get_param_cb(obj_p->connid,pscc_paramid_auth_uid);
  if(NULL != param_p)
    uid = (char*)param_p->value_p;

  param_p = obj_p->get_param_cb(obj_p->connid,pscc_paramid_auth_pwd);
  if(NULL != param_p)
    pwd = (char*)param_p->value_p;

  param_p = obj_p->get_param_cb(obj_p->connid,pscc_paramid_pdp_type);
  if(NULL != param_p)
    pdp_type = (*((pscc_pdp_type_t*)param_p->value_p));

  param_p = obj_p->get_param_cb(obj_p->connid,pscc_paramid_static_ip_address);
  if(NULL != param_p)
  {
    /* only valid for IPv4 */
    if(inet_pton(AF_INET,param_p->value_p,&static_ip) <= 0)
      /* inet_pton failed */
      static_ip = 0;
  }

  PSCC_DBG_TRACE(LOG_DEBUG, "auth_method %d, uid=%s, pwd=%s\n",auth_method,uid,pwd);

  switch(auth_method)
  {
  case pscc_auth_method_chap:
    /* chap challenge */
    param_p = obj_p->get_param_cb(obj_p->connid,pscc_paramid_auth_chap_challenge);
    if(NULL != param_p)
    {
      challengelen = ((mpl_uint8_array_t*)param_p->value_p)->len;
      challenge_p = ((mpl_uint8_array_t*)param_p->value_p)->arr_p;
      free_challenge = false;
      PSCC_DBG_TRACE(LOG_DEBUG, "challengelen %d\n",challengelen);
    }
    else
      challengelen = pscc_utils_generate_chap_challenge(&challenge_p, 0);


    /* chap id */
    param_p = obj_p->get_param_cb(obj_p->connid,pscc_paramid_auth_chap_id);
    if(NULL != param_p)
      auth_id = (*(uint8_t*)param_p->value_p);
    else
      auth_id = (++pscc_chap_id);

    /* chap response */
    param_p = obj_p->get_param_cb(obj_p->connid,pscc_paramid_auth_chap_response);
    if(NULL != param_p)
    {
      responselen = ((mpl_uint8_array_t*)param_p->value_p)->len;
      response_p = ((mpl_uint8_array_t*)param_p->value_p)->arr_p;
      free_response = false;
    }
    else
      responselen = pscc_utils_calculate_chap_response(auth_id,
                                                     challenge_p,
                                                     challengelen,
                                                     pwd,
                                                     strlen(pwd),
                                                     &response_p);
    break;
  case pscc_auth_method_pap:
    auth_id = (++pscc_pap_id);
    break;

  case pscc_auth_method_none:
  default:
    break;
  }

  if(0 > (tmplen = pscc_pco_add_auth(auth_method,
                                     uid,
                                     pwd,
                                     challenge_p,
                                     challengelen,
                                     response_p,
                                     responselen,
                                     auth_id,
                                     (len == 0),
                                     pco_p,
                                     (pcolen-len))))
  {
    PSCC_DBG_TRACE(LOG_ERR, "failed to add authentication parameters to pco\n");
    len = 0;
    goto free_and_return;
  }

  len += tmplen;

  if(pdp_type == pscc_pdp_type_ipv4)
  {
    if(0 > (tmplen = pscc_pco_add_ipcp(static_ip,(len == 0),&pco_p[len],(pcolen-len))))
    {
      PSCC_DBG_TRACE(LOG_ERR, "failed to add ipcp parameters to pco\n");
      len = 0;
      goto free_and_return;
    }
    len += tmplen;
  }

  /* TODO: add IMS and IPv6 specific pco options */

  free_and_return:
  if(free_challenge && NULL != challenge_p)
    free(challenge_p);

  if(free_response && NULL != response_p)
    free(response_p);

  return len;
}

static void handle_context_activated(pscc_bearer_t *obj_p,
                                     PS_PDP_Type_t pdptype,
                                     PS_PDP_Address_t *pdpaddr_p,
                                     uint8_t *pco_p,
                                     uint8_t pcolen)
{
  mpl_param_element_t *param_p;
  mpl_list_t *param_list_p;
  RequestControlBlock_t* rcb_p;
  PS_QOS_Profile_t QOS_Profile;
  PS_GeneralResult_t result=0;

  assert(obj_p != NULL);

  PSCC_DBG_TRACE(LOG_DEBUG, "inside handle_context_activated()\n");

  if(pscc_bearer_mfl_activating != obj_p->state)
  {
    PSCC_DBG_TRACE(LOG_DEBUG, "context activated received in unexpeced state\n");
    return;
  }

  if(NULL == obj_p->set_param_cb)
  {
    PSCC_DBG_TRACE(LOG_DEBUG, "no set_param_cb registered\n");
    return;
  }

  if((PS_PDP_TYPE_IP_V4 != pdptype) && (PS_PDP_TYPE_IP_V6 != pdptype))
  {
    /* unknown PDP type received, deactivate PDP context */
    PSCC_DBG_TRACE(LOG_ERR, "unsupported PDP type received\n");
    if(deactivate_pdp(obj_p) < 0)
    {
      PSCC_DBG_TRACE(LOG_ERR, "deactivate pdp failed\n");
      obj_p->reason = pscc_reason_pdp_context_activation_failed;
      obj_p->cause = 0;
      reset_bearer(obj_p,true);
    }
    return;
  }


  if(PS_PDP_TYPE_IP_V6 == pdptype)
  {
    /* TODO: Remove this if case when ipv6 is supported */
    PSCC_DBG_TRACE(LOG_ERR, "Received an ipv6 address from mfl. Not supported yet.\n");
    if(deactivate_pdp(obj_p) < 0)
    {
      PSCC_DBG_TRACE(LOG_ERR, "deactivate pdp failed\n");
      obj_p->reason = pscc_reason_pdp_context_activation_failed;
      obj_p->cause = 0;
      reset_bearer(obj_p,true);
    }
    return;
  }

  /* get parameters from pco */
  param_list_p = pscc_pco_get(pco_p,pcolen);
  if(NULL != param_list_p)
  {
    if(PS_PDP_TYPE_IP_V4 == pdptype)
    {
      char ownip[sizeof("255.255.255.255")];
      mpl_param_element_t *tmp_param_p;

      (void)inet_ntop(AF_INET,pdpaddr_p,ownip,sizeof(ownip));

      param_p = mpl_param_element_create_stringn(pscc_paramid_own_ip_address,
                                                 ownip, strlen(ownip));
      if(NULL == param_p)
      {
        PSCC_DBG_TRACE(LOG_ERR, "failed to create parameter for own ip address\n");
        if(deactivate_pdp(obj_p) < 0)
        {
          PSCC_DBG_TRACE(LOG_ERR, "deactivate pdp failed\n");
          obj_p->reason = pscc_reason_pdp_context_activation_failed;
          obj_p->cause = 0;
          reset_bearer(obj_p,true);
        }
        return;
      }

      PSCC_DBG_TRACE(LOG_DEBUG, "IP address: %s\n",ownip);

      /* for curiosity try to get own IP address from PCO and compare it with
         what we got in the pdp address field of the event (should be equal) */
      tmp_param_p = mpl_param_list_find(pscc_paramid_own_ip_address, param_list_p);
      if(NULL != tmp_param_p)
      {
        if(strncmp(ownip,(char*)tmp_param_p->value_p,strlen((char*)tmp_param_p->value_p)) != 0)
          PSCC_DBG_TRACE(LOG_DEBUG, "IP address from PCO does not match IP address received\n");
      }

      if(obj_p->set_param_cb(obj_p->connid,param_p) < 0)
      {
        PSCC_DBG_TRACE(LOG_ERR, "not able to store own ip address parameter\n");
        mpl_param_element_destroy(param_p);
      }

      /* fetch primary DNS address */
      param_p = mpl_param_list_find(pscc_paramid_dns_address, param_list_p);
      if (NULL != param_p)
      {
        PSCC_DBG_TRACE(LOG_DEBUG, "Primary DNS address: %s\n",(char*)param_p->value_p);

        (void)mpl_list_remove(&param_list_p, &param_p->list_entry);
        if(obj_p->set_param_cb(obj_p->connid, param_p) < 0)
        {
          PSCC_DBG_TRACE(LOG_ERR, "not able to store dns address parameter\n");
          mpl_param_element_destroy(param_p);
        }
      }

      /* fetch secondary DNS address */
      param_p = mpl_param_list_find(pscc_paramid_secondary_dns_address, param_list_p);
      if (NULL != param_p)
      {
        PSCC_DBG_TRACE(LOG_DEBUG, "Secondary DNS address: %s\n",(char*)param_p->value_p);

        (void)mpl_list_remove(&param_list_p, &param_p->list_entry);
        if(obj_p->set_param_cb(obj_p->connid, param_p) < 0)
        {
          PSCC_DBG_TRACE(LOG_ERR, "not able to store dns address parameter\n");
          mpl_param_element_destroy(param_p);
        }
      }

      /* fetch GW address */
      param_p = mpl_param_list_find(pscc_paramid_gw_address, param_list_p);
      if(NULL != param_p)
      {
        PSCC_DBG_TRACE(LOG_DEBUG, "GW address: %s\n",(char*)param_p->value_p);

        (void)mpl_list_remove(&param_list_p, &param_p->list_entry);
        if(obj_p->set_param_cb(obj_p->connid,param_p) < 0)
        {
          PSCC_DBG_TRACE(LOG_ERR, "not able to store gw address parameter\n");
          mpl_param_element_destroy(param_p);
        }
      }
    }
    else if(PS_PDP_TYPE_IP_V6 == pdptype)
    {
      /* TODO: When IPv6 is supported, the handling of the IP addresses should be located here */
    }
  }

  /* TODO: fetch PCSCF address(es) */
  /* TODO: fetch signalling flag */

  /* we're done with the parameter list, destroy it */
  mpl_param_list_destroy(&param_list_p);

  /*Fetch QOS params. Report Activated when they are received*/
  rcb_p = mfl_request_control_block_alloc(mfl_session_p, (ClientTag_t)obj_p->handle);
  if(NULL == rcb_p)
  {
    PSCC_DBG_TRACE(LOG_ERR, "failed to allocate rcb\n");
    goto error_free_and_return;
  }
  PSCC_DBG_TRACE(LOG_DEBUG, "calling Request_PS_QOS_Profile_Read\n");
  if(Request_PS_QOS_Profile_Read((RequestControlBlock_t *)rcb_p,
                                 obj_p->nsapi,
                                 &QOS_Profile,
                                 &result) != REQUEST_PENDING)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Request_PS_QOS_Profile_Read failed\n");
    goto error_free_and_return;
  }
  mfl_request_control_block_free(rcb_p);

  return;

  error_free_and_return:
  if (NULL != rcb_p)
    mfl_request_control_block_free(rcb_p);
  
  if(deactivate_pdp(obj_p) < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "deactivate pdp failed\n");
    obj_p->reason = pscc_reason_pdp_context_activation_failed;
    obj_p->cause = 0;
    reset_bearer(obj_p,true);
  } 
}

static void reset_bearer(pscc_bearer_t *obj_p, bool send_event_deactivated)
{
  assert(obj_p != NULL);

  pscc_bearer_change_state(obj_p,pscc_bearer_mfl_idle);

  if(send_event_deactivated && (NULL != obj_p->event_cb))
    obj_p->event_cb(obj_p->connid,
                    pscc_bearer_event_deactivated,
                    obj_p->reason,
                    obj_p->cause);

  /* delete all parameters set by the bearer*/
  if(NULL != obj_p->delete_param_cb)
  {
    obj_p->delete_param_cb(obj_p->connid,pscc_paramid_own_ip_address);
    obj_p->delete_param_cb(obj_p->connid,pscc_paramid_gw_address);
    obj_p->delete_param_cb(obj_p->connid,pscc_paramid_dns_address);
    obj_p->delete_param_cb(obj_p->connid,pscc_paramid_secondary_dns_address);
    obj_p->delete_param_cb(obj_p->connid,pscc_paramid_nsapi);
  }

  obj_p->nsapi = PS_NSAPI_UNDEFINED;
  obj_p->event_cb = NULL;
  obj_p->set_param_cb = NULL;
  obj_p->get_param_cb = NULL;
  obj_p->delete_param_cb = NULL;
  obj_p->reason = pscc_reason_pdp_context_ms_deactivated;
  obj_p->cause = 0;
}

static int deactivate_pdp(pscc_bearer_t *obj_p)
{
  RequestControlBlock_t* rcb_p;

  assert(obj_p != NULL);

  rcb_p = mfl_request_control_block_alloc(mfl_session_p, (ClientTag_t)obj_p->handle);
  if(NULL == rcb_p)
  {
    PSCC_DBG_TRACE(LOG_ERR, "failed to allocate rcb\n");
    return (-1);
  }

  PSCC_DBG_TRACE(LOG_DEBUG, "calling Request_PS_PDP_Context_Deactivate\n");
  if(Request_PS_PDP_Context_Deactivate_V2((RequestControlBlock_t *)rcb_p,
                                          (PS_NSAPI_t)obj_p->nsapi,
                                          PS_TEAR_DOWN_INDICATOR_TEARDOWN_NOT_REQUESTED,
                                          NULL,
                                          0,
                                          NULL) != REQUEST_PENDING)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Request_PS_PDP_Context_Deactivate_V2 failed\n");

    mfl_request_control_block_free(rcb_p);
    return (-1);
  }

  mfl_request_control_block_free(rcb_p);
  return (0);
}

/**
 * find_bearer_by_handle()
 **/
static pscc_bearer_t *find_bearer_by_handle(int handle)
{
  mpl_list_t *obj_p;
  pscc_bearer_t *bearer_p;

  MPL_LIST_FOR_EACH(pscc_bearer_list_p, obj_p)
  {
    bearer_p = MPL_LIST_CONTAINER(obj_p, pscc_bearer_t, list_entry);
    if((NULL != bearer_p) && (bearer_p->handle == handle))
      return (bearer_p);
  }
  return NULL;
}
