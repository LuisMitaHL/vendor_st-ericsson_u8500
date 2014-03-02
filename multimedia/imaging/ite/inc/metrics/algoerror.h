/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __INCLUDE_ALGO_ERROR_H__
#define __INCLUDE_ALGO_ERROR_H__

#include "algodebug.h"

/** 
 * An enum TAlgoError.
 * Errors Enumerations For Return Type in Algorithm Library. 
 */
typedef enum
    {
    EErrorNone              = 0,        /**< enum value EErrorNone  -   No Error. */
    EErrorGeneral           = -500,     /**< enum value EErrorGeneral  -   General Error in enviornment. */
    EErrorNotFound          = -501,     /**< enum value EErrorNotFound  -   Pointer is NULL. */
    EErrorNoMemory          = -502,     /**< enum value EErrorNoMemory  -   Memory not available. */ 
    EErrorArgument          = -503,     /**< enum value EErrorArgument  -   Input Argument not valid. */
    EErrorNotSupported      = -504,     /**< enum value EErrorNotSupported  -   Value not supported. */
    EErrorAlreadyExists     = -505,     /**< enum value EErrorAlreadyExists  -   Value already exists. */
    EErrorPathNotFound      = -506,     /**< enum value EErrorPathNotFound  -   Path not found. */
    EErrorNotValidated      = -507,     /**< enum value EErrorNotValidated  -   Algo Not Validated. */
    EErrorImageNotRelevant  = -508      /**< enum value EErrorImageNotRelevant  -   Image Not Relevant for the Metric Validation. */

    }TAlgoError;

#ifdef __cplusplus
extern "C"
{
#endif 

#ifdef _ALGOINTERFACE_C_
#define Alog_Extern
#else
#define Alog_Extern extern
#endif

Alog_Extern void Metrcis_Print_Result(TAlgoError error);

#ifdef _ALGOINTERFACE_C_
Alog_Extern void Metrcis_Print_Result(TAlgoError error)
{
	if(error == EErrorNone)
	{
		ALGO_Log_0("=================== TEST PASSED :) ================= \n\n");
	}
	else
	{
		ALGO_Log_0("=================== TEST FAILED :( ================= \n\n");
	}

}
#endif

#ifdef __cplusplus
}
#endif 

#endif	//__INCLUDE_ALGOERROR_H__


