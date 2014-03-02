/**
 * \file  Mal_monitor.h
 * \brief This file includes headers for MONITOR MAL API.
 *
 * \n Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * \n This code is ST-Ericsson proprietary and confidential.
 * \n Any use of the code for whatever purpose is subject to
 * \n specific written permission of ST-Ericsson SA.
 *
 * \par
 * \n MAL API header file for AML MONITOR Library.
 * \n \author ST-Ericsson
 * \n
 * \n Version : 0.0.1
 * \n
 * \n Revision History:
 * \par
 * \n v0.0.1           Added Monitor trace activate request interface and
 *                            related data structures
 * \n                  Aligned and latest tested on:
 * \n                  MODEM SW version : DV 9.0, MON ISI SPEC version: 001.002
 */

#ifndef MON_MAL_API_H
#define MON_MAL_API_H

#include <stdint.h>
#include "mal_utils.h"



#define MAL_MON_TRACE_ACTIVATE_REQ                   0x00
#define MAL_MON_TRACE_ACTIVATE_RESP                  0x01
#define MAL_MON_TRACE_DEACTIVATE_REQ                 0x03
#define MAL_MON_TRACE_DEACTIVATE_RESP                0x04
#define MAL_MON_TRACE_ACTIVATION_READ_STATUS_REQ     0x46
#define MAL_MON_TRACE_ACTIVATION_READ_STATUS_RESP    0x47
#define MAL_MON_TRACE_INV_IND                        0x8F
#define MAL_MON_TRACE_CONFIG_SET_REQ                 0xB0
#define MAL_MON_TRACE_CONFIG_SET_RESP                0xB1
#define MAL_MON_TRACE_CONFIG_GET_REQ                 0xB2
#define MAL_MON_TRACE_CONFIG_GET_RESP                0xB3
#define MAL_MON_TRACE_ROUTING_REQ                    0xB4
#define MAL_MON_TRACE_ROUTING_RESP                   0xB5
#define MAL_MON_TRACE_BUFFER_FLUSH_IND               0xB8
#define MAL_MON_TRACE_LOG_TRIGGER_IND                0xB9
#define MAL_MON_TRACE_TRIGGER_REQ                    0xBA
#define MAL_MON_TRACE_TRIGGER_RESP                   0xBB
#define MAL_MON_TRACE_IDS_ACTIVATE_ROUTE_REQ         0xBC
#define MAL_MON_TRACE_IDS_ACTIVATE_ROUTE_RESP        0xBD
#define MAL_MON_TRACE_BUFFER_MARKER_REQ              0xBE
#define MAL_MON_TRACE_BUFFER_MARKER_RESP             0xBF
#define MAL_MON_TRACE_BUFFERING_MODE_REQ             0xEA
#define MAL_MON_TRACE_BUFFERING_MODE_RESP            0xEB
#define MAL_MON_TRACE_BUFFERING_MODE_READ_REQ        0xEC
#define MAL_MON_TRACE_BUFFERING_MODE_READ_RESP       0xED


/**
 * \struct mon_sb_trace_act_bitmap_t
 * \brief This Structure corresponds to subblock
 * \n     MON_SB_TRACE_ACTIVATION_BITMAP
 * \n it is used for
 * \n MAL_MON_TRACE_ACTIVATE_REQ
 * \n MAL_MON_TRACE_ACTIVATION_READ_STATUS_RESP
 */
typedef struct {

    /*  MAL_MON_TRACE_TYPE_SYMBOL  */
#define MAL_MON_OS_TRACES 0x0
#define MAL_MON_MASTER_TRACES 0x2
#define MAL_MON_VENDOR_DLL_TRACES 0x3
#define MAL_MON_MCU_TRACES 0x6
#define MAL_MON_INVARIANT_TRACES 0x8
    uint8_t trace_type;     /**< A subset of values from the constant table MON_TRACE_TYPE_SYMBOLS */
    uint8_t entity_id;      /**< entity id */
    uint8_t bitmap_length;  /**< bitmap length */
    /*  deallocate by receiver  */
    uint32_t *bitmap_array; /**< bitmap array */

} mon_sb_trace_act_bitmap_t;




/* No timestamp required. */
#define MAL_MON_TRACE_NO_TIME_STAMP                  0x44
/* 32-bit timestamp required. */
#define MAL_MON_TRACE_EXT_TIME_STAMP_REQUESTED       0x47

/**
 * \struct mon_sb_trace_act_timestamp_t
 * \brief This Structure corresponds to subblock
 * \n     MON_SB_TRACE_ACTIVATION_TIMESTAMP
 * \n it is used for MAL_MON_TRACE_ACTIVATE_REQ
 */
typedef struct {

    uint8_t time_stamp; /**< time stamp value, one of:
                 * MAL_MON_TRACE_NO_TIME_STAMP
                 * MAL_MON_TRACE_EXT_TIME_STAMP_REQUESTED */

} mon_sb_trace_act_timestamp_t;


/**
 * \struct mon_trace_act_req_t
 * \brief This main Structure is for trace activation related feature requests.
 * \n it is used in MAL_MON_TRACE_ACTIVATE_REQ
 */
typedef struct {

    mon_sb_trace_act_bitmap_t sb_act_bitmap; /**< subblock: MON_SB_TRACE_ACTIVATION_BITMAP */
    mon_sb_trace_act_timestamp_t sb_act_timestamp; /**< subblock: MON_SB_TRACE_ACTIVATION_TIMESTAMP */
} mon_trace_act_req_t;




/**
 * \struct mon_trace_deact_req_t
 * \brief This main Structure is for MAL_MON_TRACE_DEACTIVATE_REQ.
 */
typedef struct {
    /* All traces sent over STM are deactivated. */
#define MAL_MON_DEACTIVATE_STM_TRACES                0x00
    /* Invariant traces sent over ISI interface are deactivated. */
#define MAL_MON_DEACTIVATE_ISI_INV_TRACES            0x01
    /* All traces sent over STM and ISI interfaces are deactivated. */
#define MAL_MON_DEACTIVATE_STM_ISI_TRACES            0x02
    uint8_t deactive;
} mon_trace_deact_req_t;

/**
 * \struct mon_trace_act_resp_t
 * \brief This main Structure is for trace response related feature requests.
 * \n it is applicable for
 * \n MAL_MON_TRACE_ACTIVATE_RESP
 * \n MAL_MON_TRACE_DEACTIVATE_RESP
 * \n MAL_MON_TRACE_CONFIG_SET_RESP
 * \n MAL_MON_TRACE_ROUTING_RESP
 * \n MAL_MON_TRACE_TRIGGER_RESP
 * \n MAL_MON_TRACE_IDS_ACTIVATE_ROUTE_RESP
 * \n
 */
typedef struct {
    /*  MAL_MON_RESPONSE_SYMBOLS  */
#define MAL_MON_RESPONSE_OK                          0x00
#define MAL_MON_RESPONSE_FAIL                        0x01
#define MAL_MON_CAUSE_INVALID_PARAMETER              0x02
#define MAL_MON_CAUSE_PERM_FAIL                      0x03
#define MAL_MON_CAUSE_WRONG_TRACE_TYPE               0x05
#define MAL_MON_CAUSE_PARTIAL_UPDATE                 0x0B
#define MAL_MON_CAUSE_UNKNOWN_ENTITY                 0x0A
    uint8_t status; /**< A subset of values from the constant table MAL_MON_RESPONSE_SYMBOLS */
} mon_trace_resp_t;

/**
 * \struct mon_sb_trace_ext_time_stamp_info_t
 * \brief  This Structure corresponds to subblock
 * MON_SB_TRACE_EXT_TIME_STAMP_INFO
 * \n it is used for MAL_MON_TRACE_INV_IND
 */

typedef struct {
    uint32_t extended_time_stamp; /**< OS Ticks timestamp  */
} mon_sb_trace_ext_time_stamp_info_t;

/**
 * \struct mon_sb_trace_inv_data_t
 * \brief  This Structure corresponds to subblock MON_SB_TRACE_INV_DATA
 * \n
 */
typedef struct {

    uint8_t trace_entity;    /**< trace entity value */
    uint8_t trace_grp;       /**< trace group value */
    uint8_t trace_id;        /**< trace id value */
    uint16_t trace_data_len; /**< length of trace data */
    /*  deallocate by receiver */
    uint8_t *trace_data;     /**< Protocol data, ENTRY_POINT_INVARIANT_TRACE */

} mon_sb_trace_inv_data_t;

/**
 * \name MON_TRACE_INV_IND
 * \par
 * Structures involved in case of MON TRACE ACTIVATE Indications
 */

/**
 * \struct mon_trave_inv_ind_t
 * \brief  This main structure is applicable for the Monitor trace
 * \n      invariant indications coming from modem
 */
typedef struct {
    /*  if n_sb = 0 nothing is filled */
    /*  if n_sb = 1  mon_sb_trace_inv_data is filled*/
    /*  if n_sb = 2  extended_time_stamp is also filled */
    uint8_t n_sb; /**< No. of subblocks inside response data, Possible values: 0,1,2 */
    mon_sb_trace_ext_time_stamp_info_t mon_sb_trace_ext_time_stamp_info;
    mon_sb_trace_inv_data_t mon_sb_trace_inv_data; /**< subblock:  MON_SB_TRACE_INV_DATA */
} mon_trace_inv_ind_t;



/**
 * \struct mon_trace_conf_set_req_t
 * \brief  This main structure is applicable for the Monitor trace
 * \n      MAL_MON_TRACE_CONFIG_SET_REQ  */
typedef struct {
    /*  deallocate by receiver */
    char *ascii;

} mon_trace_conf_set_req_t;


/**
 * \struct mon_trace_ids_act_route_req_t
 * \brief  This main structure is applicable for the Monitor trace
 * \n    MAL_MON_TRACE_IDS_ACTIVATE_ROUTE_REQ  */

typedef struct {
#define MAL_MON_TRACE_PROCESSOR_L23 0x0
#define MAL_MON_TRACE_PROCESSOR_L1 0x1
    uint8_t   processor; /* Values from the constant table MON_TRACE_PROCESSOR_SYMBOLS */
    uint8_t   trace_type; /* Values from the constant table MON_TRACE_TYPE_SYMBOLS */
    uint32_t  bitmap_entity;
#define MAL_MON_TRACE_OFF 0x0
#define MAL_MON_TRACE_ON 0x1
#define MAL_MON_TRACE_SHORT 0x2
#define MAL_MON_TRACE_EXTENDED 0x3
#define MAL_MON_TRACE_BUFFER_SHORT 0x4
#define MAL_MON_TRACE_BUFFER_MEDIUM 0x5
#define MAL_MON_TRACE_BUFFER_LONG 0x6
    uint8_t   action; /* Values from the constant table MON_TRACE_ACTIVATE_ROUTE_SYMBOLS */
    uint8_t   trace_count;
    /*  deallocate by receiver  */
    uint16_t  *trace_id;

} mon_trace_ids_act_route_req_t;

/**
 * \struct bitmap_t
 * \brief This  Structure is for MAL_MON_TRACE_ACTIVATION_READ_STATUS_REQ.
 * \n*/

typedef struct {
    uint8_t trace_type; /* Values from the constant table MON_TRACE_TYPE_SYMBOLS */
    uint8_t entity_id;
} bitmap_t;
/**
 * \struct mon_trace_act_read_status_req_t
 * \brief This main Structure is for MAL_MON_TRACE_ACTIVATION_READ_STATUS_REQ.
 * \n*/
typedef struct {
    uint8_t nb_bitmap;
    /*  deallocate by receiver  */
    bitmap_t *array_bitmap;
} mon_trace_act_read_status_req_t;


/**
 * \struct mon_trace_act_read_status_rsp_t
 * \brief This main Structure is for
 * MAL_MON_TRACE_ACTIVATION_READ_STATUS_RESP.
 */
typedef struct {
    uint8_t status;
    uint8_t nb_subblocks;
    /*  deallocate by receiver  */
    mon_sb_trace_act_bitmap_t *array_act;
} mon_trace_act_read_status_rsp_t;

/**
 * \struct mon_trace_conf_t
 * \brief This main Structure is used in
 * \n MAL_MON_TRACE_CONFIG_GET_RESP.
 */

typedef struct {
    /*  deallocate by receiver */
    char *ascii;
} mon_trace_conf_t;

/**
 * \struct mon_trace_config_get_rsp_t
 * \brief This main Structure is for
 * \n MAL_MON_TRACE_CONFIG_GET_RESP.
 */
typedef struct {
    uint8_t version;
    char *cur_ascii;
} mon_trace_config_get_rsp_t;


/**
 * \struct mon_trace_log_trigger_ind_t
 * \brief  This main structure is applicable for the Monitor trace
 * \n  MAL_MON_TRACE_LOG_TRIGGER_IND   */

typedef struct {
    uint16_t trigger_id;
} mon_trace_log_trigger_ind_t;

/**
 * \struct mon_trace_flush_ind_t
 * \brief  This main structure is applicable for the Monitor trace
 * \n    MAL_MON_TRACE_BUFFER_FLUSH_IND
 * \n*/
typedef struct  {
    uint8_t filler;
#define MAL_MON_TRACE_BUFFER_LONG_L1 0x0
#define MAL_MON_TRACE_BUFFER_LONG_L23 0x1
#define MAL_MON_TRACE_BUFFER_MEDIUM_L1 0x2
#define MAL_MON_TRACE_BUFFER_MEDIUM_L23 0x3
    uint8_t buffer_id;
    uint32_t address;
    uint32_t size;
} mon_trace_flush_ind_t;

/**
 * \struct mon_trace_route_req_t
 * \brief  This main structure is applicable for the Monitor trace
 * \n    MAL_MON_TRACE_ROUTING_REQ
 * \n*/
typedef struct  {
#define MAL_MON_TRACE_ROUTING_NO_TRACE 0x0
#define MAL_MON_TRACE_ROUTING_XTI 0x1
#define MAL_MON_TRACE_ROUTING_BUFFER 0x2
#define MAL_MON_TRACE_ROUTING_XTI_BUFFER 0x3
    uint8_t routing;
} mon_trace_routing_req_t;

/**
 * \struct mon_trace_route_req_t
 * \brief  This main structure is applicable for the Monitor trace
 * \n    MAL_MON_TRACE_TRIGGER_REQ
 * \n*/
typedef struct  {
#define MAL_MON_TRIGGER_START 0x0
#define MAL_MON_TRIGGER_STOP 0x1
    uint8_t action;
} mon_trace_trigger_req_t;

/**
 * \struct mon_trace_buffer_marker_req_t
 * \brief  This main structure is applicable for the Monitor trace
 * \n    MAL_MON_TRACE_BUFFER_MARKER_REQ
 * \n*/
typedef struct  {
#define MAL_MON_MARKER_START 0x0
#define MAL_MON_MARKER_END 0x1
#define MAL_MON_MARKER_MESSAGE 0x2
    uint8_t action;
    uint8_t timestamp[8];
} mon_trace_buffer_marker_req_t;


/**
 * \struct mon_trace_buffering_mode_t
 * \brief This main structure is applicable for the Monitor trace
 * \n MAL_MON_TRACE_BUFFERING_MODE_REQ,
 * \n MAL_MON_TRACE_BUFFERING_MODE_RESP,
 * \n MAL_MON_TRACE_BUFFERING_MODE_READ_REQ and
 * \n MAL_MON_TRACE_BUFFERING_MODE_READ_RESP
 */
typedef struct {
#define MAL_MON_FLUSHING_MODE 0x00
#define MAL_MON_SDRAM_ONLY_MODE 0x01
    uint8_t mode;
} mon_trace_buffering_mode_t;

/**
 * \name Function/Service list
 * \par
 * Function/Service list of MONITOR MAL library
 */

/* ----------------------------------------------------------------------- */
/**
 * \fn  int mal_mon_sub(void)
 *
 * \brief MAL_API -  subscribe to the indication messages socket
 *
 * \return 1 (SUCCESS)
 */
/* ----------------------------------------------------------------------- */
int mal_mon_sub(void);

/* ----------------------------------------------------------------------- */
/**
 * \fn  int mal_mon_sub_modem_trigger(void)
 *
 * \brief MAL_API -  subscribe to the modem generated triggers
 *
 * \return 0 on success, -1 on error.
 */
/* ----------------------------------------------------------------------- */
int mal_mon_sub_modem_trigger(void);

/* ----------------------------------------------------------------------- */
/**
 * \fn  int32_t mal_mon_init(int32_t *fd_mon)
 *
 * \brief MAL_API - Startup function for using MONITOR MAL.
 *
 * \par
 * \n Initializes MON MAL data structures and state,
 * \n Creates Phonet Socket Session.
 *
 * \param [out] fd_mon points to file descriptor of mon socket
 * \return  - \ref mal_error_type (SUCCESS/FAIL)
 */
/* ----------------------------------------------------------------------- */
int32_t    mal_mon_init(int32_t *fd_mon);


/* ----------------------------------------------------------------------- */
/**
 * \fn int32_t    mal_mon_config(void);
 * \brief MAL-API - Configures MON MAL Parameters.
 *
 * \par
 * \n Reads default config parameters or Sets specific config parameters
 * \n This function should be called only once and immediately
 * \n after mal_mon_init()
 *
 * @return  - \ref mal_error_type (SUCCESS/FAIL)
 */
/* ----------------------------------------------------------------------- */
int32_t    mal_mon_config(int32_t *fd_mon);


/* ----------------------------------------------------------------------- */
/**
 * \typedef  void (*mal_mon_event_cb_t)(int32_t message_id, void *data,
 * \n             mal_error_type  mal_error, void *client_tag);
 * \brief Event notification to MAL MONITOR server
 * \param [in]  message_id of solicited/unsolicited event.
 * \param [in]  data pointer to parameter values associated with this event
 * \param [in]  mal_error error type associated with this event
 * \param [in]  Client tag associated with the response or indication
 */
/* ---------------------------------------------------------------------- */
typedef    void (*mal_mon_event_cb_t)(int32_t message_id, void *data,
                                      mal_error_type  mal_error, void *client_tag);

/* ----------------------------------------------------------------------- */
/**
 * \fn int32_t    mal_mon_register_callback(mal_mon_event_cb_t event_cb);
 * \brief MAL_API - Register Callback.
 *
 * \param [in] mal_mon_event_cb_t - Callback function pointer
 * \return  - \ref mal_error_type (SUCCESS/FAIL)
 */
/* ----------------------------------------------------------------------- */
int32_t    mal_mon_register_callback(mal_mon_event_cb_t event_cb);


/* ----------------------------------------------------------------------- */
/**
 * \fn int32_t    mal_mon_deinit(void);
 * \brief MAL_API - Deinitializes MON Mal.
 * \return  - \ref mal_error_type (SUCCESS/FAIL)
 */
/* ----------------------------------------------------------------------- */
int32_t    mal_mon_deinit(void);

/**
 * \fn int32_t    mal_mon_request(int32_t message_id,  void *data);
 * \brief MAL_API -  MON Mal Request.
 *
 * \param [in] messageID  - Message ID
 * \param [in] data       - Message Data pointer
 * \param [in] client_tag - Specific client tag value passed from the user
 * \return  - \ref mal_error_type (SUCCESS/FAIL)
 */
/* ----------------------------------------------------------------------- */
int32_t    mal_mon_request(int32_t message_id,  void *data, void *client_tag);


/* ----------------------------------------------------------------------- */
/**
 * \fn void mal_mon_response_handler();
 * \brief MAL-API - Monitor Response and Indication Handler
 *
 * \return  - void
 */
/* ----------------------------------------------------------------------- */
void mal_mon_response_handler(int32_t dgram);

/**
 *  \fn int32_t mal_mon_request_set_debug_level(uint8_t level)
 *  \brief  This Api is used to set the debug level for gss  module. This is a synchronous call.
 *  \param [in] level  0 - No Prints; 1 - Entry & Exit Prints; 2 - Value/Info Prints; 3 - ISI MSG Prints
 *                               4 - All Prints
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_mon_request_set_debug_level(uint8_t level);

/**
 *  \fn int32_t mal_mon_request_get_debug_level(uint8_t *level)
 *  \brief  This Api is used to get the debug level for gss module. This is a synchronous call.
 *   \param[out] debug level for gss  module. data type: pointer to uint8_t.
 *                       Caller needs to take care of memory allocation
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_mon_request_get_debug_level(uint8_t *level);


#endif /*MON_MAL_API_H*/
