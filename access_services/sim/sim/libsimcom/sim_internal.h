/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : sim_internal.h
 * Description     : Internal definitions.
 *
 * Author          : Jessica Nilsson <jessica.j.nilsson@stericsson.com>
 *
 */

#ifndef __sim_internal_h__
#define __sim_internal_h__ (1)

#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>

#include "sim.h"
#include "event_stream.h"
#include "msgq.h"
#include <string.h>

/**
 * Macro that return the smaller value of two values.
 *
 * This version of the macro is NOT double-evaluation safe, e.g.
 * calls similar to MIN(++a, b++) are UNSAFE...
 *
 * It does however comply with ISO C restrictions on braced groups within expressions.
 */
#ifndef MIN
#define MIN(x, y) (x < y ? x : y)
#endif

// The 16 bit "command" code used by SIM, CAT and UICC is divided into
// 4 parts:
// bits 0..7 (mask=0x00ff)      Decode the individual values for SIM,CAT,UICC
// bits 8..9 (mask=0x0300)      Message kind:{0,1,2,3}={REQ,RSP,IND,not used}
// bits 10..11 (mask=0x0c00)    Always 0
// bits 12..15 (mask=0xf000)    {0,1,2,...}={SIM,CAT,UICC,unused}

#define STE_SIM_IS_REQ(X) (((X)&0x0300)==0x0000)
#define STE_SIM_IS_RSP(X) (((X)&0x0300)==0x0100)
#define STE_SIM_IS_IND(X) (((X)&0x0300)==0x0200)


#define STE_SIM_RANGE_MASK                      (0xff00)
#define STE_SIM_RANGE_REQ                       (0x0000)
#define STE_SIM_RANGE_RSP                       (0x0100)
#define STE_SIM_RANGE_IND                       (0x0200)

typedef enum {
    STE_SIM_REQ_PING = (STE_SIM_RANGE_REQ | 0x0001),
    STE_SIM_REQ_STARTUP,
    STE_SIM_REQ_SHUTDOWN,
    STE_SIM_REQ_SET_SIM_DEBUG_LEVEL,
    STE_SIM_REQ_SET_MODEM_DEBUG_LEVEL,
} ste_sim_req_t;
typedef enum {
    STE_SIM_RSP_PING = (STE_SIM_RANGE_RSP | 0x0001),
    STE_SIM_RSP_STARTUP,
    STE_SIM_RSP_SHUTDOWN,
    STE_SIM_RSP_SET_SIM_DEBUG_LEVEL,
    STE_SIM_RSP_SET_MODEM_DEBUG_LEVEL,
} ste_sim_rsp_t;

#ifndef TRUE
#define TRUE (1)
#endif
#ifndef FALSE
#define FALSE (0)
#endif

typedef enum {
    ste_sim_state_disconnected,
    ste_sim_state_connected
} ste_sim_lib_state_t;


typedef struct {
    uint16_t    expected_response_id;    /* What we wait for */
    void*       vdata;                   /* Response dependent data */
    int         is_sync;                 /* 1/0 for sync/async */
    int         done;                    /* set to 1 by es cb func */
    int         rv;                      /* Return value */
    uintptr_t   client_tag;              /* Client tag */
} ste_sim_sync_data_t;

/* Info interface needs about the client */
struct ste_sim_s {
    ste_sim_lib_state_t     state;          /* Connection state; on or off */
    ste_sim_closure_t       closure;        /* Closure for call back */
    int                     thread_pipe;    /* -1 if no thread is running */
    int                     fd;             /* Socket file descriptor */
    int                     is_st;          /* 0/1 if we are single threaded */
    pthread_t               rdr_tid;        /* Reader thread (-1 if not connected in multi-thread mode) */
    ste_es_t                *es;            /* Event stream used in single threaded mode */
    ste_sim_sync_data_t     sync_data;      /* Used in sync mode */
};

typedef int            ste_sim_transaction_handler_func(ste_msg_t *);

typedef struct {
    uintptr_t                          client_tag;               /* Client tag received from client */
    uintptr_t                          sim_tag;                  /* Sim's own client tag */
    int                                fd;                       /* Socket file descriptor */
    uint32_t                           transaction_id;           /* transaction id, = msg id passed from client */
    ste_sim_transaction_handler_func * transaction_handler_func; /* handler function for the transaction */
    void                             * transaction_data_p;       /* Data for a transaction */
} ste_sim_ctrl_block_t;

ste_sim_ctrl_block_t * ste_sim_create_ctrl_block(uintptr_t client_tag,
                                                 int fd,
                                                 uint32_t trans_id,
                                                 ste_sim_transaction_handler_func* handler_func,
                                                 void * data_p);


void ste_sim_i_safe_copy(char *dst, const char *src, size_t n);

/**
 * @brief Copies the source memory into the destination memory.
 * @param dst	Destination memory area.
 * @param vsrc	Source memory area.
 * @param n	Size of vsrc as the integral data type returned
 *              by the language operator sizeof.
 * @return      Destination memory area returned. Destination will be empty if n is zero.
 */
char                   *sim_enc(char *dst, const void *vsrc, size_t n);


/**
 * @brief Copy n size of src into vdst and step src forward n steps.
 * @param src	Source memory area.
 * @param vdst  Destination memory area.
 * @param n	Size of vsrc as the integral data type returned
 *              by the language operator sizeof.

 * @param smax	Max allowed length of src.
 * @return	Return src or 0 if src < smax.
 */
const char             *sim_dec(const char *src, void *vdst, size_t n,
                                const char *smax);


/**
 * @brief This function sends a command, payload and client tag to a destination pipe or socket.
 * @param fd            The receiver of the message
 * @param cmd           The command
 * @param payload       The command data
 * @param client_tag    The original client tag
 * @return 0 if successful, -1 on error (no memory, or write fail)
 */
int sim_send_generic(int fd, uint16_t cmd, const char *payload, size_t n, uintptr_t client_tag);


/**
 * @brief This function sends a command, payload and client tag to all registered clients.
 * This is basically the same as @ref sim_send_generic
 *
 * @param cmd           The command
 * @param payload       The command data
 * @param client_tag    The original client tag
 * @return 0 if successful, -1 on error (no memory)
 */
int sim_send_generic_all(uint16_t cmd, const char *payload, size_t n, uintptr_t client_tag);


/**
 * @brief Creates a thread in a detached state.
 * @param tid
 * @param detach_state Should be set to either PTHREAD_CREATE_DETACHED or PTHREAD_CREATE_JOINABLE.
 * @param arg	Thread arg for start_routine.
 * @return	If successful return zero; otherwise, an error number shall be returned to indicate the error..
 */
int                     sim_launch_thread(pthread_t * tid,
                                          int detach_state,
                                          void *(*func) (void *),
                                          void *arg);

/**
 * @brief Create a pipe, read non blocking, write blocking.
 * @param wfs	Filedescriptor write end.
 * @param rfs	Filedescriptor read end.
 * @return	On success, zero is returned. On error non-zero is returned.
 */
int                     sim_create_pipe(int *wfs, int *rfs);


/**
 * @brief Copy the client's call control data into CAT's own memory.
 * @param dest_p	The pointer to save the data.
 * @param source_p	The pointer to the source of CC data.
 * @return	On success, zero is returned. On error non-zero is returned.
 */
//int                     sim_copy_call_control_data(ste_cat_call_control_t * dest_p, ste_cat_call_control_t * source_p);


//Tags to encode/decode the call control data
#define SIM_CC_TAG_CC_TYPE                   (0x01)
#define SIM_CC_TAG_TON                       (0x02)
#define SIM_CC_TAG_NPI                       (0x03)
#define SIM_CC_TAG_CODING                    (0x04)
#define SIM_CC_TAG_NO_OF_CHARACTERS          (0x05)
#define SIM_CC_TAG_DIAL_STR_TEXT             (0x06)
#define SIM_CC_TAG_DCS                       (0x07)
#define SIM_CC_TAG_SUB_ADDRESS               (0x08)
#define SIM_CC_TAG_CCP_1                     (0x09)
#define SIM_CC_TAG_CCP_2                     (0x0A)
#define SIM_CC_TAG_BC_REPEAT_IND             (0x0B)
#define SIM_CC_TAG_NO_OF_BYTES               (0x0C)
#define SIM_CC_TAG_SMSC                      (0x0D)
#define SIM_CC_TAG_DEST                      (0x0E)

#define SIM_CC_RSP_TAG_CC_RESULT             (0x0F)
#define SIM_CC_RSP_TAG_CC_INFO               (0x10)
#define SIM_CC_RSP_TAG_USER_INDICATION       (0x11)
#define SIM_CC_RSP_TAG_USER_IND_CODING       (0x12)
#define SIM_CC_RSP_TAG_USER_IND_NO_OF_BYTES  (0x13)
#define SIM_CC_RSP_TAG_USER_IND_STRING       (0x14)

//Tags to encode/decode the event download data
#define SIM_EVENT_TAG_TRANSACTION_ID         (0x01)
#define SIM_EVENT_TAG_TON                    (0x02)
#define SIM_EVENT_TAG_NPI                    (0x03)
#define SIM_EVENT_TAG_CODING                 (0x04)
#define SIM_EVENT_TAG_NO_OF_CHARACTERS       (0x05)
#define SIM_EVENT_TAG_TEXT                   (0x06)
#define SIM_EVENT_TAG_SUB_ADDRESS            (0x07)


/* PLMN bit masks used for copying raw PLMN data into the SIM_PLMN_t Defn. */
#define SIM_PLMN_HIGHNIBBLE_MASK    (0xF0)  /* 1111 0000 */
#define SIM_PLMN_LOWNIBBLE_MASK     (0x0F)  /* 0000 1111 */

// Number of bytes occupied by PLMN Access Technology data on SIM
#define SIM_PLMN_ACCESS_TECH_SIZE  (2)
// Number of bytes occupied by PLMN data on SIM
#define SIM_PLMN_SIZE              (3)
/* Number of bytes ocupied by PLMN and Access Technology data on SIM*/
#define SIM_PLMN_WITH_ACT_SIZE (SIM_PLMN_ACCESS_TECH_SIZE + SIM_PLMN_SIZE)
/* Default PLMN Access Technology */
#define SIM_PLMN_DEFAULT_ACT (0x8080)

/**
 * @brief Encode the client's call control data into a binary string to send over socket.
 * @param cc_p	        The pointer to the source of CC data.
 * @param len_p 	The length of the encoded data.
 * @return	On success, the created binary string pointer is returned. On error NULL is returned.
 */
char* sim_encode_call_control_data(ste_cat_call_control_t * cc_p, size_t * len_p);

/**
 * @brief Decode the client's call control binary data from socket into a call control data structure.
 * @param cc_p	        The pointer to the destination of CC data.
 * @param buf_p   	The pointer to the binary data to be decoded.
 * @param len   	The length of the binary data.
 * @return	On success, zero is returned. On error non-zero is returned.
 */
int sim_decode_call_control_data(ste_cat_call_control_t * cc_p, uint8_t * buf_p, size_t len);

/**
 * @brief Copy the call control data structure from source to dest.
 * @param dest_p        The pointer to the CC data to be copied to.
 * @param source_p      The pointer to the CC data to be copied from.
 * @return	On success, zero is returned. On error non-zero is returned.
 */
int sim_copy_call_control_data(ste_cat_call_control_t * dest_p, ste_cat_call_control_t * source_p);

/**
 * @brief Free the client's call control data structure, its members.
 * @param cc_p	        The pointer to the CC data to be freed.
 * @return	Void.
 */
void sim_free_call_control_data(ste_cat_call_control_t * cc_p);

/**
 * @brief Encode the returned call control response data into a binary string to send over socket.
 * @param cc_result     The call control result.
 * @param cc_rsp_p      The pointer to the source of CC response data.
 * @param result_len_p 	The length of the encoded data.
 * @return	On success, the created binary string pointer is returned. On error NULL is returned.
 */
char * ste_sim_encode_cc_response(ste_cat_call_control_result_t     cc_result,
                                  ste_cat_call_control_response_t * cc_rsp_p,
                                  size_t * result_len_p);

/**
 * @brief Decode the call control response from CATD.
 * @param ec_status_p	The pointer to the EC status from modem.
 * @param cc_rsp_p	The pointer to the destination of CC response data.
 * @param data_p        The binary string to be decoded.
 * @param data_len      Length of the binary string to be decoded.
 * @return	Void.
 */
int  ste_sim_decode_cc_response(int * ec_status_p,
                                ste_cat_call_control_response_t * cc_rsp_p,
                                const char * data_p,
                                size_t data_len);

/**
 * @brief Free the call control response data structure, and its members.
 * @param cc_rsp_p      The pointer to the CC data to be freed.
 * @return	Void.
 */
void sim_free_call_control_response_data(ste_cat_call_control_response_t * cc_rsp_p);


/**
 * @brief Encode the client's sms control data into a binary string to send over socket.
 * @param cc_p	        The pointer to the source of sms control data.
 * @param len_p 	The length of the encoded data.
 * @return	On success, the created binary string pointer is returned. On error NULL is returned.
 */
char* sim_encode_sms_control_data(ste_cat_sms_control_t * sc_p, size_t * len_p);

/**
 * @brief Decode the client's sms control binary data from socket into a sms control data structure.
 * @param cc_p	        The pointer to the destination of sms control data.
 * @param buf_p   	The pointer to the binary data to be decoded.
 * @param len   	The length of the binary data.
 * @return	On success, zero is returned. On error non-zero is returned.
 */
int sim_decode_sms_control_data(ste_cat_sms_control_t * sc_p, uint8_t * buf_p, size_t len);

/**
 * @brief Copy the sms control data structure from source to dest.
 * @param dest_p        The pointer to the sms control data to be copied to.
 * @param source_p      The pointer to the sms control data to be copied from.
 * @return	On success, zero is returned. On error non-zero is returned.
 */
int sim_copy_sms_control_data(ste_cat_sms_control_t * dest_p, ste_cat_sms_control_t * source_p);

/**
 * @brief Free the client's sms control data structure, its members.
 * @param sc_p	        The pointer to the sms control data to be freed.
 * @return	Void.
 */
void sim_free_sms_control_data(ste_cat_sms_control_t * sc_p);

/**
 * @brief Encode the returned sms control response data into a binary string to send over socket.
 * @param sc_result     The sms control result.
 * @param sc_rsp_p      The pointer to the source of SMS control response data.
 * @param result_len_p 	The length of the encoded data.
 * @return	On success, the created binary string pointer is returned. On error NULL is returned.
 */
char * ste_sim_encode_sc_response(ste_cat_sms_control_result_t     sc_result,
                                  ste_cat_sms_control_response_t * sc_rsp_p,
                                  size_t * result_len_p);

/**
 * @brief Decode the sms control response from CATD.
 * @param ec_status_p	The pointer to the EC status from modem.
 * @param sc_rsp_p	The pointer to the destination of SMS control response data.
 * @param data_p        The binary string to be decoded.
 * @param data_len      Length of the binary string to be decoded.
 * @return	Void.
 */
int  ste_sim_decode_sc_response(int * ec_status_p,
                                ste_cat_sms_control_response_t * sc_rsp_p,
                                const char * data_p,
                                size_t data_len);

/**
 * @brief Free the sms control response data structure, and its members.
 * @param sc_rsp_p      The pointer to the SMS control data to be freed.
 * @return	Void.
 */
void sim_free_sms_control_response_data(ste_cat_sms_control_response_t * sc_rsp_p);



//Declarations and Definitions for SMSP file
/**
 * SMR_MAX_SM_ADDRESS_LENGTH is maximum address length in bytes.
 */
#define SMR_MAX_SM_ADDRESS_LENGTH       (10)

/**
 * Short Message Address.
 *
 * @param length          Length of Address excluding the type of address,
 *                        i.e. the number of bytes used in addr_value.
 * @param addr_type       Type of Address.
 * @param addr_value      TS-Service Centre Address as in TS 24.011 excluding the Ton/Npi byte.
 */
typedef struct
{
  uint8_t length;
  uint8_t addr_type;
  uint8_t addr_value[SMR_MAX_SM_ADDRESS_LENGTH];
} ste_sim_short_message_address_t;

/**
 * Structure containing mandatory SMS Parameters record data.
 *
 * @param indicators                See description of Parameter Indicators. TS 51.011, Chapter 10.5.6
 * @param destination_address       Refer to SM-TL Address in TS 23.040.
 * @param service_center_address    Refer to RP-Destination Address in TS 24.011.
 * @param protocol_id               Refer to TS 23.040.
 * @param coding_scheme             Refer to TS 23.038.
 * @param validity_period           Refer to time format in TS 23.040.
 */
typedef struct
{
  uint8_t                            indicators;
#define STE_SIM_SMSP_DEST_ADDRESS_ABSENT             0x01
#define STE_SIM_SMSP_SC_ADDRESS_ABSENT               0x02
#define STE_SIM_SMSP_PROTOCOL_ID_ABSENT              0x04
#define STE_SIM_SMSP_CODING_SCHEME_ABSENT            0x08
#define STE_SIM_SMSP_VALID_PERIOD_ABSENT             0x10
#define STE_SIM_SMSP_BIT_RESERVED                    0xE0
  ste_sim_short_message_address_t    destination_address;
  ste_sim_short_message_address_t    service_center_address;
  uint8_t                            protocol_id;
  uint8_t                            coding_scheme;
  uint8_t                            validity_period;
} ste_sim_smsp_t;

/**
 * @brief Converts one UCS2 character to a (Low) BCD 4-bit nibble.
 *
 * This is according to GSM 11.11. Extended BCD is supported.
 *
 * @param UCS2_char The character to be converted. Extended BCD is supported.
 * @return  The BCD character stored in the low nibble.
 */
uint8_t utility_UCS2char_to_BCD (uint16_t UCS2_char);

/**
 * @brief Converts one BCD character stored in a low nibble to ASCII8.
 *
 * This is according to GSM 11.11. Extended BCD is supported.
 *
 * @param bcd The character to be converted.
 * @return  The corresponding ASCII8 character.
 */
uint8_t utility_BCD_to_ASCII8(uint8_t bcd);

/**
 * @brief Convert a sim UCS2 string into BCD format.
 * @param source_text_p	The pointer to the text data to be converted.
 * @param max_len   	Length of output buffer in bytes.
 * @param actual_len_p  The actual length of the converted data.
 * @param BCD_Text_p  	The pointer to the result, the format is according to GSM 11.11.
 * @return	On success, zero is returned. On error non-zero is returned.
 */
int utility_UCS2_to_BCD( ste_sim_text_t  * source_text_p,
                         uint8_t         * BCD_text_p,
                         uint16_t          max_len,
                         uint16_t        * actual_len_p);

/**
 * @brief Convert a sim ASCII8 string into UCS2 format.
 * @param source_text_p	The pointer to the text data to be converted.
 * @param max_len   	Length of output buffer in bytes.
 * @param actual_len_p  The actual length of the converted data.
 * @param UCS2_Text_p  	The pointer to the result, the format is according to GSM 11.11/TS 102.221.
 * @return	On success, zero is returned. On error non-zero is returned.
 */
int utility_ASCII8_to_UCS2(ste_sim_text_t  * source_text_p,
                           uint16_t        * UCS2_text_p,
                           uint16_t          max_len,
                           uint16_t        * actual_len_p);

/**
 * @brief Convert a sim ASCII8 string into BCD format.
 * @param source_text_p The pointer to the text data to be converted.
 * @param BCD_text_p    The pointer to the output buffer, the format is according to GSM 11.11/TS 102.221.
 * @param max_len       The size of the output buffer (in bytes).
 * @param actual_len_p  The actual length of the converted data (output from conversion).
 * @return  On success, zero is returned. On error non-zero is returned.
 */
int utility_ASCII8_to_BCD(ste_sim_text_t* source_text_p,
                          uint8_t*        BCD_text_p,
                          uint16_t        max_len,
                          uint16_t*       actual_len_p);


/**
 * @brief Convert a sim UCS2 string into BCD format.
 * @param source_p   	The pointer to the text data to be converted.
 * @param max_len   	Length of output buffer in bytes.
 * @param actual_len_p  The actual length of the converted data.
 * @param buf_p   	The pointer to the result, the format is according to GSM 11.11/TS 102.221.
 * @return	On success, zero is returned. On error non-zero is returned.
 */
int sim_convert_UCS2_to_BCD(ste_sim_text_t  * source_p,
                            uint16_t          max_len,
                            uint16_t        * actual_len_p,
                            uint8_t         * buf_p);

/**
 * @brief Convert a sim ASCII8 string into BCD format.
 * @param source_p   	The pointer to the text data to be converted.
 * @param max_len   	Length of output buffer in bytes.
 * @param actual_len_p  The actual length of the converted data.
 * @param buf_p   	The pointer to the result, the format is according to GSM 11.11/TS 102.221.
 * @return	On success, zero is returned. On error non-zero is returned.
 */
int sim_convert_ASCII8_to_BCD(ste_sim_text_t  * source_p,
                              uint16_t          max_len,
                              uint16_t        * actual_len_p,
                              uint8_t         * buf_p);

/**
 * @brief Convert a sim BCD string into ASCII8 format.
 * @param source_p   	The pointer to the text data to be converted.
 * @param max_len   	Length of output buffer in bytes.
 * @param actual_len_p  The actual length of the converted data.
 * @param buf_p   	The pointer to the result
 * @return	On success, zero is returned. On error non-zero is returned.
 */
int sim_convert_BCD_to_ASCII8(ste_sim_text_t  * source_p,
                              uint16_t          max_len,
                              uint16_t        * actual_len_p,
                              uint8_t         * buf_p);

/**
 * @brief Get the length in bytes for a BCD format string.
 * @param nr_char   	The number of characters for the BCD string.
 * @return	The number in bytes for the BCD string is returned.
 */
uint8_t get_length_in_bytes_for_BCD(uint8_t nr_char);

/**
 * @brief Get the length in digits for a BCD format string.
 * @param buf    buffer with the left-justified BCD string.
 * @param len    buffer size
 * @return	The number of digits for the BCD string is returned.
 *
 * Buffer may be larger than necessary, the routine will end its count
 * when reaching a non-bcd nibble, or end of buffer, whichever occurs first
 */
uint8_t get_length_in_digits_for_BCD(uint8_t* buf, size_t len);
/**
 * @brief Convert a sim address structure to a ASCII8 string required by RIL.
 * @param address_p   	The pointer to the structure data to be converted.
 * @param smsc_p   	    The pointer to the result.
 * @return	On success, zero is returned. On error non-zero is returned.
 */
int sim_create_ril_smsc_str(ste_sim_call_number_t * smsc_p, ste_sim_short_message_address_t * address_p);

/**
 * @brief Does the reverse of sim_create_ril_smsc_str.
 * @param smsc_p    The pointer to the structure data to be converted.
 * @param address_p The pointer to the result.
 * @return  On success, zero is returned. On error non-zero is returned.
 */
int sim_undo_create_ril_smsc_str(ste_sim_short_message_address_t * address_p, ste_sim_call_number_t * smsc_p);

/**
 * @brief Encode a sim address structure into a string required by read record.
 * @param result_len_p 	Created string length.
 * @param smsc_p   	The pointer to the input call number.
 * @return	On success, the result pointer is returned. On error NULL is returned.
 */
uint8_t * sim_encode_smsp_record(ste_sim_call_number_t * smsc_p, int * result_len_p, ste_sim_smsp_t * smsp_p, uint8_t rec_len);

/**
 * @brief Decode the smsp binary data from uicc into a smsp data structure.
 * @param smsp_p        The pointer to the destination of smsp data.
 * @param data_p   	The pointer to the binary data to be decoded.
 * @param len   	The length of the binary data.
 * @return	On success, zero is returned. On error non-zero is returned.
 */
int sim_decode_smsp_record(ste_sim_smsp_t * smsp_p, uint8_t * data_p, size_t len);

/**
 * @brief get the bytes length for a text structure.
 * @param source_p      The pointer to the text structure to be calculated.
 * @return	On success, the length of the text is returned. On error zero is returned.
 */
size_t sim_get_text_string_length(ste_sim_text_t  * source_p);

/**
 * @brief encode the smsc info into a binary string, to be sent via socket.
 * @param smsc_p        The pointer to the smsc data to be encoded.
 * @param result_len_p  The length of the encoded result.
 * @return	On success, the pointer to the encoded string is returned. On error NULL is returned.
 */
char * sim_encode_smsc_string(ste_sim_call_number_t * smsc_p, size_t * result_len_p);

/**
 * @brief decode the binary from socket into smsc structure info.
 * @param smsc_p        The structure to hold the decoded info.
 * @param data_p        The binary string to be decoded.
 * @param data_len      Length of the binary string to be decoded.
 * @return	On success, the pointer to the encoded string is returned. On error NULL is returned.
 */
void sim_decode_smsc_string(ste_sim_call_number_t * smsc_p, const char * data_p, size_t data_len);

/**
 * @brief decode the binary from socket into read generic structure info.
 * @param result_p      The structure to hold the decoded info.
 * @param data_p        The binary string to be decoded.
 * @param data_len      Length of the binary string to be decoded.
 * @return	void.
 */
void sim_decode_read_generic_result(ste_uicc_sim_file_read_generic_response_t * result_p, const char * data_p, size_t data_len);

/**
 * @brief put the data read from imsi file into the response structure to be passed to client.
 * @param imsi_p        The structure to hold the created response info.
 * @param data_p        The binary string to be put into the response.
 * @param data_len      Length of the binary string.
 * @return	void.
 */
void sim_create_imsi_response(ste_uicc_sim_file_read_imsi_response_t * imsi_p, uint8_t * data_p, size_t data_len);

/**
 * @brief encode ton and npi information into one byte required by the spec.
 * @param ton           Type of Number.
 * @param npi           Numbering plan id.
 * @param buf_p         The Result byte.
 * @return	On success, zero is returned. On error non-zero is returned.
 */
int sim_encode_ton_npi(ste_sim_type_of_number_t ton, ste_sim_numbering_plan_id_t npi, uint8_t * buf_p);

/**
 * @brief decode ton and npi information from the one byte in the format of the spec.
 * @param ton_p         Result of Type of Number.
 * @param npi_p         Result of Numbering plan id.
 * @param ton_npi       The ton npi byte in the format of spec.
 * @return	On success, zero is returned. On error non-zero is returned.
 */
int sim_decode_ton_npi(uint8_t ton_npi, ste_sim_type_of_number_t * ton_p, ste_sim_numbering_plan_id_t * npi_p);

/**
 * @brief decode the binary from socket into a general status info.
 * @param status_p      The variable to hold the decoded status info.
 * @param data_p        The binary string to be decoded.
 * @param data_len      Length of the binary string to be decoded.
 * @return	void.
 */
void ste_sim_decode_status_response(int * status_p, const char * data_p, size_t data_len);

/**
 * @brief decode the binary from socket into a string and the length of the string
 * @param buf_len_p     The length of the result buffer.
 * @param data_p        The binary string to be decoded.
 * @param data_len      Length of the binary string to be decoded.
 * @return	On success, The buffer to hold the decoded string. On error, NULL is returned
 */
char * ste_sim_decode_length_data(size_t * buf_len_p, const char * data_p, size_t data_len);

/**
 * @brief encode the provided string and length info into a binary to be sent by socket
 * @param buf_len_p     The length of the result buffer.
 * @param data_p        The binary string to be encoded.
 * @param data_len      Length of the binary string to be encoded.
 * @return	On success, The buffer to hold the encoded binary is returned. On error, NULL is returned
 */
char * ste_sim_encode_length_data(size_t * buf_len_p, const char * data_p, size_t data_len);

/**
 * @brief decode the binary from socket into a PC ind data structure
 * @param pc_p          The decoded result, PC ind data structure.
 * @param data_p        The binary string to be decoded.
 * @param data_len      Length of the binary string to be decoded.
 * @return	void.
 */
void sim_decode_pc_ind(ste_cat_pc_ind_t * pc_p, const char * data_p, size_t data_len);

/**
 * @brief decode the binary from socket into a EC cat response structure
 * @param ec_p          The decoded result, EC cat response structure.
 * @param data_p        The binary string to be decoded.
 * @param data_len      Length of the binary string to be decoded.
 * @return	void.
 */
void ste_sim_decode_cat_response(ste_cat_ec_response_t * ec_p, const char * data_p, size_t data_len);

/**
 * @brief decode the binary from socket into a plmn response structure
 * @param plmn_p        The decoded result, plmn response structure.
 * @param data_p        The binary string to be decoded.
 * @param data_len      Length of the binary string to be decoded.
 * @return  void.
 */
void ste_sim_decode_plmn_response(ste_uicc_sim_file_read_plmn_response_t * plmn_p, const char * data_p, size_t data_len);

/**
 * @brief encode the plmn info into a binary string, to be sent via socket.
 * @param plmn_p        The pointer to the plmn data to be encoded.
 * @param index         The index to be updated.
 * @param result_len_p  The length of the encoded result.
 * @return  On success, the pointer to the encoded string is returned. On error NULL is returned.
 */
char *sim_encode_plmn_element(ste_sim_plmn_with_AcT_t *plmn_p, int index, ste_uicc_sim_plmn_file_id_t file_id, size_t *result_len_p);

/**
 * @brief Decode the binary from socket into plmn structure info.
 * @param [out] plmn_p          The structure to hold the decoded info. Set to NULL if element at index should be removed
 * @param [out] index           The index to be updated.
 * @param [out] remove          Will be set to 1 if the element at index should be removed.
 * @param [in] data_p           The binary string to be decoded.
 * @param [in] data_len         Length of the binary string to be decoded.
 * @return  On success, the pointer to the encoded string is returned. On error NULL is returned.
 */
void sim_decode_plmn_string(ste_sim_plmn_with_AcT_t *plmn_p, int *index,ste_uicc_sim_plmn_file_id_t *file_id, int *remove, const char *data_p, size_t data_len);

/**
 * @brief Decode the binary plmn with access technology into a ste_sim_plmn_with_AcT_t type
 * @param [out] ste_sim_plmn_withAcT_p  The structure to hold the decoded info.
 * @param [in] data_index_p             The binary string to be decoded.
 * @return  On success, ste_sim_plmn_with_AcT_t holds the decoded access tech. On error ste_sim_plmn_withAcT_p is NULL.
 */
void ste_sim_decode_plmn_with_AcT(ste_sim_plmn_with_AcT_t *ste_sim_plmn_withAcT_p, unsigned char *data_index_p);

/*************************************************************************
 * Function:      ste_sim_get_accesstechnology
 *
 * Description:   Returns the access technology  information given the raw
 *                data (2 bytes) for access technology as read from the SIM.
 *
 * Input Params:  raw_access_tech_p  Pointer to raw data for access technology
 *                                   read from SIM PLMN List file
 *
 * Output Params: None
 *
 * Return:        ste_uicc_sim_plmn_AccessTechnology_t
 *
 * Notes:
 *
 *************************************************************************/
ste_uicc_sim_plmn_AccessTechnology_t ste_sim_get_accesstechnology(unsigned const char *raw_access_tech_p);

/**
 * @brief encode the plmn read info into a binary string, to be sent via socket.
 * @param file_id_p     file to read from
 * @param result_len_p  The length of the encoded result.
 * @return  On success, the pointer to the encoded string is returned. On error NULL is returned.
 */
char *sim_encode_read_plmn(ste_uicc_sim_plmn_file_id_t *file_id_p, size_t *result_len_p);

/**
 * @brief decode the binary data from socket into plmn read info.
 * @param file_id       file to read from
 * @param data_p        The binary string to be decoded.
 * @param data_len      Length of the binary string to be decoded.
 *@return  void.
 */
void sim_decode_read_plmn(ste_uicc_sim_plmn_file_id_t *file_id, const char * data_p, size_t data_len);

/**
 * @brief Encode the client's event download data into a binary string to send over socket.
 * @param event_type    event type data.
 * @param event_data_p  data structure related to event type.
 * @param len_p 	The length of the encoded data.
 * @return	On success, the created binary string pointer is returned. On error NULL is returned.
 */
char* sim_encode_event_download_data(uint8_t   event_type,
                                     void    * event_data_p,
                                     size_t  * len_p);

/**
 * @brief Decode the client's event download binary data from socket.
 * @param event_type_p  event type data.
 * @param event_data_p  data structure related to event type.
 * @param buf_p   	The pointer to the binary data to be decoded.
 * @param len   	The length of the binary data.
 * @return	On success, zero is returned. On error non-zero is returned.
 */
int sim_decode_event_download_data(uint8_t * event_type_p,
                                   void   ** event_data_pp,
                                   uint8_t * buf_p,
                                   size_t    len);

/**
 * @brief Free the client's event download data structure.
 * @param event_type    event type.
 * @param event_data_p  data structure related to event type.
 * @return	void.
 */
void sim_free_event_download_data(uint8_t event_type, void * event_data_p);

/*
 * @brief Helper function for decoding a bcd encoded byte.
 *
 * @param x BCD encoded byte
 *
 * return Decoded version of x
 */
uint8_t ste_apdu_decode_bcd(uint8_t x);

/*
 * @brief Helper function for encoding a bcd decoded byte.
 *
 * @param x BCD decoded byte
 *
 * return Encoded version of x
 */
uint8_t ste_apdu_encode_bcd(uint8_t x);

/*
 * @brief Helper function to compute min
 * return min of the two arg
 */

int min(int a, int b);

/**
 * @brief Decode alpha id field on sim card to understandable format by client.
 * @param alpha_id_p         original alpha id text.
 * @param total_alpha_len    original alpha id length.
 * @param alpha_coding_p     alpha id coding scheme.
 * @param dest_p             buffer to hold the result.
 * @return  Length of actual alpha id.
 */
uint8_t sim_decode_alpha_id(const uint8_t*               alpha_id_p,
                            const uint8_t                total_alpha_len,
                            uint8_t*               const alpha_coding_p,
                            uint8_t*               const dest_p);

/**
 * @brief Convert the packed UCS2 81 prefix format to "standard" UCS2
 * @param source_p      The pointer to the text data to be converted.
 * @param max_len       Length of output buffer in bytes.
 * @param actual_len_p  The actual length of the converted data.
 * @param dest_p        The pointer to the result
 * @return  On success, zero is returned. On error non-zero is returned.
 */
int sim_convert_ucs2_81prefix_to_ucs2(uint8_t   * source_p,
                                      uint16_t    max_len,
                                      uint16_t  * actual_len_p,
                                      uint16_t  * dest_p);

/**
 * @brief Convert the packed UCS2 82 prefix format to "standard" UCS2
 * @param source_p      The pointer to the text data to be converted.
 * @param max_len       Length of output buffer in bytes.
 * @param actual_len_p  The actual length of the converted data.
 * @param dest_p        The pointer to the result
 * @return  On success, zero is returned. On error non-zero is returned.
 */
int sim_convert_ucs2_82prefix_to_ucs2(uint8_t   * source_p,
                                      uint16_t    max_len,
                                      uint16_t  * actual_len_p,
                                      uint16_t  * dest_p);


#endif
