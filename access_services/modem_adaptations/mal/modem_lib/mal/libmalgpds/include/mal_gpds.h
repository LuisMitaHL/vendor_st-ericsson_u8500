/**
 * \file mal_gpds.h
 * \brief This file includes headers for GPDS MAL.
 *
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \n MAL GPDS header file for GPDS Library.
 * \n \author ST-Ericsson
 * \n
 * \n Revision History:
 * \n
 * \n v1.0 Initial Version
 * \n v2.0 Added support for
 * \n    PS Attach Management
 * \n     PS Properties such as AOL, MT PDP management.
 * \n   QoS Management
 * \n v3.0 Added support for phonet stack to work in
 * \n    test or normal mode
 * \n
 * \n v4.0 Added a API to query GPDS configuration
 * \n
 * \n V5.0 Added an API to query the PS attach status
 * \n      Added support for NW initiated detach event
 * \n
 * \n V6.0 Added an API to query the Tx and Rx Data
 * \n      byte counts for particular active PDP Context id
 * \n
 * \n v7.0 Added netlink socket fd, used to communicate with shm driver
 * \n    for modem silent reboot.
 * \n
 * \n v8.0 Added API for Resource Configuration
 * \n      Added API for Resource Control
 * \n
 */

/**
 * \defgroup  MAL_GPDS GPDS API
 * \par
 * \n This part describes the interface to GPDS MAL Client
 */

#ifndef __MAL_GPDS_H
#define __MAL_GPDS_H

#include <stdint.h>

/*Constants*/
#define MAL_GPDS_MAX_IPV4_PDPADDRESS_SIZE 4
#define MAL_GPDS_MAX_PDPADDRESS_SIZE    16
#define MAL_GPDS_MAX_PACKET_FILTERS_SIZE     8
#define MAL_GPDS_MAX_APN_SIZE    100
#define MAL_GPDS_MAX_USERNAME_SIZE     53
#define MAL_GPDS_MAX_PASSWORD_SIZE     53
#define MAL_GPDS_MAX_CHALLENGE_SIZE    50
#define MAL_GPDS_MAX_RESPONSE_SIZE     50
#define MAL_GPDS_IPV4_ADDRESS_PRESENT 0x01
#define MAL_GPDS_IPV6_ADDRESS_PRESENT 0x02
#define MAL_GPDS_IPV4_PDNS_PRESENT    0x04
#define MAL_GPDS_IPV6_PDNS_PRESENT    0x08
#define MAL_GPDS_IPV4_SDNS_PRESENT    0x10
#define MAL_GPDS_IPV6_SDNS_PRESENT    0x20

/*-------------------------*/
/* GPDS Request IDs           */
/*-------------------------*/

/**
 * \def MAL_GPDS_REQ_PSCONN_CREATE
 * \brief Request ID to Create a PS connections.
 * \brief Each connection corresponds to one PDP context (primary/secondary).
 * \brief \ref mal_gpds_request is used to execute this request. \n Argument details.
 * \param [in] conn_id Connection id to create (should be unique).
 * \param [in] mal_gpds_req_id is set to MAL_GPDS_REQ_PSCONN_CREATE
 * \param [in] data is set to NULL
 * \return 0 for success and negative for failure.
 */
#define MAL_GPDS_REQ_PSCONN_CREATE        0x01

/**
 * \def MAL_GPDS_REQ_PSCONN_RELEASE
 * \brief Request ID to delete existing PS connections. \n Make sure to
 * deactivate the connection if active before connection release.
 * \brief \ref mal_gpds_request is used to execute this request. \n Argument details.
 * \param [in] conn_id Connection id to delete.
 * \param [in] mal_gpds_req_id is set to MAL_GPDS_REQ_PSCONN_RELEASE
 * \param [in] data is set to NULL
 * \return 0 for success and negative for failure.
 */
#define MAL_GPDS_REQ_PSCONN_RELEASE        0x02

/**
 * \def MAL_GPDS_REQ_PSCONN_ACTIVATE
 * \brief Request ID to activate existing PS connections.
 * \brief \ref mal_gpds_request is used to execute this request. \n Argument details.
 * \param [in] conn_id Connection id to create.
 * \param [in] mal_gpds_req_id is set to MAL_GPDS_REQ_PSCONN_ACTIVATE
 * \param [in] Phonet configuration type set to  \ref mal_gpds_phonet_conf_t
 * \return 0 for success and negative for failure. \n Actual activation result is
 * indicated by a callback function \ref mal_gpds_event_cb_t.
 */
#define MAL_GPDS_REQ_PSCONN_ACTIVATE     0x03

/**
 * \def MAL_GPDS_REQ_PSCONN_DEACTIVATE
 * \brief Request ID to deactivate active PS connections.
 * \brief \ref mal_gpds_request is used to execute this request. \n Argument details.
 * \param [in] conn_id Connection id to deactivate.
 * \param [in] mal_gpds_req_id is set to MAL_GPDS_REQ_PSCONN_DEACTIVATE
 * \param [in] data is set to NULL
 * \return 0 for success and negative for failure. \n Actual deactivation result is
 * indicated by a callback function \ref mal_gpds_event_cb_t.
 */
#define MAL_GPDS_REQ_PSCONN_DEACTIVATE    0x04

/**
 * \def MAL_GPDS_REQ_PSCONN_MODIFY
 * \brief Modify QoS of active PDP context.
 * \brief \ref mal_gpds_request is used to execute this request. \n Argument details.
 * \param [in] conn_id Connection id to modify QoS
 * \param [in] mal_gpds_req_id is set to MAL_GPDS_REQ_PSCONN_MODIFY
 * \param [in] data is set to NULL.
 */
#define MAL_GPDS_REQ_PSCONN_MODIFY 0x05

/**
 * \def MAL_GPDS_REQ_CONFIG_CHANGE
 * \brief Request ID to change general configuration.
 * \brief This request is optional.
 * \param [in] conn_id Don't care.
 * \param [in] mal_gpds_req_id is set to MAL_GPDS_REQ_CONFIG_CHANGE
 * \param [in] data is set to \ref mal_gpds_config_t structure.
 * \return 0 for success and negative for failure. \n Actual deactivation result is
 * indicated by a callback function \ref mal_gpds_event_cb_t with event set to
 * MAL_GPDS_EVENT_ID_CONFIGURED or MAL_GPDS_EVENT_ID_CONFIGURATION_FAILED.
 */
#define MAL_GPDS_REQ_CONFIG_CHANGE 0x06
/**
 * \def MAL_GPDS_REQ_PS_ATTACH
 * \brief Request ID to explicitly attach to PS bearer service.
 * \brief This request is optional and if not used, attach to PS
 * \brief bearer service will happen before activating first PDP context.
 * \return 0 for success and negative for failure. \n Actual attach result is
 * indicated by a callback function \ref mal_gpds_event_cb_t with event set to
 * MAL_GPDS_EVENT_ID_PS_ATTACHED or MAL_GPDS_EVENT_ID_PS_ATTACH_FAILED.
 */
#define MAL_GPDS_REQ_PS_ATTACH 0x07

/**
 * \def MAL_GPDS_REQ_PS_DETACH
 * \brief Request ID to explicitly detach from PS bearer service.
 * \brief This request is optional and if not used, detach from PS
 * \brief bearer service will happen after deactivating last PDP context.
 * \brief Note that all PDP context are lost if this request is successful.
 * \return 0 for success and negative for failure. \n Actual detach result is
 * indicated by a callback function \ref mal_gpds_event_cb_t with event set to
 * MAL_GPDS_EVENT_ID_PS_DETACHED or MAL_GPDS_EVENT_ID_PS_NWI_DETACHED  or
 * MAL_GPDS_EVENT_ID_PS_DETACH_FAILED.
 */
#define MAL_GPDS_REQ_PS_DETACH 0x08

/**
 * \def  MAL_GPDS_REQ_QUERY_CONFIG
 * \brief  Request ID to query current GPDS Configuration
 * \param [in] conn_id Don't care.
 * \param [in] mal_gpds_req_id is set to MAL_GPDS_REQ_QUERY_CONFIG
 * \param [in] data is NULL
 * \return 0 for success and negative for failure. \n Query Configuration  result is
 * indicated by a callback function \ref mal_gpds_event_cb_t with event set to
 * MAL_GPDS_EVENT_ID_CONFIG_QUERIED
 */
#define MAL_GPDS_REQ_QUERY_CONFIG 0x09

/**
 * \def MAL_GPDS_REQ_PS_ATTACH_STATUS
 * \brief Request ID to explicitly inquire attach status of the GPDS
 * \return 0 for success and negative for failure.  Actual PS attach result is
 * indicated by a callback function \ref mal_gpds_event_cb_t with event set to
 * MAL_GPDS_EVENT_ID_PS_ATTACH_STATUS
 */
#define MAL_GPDS_REQ_PS_ATTACH_STATUS 0x0A
/**
 * \def MAL_GPDS_REQ_DATA_COUNTER
 * \brief Request ID to inquire Tx and Rx byte counts for a particular active PDP context
 * \param [in] conn_id to inquire Tx and Rx data byte counts
 * \param [in] mal_gpds_req_id is set to MAL_GPDS_REQ_DATA_COUNTER
 * \param [in] data is set to NULL.
 * \return 0 for success and negative for failure.  Actual Data counter  result is
 * indicated by a callback function \ref mal_gpds_event_cb_t with event set to
 * MAL_GPDS_EVENT_ID_DATA_COUNTER
 */
#define MAL_GPDS_REQ_DATA_COUNTER 0x0B
/**
 * \def MAL_GPDS_REQ_RESOURCE_CONFIGURE
 * \brief Request ID to configure resources for GPRS
 * \param [in] conn_id Don't care.
 * \param [in] mal_gpds_req_id is set to MAL_GPDS_REQ_RESOURCE_CONFIGURE
 * \param [in] data is set to NULL.
 * \return 0 for success and negative for failure.  Actual Resource control  result is
 * indicated by a callback function \ref mal_gpds_event_cb_t with event set to
 * MAL_GPDS_EVENT_ID_RESOURCE_CONFIGURED or MAL_GPDS_EVENT_ID_RESOURCE_CONF_FAILED.
 */
#define MAL_GPDS_REQ_RESOURCE_CONFIGURE 0x0C
/**
 * \def MAL_GPDS_REQ_RESOURCE_CONTROL
 * \brief Request ID to allow or deny the resource control
 * \param [in] conn_id Don't care.
 * \param [in] mal_gpds_req_id is set to MAL_GPDS_REQ_RESOURCE_CONTROL
 * \param [in] data is set to \ref mal_gpds_resource_control_req_info_t structure.
 * \return 0 for success and negative for failure.  Actual Resource control  result is
 * indicated by a callback function \ref mal_gpds_event_cb_t with event set to
 * MAL_GPDS_EVENT_ID_RESOURCE_CONTROL_DONE or MAL_GPDS_EVENT_ID_RESOURCE_CONTROL_FAILED.
 */
#define MAL_GPDS_REQ_RESOURCE_CONTROL 0x0D
/**
 * \typedef mal_gpds_req_ids_t
 * \brief Request IDs.
 * \brief Takes one of the following values.
 * \par
 * \n \ref MAL_GPDS_REQ_PSCONN_CREATE
 * \n \ref MAL_GPDS_REQ_PSCONN_RELEASE
 * \n \ref MAL_GPDS_REQ_PSCONN_ACTIVATE
 * \n \ref MAL_GPDS_REQ_PSCONN_DEACTIVATE
 * \n \ref MAL_GPDS_REQ_PSCONN_MODIFY
 * \n \ref MAL_GPDS_REQ_CONFIG_CHANGE
 * \n \ref MAL_GPDS_REQ_PS_ATTACH
 * \n \ref MAL_GPDS_REQ_PS_DETACH
 * \n \ref MAL_GPDS_REQ_QUERY_CONFIG
 * \n \ref MAL_GPDS_REQ_PS_ATTACH_STATUS
 * \n \ref MAL_GPDS_REQ_DATA_COUNTER
 * \n \ref MAL_GPDS_REQ_RESOURCE_CONFIGURE
 * \n \ref MAL_GPDS_REQ_RESOURCE_CONTROL
 */
typedef uint8_t mal_gpds_req_ids_t;

/*-------------------------*/
/* GPDS Event IDs          */
/*-------------------------*/

/**
 * \def MAL_GPDS_EVENT_ID_PDP_ACTIVATING
 * \brief Event ID to to indicate PDP activation progress.
 * \brief \ref mal_gpds_event_cb_t is used to inform this event. \n Argument details.
 * \param [out] conn_id Connection id to which this event is associated.
 * \param [out] mal_gpds_event_id is set to MAL_GPDS_EVENT_ID_PDP_ACTIVATING
 * \param [out] data is set to NULL
 * \return None.
 */
#define MAL_GPDS_EVENT_ID_PDP_ACTIVATING    0x00

/**
 * \def MAL_GPDS_EVENT_ID_PDP_ACTIVE
 * \brief Event ID to to indicate PDP activation success.
 * \brief \ref mal_gpds_event_cb_t is used to inform this event. \n Argument details.
 * \param [out] conn_id Connection id to which this event is associated.
 * \param [out] mal_gpds_event_id is set to MAL_GPDS_EVENT_ID_PDP_ACTIVE
 * \param [out] data is set to NULL
 * \return None.
 */
#define MAL_GPDS_EVENT_ID_PDP_ACTIVE            0x01

/**
 * \def MAL_GPDS_EVENT_ID_PDP_ACTIVATION_FAILED
 * \brief Event ID to to indicate PDP activation failure.
 * \brief \ref mal_gpds_event_cb_t is used to inform this event. \n Argument details.
 * \param [out] conn_id Connection id to which this event is associated.
 * \param [out] mal_gpds_event_id is set to MAL_GPDS_EVENT_ID_PDP_ACTIVATION_FAILED
 * \param [out] data is set to activation failure cause. Refer 3GPP TS 24.008 for details.
 * \return None.
 */
#define MAL_GPDS_EVENT_ID_PDP_ACTIVATION_FAILED    0x02

/**
 * \def MAL_GPDS_EVENT_ID_PDP_SUSPENDED
 * \brief Event ID to to indicate temporary suspension of connection.
 * \brief \ref mal_gpds_event_cb_t is used to inform this event. \n Argument details.
 * \param [out] conn_id Connection id to which this event is associated.
 * \param [out] mal_gpds_event_id is set to MAL_GPDS_EVENT_ID_PDP_SUSPENDED.
 * \param [out] data is set to NULL
 * \return None.
 */
#define MAL_GPDS_EVENT_ID_PDP_SUSPENDED            0x03

/**
 * \def MAL_GPDS_EVENT_ID_PDP_DEACTIVATED
 * \brief Event ID to to indicate PDP deactivation success.
 * \brief \ref mal_gpds_event_cb_t is used to inform this event. \n Argument details.
 * \param [out] conn_id Connection id to which this event is associated.
 * \param [out] mal_gpds_event_id is set to MAL_GPDS_EVENT_ID_PDP_DEACTIVATED
 * \param [out] data is set to NULL
 * \return None.
 */
#define MAL_GPDS_EVENT_ID_PDP_DEACTIVATED        0x04

/**
 * \def MAL_GPDS_EVENT_ID_NWI_PDP_REQUESTED
 * \note Unsupported.
 * \brief Event ID to indicate network initiated PDP context activation request.
 * \brief \ref mal_gpds_event_cb_t is used to inform this event. \n Argument details.
 * \param [out] conn_id Connection id to which this event is associated.
 * \param [out] mal_gpds_event_id is set to MAL_GPDS_EVENT_ID_NWI_PDP_REQUESTED
 * \param [out] data points to structure of type \ref mal_gpds_nwi_pdp_params_t .
 * \return None.
 */
#define MAL_GPDS_EVENT_ID_NWI_PDP_REQUESTED        0x05

/**
 * \def MAL_GPDS_EVENT_ID_PDP_DEACTIVATION_FAILED
 * \brief Event ID to indicate PDP deactivation failure.
 * \brief \ref mal_gpds_event_cb_t is used to inform this event. \n Argument details.
 * \param [out] conn_id Connection id to which this event is associated.
 * \param [out] mal_gpds_event_id is set to MAL_GPDS_EVENT_ID_PDP_DEACTIVATION_FAILED
 * \param [out] data is set to activation failure cause.
 * \return None.
 */
#define MAL_GPDS_EVENT_ID_PDP_DEACTIVATION_FAILED    0x06

/**
 * \def MAL_GPDS_EVENT_ID_PS_ATTACHED
 * \brief Event ID to indicate attach to PS service while activating PDP context
 * \brief if PS service attach is configured as automatic. This indication
 * \brief shall come only while activating first PDP context. \n Also
 * this indication shall be sent for any successful attach in response to request
 * MAL_GPDS_REQ_PS_ATTACH.
 * \param [out] conn_id Don't care
 * \param [out] mal_gpds_event_id is set to MAL_GPDS_EVENT_ID_PS_ATTACHED
 * \param [out] data is set to Null.
 */
#define MAL_GPDS_EVENT_ID_PS_ATTACHED    0x07

/**
 * \def MAL_GPDS_EVENT_ID_PS_ATTACH_FAILED
 * \brief Event ID to indicate attach failure to PS service in response to request
 * MAL_GPDS_REQ_PS_ATTACH.
 * \param [out] conn_id Don't care
 * \param [out] mal_gpds_event_id is set to MAL_GPDS_EVENT_ID_PS_ATTACH_FAILED
 * \param [out] data is set to cause of attach failure.
 */
#define MAL_GPDS_EVENT_ID_PS_ATTACH_FAILED 0x08

/**
 * \def MAL_GPDS_EVENT_ID_PS_DETACHED
 * \brief Event ID to indicate dettach from PS service at anytime.
 * \brief This event also indicated deactivation of all active PDP context, and
 * \brief no seperate indication is sent for deactivation.
 */
#define MAL_GPDS_EVENT_ID_PS_DETACHED    0x09

/**
 * \def MAL_GPDS_EVENT_ID_PS_DETACH_FAILED
 * \brief Event ID to indicate dettach failure to PS service in response to request
 * MAL_GPDS_REQ_PS_DETACH.
 */
#define MAL_GPDS_EVENT_ID_PS_DETACH_FAILED 0x0A

/**
 * \def MAL_GPDS_EVENT_ID_CONFIGURED
 * \brief Event ID to indicate change in GPDS general configuration.
 * \param [out] conn_id Don't care
 * \param [out] mal_gpds_event_id is set to MAL_GPDS_EVENT_ID_CONFIGURED
 * \param [out] data is set to mal_gpds_config_t with new configuration.
 */
#define MAL_GPDS_EVENT_ID_CONFIGURED 0x0B

/**
 * \def MAL_GPDS_EVENT_ID_CONFIGURATION_FAILED
 * \brief Event ID to indicate change in GPDS general configuration.
 * \param [out] conn_id Don't care
 * \param [out] mal_gpds_event_id is set to MAL_GPDS_EVENT_ID_CONFIGURATION_FAILED
 * \param [out] data is set to null.
 */
#define MAL_GPDS_EVENT_ID_CONFIGURATION_FAILED 0x0C

/**
 * \def MAL_GPDS_EVENT_ID_CONTEXT_MODIFIED
 * \brief Event ID to indicate Successfull Context Modification.
 * \param [out] conn_id Connection id to which this event is associated.
 * \param [out] mal_gpds_event_id is set to MAL_GPDS_EVENT_ID_CONTEXT_MODIFIED
 * \param [out] data is set to null.
 */
#define MAL_GPDS_EVENT_ID_CONTEXT_MODIFIED 0x0D

/**
 * \def MAL_GPDS_EVENT_ID_CONTEXT_MODIFICATION_FAILED
 * \brief Event ID to indicate change in GPDS general configuration.
 * \param [out] conn_id Connection id to which this event is associated.
 * \param [out] mal_gpds_event_id is set to MAL_GPDS_EVENT_ID_CONTEXT_MODIFICATION_FAILED
 * \param [out] data is set to null.
 */
#define MAL_GPDS_EVENT_ID_CONTEXT_MODIFICATION_FAILED 0x0E

/**
 * \def MAL_GPDS_EVENT_ID_CONFIG_QUERIED
 * \brief EVENT_ID to inicate that GPDS configuration has been queried.
 * \param [out] conn_id Don't care.
 * \param [out] mal_gpds_event_id is set to MAL_GPDS_EVENT_ID_CONFIG_QUERIED.
 * \param [out] data is set to \ref mal_gpds_config_t structure.
*/
#define MAL_GPDS_EVENT_ID_CONFIG_QUERIED 0x0F

/**
 * \def MAL_GPDS_EVENT_ID_PS_NWI_DETACHED
 * \brief Event ID to indicate for network originated detach
 * \param [out] conn_id Don't care .
 * \param [out] mal_gpds_event_id is set to MAL_GPDS_EVENT_ID_PS_NWI_DETACHED
 * \param [out] data is set to \ref mal_gpds_detach_info_t.
 */
#define MAL_GPDS_EVENT_ID_PS_NWI_DETACHED 0x10

/**
 * \def MAL_GPDS_EVENT_ID_PS_ATTACH_STATUS
 * \brief Event ID to indicate the PS attach status
 * \param [out] conn_id Don't care
 * \param [out] mal_gpds_event_ids_t is set to MAL_GPDS_EVENT_ID_PS_ATTACH_STATUS
 * \param [out] data is set to PS attach status \ref mal_gpds_ps_attach_status_t
 */
#define MAL_GPDS_EVENT_ID_PS_ATTACH_STATUS 0x11

/**
 * \def MAL_GPDS_EVENT_ID_DATA_COUNTER
 * \brief Event ID to indicate Tx and Rx data byte counts for particular active PDP Context.
 * \param [out] conn_id Connection id to which this event is associated.
 * \param [out] mal_gpds_event_ids_t is set to MAL_GPDS_EVENT_ID_DATA_COUNTER
 * \param [out] data is set to \ref mal_gpds_data_counter_info_t
 */
#define MAL_GPDS_EVENT_ID_DATA_COUNTER 0x12

/**
 * \def MAL_GPDS_EVENT_ID_RESOURCE_CONFIGURED
 * \brief Event ID to indicate the resource is configured.
 * \param [out] conn_id Don't care.
 * \param [out] mal_gpds_event_ids_t is set to MAL_GPDS_EVENT_ID_RESOURCE_CONFIGURED
 * \param [out] data is set to \ref mal_gpds_configuration_status_t
 */
#define MAL_GPDS_EVENT_ID_RESOURCE_CONFIGURED 0x13

/**
 * \def MAL_GPDS_EVENT_ID_RESOURCE_CONF_FAILED
 * \brief Event ID to indicate that the resource configuration is failed.
 * \param [out] conn_id Don't care.
 * \param [out] mal_gpds_event_ids_t is set to MAL_GPDS_EVENT_ID_RESOURCE_CONF_FAILED
 * \param [out] data is set to NULL
 */
#define MAL_GPDS_EVENT_ID_RESOURCE_CONF_FAILED 0x14

/**
 * \def MAL_GPDS_EVENT_ID_RESOURCE_CONTROL_REQUESTED
 * \brief Event ID to indicate if the resource control is allowed or denied.
 * \param [out] conn_id Don't care.
 * \param [out] mal_gpds_event_ids_t is set to MAL_GPDS_EVENT_ID_RESOURCE_CONTROL_REQUESTED
 * \param [out] data is set to \ref mal_gpds_resource_control_info_t
 */
#define MAL_GPDS_EVENT_ID_RESOURCE_CONTROL_REQUESTED 0x15

/**
 * \def MAL_GPDS_EVENT_ID_RESOURCE_CONTROL_DONE
 * \brief Event ID to indicate the resource control is successful.
 * \param [out] conn_id Don't care.
 * \param [out] mal_gpds_event_ids_t is set to MAL_GPDS_EVENT_ID_RESOURCE_CONTROL_DONE
 * \param [out] data is set to NULL
 */
#define MAL_GPDS_EVENT_ID_RESOURCE_CONTROL_DONE 0x16

/**
 * \def MAL_GPDS_EVENT_ID_RESOURCE_CONTROL_FAILED
 * \brief Event ID to indicate that the resource control is failed.
 * \param [out] conn_id Don't care.
 * \param [out] mal_gpds_event_ids_t is set to MAL_GPDS_EVENT_ID_RESOURCE_CONTROL_FAILED
 * \param [out] data is set to NULL
 */
#define MAL_GPDS_EVENT_ID_RESOURCE_CONTROL_FAILED 0x17

/**
 * \typedef mal_gpds_event_ids_t
 * \brief Event IDs.
 * \brief Takes one of the following values.
 * \par
 * \n \ref MAL_GPDS_EVENT_ID_PDP_ACTIVATING
 * \n \ref MAL_GPDS_EVENT_ID_PDP_ACTIVE
 * \n \ref MAL_GPDS_EVENT_ID_PDP_ACTIVATION_FAILED
 * \n \ref MAL_GPDS_EVENT_ID_PDP_SUSPENDED
 * \n \ref MAL_GPDS_EVENT_ID_PDP_DEACTIVATED
 * \n \ref MAL_GPDS_EVENT_ID_NWI_PDP_REQUESTED
 * \n \ref MAL_GPDS_EVENT_ID_PDP_DEACTIVATION_FAILED
 * \n \ref MAL_GPDS_EVENT_ID_PS_ATTACHED
 * \n \ref MAL_GPDS_EVENT_ID_PS_ATTACH_FAILED
 * \n \ref MAL_GPDS_EVENT_ID_PS_DETACHED
 * \n \ref MAL_GPDS_EVENT_ID_PS_DETACH_FAILED
 * \n \ref MAL_GPDS_EVENT_ID_CONFIGURED
 * \n \ref MAL_GPDS_EVENT_ID_CONFIGURATION_FAILED
 * \n \ref MAL_GPDS_EVENT_ID_CONTEXT_MODIFIED
 * \n \ref MAL_GPDS_EVENT_ID_CONTEXT_MODIFICATION_FAILED
 * \n \ref MAL_GPDS_EVENT_ID_CONFIG_QUERIED
 * \n \ref MAL_GPDS_EVENT_ID_PS_NWI_DETACHED
 * \n \ref MAL_GPDS_EVENT_ID_PS_ATTACH_STATUS
 * \n \ref MAL_GPDS_EVENT_ID_DATA_COUNTER
 * \n \ref MAL_GPDS_EVENT_ID_RESOURCE_CONFIGURED
 * \n \ref MAL_GPDS_EVENT_ID_RESOURCE_CONF_FAILED
 * \n \ref MAL_GPDS_EVENT_ID_RESOURCE_CONTROL_STATUS
 * \n \ref MAL_GPDS_EVENT_ID_RESOURCE_CONTROL_DONE
 * \n \ref MAL_GPDS_EVENT_ID_RESOURCE_CONTROL_FAILED
 */
typedef uint8_t mal_gpds_event_ids_t;

/**
 * \enum mal_gpds_params_id_t
 * \brief This provides list of parameters which can be set to gpds mal
 * \brief using \ref mal_gpds_set_param or
 * \brief retrieved from gpds mal using \ref mal_gpds_get_param.
 * \param MAL_PARAMS_APN_ID Set APN - Set APN. The data points to null terminated string.
 * \param MAL_PARAMS_AUTH_METHOD_ID - Set Authentication method . The data points to \ref mal_gpds_auth_method_t
 * \param MAL_PARAMS_USERNAME_ID - Set username - The data points to null terminated string.
 * \param MAL_PARAMS_PASSWORD_ID - Set password - The data points to null terminated string.
 * \param MAL_PARAMS_CHAP_IDENTIFIER_ID - Set CHAP identifier. The data points to unsigned byte.
 * \param MAL_PARAMS_CHALLENGE_DATA_ID - Set challenge data. Use pointer to \ref mal_gpds_chap_auth_data_t to pass this.
 * \param MAL_PARAMS_RESPONSE_DATA_ID - Set response (calculated using MD5 algorithm). Use pointer to \ref mal_gpds_chap_auth_data_t to pass this.
 * \param MAL_PARAMS_PDP_TYPE_ID - Set PDP type.
 * \param MAL_PARAMS_PDP_CONTEXT_TYPE_ID - Set PDP Context type.
 * \param MAL_PARAMS_PRIMARY_CID_ID - Set Primary cid for secondary.
 * \param MAL_PARAMS_IP_MAP_ID - Get IP Address MAP.
 * \param MAL_PARAMS_IP_ID - Get IP Address.
 * \param MAL_PARAMS_IPV6_ID - Get IPv6 Address.
 * \param MAL_PARAMS_PDNS_ID - Get Primary DNS.
 * \param MAL_PARAMS_IPV6_PDNS_ID - Get IPv6Primary DNS.
 * \param MAL_PARAMS_SDNS_ID - Get Secondary DNS.
 * \param MAL_PARAMS_IPV6_SDNS_ID - Get IPv6 Secondary DNS.
 * \param MAL_PARAMS_IF_NAME_ID - Get network interface name.
 * \param MAL_PARAMS_HCMP_ID - Set Header compression.
 * \param MAL_PARAMS_DCMP_ID - Set Data compression.
 * \param MAL_PARAMS_QOS_REQ_ID - Request QoS.
 * \param MAL_PARAMS_QOS_MIN_ID - Set minimum QoS.
 * \param MAL_PARAMS_QOS_REL5_ID - Set Rel-5 QoS.
 * \param MAL_PARAMS_QOS_NEG_ID -  Get Negotiated QoS.
 * \param MAL_PARAMS_TFT_INFO_ID - Set Traffic Flow Template
 * \param MAL_PARAMS_PIPEFD_ID - Get Phonet Pipe FD
 */
typedef enum {
    MAL_PARAMS_APN_ID,
    MAL_PARAMS_AUTH_METHOD_ID,
    MAL_PARAMS_USERNAME_ID,
    MAL_PARAMS_PASSWORD_ID,
    MAL_PARAMS_CHAP_IDENTIFIER_ID,
    MAL_PARAMS_CHALLENGE_DATA_ID,
    MAL_PARAMS_RESPONSE_DATA_ID,
    MAL_PARAMS_PDP_TYPE_ID,
    MAL_PARAMS_PDP_CONTEXT_TYPE_ID,
    MAL_PARAMS_PRIMARY_CID_ID,
    MAL_PARAMS_IP_MAP_ID,
    MAL_PARAMS_IP_ID,
    MAL_PARAMS_IPV6_ID,
    MAL_PARAMS_PDNS_ID,
    MAL_PARAMS_IPV6_PDNS_ID,
    MAL_PARAMS_SDNS_ID,
    MAL_PARAMS_IPV6_SDNS_ID,
    MAL_PARAMS_IF_NAME_ID,
    MAL_PARAMS_HCMP_ID,
    MAL_PARAMS_DCMP_ID,
    MAL_PARAMS_QOS_REQ_ID,
    MAL_PARAMS_QOS_MIN_ID,
    MAL_PARAMS_QOS_REL5_ID,
    MAL_PARAMS_QOS_NEG_ID,
    MAL_PARAMS_TFT_INFO_ID,
    MAL_PARAMS_PIPEFD_ID,
} mal_gpds_params_id_t;

/**
 * \typedef ip_address_t
 * \brief Use for IP addresses
 */
typedef uint8_t ip_address_t[MAL_GPDS_MAX_PDPADDRESS_SIZE];

/**
 * \enum mal_gpds_auth_method_t
 * \brief Supported authentication methods
 * \param MAL_AUTH_METHOD_NONE No authentication
 * \param MAL_AUTH_METHOD_NORMAL Use username & password authentication
 * \param MAL_AUTH_METHOD_SECURE Use for CHAP authentication
 */
typedef enum {
    MAL_AUTH_METHOD_NONE,
    MAL_AUTH_METHOD_NORMAL,
    MAL_AUTH_METHOD_SECURE
} mal_gpds_auth_method_t;

/**
 * \struct mal_gpds_chap_auth_data_t
 * \brief This structure is used to pass CHAP authentication data such
 * as CHAP challenge and response.
 * \param length length of data
 * \param data pointer to actual data.
 */
typedef struct {
    uint8_t length;
    uint8_t *data;
} mal_gpds_chap_auth_data_t;

/**
 * \enum mal_gpds_pdp_type_t
 * \brief Supported PDP types
 * \param MAL_PDP_TYPE_PPP PPP
 * \param MAL_PDP_TYPE_IPV4 IP
 * \param MAL_PDP_TYPE_IPV6 IPv6
 * \param MAL_PDP_TYPE_IPV4V6 IPv4v6
 */
typedef enum {
    MAL_PDP_TYPE_PPP,
    MAL_PDP_TYPE_IPV4,
    MAL_PDP_TYPE_IPV6,
    MAL_PDP_TYPE_IPV4V6
} mal_gpds_pdp_type_t;

/**
 * \enum  mal_gpds_pdp_context_type_t
 * \brief Supported PDP Context types
 * \param MAL_PDP_CONTEXT_TYPE_PRIMARY Primary PDP context
 * \param MAL_PDP_CONTEXT_TYPE_NWI Network initiated PDP context
 * \param MAL_PDP_CONTEXT_TYPE_SEC Secondary PDP context
 */
typedef enum {
    MAL_PDP_CONTEXT_TYPE_PRIMARY,
    MAL_PDP_CONTEXT_TYPE_NWI,
    MAL_PDP_CONTEXT_TYPE_SEC
} mal_gpds_pdp_context_type_t;

/**
 * \enum mal_gpds_pdp_dcmp_t
 * \brief Support for data compression
 * \param MAL_PDP_DCMP_OFF Data compression off
 * \param MAL_PDP_DCMP_ON Data compression On
 * \param MAL_PDP_DCMP_DEFAULT Keep current value
 */
typedef enum {
    MAL_PDP_DCMP_OFF,
    MAL_PDP_DCMP_ON,
    MAL_PDP_DCMP_DEFAULT
} mal_gpds_pdp_dcmp_t;

/**
 * \enum mal_gpds_pdp_hcmp_t
 * \brief Support for header compression
 * \param MAL_PDP_HCMP_OFF Header compression off
 * \param MAL_PDP_HCMP_ON Header compression On
 * \param MAL_PDP_HCMP_DEFAULT Keep current value
 */
typedef enum {
    MAL_PDP_HCMP_OFF,
    MAL_PDP_HCMP_ON,
    MAL_PDP_HCMP_DEFAULT
} mal_gpds_pdp_hcmp_t;

/**
 * \enum mal_gpds_ps_attach_mode_t
 * \brief Enum value for PS attach mode, tells when to attach and detach.
 * \param MAL_GPDS_PS_ATTACH_MANUAL Attach when first PDP is activated or \
 * when explicitly requested using MAL_GPDS_REQ_PS_ATTACH. Also detach when \
 * last PDP is deactivated or when explicitly requested MAL_GPDS_REQ_PS_DETACH.
 * \param MAL_GPDS_PS_ATTACH_AUTOMATIC Attach whenever there is a PS service.
 * \param MAL_GPDS_PS_ATTACH_DEFAULT Use current value, no mode change happens.
 */
typedef enum {
    MAL_GPDS_PS_ATTACH_MANUAL,
    MAL_GPDS_PS_ATTACH_AUTOMATIC,
    MAL_GPDS_PS_ATTACH_DEFAULT
} mal_gpds_ps_attach_mode_t;

/**
 * \enum mal_gpds_nwi_context_act_mode_t
 * \brief Enum values for network initiated PDP context feature control.
 * \param MAL_GPDS_NWI_CONTEXT_REJECT Automatically reject MT context activation requests.
 * \param MAL_GPDS_NWI_CONTEXT_ACCEPT Send an indication MAL_GPDS_EVENT_ID_NWI_PDP_REQUESTED \
 * when MT context activation requested.
 * \param MAL_GPDS_NWI_CONTEXT_DEFAULT Default value
 */
typedef enum {
    MAL_GPDS_NWI_CONTEXT_REJECT,
    MAL_GPDS_NWI_CONTEXT_ACCEPT,
    MAL_GPDS_NWI_CONTEXT_DEFAULT
} mal_gpds_nwi_context_act_mode_t;

/**
 * \enum mal_gpds_aol_context_mode_t
 * \brief Enum value to control Always online PDP context feature.
 * \param MAL_GPDS_AOL_CONTEXT_DISABLE AOL functionality is inactive.
 * \param MAL_GPDS_AOL_CONTEXT_ENABLE AOL is active always.
 * \param MAL_GPDS_AOL_CONTEXT_ENABLE_IN_HPLMN AOL is active while in Home PLMN.
 * \param MAL_GPDS_AOL_CONTEXT_ENABLE_IN_VPLMN AOL is active in visited PLMN.
 */
typedef enum {
    MAL_GPDS_AOL_CONTEXT_DISABLE,
    MAL_GPDS_AOL_CONTEXT_ENABLE,
    MAL_GPDS_AOL_CONTEXT_ENABLE_IN_HPLMN,
    MAL_GPDS_AOL_CONTEXT_ENABLE_IN_VPLMN,
    MAL_GPDS_AOL_CONTEXT_DEFAULT
} mal_gpds_aol_context_mode_t;

/**
 * \enum mal_gpds_icmp_drop_mode_t
 * \brief Enum value for MT ICMP drop mode, tells when to disable and enable MT ICMP packet dropping.
 * \param MAL_GPDS_ICMP_DROP_MODE_DISABLED MT ICMP packet dropping is disabled
 * \param MAL_GPDS_ICMP_DROP_MODE_ENABLED MT ICMP packet dropping is enabled
 * \param MAL_GPDS_ICMP_DROP_MODE_KEEP_EXISTING Use current value, no mode change happens.
 */
typedef enum {
    MAL_GPDS_ICMP_DROP_MODE_DISABLED = 0x01,
    MAL_GPDS_ICMP_DROP_MODE_ENABLED = 0x02,
    MAL_GPDS_ICMP_DROP_MODE_KEEP_EXISTING = 0x00
} mal_gpds_icmp_drop_mode_t;

/*-----------------------*/
/* QoS Settings          */
/*-----------------------*/

/**
 * \enum mal_gpds_qos_precedence_class_t
 * \brief Enum describes Precedence class.
 * \brief See 3GPP TS 24.008 specs for details.
 * \param MAL_GPDS_QOS_PRECEDENCE_CLASS_0 Subscribed Value
 * \param MAL_GPDS_QOS_PRECEDENCE_CLASS_1 High priority
 * \param MAL_GPDS_QOS_PRECEDENCE_CLASS_2 Normal priority
 * \param MAL_GPDS_QOS_PRECEDENCE_CLASS_3 Low priority
 * \param MAL_GPDS_QOS_PRECEDENCE_CLASS_DEFAULT Default
 */
typedef enum {
    MAL_GPDS_QOS_PRECEDENCE_CLASS_0 = 0x00,
    MAL_GPDS_QOS_PRECEDENCE_CLASS_1 = 0x01,
    MAL_GPDS_QOS_PRECEDENCE_CLASS_2 = 0x02,
    MAL_GPDS_QOS_PRECEDENCE_CLASS_3 = 0x03,
    MAL_GPDS_QOS_PRECEDENCE_CLASS_DEFAULT = 0xFF
} mal_gpds_qos_precedence_class_t;

/**
 * \enum mal_gpds_qos_delay_class_t
 * \brief Enum describes Delay class.
 * \brief See 3GPP TS 24.008 specs for details.
 * \param MAL_GPDS_QOS_DELAY_CLASS_0 Subscribed delay class
 * \param MAL_GPDS_QOS_DELAY_CLASS_1 Delay class 1
 * \param MAL_GPDS_QOS_DELAY_CLASS_2 Delay class 2
 * \param MAL_GPDS_QOS_DELAY_CLASS_3 Delay class 3
 * \param MAL_GPDS_QOS_DELAY_CLASS_4 Delay class 4
 * \param MAL_GPDS_QOS_DELAY_CLASS_DEFAULT Current value
 */
typedef enum {
    MAL_GPDS_QOS_DELAY_CLASS_0 = 0x00,
    MAL_GPDS_QOS_DELAY_CLASS_1 = 0x01,
    MAL_GPDS_QOS_DELAY_CLASS_2 = 0x02,
    MAL_GPDS_QOS_DELAY_CLASS_3 = 0x03,
    MAL_GPDS_QOS_DELAY_CLASS_4 = 0x04,
    MAL_GPDS_QOS_DELAY_CLASS_DEFAULT = 0xFF
} mal_gpds_qos_delay_class_t;

/**
 * \enum mal_gpds_qos_reliability_class_t
 * \brief Enum describes Reliability class.
 * \brief See 3GPP TS 24.008 specs for details.
 * \param MAL_GPDS_QOS_RELIABILITY_CLASS_0 Subscribed value
 * \param MAL_GPDS_QOS_RELIABILITY_CLASS_1 Unused, same as class-2
 * \param MAL_GPDS_QOS_RELIABILITY_CLASS_2 Unacknowledged GTP; Acknowledged LLC and RLC, Protected data
 * \param MAL_GPDS_QOS_RELIABILITY_CLASS_3 Unacknowledged GTP and LLC; Acknowledged RLC, Protected data
 * \param MAL_GPDS_QOS_RELIABILITY_CLASS_4 Unacknowledged GTP, LLC, and RLC, Protected data
 * \param MAL_GPDS_QOS_RELIABILITY_CLASS_5 Unacknowledged GTP, LLC, and RLC, Unprotected data
 * \param MAL_GPDS_QOS_RELIABILITY_CLASS_DEFAULT

 */
typedef enum {
    MAL_GPDS_QOS_RELIABILITY_CLASS_0 = 0x00,
    MAL_GPDS_QOS_RELIABILITY_CLASS_1 = 0x01,
    MAL_GPDS_QOS_RELIABILITY_CLASS_2 = 0x02,
    MAL_GPDS_QOS_RELIABILITY_CLASS_3 = 0x03,
    MAL_GPDS_QOS_RELIABILITY_CLASS_4 = 0x04,
    MAL_GPDS_QOS_RELIABILITY_CLASS_5 = 0x05,
    MAL_GPDS_QOS_RELIABILITY_CLASS_DEFAULT = 0xFF
} mal_gpds_qos_reliability_class_t;

/**
 * \enum mal_gpds_qos_peak_throughput_class_t
 * \brief Enum describes Peak Throughput class.
 * \brief See 3GPP TS 24.008 specs for details.
 * \param MAL_GPDS_QOS_PEAK_THROUGHPUT_CLASS_SUBSCRIBED Subscribed Value
 * \param MAL_GPDS_QOS_PEAK_THROUGHPUT_CLASS_8000 8 kbps
 * \param MAL_GPDS_QOS_PEAK_THROUGHPUT_CLASS_16000 16000 bps
 * \param MAL_GPDS_QOS_PEAK_THROUGHPUT_CLASS_32000 32 kbps
 * \param MAL_GPDS_QOS_PEAK_THROUGHPUT_CLASS_64000 64 kbps
 * \param MAL_GPDS_QOS_PEAK_THROUGHPUT_CLASS_128000 128 kbps
 * \param MAL_GPDS_QOS_PEAK_THROUGHPUT_CLASS_256000 256 kbps
 * \param MAL_GPDS_QOS_PEAK_THROUGHPUT_CLASS_512000 512 kbps
 * \param MAL_GPDS_QOS_PEAK_THROUGHPUT_CLASS_1024000 1 Mbps
 * \param MAL_GPDS_QOS_PEAK_THROUGHPUT_CLASS_2048000 2 Mbps
 * \param MAL_GPDS_QOS_PEAK_THROUGHPUT_CLASS_DEFAULT Current default
 */
typedef enum {
    MAL_GPDS_QOS_PEAK_THROUGHPUT_CLASS_SUBSCRIBED = 0x00,
    MAL_GPDS_QOS_PEAK_THROUGHPUT_CLASS_8000 = 0x01,
    MAL_GPDS_QOS_PEAK_THROUGHPUT_CLASS_16000 = 0x02,
    MAL_GPDS_QOS_PEAK_THROUGHPUT_CLASS_32000 = 0x03,
    MAL_GPDS_QOS_PEAK_THROUGHPUT_CLASS_64000 = 0x04,
    MAL_GPDS_QOS_PEAK_THROUGHPUT_CLASS_128000 = 0x05,
    MAL_GPDS_QOS_PEAK_THROUGHPUT_CLASS_256000 = 0x06,
    MAL_GPDS_QOS_PEAK_THROUGHPUT_CLASS_512000 = 0x07,
    MAL_GPDS_QOS_PEAK_THROUGHPUT_CLASS_1024000 = 0x08,
    MAL_GPDS_QOS_PEAK_THROUGHPUT_CLASS_2048000 = 0x09,
    MAL_GPDS_QOS_PEAK_THROUGHPUT_CLASS_DEFAULT = 0xFF
} mal_gpds_qos_peak_throughput_class_t;

/**
 * \enum mal_gpds_qos_mean_throughput_class_t
 * \brief Enum deascribes Mean Throughput class.
 * \brief See 3GPP TS 24.008 specs for details.
 * \param MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_SUBSCRIBED  Subscribed Value
 * \param MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_100 0.22 bps
 * \param MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_200 0.44 bps
 * \param MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_500 1.11 bps
 * \param MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_1000 2.22 bps
 * \param MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_2000 4.44 bps
 * \param MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_5000 11.1 bps
 * \param MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_10000 22 bps
 * \param MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_20000 44 bps
 * \param MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_50000 111 bps
 * \param MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_100000 220 bps
 * \param MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_200000 440 bps
 * \param MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_500000 1110 bps
 * \param MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_1000000 2200 bps
 * \param MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_2000000 4400 bps
 * \param MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_5000000 11100 bps
 * \param MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_10000000 22000 bps
 * \param MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_20000000 44000 bps
 * \param MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_50000000 111000 bps
 * \param MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_BEST_EFFORT  Best Effort
 * \param MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_DEFAULT Current Default
 */
typedef enum {
    MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_SUBSCRIBED = 0x00,
    MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_100 = 0x01,
    MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_200 = 0x02,
    MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_500 = 0x03,
    MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_1000 = 0x04,
    MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_2000 = 0x05,
    MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_5000 = 0x06,
    MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_10000 = 0x07,
    MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_20000 = 0x08,
    MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_50000 = 0x09,
    MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_100000 = 0x0A,
    MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_200000 = 0x0B,
    MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_500000 = 0x0C,
    MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_1000000 = 0x0D,
    MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_2000000 = 0x0E,
    MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_5000000 = 0x0F,
    MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_10000000 = 0x10,
    MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_20000000 = 0x11,
    MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_50000000 = 0x12,
    MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_BEST_EFFORT = 0x1F,
    MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_DEFAULT = 0xFF
} mal_gpds_qos_mean_throughput_class_t;

/**
 * \enum mal_gpds_qos_traffic_class_t
 * \brief Enum describes Type of application for which the UMTS bearer service is optimized.
 * \brief See 3GPP TS 24.008 specs for details.
 * \param MAL_GPDS_QOS_TRAFFICCLASS_SUBSCRIBED Subscribed value
 * \param MAL_GPDS_QOS_TRAFFICCLASS_CONVERSATIONAL Conversational
 * \param MAL_GPDS_QOS_TRAFFICCLASS_STREAMING Streaming
 * \param MAL_GPDS_QOS_TRAFFICCLASS_INTERACTIVE Interactive
 * \param MAL_GPDS_QOS_TRAFFICCLASS_BACKGROUND Background
 * \param MAL_GPDS_QOS_TRAFFICCLASS_UNKNOWN Unknown/default
 */
typedef enum {
    MAL_GPDS_QOS_TRAFFICCLASS_SUBSCRIBED = 0x00,
    MAL_GPDS_QOS_TRAFFICCLASS_CONVERSATIONAL = 0x01,
    MAL_GPDS_QOS_TRAFFICCLASS_STREAMING = 0x02,
    MAL_GPDS_QOS_TRAFFICCLASS_INTERACTIVE = 0x03,
    MAL_GPDS_QOS_TRAFFICCLASS_BACKGROUND = 0x04,
    MAL_GPDS_QOS_TRAFFICCLASS_DEFAULT = 0xFF
} mal_gpds_qos_traffic_class_t;

/**
 * \enum mal_gpds_qos_delivery_order_t
 * \brief Enum describes SDU delivery options.
 * \brief See 3GPP TS 24.008 specs for details.
 * \param MAL_GPDS_QOS_DELIVERY_ORDER_SUBSCRIBED Subscribed Value
 * \param MAL_GPDS_QOS_DELIVERY_ORDER_YES Enable SDU delivery
 * \param MAL_GPDS_QOS_DELIVERY_ORDER_NO Disable SDU delivery
 * \param MAL_GPDS_QOS_DELIVERY_ORDER_DEFAULT Default
 */
typedef enum {
    MAL_GPDS_QOS_DELIVERY_ORDER_SUBSCRIBED = 0x00,
    MAL_GPDS_QOS_DELIVERY_ORDER_YES = 0x01,
    MAL_GPDS_QOS_DELIVERY_ORDER_NO = 0x02,
    MAL_GPDS_QOS_DELIVERY_ORDER_DEFAULT = 0xFF
} mal_gpds_qos_delivery_order_t;

/**
 * \enum mal_gpds_qos_residual_ber_t
 * \brief Enum describes Residual bit error ratio (represented as mantissa-exponent).
 * \brief See 3GPP TS 24.008 specs for details.
 * \param MAL_GPDS_QOS_RES_BER_SUBSCRIBED Subscribed value
 * \param MAL_GPDS_QOS_RES_BER_1 5*10E-2
 * \param MAL_GPDS_QOS_RES_BER_2 1*10E-2
 * \param MAL_GPDS_QOS_RES_BER_3 5*10E-3
 * \param MAL_GPDS_QOS_RES_BER_4 4*10E-3
 * \param MAL_GPDS_QOS_RES_BER_5 1*10E-3
 * \param MAL_GPDS_QOS_RES_BER_6 1*10E-4
 * \param MAL_GPDS_QOS_RES_BER_7 1*10E-5
 * \param MAL_GPDS_QOS_RES_BER_8 1*10E-6
 * \param MAL_GPDS_QOS_RES_BER_9 6*10E-8
 * \param MAL_GPDS_QOS_RES_BER_DEFAULT Current value
 */
typedef enum {
    MAL_GPDS_QOS_RES_BER_SUBSCRIBED = 0x00,
    MAL_GPDS_QOS_RES_BER_1 = 0x52,
    MAL_GPDS_QOS_RES_BER_2 = 0x12,
    MAL_GPDS_QOS_RES_BER_3 = 0x53,
    MAL_GPDS_QOS_RES_BER_4 = 0x43,
    MAL_GPDS_QOS_RES_BER_5 = 0x13,
    MAL_GPDS_QOS_RES_BER_6 = 0x14,
    MAL_GPDS_QOS_RES_BER_7 = 0x15,
    MAL_GPDS_QOS_RES_BER_8 = 0x16,
    MAL_GPDS_QOS_RES_BER_9 = 0x68,
    MAL_GPDS_QOS_RES_BER_DEFAULT = 0xFF
} mal_gpds_qos_residual_ber_t;

/**
 * \enum mal_gpds_qos_sdu_error_ratio_t
 * \brief Enum describes SDU error ratio (represented as mantissa-exponent).
 * \brief See 3GPP TS 24.008 specs for details.
 * \param MAL_GPDS_QOS_RES_BER_SUBSCRIBED Subscribed value
 * \param MAL_GPDS_QOS_SDU_ERROR_RATIO_1 1*10E-2
 * \param MAL_GPDS_QOS_SDU_ERROR_RATIO_2 7*10E-3
 * \param MAL_GPDS_QOS_SDU_ERROR_RATIO_3 1*10E-3
 * \param MAL_GPDS_QOS_SDU_ERROR_RATIO_4 1*10E-4
 * \param MAL_GPDS_QOS_SDU_ERROR_RATIO_5 1*10E-5
 * \param MAL_GPDS_QOS_SDU_ERROR_RATIO_6 1*10E-6
 * \param MAL_GPDS_QOS_SDU_ERROR_RATIO_7 1*10E-1
 * \param MAL_GPDS_QOS_SDU_ERROR_RATIO_DEFAULT Current value
 */
typedef enum {
    MAL_GPDS_QOS_SDU_ERROR_RATIO_SUBSCRIBED = 0x00,
    MAL_GPDS_QOS_SDU_ERROR_RATIO_1 = 0x12,
    MAL_GPDS_QOS_SDU_ERROR_RATIO_2 = 0x73,
    MAL_GPDS_QOS_SDU_ERROR_RATIO_3 = 0x13,
    MAL_GPDS_QOS_SDU_ERROR_RATIO_4 = 0x14,
    MAL_GPDS_QOS_SDU_ERROR_RATIO_5 = 0x15,
    MAL_GPDS_QOS_SDU_ERROR_RATIO_6 = 0x16,
    MAL_GPDS_QOS_SDU_ERROR_RATIO_7 = 0x11,
    MAL_GPDS_QOS_SDU_ERROR_RATIO_DEFAULT = 0xFF
} mal_gpds_qos_sdu_error_ratio_t;

/**
 * \enum mal_gpds_qos_trafic_handling_priority_t
 * \brief Enum describes Traffic handling priority
 * \brief See 3GPP TS 24.008 specs for details.
 * \param MAL_GPDS_QOS_TRAFFIC_HANDLING_PRIO_SUBSCRIBED Subscribed priority
 * \param MAL_GPDS_QOS_TRAFFIC_HANDLING_PRIO_1 Priority 1
 * \param MAL_GPDS_QOS_TRAFFIC_HANDLING_PRIO_2 Priority 2
 * \param MAL_GPDS_QOS_TRAFFIC_HANDLING_PRIO_3 Priority 3
 * \param MAL_GPDS_QOS_TRAFFIC_HANDLING_PRIO_DEFAULT Current Value
 */
typedef enum {
    MAL_GPDS_QOS_TRAFFIC_HANDLING_PRIO_SUBSCRIBED = 0x00,
    MAL_GPDS_QOS_TRAFFIC_HANDLING_PRIO_1 = 0x01,
    MAL_GPDS_QOS_TRAFFIC_HANDLING_PRIO_2 = 0x02,
    MAL_GPDS_QOS_TRAFFIC_HANDLING_PRIO_3 = 0x03,
    MAL_GPDS_QOS_TRAFFIC_HANDLING_PRIO_DEFAULT = 0xFF
} mal_gpds_qos_trafic_handling_priority_t;

/**
 * \enum mal_gpds_qos_delivery_of_error_sdu_t
 * \brief Enum describes Errorneous SDU delivery values.
 * \brief See 3GPP TS 24.008 specs for details.
 * \param MAL_GPDS_QOS_DELIVERY_ERROR_SDU_SUBSCRIBED Subscribed
 * \param MAL_GPDS_QOS_DELIVERY_ERROR_SDU_NO_DETECT Detect
 * \param MAL_GPDS_QOS_DELIVERY_ERROR_SDU_YES Enable
 * \param MAL_GPDS_QOS_DELIVERY_ERROR_SDU_NO  Disable
 * \param MAL_GPDS_QOS_DELIVERY_ERROR_SDU_DEFAUT  Default
 */
typedef enum {
    MAL_GPDS_QOS_DELIVERY_ERROR_SDU_SUBSCRIBED = 0x00,
    MAL_GPDS_QOS_DELIVERY_ERROR_SDU_NO_DETECT = 0x01,
    MAL_GPDS_QOS_DELIVERY_ERROR_SDU_YES = 0x02,
    MAL_GPDS_QOS_DELIVERY_ERROR_SDU_NO = 0x03,
    MAL_GPDS_QOS_DELIVERY_ERROR_SDU_DEFAUT = 0xFF
} mal_gpds_qos_delivery_of_error_sdu_t;

/**
 * \enum mal_gpds_qos_src_stat_descriptor_t
 * \brief Enum describes Rel-5 Source statistics descriptor
 * \brief See 3GPP TS 24.008 specs for details.
 * \param MAL_GPDS_QOS_SRC_STAT_DESC_UNKNOWN Unknown Source
 * \param MAL_GPDS_QOS_SRC_STAT_DESC_SPEECH Speech source
 * \param MAL_GPDS_QOS_SRC_STAT_DESC_DEFAULT
 */
typedef enum {
    MAL_GPDS_QOS_SRC_STAT_DESC_UNKNOWN = 0x00,
    MAL_GPDS_QOS_SRC_STAT_DESC_SPEECH = 0x01,
    MAL_GPDS_QOS_SRC_STAT_DESC_DEFAULT = 0xFF
} mal_gpds_qos_src_stat_descriptor_t;

/**
 * \enum mal_gpds_qos_sgn_indication_t
 * \brief Enum describes Rel-5 Signalling Indication Flag.
 * \brief See 3GPP TS 24.008 specs for details.
 * \param MAL_GPDS_QOS_NOT_OPT_SGN Not Optimized for signaling
 * \param MAL_GPDS_QOS_OPT_SGN Optimized for signalling
 * \param MAL_GPDS_QOS_OPT_DEFAULT
 */
typedef enum {
    MAL_GPDS_QOS_NOT_OPT_SGN = 0x00,
    MAL_GPDS_QOS_OPT_SGN = 0x01,
    MAL_GPDS_QOS_OPT_DEFAULT = 0xFF
} mal_gpds_qos_sgn_indication_t;

/**
 * \enum mal_gpds_qos_type_t
 * \brief QoS Type
 * \param MAL_GPDS_QOS_REL97_98 Rel-97/98 QoS Type
 * \param MAL_GPDS_QOS_REL99 Rel-99 QoS Type
 * \param MAL_GPDS_QOS_REL5 Rel-5 QoS
 */
typedef enum {
    MAL_GPDS_QOS_REL97_98,
    MAL_GPDS_QOS_REL99,
    MAL_GPDS_QOS_REL5
} mal_gpds_qos_type_t;

/**
 * \struct mal_gpds_qos_profile_t
 * \brief This structures stores 3G Quality of Service (QoS) profile.
 * Note that Rel-99 contents override Rel-97/98 contents.
 *
 * \param qos_type Type of QoS to use. Refer \ref mal_gpds_qos_type_t .
 *
 * QoS Rel-97/98 Values
 *
 * \param precedence_class Enum value of Precedence class. Refer \ref mal_gpds_qos_precedence_class_t
 * \param delay_class Enum value of Delay class. Refer \ref mal_gpds_qos_delay_class_t
 * \param reliability_class Enum value of Reliability class. Refer \ref mal_gpds_qos_reliability_class_t
 * \param peak_throughput_class Enum value of Peak throughput class. Refer \ref mal_gpds_qos_peak_throughput_class_t
 * \param mean_throughput_class Enum value of Mean throughput class. Refer \ref mal_gpds_qos_mean_throughput_class_t
 *
 * QoS Rel-99 Values
 *
 * \param traffic_class Enum of type of application. Refer \ref mal_gpds_qos_traffic_class_t
 * \param delivery_order Enum of type of delivery order. Refer \ref mal_gpds_qos_delivery_order_t
 * \param delivery_of_error_sdu Enum of type of delivery of error SDU. Refer \ref mal_gpds_qos_delivery_of_error_sdu_t
 * \param residual_ber Enum of type residual BER. Refer \ref mal_gpds_qos_residual_ber_t
 * \param sdu_error_ration Enum of SDU error ratio. Refer \ref mal_gpds_qos_sdu_error_ratio_t
 * \param trafic_handling_priority Enum of traffic handling priority. Refer \ref mal_gpds_qos_trafic_handling_priority_t
 * \param transfer_delay Transfer Delay in milliseconds. (1 to 4000, 0 is default value)
 * \param maximum_sdu_size The maximum allowed SDU size, in octets (1 to 1520, 0 is default value).
 * \param maximum_bitrate_ul The maximum uplink bitrate in kbps (1 to 8640 kbps, 0 is default value).
 * \param maximum_bitrate_dl The maximum downlink bitrate in kbps (1 to 16000 kbps, 0 is default value).
 * \param guaranteed_bitrate_ul The guranteed uplink bitrate in kbps (1 to 8640 kbps, 0 is default value).
 * \param guaranteed_bitrate_dl The guaranteed downlink bitrate in kbps (1 to 16000 kbps, 0 is default value).
 *
 * QoS Rel-5
 *
 * \param src_stat_desc Source Statistics Descriptor.Refer \ref mal_gpds_qos_src_stat_descriptor_t
 * \param sgn_ind Signaling Indication. Refer \ref mal_gpds_qos_sgn_indication_t
 */
typedef struct {
    mal_gpds_qos_type_t qos_type;
    mal_gpds_qos_precedence_class_t precedence_class;
    mal_gpds_qos_delay_class_t delay_class;
    mal_gpds_qos_reliability_class_t reliability_class;
    mal_gpds_qos_peak_throughput_class_t peak_throughput_class;
    mal_gpds_qos_mean_throughput_class_t mean_throughput_class;
    mal_gpds_qos_traffic_class_t traffic_class;
    mal_gpds_qos_delivery_order_t delivery_order;
    mal_gpds_qos_delivery_of_error_sdu_t delivery_of_error_sdu;
    mal_gpds_qos_residual_ber_t residual_ber;
    mal_gpds_qos_sdu_error_ratio_t sdu_error_ratio;
    mal_gpds_qos_trafic_handling_priority_t trafic_handling_priority;
    uint16_t transfer_delay;
    uint16_t maximum_sdu_size;
    uint16_t maximum_bitrate_ul;
    uint16_t maximum_bitrate_dl;
    uint16_t guaranteed_bitrate_ul;
    uint16_t guaranteed_bitrate_dl;
    mal_gpds_qos_src_stat_descriptor_t src_stat_desc;
    mal_gpds_qos_sgn_indication_t sgn_ind;
} mal_gpds_qos_profile_t;

/*----------------------*/
/* Traffic Flow Template*/
/*----------------------*/
/**
 * \enum mal_gpds_tft_operation_t
 * \brief Enum ddescribes TFT Operation Code
 */
typedef enum {
    MAL_GPDS_TFT_OPERATION_CREATE_NEW = 0x01,
    MAL_GPDS_TFT_OPERATION_DELETE_EXISTING = 0x02,
    MAL_GPDS_TFT_OPERATION_ADD_PACKET_FILTERS = 0x03,
    MAL_GPDS_TFT_OPERATION_REPLACE_PACKET_FILTERS = 0x04,
    MAL_GPDS_TFT_OPERATION_DELETE_PACKET_FILTERS = 0x05,
    MAL_GPDS_TFT_OPERATION_NO_OPERATION = 0x06
} mal_gpds_tft_operation_t;

/**
 * \struct mal_gpds_packet_filter_info_t
 * \brief Defines Packet Filter.
 * There can not be more than one occurrence of each packet filter content.
 * The Packet Filter Identifier and Filter Evaluation Precedence must be unique among different packet
 * filters.
 * Either protocol_info or next_header shall be used, based on pdp type.
 * Either type of service (and mask) or traffic class (and mask) is used based on pdp type.
 * Flow label is used only if pdp type is IPv6.
 * \param packet_filter_id Packet filter identifier.
 * \param filter_eval_prec Filter Evaluation Precedence
 * \param protocol_info_or_next_header Protocol Identifier(IpV4) or Next Header (IPv6).
 * \param type_of_service_or_traffic_class
 * \param type_of_service_or_traffic_class_mask
 * \param src_address IPv4/IPv6 Source Address
 * \param src_subnet_mask Subnet mask
 * \param dst_port_range_low_limit Destination port range -low limit
 * \param dst_port_range_high_limit Destination port range -high limit
 * \param src_port_range_low_limit Source port range -low limit
 * \param src_port_range_high_limit Source port range -high limit
 * \param spi IPSec security parameter index (in big-endien format)
 * \param flow_label IPv6 flow label (in big-endien format)
 */
typedef struct {
    uint8_t packet_filter_id;
    uint8_t filter_eval_prec;
    uint8_t protocol_info_or_next_header;
    uint8_t type_of_service_or_traffic_class;
    uint8_t type_of_service_or_traffic_class_mask;
    ip_address_t src_address;
    ip_address_t src_subnet_mask;
    uint16_t dst_port_range_low_limit;
    uint16_t dst_port_range_high_limit;
    uint16_t src_port_range_low_limit;
    uint16_t src_port_range_high_limit;
    uint32_t spi;
    uint32_t flow_label;
} mal_gpds_packet_filter_info_t;

/**
 * \struct mal_gpds_tft_info_t
 * \brief Defines Traffic-Flow Template.
 * \param tft_operation_code TFT Operation code. Refer \ref mal_gpds_tft_operation_t
 * \param no_of_packet_filters No of packet filters shall be greater than 0 and less than or
 * equal to 8 except in case TFT operation code is set to MAL_GPDS_TFT_OPERATION_DELETE_EXISTING or
 * MAL_GPDS_TFT_OPERATION_NO_OPERATION where it can be 0.
 * \param packet_filter Packet Filter Contents. Refer \ref mal_gpds_packet_filter_info_t
 */
typedef struct {
    mal_gpds_tft_operation_t tft_operation_code;
    uint8_t no_of_packet_filters;
    mal_gpds_packet_filter_info_t packet_filter[MAL_GPDS_MAX_PACKET_FILTERS_SIZE];
} mal_gpds_tft_info_t;

/**
 * \struct mal_gpds_nwi_pdp_params_t
 * \brief This structure stores network initiated pdp parameters.
 * \param pdp_type PDP Connection type. Refer \ref mal_gpds_pdp_type_t.
 * \param pdp_address PDP Address (Not in DNS format, null terminated string).
 * \param apn APN Name is Null terminated string.
 */
typedef struct {
    mal_gpds_pdp_type_t pdp_type;
    ip_address_t pdp_address;
    char apn[MAL_GPDS_MAX_APN_SIZE];
} mal_gpds_nwi_pdp_params_t;

/*Not used*/
/**
 * \struct mal_gpds_pdp_param_t
 * \brief This structure stores activated pdp context parameters.
 * \param cid PDP Context ID.
 * \param ul_max_bitrate Maximum uplink bitrate.
 * \param ul_min_bitrate Minimum uplink bitrate.
 * \param dl_max_bitrate Maximum downlink bitrate.
 * \param dl_min_bitrate Minimum downlink bitrate.
 * \param ip IP address (in DNS format).
 * \param pdns Primary DNS (in DNS format).
 * \param sdns Secondary DNS (in DNS format).
 */
typedef struct {
    uint8_t cid;
    uint16_t ul_max_bitrate;
    uint16_t ul_min_bitrate;
    uint16_t dl_max_bitrate;
    uint16_t dl_min_bitrate;
    ip_address_t pdp_address;
    ip_address_t pdns;
    ip_address_t sdns;
} mal_gpds_pdp_param_t;

/**
 * \struct mal_gpds_config_t
 * \brief This structure stores global properties of PS services. It is optional
 * to set these properties and can be changed at any time using MAL_GPDS_REQ_CONFIG_CHANGE request.
 * \param ps_attach_mode Control Attach mode (Default is Manual attach). Refer \ref mal_gpds_ps_attach_mode_t.
 * \param nwi_act_mode Control MT context activation mode (Default is Reject). Refer \ref mal_gpds_nwi_context_act_mode_t.
 * \param aol_mode Control Always-online PDP feature (Default is inactive). Refer \ref mal_gpds_aol_context_mode_t.
 * \param drop_mode MT ICMP drop mode (Default is disabled). Refer \ref mal_gpds_icmp_drop_mode_t.
 */
typedef struct {
    mal_gpds_ps_attach_mode_t ps_attach_mode;
    mal_gpds_nwi_context_act_mode_t nwi_act_mode;
    mal_gpds_aol_context_mode_t aol_mode;
    mal_gpds_icmp_drop_mode_t drop_icmp_mode;
} mal_gpds_config_t;

/**
 * \struct mal_gpds_data_counter_info_t
 * \brief This structure stores the data counter information.
 * \param tx_byte_count 64 bit Tx data byte counter.
 * \param rx_byte_count 64 bit Rx data byte counter.
 */
typedef struct {
    uint8_t tx_byte_count[8];
    uint8_t rx_byte_count[8];
} mal_gpds_data_counter_info_t;

/**
 * \struct activate_pdpc_request_pdu_info_t
 * \brief This structure stores the resource control information
 * \param data_length length of Activate PDP request.
 * \param data Activate PDP request.
 */
typedef struct {
    uint16_t data_length;
    uint8_t   *data;
} activate_pdpc_request_pdu_info_t;

/**
 * \enum mal_gpds_cc_result_t
 * \brief Enum value to indicate supported GPDS Resource control result.
 * \param MAL_GPDS_RESOURCE_ALLOWED Resource control for GPRS is allowed.
 * \param MAL_GPDS_RESOURCE_MODIFIED Resource control for GPRS is modified.
 * \param MAL_GPDS_RESOURCE_REJECTED Resource control for GPRS is rejected.
 */
typedef enum {
    MAL_GPDS_RESOURCE_ALLOWED = 0x01,
    MAL_GPDS_RESOURCE_MODIFIED = 0x02,
    MAL_GPDS_RESOURCE_REJECTED = 0x03
} mal_gpds_cc_result_t;

/**
 * \enum mal_gpds_configuration_status_t
 * \brief Enum value to indicate supported GPDS Resource configuration status
 * \param MAL_GPDS_RESOURCE_CONF_READY Resource configuration ready at GPDS start up.
 * \param MAL_GPDS_RESOURCE_CONF_RECONFIGURED Resource is reconfigured.
  */
typedef enum {
    MAL_GPDS_RESOURCE_CONF_READY = 0x01,
    MAL_GPDS_RESOURCE_CONF_RECONFIGURED = 0x02
} mal_gpds_configuration_status_t;

/**
 * \struct mal_gpds_resource_control_info_t
 * \brief This structure stores the resource control information
 * \param seq_id Sequence ID.
 * \param resource_control_data PDP activate message.Refer \ref activate_pdpc_request_pdu_info_t.
 */
typedef struct {
    uint8_t   seq_id;
    activate_pdpc_request_pdu_info_t resource_control_data;
} mal_gpds_resource_control_info_t;

/**
 * \struct mal_gpds_resource_control_req_info_t
 * \brief This structure stores the resource control information
 * \param seq_id Sequence ID.
 * \param cc_result Resource control result.Refer \ref mal_gpds_cc_result_t.
 * \param resource_control_data PDP activate message.Refer \ref activate_pdpc_request_pdu_info_t.
 */
typedef struct {
    uint8_t   seq_id;
    mal_gpds_cc_result_t cc_result;
    activate_pdpc_request_pdu_info_t resource_control_data;
} mal_gpds_resource_control_req_info_t;


/*--------------------------*/
/* Error codes              */
/*--------------------------*/
typedef enum {
    MAL_GPDS_CAUSE_UNKNOWN                       = 0x00,
    MAL_GPDS_CAUSE_IMSI                          = 0x02,
    MAL_GPDS_CAUSE_MS_ILLEGAL                    = 0x03,
    MAL_GPDS_CAUSE_ME_ILLEGAL                    = 0x06,
    MAL_GPDS_CAUSE_GPRS_NOT_ALLOWED              = 0x07,
    MAL_GPDS_NOT_ALLOWED                         = 0x08,
    MAL_GPDS_CAUSE_MS_IDENTITY                   = 0x09,
    MAL_GPDS_CAUSE_DETACH                        = 0x0A,
    MAL_GPDS_PLMN_NOT_ALLOWED                    = 0x0B,
    MAL_GPDS_LA_NOT_ALLOWED                      = 0x0C,
    MAL_GPDS_ROAMING_NOT_ALLOWED                 = 0x0D,
    MAL_GPDS_CAUSE_GPRS_NOT_ALLOWED_IN_PLMN      = 0x0E,
    MAL_GPDS_CAUSE_NO_SUITABLE_CELLS_IN_LA       = 0x0F,
    MAL_GPDS_CAUSE_MSC_NOT_REACH                 = 0x10,
    MAL_GPDS_CAUSE_PLMN_FAIL                     = 0x11,
    MAL_GPDS_CAUSE_NETWORK_CONGESTION            = 0x16,
    MAL_GPDS_CAUSE_LLC_SNDCP_FAILURE             = 0x19,
    MAL_GPDS_CAUSE_RESOURCE_INSUFF               = 0x1A,
    MAL_GPDS_CAUSE_APN                           = 0x1B,
    MAL_GPDS_CAUSE_PDP_UNKNOWN                   = 0x1C,
    MAL_GPDS_CAUSE_AUTHENTICATION                = 0x1D,
    MAL_GPDS_CAUSE_ACT_REJECT_GGSN               = 0x1E,
    MAL_GPDS_CAUSE_ACT_REJECT                    = 0x1F,
    MAL_GPDS_CAUSE_SERV_OPT_NOT_SUPPORTED        = 0x20,
    MAL_GPDS_CAUSE_SERV_OPT_NOT_SUBSCRIBED       = 0x21,
    MAL_GPDS_CAUSE_SERV_OPT_OUT_OF_ORDER         = 0x22,
    MAL_GPDS_CAUSE_NSAPI_ALREADY_USED            = 0x23,
    MAL_GPDS_CAUSE_DEACT_REGULAR                 = 0x24,
    MAL_GPDS_CAUSE_QOS                           = 0x25,
    MAL_GPDS_CAUSE_NETWORK_FAIL                  = 0x26,
    MAL_GPDS_CAUSE_REACTIVATION_REQ              = 0x27,
    MAL_GPDS_CAUSE_FEAT_NOT_SUPPORTED            = 0x28,
    MAL_GPDS_CAUSE_TFT_SEMANTIC_ERROR            = 0x29,
    MAL_GPDS_CAUSE_TFT_SYNTAX_ERROR              = 0x2A,
    MAL_GPDS_CAUSE_CONTEXT_UNKNOWN               = 0x2B,
    MAL_GPDS_CAUSE_FILTER_SEMANTIC_ERROR         = 0x2C,
    MAL_GPDS_CAUSE_FILTER_SYNTAX_ERROR           = 0x2D,
    MAL_GPDS_CAUSE_CONT_WITHOUT_TFT              = 0x2E,
    MAL_GPDS_CAUSE_MULTICAST_MEMBERSHIP_TIMEOUT  = 0x2F,
    MAL_GPDS_CAUSE_INVALID_MANDATORY_INFO        = 0x60,
    MAL_GPDS_CAUSE_MSG_TYPE_NON_EXISTENTOR_NOT_IMPLTD = 0x61,
    MAL_GPDS_CAUSE_MSG_TYPE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE = 0x62,
    MAL_GPDS_CAUSE_IE_NON_EXISTENT_OR_NOT_IMPLEMENTED = 0x63,
    MAL_GPDS_CAUSE_CONDITIONAL_IE_ERROR          = 0x64,
    MAL_GPDS_CAUSE_MSG_NOT_COMPATIBLE_WITH_PROTOCOL_STATE = 0x65,
    MAL_GPDS_CAUSE_UNSPECIFIED                   = 0x6F,
    MAL_GPDS_CAUSE_APN_INCOMPATIBLE_WITH_CURR_CTXT = 0x70,
    MAL_GPDS_CAUSE_FDN                           = 0xA0,
    MAL_GPDS_CAUSE_USER_ABORT                    = 0xA1,
    MAL_GPDS_CAUSE_CS_INACTIVE                   = 0xA2,
    MAL_GPDS_CAUSE_CSD_OVERRIDE                  = 0xA3,
    MAL_GPDS_CAUSE_APN_CONTROL                   = 0xA4,
    MAL_GPDS_CAUSE_CALL_CONTROL                  = 0xA5,
    MAL_GPDS_CAUSE_RETRY_COUNTER_EXPIRED         = 0xC8,
    MAL_GPDS_CAUSE_NO_CONNECTION                 = 0xC9,
    MAL_GPDS_CAUSE_DETACHED                      = 0xF5,
    MAL_GPDS_CAUSE_NO_SERVICE_POWER_SAVE         = 0xF7,
    MAL_GPDS_CAUSE_SIM_REMOVED                   = 0xF9,
    MAL_GPDS_CAUSE_POWER_OFF                     = 0xFA,
    MAL_GPDS_CAUSE_LAI_FORBIDDEN_NATIONAL_ROAM_LIST = 0xFB,
    MAL_GPDS_CAUSE_LAI_FORBIDDEN_REG_PROVISION_LIST = 0xFC,
    MAL_GPDS_CAUSE_ACCESS_BARRED                 = 0xFD,
    MAL_GPDS_CAUSE_FATAL_FAILURE                 = 0xFE,
    MAL_GPDS_CAUSE_AUT_FAILURE                   = 0xFF
} mal_gpds_error_type_t;

/**
 * \enum mal_gpds_phonet_conf_t
 * \brief Phonet lib Configuration
 * \param MAL_GPDS_CONFIG_PHONET_NORMAL Phonet stack to use TCP/IP Stack
 *        for Send/Receive
 * \param MAL_GPDS_CONFIG_PHONET_TEST   Phonet stack to by pass TCP/IP Stack
 *        for Send/Receive
 * */
typedef enum {
    MAL_GPDS_CONFIG_PHONET_NORMAL,
    MAL_GPDS_CONFIG_PHONET_TEST
} mal_gpds_phonet_conf_t;
/*---------------------------------------*/
/* Enum/Structure for event notification */
/*---------------------------------------*/

/**
 * \enum mal_gpds_net_detach_type_t
 * \brief Network originated detach type
 * \param MAL_GPDS_DETACH_TYPE_MS MS originated detach type
 * \param MAL_GPDS_DETACH_TYPE_MT_REATTACH_REQ Network originated detach with reattach required
 * \param MAL_GPDS_DETACH_TYPE_MT Network originated detach without reattach required
 */
typedef enum {
    MAL_GPDS_DETACH_TYPE_MS = 0x0,
    MAL_GPDS_DETACH_TYPE_MT_REATTACH_REQ,
    MAL_GPDS_DETACH_TYPE_MT
} mal_gpds_net_detach_type_t;

/**
 * \struct mal_gpds_detach_info_t
 * \brief This structure informs about the detach cause and the type of network detach
 * \param detach_cause  Detach cause \ref mal_gpds_error_type_t
 * \param detach_type  Network originated detach type \ref mal_gpds_net_detach_type_t
 */
typedef struct {
    mal_gpds_error_type_t detach_cause;
    mal_gpds_net_detach_type_t detach_type;
} mal_gpds_detach_info_t;

/**
 * \enum mal_gpds_ps_attach_status_t
 * \brief Enum value to indicate supported GPDS attach status types
 * \param MAL_GPDS_PS_STATUS_ATTACHED
 * \param MAL_GPDS_PS_STATUS_DETACHED
 */
typedef enum {
    MAL_GPDS_PS_STATUS_ATTACHED,
    MAL_GPDS_PS_STATUS_DETACHED
} mal_gpds_ps_attach_status_t;


/*-------------------------*/
/* GPDS MAL API definitions*/
/*-------------------------*/

/**
 * \fn void (*mal_gpds_event_cb_t)(uint8_t conn_id, mal_gpds_event_ids_t event_id, void *data)
 * \brief callback function for any expected or unexpected event information from modem.
 * \param [out] conn_id connection id to which this event is associated.
 * \param [out] event_id id of unsolicited event: /ref mal_gpds_event_ids_t
 * \param [out] data pointer to parameter values associated with this event (only fail cause at the moment).
 * \return None *
 */
typedef void (*mal_gpds_event_cb_t)(uint8_t conn_id, mal_gpds_event_ids_t event_id, void *data);

/**
 * \fn int mal_gpds_init(int *fd_gpds, int *fd_netlnk, int *fd_pipe)
 * \brief Opens socket using phonet lib. Also open netlink socket using netlink library
 * \brief Should be called only once at the time of initialization.
 * \brief Initializes pipe library also.
 * \param [in] fd_gpds points to file descriptor of gpds socket.
 * \param [in] fd_netlnk points to file descriptor of netlink socket.
 * \param [in] fd_pipe points to the file descriptor of pipe socket.
 * \return positive if successful.
 * \return negative on failure.
 */
int mal_gpds_init(int *fd_gpds, int *fd_netlnk, int *fd_pipe);

/**
 *  \fn int mal_gpds_deinit(void)
 *  \brief deinit gpds and pipe mal.
 *  \return None
 */
void mal_gpds_deinit(void);

/**
 * \fn int mal_gpds_config(void)
 * \brief Does nothing.(To be used for configuring GPDS property)
 * \return positive always.
 */
int mal_gpds_config(void);

/**
 * \fn int mal_gpds_request(uint8_t conn_id, mal_gpds_req_ids_t req_id, void* data)
 * \brief gpds mal request handler function.
 * \param [in] conn_id connection id to which request is associated.
 * \param [in] req_id request id: \ref mal_gpds_req_ids_t
 * \param [in] data pointer to parameter value
 * \return positive on success.
 * \return negative on failure.
 */
int mal_gpds_request(uint8_t conn_id, mal_gpds_req_ids_t req_id, void *data);

/**
 * \fn void mal_gpds_register_callback(mal_gpds_event_cb_t event_cb)
 * \brief callback handler
 * \param [in] event_cb callback handler: \ref mal_gpds_event_cb_t
 * \return None.
 */
void mal_gpds_register_callback(mal_gpds_event_cb_t event_cb);

/**
 * \fn int mal_gpds_set_param(uint8_t conn_id, mal_gpds_params_id_t param_id, void* data)
 * \brief set parameters associated with a perticular connectio (pdp context).
 * \param [in] conn_id connection identifier
 * \param [in] param_id parameter id: \ref mal_gpds_params_id_t
 * \param [in] data pointer to parameter value.
 * \return positive on success.
 * \return negative on failure.
 */
int mal_gpds_set_param(uint8_t conn_id, mal_gpds_params_id_t param_id, void *data);

/**
 * \fn int mal_gpds_get_param(uint8_t conn_id, mal_gpds_params_id_t param_id, void* data);
 * \brief get parameters associated with a perticular connectio (pdp context).
 * \param [in] conn_id connection identifier
 * \param [in] param_id parameter id: \ref mal_gpds_params_id_t
 * \param [in] data pointer to parameter value.
 * \return positive on success.
 * \return negative on failure.
 */
int mal_gpds_get_param(uint8_t conn_id, mal_gpds_params_id_t param_id, void *data);

/**
 * \fn void mal_gpds_response_handler(int fd)
 * \brief This function is called by application to inform that some message is
 * \brief available from modem for processing.
 * \param [in] fd socket file desriptor which is available for reading.
 * \return None.
 */
void mal_gpds_response_handler(int fd);

/**
 *  \fn  int32_t mal_gpds_request_set_debug_level(uint8_t level);
 *  \brief  This Api is used to set the debug level for gpds module. This is a synchronous call.
 *  \param [in] level  0 - No Prints; 1 - Entry & Exit Prints; 2 - Value/Info Prints; 3 - ISI MSG Prints
 *                               4 - All Prints
 * \return positive on success.
 * \return negative on failure.
 */
int32_t mal_gpds_request_set_debug_level(uint8_t level);

/**
 *  \fn  int32_t mal_gpds_request_get_debug_level(uint8_t submodule, uint8_t *level);
  *  \brief  This Api is used to get the debug level for gpds module. This is a synchronous call.
 *   \param[out] debug level for gpds module. data type: pointer to uint8_t.
 *                       Caller needs to take care of memory allocation
 * \return positive on success.
 * \return negative on failure.
 */
int32_t mal_gpds_request_get_debug_level(uint8_t *level);

#endif //__MAL_GPDS_H

