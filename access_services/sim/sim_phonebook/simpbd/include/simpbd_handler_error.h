/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Description: Error code translation.
 *
 * Author:  Dag Gullberg <dag.xd.gullberg@stericsson.com>
 */


#ifndef _INCLUSION_GUARD_SIMPB_HANDLER_ERROR_H_
#define _INCLUSION_GUARD_SIMPB_HANDLER_ERROR_H_

#include <stdint.h>

#include "simpb.h"

/* definitions */
#define SIMPBD_SW1SW2_NORMAL_COMPLETION                         (0x9000)

#define SIMPBD_SW1SW2_WARNING_6200_GENERAL_WARNING              (0x6200)
#define SIMPBD_SW1SW2_WARNING_6281_POSSIBLY_CURRUPTED_DATA      (0x6281)
#define SIMPBD_SW1SW2_WARNING_6282_POSSIBLY_CURRUPTED_DATA      (0x6282)
#define SIMPBD_SW1SW2_WARNING_6283_FILE_INVALIDATED             (0x6283)
#define SIMPBD_SW1SW2_WARNING_6285_FILE_IN_TERMINATION          (0x6285)
#define SIMPBD_SW1SW2_WARNING_62F1_MORE_DATA_AVALIABLE          (0x62F1)
#define SIMPBD_SW1SW2_WARNING_62F2_MORE_DATA_AVALIABLE          (0x62F2)
#define SIMPBD_SW1SW2_WARNING_62F3_RESPONSE_DATA_AVAILABLE      (0x62F3)
#define SIMPBD_SW1SW2_WARNING_6300_GENERAL_WARNING              (0x6300)
#define SIMPBD_SW1SW2_WARNING_63F1_MORE_DATA_EXPECTED           (0x63F1)
#define SIMPBD_SW1SW2_WARNING_63F2_MORE_DATA_EXPECTED           (0x63F2)
#define SIMPBD_SW1SW2_ERROR_6581_MEMORY_FAILURE                 (0x6581)
#define SIMPBD_SW1SW2_ERROR_6700_WRONG_LENGTH                   (0x6700)
#define SIMPBD_SW1SW2_ERROR_6982_SECURITY_NOT_SATISFIED         (0x6982)
#define SIMPBD_SW1SW2_ERROR_6983_PIN_BLOCKED                    (0x6983)
#define SIMPBD_SW1SW2_ERROR_6984_DATA_INVALIDATED               (0x6984)
#define SIMPBD_SW1SW2_ERROR_6985_CONDITION_OF_USE               (0x6985)
#define SIMPBD_SW1SW2_ERROR_6A80_WRONG_PARAMETER_DATA           (0x6980)
#define SIMPBD_SW1SW2_ERROR_6A81_FUNCTION_NOT_SUPPORTED         (0x6A81)
#define SIMPBD_SW1SW2_ERROR_6A82_FILE_NOT_FOUND                 (0x6A82)
#define SIMPBD_SW1SW2_ERROR_6A83_RECORD_NOT_FOUND               (0x6A83)
#define SIMPBD_SW1SW2_ERROR_6A84_NO_MEMORY                      (0x6A84)
#define SIMPBD_SW1SW2_ERROR_6A86_WRONG_P1_P2                    (0x6A86)
#define SIMPBD_SW1SW2_ERROR_6A88_DATA_NOT_FOUND                 (0x6A88)

#define SIMPBD_SW1SW2_WARNING_62XX_GENERAL_WARNING              (0x62)
#define SIMPBD_SW1SW2_WARNING_63CX_VERIFICATION_FAILED          (0x63) /* used to get a hold on 0x63CX */
#define SIMPBD_SW1SW2_ERROR_64XX_NVM_UNCHANGED                  (0x64)
#define SIMPBD_SW1SW2_ERROR_65XX_NVM_CHANGED                    (0x65)
#define SIMPBD_SW1SW2_ERROR_66XX_SECURITY_RELATED               (0x66)
#define SIMPBD_SW1SW2_ERROR_68XX_FUNCTION_NOT_SUPPORTED         (0x68)
#define SIMPBD_SW1SW2_ERROR_69XX_COMMAND_NOT_ALLOWED            (0x69)
#define SIMPBD_SW1SW2_ERROR_6AXX_WRONG_PARAMETERS               (0x6A)
#define SIMPBD_SW1SW2_ERROR_6BXX_WRONG_PARAMETERS               (0x6B)
#define SIMPBD_SW1SW2_ERROR_6CXX_WRONG_LENGTH                   (0x6C)
#define SIMPBD_SW1SW2_ERROR_6DXX_INS_NOT_SUPPORTED              (0x6D)
#define SIMPBD_SW1SW2_ERROR_6EXX_CLA_NOT_SUPPORTED              (0x6E)

/* functions */
ste_simpb_iso_t simpbd_translate_status_bytes_to_iso_error(uint8_t sw1, uint8_t sw2);


#endif /* _INCLUSION_GUARD_SIMPB_HANDLER_ERROR_H_ */
