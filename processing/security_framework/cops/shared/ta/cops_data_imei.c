/*************************************************************************
 * Copyright ST-Ericsson 2010
 ************************************************************************/
#include <cops_data.h>
#include <cops_common.h>
#include <string.h>

#ifndef COPS_FILE_NUMBER
#define COPS_FILE_NUMBER cops_data_imei_c
#endif

cops_return_code_t
cops_data_get_imei(const cops_data_t *cd, cops_data_imei_t *imei)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_parameter_data_t d;

    d.id = COPS_PARAMETER_ID_IMEI;
    COPS_CHK_RC(cops_data_get_parameter(cd, &d));
    COPS_CHK(d.data != NULL, COPS_RC_DATA_MISSING_ERROR);

    /* assume one or more IMEIs */
    COPS_CHK((d.length % sizeof(*imei)) == 0, COPS_RC_DATA_TAMPERED_ERROR);
    memcpy(imei, d.data, sizeof(*imei));

function_exit:
    return ret_code;
}

cops_return_code_t cops_data_set_imei(cops_data_t *cd, cops_data_imei_t *imei)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_parameter_data_t d;

    d.id = COPS_PARAMETER_ID_IMEI;
    d.data = (uint8_t *)imei;
    d.length = sizeof(*imei);
    COPS_CHK_RC(cops_data_set_parameter(cd, &d));

function_exit:
    return ret_code;
}

void util_cops_unpack_imei(const cops_data_imei_t *packed_imei,
                          cops_imei_t *imei)
{
    uint16_t  PackedIMEI[sizeof(packed_imei->imei) / 2];
    uint32_t  Number;
    int       i;

    /* We copy to make sure the alignment is correct */
    memcpy(PackedIMEI, packed_imei->imei, sizeof(packed_imei->imei));

    /* Type Allocataion Code (TAC, 20 bits) */
    Number = PackedIMEI[0];
    Number |= 0xF0000 & (PackedIMEI[1] << 16);

    for (i = 5; i >= 0; i--) {
        imei->digits[i] = (uint8_t)(Number % 10);
        Number /= 10;
    }

    /* Factory Assembly Code (FAC, 8 bits) */
    Number = 0xFF & (PackedIMEI[1] >> 4);

    for (i = 7; i >= 6; i--) {
        imei->digits[i] = (uint8_t)(Number % 10);
        Number /= 10;
    }

    /* Serial number (SNR, 20 bits) */
    Number = 0x0F & (PackedIMEI[1] >> 12);
    Number |= 0xFFFF0 & (PackedIMEI[2] << 4);

    for (i = 13; i >= 8; i--) {
        imei->digits[i] = (uint8_t)(Number % 10);
        Number /= 10;
    }

    /* Calculate luhn check digit */
    Number = 0;

    for (i = 0; i < 14; i++) {
        uint32_t  Weigth;
        uint32_t  WeightedDigit;

        /*
         * Calculate weigth. Weigth is 1 for even values of n
         * and 2 for odd values of n.
         */
        Weigth = ((uint32_t)i % 2) + 1;

        WeightedDigit = imei->digits[i] * Weigth;
        Number += (WeightedDigit / 10) + (WeightedDigit % 10);
    }

    /*
     * Now we have the sum of the digits according to the luhn algorithm.
     *
     * Now we need to figure out what to add to this sum to make it
     * divisible by 10.
     */

    /* Make it in the range 0..9 */
    Number %= 10;

    /*
     * Calculate what would should need to be added to make it 10
     * and we're done.
     */
    Number = 10 - Number;

    if (Number == 10) {
        /* If the digit is 10 then is should be 0. */
        Number = 0;
    }

    imei->digits[14] = (uint8_t)Number;
}

void util_cops_pack_imei(const cops_imei_t *imei, cops_data_imei_t *packed_imei)
{
    uint16_t  PackedIMEI[sizeof(packed_imei->imei) / 2];
    uint32_t  tac = 0;
    uint8_t   fac = 0;
    uint32_t  srn = 0;

    /* Type Allocataion Code (TAC, 20 bits) */
    tac = imei->digits[0] * 100000 +
          imei->digits[1] * 10000 +
          imei->digits[2] * 1000 +
          imei->digits[3] * 100 + imei->digits[4] * 10 + imei->digits[5] * 1;

    /* Factory Assembly Code (FAC, 8 bits) */
    fac = imei->digits[6] * 10 + imei->digits[7] * 1;

    /* Serial number (SNR, 20 bits) */
    srn = imei->digits[8] * 100000 +
          imei->digits[9] * 10000 +
          imei->digits[10] * 1000 +
          imei->digits[11] * 100 + imei->digits[12] * 10 + imei->digits[13] * 1;

    /* Combine all codes */

    /* Type Allocataion Code (TAC, 20 bits) */
    PackedIMEI[0] = (uint16_t)(0x0FFFF & tac);
    PackedIMEI[1] = (uint16_t)((0xF0000 & tac) >> 16);

    /* Factory Assembly Code (FAC, 8 bits) */
    PackedIMEI[1] |= (uint16_t)((0x000FF & fac) << 4);

    /* Serial number (SNR, 20 bits) */
    PackedIMEI[1] |= (uint16_t)((0x0000F & srn) << 12);
    PackedIMEI[2] = (uint16_t)((0xFFFF0 & srn) >> 4);

    /* We copy to make sure the alignment is correct */
    memcpy(packed_imei->imei, PackedIMEI, sizeof(packed_imei->imei));
}
