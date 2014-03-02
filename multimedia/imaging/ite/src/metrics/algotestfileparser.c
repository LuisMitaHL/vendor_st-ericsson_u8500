/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
* \file     algotestfileparser.c
* \brief    Contains Algorithm Library Test File Parsing Routines
* \author   ST Ericsson
*/

/*
 * Defines
 */

/*
 * Includes 
 */
#include "algotestfileparser.h"
#include "algofileroutines.h"
#include "algoutilities.h"
#include "algostring.h"

/**
 * Extract tokens from the file
 *
 * @param aFilePointer              [TAny*] File pointer to the opened file.
 *
 * @return                          [TInt32] Number of tokens found.
 */
TInt32 ExtractTokens(TAny* aFilePointer)
    {
	// Purpose of this function is to retrieve valid tokens from the file 
	// Valid characters are:- a-z/A-Z, 0-9, <, >, /, \, ", :              
	TInt8 token = ' ';
	TInt32 noInvalidToken = 1;
	
	TInt32 tokenIndex = 0;
	TInt32 charIndex = 0;

	do
	    {
		while(IsSpace(token))
		    {
			//token = fgetc(fp);
            ALGO_fread(&token, 1, 1, aFilePointer);
		    }

		charIndex =0;
		
        // First non space is found
		while(!IsSpace(token))
		    {
			TokenArray[tokenIndex][charIndex++] = token;
			//token = fgetc(fp);
            ALGO_fread(&token, 1, 1, aFilePointer);

			if (token == -1 || (token == '<'))
			    {
				break;
			    }
		    }
		
        TokenArray[tokenIndex][charIndex] = 0;
		tokenIndex++;
	    }while ((token != -1) && (noInvalidToken));

	return tokenIndex;
    }

/**
 * Run Tests after reading from the script file
 *
 * @param aTestFile                 [TUint8*] Test script file to run the tests.
 *
 * @return                          [TAlgoError] EErrorNone if the test are executed.
 */
TAlgoError RunTest(TUint8* aTestFile)
    {
    TAny* filePointer = (TAny*)NULL;

    TInt32 ret = 0;
	TInt32 loop = 0;
	TInt32 index = 0;
    TInt32 tokenIndex = 0;
	TAlgoTestFileParams TestMetric;
	
    filePointer = ALGO_fopen((const char*)aTestFile, "rb");
	if (filePointer == NULL)
	    {
        return EErrorNotFound;
	    }

	tokenIndex = ExtractTokens(filePointer);

	for(index=0; index<tokenIndex; index++)
	    {
	    ret = AlgoStrcmp(TokenArray[index], (TUint8 *)"<Name>");
	    if(ret == 0)
		    {
            AlgoStrcpy(TestMetric.iMetricName, TokenArray[index+1]);
            continue;
		    }

	    ret = AlgoStrcmp(TokenArray[index], (TUint8 *)"<InputFile>");
	    if(ret == 0)
		    {
            AlgoStrcpy(TestMetric.iInputfilename, TokenArray[index+1]);
            continue;
		    }

	    ret = AlgoStrcmp(TokenArray[index], (TUint8 *)"<OutputFile>");
	    if(ret == 0)
		    {
            AlgoStrcpy(TestMetric.iOutputfilename, TokenArray[index+1]);
            continue;
		    }

	    ret = AlgoStrcmp(TokenArray[index], (TUint8 *)"<Param1>");
	    if(ret == 0)
		    {
            //TestMetric.iTestCoordinates.iStartIndexX = TokenArray[index+1];
            continue;
		    }

	    ret = AlgoStrcmp(TokenArray[index], (TUint8 *)"<Param2>");
	    if(ret == 0)
		    {
		    //TestMetric.iTestCoordinates.iStartIndexY = TokenArray[index+1];
            continue;
		    }

        ret = AlgoStrcmp(TokenArray[index], (TUint8 *)"<Param3>");
	    if(ret == 0)
		    {
            //TestMetric.iTestCoordinates.iPixelsToGrabX = TokenArray[index+1];
            continue;
		    }

        ret = AlgoStrcmp(TokenArray[index], (TUint8 *)"<Param4>");
	    if(ret == 0)
		    {
            //TestMetric.iTestCoordinates.iPixelsToGrabY = TokenArray[index+1];
            continue;
		    }

        ret = AlgoStrcmp(TokenArray[index], (TUint8 *)"<Param5>");
	    if(ret == 0)
		    {
            //TestMetric.iErrorTolerance.iErrorToleranceR = TokenArray[index+1];
            continue;
		    }

        ret = AlgoStrcmp(TokenArray[index], (TUint8 *)"<Param6>");
	    if(ret == 0)
		    {
            //TestMetric.iErrorTolerance.iErrorToleranceG = TokenArray[index+1];
            continue;
		    }

        ret = AlgoStrcmp(TokenArray[index], (TUint8 *)"<Param7>");
	    if(ret == 0)
		    {
            //TestMetric.iErrorTolerance.iErrorToleranceB = TokenArray[index+1];
            continue;
		    }

	    ret = AlgoStrcmp(TokenArray[index], (TUint8 *)"</Metric>");
	    if(ret == 0)
		    {
		    ret = AlgoStrcmp(TokenArray[index+1], (TUint8 *)"<Metric>");
		    if(ret == 0)
			    loop++;
		    }

        //ALGO_Log_1("%s \n", TokenArray[index]);
	    }

	//ALGO_Log_1("The Tokens extracted are as follows:%d\n",tokenIndex);

	ALGO_fclose(filePointer);		

    return EErrorNone;
    }

