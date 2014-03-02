/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
* \file     algointerface.c
* \brief    Contains Algorithm Library Interface functions
* \author   ST Ericsson
*/

/*
 * Defines
 */
#define _ALGOINTERFACE_C_

/*
 * Includes 
 */
#include "algointerface.h"
#include "algostring.h"
#include "algoutilities.h"
#include "algodebug.h"
#include "algonegativeeffect.h"
#include "algoblackandwhiteeffect.h"
#include "algoautofocuseffect.h"
#include "algosepiaeffect.h"
#include "algowhitebalanceeffect.h"
#include "algospatialsimilareffect.h"
#include "algoembosseffect.h"
#include "algocolortoneeffect.h"
#include "algozoomeffect.h"
#include "algocontrasteffect.h"
#include "algosaturationeffect.h"
#include "algobrightnesseffect.h"
#include "algosolarizeeffect.h"
#include "algogammaeffect.h"
#include "algomirroringeffect.h"
#include "algorotationeffect.h"
#include "algofileroutines.h"
#include "algomemoryroutines.h"
#include "algoextractionroutines.h"
#include "algoconfigfileparser.h"
#include "algotestingmode.h"
#include "algoexposurepreset.h"
#include "algoexposurevalue.h"
#include "algoluminancemean.h"
#include "algocenterfieldofview.h"

TUint8 gConfigFilePath[256] = {0};

void SaveChannelsInFiles(char * fileName, TUint8* data, TUint32 size)
{
	TAny* file = (TAny*)NULL;
	if(fileName)
	{
		file = ALGO_fopen((const char*)fileName, "wb");
		if(file)
		{
			ALGO_fwrite(data, sizeof(TUint8), size, file);
			ALGO_fclose(file);
		}
	}
}

void trace_TAlgoImageParams(const TAlgoImageParams *aImageParams)
{
	ALGO_Log_1("trace aImageParams->iImageWidth = %d\n", aImageParams->iImageWidth);
	ALGO_Log_1("trace aImageParams->iImageHeight = %d\n", aImageParams->iImageHeight);
	ALGO_Log_1("trace aImageParams->iImageStride = %d\n", aImageParams->iImageStride);
	ALGO_Log_1("trace aImageParams->iImageSliceHeight = %d\n", aImageParams->iImageSliceHeight);
	ALGO_Log_1("trace aImageParams->iImageNbStatusLine = %d\n", aImageParams->iImageNbStatusLine);
	ALGO_Log_1("trace aImageParams->iImagePixelOrder = %d\n", aImageParams->iImagePixelOrder);
	ALGO_Log_1("trace aImageParams->iImageHeaderLength = %d\n", aImageParams->iImageHeaderLength);
	switch(aImageParams->iImageFormat)
	{
	case EImageFormatUnknown:ALGO_Log_0("trace aImageParams->iImageFormat = EImageFormatUnknown\n");break;
	case EImageFormat16bitRGB565:ALGO_Log_0("trace aImageParams->iImageFormat = EImageFormat16bitRGB565\n");break;
	case EImageFormat16bitBGR565:ALGO_Log_0("trace aImageParams->iImageFormat = EImageFormat16bitBGR565\n");break;
	case EImageFormat24bitRGB888:ALGO_Log_0("trace aImageParams->iImageFormat = EImageFormat24bitRGB888\n");break;
	case EImageFormat24bitBGR888:ALGO_Log_0("trace aImageParams->iImageFormat = EImageFormat24bitBGR888\n");break;
	case EImageFormat16bitARGB4444:ALGO_Log_0("trace aImageParams->iImageFormat = EImageFormat16bitARGB4444\n");break;
	case EImageFormat16bitARGB1555:ALGO_Log_0("trace aImageParams->iImageFormat = EImageFormat16bitARGB1555\n");break;
	case EImageFormat32bitARGB8888:ALGO_Log_0("trace aImageParams->iImageFormat = EImageFormat32bitARGB8888\n");break;
	case EImageFormatBMP:ALGO_Log_0("trace aImageParams->iImageFormat = EImageFormatBMP\n");break;
	case EImageFormatYUV422Interleaved:ALGO_Log_0("trace aImageParams->iImageFormat = EImageFormatYUV422Interleaved\n");break;
	case EImageFormatYUV420PackedPlanarI420:ALGO_Log_0("trace aImageParams->iImageFormat = EImageFormatYUV420PackedPlanarI420\n");break;
	case EImageFormatYUV420MBSemiPlanarFiltered:ALGO_Log_0("trace aImageParams->iImageFormat = EImageFormatYUV420MBSemiPlanarFiltered\n");break;
	case EImageFormatRAW8:ALGO_Log_0("trace aImageParams->iImageFormat = EImageFormatRAW8\n");break;
	case EImageFormatRAW12:ALGO_Log_0("trace aImageParams->iImageFormat = EImageFormatRAW12\n");break;
	case EImageMaxNb:ALGO_Log_0("trace aImageParams->iImageFormat = EImageMaxNb\n");break;
	default:ALGO_Log_0("trace aImageParams->iImageFormat = default\n");break;
	}
	ALGO_Log_1("trace aImageParams->iBitMaxR = %d\n", aImageParams->iBitMaxR);
	ALGO_Log_1("trace aImageParams->iBitMaxG = %d\n", aImageParams->iBitMaxG);
	ALGO_Log_1("trace aImageParams->iBitMaxB = %d\n", aImageParams->iBitMaxB);
	ALGO_Log_0("\n");
}

void trace_TAlgoMetricParams(const TAlgoMetricParams *aMetricParams)
{
	switch(aMetricParams->iType)
	{
	case ETypeUnknown:ALGO_Log_0("trace aMetricParams->iType = ETypeUnknown\n");break;
	case ETypeNegativeImage:ALGO_Log_0("trace aMetricParams->iType = ETypeNegativeImage\n");break;
	case ETypeBlackAndWhite:ALGO_Log_0("trace aMetricParams->iType = ETypeBlackAndWhite\n");break;
	case ETypeAutoFocus:ALGO_Log_0("trace aMetricParams->iType = ETypeAutoFocus\n");break;
	case ETypeSepia:ALGO_Log_0("trace aMetricParams->iType = ETypeSepia\n");break;
	case ETypeWhiteBalance:ALGO_Log_0("trace aMetricParams->iType = ETypeWhiteBalance\n");break;
	case ETypeSpatialSimilar:ALGO_Log_0("trace aMetricParams->iType = ETypeSpatialSimilar\n");break;
	case ETypeEmboss:ALGO_Log_0("trace aMetricParams->iType = ETypeEmboss\n");break;
	case ETypeSolarize:ALGO_Log_0("trace aMetricParams->iType = ETypeSolarize\n");break;
	case ETypeSolarize_basic:ALGO_Log_0("trace aMetricParams->iType = ETypeSolarize_basic\n");break;
	case ETypeColorTone:ALGO_Log_0("trace aMetricParams->iType = ETypeColorTone\n");break;
	case ETypeZoom:ALGO_Log_0("trace aMetricParams->iType = ETypeZoom\n");break;
	case ETypeContrast:ALGO_Log_0("trace aMetricParams->iType = ETypeContrast\n");break;
	case ETypeSaturation:ALGO_Log_0("trace aMetricParams->iType = ETypeSaturation\n");break;
	case ETypeBrightness:ALGO_Log_0("trace aMetricParams->iType = ETypeBrightness\n");break;
	case ETypeSharpness:ALGO_Log_0("trace aMetricParams->iType = ETypeSharpness\n");break;
	case ETypeExposureValue:ALGO_Log_0("trace aMetricParams->iType = ETypeExposureValue\n");break;
	case ETypeExposurePreset:ALGO_Log_0("trace aMetricParams->iType = ETypeExposurePreset\n");break;    
	case ETypeGamma:ALGO_Log_0("trace aMetricParams->iType = ETypeGamma\n");break;
	case ETypeGamma_basic:ALGO_Log_0("trace aMetricParams->iType = ETypeGamma_basic\n");break;
	case ETypeVerticalMirroring:ALGO_Log_0("trace aMetricParams->iType = ETypeVerticalMirroring\n");break;
	case ETypeHorizontalMirroring:ALGO_Log_0("trace aMetricParams->iType = ETypeHorizontalMirroring\n");break;
	case ETypeBothMirroring:ALGO_Log_0("trace aMetricParams->iType = ETypeBothMirroring\n");break;
	case EType0degreesRotation:ALGO_Log_0("trace aMetricParams->iType = EType0degreesRotation\n");break;
	case EType90degreesRotation:ALGO_Log_0("trace aMetricParams->iType = EType90degreesRotation\n");break;
	case EType180degreesRotation:ALGO_Log_0("trace aMetricParams->iType = EType180degreesRotation\n");break;
	case EType270degreesRotation:ALGO_Log_0("trace aMetricParams->iType = EType270degreesRotation\n");break;
	case ETypeImageContent:ALGO_Log_0("trace aMetricParams->iType = ETypeImageContent\n");break;
	case ETypeLuminanceMean:ALGO_Log_0("trace aMetricParams->iType = ETypeLuminanceMean\n");break;
	case ETypeCenterFieldOfView:ALGO_Log_0("trace aMetricParams->iType = ETypeCenterFieldOfView\n");break;
	case ETypeMaxNb:ALGO_Log_0("trace aMetricParams->iType = ETypeMaxNb\n");break;
	default:ALGO_Log_0("trace aMetricParams->iType = default\n");break;
	}
	ALGO_Log_1("trace aMetricParams->iErrorTolerance.iErrorToleranceR = %f\n", aMetricParams->iErrorTolerance.iErrorToleranceR);
	ALGO_Log_1("trace aMetricParams->iErrorTolerance.iErrorToleranceG = %f\n", aMetricParams->iErrorTolerance.iErrorToleranceG);
	ALGO_Log_1("trace aMetricParams->iErrorTolerance.iErrorToleranceB = %f\n", aMetricParams->iErrorTolerance.iErrorToleranceB);
	ALGO_Log_1("trace aMetricParams->iTestCoordinates.iStartIndexX = %d\n", aMetricParams->iTestCoordinates.iStartIndexX);
	ALGO_Log_1("trace aMetricParams->iTestCoordinates.iStartIndexY = %d\n", aMetricParams->iTestCoordinates.iStartIndexY);
	ALGO_Log_1("trace aMetricParams->iTestCoordinates.iPixelsToGrabX = %d\n", aMetricParams->iTestCoordinates.iPixelsToGrabX);
	ALGO_Log_1("trace aMetricParams->iTestCoordinates.iPixelsToGrabY = %d\n", aMetricParams->iTestCoordinates.iPixelsToGrabY);
	ALGO_Log_1("trace aMetricParams->iParams = 0x%x\n", (int)aMetricParams->iParams);
	ALGO_Log_0("\n");
}


void SaveRGBChannelsInFiles(char * fileName, TAlgoImage *algoImage)
{
	TUint32 memString = 0;
	TUint32 stringLength = 0;
	TUint8 *string = (TUint8 *)NULL;
	if(fileName)
	{
		stringLength = AlgoStrlen((const TUint8 *)fileName);
		memString = (TUint32)AlgoMalloc(stringLength+1+6);
		if(0 == memString){ALGO_Log_2("Error, can not allocate memory : fct %s, line %d\n", __FUNCTION__, __LINE__);return;}
		
		string = (TUint8*)LOS_GetLogicalAddress(memString);
		AlgoMemcpy((TAny *)string, fileName, stringLength);
		((TUint8*)string)[stringLength]='_';
		((TUint8*)string)[stringLength+1]='R';
		((TUint8*)string)[stringLength+2]='.';
		((TUint8*)string)[stringLength+3]='R';
		((TUint8*)string)[stringLength+4]='A';
		((TUint8*)string)[stringLength+5]='W';
		((TUint8*)string)[stringLength+6]=0;
		SaveChannelsInFiles((char*)string, algoImage->iImageData.iRChannel, algoImage->iImageParams.iImageWidth*algoImage->iImageParams.iImageHeight);
		((TUint8*)string)[stringLength+1]='G';
		SaveChannelsInFiles((char*)string, algoImage->iImageData.iGChannel, algoImage->iImageParams.iImageWidth*algoImage->iImageParams.iImageHeight);
		((TUint8*)string)[stringLength+1]='B';
		SaveChannelsInFiles((char*)string, algoImage->iImageData.iBChannel, algoImage->iImageParams.iImageWidth*algoImage->iImageParams.iImageHeight);
		AlgoFree((TAny*) memString);
	}
}


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
					                  const TAlgoMetricParams* aMetricParams )

    {
    TAlgoError ret = EErrorNotSupported;

	trace_TAlgoImageParams(&(aImageEffectDisable->iImageParams));
	trace_TAlgoImageParams(&(aImageEffectEnable->iImageParams));
	trace_TAlgoMetricParams(aMetricParams);

    switch( aMetricParams->iType )
        {
        case ETypeNegativeImage:
            ret = ValidationMetricNegativeEffect(aImageEffectDisable, aImageEffectEnable, aMetricParams );
            break;
                
        case ETypeBlackAndWhite:
            ret = ValidationMetricBlackAndWhiteEffect(aImageEffectDisable, aImageEffectEnable, aMetricParams );
            break;

        case ETypeAutoFocus:
            ret = ValidationMetricAutoFocusEffect(aImageEffectDisable, aImageEffectEnable, aMetricParams );
            break;

         case ETypeSepia:
            ret = ValidationMetricSepiaEffect_Hue(aImageEffectDisable, aImageEffectEnable, aMetricParams );
            break;

        case ETypeWhiteBalance:
            ret = ValidationMetricWhiteBalanceEffect(aImageEffectDisable, aImageEffectEnable, aMetricParams );
            break;

         case ETypeSpatialSimilar:
            ret = ValidationMetricSpatialSimilarEffect(aImageEffectDisable, aImageEffectEnable, aMetricParams );
            break;

        case ETypeEmboss:
            ret = ValidationMetricEmbossEffect(aImageEffectDisable, aImageEffectEnable, aMetricParams );
            break;

        case ETypeColorTone:
            ret = ValidationMetricColorToneEffect(aImageEffectDisable, aImageEffectEnable, aMetricParams );
            break;

		case ETypeZoom:
            ret = ValidationMetricZoomEffect(aImageEffectDisable, aImageEffectEnable, aMetricParams );
            break;

        case ETypeContrast:
            ret = ValidationMetricContrastEffect(aImageEffectDisable, aImageEffectEnable, aMetricParams );
            break;

        case ETypeSaturation:
            ret = ValidationMetricSaturationEffect(aImageEffectDisable, aImageEffectEnable, aMetricParams );
            break;

        case ETypeBrightness:
            ret = ValidationMetricBrightnessEffect(aImageEffectDisable, aImageEffectEnable, aMetricParams );
            break;
        case ETypeSharpness:
        // sharpness uses same metric that for autofocus
            ret = ValidationMetricAutoFocusEffect(aImageEffectDisable, aImageEffectEnable, aMetricParams );
            break;

        case ETypeSolarize:
            ret = ValidationMetricSolarizeEffect(aImageEffectDisable, aImageEffectEnable, aMetricParams );
            break;
        case ETypeSolarize_basic:
            ret = ValidationMetricSolarizeEffect_basic(aImageEffectDisable, aImageEffectEnable, aMetricParams );
            break;

        case ETypeGamma:
            ret = ValidationMetricGammaEffect(aImageEffectDisable, aImageEffectEnable, aMetricParams );
            break;
        case ETypeGamma_basic:
            ret = ValidationMetricGammaEffect_basic(aImageEffectDisable, aImageEffectEnable, aMetricParams );
            break;

		case ETypeHorizontalMirroring:
		case ETypeVerticalMirroring:
		case ETypeBothMirroring:
            ret = ValidationMetricMirroringEffect(aImageEffectDisable, aImageEffectEnable, aMetricParams );
            break;

		case EType0degreesRotation:
		case EType90degreesRotation:
		case EType180degreesRotation:
		case EType270degreesRotation:
            ret = ValidationMetricRotationEffect(aImageEffectDisable, aImageEffectEnable, aMetricParams );
			break;

         case ETypeExposurePreset:
            ret = ValidationMetricExposurePreset(aImageEffectDisable, aImageEffectEnable, aMetricParams );
            break;

         case ETypeExposureValue:
            ret = ValidationMetricExposureValue(aImageEffectDisable, aImageEffectEnable, aMetricParams );
            break;

		 case ETypeImageContent:
            ret = ValidationMetricImageContent(aImageEffectDisable, aImageEffectEnable, aMetricParams );
            break;
  
		case ETypeLuminanceMean:
            ret = ValidationMetricLuminanceMean(aImageEffectDisable, aImageEffectEnable, aMetricParams );
            break;

		case ETypeCenterFieldOfView:
            ret = ValidationMetricCenterFieldOfView(aImageEffectDisable, aImageEffectEnable, aMetricParams );
            break;
  
       default:
            ret = EErrorNotSupported;
        }

    return ret;
    }

/**
 * Function to read the header of the input file and extract the file format specfic information 
 * into TAlgoImageParams structure.
 *
 * @param aFileName                 [const TUint8*] The name of the file.
 * @param aImageParams              [TAlgoImageParams*] The image param structure to be filled with the information.
 *
 * @return                          [TAlgoError] EErrorNone if the header extraction is successful.
 */
TAlgoError ReadFileHeader( const TUint8* aFileName, TAlgoImageParams* aImageParams , TAlgoImageParams* aImageParamsDefault)
{
    TAny* buffer = (TAny*)NULL;
    TAny* file = (TAny*)NULL;
	TUint16 maxHeaderSize = 128;
	TUint8* tempData = (TUint8*)NULL;
	TUint32 tempHeader1[12];
	TUint16 tempHeader2=0;
	TUint32 memBuffer=0;
	TAlgoError ret = EErrorPathNotFound;

	if(aFileName)
	{
		file = ALGO_fopen((const char*)aFileName, "rb");
		if(file)
		    {
            memBuffer = (TUint32) AlgoMalloc(maxHeaderSize);
			if(0 == memBuffer){ALGO_Log_2("Error, can not allocate memory : fct %s, line %d\n", __FUNCTION__, __LINE__);return EErrorNoMemory;}
            if(memBuffer != 0)
			{
				buffer = (TAny*)(LOS_GetLogicalAddress(memBuffer));
                ALGO_fread(buffer, sizeof(TUint8), maxHeaderSize, file);
                ALGO_fclose(file);

                tempData = buffer;
                tempHeader1[0] = (TUint32)(tempData[0]) + (TUint32)(tempData[1]<<8) + 
                                 (TUint32)(tempData[2]<<16) + (TUint32)(tempData[3]<<24);
                tempHeader2 = (TUint32)(tempData[0]) + (TUint32)(tempData[1]<<8);

                if(tempHeader1[0] == 0x444f0006)    
                    {
                    //ALGO_Log_0(" File Format detected is RGB565\n");
					tempData += 4;
					tempData += 4;
					tempHeader1[2] = (TUint32)(tempData[0]) + (TUint32)(tempData[1]<<8) + (TUint32)(tempData[2]<<16) + (TUint32)(tempData[3]<<24);
					tempData += 4;
					tempHeader1[3] = (TUint32)(tempData[0]) + (TUint32)(tempData[1]<<8) + (TUint32)(tempData[2]<<16) + (TUint32)(tempData[3]<<24);

                    aImageParams->iImageWidth = tempHeader1[2];
                    aImageParams->iImageHeight = tempHeader1[3];
					aImageParams->iImageStride =  aImageParams->iImageWidth * 2;
                    aImageParams->iImageSliceHeight = aImageParams->iImageHeight;
                    aImageParams->iImageHeaderLength = 6*4;
					aImageParams->iImageNbStatusLine = 0;
					aImageParams->iImagePixelOrder = 0;
                    aImageParams->iImageFormat = EImageFormat16bitRGB565;
                    aImageParams->iBitMaxR = 5;
                    aImageParams->iBitMaxG = 6;
                    aImageParams->iBitMaxB = 5;
                    
					ret = EErrorNone;
                    }
                else
                    {
                    if (tempHeader2 == 0x4d42)
				        {
                        //ALGO_Log_0(" File Format detected is BMP\n");
                        tempData += 2;
					    tempData += 4;
					    tempData += 4;
					    tempHeader1[2] = (TUint32)(tempData[0]) + (TUint32)(tempData[1]<<8) + (TUint32)(tempData[2]<<16) + (TUint32)(tempData[3]<<24);
					    tempData += 4;
					    tempData += 4;
					    tempHeader1[4] = (TUint32)(tempData[0]) + (TUint32)(tempData[1]<<8) + (TUint32)(tempData[2]<<16) + (TUint32)(tempData[3]<<24);
					    tempData += 4;
					    tempHeader1[5] = (TUint32)(tempData[0]) + (TUint32)(tempData[1]<<8) + (TUint32)(tempData[2]<<16) + (TUint32)(tempData[3]<<24);

                        aImageParams->iImageWidth = tempHeader1[4];
                        aImageParams->iImageHeight = tempHeader1[5];
                        aImageParams->iImageHeaderLength = tempHeader1[2];
						aImageParams->iImageStride =  aImageParams->iImageWidth * 3;
						aImageParams->iImageSliceHeight = aImageParams->iImageHeight;
						aImageParams->iImageNbStatusLine = 0;
						aImageParams->iImagePixelOrder = 0;
                        aImageParams->iImageFormat = EImageFormatBMP;
                        aImageParams->iBitMaxR = 8;
                        aImageParams->iBitMaxG = 8;
                        aImageParams->iBitMaxB = 8;
						
						ret = EErrorNone;
                        }
					else
					{
						if(aImageParamsDefault)
						{
							AlgoMemcpy(aImageParams, aImageParamsDefault, sizeof(TAlgoImageParams));
							/*
							aImageParams->iImageWidth = aImageParamsDefault->iImageWidth ;
							aImageParams->iImageHeight = aImageParamsDefault->iImageHeight;
							aImageParams->iImageStride = aImageParamsDefault->iImageStride;
							aImageParams->iImageSliceHeight = aImageParamsDefault->iImageSliceHeight;
							aImageParams->iImageNbStatusLine = aImageParamsDefault->iImageNbStatusLine;
							aImageParams->iImagePixelOrder = aImageParamsDefault->iImagePixelOrder;
							aImageParams->iImageHeaderLength = aImageParamsDefault->iImageHeaderLength;
							aImageParams->iImageFormat = aImageParamsDefault->iImageFormat;
							aImageParams->iBitMaxR = aImageParamsDefault->iBitMaxR;
							aImageParams->iBitMaxG = aImageParamsDefault->iBitMaxG;
							aImageParams->iBitMaxB = aImageParamsDefault->iBitMaxB;
							*/
							ret = EErrorNone;
						}
						else
						{
							ALGO_Log_0(" Error : no default param\n");
						}
					}
                }
            }
        }
    }

    if(memBuffer)
		AlgoFree((TAny*)memBuffer);

    return ret;
}

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
TAlgoError ReadFileData(const TUint8* aFileName, TAlgoImageParams* aImageParams, TAlgoImage* aImage, TUint32 *aMemR,TUint32 *aMemG, TUint32 *aMemB)
    {
	TAlgoError ret = EErrorPathNotFound;
    TAny* file = (TAny*)NULL;
    TUint8* fileBuffer = (TUint8*)NULL;
    TUint32 fileSize = 0;
	TUint32 memFile=0,memR=0,memG=0,memB=0;
    
    file = ALGO_fopen((const char*)aFileName, "rb");
    if(file)
        {
        switch(aImageParams->iImageFormat)
            {
            case EImageFormat16bitRGB565:
                {
                aImage->iImageParams = *aImageParams;
                //fileSize = (aImage->iImageParams.iImageWidth * aImage->iImageParams.iImageHeight * 2)+ aImage->iImageParams.iImageHeaderLength; 
                fileSize = (aImageParams->iImageStride * aImageParams->iImageSliceHeight)+ aImage->iImageParams.iImageHeaderLength;
                memFile = (TUint32) AlgoMalloc(fileSize);
				if(0 == memFile){ALGO_Log_2("Error, can not allocate memory : fct %s, line %d\n", __FUNCTION__, __LINE__);return EErrorNoMemory;}
				fileBuffer = (TUint8*)(LOS_GetLogicalAddress(memFile));
                
                ALGO_fseek(file, 0, ALGO_SEEK_SET);

                if (ALGO_fread(fileBuffer,sizeof(char),fileSize,file) != fileSize)
                    {
	                return EErrorNotFound;
                    }
                ALGO_fclose(file);

                //Memory allocations
                memR = (TUint32) AlgoMalloc( aImage->iImageParams.iImageWidth * aImage->iImageParams.iImageHeight);
				if(0 == memR){ALGO_Log_2("Error, can not allocate memory : fct %s, line %d\n", __FUNCTION__, __LINE__);return EErrorNoMemory;}
                memG = (TUint32) AlgoMalloc( aImage->iImageParams.iImageWidth * aImage->iImageParams.iImageHeight);
				if(0 == memG){ALGO_Log_2("Error, can not allocate memory : fct %s, line %d\n", __FUNCTION__, __LINE__);return EErrorNoMemory;}
                memB = (TUint32) AlgoMalloc( aImage->iImageParams.iImageWidth * aImage->iImageParams.iImageHeight);
				if(0 == memB){ALGO_Log_2("Error, can not allocate memory : fct %s, line %d\n", __FUNCTION__, __LINE__);return EErrorNoMemory;}

				aImage->iImageData.iRChannel = (TUint8*)(LOS_GetLogicalAddress(memR));
				aImage->iImageData.iGChannel = (TUint8*)(LOS_GetLogicalAddress(memG));
				aImage->iImageData.iBChannel = (TUint8*)(LOS_GetLogicalAddress(memB));

                if ((aImage->iImageData.iRChannel == NULL) || 
                    (aImage->iImageData.iGChannel == NULL) ||
                    (aImage->iImageData.iBChannel == NULL))
                    {
                    return EErrorNoMemory;
                    }
                
                ret = RGB565toRGBPlanes(fileBuffer, &(aImage->iImageData), &(aImage->iImageParams));
				if(memFile)
					AlgoFree((TAny*) memFile);
                }
                break;
            case EImageFormat16bitBGR565:
                {
                aImage->iImageParams = *aImageParams;
                //fileSize = (aImage->iImageParams.iImageWidth * aImage->iImageParams.iImageHeight * 2) + aImage->iImageParams.iImageHeaderLength; 
                fileSize = (aImageParams->iImageStride * aImageParams->iImageSliceHeight)+ aImage->iImageParams.iImageHeaderLength;
                memFile = (TUint32) AlgoMalloc(fileSize);
				if(0 == memFile){ALGO_Log_2("Error, can not allocate memory : fct %s, line %d\n", __FUNCTION__, __LINE__);return EErrorNoMemory;}
				fileBuffer = (TUint8*)(LOS_GetLogicalAddress(memFile));
                
                ALGO_fseek(file, 0, ALGO_SEEK_SET);

                if (ALGO_fread(fileBuffer,sizeof(char),fileSize,file) != fileSize)
                    {
	                return EErrorNotFound;
                    }
                ALGO_fclose(file);

                //Memory allocations
                memR = (TUint32) AlgoMalloc( aImage->iImageParams.iImageWidth * aImage->iImageParams.iImageHeight);
				if(0 == memR){ALGO_Log_2("Error, can not allocate memory : fct %s, line %d\n", __FUNCTION__, __LINE__);return EErrorNoMemory;}
                memG = (TUint32) AlgoMalloc( aImage->iImageParams.iImageWidth * aImage->iImageParams.iImageHeight);
				if(0 == memG){ALGO_Log_2("Error, can not allocate memory : fct %s, line %d\n", __FUNCTION__, __LINE__);return EErrorNoMemory;}
                memB = (TUint32) AlgoMalloc( aImage->iImageParams.iImageWidth * aImage->iImageParams.iImageHeight);
				if(0 == memB){ALGO_Log_2("Error, can not allocate memory : fct %s, line %d\n", __FUNCTION__, __LINE__);return EErrorNoMemory;}

				aImage->iImageData.iRChannel = (TUint8*)(LOS_GetLogicalAddress(memR));
				aImage->iImageData.iGChannel = (TUint8*)(LOS_GetLogicalAddress(memG));
				aImage->iImageData.iBChannel = (TUint8*)(LOS_GetLogicalAddress(memB));

                if ((aImage->iImageData.iRChannel == NULL) || 
                    (aImage->iImageData.iGChannel == NULL) ||
                    (aImage->iImageData.iBChannel == NULL))
                    {
                    return EErrorNoMemory;
                    }
                
                ret = BGR565toRGBPlanes(fileBuffer, &(aImage->iImageData), &(aImage->iImageParams));
				if(memFile)
					AlgoFree((TAny*) memFile);
                }
                break;

            case EImageFormatBMP:
                {
				TUint32 nb_byte_of_padding_by_row = 0;
				if ((aImageParams->iImageWidth * 3) % 4)
				{
					nb_byte_of_padding_by_row = 4 - (aImageParams->iImageWidth * 3) % 4;
				}
                aImage->iImageParams = *aImageParams;
                fileSize = (aImage->iImageParams.iImageWidth *3 + nb_byte_of_padding_by_row) * aImage->iImageParams.iImageHeight
                                                    + aImage->iImageParams.iImageHeaderLength; 
                memFile = (TUint32) AlgoMalloc(fileSize);
				if(0 == memFile){ALGO_Log_2("Error, can not allocate memory : fct %s, line %d\n", __FUNCTION__, __LINE__);return EErrorNoMemory;}
				fileBuffer = (TUint8*)(LOS_GetLogicalAddress(memFile));

                ALGO_fseek(file, 0, ALGO_SEEK_SET);

                if (ALGO_fread(fileBuffer,sizeof(char),fileSize,file) != fileSize)
                    {
                    return EErrorNotFound;
                    }

                ALGO_fclose(file);

                //Memory allocations
                memR = (TUint32) AlgoMalloc( aImage->iImageParams.iImageWidth * aImage->iImageParams.iImageHeight);
				if(0 == memR){ALGO_Log_2("Error, can not allocate memory : fct %s, line %d\n", __FUNCTION__, __LINE__);return EErrorNoMemory;}
                memG = (TUint32) AlgoMalloc( aImage->iImageParams.iImageWidth * aImage->iImageParams.iImageHeight);
				if(0 == memG){ALGO_Log_2("Error, can not allocate memory : fct %s, line %d\n", __FUNCTION__, __LINE__);return EErrorNoMemory;}
                memB = (TUint32) AlgoMalloc( aImage->iImageParams.iImageWidth * aImage->iImageParams.iImageHeight);
				if(0 == memB){ALGO_Log_2("Error, can not allocate memory : fct %s, line %d\n", __FUNCTION__, __LINE__);return EErrorNoMemory;}

				aImage->iImageData.iRChannel = (TUint8*)(LOS_GetLogicalAddress(memR));
				aImage->iImageData.iGChannel = (TUint8*)(LOS_GetLogicalAddress(memG));
				aImage->iImageData.iBChannel = (TUint8*)(LOS_GetLogicalAddress(memB));

                if ((aImage->iImageData.iRChannel == NULL) || 
                    (aImage->iImageData.iGChannel == NULL) ||
                    (aImage->iImageData.iBChannel == NULL))
                    {
                    return EErrorNoMemory;
                    }
                
		        ret = BMP_BGR24toRGBPlanes(fileBuffer, &(aImage->iImageData), &(aImage->iImageParams));
 				if(memFile)
					AlgoFree((TAny*) memFile);
				}
                break;

            case EImageFormatYUV422Interleaved:
                {
                aImage->iImageParams = *aImageParams;
                //fileSize = (aImageParams->iImageWidth * aImageParams->iImageHeight * 2)+ aImage->iImageParams.iImageHeaderLength;
                fileSize = (aImageParams->iImageStride * aImageParams->iImageSliceHeight)+ aImage->iImageParams.iImageHeaderLength;
                memFile = (TUint32) AlgoMalloc(fileSize);
				if(0 == memFile){ALGO_Log_2("Error, can not allocate memory : fct %s, line %d\n", __FUNCTION__, __LINE__);return EErrorNoMemory;}
				fileBuffer = (TUint8*)(LOS_GetLogicalAddress(memFile));

				ALGO_fseek(file, 0, ALGO_SEEK_SET);

				if (ALGO_fread(fileBuffer,sizeof(char),fileSize,file) != fileSize)
					return EErrorNotFound;
				ALGO_fclose(file);

				//Memory allocations
				memR = (TUint32) AlgoMalloc( aImage->iImageParams.iImageWidth * aImage->iImageParams.iImageHeight);
				if(0 == memR){ALGO_Log_2("Error, can not allocate memory : fct %s, line %d\n", __FUNCTION__, __LINE__);return EErrorNoMemory;}
				memG = (TUint32) AlgoMalloc( aImage->iImageParams.iImageWidth * aImage->iImageParams.iImageHeight);
				if(0 == memG){ALGO_Log_2("Error, can not allocate memory : fct %s, line %d\n", __FUNCTION__, __LINE__);return EErrorNoMemory;}
				memB = (TUint32) AlgoMalloc( aImage->iImageParams.iImageWidth * aImage->iImageParams.iImageHeight);
				if(0 == memB){ALGO_Log_2("Error, can not allocate memory : fct %s, line %d\n", __FUNCTION__, __LINE__);return EErrorNoMemory;}

				aImage->iImageData.iRChannel = (TUint8*)(LOS_GetLogicalAddress(memR));
				aImage->iImageData.iGChannel = (TUint8*)(LOS_GetLogicalAddress(memG));
				aImage->iImageData.iBChannel = (TUint8*)(LOS_GetLogicalAddress(memB));

				if ((aImage->iImageData.iRChannel == NULL) || 
				(aImage->iImageData.iGChannel == NULL) ||
				(aImage->iImageData.iBChannel == NULL))
				{
					return EErrorNoMemory;
				}
                else
                {                    
                    ret = YUV422InterleavedtoRGBPlanes(fileBuffer, &(aImage->iImageData), &(aImage->iImageParams));
                 }

                if(memFile)
					AlgoFree((TAny*) memFile);
                }
                break;

            case EImageFormatYUV420PackedPlanarI420:
                {
                aImage->iImageParams = *aImageParams;
#ifdef STRIDE_IMPLEMENTATION_CORRECT
// when camera will set correctly stride for OMX_COLOR_FormatYUV420PackedPlanar, activate next line
                fileSize = (aImageParams->iImageStride * aImageParams->iImageSliceHeight * 3/2)+ aImage->iImageParams.iImageHeaderLength;
#else
                fileSize = (aImageParams->iImageStride * aImageParams->iImageSliceHeight)+ aImage->iImageParams.iImageHeaderLength;
#endif
				memFile = (TUint32) AlgoMalloc(fileSize);
				if(0 == memFile){ALGO_Log_2("Error, can not allocate memory : fct %s, line %d\n", __FUNCTION__, __LINE__);return EErrorNoMemory;}
				fileBuffer = (TUint8*)(LOS_GetLogicalAddress(memFile));

				ALGO_fseek(file, 0, ALGO_SEEK_SET);

				if (ALGO_fread(fileBuffer,sizeof(char),fileSize,file) != fileSize)
					return EErrorNotFound;
				ALGO_fclose(file);

				//Memory allocations
				memR = (TUint32) AlgoMalloc( aImage->iImageParams.iImageWidth * aImage->iImageParams.iImageHeight);
				if(0 == memR){ALGO_Log_2("Error, can not allocate memory : fct %s, line %d\n", __FUNCTION__, __LINE__);return EErrorNoMemory;}
				memG = (TUint32) AlgoMalloc( aImage->iImageParams.iImageWidth * aImage->iImageParams.iImageHeight);
				if(0 == memG){ALGO_Log_2("Error, can not allocate memory : fct %s, line %d\n", __FUNCTION__, __LINE__);return EErrorNoMemory;}
				memB = (TUint32) AlgoMalloc( aImage->iImageParams.iImageWidth * aImage->iImageParams.iImageHeight);
				if(0 == memB){ALGO_Log_2("Error, can not allocate memory : fct %s, line %d\n", __FUNCTION__, __LINE__);return EErrorNoMemory;}

				aImage->iImageData.iRChannel = (TUint8*)(LOS_GetLogicalAddress(memR));
				aImage->iImageData.iGChannel = (TUint8*)(LOS_GetLogicalAddress(memG));
				aImage->iImageData.iBChannel = (TUint8*)(LOS_GetLogicalAddress(memB));

				if ((aImage->iImageData.iRChannel == NULL) || 
				(aImage->iImageData.iGChannel == NULL) ||
				(aImage->iImageData.iBChannel == NULL))
				{
					return EErrorNoMemory;
				}
                else
                {                    
                    ret = YUV420PackedPlanarI420toRGBPlanes(fileBuffer, &(aImage->iImageData), &(aImage->iImageParams));
                 }

                if(memFile)
					AlgoFree((TAny*) memFile);
                }
                break;

            case EImageFormatYUV420MBSemiPlanarFiltered:
			    {
				aImage->iImageParams = *aImageParams;
				//fileSize = (aImage->iImageParams.iImageWidth * aImage->iImageParams.iImageHeight * 3/2) + aImage->iImageParams.iImageHeaderLength; 
                fileSize = (aImageParams->iImageStride * aImageParams->iImageSliceHeight)+ aImage->iImageParams.iImageHeaderLength;
				memFile = (TUint32) AlgoMalloc(fileSize);
				if(0 == memFile){ALGO_Log_2("Error, can not allocate memory : fct %s, line %d\n", __FUNCTION__, __LINE__);return EErrorNoMemory;}
				fileBuffer = (TUint8*)(LOS_GetLogicalAddress(memFile));

				ALGO_fseek(file, 0, ALGO_SEEK_SET);

				if (ALGO_fread(fileBuffer,sizeof(char),fileSize,file) != fileSize)
					return EErrorNotFound;
				ALGO_fclose(file);

				//Memory allocations
				memR = (TUint32) AlgoMalloc( aImage->iImageParams.iImageWidth * aImage->iImageParams.iImageHeight);
				if(0 == memR){ALGO_Log_2("Error, can not allocate memory : fct %s, line %d\n", __FUNCTION__, __LINE__);return EErrorNoMemory;}
				memG = (TUint32) AlgoMalloc( aImage->iImageParams.iImageWidth * aImage->iImageParams.iImageHeight);
				if(0 == memG){ALGO_Log_2("Error, can not allocate memory : fct %s, line %d\n", __FUNCTION__, __LINE__);return EErrorNoMemory;}
				memB = (TUint32) AlgoMalloc( aImage->iImageParams.iImageWidth * aImage->iImageParams.iImageHeight);
				if(0 == memB){ALGO_Log_2("Error, can not allocate memory : fct %s, line %d\n", __FUNCTION__, __LINE__);return EErrorNoMemory;}

				aImage->iImageData.iRChannel = (TUint8*)(LOS_GetLogicalAddress(memR));
				aImage->iImageData.iGChannel = (TUint8*)(LOS_GetLogicalAddress(memG));
				aImage->iImageData.iBChannel = (TUint8*)(LOS_GetLogicalAddress(memB));

				if ((aImage->iImageData.iRChannel == NULL) || 
				(aImage->iImageData.iGChannel == NULL) ||
				(aImage->iImageData.iBChannel == NULL))
				   {
					return EErrorNoMemory;
				   }
                else
				   {
					TUint32 memTempYUV = 0;
					TUint8* tempYUV = (TUint8*)NULL, *inputData = (TUint8*)NULL;
					memTempYUV = (TUint32) AlgoMalloc(aImageParams->iImageStride * aImageParams->iImageSliceHeight);
					if(0 == memTempYUV){ALGO_Log_2("Error, can not allocate memory : fct %s, line %d\n", __FUNCTION__, __LINE__);return EErrorNoMemory;}
					tempYUV = (TUint8*)(LOS_GetLogicalAddress(memTempYUV));
					inputData = fileBuffer + aImage->iImageParams.iImageHeaderLength;
					ret = YUV420MBSemiPlanarFilteredtoYUV420Planar(inputData , tempYUV, &(aImage->iImageParams));
					if(ret == EErrorNone)
					{
#ifdef STRIDE_IMPLEMENTATION_CORRECT
// when camera will set correctly stride for OMX_COLOR_FormatYUV420PackedPlanar, activate next line
						aImageParams->iImageStride = aImageParams->iImageStride * 2/3;
#endif
						ret = YUV420PackedPlanarI420toRGBPlanes(tempYUV, &(aImage->iImageData), &(aImage->iImageParams));
#ifdef STRIDE_IMPLEMENTATION_CORRECT
// when camera will set correctly stride for OMX_COLOR_FormatYUV420PackedPlanar, activate next line
						aImageParams->iImageStride = aImageParams->iImageStride * 3/2;
#endif
				    }
					AlgoFree((TAny*) memTempYUV);

					if(memFile)
						AlgoFree((TAny*) memFile);
					}
			    }
			break;

            case EImageFormatRAW8:
                {
                aImage->iImageParams = *aImageParams;
                //fileSize = (aImageParams->iImageWidth * aImageParams->iImageHeight * 1)+ aImage->iImageParams.iImageHeaderLength;
                fileSize = (aImageParams->iImageStride * aImageParams->iImageSliceHeight)+ aImage->iImageParams.iImageHeaderLength;
                memFile = (TUint32) AlgoMalloc(fileSize);
				if(0 == memFile){ALGO_Log_2("Error, can not allocate memory : fct %s, line %d\n", __FUNCTION__, __LINE__);return EErrorNoMemory;}
				fileBuffer = (TUint8*)(LOS_GetLogicalAddress(memFile));

				ALGO_fseek(file, 0, ALGO_SEEK_SET);

				if (ALGO_fread(fileBuffer,sizeof(char),fileSize,file) != fileSize)
					return EErrorNotFound;
				ALGO_fclose(file);

				//Memory allocations
				memR = (TUint32) AlgoMalloc( aImage->iImageParams.iImageWidth * aImage->iImageParams.iImageHeight);
				if(0 == memR){ALGO_Log_2("Error, can not allocate memory : fct %s, line %d\n", __FUNCTION__, __LINE__);return EErrorNoMemory;}
				memG = (TUint32) AlgoMalloc( aImage->iImageParams.iImageWidth * aImage->iImageParams.iImageHeight);
				if(0 == memG){ALGO_Log_2("Error, can not allocate memory : fct %s, line %d\n", __FUNCTION__, __LINE__);return EErrorNoMemory;}
				memB = (TUint32) AlgoMalloc( aImage->iImageParams.iImageWidth * aImage->iImageParams.iImageHeight);
				if(0 == memB){ALGO_Log_2("Error, can not allocate memory : fct %s, line %d\n", __FUNCTION__, __LINE__);return EErrorNoMemory;}

				aImage->iImageData.iRChannel = (TUint8*)(LOS_GetLogicalAddress(memR));
				aImage->iImageData.iGChannel = (TUint8*)(LOS_GetLogicalAddress(memG));
				aImage->iImageData.iBChannel = (TUint8*)(LOS_GetLogicalAddress(memB));

				if ((aImage->iImageData.iRChannel == NULL) || 
				(aImage->iImageData.iGChannel == NULL) ||
				(aImage->iImageData.iBChannel == NULL))
				{
					return EErrorNoMemory;
				}
                else
                {  
					TUint32 aNbStatusLine = 0;
                    ret = RAW8toRGBPlanes(fileBuffer, &(aImage->iImageData), &(aImage->iImageParams), &aNbStatusLine);
                 }

                if(memFile)
					AlgoFree((TAny*) memFile);
                }
                break;

            case EImageFormatRAW12:
                {
                aImage->iImageParams = *aImageParams;
                //fileSize = (aImageParams->iImageWidth * aImageParams->iImageHeight * 3/2)+ aImage->iImageParams.iImageHeaderLength;
                fileSize = (aImageParams->iImageStride * aImageParams->iImageSliceHeight)+ aImage->iImageParams.iImageHeaderLength;
                memFile = (TUint32) AlgoMalloc(fileSize);
				if(0 == memFile){ALGO_Log_2("Error, can not allocate memory : fct %s, line %d\n", __FUNCTION__, __LINE__);return EErrorNoMemory;}
				fileBuffer = (TUint8*)(LOS_GetLogicalAddress(memFile));

				ALGO_fseek(file, 0, ALGO_SEEK_SET);

				if (ALGO_fread(fileBuffer,sizeof(char),fileSize,file) != fileSize)
					return EErrorNotFound;
				ALGO_fclose(file);

				//Memory allocations
				memR = (TUint32) AlgoMalloc( aImage->iImageParams.iImageWidth * aImage->iImageParams.iImageHeight);
				if(0 == memR){ALGO_Log_2("Error, can not allocate memory : fct %s, line %d\n", __FUNCTION__, __LINE__);return EErrorNoMemory;}
				memG = (TUint32) AlgoMalloc( aImage->iImageParams.iImageWidth * aImage->iImageParams.iImageHeight);
				if(0 == memG){ALGO_Log_2("Error, can not allocate memory : fct %s, line %d\n", __FUNCTION__, __LINE__);return EErrorNoMemory;}
				memB = (TUint32) AlgoMalloc( aImage->iImageParams.iImageWidth * aImage->iImageParams.iImageHeight);
				if(0 == memB){ALGO_Log_2("Error, can not allocate memory : fct %s, line %d\n", __FUNCTION__, __LINE__);return EErrorNoMemory;}

				aImage->iImageData.iRChannel = (TUint8*)(LOS_GetLogicalAddress(memR));
				aImage->iImageData.iGChannel = (TUint8*)(LOS_GetLogicalAddress(memG));
				aImage->iImageData.iBChannel = (TUint8*)(LOS_GetLogicalAddress(memB));

				if ((aImage->iImageData.iRChannel == NULL) || 
				(aImage->iImageData.iGChannel == NULL) ||
				(aImage->iImageData.iBChannel == NULL))
				{
					return EErrorNoMemory;
				}
                else
                {   
					TUint32 aNbStatusLine = 0;
                    ret = RAW12toRGBPlanes(fileBuffer, &(aImage->iImageData), &(aImage->iImageParams), &aNbStatusLine);
                 }

                if(memFile)
					AlgoFree((TAny*) memFile);
                }
                break;

			default:
				ret = EErrorNotSupported;
		}
	}

	return ret;
}

/**
 * Check the Metric parametrs to be used to test the metric. 
 *
 * @param aImageEffectDisable   [const TAlgoImage*] image with Effect Disable.
 * @param aImageEffectEnable    [const TAlgoImage*] image with Effect Enable.
 * @param aMetricParams					[TAlgoMetricParams] The metric parameters to be applied.
 *
 * @return                              [TAlgoError] EErrorNone if the check is passed.
 */
TAlgoError CheckMetricParams( const TAlgoImage* aImageEffectDisable, 
					          const TAlgoImage* aImageEffectEnable,
					          TAlgoMetricParams* aMetricParams )
    {
    if(aMetricParams->iTestCoordinates.iStartIndexX >= aImageEffectDisable->iImageParams.iImageWidth)
        {
        aMetricParams->iTestCoordinates.iStartIndexX  = 0;
        }

    if(aMetricParams->iTestCoordinates.iStartIndexY >= aImageEffectDisable->iImageParams.iImageHeight)
        {
        aMetricParams->iTestCoordinates.iStartIndexY  = 0;
        }

    if(aMetricParams->iTestCoordinates.iPixelsToGrabX > aImageEffectDisable->iImageParams.iImageWidth)
        {
        //aMetricParams->iTestCoordinates.iPixelsToGrabX  = aImageEffectDisable->iImageParams.iImageWidth;
        }

    if(aMetricParams->iTestCoordinates.iPixelsToGrabY > aImageEffectDisable->iImageParams.iImageHeight)
        {
        //aMetricParams->iTestCoordinates.iPixelsToGrabY  = aImageEffectDisable->iImageParams.iImageHeight;
        }    

    return EErrorNone;
    }

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
					                           TAlgoMetricParams* aMetricParams )
    {
    TAlgoError ret = EErrorNone;
    TUint8 checkSpatialSimilar = 0;
    TAlgoType mainAlgorithm = ETypeUnknown;

    mainAlgorithm = aMetricParams->iType;
    GetConfigParams(gConfigFilePath,  aMetricParams, &checkSpatialSimilar);
    if(checkSpatialSimilar == 1)
        {
        aMetricParams->iType = ETypeSpatialSimilar;
        GetConfigParams(gConfigFilePath,  aMetricParams, &checkSpatialSimilar);
        ret = ValidationMetricInterfaceWithFilesRoutine(aImageParamsDefault_1, 
														aImageParamsDefault_2, 
                                                        aFileNameImageEffectDisable, 
                                                        aFileNameImageEffectEnable,
                                                        aMetricParams);
        aMetricParams->iType = mainAlgorithm;
        }
    if(ret == EErrorNone)
        {
        GetConfigParams(gConfigFilePath,  aMetricParams, &checkSpatialSimilar);
        ret = ValidationMetricInterfaceWithFilesRoutine(aImageParamsDefault_1, 
														aImageParamsDefault_2,
                                                        aFileNameImageEffectDisable, 
                                                        aFileNameImageEffectEnable,
                                                        aMetricParams);
        }
    else
        {
        ret = EErrorImageNotRelevant;
        }

    return ret;
    }

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
					                                  TAlgoMetricParams* aMetricParams )
    {
    TAlgoError ret = EErrorNotSupported;
	TUint32 memR_disable=0,memG_disable=0,memB_disable=0;
	TUint32 memR_enable=0,memG_enable=0,memB_enable=0;

    TAlgoImageParams aImageParamEffectDisable;
    TAlgoImageParams aImageParamEffectEnable;

    TAlgoImage aImageEffectDisable;
    TAlgoImage aImageEffectEnable;

	memset(&aImageParamEffectDisable, 0, sizeof(TAlgoImageParams));
 	memset(&aImageParamEffectEnable, 0, sizeof(TAlgoImageParams));
	memcpy(&aImageParamEffectDisable, aImageParamsDefault_1, sizeof(TAlgoImageParams));
 	memcpy(&aImageParamEffectEnable, aImageParamsDefault_2, sizeof(TAlgoImageParams));
	memset(&aImageEffectDisable, 0, sizeof(TAlgoImage));
	memset(&aImageEffectEnable, 0, sizeof(TAlgoImage));
	memcpy(&(aImageEffectDisable.iImageParams), aImageParamsDefault_1, sizeof(TAlgoImageParams));
	memcpy(&(aImageEffectEnable.iImageParams), aImageParamsDefault_2, sizeof(TAlgoImageParams));

    ret = CheckMetricParams((const TAlgoImage*)&aImageEffectDisable, (const TAlgoImage*)&aImageEffectEnable, (TAlgoMetricParams*)aMetricParams);
    if(ret != EErrorNone)
    {
 		ALGO_Log_2("Error : CheckMetricParams, file %s, line %d\n", __FILE__, __LINE__);
		goto end;
	}

    ret = ReadFileHeader( aFileNameImageEffectDisable, &aImageParamEffectDisable, aImageParamsDefault_1);
	if(ret != EErrorNone)
	{
		ALGO_Log_2("Error : ReadFileHeader, file %s, line %d\n", __FILE__, __LINE__);
		goto end;
	}
    ret = ReadFileData(aFileNameImageEffectDisable, &aImageParamEffectDisable, &aImageEffectDisable, &memR_disable, &memG_disable, &memB_disable);
	if(ret != EErrorNone)
	{
		ALGO_Log_2("Error : ReadFileData, file %s, line %d\n", __FILE__, __LINE__);
		goto end;
	}

    ret = ReadFileHeader( aFileNameImageEffectEnable, &aImageParamEffectEnable, aImageParamsDefault_2 );
	if(ret != EErrorNone)
	{
		ALGO_Log_2("Error : ReadFileHeader, file %s, line %d\n", __FILE__, __LINE__);
		goto end;
	}
	ret = ReadFileData(aFileNameImageEffectEnable, &aImageParamEffectEnable, &aImageEffectEnable, &memR_enable, &memG_enable, &memB_enable);
	if(ret != EErrorNone)
	{
		ALGO_Log_2("Error : ReadFileData, file %s, line %d\n", __FILE__, __LINE__);
		goto end;
	}
    
    ret = ValidationMetricInterface(&aImageEffectDisable, &aImageEffectEnable, aMetricParams);

end :

	if(memR_disable){AlgoFree((TAny*) memR_disable);memR_disable=0;}
	if(memG_disable){AlgoFree((TAny*) memG_disable);memG_disable=0;}
	if(memB_disable){AlgoFree((TAny*) memB_disable);memB_disable=0;}
	if(memR_enable){AlgoFree((TAny*) memR_enable);memR_enable=0;}
	if(memG_enable){AlgoFree((TAny*) memG_enable);memG_enable=0;}
	if(memB_enable){AlgoFree((TAny*) memB_enable);memB_enable=0;}

    return ret;
}

/**
 * Validation Metric Interface with buffers to be used. The correct Metric will be validated 
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
					                             TAlgoMetricParams* aMetricParams )
    {
    TAlgoError ret = EErrorNone;
    TUint8 checkSpatialSimilar = 0;
    TAlgoType mainAlgorithm = ETypeUnknown;

    mainAlgorithm = aMetricParams->iType;
    GetConfigParams(gConfigFilePath,  aMetricParams, &checkSpatialSimilar);
    if(checkSpatialSimilar == 1)
        {
        aMetricParams->iType = ETypeSpatialSimilar;
        GetConfigParams(gConfigFilePath,  aMetricParams, &checkSpatialSimilar);
        ret = ValidationMetricInterfaceWithBuffersRoutine(aImageParams, 
														  aImageParams_2,
                                                          bufferEffectDisable, 
                                                          bufferEffectEnable,
                                                          aMetricParams);
        aMetricParams->iType = mainAlgorithm;
        }
    if(ret == EErrorNone)
        {
        GetConfigParams(gConfigFilePath,  aMetricParams, &checkSpatialSimilar);
        ret = ValidationMetricInterfaceWithBuffersRoutine(aImageParams, 
 														  aImageParams_2,
                                                          bufferEffectDisable, 
                                                          bufferEffectEnable,
                                                          aMetricParams);
        }
    else
        {
        ret = EErrorImageNotRelevant;
        }

    return ret;
    }


TAlgoError AllocateAndExtractRGBChannels( TAlgoImageParams* aImageParams,
										 TAlgoImage* aImageEffect,
										 TUint32* memR,
										 TUint32* memG,
										 TUint32* memB,
										 TUint32* memTempYUV,
										 TUint32* aNbStatusLine,
									     TAny* bufferEffect, 
					                     TAlgoMetricParams* aMetricParams )
{
    TAlgoError ret = EErrorNotSupported;
    TUint8* tempYUV=(TUint8*)NULL;

	*memR = (TUint32) AlgoMalloc(aImageParams->iImageWidth * aImageParams->iImageHeight);
	if(0 == *memR){ALGO_Log_2("Error, can not allocate memory : fct %s, line %d\n", __FUNCTION__, __LINE__);ret = EErrorNoMemory;goto end;}
	*memG = (TUint32) AlgoMalloc(aImageParams->iImageWidth * aImageParams->iImageHeight);
	if(0 == *memG){ALGO_Log_2("Error, can not allocate memory : fct %s, line %d\n", __FUNCTION__, __LINE__);ret = EErrorNoMemory;goto end;}
	*memB = (TUint32) AlgoMalloc(aImageParams->iImageWidth * aImageParams->iImageHeight);
	if(0 == *memB){ALGO_Log_2("Error, can not allocate memory : fct %s, line %d\n", __FUNCTION__, __LINE__);ret = EErrorNoMemory;goto end;}
	aImageEffect->iImageData.iRChannel = (TUint8*)(LOS_GetLogicalAddress(*memR));
    aImageEffect->iImageData.iGChannel = (TUint8*)(LOS_GetLogicalAddress(*memG));
    aImageEffect->iImageData.iBChannel = (TUint8*)(LOS_GetLogicalAddress(*memB));

	//ALGO_Log_0(" extract RGB for effect disable\n");
	if(aImageParams->iImageFormat == EImageFormat16bitRGB565)
	{
		ret = RGB565toRGBPlanes((TUint8*) bufferEffect, &aImageEffect->iImageData, aImageParams);
		//SaveRGBChannelsInFiles("backup_from_RGB565_", aImageEffect);
	}
	else if(aImageParams->iImageFormat == EImageFormat24bitRGB888)
	{
		ret = RGB24toRGBPlanes((TUint8*) bufferEffect, &aImageEffect->iImageData, aImageParams);
		//SaveRGBChannelsInFiles("backup_from_RGB24_", aImageEffect);
	}
	else if(aImageParams->iImageFormat == EImageFormat24bitBGR888)
	{
		ret = BGR24toRGBPlanes((TUint8*) bufferEffect, &aImageEffect->iImageData, aImageParams);
		//SaveRGBChannelsInFiles("backup_from_BGR24_", aImageEffect);
	}
	else if(aImageParams->iImageFormat == EImageFormatYUV422Interleaved)
	{
		ret = YUV422InterleavedtoRGBPlanes((TUint8*) bufferEffect, &aImageEffect->iImageData, aImageParams);
		//SaveRGBChannelsInFiles("backup_from_YUV422Interleaved_", aImageEffect);
	}
	else if(aImageParams->iImageFormat == EImageFormatYUV420PackedPlanarI420)
	{
		ret = YUV420PackedPlanarI420toRGBPlanes((TUint8*) bufferEffect, &aImageEffect->iImageData, aImageParams);
		//SaveRGBChannelsInFiles("backup_from_YUV420PackedPlanar_", aImageEffect);
	}
	else if(aImageParams->iImageFormat == EImageFormat16bitARGB4444)
	{
		ret = ARGB4444toRGBPlanes((TUint8*) bufferEffect, &aImageEffect->iImageData, aImageParams);
		//SaveRGBChannelsInFiles("backup_from_ARGB4444_", aImageEffect);
	}
	else if(aImageParams->iImageFormat == EImageFormat16bitARGB1555)
	{
		ret = ARGB1555toRGBPlanes((TUint8*) bufferEffect, &aImageEffect->iImageData, aImageParams);
		//SaveRGBChannelsInFiles("backup_from_ARGB1555", aImageEffect);
	}
	else if(aImageParams->iImageFormat == EImageFormat32bitARGB8888)
	{
		ret = ARGB8888toRGBPlanes((TUint8*) bufferEffect, &aImageEffect->iImageData, aImageParams);
		//SaveRGBChannelsInFiles("backup_from_ARGB8888_", aImageEffect);
	}
	else if(aImageParams->iImageFormat == EImageFormatYUV420MBSemiPlanarFiltered)
	{
		*memTempYUV = (TUint32) AlgoMalloc(aImageParams->iImageStride * aImageParams->iImageSliceHeight);
		if(0 == memTempYUV){ALGO_Log_2("Error, can not allocate memory : fct %s, line %d\n", __FUNCTION__, __LINE__);ret = EErrorNoMemory;goto end;}
		tempYUV = (TUint8*)(LOS_GetLogicalAddress(*memTempYUV));
		ret = YUV420MBSemiPlanarFilteredtoYUV420Planar((TUint8*) bufferEffect, tempYUV, aImageParams);
		if(ret != EErrorNone)
			goto end;
#ifdef STRIDE_IMPLEMENTATION_CORRECT
// when camera will set correctly stride for OMX_COLOR_FormatYUV420PackedPlanar, activate next line
		aImageParams->iImageStride = aImageParams->iImageStride * 2/3;
#endif
		ret = YUV420PackedPlanarI420toRGBPlanes(tempYUV, &aImageEffect->iImageData, aImageParams);
#ifdef STRIDE_IMPLEMENTATION_CORRECT
// when camera will set correctly stride for OMX_COLOR_FormatYUV420PackedPlanar, activate next line
		aImageParams->iImageStride = aImageParams->iImageStride * 3/2;
#endif
		if(memTempYUV){AlgoFree((TAny*) *memTempYUV);*memTempYUV = 0;};
		//SaveRGBChannelsInFiles("backup_from_YUV420MBSemiPlanarFiltered_", aImageEffect);
	}
	else if(aImageParams->iImageFormat == EImageFormatRAW8)
	{
		ret = RAW8toRGBPlanes((TUint8*) bufferEffect, &aImageEffect->iImageData, aImageParams, aNbStatusLine);
		aImageEffect->iImageParams.iImageHeight	-= *aNbStatusLine;
	}
	else if(aImageParams->iImageFormat == EImageFormatRAW12)
	{
		ret = RAW12toRGBPlanes((TUint8*) bufferEffect, &aImageEffect->iImageData, aImageParams, aNbStatusLine);
		aImageEffect->iImageParams.iImageHeight	-= *aNbStatusLine;
	}
	else
	{
		ALGO_Log_1(" ERROR : file format not supported : %d\n", aImageParams->iImageFormat);
	}

	return ret;

end :
	if(*memR){AlgoFree((TAny*) *memR);*memR=0;}
	if(*memG){AlgoFree((TAny*) *memG);*memG=0;}
	if(*memB){AlgoFree((TAny*) *memB);*memB=0;}
	if(*memTempYUV){AlgoFree((TAny*) *memTempYUV);*memTempYUV = 0;};

    return ret;
}

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
					                                    TAlgoMetricParams* aMetricParams )
{
    TAlgoError ret = EErrorNotSupported;
	TAlgoImage aImageEffectDisable;
	TAlgoImage aImageEffectEnable;
	TUint32 memDisR=0,memDisG=0,memDisB=0,memEnR=0,memEnG=0,memEnB=0, memTempYUV=0;
	TUint32 aNbStatusLine = 0;

	AlgoMemcpy(&aImageEffectDisable.iImageParams, aImageParams, sizeof(TAlgoImageParams));
	AlgoMemcpy(&aImageEffectEnable.iImageParams, aImageParams_2, sizeof(TAlgoImageParams));
	ret = CheckMetricParams((const TAlgoImage*)&aImageEffectDisable, (const TAlgoImage*)&aImageEffectEnable, (TAlgoMetricParams*)aMetricParams);
    if(ret != EErrorNone)
    {
 		ALGO_Log_2("Error : CheckMetricParams, file %s, line %d\n", __FILE__, __LINE__);
		goto end;
    }	

	if( aMetricParams->iType == ETypeImageContent)
	{
		ret = ValidationMetricImageContent_AllocationOptimized(aImageParams, bufferEffectDisable, bufferEffectEnable, aMetricParams );
	}
	else
	{ 
		ret = AllocateAndExtractRGBChannels( aImageParams,
											 &aImageEffectDisable,
											 &memDisR,
											 &memDisG,
											 &memDisB,
											 &memTempYUV,
											 &aNbStatusLine,
											 bufferEffectDisable, 
											 aMetricParams);
		if(ret != EErrorNone)
			goto end;

		ret = AllocateAndExtractRGBChannels( aImageParams_2,
											 &aImageEffectEnable,
											 &memEnR,
											 &memEnG,
											 &memEnB,
											 &memTempYUV,
											 &aNbStatusLine,
											 bufferEffectEnable, 
											 aMetricParams);
		if(ret != EErrorNone)
			goto end;

		ret = ValidationMetricInterface(&aImageEffectDisable, &aImageEffectEnable, aMetricParams);
	}

	if(ret != EErrorNone)
	{
		ALGO_Log_0(" ERROR : in metric \n");
		goto end;
	}

end :
	if(memDisR){AlgoFree((TAny*) memDisR);memDisR=0;}
	if(memDisG){AlgoFree((TAny*) memDisG);memDisG=0;}
	if(memDisB){AlgoFree((TAny*) memDisB);memDisB=0;}
	if(memEnR){AlgoFree((TAny*) memEnR);memEnR=0;}
	if(memEnG){AlgoFree((TAny*) memEnG);memEnG=0;}
	if(memEnB){AlgoFree((TAny*) memEnB);memEnB=0;}
	if(memTempYUV){AlgoFree((TAny*) memTempYUV);memTempYUV = 0;};

    return ret;
}

/**
 * Validation Metric Interface convertToBMPandSaveWithBuffers_rgb_extraction.
 *
 * @param aImageParams					[TAlgoImageParams] Image Params.
 * @param bufferInputData           [TAny*] The image buffer with Effect Disable.
 *
 * @return                              [TAlgoError] EErrorNone if the validation is passed.
 */
TAlgoError convertToBMPandSaveWithBuffers_rgb_extraction( TAlgoImageParams* aImageParams,
									                    TAny* bufferInputData,
														TAlgoImage *aImageAlgo,
														TUint32* memDisR,
														TUint32* memDisG,
														TUint32* memDisB
														)
{
    TAlgoError ret = EErrorNotSupported;

	if(!memDisR)
	{ALGO_Log_0(" ERROR : bad paramt memDisR\n");goto end;}
	if(!memDisG)
	{ALGO_Log_0(" ERROR : bad paramt memDisG\n");goto end;}
	if(!memDisB)
	{ALGO_Log_0(" ERROR : bad paramt memDisB\n");goto end;}

	*memDisR=0;
	*memDisG=0;
	*memDisR=0;
	
	AlgoMemcpy(&aImageAlgo->iImageParams, aImageParams, sizeof(TAlgoImageParams));
	*memDisR = (TUint32) AlgoMalloc(aImageParams->iImageWidth * aImageParams->iImageHeight);
	if(0 == *memDisR){ALGO_Log_4("Error, can not allocate memory : width = %d, height = %d,\n fct %s, line %d\n", aImageParams->iImageWidth, aImageParams->iImageHeight, __FUNCTION__, __LINE__);ret = EErrorNoMemory;goto end;}
	*memDisG = (TUint32) AlgoMalloc(aImageParams->iImageWidth * aImageParams->iImageHeight);
	if(0 == *memDisG){ALGO_Log_4("Error, can not allocate memory : width = %d, height = %d,\n fct %s, line %d\n", aImageParams->iImageWidth, aImageParams->iImageHeight,__FUNCTION__, __LINE__);ret = EErrorNoMemory;goto end;}
	*memDisB = (TUint32) AlgoMalloc(aImageParams->iImageWidth * aImageParams->iImageHeight);
	if(0 == *memDisB){ALGO_Log_4("Error, can not allocate memory : width = %d, height = %d,\n fct %s, line %d\n", aImageParams->iImageWidth, aImageParams->iImageHeight,__FUNCTION__, __LINE__);ret = EErrorNoMemory;goto end;}
	aImageAlgo->iImageData.iRChannel = (TUint8*)(LOS_GetLogicalAddress(*memDisR));
    aImageAlgo->iImageData.iGChannel = (TUint8*)(LOS_GetLogicalAddress(*memDisG));
    aImageAlgo->iImageData.iBChannel = (TUint8*)(LOS_GetLogicalAddress(*memDisB));

	if(aImageParams->iImageFormat == EImageFormat16bitRGB565)
	{
		ret = RGB565toRGBPlanes((TUint8*) bufferInputData, &aImageAlgo->iImageData, aImageParams);
	}
	else if(aImageParams->iImageFormat == EImageFormat24bitRGB888)
	{
		ret = RGB24toRGBPlanes((TUint8*) bufferInputData, &aImageAlgo->iImageData, aImageParams);
	}
	else if(aImageParams->iImageFormat == EImageFormat24bitBGR888)
	{
		ret = BGR24toRGBPlanes((TUint8*) bufferInputData, &aImageAlgo->iImageData, aImageParams);
	}
	else if(aImageParams->iImageFormat == EImageFormatYUV422Interleaved)
	{
		ret = YUV422InterleavedtoRGBPlanes((TUint8*) bufferInputData, &aImageAlgo->iImageData, aImageParams);
	}
	else if(aImageParams->iImageFormat == EImageFormatYUV420PackedPlanarI420)
	{
		ret = YUV420PackedPlanarI420toRGBPlanes((TUint8*) bufferInputData, &aImageAlgo->iImageData, aImageParams);
	}
	else if(aImageParams->iImageFormat == EImageFormat16bitARGB4444)
	{
		ret = ARGB4444toRGBPlanes((TUint8*) bufferInputData, &aImageAlgo->iImageData, aImageParams);
	}
	else if(aImageParams->iImageFormat == EImageFormat16bitARGB1555)
	{
		ret = ARGB1555toRGBPlanes((TUint8*) bufferInputData, &aImageAlgo->iImageData, aImageParams);
	}
	else if(aImageParams->iImageFormat == EImageFormat32bitARGB8888)
	{
		ret = ARGB8888toRGBPlanes((TUint8*) bufferInputData, &aImageAlgo->iImageData, aImageParams);
	}
	else if(aImageParams->iImageFormat == EImageFormatYUV420MBSemiPlanarFiltered)
	{
		TUint32 memTempYUV=0;
		TUint8* tempYUV = (TUint8*)NULL;
		memTempYUV = (TUint32) AlgoMalloc(aImageParams->iImageStride * aImageParams->iImageSliceHeight);
		if(0 == memTempYUV){ALGO_Log_4("Error, can not allocate memory : width = %d, height = %d,\n fct %s, line %d\n", aImageParams->iImageWidth, aImageParams->iImageHeight, __FUNCTION__, __LINE__);ret = EErrorNoMemory;goto end;}
		tempYUV = (TUint8*)(LOS_GetLogicalAddress(memTempYUV));
		ret = YUV420MBSemiPlanarFilteredtoYUV420Planar((TUint8*) bufferInputData, tempYUV, aImageParams);
		if(ret != EErrorNone)
		{
			AlgoFree((TAny*) memTempYUV);
			goto end;
		}
#ifdef STRIDE_IMPLEMENTATION_CORRECT
// when camera will set correctly stride for OMX_COLOR_FormatYUV420PackedPlanar, activate next line
		aImageParams->iImageStride = aImageParams->iImageStride * 2/3;
#endif
		ret = YUV420PackedPlanarI420toRGBPlanes(tempYUV, &(aImageAlgo->iImageData), aImageParams);
#ifdef STRIDE_IMPLEMENTATION_CORRECT
// when camera will set correctly stride for OMX_COLOR_FormatYUV420PackedPlanar, activate next line
		aImageParams->iImageStride = aImageParams->iImageStride * 3/2;
#endif
		AlgoFree((TAny*) memTempYUV);
	}
	else if(aImageParams->iImageFormat == EImageFormatRAW8)
	{
		TUint32 aNbStatusLine = 0;
		ret = RAW8toRGBPlanes((TUint8*) bufferInputData, &aImageAlgo->iImageData, aImageParams, &aNbStatusLine);
		aImageParams->iImageHeight -= aNbStatusLine;
		aImageAlgo->iImageParams.iImageHeight -= aNbStatusLine;
	}
	else if(aImageParams->iImageFormat == EImageFormatRAW12)
	{
		TUint32 aNbStatusLine = 0;
		ret = RAW12toRGBPlanes((TUint8*) bufferInputData, &aImageAlgo->iImageData, aImageParams, &aNbStatusLine);
		aImageParams->iImageHeight -= aNbStatusLine;
		aImageAlgo->iImageParams.iImageHeight -= aNbStatusLine;
	}
	else
	{
		ALGO_Log_1(" ERROR : file format not supported : %d\n", aImageParams->iImageFormat);
	}

end:
	return ret;
}



/**
 * Validation Metric Interface convertToBMPandSaveWithBuffers_insert_roi.
 *
 * @param aImageAlgo				
 * @param aROIOBJECTINFOTYPE    
 *
 * @return                             
 */
void convertToBMPandSaveWithBuffers_insert_roi(TAlgoImageParams* aImageParams, TAlgoImage *aImageAlgo,	TAlgoROIOBJECTINFOTYPE* aROIOBJECTINFOTYPE)
{
	if(aROIOBJECTINFOTYPE != NULL)
	{
		TUint32 i = 0;
		TAlgo_RELATIVERECTTYPE *pRELATIVERECTTYPE = (TAlgo_RELATIVERECTTYPE*)NULL;
		TAlgo_RELATIVERECTTYPE newRELATIVERECTTYPE;

		ALGO_Log_0(" creation of ROI \n");
		for(i=0; i< aROIOBJECTINFOTYPE->nNumberOfROIs; i++)
		{
			if(
				(aROIOBJECTINFOTYPE->sROI[i].sReference_nWidth != aImageParams->iImageWidth)||
				(aROIOBJECTINFOTYPE->sROI[i].sReference_nHeight != aImageParams->iImageHeight)
			)
			{
				ALGO_Log_0(" need recalculation of RELATIVERECTTYPE \n");
				ALGO_Log_1("\n before sReference_nWidth = % d \n", aROIOBJECTINFOTYPE->sROI[i].sReference_nWidth);
				ALGO_Log_1(" before sReference_nHeight = % d \n", aROIOBJECTINFOTYPE->sROI[i].sReference_nHeight);
				ALGO_Log_1(" before sReference_nWidth = % d \n", aROIOBJECTINFOTYPE->sROI[i].sReference_nWidth);
				ALGO_Log_1(" before sReference_nHeight = % d \n", aROIOBJECTINFOTYPE->sROI[i].sReference_nHeight);
				ALGO_Log_1(" before sRect_sTopLeft_nX = % d \n", aROIOBJECTINFOTYPE->sROI[i].sRect_sTopLeft_nX);
				ALGO_Log_1(" before sRect_sTopLeft_nY = % d \n", aROIOBJECTINFOTYPE->sROI[i].sRect_sTopLeft_nY);
				AlgoMemcpy(&newRELATIVERECTTYPE, &aROIOBJECTINFOTYPE->sROI[i], sizeof(TAlgo_RELATIVERECTTYPE));
				newRELATIVERECTTYPE.sRect_sTopLeft_nX = newRELATIVERECTTYPE.sRect_sTopLeft_nX * aImageParams->iImageWidth / newRELATIVERECTTYPE.sReference_nWidth;
				newRELATIVERECTTYPE.sRect_sTopLeft_nY = newRELATIVERECTTYPE.sRect_sTopLeft_nY * aImageParams->iImageHeight / newRELATIVERECTTYPE.sReference_nHeight;
				newRELATIVERECTTYPE.sRect_sSize_nWidth = newRELATIVERECTTYPE.sRect_sSize_nWidth * aImageParams->iImageWidth / newRELATIVERECTTYPE.sReference_nWidth;
				newRELATIVERECTTYPE.sRect_sSize_nHeight = newRELATIVERECTTYPE.sRect_sSize_nHeight * aImageParams->iImageHeight / newRELATIVERECTTYPE.sReference_nHeight;
				newRELATIVERECTTYPE.sReference_nWidth = aImageParams->iImageWidth;
				newRELATIVERECTTYPE.sReference_nHeight = aImageParams->iImageHeight;
				ALGO_Log_1("\n after sReference_nWidth = % d \n", newRELATIVERECTTYPE.sReference_nWidth);
				ALGO_Log_1(" after sReference_nHeight = % d \n", newRELATIVERECTTYPE.sReference_nHeight);
				ALGO_Log_1(" after sReference_nWidth = % d \n", newRELATIVERECTTYPE.sReference_nWidth);
				ALGO_Log_1(" after sReference_nHeight = % d \n", newRELATIVERECTTYPE.sReference_nHeight);
				ALGO_Log_1(" after sRect_sTopLeft_nX = % d \n", newRELATIVERECTTYPE.sRect_sTopLeft_nX);
				ALGO_Log_1(" after sRect_sTopLeft_nY = % d \n", newRELATIVERECTTYPE.sRect_sTopLeft_nY);

				pRELATIVERECTTYPE = &newRELATIVERECTTYPE;
			}
			else
			{
				pRELATIVERECTTYPE = &aROIOBJECTINFOTYPE->sROI[i];
			}

			switch(pRELATIVERECTTYPE->eObjectType)
			{
			case EObjectTypeFace:
				writte_rectangle_in_channel( aImageAlgo->iImageData.iRChannel, 255, pRELATIVERECTTYPE);
				writte_rectangle_in_channel( aImageAlgo->iImageData.iGChannel,   0, pRELATIVERECTTYPE);
				writte_rectangle_in_channel( aImageAlgo->iImageData.iBChannel,   0, pRELATIVERECTTYPE);
				break;
			case EObjectTypeObject:
				writte_rectangle_in_channel( aImageAlgo->iImageData.iRChannel,   0, pRELATIVERECTTYPE);
				writte_rectangle_in_channel( aImageAlgo->iImageData.iGChannel,   0, pRELATIVERECTTYPE);
				writte_rectangle_in_channel( aImageAlgo->iImageData.iBChannel, 255, pRELATIVERECTTYPE);
				break;
			case EObjectTypeFocusedArea:
				writte_rectangle_in_channel( aImageAlgo->iImageData.iRChannel,   0, pRELATIVERECTTYPE);
				writte_rectangle_in_channel( aImageAlgo->iImageData.iGChannel, 255, pRELATIVERECTTYPE);
				writte_rectangle_in_channel( aImageAlgo->iImageData.iBChannel,   0, pRELATIVERECTTYPE);
				break;
			default:
				ALGO_Log_1(" unsupported eObjectType %d \n", pRELATIVERECTTYPE->eObjectType);
			}
		}		
	}
}

/**
 * Validation Metric Interface convertToBMPandSaveWithBuffers_create_bmp.
 *
 * @param aImageParams					[TAlgoImageParams] Image Params.
 * @param aImageAlgo          
 * @param aFileNameSavedFile          
 *
 * @return                              [TAlgoError] EErrorNone if the validation is passed.
 */
TAlgoError convertToBMPandSaveWithBuffers_create_bmp( TAlgoImageParams* aImageParams,
										  TAlgoImage *aImageAlgo,
										  const TUint8* aFileNameSavedFile
														)
{
    TAlgoError ret = EErrorNone;
	TUint32 count = 0, count_2 = 0;
	TUint8* BMPData = (TUint8*)NULL;
	TAny* file = (TAny*)NULL;

	if(aFileNameSavedFile)
	{
		file = ALGO_fopen((const char*)aFileNameSavedFile, "wb");
		if(file)
		{
			TUint32 memBMP=0;
			// write BMP header
		   TUint16 header[1];
		   TUint32 file_header[3];
		   TUint32 pict_header[10];
		   TUint8 bmpPadding[3] = {0,0,0};
			TUint32 nb_byte_of_padding_by_row = 0;
			ALGO_Log_1(" creating output file : %s \n", aFileNameSavedFile);
			if ((aImageParams->iImageWidth * 3) % 4)
			{
				nb_byte_of_padding_by_row = 4 - (aImageParams->iImageWidth * 3) % 4;
			}
			ALGO_Log_1(" creating bmp nb_byte_of_padding_by_row %d \n", nb_byte_of_padding_by_row);

		   header[0]=0x4d42;     	// Bitmap Windows
		   ALGO_fwrite((TUint8 *)(header), 2, 1, file);
		   
		   file_header[0]= ((3*aImageParams->iImageWidth + nb_byte_of_padding_by_row) * aImageParams->iImageHeight) + 40 + 14;   // total size of file
		   file_header[1]= 0;		// reserved
		   file_header[2]= 40 + 14;	// picture offset 40+14 ou 14????
		   ALGO_fwrite((TUint8 *)(file_header), 3*4, 1, file);
		   
		   pict_header[0]= 40;     	// Picture header size
		   pict_header[1]= aImageParams->iImageWidth; 	
		   pict_header[2]= aImageParams->iImageHeight; 	
		   pict_header[3]= 0x00180001; 	// color coding 24bit/ plan
		   pict_header[4]= 0;		// compression methode 0: no compression
		   pict_header[5]= (3*aImageParams->iImageWidth+nb_byte_of_padding_by_row)*aImageParams->iImageHeight;   // Picture total size
		   pict_header[6]= 0x0ec4;	// horiz resolution
		   pict_header[7]= 0x0ec4;	// vertical resolution
		   pict_header[8]= 0;		// palette nb of color 
		   pict_header[9]= 0;		// palette nb of important color
		   ALGO_fwrite((TUint8 *)(pict_header), sizeof(TUint32), sizeof(pict_header)/sizeof(TUint32), file);

			// write BMP data (on target problem of not enough memory, so write it by packet)
			memBMP = (TUint32) AlgoMalloc(aImageParams->iImageWidth * 3);
			if(0 == memBMP)
			{
				ALGO_Log_2("Error, can not allocate memory : fct %s, line %d\n", __FUNCTION__, __LINE__);
				ALGO_fclose(file);
				file = (TAny*)NULL;
				ret = EErrorNoMemory;
				goto end;
			}
			else
			{
				BMPData = (TUint8*)(LOS_GetLogicalAddress(memBMP));

				for(count_2 = 0; count_2 < aImageParams->iImageHeight; count_2++)
				{
					for(count = 0; count < aImageParams->iImageWidth; count++)
					{
						// http://fr.wikipedia.org/wiki/Windows_bitmap
						// Si l'image est codée en 24 bits, chaque pixel est codé par un entier 24 bits (RVB), ordre little-endian, c’est-à-dire que les trois octets codent successivement les niveaux de bleu, vert et rouge.
						// http://en.wikipedia.org/wiki/BMP_file_format
						// first data of image correspond to last line... ect..
						BMPData[count*3 + 0] = aImageAlgo->iImageData.iBChannel[(aImageParams->iImageHeight -1 - count_2) * aImageParams->iImageWidth + count]; 
						BMPData[count*3 + 1] = aImageAlgo->iImageData.iGChannel[(aImageParams->iImageHeight -1 - count_2) * aImageParams->iImageWidth + count];
						BMPData[count*3 + 2] = aImageAlgo->iImageData.iRChannel[(aImageParams->iImageHeight -1 - count_2) * aImageParams->iImageWidth + count];
					}
					ALGO_fwrite(BMPData, sizeof(TUint8), aImageParams->iImageWidth * 3, file);
					if(nb_byte_of_padding_by_row)
					{
						ALGO_fwrite(bmpPadding, sizeof(TUint8), nb_byte_of_padding_by_row, file);
					}
				}

				AlgoFree((TAny*) memBMP);
				memBMP = 0;

				ALGO_fclose(file);
				file = (TAny*)NULL;
			}
		}
		else
		{
			ALGO_Log_1(" Error : can not creat output file : %s \n", aFileNameSavedFile);
		}
	}

end :

    return ret;
}

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
														TAlgoROIOBJECTINFOTYPE* aROIOBJECTINFOTYPE)
{
    TAlgoError ret = EErrorNotSupported;
	TAlgoImage aImageAlgo;
	TUint32 memDisR=0,memDisG=0,memDisB=0;

	ret = convertToBMPandSaveWithBuffers_rgb_extraction(aImageParams, bufferInputData, &aImageAlgo, &memDisR,&memDisG,&memDisB);
	if(ret != EErrorNone)
		goto end;

	convertToBMPandSaveWithBuffers_insert_roi(aImageParams, &aImageAlgo, aROIOBJECTINFOTYPE);

	ret = convertToBMPandSaveWithBuffers_create_bmp(aImageParams,&aImageAlgo,aFileNameSavedFile);
	if(ret != EErrorNone)
		goto end;

end :
	if(memDisR){AlgoFree((TAny*) memDisR);memDisR = 0;}
	if(memDisG){AlgoFree((TAny*) memDisG);memDisG = 0;}
	if(memDisB){AlgoFree((TAny*) memDisB);memDisB = 0;}

    return ret;
}

/**
 * Validation Metric Interface convertToBMPandSaveWithBuffers_create_bmp.
 *
 * @param aImageAlgo          
 * @param aFileNameSavedFile          
 *
 * @return                              [TAlgoError] EErrorNone if the validation is passed.
 */
TAlgoError convertToBMPandSaveWithBuffers_create_bmp_with_zoom(
										  TAlgoImage *aImageAlgo,
										  const TUint8* aFileNameSavedFile,
										  TReal32 zoomFactor
														)
{
    TAlgoError ret = EErrorNone;
	TUint32 count = 0, count_2 = 0;
	TUint8* BMPData = (TUint8*)NULL;
	TAny* file = (TAny*)NULL;
	TUint32 newWidth = 0, newHeight = 0;

	if(aFileNameSavedFile)
	{
		file = ALGO_fopen((const char*)aFileNameSavedFile, "wb");
		if(file)
		{
			TUint32 memBMP=0;
			// write BMP header
		   TUint16 header[1];
		   TUint32 file_header[3];
		   TUint32 pict_header[10];
		   TUint8 bmpPadding[3] = {0,0,0};
			TUint32 nb_byte_of_padding_by_row = 0;

			if(zoomFactor < 1.0)
			{
				ret = EErrorNotValidated;
				goto end;
			}
			newWidth = aImageAlgo->iImageParams.iImageWidth / zoomFactor;
			newHeight = aImageAlgo->iImageParams.iImageHeight / zoomFactor;

			if ((newWidth * 3) % 4)
			{
				nb_byte_of_padding_by_row = 4 - (newWidth * 3) % 4;
			}
			ALGO_Log_1(" creating bmp nb_byte_of_padding_by_row %d \n", nb_byte_of_padding_by_row);

		   header[0]=0x4d42;     	// Bitmap Windows
		   ALGO_fwrite((TUint8 *)(header), 2, 1, file);
		   
		   file_header[0]= ((3*newWidth + nb_byte_of_padding_by_row)*newHeight) + 40 + 14;   // total size of file
		   file_header[1]= 0;		// reserved
		   file_header[2]= 40 + 14;	// picture offset 40+14 ou 14????
		   ALGO_fwrite((TUint8 *)(file_header), 3*4, 1, file);
		   
		   pict_header[0]= 40;     	// Picture header size
		   pict_header[1]= newWidth; 	
		   pict_header[2]= newHeight; 	
		   pict_header[3]= 0x00180001; 	// color coding 24bit/ plan
		   pict_header[4]= 0;		// compression methode 0: no compression
		   pict_header[5]= (3*newWidth+nb_byte_of_padding_by_row)*newHeight;   // Picture total size
		   pict_header[6]= 0x0ec4;	// horiz resolution
		   pict_header[7]= 0x0ec4;	// vertical resolution
		   pict_header[8]= 0;		// palette nb of color 
		   pict_header[9]= 0;		// palette nb of important color
		   ALGO_fwrite((TUint8 *)(pict_header), sizeof(TUint32), sizeof(pict_header)/sizeof(TUint32), file);

			// write BMP data (on target problem of not enough memory, so write it by packet)
			memBMP = (TUint32) AlgoMalloc(aImageAlgo->iImageParams.iImageWidth * 3);
			if(0 == memBMP)
			{
				ALGO_Log_2("Error, can not allocate memory : fct %s, line %d\n", __FUNCTION__, __LINE__);
				ALGO_fclose(file);
				file = (TAny*)NULL;
				ret = EErrorNoMemory;
				goto end;
			}
			else
			{
				BMPData = (TUint8*)(LOS_GetLogicalAddress(memBMP));

				for(count_2 = 0; count_2 < aImageAlgo->iImageParams.iImageHeight; count_2++)
				{
					for(count = 0; count < aImageAlgo->iImageParams.iImageWidth; count++)
					{
						// http://fr.wikipedia.org/wiki/Windows_bitmap
						// Si l'image est codée en 24 bits, chaque pixel est codé par un entier 24 bits (RVB), ordre little-endian, c’est-à-dire que les trois octets codent successivement les niveaux de bleu, vert et rouge.
						// http://en.wikipedia.org/wiki/BMP_file_format
						// first data of image correspond to last line... ect..
						BMPData[count*3 + 0] = aImageAlgo->iImageData.iBChannel[(aImageAlgo->iImageParams.iImageHeight -1 - count_2) * aImageAlgo->iImageParams.iImageWidth + count]; 
						BMPData[count*3 + 1] = aImageAlgo->iImageData.iGChannel[(aImageAlgo->iImageParams.iImageHeight -1 - count_2) * aImageAlgo->iImageParams.iImageWidth + count];
						BMPData[count*3 + 2] = aImageAlgo->iImageData.iRChannel[(aImageAlgo->iImageParams.iImageHeight -1 - count_2) * aImageAlgo->iImageParams.iImageWidth + count];
					}
					if(
						(count_2 >= ((aImageAlgo->iImageParams.iImageHeight - newHeight)/2)) &&
						(count_2 < ((aImageAlgo->iImageParams.iImageHeight - newHeight)/2 + newHeight))
						)
					{
						ALGO_fwrite(BMPData+3*((aImageAlgo->iImageParams.iImageWidth - newWidth)/2), sizeof(TUint8), newWidth * 3, file);
						if(nb_byte_of_padding_by_row)
						{
							ALGO_fwrite(bmpPadding, sizeof(TUint8), nb_byte_of_padding_by_row, file);
						}
					}
				}

				AlgoFree((TAny*) memBMP);
				memBMP = 0;

				ALGO_fclose(file);
				file = (TAny*)NULL;
			}
		}
	}

end :

    return ret;
}

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
					                 TReal32 zoomFactor )
{
    TAlgoError ret = EErrorNotSupported;
	TUint32 memR_disable=0,memG_disable=0,memB_disable=0;
    TAlgoImageParams aImageParamEffectDisable;
    TAlgoImage aImageEffectDisable;

	memset(&aImageParamEffectDisable, 0, sizeof(TAlgoImageParams));
	memset(&aImageEffectDisable, 0, sizeof(TAlgoImage));

    ret = ReadFileHeader( aFileNameInput, &aImageParamEffectDisable, (TAlgoImageParams*)NULL);
	if(ret != EErrorNone)
	{
		ALGO_Log_2("Error : ReadFileHeader, file %s, line %d\n", __FILE__, __LINE__);
		goto end;
	}
    ret = ReadFileData(aFileNameInput, &aImageParamEffectDisable, &aImageEffectDisable, &memR_disable, &memG_disable, &memB_disable);
	if(ret != EErrorNone)
	{
		ALGO_Log_2("Error : ReadFileData, file %s, line %d\n", __FILE__, __LINE__);
		goto end;
	}
    
	ret = convertToBMPandSaveWithBuffers_create_bmp_with_zoom(&aImageEffectDisable,aFileNameOutput,zoomFactor);
	if(ret != EErrorNone)
	{
		ALGO_Log_2("Error : convertToBMPandSaveWithBuffers_create_bmp_with_zoom, file %s, line %d\n", __FILE__, __LINE__);
		goto end;
	}

end :

	if(memR_disable){AlgoFree((TAny*) memR_disable);memR_disable=0;}
	if(memG_disable){AlgoFree((TAny*) memG_disable);memG_disable=0;}
	if(memB_disable){AlgoFree((TAny*) memB_disable);memB_disable=0;}

    return ret;
}


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
														TReal32 zoomFactor)
{
    TAlgoError ret = EErrorNotSupported;
	TAlgoImage aImageAlgo;
	TUint32 memDisR=0,memDisG=0,memDisB=0;

	ALGO_Log_2("%s zoomFactor = %f\n", __FUNCTION__, zoomFactor);

	ret = convertToBMPandSaveWithBuffers_rgb_extraction(aImageParams, bufferInputData, &aImageAlgo, &memDisR,&memDisG,&memDisB);
	if(ret != EErrorNone)
		goto end;

	ret = convertToBMPandSaveWithBuffers_create_bmp_with_zoom(&aImageAlgo,aFileNameSavedFile, zoomFactor);
	if(ret != EErrorNone)
		goto end;

end :
	if(memDisR){AlgoFree((TAny*) memDisR);memDisR = 0;}
	if(memDisG){AlgoFree((TAny*) memDisG);memDisG = 0;}
	if(memDisB){AlgoFree((TAny*) memDisB);memDisB = 0;}

    return ret;
}

/**
 * To set the configuration file path.
 *
 * @param aConfigFile					[TUint8*] Configuration File Path.
 *
 * @return                              [TAlgoError] EErrorNone if the Path set correctly.
 */
 TAlgoError SetConfigFilePath( TUint8* aConfigFile )
{
    if( NULL == aConfigFile)
    {
        return EErrorNotFound; 
    }
        
	ALGO_Log_1("SetConfigFilePath : %s\n", aConfigFile);
    AlgoStrcpy( gConfigFilePath, aConfigFile );
    return EErrorNone; 
}
	
