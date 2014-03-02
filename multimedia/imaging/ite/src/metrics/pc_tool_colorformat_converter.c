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

TAlgoError convertImageToBMPProcessParam(char* inputFileName, char *outputFileName, TUint32 width, TUint32 height, TUint32 stride, TUint32 sliceheight, TAlgoImageFormat inputFormat, TUint32 headerLine, TUint32 pixelOrder)
{
	TAlgoImageParams aImageParams;
	TAny* bufferInputData = (TAny*)NULL;
	TUint32 imageSize = 0;
	TUint32 memHandle=0, file_size = 0;
	TAny* file = (TAny*)NULL;
	TAlgoROIOBJECTINFOTYPE aROIOBJECTINFOTYPE;
	memset(&aROIOBJECTINFOTYPE, 0, sizeof(TAlgoROIOBJECTINFOTYPE));

	imageSize=stride*(sliceheight + headerLine);
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
			return EErrorArgument;
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
		/*if(file_size != imageSize + aImageParams.iImageHeaderLength)
		{
			ALGO_Log_2("Error : bad file size = %d != %d expected\n", file_size, imageSize + aImageParams.iImageHeaderLength);
			ALGO_Log_0("Type enter to quit\n");
			getc(stdin);
			goto end;
		}*/
		ALGO_fread(bufferInputData, sizeof(TUint8), imageSize, file);
		aROIOBJECTINFOTYPE.nNumberOfROIs = 2;
		// avoid use of roi
		aROIOBJECTINFOTYPE.nNumberOfROIs = 0;

		aROIOBJECTINFOTYPE.sROI[0].sRect_sTopLeft_nX = 10;
		aROIOBJECTINFOTYPE.sROI[0].sRect_sTopLeft_nY = 10;
		aROIOBJECTINFOTYPE.sROI[0].sRect_sSize_nWidth = 30;
		aROIOBJECTINFOTYPE.sROI[0].sRect_sSize_nHeight = 15;
		aROIOBJECTINFOTYPE.sROI[0].sReference_nWidth = width;
		aROIOBJECTINFOTYPE.sROI[0].sReference_nHeight = height;
		aROIOBJECTINFOTYPE.sROI[0].eObjectType = EObjectTypeFocusedArea;

		aROIOBJECTINFOTYPE.sROI[1].sRect_sTopLeft_nX = width / 2 + 10;
		aROIOBJECTINFOTYPE.sROI[1].sRect_sTopLeft_nY = height / 2;
		aROIOBJECTINFOTYPE.sROI[1].sRect_sSize_nWidth = width * 5/ 16;
		aROIOBJECTINFOTYPE.sROI[1].sRect_sSize_nHeight = height * 5 / 16;
		aROIOBJECTINFOTYPE.sROI[1].sReference_nWidth = width;
		aROIOBJECTINFOTYPE.sROI[1].sReference_nHeight = height;
		aROIOBJECTINFOTYPE.sROI[1].eObjectType = EObjectTypeFace;
		convertToBMPandSaveWithBuffers(&aImageParams, (char*)bufferInputData + aImageParams.iImageHeaderLength, outputFileName, &aROIOBJECTINFOTYPE);
	}
	else
	{
		ALGO_Log_1("Error : can not open file : %s\n", inputFileName);
	}
end:
	if(file)
		ALGO_fclose(file);
	if(memHandle)
		AlgoFree((TAny*)memHandle);
	return EErrorNone;
}

int convertImageToBMPProcess(char* inputFileName, char *outputFileName)
{
	TInt8 *pHandle = (TInt8 *)NULL;
	TAlgoImageFormat inputFormat;
	TUint32 width = 0, height = 0, stride = 0, sliceheight = 0, id = 0, frame = 0, headerLine = 0, pixelOrder = 0;

	getParamFromFileName(inputFileName, &inputFormat, &width, &height, &stride, &sliceheight, &id, &frame, &headerLine, &pixelOrder);

	convertImageToBMPProcessParam(inputFileName, outputFileName, width, height, stride, sliceheight, inputFormat, headerLine, pixelOrder);

end:
	return 0;
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
		/*if(file_size != imageSize + aImageParams.iImageHeaderLength)
		{
			ALGO_Log_2("Error : bad file size = %d != %d expected\n", file_size, imageSize + aImageParams.iImageHeaderLength);
			ALGO_Log_0("Type enter to quit\n");
			getc(stdin);
			goto end;
		}*/
		ALGO_fread(bufferInputData, sizeof(TUint8), imageSize, file);
		convertToBMPandSaveWithBuffers_with_zoom(&aImageParams, (char*)bufferInputData + aImageParams.iImageHeaderLength, outputFileName, zoomFactor);
	}
	else
	{
		ALGO_Log_1("Error : can not open file : %s\n", inputFileName);
	}
end:
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

end:
	return 0;
}


typedef struct
{
	char inputFileName[512];
	char outputFileName[512];
}TFileProcessInfo;

int g_DoProcessFromConversionIntoBMP = 1;
int convertImageToBMP(void)
{
	int index = 0, count = 0;
	TFileProcessInfo fileInfo[8];
	for(index = 0; index < sizeof(fileInfo) / sizeof(TFileProcessInfo); index++)
	{
		fileInfo[index].inputFileName[0] = 0;
		fileInfo[index].outputFileName[0] = 0;
	}

	count = 0;

	strcpy(fileInfo[count].inputFileName,  ".//inputfiles//colorformat//frame_1_id_556_sensor_lada_port_0_reso_0320_x_0240_stride_0640_format_yuv422interleaved.gam");
	strcpy(fileInfo[count].outputFileName, ".//outputfiles//frame_1_id_556_sensor_lada_port_0_reso_0320_x_0240_stride_0640_format_yuv422interleaved.bmp");
	count++;
/*
	strcpy(fileInfo[count].inputFileName,  ".//inputfiles//colorformat//frame_1_id_557_sensor_lada_port_3_reso_0320_x_0240_stride_0480_format_yuv420mbpackedsemiplanar.gam");
	strcpy(fileInfo[count].outputFileName, ".//outputfiles//frame_1_id_557_sensor_lada_port_3_reso_0320_x_0240_stride_0480_format_yuv420mbpackedsemiplanar.bmp");
	count++;

	strcpy(fileInfo[count].inputFileName,  ".//inputfiles//colorformat//frame_1_id_558_sensor_lada_port_0_reso_0320_x_0240_stride_0640_format_rgb565.gam");
	strcpy(fileInfo[count].outputFileName, ".//outputfiles//frame_1_id_558_sensor_lada_port_0_reso_0320_x_0240_stride_0640_format_rgb565.bmp");
	count++;

	strcpy(fileInfo[count].inputFileName,  ".//inputfiles//colorformat//frame_1_id_559_sensor_lada_port_3_reso_0320_x_0240_stride_0480_format_yuv420packedplanari420.gam");
	strcpy(fileInfo[count].outputFileName, ".//outputfiles//frame_1_id_559_sensor_lada_port_3_reso_0320_x_0240_stride_0480_format_yuv420packedplanari420.bmp");
	count++;

	strcpy(fileInfo[count].inputFileName,  ".//inputfiles//colorformat//frame_1_id_560_sensor_lada_port_0_reso_0320_x_0240_stride_0960_format_rgb888.gam");
	strcpy(fileInfo[count].outputFileName, ".//outputfiles//frame_1_id_560_sensor_lada_port_0_reso_0320_x_0240_stride_0960_format_rgb888.bmp");
	count++;

	strcpy(fileInfo[count].inputFileName,  ".//inputfiles//colorformat//frame_1_id_566_sensor_558_port_1_reso_0648_x_0486_stride_0648_hl_3_po_0__format_RawBayer8bit.gam");
	strcpy(fileInfo[count].outputFileName, ".//outputfiles//frame_1_id_566_sensor_558_port_1_reso_0648_x_0486_stride_0648_hl_3_po_0__format_RawBayer8bit.bmp");
	count++;

	strcpy(fileInfo[count].inputFileName,  ".//inputfiles//colorformat//frame_1_id_567_sensor_558_port_1_reso_0648_x_0486_stride_0984_hl_3_po_0__format_RawBayer12bit.gam");
	strcpy(fileInfo[count].outputFileName, ".//outputfiles//frame_1_id_567_sensor_558_port_1_reso_0648_x_0486_stride_0984_hl_3_po_0__format_RawBayer12bit.bmp");
	count++;
*/
	for(index = 0; index < sizeof(fileInfo) / sizeof(TFileProcessInfo); index++)
	{
		if( (fileInfo[index].inputFileName[0] != 0) && (fileInfo[index].outputFileName[0] != 0))
		{
			printf("\n processing file : %s\n", fileInfo[index].inputFileName);
			convertImageToBMPProcess(fileInfo[index].inputFileName, fileInfo[index].outputFileName);
		}
	}	
	return 0;
}

int BMP_Zoom(void)
{
	int index = 0, count = 0;
	TFileProcessInfo fileInfo[8];
	TReal32 zoomFactor = 2.0;
	for(index = 0; index < sizeof(fileInfo) / sizeof(TFileProcessInfo); index++)
	{
		fileInfo[index].inputFileName[0] = 0;
		fileInfo[index].outputFileName[0] = 0;
	}

	count = 0;
/*
	strcpy(fileInfo[count].inputFileName,  ".//inputfiles//colorformat//frame_1_id_556_sensor_lada_port_0_reso_0320_x_0240_stride_0640_format_yuv422interleaved.gam");
	strcpy(fileInfo[count].outputFileName, ".//outputfiles//frame_1_id_556_sensor_lada_port_0_reso_0320_x_0240_stride_0640_format_yuv422interleaved.bmp");
	count++;

	for(index = 0; index < sizeof(fileInfo) / sizeof(TFileProcessInfo); index++)
	{
		if( (fileInfo[index].inputFileName[0] != 0) && (fileInfo[index].outputFileName[0] != 0))
		{
			printf("\n processing file : %s\n", fileInfo[index].inputFileName);
			convertImageToBMPProcess(fileInfo[index].inputFileName, fileInfo[index].outputFileName);
			zoomBMPProcess(fileInfo[index].inputFileName, ".//outputfiles//zoom_output.bmp", zoomFactor);
		}
	}
*/

	strcpy(fileInfo[count].inputFileName,  ".//inputfiles//zoom//frame_1__reso_320_x_240_id_001_format_bmp_zoom_65536.bmp");
	strcpy(fileInfo[count].outputFileName, ".//outputfiles//zoom_frame_1__reso_320_x_240_id_001_format_bmp_zoom_65536.bmp");
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


//#define _CRT_SECURE_NO_DEPRECATE // to avoid deprecated warning for sscanf

// C standard functions to access directory are different under unix or windows

#ifdef WIN32
#include <io.h>
#include <direct.h>
#define PROG01_SEPARATOR  '\\'
#else
#include <dirent.h>
#include <unistd.h>
#define PROG01_SEPARATOR  '/'
#endif

#include <ctype.h>
#include <errno.h>
#include <assert.h>
#include <stdarg.h>
#include <time.h>
#include <malloc.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>


#define TEST_SWAP_WORD(x)  (((x&0xFF00)>>8) | ((x&0x00FF)<<8))
#define TEST_SWAP_BYTE(x)  ((x<<24) | (x>>24) | ((x&0xFF00)<<8) | ((x&0xFF0000)>>8))
#define TEST_SWAP_BYTE2(x)  ((x<<16) | (x>>16))
#define TEST_SWAP_BYTE3(x)  ( ((x&0xFF000000)>>8) | ((x&0x00FF0000)<<8) | ((x&0x0000FF00)>>8) | ((x&0x000000FF)<<8) )

#define TEST_MIN(a, b)    ( (a)<(b) ? (a) : (b) )
#define TEST_MAX(a, b)    ( (a)<(b) ? (b) : (a) )


/**
 * e_prog_01_Ret return ids.
 *
 */
typedef enum
{
PROG01_RET_UNKNOWN,    /**< Unknown return id.*/
PROG01_RET_SUCCESS,    /**< Success return id.*/
PROG01_RET_FAILED    /**< Failure return id.*/
} e_prog_01_Ret;



/**
* @struct st_Tsr_ArgList
* @brief structure to be used for program arguments parsing
* @note arguments other than those in this function will be ignored
* @note you will have to parse again if you need other info
*/
typedef struct
{
  char* p_InputFile; /**< Input file, or NULL if not set (to be freed by caller) */
  char* p_OutputFile; /**< Output file, or NULL if not set (to be freed by caller) */
  char* p_InputDirectory; /**< Input directory, or NULL if not set (to be freed by caller) */
  int v_IsHelpAsked; /**< Set if help prompt is asked */
  int v_IsVerbose; /**< Set if verbose mode is activated */
  char* p_InputDirectoryExtension; /**< Input directory extension, or NULL if not set (to be freed by caller) */
} st_Tsr_ArgList;

static int vg_GenerateIndividualFiles = 0;
static int vg_IsVerbose = 0;


/**
* @brief PROG free structure of program argument parsersing
* @param pp_ArgList will be filled with parsed arguments.
* @return none.
*/
void PROG01_ParseProgArgsFree(st_Tsr_ArgList* pp_ArgList )
{
  if(pp_ArgList)
  {
    if ( pp_ArgList->p_InputFile )
    {
      free( pp_ArgList->p_InputFile );
      pp_ArgList->p_InputFile = (char*)NULL;
    }
    if ( pp_ArgList->p_OutputFile )
    {
      free( pp_ArgList->p_OutputFile );
      pp_ArgList->p_OutputFile = (char*)NULL;
    }
    if ( pp_ArgList->p_InputDirectory )
    {
      free( pp_ArgList->p_InputDirectory );
      pp_ArgList->p_InputDirectory = (char*)NULL;
    }
    if ( pp_ArgList->p_InputDirectoryExtension )
    {
      free( pp_ArgList->p_InputDirectoryExtension );
      pp_ArgList->p_InputDirectoryExtension = (char*)NULL;
    }
  }
}

/**
* @brief program argument parser function
* @param argc number of arguments (at least 1).
* @param argv list of argument(s).
* @param pp_ArgList will be filled with parsed arguments.
* @return none.
*/
void PROG01_ParseProgArgs( int argc, char* argv[], st_Tsr_ArgList* pp_ArgList )
{
  /* Local variables */
  int vl_ArgIndex = 0;

  /* every parameters is compulsory */
  assert( argc && argv && pp_ArgList );

  vg_IsVerbose = 0;
  vg_GenerateIndividualFiles = 0;

  /* Initialize structure for args */
  pp_ArgList->p_InputFile = (char*)NULL;
  pp_ArgList->p_OutputFile = (char*)NULL;
  pp_ArgList->p_InputDirectory =  (char*)NULL;
  pp_ArgList->p_InputDirectoryExtension = (char*)NULL;
  pp_ArgList->v_IsHelpAsked = 0;
  pp_ArgList->v_IsVerbose = 0;

  /* Now that we're done with initializations, let's parse arguments */
  for ( vl_ArgIndex = 0; vl_ArgIndex < argc; vl_ArgIndex++ )
  {
    if ( !_stricmp( argv[ vl_ArgIndex ], "-i" ) ||
         !_stricmp( argv[ vl_ArgIndex ], "--infile" ) )
    {
      if( (vl_ArgIndex +1) >= argc)
      {
        printf( "a parmeter is missing\n");
      }
      else
      {
       /* check if it's not just followed by another option */
       if ( strncmp( argv[ ++vl_ArgIndex ], "-", 1 ) )
       {
         /* one input file only, others discarded */
         if ( !pp_ArgList->p_InputFile )
         {
           pp_ArgList->p_InputFile = ( char* ) malloc( strlen( argv[ vl_ArgIndex ] ) + 1 );
           strcpy( pp_ArgList->p_InputFile, argv[ vl_ArgIndex ] );
         }
       }
     }
    }
    else if ( !_stricmp( argv[ vl_ArgIndex ], "-o" ) ||
              !_stricmp( argv[ vl_ArgIndex ], "--outfile" ) )
    {
      if( (vl_ArgIndex +1) >= argc)
      {
        printf( "a parmeter is missing\n");
      }
      else
      {
       /* check if it's not just followed by another option */
       if ( strncmp( argv[ ++vl_ArgIndex ], "-", 1 ) )
       {
         /* one output file only, others discarded */
         if ( !pp_ArgList->p_OutputFile )
         {
           pp_ArgList->p_OutputFile = ( char* ) malloc( strlen( argv[ vl_ArgIndex ] ) + 1 );
           strcpy( pp_ArgList->p_OutputFile, argv[ vl_ArgIndex ] );
         }
       }
     }
    }
    else if ( !_stricmp( argv[ vl_ArgIndex ], "-r" ) ||
              !_stricmp( argv[ vl_ArgIndex ], "--indir" ) )
    {
      if( (vl_ArgIndex +1) >= argc)
      {
        printf( "a parmeter is missing\n");
      }
      else
      {
       /* check if it's not just followed by another option */
       if ( strncmp( argv[ ++vl_ArgIndex ], "-", 1 ) )
       {
         /* one input directory only, others discarded */
         if ( !pp_ArgList->p_InputDirectory )
         {
           pp_ArgList->p_InputDirectory = ( char* ) malloc( strlen( argv[ vl_ArgIndex ] ) + 1 );
           strcpy( pp_ArgList->p_InputDirectory, argv[ vl_ArgIndex ] );
         }
       }
     }
    }
    else if ( !_stricmp( argv[ vl_ArgIndex ], "-ext" ) ||
              !_stricmp( argv[ vl_ArgIndex ], "--ext" ) )
    {
      if( (vl_ArgIndex +1) >= argc)
      {
        printf( "a parmeter is missing\n");
      }
      else
      {
       /* check if it's not just followed by another option */
       if ( strncmp( argv[ ++vl_ArgIndex ], "-", 1 ) )
       {
         /* one input directory extension only, others discarded */
         if ( !pp_ArgList->p_InputDirectoryExtension )
         {
           pp_ArgList->p_InputDirectoryExtension = ( char* ) malloc( strlen( argv[ vl_ArgIndex ] ) + 1 );
           strcpy( pp_ArgList->p_InputDirectoryExtension, argv[ vl_ArgIndex ] );
         }
       }
     }
    }
    else if ( !_stricmp( argv[ vl_ArgIndex ], "-v" ) ||
              !_stricmp( argv[ vl_ArgIndex ], "--verbose" ) )
    {
      pp_ArgList->v_IsVerbose = vg_IsVerbose = 1;
    }
    else if ( !_stricmp( argv[ vl_ArgIndex ], "-h" ) ||
              !_stricmp( argv[ vl_ArgIndex ], "--help" ) )
    {
      /* For caller */
      pp_ArgList->v_IsHelpAsked = 1;
      vg_IsVerbose = 1;

      /* For PROG to print its help */
      printf( "This program enable to extract data in a file or in all files of a directory.\n"
              "The searched data are bewtween BeginDetectionPattern and EndDetectionPattern.\n"
              "You can also exclude for search data after BeginDetectionPattern that begin by PatternNotToFindAfterBeginDetectionPattern.\n\n"
              "Usage: %s\n"
              "\t-h, --help: display this help and exits\n"
              "\t-i, --infile [ file ]: input file\n"
			  "\t-o, --outfile [ file or repository path]: output file ou output-repository (if process asked on a repository)\n"
              "\t-r, --indir [ repository path ]: input directory\n"
              "\t-ext, --ext [ repository extension ]: input repository extension filter\n"
              "\t-v, --verbose: enable output on stdout\n\n"
			  "\tFile names to process must be like:  xxx_reso_???_x_???_xxx_format_???.gam\n"
			  "\tHere is an exemple : prefixe_name_reso_320_x_240_format_yuv422interleaved.gam\n"
			  "\tcolor format can be: rgb565, bgr565, rgb888, bgr888, yuv422interleaved, yuv420mbpackedsemiplanar, yuv420packedplanari420, RawBayer8bit, RawBayer12bit\n"
			  "\tFor raw colorformat, the number of status lines must also appear in the file name : xxx_hl_???_\n"
			  "\tHere is an exemple : frame_1_reso_3280_x_2464_hl_2_format_RawBayer12bit.gam\n", 
//              "\n**** Options specific to this test, if any, follows****\n\n",
              argv[ 0 ] );

      /* If any allocation was done, free memory */
      PROG01_ParseProgArgsFree(pp_ArgList );
      goto goto_end;
    }
    /* any other option is just ignored */
    else
    {
      continue;
    }
  }

goto_end :
  return;
}



/**
 * Method Get Extension
 * @param pp_FileName - FileName
 * @param pp_Ptr_Extension - Extension
 * @return  void
 */
void prog_01_GetExtension(char *pp_FileName, char **pp_Ptr_Extension)
{
  char *pl_Extension = (char *)NULL;
  unsigned short vl_StringLength = 0, vl_i = 0;
  unsigned char vl_IsThereAnExtension = 0;

  if(pp_FileName != NULL)
  {
    vl_StringLength = (unsigned int)strlen(pp_FileName);
    pl_Extension = pp_FileName + vl_StringLength;
    for(vl_i = 0; vl_i < vl_StringLength; vl_i++)
    {
      if(*pl_Extension == PROG01_SEPARATOR)
        break;

      if(*pl_Extension == '.')
      {
        vl_IsThereAnExtension = 1;
        break;
      }
      pl_Extension--;
    }
  }

  if(vl_IsThereAnExtension == 0)
  {
    pl_Extension = (char*)NULL;
  }

  *pp_Ptr_Extension = pl_Extension;
}


/**
 * Method Store Path And FileName
 * @param pp_FileNameWithoutPath - FileNameWithoutPath
 * @param pp_Path - Path
 * @param pp_FileName - FileName
 * @return  void
 */
void prog_01_StorePathAndFileName(char *pp_FileNameWithoutPath, char *pp_Path, char *pp_FileName)
{
  char *pl_Data  = (char *)NULL;
  unsigned short vl_StringLength  = 0;

  if(pp_FileName == NULL)
    return;
  if(pp_Path != NULL)
  {
    vl_StringLength  = (unsigned int)strlen(pp_Path);
    strcpy(pp_FileName, pp_Path);
    if(pp_FileNameWithoutPath != NULL)
    {
      pl_Data    =  pp_FileName;
      if(vl_StringLength != 0)
      {
        pl_Data    += vl_StringLength;
        if( *(pl_Data-1) != PROG01_SEPARATOR )
        {
          *pl_Data  =  PROG01_SEPARATOR;
          pl_Data    +=  1;
        }
      }
      strcpy(pl_Data, pp_FileNameWithoutPath);
    }
  }
  else
  {
    if(pp_FileNameWithoutPath != NULL)
    {
      pl_Data  = pp_FileName;
      strcpy(pl_Data, pp_FileNameWithoutPath);
    }
  }
}


/**
 * Method Remove Extension
 * @param pp_FileName - FileName
 * @return  void
 */
void prog_01_RemoveExtension(char *pp_FileName)
{
  char *pl_SearchPoint  = (char *)NULL;
  unsigned int  vl_i;
  unsigned short vl_StringLength = 0;
  unsigned char  vl_IsThereAnExtension = 0;

  if(pp_FileName == NULL)
    return;
  vl_StringLength =  (unsigned int)strlen(pp_FileName);
  pl_SearchPoint  =  pp_FileName + vl_StringLength;
  for(vl_i = 0; vl_i < vl_StringLength; vl_i++)
  {
    if(*pl_SearchPoint  == PROG01_SEPARATOR)
      break;

    if(*pl_SearchPoint  == '.')
    {
      vl_IsThereAnExtension = 1;
      break;
    }
    
    pl_SearchPoint--;
  }

  if(vl_IsThereAnExtension == 1)
  {
    *pl_SearchPoint = '\0';
  }
}


/**
 * Method Get Path
 * @param pp_FileNameWithPath - FileNameWithPath
 * @param pp_Path - Path
 * @return  void
 */
void prog_01_GetPath(char *pp_FileNameWithPath, char *pp_Path)
{
  char *pl_Data  = (char *)NULL, *pl_Handle = (char *)NULL;
  unsigned short vl_StringLength    = 0;
  unsigned short vl_i = 0, vl_ii = 0;

  if(pp_Path == NULL)
  {
    return;
  }
  if(pp_FileNameWithPath == NULL)
  {
    *pp_Path = 0;
    return;
  }

  vl_StringLength  = (unsigned int)strlen(pp_FileNameWithPath);
  pl_Data    = pp_FileNameWithPath;
  pl_Handle  = pp_FileNameWithPath;
  for(vl_i = vl_StringLength; vl_i > 0; vl_i--)
  {
    if(pl_Data[vl_i] == PROG01_SEPARATOR)
    {
      vl_ii = 0;
      while( (pl_Handle + vl_ii) != (pl_Data + vl_i) )
      {
        pp_Path[vl_ii] = pl_Handle[vl_ii];
        vl_ii++;
      }
      pp_Path[vl_ii]=0;
      return;
    }
  }
  
  *pp_Path = 0;
}

/**
* itoa Convert an integer to a string.
* la string renvoyée doit contenir un char de plus que le nombre de char gardé car la fonction ajoute le caractere NULL de la fin de la string
* @param value Number to be converted.
* @param string result.
* @param radix Base of value; must be in the range 2 – 36.
* @return pointer to string.
*/

char* prog_01__itoa(int value, char *string, int radix)
{
  int vl_value = value, vl_y = 0, vl_reste = 0, vl_cpt = 0;
  char al_HEX[] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
  char *pl_Data;

  if(string==NULL || radix>16 || radix<2)
  {
    string[vl_cpt] = 0;
    return string;
  }

  if(vl_value != 0)
  {
    if(value < 0)
    {
      vl_value = - value;
    }

    while(vl_value!=0)
    {
      vl_y = vl_value;
      vl_value = (int) (vl_y / radix);
      vl_reste = vl_y - vl_value * radix;
      string[vl_cpt] = al_HEX[vl_reste];
      vl_cpt++;
    }
  }
  else
  {
    string[vl_cpt] = al_HEX[0];
    vl_cpt++;
  }

  if(value < 0)
  {
    string[vl_cpt] = '-';
    vl_cpt++;
  }

  string[vl_cpt] = 0; // final character for string

  //inverse the string sens 

  pl_Data = (char*) malloc(vl_cpt + 1); // +1 for null character
  strcpy(pl_Data, string);

  for(vl_y = 0; vl_y < vl_cpt; vl_y++)
  {
    string[vl_y] = pl_Data[(vl_cpt-1) - vl_y];
  }

  free(pl_Data);  

  return string;
}



/**
 * Method Get the size of a file
 * @param pp_File - File
 * @param pp_FileName - FileName
 * @return Size of the file
 */
unsigned int prog_01_GetFileSize( FILE* pp_File, char* pp_FileName)
{
  FILE* pl_File = (FILE*)NULL;
  unsigned int vl_Size = 0;
  int vl_Position = 0;
  unsigned char vl_HasFileBeenOpened = 0;

  // check that at least one parameter is not null
  if ( ( pp_FileName == NULL ) && ( pp_File == NULL ) )
  {
    return vl_Size;
  }

  if ( pp_FileName != NULL )
  {
    if ( ( pl_File = fopen( pp_FileName, "rb" ) ) == NULL )
    {
      return vl_Size;
    }
    vl_HasFileBeenOpened = 1;
  }
  else
  {
    pl_File = pp_File;
  }

  vl_Position = ftell( pl_File );

  if ( fseek( pl_File, 0, SEEK_END ) == 0 )
  {
    vl_Size = ftell( pl_File );
  }
  else
  {
    return vl_Size;
  }

  if ( fseek( pl_File, vl_Position, SEEK_SET ) != 0 )
  {
    vl_Size = 0;
    return vl_Size;
  }
  
  if ( vl_HasFileBeenOpened == 1 )
  {
    fclose( pl_File );
  }

  return vl_Size;
}


/**
 * Method Get File Name Without Path
 * @param pp_FileName - FileName
 * @param pp_Ptr_FileNameWithoutPath - FileNameWithoutPath
 * @return  void
 */
void prog_01_GetFileNameWithoutPath(char *pp_FileName, char **pp_Ptr_FileNameWithoutPath)
{
  char *pl_FileNameWithoutPath = (char *)NULL;
  unsigned short vl_StringLength = 0, vl_i = 0;

  if(pp_FileName != NULL)
  {
    vl_StringLength = (unsigned int)strlen(pp_FileName);
    pl_FileNameWithoutPath = pp_FileName + vl_StringLength;
    for(vl_i = 0; vl_i < vl_StringLength; vl_i++)
    {
      if(*pl_FileNameWithoutPath == PROG01_SEPARATOR)
      {
        pl_FileNameWithoutPath++;
        break;
      }
      pl_FileNameWithoutPath--;
    }
  }

  *pp_Ptr_FileNameWithoutPath = pl_FileNameWithoutPath;
}



/**
 * Method Store Path Without FileName
 * @param pp_FileName - FileName
 * @param pp_Path - Path
 * @return  void
 */
void prog_01_StorePathWithoutFileName(char *pp_FileName, char *pp_Path)
{
  char *pl_Path = pp_Path;
  char *pl_Data = (char *)NULL;
  unsigned short vl_StringLength    = 0;
  int vl_i = 0;
  unsigned char  vl_IsThereAPath = 0;

  if(pp_FileName== NULL)
    return;
  strcpy(pl_Path, pp_FileName);
  vl_StringLength  = (unsigned int)strlen(pl_Path);

  pl_Data = pl_Path + vl_StringLength;
  for(vl_i = 0; vl_i < vl_StringLength; vl_i++)
  {
    if( *pl_Data == PROG01_SEPARATOR )
    {
      *pl_Data = '\0';
      vl_IsThereAPath = 1;
      break;
    }
    pl_Data--;
  }
  if(vl_IsThereAPath == 0)
  {
    *pl_Data = '\0';
  }
}



/**
 * Method Store Modified file name of a file (pp_NewFileName can be == pp_FileName)
 * @param pp_FileName - FileName
 * @param pp_NewFileName - NewFileName
 * @param pp_StringBefore - StringBefore
 * @param pp_StringAfter - StringAfter
 * @param vp_MustAddBefore - MustAddBefore
 * @param vp_MustAddAfter - MustAddAfter
 * @return  void
 */
void prog_01_StoreModifiedFileName(char *pp_FileName, char *pp_NewFileName, char *pp_StringBefore, char *pp_StringAfter, char vp_MustAddBefore, char vp_MustAddAfter)
{
  char  *pl_Data = (char *)NULL;
  char  *pl_Path = (char *)NULL;
  char  *pl_Extension = (char *)NULL;
  char  *pl_FileNameWithoutPath = (char *)NULL, *pl_FileNameWithoutPathHandle = (char *)NULL;
  char  *pl_FileName = (char *)NULL;
  unsigned short vl_StringLength = 0;
  
  if(pp_NewFileName == NULL)
    return;

  if(pp_FileName == NULL)
    return;
  vl_StringLength =  (unsigned int)strlen(pp_FileName);
  pl_Path = (char*)malloc(vl_StringLength + 1);
  if(pl_Path == NULL)
    return;

  pl_FileName = (char*)malloc(vl_StringLength + 1);
  if(pl_FileName == NULL)
    return;
  strcpy(pl_FileName, pp_FileName);

  prog_01_StorePathWithoutFileName(pl_FileName, pl_Path);

  pl_Data = pp_NewFileName;
  if(pl_Path[0] != 0)
  {
    strcpy(pl_Data, pl_Path);
    pl_Data = pl_Data + strlen(pl_Data);
    free(pl_Path);
    *pl_Data = PROG01_SEPARATOR;
    pl_Data++;
  }

  if(vp_MustAddBefore == 1)
  {
    if(pp_StringBefore != NULL)
    {
      strcpy(pl_Data, pp_StringBefore);
      pl_Data = pl_Data + strlen(pl_Data);
    }
  }
  
  prog_01_GetFileNameWithoutPath(pl_FileName, &pl_FileNameWithoutPath);
  pl_FileNameWithoutPathHandle = (char*)malloc(strlen(pl_FileNameWithoutPath) + 1);
  if(pl_FileNameWithoutPathHandle == NULL)
    return;
  strcpy(pl_FileNameWithoutPathHandle, pl_FileNameWithoutPath);
  prog_01_RemoveExtension(pl_FileNameWithoutPathHandle);

  strcpy(pl_Data, pl_FileNameWithoutPathHandle);
  pl_Data = pl_Data + strlen(pl_Data);

  if(vp_MustAddAfter == 1)
  {
    if(pp_StringAfter != NULL)
    {
      strcpy(pl_Data, pp_StringAfter);
      pl_Data = pl_Data + strlen(pl_Data);
    }
  }

  prog_01_GetExtension(pl_FileName, &pl_Extension);
  if(pl_Extension != NULL)
  {
    strcpy(pl_Data, pl_Extension);
    pl_Data = pl_Data + strlen(pl_Data);
  }

  free(pl_FileName);
  free(pl_FileNameWithoutPathHandle);
}

/**
 * Method Store file name with new extension 
 * @param pp_FileName - FileName
 * @param pp_NewFileName - NewFileName
 * @param pp_Extension - Extension
 * @return  void
 */
void prog_01_StoreFileNameWithNewExtension(char *pp_FileName, char *pp_NewFileName, char *pp_Extension)
{
  char  *pl_SearchPoint;
  unsigned int  vl_i;
  unsigned short vl_StringLength = 0;
  unsigned char  vl_IsThereAnExtension = 0;

  if(pp_FileName != NULL)
  {
    strcpy(pp_NewFileName, pp_FileName);
    vl_StringLength =  (unsigned int)strlen(pp_NewFileName);
    pl_SearchPoint  =  pp_NewFileName + vl_StringLength;
    for(vl_i = 0; vl_i < vl_StringLength; vl_i++)
    {
      if(*pl_SearchPoint  == PROG01_SEPARATOR)
        break;

      if(*pl_SearchPoint  == '.')
      {
        vl_IsThereAnExtension = 1;
        break;
      }

      pl_SearchPoint--;
    }

    if(vl_IsThereAnExtension == 0)
    {
      pl_SearchPoint = pp_FileName + vl_StringLength;
      *pl_SearchPoint = '.';
    }
    pl_SearchPoint++;

    strcpy(pl_SearchPoint, pp_Extension);
  }
}



/**
* itoa_GetSizeString
* @param value Number to be converted.
* @param radix Base of value; must be in the range 2 – 36.
* @return string length.
*/

int prog_01__itoa_GetSizeString(int value, int radix)
{
  char string[32+1];
  char *pl_Data = (char *)NULL;
  int  vl_Result = 0;

  pl_Data = prog_01__itoa(value, string, radix);
  if(pl_Data != NULL)
  {
    vl_Result = (int)strlen(pl_Data);
  }
  return vl_Result;
}



/**
 * function Swap Word
 * @param pp_Buffer - Buffer
 * @param vp_SizeDataInWords - SizeDataInWords
 * @return  void
 */
void prog_01_SwapWord(void* pp_Buffer, unsigned int vp_SizeDataInWords)
{
  unsigned int    vl_i;
  unsigned short* pl_Buffer = (unsigned short*)pp_Buffer;
  for(vl_i = 0; vl_i < vp_SizeDataInWords; vl_i++)
  {
    pl_Buffer[vl_i] = TEST_SWAP_WORD(pl_Buffer[vl_i]);
  }
}

/**
  * function Inflate data
  * @param pp_Destination - Destination
  * @param pp_Source - Source
  * @param vp_NbOfWordsToInflate - NbOfWordsToInflate
  * @return  void
 */
void prog_01_Inflate(void *pp_Destination, void *pp_Source, unsigned int vp_NbOfWordsToInflate)
{
  unsigned int vl_i ;
  unsigned short *pl_Destination =  (unsigned short*)pp_Destination;
  unsigned short *pl_Source  =  (unsigned short*)pp_Source;
  for( vl_i = vp_NbOfWordsToInflate ; vl_i > 0; vl_i --)
  {
    pl_Destination[(vl_i*2)-1] =  (pl_Source[vl_i-1]>>8)&0xff;
    pl_Destination[(vl_i*2)-2] =  pl_Source[vl_i-1]&0xff;
  }
}

 /**
  * function Deflate data
  * @param pp_Destination - Destination
  * @param pp_Source - Source
  * @param vp_NbOfWordsToIDeflate - NbOfWordsToIDeflate
  * @return  void
 */
void prog_01_Deflate(void *pp_Destination, void *pp_Source, unsigned int vp_NbOfWordsToIDeflate)
{
  unsigned short vl_i;
  unsigned short *pl_Destination =  (unsigned short*)pp_Destination;
  unsigned short *pl_Source  =  (unsigned short*)pp_Source;

  for (vl_i=0; vl_i < (vp_NbOfWordsToIDeflate/2); vl_i++)
  {
    pl_Destination[vl_i] = (pl_Source[2*vl_i]<<8) + (pl_Source[2*vl_i+1]&0xff);
  }

  if(vp_NbOfWordsToIDeflate%2==1)
  {
    pl_Destination[vp_NbOfWordsToIDeflate/2] = pl_Source[vp_NbOfWordsToIDeflate-1]<<8;
  }
}

/**
 * function Copy in a file
 * @param pp_FileDest - FileDest
 * @param vp_PositionDest - PositionDest
 * @param pp_FileSource - FileSource
 * @param vp_PositionSource - PositionSource
 * @return  Error Code
 */
e_prog_01_Ret prog_01_MakeFileCopy(FILE* pp_FileDest, int vp_PositionDest, FILE* pp_FileSource, int vp_PositionSource)
{
  e_prog_01_Ret vl_EC = PROG01_RET_SUCCESS;
  unsigned char  *pl_Buffer = (unsigned char *)NULL;
  unsigned int vl_BufferSize = 3000;
  unsigned int vl_SizeReadData = vl_BufferSize;

  pl_Buffer = (unsigned char*)malloc(vl_BufferSize);
  if(pl_Buffer == NULL)
  {
    vl_EC = PROG01_RET_FAILED;
    return vl_EC;
  }

  fseek(pp_FileSource, vp_PositionSource, SEEK_SET);
  while(vl_SizeReadData == vl_BufferSize)
  {
    vl_SizeReadData = (unsigned int)fread(pl_Buffer, sizeof(char), vl_BufferSize, pp_FileSource);
    fwrite((void*) pl_Buffer, sizeof(char), vl_SizeReadData, pp_FileDest);
  }

  free(pl_Buffer);

  return vl_EC;
}


/**
 * Method Copy in a file
 * @param pp_FileDest - FileDest
 * @param vp_PositionDest - PositionDest
 * @param pp_FileSource - FileSource
 * @param vp_PositionSource - PositionSource
 * @param vp_SizeDataToCopy - SizeDataToCopy
 * @return  Error Code
 */
e_prog_01_Ret prog_01_MakePartialFileCopy(FILE* pp_FileDest, int vp_PositionDest, FILE* pp_FileSource, int vp_PositionSource, unsigned int vp_SizeDataToCopy)
{
  e_prog_01_Ret vl_EC = PROG01_RET_SUCCESS;
  unsigned char  *pl_Buffer;
  unsigned int vl_BufferSize = 3000;
  unsigned int vl_SizeReadData = vl_BufferSize;
  unsigned int vl_SizeTotalReadData = 0;

  pl_Buffer = (unsigned char*)malloc(vl_BufferSize);
  if(pl_Buffer == NULL)
  {
    vl_EC = PROG01_RET_FAILED;
    return vl_EC;
  }

  fseek(pp_FileDest, vp_PositionDest, SEEK_SET);
  fseek(pp_FileSource, vp_PositionSource, SEEK_SET);
  while(vl_SizeTotalReadData != vp_SizeDataToCopy)
  {
    vl_SizeReadData = (unsigned int)fread(pl_Buffer, sizeof(char), vl_BufferSize, pp_FileSource);
    if(vp_SizeDataToCopy <= vl_SizeReadData)
    {
      vl_SizeReadData      =  vp_SizeDataToCopy;
      vl_SizeTotalReadData  =  vp_SizeDataToCopy;
    }
    else
    vl_SizeTotalReadData += vl_SizeReadData;
    fwrite((void*) pl_Buffer, sizeof(char), vl_SizeReadData, pp_FileDest);
  }

  free(pl_Buffer);

  return vl_EC;
}


/**
 * function check input
 * @return void.
 */
int prog_01_filter_for_scan(const struct dirent *dir)
{
#ifdef WIN32
	return 1;
#else
	if(
     (strstr(dir->d_name, "."))
     &&
     (strcmp(dir->d_name,".")!=0)
     &&
     (strcmp(dir->d_name,"..")!=0)
     &&
     (strcmp(dir->d_name,".svn")!=0)
     )
       return 1;
     else
       return 0;
#endif
}


/**
 * String To Upper.
 * @param pp_String String.
 */
void prog_01_StringToUpper(char *pp_String)
{
  int vl_i = 0;
  if(pp_String)
  {
    for(vl_i = 0; pp_String[vl_i]; vl_i++)
    {
      pp_String[vl_i] = toupper(pp_String[vl_i] );
    }
  }
}

/**
 * String To Lower.
 * @param pp_String String.
 */
void prog_01_StringToLower(char *pp_String)
{
  int vl_i = 0;
  if(pp_String)
  {
    for(vl_i = 0; pp_String[vl_i]; vl_i++)
    {
      pp_String[vl_i] = tolower(pp_String[vl_i] );
    }
  }
}


/**
 * prog_01_ProcessFile.
 * @param pp_FileName FileName
 * @param pp_OutputFileName OutputFileName
*/
void prog_01_ProcessFile(char *pp_FileName, char *pp_OutputFileName)
{
	char al_prefix_OutputFileName[] = "output_";
	char *pl_OutputFileName = (char *)NULL;

	if(vg_GenerateIndividualFiles != 0)
	{
		// pp_OutputFileName is a path in this case
		char pl_NewExtension[] = "bmp";
		char* pl_FileNameWithoutPath = (char *)NULL;
		prog_01_GetFileNameWithoutPath(pp_FileName, &pl_FileNameWithoutPath);
		pl_OutputFileName = (char*)malloc(strlen(pp_OutputFileName) + strlen(al_prefix_OutputFileName) + strlen(pl_FileNameWithoutPath) + strlen(pl_NewExtension) + 1 + 1);
		strcpy(pl_OutputFileName, pp_OutputFileName);
		if(pp_OutputFileName[strlen(pp_OutputFileName)] != '\\')
		{
			strcat(pl_OutputFileName, "\\");
		}
		strcat(pl_OutputFileName, al_prefix_OutputFileName);
		strcat(pl_OutputFileName, pl_FileNameWithoutPath);
		prog_01_StoreFileNameWithNewExtension(pl_OutputFileName, pl_OutputFileName, pl_NewExtension);
		pp_OutputFileName = pl_OutputFileName;
	}
	
	convertImageToBMPProcess(pp_FileName, pp_OutputFileName);
	
	if(pl_OutputFileName != NULL)
	{
		free(pl_OutputFileName);
	}
}

/**
 * Scan Directory.
 * @param pp_DirectoryPath Input directory path.
 * @param pp_InputDirectoryExtension Input Directory Extension.
 * @param pp_OutputFileName OutputFileName
*/
void prog_01_ScanDirectory(char *pp_DirectoryPath, char *pp_InputDirectoryExtension, char *pp_OutputFileName)
{
  struct dirent **pl_namelist=(struct dirent **)NULL;
  char *pl_DisplayName = (char *)NULL;
  char *pl_FileNameWithPath = (char *)NULL;
  int vl_nbFiles = -1;
  int vl_i = 0;
  int vl_DisplayNameLength = 0,  vl_PathLength = 0, vl_NameLength = 0;
  char *pl_Extension = (char *)NULL;
  char *pl_FileNameWithPath_copy = (char *)NULL;
  char *pl_InputDirectoryExtension_copy = (char *)NULL;
  char *pl_FileNameToProcess = (char *)NULL;
#ifdef WIN32
    int iHandle = 0;
    int iResult = 0;
    struct _finddata_t s_find;
    char al_Extension[] = "*";
    char *pl_InputDirectoryExtension = (char *)NULL;
	char sl_InputDirectoryExtension[32];
	char sl_PathOrigine[512];
	char *pl_PathOrigine = (char *)NULL;
#endif
    if(pp_DirectoryPath == NULL)
    {
        printf("error DirectoryPath is null\n");
        return;
    }

	vg_GenerateIndividualFiles = 1;

#ifdef WIN32
		pl_PathOrigine = _getcwd(sl_PathOrigine, sizeof(sl_PathOrigine)/sizeof(char));
		if(vg_IsVerbose)
			printf("\n PathOrigine %s\n", pl_PathOrigine);

	iResult = _chdir (pp_DirectoryPath); 
    if(iResult != 0)
    {
        printf("error _chdir to %s, result: %d\n", pp_DirectoryPath, iResult);
        return;
    }
	else
	{
		if(vg_IsVerbose)
			printf("\n chdir to %s\n", pp_DirectoryPath);
	}

    {
        char *pl_Path = (char *)NULL;
        pl_Path = malloc(strlen(pp_OutputFileName) + 1);
        prog_01_GetPath(pp_OutputFileName, pl_Path);
        //printf("\nOutput of the program will be in %s\n\n", pl_Path);
        printf("\nOutput of the program will be in %s\n\n", pp_OutputFileName);
        if(pl_Path)
            free(pl_Path);
    }
	
	if(pp_InputDirectoryExtension)
    {
        pl_InputDirectoryExtension = pp_InputDirectoryExtension;
        //printf("ScanDirectory_findfirst with extension %s .\n", pl_InputDirectoryExtension);
    }
    else
    {
        pl_InputDirectoryExtension = al_Extension;
        //printf("ScanDirectory _findfirst with extension replaced by * (for all files).\n");
    }
	sl_InputDirectoryExtension[0]='*';
	if(pl_InputDirectoryExtension[0] == '.')
	{
		strcpy(&sl_InputDirectoryExtension[1], pl_InputDirectoryExtension);
	}
	else
	{
		sl_InputDirectoryExtension[1]='.';
		strcpy(&sl_InputDirectoryExtension[2], pl_InputDirectoryExtension);
	}
	//iHandle = _findfirst (pl_InputDirectoryExtension, &s_find);
	iHandle = _findfirst (sl_InputDirectoryExtension, &s_find);

	if ((iHandle == -1))
    {
        //printf("ScanDirectory no files found.\n");
    }
    else
    {    
        vl_nbFiles = 1;
        //printf("ScanDirectory file name :%s\n", s_find.name);
        while (!(iResult == -1))
        {
            iResult = _findnext (iHandle, &s_find);
            if(iResult != -1)
            {
                //printf("ScanDirectory file name :%s\n", s_find.name);
                vl_nbFiles++;
            }
        }
        _findclose (iHandle);
    }
#else
  vl_nbFiles = scandir(pp_DirectoryPath, &pl_namelist, prog_01_filter_for_scan, 0);
#endif
  if(vg_IsVerbose)
      printf("Number of files found in directory %d\n", vl_nbFiles);
  if(vl_nbFiles == -1)
  {
    if(vg_IsVerbose)
        printf("\nNo file found in directory %s\n", pp_DirectoryPath);
    return;
  }

  if(pp_InputDirectoryExtension)
  {
    if(pp_InputDirectoryExtension[0] == '.')
    {
      pl_InputDirectoryExtension_copy = (char*)malloc(strlen(pp_InputDirectoryExtension) + 1);
      strcpy(pl_InputDirectoryExtension_copy, pp_InputDirectoryExtension);
    }
    else
    {
      pl_InputDirectoryExtension_copy = (char*)malloc(strlen(pp_InputDirectoryExtension) + 2);
      pl_InputDirectoryExtension_copy[0] = '.';
      strcpy(pl_InputDirectoryExtension_copy+1, pp_InputDirectoryExtension);
    }
    prog_01_StringToUpper(pl_InputDirectoryExtension_copy);
    //printf("extension filter: %s\n", pl_InputDirectoryExtension_copy);
  }

 for(vl_i=0;vl_i<vl_nbFiles;vl_i++)
 {
     char *pl_tmp_name = (char *)NULL;
#ifdef WIN32
    if(vl_i == 0)
    {
	    //iHandle = _findfirst (pl_InputDirectoryExtension, &s_find);
	    iHandle = _findfirst (sl_InputDirectoryExtension, &s_find);
        if ((iHandle != -1))
        {
            pl_tmp_name = s_find.name;
        }
    }
    else
    {
        iResult = _findnext (iHandle, &s_find);
        if ((iResult != -1))
        {
            pl_tmp_name = s_find.name;
        }
    }
    //printf("from directory, file processed :%s\n", pl_tmp_name);
    if(vl_i == (vl_nbFiles - 1))
    {
        _findclose (iHandle);
    }

#else
     pl_tmp_name = pl_namelist[vl_i]->d_name;
#endif
    if(pl_tmp_name == NULL)
    {
        printf("error, file name empty\n");
        return;
    }
    vl_DisplayNameLength = (int)strlen(pl_tmp_name)+1;
    pl_DisplayName = (char*)malloc(vl_DisplayNameLength); 
    strcpy(pl_DisplayName, pl_tmp_name);
    if( (pp_DirectoryPath[strlen(pp_DirectoryPath)] != PROG01_SEPARATOR) )
    {
        vl_PathLength = (int)strlen(pp_DirectoryPath) + 1;
    }
    else
    {
        vl_PathLength = (int)strlen(pp_DirectoryPath);
    }
    vl_NameLength = vl_DisplayNameLength + vl_PathLength + 1;
    pl_FileNameWithPath = (char*)malloc(vl_NameLength);
    strcpy(pl_FileNameWithPath, pp_DirectoryPath);
    if( (pp_DirectoryPath[strlen(pp_DirectoryPath)] != PROG01_SEPARATOR) )
    {
        strcat(pl_FileNameWithPath, "\\");
    }
    strcat(pl_FileNameWithPath, pl_tmp_name);
    if(pl_namelist)
    {
        if(pl_namelist[vl_i])
        {
            free(pl_namelist[vl_i]);
        }
    }

    if(pl_FileNameWithPath_copy)
    {
      free(pl_FileNameWithPath_copy);
    }
    pl_FileNameWithPath_copy = (char*)malloc(strlen(pl_FileNameWithPath) + 1);
    strcpy(pl_FileNameWithPath_copy, pl_FileNameWithPath);
    prog_01_StringToUpper(pl_FileNameWithPath_copy);

    prog_01_GetExtension(pl_FileNameWithPath_copy,  &pl_Extension);
    pl_FileNameToProcess = NULL;
    if(pl_Extension)
    {
      if(pl_InputDirectoryExtension_copy)
      {
        if(strcmp(pl_Extension, pl_InputDirectoryExtension_copy) == 0)
        {
            if(vg_IsVerbose)
                printf("found file with good extension: %s\n", pl_FileNameWithPath);
            pl_FileNameToProcess = pl_FileNameWithPath;
        }
        else
        {
            if(vg_IsVerbose)
                printf("file with bad extension: %s\n", pl_FileNameWithPath);
        }
      }
      else
      {
        if(vg_IsVerbose)
            printf("no extension specified so file must be processed: %s\n", pl_FileNameWithPath);
        pl_FileNameToProcess = pl_FileNameWithPath;
      }
    }
    else
    {
        if(vg_IsVerbose)
            printf("file with no extension: %s\n", pl_FileNameWithPath);
        if(pp_InputDirectoryExtension == NULL)
        {
            if(vg_IsVerbose)
                printf("file that must be processed: %s\n", pl_FileNameWithPath);
            pl_FileNameToProcess = pl_FileNameWithPath;
        }
        else
        {
            if(vg_IsVerbose)
                printf("file that must not be processed: %s\n", pl_FileNameWithPath);
        }
    }

    if(pl_FileNameToProcess)
    {
#ifdef WIN32
	iResult = _chdir (pl_PathOrigine);
	if(iResult != 0)
    {
        printf("error _chdir to %s, result: %d\n", pp_DirectoryPath, iResult);
        return;
    }
#endif
	prog_01_ProcessFile(pl_FileNameToProcess, pp_OutputFileName);
#ifdef WIN32
	iResult = _chdir (pp_DirectoryPath); 
    if(iResult != 0)
    {
        printf("error _chdir to %s, result: %d\n", pp_DirectoryPath, iResult);
        return;
    }
#endif
    }
  }

  if(pl_namelist)
  {
    free(pl_namelist);
  }
  if(pl_InputDirectoryExtension_copy)
  {
    free(pl_InputDirectoryExtension_copy);
  }
  if(pl_FileNameWithPath_copy)
  {
    free(pl_FileNameWithPath_copy);
  }
}


/**
* usage.
* @param pp_ProgramName ProgramName.
* @return void.
*/
void usage(char *pp_ProgramName)
{
//    fprintf(stderr, "\nUsage:\n");
//    fprintf(stderr, "%s has no other specific usage)\n", pp_ProgramName);
}

/**
* main.
* @param argc argc.
* @param argv argv.
* @return error code.
*/
int main(int argc, char *argv[])
{
  char* pl_ProgramName = (char *)NULL, *pl_FileNameInputWithoutPath = (char *)NULL;
  int vl_Result = 0;
  time_t result;
  char *pl_ProgName = (char *)NULL;
  st_Tsr_ArgList sl_ArgList;
  char *pl_InputDirectory = (char *)NULL;
  char al_InputDirectory_default[] = ".";
  char *pl_InputDirectoryExtension = (char *)NULL;
  char *pl_OutputFileName = (char *)NULL;
/*
  //convertImageToBMP();
  BMP_Zoom();
  ALGO_Log_0("\n\nType a key to exit\n");
  getc(stdin);
  return 0;
*/
  result = time(NULL);
  prog_01_GetFileNameWithoutPath(argv[0], &pl_ProgName);

  fprintf(stderr, "\n\n\n*****************************\n\n"
                  "Program name: %s\n"
                  "Build: %s\n"
                  "Copyright (C) ST-Ericsson SA 2010. All rights reserved\n"
				  "This code is ST-Ericsson proprietary and confidential.\n"
				  "Any use of the code for whatever purpose is subject to\n"
				  "specific written permission of ST-Ericsson SA.\n\n"
				  "This program enables to convert images into bmp.\n\n" 
				  "Web site: http://www.stericsson.com\n\n", pl_ProgName, asctime(localtime(&result)) );
  fprintf(stderr, "******************************************\n\n");

  PROG01_ParseProgArgs( argc, argv, &sl_ArgList);
  if (sl_ArgList.v_IsHelpAsked)
  {
    prog_01_GetFileNameWithoutPath(argv[0], &pl_ProgramName);
    usage(pl_ProgramName);
    goto goto_end;
  }

  if(sl_ArgList.p_InputDirectory)
  {
    pl_InputDirectory = sl_ArgList.p_InputDirectory;
    if(vg_IsVerbose)
        printf("InputDirectory : %s\n", pl_InputDirectory);
  }
  else
  {
    pl_InputDirectory = al_InputDirectory_default;
    if(sl_ArgList.p_InputFile == NULL)
    {
        if(vg_IsVerbose)
            printf("InputDirectory not specified so \".\" is used.\n");
    }
  }

  if(sl_ArgList.p_InputDirectoryExtension)
  {
    pl_InputDirectoryExtension = sl_ArgList.p_InputDirectoryExtension;
    if(vg_IsVerbose)
        printf("InputDirectoryExtension : %s\n", pl_InputDirectoryExtension);
  }
  else
  {
    pl_InputDirectoryExtension = (char*)NULL;
    if(vg_IsVerbose)
        printf("InputDirectoryExtension not specified so \".\" is used.\n");
  }

  if(sl_ArgList.p_OutputFile)
  {
    pl_OutputFileName = (char*)malloc(strlen(sl_ArgList.p_OutputFile) + 1);
    strcpy(pl_OutputFileName, sl_ArgList.p_OutputFile);
  }
  else
  {
    char al_suffix_OutputFileName_default[] = "_output.txt";
    pl_OutputFileName = (char*)malloc(strlen(pl_ProgName) + strlen(al_suffix_OutputFileName_default) + 1);
    strcpy(pl_OutputFileName, pl_ProgName);
    strcpy(pl_OutputFileName + strlen(pl_ProgName), al_suffix_OutputFileName_default);
  }
  if(vg_IsVerbose)
      printf("Output File Name : %s\n", pl_OutputFileName);

  if(sl_ArgList.p_InputFile)
  {
    if(vg_IsVerbose)
        printf("Process one file: %s\n", sl_ArgList.p_InputFile);
    prog_01_ProcessFile(sl_ArgList.p_InputFile, pl_OutputFileName);
  }
  else
  {
    if(vg_IsVerbose)
        printf("Process one directory: %s\n", pl_InputDirectory);
    prog_01_ScanDirectory(pl_InputDirectory, pl_InputDirectoryExtension, pl_OutputFileName);
  }

goto_end :
  PROG01_ParseProgArgsFree(&sl_ArgList);
  if(pl_OutputFileName)
  {
    free(pl_OutputFileName);
  }
  printf("\nEnd of processing.\n");
  //printf("\nYou can now type a character to exit.\n");  getchar();
  return 0;
}
