/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef STE_RIL_SMS_UTILS_H
#define STE_RIL_SMS_UTILS_H 1

#include "t_sms.h"
#include "t_cbs.h"
#include "exe.h"


/*
 * This function is used to indicate error reason for a SMS request that fails.
 *
 * @param [in] prefix_string_p   Pointer to string that is added in the beginning of the printout. It is
 *                               intended to contain the name of the calling function.
 *
 * @param [in] sms_error         The SMS error description that should be printed to the log.
 *
 */
void print_sms_error(const char *prefix_string_p, SMS_Error_t sms_error);

/*
 * This function is used to return a pointer to an error string corresponding to the SMS error.
 *
 * @param [in] sms_error         The SMS error code to be translated to a string error
 *                               description.
 *
 */
const char *str_sms_error(SMS_Error_t sms_error);

/*
 * This function is used to indicate error reason for a CBS request that fails.
 *
 * @param [in] prefix_string_p   Pointer to string that is added in the beginning of the printout. It is
 *                               intended to contain the name of the calling function.
 *
 * @param [in] cbs_error         The CBS error description that should be printed to the log.
 *
 */
void print_cbs_error(const char *prefix_string_p, CBS_Error_t cbs_error);

/*
 * This function is used to return a pointer to an error string corresponding to the CBS error.
 *
 * @param [in] cbs_error         The CBS error code to be translated to a string error
 *                               description.
 *
 */
const char *str_cbs_error(CBS_Error_t cbs_error);

/*
 * This function is used to convert a MSG_ErrorCode_t to a MSG_Error.
 *
 *
 * @param [in]  MSG_Error        The MSG requester response
 *
 * @param [out] CMS_ErrorCodes_t Converted CMS error code
 *
 *
 */
exe_cms_error_t SMS_ErrorToCMS(SMS_Error_t SMS_Error);


/*
 * This function is used to combine the Service Center packed address
 * and SMS TPDU to form a single pdu
 *
 */
uint8_t smsutil_sms_to_pdu(SMS_SMSC_Address_TPDU_t *sms_address_p, uint8_t *pdu_p);
#endif
