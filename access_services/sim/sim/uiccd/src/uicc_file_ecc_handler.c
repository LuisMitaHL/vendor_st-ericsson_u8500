/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "sim_internal.h"
#include "catd_modem_types.h"
#include "uicc_internal.h"
#include "simd.h"
#include "uiccd_msg.h"
#include "uicc_state_machine.h"

// Raw data read out of the ECC file
typedef struct {
  uint8_t  code[3];
  size_t   length;
  uint8_t *alpha;
  uint8_t  cat;
} ecc_t;

// an ecc code is max 3 bytes containing max 6 bcd digits
typedef struct {
  int fpos;  // Position to read from (if rb bytes offset, if rr record id)
  int fsize; // File size (if rb no of bytes, if rr no of records)
  size_t rsize; // Record size (only if rr, for rb not used)
  int ecc_n; // Max entries of ecc
  int ecc_i; // Current index into ecc
  ecc_t *ecc; // ecc under construction
} trans_read_ecc_data_t;

static const struct {
  const char *gsm;
  const char *usim;
} ecc_paths = { "3F007F20", "3F007FFF" };

static const int ecc_file_id = 0x6FB7;

// Assume b is bcd according to spec
static int ecc_number_of_digits_byte(uint8_t b) {
  if (b == 0xff) {
    return 0;
  }

  if (b >= 0xf0) {
    return 1;
  }

  return 2;
}

static int ecc_number_of_digits(uint8_t *data) {
  return ecc_number_of_digits_byte(data[0])+
    ecc_number_of_digits_byte(data[1])+
    ecc_number_of_digits_byte(data[2]);
}

// Simplest check is enough here.
static int ecc_is_number(uint8_t *data) {
  if (!data) return 0;
  return ecc_number_of_digits(data) > 0;
}

// Return pointer to ecc code in record
static uint8_t *ecc_rr_get_number(uiccd_msg_read_sim_file_record_response_t *rr) {
  return rr->data;
}

static uint8_t *ecc_rr_get_alpha(uiccd_msg_read_sim_file_record_response_t *rr) {
  return rr->data+3;
}

static size_t ecc_rr_get_alpha_len(uiccd_msg_read_sim_file_record_response_t *rr) {
  return rr->len-4;
}

static uint8_t ecc_rr_get_cat(uiccd_msg_read_sim_file_record_response_t *rr) {
  return *(rr->data + rr->len-1);
}

static void uiccd_ecc_cleanup(ste_sim_ctrl_block_t *ctrl_p) {
  trans_read_ecc_data_t *tdata_p;
  ecc_t *ecc_p;
  int   ecc_n;
  int   i;

  if (!ctrl_p) {
    return;
  }
  tdata_p = ctrl_p->transaction_data_p;
  if (tdata_p) {
    ecc_p = tdata_p->ecc;
    ecc_n = tdata_p->ecc_n;
    if (ecc_p) {
      for (i = 0; i < ecc_n; i++)
        if (ecc_p[i].alpha) {
          free(ecc_p[i].alpha);
        }
      free(ecc_p);
    }
    free(tdata_p);
  }
  free(ctrl_p);
}

static void uiccd_read_ecc_send_response(int fd, uintptr_t client_tag,
                                         sim_uicc_status_code_t uicc_status_code,
                                         sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                         sim_uicc_status_word_t status_word,
                                         ecc_t *ecc,
                                         int n) {
  int i;
  size_t buf_len;
  size_t ecc_len = (size_t)n;
  char *p = NULL;
  char* buf_p = NULL;

  // This should write out data using the same basic types as in sim.h
  buf_len = sizeof(uicc_status_code)+sizeof(uicc_status_code_fail_details);
  buf_len += sizeof(status_word.sw1)+sizeof(status_word.sw2);
  buf_len += sizeof(ecc_len);
  // Sum up the size of all ecc entries
  for (i = 0; i < n; i++) {
    int num_len = ecc_number_of_digits(ecc[i].code);
    size_t my_size;

    my_size = sizeof(num_len);
    my_size += sizeof(char) * num_len;
    my_size += sizeof(ecc[i].length);
    my_size += sizeof(uint8_t) * ecc[i].length;
    my_size += sizeof(ste_uicc_ecc_category_t);
    catd_log_f(SIM_LOGGING_V, "uicc : ecc_send_response num_len is %d, size of ecc %d is %d", num_len, i, my_size);
    buf_len += my_size;
  }

  buf_p = malloc(buf_len);
  if (!buf_p) {
    catd_log_f(SIM_LOGGING_E, "uicc : memory allocation failed");
    goto error;
  }

  p = buf_p;
  p = sim_enc(p, &uicc_status_code, sizeof(uicc_status_code));
  p = sim_enc(p, &uicc_status_code_fail_details, sizeof(uicc_status_code_fail_details));
  p = sim_enc(p, &status_word.sw1, sizeof(status_word.sw1));
  p = sim_enc(p, &status_word.sw2, sizeof(status_word.sw2));
  p = sim_enc(p, &ecc_len, sizeof(ecc_len));
  for (i = 0; i < n; i++) {
    int num_len;
    ste_uicc_ecc_category_t cat;

    num_len = ecc_number_of_digits(ecc[i].code);
    p = sim_enc(p, &num_len, sizeof(num_len));
    {
      ste_sim_text_t text;
      uint16_t res_len;
      int res;

      text.text_coding = STE_SIM_BCD;
      text.no_of_characters = (unsigned)num_len;
      text.text_p = ecc[i].code;

      res = sim_convert_BCD_to_ASCII8(&text,
                                      (uint16_t)num_len*2,
                                      &res_len,
                                      (uint8_t *)p);
      if (res < 0) {
        goto error;
      }
      p += res_len;
    }
    p = sim_enc(p, &ecc[i].length, sizeof(ecc[i].length));
    if (ecc[i].length > 0) {
      p = sim_enc(p, ecc[i].alpha, ecc[i].length);
    }
    cat = (ste_uicc_ecc_category_t)ecc[i].cat;
    p = sim_enc(p, &cat, sizeof(cat));
  }

  if (buf_len != (size_t)(p - buf_p)) {
    catd_log_f(SIM_LOGGING_E, "uicc: read_ecc_send_response assert failure");
  }

  assert( buf_len == (size_t)(p - buf_p) );

  sim_send_generic(fd, STE_UICC_RSP_READ_ECC,
                   buf_p, buf_len, client_tag);
  free(buf_p);
  return;

 error:
  // The lib classifies this case as an internal error
  sim_send_generic(fd, STE_UICC_RSP_READ_ECC,
                   NULL, 0, client_tag);
  if (buf_p) {
    free(buf_p);
  }
}

// There are only three bytes for the dial string
static void ecc_save_ecc(ecc_t *ecc, uint8_t *code, uint8_t *alpha, size_t max_length, uint8_t cat) {

  ecc->code[0] = code[0];
  ecc->code[1] = code[1];
  ecc->code[2] = code[2];
  ecc->length = 0;
  ecc->alpha = NULL;
  if (max_length > 0) {
    size_t len;

    for (len = 0; len < max_length; len++) {
      if (alpha[len] == 0xff) {
        break;
      }
    }
    if (len > 0) {
      uint8_t *a = malloc(len);

      if (a) {
        ecc->alpha = a;
        memcpy(a, alpha, len);
        ecc->length = len;
      }
    }
  }
  ecc->cat = cat;

  catd_log_f(SIM_LOGGING_V, "uicc : ecc_save num=%x %x %x alpha='%.*s' len=%d cat=%x", ecc->code[0], ecc->code[1], ecc->code[2], ecc->length, ecc->alpha, ecc->length, ecc->cat);
}

static int uiccd_read_ecc_rr_response(ste_msg_t *ste_msg)
{
  ste_sim_ctrl_block_t  *ctrl_p;
  trans_read_ecc_data_t *tdata;
  uiccd_msg_read_sim_file_record_response_t *rr;

  catd_log_f(SIM_LOGGING_I, "uicc : read ecc rr response called");
  // Initial assertions
  assert(ste_msg);
  assert(ste_msg->type == UICCD_MSG_READ_SIM_FILE_RECORD_RSP);
  ctrl_p = (ste_sim_ctrl_block_t *)(ste_msg->client_tag);
  assert(ctrl_p);
  tdata = (trans_read_ecc_data_t *)(ctrl_p->transaction_data_p);
  // tdata must have info from previous file format
  assert(tdata);

  rr = (uiccd_msg_read_sim_file_record_response_t *)ste_msg;
  // Check if read record returned valid data
  if (rr->uicc_status_code != SIM_UICC_STATUS_CODE_OK ||
      rr->len != tdata->rsize) {
    // Modem error, or sim card is corrupt if data has wrong size
    goto error;
  }
  else if (ecc_is_number(ecc_rr_get_number(rr))) {
    catd_log_f(SIM_LOGGING_V, "uicc : ecc found a number at rec %d of %d", tdata->fpos, tdata->fsize);
    ecc_save_ecc(&tdata->ecc[tdata->ecc_i],
                 ecc_rr_get_number(rr),
                 ecc_rr_get_alpha(rr),
                 ecc_rr_get_alpha_len(rr),
                 ecc_rr_get_cat(rr));
    tdata->ecc_i++;
  } else {
    catd_log_f(SIM_LOGGING_V, "uicc : ecc skipped a blank number at rec %d of %d", tdata->fpos, tdata->fsize);
  }

  tdata->fpos++;
  // Data still to read?
  if (tdata->fpos <= tdata->fsize && tdata->ecc_i < tdata->ecc_n) {
    int rv;

    rv = ste_modem_file_read_record(catd_get_modem(),
                                        (uintptr_t)ctrl_p, uicc_get_app_id(),
                                        ecc_file_id, tdata->fpos, 0,
                                        ecc_paths.usim);
    if (rv < 0) {
      goto error;
    }
    return 1;
  }
  else {
    // Done reading the file successfully.
    catd_log_f(SIM_LOGGING_V, "uicc : ecc collected %d numbers out of estimated %d", tdata->ecc_i, tdata->ecc_n);

  uiccd_read_ecc_send_response(ctrl_p->fd, ctrl_p->client_tag,
                                 SIM_UICC_STATUS_CODE_OK,
                                 SIM_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS,
                                 rr->status_word,
                                 tdata->ecc,
                                 tdata->ecc_i);
    uiccd_ecc_cleanup(ctrl_p);
    return 0;
  }
 error:
  // Done reading the file and fail
  uiccd_read_ecc_send_response(ctrl_p->fd, ctrl_p->client_tag,
                               SIM_UICC_STATUS_CODE_FAIL,
                               SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                               rr->status_word, NULL, 0);
  uiccd_ecc_cleanup(ctrl_p);
  return 0;
}

static int uiccd_read_ecc_rb_response(ste_msg_t *ste_msg)
{
  ste_sim_ctrl_block_t  *ctrl_p;
  trans_read_ecc_data_t *tdata;
  uiccd_msg_read_sim_file_binary_response_t *rb;

  catd_log_f(SIM_LOGGING_I, "uicc : read ecc rb response called");

  // Initial assertions
  assert(ste_msg);
  assert(ste_msg->type == UICCD_MSG_READ_SIM_FILE_BINARY_RSP);
  ctrl_p = (ste_sim_ctrl_block_t *)(ste_msg->client_tag);
  assert(ctrl_p);
  tdata = (trans_read_ecc_data_t *)(ctrl_p->transaction_data_p);
  // tdata must have info from previous file format
  assert(tdata);

  rb = (uiccd_msg_read_sim_file_binary_response_t *)ste_msg;

  // Check if read binary returned useless contents
  if (rb->uicc_status_code != SIM_UICC_STATUS_CODE_OK ||
      rb->len != 3) {
    // Modem returned error, or sim card is corrupt.
    goto error;
  }
  else if (ecc_is_number(rb->data)) {

    catd_log_f(SIM_LOGGING_V, "uicc : ecc found a number at pos %d of %d", tdata->fpos, tdata->fsize);

    ecc_save_ecc(&tdata->ecc[tdata->ecc_i], rb->data, NULL, 0, 0);
    tdata->ecc_i++;
  } else {
    catd_log_f(SIM_LOGGING_V, "uicc : ecc skipped a blank number at pos %d of %", tdata->fpos, tdata->fsize);
  }
  // Advance file pointer to next record
  tdata->fpos += 3;
  // Material left to read?
  if (tdata->fpos <= tdata->fsize-3 && tdata->ecc_i < tdata->ecc_n) {
    int rv;

    rv = ste_modem_file_read_binary(catd_get_modem(),
                                        (uintptr_t)ctrl_p, uicc_get_app_id(),
                                        ecc_file_id, tdata->fpos, 3,
                                        ecc_paths.gsm);
    if (rv < 0) {
      goto error;
    }
    return 1;
  }
  else {
    // Done reading the file successfully.
    catd_log_f(SIM_LOGGING_V, "uicc : ecc collected %d numbers out of estimated %d", tdata->ecc_i, tdata->ecc_n);
    // goto error;
    uiccd_read_ecc_send_response(ctrl_p->fd, ctrl_p->client_tag,
                                 SIM_UICC_STATUS_CODE_OK,
                                 SIM_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS,
                                 rb->status_word,
                                 tdata->ecc,
                                 tdata->ecc_i);
    uiccd_ecc_cleanup(ctrl_p);
    return 0;
  }

 error:
  // Done reading the file and fail
  catd_log_f(SIM_LOGGING_V, "uicc : ecc failed");
  uiccd_read_ecc_send_response(ctrl_p->fd, ctrl_p->client_tag,
                               SIM_UICC_STATUS_CODE_FAIL,
                               SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                               rb->status_word, NULL, 0);
  uiccd_ecc_cleanup(ctrl_p);
  return 0;
}

static int read_ecc_rr_request(ste_sim_ctrl_block_t *ctrl_p, int record_len, int num_records)
{
  size_t ecc_size;
  int ecc_n = num_records;
  ecc_t *ecc;
  int rv;
  trans_read_ecc_data_t *tdata = ctrl_p->transaction_data_p;

  ecc_size = sizeof(*ecc)*ecc_n;
  ecc = (ecc_t *)malloc(ecc_size);
  if (!ecc) return -1;
  memset(ecc, 0, ecc_size); // So all pointers, indexes and lengths are 0!
  tdata->fpos = 1; // Start reading record 1
  tdata->fsize = num_records; // File size is measured (here) in records
  tdata->rsize = record_len;
  tdata->ecc = ecc;
  tdata->ecc_n = ecc_n;
  tdata->ecc_i = 0;
  ctrl_p->transaction_handler_func = uiccd_read_ecc_rr_response;
  catd_log_f(SIM_LOGGING_V, "uicc : ecc has %d records of size %d", num_records, record_len);

  rv = ste_modem_file_read_record(catd_get_modem(),
                                  (uintptr_t)ctrl_p, uicc_get_app_id(),
                                  ecc_file_id, tdata->fpos, 0,
                                  ecc_paths.usim);
  if (rv < 0) {
    return -1;
  }
  return 0;
}

static int read_ecc_rb_request(ste_sim_ctrl_block_t *ctrl_p, int file_size)
{
  size_t ecc_size;
  int ecc_n;
  ecc_t *ecc = NULL;
  int rv;
  trans_read_ecc_data_t *tdata = ctrl_p->transaction_data_p;

  // Compute size of ecc. No more than 5 records
  ecc_n = min(5, file_size / 3);
  ecc_size = sizeof(*ecc) * ecc_n;
  ecc = (ecc_t *)malloc(ecc_size);
  if (!ecc) {
    return -1;
  }
  memset(ecc, 0, ecc_size); // So all pointers, indexes and lengths are 0!
  // Start reading from byte 0
  // Init tdata for read binary
  tdata->fpos = 0;
  tdata->fsize = file_size;
  tdata->rsize = 0;
  tdata->ecc_n = ecc_n;
  tdata->ecc = ecc;
  tdata->ecc_i = 0;

  ctrl_p->transaction_handler_func = uiccd_read_ecc_rb_response;
  catd_log_f(SIM_LOGGING_V, "uicc : ecc has %d entries", file_size/3);

  rv = ste_modem_file_read_binary(catd_get_modem(),
                                  (uintptr_t)ctrl_p, uicc_get_app_id(),
                                  ecc_file_id, tdata->fpos, 3,
                                  ecc_paths.gsm);
  if (rv < 0) {
    return -1;
  }
  return 0;
}

static int uiccd_read_ecc_ff_response(ste_msg_t *ste_msg)
{
  ste_sim_ctrl_block_t *ctrl_p;
  trans_read_ecc_data_t *tdata = NULL;
  uiccd_msg_sim_file_get_format_response_t *ff;

  catd_log_f(SIM_LOGGING_I, "uicc : read ecc ff response called");

  // Assertion to make sure ste_msg is setup ok
  assert(ste_msg);
  assert(ste_msg->type == UICCD_MSG_SIM_FILE_GET_FORMAT_RSP);
  ctrl_p = (ste_sim_ctrl_block_t *)(ste_msg->client_tag);
  assert(ctrl_p);
  assert(!ctrl_p->transaction_data_p);

  ff = (uiccd_msg_sim_file_get_format_response_t *)ste_msg;
  // Check if file has useless contents
  if (ff->uicc_status_code != SIM_UICC_STATUS_CODE_OK ||
      (ff->file_type == SIM_FILE_STRUCTURE_LINEAR_FIXED &&
       ff->num_records == 0) ||
      (ff->file_type == SIM_FILE_STRUCTURE_TRANSPARENT &&
       ff->file_size < 3)) {
    uiccd_read_ecc_send_response(ctrl_p->fd, ctrl_p->client_tag,
                                 SIM_UICC_STATUS_CODE_OK,
                                 SIM_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS,
                                 ff->status_word,
                                 NULL,
                                 0);
    uiccd_ecc_cleanup(ctrl_p);
    return 0; // Return transaction done

  }
  // The file exists with some content
  tdata = (trans_read_ecc_data_t *)malloc(sizeof(*tdata));
  ctrl_p->transaction_data_p = (void *)tdata;
  if (!tdata) {
    goto error;
  }
  if (ff->file_type == SIM_FILE_STRUCTURE_LINEAR_FIXED) {
    int rv = read_ecc_rr_request(ctrl_p, ff->record_len, ff->num_records);
    if (rv < 0) {
      goto error;
    }
    return 1; // Continue transaction
  }
  if (ff->file_type == SIM_FILE_STRUCTURE_TRANSPARENT) {
    // All parts of the file is optional, meaning that the max 5 ecc in there
    // may be interspersed with FF's.
    int rv = read_ecc_rb_request(ctrl_p, ff->file_size);
    if (rv < 0) {
      goto error;
    }
    return 1; // Success, continue transaction
  }
  // Fall through to error

 error:
  // Handle an internal error, and stop.
  uiccd_read_ecc_send_response(ctrl_p->fd, ctrl_p->client_tag,
                               SIM_UICC_STATUS_CODE_FAIL,
                               SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                               ff->status_word,
                               NULL, 0);
  uiccd_ecc_cleanup(ctrl_p);
  return 0;
}

/*
 * Reading out ecc codes works like this. ecc file is coded differently on
 * gsm and usim. For gsm, file is binary with 3 bytes per ecc. An ecc is only
 * the phone number coded in bcd, with unused digits set to f. (This is
 * like other dialling numbers). Max number of ecc codes are 5. The file has no
 * limitation on size, and the function allocates memory for the

 * ecc to be min of 5 and file size / 3.
 * The function excludes blank entries (numbers with only f),
 * meaning that the actual codes read out may not use up all memory allocated.
 * Then the spec allows ecc not to include any digits, meaning that actual codes
 * read out may not use up all memory allocated.
 */
int uiccd_main_read_ecc(ste_msg_t *ste_msg)
{
  int rv;
  uiccd_msg_read_ecc_t *msg = (uiccd_msg_read_ecc_t *)ste_msg;
  ste_sim_ctrl_block_t   *ctrl_p;

  catd_log_f(SIM_LOGGING_I, "uicc : READ ECC message received from: %d ",msg->fd);
  // The response ff function creates the transaction data, if needed.
  ctrl_p = ste_sim_create_ctrl_block(msg->client_tag, msg->fd, msg->type,
                                     uiccd_read_ecc_ff_response,
                                     NULL);
  if (!ctrl_p) {
    goto error;
  }
  // First step is to find out size and structure of the ecc then
  // the response function will go in and fill in the tdata.
  switch (uicc_get_app_type()) {
  case STE_SIM_APP_TYPE_SIM:
    catd_log_f(SIM_LOGGING_V, "uicc : ECC SIM");

    rv = ste_modem_sim_file_get_format(catd_get_modem(), (uintptr_t)ctrl_p,
                                       uicc_get_app_id(),
                                       ecc_file_id, ecc_paths.gsm);
    if (rv < 0) {
      goto error;
    }
    break;
  case STE_SIM_APP_TYPE_USIM:
    catd_log_f(SIM_LOGGING_V, "uicc : ECC USIM");

    rv = ste_modem_sim_file_get_format(catd_get_modem(), (uintptr_t)ctrl_p,
                                       uicc_get_app_id(),
                                       ecc_file_id, ecc_paths.usim);
    if (rv < 0) {
      goto error;
    }
    break;
  default:
    goto error;
    break;
  }
  // Point of successful return
  return 0;

  // Code below is cleanup only if error
 error:
  {
    sim_uicc_status_word_t status_word = {0,0};
    uiccd_read_ecc_send_response(msg->fd, msg->client_tag,
                                    SIM_UICC_STATUS_CODE_FAIL,
                                    SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                    status_word,
                                    NULL,
                                    0);
  }

  if (ctrl_p) {
    free(ctrl_p);
  }
  return -1;
}
