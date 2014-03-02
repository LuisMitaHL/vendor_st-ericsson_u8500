/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include "algodebug.h"
#include "algotypedefs.h"
#include "algomemoryroutines.h"
#include "algoextractionroutines.h"
#include "algoutilities.h"
#include "algostring.h"

#ifndef WIN32
#include "algointerface.h"
#else

#ifdef IMAGING_INTEGRATION_ARBO
#include "algointerface.h"
#include "algofileroutines.h"
#else
#include ".\AlgoValidationModules\Interface\algointerface.h"
#endif

#endif

#include <stdio.h>
#include <string.h>

//Auto Focus Metrices Test.
TInt32 test_metric_autofocus(char *aFileNameImageEffectDisable, char *aFileNameImageEffectEnable);
TInt32 test_metric_autofocus_line(char *aFileNameImageTop, char *aFileNameImageMiddle, char *aFileNameImageBottom);
TInt32 test_metric_autofocus_colon(char *aFileNameImageLeft, char *aFileNameImageMiddle, char *aFileNameImageRight);
TInt32 test_metric_autofocus_with_spatial_test(TUint8* aDirPath);

//White Balance Metrices Test.
TInt32 test_metric_whitebalance(const TUint8* aFileNameImageEffectDisable, 
					            const TUint8* aFileNameImageEffectEnable,
					            const TAlgoMetricParams* aMetricParams );
TInt32 test_metric_whitebalance_characterization(TUint8* aDirPath);

//Black and White Metrices Test
TInt32 test_metric_blackandwhite_characterization(TUint8* aDirPath);

//Negative Metrices Test
TInt32 test_metric_negative_characterization(TUint8* aDirPath);

//Sepia Metrices Test
TInt32 test_metric_sepia_characterization(TUint8* aDirPath);

//Emboss Metrices Test
TInt32 test_metric_emboss_characterization(TUint8* aDirPath);

//Spatial Similar Metrices Test
TInt32 test_metric_spatialsimilar(char *aFileNameImageEffectDisable, char *aFileNameImageEffectEnable);
TInt32 test_metric_spatialsimilar_characterization(TUint8* aDirPath);

//Zoom Metrices Test
TInt32 test_metric_zoom_characterization(TUint8* aDirPath);

//Color Tone Metrices Test
TInt32 test_metric_colortone_characterization(TUint8* aDirPath);

//Contrast Metrices Test
TInt32 test_metric_contrast_characterization(TUint8* aDirPath);

//Saturation Metrices Test
TInt32 test_metric_saturation_characterization(TUint8* aDirPath);

//Brightness Metrices Test
TInt32 test_metric_brightness_characterization(TUint8* aDirPath);

//Solarize Metrices Test
TInt32 test_metric_solarize_characterization(TUint8* aDirPath);

//Gamma Metrices Test
TInt32 test_metric_gamma_characterization(TUint8* aDirPath);

//Mirroring Metrices Test
TInt32 test_metric_mirroring_characterization(TUint8* aDirPath);
//Rotation Metrices Test
TInt32 test_metric_rotation_characterization(TUint8* aDirPath);

//centerfieldofview Metrices Test
TInt32 test_metric_centerfieldofview_characterization(TUint8* aDirPath);



void getParamFromFileName(char* inputFileName, TAlgoImageFormat *inputFormat, TUint32 *width, TUint32 *height, TUint32 *stride, TUint32 *sliceheight, TUint32 *id, TUint32 *frame, TUint32 *headerLine, TUint32 *pixelOrder)
{
	TInt8 *pHandle = (TInt8 *)NULL;
	*inputFormat = EImageFormatUnknown;
	*width = 0;
	*height = 0;
	*stride = 0;
	*sliceheight = 0;
	*id = 0;
	*frame = 0;
	*headerLine = 0;
	*pixelOrder = 0;

	if(strstr(inputFileName, "format_rgb565"))
	{
		*inputFormat = EImageFormat16bitRGB565;
		ALGO_Log_0("detected format EImageFormat16bitRGB565\n");
	}
	if(strstr(inputFileName, "format_bgr565"))
	{
		*inputFormat = EImageFormat16bitBGR565;
		ALGO_Log_0("detected format EImageFormat16bitBGR565\n");
	}
	if(strstr(inputFileName, "format_rgb888"))
	{
		*inputFormat = EImageFormat24bitRGB888;
		ALGO_Log_0("detected format EImageFormat24bitRGB888\n");
	}
	if(strstr(inputFileName, "format_bgr888"))
	{
		*inputFormat = EImageFormat24bitBGR888;
		ALGO_Log_0("detected format EImageFormat24bitBGR888\n");
	}
	if(strstr(inputFileName, "format_yuv420mbpackedsemiplanar"))
	{
		*inputFormat = EImageFormatYUV420MBSemiPlanarFiltered;
		ALGO_Log_0("detected format EImageFormatYUV420MBSemiPlanarFiltered\n");
	}
	if(strstr(inputFileName, "format_yuv420packedplanari420"))
	{
		*inputFormat = EImageFormatYUV420PackedPlanarI420;
		ALGO_Log_0("detected format format_yuv420packedplanari420\n");
	}
	if(strstr(inputFileName, "format_yuv422interleaved"))
	{
		*inputFormat = EImageFormatYUV422Interleaved;
		ALGO_Log_0("detected format EImageFormatYUV422Interleaved\n");
	}
	if(strstr(inputFileName, "format_RawBayer8bit"))
	{
		*inputFormat = EImageFormatRAW8;
		ALGO_Log_0("detected format EImageFormatRAW8\n");
	}
	if(strstr(inputFileName, "format_RawBayer12bit"))
	{
		*inputFormat = EImageFormatRAW12;
		ALGO_Log_0("detected format EImageFormatRAW12\n");
	}
	if(*inputFormat == EImageFormatUnknown)
	{
		ALGO_Log_0("inputFormat not recognazied\n");
	}

	pHandle = strstr(inputFileName, "_reso_");
	if(pHandle)
	{
		pHandle += strlen("_reso_");
		sscanf (pHandle,"%d_x_%d", width, height);
		ALGO_Log_2 ("detected width %d and height %d\n", *width, *height);
	}
	else
	{
		ALGO_Log_0("Resolution not recognazied\n");
	}

	pHandle = strstr(inputFileName, "stride_");
	if(pHandle)
	{
		pHandle += strlen("stride_");
		sscanf (pHandle,"%d", stride);
		ALGO_Log_1 ("detected stride %d\n", *stride);
	}
	else
	{
		ALGO_Log_0("stride not recognazied\n");
	}

	pHandle = strstr(inputFileName, "_sh_");
	if(pHandle)
	{
		pHandle += strlen("_sh_");
		sscanf (pHandle,"%d", sliceheight);
		ALGO_Log_1 ("detected sliceheight %d\n", *sliceheight);
	}
	else
	{
		ALGO_Log_0("sliceheight not recognazied\n");
		*sliceheight = *height;
		ALGO_Log_0("=> sliceheight = height\n");
	}

	pHandle = strstr(inputFileName, "frame_");
	if(pHandle)
	{
		pHandle += strlen("frame_");
		sscanf (pHandle,"%d", frame);
		ALGO_Log_1 ("detected frame %d\n", *frame);
	}
	else
	{
		ALGO_Log_0("frame not recognazied\n");
	}
	
	pHandle = strstr(inputFileName, "_po_");
	if(pHandle)
	{
		pHandle += strlen("_po_");
		sscanf (pHandle,"%d", pixelOrder);
		ALGO_Log_1 ("detected pixelOrder %d\n", *pixelOrder);
	}
	else
	{
		ALGO_Log_0("pixelOrder not recognazied\n");
	}

	pHandle = strstr(inputFileName, "_hl_");
	if(pHandle)
	{
		pHandle += strlen("_hl_");
		sscanf (pHandle,"%d", headerLine);
		ALGO_Log_1 ("detected headerLine %d\n", *headerLine);
	}
	else
	{
		ALGO_Log_0("headerLine not recognazied\n");
	}
}


void getParamValueFromFileName(char* inputFileName, char* prefixe_before_value, TUint32 *value)
{
	TInt8 *pHandle = (TInt8 *)NULL;
	*value = 0;

	pHandle = strstr(inputFileName, prefixe_before_value);
	if(pHandle)
	{
		pHandle += strlen(prefixe_before_value);
		sscanf (pHandle,"%d_", value);
		ALGO_Log_1 ("detected value %d\n", *value);
	}
	else
	{
		ALGO_Log_0("Value not recognazied\n");
	}
}

void getParamValue2FromFileName(char* inputFileName, char* prefixe_before_value, TUint32 *value, TUint32 *value2)
{
	TInt8 *pHandle = (TInt8 *)NULL;
	*value = 0;

	pHandle = strstr(inputFileName, prefixe_before_value);
	if(pHandle)
	{
		pHandle += strlen(prefixe_before_value);
		sscanf (pHandle,"%d_%d_", value, value2);
		ALGO_Log_2 ("detected value1 %d, value2 %d\n", *value, *value2);
	}
	else
	{
		ALGO_Log_0("Value not recognazied\n");
	}
}


int zoomBMPProcessParam(char* inputFileName, char *outputFileName, TUint32 width, TUint32 height, TUint32 stride, TUint32 sliceheight, TAlgoImageFormat inputFormat, TUint32 headerLine, TUint32 pixelOrder, TReal32 zoomFactor)
{
	TAlgoImageParams aImageParams;
	TAny* bufferInputData = (TAny*)NULL;
	TUint32 imageSize = 0;
	TUint32 memHandle=0, file_size = 0;
	TAny* file = (TAny*)NULL;

	imageSize=stride*height;
	switch(inputFormat)
	{
		case EImageFormatYUV420MBSemiPlanarFiltered:
			imageSize=imageSize*3/2;
			break;
		case EImageFormatYUV420PackedPlanarI420:
			imageSize=imageSize*3/2;
			break;
		case EImageFormatYUV422Interleaved:
			imageSize=imageSize*2;
			break;
		case EImageFormat16bitRGB565:
		case EImageFormat16bitBGR565:
			imageSize=imageSize*2;
			break;
		case EImageFormat24bitRGB888:
		case EImageFormat24bitBGR888:
			imageSize=imageSize*3;
			break;
		case EImageFormatRAW8:
			imageSize=imageSize*1;
			break;
		case EImageFormatRAW12:
			imageSize=imageSize*3/2;
			break;
		default:
			ALGO_Log_0("Unsupported file format\n");
			return 1;
	}
	memHandle = (TUint32) AlgoMalloc(imageSize); 
	if(0 == memHandle){ALGO_Log_2("Error, can not allocate memory : fct %s, line %d\n", __FUNCTION__, __LINE__);return EErrorNoMemory;}
	bufferInputData = (TAny*)(LOS_GetLogicalAddress(memHandle));
	file = ALGO_fopen(inputFileName, "rb");
	if(file)
	{
		aImageParams.iImageWidth	= width;
		aImageParams.iImageHeight	= height;
		aImageParams.iImageStride	= stride;
		aImageParams.iImageSliceHeight	= sliceheight;
		aImageParams.iImageHeaderLength = 0;
		aImageParams.iImageNbStatusLine = headerLine;
		aImageParams.iImagePixelOrder = pixelOrder;
		aImageParams.iImageFormat = inputFormat;
		aImageParams.iBitMaxR = 0;
		aImageParams.iBitMaxG = 0;
		aImageParams.iBitMaxB = 0;
		aImageParams.iParams = 0;
		ALGO_fseek(file, 0, ALGO_SEEK_END);
		file_size = ALGO_ftell(file);
		ALGO_fseek(file, 0, ALGO_SEEK_SET);
		ALGO_fread(bufferInputData, sizeof(TUint8), imageSize, file);
		convertToBMPandSaveWithBuffers_with_zoom(&aImageParams, (char*)bufferInputData + aImageParams.iImageHeaderLength, outputFileName, zoomFactor);
	}
	else
	{
		ALGO_Log_1("Error : can not open file : %s\n", inputFileName);
	}

	if(file)
		ALGO_fclose(file);
	if(memHandle)
		AlgoFree((TAny*)memHandle);
	return 0;
}

int zoomBMPProcess(char* inputFileName, char *outputFileName, TReal32 zoomFactor)
{
	TInt8 *pHandle = (TInt8 *)NULL;
	TAlgoImageFormat inputFormat;
	TUint32 width = 0, height = 0, stride = 0, sliceheight = 0, id = 0, frame = 0, headerLine = 0, pixelOrder = 0;

	getParamFromFileName(inputFileName, &inputFormat, &width, &height, &stride, &sliceheight, &id, &frame, &headerLine, &pixelOrder);

	zoomBMPProcessParam(inputFileName, outputFileName, width, height, stride, sliceheight, inputFormat, headerLine, pixelOrder, zoomFactor);

	return 0;
}


typedef struct
{
	char inputFileName[512];
	char outputFileName[512];
}TFileProcessInfo;


int BMP_Zoom(void)
{
	int index = 0, count = 0;
	TFileProcessInfo fileInfo[8];
	TReal32 zoomFactor = 1.0;
	for(index = 0; index < sizeof(fileInfo) / sizeof(TFileProcessInfo); index++)
	{
		fileInfo[index].inputFileName[0] = 0;
		fileInfo[index].outputFileName[0] = 0;
	}

	count = 0;


	zoomFactor = 196608.0 / 131072.0;
//	strcpy(fileInfo[count].inputFileName,  ".//inputfiles//zoom//frame_1__reso_320_x_240_id_001_format_bmp_zoom_65536.bmp");
//	strcpy(fileInfo[count].outputFileName, ".//outputfiles//zoom_frame_1__reso_320_x_240_id_001_format_bmp_zoom_65536.bmp");
//	count++;
	strcpy(fileInfo[count].inputFileName,  ".//inputfiles//zoom//frame_1_DIGITALZOOMTYPE__id_003__v_131072_sensor_lada_port_0_reso_1024_x_0768_stride_2048_hl_0_po_0_sh_768__format_yuv422interleaved_nc_0_nc_0.bmp");
	strcpy(fileInfo[count].outputFileName, ".//outputfiles//frame_1_DIGITALZOOMTYPE__id_004__v_196608_sensor_lada_port_0_reso_1024_x_0768_stride_2048_hl_0_po_0_sh_768__format_yuv422interleaved_nc_0_nc_0_bis.bmp");
	count++;

	for(index = 0; index < sizeof(fileInfo) / sizeof(TFileProcessInfo); index++)
	{
		if( (fileInfo[index].inputFileName[0] != 0) && (fileInfo[index].outputFileName[0] != 0))
		{
			printf("\n processing file : %s\n", fileInfo[index].inputFileName);
			applyZoomWithFilesRoutine(fileInfo[index].inputFileName,fileInfo[index].outputFileName, zoomFactor);
		}
	}

	return 0;
}



//main function
TInt32 main(TInt32 argc, TUint8* argv[])
{
    TUint8 dirbuf[256];

/*
	{
#define LOCAL_STRING_SIZE 6
		char my_string[LOCAL_STRING_SIZE];
		char my_text[]="1234567890";
		unsigned int my_string_sizeof = sizeof(my_string);
		unsigned int nb_characters = 3;
		printf("my_string_sizeof %d\n", my_string_sizeof);
		memset(my_string, 5, my_string_sizeof);
		strncpy(my_string, my_text, nb_characters);
		printf("strncpy of %d char, my_string[%d]=%d\n", nb_characters, nb_characters, my_string[nb_characters]);
		printf("strncpy of %d char, my_string[%d]=%d\n", nb_characters, nb_characters+1, my_string[nb_characters+1]);

		printf("%s\n", my_string);

		getc(stdin);
		return 0;
	}
*/
/*
  BMP_Zoom();
  ALGO_Log_0("\n\nType a key to exit\n");
  getc(stdin);
  return 0;
*/

	{
		TAlgoError ret = EErrorNone; 
		TAlgoMetricParams aMetricParams;
		TAlgoImageParams aImageParamsDefault;
		TUint8 dir1[512];
		TUint8 dir2[512];  
		TUint32 width = 0, height = 0, stride = 0, sliceheight = 0, id = 0, frame = 0, headerLine = 0, pixelOrder = 0;
		TAlgoImageFormat inputFormat = EImageFormatUnknown;
		char *string1, *string2;
			
		AlgoStrcpy(dir1, ".//inputfiles//mirror//frame_1_MIRRORTYPE__id_001__v_none_sensor_558_port_0_reso_0320_x_0240_stride_0704_hl_0_po_0_sh_272__format_yuv422interleaved_nc_0_nc_0.bmp");
		AlgoStrcpy(dir2, ".//inputfiles//mirror//frame_1_MIRRORTYPE__id_002__v_vertical_sensor_558_port_0_reso_0320_x_0240_stride_0704_hl_0_po_0_sh_272__format_yuv422interleaved_nc_0_nc_0.bmp");
		
		AlgoStrcpy(dir1, ".//inputfiles//debug//frame_1_MIRRORTYPE__id_001__v_none_sensor_lada_port_3_reso_1920_x_1080.bmp");
		AlgoStrcpy(dir2, ".//inputfiles//debug//frame_1_MIRRORTYPE__id_002__v_both_sensor_lada_port_3_reso_1920_x_1080.bmp");
		getParamFromFileName(dir1, &inputFormat, &width, &height, &stride, &sliceheight, &id, &frame, &headerLine, &pixelOrder);
		string1 = strstr(dir1, "_v_");
		string1 += strlen("_v_");
		string2 = strstr(dir2, "_v_");
		string2 += strlen("_v_");

		aMetricParams.iErrorTolerance.iErrorToleranceR = 5;
		aMetricParams.iErrorTolerance.iErrorToleranceG = 0;
		aMetricParams.iErrorTolerance.iErrorToleranceB = 0;
		aMetricParams.iMetricRange.iRangeR = 0;
		aMetricParams.iMetricRange.iRangeG = 0;
		aMetricParams.iMetricRange.iRangeB = 0;
		aMetricParams.iTestCoordinates.iStartIndexX = 0;
		aMetricParams.iTestCoordinates.iStartIndexY = 0;
		aMetricParams.iTestCoordinates.iPixelsToGrabX = width;
		aMetricParams.iTestCoordinates.iPixelsToGrabY = height;

		aImageParamsDefault.iBitMaxB = 0;
		aImageParamsDefault.iBitMaxG = 0;
		aImageParamsDefault.iBitMaxR = 0;
		aImageParamsDefault.iImageFormat = 0;
		aImageParamsDefault.iImageWidth = 0;
		aImageParamsDefault.iImageStride = 0;
		aImageParamsDefault.iImageHeight = 0;
		aImageParamsDefault.iImageSliceHeight = 0;
		aImageParamsDefault.iImageHeaderLength = 0;
		aImageParamsDefault.iImageNbStatusLine = 0;
		aImageParamsDefault.iImagePixelOrder = 0;
		aImageParamsDefault.iParams = 0;

		aImageParamsDefault.iImageWidth	= width;
		aImageParamsDefault.iImageHeight	= height;
		aImageParamsDefault.iImageStride	= stride;
		aImageParamsDefault.iImageSliceHeight	= sliceheight;
		aImageParamsDefault.iImageHeaderLength = 0;
		aImageParamsDefault.iImageNbStatusLine = headerLine;
		aImageParamsDefault.iImagePixelOrder = pixelOrder;
		aImageParamsDefault.iImageFormat = inputFormat;

		aMetricParams.iParams = NULL;

		if(strstr(string2, "none"))
		{
			aMetricParams.iType = ETypeVerticalMirroring;
		}
		else
		{
			if(strstr(string2, "vertical"))
			{
				aMetricParams.iType = ETypeVerticalMirroring;
			}
			else
			{
				if(strstr(string2, "horizontal"))
				{
					aMetricParams.iType = ETypeHorizontalMirroring;
				}
				else
				{
					if(strstr(string2, "both"))
					{
						aMetricParams.iType = ETypeBothMirroring;
					}
					else
					{
						ALGO_Log_1("\t Mirror not supported: %s\n", string2);
					}
				}
			}
		}

		ret = ValidationMetricInterfaceWithFiles(&aImageParamsDefault, &aImageParamsDefault, dir1, dir2, &aMetricParams );
		Metrcis_Print_Result(ret);
		ALGO_Log_0("\n\nType a key to exit\n");
		getc(stdin);
		return 0;
	}


	{
		TAlgoError ret = EErrorNone; 
		TAlgoMetricParams aMetricParams;
		TAlgoImageParams aImageParamsDefault;
		TUint8 dir1[512];
		TUint8 dir2[512];  
		TUint32 width = 0, height = 0, stride = 0, sliceheight = 0, id = 0, frame = 0, headerLine = 0, pixelOrder = 0;
		TAlgoImageFormat inputFormat = EImageFormatUnknown;
		TAlgoParamCenterFieldOfView paramCenterFieldOfView;
		TInt32 value_1_a = 0, value_1_b = 0, value_2_a, value_2_b = 0; 
		TInt32 nX = 0, nY = 0, nX1 = 0, nY1 = 0;
			
		AlgoStrcpy(dir1, ".//inputfiles//cfow//frame_1_CENTERFILEDOFVIEWTYPE__id_001__v_819_-2184_sensor_558_port_0_reso_0640_x_0480_stride_1408_hl_0_po_0_sh_528__format_yuv422interleaved_nc_0_nc_0.bmp");
		AlgoStrcpy(dir2, ".//inputfiles//cfow//frame_1_CENTERFILEDOFVIEWTYPE__id_002__v_-3276_6553_sensor_558_port_0_reso_0640_x_0480_stride_1408_hl_0_po_0_sh_528__format_yuv422interleaved_nc_0_nc_0.bmp");
		AlgoStrcpy(dir1, ".//inputfiles//cfow//frame_1_CENTERFILEDOFVIEWTYPE__id_001__v_819_-2184_sensor_558_port_0_reso_0640_x_0480.bmp");
		AlgoStrcpy(dir2, ".//inputfiles//cfow//frame_1_CENTERFILEDOFVIEWTYPE__id_002__v_-3276_6553_sensor_558_port_0_reso_0640_x_0480.bmp");
		getParamFromFileName(dir1, &inputFormat, &width, &height, &stride, &sliceheight, &id, &frame, &headerLine, &pixelOrder);
		getParamValue2FromFileName(dir1, "_v_", &value_1_a, &value_1_b);
		getParamValue2FromFileName(dir2, "_v_", &value_2_a, &value_2_b);

		aMetricParams.iErrorTolerance.iErrorToleranceR = 5;
		aMetricParams.iErrorTolerance.iErrorToleranceG = 0;
		aMetricParams.iErrorTolerance.iErrorToleranceB = 0;
		aMetricParams.iMetricRange.iRangeR = 0;
		aMetricParams.iMetricRange.iRangeG = 0;
		aMetricParams.iMetricRange.iRangeB = 0;
		aMetricParams.iType = ETypeCenterFieldOfView;
		aMetricParams.iTestCoordinates.iStartIndexX = 0; // not used
		aMetricParams.iTestCoordinates.iStartIndexY = 0; // not used
		aMetricParams.iTestCoordinates.iPixelsToGrabX = 0; // size max of common widow is computed by metric
		aMetricParams.iTestCoordinates.iPixelsToGrabY = 0; // size max of common widow is computed by metric

		aImageParamsDefault.iBitMaxB = 0;
		aImageParamsDefault.iBitMaxG = 0;
		aImageParamsDefault.iBitMaxR = 0;
		aImageParamsDefault.iImageFormat = 0;
		aImageParamsDefault.iImageWidth = 0;
		aImageParamsDefault.iImageStride = 0;
		aImageParamsDefault.iImageHeight = 0;
		aImageParamsDefault.iImageSliceHeight = 0;
		aImageParamsDefault.iImageHeaderLength = 0;
		aImageParamsDefault.iImageNbStatusLine = 0;
		aImageParamsDefault.iImagePixelOrder = 0;
		aImageParamsDefault.iParams = 0;


		aImageParamsDefault.iImageWidth	= width;
		aImageParamsDefault.iImageHeight	= height;
		aImageParamsDefault.iImageStride	= stride;
		aImageParamsDefault.iImageSliceHeight	= sliceheight;
		aImageParamsDefault.iImageHeaderLength = 0;
		aImageParamsDefault.iImageNbStatusLine = headerLine;
		aImageParamsDefault.iImagePixelOrder = pixelOrder;
		aImageParamsDefault.iImageFormat = inputFormat;

		nX1		= (TInt32)(value_2_a * width) / (2 * 65536);
		nX		= (TInt32)(value_1_a * width) / (2 * 65536);
		nY1		= (TInt32)(value_2_b * height) / (2 * 65536);
		nY		= (TInt32)(value_1_b * height) / (2 * 65536);
		paramCenterFieldOfView.iX		= nX1 - nX;
		paramCenterFieldOfView.iY		= nY1 - nY;
		paramCenterFieldOfView.iX		= (TInt32)((value_2_a-value_1_a) * width) / (2 * 65536);
		paramCenterFieldOfView.iY		= (TInt32)((value_2_b-value_1_b) * height) / (2 * 65536);
		// probleme d arrondi
		paramCenterFieldOfView.iX		= -20;
		paramCenterFieldOfView.iY		= 32;
		paramCenterFieldOfView.iTolerance		= 5;
		aMetricParams.iParams = &paramCenterFieldOfView;

		ret = ValidationMetricInterfaceWithFiles(&aImageParamsDefault, &aImageParamsDefault, dir1, dir2, &aMetricParams );
		Metrcis_Print_Result(ret);
		ALGO_Log_0("\n\nType a key to exit\n");
		getc(stdin);
		return 0;
	}

	{
		TAlgoError ret = EErrorNone; 
		TAlgoMetricParams aMetricParams;
		TAlgoImageParams aImageParamsDefault;
		TUint8 dir1[512];
		TUint8 dir2[512];  
		TUint32 width = 0, height = 0, stride = 0, sliceheight = 0, id = 0, frame = 0, headerLine = 0, pixelOrder = 0;
		TAlgoParamImageContent imageContent;
		TAlgoImageFormat inputFormat = EImageFormatUnknown;
		TInt32 value_1_R = 0, value_1_G = 0, value_1_B = 0, value_2_R = 0, value_2_G = 0, value_2_B = 0;
		TInt32 value_1_lux = -1, value_2_lux = -1;
			
		AlgoStrcpy(dir1, ".//inputfiles//color//valid_dbox_color_one-shot_1_at_port_0_reso_320_x_240_value_R_0_G_0_B_255_LUX_0_nc_0.bmp");
		AlgoStrcpy(dir2, ".//inputfiles//color//valid_dbox_color_one-shot_1_at_port_0_reso_320_x_240_value_R_255_G_0_B_0_LUX_0_nc_0.bmp");
		AlgoStrcpy(dir1, ".//inputfiles//color//valid_dbox_color_one-shot_1_at_port_0_reso_320_x_240_value_R_0_G_255_B_0_LUX_0_nc_0.bmp");
		AlgoStrcpy(dir2, ".//inputfiles//color//valid_dbox_color_one-shot_1_at_port_0_reso_320_x_240_value_R_0_G_0_B_0_LUX_2000_nc_0.bmp");
		getParamFromFileName(dir1, &inputFormat, &width, &height, &stride, &sliceheight, &id, &frame, &headerLine, &pixelOrder);
		getParamValueFromFileName(dir1, "_R_", &value_1_R);
		getParamValueFromFileName(dir1, "_G_", &value_1_G);
		getParamValueFromFileName(dir1, "_B_", &value_1_B);
		getParamValueFromFileName(dir1, "_LUX_", &value_1_lux);
		getParamValueFromFileName(dir2, "_R_", &value_2_R);
		getParamValueFromFileName(dir2, "_G_", &value_2_G);
		getParamValueFromFileName(dir2, "_B_", &value_2_B);
		getParamValueFromFileName(dir2, "_LUX_", &value_2_lux);
		if(0 != value_1_lux)
		{
			value_1_R = 0xff;
			value_1_G = 0xff;
			value_1_B = 0xff;
		}
		if(0 != value_2_lux)
		{
			value_2_R = 0xff;
			value_2_G = 0xff;
			value_2_B = 0xff;
		}

		aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
		aMetricParams.iErrorTolerance.iErrorToleranceG = aMetricParams.iErrorTolerance.iErrorToleranceR;
		aMetricParams.iErrorTolerance.iErrorToleranceB = aMetricParams.iErrorTolerance.iErrorToleranceR;
		aMetricParams.iMetricRange.iRangeR = 0;
		aMetricParams.iMetricRange.iRangeG = 0;
		aMetricParams.iMetricRange.iRangeB = 0;
		aMetricParams.iType = ETypeImageContent;
		aMetricParams.iTestCoordinates.iStartIndexX = (width/2)-1;
		aMetricParams.iTestCoordinates.iStartIndexY = (height/2)-1;
		aMetricParams.iTestCoordinates.iPixelsToGrabX = 2;
		aMetricParams.iTestCoordinates.iPixelsToGrabY = 2;

		aImageParamsDefault.iBitMaxB = 0;
		aImageParamsDefault.iBitMaxG = 0;
		aImageParamsDefault.iBitMaxR = 0;
		aImageParamsDefault.iImageFormat = 0;
		aImageParamsDefault.iImageWidth = 0;
		aImageParamsDefault.iImageStride = 0;
		aImageParamsDefault.iImageHeight = 0;
		aImageParamsDefault.iImageSliceHeight = 0;
		aImageParamsDefault.iImageHeaderLength = 0;
		aImageParamsDefault.iImageNbStatusLine = 0;
		aImageParamsDefault.iImagePixelOrder = 0;
		aImageParamsDefault.iParams = 0;


		aImageParamsDefault.iImageWidth	= width;
		aImageParamsDefault.iImageHeight	= height;
		aImageParamsDefault.iImageStride	= stride;
		aImageParamsDefault.iImageSliceHeight	= sliceheight;
		aImageParamsDefault.iImageHeaderLength = 0;
		aImageParamsDefault.iImageNbStatusLine = headerLine;
		aImageParamsDefault.iImagePixelOrder = pixelOrder;
		aImageParamsDefault.iImageFormat = inputFormat;

		imageContent.iContentType_1 = EImageContentSolidBarRorGorBorBorW;
		imageContent.iR_1 = value_1_R;
		imageContent.iG_1 = value_1_G;
		imageContent.iB_1 = value_1_B;
		imageContent.tolerance_1 = 0;
		imageContent.iContentType_2 = EImageContentSolidBarRorGorBorBorW;
		imageContent.iR_2 = value_2_R;
		imageContent.iG_2 = value_2_G;
		imageContent.iB_2 = value_2_B;
		imageContent.tolerance_2 = 0;
		imageContent.tolerance_R = 0;
		imageContent.tolerance_G = 0;
		imageContent.tolerance_B = 0;
		aMetricParams.iParams = &imageContent;

		ret = ValidationMetricInterfaceWithFiles(&aImageParamsDefault, &aImageParamsDefault, dir1, dir2, &aMetricParams );
		Metrcis_Print_Result(ret);
		ALGO_Log_0("\n\nType a key to exit\n");
		getc(stdin);
		return 0;
	}
	
	{
		TAlgoError ret = EErrorNone; 
		TAlgoMetricParams aMetricParams;
		TAlgoImageParams aImageParamsDefault;
		TUint8 dir1[512];
		TUint8 dir2[512];  
		TUint32 width = 0, height = 0, stride = 0, sliceheight = 0, id = 0, frame = 0, headerLine = 0, pixelOrder = 0;
		TAlgoImageFormat inputFormat = EImageFormatUnknown;
		TAlgoParamContrast paramContrast;
		TInt32 value_1 = 0, value_2 = 0; 
			
		//AlgoStrcpy(dir1, ".//inputfiles//contrast//frame_1_CONTRASTTYPE__id_005__v_32_sensor_lada_port_2_reso_3264_x_2448_stride_4896_hl_4_po_1_sh_2448__format_yuv420packedplanari420_nc_0_nc_0.bmp");
		//AlgoStrcpy(dir2, ".//inputfiles//contrast//frame_1_CONTRASTTYPE__id_006__v_98_sensor_lada_port_2_reso_3264_x_2448_stride_4896_hl_4_po_1_sh_2448__format_yuv420packedplanari420_nc_0_nc_0.bmp");
		AlgoStrcpy(dir1, ".//inputfiles//contrast//frame_1_CONTRASTTYPE__id_001__v_-100_sensor_lada_port_2_reso_3264_x_2448_stride_4896_hl_4_po_1_sh_2448__format_yuv420packedplanari420_nc_0_nc_0.bmp");
		AlgoStrcpy(dir2, ".//inputfiles//contrast//frame_1_CONTRASTTYPE__id_002__v_-34_sensor_lada_port_2_reso_3264_x_2448_stride_4896_hl_4_po_1_sh_2448__format_yuv420packedplanari420_nc_0_nc_0.bmp");
		getParamFromFileName(dir1, &inputFormat, &width, &height, &stride, &sliceheight, &id, &frame, &headerLine, &pixelOrder);
		getParamValueFromFileName(dir1, "_v_", &value_1);
		getParamValueFromFileName(dir2, "_v_", &value_2);

		aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
		aMetricParams.iErrorTolerance.iErrorToleranceG = aMetricParams.iErrorTolerance.iErrorToleranceR;
		aMetricParams.iErrorTolerance.iErrorToleranceB = aMetricParams.iErrorTolerance.iErrorToleranceR;
		aMetricParams.iMetricRange.iRangeR = 0;
		aMetricParams.iMetricRange.iRangeG = 0;
		aMetricParams.iMetricRange.iRangeB = 0;
		aMetricParams.iType = ETypeContrast;
		aMetricParams.iTestCoordinates.iStartIndexX = 0;
		aMetricParams.iTestCoordinates.iStartIndexY = 0;
		aMetricParams.iTestCoordinates.iPixelsToGrabX = width;
		aMetricParams.iTestCoordinates.iPixelsToGrabY = height;

		aImageParamsDefault.iBitMaxB = 0;
		aImageParamsDefault.iBitMaxG = 0;
		aImageParamsDefault.iBitMaxR = 0;
		aImageParamsDefault.iImageFormat = 0;
		aImageParamsDefault.iImageWidth = 0;
		aImageParamsDefault.iImageStride = 0;
		aImageParamsDefault.iImageHeight = 0;
		aImageParamsDefault.iImageSliceHeight = 0;
		aImageParamsDefault.iImageHeaderLength = 0;
		aImageParamsDefault.iImageNbStatusLine = 0;
		aImageParamsDefault.iImagePixelOrder = 0;
		aImageParamsDefault.iParams = 0;


		aImageParamsDefault.iImageWidth	= width;
		aImageParamsDefault.iImageHeight	= height;
		aImageParamsDefault.iImageStride	= stride;
		aImageParamsDefault.iImageSliceHeight	= sliceheight;
		aImageParamsDefault.iImageHeaderLength = 0;
		aImageParamsDefault.iImageNbStatusLine = headerLine;
		aImageParamsDefault.iImagePixelOrder = pixelOrder;
		aImageParamsDefault.iImageFormat = inputFormat;

		paramContrast.iPrContrast_1 = (TInt32)value_1;
		paramContrast.iPrContrast_2 = (TInt32)value_2;
		aMetricParams.iParams = &paramContrast;

		ret = ValidationMetricInterfaceWithFiles(&aImageParamsDefault, &aImageParamsDefault, dir1, dir2, &aMetricParams );
		Metrcis_Print_Result(ret);
		ALGO_Log_0("\n\nType a key to exit\n");
		getc(stdin);
		return 0;
	}


	{
		test_metric_centerfieldofview_characterization(".//inputfiles//");
		ALGO_Log_0("\n\nType a key to exit\n");
		getc(stdin);
		return 0;
	}


	{
		TAlgoError ret = EErrorNone; 
		TAlgoMetricParams aMetricParams;
		TAlgoImageParams aImageParamsDefault;
		TUint8 dir1[512];
		TUint8 dir2[512];  
		TReal64 zoomfactor = 1;
		TUint32 width = 0, height = 0, stride = 0, sliceheight = 0, id = 0, frame = 0, headerLine = 0, pixelOrder = 0;
		TAlgoImageFormat inputFormat = EImageFormatUnknown;
		
		zoomfactor = ((TReal64)196608.0/131072.0);
		
		AlgoStrcpy(dir1, ".//inputfiles//zoom//frame_1_DIGITALZOOMTYPE__id_003__v_131072_sensor_lada_port_0_reso_1024_x_0768_stride_2048_hl_0_po_0_sh_768__format_yuv422interleaved_nc_0_nc_0.bmp");
		AlgoStrcpy(dir2, ".//inputfiles//zoom//frame_1_DIGITALZOOMTYPE__id_004__v_196608_sensor_lada_port_0_reso_1024_x_0768_stride_2048_hl_0_po_0_sh_768__format_yuv422interleaved_nc_0_nc_0.bmp");
//		AlgoStrcpy(dir1, ".//inputfiles//zoom//frame_1_DIGITALZOOMTYPE__id_001__v_65536_sensor_lada_port_0_reso_0320_x_0240_stride_0704_hl_0_po_0_sh_272__format_yuv422interleaved_nc_0.gam");
//		AlgoStrcpy(dir2, ".//inputfiles//zoom//frame_1_DIGITALZOOMTYPE__id_002__v_131072_sensor_lada_port_0_reso_0320_x_0240_stride_0704_hl_0_po_0_sh_272__format_yuv422interleaved_nc_0.gam");
//		AlgoStrcpy(dir1, ".//inputfiles//zoom//frame_1_DIGITALZOOMTYPE__id_001__v_65536_sensor_lada_port_0_reso_0128_x_0096_stride_0288_hl_0_po_0_sh_112__format_yuv422interleaved_nc_0.gam");
//		AlgoStrcpy(dir2, ".//inputfiles//zoom//frame_1_DIGITALZOOMTYPE__id_002__v_131072_sensor_lada_port_0_reso_0128_x_0096_stride_0288_hl_0_po_0_sh_112__format_yuv422interleaved_nc_0.gam");
		getParamFromFileName(dir1, &inputFormat, &width, &height, &stride, &sliceheight, &id, &frame, &headerLine, &pixelOrder);

		aMetricParams.iErrorTolerance.iErrorToleranceR = 10;
		aMetricParams.iErrorTolerance.iErrorToleranceG = aMetricParams.iErrorTolerance.iErrorToleranceR;
		aMetricParams.iErrorTolerance.iErrorToleranceB = aMetricParams.iErrorTolerance.iErrorToleranceR;
		aMetricParams.iMetricRange.iRangeR = 0;
		aMetricParams.iMetricRange.iRangeG = 0;
		aMetricParams.iMetricRange.iRangeB = 0;
		aMetricParams.iType = ETypeZoom;
		aMetricParams.iTestCoordinates.iStartIndexX = 0;
		aMetricParams.iTestCoordinates.iStartIndexY = 0;
		aMetricParams.iTestCoordinates.iPixelsToGrabX = width;
		aMetricParams.iTestCoordinates.iPixelsToGrabY = height;

		aImageParamsDefault.iBitMaxB = 0;
		aImageParamsDefault.iBitMaxG = 0;
		aImageParamsDefault.iBitMaxR = 0;
		aImageParamsDefault.iImageFormat = 0;
		aImageParamsDefault.iImageWidth = 0;
		aImageParamsDefault.iImageStride = 0;
		aImageParamsDefault.iImageHeight = 0;
		aImageParamsDefault.iImageSliceHeight = 0;
		aImageParamsDefault.iImageHeaderLength = 0;
		aImageParamsDefault.iImageNbStatusLine = 0;
		aImageParamsDefault.iImagePixelOrder = 0;
		aImageParamsDefault.iParams = 0;

		aImageParamsDefault.iImageWidth	= width;
		aImageParamsDefault.iImageHeight	= height;
		aImageParamsDefault.iImageStride	= stride;
		aImageParamsDefault.iImageSliceHeight	= sliceheight;
		aImageParamsDefault.iImageHeaderLength = 0;
		aImageParamsDefault.iImageNbStatusLine = headerLine;
		aImageParamsDefault.iImagePixelOrder = pixelOrder;
		aImageParamsDefault.iImageFormat = inputFormat;

		aMetricParams.iParams = &zoomfactor;
		ret = ValidationMetricInterfaceWithFiles(&aImageParamsDefault, &aImageParamsDefault, dir1, dir2, &aMetricParams );
		Metrcis_Print_Result(ret);
		ALGO_Log_0("\n\nType a key to exit\n");
		getc(stdin);
		return 0;
	}


	{
		TAlgoError ret = EErrorNone; 
		TAlgoMetricParams aMetricParams;
		TAlgoImageParams aImageParamsDefault;
		TUint8 dir1[512];
		TUint8 dir2[512];  
		TUint32 width = 0, height = 0, stride = 0, sliceheight = 0, id = 0, frame = 0, headerLine = 0, pixelOrder = 0;
		TAlgoImageFormat inputFormat = EImageFormatUnknown;
		
		aImageParamsDefault.iBitMaxB = 0;
		aImageParamsDefault.iBitMaxG = 0;
		aImageParamsDefault.iBitMaxR = 0;
		aImageParamsDefault.iImageFormat = 0;
		aImageParamsDefault.iImageWidth = 0;
		aImageParamsDefault.iImageStride = 0;
		aImageParamsDefault.iImageSliceHeight = 0;
		aImageParamsDefault.iImageHeight = 0;
		aImageParamsDefault.iImageHeaderLength = 0;
		aImageParamsDefault.iImageNbStatusLine = 0;
		aImageParamsDefault.iImagePixelOrder = 0;
		aImageParamsDefault.iParams = 0;

		AlgoStrcpy(dir1, ".//inputfiles//rot//frame_1_ROTATIONTYPE__id_001__v_-270_sensor_lada_port_3_reso_0240_x_0320_stride_0408_hl_0_po_0_sh_352__format_yuv420mbpackedsemiplanar_nc_0_nc_0.bmp");
		getParamFromFileName(dir1, &inputFormat, &width, &height, &stride, &sliceheight, &id, &frame, &headerLine, &pixelOrder);
		aMetricParams.iTestCoordinates.iPixelsToGrabX = width;
		aMetricParams.iTestCoordinates.iPixelsToGrabY = height;
		aMetricParams.iType = EType180degreesRotation;
		AlgoStrcpy(dir2, ".//inputfiles//rot//frame_1_ROTATIONTYPE__id_002__v_-90_sensor_lada_port_3_reso_0240_x_0320_stride_0408_hl_0_po_0_sh_352__format_yuv420mbpackedsemiplanar_nc_0_nc_1.bmp");

		//aMetricParams.iTestCoordinates.iPixelsToGrabX = 2;
		//aMetricParams.iTestCoordinates.iPixelsToGrabY = 2;
		//AlgoStrcpy(dir1, ".//inputfiles//bmp_2x2__0_63_127_255.bmp");
		//aMetricParams.iType = EType90degreesRotation;
		//AlgoStrcpy(dir2, ".//inputfiles//bmp_2x2__0_63_127_255_rotated_90.bmp");
		
		//aMetricParams.iTestCoordinates.iPixelsToGrabX = 3;
		//aMetricParams.iTestCoordinates.iPixelsToGrabY = 2;
		//AlgoStrcpy(dir1, ".//inputfiles//bmp_3x2__0x0_0x0f_0x1f_0x2f_0x3f_0x4f.bmp");
		//aMetricParams.iType = EType90degreesRotation;
		//AlgoStrcpy(dir2, ".//inputfiles//bmp_3x2__0x0_0x0f_0x1f_0x2f_0x3f_0x4f_rotated_90.bmp");

		//aMetricParams.iTestCoordinates.iPixelsToGrabX = 3;
		//aMetricParams.iTestCoordinates.iPixelsToGrabY = 2;
		//AlgoStrcpy(dir1, ".//inputfiles//bmp_3x2__0x0_0x0f_0x1f_0x2f_0x3f_0x4f.bmp");
		//aMetricParams.iType = EType270degreesRotation;
		//AlgoStrcpy(dir2, ".//inputfiles//bmp_3x2__0x0_0x0f_0x1f_0x2f_0x3f_0x4f_rotated_270.bmp");
	
		aMetricParams.iErrorTolerance.iErrorToleranceR = 5;
		aMetricParams.iErrorTolerance.iErrorToleranceG = 0;
		aMetricParams.iErrorTolerance.iErrorToleranceB = 0;
		aMetricParams.iTestCoordinates.iStartIndexX = 0;
		aMetricParams.iTestCoordinates.iStartIndexY = 0;
		aMetricParams.iParams = NULL;
		ret = ValidationMetricInterfaceWithFiles(&aImageParamsDefault, &aImageParamsDefault, dir1, dir2, &aMetricParams );
		Metrcis_Print_Result(ret);
		ALGO_Log_0("\n\nType a key to exit\n");
		getc(stdin);
		return 0;
	}


	{
		TAlgoError ret = EErrorNone; 
		TAlgoMetricParams aMetricParams;
		TAlgoImageParams aImageParamsDefault;
		TUint8 dir1[512];
		TUint8 dir2[512];
		TAlgoImageFormat inputFormat = EImageFormatUnknown;
		TUint32 width = 0, height = 0, stride = 0, sliceheight = 0, id = 0, frame = 0, headerLine = 0, pixelOrder = 0;

		//AlgoStrcpy(dir1, ".//inputfiles//focus//frame_1_FOCUSCONTROLTYPE__id_001__v_Off_sensor_lada_port_0_reso_1024_x_0768_stride_2048_format_yuv422interleaved_nc_0_nc_0.bmp");
		//AlgoStrcpy(dir2, ".//inputfiles//focus//frame_1_FOCUSCONTROLTYPE__id_002__v_On_sensor_lada_port_0_reso_1024_x_0768_stride_2048_format_yuv422interleaved_nc_0_nc_0.bmp");
		AlgoStrcpy(dir1, ".//inputfiles//focus//frame_1_FOCUSCONTROLTYPE__id_001__v_Off_sensor_lada_port_1_reso_1024_x_0768_stride_3072_format_rgb888_nc_0_nc_0.bmp");
		AlgoStrcpy(dir2, ".//inputfiles//focus//frame_1_FOCUSCONTROLTYPE__id_002__v_On_sensor_lada_port_1_reso_1024_x_0768_stride_3072_format_rgb888_nc_0_nc_0.bmp");
		getParamFromFileName(dir1, &inputFormat, &width, &height, &stride, &sliceheight, &id, &frame, &headerLine, &pixelOrder);

		aMetricParams.iErrorTolerance.iErrorToleranceR = 5;
		aMetricParams.iErrorTolerance.iErrorToleranceG = 5;
		aMetricParams.iErrorTolerance.iErrorToleranceB = 5;
		aMetricParams.iType = ETypeAutoFocus;
		aMetricParams.iTestCoordinates.iStartIndexX = 0;
		aMetricParams.iTestCoordinates.iStartIndexY = 0;
		aMetricParams.iTestCoordinates.iPixelsToGrabX = width / 2;
		aMetricParams.iTestCoordinates.iPixelsToGrabY = height / 2;

		aImageParamsDefault.iBitMaxB = 0;
		aImageParamsDefault.iBitMaxG = 0;
		aImageParamsDefault.iBitMaxR = 0;
		aImageParamsDefault.iImageFormat = 0;
		aImageParamsDefault.iImageWidth = 0;
		aImageParamsDefault.iImageStride = 0;
		aImageParamsDefault.iImageSliceHeight = 0;
		aImageParamsDefault.iImageHeight = 0;
		aImageParamsDefault.iImageHeaderLength = 0;
		aImageParamsDefault.iImageNbStatusLine = 0;
		aImageParamsDefault.iImagePixelOrder = 0;
		aImageParamsDefault.iParams = 0;

		
		ret = ValidationMetricInterfaceWithFiles(&aImageParamsDefault, &aImageParamsDefault, dir1, dir2, &aMetricParams );
		Metrcis_Print_Result(ret);
		ALGO_Log_0("\n\nType a key to exit\n");
		getc(stdin);
		return 0;
	}

return 0;

    if ( argc == 1 )
    {
        argv[1] = dirbuf;
        ALGO_Log_0("Initialisation of the Default Directory Path is needed: give it as parameter \n\n");
		ALGO_Log_0("Type ENTER to quit\n");
		getc(stdin);
        return 0;
        //AlgoStrcpy(argv[1], "C:\\Programs\\Algorithms\\testimages"); 
        //AlgoStrcpy(argv[1], "C:\\testimages"); 
    }
    
    if ( argc <= 2 )
        {
        //Auto Focus Metrices Test.
        //test_metric_autofocus_with_spatial_test(argv[1]);
        
        //test_metric_autofocus_line("C:\\Programs\\Algorithms\\Deliveries\\metrices_v1.1_170409\\testimages\\AutoFocus\\focus_auto_line_1_top.bmp",
        //                           "C:\\Programs\\Algorithms\\Deliveries\\metrices_v1.1_170409\\testimages\\AutoFocus\\focus_auto_line_1_middle.bmp",
        //                           "C:\\Programs\\Algorithms\\Deliveries\\metrices_v1.1_170409\\testimages\\AutoFocus\\focus_auto_line_1_bottom.bmp");
        //test_metric_autofocus_colon("C:\\Programs\\Algorithms\\Deliveries\\metrices_v1.1_170409\\testimages\\AutoFocus\\focus_auto_colon_1_left.bmp",
        //                            "C:\\Programs\\Algorithms\\Deliveries\\metrices_v1.1_170409\\testimages\\AutoFocus\\focus_auto_colon_1_middle.bmp",
        //                            "C:\\Programs\\Algorithms\\Deliveries\\metrices_v1.1_170409\\testimages\\AutoFocus\\focus_auto_colon_1_right.bmp");
        
        //test_metric_autofocus_line("C:\\Programs\\Algorithms\\Deliveries\\metrices_v1.1_170409\\testimages\\AutoFocus\\focus_auto_line_2_top.bmp",
        //                           "C:\\Programs\\Algorithms\\Deliveries\\metrices_v1.1_170409\\testimages\\AutoFocus\\focus_auto_line_2_middle.bmp",
        //                           "C:\\Programs\\Algorithms\\Deliveries\\metrices_v1.1_170409\\testimages\\AutoFocus\\focus_auto_line_2_bottom.bmp");
        //test_metric_autofocus_colon("C:\\Programs\\Algorithms\\Deliveries\\metrices_v1.1_170409\\testimages\\AutoFocus\\focus_auto_colon_2_left.bmp",
        //                            "C:\\Programs\\Algorithms\\Deliveries\\metrices_v1.1_170409\\testimages\\AutoFocus\\focus_auto_colon_2_middle.bmp",
        //                            "C:\\Programs\\Algorithms\\Deliveries\\metrices_v1.1_170409\\testimages\\AutoFocus\\focus_auto_colon_2_right.bmp");
        
        //test_metric_autofocus_line("C:\\Programs\\Algorithms\\Deliveries\\metrices_v1.1_170409\\testimages\\AutoFocus\\focus_auto_line_3_top.bmp",
        //                           "C:\\Programs\\Algorithms\\Deliveries\\metrices_v1.1_170409\\testimages\\AutoFocus\\focus_auto_line_3_middle.bmp",
        //                           "C:\\Programs\\Algorithms\\Deliveries\\metrices_v1.1_170409\\testimages\\AutoFocus\\focus_auto_line_3_bottom.bmp");
        //test_metric_autofocus_colon("C:\\Programs\\Algorithms\\Deliveries\\metrices_v1.1_170409\\testimages\\AutoFocus\\focus_auto_colon_3_left.bmp",
        //                            "C:\\Programs\\Algorithms\\Deliveries\\metrices_v1.1_170409\\testimages\\AutoFocus\\focus_auto_colon_3_middle.bmp",
        //                            "C:\\Programs\\Algorithms\\Deliveries\\metrices_v1.1_170409\\testimages\\AutoFocus\\focus_auto_colon_3_right.bmp");
        
        //Black & Wihite Metrices Test.
        //test_metric_blackandwhite_characterization(argv[1]);

        //Negative Metrices Test.
        //test_metric_negative_characterization(argv[1]);

		//White Balance Metrices Test.
        //test_metric_whitebalance_characterization(argv[1]);

        //Emboss Metrices Test.
        //test_metric_emboss_characterization(argv[1]);

        //Spatial Similar Metrices Test.
        //test_metric_spatialsimilar_characterization(argv[1]);

		//Zoom Metrices Test.
		//test_metric_zoom_characterization(argv[1]);

        //Color Tone Metrices Test
        //test_metric_colortone_characterization(argv[1]);

        //Contrast Metrices Test
        //test_metric_contrast_characterization(argv[1]);

        //Saturation Metrices Test
        //test_metric_saturation_characterization(argv[1]);

        //Brightness Metrices Test
        //test_metric_brightness_characterization(argv[1]);

        //Solarize Metrices Test
        //test_metric_solarize_characterization(argv[1]);

        //Gamma Metrices Test
        //test_metric_gamma_characterization(argv[1]);

		//Mirroring Metrices Test
		test_metric_mirroring_characterization(argv[1]);

		//Rotation Metrices Test
		//test_metric_rotation_characterization(argv[1]);
        }
	else
		{
		test_metric_autofocus("..\\lauterbach\\axf\\saved_FOCUSCONTROLTYPE__Off_rgb565.gam","..\\lauterbach\\axf\\saved_FOCUSCONTROLTYPE__AutoLock_rgb565.gam");
		}

	ALGO_Log_0("Type ENTER to quit\n");
	getc(stdin);

    return 0;
    }

