/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Maximum values for 16-bit and 32-bit float exponents. */
#define MAX_EXPONENT_VALUE_16_BIT (unsigned char)0x3F
#define MAX_EXPONENT_VALUE_32_BIT (unsigned char)0xFF

/* Masks to extract the sign, exponent and mantissa of a 16-bit float. */
#define SIGN_16_BIT_MASK     (unsigned short)0x8000
#define EXPONENT_16_BIT_MASK (unsigned short)0x7E00
#define MANTISSA_16_BIT_MASK (unsigned short)0x01FF

/* Shifts to extract the sign, exponent and mantissa of a 16-bit float. */
#define SIGN_16_BIT_SHIFT     15
#define EXPONENT_16_BIT_SHIFT  9
#define MANTISSA_16_BIT_SHIFT  0

/* Mantissa of the special value NaN in the 16-bit hexadecimal format. */
#define MANTISSA_16_BIT_NAN (unsigned short)0x01FF

/* Masks to extract the sign, exponent and mantissa of a 32-bit float. */
#define SIGN_32_BIT_MASK     (unsigned long)0x80000000
#define EXPONENT_32_BIT_MASK (unsigned long)0x7F800000
#define MANTISSA_32_BIT_MASK (unsigned long)0x007FFFFF

/* Shifts to extract the sign, exponent and mantissa of a 32-bit float. */
#define SIGN_32_BIT_SHIFT     31
#define EXPONENT_32_BIT_SHIFT 23
#define MANTISSA_32_BIT_SHIFT  0

/* Mantissa of the special value NaN in the 32-bit hexadecimal format. */
#define MANTISSA_32_BIT_NAN (unsigned long)0x007FFFFF

/* Shift for renormalizing the mantissa between 16-bit and 32-bit formats. */
#define MANTISSA_RENORMALIZATION_SHIFT 14

/* Bounds which define the range within which a 32-bit float can be represented
   using a 16-bit float format (with a loss of precision for the mantissa). */
#define REPRESENTATION_LOWER_BOUND (unsigned char)0x61
#define REPRESENTATION_UPPER_BOUND (unsigned char)0x9E

void
ITE_ConvertTo32BitFloat(unsigned short fpIn, float* fpOut)
{
    unsigned long* fpAux; /* Necessary to apply the masks on fpOut. */
    unsigned long  mantissa_16_bit;
    unsigned char  sign_16_bit;
    unsigned char  exponent_16_bit;

    fpAux = (unsigned long*)fpOut;

    sign_16_bit
      = (fpIn & SIGN_16_BIT_MASK) >> SIGN_16_BIT_SHIFT;

    //printf("sign_16_bit = 0x%x \n", sign_16_bit);

    exponent_16_bit
      = (fpIn & EXPONENT_16_BIT_MASK) >> EXPONENT_16_BIT_SHIFT;

    //printf("exponent_16_bit = 0x%x \n", exponent_16_bit);

    mantissa_16_bit
      = (fpIn & MANTISSA_16_BIT_MASK) >> MANTISSA_16_BIT_SHIFT;

    //printf("mantissa_16_bit = 0x%lx \n", mantissa_16_bit);

    if (exponent_16_bit == MAX_EXPONENT_VALUE_16_BIT)
    {
        /* Not a number, positive or negative infinity. */
        exponent_16_bit = MAX_EXPONENT_VALUE_32_BIT;

        if (mantissa_16_bit != 0)
        {
            mantissa_16_bit = MANTISSA_32_BIT_NAN;
        }
    }
    else
    {
        if (exponent_16_bit == 0)
        {
            /* Original denormalized numbers are rounded off to zero. */
            sign_16_bit = 0;
            mantissa_16_bit = 0;
        }
        else
        {
            /* Renormalization of the exponent into the 32-bit format. */
            exponent_16_bit += REPRESENTATION_LOWER_BOUND - 1;

            /* Renormalization of the mantissa into the 32-bit format. */
            mantissa_16_bit <<= MANTISSA_RENORMALIZATION_SHIFT;
        }
    }

    //printf("mantissa_16_bit = 0x%lx \n", mantissa_16_bit);

    *fpAux
      = ((sign_16_bit << SIGN_32_BIT_SHIFT) & SIGN_32_BIT_MASK)
      | ((exponent_16_bit << EXPONENT_32_BIT_SHIFT) & EXPONENT_32_BIT_MASK)
      | ((mantissa_16_bit << MANTISSA_32_BIT_SHIFT) & MANTISSA_32_BIT_MASK);

     //printf("*fpAux = 0x%lx \n", *fpAux);
}

void
ITE_ConvertTo16BitFloat(volatile float fpIn, unsigned short* fpOut)
{
    unsigned long* fpAux; /* Necessary to apply the masks on fpIn. */
    unsigned long  mantissa_32_bit;
    unsigned char  sign_32_bit;
    unsigned char  exponent_32_bit;

    fpAux = (unsigned long*)&fpIn;

    sign_32_bit
      = (*fpAux & SIGN_32_BIT_MASK) >> SIGN_32_BIT_SHIFT;

    //printf("sign_32_bit = 0x%x \n", sign_32_bit);

    exponent_32_bit
      = (*fpAux & EXPONENT_32_BIT_MASK) >> EXPONENT_32_BIT_SHIFT;

    //printf("exponent_32_bit = 0x%x \n", exponent_32_bit);

    mantissa_32_bit
      = (*fpAux & MANTISSA_32_BIT_MASK) >> MANTISSA_32_BIT_SHIFT;

    //printf("mantissa_32_bit = 0x%lx \n", mantissa_32_bit);

    if (exponent_32_bit == MAX_EXPONENT_VALUE_32_BIT)
    {
        /* Not a number, positive or negative infinity. */
        exponent_32_bit = MAX_EXPONENT_VALUE_16_BIT;

        if (mantissa_32_bit != 0)
        {
            mantissa_32_bit = MANTISSA_16_BIT_NAN;
        }
    }
    else
    {
        if (exponent_32_bit < REPRESENTATION_LOWER_BOUND)
        {
            /* Original denormalized numbers as well as numbers
               whose exponent in the 32-bit format is inferior
               to the lower bound are rounded off to zero. */
            sign_32_bit = 0;
            exponent_32_bit = 0;
            mantissa_32_bit = 0;
        }
        else
        {
            if (exponent_32_bit > REPRESENTATION_UPPER_BOUND)
            {
                /* Positive or negative infinity. */
                exponent_32_bit = MAX_EXPONENT_VALUE_16_BIT;
                mantissa_32_bit = 0;
            }
            else
            {
                /* Renormalization of the exponent into the 16-bit format. */
                exponent_32_bit -= REPRESENTATION_LOWER_BOUND - 1;

                /* Renormalization of the mantissa into the 16-bit format
                   (approximation is by default). */

                mantissa_32_bit >>= MANTISSA_RENORMALIZATION_SHIFT;
            }
        }
    }

    //printf("mantissa_32_bit = 0x%lx \n", mantissa_32_bit);

    *fpOut
      = ((sign_32_bit << SIGN_16_BIT_SHIFT) & SIGN_16_BIT_MASK)
      | ((exponent_32_bit << EXPONENT_16_BIT_SHIFT) & EXPONENT_16_BIT_MASK)
      | ((mantissa_32_bit << MANTISSA_16_BIT_SHIFT) & MANTISSA_16_BIT_MASK);

    //printf("*fpOut = 0x%x \n", *fpOut);
}


/*
int
main(void)
{
    float f32;
    float fcheck;
    unsigned short f16;

    printf("Rentrer la valeur du flottant sur 32 bits (format decimal) : ");

    scanf("%f", &f32);

    FPAlu_ConvertTo16BitFloat(f32, &f16);

    printf("Valeur du flottant sur 16 bits (format hex.) : 0x%x \n", f16);

    FPAlu_ConvertTo32BitFloat(f16, &fcheck);

    printf("Valeur du flottant sur 32 bits (format decimal) : %.7f \n", fcheck);

    return 0;
}
*/
