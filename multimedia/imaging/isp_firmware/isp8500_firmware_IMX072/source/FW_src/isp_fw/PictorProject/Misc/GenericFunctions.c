/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \if INCLUDE_IN_HTML_ONLY
 \file      GenericFunctions.c
 \brief     Contains generic utility functions that can be utilised
            throughout the project.
 \ingroup   Miscellaneous
 \endif
*/
#include "GenericFunctions.h"

/**
 \if INCLUDE_IN_HTML_ONLY
 \fn        uint32_t GenericFunctions_CeilingOfMByN(uint32_t u32_Numerator, uint32_t u32_Denominator)
 \brief     Implements a function to return the ceiling of u32_Numerator/u32_Denominator. If
            u32_Numerator/u32_Denominator is an integer, then the same value is returned else
            the next higher integer is returned. Both u32_Numerator and u32_Denominator are positive
            numbers.
 \param     u32_Numerator: numerator
            u32_Denominator: denominator
 \return    uint32_t
 \ingroup   Miscellaneous
 \callgraph
 \callergraph
 \endif
*/
uint32_t
GenericFunctions_CeilingOfMByN(
uint32_t    u32_Numerator,
uint32_t    u32_Denominator)
{
    /*~~~~~~~~~~~~~~~~~~*/
    float_t     f_Value;
    uint32_t    u32_Value;

    /*~~~~~~~~~~~~~~~~~~*/
    f_Value = (float_t) ((float_t) u32_Numerator / (float_t) u32_Denominator);

    u32_Value = GenericFunctions_Ceiling(f_Value);

    return (u32_Value);
}


/**
 \if INCLUDE_IN_HTML_ONLY
 \fn        uint32_t GenericFunctions_Ceiling(float_t f_Value)
 \brief     Implements a function to return the ceiling of an input positive floating point number.
            If f_Value is an integer, then the same value is returned else its next higher integer
            is returned.
 \param     f_Value: floating point positive number
 \return    uint32_t
 \ingroup   Miscellaneous
 \callgraph
 \callergraph
 \endif
*/
uint32_t
GenericFunctions_Ceiling(
float_t f_Value)
{
    /*~~~~~~~~~~~~~~~~~~*/
    uint32_t    u32_Value;

    /*~~~~~~~~~~~~~~~~~~*/
    if (f_Value == (uint32_t) f_Value)
    {
        u32_Value = (uint32_t) f_Value;
    }
    else
    {
        u32_Value = (uint32_t) (f_Value + 1.0);
    }


    return (u32_Value);
}

/**
 \if INCLUDE_IN_HTML_ONLY
 \fn        uint32_t GenericFunctions_Log(uint32_t u32_Operand, uint32_t u32_Base)
 \brief     Implements a function to return int(log(u32_Operand, u32_Base)).
 \param     u32_Operand: Operand for which the log has to be computed
			u32_Base: Specifies the base of the log
 \return    uint32_t
 \ingroup   Miscellaneous
 \callgraph
 \callergraph
 \endif
*/
uint32_t GenericFunctions_Log(uint32_t u32_Operand, uint32_t u32_Base)
{

	uint32_t u32_Value;

	u32_Value = 0;

	if (u32_Base > 1)
	{
		u32_Operand = (uint32_t)(u32_Operand/u32_Base);

		while(u32_Operand >= 1)
		{
			u32_Value += 1;
			u32_Operand = (uint32_t)(u32_Operand/u32_Base);
		}
	}

	return u32_Value;
}

/**
 \if INCLUDE_IN_HTML_ONLY
 \fn        uint32_t GenericFunctions_PowerOfInt(uint32_t base, uint32_t exponent)
 \brief     Implements a function to return power of an integer.
 \param     u32_Base: Operand whose power has to be computed
   			      u32_Exponent: The power to which the base has to be raised
 \return    uint32_t
 \ingroup   Miscellaneous
 \callgraph
 \callergraph
 \endif
*/
uint32_t GenericFunctions_PowerOfInt(uint32_t u32_Base, uint32_t u32_Exponent)
{
  uint32_t u32_Result = 1;

  while (u32_Exponent-- > 0)
  {
    u32_Result *= u32_Base;
  }

  return u32_Result;
}

/**
 \if INCLUDE_IN_HTML_ONLY
 \fn int16_t GetRounded(float_t f_Input)
 \brief Convert a fp16 to a rounded integer
 \details
 \param f_Input: input to convert into integer
 \return int16_t: converted value
 \callgraph
 \callergraph
 \ingroup RgbToYuvCoder
 \endif
*/
int16_t
GetRounded(
float_t f_Input)
{
    /*~~~~~~~~~~~~~~~~~~~*/
    float_t f_Tmp;
    int16_t s16_Output = 0;
    /*~~~~~~~~~~~~~~~~~~~*/

    // dInput += (dInput < 0)? -0.5:0.5;
    // first check for 0.0
    // if ip is zero return 0.
    if (f_Input != 0.0)
    {
        if (f_Input < 0.0)
        {
            // -ve number
            f_Tmp = -0.5;
        }
        else
        {
            // +ve number
            f_Tmp = 0.5;
        }


        s16_Output = (int16_t) (f_Input + f_Tmp);
    }


    return (s16_Output);
}

