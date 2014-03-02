/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * File name       : uiccd_msg.c
 * Description     : message handling
 *
 * Author          : Mats Bergstrom <mats.m.bergstrom@stericsson.com>
 *
 */


#include "uiccd_msg.h"

#include <stdlib.h>
#include <string.h>


static uiccd_msg_fd_t  *uiccd_msg_fd_create(int type, int fd, uintptr_t client_tag);
static uiccd_msg_byte_data_status_t *uicc_msg_uint_data_create(int                                 type,
                                                               uintptr_t                           client_tag,
                                                               sim_uicc_status_code_t              uicc_status_code,
                                                               sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                               sim_uicc_status_word_t              status_word,
                                                               const uint8_t                      *data,
                                                               size_t                              len);
static uiccd_msg_data_t *uicc_msg_char_data_create(int type,
                                                   int fd,
                                                   uintptr_t client_tag,
                                                   const char *data,
                                                   size_t len);

// -----------------------------------------------------------------------------
// General empty messages

// -----------------------------------------------------------------------------
// file descriptor messages

static uiccd_msg_fd_t  *uiccd_msg_fd_create(int type, int fd, uintptr_t client_tag)
{
    uiccd_msg_fd_t         *p;
    p = malloc(sizeof(*p));
    if (p) {
        p->type = type;
        p->delete_func = 0;
        p->client_tag = client_tag;
        p->fd = fd;
    }
    return p;
}

static void uiccd_msg_cn_delete(ste_msg_t * ste_msg)
{
    uiccd_msg_cn_t *msg = (uiccd_msg_cn_t *) ste_msg;
    if (msg) {
        if (msg->cn_msg) {
            free(msg->cn_msg);
        }
        free(msg);
    }
}

uiccd_msg_cn_t *uiccd_msg_cn_create(int type, cn_message_t * cn_msg)
{
    uiccd_msg_cn_t *p;
    p = malloc(sizeof(*p));
    if (p) {
        p->type = type;
        p->delete_func = uiccd_msg_cn_delete;
        p->client_tag = cn_msg->client_tag;
        p->cn_msg = cn_msg;
    }
    return p;
}

uiccd_msg_connect_t    *uiccd_msg_connect_create(int fd, uintptr_t client_tag)
{
    uiccd_msg_connect_t    *p = uiccd_msg_fd_create(UICCD_MSG_CONNECT, fd, client_tag);
    return p;
}



uiccd_msg_disconnect_t *uiccd_msg_disconnect_create(int fd, uintptr_t client_tag)
{
    uiccd_msg_disconnect_t *p =
        uiccd_msg_fd_create(UICCD_MSG_DISCONNECT, fd, client_tag);
    return p;
}



uiccd_msg_startup_t   *uiccd_msg_startup_create(int fd, uintptr_t client_tag)
{
    uiccd_msg_startup_t   *p =
        uiccd_msg_fd_create(UICCD_MSG_STARTUP, fd, client_tag);
    return p;
}

uiccd_msg_startup_done_t   *uiccd_msg_startup_done_create(int fd, uintptr_t client_tag)
{
    uiccd_msg_startup_done_t   *p =
        uiccd_msg_fd_create(UICCD_MSG_STARTUP_DONE, fd, client_tag);
    return p;
}

uiccd_msg_shutdown_t   *uiccd_msg_shutdown_create(int fd, uintptr_t client_tag)
{
    uiccd_msg_shutdown_t   *p =
        uiccd_msg_fd_create(UICCD_MSG_SHUTDOWN, fd, client_tag);
    return p;
}

uiccd_msg_modem_silent_reset_t   *uiccd_msg_modem_silent_reset_create(int fd, uintptr_t client_tag)
{
    uiccd_msg_modem_silent_reset_t   *p =
        uiccd_msg_fd_create(UICCD_MSG_MODEM_SILENT_RESET, fd, client_tag);
    return p;
}

uiccd_msg_register_t   *uiccd_msg_register_create(int fd, uintptr_t client_tag)
{
    uiccd_msg_register_t   *p =
        uiccd_msg_fd_create(UICCD_MSG_REGISTER, fd, client_tag);
    return p;
}


uiccd_msg_pin_info_t   *uiccd_msg_pin_info_create(int fd, uintptr_t client_tag, const char *data,
        size_t len)
{
    uiccd_msg_pin_info_t   *p =
        uicc_msg_char_data_create(UICCD_MSG_PIN_INFO, fd, client_tag, data, len);
    return p;
}

uiccd_msg_app_info_t   *uiccd_msg_app_info_create(int fd, uintptr_t client_tag)
{
  uiccd_msg_app_info_t *p =
    uiccd_msg_fd_create(UICCD_MSG_APP_INFO, fd, client_tag);
  return p;
}

// Generic status messages, useful in responses
static uiccd_msg_status_t  *uiccd_msg_status_create(int                                 type,
                                                    sim_uicc_status_code_t              uicc_status_code,
                                                    sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                    sim_uicc_status_word_t              status_word,
                                                    uintptr_t                           client_tag)
{
    uiccd_msg_status_t         *p;
    p = malloc(sizeof(*p));
    if (p) {
        p->type = type;
        p->delete_func = 0;
        p->client_tag = client_tag;
        p->uicc_status_code = uicc_status_code;
        p->uicc_status_code_fail_details = uicc_status_code_fail_details;
        p->status_word = status_word;
    }
    return p;
}

static uiccd_msg_server_status_t *uiccd_msg_server_status_create(int type, uint32_t status, int server_status, uintptr_t client_tag)
{
    uiccd_msg_server_status_t         *p;
    p = malloc(sizeof(*p));
    if (p) {
        p->type = type;
        p->delete_func = 0;
        p->client_tag = client_tag;
        p->status = status;
        p->server_status = server_status;
    }
    return p;
}


uiccd_msg_pin_change_response_t* uiccd_msg_pin_change_response_create(uintptr_t                           client_tag,
                                                                      sim_uicc_status_code_t              uicc_status_code,
                                                                      sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                                      sim_uicc_status_word_t              status_word)
{
    uiccd_msg_pin_change_response_t* p;
    p = uiccd_msg_status_create(UICCD_MSG_PIN_CHANGE_RSP,
                                uicc_status_code,
                                uicc_status_code_fail_details,
                                status_word,
                                client_tag);
    return p;
}

uiccd_msg_pin_verify_response_t* uiccd_msg_pin_verify_response_create(uintptr_t                           client_tag,
                                                                      sim_uicc_status_code_t              uicc_status_code,
                                                                      sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                                      sim_uicc_status_word_t              status_word)
{
    uiccd_msg_pin_verify_response_t* p;
    p = uiccd_msg_status_create(UICCD_MSG_PIN_VERIFY_RSP,
                                    uicc_status_code,
                                    uicc_status_code_fail_details,
                                    status_word,
                                    client_tag);
    return p;
}

uiccd_msg_server_status_t* uiccd_msg_server_status_response_create(uintptr_t client_tag, uint32_t status, int startup_completed	)
{
    uiccd_msg_server_status_t* p;
    p = uiccd_msg_server_status_create( UICCD_MSG_SERVER_STATUS_RSP, status, startup_completed, client_tag);
    return p;
}

uiccd_msg_pin_disable_response_t* uiccd_msg_pin_disable_response_create(uintptr_t                           client_tag,
                                                                        sim_uicc_status_code_t              uicc_status_code,
                                                                        sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                                        sim_uicc_status_word_t              status_word)
{
    uiccd_msg_pin_disable_response_t* p;
    p = uiccd_msg_status_create(UICCD_MSG_PIN_DISABLE_RSP,
                                uicc_status_code,
                                uicc_status_code_fail_details,
                                status_word,
                                client_tag);
    return p;
}

uiccd_msg_pin_enable_response_t* uiccd_msg_pin_enable_response_create(uintptr_t                           client_tag,
                                                                      sim_uicc_status_code_t              uicc_status_code,
                                                                      sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                                      sim_uicc_status_word_t              status_word)
{
    uiccd_msg_pin_enable_response_t* p;
    p = uiccd_msg_status_create(UICCD_MSG_PIN_ENABLE_RSP,
                                uicc_status_code,
                                uicc_status_code_fail_details,
                                status_word,
                                client_tag);
    return p;
}

uiccd_msg_pin_unblock_response_t* uiccd_msg_pin_unblock_response_create(uintptr_t                           client_tag,
                                                                        sim_uicc_status_code_t              uicc_status_code,
                                                                        sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                                        sim_uicc_status_word_t              status_word)
{
    uiccd_msg_pin_unblock_response_t* p;
    p = uiccd_msg_status_create(UICCD_MSG_PIN_UNBLOCK_RSP,
                                uicc_status_code,
                                uicc_status_code_fail_details,
                                status_word,
                                client_tag);
    return p;
}


uiccd_msg_server_card_status_t* uiccd_msg_server_card_status_create(uintptr_t client_tag, uint32_t status, ste_sim_card_status_t card_status)
{
    uiccd_msg_server_card_status_t* p;
    p = malloc(sizeof *p);
    if (p) {
      p->type=UICCD_MSG_SERVER_CARD_STATUS_RSP;
      p->delete_func=0;
      p->client_tag = client_tag;
      p->status=status;
      p->card_status=card_status;
    }
    return p;
}

uiccd_msg_uicc_status_ind_t* uiccd_msg_uicc_status_ind_create(uintptr_t client_tag, ste_uicc_status_t uicc_status) {
  uiccd_msg_uicc_status_ind_t *p;
  p = malloc(sizeof(*p));
  if (p) {
    p->type=UICCD_MSG_UICC_STATUS_IND;
    p->delete_func=0;
    p->client_tag = client_tag;
    p->uicc_status=uicc_status;
    p->app_type=STE_SIM_APP_TYPE_UNKNOWN;
    p->card_type=STE_SIM_APP_TYPE_UNKNOWN;
  }
  return p;
}

uiccd_msg_uicc_status_ind_t* uiccd_msg_uicc_status_payload_ind_create(uintptr_t client_tag, ste_uicc_status_t uicc_status, ste_sim_app_type_t app_type, ste_uicc_card_type_t card_type) {
  uiccd_msg_uicc_status_ind_t *p;
  p = malloc(sizeof(*p));
  if (p) {
    p->type=UICCD_MSG_UICC_STATUS_IND;
    p->delete_func=0;
    p->client_tag = client_tag;
    p->uicc_status=uicc_status;
    p->app_type=app_type;
    p->card_type=card_type;
  }
  return p;
}

uiccd_msg_card_status_ind_t* uiccd_msg_card_status_ind_create(uintptr_t client_tag, ste_sim_card_status_t card_status)
{
    uiccd_msg_card_status_ind_t* p;
    p = malloc(sizeof *p);
    if (p) {
      p->type=UICCD_MSG_CARD_STATUS_IND;
      p->delete_func=0;
      p->client_tag = client_tag;
      p->card_status=card_status;
    }
    return p;
}

// App list message
uiccd_msg_app_list_t*
uiccd_msg_app_list_create(uintptr_t client_tag, sim_uicc_status_code_t status, int app_len)
{
  uiccd_msg_app_list_t *p;

  p = malloc(sizeof(*p));
  if (p) {
    p->type = UICCD_MSG_APP_LIST_RSP;
    p->delete_func = 0;
    p->client_tag = client_tag;
    p->status=status;
    p->app_len=app_len;
  }
  return p;
}

uiccd_msg_card_fallback_t* uiccd_msg_card_fallback_create(uintptr_t client_tag, ste_sim_app_type_t app_type, int app_id, ste_uicc_card_type_t card_type)
{
    uiccd_msg_card_fallback_t* p;
    p = malloc(sizeof *p);
    if (p) {
      p->type=UICCD_MSG_CARD_FALLBACK;
      p->delete_func=0;
      p->client_tag = client_tag;
      p->card_type=card_type;
      p->app_id=app_id;
      p->app_type=app_type;
    }
    return p;
}

uiccd_msg_app_activate_t* uiccd_msg_app_activate_create(uintptr_t client_tag, sim_uicc_status_code_t status, int app_id, ste_sim_app_type_t app_type)
{
    uiccd_msg_app_activate_t* p;
    p = malloc(sizeof *p);
    if (p) {
      p->type=UICCD_MSG_APP_ACTIVATE_RSP;
      p->delete_func=0;
      p->client_tag = client_tag;
      p->status=status;
      p->app_id=app_id;
      p->app_type=app_type;
    }
    return p;
}

uiccd_msg_app_shutdown_init_t* uiccd_msg_app_shutdown_init_create(uintptr_t client_tag, sim_uicc_status_code_t status)
{
    uiccd_msg_app_shutdown_init_t* p;
    p = malloc(sizeof *p);
    if (p) {
      p->type=UICCD_MSG_APP_SHUTDOWN_INITIATE_RSP;
      p->delete_func=0;
      p->client_tag = client_tag;
      p->status=status;
    }
    return p;
}

// -----------------------------------------------------------------------------
// STOP Message

uiccd_msg_stop_t       *uiccd_msg_stop_create(cat_barrier_t * bar, uintptr_t client_tag)
{
    uiccd_msg_stop_t       *p;
    p = malloc(sizeof(*p));
    if (p) {
        p->type = UICCD_MSG_STOP;
        p->delete_func = 0;
        p->client_tag = client_tag;
        p->bar = bar;
    }
    return p;

}



// -----------------------------------------------------------------------------
// Generic data Message


static void uicc_msg_data_delete(ste_msg_t * ste_msg)
{
    uiccd_msg_data_t       *msg = (uiccd_msg_data_t *) ste_msg;
    if (msg) {
        if (msg->data)
            free(msg->data);
        msg->data = 0;
        free(msg);
    }
}

static void uicc_msg_byte_data_status_delete(ste_msg_t * ste_msg)
{
    uiccd_msg_byte_data_status_t       *msg = (uiccd_msg_byte_data_status_t *) ste_msg;
    if (msg) {
        if (msg->data)
            free(msg->data);
        msg->data = 0;
        free(msg);
    }
}
static void uiccd_msg_char_data_status_delete(ste_msg_t * ste_msg)
{
    uiccd_msg_char_data_status_t       *msg = (uiccd_msg_char_data_status_t *) ste_msg;
    if (msg) {
        if (msg->data)
            free(msg->data);
        msg->data = 0;
        free(msg);
    }
}
static uiccd_msg_byte_data_status_t *uicc_msg_uint_data_create(int                                 type,
                                                               uintptr_t                           client_tag,
                                                               sim_uicc_status_code_t              uicc_status_code,
                                                               sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                               sim_uicc_status_word_t              status_word,
                                                               const uint8_t                      *data,
                                                               size_t                              len)
{
  uiccd_msg_byte_data_status_t *p;
  uint8_t *d = NULL;

  p = malloc(sizeof(*p));
  if (!p) return 0;
  if (len > 0) {
    d = malloc(len);
    if (!d) {
      free(p);
      return 0;
    }
    memcpy(d, data, len);
  }
  p->type = type;
  p->delete_func = uicc_msg_byte_data_status_delete;
  p->client_tag = client_tag;
  p->uicc_status_code = uicc_status_code;
  p->uicc_status_code_fail_details = uicc_status_code_fail_details;
  p->status_word = status_word;
  p->data = d;
  p->len = len;
  return p;
}


uiccd_msg_byte_data_status_t *uicc_msg_byte_data_response_create(int                                 type,
                                                                 sim_uicc_status_code_t              uicc_status_code,
                                                                 sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                                 sim_uicc_status_word_t              status_word,
                                                                 uintptr_t                           client_tag,
                                                                 const uint8_t                      *data,
                                                                 size_t                              len)
{
  uiccd_msg_byte_data_status_t *p;
  uint8_t *d = NULL;
  p = malloc(sizeof(*p));
  if (!p) return 0;
  if (len > 0) {
    d = malloc(len);
    if (!d) {
      free(p);
      return 0;
    }
    memcpy(d, data, len);
  }
  p->type = type;
  p->delete_func = uicc_msg_byte_data_status_delete;
  p->client_tag = client_tag;
  p->uicc_status_code = uicc_status_code;
  p->uicc_status_code_fail_details = uicc_status_code_fail_details;
  p->data = d;
  p->len = len;
  p->status_word = status_word;
  return p;
}

// FIXME: Make sure it is capable of creating a message with null data.
// FIXME: Should only be used to carry strings.
static uiccd_msg_data_t *uicc_msg_char_data_create(int type,
                                                   int fd,
                                                   uintptr_t client_tag,
                                                   const char *data,
                                                   size_t len)
{
    uiccd_msg_data_t       *p;
    p = malloc(sizeof(*p));
    if (p) {
        char                   *d = NULL;

        if (len > 0) {
            d = malloc(len);
            if (!d) {
                free(p);
                return 0;
            }
            memcpy(d, data, len);
        }

        p->type = type;
        p->delete_func = uicc_msg_data_delete;
        p->client_tag = client_tag;
        p->fd = fd;
        p->data = d;
        p->len = len;
    }
    return p;

}

uiccd_msg_char_data_status_t *uicc_msg_char_data_response_create(int                                 type,
                                                                 sim_uicc_status_code_t              uicc_status_code,
                                                                 sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                                 sim_uicc_status_word_t              status_word,
                                                                 uintptr_t                           client_tag,
                                                                 const uint8_t                       *data,
                                                                 size_t                              len)
{
    uiccd_msg_char_data_status_t *p;
    p = malloc(sizeof(*p));
    if (p) {
        uint8_t                   *d = NULL;

        if (len > 0) {
            d = malloc(len);
            if (!d) {
                free(p);
                return 0;
            }
            memcpy(d, data, len);
        }

        p->type = type;
        p->delete_func = uiccd_msg_char_data_status_delete;
        p->client_tag = client_tag;
        p->uicc_status_code = uicc_status_code;
        p->uicc_status_code_fail_details = uicc_status_code_fail_details;
        p->data = d;
        p->len = len;
        p->status_word = status_word;
    }
    return p;

}


uiccd_msg_pin_verify_t *uiccd_msg_pin_verify_create(int fd,
                                                    uintptr_t client_tag,
                                                    const char *data,
                                                    size_t len)
{
    uiccd_msg_data_t       *msg;
    msg = uicc_msg_char_data_create(UICCD_MSG_PIN_VERIFY, fd, client_tag, data, len);
    return (uiccd_msg_pin_verify_t *) msg;
}

uiccd_msg_pin_change_t *uiccd_msg_pin_change_create(int fd,
                                                    uintptr_t client_tag,
                                                    const char *data,
                                                    size_t len)
{
    uiccd_msg_data_t       *msg;
    msg = uicc_msg_char_data_create(UICCD_MSG_PIN_CHANGE, fd, client_tag, data, len);
    return (uiccd_msg_pin_change_t *) msg;
}

uiccd_msg_pin_disable_t *uiccd_msg_pin_disable_create(int fd,
                                                    uintptr_t client_tag,
                                                    const char *data,
                                                    size_t len)
{
    uiccd_msg_data_t       *msg;
    msg = uicc_msg_char_data_create(UICCD_MSG_PIN_DISABLE, fd, client_tag, data, len);
    return (uiccd_msg_pin_disable_t *) msg;
}

uiccd_msg_pin_enable_t *uiccd_msg_pin_enable_create(int fd,
                                                    uintptr_t client_tag,
                                                    const char *data,
                                                    size_t len)
{
    uiccd_msg_data_t       *msg;
    msg = uicc_msg_char_data_create(UICCD_MSG_PIN_ENABLE, fd, client_tag, data, len);
    return (uiccd_msg_pin_enable_t *) msg;
}


uiccd_msg_pin_unblock_t *uiccd_msg_pin_unblock_create(int fd,
                                                    uintptr_t client_tag,
                                                    const char *data,
                                                    size_t len)
{
    uiccd_msg_data_t       *msg;
    msg = uicc_msg_char_data_create(UICCD_MSG_PIN_UNBLOCK, fd, client_tag, data, len);
    return (uiccd_msg_pin_unblock_t *) msg;
}


uiccd_msg_read_sim_file_record_t *uiccd_msg_read_sim_file_record_create(int fd,
                                                            uintptr_t client_tag,
                                                            const char *data,
                                                            size_t len)
{
    uiccd_msg_data_t       *msg;
    msg = uicc_msg_char_data_create(UICCD_MSG_READ_SIM_FILE_RECORD, fd, client_tag, data, len);
    return (uiccd_msg_read_sim_file_record_t *) msg;
}

uiccd_msg_read_sim_file_binary_t *uiccd_msg_read_sim_file_binary_create(int fd,
                                                            uintptr_t client_tag,
                                                            const char *data,
                                                            size_t len)
{
    uiccd_msg_data_t       *msg;
    msg = uicc_msg_char_data_create(UICCD_MSG_READ_SIM_FILE_BINARY, fd, client_tag, data, len);
    return (uiccd_msg_read_sim_file_binary_t *) msg;
}

uiccd_msg_update_sim_file_record_t *uiccd_msg_update_sim_file_record_create(int fd,
                                                                  uintptr_t client_tag,
                                                                  const char *data,
                                                                  size_t len)
{
    uiccd_msg_data_t       *msg;
    msg = uicc_msg_char_data_create(UICCD_MSG_UPDATE_SIM_FILE_RECORD, fd, client_tag, data, len);
    return (uiccd_msg_update_sim_file_record_t *) msg;
}

uiccd_msg_update_sim_file_binary_t *uiccd_msg_update_sim_file_binary_create(int fd,
                                                                  uintptr_t client_tag,
                                                                  const char *data,
                                                                  size_t len)
{
    uiccd_msg_data_t       *msg;
    msg = uicc_msg_char_data_create(UICCD_MSG_UPDATE_SIM_FILE_BINARY, fd, client_tag, data, len);
    return (uiccd_msg_update_sim_file_binary_t *) msg;
}

uiccd_msg_sim_file_get_format_t *uiccd_msg_sim_file_get_format_create(int fd,
                                                                      uintptr_t client_tag,
                                                                      const char *data,
                                                                      size_t len)
{
    uiccd_msg_data_t       *msg;
    msg = uicc_msg_char_data_create(UICCD_MSG_SIM_FILE_GET_FORMAT, fd, client_tag, data, len);
    return (uiccd_msg_sim_file_get_format_t *) msg;
}

uiccd_msg_get_file_information_t *uiccd_msg_get_file_information_create(int fd,
                                                                        uintptr_t client_tag,
                                                                        const char *data,
                                                                        size_t len)
{
    uiccd_msg_data_t       *msg;
    msg = uicc_msg_char_data_create(UICCD_MSG_GET_FILE_INFORMATION, fd, client_tag, data, len);
    return (uiccd_msg_get_file_information_t *) msg;
}

uiccd_msg_get_sim_state_t *uiccd_msg_get_sim_state_create(int fd,
                                                          uintptr_t client_tag,
                                                          const char *data,
                                                          size_t len)
{
    uiccd_msg_data_t       *msg;
    msg = uicc_msg_char_data_create(UICCD_MSG_GET_SIM_STATE, fd, client_tag, data, len);
    return (uiccd_msg_get_sim_state_t *) msg;
}

uiccd_msg_read_sim_file_record_response_t* uiccd_msg_read_sim_file_record_response_create(
                                                                          uintptr_t                           client_tag,
                                                                          sim_uicc_status_code_t              uicc_status_code,
                                                                          sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                                          sim_uicc_status_word_t              status_word,
                                                                          const uint8_t*                      data,
                                                                          size_t                              len)
{
    uiccd_msg_byte_data_status_t       *msg;
    msg = uicc_msg_byte_data_response_create(UICCD_MSG_READ_SIM_FILE_RECORD_RSP, uicc_status_code, uicc_status_code_fail_details,
                                             status_word, client_tag, data, len);

    return (uiccd_msg_read_sim_file_record_response_t *) msg;
}


uiccd_msg_read_sim_file_binary_response_t* uiccd_msg_read_sim_file_binary_response_create(
                                                                          uintptr_t                           client_tag,
                                                                          sim_uicc_status_code_t              uicc_status_code,
                                                                          sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                                          sim_uicc_status_word_t              status_word,
                                                                          const uint8_t*                      data,
                                                                          size_t                              len)
{
    uiccd_msg_byte_data_status_t       *msg;
    msg = uicc_msg_byte_data_response_create(UICCD_MSG_READ_SIM_FILE_BINARY_RSP, uicc_status_code, uicc_status_code_fail_details,
                                             status_word, client_tag, data, len);

    return (uiccd_msg_read_sim_file_binary_response_t *) msg;
}

// FIXME: Remove parameters data and data_len, note used in this response.
uiccd_msg_update_sim_file_record_response_t* uiccd_msg_update_sim_file_record_response_create(
                                                                          uintptr_t                           client_tag,
                                                                          sim_uicc_status_code_t              uicc_status_code,
                                                                          sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                                          sim_uicc_status_word_t              status_word,
                                                                          const uint8_t*                      data,
                                                                          size_t                              len)
{
    uiccd_msg_char_data_status_t       *msg;
    msg = uicc_msg_char_data_response_create
      (UICCD_MSG_UPDATE_SIM_FILE_RECORD_RSP, uicc_status_code, uicc_status_code_fail_details, status_word, client_tag, data, len);
    return (uiccd_msg_update_sim_file_record_response_t *) msg;
}

// FIXME: Remove parameters data and data_len, note used in this response.
uiccd_msg_update_sim_file_binary_response_t* uiccd_msg_update_sim_file_binary_response_create(
                                                                          uintptr_t                           client_tag,
                                                                          sim_uicc_status_code_t              uicc_status_code,
                                                                          sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                                          sim_uicc_status_word_t              status_word,
                                                                          const uint8_t*                      data,
                                                                          size_t                              len)
{
    uiccd_msg_char_data_status_t       *msg;
    msg = uicc_msg_char_data_response_create(UICCD_MSG_UPDATE_SIM_FILE_BINARY_RSP, uicc_status_code, uicc_status_code_fail_details,
                                             status_word, client_tag, data, len);
    return (uiccd_msg_update_sim_file_binary_response_t *) msg;
}


// Right now supports only EF
uiccd_msg_sim_file_get_format_response_t*
uiccd_msg_sim_file_get_format_response_create(uintptr_t                           client_tag,
                                              sim_uicc_status_code_t              uicc_status_code,
                                              sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                              sim_uicc_status_word_t              status_word,
                                              sim_file_structure_t                file_type,
                                              int                                 file_size,
                                              int                                 record_len,
                                              int                                 num_records)
{
  uiccd_msg_sim_file_get_format_response_t *p;
  p = malloc(sizeof(*p));
  if (p) {
    p->type=UICCD_MSG_SIM_FILE_GET_FORMAT_RSP;
    p->delete_func=0;
    p->client_tag=client_tag;
    p->file_type=file_type;
    p->file_size=file_size;
    p->record_len=record_len;
    p->num_records=num_records;
    p->uicc_status_code = uicc_status_code;
    p->uicc_status_code_fail_details = uicc_status_code_fail_details;
    p->status_word = status_word;
  }
  return p;
}

uiccd_msg_get_file_information_response_t*
uiccd_msg_get_file_information_response_create(uintptr_t                           client_tag,
                                               sim_uicc_status_code_t              uicc_status_code,
                                               sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                               sim_uicc_status_word_t              status_word,
                                               uint8_t*                            data,
                                               size_t                              len)
{
    uiccd_msg_byte_data_status_t *msg;
    msg = uicc_msg_byte_data_response_create(UICCD_MSG_GET_FILE_INFORMATION_RSP, uicc_status_code, uicc_status_code_fail_details,
                                             status_word, client_tag, data, len);
    return (uiccd_msg_get_file_information_response_t *) msg;
}


uiccd_msg_appl_apdu_send_response_t* uiccd_msg_appl_apdu_send_response_create(uintptr_t                           client_tag,
                                                                              sim_uicc_status_code_t              uicc_status_code,
                                                                              sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                                              sim_uicc_status_word_t              status_word,
                                                                              const uint8_t*                      data,
                                                                              size_t                              len)
{
    uiccd_msg_byte_data_status_t       *msg;
    msg = uicc_msg_byte_data_response_create(UICCD_MSG_APPL_APDU_SEND_RSP, uicc_status_code, uicc_status_code_fail_details,
                                             status_word, client_tag, data, len);

    return (uiccd_msg_appl_apdu_send_response_t *) msg;
}


uiccd_msg_pin_info_response_t* uiccd_msg_pin_info_response_create(uintptr_t                           client_tag,
                                                                  sim_uicc_status_code_t              uicc_status_code,
                                                                  sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                                  ste_sim_pin_status_t                pin_status,
                                                                  int                                 attempts,
                                                                  int                                 attempts2)
{
    uiccd_msg_pin_info_response_t* p;
    p = malloc(sizeof *p);
    if (p) {
      p->type=UICCD_MSG_PIN_INFO_RSP;
      p->delete_func=0;
      p->client_tag = client_tag;
      p->uicc_status_code=uicc_status_code;
      p->uicc_status_code_fail_details=uicc_status_code_fail_details;
      p->pin_status=pin_status;
      p->attempts=attempts;
      p->attempts2=attempts2;
    }
    return p;
}


uiccd_msg_modem_state_t* uiccd_msg_modem_state_create(uintptr_t client_tag,
                                                      int status)
{
    uiccd_msg_modem_state_t     *p = malloc( sizeof *p );
    if ( p ) {
        p->type=UICCD_MSG_MODEM_STATE;
        p->delete_func=0;
        p->client_tag = client_tag;
        p->status=status;
    }
    return p;
}
uiccd_msg_read_smsc_t *uiccd_msg_read_smsc_create(int fd,
                                                  uintptr_t client_tag,
                                                  const char *data,
                                                  size_t len)
{
    uiccd_msg_data_t       *msg;
    msg = uicc_msg_char_data_create(UICCD_MSG_READ_SMSC, fd, client_tag, data, len);
    return (uiccd_msg_read_smsc_t *) msg;
}

uiccd_msg_update_smsc_t *uiccd_msg_update_smsc_create(int fd,
                                                      uintptr_t client_tag,
                                                      const char *data,
                                                      size_t len)
{
    uiccd_msg_data_t       *msg;
    msg = uicc_msg_char_data_create(UICCD_MSG_UPDATE_SMSC, fd, client_tag, data, len);
    return (uiccd_msg_update_smsc_t *) msg;
}

uiccd_msg_smsc_get_record_max_t *uiccd_msg_smsc_get_record_max_create(int fd,
                                                                      uintptr_t client_tag,
                                                                      const char *data,
                                                                      size_t len)
{
    uiccd_msg_data_t       *msg;
    msg = uicc_msg_char_data_create(UICCD_MSG_SMSC_GET_RECORD_MAX, fd, client_tag, data, len);
    return (uiccd_msg_smsc_get_record_max_t *) msg;
}

uiccd_msg_smsc_save_to_record_t *uiccd_msg_smsc_save_to_record_create(int fd,
                                                                      uintptr_t client_tag,
                                                                      const char *data,
                                                                      size_t len)
{
    uiccd_msg_data_t       *msg;
    msg = uicc_msg_char_data_create(UICCD_MSG_SMSC_SAVE_TO_RECORD, fd, client_tag, data, len);
    return (uiccd_msg_smsc_get_record_max_t *) msg;
}

uiccd_msg_smsc_restore_from_record_t *uiccd_msg_smsc_restore_from_record_create(int fd,
                                                                                uintptr_t client_tag,
                                                                                const char *data,
                                                                                size_t len)
{
    uiccd_msg_data_t       *msg;
    msg = uicc_msg_char_data_create(UICCD_MSG_SMSC_RESTORE_FROM_RECORD, fd, client_tag, data, len);
    return (uiccd_msg_smsc_get_record_max_t *) msg;
}

uiccd_msg_read_smsc_response_t* uiccd_msg_read_smsc_response_create(uintptr_t client_tag,
                                                                    int status,
                                                                    const char* data,
                                                                    size_t len)
{
    uiccd_msg_data_t       *msg;
    msg = uicc_msg_char_data_create
      (UICCD_MSG_READ_SMSC_RSP, status, client_tag, data, len);
    return (uiccd_msg_read_smsc_response_t *) msg;
}

uiccd_msg_update_smsc_response_t* uiccd_msg_update_smsc_response_create(uintptr_t client_tag,
                                                                        int status,
                                                                        const char* data,
                                                                        size_t len)
{
    uiccd_msg_data_t       *msg;
    msg = uicc_msg_char_data_create(UICCD_MSG_UPDATE_SMSC_RSP, status, client_tag, data, len);
    return (uiccd_msg_update_smsc_response_t *) msg;
}

uiccd_msg_sim_file_read_generic_t *uiccd_msg_sim_file_read_generic_create(int fd,
                                                                          uintptr_t client_tag,
                                                                          const char *data,
                                                                          size_t len)
{
    uiccd_msg_data_t       *msg;
    msg = uicc_msg_char_data_create(UICCD_MSG_SIM_FILE_READ_GENERIC, fd, client_tag, data, len);
    return (uiccd_msg_sim_file_read_generic_t *) msg;
}

uiccd_msg_sim_file_read_generic_response_t* uiccd_msg_sim_file_read_generic_response_create(
                                                                          uintptr_t                           client_tag,
                                                                          sim_uicc_status_code_t              uicc_status_code,
                                                                          sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                                          sim_uicc_status_word_t              status_word,
                                                                          const uint8_t*                      data,
                                                                          size_t                              len)
{
    uiccd_msg_byte_data_status_t *msg;
    msg = uicc_msg_uint_data_create(UICCD_MSG_SIM_FILE_READ_GENERIC_RSP, client_tag,
                                    uicc_status_code, uicc_status_code_fail_details, status_word, data, len);

    return (uiccd_msg_sim_file_read_generic_response_t *) msg;
}

uiccd_msg_sim_connect_response_t*
uiccd_msg_sim_connect_response_create(uintptr_t                           client_tag,
                                      sim_uicc_status_code_t              uicc_status_code,
                                      sim_uicc_status_code_fail_details_t uicc_status_code_fail_details)
{
    uiccd_msg_status_t *msg;
    sim_uicc_status_word_t              status_word = {0, 0};

    msg = uiccd_msg_status_create(UICCD_MSG_SIM_CONNECT_RSP,
                                  uicc_status_code,
                                  uicc_status_code_fail_details,
                                  status_word, // Not applicable
                                  client_tag);

    return (uiccd_msg_sim_connect_response_t *) msg;
}

uiccd_msg_update_service_table_t *uiccd_msg_update_service_table_create(int fd,
                                                                        uintptr_t client_tag,
                                                                        const char *data,
                                                                        size_t len)
{
    uiccd_msg_data_t       *msg;
    msg = uicc_msg_char_data_create(UICCD_MSG_UPDATE_SERVICE_TABLE, fd, client_tag, data, len);
    return (uiccd_msg_update_service_table_t *) msg;
}

uiccd_msg_get_service_table_t *uiccd_msg_get_service_table_create(int fd,
                                                                  uintptr_t client_tag,
                                                                  const char *data,
                                                                  size_t len)
{
    uiccd_msg_data_t       *msg;
    msg = uicc_msg_char_data_create(UICCD_MSG_GET_SERVICE_TABLE, fd, client_tag, data, len);
    return (uiccd_msg_get_service_table_t *) msg;
}

uiccd_msg_get_service_availability_t *uiccd_msg_get_service_availability_create(int fd,
                                                                                uintptr_t client_tag,
                                                                                const char *data,
                                                                                size_t len)
{
    uiccd_msg_data_t       *msg;
    msg = uicc_msg_char_data_create(UICCD_MSG_GET_SERVICE_AVAILABILITY, fd, client_tag, data, len);
    return (uiccd_msg_get_service_availability_t *) msg;
}

uiccd_msg_app_status_t   *uiccd_msg_app_status_create(int fd, uintptr_t client_tag, const char *buf, size_t len)
{
  uiccd_msg_data_t       *msg;
  msg = uicc_msg_char_data_create(UICCD_MSG_APP_STATUS, fd, client_tag, buf, len);
  return (uiccd_msg_app_status_t *) msg;
}

uiccd_msg_card_status_t *uiccd_msg_card_status_create(int fd, uintptr_t client_tag)
{
  uiccd_msg_card_status_t *p =
    uiccd_msg_fd_create(UICCD_MSG_CARD_STATUS, fd, client_tag);
  return p;
}

uiccd_msg_sim_status_t* uiccd_msg_sim_status_create(uintptr_t client_tag,
                                                    ste_reason_t reason)
{
  uiccd_msg_sim_status_t *p = malloc(sizeof(*p));
    if ( p ) {
        p->type=UICCD_MSG_SIM_STATUS;
        p->delete_func=0;
        p->client_tag = client_tag;
        p->reason=reason;
    }
    return p;
}

uiccd_msg_read_fdn_t *uiccd_msg_read_fdn_create(int fd, uintptr_t client_tag)
{
  uiccd_msg_read_fdn_t *p =
    uiccd_msg_fd_create(UICCD_MSG_READ_FDN, fd, client_tag);
  return p;
}

uiccd_msg_read_ecc_t *uiccd_msg_read_ecc_create(int fd, uintptr_t client_tag)
{
  uiccd_msg_read_ecc_t *p =
    uiccd_msg_fd_create(UICCD_MSG_READ_ECC, fd, client_tag);
  return p;
}

uiccd_msg_sim_disconnect_response_t*
uiccd_msg_sim_disconnect_response_create(uintptr_t                           client_tag,
                                         sim_uicc_status_code_t              uicc_status_code,
                                         sim_uicc_status_code_fail_details_t uicc_status_code_fail_details)
{
    uiccd_msg_status_t *msg;
    sim_uicc_status_word_t              status_word = {0, 0};

    msg = uiccd_msg_status_create(UICCD_MSG_SIM_DISCONNECT_RSP,
                                  uicc_status_code,
                                  uicc_status_code_fail_details,
                                  status_word, // Not applicable
                                  client_tag);

    return (uiccd_msg_sim_disconnect_response_t *) msg;
}

uiccd_msg_sim_icon_read_t *uiccd_msg_sim_icon_read_create(int fd,
                                                          uintptr_t client_tag,
                                                          const char *data,
                                                          size_t len)
{
    uiccd_msg_data_t *msg;
    msg = uicc_msg_char_data_create(UICCD_MSG_SIM_ICON_READ, fd, client_tag,
                                    data, len);
    return (uiccd_msg_sim_icon_read_t *) msg;
}

uiccd_msg_sim_icon_read_response_t* uiccd_msg_sim_icon_read_response_create(
                                                                          uintptr_t                           client_tag,
                                                                          sim_uicc_status_code_t              uicc_status_code,
                                                                          sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                                          sim_uicc_status_word_t              status_word,
                                                                          const uint8_t*                      data,
                                                                          size_t                              len)
{
    uiccd_msg_sim_icon_read_response_t       *msg;
    msg = uicc_msg_byte_data_response_create(UICCD_MSG_SIM_ICON_READ_RSP, uicc_status_code, uicc_status_code_fail_details,
                                             status_word, client_tag, data, len);

    return (uiccd_msg_sim_icon_read_response_t *) msg;
}

uiccd_msg_read_plmn_t *uiccd_msg_read_plmn_create(int fd,
                                                  uintptr_t client_tag,
                                                  const char *data,
                                                  size_t len)
{
    uiccd_msg_data_t       *msg;
    msg = uicc_msg_char_data_create(UICCD_MSG_READ_PLMN, fd, client_tag, data, len);
    return (uiccd_msg_read_plmn_t *) msg;
}

uiccd_msg_update_plmn_t *uiccd_msg_update_plmn_create(int fd,
                                                    uintptr_t client_tag,
                                                    const char *data,
                                                    size_t len)
{
    uiccd_msg_data_t *msg;
    msg = uicc_msg_char_data_create(UICCD_MSG_UPDATE_PLMN, fd, client_tag, data, len);
    return (uiccd_msg_update_plmn_t *) msg;
}

uiccd_msg_subscriber_number_t *uiccd_msg_subscriber_number_create(int fd,
                                                                  uintptr_t client_tag,
                                                                  const char *data,
                                                                  size_t len)
{
    uiccd_msg_data_t *msg;

    msg = uicc_msg_char_data_create(UICCD_MSG_READ_SUBSCRIBER_NUMBER, fd, client_tag, data, len);
    return (uiccd_msg_subscriber_number_t *) msg;
}


uiccd_msg_subscriber_number_response_t* uiccd_msg_subscriber_number_response_create(
                                                                          uintptr_t                           client_tag,
                                                                          sim_uicc_status_code_t              uicc_status_code,
                                                                          sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                                          sim_uicc_status_word_t              status_word,
                                                                          const uint8_t*                      data,
                                                                          size_t                              len)
{
    uiccd_msg_subscriber_number_response_t *msg;
    msg = uicc_msg_byte_data_response_create(UICCD_MSG_READ_SUBSCRIBER_NUMBER_RSP, uicc_status_code, uicc_status_code_fail_details,
                                             status_word, client_tag, data, len);

    return (uiccd_msg_subscriber_number_response_t *) msg;
}

uiccd_msg_sim_channel_send_t *uiccd_msg_sim_channel_send_create(int fd,
                                                                uintptr_t client_tag,
                                                                const char *data,
                                                                size_t len)
{
    uiccd_msg_data_t *msg;
    msg = uicc_msg_char_data_create(UICCD_MSG_SIM_CHANNEL_SEND, fd, client_tag,
                                    data, len);
    return (uiccd_msg_sim_channel_send_t *) msg;
}

uiccd_msg_sim_channel_send_response_t*
uiccd_msg_sim_channel_send_response_create(uintptr_t                           client_tag,
                                           sim_uicc_status_code_t              uicc_status_code,
                                           sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                           sim_uicc_status_word_t              status_word,
                                           const uint8_t*                      data,
                                           size_t                              len)
{
    uiccd_msg_byte_data_status_t       *msg;
    msg = uicc_msg_byte_data_response_create(UICCD_MSG_SIM_CHANNEL_SEND_RSP,
                                             uicc_status_code,
                                             uicc_status_code_fail_details,
                                             status_word, client_tag, data, len);
    return (uiccd_msg_sim_channel_send_response_t *) msg;
}

uiccd_msg_sim_channel_open_t *uiccd_msg_sim_channel_open_create(int fd,
                                                                uintptr_t client_tag,
                                                                const char *data,
                                                                size_t len)
{
  uiccd_msg_data_t *msg;
  msg = uicc_msg_char_data_create(UICCD_MSG_SIM_CHANNEL_OPEN, fd, client_tag,
                                  data, len);
  return (uiccd_msg_sim_channel_open_t *) msg;
}

uiccd_msg_sim_channel_open_response_t*
uiccd_msg_sim_channel_open_response_create(uintptr_t                           client_tag,
                                           sim_uicc_status_code_t              uicc_status_code,
                                           sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                           sim_uicc_status_word_t              status_word,
                                           uint16_t                            session_id)
{
    uiccd_msg_sim_channel_open_response_t* p;
    p = malloc(sizeof *p);
    if (p) {
        p->type=UICCD_MSG_SIM_CHANNEL_OPEN_RSP;
        p->delete_func=0;
        p->client_tag = client_tag;
        p->uicc_status_code=uicc_status_code;
        p->uicc_status_code_fail_details=uicc_status_code_fail_details;
        p->status_word = status_word;
        p->session_id = session_id;
    }
    return p;
}

uiccd_msg_sim_channel_close_t *uiccd_msg_sim_channel_close_create(int fd,
                                                                  uintptr_t client_tag,
                                                                  const char *data,
                                                                  size_t len)
{
  uiccd_msg_data_t *msg;
  msg = uicc_msg_char_data_create(UICCD_MSG_SIM_CHANNEL_CLOSE, fd, client_tag,
                                  data, len);
  return (uiccd_msg_sim_channel_close_t *) msg;
}

uiccd_msg_sim_channel_close_response_t*
uiccd_msg_sim_channel_close_response_create(uintptr_t                           client_tag,
                                            sim_uicc_status_code_t              uicc_status_code,
                                            sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                            sim_uicc_status_word_t              status_word)
{
    uiccd_msg_sim_channel_close_response_t* p;
    p = malloc(sizeof *p);
    if (p) {
        p->type=UICCD_MSG_SIM_CHANNEL_CLOSE_RSP;
        p->delete_func=0;
        p->client_tag = client_tag;
        p->uicc_status_code=uicc_status_code;
        p->uicc_status_code_fail_details=uicc_status_code_fail_details;
        p->status_word = status_word;
    }
    return p;
}

uiccd_msg_sap_session_t*   uiccd_msg_sap_session_start_create(int fd, uintptr_t client_tag)
{
    uiccd_msg_sap_session_t *msg = uiccd_msg_fd_create(UICCD_MSG_SAP_SESSION_START, fd, client_tag);
    return msg;
}


uiccd_msg_sap_session_status_t*  uiccd_msg_sap_session_start_response_create(uintptr_t client_tag, int status)
{
    uiccd_msg_server_status_t *msg = uiccd_msg_server_status_create(UICCD_MSG_SAP_SESSION_START_RSP, status, 0, client_tag);
    return msg;
}


uiccd_msg_sap_session_t*   uiccd_msg_sap_session_exit_create(int fd, uintptr_t client_tag)
{
    uiccd_msg_sap_session_t *msg = uiccd_msg_fd_create(UICCD_MSG_SAP_SESSION_EXIT, fd, client_tag);
    return msg;
}


uiccd_msg_sap_session_status_t*  uiccd_msg_sap_session_exit_response_create(uintptr_t client_tag, int status)
{
    uiccd_msg_server_status_t *msg = uiccd_msg_server_status_create(UICCD_MSG_SAP_SESSION_EXIT_RSP, status, 0, client_tag);
    return msg;
}

uiccd_msg_sap_session_t*  uiccd_msg_sap_session_ctrl_card_create(int fd, uintptr_t client_tag, UICCD_MSG_TYPES_t msgt)
{
    uiccd_msg_sap_session_t *msg = uiccd_msg_fd_create(msgt, fd, client_tag);
    return msg;
}

uiccd_msg_sap_session_apdu_data_t*  uiccd_msg_sap_session_apdu_data_create(int fd,
                                                                                     uintptr_t client_tag,
                                                                                     const char *data,
                                                                                     size_t len)
{
    uiccd_msg_data_t       *msg;
    msg = uicc_msg_char_data_create(UICCD_MSG_SAP_SESSION_APDU_DATA, fd, client_tag, data, len);
    return (uiccd_msg_sap_session_apdu_data_t *) msg;
}

uiccd_msg_sap_session_t*  uiccd_msg_sap_session_status_get_create(int fd, uintptr_t client_tag)
{
    uiccd_msg_sap_session_t *msg;
    msg = uiccd_msg_fd_create(UICCD_MSG_SAP_SESSION_STATUS_GET, fd, client_tag);
    return msg;
}
uiccd_msg_sap_session_status_t* uiccd_msg_sap_session_ctrlcard_deactivate_create_response(uintptr_t client_tag, int status)
{
    uiccd_msg_server_status_t *msg = uiccd_msg_server_status_create(UICCD_MSG_SAP_SESSION_POWEROFF_CARD_RSP, status, 0, client_tag);
    return msg;
}

uiccd_msg_sap_session_status_t* uiccd_msg_sap_session_ctrlcard_activate_create_response(uintptr_t client_tag, int status)
{
    uiccd_msg_server_status_t *msg = uiccd_msg_server_status_create(UICCD_MSG_SAP_SESSION_POWERON_CARD_RSP, status, 0, client_tag);
    return msg;
}

uiccd_msg_sap_session_status_t* uiccd_msg_sap_session_ctrlcard_coldreset_create_response(uintptr_t client_tag, int status)
{
    uiccd_msg_server_status_t *msg = uiccd_msg_server_status_create(UICCD_MSG_SAP_SESSION_COLDRESET_CARD_RSP, status, 0, client_tag);
    return msg;
}

uiccd_msg_sap_session_status_t* uiccd_msg_sap_session_ctrlcard_warmreset_create_response(uintptr_t client_tag, int status)
{
    uiccd_msg_server_status_t *msg = uiccd_msg_server_status_create(UICCD_MSG_SAP_SESSION_WARMRESET_CARD_RSP, status, 0, client_tag);
    return msg;
}

uiccd_msg_sap_session_status_t* uiccd_msg_sap_session_reset_ind_create_response(uintptr_t client_tag, int status)
{
    uiccd_msg_server_status_t *msg = uiccd_msg_server_status_create(UICCD_MSG_SAP_SESSION_RESET_CARD_IND, status, 0, client_tag);
    return msg;
}

uiccd_msg_byte_data_status_t* uiccd_msg_sap_session_atr_get_create_response(uintptr_t client_tag,
                                                                            sim_uicc_status_code_t              uicc_status_code,
                                                                            sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                                            sim_uicc_status_word_t              status_word,
                                                                            const uint8_t* data,
                                                                            size_t len)
{
    uiccd_msg_byte_data_status_t *msg = uicc_msg_uint_data_create(UICCD_MSG_SAP_SESSION_ATR_GET_RSP, client_tag,
                                                                    uicc_status_code, uicc_status_code_fail_details, status_word, data, len);
    return msg;
}

uiccd_msg_byte_data_status_t* uiccd_msg_sap_session_apdu_data_create_response(uintptr_t client_tag,
                                                                            sim_uicc_status_code_t              uicc_status_code,
                                                                            sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                                            sim_uicc_status_word_t              status_word,
                                                                            const uint8_t* data,
                                                                            size_t len)
{
    uiccd_msg_byte_data_status_t *msg = uicc_msg_uint_data_create(UICCD_MSG_SAP_SESSION_APDU_DATA_RSP, client_tag,
                                                                    uicc_status_code, uicc_status_code_fail_details, status_word, data, len);
    return msg;
}

uiccd_msg_reset_t *uiccd_msg_reset_create(int fd, uintptr_t client_tag)
{
  uiccd_msg_reset_t *p =
    uiccd_msg_fd_create(UICCD_MSG_RESET, fd, client_tag);

  return p;
}

uiccd_msg_sim_power_on_t   *uiccd_msg_sim_power_on_create(int fd, uintptr_t client_tag)
{
    uiccd_msg_sim_power_on_t *p = uiccd_msg_fd_create(UICCD_MSG_SIM_POWER_ON, fd, client_tag);

    return p;
}

uiccd_msg_sim_power_off_t   *uiccd_msg_sim_power_off_create(int fd, uintptr_t client_tag)
{
    uiccd_msg_sim_power_off_t *p = uiccd_msg_fd_create(UICCD_MSG_SIM_POWER_OFF, fd, client_tag);

    return p;
}

uiccd_msg_sim_read_preferred_rat_setting_t   *uiccd_msg_sim_read_preferred_rat_setting_create(int fd, uintptr_t client_tag)
{
    uiccd_msg_sim_read_preferred_rat_setting_t *p = uiccd_msg_fd_create(UICCD_MSG_SIM_READ_PREFERRED_RAT_SETTING, fd, client_tag);

    return p;
}

uiccd_msg_cn_t *uiccd_msg_cn_response_create(cn_message_t *msg_p)
{
    uiccd_msg_cn_t *p = uiccd_msg_cn_create(UICCD_MSG_CN_RSP, msg_p);

    return p;
}

uiccd_msg_cn_t *uiccd_msg_cn_event_create(cn_message_t *msg_p)
{
    uiccd_msg_cn_t *p = uiccd_msg_cn_create(UICCD_MSG_CN_EVENT, msg_p);

    return p;
}
