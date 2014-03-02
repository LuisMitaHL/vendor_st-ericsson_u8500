/**
 * \file  mal_uicc.h
 * \brief This file includes headers for UICC MAL API.
 *
 * \n Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * \n This code is ST-Ericsson proprietary and confidential.
 * \n Any use of the code for whatever purpose is subject to
 * \n specific written permission of ST-Ericsson SA.
 *
 * \par
 * \n MAL API header file for UICC Library.
 * \n \author ST-Ericsson
 * \n
 * \n Version : 1.0.8
 * \n
 * \n Revision History:
 * \par
 * \n v1.0.8               Added id MAL_UICC_CARD_READER_IND Indication for Sim card removal
 * \n v1.0.7               Added netlink socket fd, used to communicate with shm driver for modem silent reboot.
 * \n v1.0.6               Added SIM REFRESH support.
 * \n v1.0.5               Updated with CAT/SAT messages UICC_CAT_POLL, UICC_CAT_POLLING SET.
 * \n v1.0.4               CAT/SAT messages integrated.
 * \n v1.0.3               Aligned and latest tested on:
 * \n                      MODEM SW version : DV7 Pre-release, NOKIA UICC ISI SPEC version : 005.000
 * \n v1.0.2               Added SW(status word) subblock as part of SIMIO response data strutcure supported in DV7.
 * \n                      Changed the heap allocated response data struture to stack one for non SIMIO cases
 * \n                      where the response data available is fixed and known apriori.
 * \n v1.0.1               Code review comments incorporated.
 * \n v1.0                 Adopted common error handling and logging methods.
 * \n                      Added remaining service types of UICC_APPLICATION_REQ, UICC_PIN_REQ, UICC_APPL_CMD_REQ.
 * \n                      Added UICC details constants, added some UICC status constants.
 * \n v0.9.3               Added UICC CAT specific REQ ID's/Event ID's and associated data structures
 * \n v0.9.2               Added the support for UICC_SB_CHV multiple instances
 * \n v0.9.1               Added the Data Structures for UICC_APPL_STATUS_GET, UICC_APPLICATION_IND,
 * \n                      UICC_CARD_IND, UICC_IND. UICC_APPL_SHUT_DOWN_INITIATED, UICC_APPL_HOST_DEACTIVATE
 * \n                      Formatted for Doxygen, Added the Client tag Support
 * \n v0.9                 Added the Data Structures for the SIMIO
 * \n                      Update Linear Fixed and Update Transparent support
 * \n v0.8                 Added the Data Structures for the PIN
 * \n                      verify, PUK unblock and PIN Change support
 * \n v0.7                 Removed Pack4.5 Specific Code
 * \n                      Added constants for Card Type
 * \n v0.6                 Added API specific for SIM Status and IMSI Read
 * \n                      (For Pack 4.5 alone)
 * \n                      Added constants and structures
 * \n v0.5                 Changed struct typedef
 * \n                      Added #include <stdint.h> & all types integer
 * \n                      updated
 * \n                      Removed #include "type_def_modem_ext.h"
 */


/**
 * \defgroup  MAL_UICC UICC API
 * \par
 * \n This part describes the interface to UICC MAL Client
 */

#ifndef UICC_MAL_API_H
#define UICC_MAL_API_H

#include <stdint.h>
#include "mal_utils.h"

#define UICC_LIB_VERSION            "UICC Lib V 1.0.r3"

/**
 * \name Message ID's
 * \par
 * This part describes the different message id's or event id's supported
 * as part of UICC MAL client library.
 */

/**
 * \def   MAL_UICC_REQ
 * \brief Requests status of the UICC server
 * \n     Events and the response data outcome for this request.
 * \n     Refer \ref MAL_UICC_RESP, \ref MAL_UICC_IND
 * \param [in] - "uicc_req_t" \ref uicc_req_t
 */
#define MAL_UICC_REQ                                 0x00

/**
 * \def   MAL_UICC_RESP
 * \brief Response with status of SIM server
 * \param [out] - "mal_uicc_resp_sim_status_t" \ref mal_uicc_resp_sim_status_t
 */
#define MAL_UICC_RESP                                0x01

/**
 * \def   MAL_UICC_IND
 * \brief Indication for Server Ready or Not ready
 * \param [out] - "uicc_ind_t" \ref uicc_ind_t
 */
#define MAL_UICC_IND                                 0x02

/**
 * \def   MAL_UICC_CARD_READER_IND
 * \brief Indication for Sim card removal
 * \param [out] - "uicc_ind_t" \ref uicc_ind_t
 */
#define MAL_UICC_CARD_READER_IND                     0x26

/**
 * \def   MAL_UICC_APPLICATION_REQ
 * \brief Application request for knowing the application list and application activation
 * \n     Events and the response data outcome for this request.
 * \n     Refer \ref MAL_UICC_APPLICATION_RESP, \ref MAL_UICC_APPLICATION_IND
 * \param [in] - "mal_uicc_appln_req_t" \ref mal_uicc_appln_req_t
 */
#define MAL_UICC_APPLICATION_REQ                     0x06

/**
 * \def   MAL_UICC_APPLICATION_RESP
 * \brief Application response having the info on Application List and application activation
 * \param [out] - "mal_uicc_appln_resp_t" \ref mal_uicc_appln_resp_t
 */
#define MAL_UICC_APPLICATION_RESP                    0x07

/**
 * \def   MAL_UICC_APPLICATION_IND
 * \brief Indications giving the application Status
 * \param [out] - "uicc_application_ind_t" \ref uicc_application_ind_t
 */
#define MAL_UICC_APPLICATION_IND                     0x08

/**
 * \def   MAL_UICC_CARD_REQ
 * \brief Requests status of the SIM card
 * \n     Events and the response data outcome for this request.
 * \n     Refer \ref MAL_UICC_CARD_RESP, \ref MAL_UICC_CARD_IND
 * \param [in] - "uicc_card_req_t"  \ref uicc_card_req_t
 */
#define MAL_UICC_CARD_REQ                            0x03

/**
 * \def   MAL_UICC_CARD_RESP
 * \brief Response with status of SIM card
 * \param [out] - "mal_uicc_card_resp_t"  \ref mal_uicc_card_resp_t
 */
#define MAL_UICC_CARD_RESP                           0x04

/**
 * \def   MAL_UICC_CARD_IND
 * \brief Indication for Card Ready, Not present, Removed, Disconnected, Rejected
 * \param [out] - "uicc_card_ind_t" \ref uicc_card_ind_t
 */
#define MAL_UICC_CARD_IND                            0x05

/**
 * \def   MAL_UICC_PIN_REQ
 * \brief Requests for PIN verificcation, change, unblock
 * \n     Events and the response data outcome for this request.
 * \n     Refer \ref MAL_UICC_PIN_RESP, \ref MAL_UICC_PIN_IND
 * \param [in] - "uicc_pin_req_t" \ref uicc_pin_req_t
 */
#define MAL_UICC_PIN_REQ                             0x09

/**
 * \def   MAL_UICC_PIN_RESP
 * \brief Responses to pin verification, change, unblock requests
 * \param [out] - "uicc_pin_resp_t" \ref uicc_pin_resp_t
 */
#define MAL_UICC_PIN_RESP                            0x0A

/**
 * \def   MAL_UICC_PIN_IND
 * \brief Indications of PIN verify needed, PIN verified, PIN unblock needed
 * \param [out] - "uicc_pin_ind_t" \ref uicc_pin_ind_t
 */
#define MAL_UICC_PIN_IND                             0x0B

/**
 * \def   MAL_UICC_APPL_CMD_REQ
 * \brief Requests to read or update or fetch file info of SIM files
 * \n     Events and the response data outcome for this request.
 * \n     Refer \ref MAL_UICC_APPL_CMD_RESP, \ref MAL_UICC_APPL_CMD_IND
 * \param [in] - "uicc_appl_cmd_req_t" \ref uicc_appl_cmd_req_t
 */
#define MAL_UICC_APPL_CMD_REQ                        0x0C

/**
 * \def   MAL_UICC_APPL_CMD_RESP
 * \brief Response for MAL_UICC_APPL_CMD_REQ
 * \param [out] - "uicc_appl_cmd_resp_t" \ref uicc_appl_cmd_resp_t
 */
#define MAL_UICC_APPL_CMD_RESP                       0x0D

/**
 * \def   MAL_UICC_APPL_CMD_IND
 * \brief Indications specific to SIM file access commands
 */
#define MAL_UICC_APPL_CMD_IND                        0x0E

/**
 * \def   MAL_UICC_CAT_REQ
 * \brief Request UICC server for CAT requests
 * \n     Events and the response data outcome for this request.
 * \n     Refer \ref MAL_UICC_CAT_RESP, \ref MAL_UICC_CAT_IND
 * \param [in] - "uicc_cat_req_t" \ref uicc_cat_req_t
 */
#define MAL_UICC_CAT_REQ                             0x12

/**
 * \def   MAL_UICC_CAT_RESP
 * \brief Response for MAL_UICC_CAT_REQ
 * \param [out] - "uicc_cat_resp_t" \ref uicc_cat_resp_t
 */
#define MAL_UICC_CAT_RESP                            0x13

/**
 * \def   MAL_UICC_CAT_IND
 * \brief CAT Indications
 * \param [out] - "uicc_cat_ind_t" \ref uicc_cat_ind_t
 */
#define MAL_UICC_CAT_IND                             0x14

/**
 * \def   MAL_UICC_REFRESH_REQ
 * \brief Request UICC server for SIM Refresh
 * \n     Events and the response data outcome for this request.
 * \n     Refer \ref MAL_UICC_REFRESH_RESP, \ref MAL_UICC_REFRESH_IND
 * \param [in] - "uicc_refresh_req_t" \ref uicc_refresh_req_t
 */
#define MAL_UICC_REFRESH_REQ                         0x18

/**
 * \def   MAL_UICC_REFRESH_RESP
 * \brief Response for MAL_UICC_REFRESH_REQ
 * \param [out] "uicc_refresh_resp_t" \ref uicc_refresh_resp_t
 */
#define MAL_UICC_REFRESH_RESP                        0x19

/**
 * \def   MAL_UICC_REFRESH_IND
 * \brief REFRESH Indications
 * \param [out] "uicc_refresh_ind_t" \ref uicc_refresh_ind_t
 */
#define MAL_UICC_REFRESH_IND                         0x1A

/*------------------------------------------------------*/


/**
 * \name Service Type ID's - UICC_SERVICE_TYPE_CONSTANTS
 * \par
 * This part describes the different service types supported
 * as part of UICC MAL client library. Also termed as UICC_SERVICE_TYPE_CONSTANTS.
 */
/*--------------------------------------------------------------------------------*/

/**
 * \def   MAL_UICC_STATUS_GET
 * \brief Service type for acquiring the UICC server status. Supported in request \ref MAL_UICC_REQ
 */
#define MAL_UICC_STATUS_GET                          0xB1

/**
 * \def   MAL_UICC_START_UP_COMPLETE
 * \brief Service type indicating UICC server start up complete. Supported in indication \ref MAL_UICC_IND
 */
#define MAL_UICC_START_UP_COMPLETE                   0xB0

/**
 * \def   MAL_UICC_SHUTTING_DOWN
 * \brief Service type indicating UICC server shutting down. Supported in indication \ref MAL_UICC_IND
 */
#define MAL_UICC_SHUTTING_DOWN                       0xB5

/**
 * \def   MAL_UICC_CARD_DISCONNECTED
 * \brief Service type indicating card is disconnected. Supported in indication \ref MAL_UICC_CARD_IND
 */
#define MAL_UICC_CARD_DISCONNECTED                   0xC0

/**
 * \def   MAL_UICC_CARD_REMOVED
 * \brief Service type indicating card is removed. Supported in indication \ref MAL_UICC_CARD_IND
 */
#define MAL_UICC_CARD_REMOVED                        0xC1

/**
 * \def   MAL_UICC_CARD_NOT_PRESENT
 * \brief Service type indicating card is not present. Supported in indication \ref MAL_UICC_CARD_IND
 */
#define MAL_UICC_CARD_NOT_PRESENT                    0xC2

/**
 * \def   MAL_UICC_CARD_READY
 * \brief Service type indicating card is ready. Supported in indication \ref MAL_UICC_CARD_IND
 */
#define MAL_UICC_CARD_READY                          0xC4

/**
 * \def   MAL_UICC_CARD_REJECTED
 * \brief Service type indicating card is rejected. Supported in indication \ref MAL_UICC_CARD_IND
 */
#define MAL_UICC_CARD_REJECTED                       0xC8

/**
 * \def   MAL_UICC_APPL_LIST
 * \brief Service type requesting for UICC application List. Supported in request \ref MAL_UICC_APPLICATION_REQ
 */
#define MAL_UICC_APPL_LIST                           0x01

/**
 * \def   MAL_UICC_APPL_HOST_ACTIVATE
 * \brief Service type requesting for UICC application activation from host.
 * \n     Supported in request \ref MAL_UICC_APPLICATION_REQ
 */
#define MAL_UICC_APPL_HOST_ACTIVATE                  0x03

/**
 * \def   MAL_UICC_APPL_START_UP_COMPLETE
 * \brief Service type requesting for UICC application start up complete.
 * \n     Supported in request \ref MAL_UICC_APPLICATION_REQ
 */
#define MAL_UICC_APPL_START_UP_COMPLETE              0x05

/**
 * \def   MAL_UICC_APPL_SHUT_DOWN_INITIATED
 * \brief Service type request for initiating application shutdown. Supported in request \ref MAL_UICC_APPLICATION_REQ
 */
#define MAL_UICC_APPL_SHUT_DOWN_INITIATED            0x06

/**
 * \def   MAL_UICC_APPL_STATUS_GET
 * \brief Service type request to get status of the application. Supported in request  \ref MAL_UICC_APPLICATION_REQ
 */
#define MAL_UICC_APPL_STATUS_GET                     0x07

/**
 * \def   MAL_UICC_APPL_HOST_DEACTIVATE
 * \brief Service type request to application de-activation from host.
 * \n     Supported in request \ref MAL_UICC_APPLICATION_REQ
 */
#define MAL_UICC_APPL_HOST_DEACTIVATE                0x09

/**
 * \def   MAL_UICC_APPL_TERMINATED
 * \brief Service type indicationg that application is terminated.
 * \n     Supported in indication \ref MAL_UICC_APPLICATION_IND
 */
#define MAL_UICC_APPL_TERMINATED                     0x71

/**
 * \def   MAL_UICC_APPL_RECOVERED
 * \brief Service type indicating that application is recovered.
 * \n     Supported in indication \ref MAL_UICC_APPLICATION_IND
 */
#define MAL_UICC_APPL_RECOVERED                      0x72

/**
 * \def   MAL_UICC_APPL_ACTIVATED
 * \brief Service type indicating that application is activated.
 * \n     Supported in indication  \ref MAL_UICC_APPLICATION_IND
 */
#define MAL_UICC_APPL_ACTIVATED                      0x75

/**
 * \def   MAL_UICC_CARD_STATUS_GET
 * \brief Service type request to get SIM card status.
 * \n     Supported in request \ref MAL_UICC_CARD_REQ
 */
#define MAL_UICC_CARD_STATUS_GET                     0xC5

/**
 * \def   MAL_UICC_CARD_INFO_GET
 * \brief Service type request to get SIM card info. Supported in request \ref MAL_UICC_CARD_REQ
 */
#define MAL_UICC_CARD_INFO_GET                       0xC9

/**
 * \def   MAL_UICC_APPL_READ_TRANSPARENT
 * \brief Service type request to read transparent type files on SIM. Supported in request \ref MAL_UICC_APPL_CMD_REQ
 */
#define MAL_UICC_APPL_READ_TRANSPARENT               0x21

/**
 * \def   MAL_UICC_APPL_UPDATE_TRANSPARENT
 * \brief Service type request to update transparent type files on SIM. Supported in request \ref MAL_UICC_APPL_CMD_REQ
 */
#define MAL_UICC_APPL_UPDATE_TRANSPARENT             0x22

/**
 * \def   MAL_UICC_APPL_READ_LINEAR_FIXED
 * \brief Service type request to read linear fixed type files on SIM. Supported in request \ref MAL_UICC_APPL_CMD_REQ
 */
#define MAL_UICC_APPL_READ_LINEAR_FIXED              0x23

/**
 * \def   MAL_UICC_APPL_UPDATE_LINEAR_FIXED
 * \brief Service type request to update linear fixed type files on SIM. Supported in request \ref MAL_UICC_APPL_CMD_REQ
 */
#define MAL_UICC_APPL_UPDATE_LINEAR_FIXED            0x24

/**
 * \def   MAL_UICC_APPL_FILE_INFO
 * \brief Service type request to read file info on SIM. Supported in request \ref MAL_UICC_APPL_CMD_REQ
 */
#define MAL_UICC_APPL_FILE_INFO                      0x25

/**
 * \def   MAL_UICC_APPL_APDU_SEND
 * \brief Service type request to send an APDU. Supported in request \ref MAL_UICC_APPL_CMD_REQ
 */
#define MAL_UICC_APPL_APDU_SEND                      0x26

/**
 * \def   MAL_UICC_APPL_CLEAR_CACHE
 * \brief Service type request to clear cached data in UICC server. Supported in request \ref MAL_UICC_APPL_CMD_REQ
 */
#define MAL_UICC_APPL_CLEAR_CACHE                    0x27

/**
 * \def   MAL_UICC_APPL_SESSION_START
 * \brief Service type request for starting file access session. Supported in request \ref MAL_UICC_APPL_CMD_REQ
 */
#define MAL_UICC_APPL_SESSION_START                  0x28

/**
 * \def   MAL_UICC_APPL_SESSION_END
 * \brief Service type request to end file access session. Supported in request \ref MAL_UICC_APPL_CMD_REQ
 */
#define MAL_UICC_APPL_SESSION_END                    0x29

/**
 * \def   MAL_UICC_APPL_READ_CYCLIC
 * \brief Service type request for reading a record in a cyclic file. Supported in request \ref MAL_UICC_APPL_CMD_REQ
 */
#define MAL_UICC_APPL_READ_CYCLIC                    0x2A

/**
 * \def   MAL_UICC_APPL_UPDATE_CYCLIC
 * \brief Service type request for updating a record in a cyclic file. Supported in request \ref MAL_UICC_APPL_CMD_REQ
 */
#define MAL_UICC_APPL_UPDATE_CYCLIC                  0x2B

/**
 * \def   MAL_UICC_PIN_VERIFY
 * \brief Service type request for PIN verification. Supported in request \ref MAL_UICC_PIN_REQ
 */
#define MAL_UICC_PIN_VERIFY                          0x11

/**
 * \def   MAL_UICC_PIN_UNBLOCK
 * \brief Service type request for PIN Unblock(PUK). Supported in request  \ref MAL_UICC_PIN_REQ
 */
#define MAL_UICC_PIN_UNBLOCK                         0x12

/**
 * \def   MAL_UICC_PIN_DISABLE
 * \brief Service type request for PIN Disable. Supported in request  \ref MAL_UICC_PIN_REQ
 */
#define MAL_UICC_PIN_DISABLE                         0x13

/**
 * \def   MAL_UICC_PIN_ENABLE
 * \brief Service type request for PIN Enable. Supported in request  \ref MAL_UICC_PIN_REQ
 */
#define MAL_UICC_PIN_ENABLE                          0x14

/**
 * \def   MAL_UICC_PIN_CHANGE
 * \brief Service type request for PIN Change. Supported in request  \ref MAL_UICC_PIN_REQ
 */
#define MAL_UICC_PIN_CHANGE                          0x15

/**
 * \def   MAL_UICC_PIN_SUBSTITUTE
 * \brief Service type request for PIN Substitute. Supported in request  \ref MAL_UICC_PIN_REQ
 */
#define MAL_UICC_PIN_SUBSTITUTE                      0x16

/**
 * \def   MAL_UICC_PIN_INFO
 * \brief Service type request for PIN Info. Supported in request \ref MAL_UICC_PIN_REQ
 */
#define MAL_UICC_PIN_INFO                            0x17

/**
 * \def   MAL_UICC_PIN_PROMPT_VERIFY
 * \brief Service type request for PIN Prompt Verify. Supported in request \ref MAL_UICC_PIN_REQ
 */
#define MAL_UICC_PIN_PROMPT_VERIFY                   0x18

/**
 * \def   MAL_UICC_CAT_ENABLE
 * \brief Service type request to enable UICC server to fetch proactive commands. Supported in request \ref MAL_UICC_CAT_REQ
 */
#define MAL_UICC_CAT_ENABLE                          0x41

/**
 * \def   MAL_UICC_CAT_DISABLE
 * \brief Service type request to disable UICC server to fetch proactive commands. Supported in request \ref MAL_UICC_CAT_REQ
 */
#define MAL_UICC_CAT_DISABLE                         0x42

/**
 * \def   MAL_UICC_CAT_TERMINAL_PROFILE
 * \brief Service type request to provide terminal profile to the UICC server. Supported in request \ref MAL_UICC_CAT_REQ
 */
#define MAL_UICC_CAT_TERMINAL_PROFILE                0x43

/**
 * \def   MAL_UICC_CAT_TERMINAL_RESPONSE
 * \brief Service type request to be sent after finishing the processing of proactive command. Supported in request \ref MAL_UICC_CAT_REQ
 */
#define MAL_UICC_CAT_TERMINAL_RESPONSE               0x44

/**
 * \def   MAL_UICC_CAT_ENVELOPE
 * \brief Service type request to send envelope commands to the SIM. Supported in request \ref MAL_UICC_CAT_REQ
 */
#define MAL_UICC_CAT_ENVELOPE                        0x45

/**
 * \def   MAL_UICC_CAT_POLLING_SET
 * \brief Service type request for controlling the interval at which the UICC server polling the SIM card
 * \n     for proactive commands. Supported in request \ref MAL_UICC_CAT_REQ
 */
#define MAL_UICC_CAT_POLLING_SET                     0x46

/**
 * \def   MAL_UICC_CAT_REFRESH
 * \brief Service type request to initiate a SIM refresh.
 * \n     Supported in request \ref MAL_UICC_CAT_REQ
 */
#define MAL_UICC_CAT_REFRESH                         0x47

/**
 * \def   MAL_UICC_CAT_POLL
 * \brief Service type requesting UICC Server to perform single proactive command poll.
 * \n     Supported in request \ref MAL_UICC_CAT_REQ
 */
#define MAL_UICC_CAT_POLL                            0x48

/**
 * \def   MAL_UICC_REFRESH_STATUS
 * \brief Service type requesting UICC Server to inform that UICC Client's application is affected by the refresh.
 * \n     Supported in request \ref MAL_UICC_REFRESH_REQ
 */
#define MAL_UICC_REFRESH_STATUS                      0x61

/**
 * \def   MAL_UICC_CAT_FETCHED_CMD
 * \brief Service type indicating that proactive command is received.
 * \n     Supported in indication  \ref MAL_UICC_CAT_IND
 */
#define MAL_UICC_CAT_FETCHED_CMD                     0x91

/**
 * \def   MAL_UICC_CAT_NOT_SUPPORTED
 * \brief Service type indicating that CAT is not supported.
 * \n     Supported in indication  \ref MAL_UICC_CAT_IND
 */
#define MAL_UICC_CAT_NOT_SUPPORTED                   0x92

/**
 * \def   MAL_UICC_CAT_REG_FAILED
 * \brief Service type indicating that CAT registration is failed.
 * \n     Supported in indication  \ref MAL_UICC_CAT_IND
 */
#define MAL_UICC_CAT_REG_FAILED                      0x93

/**
 * \def   MAL_UICC_CAT_REG_OK
 * \brief Service type indicating that CAT registration is OK.
 * \n     Supported in indication  \ref MAL_UICC_CAT_IND
 */
#define MAL_UICC_CAT_REG_OK                          0x94

/**
 * \def   MAL_UICC_REFRESH_PERMISSION
 * \brief Service type indicating permission to start a refresh.
 * \n     Supported in indication  \ref MAL_UICC_REFRESH_IND
 */
#define MAL_UICC_REFRESH_PERMISSION                  0xA1

/**
 * \def   MAL_UICC_REFRESH_STARTING
 * \brief Service type indicating that a refresh is starting and the application may be inaccessible.
 * \n     Supported in indication  \ref MAL_UICC_REFRESH_IND
 */
#define MAL_UICC_REFRESH_STARTING                    0xA2

/**
 * \def   MAL_UICC_REFRESH_CANCELLED
 * \brief Service type indicating that a pending refresh is cancelled.
 * \n     Supported in indication  \ref MAL_UICC_REFRESH_IND
 */
#define MAL_UICC_REFRESH_CANCELLED                   0xA3

/**
 * \def   MAL_UICC_REFRESH_NOW
 * \brief Service type indicating that UICC Server has completed internal refreshing and the clients
 * \n     can start accessing the refreshed data.
 * \n     Supported in indication  \ref MAL_UICC_REFRESH_IND
 */
#define MAL_UICC_REFRESH_NOW                         0xA4

/**
 * \def   MAL_UICC_READY
 * \brief Service type indicating that proactive command is received.
 * \n     Supported in indication  \ref MAL_UICC_CAT_IND
 */
#define MAL_UICC_READY                               0xB2

/*-------------------------------------------------------------------------------------------------------*/


/**
 * \name Various Status Constants - UICC_STATUS_CONSTANTS
 * \par
 * This part describes the different status constants pertaining to
 * different services supported as part of UICC MAL client library.
 */
/*----------------------------------------------------------------*/

/**
 * \def   MAL_UICC_STATUS_OK
 * \brief Request performed successfully
 */
#define MAL_UICC_STATUS_OK                           0x00

/**
 * \def   MAL_UICC_STATUS_FAIL
 * \brief Error in performing the command
 */
#define MAL_UICC_STATUS_FAIL                         0x01

/**
 * \def   MAL_UICC_STATUS_UNKNOWN
 * \brief Status is Unknown
 */
#define MAL_UICC_STATUS_UNKNOWN                      0x02

/**
 * \def   MAL_UICC_STATUS_NOT_READY
 * \brief uicc server status "Server is not ready" as part of response \ref MAL_UICC_RESP
 */
#define MAL_UICC_STATUS_NOT_READY                    0x10

/**
 * \def   MAL_UICC_STATUS_START_UP_COMPLETED
 * \brief uicc server status "Server start up is completed" as part of response \ref MAL_UICC_RESP
 */
#define MAL_UICC_STATUS_START_UP_COMPLETED           0x11

/**
 * \def   MAL_UICC_STATUS_SHUTTING_DOWN
 * \brief uicc server status "Server is shutting down" as part of response \ref MAL_UICC_RESP
 */
#define MAL_UICC_STATUS_SHUTTING_DOWN                0x12

/**
 * \def   MAL_UICC_STATUS_CARD_NOT_READY
 * \brief card status "Smart card is not ready" as part of response \ref MAL_UICC_CARD_RESP
 */
#define MAL_UICC_STATUS_CARD_NOT_READY               0x20

/**
 * \def   MAL_UICC_STATUS_CARD_READY
 * \brief card status "Smart card is ready" as part of response \ref MAL_UICC_CARD_RESP
 */
#define MAL_UICC_STATUS_CARD_READY                   0x21

/**
 * \def   MAL_UICC_STATUS_CARD_DISCONNECTED
 * \brief card status "Smart card is disconnected" as part of response \ref MAL_UICC_CARD_RESP
 */
#define MAL_UICC_STATUS_CARD_DISCONNECTED            0x22

/**
 * \def   MAL_UICC_STATUS_CARD_NOT_PRESENT
 * \brief card status "Smart card is not present" as part of response \ref MAL_UICC_CARD_RESP
 */
#define MAL_UICC_STATUS_CARD_NOT_PRESENT             0x23

/**
 * \def   MAL_UICC_STATUS_CARD_REJECTED
 * \brief card status "Smart card has been rejected" as part of response \ref MAL_UICC_CARD_RESP
 */
#define MAL_UICC_STATUS_CARD_REJECTED                0x24

/**
 * \def   MAL_UICC_STATUS_APPL_ACTIVE
 * \brief Application is active as part of response \ref MAL_UICC_APPLICATION_RESP
 */
#define MAL_UICC_STATUS_APPL_ACTIVE                  0x30

/**
 * \def   MAL_UICC_STATUS_APPL_NOT_ACTIVE
 * \brief Application is not active as part of response \ref MAL_UICC_APPLICATION_RESP
 */
#define MAL_UICC_STATUS_APPL_NOT_ACTIVE              0x31

/**
 * \def   MAL_UICC_STATUS_PIN_ENABLED
 * \brief PIN verification used \ref MAL_UICC_PIN_RESP
 */
#define MAL_UICC_STATUS_PIN_ENABLED                  0x40

/**
 * \def   MAL_UICC_STATUS_PIN_ENABLED
 * \brief PIN verification not used \ref MAL_UICC_PIN_RESP
 */
#define MAL_UICC_STATUS_PIN_DISABLED                 0x41

/*---------------------------------------------------------------------------------*/


/**
 * \name Various UICC Details Constants - UICC_DETAILS_CONSTANTS
 * \par
 * This part describes the different UICC details constants pertaining to
 * different services supported as part of UICC MAL client library.
 */
/*----------------------------------------------------------------*/

/**
 * \def   MAL_UICC_NO_DETAILS
 * \brief Used when status differs from MAL_UICC_STATUS_FAIL
 */
#define MAL_UICC_NO_DETAILS                          0x00

/**
 * \def   MAL_UICC_INVALID_PARAMETERS
 * \brief Request was sent with one or more invalid parameters
 */
#define MAL_UICC_INVALID_PARAMETERS                  0x01

/**
 * \def   MAL_UICC_FILE_NOT_FOUND
 * \brief The file wasn't found
 */
#define MAL_UICC_FILE_NOT_FOUND                      0x02

/**
 * \def   MAL_UICC_SECURITY_CONDITIONS_NOT_SATISFIED
 * \brief User does not have the required privileges for this
 */
#define MAL_UICC_SECURITY_CONDITIONS_NOT_SATISFIED   0x03

/**
 * \def   MAL_UICC_APPL_CONFLICT
 * \brief Application can not be activated due to already active application
 */
#define MAL_UICC_APPL_CONFLICT                       0x04

/**
 * \def   MAL_UICC_CARD_ERROR
 * \brief Card Communication error
 */
#define MAL_UICC_CARD_ERROR                          0x05

/**
 * \def   MAL_UICC_SERVICE_NOT_SUPPORTED
 * \brief Operation not supported
 */
#define MAL_UICC_SERVICE_NOT_SUPPORTED               0x06

/**
 * \def   MAL_UICC_SERVICE_NOT_SUPPORTED
 * \brief Session expired
 */
#define MAL_UICC_SESSION_EXPIRED                     0x07


/**
 * \name Various Application Type Macros
 * \par
 * This part describes the different application types
 * supported as part of UICC MAL client library
 */
/*------------------------------------------------------------------------------------*/

/**
 * \def   MAL_UICC_APPL_TYPE_UNKNOWN
 * \brief Application type unknown as part of response \ref MAL_UICC_APPLICATION_RESP
 */
#define MAL_UICC_APPL_TYPE_UNKNOWN                   0x00

/**
 * \def   MAL_UICC_APPL_TYPE_ICC_SIM
 * \brief Application of type ICC SIM as part of response \ref MAL_UICC_APPLICATION_RESP
 */
#define MAL_UICC_APPL_TYPE_ICC_SIM                   0x01

/**
 * \def   MAL_UICC_APPL_TYPE_UICC_USIM
 * \brief Application of type UICC USIM as part of response  \ref MAL_UICC_APPLICATION_RESP
 */
#define MAL_UICC_APPL_TYPE_UICC_USIM                 0x02

/**
 * \def   MAL_UICC_APPL_TYPE_UICC_ISIM
 * \brief Application of type UICC ISIM as part of response \ref MAL_UICC_APPLICATION_RESP
 */
#define MAL_UICC_APPL_TYPE_UICC_ISIM                 0x03
/*----------------------------------------------------------------------------------------*/


/**
 * \name Various Card Type Macros - UICC_CARD_TYPE_TABLE
 * \par
 * This part describes the different card types
 * supported as part of UICC MAL client library
 */
/*---------------------------------------------------------------------------------------*/

/**
 * \def   MAL_UICC_CARD_TYPE_UNKNOWN
 * \brief smart card type \ref MAL_UICC_APPLICATION_RESP
 */
#define MAL_UICC_CARD_TYPE_UNKNOWN                   0x00

/**
 * \def   MAL_UICC_CARD_TYPE_ICC
 * \brief smart card type \ref MAL_UICC_APPLICATION_RESP
 */
#define MAL_UICC_CARD_TYPE_ICC                       0x01

/**
 * \def   MAL_UICC_CARD_TYPE_UICC
 * \brief smart card type \ref MAL_UICC_APPLICATION_RESP
 */
#define MAL_UICC_CARD_TYPE_UICC                      0x02

/**
 * \def   MAL_UICC_CARD_TYPE_UICC
 * \brief smart card type \ref MAL_UICC_APPLICATION_RESP
 */
#define MAL_UICC_CARD_TYPE_USB                       0x03
/*-----------------------------------------------------------------------------------------*/


/**
 * \name Various PIN Indication Constants
 * \par
 * This part describes the different Pin indication constants
 * supported as part of UICC MAL client library.
 */
/*------------------------------------------------------------------------------------*/

/**
 * \def   MAL_UICC_PIN_VERIFY_NEEDED
 * \brief PIN Verification Needed \ref MAL_UICC_PIN_IND
 */
#define MAL_UICC_PIN_VERIFY_NEEDED                   0x81

/**
 * \def   MAL_UICC_PIN_UNBLOCK_NEEDED
 * \brief PIN Unblock Needed \ref MAL_UICC_PIN_IND
 */
#define MAL_UICC_PIN_UNBLOCK_NEEDED                  0x82

/**
 * \def   MAL_UICC_PIN_PERMANENTLY_BLOCKED
 * \brief PIN Permanently Blocked \ref MAL_UICC_PIN_IND
 */
#define MAL_UICC_PIN_PERMANENTLY_BLOCKED             0x83

/**
 * \def   MAL_UICC_PIN_VERIFIED
 * \brief PIN Verified  \ref MAL_UICC_PIN_IND
 */
#define MAL_UICC_PIN_VERIFIED                        0x84
/*-------------------------------------------------------------------------------------*/


/**
 * \name Various PIN Code Qualifier Constants
 * \par
 * \n This part describes the different PIN Code Qualifier constants
 * \n supported as part of UICC MAL client library.
 * \n
 */
/*-----------------------------------------------------------------------------------*/

/**
 * \def   MAL_UICC_PIN_UNKNOWN
 * \brief PIN Code Qualifier Not Needed  \ref MAL_UICC_PIN_REQ
 */
#define MAL_UICC_PIN_UNKNOWN                         0x00

/**
 * \def   MAL_UICC_PIN_NEW
 * \brief PIN Code Qualifier NEW PIN  \ref MAL_UICC_PIN_REQ \ref MAL_UICC_PIN_CHANGE
 */
#define MAL_UICC_PIN_NEW                             0x01

/**
 * \def   MAL_UICC_PIN_OLD
 * \brief PIN Code Qualifier OLD PIN  \ref MAL_UICC_PIN_REQ \ref MAL_UICC_PIN_CHANGE
 */
#define MAL_UICC_PIN_OLD                             0x02
/*-------------------------------------------------------------------------------------*/


/**
 * \name Generic constants
 * \par
 * \n This part describes the generic constants used in UICC MAL.
 * \n
 */
/*--------------------------------------------*/

/**
 * \def   MAX_APPL_LIST_SIZE
 * \brief Macro to define the Max no. of applications possible on SIM
 * \n
 */
#define MAX_APPLIST_SIZE                             256

/**
 * \def   MAX_CHV_LIST_SIZE
 * \brief Macro to define the Max no. of CHV instances holding pin id and pin qualifiers possible.
 * \n     Applicable for only for ICC Card Type. Otherwise the same info has to be taken from PIN Indications.
 */
#define MAX_CHV_LIST_SIZE                            2

/**
 * \def   MAX_AID
 * \brief Macro to define the Maximum no. of Applications identified using Appl id undergo refresh.
 * \n
 */
#define MAX_AID                                      1

/**
 * \def   MAX_APPL_PATH
 * \brief Macro to define the Maximum no. of EF's/DF's/MF's identified using the APPL_PATH that undergo refresh.
 */
#define MAX_APPL_PATH                                256

/**
 * \def   MAL_UICC_EF_ID_NOT_PRESENT
 * \brief Value to be used when EF Identifier is not present.
 */
#define MAL_UICC_EF_ID_NOT_PRESENT                   0x00

/**
 * \def   MAL_UICC_SFI_NOT_PRESENT
 * \brief Value to be used when Short File Identifier is not present.
 */
#define MAL_UICC_SFI_NOT_PRESENT                     0x00

/**
 * \def   MAL_UICC_APPL_ID_UNKNOWN
 * \brief Value used for Application ID when it is unknown.
 */
#define MAL_UICC_APPL_ID_UNKNOWN                     0x00

/**
 * \def   MAL_UICC_APPL_LAST
 * \brief Value for selecting last active application.
 */
#define MAL_UICC_APPL_LAST                           0xFF

/**
 * \def   MAL_UICC_APPL_ID_NOT_USED
 * \brief Value to be used when application ID is not required.
 */
#define MAL_UICC_APPL_ID_NOT_USED                    0x00

/**
 * \def   MAL_UICC_SESSION_ID_NOT_USED
 * \brief Value to be used when there is no file access session needed.
 */
#define MAL_UICC_SESSION_ID_NOT_USED                 0x00


/**
 * \name Bitmask Table: UICC_RESP_INFO_BIT_TABLE
 * \par
 * \n This part describes the Response info bitfield used in UICC MAL.
 * \n
 */
/*--------------------------------------------*/

/**
 * \def   MAL_UICC_RESP_INFO_STATUS_WORD
 * \brief UICC_SB_STATUS_WORD is returned if this is set.
 */
#define MAL_UICC_RESP_INFO_STATUS_WORD               0x01  /* -------1 */


/**
 * \name Constant Table: UICC_ACCESS_MODE_TABLE
 * \par
 * \n This part describes the Access mode constants used in UICC MAL.
 * \n
 */
/*--------------------------------------------*/

/**
 * \def   MAL_UICC_ACCESS_MODE_READ
 * \brief Access mode read
 */
#define MAL_UICC_ACCESS_MODE_READ                    0x01

/**
 * \def   MAL_UICC_ACCESS_MODE_UPDATE
 * \brief Access mode update
 */
#define MAL_UICC_ACCESS_MODE_UPDATE                  0x02

/**
 * \def   MAL_UICC_ACCESS_MODE_ACTIVATE
 * \brief Access mode activate
 */
#define MAL_UICC_ACCESS_MODE_ACTIVATE                0x03

/**
 * \def   MAL_UICC_ACCESS_MODE_DEACTIVATE
 * \brief Access mode deactivate
 */
#define MAL_UICC_ACCESS_MODE_DEACTIVATE              0x04

/**
 * \name Constant Table: Refresh type constants
 * \par
 * \n This part describes the SIM refresh type constants used in UICC MAL.
 * \n
 */
/*--------------------------------------------*/

/**
 * \def   MAL_UICC_ACCESS_MODE_READ
 * \brief Init from PIN
 */
#define MAL_UICC_REFRESH_NAA_INIT                    0x00

/**
 * \def   MAL_UICC_REFRESH_NAA_FILE_CHANGE
 * \brief FCN, File Change Notification
 */
#define MAL_UICC_REFRESH_NAA_FILE_CHANGE             0x01

/**
 * \def   MAL_UICC_REFRESH_NAA_INIT_FILE_CHANGE
 * \brief Init from PIN FCN
 */
#define MAL_UICC_REFRESH_NAA_INIT_FILE_CHANGE        0x02

/**
 * \def   MAL_UICC_REFRESH_NAA_INIT_FULL_FILE_CHANGE
 * \brief Init from PIN FFCN
 */
#define MAL_UICC_REFRESH_NAA_INIT_FULL_FILE_CHANGE   0x03

/**
 * \def   MAL_UICC_REFRESH_UICC_RESET
 * \brief Reset
 */
#define MAL_UICC_REFRESH_UICC_RESET                  0x04

/**
 * \def   MAL_UICC_REFRESH_NAA_APPLICATION_RESET
 * \brief Application Reset
 */
#define MAL_UICC_REFRESH_NAA_APPLICATION_RESET       0x05

/**
 * \def   MAL_UICC_REFRESH_NAA_SESSION_RESET
 * \brief Refresh session Reset
 */
#define MAL_UICC_REFRESH_NAA_SESSION_RESET           0x06

/**
 * \name Constant Table: UICC_REFRESH_STATUS_TABLE
 * \par
 * \n This part describes the SIM refresh status constants used in UICC MAL.
 * \n
 */
/*--------------------------------------------*/

/**
 * \def   MAL_UICC_REFRESH_OK
 * \brief Refresh OK
 */
#define MAL_UICC_REFRESH_OK                          0x01

/**
 * \def   MAL_UICC_REFRESH_NOT_OK
 * \brief Refresh NOT OK
 */
#define MAL_UICC_REFRESH_NOT_OK                      0x02

/**
 * \def   MAL_UICC_REFRESH_DONE
 * \brief Refresh Done
 */
#define MAL_UICC_REFRESH_DONE                        0x03

/**
 * \name Various UICC_APDU_CMD_FORCE_TYPE_TABLE Constants
 * \par
 * \n This part describes the command force constants used in UICC_SB_APDU
 */
/*------------------------------------------------------------------------------------*/

/**
 * \def   MAL_UICC_APDU_CMD_FORCE_NOT_USED
 * \brief command force value not used
 */
#define MAL_UICC_APDU_CMD_FORCE_NOT_USED             0x00

/**
 * \def   MAL_UICC_APDU_CMD_FORCE
 * \brief command force value used
 */
#define MAL_UICC_APDU_CMD_FORCE                      0x01

/**
 * \name Various UICC_CLF_IF_SUPPORT_TABLE Contants
 * \par
 * \n This part describes the CLF interface support in uicc_sb_card_info_t
 */
/*------------------------------------------------------------------------------------*/

/** \def MAL_UICC_CLF_IF_NOT_SUPPORTED
  * \par
  * \n CLF Interface not supported by the UICC
  */
#define MAL_UICC_CLF_IF_NOT_SUPPORTED     0x01

/** \def MAL_UICC_CLF_IF_SUPPORTED
 *  \brief CLF Interface supported by the UICC
 */
#define MAL_UICC_CLF_IF_SUPPORTED         0x02

/**
 * \name Various UICC_CARD_REJECT_CAUSE_TABLE Contants
 * \par
 * \n This part describes the reject cause in uicc_sb_card_reject_cause_t
 */
/*------------------------------------------------------------------------------------*/

/** \def MAL_UICC_CARD_REJECT_CAUSE_NOT_AVAILABLE
  * \par
  * \n The card was rejected due to unknown reason
  */
#define MAL_UICC_CARD_REJECT_CAUSE_NOT_AVAILABLE     0x00

/** \def MAL_UICC_CARD_REJECT_CAUSE_SIMLOCK
 *  \brief The card was rejected due to SIM lock
 */
#define MAL_UICC_CARD_REJECT_CAUSE_SIMLOCK           0x01

/** \def MAL_UICC_CARD_REJECT_CAUSE_SW_6F00
 *  \brief The card was rejected due to three consecutive 6F00 results.
 */
#define MAL_UICC_CARD_REJECT_CAUSE_SW_6F00           0x02

/**
 * \name Various UICC_CARD_REMOVE_CAUSE_TABLE Contants
 * \par
 * \n This part describes the remove cause in uicc_sb_card_remove_cause_t
 */
/*------------------------------------------------------------------------------------*/

/** \def MAL_UICC_CARD_REMOVE_CAUSE_NOT_AVAILABLE
  * \par
  * \n The card was removed due to unknown reason
  */
#define MAL_UICC_CARD_REMOVE_CAUSE_NOT_AVAILABLE     0x00

/** \def MAL_UICC_CARD_REMOVE_CAUSE_PRESENCE_DETECTION
  * \par
  * \n The card was removed due to resence detection failure. Card recovery in progress.
  */
#define MAL_UICC_CARD_REMOVE_CAUSE_PRESENCE_DETECTION 0x01

/** \name Various UICC_VOLTAGE_CLASS_TABLE constants
 *  \par
 *  \n This part describes the voltage class in uicc_sb_card_info_t
 *  \n and uicc_sb_card_info_get_t.
 */

/** \def MAL_UICC_VOLTAGE_CLASS_NOT_AVAILABLE
 *  \brief The voltage class is not available
 */
#define MAL_UICC_VOLTAGE_CLASS_NOT_AVAILABLE            0x00

/** \def MAL_UICC_VOLTAGE_CLASS_A
 *  \brief Voltage class A, 5.0V
 */
#define MAL_UICC_VOLTAGE_CLASS_A                        0x50

/** \def MAL_UICC_VOLTAGE_CLASS_B
 *  \brief Voltage class B, 3.0V
 */
#define MAL_UICC_VOLTAGE_CLASS_B                        0x30

/** \def MAL_UICC_VOLTAGE_CLASS_C
 *  \brief Voltage class C, 1.8V
 */
#define MAL_UICC_VOLTAGE_CLASS_C                        0x18

/**
 * \name Data structures types
 * \par
 * This part describes the different data structures applicable
 * as param([in]/[out]) for the UICC MAL API and corresponding
 * to different message id's/event id's.
 */
/*-----------------------------------------------------------------*/

/**
 * \struct data_t
 * \brief  Used for preparing the data
 * \par
 * \n  This subblock is used as part of \ref uicc_appl_cmd_resp_t,
 * \n  \ref uicc_cat_req_t, \ref uicc_cat_resp_t, \ref uicc_cat_ind_t.
 */
typedef struct {
    uint32_t data_length; /**< The length of the data in bytes */
    uint8_t  *data;       /**< data */
} data_t;
/*--------------------------------------------*/


/**
 * \name MAL_UICC_REQ
 * \par
 * UICC server interface allows client to request the status of the UICC server
 */

/**
 * \struct uicc_req_t
 * \brief  This main structure is for UICC server related requests.
 */
typedef struct {
    uint8_t service_type; /**< A subset of values from the constant table UICC_SERVICE_TYPE_CONSTANTS */
} uicc_req_t;


/**
 * \name MAL_UICC_IND
 * \par
 * Structures involved in case of UICC Server related Indications.
 */

/**
 * \struct uicc_ind_t
 * \brief This main structure is applicable for the UICC Server related indications coming from modem
 */
typedef struct {
    uint8_t service_type; /**< A subset of values from the constant table UICC_SERVICE_TYPE_CONSTANTS */
} uicc_ind_t;


/**
 * \name MAL_UICC_RESP
 * \par
 * Structures involved for uicc response indicating the uicc server status
 */

/**
 * \struct mal_uicc_resp_sim_status_t
 * \brief This Structure is the SIM server status
 * \par
 * \n The response data is filled and sent back to the user of uicc mal client using
 * \n the registered callback.
 */
typedef struct {
    uint8_t service_type;  /**< A subset of values from the constant table UICC_SERVICE_TYPE_CONSTANTS */
    uint8_t status;        /**< A subset of values from the constant table UICC_STATUS_CONSTANTS */
    uint8_t details;       /**< A subset of values from the constant table UICC_DETAILS_CONSTANTS */
    uint8_t server_status; /**< A subset of values from the constant table UICC_STATUS_CONSTANTS */
} mal_uicc_resp_sim_status_t;


/**
 * \name MAL_UICC_APPLICATION_REQ
 * \par
 * Involved in getting the application list and application host activation
 */

/**
 * \struct uicc_sb_client_t
 * \brief  This Structure corresponds to subblock UICC_SB_CLIENT
 */
typedef struct {
    uint8_t client_id; /**< Unique identifier for the Client */
} uicc_sb_client_t;

/**
 * \struct sb_application_t
 * \brief This Structure corresponds to Subblock: UICC_SB_APPLICATION
 */
typedef struct {
    uint8_t application_type; /**< Values from the constant table UICC_APPL_TYPE_TABLE */
    uint8_t appl_id;          /**< Unique identifier for the Application taken from
                             *   subblock UICC_SB_APPL_DATA_OBJECT
                             */
} sb_application_t;

/**
 * \struct sb_appl_info_t
 * \brief This Structure corresponds to Subblock: UICC_SB_APPL_INFO
 */
typedef struct {
    uint8_t strat_up_type; /**< Values from the constant table UICC_APPL_START_UP_TYPE_TABLE */
} sb_appl_info_t;

/**
 * \struct uicc_sb_aid_t
 * \brief This structure corresponds to subblock: UICC_SB_AID
 */
typedef struct {
    uint8_t   str_len; /**< String length */
    uint8_t   *str;    /**< Full AID or truncated AID (i.e. RID + App Code) as defined in ETSI TS
                        *   101.220  ASCII String, String is not zero-terminated
                        */
} uicc_sb_aid_t;

/**
 * \struct uicc_appl_shut_down_initiated_t
 * \brief Structure holding subblocks for service type MAL_UICC_APPL_SHUT_DOWN_INITIATED
 */
typedef struct {
    uicc_sb_client_t uicc_sb_client; /**< UICC_SB_CLIENT */
    sb_application_t sb_appln;       /**< UICC_SB_APPLICATION */
} uicc_appl_shut_down_initiated_t;

/**
 * \struct uicc_appln_host_activate_t
 * \brief Structure holding subblocks for service type MAL_UICC_APPL_HOST_ACTIVATE
 */
typedef struct {
    sb_application_t sb_appln;     /**< UICC_SB_APPLICATION */
    sb_appl_info_t   sb_appl_info; /**< UICC_SB_APPL_INFO */
    uicc_sb_aid_t    uicc_sb_aid;  /**< UICC_SB_AID */
} uicc_appln_host_activate_t;

/**
 * \struct uicc_appl_start_up_complete_t
 * \brief Structure holding subblocks for service type MAL_UICC_APPL_START_UP_COMPLETE
 */
typedef struct {
    uicc_sb_client_t uicc_sb_client; /**< UICC_SB_CLIENT */
    sb_application_t sb_appln;       /**< UICC_SB_APPLICATION */
} uicc_appl_start_up_complete_t;


/**
 * \struct uicc_appl_host_deactivate_t
 * \brief Structure holding subblocks for service type MAL_UICC_APPL_HOST_DEACTIVATE
 */
typedef struct {
    uicc_sb_client_t uicc_sb_client; /**< UICC_SB_CLIENT */
    sb_application_t sb_appln;       /**< UICC_SB_APPLICATION */
} uicc_appl_host_deactivate_t;

/**
 * \struct uicc_appl_status_get_t
 * \brief Structure holding subblocks for service type MAL_UICC_APPL_STATUS_GET
 */
typedef struct {
    sb_application_t sb_appln; /**< UICC_SB_APPLICATION */
} uicc_appl_status_get_t;

/**
 * \struct mal_uicc_appln_req_t
 * \brief Main Structure invloved in uicc application request
 * \par
 * \n To get application list and application host activation
 */
typedef struct {
    uint8_t service_type;        /**< A subset of values from the constant table  MAL_UICC_SERVICE_TYPE_CONSTANTS */
    uint8_t activate_last_appln; /**< flag check for having the activation of last application, 0 - NO, nonzero - YES */
    union {
        uicc_appl_shut_down_initiated_t uicc_appl_shut_down_initiated; /**< for service type MAL_UICC_APPL_SHUT_DOWN_INITIATED */
        uicc_appln_host_activate_t      appln_host_activate;           /**< for service type MAL_UICC_APPL_HOST_ACTIVATE */
        uicc_appl_start_up_complete_t   uicc_appl_start_up_complete;   /**< for service type MAL_UICC_APPL_START_UP_COMPLETE */
        uicc_appl_host_deactivate_t     uicc_appl_host_deactivate;     /**< for service type MAL_UICC_APPL_HOST_DEACTIVATE */
        uicc_appl_status_get_t          appl_status_get;               /**< for service type MAL_UICC_APPL_STATUS_GET */
    } sub_block;                                                     /**< union holding different structures corresponding
                                                                    *   to different service type requested
                                                                    */
    /* Subblock list:
       UICC_SB_AID
       UICC_SB_APPLICATION
       UICC_SB_APPL_INFO
       UICC_SB_CLIENT
     */
} mal_uicc_appln_req_t;


/**
 * \name MAL_UICC_APPLICATION_IND
 * \par
 * Structures involved in case of application access related indications.
 */

/**
 * \struct appl_activated_t
 * \brief Structure holding subblocks for service type MAL_UICC_APPL_ACTIVATED
 */
typedef struct {
    sb_application_t sb_appln; /**< UICC_SB_APPLICATION */
} appl_activated_t;

/**
 * \struct uicc_application_ind_t
 * \brief This main structure is applicable for the application related indications coming from modem
 */
typedef struct {
    uint8_t          service_type;   /**< A subset of values from the constant table UICC_SERVICE_TYPE_CONSTANTS */
    uint8_t          appl_id;        /**< Unique identifier for the Application taken from subblock
                                    *   UICC_SB_APPL_DATA_OBJECT
                                    */
    uint8_t          n_sb;           /**< No. of subblocks inside response data , Possible values: 0,1 */
    union {
        appl_activated_t appl_activated; /**< For service type:MAL_UICC_APPL_ACTIVATED */
    } sub_block_t;
} uicc_application_ind_t;


/**
 * \name MAL_UICC_APPLICATION_RESP
 * \par
 * Structures involved in response to uicc application request.
 * To get application list on SIM and application host activation.
 */

/**
 * \struct uicc_sb_appl_data_object_t
 * \brief This Structure corresponds to subblock UICC_SB_APPL_DATA_OBJECT
 */
typedef struct {
    uint8_t application_type; /**< Values from the constant table UICC_APPL_TYPE_TABLE */
    uint8_t appl_id;          /**< Unique identifier for the Application */
    uint8_t appl_status;      /**< A subset of values from the constant table UICC_STATUS_CONSTANTS */
    uint8_t appl_do_len;      /**< Byte string length */
    uint8_t *appl_do;         /**< Byte string */
} uicc_sb_appl_data_object_t;

/**
 * \struct uicc_sb_fci_t
 * \brief  This Structure corresponds to subblock UICC_SB_FCI
 */
typedef struct {
    uint16_t fci_length; /**< The length of the FCI in bytes */
    uint8_t  card_type;  /**< Values from the constant table UICC_CARD_TYPE_TABLE */
    uint8_t  *fci;       /**< File Control Info */
} uicc_sb_fci_t;

/**
 * \struct uicc_sb_chv_t
 * \brief  This Structure corresponds to subblock UICC_SB_CHV
 */
typedef struct {
    uint8_t chv_qualifier; /**< Values from the constant table UICC_CHV_QUALIFIER_TABLE */
    uint8_t pin_id;        /**< The PIN ID used for the CHV  */
} uicc_sb_chv_t;

/**
 * \struct uicc_appln_list_resp_t
 * \brief Structure holding subblocks for service type UICC_APPL_LIST
 */
typedef struct {
    uicc_sb_appl_data_object_t *uicc_sb_appl_data_object[MAX_APPLIST_SIZE]; /**< UICC_SB_APPL_DATA_OBJECT */
} uicc_appln_list_resp_t;

/**
 * \struct uicc_appln_host_activate_resp_t
 * \brief Structure holding subblocks for service type UICC_APPL_HOST_ACTIVATE
 */
typedef struct {
    uicc_sb_client_t uicc_sb_client;                  /**< UICC_SB_CLIENT */
    uicc_sb_fci_t    uicc_sb_fci;                     /**< UICC_SB_FCI
                                                     *   For UICC card type the data format is in ETSI TS 102.221
                                                     *   For ICC card type the data format is in 3GPP TS 51.011
                                                     */
    sb_application_t sb_application;                  /**< UICC_SB_APPLICATION, This subblock is present in
                                                     *   following cases
                                                     *   1. Activate application that was last active
                                                     *   2. Activate SIM application on ICC type smart card
                                                     *   3. Activation fails due to already active application
                                                     */
    uicc_sb_chv_t    *uicc_sb_chv[MAX_CHV_LIST_SIZE]; /**< UICC_SB_CHV, For Only ICC type smart card, 1 or 2
                                                     *   instances of this sub-blocks are present
                                                     */

} uicc_appln_host_activate_resp_t;

/**
 * \struct uicc_sb_appl_status_t
 * \brief  This Structure corresponds to subblock UICC_SB_APPL_STATUS
 */
typedef struct {
    uint8_t appl_status; /**< A subset of values from the constant table UICC_STATUS_CONSTANTS */
} uicc_sb_appl_status_t;

/**
 * \struct uicc_appl_status_get_resp_t
 * \brief Structure holding subblocks for service type MAL_UICC_APPL_STATUS_GET
 */
typedef struct {
    uicc_sb_appl_status_t uicc_sb_appl_status; /**< UICC_SB_APPL_STATUS */
} uicc_appl_status_get_resp_t;

/**
 * \struct mal_uicc_appln_resp_t
 * \brief This main structure is the response to application request
 * \par
 * \n The response data filled and sent back to the user of the UICC MAL client using the registered
 * \n callback.
 */
typedef struct {
    uint8_t   service_type; /**< A subset of values from the constant table UICC_SERVICE_TYPE_CONSTANTS */
    uint8_t   status;       /**< A subset of values from the constant table UICC_STATUS_CONSTANTS */
    uint8_t   details;      /**< A subset of values from the constant table UICC_DETAILS_CONSTANTS */
    uint8_t   card_type;    /**< Values from the constant table UICC_CARD_TYPE_TABLE */
    uint8_t   n_sb;         /**< No. of subblocks inside response data */
    union {
        uicc_appln_list_resp_t          uicc_appln_list;          /**< for service type UICC_APPL_LIST */
        uicc_appln_host_activate_resp_t uicc_appln_host_activate; /**< for service type
                                                               *   UICC_APPL_HOST_ACTIVATE
                                                               */
        uicc_appl_status_get_resp_t     uicc_appl_status_get;     /**< for service type MAL_UICC_APPL_STATUS_GET */
    } sub_block; /**< union holding different structures corresponding to different service type requested */
    /* Subblock list:
       UICC_SB_APPLICATION
       UICC_SB_APPL_DATA_OBJECT
       UICC_SB_APPL_STATUS
       UICC_SB_CHV
       UICC_SB_CLIENT
       UICC_SB_FCI
       UICC_SB_FCP
     */
} mal_uicc_appln_resp_t;


/**
 * \name MAL_UICC_CARD_REQ
 * \par
 * UICC card interface allows client to get the card status and card information
 */

/**
 * \struct uicc_card_req_t
 * \brief  This main structure is for UICC card interface giving the card status and information.
 */
typedef struct {
    uint8_t service_type; /**< A subset of values from the constant table UICC_SERVICE_TYPE_CONSTANTS */
    uint8_t card_type;    /**< Values from the constant table UICC_CARD_TYPE_TABLE */
} uicc_card_req_t;


/**
 * \name MAL_UICC_CARD_IND
 * \par
 * Structures involved in case of SIM detection related Indications.
 */

/**
 * \struct uicc_sb_card_info_t
 * \brief  This Structure corresponds to subblock UICC_SB_CARD_INFO
 */
typedef struct {
    uint8_t voltage_class; /**< Values from the constant table UICC_VOLTAGE_CLASS_TABLE */
    uint8_t clf_support;   /**< Values from the constant table UICC_CLF_IF_SUPPORT_TABLE */
} uicc_sb_card_info_t;

/**
 * \struct uicc_sb_card_reject_cause_t
 * \brief  This Structure corresponds to subblock UICC_SB_CARD_REJECT_CAUSE
 */
typedef struct {
    uint8_t reject_cause; /**< Values from the constant table UICC_CARD_REJECT_CAUSE_TABLE */
} uicc_sb_card_reject_cause_t;

/**
 * \struct uicc_sb_card_remove_cause_t
 * \brief  This Structure corresponds to subblock UICC_SB_CARD_REMOVE_CAUSE
 */
typedef struct {
    uint8_t remove_cause; /**< Values from the constant table UICC_CARD_REMOVE_CAUSE_TABLE */
} uicc_sb_card_remove_cause_t;

/**
 * \struct uicc_card_ind_t
 * \brief This main structure is applicable for the SIM detection related indications coming from modem
 */
typedef struct {
    uint8_t service_type; /**< A subset of values from the constant table UICC_SERVICE_TYPE_CONSTANTS */
    uint8_t card_type;    /**< Values from the constant table UICC_CARD_TYPE_TABLE */
    uint8_t n_sb;         /**< No. of subblocks inside response data, Possible values: 0,1 */
    union {
        uicc_sb_card_info_t         uicc_sb_card_info;         /**< For service type: UICC_CARD_READY */
        uicc_sb_card_reject_cause_t uicc_sb_card_reject_cause; /**< For service type: UICC_CARD_REJECTED */
        uicc_sb_card_remove_cause_t uicc_sb_card_remove_cause; /**< For service type: UICC_CARD_REMOVED */
    } sub_block_t;
} uicc_card_ind_t;


/**
 * \name MAL_UICC_CARD_RESP
 * \par
 * Structures involved in response to uicc card request
 * To get the card status
 */

/**
 * \struct uicc_sb_card_status_t
 * \brief This Structure corresponds to subblock UICC_SB_CARD_STATUS
 */
typedef struct {
    uint8_t card_status; /**< A subset of values from the constant table UICC_STATUS_CONSTANTS */
} uicc_sb_card_status_get_t;

/**
 * \struct uicc_sb_card_status_t
 * \brief This Structure corresponds to subblock UICC_SB_CARD_INFO
 */
typedef struct {
    uint8_t voltage_class; /**< Values from the constant table UICC_VOLTAGE_CLASS_TABLE */
    uint8_t clf_support;   /**< Values from the constant table UICC_CLF_IF_SUPPORT_TABLE */
} uicc_sb_card_info_get_t;

/**
 * \struct mal_uicc_card_resp_t
 * \brief This Structure is the SIM Card status response
 * \par
 * \n The response data filled and sent back to the user of UICC mal client using
 * \n the registered callback.
 * \n USE CASE mapped RIL_REQUEST_GET_SIM_STATUS
 */
typedef struct {
    uint8_t service_type; /**< A subset of values from the constanttable UICC_SERVICE_TYPE_CONSTANTS*/
    uint8_t status;       /**< A subset of values from the constant table UICC_STATUS_CONSTANTS */
    uint8_t details;      /**< A subset of values from the constant table UICC_DETAILS_CONSTANTS */
    uint8_t card_type;    /**< Values from the constant table UICC_CARD_TYPE_TABLE */
    uint8_t n_sb;         /**< No. of subblocks inside response data, Possible values: 0,1 */
    union {
        uicc_sb_card_status_get_t uicc_sb_card_status_get; /**< For Service type: UICC_CARD_STATUS */
        uicc_sb_card_info_get_t   uicc_sb_card_info_get;   /**< For Service type: UICC_CARD_INFO_GET */
    } sub_block_t;
} mal_uicc_card_resp_t;


/**
 * \name MAL_UICC_APPL_CMD_REQ
 * \par
 * Structures involved in uicc application command request for SIM IO related features.
 */

/**
 * \struct uicc_sb_appl_path_t
 * \brief  This Structure corresponds to subblock UICC_SB_APPL_PATH
 */
typedef struct {
    uint16_t ef;          /**< The ID of the Elementary File */
    uint8_t  sfi;         /**< The Elementary Files Short File Identifier  */
    uint8_t  path_length; /**< The length of the Dedicated File path in bytes */
    uint8_t  *path;       /**<  DF path starting from MF (3F00), coded as defined in ETSI TS 102.221 */
} uicc_sb_appl_path_t;

/**
 * \struct appl_session_start_req_t
 * \brief  Subblocks involved for service type: UICC_APPL_SESSION_START
 */
typedef struct {
    uicc_sb_appl_path_t uicc_sb_appl_path; /**< Subblock list:UICC_SB_APPL_PATH */
} appl_session_start_req_t;

/**
 * \struct uicc_sb_resp_info_t
 * \brief  This Structure corresponds to subblock UICC_SB_RESP_INFO
 */
typedef struct {
    uint8_t response_info; /**< Values from the bitmask table UICC_RESP_INFO_BIT_TABLE */
} uicc_sb_resp_info_t;

/**
 * \struct read_transparent_t
 * \brief  Subblocks involved for service type: UICC_APPL_READ_TRANSPARENT
 */
typedef struct {
    uint16_t ef;          /**< The ID of the Elementary File */
    uint8_t  sfi;         /**< The Elementary Files Short File Identifier */
    uint8_t  path_length; /**< The length of the Dedicated File path in bytes DF path starting
                         *   from MF (3F00), coded as defined in ETSI TS 102.221
                         */
    uint8_t  *path;       /**< DF (Dedicated file) path eg:- 0xMF/DF/EF */
    /**< For subblock UICC_SB_APPL_PATH */

    uint16_t offset; /**< The offset in the file (0 means beginning of file) */
    uint16_t amount; /**< The amount of data to read in bytes (0 means read until end of file) */
    /**< For subblock UICC_SB_TRANSPARENT */

    uint8_t client_id; /**< Unique identifier for the Client */
    /**< For subblock UICC_SB_CLIENT */

    uicc_sb_resp_info_t  sb_resp_info; /**< subblock UICC_SB_RESP_INFO */
} read_transparent_t;

/**
 * \struct update_transparent_t
 * \brief  Subblocks involved for service type: UICC_APPL_UPDATE_TRANSPARENT
 */
typedef struct {
    read_transparent_t read_transp; /**<  Same subblocks used as in SIMIO read transparent feature */
    data_t             data;        /**< Structure holding data length and the data of that length
                                   *   to be updated
                                   */
    uint8_t client_id; /**< Unique identifier for the Client */
    /**< For subblock UICC_SB_CLIENT */
    uicc_sb_resp_info_t  sb_resp_info; /**< subblock UICC_SB_RESP_INFO */
} update_transparent_t;

/**
 * \struct read_linearfixed_t
 * \brief  Subblocks involved for service type: UICC_APPL_READ_LINEAR_FIXED
 */
typedef struct {
    uint16_t ef;          /**< The ID of the Elementary File */
    uint8_t  sfi;         /**< The Elementary Files Short File Identifier */
    uint8_t  path_length; /**< The length of the Dedicated File path in bytes DF path starting
                         *   from MF (3F00), coded as defined in ETSI TS 102.221
                         */
    uint8_t  *path;       /**< DF (Dedicated file) path eg:- 0xMF/DF/EF */
    /**< For subblock UICC_SB_APPL_PATH */

    uint8_t  record; /**< The record in the file */
    uint8_t  offset; /**< The offset in the record (0 means beginning of record) */
    uint8_t  amount; /**< FIELD ONLY USED FOR READING: The amount of data in bytes
                    *   (0 means read until end of record)
                    */
    /**< For subblock UICC_SB_LINEAR_FIXED */

    uint8_t client_id; /**< Unique identifier for the Client */
    /**< For subblock UICC_SB_CLIENT */
    uicc_sb_resp_info_t  sb_resp_info; /**< subblock UICC_SB_RESP_INFO */
} read_linearfixed_t;

/**
 * \struct update_linearfixed_t
 * \brief  Subblocks involved for service type: UICC_APPL_UPDATE_LINEAR_FIXED
 */
typedef struct {
    read_linearfixed_t read_linearfixed; /**, Same subblocks used as in SIMIO read linearfixed feature */
    data_t             data;             /**< Structure holding data length and the data of that length
                                        *   to be updated
                                        */
    uint8_t client_id; /**< Unique identifier for the Client */
    /**< For subblock UICC_SB_CLIENT */
    uicc_sb_resp_info_t  sb_resp_info; /**< subblock UICC_SB_RESP_INFO */
} update_linearfixed_t;

/**
 * \struct uicc_sb_cyclic_t
 * \brief  This Structure corresponds to subblock UICC_SB_CYCLIC
 */
typedef struct {

    uint8_t record; /**< The record in the file.
                   *   Note: In case of update it is only possible to write to record 1
                   */
    uint8_t offset; /**< The offset in the record (0 means beginning of record) */
    uint8_t amount; /**< FIELD ONLY USED FOR READING: The amount of data in bytes (0 means read
                   *   until end of record)
                   */
} uicc_sb_cyclic_t;

/**
 * \struct appl_read_cyclic_req_t
 * \brief  Subblocks involved for service type: UICC_APPL_READ_CYCLIC
 */
typedef struct {
    uicc_sb_appl_path_t uicc_sb_appl_path; /**< Subblock list:UICC_SB_APPL_PATH */
    uicc_sb_client_t    uicc_sb_client;    /**< UICC_SB_CLIENT */
    uicc_sb_cyclic_t    uicc_sb_cyclic;    /**< UICC_SB_CYCLIC */
    uicc_sb_resp_info_t sb_resp_info;      /**< subblock UICC_SB_RESP_INFO */
} appl_read_cyclic_req_t;

/**
 * \struct uicc_sb_file_data_t
 * \brief  This Structure corresponds to subblock UICC_SB_FILE_DATA
 */
typedef struct {
    uint32_t data_length; /**< The length of the file data in bytes */
    uint8_t  *data;       /**< File data */
} uicc_sb_file_data_t;

/**
 * \struct appl_update_cyclic_req_t
 * \brief  Subblocks involved for service type: UICC_APPL_UPDATE_CYCLIC
 */
typedef struct {
    uicc_sb_appl_path_t uicc_sb_appl_path; /**< Subblock list:UICC_SB_APPL_PATH */
    uicc_sb_client_t    uicc_sb_client;    /**< UICC_SB_CLIENT */
    uicc_sb_cyclic_t    uicc_sb_cyclic;    /**< UICC_SB_CYCLIC */
    uicc_sb_file_data_t uicc_sb_file_data; /**< UICC_SB_FILE_DATA */
    uicc_sb_resp_info_t sb_resp_info;      /**< subblock UICC_SB_RESP_INFO */
} appl_update_cyclic_req_t;

/**
 * \struct appl_clear_cache_req_t
 * \brief  Subblocks involved for service type: UICC_APPL_CLEAR_CACHE
 */
typedef struct {
    uicc_sb_appl_path_t uicc_sb_appl_path;   /**< Subblock list:UICC_SB_APPL_PATH */
} appl_clear_cache_req_t;

/**
 * \struct uicc_sb_apdu_t
 * \brief  This Structure corresponds to subblock UICC_SB_APDU
 */
typedef struct {
    uint8_t  cmd_force;   /**< Values from the constant table UICC_APDU_CMD_FORCE_TYPE_TABLE */
    uint16_t apdu_length; /**< The length of the APDU data in bytes */
    uint8_t  *apdu;       /**< APDU */
} uicc_sb_apdu_t;

/**
 * \struct appl_apdu_send_req_t
 * \brief  Subblocks involved for service type: UICC_APPL_APDU_SEND
 */
typedef struct {
    uicc_sb_appl_path_t uicc_sb_appl_path; /**< Subblock list:UICC_SB_APPL_PATH */
    uicc_sb_client_t    uicc_sb_client;    /**< UICC_SB_CLIENT */
    uicc_sb_apdu_t      uicc_sb_apdu;      /**< UICC_SB_APDU */
} appl_apdu_send_req_t;

/**
 * \struct uicc_appl_file_info_t
 * \brief  Subblocks involved for service type: UICC_APPL_FILE_INFO
 */
typedef struct {
    uint16_t ef;          /**< The ID of the Elementary File */
    uint8_t  sfi;         /**< The Elementary Files Short File Identifier */
    uint8_t  path_length; /**< The length of the Dedicated File path in bytes DF path
                         *   starting from MF (3F00), coded as defined in ETSI TS 102.221
                         */
    uint8_t  *path;       /**< DF (Dedicated file) path eg:- 0xMF/DF/EF */
    /**< For subblock UICC_SB_APPL_PATH */
    uicc_sb_resp_info_t sb_resp_info; /**< subblock UICC_SB_RESP_INFO */
} uicc_appl_file_info_t;

/**
 * \struct uicc_appl_cmd_req_t
 * \brief This main Structure is for SIM IO, read or update or get file info of specific EF
 */
typedef struct {
    uint8_t service_type; /**< A subset of values from the constant table UICC_SERVICE_TYPE_CONSTANTS */
    uint8_t appl_id;      /**< Unique identifier for the Application */
    uint8_t session_id;   /**< Unique identifier for file access session */
    union {
        appl_session_start_req_t appl_session_start_req; /**< for service type UICC_APPL_SESSION_START */
        read_transparent_t       ru_transp;              /**< for service type UICC_APPL_READ_TRANSPARENT */
        read_linearfixed_t       sb_linearfixed;         /**< for service type UICC_APPL_READ_LINEAR_FIXED */
        uicc_appl_file_info_t    uicc_appl_file_info;    /**< for service type UICC_APPL_FILE_INFO */
        update_transparent_t     update_transp;          /**< for service type UICC_APPL_UPDATE_TRANSPARENT */
        update_linearfixed_t     update_linearfixed;     /**< for service type UICC_APPL_UPDATE_LINEAR_FIXED */
        appl_read_cyclic_req_t   appl_read_cyclic_req;   /**< for service type UICC_APPL_READ_CYCLIC */
        appl_update_cyclic_req_t appl_update_cyclic_req; /**< for service type UICC_APPL_UPDATE_CYCLIC */
        appl_clear_cache_req_t   appl_clear_cache_req;   /**< for service type UICC_APPL_CLEAR_CACHE */
        appl_apdu_send_req_t     appl_apdu_send_req;     /**< for service type UICC_APPL_APDU_SEND */
    } sub_block_t; /**< union holding different structures corresponding to different service type requested */
} uicc_appl_cmd_req_t;


/**
 * \name MAL_UICC_APPL_CMD_RESP
 * \par
 * Structures involved in response to uicc application command request for SIM IO related features.
 */

/**
 * \struct uicc_sb_session_t
 * \brief This Structure corresponds to subblock UICC_SB_SESSION
 */
typedef struct {
    uint8_t   session_id; /**< Unique identifier for the file access session */
} uicc_sb_session_t;


/**
 * \struct appl_session_start_resp_t
 * \brief  Subblocks involved for service type: UICC_APPL_SESSION_START
 */
typedef struct {
    uicc_sb_session_t uicc_sb_session; /**< Subblock list:UICC_SB_APPLICATION */
} appl_session_start_resp_t;

/**
 * \struct appl_apdu_send_resp_t
 * \brief  Subblocks involved for service type: UICC_APPL_APDU_SEND
 */
typedef struct {
    uicc_sb_apdu_t      uicc_sb_apdu; /**< UICC_SB_APDU */
} appl_apdu_send_resp_t;

/**
 * \struct uicc_sb_status_word_t
 * \brief  This Structure corresponds to subblock UICC_SB_STATUS_WORD
 */
typedef struct {
    uint8_t sw1; /**< Response APDU Status byte 1 */
    uint8_t sw2; /**< Response APDU Status byte 2 */
} uicc_sb_status_word_t;

/**
 * \struct uicc_sb_pin_ref_t
 * \brief  This Structure corresponds to subblock UICC_SB_PIN_REF
 */
typedef struct {
    uint8_t   pin_id; /**< The PIN ID taken from the Application FCP taken from the subblock
                     *   UICC_SB_FCP / UICC_SB_FCI or the PIN ID from the subblock UICC_SB_CHV
                     */
} uicc_sb_pin_ref_t;

/**
 * \struct appl_file_read_resp_t
 * \brief  Subblocks involved for service type: UICC_APPL_READ_TRANSPARENT
 * \n      /UICC_APPL_READ_LINEAR_FIXED/UICC_APPL_READ_CYCLIC
 */
typedef struct {
    data_t                data;                /**< UICC_SB_FILE_DATA */
    uicc_sb_status_word_t uicc_sb_status_word; /**< UICC_SB_STATUS_WORD:
                                              *   Present if UICC_SB_RESP_INFO subblock,
                                              *   UICC_RESP_INFO_STATUS_WORD bit set in request
                                              */
    uicc_sb_pin_ref_t     uicc_sb_pin_ref;     /**< UICC_SB_PIN_REF:
                                              *   Present if UICC_STATUS_FAIL &&
                                              *   UICC_SECURITY_STATUS_NOT_SATISFIED
                                              */
} appl_file_read_resp_t;

/**
 * \struct appl_file_update_resp_t
 * \brief  Subblocks involved for service type: UICC_APPL_UPDATE_TRANSPARENT
 * \n      /UICC_APPL_UPDATE_LINEAR_FIXED/UICC_APPL_UPDATE_CYCLIC
 */
typedef struct {
    uicc_sb_status_word_t uicc_sb_status_word; /**< UICC_SB_STATUS_WORD
                                              *   Present if UICC_SB_RESP_INFO subblock,
                                              *   UICC_RESP_INFO_STATUS_WORD bit set in request
                                              */
    uicc_sb_pin_ref_t     uicc_sb_pin_ref;     /**< UICC_SB_PIN_REF
                                              *   Present if UICC_STATUS_FAIL &&
                                              *   UICC_SECURITY_STATUS_NOT_SATISFIED
                                              */
} appl_file_update_resp_t;

/**
 * \struct appl_file_info_resp_t
 * \brief  Subblocks involved for service type: UICC_APPL_FILE_INFO
 */
typedef struct {
    uicc_sb_fci_t         uicc_sb_fci;         /**< UICC_SB_FCI */
    uicc_sb_status_word_t uicc_sb_status_word; /**< UICC_SB_STATUS_WORD
                                              *   Present if UICC_SB_RESP_INFO subblock,
                                              *   UICC_RESP_INFO_STATUS_WORD bit set in request
                                              */
} appl_file_info_resp_t;


/**
 * \struct uicc_appl_cmd_resp_t
 * \brief This main structure is applicable for the responses to the SIM IO related requests
 * \par
 * \n For read or update or file info of a specific EF
 */
typedef struct {
    uint8_t service_type; /**< A subset of values from the constant table UICC_SERVICE_TYPE_CONSTANTS */
    uint8_t status;       /**< A subset of values from the constant table UICC_STATUS_CONSTANTS */
    uint8_t details;      /**< A subset of values from the constant table UICC_DETAILS_CONSTANTS */
    uint8_t card_type;    /**< Values from the constant table UICC_CARD_TYPE_TABLE */
    uint8_t n_sb;         /**< No. of subblocks inside response data */
    union {
        appl_file_read_resp_t     appl_file_read_resp;     /**< for service type UICC_APPL_READ_TRANSPARENT
                                                        *   /UICC_APPL_READ_LINEAR_FIXED/UICC_APPL_READ_CYCLIC
                                                        */
        appl_file_update_resp_t   appl_file_update_resp;   /**< for service type UICC_APPL_UPDATE_TRANSPARENT
                                                        *   /UICC_APPL_UPDATE_LINEAR_FIXED/UICC_APPL_UPDATE_CYCLIC
                                                        */
        appl_file_info_resp_t     appl_file_info_resp;     /**< for service type UICC_APPL_APDU_SEND */
        appl_session_start_resp_t appl_session_start_resp; /**< for service type UICC_APPL_SESSION_START */
        appl_apdu_send_resp_t     appl_apdu_send_resp;     /**< for service type UICC_APPL_APDU_SEND */
    } sub_block; /**< union holding different structures corresponding to different service type requested */
} uicc_appl_cmd_resp_t;


/**
 * \name MAL_UICC_PIN_IND
 * \par
 * Structures involved in case of PIN Indications
 */

/**
 * \struct uicc_pin_ind_t
 * \brief This main structure is applicable for the PIN indications coming from modem
 */
typedef struct {
    uint8_t service_type; /**< service type eg:- UICC_PIN_VERIFY_NEEDED */
    uint8_t pin_id;       /**< The PIN ID taken from the Application FCP taken from the subblock
                         *   UICC_SB_FCP / UICC_SB_FCI or the PIN ID from the subblock UICC_SB_CHV
                         */
    uint8_t appl_id;      /**< Unique identifier for the Application taken from subblock
                         *   UICC_SB_APPL_DATA_OBJECT
                         */
} uicc_pin_ind_t;


/**
 * \name MAL_UICC_PIN_REQ
 * \par
 * Structures involved in uicc pin request for PIN or PUK related features.
 */

/**
 * \struct uicc_sb_pin_t
 * \brief  This Structure corresponds to subblock UICC_SB_PIN
 */
typedef struct {
    uint8_t pin_id;        /**< The PIN ID taken from the Application FCP taken from the subblock
                          *   UICC_SB_FCP / UICC_SB_FCI or the PIN ID from the subblock UICC_SB_CHV
                          */
    uint8_t pin_qualifier; /**< Values from the constant table UICC_PIN_QUALIFIER_TABLE */
    uint8_t length;        /**< PIN code length */
    char    pin_code[8];   /**< PIN code */
} uicc_sb_pin_t;

/**
 * \struct uicc_sb_pin_susbt_t
 * \brief  This Structure corresponds to subblock UICC_SB_PIN_SUBST
 */
typedef struct {
    uint8_t pin_id1; /**< The PIN ID taken from the Application FCP taken from the subblock
                    *   UICC_SB_FCP / UICC_SB_FCI
                    */
    uint8_t pin_id2; /**< The PIN ID taken from the Application FCP taken from the subblock
                    *   UICC_SB_FCP / UICC_SB_FCI
                    */
} uicc_sb_pin_susbt_t;

/**
 * \struct uicc_sb_puk_t
 * \brief  This Structure corresponds to subblock UICC_SB_PUK
 */
typedef struct {
    uint8_t pin_id;      /**< The PIN ID taken from the Application FCP taken from the subblock
                        *   UICC_SB_FCP / UICC_SB_FCI or the PIN ID from the subblock UICC_SB_CHV
                        */
    uint8_t puk_length;  /**< PUK code length */
    char    puk_code[8]; /**< PUK code */
} uicc_sb_puk_t;

/**
 * \struct uicc_pin_enable_req_t
 * \brief Structure holding subblocks for service type UICC_PIN_ENABLE
 */
typedef struct {
    uicc_sb_pin_t pin_uicc_sb_pin; /**< subblock: UICC_SB_PIN */
} uicc_pin_enable_req_t;

/**
 * \struct uicc_pin_disable_req_t
 * \brief Structure holding subblocks for service type UICC_PIN_DISABLE
 */
typedef struct {
    uicc_sb_pin_t pin_uicc_sb_pin; /**< subblock: UICC_SB_PIN */
} uicc_pin_disable_req_t;

/**
 * \struct uicc_pin_verify_req_t
 * \brief Structure holding subblocks for service type UICC_PIN_VERIFY
 */
typedef struct {
    uicc_sb_pin_t pin_uicc_sb_pin; /**< subblock: UICC_SB_PIN */
} uicc_pin_verify_req_t;

/**
 * \struct uicc_pin_change_req_t
 * \brief Structure holding subblocks for service type UICC_PIN_CHANGE
 */
typedef struct {
    uicc_sb_pin_t pin_uicc_sb_pin_old; /**< subblock: UICC_SB_PIN */
    uicc_sb_pin_t pin_uicc_sb_pin_new; /**< subblock: UICC_SB_PIN */
} uicc_pin_change_req_t;

/**
 * \struct uicc_pin_substitute_req_t
 * \brief Structure holding subblocks for service type UICC_PIN_SUBSTITUTE
 */
typedef struct {
    uicc_sb_pin_t       uicc_sb_pin;       /**< subblock: UICC_SB_PIN */
    uicc_sb_pin_susbt_t uicc_sb_pin_susbt; /**< subblock: UICC_SB_PIN_SUBST */
} uicc_pin_substitute_req_t;

/**
 * \struct uicc_pin_info_req_t
 * \brief Structure holding subblocks for service type UICC_PIN_INFO
 */
typedef struct {
    uicc_sb_pin_ref_t pin_uicc_sb_ref; /**< subblock: UICC_SB_PIN_REF */
} uicc_pin_info_req_t;

/**
 * \struct uicc_sb_access_mode_t
 * \brief  This Structure corresponds to subblock UICC_SB_ACCESS_MODE
 */
typedef struct {
    uint8_t access_mode; /**< Values from the constant table UICC_ACCESS_MODE_TABLE */
} uicc_sb_access_mode_t;


/**
 * \struct uicc_pin_prompt_verify_req_t
 * \brief Structure holding subblocks for service type UICC_PIN_PROMPT_VERIFY
 */
typedef struct {
    uicc_sb_pin_ref_t     uicc_sb_pin_ref; /**< subblock: UICC_SB_PIN_REF */
    uicc_sb_access_mode_t sb_access_mode;  /**< subblock: UICC_SB_ACCESS_MODE */
    uicc_sb_appl_path_t   sb_appl_path;    /**< Subblock list:UICC_SB_APPL_PATH */
} uicc_pin_prompt_verify_req_t;

/**
 * \struct uicc_pin_unblock_req_t
 * \brief Structure holding subblocks for service type UICC_PIN_UNBLOCK
 */
typedef struct {
    uicc_sb_pin_t pin_uicc_sb_pin; /**< subblock: UICC_SB_PIN */
    uicc_sb_puk_t pin_uicc_sb_puk; /**< subblock: UICC_SB_PUK */
} uicc_pin_unblock_req_t;

/**
 * \struct uicc_pin_req_t
 * \brief This main Structure is for PIN/PUK related feature requests.
 */
typedef struct {
    uint8_t service_type; /**< A subset of values from the constant table UICC_SERVICE_TYPE_CONSTANTS */
    uint8_t appl_id;      /**< Unique identifier for the Application taken from subblock
                         *   UICC_SB_APPL_DATA_OBJECT
                         */
    union {
        uicc_pin_enable_req_t        uicc_pin_enable_req;        /**< for service type UICC_PIN_ENABLE */
        uicc_pin_disable_req_t       uicc_pin_disable_req;       /**< for service type UICC_PIN_DISABLE */
        uicc_pin_verify_req_t        uicc_pin_verify_req;        /**< for service type UICC_PIN_VERIFY */
        uicc_pin_change_req_t        uicc_pin_change_req;        /**< for service type UICC_PIN_CHANGE */
        uicc_pin_substitute_req_t    uicc_pin_substitute_req;    /**< for service type UICC_PIN_SUBSTITUTE */
        uicc_pin_info_req_t          uicc_pin_info_req;          /**< for service type UICC_PIN_INFO */
        uicc_pin_prompt_verify_req_t uicc_pin_prompt_verify_req; /**< for service type UICC_PIN_PROMPT_VERIFY */
        uicc_pin_unblock_req_t       uicc_pin_unblock_req;       /**< for service type UICC_PIN_UNBLOCK */
    } sub_block_t; /**< union holding different structures corresponding to different service type requested */
    /* Subblock list:
       UICC_SB_APPLICATION
       UICC_SB_PIN
       UICC_SB_PIN_REF
       UICC_SB_PIN_SUBST
       UICC_SB_PUK
     */
} uicc_pin_req_t;


/**
 * \name MAL_UICC_PIN_RESP
 * \par
 * Structures involved in responses to uicc pin request for PIN or PUK related features.
 */

/**
 * \struct uicc_sb_pin_info_t
 * \brief  This Structure corresponds to subblock UICC_SB_PIN_INFO
 */
typedef struct {
    uint8_t pin_status; /**< A subset of values from the constant table UICC_STATUS_CONSTANTS */
    uint8_t pin_att;    /**< PIN attempts left */
    uint8_t puk_att;    /**< PUK attempts left */
} uicc_sb_pin_info_t;

/**
 * \struct uicc_pin_enable_resp_t
 * \brief Structure holding subblocks for service type UICC_PIN_ENABLE
 */
typedef struct {
    uicc_sb_status_word_t pin_uicc_sb_status_word; /**< subblock: UICC_SB_STATUS_WORD */
} uicc_pin_enable_resp_t;

/**
 * \struct uicc_pin_disable_resp_t
 * \brief Structure holding subblocks for service type UICC_PIN_DISABLE
 */
typedef struct {
    uicc_sb_status_word_t pin_uicc_sb_status_word; /**< subblock: UICC_SB_STATUS_WORD */
} uicc_pin_disable_resp_t;

/**
 * \struct uicc_pin_verify_resp_t
 * \brief Structure holding subblocks for service type UICC_PIN_VERIFY
 */
typedef struct {
    uicc_sb_status_word_t pin_uicc_sb_status_word; /**< subblock: UICC_SB_STATUS_WORD */
} uicc_pin_verify_resp_t;

/**
 * \struct uicc_pin_change_resp_t
 * \brief Structure holding subblocks for service type UICC_PIN_CHANGE
 */
typedef struct {
    uicc_sb_status_word_t pin_uicc_sb_status_word; /**< subblock: UICC_SB_STATUS_WORD */
} uicc_pin_change_resp_t;

/**
 * \struct uicc_pin_substitute_resp_t
 * \brief Structure holding subblocks for service type UICC_PIN_SUBSTITUTE
 */
typedef struct {
    uicc_sb_status_word_t pin_uicc_sb_status_word; /**< subblock: UICC_SB_STATUS_WORD */
} uicc_pin_substitute_resp_t;


/**
 * \struct uicc_pin_unblock_resp_t
 * \brief Structure holding subblocks for service type UICC_PIN_UNBLOCK
 */
typedef struct {
    uicc_sb_status_word_t pin_uicc_sb_status_word; /**< subblock: UICC_SB_STATUS_WORD */
} uicc_pin_unblock_resp_t;

/**
 * \struct uicc_pin_info_resp_t
 * \brief Structure holding subblocks for service type UICC_PIN_INFO
 */
typedef struct {
    uicc_sb_pin_info_t pin_uicc_sb_pin_info; /**< subblock: UICC_SB_PIN_INFO */
} uicc_pin_info_resp_t;

/**
 * \struct uicc_pin_resp_t
 * \brief This main structure is for PIN/PUK related feature responses to the corresponding requests.
 */
typedef struct {
    uint8_t service_type; /**< A subset of values from the constant table UICC_SERVICE_TYPE_CONSTANTS */
    uint8_t status;       /**< A subset of values from the constant table UICC_STATUS_CONSTANTS */
    uint8_t details;      /**< A subset of values from the constant table UICC_DETAILS_CONSTANTS */
    uint8_t card_type;    /**< Values from the constant table UICC_CARD_TYPE_TABLE */
    uint8_t n_sb;         /**< No. of subblocks inside response data */
    union {
        uicc_pin_enable_resp_t      uicc_pin_enable_resp;      /**< for service type UICC_PIN_ENABLE */
        uicc_pin_disable_resp_t     uicc_pin_disable_resp;     /**< for service type UICC_PIN_DISABLE */
        uicc_pin_verify_resp_t      uicc_pin_verify_resp;      /**< for service type UICC_PIN_VERIFY */
        uicc_pin_change_resp_t      uicc_pin_change_resp;      /**< for service type UICC_PIN_CHANGE */
        uicc_pin_substitute_resp_t  uicc_pin_substitute_resp;  /**< for service type UICC_PIN_SUBSTITUTE */
        uicc_pin_info_resp_t        uicc_pin_info_resp;        /**< for service type UICC_PIN_INFO */
        uicc_pin_unblock_resp_t     uicc_pin_unblock_resp;     /**< for service type UICC_PIN_UNBLOCK */
    } sub_block_t; /**< union holding different structures corresponding to different service type requested */
    /* Subblock list:
       UICC_SB_PIN_INFO
       UICC_SB_STATUS_WORD
     */
} uicc_pin_resp_t;


/**
 * \name MAL_UICC_CAT_REQ
 * \par
 * Structures involved in uicc cat request for STK related features.
 */

/**
 * \struct uicc_sb_terminal_profile_t
 * \brief  This Structure corresponds to subblock UICC_SB_TERMINAL_PROFILE
 */
typedef struct {
    uint16_t tp_length; /**< The length of the TERMINAL PROFILE data in bytes */
    uint8_t  *tp;       /**< The TERMINAL PROFILE data */
} uicc_sb_terminal_profile_t;

/**
 * \struct uicc_sb_transparent_t
 * \brief  This Structure corresponds to subblock UICC_SB_TRANSPARENT
 */
typedef struct {
    uint16_t offset; /**< The offset in the file (0 means beginning of file) */
    uint16_t amount; /**< FIELD ONLY USED FOR READING: The amount of data to read in bytes (0
                    *   means read until end of file)
                    */
} uicc_sb_transparent_t;

/**
 * \struct uicc_cat_terminal_profile_t
 * \brief Structure holding subblocks for service type UICC_CAT_TERMINAL_PROFILE
 */
typedef struct {
    uicc_sb_terminal_profile_t uicc_sb_terminal_profile;           /**< subblock: UICC_SB_TERMINAL_PROFILE */
    uint8_t                    update_terminal_support_table_flag; /**< 0 - Disable, 1 - Enable */
    uicc_sb_appl_path_t        uicc_sb_appl_path;                  /**< subblock: UICC_SB_APPL_PATH */
    uicc_sb_transparent_t      uicc_sb_transparent;                /**< subblock: UICC_SB_TRANSPARENT */
    uicc_sb_file_data_t        uicc_sb_file_data;                  /**< subblock: UICC_SB_FILE_DATA */
} uicc_cat_terminal_profile_t;

/**
 * \struct uicc_sb_terminal_response_t
 * \brief  This Structure corresponds to subblock UICC_SB_TERMINAL_RESPONSE
 */
typedef struct {
    uint16_t  tr_length; /**< The length of the TERMINAL RESPONSE data in bytes */
    uint8_t   *tr;       /**< TERMINAL RESPONSE data */
} uicc_sb_terminal_response_t;

/**
 * \struct uicc_cat_terminal_response_t
 * \brief Structure holding subblocks for service type UICC_CAT_TERMINAL_RESPONSE
 */
typedef struct {
    uicc_sb_terminal_response_t uicc_sb_terminal_response; /**< subblock: UICC_SB_TERMINAL_RESPONSE */
} uicc_cat_terminal_response_t;

/**
 * \struct uicc_sb_envelope_t
 * \brief  This Structure corresponds to subblock UICC_SB_ENVELOPE
 */
typedef struct {
    uint16_t  envelope_length; /**< The length of the ENVELOPE data in bytes */
    uint8_t   *envelope;       /**< ENVELOPE data */
} uicc_sb_envelope_t;

/**
 * \struct uicc_cat_envelope_t
 * \brief Structure holding subblocks for service type UICC_CAT_ENVELOPE
 */
typedef struct {
    uicc_sb_envelope_t uicc_sb_envelope; /**< subblock: UICC_SB_ENVELOPE */
} uicc_cat_envelope_t;

/**
 * \struct uicc_sb_polling_set_t
 * \brief  This Structure corresponds to subblock UICC_SB_POLLING_SET
 */
typedef struct {
    uint16_t  poll_int; /**< The polling interval in seconds (0 means proactive polling off) */
} uicc_sb_polling_set_t;

/**
 * \struct uicc_cat_polling_set_t
 * \brief Structure holding subblocks for service type UICC_CAT_POLLING_SET
 */
typedef struct {
    uicc_sb_polling_set_t uicc_sb_polling_set; /**< subblock: UICC_SB_POLLING_SET */
} uicc_cat_polling_set_t;

/**
 * \struct uicc_sb_refresh_t
 * \brief  This Structure corresponds to subblock UICC_SB_REFRESH
 */
typedef struct {
    uint8_t type; /**< Values from the constant table UICC_REFRESH_TYPE_CONSTANTS */
} uicc_sb_refresh_t;

/**
 * \struct uicc_cat_refresh_t
 * \brief Structure holding subblocks for service type UICC_CAT_REFRESH
 */
typedef struct {
    uicc_sb_refresh_t    uicc_sb_refresh;                   /**< SB: UICC_SB_REFRESH */
    uint8_t              is_aid_present;                    /**< AID to be provided or not */
    uicc_sb_aid_t        uicc_sb_aid;             /**< SB: UICC_SB_AID, None or 1 instance of this sub-block needed */
    uint8_t              nb_appl_path;                      /**< No. of SB's of APPL_PATH needed provided by MAL client user */
    uicc_sb_appl_path_t  *uicc_sb_appl_path[MAX_APPL_PATH]; /**< SB: UICC_SB_APPL_PATH, None or more instances of this sub-block needed */
} uicc_cat_refresh_t;

/**
 * \struct uicc_cat_req_t
 * \brief This Structure is for CAT Server Requests
 */
typedef struct {
    uint8_t service_type; /**< service type */
    union {
        uicc_cat_terminal_profile_t  uicc_cat_terminal_profile;  /**< for service type UICC_CAT_TERMINAL_PROFILE */
        uicc_cat_terminal_response_t uicc_cat_terminal_response; /**< for service type UICC_CAT_TERMINAL_RESPONSE */
        uicc_cat_envelope_t          uicc_cat_envelope;          /**< for service type UICC_CAT_ENVELOPE */
        uicc_cat_polling_set_t       uicc_cat_polling_set;       /**< for service type UICC_CAT_POLLING_SET */
        uicc_cat_refresh_t           uicc_cat_refresh;           /**< for service type UICC_CAT_REFRESH */
    } sub_block_t; /**< union holding different structures corresponding to different service type requested */
} uicc_cat_req_t;


/**
 * \name MAL_UICC_CAT_RESP
 * \par
 * Structures involved in responses to uicc cat request for STK related features.
 */

/**
 * \struct uicc_cat_terminal_response_resp_t
 * \brief Structure holding subblocks for service type UICC_CAT_TERMINAL_RESPONSE
 */
typedef struct {
    uicc_sb_apdu_t uicc_sb_apdu; /**< subblock: UICC_SB_APDU */
} uicc_cat_terminal_response_resp_t;

/**
 * \struct uicc_cat_envelope_resp_t
 * \brief Structure holding subblocks for service type UICC_CAT_ENVELOPE
 */
typedef struct {
    uicc_sb_apdu_t uicc_sb_apdu; /**< subblock: UICC_SB_APDU */
} uicc_cat_envelope_resp_t;

/**
 * \struct uicc_cat_poll_resp_t
 * \brief Structure holding subblocks for service type UICC_CAT_POLL
 */
typedef struct {
    uicc_sb_apdu_t uicc_sb_apdu; /**< subblock: UICC_SB_APDU */
} uicc_cat_poll_resp_t;

/**
 * \struct uicc_sb_refresh_result_t
 * \brief  This Structure corresponds to subblock UICC_SB_REFRESH_RESULT
 */
typedef struct {
    uint8_t refresh_status;         /**< For possible refresh results, please see ETSI TS 102.223 */
    uint8_t additional_info_length; /**< Additional length, please see ETSI TS 102.223 */
    uint8_t *additional_info;       /**< Additional Information on result, please see ETSI TS 102.223 */
} uicc_sb_refresh_result_t;

/**
 * \struct uicc_cat_refresh_resp_t
 * \brief Structure holding subblocks for service type UICC_CAT_REFRESH
 */
typedef struct {
    uint8_t                  nb_refresh_result_instances;            /**< No of SB: UICC_SB_REFRESH_RESULT instances */
    uicc_sb_refresh_result_t *uicc_sb_refresh_result[MAX_APPL_PATH]; /**< SB: UICC_SB_REFRESH_RESULT, 1 or More instances of this SB will be present */
    uicc_sb_refresh_t        uicc_sb_refresh;                        /**< SB: UICC_SB_REFRESH */
} uicc_cat_refresh_resp_t;


/**
 * \struct uicc_cat_resp_t
 * \brief This Structure is for CAT Server Responses
 */
typedef struct {
    uint8_t service_type; /**< A subset of values from the constant table UICC_SERVICE_TYPE_CONSTANTS */
    uint8_t status;       /**< status */
    uint8_t details;      /**< details */
    uint8_t card_type;    /**< card type */
    uint8_t n_sb;         /**< No. of subblocks inside response data */
    union {
        uicc_cat_terminal_response_resp_t uicc_cat_terminal_response_resp; /**< for service type UICC_CAT_TERMINAL_RESPONSE */
        uicc_cat_envelope_resp_t          uicc_cat_envelope_resp;          /**< for service type UICC_CAT_ENVELOPE */
        uicc_cat_poll_resp_t              uicc_cat_poll_resp;              /**< for service type UICC_CAT_POLL */
        uicc_cat_refresh_resp_t           uicc_cat_refresh_resp;           /**< for service type UICC_CAT_REFRESH */
    } sub_block; /**< union holding different structures corresponding to different service type requested */
} uicc_cat_resp_t;


/**
 * \name MAL_UICC_CAT_IND
 * \par
 * Structures involved in uicc cat indications
 */

/**
 * \struct uicc_cat_fetched_cmd_t
 * \brief Structure holding subblocks for service type UICC_CAT_FETCHED_CMD
 */
typedef struct {
    uicc_sb_apdu_t  uicc_sb_apdu; /**< subblock: UICC_SB_APDU */
} uicc_cat_fetched_cmd_t;

/**
 * \struct uicc_cat_ind_t
 * \brief This Structure is for CAT Server Indications from Modem Side.
 */
typedef struct {
    uint8_t service_type; /**< A subset of values from the constant table UICC_SERVICE_TYPE_CONSTANTS */
    uint8_t card_type;    /**< Values from the constant table UICC_CARD_TYPE_TABLE */
    uint8_t n_sb;         /**< No. of subblocks inside response data */
    union {
        uicc_cat_fetched_cmd_t  uicc_cat_fetched_cmd; /**< UICC_SB_APDU */
    } sub_block; /**< union holding different structures corresponding to different service type requested */
} uicc_cat_ind_t;


/**
 * \name MAL_UICC_REFRESH_REQ
 * \par
 * Structures involved in uicc refresh request for SIM refresh related features.
 */

/**
 * \struct uicc_refresh_status_t
 * \brief Structure holding subblocks for service type UICC_REFRESH_STATUS
 */
typedef struct {
    uint8_t                  is_refresh_result_present; /**< Is SB: refresh result to be provided or not */
    uicc_sb_refresh_result_t uicc_sb_refresh_result;    /**< SB: UICC_SB_REFRESH_RESULT, None or One instance of this SB will be present */
} uicc_refresh_status_t;

/**
 * \struct uicc_refresh_req_t
 * \brief This Structure is for SIM refresh requests.
 */
typedef struct {
    uint8_t service_type; /**< A subset of values from the constant table UICC_SERVICE_TYPE_CONSTANTS */
    uint8_t status;       /**< Values from the constant table UICC_REFRESH_STATUS_TABLE */
    uint8_t client_id;    /**< Unique identifier for the Client, get in application activation */
    union {
        uicc_refresh_status_t  uicc_refresh_status;  /**< for service type UICC_REFRESH_STATUS */
    } sub_block_t; /**< union holding different structures corresponding to different service type requested */
} uicc_refresh_req_t;


/**
 * \name MAL_UICC_REFRESH_RESP
 * \par
 * Structures involved in responses to uicc refresh request for SIM refresh related features.
 */

/**
 * \struct uicc_refresh_resp_t
 * \brief This Structure is for SIM refresh Responses
 */
typedef struct {
    uint8_t service_type; /**< A subset of values from the constant table UICC_SERVICE_TYPE_CONSTANTS */
    uint8_t status;       /**< status */
    uint8_t details;      /**< details */
} uicc_refresh_resp_t;

/**
 * \name MAL_UICC_REFRESH_IND
 * \par
 * Structures involved in uicc refresh indications
 */

/**
 * \struct uicc_refresh_permission_t
 * \brief Structure holding subblocks for service type UICC_REFRESH_PERMISSION
 */
typedef struct {
    uicc_sb_appl_path_t  *uicc_sb_appl_path[MAX_APPL_PATH]; /**< SB: UICC_SB_APPL_PATH, None or more instances of this sub-block needed */
} uicc_refresh_permission_t;

/**
 * \struct uicc_refresh_now_t
 * \brief Structure holding subblocks for service type UICC_REFRESH_NOW
 */
typedef struct {
    uicc_sb_appl_path_t  *uicc_sb_appl_path[MAX_APPL_PATH]; /**< SB: UICC_SB_APPL_PATH, None or more instances of this sub-block needed */
} uicc_refresh_now_t;

/**
 * \struct uicc_refresh_ind_t
 * \brief This Structure is for Refresh Indications from Modem Side.
 */
typedef struct {
    uint8_t service_type; /**< A subset of values from the constant table UICC_SERVICE_TYPE_CONSTANTS */
    uint8_t type;         /**< Values from the constant table UICC_REFRESH_TYPE_CONSTANTS */
    uint8_t appl_id;      /**< Unique identifier for the Application taken from subblock UICC_SB_APPL_DATA_OBJECT */
    uint8_t n_sb;         /**< No. of subblocks inside response data */
    union {
        uicc_refresh_permission_t uicc_refresh_permission; /**< for service type UICC_REFRESH_PERMISSION */
        uicc_refresh_now_t        uicc_refresh_now;        /**< for service type UICC_REFRESH_NOW */
    } sub_block; /**< union holding different structures corresponding to different service type requested */
} uicc_refresh_ind_t;

/*--------------------------------------------*/


/**
 * \name Function/Service list
 * \par
 * Function/Service list of UICC MAL library
 */

/* ----------------------------------------------------------------------- */
/**
 * \fn int32_t mal_uicc_init(int32_t *fd_uicc);
 * \brief MAL_API - Startup function for using UICC MAL.
 *
 * \par
 * \n Initializes UICC MAL data structures and state,
 * \n Creates Phonet Socket Session.
 * \n Creates Netlink Socket Session
 *
 * \param [out] fd_uicc points to file descriptor of uicc socket
 * \param [out] fd_netlnk points to file descriptor of netlink socket
 * \return  - \ref mal_error_type (SUCCESS/FAIL)
 */
/* ----------------------------------------------------------------------- */
int32_t    mal_uicc_init(int32_t *fd_uicc, int32_t *fd_netlnk);


/* ----------------------------------------------------------------------- */
/**
 * \fn int32_t    mal_uicc_config(void);
 * \brief MAL-API - Configures UICC MAL Parameters.
 *
 * \par
 * \n Reads default config parameters or Sets specific config parameters
 * \n This function should be called only once and immediately
 * \n after mal_uicc_init()
 *
 * @return  - \ref mal_error_type (SUCCESS/FAIL)
 */
/* ----------------------------------------------------------------------- */
int32_t    mal_uicc_config(void);


/* ----------------------------------------------------------------------- */
/**
 * \typedef  typedef    void (*mal_uicc_event_cb_t)(int32_t message_id, void *data, int32_t mal_error, void* client_tag);
 * \brief Event notification to UICC-CAT server
 * \param [in]  message_id of solicited/unsolicited event.
 * \param [in]  data pointer to parameter values associated with this event
 * \param [in]  mal_error error type associated with this event
 * \param [in]  Client tag associated with the response or indication
 */
/* ---------------------------------------------------------------------- */
typedef    void (*mal_uicc_event_cb_t)(int32_t message_id, void *data, mal_error_type  mal_error, void *client_tag);

/* ----------------------------------------------------------------------- */
/**
 * \fn int32_t    mal_uicc_register_callback(mal_uicc_event_cb_t event_cb);
 * \brief MAL_API - Register Callback.
 *
 * \param [in] mal_uicc_event_cb_t - Callback function pointer
 * \return  - \ref mal_error_type (SUCCESS/FAIL)
 */
/* ----------------------------------------------------------------------- */
int32_t    mal_uicc_register_callback(mal_uicc_event_cb_t event_cb);


/* ----------------------------------------------------------------------- */
/**
 * \fn int32_t    mal_uicc_deinit(void);
 * \brief MAL_API - Deinitializes UICC Mal.
 *
 * \return  - \ref mal_error_type (SUCCESS/FAIL)
 */
/* ----------------------------------------------------------------------- */
int32_t    mal_uicc_deinit(void);


/**
 * \fn int32_t    mal_uicc_request(int32_t message_id,  void *data);
 * \brief MAL_API -  UICC Mal Request.
 *
 * \param [in] messageID - Message ID
 * \param [in] data  - Message Data pointer
 * \param [in] client_tag  - Specific client tag value passed from mal client user
 * \return  - \ref mal_error_type (SUCCESS/FAIL)
 */
/* ----------------------------------------------------------------------- */
int32_t    mal_uicc_request(int32_t message_id,  void *data, void *client_tag);


/* ----------------------------------------------------------------------- */
/**
 * \fn void mal_uicc_response_handler();
 * \brief MAL-API - UICC Response and Indication Handler
 *
 * \return  - void
 */
/* ----------------------------------------------------------------------- */
void mal_uicc_response_handler();

/**
 *  \fn int32_t mal_uicc_request_set_debug_level(uint8_t level)
 *  \brief  This Api is used to set the debug level for uicc module. This is a synchronous call.
 *  \param [in] level  0 - No Prints; 1 - Entry & Exit Prints; 2 - Value/Info Prints; 3 - ISI MSG Prints
 *                               4 - All Prints
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_uicc_request_set_debug_level(uint8_t level);

/**
 *  \fn int32_t mal_uicc_request_get_debug_level(uint8_t *level)
 *  \brief  This Api is used to get the debug level for uicc module. This is a synchronous call.
 *   \param[out] debug level for uicc  module. data type: pointer to uint8_t.
 *                       Caller needs to take care of memory allocation
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_uicc_request_get_debug_level(uint8_t *level);

#endif /*UICC_MAL_API_H*/
