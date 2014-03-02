/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * Defines
 */
#ifdef __cplusplus
extern "C"
{
#endif 

#ifndef __INCLUDE_ALGO_TEST_FILE_PARSER_H__
#define __INCLUDE_ALGO_TEST_FILE_PARSER_H__

#define MAX_TOKENS		1000	
#define MAX_TOKEN_SIZE	200

/*
 * Includes 
 */
#include "algotypedefs.h"
#include "algoerror.h"

TUint8 TokenArray[MAX_TOKENS][MAX_TOKEN_SIZE];

/**
 * Extract tokens from the file
 *
 * @param aFilePointer              [TAny*] File pointer to the opened file.
 *
 * @return                          [TInt32] Number of tokens found.
 */
TInt32 ExtractTokens(TAny* aFilePointer);

/**
 * Run Tests after reading from the script file
 *
 * @param aTestFile                 [TUint8*] Test script file to run the tests.
 *
 * @return                          [TAlgoError] EErrorNone if the test are executed.
 */
TAlgoError RunTest(TUint8* aTestFile);

#endif	//__INCLUDE_ALGO_TEST_FILE_PARSER_H__

#ifdef __cplusplus
}
#endif 
