/**
 * \file  Mal_ftd.h
 * \brief This file includes headers for FTD MAL API.
 *
 * \n Copyright (C) ST-Ericsson SA 2011-2012. All rights reserved.
 * \n This code is ST-Ericsson proprietary and confidential.
 * \n Any use of the code for whatever purpose is subject to
 * \n specific written permission of ST-Ericsson SA.
 *
 * \par
 * \n MAL API header file for FTD Library.
 * \n \author ST-Ericsson
 * \n
 * \n Version : 0.0.1
 * \n
 * \n Revision History:
 * \par
 * \n v0.0.1           Added FTD activation request and
 *                            related data structures
 * \n                  Aligned and latest tested on:
 * \n                  DV 31.0
 */

#ifndef FTD_MAL_API_H
#define FTD_MAL_API_H
#define FTD_LIB_VERSION "FTD Lib V 000.001"

/*Strings/reasons, when no data is available for an FTD ID*/
#define FTD_MEASUREMENT_NOT_SUPPORTED "MODEM_TEST_FTD_ITEM_NOT_SUPPORTED"
#define FTD_MEASUREMENT_NOT_AVAILABLE "MODEM_TEST_FTD_ITEM_NOT_AVAILABLE"
#define FTD_MEASUREMENT_REQ_FAILED "REQ_FAILED"
#define FTD_MEASUREMENT_REQ_TIMEOUT "REQ_TIMEOUT"
#ifndef COMM_MAX_FTD_STRING_LENGTH
#define COMM_MAX_FTD_STRING_LENGTH 0x7C
#endif

/*MAL_FTD and MAL_MTE use the same phonet resource, hence define new phonet obj ID*/
#define PN_FTD_OBJ_ID           0x44



/*---------------------------------------------------------------------------*/
/* Event IDs                                                                 */
/*---------------------------------------------------------------------------*/
/**/

/*
 * \def MAL_FTD_MEASUREMENT_IND
 * \brief Field Test Measurement Indication
 *
 * \n Unsolicited indication reporting field test measurements.
 */
#define MAL_FTD_MEASUREMENT_IND 0x00

/**
 * \def MAL_FTD_ERROR_IND
 * \brief Field Test Error Indication
 *
 * \n Unsolicited indication reporting an error.
 */
#define MAL_FTD_ERROR_IND 0x01

/**
 * \def MAL_FTD_ACT_IND
 * \brief Field Test Activation request Indication
 *
 * \n Unsolicited indication reporting an Trace activation success or failed.
 */
#define MAL_FTD_ACT_IND 0x02

/**
 * \def MAL_FTD_DEACT_IND
 * \brief Field Test Activation request Indication
 *
 * \n Unsolicited indication reporting an Trace deactivation success or failed.
 */

#define MAL_FTD_DEACT_IND 0x03


/**
 * \enum mal_ftd_error_type
 * \brief Error types
 *
 * \n This enum defines the return values for ftd functions and Event callbacks.
 */

typedef enum {
    MAL_FTD_SUCCESS = 0, /**< Success  */
    MAL_FTD_FAIL = 1, /**< Success  */
    MAL_FTD_NOT_SUPPORTED = 2 /**< Success  */
} mal_ftd_error_type;

/**
 * \enum ftd_sub_block_template_t
 * \brief FTD id (Sub Blocks) data type
 *
 * \n This enum defines the type of data present in FTD Ids (Sub blocks).
 */
typedef enum {
    COMM_FTD_DATA_BYTE,
    COMM_FTD_DATA_WORD,
    COMM_FTD_DATA_DWORD,
    COMM_FTD_DATA_STRING,
    COMM_FTD_DATA_NONE
} ftd_sub_block_template_t;

/**
 * \struct ftd_table_entry_t
 * \brief Page table row
 *
 * \n This structure defines a row in the page table.
 * \n In general, it defines the FTD id and all its attributes.
 */
typedef struct {
    uint8_t received_sub;
    uint8_t ftd_entity_id;
    ftd_sub_block_template_t sub_block_template;
    uint16_t ftd_id;
    char name_p[COMM_MAX_FTD_STRING_LENGTH];
    char str_value[COMM_MAX_FTD_STRING_LENGTH];
} ftd_table_entry_t;

/**
 * \struct view_details_t
 * \brief  This will be sent to Application
 *
 * \n This structure will be sent as part of the call back for printing table to file.
 */
typedef struct {
    uint8_t size_of_table;
    ftd_table_entry_t *table_to_print;
} view_details_t;

/**
 * \name Function/Service list
 * \par
 * Function/Service list of FTD MAL library
 */

/* ----------------------------------------------------------------------- */
/**
 * \fn  int32_t mal_ftd_init(int32_t *fd_ftd)
 *
 * \brief MAL_API - Startup function for using FTD MAL.
 *
 * \par
 * \n Initializes FTD MAL data structures and state,
 * \n Creates Phonet Socket Session.
 *
 * \param [out] fd_ftd points to file descriptor of ftd socket
 * \return  - \ref mal_error_type (SUCCESS/FAIL)
 */
/* ----------------------------------------------------------------------- */
int32_t    mal_ftd_init(int32_t *fd_ftd);

/* ----------------------------------------------------------------------- */
/**
 * \typedef  void (*mal_ftd_event_cb_t)(int32_t message_id, void *data,
 * \n             mal_error_type  mal_error, void *client_tag);
 * \brief Event notification to MAL FTD server
 * \param [in]  data pointer to parameter values associated with this event
 * \param [in]  mal_error error type associated with this event
 * \param [in]  Client tag associated with the response or indication
 */
/* ---------------------------------------------------------------------- */
typedef    void (*mal_ftd_event_cb_t)(int32_t message_id, void *data,
                                      mal_ftd_error_type  mal_error, void *client_tag);


/* ----------------------------------------------------------------------- */
/**
 * \fn int32_t    mal_ftd_register_callback(mal_ftd_event_cb_t event_cb);
 * \brief MAL_API - Register Callback.
 *
 * \param [in] mal_ftd_event_cb_t - Callback function pointer
 * \return  - \ref mal_error_type (SUCCESS/FAIL)
 */
/* ----------------------------------------------------------------------- */
int32_t    mal_ftd_register_callback(mal_ftd_event_cb_t event_cb);


/* ----------------------------------------------------------------------- */
/**
 * \fn int32_t    mal_ftd_deinit(void);
 * \brief MAL_API - Deinitializes FTD Mal.
 * \return  - \ref mal_error_type (SUCCESS/FAIL)
 */
/* ----------------------------------------------------------------------- */
int32_t    mal_ftd_deinit(void);

/* ----------------------------------------------------------------------- */
/**
 * \fn int32_t    mal_ftd_activate_request(int32_t page_id, uint8_t mode);
 * \brief MAL_API -  FTD Mal Request to activate the FTD entities for a particular page.
 *
 * \param [in] page_id  - page_id
 * \param [in] mode       - DUMP_ONCE/PERIODIC_DUMP
 * \return  - \ref mal_error_type (SUCCESS/FAIL)
*/
/* ----------------------------------------------------------------------- */

int32_t mal_ftd_activate_request(int32_t page_id, uint8_t mode);


/* ----------------------------------------------------------------------- */
/**
 * \fn int32_t    mal_ftd_deactivate_request(int32_t page_id);
 * \brief MAL_API -  FTD Mal Request to deactivate the active FTD entities.
*  \param [in] page_id  - page_id
 * \return  - \ref mal_error_type (SUCCESS/FAIL)
 */
/* ----------------------------------------------------------------------- */
int32_t mal_ftd_deactivate_request(int32_t page_id);


/* ----------------------------------------------------------------------- */
/**
 * \fn int32_t    mal_ftd_deactivate_request(int32_t page_id);
 * \brief MAL_API -  FTD Mal Request to abort a one shot dump request if MAL is waiting for too long.
*  \param [in] page_id  - page_id
 * \return  - \ref mal_error_type (SUCCESS/FAIL)
 */
/* ----------------------------------------------------------------------- */
int32_t mal_ftd_stop_dump_once_report(int32_t page_id);


/* ----------------------------------------------------------------------- */
/**
 * \fn void mal_ftd_response_handler(int32_t dgram);
 * \brief MAL-API - FTD Response and Indication Handler
 * \param [in] dgram  - socket identifier/descriptor
 * \return  - void
 */
/* ----------------------------------------------------------------------- */
void mal_ftd_response_handler(int32_t dgram);

/* ----------------------------------------------------------------------- */
/**
 *  \fn int32_t mal_ftd_request_set_debug_level(uint8_t level)
 *  \brief  This Api is used to set the debug level for gss  module. This is a synchronous call.
 *  \param [in] level  0 - No Prints; 1 - Entry & Exit Prints; 2 - Value/Info Prints; 3 - ISI MSG Prints
 *                               4 - All Prints
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
/* ----------------------------------------------------------------------- */
int32_t mal_ftd_request_set_debug_level(uint8_t level);


/* ----------------------------------------------------------------------- */
/**
 *  \fn int32_t mal_ftd_request_get_debug_level(uint8_t *level)
 *  \brief  This Api is used to get the debug level for gss module. This is a synchronous call.
 *   \param[out] debug level for gss  module. data type: pointer to uint8_t.
 *                       Caller needs to take care of memory allocation
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
/* ----------------------------------------------------------------------- */

int32_t mal_ftd_request_get_debug_level(uint8_t *level);

#endif /*FTD_MAL_API_H*/
