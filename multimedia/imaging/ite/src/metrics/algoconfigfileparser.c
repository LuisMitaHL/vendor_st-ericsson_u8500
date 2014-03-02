/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
* \file     algoconfigfileparser.c
* \brief    Contains Algorithm Library Configuration File Parsing Routines
* \author   ST Ericsson
*/

/*
 * Defines
 */

/*
 * Includes 
 */
#include "algoconfigfileparser.h"
#include "algoutilities.h"
#include "algostring.h"
#include "algofileroutines.h"

TUint8 ConfigTokenArray[CONFIG_MAX_TOKENS][CONFIG_MAX_TOKEN_SIZE];

/**
 * Extract tokens from the file
 *
 * @param aFilePointer              [TAny*] File pointer to the opened file.
 *
 * @return                          [TInt32] Number of tokens found.
 */
TInt32 ExtractConfigTokens(TAny* aFilePointer)
    {
    // Purpose of this function is to retrieve valid tokens from the file 
	// Valid characters are:- a-z/A-Z, 0-9, <, >, /, \, ", :              
	TInt8 token = ' ';
	TInt32 noInvalidToken = 1;
	
	TInt32 tokenIndex = 0;
	TInt32 charIndex = 0;
    TInt32 ret = 0;

	do
	    {
        ALGO_fread(&token, 1, 1, aFilePointer);

		while(IsSpace(token) || token == 13)
		    {
            ALGO_fread(&token, 1, 1, aFilePointer);
		    }

        if(token == '!')
            {
            ALGO_fread(&token, 1, 1, aFilePointer);
            while(token != '!')
                {
                ALGO_fread(&token, 1, 1, aFilePointer);
                }
            continue;
            }

		charIndex =0;
		
        // First non space is found
		while(!IsSpace(token) && token !=13)
		    {
            if(token != '<')
                {
			    ConfigTokenArray[tokenIndex][charIndex++] = token;
                }
			//token = fgetc(fp);
            ALGO_fread(&token, 1, 1, aFilePointer);

			if (token == -1 || (token == '>'))
			    {
				break;
			    }
		    }
		
        //ConfigTokenArray[tokenIndex][charIndex] = 0;
        ret = AlgoStrcmp(ConfigTokenArray[tokenIndex], (TUint8 *)"EOF");
        if( ret == 0 )
            {
            break;
            }
        
		tokenIndex++;
	    }while ((token != -1) && (noInvalidToken));


        return tokenIndex;
    }

/**
 * Get config values from the metric configuration File.
 *
 * @param aConfigFile               [TUint8*] Configuration file to get the values.
 * @param aMetricParams             [TAlgoMetricParams*] Metric param structure to get the parameter values.
 * @param aCheckSpatialSimilar      [TUint8*] Parameter to get if the SpatialSimilar metric is required to check the images.
 *
 * @return                          [TAlgoError] EErrorNone if the config values are successfully retrieved.
 */
TAlgoError GetConfigParams(TUint8* aConfigFile, TAlgoMetricParams* aMetricParams, TUint8* aCheckSpatialSimilar)
    {
    TAny* filePointer = (TAny*)NULL;

    TInt32 ret = 0;
	TInt32 index = 0;
    TInt32 tokenIndex = 0;
    TInt8 configCheck = 0;
	
    filePointer = ALGO_fopen((const char*)aConfigFile, "rb");
	if (filePointer == NULL)
	{
		ALGO_Log_1("!CONFIGURATION FILE IS NOT FOUND! : %s\n", aConfigFile);
		return EErrorNone;
	}
	else
	{
		ALGO_Log_1("!CONFIGURATION FILE IS FOUND! : %s\n", aConfigFile);
	}

	tokenIndex = ExtractConfigTokens(filePointer);

    ALGO_fclose(filePointer);

    configCheck = (TInt8)ALGO_ATOF((const TInt8*)ConfigTokenArray[0]);

    if( configCheck == 0 )
        {
        ALGO_Log_0("!CONFIGURATION FILE IS NOT USED!\n");
        return EErrorNone;
        }
    else
        {
        ALGO_Log_0("#CONFIGURATION FILE IS USED#\n");
        }

    switch(aMetricParams->iType)
        {
         case ETypeAutoFocus:
            {
             for(index=0; index<tokenIndex; index++)
	            {
	            ret = AlgoStrcmp(ConfigTokenArray[index], (TUint8 *)"AF");
                if(ret == 0)
                    {
                    break;
                    }
                }
            }
            break;

        case ETypeBlackAndWhite:
            {
             for(index=0; index<tokenIndex; index++)
	            {
	            ret = AlgoStrcmp(ConfigTokenArray[index], (TUint8 *)"BW");
                if(ret == 0)
                    {
                    break;
                    }
                }
            }
            break;

         case ETypeBrightness:
            {
             for(index=0; index<tokenIndex; index++)
	            {
	            ret = AlgoStrcmp(ConfigTokenArray[index], (TUint8 *)"BR");
                if(ret == 0)
                    {
                    break;
                    }
                }
            }
            break;

        case ETypeColorTone:
            {
             for(index=0; index<tokenIndex; index++)
	            {
	            ret = AlgoStrcmp(ConfigTokenArray[index], (TUint8 *)"CT");
                if(ret == 0)
                    {
                    break;
                    }
                }
            }
            break;

        case ETypeContrast:
            {
             for(index=0; index<tokenIndex; index++)
	            {
	            ret = AlgoStrcmp(ConfigTokenArray[index], (TUint8 *)"CR");
                if(ret == 0)
                    {
                    break;
                    }
                }
            }
            break;

        case ETypeEmboss:
            {
             for(index=0; index<tokenIndex; index++)
	            {
	            ret = AlgoStrcmp(ConfigTokenArray[index], (TUint8 *)"EM");
                if(ret == 0)
                    {
                    break;
                    }
                }
            }
            break;

         case ETypeNegativeImage:
            {
             for(index=0; index<tokenIndex; index++)
	            {
	            ret = AlgoStrcmp(ConfigTokenArray[index], (TUint8 *)"NG");
                if(ret == 0)
                    {
                    break;
                    }
                }
            }
            break;

        case ETypeSaturation:
            {
             for(index=0; index<tokenIndex; index++)
	            {
	            ret = AlgoStrcmp(ConfigTokenArray[index], (TUint8 *)"ST");
                if(ret == 0)
                    {
                    break;
                    }
                }
            }
            break;

        case ETypeSepia:
            {
             for(index=0; index<tokenIndex; index++)
	            {
	            ret = AlgoStrcmp(ConfigTokenArray[index], (TUint8 *)"SP");
                if(ret == 0)
                    {
                    break;
                    }
                }
            }
            break;

        case ETypeSolarize:
            {
             for(index=0; index<tokenIndex; index++)
	            {
	            ret = AlgoStrcmp(ConfigTokenArray[index], (TUint8 *)"SL");
                if(ret == 0)
                    {
                    break;
                    }
                }
            }
            break;

        case ETypeSpatialSimilar:
            {
             for(index=0; index<tokenIndex; index++)
	            {
	            ret = AlgoStrcmp(ConfigTokenArray[index], (TUint8 *)"SS");
                if(ret == 0)
                    {
                    break;
                    }
                }
            }
            break;

        case ETypeWhiteBalance:
            {
             for(index=0; index<tokenIndex; index++)
	            {
	            ret = AlgoStrcmp(ConfigTokenArray[index], (TUint8 *)"WB");
                if(ret == 0)
                    {
                    break;
                    }
                }
            }
            break;

        case ETypeZoom:
            {
             for(index=0; index<tokenIndex; index++)
	            {
	            ret = AlgoStrcmp(ConfigTokenArray[index], (TUint8 *)"ZM");
                if(ret == 0)
                    {
                    break;
                    }
                }
            }
            break;

       
        default:
            return EErrorNotFound;
        }

    aMetricParams->iErrorTolerance.iErrorToleranceR = (TReal32)ALGO_ATOF((const TInt8*)ConfigTokenArray[index+2]);	
    aMetricParams->iErrorTolerance.iErrorToleranceG = (TReal32)ALGO_ATOF((const TInt8*)ConfigTokenArray[index+3]);
    aMetricParams->iErrorTolerance.iErrorToleranceB = (TReal32)ALGO_ATOF((const TInt8*)ConfigTokenArray[index+4]);
    aMetricParams->iTestCoordinates.iStartIndexX = (TUint32)ALGO_ATOF((const TInt8*)ConfigTokenArray[index+7]);
    aMetricParams->iTestCoordinates.iStartIndexY = (TUint32)ALGO_ATOF((const TInt8*)ConfigTokenArray[index+8]);
    aMetricParams->iTestCoordinates.iPixelsToGrabX = (TUint32)ALGO_ATOF((const TInt8*)ConfigTokenArray[index+9]);
    aMetricParams->iTestCoordinates.iPixelsToGrabY = (TUint32)ALGO_ATOF((const TInt8*)ConfigTokenArray[index+10]);
    *aCheckSpatialSimilar = (TUint32)ALGO_ATOF((const TInt8*)ConfigTokenArray[index+13]);
    return EErrorNone;
    }
