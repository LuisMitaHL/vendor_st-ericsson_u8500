/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : catd_modem.h
 * Description     : catd modem abstraction
 *
 * Author          : Mats Bergstrom <mats.m.bergstrom@stericsson.com>
 *
 */


#ifndef __catd_modem_h__
#define __catd_modem_h__ (1)
#include "apdu.h"
#include "catd_modem_types.h"

typedef struct ste_modem_s ste_modem_t;

// FIXME: Remove
enum ste_sim_file_type_e {
    STE_SIM_FILE_UNKNOWN = 0,
    STE_SIM_FILE_IMSI,
    STE_SIM_FILE_CPHS_OPERATOR_NAME,
    STE_SIM_FILE_CPHS_CSP,
    STE_SIM_FILE_CPHS_INFO,
    STE_SIM_FILE_CPHS_MAILBOX,
    STE_SIM_FILE_CPHS_OPERATOR_NAME_SHORTFORM,
    STE_SIM_FILE_CPHS_INFO_NUMBERS,
    STE_SIM_FILE_PLMN_SEL,
    STE_SIM_FILE_ACM_MAX,
    STE_SIM_FILE_SERVICE_TABLE,
    STE_SIM_FILE_ACM,
    STE_SIM_FILE_SMS,
    STE_SIM_FILE_SMSP,
    STE_SIM_FILE_SMSS,
    STE_SIM_FILE__MAX                             /* MAX VALUE */
};

typedef enum ste_sim_file_type_e ste_sim_file_type_t;

// FIXME: Remove? No use for this now.
enum ste_sim_record_id_e {
    STE_SIM_RECORD_ID_ABSOLUTE_FIRST = 0x0001,
    STE_SIM_RECORD_ID_ABSOLUTE_LAST  = 0x00FE,
    STE_SIM_RECORD_ID_PREVIOUS       = 0x0100,
    STE_SIM_RECORD_ID_CURRENT        = 0x0200,
    STE_SIM_RECORD_ID_NEXT           = 0x0400
};

// FIXME: Decide if such a path length is safe.
// FIXME: Remove this structure, as it should not be used.
#define SIM_MAX_PATH_LENGTH                       (10)
typedef struct
{
  uint8_t pathlen;
  uint8_t path[SIM_MAX_PATH_LENGTH];
} ste_sim_path_t;

typedef enum ste_sim_record_id_e ste_sim_record_id_t;

// FIXME: What is this for?
// Type of pin event, but should be reconsidered
#define PIN_EVENT_NONE (0) // Added by Stefan for now
#define INITIAL_PIN_REQUEST (1)

/**
 * @brief       Start the modem thread.  Does not connect to the modem!
 * @return      - New object representing the modem.
 */
ste_modem_t            *ste_catd_modem_new();


/**
 * @brief       Deletes a modem object.
 *              Disconnects from the modem if not already done
 */
void                    ste_catd_modem_delete(ste_modem_t * m, uintptr_t client_tag);


/**
 * @brief       Connects to the modem and initialises.
 * @param       m       Modem object to operate on
 * @param       client_tag Tag supplied by the client
 * @return      -       Status of the operation. 0 on success, -1 on fail.
 */
int                     ste_catd_modem_connect(ste_modem_t * m,
                                               uintptr_t client_tag);

/**
 * @brief       Disconnects from the modem.
 * @param       m       Modem object to operate on
 * @param       client_tag Tag supplied by the client
 * @return      -       Status of the operation. 0 on success, -1 on fail.
 */
int                     ste_catd_modem_disconnect(ste_modem_t * m,
                                                  uintptr_t client_tag);

/**
 * @brief       Send the terminal profile download to the modem.
 * @param       m       Modem object to operate on
 * @param       client_tag Tag supplied by the client
 * @param       tp      The raw terminal profile to send
 * @param       tp_size Size of the raw terminal profile
 * @return      -       Status of the operation. 0 on success, -1 on fail.
 */
int                     ste_catd_modem_tpdl(ste_modem_t * m,
                                            uintptr_t client_tag,
                                            uint8_t* tp,
                                            size_t tp_size);

/**
 * @brief       Request to enable modem to fetch proactive commands.
 * @param       m       Modem object to operate on
 * @param       client_tag Tag supplied by the client
 * @return      -       Status of the operation. 0 on success, -1 on fail.
 */
int                     ste_catd_modem_enable(ste_modem_t * m,
                                              uintptr_t client_tag);

/**
 * @brief       Request to disable modem to fetch proactive commands.
 * @param       m       Modem object to operate on
 * @param       client_tag Tag supplied by the client
 * @return      -       Status of the operation. 0 on success, -1 on fail.
 */
int                     ste_catd_modem_disable(ste_modem_t * m,
                                               uintptr_t client_tag);

/**
 * @brief       Send an envelope command APDU to the modem.
 * @param       m       Modem object to operate on
 * @param       client_tag Tag supplied by the client
 * @param       apdu    APDU containing envelope command to send
 * @return      -       Status of the operation. 0 on success, -1 on fail.
 */
int                     ste_catd_modem_ec(ste_modem_t * m,
                                          uintptr_t client_tag,
                                          ste_apdu_t * apdu);

/**
 * @brief       Send a terminal response APDU to the modem.
 * @param       m       Modem object to operate on
 * @param       client_tag Tag supplied by the client
 * @param       apdu    APDU containing terminal response to send
 * @return      -       Status of the operation. 0 on success, -1 on fail.
 */
int                     ste_catd_modem_tr(ste_modem_t * m,
                                          uintptr_t client_tag,
                                          ste_apdu_t * apdu);

/**
 * @brief       Set the poll interval
 *
 * This function sets the poll interval asynchronously.
 *
 * Setting the interval to zero means that polling is turned off and setting it positive number means polling is enabled.
 * The poll interval is specified in 1/10ths of seconds (i.e. 100 ms units). However the actual precision depends on the
 * modem used and could be less.
 *
 * The response will come in the catd_sig_modem_set_poll_intervall_response() call back function, see catd.h
 * The modem must reply with the actual poll interval used.
 *
 * @param       m          Modem object to operate on
 * @param       client_tag Tag supplied by the client
 * @param       interval   The poll interval in tenths of seconds, 0 means polling off. Max value is 153000.
 * @return      -          Status of the operation. 0 on success, -1 on fail.
 */
int ste_catd_modem_set_poll_intervall(ste_modem_t * m,
                                      uintptr_t client_tag,
                                      int interval);

/**
 * @brief       Set the poll req
 *
 * This function requests to poll the card.
 *
 *
 * @param       m          Modem object to operate on
 * @param       client_tag Tag supplied by the client
 * @return      -          Status of the operation. 0 on success, -1 on fail.
 */

int ste_catd_modem_poll_req(ste_modem_t * m, uintptr_t client_tag);

/**
 * @brief       Send an APDU to the modem mirror.
 * @param       m       Modem object to operate on
 * @param       client_tag Tag supplied by the client
 * @param       apdu    APDU to send
 * @return      -       Status of the operation. 0 on success, -1 on fail.
 */
int                     ste_catd_modem_mirror(ste_modem_t * m,
                                              uintptr_t client_tag,
                                              int fd, ste_apdu_t * apdu);

/**
 * @brief       Verify the pin.
 * @param       m       Modem object to operate on
 * @param       client_tag Tag supplied by the client
 * @param       app_id  Current application id
 * @param       pin_id  Current PIN id
 * @param       pin     Pointer to PIN data.
 * @param       len     Length of the pin data.
 * @return      -       Status of the operation. 0 on success, -1 on fail.
 */
int                     ste_modem_pin_verify(ste_modem_t * m,
                                             uintptr_t client_tag,
                                             int app_id,
                                             sim_uicc_pin_id_t pin_id,
                                             const char *pin,
                                             unsigned len);


/**
 * @brief       Disable the pin.
 * @param       m       Modem object to operate on
 * @param       client_tag Tag supplied by the client
 * @param       app_id  Current application id
 * @param       pin_id  Current PIN id
 * @param       pin     Pointer to PIN data.
 * @param       len     Length of the pin data.
 * @return      -       Status of the operation. 0 on success, -1 on fail.
 */
int                     ste_modem_pin_disable(ste_modem_t * m,
                                              uintptr_t client_tag,
                                              int app_id, int pin_id,
                                              const char *pin, unsigned len);


/**
 * @brief       Enable the pin.
 * @param       m       Modem object to operate on
 * @param       client_tag Tag supplied by the client
 * @param       app_id  Current application id
 * @param       pin_id  Current PIN id
 * @param       pin     Pointer to PIN data.
 * @param       len     Length of the pin data.
 * @return      -       Status of the operation. 0 on success, -1 on fail.
 */
int                     ste_modem_pin_enable(ste_modem_t * m,
                                             uintptr_t client_tag,
                                             int app_id,
                                             int pin_id,
                                             const char *pin, unsigned len);

/**
 * @brief       Request PIN info
 * @param       m       Modem object to operate on
 * @param       client_tag Tag supplied by the client
 * @param       app_id  Current application id
 * @param       pin_id  Current PIN
 * @return      -       Status of the operation. 0 on success, -1 on fail.
 */
int                     ste_modem_pin_info(ste_modem_t * m,
                                           uintptr_t client_tag,
                                           int app_id,
                                           sim_uicc_pin_puk_id_t pin);


/**
 * @brief       Change the pin.
 * @param       m            Modem object to operate on
 * @param       client_tag   Tag supplied by the client
 * @param       app_id       Current application id
 * @param       pin_id       Pin ID of the PIN that will be changed.
 * @param       old_pin      Pointer to old PIN data.
 * @param       old_pin_len  Length of the old pin data.
 * @param       new_pin      Pointer to new PIN data.
 * @param       new_pin_len  Length of the new pin data.
 * @return      -       Status of the operation. 0 on success, -1 on fail.
 */
int                     ste_modem_pin_change(ste_modem_t * m,
                                             uintptr_t client_tag,
                                             int app_id,
                                             sim_uicc_pin_id_t pin_id,
                                             const char *old_pin,
                                             unsigned old_pin_len,
                                             const char *new_pin,
                                             unsigned new_pin_len);


/**
 * @brief       Unblock the PIN.
 * @param       m       Modem object to operate on
 * @param       client_tag Tag supplied by the client
 * @param       app_id  Current application id
 * @param       pin_id  Current PIN id
 * @param       pin     Pointer to PIN code data.
 * @param       pin_len Length of the pin code data.
 * @param       puk     Pointer to PUK code data.
 * @param       puk_len Length of the PUK code data.
 * @return      -       Status of the operation. 0 on success, -1 on fail.
 */
int                     ste_modem_pin_unblock(ste_modem_t * m,
                                              uintptr_t client_tag,
                                              int app_id,
                                              sim_uicc_pin_id_t pin_id,
                                              const char *pin, unsigned pin_len,
                                              const char *puk, unsigned puk_len);




// FIXME: We still need a low-level read and update!

/**
 * @brief       Read request towards EF file.
 * @param       m                     Modem object to operate on
 * @param       client_tag            Tag supplied by the client
 * @param       app_id                Id for SIM application
 * @param       sim_file_type         Type of SIM file to read from
 * @param       rec_id                Record number or record access mode
 * @param       number_of_records     Number of records to read from EF file
 * @param       length                Length of data to read from EF file
 * @param       file_path             Path to SIM file
 * @param       path_len              Length of the file path
 * @return      -       Status of the operation. 0 on success, -1 on fail.
 */
int ste_modem_file_read_record(ste_modem_t * m,
                               uintptr_t client_tag,
			       int app_id,
                               int file_id,
                               int rec_id,
                               int length,
                               const char *file_path);


/**
 * @brief       Read request towards EF binary file.
 * @param       m                     Modem object to operate on
 * @param       client_tag            Tag supplied by the client
 * @param       app_id                Id for SIM application
 * @param       sim_file_type         Type of SIM file to read from
 * @param       offset                Offset in bytes into the file (to be) read.
 * @param       length                Length of data to read from EF file
 * @param       file_path             Path to SIM file
 * @param       path_len              Length of the file path
 * @return      -       Status of the operation. 0 on success, -1 on fail.
 */
int ste_modem_file_read_binary(ste_modem_t * m,
                               uintptr_t client_tag,
			       int app_id,
                               int file_id,
                               int offset,
                               int length,
                               const char *file_path);


/**
 * @brief       Update request towards EF file.
 * @param       m          Modem object to operate on
 * @param       client_tag Tag supplied by the client
 * @param       app_id     Id for SIM application
 * @param       data       Data to be written to the ELF file
 * @param       length     Length of data to be written to the EF file
 * @param       ef_type    The EF file to be updated
 * @param       rec_id     Record number or write access mode
 * @param       file_path  Pointer to the path of the sim file
 * @param       path_len   Length of the file path
 * @return      -       Status of the operation. 0 on success, -1 on fail.
 */

int ste_modem_file_update_record(ste_modem_t * m,
				 uintptr_t client_tag,
				 int app_id,
				 int file_id,
				 int rec_id,
				 int length,
				 const char *file_path,
				 uint8_t *data);

/**
 * @brief       Update request towards ELF file.
 * @param       m          Modem object to operate on
 * @param       client_tag Tag supplied by the client
 * @param       app_id     Id for SIM application
 * @param       data       Data to be written to the ELF file
 * @param       length     Length of data to be written to the EF file
 * @param       ef_type    The EF file to be updated
 * @param       offset     Offset of the file to be written
 * @param       file_path  Pointer to the path of the sim file
 * @param       path_len   Length of the file path
 * @return      -       Status of the operation. 0 on success, -1 on fail.
 */

int ste_modem_file_update_binary(ste_modem_t * m,
				 uintptr_t client_tag,
				 int app_id,
				 int file_id,
				 int offset,
				 int length,
				 const char *file_path,
				 uint8_t *data);

/**
 * @brief       Get information for a particular EF file.
 * @param       m                     Modem object to operate on
 * @param       client_tag            Tag supplied by the client
 * @param       app_id                Id for SIM application
 * @param       sim_file_id           ID of a EF file
 * @param       path                  SIM file path
 * @return      -       Status of the operation. 0 on success, -1 on fail.
 */
int ste_modem_get_file_information(ste_modem_t * m,
                                   uintptr_t client_tag,
                                   int app_id,
                                   int file_id,
                                   const char *file_path,
                                   sim_uicc_get_file_info_type_t type);



/**
 * @brief       Get Format for EF binary file.
 * @param       m                     Modem object to operate on
 * @param       client_tag            Tag supplied by the client
 * @param       sim_file_type         Type of SIM file for which the format is needed
 * @param       file_path             Pointer to the path of the sim file
 * @param       path_len              Length of the file path
 * @return      -       Status of the operation. 0 on success, -1 on fail.
 */
int ste_modem_sim_file_get_format(ste_modem_t * m,
                                  uintptr_t client_tag,
                                  int app_id,
                                  int file_id,
                                  const char * file_path);

/**
 * @brief       Send an appl APDU.
 * @param       m                     Modem object to operate on
 * @param       client_tag            Tag supplied by the client
 * @param       app_id                Id for SIM application
 * @param       cmd_force             Force APDU command
 * @param       apdu_len              Length of APDU to be sent
 * @param       apdu                  APDU to be sent
 * @param       file_id               File_id of file to be operated on
 * @param       file_path             Pointer to the path of the sim file
 * @return      -       Status of the operation. 0 on success, -1 on fail.
 */
int ste_modem_appl_apdu_send(ste_modem_t * m,
                             uintptr_t client_tag,
                             int app_id,
                             int cmd_force,
                             int apdu_len,
                             uint8_t *apdu,
                             int file_id,
                             const char *file_path);

/**
 * @brief       Connect SIM.
 * @param       m                     Modem object to operate on
 * @param       client_tag            Tag supplied by the client
 * @return      -       Status of the operation. 0 on success, -1 on fail.
 */
int ste_modem_sim_connect(ste_modem_t * m,
                          uintptr_t client_tag);

/**
 * @brief       Disconnect SIM.
 * @param       m                     Modem object to operate on
 * @param       client_tag            Tag supplied by the client
 * @return      -       Status of the operation. 0 on success, -1 on fail.
 */
int ste_modem_sim_disconnect(ste_modem_t * m,
                          uintptr_t client_tag);

/**
 * @brief       Activates the selected a telecom app on card
 * @param       m       Modem object to operate on
 * @param       client_tag Tag supplied by the client
 *
 * NOTE: appl list MUST have been called before calling this function
 */

int ste_modem_activate_selected_app(ste_modem_t *m, uintptr_t client_tag);

/**
 * @brief       Deactivates the selected telecom app on card
 * @param       m       Modem object to operate on
 * @param       client_tag Tag supplied by the client
 *
 */
int ste_modem_deactivate_selected_app(ste_modem_t *m, uintptr_t client_tag);

/**
 * @brief       Requests for initiating application shutdown
 * @param       m       Modem object to operate on
 * @param       client_tag Tag supplied by the client
 *
 */
int ste_modem_app_initiate_shutdown(ste_modem_t *m, uintptr_t client_tag);

/**
 * @brief       Selects the telecom appl to activate from card
 * @param       m       Modem object to operate on
 * @param       client_tag Tag supplied by the client
 * @return      -       Status of the operation. 0 on success, -1 on fail.
 *
 * The function saved the apps found on the card in the apps table, and then
 * sets the active_i to the selected telecom app.
 */
int ste_modem_application_list_and_select(ste_modem_t *m, uintptr_t client_tag);

/**
 * @brief       Returns uicc application list
 * @param       m       Modem object to operate on
 * @param       client_tag Tag supplied by the client
 * @return      -       Status of the operation. 0 on success, -1 on fail.
 */
int ste_modem_application_list(ste_modem_t * m, uintptr_t client_tag);

/**
 * @brief       Card status
 * @param       m       Modem object to operate on
 * @param       client_tag Tag supplied by the client
 * @return      -       Status of the operation. 0 on success, -1 on fail.
 */

int ste_modem_card_status(ste_modem_t * m, uintptr_t client_tag);

/**
 * @brief       Refresh request
 * @param       m          Modem object to operate on
 * @param       client_tag Tag supplied by the client
 * @param       type       The kind of PC refresh to be performed.
 * @param       app_id     The application id to be refreshed on.
 * @param       file_list  The files to be refreshed.
 * @return      -       Status of the operation. 0 on success, -1 on fail.
 */
int ste_catd_modem_refresh_request(ste_modem_t * m,
                                   uintptr_t client_tag,
                                   ste_apdu_refresh_type_t type,
                                   ste_apdu_application_id_t * app_id,
                                   ste_apdu_file_list_t * file_list);

/**
 * @brief       Card cold reset
 * @param       m       Modem object to operate on
 * @param       client_tag Tag supplied by the client
 * @return      -       Status of the operation. 0 on success, -1 on fail.
 */

int ste_modem_sap_session_ctrl_coldreset_card(ste_modem_t * m, uintptr_t client_tag);

/**
 * @brief       Card warm reset
 * @param       m       Modem object to operate on
 * @param       client_tag Tag supplied by the client
 * @return      -       Status of the operation. 0 on success, -1 on fail.
 */
int ste_modem_sap_session_ctrl_warmreset_card(ste_modem_t * m, uintptr_t client_tag);

/**
 * @brief       Card activate (power on)
 * @param       m       Modem object to operate on
 * @param       client_tag Tag supplied by the client
 * @return      -       Status of the operation. 0 on success, -1 on fail.
 */
int ste_modem_sap_session_ctrl_activate_card(ste_modem_t * m, uintptr_t client_tag);

/**
 * @brief       Card decativate (power off)
 * @param       m       Modem object to operate on
 * @param       client_tag Tag supplied by the client
 * @return      -       Status of the operation. 0 on success, -1 on fail.
 */

int ste_modem_sap_session_ctrl_deactivate_card(ste_modem_t * m, uintptr_t client_tag);

/**
 * @brief       Card send ATR
 * @param       m       Modem object to operate on
 * @param       client_tag Tag supplied by the client
 * @return      -       Status of the operation. 0 on success, -1 on fail.
 */

int ste_modem_sap_session_atr_get(ste_modem_t * m, uintptr_t client_tag);

/**
 * @brief       Card send APDU
 * @param       m       Modem object to operate on
 * @param       client_tag Tag supplied by the client
 * @return      -       Status of the operation. 0 on success, -1 on fail.
 */

int ste_modem_sap_session_apdu_data(ste_modem_t * m, uintptr_t client_tag, uint8_t *data, size_t length);

int ste_mal_card_status(ste_mal_card_status_data_t *data);

int ste_mal_app_status(int app_index, ste_mal_app_status_data_t *data);

/**
 * @brief       Channel send APDU
 * @param       m           Modem object to operate on
 * @param       client_tag  Tag supplied by the client
 * @param       session_id  The USIM application session identifier
 * @param       apdu_len    The length of the APDU
 * @param       apdu        The APDU
 * @param       path        The path if the select has previously been cached (NULL if no caching has been done)
 * @param       path_length The path length
 * @return      -       Status of the operation. 0 on success, -1 on fail.
 */
int ste_modem_channel_send(ste_modem_t * m,
                           uintptr_t client_tag,
                           uint16_t session_id,
                           size_t apdu_len,
                           uint8_t *apdu,
                           uint8_t *path,
                           uint8_t path_length);

/**
 * @brief       Channel Open
 * @param       m           Modem object to operate on
 * @param       client_tag  Tag supplied by the client
 * @param       aid_len     The length of the Application Identifier
 * @param       aid         The AID
 * @return      -       Status of the operation. 0 on success, -1 on fail.
 */
int ste_modem_channel_open(ste_modem_t * m, uintptr_t client_tag, size_t aid_len, uint8_t *aid);

/**
 * @brief       Channel Close
 * @param       m           Modem object to operate on
 * @param       client_tag  Tag supplied by the client
 * @param       session_id  The USIM application session identifier
 * @return      -       Status of the operation. 0 on success, -1 on fail.
 */
int ste_modem_channel_close(ste_modem_t * m, uintptr_t client_tag, uint16_t session_id);


/**
 * @brief       Debug Level
 * @param       m           Modem object to operate on
 * @param       client_tag  Tag supplied by the client
 * @param       level       The debug level for modem layer.
 * @return      -       Status of the operation. 0 on success, -1 on fail.
 *
 * @Note: At present, this does NOT send a response.
 */
void ste_modem_set_debug_level(ste_modem_t * m, uintptr_t client_tag, int level);

/**
 * @brief       Resets the internal state of the SIM modem backend. Function resets to init state.
 * @param       m           Modem object to operate on
 * @param       client_tag  Tag supplied by the client
 * @param       level       The debug level for modem layer.
 * @return      -       Status of the operation. 0 on success, -1 on fail.
 *
 * @Note: At present, this does NOT send a response.
 */
void ste_modem_reset_backend_state(ste_modem_t * m, uintptr_t client_tag);

/**
 * @brief       Update the adapt terminal support table file on the sim
 * @param       m       Modem object to operate on
 * @param       client_tag Tag supplied by the client
 * @param       data    The raw terminal support flags
 * @param       data_size Size of the raw terminal
 * @return      -       Status of the operation. 0 on success, -1 on fail.
 */
int ste_catd_modem_adapt_terminal_support_table_update(ste_modem_t * m,
                                             uintptr_t client_tag,
                                             int offset,
                                             const uint8_t* data,
                                             size_t data_size);

/**
 * @brief       Gets state information
 * @param       m       Modem object to operate on
 *
 * The method is synchronous and returns the state information. The PIN states
 * returned from this function is related to the telecom application.
 */

void ste_modem_uicc_status_info(ste_modem_t * m, ste_uicc_status_t *status);

#endif
