/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * Defines
 */


#ifndef __INCLUDE_ALGO_INTERFACE_ROUTINES_H__
#define __INCLUDE_ALGO_INTERFACE_ROUTINES_H__

/*
 * Includes 
 */
#include "algotypedefs.h"
#include "algoerror.h"

#define NO_EXTENDED_SIZE 0
#define EXTENDED_SIZE 20


#ifdef __cplusplus
extern "C"
{
#endif 

/**
 * Validation Metric Interface to be used. The correct Metric will be validated 
 * with the information in TAlgoMetricParams.
 *
 * @param aImageEffectDisable       [TAlgoImage] The image with Effect Disable.
 * @param aImageEffectEnable        [TAlgoImage] The image with Effect Enable.
 * @param aMetricParams             [TAlgoMetricParams] The metric parameters to be applied.
 *
 * @return                          [TAlgoError] EErrorNone if the validation is passed.
 */
TAlgoError ValidationMetricInterface( const TAlgoImage* aImageEffectDisable, 
					                  const TAlgoImage* aImageEffectEnable,
					                  const TAlgoMetricParams* aMetricParams );

/**
 * Function to read the header of the input file and extract the file format specfic information 
 * into TAlgoImageParams structure.
 *
 * @param aFileName                 [const TUint8*] The name of the file.
 * @param aImageParams              [TAlgoImageParams*] The image param structure to be filled with the information.
 *
 * @return                          [TAlgoError] EErrorNone if the header extraction is successful.
 */
TAlgoError ReadFileHeader( const TUint8* aFileName, TAlgoImageParams* aImageParams, TAlgoImageParams* aImageParamsDefault );

/**
 * Function to read the data of the input file and extract the file data into the TAlgoImage structure.
 *
 * @param aFileName                 [const TUint8*] The name of the file.
 * @param aImageParams              [TAlgoImageParams*] The image param structure with the information of the header.
 * @param aImage                    [TAlgoImage*] The image structure to be filled with the image data.
 * @param aMemR                     [TUint32*] pointer in R data.
 * @param aMemG                     [TUint32*] pointer in G data.
 * @param aMemB                     [TUint32*] pointer in B data.
 *
 * @return                          [TAlgoError] EErrorNone if the header extraction is successful.
 */
TAlgoError ReadFileData(const TUint8* aFileName, TAlgoImageParams* aImageParams, TAlgoImage* aImage, TUint32 *aMemR,TUint32 *aMemG, TUint32 *aMemB);

/**
 * Validation Metric Interface with files to be used. The correct Metric will be validated 
 * with the information in TAlgoMetricParams.
 *
 * @param aFileNameImageEffectDisable   [const TUint8*] The filename of image with Effect Disable.
 * @param aFileNameImageEffectEnable    [const TUint8*] The filename of image with Effect Enable.
 * @param aMetricParams					[TAlgoMetricParams] The metric parameters to be applied.
 *
 * @return                              [TAlgoError] EErrorNone if the validation is passed.
 */
TAlgoError ValidationMetricInterfaceWithFiles( TAlgoImageParams* aImageParamsDefault_1,
											  TAlgoImageParams* aImageParamsDefault_2,
											   const TUint8* aFileNameImageEffectDisable, 
					                           const TUint8* aFileNameImageEffectEnable,
					                           TAlgoMetricParams* aMetricParams );





/**
 * Validation Metric Interface to be used. The correct Metric will be validated 
 * with the information in TAlgoMetricParams.
 *
 * @param aImageParams					[TAlgoImageParams] Image Params.
 * @param bufferEffectDisable           [TAny*] The image buffer with Effect Disable.
 * @param bufferEffectEnable            [TAny*] The image buffer with Effect Enable.
 * @param aMetricParams					[TAlgoMetricParams] The metric parameters to be applied.
 *
 * @return                              [TAlgoError] EErrorNone if the validation is passed.
 */
TAlgoError ValidationMetricInterfaceWithBuffers( TAlgoImageParams* aImageParams,
												TAlgoImageParams* aImageParams_2,
									             TAny* bufferEffectDisable, 
					                             TAny* bufferEffectEnable,
					                             TAlgoMetricParams* aMetricParams );



TAlgoError AllocateAndExtractRGBChannels( TAlgoImageParams* aImageParams,
										 TAlgoImage* aImageEffect,
										 TUint32* memR,
										 TUint32* memG,
										 TUint32* memB,
										 TUint32* memTempYUV,
										 TUint32* aNbStatusLine,
									     TAny* bufferEffect, 
					                     TAlgoMetricParams* aMetricParams );

/**
 * Validation Metric Interface with files main routine to be used. The correct Metric will be validated 
 * with the information in TAlgoMetricParams.
 *
 * @param aFileNameImageEffectDisable   [const TUint8*] The filename of image with Effect Disable.
 * @param aFileNameImageEffectEnable    [const TUint8*] The filename of image with Effect Enable.
 * @param aMetricParams					[TAlgoMetricParams] The metric parameters to be applied.
 *
 * @return                              [TAlgoError] EErrorNone if the validation is passed.
 */
TAlgoError ValidationMetricInterfaceWithFilesRoutine( TAlgoImageParams* aImageParamsDefault_1,
													 TAlgoImageParams* aImageParamsDefault_2,
											          const TUint8* aFileNameImageEffectDisable, 
					                                  const TUint8* aFileNameImageEffectEnable,
					                                  TAlgoMetricParams* aMetricParams );

/**
 * Validation Metric Interface with buffers main routine to be used. The correct Metric will be validated 
 * with the information in TAlgoMetricParams.
 *
 * @param aImageParams					[TAlgoImageParams] Image Params.
 * @param bufferEffectDisable           [TAny*] The image buffer with Effect Disable.
 * @param bufferEffectEnable            [TAny*] The image buffer with Effect Enable.
 * @param aMetricParams					[TAlgoMetricParams] The metric parameters to be applied.
 *
 * @return                              [TAlgoError] EErrorNone if the validation is passed.
 */
TAlgoError ValidationMetricInterfaceWithBuffersRoutine( TAlgoImageParams* aImageParams,
													   TAlgoImageParams* aImageParams_2,
									                    TAny* bufferEffectDisable, 
					                                    TAny* bufferEffectEnable,
					                                    TAlgoMetricParams* aMetricParams );

TAlgoError convertToBMPandSaveWithBuffers_create_bmp( TAlgoImageParams* aImageParams,
										  TAlgoImage *aImageAlgo,
										  const TUint8* aFileNameSavedFile
														);
/**
 * Validation Metric Interface convertToBMPandSaveWithBuffers.
 *
 * @param aImageParams					[TAlgoImageParams] Image Params.
 * @param bufferInputData           [TAny*] The image buffer with Effect Disable.
 * @param aFileNameSavedFile            [const TUint8**] The image buffer with Effect Enable.
 *
 * @return                              [TAlgoError] EErrorNone if the validation is passed.
 */
TAlgoError convertToBMPandSaveWithBuffers( TAlgoImageParams* aImageParams,
									                    TAny* bufferInputData,
														const TUint8* aFileNameSavedFile,
														TAlgoROIOBJECTINFOTYPE* aROIOBJECTINFOTYPE);

/**
 * applyZoomWithFilesRoutine
 *
 * @param aFileNameInput   [const TUint8*] The filename input.
 * @param aFileNameOutput    [const TUint8*] The filename output.
 * @param zoomFactor					[TReal32] zoomFactor.
 *
 * @return                              [TAlgoError] EErrorNone if the validation is passed.
 */
TAlgoError applyZoomWithFilesRoutine(const TUint8* aFileNameInput, 
					                 const TUint8* aFileNameOutput,
					                 TReal32 zoomFactor );

/**
 * Validation Metric Interface convertToBMPandSaveWithBuffers_with_zoom.
 *
 * @param aImageParams					[TAlgoImageParams] Image Params.
 * @param bufferInputData           [TAny*] The image buffer with Effect Disable.
 * @param aFileNameSavedFile            [const TUint8**] The image buffer with Effect Enable.
 * @param zoomFactor            [TReal32] zoomFactor.
 * @return                              [TAlgoError] EErrorNone if the validation is passed.
 */
TAlgoError convertToBMPandSaveWithBuffers_with_zoom( TAlgoImageParams* aImageParams,
									                    TAny* bufferInputData,
														const TUint8* aFileNameSavedFile,
														TReal32 zoomFactor);
/**
 * To set the configuration file path.
 *
 * @param aConfigFile					[TUint8*] Configuration File Path.
 *
 * @return                              [TAlgoError] EErrorNone if the Path set correctly.
 */
TAlgoError SetConfigFilePath( TUint8* aConfigFile );

#ifdef __cplusplus
}
#endif 

#endif	//__INCLUDE_ALGO_INTERFACE_ROUTINES_H__


