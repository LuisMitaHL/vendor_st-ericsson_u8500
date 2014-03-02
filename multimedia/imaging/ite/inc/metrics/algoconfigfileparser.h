/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * Defines
 */

#ifndef __INCLUDE_ALGO_CONFIG_FILE_PARSER_H__
#define __INCLUDE_ALGO_CONFIG_FILE_PARSER_H__

#define CONFIG_MAX_TOKENS		1000	
#define CONFIG_MAX_TOKEN_SIZE	200

/*
 * Includes 
 */
#include "algotypedefs.h"
#include "algoerror.h"




#ifdef __cplusplus
extern "C"
{
#endif 

/**
 * Extract tokens from the file
 *
 * @param aFilePointer              [TAny*] File pointer to the opened file.
 *
 * @return                          [TInt32] Number of tokens found.
 */
TInt32 ExtractConfigTokens(TAny* aFilePointer);

/**
 * Get config values from the metric configuration File.
 *
 * @param aConfigFile               [TUint8*] Configuration file to get the values.
 * @param aMetricParams             [TAlgoMetricParams*] Metric param structure to get the parameter values.
 * @param aCheckSpatialSimilar      [TUint8*] Parameter to get if the SpatialSimilar metric is required to check the images.
 *
 * @return                          [TAlgoError] EErrorNone if the config values are successfully retrieved.
 */
TAlgoError GetConfigParams(TUint8* aConfigFile, TAlgoMetricParams* aMetricParams, TUint8* aCheckSpatialSimilar);

#endif	//__INCLUDE_ALGO_CONFIG_FILE_PARSER_H__

#ifdef __cplusplus
}
#endif 
