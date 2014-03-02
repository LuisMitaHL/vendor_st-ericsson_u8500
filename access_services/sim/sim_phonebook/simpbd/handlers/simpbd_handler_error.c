/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Description: Error code translation.
 *
 * Author: Dag Gullberg <dag.xd.gullbermg@stericsson.com>
 */

#include "simpbd_handler_error.h"

ste_simpb_iso_t simpbd_translate_status_bytes_to_iso_error(uint8_t sw1, uint8_t sw2)
{
    uint16_t status_word = (sw1 << 8) | sw2;
    ste_simpb_iso_t result = STE_SIMPB_ISO_ERROR_UNSPECIFIED;

    switch (status_word) {
    case SIMPBD_SW1SW2_NORMAL_COMPLETION:
        result = STE_SIMPB_ISO_NORMAL_COMPLETION;
        break;
    case SIMPBD_SW1SW2_ERROR_6581_MEMORY_FAILURE:
        result = STE_SIMPB_ISO_ERROR_6581_MEMORY_FAILURE;
        break;
    case SIMPBD_SW1SW2_ERROR_6700_WRONG_LENGTH:
        result = STE_SIMPB_ISO_ERROR_6700_WRONG_LENGTH;
        break;
    case SIMPBD_SW1SW2_ERROR_6982_SECURITY_NOT_SATISFIED:
        result = STE_SIMPB_ISO_ERROR_6982_SECURITY_NOT_SATISFIED;
        break;
    case SIMPBD_SW1SW2_ERROR_6983_PIN_BLOCKED:
        result = STE_SIMPB_ISO_ERROR_6983_PIN_BLOCKED;
        break;
    case SIMPBD_SW1SW2_ERROR_6984_DATA_INVALIDATED:
        result = STE_SIMPB_ISO_ERROR_6984_DATA_INVALIDATED;
        break;
    case SIMPBD_SW1SW2_ERROR_6985_CONDITION_OF_USE:
        result = STE_SIMPB_ISO_ERROR_6985_CONDITION_OF_USE;
        break;
    case SIMPBD_SW1SW2_ERROR_6A80_WRONG_PARAMETER_DATA:
        result = STE_SIMPB_ISO_ERROR_6A80_WRONG_PARAMETER_DATA;
        break;
    case SIMPBD_SW1SW2_ERROR_6A81_FUNCTION_NOT_SUPPORTED:
        result = STE_SIMPB_ISO_ERROR_6A81_FUNCTION_NOT_SUPPORTED;
        break;
    case SIMPBD_SW1SW2_ERROR_6A82_FILE_NOT_FOUND:
        result = STE_SIMPB_ISO_ERROR_6A82_FILE_NOT_FOUND;
        break;
    case SIMPBD_SW1SW2_ERROR_6A83_RECORD_NOT_FOUND:
        result = STE_SIMPB_ISO_ERROR_6A83_RECORD_NOT_FOUND;
        break;
    case SIMPBD_SW1SW2_ERROR_6A84_NO_MEMORY:
        result = STE_SIMPB_ISO_ERROR_6A84_NO_MEMORY;
        break;
    case SIMPBD_SW1SW2_ERROR_6A86_WRONG_P1_P2:
        result = STE_SIMPB_ISO_ERROR_6A86_WRONG_P1_P2;
        break;
    case SIMPBD_SW1SW2_ERROR_6A88_DATA_NOT_FOUND:
        result = STE_SIMPB_ISO_ERROR_6A88_DATA_NOT_FOUND;
        break;

    default:
        result = STE_SIMPB_ISO_ERROR_UNSPECIFIED;
        break;
    }

    if (STE_SIMPB_ISO_ERROR_UNSPECIFIED != result) {
        return result;
    }

    switch (sw1) {
    case SIMPBD_SW1SW2_WARNING_62XX_GENERAL_WARNING:
        result = STE_SIMPB_ISO_WARNING_62XX_NVM_UNCHANGED;
        break;
    case SIMPBD_SW1SW2_WARNING_63CX_VERIFICATION_FAILED:

        if (0xC0 == (sw2 & 0xf0)) {
            result = STE_SIMPB_ISO_WARNING_63XX_VERIFICATION_FAILED;
        }

        break;
    case SIMPBD_SW1SW2_ERROR_64XX_NVM_UNCHANGED:
        result = STE_SIMPB_ISO_ERROR_64XX_NVM_UNCHANGED;
        break;
    case SIMPBD_SW1SW2_ERROR_65XX_NVM_CHANGED:
        result = STE_SIMPB_ISO_ERROR_65XX_NVM_CHANGED;
        break;
    case SIMPBD_SW1SW2_ERROR_66XX_SECURITY_RELATED:
        result = STE_SIMPB_ISO_ERROR_66XX_SECURITY_RELATED;
        break;
    case SIMPBD_SW1SW2_ERROR_68XX_FUNCTION_NOT_SUPPORTED:
        result = STE_SIMPB_ISO_ERROR_68XX_FUNCTION_NOT_SUPPORTED;
        break;
    case SIMPBD_SW1SW2_ERROR_69XX_COMMAND_NOT_ALLOWED:
        result = STE_SIMPB_ISO_ERROR_69XX_COMMAND_NOT_ALLOWED;
        break;
    case SIMPBD_SW1SW2_ERROR_6AXX_WRONG_PARAMETERS:
        result = STE_SIMPB_ISO_ERROR_6AXX_WRONG_PARAMETERS;
        break;
    case SIMPBD_SW1SW2_ERROR_6BXX_WRONG_PARAMETERS:
        result = STE_SIMPB_ISO_ERROR_6BXX_WRONG_PARAMETERS;
        break;
    case SIMPBD_SW1SW2_ERROR_6CXX_WRONG_LENGTH:
        result = STE_SIMPB_ISO_ERROR_6CXX_WRONG_LENGTH;
        break;
    case SIMPBD_SW1SW2_ERROR_6DXX_INS_NOT_SUPPORTED:
        result = STE_SIMPB_ISO_ERROR_6DXX_INS_NOT_SUPPORTED;
        break;
    case SIMPBD_SW1SW2_ERROR_6EXX_CLA_NOT_SUPPORTED:
        result = STE_SIMPB_ISO_ERROR_6EXX_CLA_NOT_SUPPORTED;
        break;
    default:
        break;
    }

    return result;
}
