/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __cn_pdc_ecc_list_handling_h__
#define __cn_pdc_ecc_list_handling_h__ (1)

#include "cn_data_types.h"
#include "cn_message_types.h"
#include "cn_pdc_internal.h"
#include "message_handler.h"


/**
 * \enum  cn_emergency_number_origin_internal_t
 * \brief This enum describes emergency number origin.
 */
typedef enum {
    CN_EMERGENCY_NUMBER_ORIGIN_HARD_CODED  = 0x1,
    CN_EMERGENCY_NUMBER_ORIGIN_SIM         = 0x2,
    CN_EMERGENCY_NUMBER_ORIGIN_NETWORK     = 0x4,
    CN_EMERGENCY_NUMBER_ORIGIN_MEMORY      = 0x8,
    CN_EMERGENCY_NUMBER_ORIGIN_CLIENT      = 0x10,
    CN_EMERGENCY_NUMBER_ORIGIN_UNSPECIFIED = 0x20
} cn_emergency_number_origin_internal_t;

/**
* \struct  cn_emergency_number_config_internal_t
* \brief   Emergency number config structure
* data related to the save-list commands, not only the number.
*/
typedef struct {
    cn_emergency_call_service_bf_t service_type;                /**< Emergency call service type                           */
    cn_emergency_number_origin_internal_t origin;               /**< Origin of the emergency call                          */
    char emergency_number[CN_EMERGENCY_NUMBER_STRING_LENGTH+1]; /**< Emergency number, NULL terminated char array          */
    char mcc[CN_MCC_STRING_LENGTH+1];                           /**< Mobile Country Code, Null terminated char array       */
    char mcc_range[CN_MCC_RANGE_STRING_LENGTH+1];               /**< Mobile Country Code Sequence char, ranges: '2' to '9' */
} cn_emergency_number_config_internal_t;

/**
* \struct  cn_emergency_number_list_internal_t
* \brief   List of stored emergency numbers. Same as cn_emergency_number_list_t but containing all
* data related to the save-list commands, not only the number.
*/
typedef struct cn_emergency_number_list_internal_struct {
    cn_uint8_t  num_of_emergency_numbers;      /**< Number of emergency numbers */
    cn_emergency_number_config_internal_t emergency_number[1]; /**< Emergency number, Dynamic array with the length num_of_emergency_numbers */
} cn_emergency_number_list_internal_t;

#define CN_ENL_SERVICE_MAX_LENGTH 2 /* Maximum number of digits in <origin> */
#define CN_ENL_ORIGIN_MAX_LENGTH 2 /* Maximum number of digits in <cat> */
#define CN_ENL_DELIMITERS_LENGTH 6 /* Number of delimiters in buffer, ';' and '\n' (including '\0') */

/* Entry buffer size for emergency number list <number>;<cat>;<mcc>;<mcc_range>;<origin>\n\0 */
#define CN_ENL_BUFFER_SIZE CN_EMERGENCY_NUMBER_STRING_LENGTH +\
    CN_ENL_SERVICE_MAX_LENGTH +\
    CN_MCC_STRING_LENGTH +\
    CN_MCC_RANGE_STRING_LENGTH +\
    CN_ENL_ORIGIN_MAX_LENGTH +\
    CN_ENL_DELIMITERS_LENGTH

/**
 * \fn cn_pdc_set_emergency_number_filepath(char* filepath)
 * \brief Set the filepath where the emergency number list is saved and read from.
 *
 * If not set a default value will be used.
 */
void cn_pdc_set_emergency_number_filepath(char* filepath_p);

/**
 * \fn cn_pdc_get_stored_emerg_number_list(void)
 * \brief Get the emergency number list containing extra emergency numbers
 *
 * This method will read the emergency number list from the file system and return it.
 * Note that the method allocates memory for the list, and it must be released by the
 * calling method.
 *
 * \return  \ref cn_emergency_number_list_internal_t, the list of emergency numbers.
 *
 */
cn_emergency_number_list_internal_t *cn_pdc_get_stored_emerg_number_list(void);

/**
 * \fn cn_pdc_get_default_emerg_number_list(void)
 * \brief Get the emergency number list containing default (3gpp specified)
 * emergency numbers
 *
 * This method will return the emergency number list.
 * Note that the method allocates memory for the list, and it must be released by the
 * calling method.*
 */
cn_emergency_number_list_internal_t *cn_pdc_get_default_emerg_number_list(void);

/**
 * \fn cn_emergency_number_list_internal_t* cn_pdc_get_sim_emerg_number_list(cn_pdc_t* pdc_p)
 * \brief Get the emergency number list containing any values in sim
 *
 * Call method to get the emergency number list from SIM. Note that the method due to
 * the SIM communication is asynchronous, and will need to be called several times
 * in order to read SIM availability and any list of emergency numbers.
 *
 * If no SIM is available a default list will be returned containing default emergency
 * numbers.
 *
 * Note that the method allocates memory for the list in the case of
 * CN_PDC_ECC_STATE_RECEIVED_SIM_LIST, which must be released by the
 * calling method.
 */
cn_emergency_number_list_internal_t* cn_pdc_get_sim_emerg_number_list(cn_pdc_t* pdc_p);

/**
 * \fn cn_pdc_util_clear_emergency_number_list()
 * \brief Clear emergency number list.
 *
 * This method will clear the emergency number list. This is done by
 * opening the file for writing (non-update mode) and then closing it.
 * This overwrites the previous file with an empty file which is seen
 * as an empty list by the other functions.
 *
 * \return  \ref cn_bool_t, TRUE is returned if successful, else FALSE.
 *
 */
cn_bool_t cn_pdc_util_clear_emergency_number_list(void);

/**
 * \fn cn_pdc_util_clear_emergency_numbers_by_origin(uint16_t origins, cn_bool_t compare_mcc)
 * \brief Clear emergency numbers with a specified origin from list.
 *
 * This method will clear the emergency numbers with the specified origin
 * from the list. It is done by caching the current file content,
 * open the file for writing (non-update mode), write back the
 * emergency numbers with a different origin from the specified origin
 * and closing the file.
 *
 * \return  \ref cn_bool_t, TRUE is returned if successful, else FALSE.
 *
 */
cn_bool_t cn_pdc_util_clear_emergency_numbers_by_origin(uint16_t origins, cn_bool_t compare_mcc);

/*
 * \fn cn_bool_t cn_pdc_util_add_emergency_numbers_to_list(cn_emergency_number_list_internal_t *sublist_p);
 * \brief Add a sublist of emergency numbers to the emergency number list stored on file.
 *
 * Method will open the file with emergency numbers, add the specified numbers last, and write
 * back the entire file to the filesystem.
 *
 * Contains checks so that an identical number cannot be added twice.
 */
cn_bool_t cn_pdc_util_add_emergency_numbers_to_list(cn_emergency_number_list_internal_t *sublist_p);

/*
 * \fn cn_bool_t cn_pdc_util_rem_emergency_number_from_list(cn_emergency_number_config_internal_t *config_p);
 * \brief Removes a number from the list of ECC numbers stored on file.
 *
 * This method reads the emergency number list from file, opens the file
 * for writing (non-update mode), write back all emergency numbers except the
 * specified number to be removed.
 *
 * Contains checks so that a number must exist before it can be removed.
 */
cn_bool_t cn_pdc_util_rem_emergency_number_from_list(cn_emergency_number_config_internal_t *config_p);
/*
 * \fn cn_bool_t cn_pdc_equal_numbers(char* number_p, char* reference_p)
 * \brief Comparison method for two numbers. Currently a straight strcmp.
 */
cn_bool_t cn_pdc_equal_numbers(char* number_p, char* reference_p);

/*
 * \fn cn_bool_t cn_pdc_fdn_equal_numbers(char* phone_number_p, char* fdn_number_p, int fdn_number_length, int fdn_ton)
 * \brief Comparison method to determine if a phone number matches an FDN entry. Adheres to rules defined in 3gpp 22.101.
 */
cn_bool_t cn_pdc_fdn_equal_numbers(char* phone_number_p, char* fdn_number_p, int fdn_number_length, int fdn_ton);

/*
 * \fn cn_bool_t cn_pdc_set_current_mcc(char *mcc_p)
 * \brief Sets current mcc to be used by the pdc emergency list handling.
 *
 * This method updates the mcc cache used when handling the emergency number list.
 *
 * \return  \ref cn_bool_t, TRUE if current network number entries needs to be refreshed, else FALSE.
 */
cn_bool_t cn_pdc_set_current_mcc(char *mcc_p);


#endif /* __cn_pdc_ecc_list_handling_h__ */
