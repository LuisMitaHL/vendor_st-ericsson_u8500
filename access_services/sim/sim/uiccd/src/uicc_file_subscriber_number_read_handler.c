/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include <stdint.h>
#include "sim_internal.h"
#include "catd_modem_types.h"
#include "uicc_internal.h"
#include "simd.h"
#include "uiccd_msg.h"
#include "uicc_file_smsc_common.h"
#include <assert.h>
#include "sim_unused.h"

/**
 * For Read Subscriber Number Transaction.
 * Control data for read STE_UICC_CAUSE_REQ_READ_SUBSCRIBER_NUMBER transaction
 */
typedef enum {
    UICCD_TRANS_SUBSCRIBER_NUMBER_INIT,
    UICCD_TRANS_SUBSCRIBER_NUMBER_MSISDN_GET_FORMAT,
    UICCD_TRANS_SUBSCRIBER_NUMBER_MSISDN_READ_RECORD,
    UICCD_TRANS_SUBSCRIBER_NUMBER_CCP2_GET_FORMAT,
    UICCD_TRANS_SUBSCRIBER_NUMBER_CCP2_READ_RECORD,
    UICCD_TRANS_SUBSCRIBER_NUMBER_EXT5_GET_FORMAT,
    UICCD_TRANS_SUBSCRIBER_NUMBER_EXT5_READ_RECORD,
    UICCD_TRANS_SUBSCRIBER_NUMBER_INVALID,
} uiccd_trans_subscriber_number_state_t;

/**
* For Read Subscriber Number Transaction.
* Response codes returned form uicc_decode_msisdn_record.
*/
typedef enum{
    MSISDN_REC_DECODE_SUCCESS = 0x0,
    MSISDN_REC_DECODE_ERROR_UNKNOWN,
    MSISDN_REC_DECODE_ERROR_INVALID_REC,
    MSISDN_REC_DECODE_ERROR_MEM_ALLOC,
    MSISDN_REC_DECODE_ERROR_BCD_TO_ASCII8,
} uicc_msisdn_record_decode_status_t;

/**
 * For Read Subscriber Number Transaction.
 * MSISDN_STATIC_DATA_LEN is the number of octets, in a file record,
 * that contains other data then the Alpha Identifier.
 */
#define MSISDN_STATIC_DATA_LEN    (14)

/**
* For Read Subscriber Number Transaction.
* Different offsets within a MSISDN record.
*/
typedef enum {
    MSISDN_REC_OFFSET_BCD_LEN = 0,
    MSISDN_REC_OFFSET_TON_NPI = 1,
    MSISDN_REC_OFFSET_DIAL_NUM = 2,
    MSISDN_REC_OFFSET_CCP2 = 12,
    MSISDN_REC_OFFSET_EXT5 = 13,
} ste_sim_msisdn_record_offset_t;

#define MSISDN_MAX_ADDRESS_LENGTH (20) /* (= 2*10) 10 is the number of octets for the MSISDN Dialling Number (See 3GPP TS 31.102). */
#define MSISDN_MAX_ALPHA_ID_LENGTH (241) /* (MSISDN record size -14) = x (Alpha ID size). (See 3GPP TS 31.102) */

/**
* For Read Subscriber Number Transaction.
* Data for a MSISDN record.
*/
typedef struct {
    uint8_t valid_record; /* Is uint8_t in lack of a boolean type */
    uint8_t alpha_id_actual_len;
    uint8_t alpha_coding;
    uint8_t alpha[MSISDN_MAX_ALPHA_ID_LENGTH+1]; /* Optional! */
    uint8_t number_actual_len;
    uint8_t number[MSISDN_MAX_ADDRESS_LENGTH+1];
    uint8_t type;
    uint8_t speed; /* Optional! */
    uint8_t service;
    uint8_t itc; /* Optional! */
    uint16_t record_len;
} uicc_subscriber_number_record_data_t;

/**
* For Read Subscriber Number Transaction.
* Transaction data.
*/
typedef struct {
    ste_uicc_status_word_t                status_word;
    ste_uicc_status_code_t                uicc_status_code;
    ste_uicc_status_code_fail_details_t   uicc_status_code_fail_details;
    uiccd_trans_subscriber_number_state_t trans_state;
    uint8_t                               record_id;
    uint8_t                               alpha_id_len;
    uint16_t                              num_records; /* The number of records stored in the MSISDN-file. */
    uint16_t                              num_valid_records; /* The number of records, in the MSISDN-file, that contain valid data e.g. records that aren not marked deleted. */
    uint16_t                              record_len;
    uint8_t                               ccp2_ext5[2];
    uicc_subscriber_number_record_data_t  *record_data_p;
} uiccd_trans_data_subscriber_number_t;

static int uicc_convert_BCD_to_ASCII8(uint8_t  * source_p,
                                      uint16_t no_of_bcd_characters,
                                      uint16_t          max_len,
                                      uint16_t        * actual_len_p,
                                      uint8_t         * buf_p)
{
    if (source_p == NULL || buf_p == NULL || actual_len_p == NULL)
    {
        abort(); // Should never happen!
    }

    *actual_len_p = 0;     /* Initialise return value */

    if (no_of_bcd_characters != 0)
    {
        uint16_t    result_len = 0;
        uint8_t    *index_p = source_p;
        uint16_t    char_nr = no_of_bcd_characters;

        while (result_len < max_len)
        {
            *buf_p++ = ((*index_p) & 0x0F) + 0x30;
            --char_nr;
            ++result_len;
            if (char_nr == 0)
            {
                break;
            }
            *buf_p++ = (((*index_p) >> 4) & 0x0F) + 0x30;
            --char_nr;
            ++result_len;
            ++index_p;
        }

        *actual_len_p = result_len;
    }
    else
    { /* There were no characters to convert */
        *actual_len_p = 0;
    }

    return 0;
}

static uicc_msisdn_record_decode_status_t
uicc_decode_msisdn_record(uicc_subscriber_number_record_data_t *record_data_p,
                          uint8_t *ccp2_ext5_p,
                          uint8_t alpha_id_offset,
                          uint8_t *data_p,
                          size_t UNUSED(len))
{
    uint8_t  *index_p;
    int      res;
    uint8_t  len_in_bytes;
    uint8_t  ton_npi;
    uint16_t result_len = 0;
    uint8_t  *bcd_text_p;
    uint16_t no_of_bcd_characters = 0;
    uint8_t  *tmp_data_p = NULL;

    if( (NULL == record_data_p) || (NULL == data_p) ) {
        return MSISDN_REC_DECODE_ERROR_UNKNOWN;
    }

    /* Set record data to values that indicates a non valid/deleted record. */
    record_data_p->valid_record = 0; //FALSE;
    record_data_p->alpha_id_actual_len = 1;
    record_data_p->alpha_coding = STE_SIM_CODING_UNKNOWN;
    record_data_p->alpha[0] = '\0';
    record_data_p->number_actual_len = 1;
    record_data_p->number[0] = '\0';
    record_data_p->type = 0xFF;
    record_data_p->speed = 0xFF;
    record_data_p->service = 0xFF;
    record_data_p->itc = 0xFF;
    record_data_p->record_len = 0;
    ccp2_ext5_p[0] = 0xFF;
    ccp2_ext5_p[1] = 0xFF;


    /* Extract the Length of BCD number. */
    tmp_data_p = (data_p + alpha_id_offset + MSISDN_REC_OFFSET_BCD_LEN);
    len_in_bytes = *tmp_data_p;
    /* and the ton_npi byte */
    ton_npi = *(tmp_data_p+1);
    // Make checks if we can process this record
    if ((ton_npi & 0x80) == 0x00) {
        // Ton npi must have msb set
        // For a record filled with 0x00 it will bail out here
        record_data_p->valid_record = 0; //FALSE;
        return MSISDN_REC_DECODE_ERROR_INVALID_REC;
    }

    if (ton_npi == 0xFF) {
        // If TON NPI is not used, then length is max 10
        // For a record filled with 0xFF, it will bail out here
        if (len_in_bytes > 0x0A) {
            record_data_p->valid_record = 0; //FALSE;
            return MSISDN_REC_DECODE_ERROR_INVALID_REC;
        }
    }
    if (len_in_bytes == 0x00 || len_in_bytes > 0x0B) {
        // if TON NPI is used, then max length is between 1 and 11
        record_data_p->valid_record = 0; //FALSE;
        return MSISDN_REC_DECODE_ERROR_INVALID_REC;
    }
    // Could do further checks, but these should suffice

    record_data_p->valid_record = 1; //TRUE;

    if (ton_npi != 0xFF) {
        len_in_bytes = len_in_bytes - 1; /* Subtract the TON & NPI octet from the length. */
    }

    /* Extract the Dialling Number/SSC content. */
    bcd_text_p = malloc(len_in_bytes + 1);

    if (bcd_text_p == NULL) {
        return MSISDN_REC_DECODE_ERROR_MEM_ALLOC;
    }

    memset(bcd_text_p, 0, len_in_bytes + 1);

    tmp_data_p = (data_p + alpha_id_offset + MSISDN_REC_OFFSET_DIAL_NUM);

    index_p = (uint8_t*)(bcd_text_p);
    memcpy(index_p, tmp_data_p, len_in_bytes);

    index_p = index_p + len_in_bytes - 1; //move to the last byte
    if (((*index_p) & 0xF0) == 0xF0) {  //odd number
        no_of_bcd_characters = len_in_bytes * 2 - 1;
    }
    else {
        no_of_bcd_characters = len_in_bytes * 2;
    }

    res = uicc_convert_BCD_to_ASCII8(bcd_text_p,
                                     no_of_bcd_characters,
                                     len_in_bytes * 2,
                                     &result_len,
                                     record_data_p->number);

    if (res == -1) {
        free(bcd_text_p);
        return MSISDN_REC_DECODE_ERROR_BCD_TO_ASCII8;
    }

    record_data_p->number_actual_len = result_len + 1; /* +1 for the terminating zero. */

    if(NULL != bcd_text_p){
        free(bcd_text_p);
    }

    /* Extract the Alpha Identifier. */
    tmp_data_p = data_p;
    /* Extract the alpha id. */
    if (alpha_id_offset > 0) {
        record_data_p->alpha_id_actual_len = sim_decode_alpha_id(tmp_data_p,
                                                                 alpha_id_offset,
                                                                 &(record_data_p->alpha_coding),
                                                                 record_data_p->alpha);

        /* Convert the coding to UCS2 if it is UCS2 with 81prefix or 82prefix. */
        if (record_data_p->alpha_coding == STE_SIM_UCS2_81Prefix) {
            uint8_t  dest[MSISDN_MAX_ALPHA_ID_LENGTH + 1];
            uint16_t result_len;
            int      rv;

            memset(dest, 0, MSISDN_MAX_ALPHA_ID_LENGTH + 1);
            rv = sim_convert_ucs2_81prefix_to_ucs2(record_data_p->alpha,
                                                   MSISDN_MAX_ALPHA_ID_LENGTH,
                                                   &result_len,
                                                   (uint16_t*)dest);
            if (rv == 0) {
                record_data_p->alpha_coding = STE_SIM_UCS2;
                record_data_p->alpha_id_actual_len = result_len;
                memcpy(record_data_p->alpha, dest, result_len);
            }
        }
        else if (record_data_p->alpha_coding == STE_SIM_UCS2_82Prefix) {
            uint8_t  dest[MSISDN_MAX_ALPHA_ID_LENGTH + 1];
            uint16_t result_len;
            int      rv;

            memset(dest, 0, MSISDN_MAX_ALPHA_ID_LENGTH + 1);
            rv = sim_convert_ucs2_82prefix_to_ucs2(record_data_p->alpha,
                                                   MSISDN_MAX_ALPHA_ID_LENGTH,
                                                   &result_len,
                                                   (uint16_t*)dest);
            if (rv == 0) {
                record_data_p->alpha_coding = STE_SIM_UCS2;
                record_data_p->alpha_id_actual_len = result_len;
                memcpy(record_data_p->alpha, dest, result_len);
            }
        }

        if (record_data_p->alpha_id_actual_len > 0) {
            record_data_p->alpha[record_data_p->alpha_id_actual_len] = '\0';
        }
        else {
            record_data_p->alpha[0] = '\0';
            record_data_p->alpha_coding = STE_SIM_CODING_UNKNOWN;
            record_data_p->alpha_id_actual_len = 0;
        }
    }
    else {
        record_data_p->alpha[0] = '\0';
        record_data_p->alpha_coding = STE_SIM_CODING_UNKNOWN;
        record_data_p->alpha_id_actual_len = 0;
    }

    /* Extract the speed (0 - 134)(See 3GPP TS 27.007). */
    /* At the moment the speed is not supported. We will return the non valid number 0xFF. */
    record_data_p->speed = 0xFF;

    /* Extract the service (0 - 5)(See 3GPP TS 27.007). */
    /* At the moment the service is not supported. We will return the non valid number 0xFF. */
    record_data_p->service = 0xFF;

    /* Extract the Information Transfer Capability (0 - 5)(See 3GPP TS 27.007). */
    /* At the moment the itc is not supported. We will return the non valid number 0xFF. */
    record_data_p->itc = 0xFF;

    /* Extract the TON (Type Of Number). */
    tmp_data_p = (data_p + alpha_id_offset + MSISDN_REC_OFFSET_TON_NPI);
    record_data_p->type = ((*tmp_data_p >> 4) & 0x7);

    /* Extract the CCP2 (Capability/Configuration2 Record ID). */
    tmp_data_p = (data_p + alpha_id_offset + MSISDN_REC_OFFSET_CCP2);
    ccp2_ext5_p[0] = *tmp_data_p;

    /* Extract the EXT5 (Extension5 Record ID). */
    tmp_data_p = (data_p + alpha_id_offset + MSISDN_REC_OFFSET_EXT5);
    ccp2_ext5_p[1] = *tmp_data_p;

    return MSISDN_REC_DECODE_SUCCESS;
}

/**
 * uiccd_main_read_subscriber_number_serialize_send_response
 */
static int uiccd_main_read_subscriber_number_serialize_send_response(ste_sim_ctrl_block_t * ctrl_blk_p,
                                                                      uiccd_trans_data_subscriber_number_t *data_p)
{
  char *p = NULL;
  char *buf_p = NULL;
  size_t buf_len = 0;

  sim_uicc_status_code_t uicc_status_code;
  sim_uicc_status_code_fail_details_t uicc_status_code_fail_details;
  sim_uicc_status_word_t status_word;

  uiccd_trans_data_subscriber_number_t *trans_data_p = NULL;
  ste_sim_ctrl_block_t * ctrl_p = NULL;
  uint8_t  alpha_id_len = 0;
  uint16_t number_of_records = 0;
  uint16_t number_of_valid_records = 0;
  uint16_t record_length = 0;

  uint8_t i = 0;

  trans_data_p = data_p;
  if(NULL ==  trans_data_p){
      catd_log_f(SIM_LOGGING_E, "uicc : Transaction data is missing!");
      return 0;
  }

  ctrl_p = ctrl_blk_p;

  if(NULL == ctrl_p){
      catd_log_f(SIM_LOGGING_E, "uicc : Control block is missing!");
      free(trans_data_p);
      return 0;
  }

  uicc_status_code = trans_data_p->uicc_status_code;
  uicc_status_code_fail_details = trans_data_p->uicc_status_code_fail_details;
  status_word.sw1 = trans_data_p->status_word.sw1;
  status_word.sw2 = trans_data_p->status_word.sw2;
  alpha_id_len = trans_data_p->alpha_id_len;
  number_of_records = trans_data_p->num_records;
  number_of_valid_records = trans_data_p->num_valid_records;
  record_length = trans_data_p->record_len;

  buf_len += sizeof(uicc_status_code);
  buf_len += sizeof(uicc_status_code_fail_details);
  buf_len += sizeof(status_word.sw1);
  buf_len += sizeof(status_word.sw2);
  buf_len += sizeof(alpha_id_len);
  buf_len += sizeof(number_of_valid_records);
  buf_len += sizeof(record_length);

  if(0 < number_of_valid_records){
      uicc_subscriber_number_record_data_t *tmp_rec_data_p = trans_data_p->record_data_p;


      for(i=0; i<number_of_records; i++){
          if(/*TRUE*/1 == tmp_rec_data_p->valid_record){
              buf_len += sizeof(tmp_rec_data_p->alpha_id_actual_len);
              buf_len += sizeof(tmp_rec_data_p->alpha_coding);
              buf_len += tmp_rec_data_p->alpha_id_actual_len;
              buf_len += sizeof(tmp_rec_data_p->number_actual_len);
              buf_len += tmp_rec_data_p->number_actual_len;
              buf_len += sizeof(tmp_rec_data_p->type);
              buf_len += sizeof(tmp_rec_data_p->speed);
              buf_len += sizeof(tmp_rec_data_p->service);
              buf_len += sizeof(tmp_rec_data_p->itc);
          }

          tmp_rec_data_p++;
      }
  }

  buf_p = malloc(buf_len);

  if (!buf_p) {
    catd_log_f(SIM_LOGGING_E, "uicc : Memory allocation for serial buffer failed");
    goto error;
  }

  p = (char *)buf_p;

  p = sim_enc(p, &uicc_status_code, sizeof(uicc_status_code));
  p = sim_enc(p, &uicc_status_code_fail_details, sizeof(uicc_status_code_fail_details));
  p = sim_enc(p, &status_word.sw1, sizeof(status_word.sw1));
  p = sim_enc(p, &status_word.sw2, sizeof(status_word.sw2));
  p = sim_enc(p, &alpha_id_len, sizeof(alpha_id_len));
  p = sim_enc(p, &number_of_valid_records, sizeof(number_of_valid_records));
  p = sim_enc(p, &record_length, sizeof(record_length));

  if(0 < number_of_valid_records){
    uicc_subscriber_number_record_data_t *tmp_rec_data_p = trans_data_p->record_data_p;

    uint8_t alpha_id_actual_len = 0;
    uint8_t alpha_coding = STE_SIM_CODING_UNKNOWN;
    uint8_t *alpha_p = NULL;
    uint8_t number_actual_len = 0;
    uint8_t *number_p = NULL;
    uint8_t type = 0;
    uint8_t speed = 0;
    uint8_t service = 0;
    uint8_t itc = 0;

    for(i=0; i<number_of_records; i++){
      if(/*TRUE*/1 == tmp_rec_data_p->valid_record){
          alpha_p = tmp_rec_data_p->alpha;
          number_p = tmp_rec_data_p->number;

          alpha_id_actual_len = tmp_rec_data_p->alpha_id_actual_len;
          alpha_coding = tmp_rec_data_p->alpha_coding;
          number_actual_len = tmp_rec_data_p->number_actual_len;
          type = tmp_rec_data_p->type;
          speed = tmp_rec_data_p->speed;
          service = tmp_rec_data_p->service;
          itc = tmp_rec_data_p->itc;

          p = sim_enc(p, &alpha_id_actual_len, sizeof(alpha_id_actual_len));
          p = sim_enc(p, &alpha_coding, sizeof(alpha_coding));
          p = sim_enc(p, alpha_p, alpha_id_actual_len);
          p = sim_enc(p, &number_actual_len, sizeof(number_actual_len));
          p = sim_enc(p, number_p, number_actual_len);
          p = sim_enc(p, &type, sizeof(type));
          p = sim_enc(p, &speed, sizeof(speed));
          p = sim_enc(p, &service, sizeof(service));
          p = sim_enc(p, &itc, sizeof(itc));
      }

      tmp_rec_data_p++;
    }
  }

  if (buf_len != (size_t)(p - buf_p)) {
    catd_log_f(SIM_LOGGING_E, "uicc: uiccd_main_read_subscriber_number_serialize_send_response assert failure");
  }

  assert( buf_len == (size_t)(p - buf_p) );

  sim_send_generic(ctrl_p->fd,
                   STE_UICC_RSP_READ_SUBSCRIBER_NUMBER,
                   buf_p,
                   buf_len,
                   ctrl_p->client_tag);

  free(buf_p);
  free(trans_data_p->record_data_p);
  free(trans_data_p);
  free(ctrl_p);

  return 0;

error:

    sim_send_generic(ctrl_p->fd,
                     STE_UICC_RSP_READ_SUBSCRIBER_NUMBER,
                     NULL,
                     0,
                     ctrl_p->client_tag);

  free(buf_p);
  free(trans_data_p->record_data_p);
  free(trans_data_p);
  free(ctrl_p);

    return 0;
}

/**
 * uiccd_read_subscriber_number_transaction_handler
 * @brief                  Handler function for read smsc responses.
 * @param ste_msg          The original message from client.
 * @return                 Status of the transaction. Transaction is DONE: 0; Transaction to be continued: -1
 *
 */
static int uiccd_read_subscriber_number_transaction_handler(ste_msg_t *ste_msg)
{
  ste_sim_ctrl_block_t *ctrl_p = NULL;
  uiccd_trans_data_subscriber_number_t *trans_data_p = NULL;
  int res = 0;
  int file_id = 0;

  /* Get control block. */
  ctrl_p = (ste_sim_ctrl_block_t*)ste_msg->client_tag;
  if(NULL == ctrl_p){
      catd_log_f(SIM_LOGGING_E, "uicc : Getting control block failed!");
      goto error;
  }

  /* Get transaction data. */
  trans_data_p = (uiccd_trans_data_subscriber_number_t *)(ctrl_p->transaction_data_p);
  if (NULL == trans_data_p) {
      catd_log_f(SIM_LOGGING_E, "uicc : Getting transaction data failed!");
      goto error;
  }

  switch (trans_data_p->trans_state)
  {
  case UICCD_TRANS_SUBSCRIBER_NUMBER_MSISDN_GET_FORMAT:
  {
      uiccd_msg_sim_file_get_format_response_t *rsp_p = NULL;

      if (ste_msg->type != UICCD_MSG_SIM_FILE_GET_FORMAT_RSP) {
          catd_log_f(SIM_LOGGING_E, "uicc : Wrong msg type for transaction state UICCD_TRANS_SUBSCRIBER_NUMBER_GET_FORMAT");
          goto proceed;
      }

      rsp_p = (uiccd_msg_sim_file_get_format_response_t *)ste_msg;

      trans_data_p->uicc_status_code = rsp_p->uicc_status_code;
      trans_data_p->uicc_status_code_fail_details = rsp_p->uicc_status_code_fail_details;
      trans_data_p->status_word.sw1 = rsp_p->status_word.sw1;
      trans_data_p->status_word.sw2 = rsp_p->status_word.sw2;

      trans_data_p->record_len = 0;
      trans_data_p->num_records = 0;
      trans_data_p->num_valid_records = 0;
      trans_data_p->alpha_id_len = 0;
      trans_data_p->ccp2_ext5[0] = 0xFF;
      trans_data_p->ccp2_ext5[1] = 0xFF;
      trans_data_p->record_id = 0;
      trans_data_p->record_data_p = NULL;

      if(SIM_UICC_STATUS_CODE_OK != trans_data_p->uicc_status_code){
          catd_log_f(SIM_LOGGING_E, "uicc : Getting file format failed!");
          goto error;
      }

      trans_data_p->record_len = rsp_p->record_len;
      trans_data_p->num_records = rsp_p->num_records;

      /**
       *  Calculate the Alpha Identifier length.  MSISDN_STATIC_DATA_LEN is the number of octets,
       *  in a file record, that contains other data then the Alpha Identifier.
       */
      trans_data_p->alpha_id_len = (trans_data_p->record_len-MSISDN_STATIC_DATA_LEN);

      if (trans_data_p->num_records == 0) {
          /* The transaction must be ended since there is no record in the file. */
          catd_log_f(SIM_LOGGING_E, "uicc : Transaction ended, no records in file!");
          goto error;
      }

      /* Allocate memory for all record data pointers. */
      trans_data_p->record_data_p = malloc(sizeof(uicc_subscriber_number_record_data_t)*trans_data_p->num_records);

      if(NULL == trans_data_p->record_data_p){
          catd_log_f(SIM_LOGGING_E, "uicc : Allocating memory for record data pointers failed!");
          goto error;
      }

      /* Read the first record. */
      trans_data_p->record_id = 1;
      trans_data_p->trans_state = UICCD_TRANS_SUBSCRIBER_NUMBER_MSISDN_READ_RECORD;
      file_id = UICCD_FILE_ID_MSISDN;

      res = uiccd_read_one_smsp_record(ctrl_p,
                                      trans_data_p->record_id,
                                      trans_data_p->record_len,
                                      file_id);

      if (res < 0) {
          //the transaction must be ended since there is error.
          catd_log_f(SIM_LOGGING_E, "uicc : Transaction ended, read record failed!");
          goto error;
      }
  }
  break;
  case UICCD_TRANS_SUBSCRIBER_NUMBER_MSISDN_READ_RECORD:
  {
      uiccd_msg_read_sim_file_record_response_t *rsp_p = NULL;
      uicc_subscriber_number_record_data_t *tmp_rec_data_p = NULL; /* Pointer to the current record data structure. */

      if (ste_msg->type != UICCD_MSG_READ_SIM_FILE_RECORD_RSP) {
          catd_log_f(SIM_LOGGING_E, "uicc : Wrong msg type for transaction state UICCD_TRANS_SMSC_READ_RECORD");
          goto proceed;
      }

      rsp_p = (uiccd_msg_read_sim_file_record_response_t*)ste_msg;

      if(SIM_UICC_STATUS_CODE_OK != rsp_p->uicc_status_code){
          catd_log_f(SIM_LOGGING_E, "uicc : Reading file record failed!");

          /* If it goes wrong we want to store the latest status */
          trans_data_p->uicc_status_code = rsp_p->uicc_status_code;
          trans_data_p->uicc_status_code_fail_details = rsp_p->uicc_status_code_fail_details;
          trans_data_p->status_word.sw1 = rsp_p->status_word.sw1;
          trans_data_p->status_word.sw2 = rsp_p->status_word.sw2;

          goto error;
      }

      trans_data_p->record_data_p->record_len = rsp_p->len;

      /* Get the pointer to the current record data structure. */
      tmp_rec_data_p = ( trans_data_p->record_data_p + (trans_data_p->record_id-1) );

      /* Set all positions in the number to zero. */
      memset(tmp_rec_data_p->number, 0, sizeof(tmp_rec_data_p->number));
      memset(tmp_rec_data_p->alpha, 0, sizeof(tmp_rec_data_p->alpha));

      /* Parse the record data and put into transaction data structure. */
      res = uicc_decode_msisdn_record(tmp_rec_data_p,
                                      trans_data_p->ccp2_ext5,
                                      trans_data_p->alpha_id_len,
                                      rsp_p->data,
                                      rsp_p->len);

      if(MSISDN_REC_DECODE_SUCCESS == res){
          trans_data_p->num_valid_records++;

          /**
           *  Check if we need to get the CCP2 (Capability/Configuration2 Record ID) record.
           * If the CCP2-ID byte in the MSISDN-record is anything but 0xFF we need to fetch
           * the record with CCP2-ID for the CCP2-file. First we have to get the CCP2 file
           * format.
           */
          if(0xFF != trans_data_p->ccp2_ext5[0]){
              /* TODO:
               *  Check if we need to get the CCP2 (Capability/Configuration2 Record ID) record.
               * If the CCP2-ID byte in the MSISDN-record is anything but 0xFF we need to fetch
               * the record with CCP2-ID for the CCP2-file. First we have to get the CCP2 file
               * format.
               */
          }

          /**
           * Check if we need to get the EXT5 (Extension5 Record ID).
           * If the EXT5-ID byte in the MSISDN-record is anything but 0xFF we need to fetch
           * the record with EXT5-ID for the EXT5-file. First we have to get the EXT5 file
           * format.
           */
         if(0xFF != trans_data_p->ccp2_ext5[1]){
             /* TODO:
             * Check if we need to get the EXT5 (Extension5 Record ID).
             * If the EXT5-ID byte in the MSISDN-record is anything but 0xFF we need to fetch
             * the record with EXT5-ID for the EXT5-file. First we have to get the EXT5 file
             * format.
             */
         }
      }
      else if(MSISDN_REC_DECODE_ERROR_INVALID_REC == res){
          /* Found invalid/deleted record. Go on with the next record. */
          catd_log_f(SIM_LOGGING_I, "uicc : Found invalid/deleted record!");
      }
      else{
          /* The transaction must be ended since there is error. */
          catd_log_f(SIM_LOGGING_E, "uicc : End transaction, decoding MSISDN record failed!");
          goto error;
      }

      /* Read next record, if needed. */
      trans_data_p->record_id++;
      if (trans_data_p->record_id <= trans_data_p->num_records) {
          trans_data_p->trans_state = UICCD_TRANS_SUBSCRIBER_NUMBER_MSISDN_READ_RECORD;
          file_id = UICCD_FILE_ID_MSISDN;

          res = uiccd_read_one_smsp_record(ctrl_p,
                                          trans_data_p->record_id,
                                          trans_data_p->record_len,
                                          file_id);
          if (res < 0) {
              /* The transaction must be ended since there is error. */
              catd_log_f(SIM_LOGGING_E, "uicc : End transaction, error occured!");
              goto error;
          }
      }
      else {
          /* We have gone through the whole msisdn file.
           * It is now time to send response to SIM-lib. */
          trans_data_p->trans_state = UICCD_TRANS_SMSC_INIT;
          catd_log_f(SIM_LOGGING_E, "uicc : All MSISDN records read! End transaction.");
          uiccd_main_read_subscriber_number_serialize_send_response(ctrl_p, trans_data_p);

          goto done;
      }
  }
  break;
  case UICCD_TRANS_SUBSCRIBER_NUMBER_CCP2_GET_FORMAT:{
      uiccd_msg_sim_file_get_format_response_t *rsp_p = NULL;

      if (ste_msg->type != UICCD_MSG_SIM_FILE_GET_FORMAT_RSP) {
          catd_log_f(SIM_LOGGING_E, "uicc : Wrong msg type for transaction state UICCD_TRANS_SUBSCRIBER_NUMBER_GET_FORMAT");
          goto proceed;
      }

      rsp_p = (uiccd_msg_sim_file_get_format_response_t *)ste_msg;

      if(SIM_UICC_STATUS_CODE_OK != rsp_p->uicc_status_code){
          catd_log_f(SIM_LOGGING_E, "uicc : Getting file format failed!");
          /* If something went wrong we want to save the status code and
           * send it back to the client.
           */
          trans_data_p->uicc_status_code = rsp_p->uicc_status_code;
          trans_data_p->uicc_status_code_fail_details = rsp_p->uicc_status_code_fail_details;
          trans_data_p->status_word.sw1 = rsp_p->status_word.sw1;
          trans_data_p->status_word.sw2 = rsp_p->status_word.sw2;

          goto error;
      }

      /* TODO: Read the CCP2-file and the trans_data_p->ccp2_ext5[0] record. */
  }
  break;
  case UICCD_TRANS_SUBSCRIBER_NUMBER_CCP2_READ_RECORD:{
      /* TODO: Read the CCP2 record. Do we need to read it? */
  }
  break;
  case UICCD_TRANS_SUBSCRIBER_NUMBER_EXT5_GET_FORMAT:{
      uiccd_msg_sim_file_get_format_response_t *rsp_p = NULL;

      if (ste_msg->type != UICCD_MSG_SIM_FILE_GET_FORMAT_RSP) {
          catd_log_f(SIM_LOGGING_E, "uicc : Wrong msg type for transaction state UICCD_TRANS_SUBSCRIBER_NUMBER_GET_FORMAT");
          goto proceed;
      }

      rsp_p = (uiccd_msg_sim_file_get_format_response_t *)ste_msg;

      if(SIM_UICC_STATUS_CODE_OK != rsp_p->uicc_status_code){
          catd_log_f(SIM_LOGGING_E, "uicc : Getting file format failed!");
          /* If something went wrong we want to save the status code and
           * send it back to the client.
           */
          trans_data_p->uicc_status_code = rsp_p->uicc_status_code;
          trans_data_p->uicc_status_code_fail_details = rsp_p->uicc_status_code_fail_details;
          trans_data_p->status_word.sw1 = rsp_p->status_word.sw1;
          trans_data_p->status_word.sw2 = rsp_p->status_word.sw2;

          goto error;
      }

      /* TODO: Read the EXT5-file and the trans_data_p->ccp2_ext5[1] record. */
  }
  break;
  case UICCD_TRANS_SUBSCRIBER_NUMBER_EXT5_READ_RECORD:{
      /* TODO: Read the EXT5 record. Do we need to read it? */
  }
  break;
  default:
  {
      catd_log_f(SIM_LOGGING_E, "uicc : Wrong transaction state for read subscriber_number=%d", trans_data_p->trans_state);
      goto error;
  }
  } /* switch end!*/

  /* return -1: Transaction to be continued. */
proceed:
  return -1;

error:
    /**
     * If an error occures send a response to cat/uicc lib containing only
     * status_code, fail_details & status_word. We will then go to done!
     */
    uiccd_main_read_subscriber_number_serialize_send_response(ctrl_p, trans_data_p);

done:

    /* return 0: Transaction is DONE!  */
    return 0;
}


/**
 * uiccd_main_read_subscriber_number
 */
int uiccd_main_read_subscriber_number(ste_msg_t *ste_msg)
{

    uiccd_msg_subscriber_number_t *msg = (uiccd_msg_subscriber_number_t *)ste_msg;
    ste_sim_ctrl_block_t *ctrl_p = NULL;
    uiccd_trans_data_subscriber_number_t  * trans_data_p = NULL;
    int res = 0;

    /* Allocate memory for transaction data. */
    trans_data_p = malloc(sizeof(uiccd_trans_data_subscriber_number_t));
    if (!trans_data_p) {
        catd_log_f(SIM_LOGGING_E, "uicc : memory allocation failed");
        sim_send_generic(msg->fd, STE_UICC_RSP_READ_SUBSCRIBER_NUMBER, NULL, 0, msg->client_tag);
        return -1;
    }
    memset(trans_data_p, 0, sizeof(uiccd_trans_data_subscriber_number_t));

    /* Set trans state to file format get! */
    trans_data_p->trans_state = UICCD_TRANS_SMSC_GET_FORMAT;

    /* Create Control Block. */
    ctrl_p = ste_sim_create_ctrl_block(msg->client_tag,
                                       msg->fd,
                                       msg->type,
                                       uiccd_read_subscriber_number_transaction_handler,
                                       trans_data_p);
    if (!ctrl_p){
        catd_log_f(SIM_LOGGING_E, "uicc : Creating control block for read subscriber_number transaction failed!");
        goto error;
    }

    /* Get file format. */
    res = uiccd_get_smsp_format(ctrl_p, UICCD_FILE_ID_MSISDN);

    if (res != 0) {
        catd_log_f(SIM_LOGGING_E, "uicc : Getting file format failed.");
        goto error;
    }

    // Point of successful return
    return 0;
    // Code below is cleanup only if error
 error:
     sim_send_generic(msg->fd, STE_UICC_RSP_READ_SUBSCRIBER_NUMBER, NULL, 0, msg->client_tag);

    free(trans_data_p);
    free(ctrl_p);

    return -1;
}
