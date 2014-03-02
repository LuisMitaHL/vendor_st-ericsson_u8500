/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \brief  PSCC SIM interface
 */


#ifndef PSCC_SIM_H
#define PSCC_SIM_H

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
 * pscc_sim_event_t
 *
 * @pscc_sim_event_cc_accept
 * @pscc_sim_event_cc_reject
 * @pscc_sim_event_cc_modified
 *
 **/
#define PSCC_SIM_EVENT_VALUES          \
  PSCC_SIM_EVENT_ELEMENT(cc_accept)\
  PSCC_SIM_EVENT_ELEMENT(cc_reject)\
  PSCC_SIM_EVENT_ELEMENT(cc_modified)

#define PSCC_SIM_EVENT_ELEMENT(EVENT) pscc_sim_event_##EVENT,
typedef enum
{
  PSCC_SIM_EVENT_VALUES
  pscc_number_of_sim_events
} pscc_sim_event_t;
#undef PSCC_SIM_EVENT_ELEMENT

/**
 * pscc_cc_gprs_availability_t
 * availability of cc for gprs
 *
 * @pscc_cc_gprs_availability_unknown
 * @pscc_cc_gprs_availability_available
 * @pscc_cc_gprs_availability_unavailable
 *
 **/
#define PSCC_CC_GPRS_AVAILABILITY                     \
  PSCC_CC_GPRS_AVAILABILITY_ELEMENT(unknown)          \
  PSCC_CC_GPRS_AVAILABILITY_ELEMENT(available)        \
  PSCC_CC_GPRS_AVAILABILITY_ELEMENT(unavailable)

#define PSCC_CC_GPRS_AVAILABILITY_ELEMENT(LEVEL) pscc_cc_gprs_availability_##LEVEL,
typedef enum
{
  PSCC_CC_GPRS_AVAILABILITY
  pscc_number_of_cc_gprs_availability
} pscc_cc_gprs_availability_t;
#undef PSCC_CC_GPRS_AVAILABILITY_ELEMENT

/**
 * pscc_sim_event_fp_t
 *
 * @connid - connection id
 * @event  - what event
 *
 * Returns: -
 *
 **/
typedef void (*pscc_sim_event_fp_t)(pscc_sim_event_t event, pscc_rc_t* pscc_rc_p);

/*******************************************************************************
 *
 * Functions and macros
 *
 *******************************************************************************/
/**
 * pscc_sim_init - initialise
 *
 * Returns: file descriptor, (-1) on error
 *
 **/
int pscc_sim_init(void);

/**
 * pscc_sim_deinit - de-initialise
 *
 * Returns: -
 *
 **/
void pscc_sim_deinit(void);

/**
 * pscc_sim_alloc - allocate a sim object
 *
 * @connid - connection id
 *
 * Returns: sim handle, (-1) on error
 *
 **/
int pscc_sim_alloc(int connid);

/**
 * pscc_sim_free - free a sim object
 *
 * @handle - sim handle to be freed
 *
 * Returns: 0 on success, (-1) on error
 *
 **/
int pscc_sim_free(int handle);

/**
 * pscc_sim_handler -
 *
 * @fd - filedescriptor to handle
 *
 * Returns: 0 on success, (-1) on error
 *
 **/
int pscc_sim_handler(int fd);

/**
 * pscc_sim_cc_gprs_availability - check if cc for gprs is available
 *
 * Returns: 0 on success, (-1) on error
 *
 **/
int pscc_sim_cc_gprs_availability(pscc_cc_gprs_availability_t* pscc_cc_gprs_availability_p);

/**
 * pscc_sim_perform_cc_gprs - perform call control on PDP context activation message
 *
 * @pscc_rc_p - resource control object to perform call control on
 * @event_cb - callback function
 *
 * Returns: 0 on success, (-1) on error
 *
 **/
int pscc_sim_perform_cc(pscc_rc_t *pscc_rc_p, pscc_sim_event_fp_t event_cb);

#endif
