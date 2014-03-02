/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * File name       : uiccd.h
 * Description     : Internal definitions.
 *
 * Author          : Mats Bergstrom <mats.m.bergstrom@stericsson.com>
 *
 */

#ifndef __uiccd_h__
#define __uiccd_h__ (1)
#include <stdint.h>
#include "simd.h"

#include "msgq.h"
#include "catd_modem_types.h"

/**
 * @brief Handle input command from the socket.
 * @param cmd   Command to handle
 * @param client_tag Tag supplied by the client
 * @param buf   Payload of command
 * @param len   Sizeof payload
 * @param cc    Client context
 */
void
ste_uiccd_handle_command(uint16_t cmd,uintptr_t client_tag,
                         const char *buf,uint16_t len,
                         ste_sim_client_context_t * cc);


// FIXME: DOXYGEN!

#include <stdlib.h>
#include "state_machine.h"

// FIXME: What kind of data is needed here?
typedef void            uiccd_response_callback(void *data,
                                                void *user_data);
typedef struct uiccd_response_closure_tag {
    uiccd_response_callback *func;
    void                   *user_data;
} uiccd_response_closure_t;

int                     uiccd_start(uintptr_t client_tag);
int                     uiccd_stop(uintptr_t client_tag);

void                    uiccd_sig_msg( ste_msg_t* msg );

/* Signals to main */

void                    uiccd_sig_register(int fd, uintptr_t client_tag);

void                    uiccd_sig_connect(int fd, uintptr_t client_tag);
void                    uiccd_sig_disconnect(int fd, uintptr_t client_tag);

void                    uiccd_sig_pin_verify(int fd, uintptr_t client_tag, const char *buf,
                                             size_t len);
void                    uiccd_sig_pin_verify_response(uintptr_t                           client_tag,
                                                      sim_uicc_status_code_t              uicc_status_code,
                                                      sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                      sim_uicc_status_word_t              status_word);

void                    uiccd_sig_pin_change(int fd, uintptr_t client_tag, const char *buf,
                                             size_t len);

void                    uiccd_sig_pin_change_response(uintptr_t                           client_tag,
                                                      sim_uicc_status_code_t              uicc_status_code,
                                                      sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                      sim_uicc_status_word_t              status_word);

void                    uiccd_sig_pin_disable(int fd, uintptr_t client_tag, const char *buf,
                                             size_t len);
void                    uiccd_sig_pin_disable_response(uintptr_t                           client_tag,
                                                       sim_uicc_status_code_t              uicc_status_code,
                                                       sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                       sim_uicc_status_word_t              status_word);

void                    uiccd_sig_pin_enable(int fd, uintptr_t client_tag, const char *buf,
                                             size_t len);
void                    uiccd_sig_pin_enable_response(uintptr_t                           client_tag,
                                                      sim_uicc_status_code_t              uicc_status_code,
                                                      sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                      sim_uicc_status_word_t              status_word);

void                    uiccd_sig_pin_info(int fd, uintptr_t client_tag, const char *buf, size_t len);

// Not the contents of attempts and attempts2 depends on the context where
// pin info was called. In case only PIN or PUK are requested, attempts
// contain those PIN or PUK attempts. If both are requested, attempts has
// PIN attempts, and attempts2 has PUK attempts remaining
void                    uiccd_sig_pin_info_response(uintptr_t                           client_tag,
                                                    sim_uicc_status_code_t              uicc_status_code,
                                                    sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                    ste_sim_pin_status_t                pin_status,
                                                    int                                 attempts,
                                                    int                                 attempts2);

void                    uiccd_sig_pin_unblock(int fd, uintptr_t client_tag, const char *buf,
                                             size_t len);
void                    uiccd_sig_pin_unblock_response(uintptr_t                           client_tag,
                                                       sim_uicc_status_code_t              uicc_status_code,
                                                       sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                       sim_uicc_status_word_t              status_word);

void                    uiccd_sig_pin_request(uintptr_t client_tag, int event);

void                    uiccd_sig_sim_connect(int fd, uintptr_t client_tag);

void                    uiccd_sig_sim_connect_response(uintptr_t                           client_tag,
                                                       sim_uicc_status_code_t              uicc_status_code,
                                                       sim_uicc_status_code_fail_details_t uicc_status_code_fail_details);

void                    uiccd_sig_sim_disconnect(int fd, uintptr_t client_tag);

void                    uiccd_sig_sim_disconnect_response(uintptr_t                           client_tag,
                                                          sim_uicc_status_code_t              uicc_status_code,
                                                          sim_uicc_status_code_fail_details_t uicc_status_code_fail_details);


void                    uiccd_sig_read_sim_file_record(int fd, uintptr_t client_tag, const char* buf, size_t len);

void                    uiccd_sig_read_sim_file_record_response(uintptr_t                           client_tag,
                                                                sim_uicc_status_code_t              uicc_status_code,
                                                                sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                                sim_uicc_status_word_t              status_word,
                                                                const uint8_t*                      data,
                                                                size_t                              len);

void                    uiccd_sig_read_sim_file_binary(int fd, uintptr_t client_tag, const char* buf, size_t len);

void                    uiccd_sig_read_sim_file_binary_response(uintptr_t                           client_tag,
                                                                sim_uicc_status_code_t              uicc_status_code,
                                                                sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                                sim_uicc_status_word_t              status_word,
                                                                const uint8_t*                      data,
                                                                size_t                              len);

void                    uiccd_sig_update_sim_file_record(int fd, uintptr_t client_tag, const char* buf, size_t len);

void                    uiccd_sig_update_sim_file_record_response(uintptr_t                           client_tag,
                                                                  sim_uicc_status_code_t              uicc_status_code,
                                                                  sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                                  sim_uicc_status_word_t              status_word,
                                                                  const uint8_t*                      buf,
                                                                  size_t                              len);

void                    uiccd_sig_update_sim_file_binary(int fd, uintptr_t client_tag, const char* buf, size_t len);

void                    uiccd_sig_update_sim_file_binary_response(uintptr_t                           client_tag,
                                                                  sim_uicc_status_code_t              uicc_status_code,
                                                                  sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                                  sim_uicc_status_word_t              status_word,
                                                                  const uint8_t*                      buf,
                                                                  size_t                              len);

void                    uiccd_sig_sim_file_get_format(int fd, uintptr_t client_tag, const char *buf, size_t len);

void                    uiccd_sig_get_file_information(int fd, uintptr_t client_tag, const char *buf, size_t len);

void                    uiccd_sig_get_sim_state(int fd, uintptr_t client_tag, const char *buf, size_t len);

void                    uiccd_sig_sim_file_get_format_response(uintptr_t                           client_tag,
                                                               sim_uicc_status_code_t              uicc_status_code,
                                                               sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                               sim_uicc_status_word_t              status_word,
                                                               sim_file_structure_t                file_type,
                                                               int                                 file_size,
                                                               int                                 record_len,
                                                               int                                 num_records);

void                    uiccd_sig_get_file_information_response(uintptr_t                           client_tag,
                                                                sim_uicc_status_code_t              uicc_status_code,
                                                                sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                                sim_uicc_status_word_t              status_word,
                                                                uint8_t*                            data,
                                                                int                                 len);

void                    uiccd_sig_appl_apdu_send_response(uintptr_t                           client_tag,
                                                          sim_uicc_status_code_t              uicc_status_code,
                                                          sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                          sim_uicc_status_word_t              status_word,
                                                          const uint8_t*                      apdu_data,
                                                          size_t                              apdu_len);



void                    uiccd_sig_startup(int fd, uintptr_t client_tag);

void     uiccd_sig_startup_done(int fd, uintptr_t client_tag);
void                    uiccd_sig_shutdown(int fd, uintptr_t client_tag);

void                    uiccd_sig_modem_silent_reset(int fd, uintptr_t client_tag);

void                    uiccd_sig_server_status_response(uintptr_t client_tag, uint32_t status, int startup_completed);

void uiccd_sig_get_app_info(int fd, uintptr_t client_tag);

// The one and only uicc status sig fn!
void uiccd_sig_uicc_status_ind(uintptr_t client_tag, ste_uicc_status_t uicc_status);

void uiccd_sig_uicc_status_payload_ind(uintptr_t client_tag, ste_uicc_status_t uicc_status, ste_sim_app_type_t app_type, ste_uicc_card_type_t card_type);

void uiccd_sig_server_card_status(uintptr_t client_tag, uint32_t status, ste_sim_card_status_t card_status);

void uiccd_sig_card_status_ind(uintptr_t client_tag, ste_sim_card_status_t card_status);

// signal response of application list
void uiccd_sig_app_list(uintptr_t client_tag, sim_uicc_status_code_t status, int app_len);

// signal response of application activate.
void uiccd_sig_app_activate(uintptr_t client_tag, sim_uicc_status_code_t status, int app_id, ste_sim_app_type_t app_type);

// signal card fallback from UICC to ICC
void uiccd_sig_card_fallback(uintptr_t client_tag, ste_sim_app_type_t app_type, int app_id, ste_uicc_card_type_t card_type);

// signal response of app shutdown initiate
void uiccd_sig_app_shutdown_init(uintptr_t client_tag, sim_uicc_status_code_t status);

// signal pin verify is needed.

void uiccd_sig_pin_verify_needed(uintptr_t client_tag, uint32_t status, int app_id, int pin_id);


// signal pin unblock is needed.

void uiccd_sig_pin_unblock_needed(uintptr_t client_tag, uint32_t status, int app_id, int pin_id);


// signal pin is permanently blocked.

void uiccd_sig_pin_permanently_blocked(uintptr_t client_tag, uint32_t status, int app_id, int pin_id);

void uiccd_sig_modem_state(uintptr_t client_tag, int status);

void uiccd_sig_read_smsc(int fd, uintptr_t client_tag, const char* buf, size_t len);

void uiccd_sig_read_smsc_response(uintptr_t client_tag, int status, const char* buf, size_t len);

void uiccd_sig_update_smsc(int fd, uintptr_t client_tag, const char* buf, size_t len);

void uiccd_sig_update_smsc_response(uintptr_t client_tag, int status, const char* buf, size_t len);

void uiccd_sig_smsc_get_record_max(int fd, uintptr_t client_tag, const char* buf, size_t len);

void uiccd_sig_smsc_save_to_record(int fd, uintptr_t client_tag, const char* buf, size_t len);

void uiccd_sig_smsc_restore_from_record(int fd, uintptr_t client_tag, const char* buf, size_t len);

void uiccd_sig_sim_file_read_generic(int fd, uintptr_t client_tag, const char *buf, size_t len);

void uiccd_sig_sim_file_read_generic_response(uintptr_t                           client_tag,
                                              sim_uicc_status_code_t              uicc_status_code,
                                              sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                              sim_uicc_status_word_t              status_word,
                                              const uint8_t*                      data,
                                              size_t                              data_len);



void uiccd_sig_update_service_table(int fd, uintptr_t client_tag, const char* buf, size_t len);

void uiccd_sig_get_service_table(int fd, uintptr_t client_tag, const char* buf, size_t len);

void uiccd_sig_get_service_availability(int fd, uintptr_t client_tag, const char* buf, size_t len);

void uiccd_sig_app_status(int fd, uintptr_t client_tag, const char *buf, size_t len);

void uiccd_sig_card_status(int fd, uintptr_t client_tag);

void uiccd_sig_sim_status(uintptr_t client_tag, ste_reason_t reason);

void uiccd_sig_read_ecc(int fd, uintptr_t client_tag);

void uiccd_sig_sim_icon_read(int fd, uintptr_t client_tag, const char* buf, size_t len);

void uiccd_sig_sim_icon_read_response(uintptr_t client_tag,
                                      sim_uicc_status_code_t uicc_status_code,
                                      sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                      sim_uicc_status_word_t status_word,
                                      const uint8_t* data,
                                      size_t len);

void uiccd_sig_update_plmn(int fd, uintptr_t client_tag, const char *buf, size_t len);
void uiccd_sig_read_plmn(int fd, uintptr_t client_tag, const char* buf, size_t len);

void uiccd_sig_read_plmn_response(uintptr_t client_tag, int status, const char* buf, size_t len);

/**
 * For Read Subscriber Number Transaction.
 * File Identifiers for Read Subscriber Number Transaction.
 */
#define  UICCD_FILE_ID_MSISDN 0x6F40
#define  UICCD_FILE_ID_CCP2   0x6F4F
#define  UICCD_FILE_ID_EXT5   0x6F4E

void uiccd_sig_read_subscriber_number(int fd, uintptr_t client_tag, const char *buf, size_t len);

void uiccd_sig_read_subscriber_number_response(uintptr_t client_tag,
                                               sim_uicc_status_code_t uicc_status_code,
                                               sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                               sim_uicc_status_word_t status_word,
                                               const uint8_t* data,
                                               size_t len);
void uiccd_sig_sim_channel_send(int fd, uintptr_t client_tag, const char* buf, size_t len);

void uiccd_sig_sim_channel_open(int fd, uintptr_t client_tag, const char* buf, size_t len);

void uiccd_sig_sim_channel_close(int fd, uintptr_t client_tag, const char* buf, size_t len);

void uiccd_sig_sim_channel_send_response(uintptr_t                           client_tag,
                                         sim_uicc_status_code_t              uicc_status_code,
                                         sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                         sim_uicc_status_word_t              status_word,
                                         const uint8_t                      *data,
                                         size_t                              len);

void uiccd_sig_sim_channel_open_response(uintptr_t                           client_tag,
                                         sim_uicc_status_code_t              uicc_status_code,
                                         sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                         sim_uicc_status_word_t              status_word,
                                         uint16_t                            session_id);

void uiccd_sig_sim_channel_close_response(uintptr_t                           client_tag,
                                          sim_uicc_status_code_t              uicc_status_code,
                                          sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                          sim_uicc_status_word_t              status_word);

void uiccd_sig_reset(int fd, uintptr_t client_tag);

void uiccd_sig_sim_power_on(int fd, uintptr_t client_tag);

void uiccd_sig_sim_power_off(int fd, uintptr_t client_tag);

void uiccd_sig_sim_read_preferred_rat_setting(int fd, uintptr_t client_tag);

void uiccd_sig_read_fdn(int fd, uintptr_t client_tag);

void uiccd_sig_cn_response(cn_message_t *msg_p);

void uiccd_sig_cn_event(cn_message_t *msg_p);

void    uiccd_sig_sap_session_start(int fd, uintptr_t client_tag);
void    uiccd_sig_sap_session_exit(int fd, uintptr_t client_tag);
void    uiccd_sig_sap_session_ctrlcard(int fd, uintptr_t client_tag, uint16_t ctrl_action);
void    uiccd_sig_sap_session_apdu_data(int fd, uintptr_t client_tag, const char *buf, size_t len);
void    uiccd_sig_sap_session_status_get(int fd, uintptr_t client_tag);
void    uiccd_sig_sap_session_start_response(uintptr_t client_tag, int status);
void    uiccd_sig_sap_session_exit_response(uintptr_t client_tag, int status);

void    uiccd_sig_sap_session_ctrlcard_activate_response(uintptr_t client_tag, int status);
void    uiccd_sig_sap_session_ctrlcard_deactivate_response(uintptr_t client_tag, int status);
void    uiccd_sig_sap_session_ctrlcard_warmreset_response(uintptr_t client_tag, int status);
void    uiccd_sig_sap_session_ctrlcard_coldreset_response(uintptr_t client_tag, int status);
void    uiccd_sig_sap_session_ctrlcard_reset_ind(uintptr_t client_tag, int status);
void    uiccd_sig_sap_session_atr_get_response(uintptr_t client_tag,
                                                 sim_uicc_status_code_t              uicc_status_code,
                                                 sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                 sim_uicc_status_word_t              status_word,
                                                 const uint8_t*                      data,
                                                 size_t                              data_len);
void    uiccd_sig_sap_session_apdu_data_response(uintptr_t client_tag,
                                                 sim_uicc_status_code_t              uicc_status_code,
                                                 sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                 sim_uicc_status_word_t              status_word,
                                                 const uint8_t*                      data,
                                                 size_t                              data_len);

// Message function prototypes
// ---------------------------
ste_msg_t         *uicc_get_any_msg(const ste_sim_state_t* s, void *ud);

#define PIN_MAX_LEN     (16)

#define SIM_FILE_INFO_FILE_STATUS 11

#define USIM_SERVICE_TYPE_FDN_ENABLE 0x01
#define USIM_SERVICE_TYPE_BDN_ENABLE 0x02
#define USIM_SERVICE_TYPE_ACL_ENABLE 0x04

#define USIM_SERVICE_TYPE_FDN_DISABLE 0xFE
#define USIM_SERVICE_TYPE_BDN_DISABLE 0xFD
#define USIM_SERVICE_TYPE_ACL_DISABLE 0xFB

#define SIM_SERVICE_TYPE_FDN_DISABLE 0xEF
#define SIM_SERVICE_TYPE_ADN_DISABLE 0xFB

#define SIM_SERVICE_TYPE_ADN_NOT_INVALIDATED 0x01

#define USIM_SERVICE_TYPE_BIT1_INDICATES_AVAILABLE     0x01
#define USIM_SERVICE_TYPE_BIT2_INDICATES_AVAILABLE     0x02
#define USIM_SERVICE_TYPE_BIT3_INDICATES_AVAILABLE     0x04
#define USIM_SERVICE_TYPE_BIT4_INDICATES_AVAILABLE     0x08
#define USIM_SERVICE_TYPE_BIT5_INDICATES_AVAILABLE     0x10
#define USIM_SERVICE_TYPE_BIT6_INDICATES_AVAILABLE     0x20
#define USIM_SERVICE_TYPE_BIT7_INDICATES_AVAILABLE     0x40
#define USIM_SERVICE_TYPE_BIT8_INDICATES_AVAILABLE     0x80


#define SIM_SERVICE_TYPE_FDN_ACTIVATED_ALLOCATED 0x30
#define SIM_SERVICE_TYPE_ADN_ACTIVATED_ALLOCATED 0x0C

#define SIM_SERVICE_TYPE_BIT1_BIT2_IND_ACTIVATED_ALLOCATED   0x03
#define SIM_SERVICE_TYPE_BIT3_BIT4_IND_ACTIVATED_ALLOCATED   0x0C
#define SIM_SERVICE_TYPE_BIT5_BIT6_IND_ACTIVATED_ALLOCATED   0x30
#define SIM_SERVICE_TYPE_BIT7_BIT8_IND_ACTIVATED_ALLOCATED   0xC0

/**
 * Struct used to store PIN related data
 */
typedef struct {
  sim_uicc_pin_id_t  pin_id;
  char               pin[PIN_MAX_LEN];
  uint8_t            pin_len;
} ste_uicc_pin_info_t;

#define UICCD_CLIENT_TAG        0x00000002 // CLIENT TAG FOR UICCD
#define UICCD_FD                0xEFFFFFFE

#ifdef SIMD_SUPPORT_PIN_CACHE_FOR_MSR
void uiccd_msr_set_cached_pin(char * pin_p, uint8_t pin_len);
int uiccd_msr_get_cached_pin(char * pin_p, uint8_t pin_len);
void uiccd_msr_init_pin_caching();
void uiccd_msr_reset_pin_caching();
int uiccd_msr_get_pin_verifying_state();
void uiccd_msr_reset_cached_pin();
#endif

#endif
