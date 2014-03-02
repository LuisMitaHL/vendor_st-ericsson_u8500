/**
 * \file  mal_uicc_apdu.h
 * \brief This file includes headers for UICC MAL API(Connector Interface and APDU Interface).
 *
 * \n Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * \n This code is ST-Ericsson proprietary and confidential.
 * \n Any use of the code for whatever purpose is subject to
 * \n specific written permission of ST-Ericsson SA.
 *
 * \par
 * \n MAL API header file for UICC(Connector Interface and APDU Interface) Library.
 * \n \author ST-Ericsson
 * \n
 * \n Version : 0.0.1
 * \n
 * \n Revision History:
 * \par
 * \n v0.0.2               Corrects the MAL_UICC_ERROR MACRO.
 * \n v0.0.1               Added UICC Connector and SAP APDU interfaces related data structures
 * \n                      Aligned and latest tested on:
 * \n                      MODEM SW version : DV 8.1.2 , UICC ISI SPEC version : 004.000
 */


/**
 * \defgroup  MAL_UICC_APDU UICC API
 * \par
 * \n This part describes the interface to UICC MAL(Connector Interface and APDU Interface) client.
 */
#ifndef UICC_MAL_APDU_H
#define UICC_MAL_APDU_H

#include "mal_uicc.h"

/**
 * \name Message ID's
 * \par
 * This part describes the different message id's or event id's supported
 * as part of UICC MAL APDU SAP client library.
 */

/**
* \def   MAL_UICC_CONNECTOR_REQ
* \brief Request UICC server for CONNECTOR requests
* \n     Events and the response data outcome for this request.
* \n     Refer /ref MAL_UICC_CONNECTOR_RESP
* \param [in] - "uicc_connector_req_t" \ref uicc_connector_req_t
*/
#define MAL_UICC_CONNECTOR_REQ                       0x0F

/**
 * \def   MAL_UICC_CONNECTOR_RESP
 * \brief Response for MAL_UICC_CONNECTOR_REQ
 * \param [out] - "uicc_connector_resp_t" \ref uicc_connector_resp_t
 */
#define MAL_UICC_CONNECTOR_RESP                      0x10

/**
 * \def   MAL_UICC_APDU_REQ
 * \brief Request UICC server for APDU SEND/ATR/CONTROL
 * \n     Events and the response data outcome for this request.
 * \n     Refer /ref MAL_UICC_APDU_RESP
 * \param [in] - "uicc_apdu_req_t" \ref uicc_apdu_req_t
 */
#define MAL_UICC_APDU_REQ                            0x15

/**
 * \def   MAL_UICC_APDU_RESP
 * \brief Response for MAL_UICC_APDU_REQ
 * \param [out] - "uicc_apdu_resp_t" \ref uicc_apdu_resp_t
 */
#define MAL_UICC_APDU_RESP                           0x16

/**
 * \def   MAL_UICC_APDU_RESET_IND
 * \brief APDU reset indications
 * \param [out] - "uicc_apdu_reset_ind_t" \ref uicc_apdu_reset_ind_t
 */
#define MAL_UICC_APDU_RESET_IND                      0x17


/**
 * \name Service Type ID's - UICC_SERVICE_TYPE_CONSTANTS
 * \par
 * This part describes the different service types supported
 * as part of UICC MAL APDU SAP client library. Also termed as UICC_SERVICE_TYPE_CONSTANTS.
 */
/*--------------------------------------------------------------------------------*/

/*FOR UICC CONNECTOR INTERFACE*/
/**
 * \def   MAL_UICC_CONNECT
 * \brief Service type indicating card is connected to UICC server.
 * \n     Supported in request of \ref MAL_UICC_CONNECTOR_REQ
 */
#define MAL_UICC_CONNECT                             0x31

/**
 * \def   MAL_UICC_DISCONNECT
 * \brief Service type indicating card is disconnected from UICC server.
 * \n     Supported in request of \ref MAL_UICC_CONNECTOR_REQ
 */
#define MAL_UICC_DISCONNECT                          0x32

/**
 * \def   MAL_UICC_RECONNECT
 * \brief Service type indicating card is reconnected to UICC server.
 * \n     Supported in request of \ref MAL_UICC_CONNECTOR_REQ
 */
#define MAL_UICC_RECONNECT                           0x33

/*FOR APDU INTERFACE*/
/**
 * \def   MAL_UICC_APDU_SEND
 * \brief Service type indicating APDU send through SAP
 * \n     Supported in request of \ref MAL_UICC_APDU_REQ
 */
#define MAL_UICC_APDU_SEND                           0x51

/**
 * \def   MAL_UICC_ATR_GET
 * \brief Service type indicating ATR get send through SAP
 * \n     Supported in request of \ref MAL_UICC_APDU_REQ
 */
#define MAL_UICC_ATR_GET                             0x52

/**
 * \def   MAL_UICC_APDU_CONTROL
 * \brief Service type indicating APDU control through SAP
 * \n     Supported in request of \ref MAL_UICC_APDU_REQ
 */
#define MAL_UICC_APDU_CONTROL                        0x53

/**
* \def   MAL_UICC_READY
* \brief Service type indicating that UICC is ready after reset indication.
* \n     Supported in indication  \ref MAL_UICC_APDU_RESET_IND
*/
#define MAL_UICC_READY                               0xB2

/**
* \def   MAL_UICC_ERROR
* \brief Service type indicating that there is UICC ERROR after reset indication.
* \n     Supported in indication  \ref MAL_UICC_APDU_RESET_IND
*/
#define MAL_UICC_ERROR                               0xB7

/**
 * \name Constant Table: UICC_ACTION_CONSTANTS
 * \par
 * \n This part describes the action constants used in UICC_SB_APDU_ACTIONS
 * \n for the service type MAL_UICC_APDU_CONTROL
 * \n
 */
/*--------------------------------------------*/
/**
 * \def   MAL_UICC_CONTROL_COLD_RESET
 * \brief cold reset
 */
#define MAL_UICC_CONTROL_COLD_RESET                  0x00

/**
 * \def   MAL_UICC_CONTROL_WARM_RESET
 * \brief warm reset
 */
#define MAL_UICC_CONTROL_WARM_RESET                  0x01

/**
 * \def   MAL_UICC_CONTROL_CARD_ACTIVATE
 * \brief card activate
 */
#define MAL_UICC_CONTROL_CARD_ACTIVATE               0x02

/**
 * \def   MAL_UICC_CONTROL_CARD_DEACTIVATE
 * \brief card deactivate
 */
#define MAL_UICC_CONTROL_CARD_DEACTIVATE             0x03

/**
 * \name Constant Table: UICC_PROTOCOL_TABLE
 * \par
 * \n This part describes the protocol constants used in UICC_SB_APDU_ACTIONS
 * \n for the service type MAL_UICC_APDU_CONTROL
 * \n
 */

/*--------------------------------------------*/
/**
 * \def   MAL_UICC_PROTOCOL_NOT_USED
 * \brief Protocol is not valid for the operation in question
 */
#define MAL_UICC_PROTOCOL_NOT_USED                   0x00

/**
 * \def   MAL_UICC_PROTOCOL_NO_PREFERENCE
 * \brief Allow UICC server select the Protocol
 */
#define MAL_UICC_PROTOCOL_NO_PREFERENCE              0x01

/**
 * \def   MAL_UICC_PROTOCOL_T0
 * \brief  Force to use T=0 Protocol
 */
#define MAL_UICC_PROTOCOL_T0                         0x02

/**
 * \def   MAL_UICC_PROTOCOL_T1
 * \brief  Force to use T=1 Protocol
 */
#define MAL_UICC_PROTOCOL_T1                         0x03

/*-----------------------------------------------------------------------------*/

/**
 * \name Data structures types
 * \par
 * This part describes the different data structures applicable
 * as param([in]/[out]) for the UICC MAL APDU SAP API and corresponding
 * to different message id's/event id's.
 */
/*-----------------------------------------------------------------*/

/**UICC CONNECTOR REQUEST*/
/**
 * \name MAL_UICC_CONNECTOR_REQ
 * \par
 * Structures involved in uicc connector request for connect, disconnect and reconnect features.
 */

/**
 * \struct uicc_sb_apdu_sap_info_t
 * \brief  This Structure corresponds to subblock UICC_SB_APDU_SAP_INFO
 */
typedef struct {
    uint8_t apdu_sap_id; /**< Unique identifier for the APDU SAP  */
} uicc_sb_apdu_sap_info_t;

/**
 * \struct uicc_reconnect_t
 * \brief  Structure holding subblocks for service type UICC_RECONNECT
 */
typedef struct {
    uicc_sb_apdu_sap_info_t uicc_sb_apdu_sap_info; /**< subblock: UICC_SB_APDU_SAP_INFO */
} uicc_reconnect_t;

/**
 * \struct uicc_connector_req_t
 * \brief This Structure is for  UICC_CONNECTOR_REQ Requests
 */
typedef struct {
    uint8_t service_type; /**< Service type -  UICC_CONNECT,UICC_DISCONNECT and UICC_RECONNECT
                           *   UICC_CONNECT and UICC_DISCONNECT no subblocks
                           *   UICC_RECONNECT - 1 sub block.But Currently UICC_RECONNECT IS NOT SUPPORTED
                           */
    union {
        uicc_reconnect_t uicc_reconnect; /**< for service type UICC_RECONNECT */
    } sub_block_t; /**< union holding structure for service type UICC_RECONNECT */
} uicc_connector_req_t;

/**UICC CONNECTOR RESPOSNSE*/
/**
 * \name UICC_CONNECTOR_RESP
 * \par
 * Structures involved in uicc connector response for connect, disconnect and reconnect features.
 */

/**
 * \struct uicc_connector_resp_t
 * \brief  This Structure is for UICC_CONNECTOR_RESP
 */
typedef struct {
    uint8_t service_type;  /**< A subset of values from the constant table UICC_SERVICE_TYPE_CONSTANTS */
    uint8_t status;        /**< A subset of values from the constant table UICC_STATUS_CONSTANTS */
    uint8_t details;       /**< A subset of values from the constant table UICC_DETAILS_CONSTANTS */
} uicc_connector_resp_t;

/**UICC APDU REQUEST*/
/**
 * \name UICC_APDU_REQ
 * \par
 * Structures involved in uicc apdu request for apdu send, apdu control and get atr.
 */

/**
 * \struct uicc_apdu_send_req_t
 * \brief  Structure holding subblocks for service type UICC_APDU_SEND
 */
typedef struct {
    uicc_sb_apdu_t uicc_sb_apdu; /**< subblock: UICC_SB_APDU */
} uicc_apdu_send_req_t;

/**
 * \struct uicc_sb_apdu_actions_t
 * \brief  This Structure corresponds to subblock UICC_SB_APDU_ACTIONS
 */
typedef struct {
    uint8_t action;
    /**<  UICC_CONTROL_COLD_RESET - FUNCTIONALITY NOT TESTED
     *    UICC_CONTROL_WARM_RESET - FUNCTIONALITY NOT TESTED
     *    UICC_CONTROL_CARD_ACTIVATE
     *    UICC_CONTROL_CARD_DEACTIVATE
     */
    uint8_t protocol;
    /**<  UICC_PROTOCOL_NOT_USED
     *    UICC_PROTOCOL_NO_PREFERENCE
     *    UICC_PROTOCOL_T0
     *    UICC_PROTOCOL_T1
     */
} uicc_sb_apdu_actions_t;

/**
 * \struct uicc_apdu_control_req_t
 * \brief  Structure holding subblocks for service type UICC_APDU_CONTROL
 */
typedef struct {
    uicc_sb_apdu_actions_t uicc_sb_apdu_actions; /**< subblock: UICC_SB_APDU_ACTIONS */
} uicc_apdu_control_req_t;

/**
 * \struct uicc_apdu_req_t
 * \brief  This Structure is for UICC_APDU_REQ
 */
typedef struct {
    uint8_t service_type; /**< service type */
    union {
        uicc_apdu_send_req_t    uicc_apdu_send_req;    /**< for service type UICC_APDU_SEND */
        uicc_apdu_control_req_t uicc_apdu_control_req; /**< for service type UICC_APDU_CONTROL */
    } sub_block_t; /**< union holding different structures corresponding to different service type requested */
} uicc_apdu_req_t;

/**UICC APDU RESPOSNSE*/
/**
 * \name UICC_APDU_RESP
 * \par
 * Structures involved in uicc apdu response for apdu send, apdu control and get atr.
 */

/**
 * \struct uicc_apdu_send_resp_t
 * \brief  Structure holding subblocks for service type UICC_APDU_SEND
 */
typedef struct {
    uicc_sb_apdu_t uicc_sb_apdu; /**< subblock: UICC_SB_APDU */
} uicc_apdu_send_resp_t;

/**
 * \struct uicc_apdu_atr_get_resp_t
 * \brief  Structure holding subblocks for service type UICC_APDU_ATR_GET
 */
typedef struct {
    uicc_sb_apdu_t uicc_sb_apdu; /**< subblock: UICC_SB_APDU */
} uicc_apdu_atr_get_resp_t;


/**
 * \struct uicc_apdu_resp_t
 * \brief  This Structure is for UICC_APDU_RESP
 */
typedef struct {
    uint8_t service_type;  /**< A subset of values from the constant table UICC_SERVICE_TYPE_CONSTANTS */
    uint8_t status;        /**< A subset of values from the constant table UICC_STATUS_CONSTANTS */
    uint8_t details;       /**< A subset of values from the constant table UICC_DETAILS_CONSTANTS */
    uint8_t n_sb;
    union {
        uicc_apdu_send_resp_t     uicc_apdu_send_resp;    /**< for service type UICC_APDU_SEND */
        uicc_apdu_atr_get_resp_t  uicc_apdu_atr_get_resp; /**< for service type UICC_APDU_ATR_GET */
    } sub_block;
} uicc_apdu_resp_t;

/**
 * \name MAL_UICC_APDU_RESET_IND
 * \par
 * Structures involved in case of APDU RESET Indications
 */

/**
 * \struct uicc_apdu_reset_ind_t
 * \brief  This main structure is applicable for the APDU Reset indications coming from modem
 */
typedef struct {
    uint8_t service_type; /**< A subset of values from the constant table UICC_SERVICE_TYPE_CONSTANTS */
} uicc_apdu_reset_ind_t;

#endif


