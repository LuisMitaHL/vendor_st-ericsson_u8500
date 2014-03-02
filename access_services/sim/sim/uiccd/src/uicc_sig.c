/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * File name       : uicc_sig.c
 * Description     : Sends (or "signals") data to the server main loop (uiccd).
 *                   The reader thread executes the functions. The input is
 *                   data from clients, or modem. Output is the data decoded
 *                   and refined into typed messages, and put into the
 *                   message queue. This relieves the top loop from further
 *                   decoding of the messages. It can apply its logic directly.
 *                   Note that for messages coming from the modem, the data
 *                   MUST be modem neutral; however it is legal for the
 *                   messages to contain SIM specific data. Modem-specific
 *                   processing takes place within the M*L (MFL or MAL).
 *
 * Author          : Mats Bergstrom <mats.m.bergstrom@stericsson.com>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdarg.h>
#include <assert.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "catd_reader.h"
#include "cat_internal.h"
#include "cat_barrier.h"
#include "msgq.h"

#include "catd_modem.h"
#include "simd.h"
#include "uicc_internal.h"
#include "uiccd.h"
#include "uiccd_msg.h"

#include "sim_unused.h"



// Client command (coming from reader thread receiving client requests)
void uiccd_sig_register(int fd, uintptr_t client_tag)
{
    uiccd_msg_register_t   *msg;
    msg = uiccd_msg_register_create(fd, client_tag);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

// Client command (coming from reader thread receiving client requests)
void uiccd_sig_disconnect(int fd, uintptr_t client_tag)
{
    uiccd_msg_disconnect_t *msg;
    msg = uiccd_msg_disconnect_create(fd, client_tag);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

// Client command (coming from reader thread receiving client requests)
void uiccd_sig_startup(int fd, uintptr_t client_tag)
{
    uiccd_msg_startup_t   *msg;

    catd_log_f(SIM_LOGGING_I, "SIMD call uiccd_sig_startup");
    msg = uiccd_msg_startup_create(fd, client_tag);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

void uiccd_sig_startup_done(int fd, uintptr_t client_tag)
{
    uiccd_msg_startup_done_t   *msg;
    msg = uiccd_msg_startup_done_create(fd, client_tag);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

// Client command (coming from reader thread receiving client requests)
void uiccd_sig_shutdown(int fd, uintptr_t client_tag)
{
    uiccd_msg_shutdown_t   *msg;
    msg = uiccd_msg_shutdown_create(fd, client_tag);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

void uiccd_sig_modem_silent_reset(int fd, uintptr_t client_tag)
{
    uiccd_msg_modem_silent_reset_t   *msg;
    msg = uiccd_msg_modem_silent_reset_create(fd, client_tag);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

// Client command (coming from reader thread receiving client requests)
void uiccd_sig_pin_verify(int fd, uintptr_t client_tag, const char *buf, size_t len)
{
    uiccd_msg_pin_verify_t *msg;
    msg = uiccd_msg_pin_verify_create(fd, client_tag, buf, len);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

// Client command (coming from reader thread receiving client requests)
void uiccd_sig_pin_change(int fd, uintptr_t client_tag, const char *buf, size_t len)
{
    uiccd_msg_pin_change_t *msg;
    msg = uiccd_msg_pin_change_create(fd, client_tag, buf, len);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

// Client command (coming from reader thread receiving client requests)
void uiccd_sig_pin_disable(int fd, uintptr_t client_tag, const char *buf, size_t len)
{
    uiccd_msg_pin_disable_t *msg;
    msg = uiccd_msg_pin_disable_create(fd, client_tag, buf, len);
    uiccd_sig_msg( (ste_msg_t *) msg);
}


void uiccd_sig_pin_enable(int fd, uintptr_t client_tag, const char *buf, size_t len)
{
    uiccd_msg_pin_enable_t *msg;
    msg = uiccd_msg_pin_enable_create(fd, client_tag, buf, len);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

// Client command (coming from reader thread receiving client requests)
void uiccd_sig_pin_info(int fd, uintptr_t client_tag, const char *buf, size_t len)
{
    uiccd_msg_pin_info_t *msg;
    msg = uiccd_msg_pin_info_create(fd, client_tag, buf, len);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

// Client command (coming from reader thread receiving client requests)
void uiccd_sig_pin_unblock(int fd, uintptr_t client_tag, const char *buf, size_t len)
{
    uiccd_msg_pin_unblock_t *msg;
    msg = uiccd_msg_pin_unblock_create(fd, client_tag, buf, len);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

void uiccd_sig_read_sim_file_record(int fd, uintptr_t client_tag, const char *buf, size_t len)
{
    uiccd_msg_read_sim_file_record_t *msg;

    msg = uiccd_msg_read_sim_file_record_create(fd, client_tag, buf, len);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

// Client command (coming from reader thread receiving client requests)
void uiccd_sig_read_sim_file_binary(int fd, uintptr_t client_tag, const char *buf, size_t len)
{
    uiccd_msg_read_sim_file_binary_t *msg;

    msg = uiccd_msg_read_sim_file_binary_create(fd, client_tag, buf, len);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

// Client command (coming from reader thread receiving client requests)
void uiccd_sig_update_sim_file_record(int fd, uintptr_t client_tag, const char *buf, size_t len)
{
    uiccd_msg_update_sim_file_record_t *msg;
    msg = uiccd_msg_update_sim_file_record_create(fd, client_tag, buf, len);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

// Client command (coming from reader thread receiving client requests)
void uiccd_sig_update_sim_file_binary(int fd, uintptr_t client_tag, const char *buf, size_t len)
{
    uiccd_msg_update_sim_file_binary_t *msg;
    msg = uiccd_msg_update_sim_file_binary_create(fd, client_tag, buf, len);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

// Client command (coming from reader thread receiving client requests)
void uiccd_sig_sim_file_get_format(int fd, uintptr_t client_tag, const char *buf, size_t len)
{
    uiccd_msg_sim_file_get_format_t *msg;
    msg = uiccd_msg_sim_file_get_format_create(fd, client_tag, buf, len);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

// Client command (coming from reader thread receiving client requests)
void uiccd_sig_get_file_information(int fd, uintptr_t client_tag, const char *buf, size_t len)
{
    uiccd_msg_get_file_information_t *msg;
    msg = uiccd_msg_get_file_information_create(fd, client_tag, buf, len);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

// Client command (coming from reader thread receiving client requests)
void uiccd_sig_get_sim_state(int fd, uintptr_t client_tag, const char *buf, size_t len)
{
    uiccd_msg_get_sim_state_t *msg;
    msg = uiccd_msg_get_sim_state_create(fd, client_tag, buf, len);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

// Client command (coming from reader thread receiving client requests)
void uiccd_sig_read_smsc(int fd, uintptr_t client_tag, const char *buf, size_t len)
{
    uiccd_msg_read_smsc_t *msg;
    msg = uiccd_msg_read_smsc_create(fd, client_tag, buf, len);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

// Client command (coming from reader thread receiving client requests)
void uiccd_sig_update_smsc(int fd, uintptr_t client_tag, const char *buf, size_t len)
{
    uiccd_msg_update_smsc_t *msg;
    msg = uiccd_msg_update_smsc_create(fd, client_tag, buf, len);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

// Client command (coming from reader thread receiving client requests)
void uiccd_sig_smsc_get_record_max(int fd, uintptr_t client_tag, const char *buf, size_t len)
{
    uiccd_msg_smsc_get_record_max_t *msg;
    msg = uiccd_msg_smsc_get_record_max_create(fd, client_tag, buf, len);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

// Client command (coming from reader thread receiving client requests)
void uiccd_sig_smsc_save_to_record(int fd, uintptr_t client_tag, const char *buf, size_t len)
{
    uiccd_msg_smsc_save_to_record_t *msg;
    msg = uiccd_msg_smsc_save_to_record_create(fd, client_tag, buf, len);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

// Client command (coming from reader thread receiving client requests)
void uiccd_sig_smsc_restore_from_record(int fd, uintptr_t client_tag, const char *buf, size_t len)
{
    uiccd_msg_smsc_restore_from_record_t *msg;
    msg = uiccd_msg_smsc_restore_from_record_create(fd, client_tag, buf, len);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

// Client command (coming from reader thread receiving client requests)
void uiccd_sig_sim_file_read_generic(int fd, uintptr_t client_tag, const char *buf, size_t len)
{
    uiccd_msg_sim_file_read_generic_t *msg;

    msg = uiccd_msg_sim_file_read_generic_create(fd, client_tag, buf, len);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

// Client command (coming from reader thread receiving client requests)
void uiccd_sig_update_service_table(int fd, uintptr_t client_tag, const char *buf, size_t len)
{
    uiccd_msg_update_service_table_t *msg;
    msg = uiccd_msg_update_service_table_create(fd, client_tag, buf, len);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

// Client command (coming from reader thread receiving client requests)
void uiccd_sig_get_service_table(int fd, uintptr_t client_tag, const char *buf, size_t len)
{
    uiccd_msg_get_service_table_t *msg;
    msg = uiccd_msg_get_service_table_create(fd, client_tag, buf, len);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

// Client command (coming from reader thread receiving client requests)
void uiccd_sig_get_service_availability(int fd, uintptr_t client_tag, const char *buf, size_t len)
{
    uiccd_msg_get_service_availability_t *msg;
    msg = uiccd_msg_get_service_availability_create(fd, client_tag, buf, len);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

// Client command (coming from reader thread receiving client requests)
void uiccd_sig_read_plmn(int fd, uintptr_t client_tag, const char *buf, size_t len)
{
    uiccd_msg_read_smsc_t *msg;
    msg = uiccd_msg_read_plmn_create(fd, client_tag, buf, len);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

// Client command (coming from reader thread receiving client requests)
void uiccd_sig_update_plmn(int fd, uintptr_t client_tag, const char *buf, size_t len)
{
    uiccd_msg_read_smsc_t *msg;
    msg = uiccd_msg_update_plmn_create(fd, client_tag, buf, len);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

#if 0
/* Example of version not using uiccd_msg.c. Fns in this files does nothing,
 * so we could remove some complexity
 */
/*
 * Basic function to create msgs with fd.
 */
static uiccd_msg_fd_t new_ste_msg_fd(size_t size, int type, int fd,
                                     uintptr_t client_tag)
{
  uiccd_msg_fd_t *p;
  assert( size >= sizeof(uiccd_msg_fd_t) );
  p = malloc(s);
  if (!p) return NULL;
  p->type = type;
  p->delete_func = 0;
  p->client_tag = client_tag;
  p->fd = fd;
}

void uiccd_sig_get_app_info(int fd, uintptr_t client_tag)
{
  uiccd_msg_app_info_t *msg;
  msg = new_ste_fd_msg(sizeof(*msg), UICCD_MSG_APP_INFO, fd, client_tag);
  if (!msg) return;
  uiccd_sig_msg( (ste_msg_t *)msg);
}
#endif

void uiccd_sig_get_app_info(int fd, uintptr_t client_tag)
{
  uiccd_msg_app_info_t *msg;
  msg = uiccd_msg_app_info_create(fd, client_tag);
  uiccd_sig_msg( (ste_msg_t *) msg);
}

// Server event (coming from msl or mal thread)
void uiccd_sig_pin_verify_response(uintptr_t                           client_tag,
                                   sim_uicc_status_code_t              uicc_status_code,
                                   sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                   sim_uicc_status_word_t              status_word)
{
    uiccd_msg_pin_verify_response_t   *msg;
    msg = uiccd_msg_pin_verify_response_create(client_tag,
                                               uicc_status_code,
                                               uicc_status_code_fail_details,
                                               status_word);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

// Server event (coming from msl or mal thread)
void uiccd_sig_pin_change_response(uintptr_t                           client_tag,
                                   sim_uicc_status_code_t              uicc_status_code,
                                   sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                   sim_uicc_status_word_t              status_word)
{
    uiccd_msg_pin_change_response_t   *msg;
    msg = uiccd_msg_pin_change_response_create(client_tag,
                                               uicc_status_code,
                                               uicc_status_code_fail_details,
                                               status_word);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

// Server event (coming from msl or mal thread)
void uiccd_sig_pin_disable_response(uintptr_t                           client_tag,
                                    sim_uicc_status_code_t              uicc_status_code,
                                    sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                    sim_uicc_status_word_t              status_word)
{
    uiccd_msg_pin_disable_response_t   *msg;
    msg = uiccd_msg_pin_disable_response_create(client_tag,
                                                uicc_status_code,
                                                uicc_status_code_fail_details,
                                                status_word);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

void uiccd_sig_pin_enable_response(uintptr_t                           client_tag,
                                   sim_uicc_status_code_t              uicc_status_code,
                                   sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                   sim_uicc_status_word_t              status_word)
{
    uiccd_msg_pin_enable_response_t   *msg;
    msg = uiccd_msg_pin_enable_response_create(client_tag,
                                               uicc_status_code,
                                               uicc_status_code_fail_details,
                                               status_word);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

void uiccd_sig_pin_info_response(uintptr_t client_tag,
                                 sim_uicc_status_code_t uicc_status_code,
                                 sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                 ste_sim_pin_status_t pin_status,
                                 int attempts,
                                 int attempts2)
{
    uiccd_msg_pin_info_response_t   *msg;
    msg = uiccd_msg_pin_info_response_create(client_tag,
                                             uicc_status_code,
                                             uicc_status_code_fail_details,
                                             pin_status, attempts, attempts2);
    uiccd_sig_msg( (ste_msg_t *) msg);
}


void uiccd_sig_pin_unblock_response(uintptr_t                          client_tag,
                                   sim_uicc_status_code_t              uicc_status_code,
                                   sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                   sim_uicc_status_word_t              status_word)
{
    uiccd_msg_pin_unblock_response_t   *msg;
    msg = uiccd_msg_pin_unblock_response_create(client_tag,
                                                uicc_status_code,
                                                uicc_status_code_fail_details,
                                                status_word);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

void uiccd_sig_read_sim_file_record_response(uintptr_t                           client_tag,
                                             sim_uicc_status_code_t              uicc_status_code,
                                             sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                             sim_uicc_status_word_t              status_word,
                                             const uint8_t*                      data,
                                             size_t                              len)
{
    uiccd_msg_read_sim_file_record_response_t   *msg;
    msg = uiccd_msg_read_sim_file_record_response_create(client_tag, uicc_status_code, uicc_status_code_fail_details,
                                                         status_word, data, len);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

// Server event (coming from msl or mal thread)

void uiccd_sig_read_sim_file_binary_response(uintptr_t                           client_tag,
                                             sim_uicc_status_code_t              uicc_status_code,
                                             sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                             sim_uicc_status_word_t              status_word,
                                             const uint8_t*                      data,
                                             size_t                              len)
{
    uiccd_msg_read_sim_file_binary_response_t   *msg;
    msg = uiccd_msg_read_sim_file_binary_response_create(client_tag, uicc_status_code, uicc_status_code_fail_details,
                                                         status_word, data, len);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

// Server event (coming from msl or mal thread)
// FIXME: Implement for DF and MF selection
// fci a'la usim 102.221
void uiccd_sig_sim_file_get_format_response(uintptr_t                           client_tag,
                                            sim_uicc_status_code_t              uicc_status_code,
                                            sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                            sim_uicc_status_word_t              status_word,
                                            sim_file_structure_t                file_type,
                                            int                                 file_size,
                                            int                                 record_len,
                                            int                                 num_records)
{
  uiccd_msg_sim_file_get_format_response_t *p;
  p = uiccd_msg_sim_file_get_format_response_create(client_tag,
                                                    uicc_status_code,
                                                    uicc_status_code_fail_details,
                                                    status_word,
                                                    file_type,
                                                    file_size,
                                                    record_len,
                                                    num_records);

  uiccd_sig_msg( (ste_msg_t *) p);
}


// Server event (coming from msl or mal thread)

void uiccd_sig_appl_apdu_send_response(uintptr_t                          client_tag,
                                       sim_uicc_status_code_t              uicc_status_code,
                                       sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                       sim_uicc_status_word_t              status_word,
                                       const uint8_t*                      apdu_data,
                                       size_t                              apdu_len)
{
    uiccd_msg_appl_apdu_send_response_t   *msg;
    msg = uiccd_msg_appl_apdu_send_response_create(client_tag, uicc_status_code, uicc_status_code_fail_details,
                                                   status_word, apdu_data, apdu_len);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

void uiccd_sig_sim_connect_response(uintptr_t client_tag,
                                    sim_uicc_status_code_t uicc_status_code,
                                    sim_uicc_status_code_fail_details_t uicc_status_code_fail_details) {

  uiccd_msg_sim_connect_response_t *p;

  p = uiccd_msg_sim_connect_response_create(client_tag,
                                            uicc_status_code,
                                            uicc_status_code_fail_details);

  uiccd_sig_msg( (ste_msg_t *) p);
}

void uiccd_sig_sim_disconnect_response(uintptr_t client_tag,
                                       sim_uicc_status_code_t uicc_status_code,
                                       sim_uicc_status_code_fail_details_t uicc_status_code_fail_details) {
  uiccd_msg_sim_disconnect_response_t *p;

  p = uiccd_msg_sim_disconnect_response_create(client_tag,
                                               uicc_status_code,
                                               uicc_status_code_fail_details);

  uiccd_sig_msg( (ste_msg_t *) p);
}


// Server event (coming from msl or mal thread)
void uiccd_sig_get_file_information_response(uintptr_t                           client_tag,
                                             sim_uicc_status_code_t              uicc_status_code,
                                             sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                             sim_uicc_status_word_t              status_word,
                                             uint8_t*                            data,
                                             int                                 len)
{
  uiccd_msg_get_file_information_response_t *msg;
  msg = uiccd_msg_get_file_information_response_create(client_tag, uicc_status_code, uicc_status_code_fail_details,
                                                       status_word, data, len);
  uiccd_sig_msg( (ste_msg_t *) msg);
}


// FIXME: Remove parameters data and data_len, note used in this response.
// Server event (coming from msl or mal thread)
void uiccd_sig_update_sim_file_record_response(uintptr_t                           client_tag,
                                               sim_uicc_status_code_t              uicc_status_code,
                                               sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                               sim_uicc_status_word_t              status_word,
                                               const uint8_t*                      buf,
                                               size_t                              len)
{
    uiccd_msg_update_sim_file_record_response_t   *msg;
    msg = uiccd_msg_update_sim_file_record_response_create(client_tag, uicc_status_code, uicc_status_code_fail_details,
                                                           status_word, buf, len);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

// FIXME: Implement for DF and MF selection
// Server event (coming from msl or mal thread)
void uiccd_sig_update_sim_file_binary_response(uintptr_t                           client_tag,
                                               sim_uicc_status_code_t              uicc_status_code,
                                               sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                               sim_uicc_status_word_t              status_word,
                                               const uint8_t*                      buf,
                                               size_t                              len)
{
    uiccd_msg_update_sim_file_binary_response_t   *msg;
    msg = uiccd_msg_update_sim_file_binary_response_create(client_tag, uicc_status_code, uicc_status_code_fail_details,
                                                           status_word, buf, len);
    uiccd_sig_msg( (ste_msg_t *) msg);
}


// Server event (coming from msl or mal thread)
void uiccd_sig_server_status_response(uintptr_t client_tag, uint32_t status, int startup_completed)
{
    uiccd_msg_server_status_t   *msg;
    msg = uiccd_msg_server_status_response_create(client_tag, status, startup_completed);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

// Server event (coming from msl or mal thread)
void uiccd_sig_app_list(uintptr_t client_tag, sim_uicc_status_code_t status, int app_len)
{
  uiccd_msg_app_list_t *msg;
  msg = uiccd_msg_app_list_create(client_tag, status, app_len);
  uiccd_sig_msg( (ste_msg_t *) msg);
}

// Server event (coming from msl or mal thread)
void uiccd_sig_card_fallback(uintptr_t client_tag, ste_sim_app_type_t app_type, int app_id, ste_uicc_card_type_t card_type)
{
    uiccd_msg_card_fallback_t *msg;
    msg = uiccd_msg_card_fallback_create(client_tag, app_type, app_id, card_type);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

// Server event (coming from msl or mal thread)
void uiccd_sig_app_shutdown_init(uintptr_t client_tag, sim_uicc_status_code_t status)
{
  uiccd_msg_app_shutdown_init_t *msg;
  msg = uiccd_msg_app_shutdown_init_create(client_tag, status);
  uiccd_sig_msg( (ste_msg_t *) msg);
}

// The one and only uicc status sig!!
void uiccd_sig_uicc_status_ind(uintptr_t client_tag, ste_uicc_status_t uicc_status) {
  uiccd_msg_uicc_status_ind_t* msg;
  const char* const s[] = { "UNKNOWN", "INIT", "PIN_NEEDED", "PUK_NEEDED",
                            "PIN2_NEEDED", "PUK2_NEEDED",
                            "PIN_VERIFIED", "PIN2_VERIFIED", "READY","STARTUP_COMPLETED",
                            "REJECTED_CARD_INVALID", "REJECTED_CARD_SIM_LOCK",
                            "REJECTED_CARD_CONSECUTIVE_6F00","NO_CARD", "REMOVED_CARD",
                            "REMOVED_CARD_PRESENCE_DETECTION","DISCONNECTED_CARD", "CLOSED",
                            "CARD_RECOVERY" };

  msg = uiccd_msg_uicc_status_ind_create(client_tag, uicc_status);
  catd_log_f(SIM_LOGGING_I, "uicc : modem says: %s", s[(int)msg->uicc_status]);
  uiccd_sig_msg((ste_msg_t *)msg);
}

// As above, but uses a payload
void uiccd_sig_uicc_status_payload_ind(uintptr_t client_tag, ste_uicc_status_t uicc_status, ste_sim_app_type_t app_type,
                                       ste_uicc_card_type_t card_type) {
  uiccd_msg_uicc_status_ind_t* msg;
  const char* const s[] = { "UNKNOWN", "INIT", "PIN_NEEDED", "PUK_NEEDED",
                            "PIN2_NEEDED", "PUK2_NEEDED",
                            "PIN_VERIFIED", "PIN2_VERIFIED", "READY","STARTUP_COMPLETED",
                            "REJECTED_CARD_INVALID", "REJECTED_CARD_SIM_LOCK",
                            "REJECTED_CARD_CONSECUTIVE_6F00","NO_CARD", "REMOVED_CARD",
                            "REMOVED_CARD_PRESENCE_DETECTION","DISCONNECTED_CARD", "CLOSED",
                            "CARD_RECOVERY" };

  msg = uiccd_msg_uicc_status_payload_ind_create(client_tag, uicc_status, app_type, card_type);
  catd_log_f(SIM_LOGGING_I, "uicc : modem says: %s", s[(int)msg->uicc_status]);
  uiccd_sig_msg((ste_msg_t *)msg);
}

// Server event (coming from msl or mal thread) TODO: remove
void uiccd_sig_card_status_ind(uintptr_t client_tag,
                               ste_sim_card_status_t card_status)
{
  uiccd_msg_card_status_ind_t   *msg;
  msg = uiccd_msg_card_status_ind_create(client_tag, card_status);
  uiccd_sig_msg( (ste_msg_t *) msg);
}

// Server event (coming from msl or mal thread)
void uiccd_sig_server_card_status(uintptr_t client_tag, uint32_t status, ste_sim_card_status_t card_status)
{
  uiccd_msg_server_card_status_t   *msg;
  msg = uiccd_msg_server_card_status_create(client_tag, status, card_status);
  uiccd_sig_msg( (ste_msg_t *) msg);
}

// Server event (coming from msl or mal thread)
void uiccd_sig_app_activate(uintptr_t client_tag, sim_uicc_status_code_t status, int app_id, ste_sim_app_type_t app_type)
{
  uiccd_msg_app_activate_t   *msg;
  msg = uiccd_msg_app_activate_create(client_tag, status, app_id, app_type);
  uiccd_sig_msg( (ste_msg_t *) msg);
}

// For catd to tell uiccd about the state of the modem (post TPDL)
// 0 = alive, 1 = dead
void uiccd_sig_modem_state(uintptr_t client_tag, int status)
{
    uiccd_msg_modem_state_t     *msg;
    msg = uiccd_msg_modem_state_create( client_tag, status );
    uiccd_sig_msg( (ste_msg_t *) msg);
}

// Server event (coming from msl or mal thread)
void uiccd_sig_read_smsc_response(uintptr_t client_tag, int status, const char* data, size_t data_len)
{
    uiccd_msg_read_smsc_response_t   *msg;
    msg = uiccd_msg_read_smsc_response_create(client_tag, status, data, data_len );
    uiccd_sig_msg( (ste_msg_t *) msg);
}

// Server event (coming from msl or mal thread)
void uiccd_sig_update_smsc_response(uintptr_t client_tag, int status, const char* data, size_t data_len)
{
    uiccd_msg_update_smsc_response_t   *msg;
    msg = uiccd_msg_update_smsc_response_create(client_tag, status, data, data_len );
    uiccd_sig_msg( (ste_msg_t *) msg);
}

// Server event (coming from msl or mal thread)
void uiccd_sig_sim_file_read_generic_response(uintptr_t                           client_tag,
                                              sim_uicc_status_code_t              uicc_status_code,
                                              sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                              sim_uicc_status_word_t              status_word,
                                              const uint8_t*                      data,
                                              size_t                              data_len)
{
    uiccd_msg_sim_file_read_generic_response_t   *msg;
    msg = uiccd_msg_sim_file_read_generic_response_create(client_tag, uicc_status_code, uicc_status_code_fail_details, status_word,
                                                          data, data_len );
    uiccd_sig_msg( (ste_msg_t *) msg);
}

void uiccd_sig_sim_status(uintptr_t client_tag, ste_reason_t reason)
{
    uiccd_msg_sim_status_t *msg;
    msg = uiccd_msg_sim_status_create(client_tag, reason);
    uiccd_sig_msg((ste_msg_t *)msg);
}

void uiccd_sig_app_status(int fd, uintptr_t client_tag, const char *buf, size_t len)
{
  uiccd_msg_app_status_t *msg;
  msg = uiccd_msg_app_status_create(fd, client_tag, buf, len);
  uiccd_sig_msg( (ste_msg_t *) msg);
}

void uiccd_sig_card_status(int fd, uintptr_t client_tag)
{
  uiccd_msg_card_status_t *msg;
  msg = uiccd_msg_card_status_create(fd, client_tag);
  uiccd_sig_msg( (ste_msg_t *) msg);
}

void uiccd_sig_sim_icon_read(int fd, uintptr_t client_tag, const char *buf, size_t len)
{
    uiccd_msg_sim_icon_read_t *msg;

    msg = uiccd_msg_sim_icon_read_create(fd, client_tag, buf, len);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

void uiccd_sig_sim_icon_read_response(uintptr_t client_tag,
                                      sim_uicc_status_code_t uicc_status_code,
                                      sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                      sim_uicc_status_word_t status_word,
                                      const uint8_t* data,
                                      size_t len)
{
    uiccd_msg_sim_icon_read_response_t *msg;
    msg = uiccd_msg_sim_icon_read_response_create(client_tag,
                                                  uicc_status_code,
                                                  uicc_status_code_fail_details,
                                                  status_word,
                                                  data, len);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

void uiccd_sig_read_subscriber_number(int fd, uintptr_t client_tag, const char *buf, size_t len)
{
    uiccd_msg_subscriber_number_t *msg;

    msg = uiccd_msg_subscriber_number_create(fd, client_tag, buf, len);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

void uiccd_sig_read_subscriber_number_response(uintptr_t client_tag,
                                               sim_uicc_status_code_t uicc_status_code,
                                               sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                               sim_uicc_status_word_t status_word,
                                               const uint8_t* data,
                                               size_t len)
{
    uiccd_msg_subscriber_number_response_t *msg;
    msg = uiccd_msg_subscriber_number_response_create(client_tag,
                                                      uicc_status_code,
                                                      uicc_status_code_fail_details,
                                                      status_word,
                                                      data,
                                                      len);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

void uiccd_sig_sim_channel_send(int fd, uintptr_t client_tag, const char *buf, size_t len)
{
    uiccd_msg_sim_channel_send_t *msg;

    msg = uiccd_msg_sim_channel_send_create(fd, client_tag, buf, len);
    uiccd_sig_msg( (ste_msg_t *) msg);
}
void uiccd_sig_sim_channel_send_response(uintptr_t                           client_tag,
                                         sim_uicc_status_code_t              uicc_status_code,
                                         sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                         sim_uicc_status_word_t              status_word,
                                         const uint8_t                      *data,
                                         size_t                              len)
{
    uiccd_msg_sim_channel_send_response_t   *msg;
    msg = uiccd_msg_sim_channel_send_response_create(client_tag,
                                                     uicc_status_code,
                                                     uicc_status_code_fail_details,
                                                     status_word,
                                                     data,
                                                     len);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

void uiccd_sig_sim_channel_open(int fd, uintptr_t client_tag, const char *buf, size_t len)
{
    uiccd_msg_sim_channel_open_t *msg;

    msg = uiccd_msg_sim_channel_open_create(fd, client_tag, buf, len);
    uiccd_sig_msg( (ste_msg_t *) msg);
}
void uiccd_sig_sim_channel_open_response(uintptr_t                           client_tag,
                                         sim_uicc_status_code_t              uicc_status_code,
                                         sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                         sim_uicc_status_word_t              status_word,
                                         uint16_t                            session_id)
{
    uiccd_msg_sim_channel_open_response_t   *msg;
    msg = uiccd_msg_sim_channel_open_response_create(client_tag,
                                                     uicc_status_code,
                                                     uicc_status_code_fail_details,
                                                     status_word,
                                                     session_id);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

void uiccd_sig_sim_channel_close(int fd, uintptr_t client_tag, const char* buf, size_t len)
{
    uiccd_msg_sim_channel_close_t *msg;

    msg = uiccd_msg_sim_channel_close_create(fd, client_tag, buf, len);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

void uiccd_sig_sim_channel_close_response(uintptr_t                           client_tag,
                                          sim_uicc_status_code_t              uicc_status_code,
                                          sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                          sim_uicc_status_word_t              status_word)
{
    uiccd_msg_sim_channel_close_response_t   *msg;
    msg = uiccd_msg_sim_channel_close_response_create(client_tag,
                                                      uicc_status_code,
                                                      uicc_status_code_fail_details,
                                                      status_word);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

void uiccd_sig_read_fdn(int fd, uintptr_t client_tag)
{
    uiccd_msg_read_fdn_t *msg;

    msg = uiccd_msg_read_fdn_create(fd, client_tag);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

void uiccd_sig_read_ecc(int fd, uintptr_t client_tag)
{
  uiccd_msg_read_ecc_t *msg;
  msg = uiccd_msg_read_ecc_create(fd, client_tag);
  uiccd_sig_msg( (ste_msg_t *) msg);
}

void uiccd_sig_reset(int fd, uintptr_t client_tag)
{
  uiccd_msg_reset_t *msg;
  msg = uiccd_msg_reset_create(fd, client_tag);
  uiccd_sig_msg( (ste_msg_t *) msg);
}

void uiccd_sig_sim_power_on(int fd, uintptr_t client_tag)
{
    uiccd_msg_sim_power_on_t *msg;
    msg = uiccd_msg_sim_power_on_create(fd, client_tag);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

void uiccd_sig_sim_power_off(int fd, uintptr_t client_tag)
{
    uiccd_msg_sim_power_off_t *msg;
    msg = uiccd_msg_sim_power_off_create(fd, client_tag);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

void uiccd_sig_sim_read_preferred_rat_setting(int fd, uintptr_t client_tag)
{
    uiccd_msg_sim_read_preferred_rat_setting_t *msg;
    msg = uiccd_msg_sim_read_preferred_rat_setting_create(fd, client_tag);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

void uiccd_sig_cn_response(cn_message_t *msg_p)
{
    uiccd_msg_cn_t *msg;
    msg = uiccd_msg_cn_response_create(msg_p);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

void uiccd_sig_cn_event(cn_message_t *msg_p)
{
    uiccd_msg_cn_t *msg;
    msg = uiccd_msg_cn_event_create(msg_p);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

// Client command (coming from reader thread receiving client requests)
void uiccd_sig_sap_session_start(int fd, uintptr_t client_tag)
{
    uiccd_msg_sap_session_t   *msg;
    msg = uiccd_msg_sap_session_start_create(fd, client_tag);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

// Client command (coming from reader thread receiving client requests)
void uiccd_sig_sap_session_exit(int fd, uintptr_t client_tag)
{
    uiccd_msg_sap_session_t   *msg;
    msg = uiccd_msg_sap_session_exit_create(fd, client_tag);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

// Client command (coming from reader thread receiving client requests)
void uiccd_sig_sap_session_ctrlcard(int fd, uintptr_t client_tag, uint16_t ctrl_action)
{
     uiccd_msg_sap_session_t   *msg;
     UICCD_MSG_TYPES_t msg_session;
     switch (ctrl_action) {
         case STE_UICC_REQ_SAP_SESSION_PWRON:
             msg_session = UICCD_MSG_SAP_SESSION_POWERON_CARD;
             break;
         case STE_UICC_REQ_SAP_SESSION_PWROFF:
             msg_session = UICCD_MSG_SAP_SESSION_POWEROFF_CARD;
             break;
         case STE_UICC_REQ_SAP_SESSION_RESET:
             msg_session = UICCD_MSG_SAP_SESSION_COLDRESET_CARD;
             //msg_session = UICCD_MSG_SAP_SESSION_WARMRESET_CARD;
             break;
         case STE_UICC_REQ_SAP_SESSION_ATR_GET:
             msg_session = UICCD_MSG_SAP_SESSION_ATR_GET;
             break;
         default:
             return;
     }
     msg = uiccd_msg_sap_session_ctrl_card_create(fd, client_tag, msg_session);
     uiccd_sig_msg( (ste_msg_t *) msg);
}

// Client command (coming from reader thread receiving client requests)
void uiccd_sig_sap_session_apdu_data(int fd, uintptr_t client_tag, const char *buf, size_t len)
{
     uiccd_msg_sap_session_apdu_data_t *msg;

     msg = uiccd_msg_sap_session_apdu_data_create(fd, client_tag, buf, len);
     uiccd_sig_msg( (ste_msg_t *) msg);
}
// Client command (coming from reader thread receiving client requests)
void uiccd_sig_sap_session_status_get(int fd, uintptr_t client_tag)
{
     uiccd_msg_sap_session_t *msg;

     msg = uiccd_msg_sap_session_status_get_create(fd, client_tag);
     uiccd_sig_msg( (ste_msg_t *) msg);
}

// Server event (coming from msl or mal thread)
void uiccd_sig_sap_session_ctrlcard_activate_response(uintptr_t client_tag, int status)
{
    uiccd_msg_sap_session_status_t   *msg;
    msg = uiccd_msg_sap_session_ctrlcard_activate_create_response(client_tag, status);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

// Server event (coming from msl or mal thread)
void uiccd_sig_sap_session_ctrlcard_deactivate_response(uintptr_t client_tag, int status)
{
    uiccd_msg_sap_session_status_t   *msg;
    msg = uiccd_msg_sap_session_ctrlcard_deactivate_create_response(client_tag, status);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

// Server event (coming from msl or mal thread)
void uiccd_sig_sap_session_ctrlcard_warmreset_response(uintptr_t client_tag, int status)
{
    uiccd_msg_sap_session_status_t   *msg;
    msg = uiccd_msg_sap_session_ctrlcard_warmreset_create_response(client_tag, status);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

// Server event (coming from msl or mal thread)
void uiccd_sig_sap_session_ctrlcard_coldreset_response(uintptr_t client_tag, int status)
{
    uiccd_msg_sap_session_status_t   *msg;
    msg = uiccd_msg_sap_session_ctrlcard_coldreset_create_response(client_tag, status);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

// Server event (coming from msl or mal thread)
void uiccd_sig_sap_session_ctrlcard_reset_ind(uintptr_t client_tag, int status)
{
    uiccd_msg_sap_session_status_t   *msg;
    msg = uiccd_msg_sap_session_reset_ind_create_response(client_tag, status);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

// Server event (coming from msl or mal thread)
void uiccd_sig_sap_session_atr_get_response(uintptr_t client_tag,
                                              sim_uicc_status_code_t              uicc_status_code,
                                              sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                              sim_uicc_status_word_t              status_word,
                                              const uint8_t* data, size_t data_len )
{
    uiccd_msg_byte_data_status_t   *msg;
    msg = uiccd_msg_sap_session_atr_get_create_response(client_tag, uicc_status_code, uicc_status_code_fail_details, status_word, data, data_len);
    uiccd_sig_msg( (ste_msg_t *) msg);
}

// Server event (coming from msl or mal thread)
void uiccd_sig_sap_session_apdu_data_response(uintptr_t client_tag,
                                              sim_uicc_status_code_t              uicc_status_code,
                                              sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                              sim_uicc_status_word_t              status_word,
                                              const uint8_t* data, size_t data_len )
{
    uiccd_msg_byte_data_status_t   *msg;
    msg = uiccd_msg_sap_session_apdu_data_create_response(client_tag, uicc_status_code, uicc_status_code_fail_details, status_word, data, data_len);
    uiccd_sig_msg( (ste_msg_t *) msg);
}
