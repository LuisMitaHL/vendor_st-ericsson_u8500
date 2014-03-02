/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#define _CNAME_ Vfm_jpegdec
#include <string.h>
#include <JPEGDec_ARMNMF_ParamAndConfig.h>

JPEGDec_ARMNMF_ParamAndConfig_CLASS::JPEGDec_ARMNMF_ParamAndConfig_CLASS()
{
	//printf("\n Inside constructor :: JPEGDec_ARMNMF_ParamAndConfig_CLASS \n");
    OMX_U8 cnt;
    horizontal_offset = 0;
    vertical_offset = 0;
    frameHeight = 480;
    frameWidth = 640;
    cropWindowHeight = 0;
    cropWindowWidth = 0;
    nbComponents = 0;
    nbScanComponents =0;
    downsamplingFactor = DOWNSAMPLING_FACTOR_1;
    restartInterval = 0;
    mode = SEQUENTIAL_JPEG;
    IdY = IdCb = IdCr = 0;
    startSpectralSelection = 0;
    endSpectralSelection =0;
    successiveApproxPosition = 0;

    for(cnt =0;cnt<MAX_NUMBER_COMPONENTS;cnt++)
	{
		samplingFactors[cnt].hSamplingFactor = 0;
		samplingFactors[cnt].vSamplingFactor = 0;
		//Proper initialization needed
		components[cnt].componentId =0xff;
		components[cnt].DCHuffmanTableDestinationSelector =0xff;
		components[cnt].ACHuffmanTableDestinationSelector =0xff;
		components[cnt].quantizationTableDestinationSelector =0xff;
		components[cnt].componentSelector =0;
    }

    for(cnt=0;cnt<NUMBER_QUANT_TABLES;cnt++)
	{
		memset(quantizationTables[cnt].QuantizationTable,0,64*2);
    }

    for(cnt=0;cnt<MAX_NUMBER_HUFFMAN_TABLES;cnt++)
	{
#ifdef __JPEGDEC_SOFTWARE
		memset(DCHuffmanTable[cnt].DCHuffmanBits,0,16*2);
		memset(DCHuffmanTable[cnt].DCHuffmanVal,0,12*2);
		memset(ACHuffmanTable[cnt].ACHuffmanBits,0,16*2);
		memset(ACHuffmanTable[cnt].ACHuffmanVal,0,256*2);

///DC NEW MEMBERS
		memset(DCHuffmanTable[cnt].MaxCode,0,18*4);
        memset(DCHuffmanTable[cnt].ValOffset,0,17*4);
        memset(DCHuffmanTable[cnt].Look_Nbits,0,256*2);
        memset(DCHuffmanTable[cnt].Look_Sym,0,256*2);
////AC NEW MEMBERS
		memset(ACHuffmanTable[cnt].MaxCode,0,18*4);
        memset(ACHuffmanTable[cnt].ValOffset,0,17*4);
        memset(ACHuffmanTable[cnt].Look_Nbits,0,256*2);
        memset(ACHuffmanTable[cnt].Look_Sym,0,256*2);

#elif __JPEGDEC_DUAL
		memset(DCHuffmanTable[cnt].DCHuffmanBits,0,16*2);
		memset(DCHuffmanTable[cnt].DCHuffmanVal,0,12*2);
		memset(ACHuffmanTable[cnt].ACHuffmanBits,0,16*2);
		memset(ACHuffmanTable[cnt].ACHuffmanVal,0,256*2);

////DC NEW MEMBERS
		memset(DCHuffmanTable[cnt].MaxCode,0,18*4);
        memset(DCHuffmanTable[cnt].ValOffset,0,17*4);
        memset(DCHuffmanTable[cnt].Look_Nbits,0,256*2);
        memset(DCHuffmanTable[cnt].Look_Sym,0,256*2);
////AC NEW MEMBERS
		memset(ACHuffmanTable[cnt].MaxCode,0,18*4);
        memset(ACHuffmanTable[cnt].ValOffset,0,17*4);
        memset(ACHuffmanTable[cnt].Look_Nbits,0,256*2);
        memset(ACHuffmanTable[cnt].Look_Sym,0,256*2);
////HARDWARE MEMBERS
		memset(DCHuffmanTable[cnt].DCHuffmanCode,0,12*2);
		memset(DCHuffmanTable[cnt].DCHuffmanSize,0,12*2);
		memset(ACHuffmanTable[cnt].ACHuffmanCode,0,256*2);
		memset(ACHuffmanTable[cnt].ACHuffmanSize,0,256*2);
#else
		memset(DCHuffmanTable[cnt].DCHuffmanCode,0,12*2);
		memset(DCHuffmanTable[cnt].DCHuffmanSize,0,12*2);
		memset(ACHuffmanTable[cnt].ACHuffmanCode,0,256*2);
		memset(ACHuffmanTable[cnt].ACHuffmanSize,0,256*2);
#endif
    }

    isCroppingEnabled =0;
	isDownScalingEnabled = 0;
    isFrameHeaderUpdated = 0;
    isScanHeaderUpdated = 0;
    isHuffmanTableUpdated = 0;
    isQuantizationTableUpdated = 0;
    isRestartIntervalUpdated = 0;

    scan.startAddress = NULL;
    scan.encodedDataSize =0;
    scan.bitstreamOffset=0;

    updateData =1;
    completeImageProcessed = OMX_FALSE;
#ifdef __JPEGDEC_SOFTWARE 
    for (int i =0;i<MAX_NUMBER_HUFFMAN_TABLES;i++)
    {
		for (int j=0;j<16;j++)
		{
			ACHuffmanTable[i].ACHuffmanBits[j] = 0;
			DCHuffmanTable[i].DCHuffmanBits[j] = 0;
		}
		for (int j=0;j<12;j++)
		{

			DCHuffmanTable[i].DCHuffmanVal[j] = 0;
		}
		for (int j=0;j<256;j++)
		{
			
			ACHuffmanTable[i].ACHuffmanVal[j] = 0;
		}
	}
#else
    for (int i =0;i<MAX_NUMBER_HUFFMAN_TABLES;i++)
    {
		for (int j=0;j<256;j++)
		{
			ACHuffmanTable[i].ACHuffmanCode[j] = 0;
			ACHuffmanTable[i].ACHuffmanSize[j] = 0;
		}
		for (int j=0;j<12;j++)
		{
			DCHuffmanTable[i].DCHuffmanCode[j] = 0;
			DCHuffmanTable[i].DCHuffmanSize[j] = 0;
		}
	}
#endif

	for (int i =0;i<NUMBER_QUANT_TABLES;i++)
	{
		for (int j=0;j<64;j++)
		{
			quantizationTables[i].QuantizationTable[j] = 0;
		}
	}
}

OMX_ERRORTYPE JPEGDec_ARMNMF_ParamAndConfig_CLASS::reset()
{
    vertical_offset = horizontal_offset = 0;
    return OMX_ErrorNone;
}

void JPEGDec_ARMNMF_ParamAndConfig_CLASS::initializeParamAndConfig()
{
    OMX_U8 cnt;
    horizontal_offset = 0;
    vertical_offset = 0;
    frameHeight = 480;
    frameWidth = 640;
    cropWindowHeight = 0;
    cropWindowWidth = 0;
    nbComponents = 0;
    nbScanComponents =0;
    downsamplingFactor = DOWNSAMPLING_FACTOR_1;
    restartInterval = 0;
    mode = SEQUENTIAL_JPEG;
    IdY = IdCb = IdCr = 0;
    startSpectralSelection = 0;
    endSpectralSelection =0;
    successiveApproxPosition = 0;

    for(cnt =0;cnt<MAX_NUMBER_COMPONENTS;cnt++)
	{
		samplingFactors[cnt].hSamplingFactor = 0;
		samplingFactors[cnt].vSamplingFactor = 0;
		components[cnt].componentId =0xff;
		components[cnt].DCHuffmanTableDestinationSelector =0xff;
		components[cnt].ACHuffmanTableDestinationSelector =0xff;
		components[cnt].quantizationTableDestinationSelector =0xff;
		components[cnt].componentSelector =0;
    }

    for(cnt=0;cnt<NUMBER_QUANT_TABLES;cnt++)
	{
		memset(quantizationTables[cnt].QuantizationTable,0,64);
    }

    for(cnt=0;cnt<MAX_NUMBER_HUFFMAN_TABLES;cnt++)
	{
#ifndef __JPEGDEC_SOFTWARE 
		memset(DCHuffmanTable[cnt].DCHuffmanCode,0,12);
		memset(DCHuffmanTable[cnt].DCHuffmanSize,0,12);
		memset(ACHuffmanTable[cnt].ACHuffmanCode,0,256);
		memset(ACHuffmanTable[cnt].ACHuffmanSize,0,256);
#else
		memset(DCHuffmanTable[cnt].DCHuffmanBits,0,16);
		memset(DCHuffmanTable[cnt].DCHuffmanVal,0,12);
		memset(ACHuffmanTable[cnt].ACHuffmanBits,0,16);
		memset(ACHuffmanTable[cnt].ACHuffmanVal,0,256);
#endif
    }
    isDownScalingEnabled = 0;
    isCroppingEnabled =0;
    isFrameHeaderUpdated = 0;
    isScanHeaderUpdated = 0;
    isHuffmanTableUpdated = 0;
    isQuantizationTableUpdated = 0;
    isRestartIntervalUpdated = 0;

    scan.startAddress = NULL;
    scan.encodedDataSize =0;

    updateData =1;
}

