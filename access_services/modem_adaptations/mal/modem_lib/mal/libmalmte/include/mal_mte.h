/**
 * \file mal_mte.h
 * \brief This file includes headers for MTE MAL.
 *
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \n MAL MTE header file for MTE Library.
 * \n \author ST-Ericsson
 * \n
 * \n Revision History:
 * \n
 * \n v0.3   Added mal_mte_select_antenna_path to select
 * \n        Main and diversity Antenna.
 * \n
 * \n v0.2   Addedd mal_mte_set_quick_release_mode to set user
 * \n        activity mode for Power saving enable and disable
 * \n
 * \n v0.1    Initial
 */

/**
 * \defgroup  MAL_MTE MTE API
 * \par
 * \n This part describes the interface to MTE (Modem TEst) MAL Client
 */

#ifndef MAL_MTE_H
#define MAL_MTE_H

#include <stdint.h>

#include "transactionmanager.h"

#define MTE_LIB_VERSION "MTE Lib V 0.1"

/* ----------------------------------------------------------------------- */
/* Event ID's                                                              */
/* ----------------------------------------------------------------------- */


/** \def MAL_MODEM_TEST_SLEEP_TEST_MODE_SET_RESP
+ *  \brief Modem response for sleep test mode request.
+ *
+ */
#define MAL_MODEM_TEST_SLEEP_TEST_MODE_SET_RESP                0x85

/*MAL_FTD and MAL_MTE use the same resource*/
#define PN_MTE_OBJ_ID           0x33

/* Test cases IDs definition
 */
#define MODEM_TEST_CASE_I2C_ACTIVITY    0x00
#define MODEM_TEST_CASE_CPUDDR_ACTIVITY 0x01
#define MODEM_TEST_CASE_ASSERT          0x10
#define MODEM_TEST_CASE_WATCHDOG_RESET  0x11
#define MODEM_TEST_CASE_DATA_ABORT      0x12
#define MODEM_TEST_CASE_PREFETCH_ABORT  0x13
#define MODEM_TEST_CASE_UNDEFINED_ABORT 0x14
#define MODEM_TEST_CASE_TRIGGER_REQ     0x15

/*---------------------------------------------------------------------------*/
/* Event IDs                                                                 */
/*---------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Structures, Enums and Typedefs                                             */
/*----------------------------------------------------------------------------*/

/**
 * \enum mal_mte_error_type
 * \brief Error types
 *
 * \n This enum defines error types for the MTE MAL Library. These error types
 * \n are used as return values for the API routines.
 */
typedef enum {
    MAL_MTE_SUCCESS             = 0, /**< Success  */
    MAL_MTE_FAIL                = 1, /**< Failure */
    MAL_MTE_GENERAL_ERROR       = 2, /**< Request cannot be completed because of a
                                      * general error
                                      */
    MAL_MTE_MODE_NOT_SUPPORTED  = 3, /**< Requested mode not supported */
    MAL_MTE_SERVICE_NOT_ALLOWED = 4, /**< Requested service not allowed */
    MAL_MTE_INVALID_BLOCK_TYPE  = 5, /**< Block type not correct */
    MAL_MTE_INSUFFICIENT_MEMORY = 6, /**< Memory allocation failure */
    MAL_MTE_INVALID_DATA        = 7, /**< Invalid data passed to the routine */
    MAL_MTE_SOCK_SEND_ERROR     = 8  /**< Send operation on socket failed */
} mal_mte_error_type;

/**
 * \enum mal_mte_path_sel_mode
 * \brief Antenna path selection mode
 *
 * \n This enum defines values for Antenna path selection
 * \n are used as input values for the API routine.
 */
typedef enum {
    MAL_MTE_MAIN_ANTENNA_MODE       = 0, /* Main Antenna Path Only */
    MAL_MTE_DIVERSITY_ANTENNA_MODE  = 1, /* Diversity Antenna Path Only */
    MAL_MTE_NOMAL_RX_DIVERSITY_MODE = 2, /* Normal RX Diversity Mode */
} mal_mte_path_sel_mode;

/**
 * \enum mal_mte_cpu_type
 * \brief Selection of the affected modem CPU
 *
 * \n This enum defines values for affected modem CPU. These values
 * \n are used as input values for the API routine.
 */
typedef enum {
    MAL_MTE_MODEM_TEST_CPU_L23  = 0, /**< Selection affects L23 CPU of a two-processor
                                      * modem and CPU of a single-processor modem
                                      */
    MAL_MTE_MODEM_TEST_CPU_L1   = 1, /**< Selection affects L1 CPU of a two-processors modem  */
    MAL_MTE_MODEM_TEST_CPU_ALL  = 2, /**< Selection affects all modem CPU's */
} mal_mte_cpu_type;

/**
 * \enum mal_mte_forced_sleep_mode
 * \brief Set the modem CPU's to the given sleep mode
 *
 * \n This enum defines values for modem CPU's to the given sleep mode. These values
 * \n are used as input values for the API routine.
 */
typedef enum {
    MAL_MTE_MODEM_TEST_SLEEP_MODE_ALL_ALLOWED                = 0x00, /**< Power-down sleep mode is allowed */
    MAL_MTE_MODEM_TEST_SLEEP_MODE_PWR_DOWN_WITH_RF_CLK_REQ   = 0x10, /**< Power-down sleep mode with RF clock request is allowed */
    MAL_MTE_MODEM_TEST_SLEEP_MODE_DEEP_SLEEP                 = 0x20, /**< Deep sleep mode without power-down is allowed */
    MAL_MTE_MODEM_TEST_SLEEP_MODE_DEEP_SLEEP_WITH_RF_CLK_REQ = 0x30,/**< Deep sleep mode with RF clock request is allowed */
    MAL_MTE_MODEM_TEST_SLEEP_MODE_LIGHT_WITHOUT_PLL          = 0x40, /**< Light sleep mode without HF clock request is allowed */
    MAL_MTE_MODEM_TEST_SLEEP_MODE_LIGHT_WITH_PLL             = 0x50, /**< Light sleep mode with HF clock request is allowed  */
    MAL_MTE_MODEM_TEST_SLEEP_MODE_NO_SLEEP                   = 0x60, /**< Sleep mode is not allowed (no WFI)  */
} mal_mte_forced_sleep_mode;;


typedef struct {
    uint8_t state;
    uint32_t duration;
    uint32_t nb_memcpy;
    uint32_t nb_cache_iteration;
} mte_data_cpuddr_t;

/**
 * \typedef void (*mal_mte_event_cb_t)(int32_t event_id, void *data,
 *                            mal_mte_error_type error_code, void *client_tag)
 * \brief Event notification to Application
 *
 * \n Callback function for event notification to the Application. This callback is
 * \n used for both solicited responses and unsolicited indications.
 *
 * \param [in] event_id      ID of the event being notified
 * \param [in] data          Pointer to data associated with the event being notified
 * \param [in] error_code    Error code associated with the event
 * \param [in] client_tag    Client tag associated with the response or indication
 *
 * \par
 * Following is the detail on data based on the event type:
 * \par
 * \arg MAL_MTE_SET_PREFERRED_NETWORK_TYPE_RESP
 * \n NULL
 * \par
 * \arg MAL_MTE_GET_PREFERRED_NETWORK_TYPE_RESP
 * \n Pointer to \ref mal_mte_network_type
 * \par
 * \arg MAL_MTE_QUERY_AVAILABLE_BAND_MODE_RESP
 * \par
 * \arg MAL_MTE_SET_USER_ACTIVITY_INFO_RESP
 * \par
 * \arg MAL_MTE_SET_USER_ACTIVITY_INFO_IND
 * \par
 * \n In the current implementation this event will not be received.
 */
typedef void (*mal_mte_event_cb_t)(int16_t case_id, void *data,
                                   int32_t error_code, void *client_tag);

/*----------------------------------------------------------------------------*/
/* Function declarations                                                      */
/*----------------------------------------------------------------------------*/

/**
 * \fn int32_t mal_mte_init(int32_t *fd_mte)
 * \brief Initialize MTE Library
 *
 * \n This routine is used to initialize the MTE Library and open the socket
 * \n using Phonet Liibrary. This routine should be called only once at the time
 * \n of initialization.
 * \param [out] fd_mte    Pointer to file descriptor of MTE socket
 * \return    \ref        mal_mte_error_type
 */
int32_t mal_mte_init(int32_t *fd_mte);

/**
 * \fn int32_t mal_mte_deinit(void)
 * \brief De-initialize MTE Library
 *
 * \n This routine is used to de-initialize the MTE Library.
 * \param            void
 * \return    \ref   mal_mte_error_type
 */
int32_t mal_mte_deinit(void);

/**
 * \fn int32_t mal_mte_register_callback(mal_mte_event_cb_t event_cb)
 * \brief Register callbacks from application
 *
 * \n This routine registers the callback function for passing events
 * \n to the Application.
 * \param [in] event_cb       Event callback handler
 * \return    \ref            mal_mte_error_type
 */
int32_t mal_mte_register_callback(mal_mte_event_cb_t event_cb);

/**
 * \fn void mal_mte_response_handler(void)
 * \brief Handler for received messages
 *
 * \n This routine is called by Application to inform that some message is
 * \n available from modem for processing.
 * \param    void
 * \return    void
 */
void mal_mte_response_handler(void);

/**
 * \fn int32_t mal_mte_gen_cpuddr_activity(uint16_t group_id, mte_data_cpuddr_t *data, void *client_tag)
 * \brief Generate CPU/DDR activity on a CPU
 *
 * \n This routine is used to generate CPU/DDR activity on a CPU
 * \param [in] group_id     CPU group id (cf. NWM MAL Modem test header)
 * \param [in] data         Structure contaning the parameters of the test
 * \param [in] client_tag   Void pointer to client specific data. The client
 *                              specific data is transparent to MAL and is
 *                              returned as-is in the response.
 * \return    int32_t       Error code return
 */
int32_t mal_mte_gen_cpuddr_activity(uint16_t group_id, mte_data_cpuddr_t *data, void *client_tag);

/**
 * \fn int32_t mal_mte_gen_assert(uint16_t group_id, void *client_tag)
 * \brief Generate Assert on a CPU
 *
 * \n This routine is used to generate an Assert on a CPU
 * \param [in] group_id     CPU group id (cf. NWM MAL Modem test header)
 * \param [in] client_tag   Void pointer to client specific data. The client
 *                              specific data is transparent to MAL and is
 *                              returned as-is in the response.
 * \return    int32_t       Error code return
 */
int32_t mal_mte_gen_assert(uint16_t group_id, void *client_tag);

/**
 * \fn int32_t mal_mte_gen_watchdog_reset(uint16_t group_id, void *client_tag)
 * \brief Generate a Watchdog reset on a CPU
 *
 * \n This routine is used to generate a Watchdog reset on a CPU
 * \param [in] group_id     CPU group id (cf. NWM MAL Modem test header)
 * \param [in] client_tag   Void pointer to client specific data. The client
 *                              specific data is transparent to MAL and is
 *                              returned as-is in the response.
 * \return    int32_t       Error code return
 */
int32_t mal_mte_gen_watchdog_reset(uint16_t group_id, void *client_tag);

/**
 * \fn int32_t mal_mte_gen_data_abort(uint16_t group_id, void *client_tag)
 * \brief Generate a data abort on a CPU
 *
 * \n This routine is used to generate a data abort on a CPU
 * \param [in] group_id     CPU group id (cf. NWM MAL Modem test header)
 * \param [in] client_tag   Void pointer to client specific data. The client
 *                              specific data is transparent to MAL and is
 *                              returned as-is in the response.
 * \return    int32_t       Error code return
 */
int32_t mal_mte_gen_data_abort(uint16_t group_id, void *client_tag);

/**
 * \fn int32_t mal_mte_gen_prefetch_abort(uint16_t group_id, void *client_tag)
 * \brief Generate prefetch abort on a CPU
 *
 * \n This routine is used to generate a prefetch abort on a CPU
 * \param [in] group_id     CPU group id (cf. NWM MAL Modem test header)
 * \param [in] client_tag   Void pointer to client specific data. The client
 *                              specific data is transparent to MAL and is
 *                              returned as-is in the response.
 * \return    int32_t       Error code return
 */
int32_t mal_mte_gen_prefetch_abort(uint16_t group_id, void *client_tag);

/**
 * \fn int32_t mal_mte_gen_undefined_abort(uint16_t group_id, void *client_tag);
 * \brief Generate an undefined abort on a CPU
 *
 * \n This routine is used to generate an undefined abort on a CPU
 * \param [in] group_id     CPU group id (cf. NWM MAL Modem test header)
 * \param [in] client_tag   Void pointer to client specific data. The client
 *                              specific data is transparent to MAL and is
 *                              returned as-is in the response.
 * \return    int32_t       Error code return
 */
int32_t mal_mte_gen_undefined_abort(uint16_t group_id, void *client_tag);

/**
 * \fn int32_t mal_mte_gen_i2c_activity(uint8_t *state, void *client_tag);
 * \brief Generate I2C activity
 *
 * \n This routine is used to generate an Assert on a CPU
 * \param [in] state        Start or Stop I2C activity
 *                              7 - START, 8 - STOP
 * \param [in] client_tag   Void pointer to client specific data. The client
 *                              specific data is transparent to MAL and is
 *                              returned as-is in the response.
 * \return    int32_t       Error code return
 */
int32_t mal_mte_gen_i2c_activity(uint8_t *state, void *client_tag);

/**
 * \fn int32_t mal_mte_gen_trigger_req(void *client_tag);
 * \brief Generate a trigger request on modem
 *
 * \n This routine is used to generate a trigger request.
 * \param [in] client_tag   Void pointer to client specific data. The client
 *                              specific data is transparent to MAL and is
 *                              returned as-is in the response.
 * \return    int32_t       Error code return
 */
int32_t mal_mte_gen_trigger_req(void *client_tag);

/**
 * \fn int32_t mal_mte_select_antenna_path(mal_mte_path_sel_mode selection_mode, void *client_tag)
 * \brief Selection of Antenna path
 *
 * \n This routine is called by Application to select Antenaa path. Expected
 * \n event type in case of successful invocation of this routine is
 * \n TBD.
 *
 * \param [in] selection_mode Selection mode to be set for
 *                0 - Enable Main Antenna Path Only
 *                1 - Enable Diversity Antenna Path Only
 *                2 - Enable Normal RX Diversity Mode
 * \param [in] client_tag     Void pointer to client specific data. The client
 *                                specific data is transparent to MAL and is
 *                                returned as-is in the response.
 * \return    \ref           mal_misc_error_type
 */
int32_t mal_mte_select_antenna_path(mal_mte_path_sel_mode selection_mode, void *client_tag);
/**
 * \fn int32_t mal_mte_reset_modem_with_dump(mal_mte_cpu_type cpu_type, void *client_tag)
 * \brief Selection of CPU type for resetting modem with dump
 *
 * \n This routine is called by Application to select cpu type.
 *
 * \param [in] cpu_type  CPU type to be set for
 *                0 - Selection affects L23 CPU of a two-processor modem/CPU of a single-processor modem
 *                1 - Selection affects L1 CPU of a two-processors modem
 *                2 - Selection affects all modem CPU's
 * \param [in] client_tag     Void pointer to client specific data. The client
 *                                specific data is transparent to MAL and is
 *                                returned as-is in the response.
 * \return    \ref           mal_misc_error_type
 */
int32_t mal_mte_reset_modem_with_dump(mal_mte_cpu_type cpu_type, void *client_tag);

/**
 * \fn int32_t mal_mte_sleep_test_mode(mal_mte_forced_sleep_mode forced_sleep_mode, mal_mte_cpu_type cpu_type, void *client_tag)
 * \brief Selection of Modem CPU's to the given sleep mode to disable modem interrupts
 *
 * \n This routine is called by Application to select forced sleep mode and cpu type
 *
 *\param [in] forced_sleep_mode  Forced sleep mode to be set for
 *                0x00 - Power-down sleep mode is allowed
 *                0x10 - Power-down sleep mode with RF clock request is allowed
 *                0x20 - Deep sleep mode without power-down is allowed
 *                0x30 - Deep sleep mode with RF clock request is allowed
 *                0x40 - Light sleep mode without HF clock request is allowed
 *                0x50 - Light sleep mode with HF clock request is allowed
 *                0x60 - Sleep mode is not allowed (no WFI)
 * \param [in] cpu_type  CPU type to be set for
 *                0 - Selection affects L23 CPU of a two-processor modem/CPU of a single-processor modem
 *                1 - Selection affects L1 CPU of a two-processors modem
 *                2 - Selection affects all modem CPU's
 * \param [in] client_tag     Void pointer to client specific data. The client
 *                                specific data is transparent to MAL and is
 *                                returned as-is in the response.
 * \return    \ref           mal_misc_error_type
 */
int32_t mal_mte_sleep_test_mode(mal_mte_forced_sleep_mode forced_sleep_mode, mal_mte_cpu_type cpu_type, void *client_tag);

/**
 * \fn int32_t mal_mte_forced_sleep_mode_set(mal_mte_forced_sleep_mode forced_sleep_mode, mal_mte_cpu_type cpu_type, void *client_tag)
 * \brief Sets the lowest allowed sleep mode for the selected CPUs
 *
 * \n This routine is called by Application to select forced sleep mode and cpu type
 *
 *\param [in] forced_sleep_mode  Forced sleep mode to be set for
 *                0x00 - Power-down sleep mode is allowed
 *                0x10 - Power-down sleep mode with RF clock request is allowed
 *                0x20 - Deep sleep mode without power-down is allowed
 *                0x30 - Deep sleep mode with RF clock request is allowed
 *                0x40 - Light sleep mode without HF clock request is allowed
 *                0x50 - Light sleep mode with HF clock request is allowed
 *                0x60 - Sleep mode is not allowed (no WFI)
 * \param [in] cpu_type  CPU type to be set for
 *                0 - Selection affects L23 CPU of a two-processor modem/CPU of a single-processor modem
 *                1 - Selection affects L1 CPU of a two-processors modem
 *                2 - Selection affects all modem CPU's
 * \param [in] client_tag     Void pointer to client specific data. The client
 *                                specific data is transparent to MAL and is
 *                                returned as-is in the response.
 * \return    \ref           mal_misc_error_type
 */
int32_t mal_mte_forced_sleep_mode_set(mal_mte_forced_sleep_mode forced_sleep_mode, mal_mte_cpu_type cpu_type, void *client_tag);

/**
 *  \fn int32_t mal_mte_request_set_debug_level(uint8_t level)
 *  \brief  This Api is used to set the debug level for mte module. This is a synchronous call.
 *  \param [in] level  0 - No Prints; 1 - Entry & Exit Prints; 2 - Value/Info Prints; 3 - ISI MSG Prints
 *                               4 - All Prints
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_mte_request_set_debug_level(uint8_t level);

/**
 *  \fn int32_t mal_mte_request_get_debug_level(uint8_t *level)
 *  \brief  This Api is used to get the debug level for mte module. This is a synchronous call.
 *   \param[out] debug level for mis  module. data type: pointer to uint8_t.
 *                       Caller needs to take care of memory allocation
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_mte_request_get_debug_level(uint8_t *level);

#endif /* MAL_MTE_H */
