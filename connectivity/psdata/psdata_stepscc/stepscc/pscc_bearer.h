/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \brief  PSCC Bearer interface
 */


#ifndef PSCC_BEARER_H
#define PSCC_BEARER_H

/********************************************************************************
 *
 * Include files
 *
 ********************************************************************************/
#include "pscc_msg.h"
#include "pscc_handler.h"

/********************************************************************************
 *
 * Defines & Type definitions
 *
 ********************************************************************************/

/**
 * pscc_bearer_event_t
 *
 * @pscc_bearer_event_activating
 * @pscc_bearer_event_activated
 * @pscc_bearer_event_deactivating
 * @pscc_bearer_event_deactivated
 * @pscc_bearer_event_modified
 * @pscc_bearer_event_modify_failed
 * @pscc_bearer_event_data_counters_fetched
 * @pscc_bearer_event_data_counters_reset
 *
 **/
#define PSCC_BEARER_EVENT_VALUES          \
  PSCC_BEARER_EVENT_ELEMENT(activating)   \
  PSCC_BEARER_EVENT_ELEMENT(activated)    \
  PSCC_BEARER_EVENT_ELEMENT(deactivating) \
  PSCC_BEARER_EVENT_ELEMENT(deactivated)  \
  PSCC_BEARER_EVENT_ELEMENT(modified)     \
  PSCC_BEARER_EVENT_ELEMENT(modify_failed)\
  PSCC_BEARER_EVENT_ELEMENT(data_counters_fetched)\
  PSCC_BEARER_EVENT_ELEMENT(data_counters_reset)

#define PSCC_BEARER_EVENT_ELEMENT(EVENT) pscc_bearer_event_##EVENT,
typedef enum
{
  PSCC_BEARER_EVENT_VALUES
  pscc_number_of_bearer_events
} pscc_bearer_event_t;
#undef PSCC_BEARER_EVENT_ELEMENT

/**
 * pscc_bearer_event_fp_t
 *
 * @connid - connection id
 * @event    - what event
 * @reason   - what reason
 * @cause    - cause
 *
 * Returns: -
 *
 **/
typedef void (*pscc_bearer_event_fp_t)(int connid, pscc_bearer_event_t event, pscc_reason_t reason, int cause);

/**
 * pscc_bearer_attach_event_t
 *
 * @pscc_bearer_attach_event_attached
 * @pscc_bearer_attach_event_attach_failed
 * @pscc_bearer_attach_event_attach_status
 * @pscc_bearer_attach_event_detached
 * @pscc_bearer_attach_event_nwi_detached
 * @pscc_bearer_attach_event_detach_failed
 * @pscc_bearer_attach_event_mode_queried
 * @pscc_bearer_attach_event_mode_configured
 * @pscc_bearer_attach_event_mode_configure_failed
 *
 **/
#define PSCC_BEARER_ATTACH_EVENT_VALUES                   \
  PSCC_BEARER_ATTACH_EVENT_ELEMENT(attached)              \
  PSCC_BEARER_ATTACH_EVENT_ELEMENT(attach_failed)         \
  PSCC_BEARER_ATTACH_EVENT_ELEMENT(attach_status)         \
  PSCC_BEARER_ATTACH_EVENT_ELEMENT(detached)              \
  PSCC_BEARER_ATTACH_EVENT_ELEMENT(nwi_detached)          \
  PSCC_BEARER_ATTACH_EVENT_ELEMENT(detach_failed)         \
  PSCC_BEARER_ATTACH_EVENT_ELEMENT(mode_queried)          \
  PSCC_BEARER_ATTACH_EVENT_ELEMENT(mode_configured)       \
  PSCC_BEARER_ATTACH_EVENT_ELEMENT(mode_configure_failed)

#define PSCC_BEARER_ATTACH_EVENT_ELEMENT(EVENT) pscc_bearer_attach_event_##EVENT,
typedef enum
{
  PSCC_BEARER_ATTACH_EVENT_VALUES
  pscc_number_of_bearer_attach_events
} pscc_bearer_attach_event_t;
#undef PSCC_BEARER_ATTACH_EVENT_ELEMENT

/**
 * pscc_bearer_ping_block_mode_event_t
 *
 * @pscc_bearer_ping_block_mode_queried_event
 * @pscc_bearer_ping_block_mode_configured_event
 * @pscc_bearer_ping_block_mode_configure_failed_event
 *
 **/
#define PSCC_BEARER_PING_BLOCK_MODE_EVENT_VALUES                   \
  PSCC_BEARER_PING_BLOCK_MODE_EVENT_ELEMENT(mode_queried)          \
  PSCC_BEARER_PING_BLOCK_MODE_EVENT_ELEMENT(mode_configured)       \
  PSCC_BEARER_PING_BLOCK_MODE_EVENT_ELEMENT(mode_configure_failed)

#define PSCC_BEARER_PING_BLOCK_MODE_EVENT_ELEMENT(EVENT) pscc_bearer_ping_block_##EVENT##_event,
typedef enum
{
  PSCC_BEARER_PING_BLOCK_MODE_EVENT_VALUES
  pscc_number_of_bearer_ping_block_mode_events
} pscc_bearer_ping_block_mode_event_t;
#undef PSCC_BEARER_PING_BLOCK_MODE_EVENT_ELEMENT


/**
 * pscc_bearer_rc_event_t
 *
 * @pscc_bearer_rc_event_requested
 * @pscc_bearer_rc_event_done
 * @pscc_bearer_rc_event_failed
 *
 **/
#define PSCC_BEARER_RC_EVENT_VALUES        \
  PSCC_BEARER_RC_EVENT_ELEMENT(requested)  \
  PSCC_BEARER_RC_EVENT_ELEMENT(done)       \
  PSCC_BEARER_RC_EVENT_ELEMENT(failed)


#define PSCC_BEARER_RC_EVENT_ELEMENT(EVENT) pscc_bearer_rc_event_##EVENT,
typedef enum
{
  PSCC_BEARER_RC_EVENT_VALUES
  pscc_number_of_bearer_rc_events
} pscc_bearer_rc_event_t;
#undef PSCC_BEARER_RC_EVENT_ELEMENT

/**
 * pscc_bearer_rc_event_fp_t
 *
 * @event      - what event
 * @pscc_rc_p  - the resource control object
 *
 * Returns: -
 *
 **/
typedef void (*pscc_bearer_rc_event_fp_t)(pscc_bearer_rc_event_t event, pscc_rc_t* pscc_rc_p);

/**
 * pscc_data_counter_state_t
 * states for data counter handling
 *
 * @pscc_data_counter_state_idle
 * @pscc_data_counter_state_fetching
 * @pscc_data_counter_state_resetting
 *
 **/
#define PSCC_DATA_COUNTER_STATE                   \
  PSCC_DATA_COUNTER_STATE_ELEMENT(idle)          \
  PSCC_DATA_COUNTER_STATE_ELEMENT(fetching)\
  PSCC_DATA_COUNTER_STATE_ELEMENT(resetting)

#define PSCC_DATA_COUNTER_STATE_ELEMENT(LEVEL) pscc_data_counter_state_##LEVEL,
typedef enum
{
  PSCC_DATA_COUNTER_STATE
  pscc_number_of_data_counter_state
} pscc_data_counter_state_t;
#undef PSCC_DATA_COUNTER_STATE_ELEMENT

/**
 * pscc_rc_config_status_t
 * status of resource configuration
 *
 * @pscc_rc_config_status_not_configured
 * @pscc_rc_config_status_configured
 * @pscc_rc_config_status_configure_failed
 *
 **/
#define PSCC_RC_CONFIG_STATUS                     \
  PSCC_RC_CONFIG_STATUS_ELEMENT(not_configured)   \
  PSCC_RC_CONFIG_STATUS_ELEMENT(configured)       \
  PSCC_RC_CONFIG_STATUS_ELEMENT(configure_failed)

#define PSCC_RC_CONFIG_STATUS_ELEMENT(LEVEL) pscc_rc_config_status_##LEVEL,
typedef enum
{
  PSCC_RC_CONFIG_STATUS
  pscc_number_of_rc_config_status
} pscc_rc_config_status_t;
#undef PSCC_RC_CONFIG_STATUS_ELEMENT

/**
 * pscc_aol_conf_state_t
 * states for aol configuration
 *
 * @pscc_aol_conf_state_idle
 * @pscc_aol_conf_state_pending
 * @pscc_aol_conf_state_failed
 *
 **/
#define PSCC_AOL_CONF_STATE                   \
  PSCC_AOL_CONF_STATE_ELEMENT(idle)          \
  PSCC_AOL_CONF_STATE_ELEMENT(pending)\
  PSCC_AOL_CONF_STATE_ELEMENT(failed)

#define PSCC_AOL_CONF_STATE_ELEMENT(LEVEL) pscc_aol_conf_state_##LEVEL,
typedef enum
{
  PSCC_AOL_CONF_STATE
  pscc_number_of_aol_conf_state
} pscc_aol_conf_state_t;
#undef PSCC_AOL_CONF_STATE_ELEMENT

typedef struct pscc_attach_event_params {
  pscc_attach_mode_t   attach_mode;     /* only valid for pscc_bearer_attach_event_mode_queried */
  pscc_attach_status_t attach_status;   /* only valid for pscc_bearer_attach_event_attach_status */
} pscc_attach_event_params_t;

/**
 * pscc_bearer_attach_event_fp_t
 *
 * @event           - what event
 * @attach_status   - current status
 * @cause           - cause
 *
 * Returns: -
 *
 **/
typedef void (*pscc_bearer_attach_event_fp_t)(pscc_bearer_attach_event_t event, pscc_attach_event_params_t attach_event_params, int cause, uint32_t ct);

/**
 * pscc_bearer_ping_block_mode_event_fp_t
 *
 * @event       - what event
 * @ping_mode   - current ping block mode status, not valid when pscc_bearer_ping_block_mode_configure_failed_event
 * @cause       - cause
 *
 * Returns: -
 *
 **/
typedef void (*pscc_bearer_ping_block_mode_event_fp_t)(pscc_bearer_ping_block_mode_event_t event, pscc_ping_block_mode_t ping_mode, uint32_t ct);



/*******************************************************************************
 *
 * Functions and macros
 *
 *******************************************************************************/
/**
 * pscc_bearer_init - initialise
 *
 * Returns: file descriptor, -1 on error
 *
 **/
int pscc_bearer_init(void);

/**
 * pscc_bearer_deinit - de-initialise
 *
 * Returns: -
 *
 **/
void pscc_bearer_deinit(void);

/**
 * pscc_bearer_alloc - allocate a bearer object
 *
 * @connid - connection id
 *
 * Returns: bearer handle, (-1) on error
 *
 **/
int pscc_bearer_alloc(int connid);

/**
 * pscc_bearer_free - free a bearer object
 *
 * @handle - bearer handle to be freed
 *
 * Returns: 0 on success, -1 on error
 *
 **/
int pscc_bearer_free(int handle);

/**
 * pscc_bearer_activate - activate PDP context
 *
 * @handle            - bearer handle
 * @get_param_cb      - callback to get a parameter
 * @set_param_cb      - callback to set a parameter
 * @delete_param_cb   - callback to delete a previously set parameter
 * @event_cb          - callback to report bearer events
 *
 * Returns: 0 on success, -1 on error
 *
 **/
int pscc_bearer_activate
  (
   int handle,
   pscc_get_param_fp_t get_param_cb,
   pscc_set_param_fp_t set_param_cb,
   pscc_delete_param_fp_t delete_param_cb,
   pscc_bearer_event_fp_t event_cb
  );

/**
 * pscc_bearer_deactivate - deactivate PDP context
 *
 * @handle - bearer handle
 *
 * Returns: 0 on success, -1 on error
 *
 **/
int pscc_bearer_deactivate(int handle);

/**
 * pscc_bearer_modify - modify PDP context
 *
 * @handle - bearer handle
 *
 * Returns: 0 on success, -1 on error
 *
 **/
int pscc_bearer_modify(int handle);

/**
 * pscc_bearer_configure_aol_mode - configure always online mode
 *
 * @enable - wether to enable or disable aol mode
 *
 * Returns: 0 on success, -1 on error
 *
 **/
int pscc_bearer_configure_aol_mode (bool enable);

/**
 * pscc_bearer_generate_uplink_data - generate uplink data
 *
 * @handle - bearer handle
 *
 * Returns: 0 on success, -1 on error
 *
 **/
int pscc_bearer_generate_uplink_data(int handle);

/**
 * pscc_bearer_handler -
 *
 * @fd - file descriptor
 *
 * Returns: 0 on success, -1 on error
 *
 **/
int pscc_bearer_handler(int fd);

/**
 * pscc_bearer_attach - attach to gprs network
 *
 * @event_cb - callback pointer for notifying events
 * @ct - clienttag used for identifying the caller
 *
 * Returns: 0 on success, -1 on error
 *
 **/
int pscc_bearer_attach
  (
   pscc_bearer_attach_event_fp_t event_cb,
   uint32_t ct
  );

/**
 * pscc_bearer_detach - detach from grps network
 *
 * @event_cb - callback pointer for notifying events
 * @ct - clienttag used for identifying the caller
 *
 * Returns: 0 on success, -1 on error
 *
 **/
int pscc_bearer_detach
  (
   pscc_bearer_attach_event_fp_t event_cb,
   uint32_t ct
  );

/**
 * pscc_bearer_attach_status - check if gprs network is attached
 *
 * @attach_status_p - pointer to the current attach status
 *
 * Returns: 0 on success, -1 on error
 *
 **/
int pscc_bearer_attach_status(pscc_attach_status_t *attach_status_p);

/**
 * pscc_bearer_query_attach_mode - query attach mode
 *
 * @event_cb - callback pointer for notifying events
 * @ct - clienttag used for identifying the caller
 *
 * Returns: 0 on success, -1 on error
 *
 **/
int pscc_bearer_query_attach_mode
  (
   pscc_bearer_attach_event_fp_t event_cb,
   uint32_t ct
  );

/**
 * pscc_bearer_query_ping_block_mode - query ping block mode
 *
 * @event_cb - callback pointer for notifying events
 * @ct - clienttag used for identifying the caller
 *
 * Returns: 0 on success, -1 on error
 *
 **/
int pscc_bearer_query_ping_block_mode
  (
   pscc_bearer_ping_block_mode_event_fp_t event_cb,
   uint32_t ct
  );

/**
 * pscc_bearer_query_attach_status - query attach status
 *
 * @event_cb - callback pointer for notifying events
 * @ct - clienttag used for identifying the caller
 *
 * Returns: 0 on success, -1 on error
 *
 **/
int pscc_bearer_query_attach_status
  (
   pscc_bearer_attach_event_fp_t event_cb,
   uint32_t ct
  );

/**
 * pscc_bearer_configure_attach_mode - configure attach mode
 *
 * @event_cb    - callback pointer for notifying events
 * @attach_mode - attach mode to set
 * @ct          - client tag used for identifying the caller
 *
 * Returns: 0 on success, -1 on error
 *
 **/
int pscc_bearer_configure_attach_mode
  (
   pscc_bearer_attach_event_fp_t event_cb,
   pscc_attach_mode_t attach_mode,
   uint32_t ct
  );

/**
 * pscc_bearer_configure_ping_block - configure ping block mode
 *
 * @event_cb  - callback pointer for notifying events
 * @ping_mode - the ping block mode to set
 * @ct        - client tag used for identifying the caller
 *
 * Returns: 0 on success, -1 on error
 **/
int pscc_bearer_configure_ping_block_mode
  (
   pscc_bearer_ping_block_mode_event_fp_t event_cb,
   pscc_ping_block_mode_t ping_mode,
   uint32_t ct
  );

/**
 * pscc_bearer_data_counters - fetch/reset data counters
 *
 * @handle - bearer handle
 * @fetch - is the request fetch (true) or reset (false)
 *
 * Returns: 0 on success, -1 on error
 *
 **/
int pscc_bearer_data_counters (int handle, bool fetch);

/**
 * pscc_bearer_respond_resource_control - respond to resource control
 *
 * @handle - bearer handle
 * @accepted - was call control accepted
 * @modified - did call control modified the message
 *
 * Returns: 0 on success, -1 on error
 *
 **/
int pscc_bearer_respond_resource_control (pscc_rc_t* pscc_rc_p, bool accepted, bool modified);

/**
 * pscc_bearer_configure_rc - configure resource control
 *
 * @rc_event_cb - Callback function for resource control
 *
 * Returns: 0 on success, -1 on error
 *
 **/
int pscc_bearer_configure_rc (pscc_bearer_rc_event_fp_t rc_event_cb);

/**
 * pscc_bearer_netlnk_init - get netlnk file descriptor
 *
 * Returns: file descriptor, -1 on error
 *
 **/
int pscc_bearer_netlnk_init(void);

/**
 * pscc_bearer_netlnk_handle_message - parse the netlnk message
 *
 * @fd: file descriptor
 *
 * Returns: 0 on success, -1 on error
 *
 **/
int pscc_bearer_netlnk_handle_message(int fd);

#endif
