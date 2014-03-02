/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include "sim_internal.h"
#include "catd_modem_types.h"
#include "uicc_internal.h"
#include "simd.h"
#include "uiccd_msg.h"
#include "uicc_state_machine.h"

/**
 * START OF FDN handling
 */

/*
 * The EF FDN file stores fixed dialling numbers, i.e.
 * only those numbers are allowed to dial, and none else. The file is PIN2
 * write-protected and the handset typically has an interface to enable fixed
 * dialling feature, and add contacts to the FDN file. The file includes an
 * alpha string typically a contact name, like 'John Doe', and a dialling
 * string that is usually an ordinary contact phone number,
 * like 1-800-555-1234.
 * The dial string can include control characters like *, # etc, to control
 * network services, while not letters replacing digits.
 *
 * There is one record in the FDN file for each number. The dialling string
 * coded in BCD is divided into 10-byte pieces. The whole alpha string and
 * the first piece of dialling string go into the FDN record. Any remaining
 * dialling string pieces go into the EXT2 file across as many records as
 * needed. Since 20 BCD characters fit in 10 bytes, the pieces are not
 * more than a few. Also handsets seem to limit the dial string length.
 * For more details see 3GPP TS31.102 spec.
 */

// Length of dialstring segments in bytes in both fdn and ext2 files.
#define FDN_DIAL_PIECE_LENGTH 10

// Where to find useful info in fdn and ext2 records
#define FDN_RECORD_CUT_ALPHA_OFFSET 14 // To get past alpha in fdn record
// All below relative FDN_RECORD_CUT_ALPHAOFFSET
#define FDN_DIAL_PIECE_LENGTH_POS 0   // Dial string piece number of bytes
#define FDN_TON_NPI_POS 1
#define FDN_DIAL_PIECE_START_POS 2
#define FDN_EXT2_REC_ID_POS 13
// Absolute pos in ext2
#define EXT2_RECORD_LENGTH 13
#define EXT2_DIAL_PIECE_LENGTH_POS 1 // Dial string piece number of bytes
#define EXT2_DIAL_PIECE_START_POS 2
#define EXT2_MORE_REC_ID_POS 12

/* A dial string piece type.  Complete dial string is stored as a linked
 * list of pieces */
typedef struct piece_s piece_t;
struct piece_s {
  uint8_t length; // Length in bytes used in this piece
  uint8_t dial[FDN_DIAL_PIECE_LENGTH]; // Native coding is BCD
  piece_t *more;  // Any more pieces in ext2 file, NULL if none.
};

/* Representation of fdn data */
typedef struct {
  size_t  total_length;  // Accumulated dial piece length in bytes
  piece_t fdn_piece;    // Mandatory dial string piece in fdn file
  size_t  alpha_length;  // Length of alpha string in bytes
  uint8_t *alpha;  // Alpha string in its native (raw) coding
  uint8_t ton_npi; // Raw ton / npi byte
} fdn_t;

/* Used to record current state of linear fixed file access */
struct lf_state_s {
  int        file_id;
  const char *file_path;
  int        rsize; // Record size in bytes
  int        pos;   // Current file record position
  int        fsize; // File size in number of records
};

/* Data used for control info between aynch MAL calls. The ctrl_p data
 * points to one such struct that is passed around and modified */
typedef struct {
  struct lf_state_s fdn_state;
  struct lf_state_s ext_state;
  int   fdn_n;  // Max entries of fdn
  int   fdn_i;  // Current index into fdn
  fdn_t *fdn; // Array of records found in the file
} trans_read_fdn_data_t;

/* Paths for fdn and ext2 files. GSM and USIM has otherwise same spec for FDN */
static const struct {
  const char *gsm;
  const char *usim;
} fdn_paths = { "3F007F10", "3F007FFF" };

static const int fdn_file_id = 0x6F3B;
static const int ext2_file_id = 0x6F4B;

static int uiccd_read_ext_rr_response(ste_msg_t *ste_msg);

/* Frees all memory */
static void uiccd_fdn_cleanup(ste_sim_ctrl_block_t *ctrl_p) {
  trans_read_fdn_data_t *tdata_p;
  fdn_t *fdn_p;
  int fdn_n,i;

  if (!ctrl_p) {
    return;
  }
  tdata_p = ctrl_p->transaction_data_p;
  if (tdata_p) {
    fdn_p = tdata_p->fdn;
    fdn_n = tdata_p->fdn_n;
    if (fdn_p) {
      for(i=0;i<fdn_n;i++) {
        piece_t *pi = fdn_p[i].fdn_piece.more, *pii;
        while (pi) {
          pii = pi;
          pi = pi->more;
          free(pii);
        }
        if (fdn_p[i].alpha) free(fdn_p[i].alpha);
      }
      free(fdn_p);
    }
    free(tdata_p);
  }
  free(ctrl_p);
}

/*
 * Count number of BCD characters in a list of dial string pieces.
 */
static int fdn_get_code_num_chars(piece_t *code) {
  piece_t *c = code;
  int n = 0;

  while (c) {
    n += get_length_in_digits_for_BCD(c->dial, FDN_DIAL_PIECE_LENGTH);
    c = c->more;
  }
  return n;
}

/*
 * Writes out the dial string pieces onto the char buffer pointed to by p
 * and converts the string from BCD to ASCII8. The function advances p.
 */
static int fdn_enc_code_chars(piece_t *code, char **p) {
  piece_t *c = code;
  char *pp = *p; // Just to avoid the double pointer (in/out param)

  while (c) {
    ste_sim_text_t text;
    uint16_t res_len;
    int res;

    text.text_coding = STE_SIM_BCD;
    text.no_of_characters = get_length_in_digits_for_BCD(c->dial, FDN_DIAL_PIECE_LENGTH);
    text.text_p = c->dial;

    res = sim_convert_BCD_to_ASCII8(&text, 20, &res_len, (uint8_t *)pp);
    if (res < 0) {
      return -1;
    }
    pp += res_len;
    c = c->more;
  }
  *p = pp;
  return 0;
}

/*
 * Parse record from fdn file. Function returns true if parse ok,
 * otherwise false. Buf is the record contents of the fd, buf_len is
 * the record size
 */
static int fdn_parse_fdn_record(const uint8_t *buf, size_t buf_len,
                                fdn_t *fdn, uint8_t *ext) {
  size_t alpha_max_len = buf_len - FDN_RECORD_CUT_ALPHA_OFFSET;
  // Buf2 set to end of alpha string
  const uint8_t *buf2 = buf + alpha_max_len;

  // Is this a blank record? Check the number of bytes position. 0xff unused.
  if (buf2[FDN_DIAL_PIECE_LENGTH_POS] == 0xff) {
    return 0;
  }

  // Get the alpha and its length
  if (alpha_max_len > 0) {
    size_t len;

    for (len = 0; len < alpha_max_len; len++)
      if (buf[len] == 0xff) break;
    if (len > 0) {
      uint8_t *a;

      a = malloc(len);
      if (a) {
        fdn->alpha = a;
        memcpy(a, buf, len);
        fdn->alpha_length = len;
      }
    }
  }

  fdn->ton_npi = buf2[FDN_TON_NPI_POS];
  fdn->fdn_piece.length = buf2[FDN_DIAL_PIECE_LENGTH_POS];

  // TON/NPI shall be left at 0xff if dial string contains no number,
  // meaning that it only has SS controls strings. Then the TON/NPI is removed
  // from sending it to network, and buf2[0] does not include it in the length
  if (fdn->ton_npi != 0xff) {
    fdn->fdn_piece.length--;
  }
  memcpy(fdn->fdn_piece.dial,
         &buf2[FDN_DIAL_PIECE_START_POS],
         FDN_DIAL_PIECE_LENGTH);
  fdn->total_length = fdn->fdn_piece.length;
  // Only set the ext flag if the dial string spills over into ext. The spec
  // allows the link to exist even if there happens to be nothing to get from
  // the ext records
  if (fdn->fdn_piece.length == FDN_DIAL_PIECE_LENGTH) {
    *ext = buf2[FDN_EXT2_REC_ID_POS];
  }
  else {
    *ext = 0xff;
  }

  catd_log_f(SIM_LOGGING_V, "uicc : fdn_parse alpha='%.*s' num_len=%d ton/npi=%x ext=%x", fdn->alpha_length, fdn->alpha, fdn->fdn_piece.length, fdn->ton_npi, *ext);

  return 1; // TRUE => Record was parsed out ok!
}

/*
 * Send response of fdn read back to client
 */
static void uiccd_read_fdn_send_response(int fd, uintptr_t client_tag,
                                         sim_uicc_status_code_t uicc_status_code,
                                         sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                         sim_uicc_status_word_t status_word,
                                         fdn_t *fdn,
                                         int n) {
  int i;
  size_t buf_len, fdn_len = (size_t)n;
  char *p = NULL, *buf_p = NULL;

  // This should write out data using the same basic types as in sim.h
  buf_len = sizeof(uicc_status_code) + sizeof(uicc_status_code_fail_details);
  buf_len += sizeof(status_word.sw1) + sizeof(status_word.sw2);
  buf_len += sizeof(fdn_len);
  // Sum up the size of all fdn records
  for (i=0;i<n;i++) {
    size_t my_size;
    unsigned code_num_chars = (unsigned)fdn_get_code_num_chars(&fdn[i].fdn_piece);
    size_t code_size = (size_t)code_num_chars;
    unsigned alpha_num_chars = (unsigned)fdn[i].alpha_length;
    // Number

    my_size = sizeof(ste_sim_text_coding_t) + sizeof(code_num_chars);
    my_size += sizeof(code_size);
    my_size += sizeof(char) * code_num_chars;
    // Alpha
    my_size += sizeof(ste_sim_text_coding_t) + sizeof(alpha_num_chars);
    my_size += sizeof(fdn[i].alpha_length);
    my_size += sizeof(uint8_t) * alpha_num_chars;
    // TON/NPI
    my_size += sizeof(ste_sim_type_of_number_t) + sizeof(ste_sim_numbering_plan_id_t);

    catd_log_f(SIM_LOGGING_V, "uicc : fdn_send_response d_size=%d, alpha_size=%d of fdn %d is %d", code_size, fdn[i].alpha_length, i, my_size);

    buf_len += my_size;
  }
  catd_log_f(SIM_LOGGING_V, "uicc : fdn_send_response buf_len=%d", buf_len);

  buf_p = malloc(buf_len);
  if (!buf_p) {
    catd_log_f(SIM_LOGGING_E, "uicc : memory allocation failed");
    goto error;
  }
  // Write out all fdn records
  p = buf_p;
  p = sim_enc(p, &uicc_status_code, sizeof(uicc_status_code));
  p = sim_enc(p, &uicc_status_code_fail_details, sizeof(uicc_status_code_fail_details));
  p = sim_enc(p, &status_word.sw1, sizeof(status_word.sw1));
  p = sim_enc(p, &status_word.sw2, sizeof(status_word.sw2));
  p = sim_enc(p, &fdn_len, sizeof(fdn_len));
  for (i=0;i<n;i++) {
    int res;
    ste_sim_text_coding_t num_coding = STE_SIM_ASCII8;
    ste_sim_text_coding_t alpha_coding = STE_SIM_ASCII8;
    unsigned code_num_chars = (unsigned)fdn_get_code_num_chars(&fdn[i].fdn_piece);
    unsigned alpha_num_chars = (unsigned)fdn[i].alpha_length;
    size_t code_size = (size_t)code_num_chars;
    ste_sim_type_of_number_t ton;
    ste_sim_numbering_plan_id_t npi;

    // Number
    p = sim_enc(p, &num_coding, sizeof(num_coding));
    p = sim_enc(p, &code_num_chars, sizeof(code_num_chars));
    p = sim_enc(p, &code_size, sizeof(code_size));
    res = fdn_enc_code_chars(&fdn[i].fdn_piece, &p);
    if (res < 0) goto error;
    // Alpha
    p = sim_enc(p, &alpha_coding, sizeof(alpha_coding));
    p = sim_enc(p, &alpha_num_chars, sizeof(alpha_num_chars));
    p = sim_enc(p, &fdn[i].alpha_length, sizeof(fdn[i].alpha_length));
    if (fdn[i].alpha_length > 0) {
      p = sim_enc(p, fdn[i].alpha, fdn[i].alpha_length);
    }
    // TON and NPI
    sim_decode_ton_npi(fdn[i].ton_npi, &ton, &npi);
    p = sim_enc(p, &ton, sizeof(ton));
    p = sim_enc(p, &npi, sizeof(npi));
  }
  if (buf_len != (size_t)(p - buf_p)) {
    catd_log_f(SIM_LOGGING_E, "uicc: read_fdn_send_response assert failure");
  }

  assert( buf_len == (size_t)(p - buf_p) );

  sim_send_generic(fd, STE_UICC_RSP_READ_FDN,
                   buf_p, buf_len, client_tag);
  free(buf_p);
  return;
 error:
  // The lib classifies this case as an internal error
  sim_send_generic(fd, STE_UICC_RSP_READ_FDN,
                   NULL, 0, client_tag);
  if (buf_p) free(buf_p);
}

static int uiccd_read_fdn_rr_response(ste_msg_t *ste_msg)
{
  ste_sim_ctrl_block_t  *ctrl_p;
  trans_read_fdn_data_t *tdata;
  uiccd_msg_read_sim_file_record_response_t *rr;
  int parsed;
  uint8_t ext;

  catd_log_f(SIM_LOGGING_I, "uicc : read fdn rr response called");

  // Initial asertions on input parameters
  assert(ste_msg);
  assert(ste_msg->type == UICCD_MSG_READ_SIM_FILE_RECORD_RSP);
  ctrl_p = (ste_sim_ctrl_block_t *)(ste_msg->client_tag);
  assert(ctrl_p);
  tdata = (trans_read_fdn_data_t *)(ctrl_p->transaction_data_p);
  // tdata must have info from previous file format
  assert(tdata);

  rr = (uiccd_msg_read_sim_file_record_response_t *)ste_msg;

  // Test to see if read record returned useless data
  if (rr->uicc_status_code != SIM_UICC_STATUS_CODE_OK ||
      rr->len != (size_t)tdata->fdn_state.rsize) {
    // Give up!
    goto error;
  }
  // Parse the record, and store result in current pos of fdn array.
  // fdn_i is not incremented there
  parsed = fdn_parse_fdn_record(rr->data, rr->len,
                                &tdata->fdn[tdata->fdn_i],
                                &ext);
  if (!parsed) {
    catd_log_f(SIM_LOGGING_V, "uicc : fdn skipped blank number at rec %d of %d", tdata->fdn_state.pos, tdata->fdn_state.fsize);
  }
  else if (ext > 0 && ext < 0xff) {
    // Set right call back for ext handling
    ctrl_p->transaction_handler_func = uiccd_read_ext_rr_response;
    int rv = ste_modem_file_read_record(catd_get_modem(),
                                        (uintptr_t)ctrl_p, uicc_get_app_id(),
                                        tdata->ext_state.file_id,
                                        ext, 0,
                                        tdata->ext_state.file_path);
    if (rv < 0) goto error;
    return 1;
  }
  else { // Parsed ok but no ext or ccp to process
    catd_log_f(SIM_LOGGING_V, "uicc : fdn found number at rec %d of %d", tdata->fdn_state.pos, tdata->fdn_state.fsize);
    // Increment fdn
    tdata->fdn_i++;
  }
  // Arrive here if not ext and not ccp, so finish up this record and leave
  tdata->fdn_state.pos++;
  // Check if we are done with file
  if (tdata->fdn_state.pos > tdata->fdn_state.fsize ||
      tdata->fdn_i >= tdata->fdn_n) {
    catd_log_f(SIM_LOGGING_V, "uicc : fdn collected %d numbers out of estimated %d", tdata->fdn_i, tdata->fdn_n);
    uiccd_read_fdn_send_response(ctrl_p->fd, ctrl_p->client_tag,
                                 SIM_UICC_STATUS_CODE_OK,
                                 SIM_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS,
                                 rr->status_word,
                                 tdata->fdn,
                                 tdata->fdn_i);
    uiccd_fdn_cleanup(ctrl_p);
    return 0; // Transaction ended
  }

  int rv = ste_modem_file_read_record(catd_get_modem(),
                                      (uintptr_t)ctrl_p, uicc_get_app_id(),
                                      tdata->fdn_state.file_id,
                                      tdata->fdn_state.pos, 0,
                                      tdata->fdn_state.file_path);
  if (rv < 0)  {
    goto error;
  }
  return 1;  // Transaction in progress

 error:
  // Transaction abort
  uiccd_read_fdn_send_response(ctrl_p->fd, ctrl_p->client_tag,
                               SIM_UICC_STATUS_CODE_FAIL,
                               SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                               rr->status_word, NULL, 0);
  uiccd_fdn_cleanup(ctrl_p);
  return 0;
}

static int uiccd_read_fdn_ff_response(ste_msg_t *ste_msg)
{
  ste_sim_ctrl_block_t *ctrl_p;
  trans_read_fdn_data_t *tdata;
  uiccd_msg_sim_file_get_format_response_t *ff;
  int rv;
  size_t fdn_size;
  fdn_t *fdn;
  int fdn_n;

  catd_log_f(SIM_LOGGING_I, "uicc : read fdn ff response called");

  // Place some assertions on things that must be true at entry
  assert(ste_msg);
  assert(ste_msg->type == UICCD_MSG_SIM_FILE_GET_FORMAT_RSP);
  ctrl_p = (ste_sim_ctrl_block_t *)(ste_msg->client_tag);
  assert(ctrl_p);
  tdata = ctrl_p->transaction_data_p;
  assert(tdata);

  // Analyze response. Test below if file lacks useful data.
  ff = (uiccd_msg_sim_file_get_format_response_t *)ste_msg;
  if (ff->uicc_status_code != SIM_UICC_STATUS_CODE_OK ||
      ff->file_type != SIM_FILE_STRUCTURE_LINEAR_FIXED ||
      (ff->file_type == SIM_FILE_STRUCTURE_LINEAR_FIXED &&
       ff->num_records == 0)) {
    // Give up
    uiccd_read_fdn_send_response(ctrl_p->fd, ctrl_p->client_tag,
                                 SIM_UICC_STATUS_CODE_OK,
                                 SIM_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS,
                                 ff->status_word,
                                 NULL,
                                 0);
    uiccd_fdn_cleanup(ctrl_p);
    return 0; // Done and stop transaction handling
  }
  catd_log_f(SIM_LOGGING_V, "uicc : fdn file: no of records=%d, rec len=%d", ff->num_records, ff->record_len);
  // Setup the fdn with size number of records in file
  fdn_n = ff->num_records;
  fdn_size = sizeof(*fdn)*fdn_n;
  fdn = (fdn_t *)malloc(fdn_size);
  memset(fdn, 0, fdn_size);
  // Initialize the transaction data to be passed around with ctrl_p
  tdata->fdn = fdn;
  tdata->fdn_n = fdn_n;
  tdata->fdn_i = 0;
  tdata->fdn_state.pos = 1;
  tdata->fdn_state.rsize = ff->record_len;
  tdata->fdn_state.fsize = ff->num_records;
  // Set handler function for this call
  ctrl_p->transaction_handler_func = uiccd_read_fdn_rr_response;

  rv = ste_modem_file_read_record(catd_get_modem(),
                                  (uintptr_t)ctrl_p, uicc_get_app_id(),
                                  tdata->fdn_state.file_id,
                                  tdata->fdn_state.pos, 0,
                                  tdata->fdn_state.file_path);
  if (rv < 0)  {
    goto error;
  }
  return 1; // Continue transaction
  // Fall through to error
 error:
  // Handle an internal error, and stop.
  uiccd_read_fdn_send_response(ctrl_p->fd, ctrl_p->client_tag,
                               SIM_UICC_STATUS_CODE_FAIL,
                               SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                               ff->status_word,
                               NULL, 0);
  uiccd_fdn_cleanup(ctrl_p);
  return 0;
}

/*
 * The main function starts by setting up transaction data and control
 * block according to type of sim. Then it requests file format to
 * know more details.
 * The ctrl_p will have its transaction handler func pointer set
 * to the proper call back function before the call rather than having one
 * BIIG function to switches on a transaction state enum value.
 */
int uiccd_main_read_fdn(ste_msg_t *ste_msg)
{
  int rv;
  ste_modem_t           *m;
  uiccd_msg_read_fdn_t  *msg = (uiccd_msg_read_fdn_t *)ste_msg;
  ste_sim_ctrl_block_t  *ctrl_p = NULL;
  trans_read_fdn_data_t *tdata_p = NULL;
  catd_log_f(SIM_LOGGING_I, "uicc : READ FDN message received from: %d ",msg->fd);
  m = catd_get_modem();
  if (!m) {
    catd_log_f(SIM_LOGGING_E, "uicc : No modem");
    goto error;
  }
  tdata_p = malloc(sizeof(*tdata_p));
  if (!tdata_p) {
    goto error;
  }
  memset(tdata_p, 0, sizeof(*tdata_p));
  ctrl_p = ste_sim_create_ctrl_block(msg->client_tag, msg->fd, msg->type,
                                     uiccd_read_fdn_ff_response,
                                     tdata_p);
  if (!ctrl_p) {
    goto error;
  }
  tdata_p->fdn_state.file_id = fdn_file_id;
  tdata_p->ext_state.file_id = ext2_file_id;

  switch (uicc_get_app_type()) {
  case STE_SIM_APP_TYPE_SIM:
    catd_log_f(SIM_LOGGING_V, "uicc : FDN SIM");
    tdata_p->fdn_state.file_path = fdn_paths.gsm;
    tdata_p->ext_state.file_path = fdn_paths.gsm;
    break;
  case STE_SIM_APP_TYPE_USIM:
    catd_log_f(SIM_LOGGING_V, "uicc : FDN USIM");
    tdata_p->fdn_state.file_path = fdn_paths.usim;
    tdata_p->ext_state.file_path = fdn_paths.usim;
    break;
  default:
    goto error;
    break;
  }

  rv = ste_modem_sim_file_get_format(m, (uintptr_t)ctrl_p, uicc_get_app_id(),
                                     tdata_p->fdn_state.file_id,
                                     tdata_p->fdn_state.file_path);
  if (rv < 0)  {
    goto error;
  }
  // Point of successful return
  return 0;
  // Code below is cleanup only if error

 error:
  {
    sim_uicc_status_word_t status_word = {0,0};
    uiccd_read_fdn_send_response(msg->fd, msg->client_tag,
                                 SIM_UICC_STATUS_CODE_FAIL,
                                 SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                 status_word,
                                 NULL,
                                 0);
  }

  uiccd_fdn_cleanup(ctrl_p);
  return -1;
}

/*
 * Handle response from reading a record from ext2 file
 */
static int uiccd_read_ext_rr_response(ste_msg_t *ste_msg)
{
  ste_sim_ctrl_block_t  *ctrl_p;
  trans_read_fdn_data_t *tdata;
  uiccd_msg_read_sim_file_record_response_t *rr;
  int rv;

  catd_log_f(SIM_LOGGING_I, "uicc : read ext rr response called");
  // Initial assertions to make sure all is ok at entry to this fn
  assert(ste_msg);
  assert(ste_msg->type == UICCD_MSG_READ_SIM_FILE_RECORD_RSP);
  ctrl_p = (ste_sim_ctrl_block_t *)(ste_msg->client_tag);
  assert(ctrl_p);
  tdata = (trans_read_fdn_data_t *)(ctrl_p->transaction_data_p);
  // tdata must have info from previous file format
  assert(tdata);

  rr = (uiccd_msg_read_sim_file_record_response_t *)ste_msg;
  // Check if the file/record has useless data
  if (rr->uicc_status_code != SIM_UICC_STATUS_CODE_OK ||
      rr->len != EXT2_RECORD_LENGTH) {
    // Give up!
    goto error;
  }

  // Length of data means something to get
  if (rr->data[EXT2_DIAL_PIECE_LENGTH_POS] > 0 &&
      rr->data[EXT2_DIAL_PIECE_LENGTH_POS] < 0xff) {
    piece_t *ext, *e;

    // Create a new cell for this data in the list of dial pieces.
    ext = (piece_t *)malloc(sizeof(piece_t));
    if (!ext)  {
      goto error;
    }
    // Setup ext and copy data from the record
    ext->length = rr->data[EXT2_DIAL_PIECE_LENGTH_POS];
    ext->more = NULL;
    memcpy(ext->dial, &rr->data[EXT2_DIAL_PIECE_START_POS], FDN_DIAL_PIECE_LENGTH);
    // Search for end of linked list of pieces
    e = tdata->fdn[tdata->fdn_i].fdn_piece.more;
    if (!e) {
      tdata->fdn[tdata->fdn_i].fdn_piece.more = ext;
    }
    else {
      while (e->more) {
        e=e->more;
      }
      e->more = ext; // Add this at end
    }
    // Add its length to total length
    tdata->fdn[tdata->fdn_i].total_length =+ ext->length;

    // Look if this record refers to another record with even more data
    if (ext->length == FDN_DIAL_PIECE_LENGTH && rr->data[EXT2_MORE_REC_ID_POS] < 0xff) {
      // Go fetch!

      rv = ste_modem_file_read_record(catd_get_modem(),
                                          (uintptr_t)ctrl_p, uicc_get_app_id(),
                                          tdata->ext_state.file_id,
                                          rr->data[EXT2_MORE_REC_ID_POS], 0,
                                          tdata->ext_state.file_path);
      if (rv < 0) {
        goto error;
      }
      return 1;
    }
  }

  // This point is reached if there is no more data in ext2 to fetch, so go
  // back to fdn and resume searching for remaining entries there.

  tdata->fdn_i++;         // Next free slot
  tdata->fdn_state.pos++; // Next record
  // Check if we're at end of file, or end of fdn
  if (tdata->fdn_state.pos > tdata->fdn_state.fsize ||
      tdata->fdn_i >= tdata->fdn_n) {

    catd_log_f(SIM_LOGGING_V, "uicc : fdn collected %d numbers out of estimated %d", tdata->fdn_i, tdata->fdn_n);
    // Done!
    uiccd_read_fdn_send_response(ctrl_p->fd, ctrl_p->client_tag,
                                 SIM_UICC_STATUS_CODE_OK,
                                 SIM_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS,
                                 rr->status_word,
                                 tdata->fdn,
                                 tdata->fdn_i);
    uiccd_fdn_cleanup(ctrl_p);
    return 0; // Transaction ended
  }

  // Not at end of file
  ctrl_p->transaction_handler_func = uiccd_read_fdn_rr_response;

  rv = ste_modem_file_read_record(catd_get_modem(),
                                      (uintptr_t)ctrl_p, uicc_get_app_id(),
                                      tdata->fdn_state.file_id,
                                      tdata->fdn_state.pos, 0,
                                      tdata->fdn_state.file_path);
  if (rv < 0)  {
    goto error;
  }
  return 1;  // Transaction in progress

 error:
  // Transaction abort
  uiccd_read_fdn_send_response(ctrl_p->fd, ctrl_p->client_tag,
                               SIM_UICC_STATUS_CODE_FAIL,
                               SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                               rr->status_word, NULL, 0);
  uiccd_fdn_cleanup(ctrl_p);
  return 0;
}
