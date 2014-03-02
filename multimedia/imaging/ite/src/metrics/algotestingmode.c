/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "algotestingmode.h"
#include "algodebug.h"
#include "algomemoryroutines.h"
#include "algointerface.h"

typedef struct ts_PixelRGB888
{
	TUint8 RedValue;
	TUint8 GreenValue;
	TUint8 BlueValue;
} ts_PixelRGB888;


TUint8 IsPixelWhite(ts_PixelRGB888 Pixel)
{
	if ((Pixel.RedValue > 190) && (Pixel.GreenValue > 190) && (Pixel.BlueValue > 190))
		return(1);
	else
		return(0);
}
TUint8 IsPixelBlack(ts_PixelRGB888 Pixel)
{
	if ((Pixel.RedValue < 50) && (Pixel.GreenValue < 65) && (Pixel.BlueValue < 65))
		return(1);
	else
		return(0);
}
TUint8 IsPixelRed(ts_PixelRGB888 Pixel)
{
	if ((Pixel.RedValue > 190) && (Pixel.GreenValue < 65) && (Pixel.BlueValue < 65))
		return(1);
	else
		return(0);
}
TUint8 IsPixelGreen(ts_PixelRGB888 Pixel)
{
	if ((Pixel.RedValue < 50) && (Pixel.GreenValue > 190) && (Pixel.BlueValue < 33))
		return(1);
	else
		return(0);
}
TUint8 IsPixelBlue(ts_PixelRGB888 Pixel)
{
	if ((Pixel.RedValue < 50) && (Pixel.GreenValue < 65) && (Pixel.BlueValue > 190))
		return(1);
	else
		return(0);
}
TUint8 IsPixelYellow(ts_PixelRGB888 Pixel)
{
	if ((Pixel.RedValue > 190) && (Pixel.GreenValue > 190) && (Pixel.BlueValue < 50))
		return(1);
	else
		return(0);
}
TUint8 IsPixelCyan(ts_PixelRGB888 Pixel)
{
	if ((Pixel.RedValue < 50) && (Pixel.GreenValue > 180) && (Pixel.BlueValue > 190))
		return(1);
	else
		return(0);
}
TUint8 IsPixelMagenta(ts_PixelRGB888 Pixel)
{
	if ((Pixel.RedValue > 190) && (Pixel.GreenValue < 65) && (Pixel.BlueValue > 190))
		return(1);
	else
		return(0);
}
TUint8 IsPixelRGBspecified(ts_PixelRGB888 Pixel, TUint32 aR, TUint32 aG, TUint32 aB, TUint32 toleranceR, TUint32 toleranceG, TUint32 toleranceB)
{
	if (
		(Pixel.RedValue		> aR - toleranceR) && (Pixel.RedValue	< aR + toleranceR) &&
		(Pixel.GreenValue	> aG - toleranceG) && (Pixel.GreenValue	< aG + toleranceG) &&
		(Pixel.BlueValue	> aB - toleranceB) && (Pixel.BlueValue	< aB + toleranceB)
		)
		return(1);
	else
		return(0);
}

void ITE_MetricGetRGBvalue(const TAlgoImage* aImage, TUint32 Xcoord, TUint32 Ycoord, ts_PixelRGB888 *Pel)
{
	Pel->RedValue	= aImage->iImageData.iRChannel[Xcoord + aImage->iImageParams.iImageWidth*Ycoord];
	Pel->GreenValue	= aImage->iImageData.iGChannel[Xcoord + aImage->iImageParams.iImageWidth*Ycoord];
	Pel->BlueValue	= aImage->iImageData.iBChannel[Xcoord + aImage->iImageParams.iImageWidth*Ycoord];
}

TAlgoError MetricTestColorBar(const TAlgoImage* aImage, const TAlgoMetricParams* aMetricParams )
{
	TAlgoError result=EErrorNotValidated;
	TUint32 border_testing_offset = 17;
	TUint32 nbColoum = 8;
	ts_PixelRGB888 color0,color1,color2,color3,color4,color5,color6,color7;
	ts_PixelRGB888 color10,color11,color12,color13,color14,color15,color16,color17;
	ts_PixelRGB888 color20,color21,color22,color23,color24,color25,color26,color27;
	TUint32 errorCode = 0;
	TUint32 half_colum_size = 0, colum_size = 0;
	TUint32 height_position = 0;
	TUint32 offset_bottom_and_top = border_testing_offset; 

	half_colum_size	= aImage->iImageParams.iImageWidth / (nbColoum*2);
	colum_size		= aImage->iImageParams.iImageWidth / nbColoum;

	// test middle of image
	height_position = aImage->iImageParams.iImageHeight / 2;

	ITE_MetricGetRGBvalue(aImage,half_colum_size + 0 * colum_size,height_position,&color0);
	errorCode = IsPixelWhite(color0);
	ITE_MetricGetRGBvalue(aImage,half_colum_size + 1 * colum_size,height_position,&color1);
	errorCode += (IsPixelYellow(color1)<<1);
	ITE_MetricGetRGBvalue(aImage,half_colum_size + 2 * colum_size,height_position,&color2);
	errorCode += (IsPixelCyan(color2)<<2);
	ITE_MetricGetRGBvalue(aImage,half_colum_size + 3 * colum_size,height_position,&color3);
	errorCode += (IsPixelGreen(color3)<<3);
	ITE_MetricGetRGBvalue(aImage,half_colum_size + 4 * colum_size,height_position,&color4);
	errorCode += (IsPixelMagenta(color4)<<4);
	ITE_MetricGetRGBvalue(aImage,half_colum_size + 5 * colum_size,height_position,&color5);
	errorCode += (IsPixelRed(color5)<<5);
	ITE_MetricGetRGBvalue(aImage,half_colum_size + 6 * colum_size,height_position,&color6);
	errorCode += (IsPixelBlue(color6)<<6);
	ITE_MetricGetRGBvalue(aImage,half_colum_size + 7 * colum_size,height_position,&color7);
	errorCode += (IsPixelBlack(color7)<<7);

	// test bottom of image
	if(aImage->iImageParams.iImageHeight >= offset_bottom_and_top)
		height_position = offset_bottom_and_top;
	else
		height_position = 1;

	ITE_MetricGetRGBvalue(aImage,half_colum_size + 0 * colum_size,height_position,&color10);
	errorCode += (IsPixelWhite(color10)<<8);
	ITE_MetricGetRGBvalue(aImage,half_colum_size + 1 * colum_size,height_position,&color11);
	errorCode += (IsPixelYellow(color11)<<9);
	ITE_MetricGetRGBvalue(aImage,half_colum_size + 2 * colum_size,height_position,&color12);
	errorCode += (IsPixelCyan(color12)<<10);
	ITE_MetricGetRGBvalue(aImage,half_colum_size + 3 * colum_size,height_position,&color13);
	errorCode += (IsPixelGreen(color13)<<11);
	ITE_MetricGetRGBvalue(aImage,half_colum_size + 4 * colum_size,height_position,&color14);
	errorCode += (IsPixelMagenta(color14)<<12);
	ITE_MetricGetRGBvalue(aImage,half_colum_size + 5 * colum_size,height_position,&color15);
	errorCode += (IsPixelRed(color15)<<13);
	ITE_MetricGetRGBvalue(aImage,half_colum_size + 6 * colum_size,height_position,&color16);
	errorCode += (IsPixelBlue(color16)<<14);
	ITE_MetricGetRGBvalue(aImage,half_colum_size + 7 * colum_size,height_position,&color17);
	errorCode += (IsPixelBlack(color17)<<15);

	// test top of image
	if(aImage->iImageParams.iImageHeight >= offset_bottom_and_top)
		height_position = aImage->iImageParams.iImageHeight - offset_bottom_and_top;
	else
		height_position = 0;

	ITE_MetricGetRGBvalue(aImage,half_colum_size + 0 * colum_size,height_position,&color20);
	errorCode += (IsPixelWhite(color20)<<16);
	ITE_MetricGetRGBvalue(aImage,half_colum_size + 1 * colum_size,height_position,&color21);
	errorCode += (IsPixelYellow(color21)<<17);
	ITE_MetricGetRGBvalue(aImage,half_colum_size + 2 * colum_size,height_position,&color22);
	errorCode += (IsPixelCyan(color22)<<18);
	ITE_MetricGetRGBvalue(aImage,half_colum_size + 3 * colum_size,height_position,&color23);
	errorCode += (IsPixelGreen(color23)<<19);
	ITE_MetricGetRGBvalue(aImage,half_colum_size + 4 * colum_size,height_position,&color24);
	errorCode += (IsPixelMagenta(color24)<<20);
	ITE_MetricGetRGBvalue(aImage,half_colum_size + 5 * colum_size,height_position,&color25);
	errorCode += (IsPixelRed(color25)<<21);
	ITE_MetricGetRGBvalue(aImage,half_colum_size + 6 * colum_size,height_position,&color26);
	errorCode += (IsPixelBlue(color26)<<22);
	ITE_MetricGetRGBvalue(aImage,half_colum_size + 7 * colum_size,height_position,&color27);
	errorCode += (IsPixelBlack(color27)<<23);

	if (errorCode == 0xffffff )
	{
		 result = EErrorNone;//pass
	}
	else
	{
		result = EErrorNotValidated; //fail
		ALGO_Log_2("test colorbar errorCode = 0x%x, border_testing_offset = %d\n",errorCode, border_testing_offset);
		ALGO_Log_0("colors from low to high: white yellow cyan green magenta red blue black\n");
		ALGO_Log_3("color0 RedValue = %d GreenValue = %d BlueValue = %d\n", color0.RedValue, color0.GreenValue,color0.BlueValue);
		ALGO_Log_3("color1 RedValue = %d GreenValue = %d BlueValue = %d\n", color1.RedValue, color1.GreenValue,color1.BlueValue);
		ALGO_Log_3("color2 RedValue = %d GreenValue = %d BlueValue = %d\n", color2.RedValue, color2.GreenValue,color2.BlueValue);
		ALGO_Log_3("color3 RedValue = %d GreenValue = %d BlueValue = %d\n", color3.RedValue, color3.GreenValue,color3.BlueValue);
		ALGO_Log_3("color4 RedValue = %d GreenValue = %d BlueValue = %d\n", color4.RedValue, color4.GreenValue,color4.BlueValue);
		ALGO_Log_3("color5 RedValue = %d GreenValue = %d BlueValue = %d\n", color5.RedValue, color5.GreenValue,color5.BlueValue);
		ALGO_Log_3("color6 RedValue = %d GreenValue = %d BlueValue = %d\n", color6.RedValue, color6.GreenValue,color6.BlueValue);
		ALGO_Log_3("color7 RedValue = %d GreenValue = %d BlueValue = %d\n\n", color7.RedValue, color7.GreenValue,color7.BlueValue);
		ALGO_Log_3("color10 RedValue = %d GreenValue = %d BlueValue = %d\n", color10.RedValue, color10.GreenValue,color10.BlueValue);
		ALGO_Log_3("color11 RedValue = %d GreenValue = %d BlueValue = %d\n", color11.RedValue, color11.GreenValue,color11.BlueValue);
		ALGO_Log_3("color12 RedValue = %d GreenValue = %d BlueValue = %d\n", color12.RedValue, color12.GreenValue,color12.BlueValue);
		ALGO_Log_3("color13 RedValue = %d GreenValue = %d BlueValue = %d\n", color13.RedValue, color13.GreenValue,color13.BlueValue);
		ALGO_Log_3("color14 RedValue = %d GreenValue = %d BlueValue = %d\n", color14.RedValue, color14.GreenValue,color14.BlueValue);
		ALGO_Log_3("color15 RedValue = %d GreenValue = %d BlueValue = %d\n", color15.RedValue, color15.GreenValue,color15.BlueValue);
		ALGO_Log_3("color16 RedValue = %d GreenValue = %d BlueValue = %d\n", color16.RedValue, color16.GreenValue,color16.BlueValue);
		ALGO_Log_3("color17 RedValue = %d GreenValue = %d BlueValue = %d\n\n", color17.RedValue, color17.GreenValue,color17.BlueValue);
		ALGO_Log_3("color20 RedValue = %d GreenValue = %d BlueValue = %d\n", color20.RedValue, color20.GreenValue,color20.BlueValue);
		ALGO_Log_3("color21 RedValue = %d GreenValue = %d BlueValue = %d\n", color21.RedValue, color21.GreenValue,color21.BlueValue);
		ALGO_Log_3("color22 RedValue = %d GreenValue = %d BlueValue = %d\n", color22.RedValue, color22.GreenValue,color22.BlueValue);
		ALGO_Log_3("color23 RedValue = %d GreenValue = %d BlueValue = %d\n", color23.RedValue, color23.GreenValue,color23.BlueValue);
		ALGO_Log_3("color24 RedValue = %d GreenValue = %d BlueValue = %d\n", color24.RedValue, color24.GreenValue,color24.BlueValue);
		ALGO_Log_3("color25 RedValue = %d GreenValue = %d BlueValue = %d\n", color25.RedValue, color25.GreenValue,color25.BlueValue);
		ALGO_Log_3("color26 RedValue = %d GreenValue = %d BlueValue = %d\n", color26.RedValue, color26.GreenValue,color26.BlueValue);
		ALGO_Log_3("color27 RedValue = %d GreenValue = %d BlueValue = %d\n\n", color27.RedValue, color27.GreenValue,color27.BlueValue);
	}
	     
	return result;
}

TAlgoError MetricTestSolidBar(const TAlgoImage* aImage, const TAlgoMetricParams* aMetricParams, TUint8 (*pf)(ts_PixelRGB888) )
{
	ts_PixelRGB888 Pixel;
	TAlgoError error=EErrorNotValidated;
	TUint32 Xcoord = 0, Ycoord = 0;

	Xcoord = 0;
	Ycoord = 0;
	ITE_MetricGetRGBvalue(aImage,Xcoord,Ycoord,&Pixel);
	if( (*pf)(Pixel)!= 1) 
	{
		ALGO_Log_2("\t Not of the expected color in %s : line %d \n", __FUNCTION__, __LINE__);
		ALGO_Log_2("\t\t X = %d, Y = %d\n", Xcoord,Ycoord);
		ALGO_Log_3("\t\t RedValue %d, GreenValue %d, BlueValue %d\n", Pixel.RedValue, Pixel.GreenValue, Pixel.BlueValue);
		goto label_end;
	}
	else
	{
		ALGO_Log_2("\t\t X = %d, Y = %d \n", Xcoord,Ycoord);
		ALGO_Log_3("\t\t RedValue %d, GreenValue %d, BlueValue %d\n", Pixel.RedValue, Pixel.GreenValue, Pixel.BlueValue);
	}

	Xcoord = aImage->iImageParams.iImageWidth/2;
	Ycoord = aImage->iImageParams.iImageHeight/2;
	ITE_MetricGetRGBvalue(aImage,Xcoord,Ycoord,&Pixel);
	if( (*pf)(Pixel)!= 1)
	{
		ALGO_Log_2("\t Not of the expected color in %s : line %d \n", __FUNCTION__, __LINE__);
		ALGO_Log_2("\t\t X = %d, Y = %d\n", Xcoord,Ycoord);
		ALGO_Log_3("\t\t RedValue %d, GreenValue %d, BlueValue %d\n", Pixel.RedValue, Pixel.GreenValue, Pixel.BlueValue);
		goto label_end;
	}
	else
	{
		ALGO_Log_2("\t\t X = %d, Y = %d\n", Xcoord,Ycoord);
		ALGO_Log_3("\t\t RedValue %d, GreenValue %d, BlueValue %d\n", Pixel.RedValue, Pixel.GreenValue, Pixel.BlueValue);
	}

	Xcoord = aImage->iImageParams.iImageWidth-1;
	Ycoord = aImage->iImageParams.iImageHeight-1;
	ITE_MetricGetRGBvalue(aImage,Xcoord,Ycoord,&Pixel);
	if( (*pf)(Pixel)!= 1)
	{
		ALGO_Log_2("\t Not of the expected color in %s : line %d \n", __FUNCTION__, __LINE__);
		ALGO_Log_2("\t\t X = %d, Y = %d\n", Xcoord,Ycoord);
		ALGO_Log_3("\t\t RedValue %d, GreenValue %d, BlueValue %d\n", Pixel.RedValue, Pixel.GreenValue, Pixel.BlueValue);
		goto label_end;
	}
	else
	{
		ALGO_Log_2("\t\t X = %d, Y = %d\n", Xcoord,Ycoord);
		ALGO_Log_3("\t\t RedValue %d, GreenValue %d, BlueValue %d\n", Pixel.RedValue, Pixel.GreenValue, Pixel.BlueValue);
	}

	// don't try to check data after the image
	//Xcoord = aImage->iImageParams.iImageWidth;
	//Ycoord = aImage->iImageParams.iImageHeight-1;
	//ITE_MetricGetRGBvalue(aImage,Xcoord,Ycoord,&Pixel);
	//if( (*pf)(Pixel)== 1)
	//{
	//	ALGO_Log_2("\t\t Of the expected color in %s : line %d : should not be of the color\n", __FUNCTION__, __LINE__);
	//	ALGO_Log_2("\t\t X = %d, Y = %d\n", Xcoord,Ycoord);
	//	ALGO_Log_3("\t\t RedValue %d, GreenValue %d, BlueValue %d\n", Pixel.RedValue, Pixel.GreenValue, Pixel.BlueValue);
	//	goto label_end;
	//}
	//else
	//{
	//	ALGO_Log_0("\t Not of the expected color : Ok\n");
	//	ALGO_Log_2("\t\t X = %d, Y = %d\n", Xcoord,Ycoord);
	//	ALGO_Log_3("\t\t RedValue %d, GreenValue %d, BlueValue %d\n", Pixel.RedValue, Pixel.GreenValue, Pixel.BlueValue);
	//}

	error=EErrorNone; // success
label_end:

	return error;
}

TAlgoError MetricTestSolidBarRGBspecified(const TAlgoImage* aImage, const TAlgoMetricParams* aMetricParams, TUint32 aR, TUint32 aG, TUint32 aB, TUint32 toleranceR, TUint32 toleranceG, TUint32 toleranceB )
{
	ts_PixelRGB888 Pixel;
	TAlgoError error=EErrorNotValidated;
	TUint32 Xcoord = 0, Ycoord = 0;
	unsigned int isThereAError = 0;

	ALGO_Log_1("\t Entering %s  \n", __FUNCTION__);
	ALGO_Log_2("\t iImageParams.iImageWidth %d, iImageParams.iImageHeight %d \n", aImage->iImageParams.iImageWidth, aImage->iImageParams.iImageHeight);
	ALGO_Log_2("\t iStartIndexX %d, iStartIndexY %d \n", aMetricParams->iTestCoordinates.iStartIndexX, aMetricParams->iTestCoordinates.iStartIndexY);
	ALGO_Log_2("\t iPixelsToGrabX %d, iPixelsToGrabY %d \n", aMetricParams->iTestCoordinates.iPixelsToGrabX, aMetricParams->iTestCoordinates.iPixelsToGrabY);
	ALGO_Log_3("\t toleranceR %d, toleranceG %d, toleranceB %d\n", toleranceR, toleranceG, toleranceB);
				
	Xcoord = aMetricParams->iTestCoordinates.iStartIndexX;
	Ycoord = aMetricParams->iTestCoordinates.iStartIndexY;
	ITE_MetricGetRGBvalue(aImage,Xcoord,Ycoord,&Pixel);
	if( IsPixelRGBspecified(Pixel, aR, aG, aB, toleranceR, toleranceG,  toleranceB)!= 1) 
	{
		ALGO_Log_2("\t Not of the expected color in %s : line %d \n", __FUNCTION__, __LINE__);
		ALGO_Log_2("\t\t X = %d, Y = %d\n", Xcoord,Ycoord);
		ALGO_Log_3("\t\t RedValue %d, GreenValue %d, BlueValue %d\n", Pixel.RedValue, Pixel.GreenValue, Pixel.BlueValue);
		isThereAError++;
	}
	else
	{
		ALGO_Log_2("\t\t X = %d, Y = %d \n", Xcoord,Ycoord);
		ALGO_Log_3("\t\t RedValue %d, GreenValue %d, BlueValue %d\n", Pixel.RedValue, Pixel.GreenValue, Pixel.BlueValue);
	}

	Xcoord = aMetricParams->iTestCoordinates.iStartIndexX + aMetricParams->iTestCoordinates.iPixelsToGrabX/2;
	Ycoord = aMetricParams->iTestCoordinates.iStartIndexY + aMetricParams->iTestCoordinates.iPixelsToGrabY/2;
	ITE_MetricGetRGBvalue(aImage,Xcoord,Ycoord,&Pixel);
	if( IsPixelRGBspecified(Pixel, aR, aG, aB, toleranceR, toleranceG,  toleranceB)!= 1)
	{
		ALGO_Log_2("\t Not of the expected color in %s : line %d \n", __FUNCTION__, __LINE__);
		ALGO_Log_2("\t\t X = %d, Y = %d\n", Xcoord,Ycoord);
		ALGO_Log_3("\t\t RedValue %d, GreenValue %d, BlueValue %d\n", Pixel.RedValue, Pixel.GreenValue, Pixel.BlueValue);
		isThereAError++;
	}
	else
	{
		ALGO_Log_2("\t\t X = %d, Y = %d\n", Xcoord,Ycoord);
		ALGO_Log_3("\t\t RedValue %d, GreenValue %d, BlueValue %d\n", Pixel.RedValue, Pixel.GreenValue, Pixel.BlueValue);
	}

	Xcoord = aMetricParams->iTestCoordinates.iStartIndexX + aMetricParams->iTestCoordinates.iPixelsToGrabX-1;
	Ycoord = aMetricParams->iTestCoordinates.iStartIndexY + aMetricParams->iTestCoordinates.iPixelsToGrabY-1;
	ITE_MetricGetRGBvalue(aImage,Xcoord,Ycoord,&Pixel);
	if( IsPixelRGBspecified(Pixel, aR, aG, aB, toleranceR, toleranceG,  toleranceB)!= 1)
	{
		ALGO_Log_2("\t Not of the expected color in %s : line %d \n", __FUNCTION__, __LINE__);
		ALGO_Log_2("\t\t X = %d, Y = %d\n", Xcoord,Ycoord);
		ALGO_Log_3("\t\t RedValue %d, GreenValue %d, BlueValue %d\n", Pixel.RedValue, Pixel.GreenValue, Pixel.BlueValue);
		isThereAError++;
	}
	else
	{
		ALGO_Log_2("\t\t X = %d, Y = %d\n", Xcoord,Ycoord);
		ALGO_Log_3("\t\t RedValue %d, GreenValue %d, BlueValue %d\n", Pixel.RedValue, Pixel.GreenValue, Pixel.BlueValue);
	}

	if(0 == isThereAError)
	{
		error=EErrorNone; // success
	}

	return error;
}

#define ALGO_THRESOLD_WHITE 150
#define ALGO_THRESOLD_BLACK 50
#define ALGO_THRESOLD_RED 160
#define ALGO_RED_FACTOR 1.5
#define ALGO_THRESOLD_GREEN 158
#define ALGO_GREEN_FACTOR 1.48
#define ALGO_THRESOLD_BLUE 156
#define ALGO_BLUE_FACTOR 1.46
TAlgoError MetricTestSolidBarRorGorBorBorW(const TAlgoImage* aImage, const TAlgoMetricParams* aMetricParams, TUint32 aR, TUint32 aG, TUint32 aB )
{
	ts_PixelRGB888 Pixel_topleft, Pixel_middle, Pixel_bottomright;
	TAlgoError error=EErrorNotValidated;
	TUint32 Xcoord = 0, Ycoord = 0;
	unsigned int isThereAError = 0;

	ALGO_Log_1("\t Entering %s  \n", __FUNCTION__);
	ALGO_Log_2("\t iImageParams.iImageWidth %d, iImageParams.iImageHeight %d \n", aImage->iImageParams.iImageWidth, aImage->iImageParams.iImageHeight);
	ALGO_Log_2("\t iStartIndexX %d, iStartIndexY %d \n", aMetricParams->iTestCoordinates.iStartIndexX, aMetricParams->iTestCoordinates.iStartIndexY);
	ALGO_Log_2("\t iPixelsToGrabX %d, iPixelsToGrabY %d \n", aMetricParams->iTestCoordinates.iPixelsToGrabX, aMetricParams->iTestCoordinates.iPixelsToGrabY);
	ALGO_Log_3("\t aR %d, aG %d, aB %d\n", aR, aG, aB);

	Xcoord = aMetricParams->iTestCoordinates.iStartIndexX;
	Ycoord = aMetricParams->iTestCoordinates.iStartIndexY;
	ITE_MetricGetRGBvalue(aImage,Xcoord,Ycoord,&Pixel_topleft);

	Xcoord = aMetricParams->iTestCoordinates.iStartIndexX + aMetricParams->iTestCoordinates.iPixelsToGrabX/2;
	Ycoord = aMetricParams->iTestCoordinates.iStartIndexY + aMetricParams->iTestCoordinates.iPixelsToGrabY/2;
	ITE_MetricGetRGBvalue(aImage,Xcoord,Ycoord,&Pixel_middle);

	Xcoord = aMetricParams->iTestCoordinates.iStartIndexX + aMetricParams->iTestCoordinates.iPixelsToGrabX-1;
	Ycoord = aMetricParams->iTestCoordinates.iStartIndexY + aMetricParams->iTestCoordinates.iPixelsToGrabY-1;
	ITE_MetricGetRGBvalue(aImage,Xcoord,Ycoord,&Pixel_bottomright);

	if(0xff == aR)
	{
		if( (0xff == aG) && (0xff == aB))
		{
			ALGO_Log_0("checking if image is white\n");
			/////
			if(Pixel_topleft.RedValue < ALGO_THRESOLD_WHITE)
			{
				ALGO_Log_1("\t error Pixel_topleft.RedValue %d\n", Pixel_topleft.RedValue);
				isThereAError++;
			}
			if(Pixel_topleft.GreenValue < ALGO_THRESOLD_WHITE)
			{
				ALGO_Log_1("\t error Pixel_topleft.GreenValue %d\n", Pixel_topleft.GreenValue);
				isThereAError++;
			}
			if(Pixel_topleft.BlueValue < ALGO_THRESOLD_WHITE)
			{
				ALGO_Log_1("\t error Pixel_topleft.BlueValue %d\n", Pixel_topleft.BlueValue);
				isThereAError++;
			}
			/////
			if(Pixel_middle.RedValue < ALGO_THRESOLD_WHITE)
			{
				ALGO_Log_1("\t error Pixel_middle.RedValue %d\n", Pixel_middle.RedValue);
				isThereAError++;
			}
			if(Pixel_middle.GreenValue < ALGO_THRESOLD_WHITE)
			{
				ALGO_Log_1("\t error Pixel_middle.GreenValue %d\n", Pixel_middle.GreenValue);
				isThereAError++;
			}
			if(Pixel_middle.BlueValue < ALGO_THRESOLD_WHITE)
			{
				ALGO_Log_1("\t error Pixel_middle.BlueValue %d\n", Pixel_middle.BlueValue);
				isThereAError++;
			}
			/////
			if(Pixel_bottomright.RedValue < ALGO_THRESOLD_WHITE)
			{
				ALGO_Log_1("\t error Pixel_bottomright.RedValue %d\n", Pixel_bottomright.RedValue);
				isThereAError++;
			}
			if(Pixel_bottomright.GreenValue < ALGO_THRESOLD_WHITE)
			{
				ALGO_Log_1("\t error Pixel_bottomright.GreenValue %d\n", Pixel_bottomright.GreenValue);
				isThereAError++;
			}
			if(Pixel_bottomright.BlueValue < ALGO_THRESOLD_WHITE)
			{
				ALGO_Log_1("\t error Pixel_bottomright.BlueValue %d\n", Pixel_bottomright.BlueValue);
				isThereAError++;
			}
		}
		else
		{
			ALGO_Log_0("checking if image is red\n");
			////
			if(Pixel_topleft.RedValue < ALGO_THRESOLD_RED)
			{
				ALGO_Log_1("\t error Pixel_topleft.RedValue %d\n", Pixel_topleft.RedValue);
				isThereAError++;
			}
			if(Pixel_topleft.RedValue < (TUint8)(Pixel_topleft.GreenValue * ALGO_RED_FACTOR))
			{
				ALGO_Log_1("\t error Pixel_topleft.GreenValue %d\n", Pixel_topleft.GreenValue);
				isThereAError++;
			}
			if(Pixel_topleft.RedValue < (TUint8)(Pixel_topleft.BlueValue * ALGO_RED_FACTOR))
			{
				ALGO_Log_1("\t error Pixel_topleft.BlueValue %d\n", Pixel_topleft.BlueValue);
				isThereAError++;
			}
			////
			if(Pixel_middle.RedValue < ALGO_THRESOLD_RED)
			{
				ALGO_Log_1("\t error Pixel_middle.RedValue %d\n", Pixel_middle.RedValue);
				isThereAError++;
			}
			if(Pixel_middle.RedValue < (TUint8)(Pixel_middle.GreenValue * ALGO_RED_FACTOR))
			{
				ALGO_Log_1("\t error Pixel_middle.GreenValue %d\n", Pixel_middle.GreenValue);
				isThereAError++;
			}
			if(Pixel_middle.RedValue < (TUint8)(Pixel_middle.BlueValue * ALGO_RED_FACTOR))
			{
				ALGO_Log_1("\t error Pixel_middle.BlueValue %d\n", Pixel_middle.BlueValue);
				isThereAError++;
			}
			////
			if(Pixel_bottomright.RedValue < ALGO_THRESOLD_RED)
			{
				ALGO_Log_1("\t error Pixel_bottomright.RedValue %d\n", Pixel_bottomright.RedValue);
				isThereAError++;
			}
			if(Pixel_bottomright.RedValue < (TUint8)(Pixel_bottomright.GreenValue * ALGO_RED_FACTOR))
			{
				ALGO_Log_1("\t error Pixel_bottomright.GreenValue %d\n", Pixel_bottomright.GreenValue);
				isThereAError++;
			}
			if(Pixel_bottomright.RedValue < (TUint8)(Pixel_bottomright.BlueValue * ALGO_RED_FACTOR))
			{
				ALGO_Log_1("\t error Pixel_bottomright.BlueValue %d\n", Pixel_bottomright.BlueValue);
				isThereAError++;
			}
		}
	}
	else
	{
		if(0xff == aG)
		{
			ALGO_Log_0("checking if image is green\n");
			////
			if(Pixel_topleft.GreenValue < ALGO_THRESOLD_GREEN)
			{
				ALGO_Log_1("\t error Pixel_topleft.GreenValue %d\n", Pixel_topleft.GreenValue);
				isThereAError++;
			}
			if(Pixel_topleft.GreenValue < (TUint8)(Pixel_topleft.RedValue * ALGO_GREEN_FACTOR))
			{
				ALGO_Log_1("\t error Pixel_topleft.RedValue %d\n", Pixel_topleft.RedValue);
				isThereAError++;
			}
			if(Pixel_topleft.GreenValue < (TUint8)(Pixel_topleft.BlueValue * ALGO_GREEN_FACTOR))
			{
				ALGO_Log_1("\t error Pixel_topleft.BlueValue %d\n", Pixel_topleft.BlueValue);
				isThereAError++;
			}
			////
			if(Pixel_middle.GreenValue < ALGO_THRESOLD_GREEN)
			{
				ALGO_Log_1("\t error Pixel_middle.GreenValue %d\n", Pixel_middle.GreenValue);
				isThereAError++;
			}
			if(Pixel_middle.GreenValue < (TUint8)(Pixel_middle.RedValue * ALGO_GREEN_FACTOR))
			{
				ALGO_Log_1("\t error Pixel_middle.RedValue %d\n", Pixel_middle.RedValue);
				isThereAError++;
			}
			if(Pixel_middle.GreenValue < (TUint8)(Pixel_middle.BlueValue * ALGO_GREEN_FACTOR))
			{
				ALGO_Log_1("\t error Pixel_middle.BlueValue %d\n", Pixel_middle.BlueValue);
				isThereAError++;
			}
			////
			if(Pixel_bottomright.GreenValue < ALGO_THRESOLD_GREEN)
			{
				ALGO_Log_1("\t error Pixel_bottomright.GreenValue %d\n", Pixel_bottomright.GreenValue);
				isThereAError++;
			}
			if(Pixel_bottomright.GreenValue < (TUint8)(Pixel_bottomright.RedValue * ALGO_GREEN_FACTOR))
			{
				ALGO_Log_1("\t error Pixel_bottomright.RedValue %d\n", Pixel_bottomright.RedValue);
				isThereAError++;
			}
			if(Pixel_bottomright.GreenValue < (TUint8)(Pixel_bottomright.BlueValue * ALGO_GREEN_FACTOR))
			{
				ALGO_Log_1("\t error Pixel_bottomright.BlueValue %d\n", Pixel_bottomright.BlueValue);
				isThereAError++;
			}
		}
		else
		{
			if(0xff == aB)
			{
				ALGO_Log_0("checking if image is blue\n");
				////
				if(Pixel_topleft.BlueValue < ALGO_THRESOLD_BLUE)
				{
					ALGO_Log_1("\t error Pixel_topleft.BlueValue %d\n", Pixel_topleft.BlueValue);
					isThereAError++;
				}
				if(Pixel_topleft.BlueValue < (TUint8)(Pixel_topleft.GreenValue * ALGO_BLUE_FACTOR))
				{
					ALGO_Log_1("\t error Pixel_topleft.GreenValue %d\n", Pixel_topleft.GreenValue);
					isThereAError++;
				}
				if(Pixel_topleft.BlueValue < (TUint8)(Pixel_topleft.RedValue * ALGO_BLUE_FACTOR))
				{
					ALGO_Log_1("\t error Pixel_topleft.RedValue %d\n", Pixel_topleft.RedValue);
					isThereAError++;
				}
				////
				if(Pixel_middle.BlueValue < ALGO_THRESOLD_BLUE)
				{
					ALGO_Log_1("\t error Pixel_middle.BlueValue %d\n", Pixel_middle.BlueValue);
					isThereAError++;
				}
				if(Pixel_middle.BlueValue < (TUint8)(Pixel_middle.GreenValue * ALGO_BLUE_FACTOR))
				{
					ALGO_Log_1("\t error Pixel_middle.GreenValue %d\n", Pixel_middle.GreenValue);
					isThereAError++;
				}
				if(Pixel_middle.BlueValue < (TUint8)(Pixel_middle.RedValue * ALGO_BLUE_FACTOR))
				{
					ALGO_Log_1("\t error Pixel_middle.RedValue %d\n", Pixel_middle.RedValue);
					isThereAError++;
				}
				////
				if(Pixel_bottomright.BlueValue < ALGO_THRESOLD_BLUE)
				{
					ALGO_Log_1("\t error Pixel_bottomright.BlueValue %d\n", Pixel_bottomright.BlueValue);
					isThereAError++;
				}
				if(Pixel_bottomright.BlueValue < (TUint8)(Pixel_bottomright.GreenValue * ALGO_BLUE_FACTOR))
				{
					ALGO_Log_1("\t error Pixel_bottomright.GreenValue %d\n", Pixel_bottomright.GreenValue);
					isThereAError++;
				}
				if(Pixel_bottomright.BlueValue < (TUint8)(Pixel_bottomright.RedValue * ALGO_BLUE_FACTOR))
				{
					ALGO_Log_1("\t error Pixel_bottomright.RedValue %d\n", Pixel_bottomright.RedValue);
					isThereAError++;
				}
			}
			else
			{
				ALGO_Log_0("checking if image is black\n");
				/////
				if(Pixel_topleft.RedValue > ALGO_THRESOLD_BLACK)
				{
					ALGO_Log_1("\t error Pixel_topleft.RedValue %d\n", Pixel_topleft.RedValue);
					isThereAError++;
				}
				if(Pixel_topleft.GreenValue > ALGO_THRESOLD_BLACK)
				{
					ALGO_Log_1("\t error Pixel_topleft.GreenValue %d\n", Pixel_topleft.GreenValue);
					isThereAError++;
				}
				if(Pixel_topleft.BlueValue > ALGO_THRESOLD_BLACK)
				{
					ALGO_Log_1("\t error Pixel_topleft.BlueValue %d\n", Pixel_topleft.BlueValue);
					isThereAError++;
				}
				/////
				if(Pixel_middle.RedValue > ALGO_THRESOLD_BLACK)
				{
					ALGO_Log_1("\t error Pixel_middle.RedValue %d\n", Pixel_middle.RedValue);
					isThereAError++;
				}
				if(Pixel_middle.GreenValue > ALGO_THRESOLD_BLACK)
				{
					ALGO_Log_1("\t error Pixel_middle.GreenValue %d\n", Pixel_middle.GreenValue);
					isThereAError++;
				}
				if(Pixel_middle.BlueValue > ALGO_THRESOLD_BLACK)
				{
					ALGO_Log_1("\t error Pixel_middle.BlueValue %d\n", Pixel_middle.BlueValue);
					isThereAError++;
				}
				/////
				if(Pixel_bottomright.RedValue > ALGO_THRESOLD_BLACK)
				{
					ALGO_Log_1("\t error Pixel_bottomright.RedValue %d\n", Pixel_bottomright.RedValue);
					isThereAError++;
				}
				if(Pixel_bottomright.GreenValue > ALGO_THRESOLD_BLACK)
				{
					ALGO_Log_1("\t error Pixel_bottomright.GreenValue %d\n", Pixel_bottomright.GreenValue);
					isThereAError++;
				}
				if(Pixel_bottomright.BlueValue > ALGO_THRESOLD_BLACK)
				{
					ALGO_Log_1("\t error Pixel_bottomright.BlueValue %d\n", Pixel_bottomright.BlueValue);
					isThereAError++;
				}
			}
		}
	}


	if(0 == isThereAError)
	{
		error=EErrorNone; // success
	}

	return error;
}



TAlgoError ValidationMetricImageContentProcess( const TAlgoImage* aImageEffect, 
					                             	TAlgoImageContent aContentType,
													TUint32 aR,
													TUint32 aG,
													TUint32 aB,
													TUint32 atolerance,
													 const TAlgoMetricParams* aMetricParams
 )
{
	TAlgoError error=EErrorNone;
	TUint8 (*pf)(ts_PixelRGB888) = NULL;

	switch(aContentType)
	{
	case EImageContentNormal:
		{
			TAlgoError errorForNormal=EErrorNone;
			pf = &IsPixelRed;
			errorForNormal =  MetricTestSolidBar(aImageEffect, aMetricParams, pf );
			if(errorForNormal == EErrorNone)
			{
				// image must not be red
				error = EErrorNotValidated;
			}
		}
		break;
	case EImageContentColorBar:
		error = MetricTestColorBar(aImageEffect, aMetricParams);
		break;
	case EImageContentSolidBarWhite:
		pf = &IsPixelWhite;
		error =  MetricTestSolidBar(aImageEffect, aMetricParams, pf );
		break;
	case EImageContentSolidBarBlack:
		pf = &IsPixelBlack;
		error =  MetricTestSolidBar(aImageEffect, aMetricParams, pf );
		break;
	case EImageContentSolidBarRed:
		pf = &IsPixelRed;
		error =  MetricTestSolidBar(aImageEffect, aMetricParams, pf );
		break;
	case EImageContentSolidBarGreen:
		pf = &IsPixelGreen;
		error =  MetricTestSolidBar(aImageEffect, aMetricParams, pf );
		break;
	case EImageContentSolidBarBlue:
		pf = &IsPixelBlue;
		error =  MetricTestSolidBar(aImageEffect, aMetricParams, pf );
		break;
	case EImageContentSolidBarYellow:
		pf = &IsPixelYellow;
		error =  MetricTestSolidBar(aImageEffect, aMetricParams, pf );
		break;
	case EImageContentSolidBarCyan:
		pf = &IsPixelCyan;
		error =  MetricTestSolidBar(aImageEffect, aMetricParams, pf );
		break;
	case EImageContentSolidBarMagenta:
		pf = &IsPixelMagenta;
		error =  MetricTestSolidBar(aImageEffect, aMetricParams, pf );
		break;
	case EImageContentSolidBarRGBspecified:
		{
		TAlgoParamImageContent *imageContent = (TAlgoParamImageContent*)aMetricParams->iParams;		
		ALGO_Log_0("\t case EImageContentSolidBarRGBspecified\n");
		ALGO_Log_3("\t aR %d, aG %d, aB %d\n", aR, aG, aB);
		ALGO_Log_3("\t tolerance_R %d, tolerance_G %d tolerance_B %d\n", imageContent->tolerance_R, imageContent->tolerance_G, imageContent->tolerance_B);
		error =   MetricTestSolidBarRGBspecified(aImageEffect, aMetricParams, aR, aG, aB, imageContent->tolerance_R, imageContent->tolerance_G, imageContent->tolerance_B);
		}
		break;
	case EImageContentSolidBarRorGorBorBorW:
		{
		ALGO_Log_3("\t aR %d, aG %d, aB %d\n", aR, aG, aB);
		error =   MetricTestSolidBarRorGorBorBorW(aImageEffect, aMetricParams, aR, aG, aB);
		}
		break;
	default:
		error=EErrorNotValidated;
	}

	if(error != EErrorNone)
	{
		ALGO_Log_2("\t\t ERROR detected %s (%d)\n", __FUNCTION__, __LINE__);
	}
	return error;
}



TAlgoError ValidationMetricImageContent( const TAlgoImage* aImageEffectDisable, 
					                             const TAlgoImage* aImageEffectEnable,
					                             const TAlgoMetricParams* aMetricParams )
{
	TAlgoError error=EErrorNone, first_error=EErrorNone;
	TAlgoParamImageContent *imageContent = (TAlgoParamImageContent*)aMetricParams->iParams;
	unsigned int isThereAError = 0;

	ALGO_Log_1("entering %s\n", __FUNCTION__);
	ALGO_Log_3("ToleranceR = %f, ToleranceG = %f, ToleranceB = %f\n", aMetricParams->iErrorTolerance.iErrorToleranceR, aMetricParams->iErrorTolerance.iErrorToleranceG, aMetricParams->iErrorTolerance.iErrorToleranceB);
	error = ValidationMetricImageContentProcess( aImageEffectDisable, 
					                             	imageContent->iContentType_1,
													imageContent->iR_1,
													imageContent->iG_1,
													imageContent->iB_1,
													imageContent->tolerance_1,
													aMetricParams);
	if(error != EErrorNone)
	{
		ALGO_Log_2("\t\t ERROR detected %s (%d)\n", __FUNCTION__, __LINE__);
		isThereAError++;
		if(EErrorNone == first_error)
		{
			first_error = error;
		}
	}


	error = ValidationMetricImageContentProcess( aImageEffectEnable, 
					                             	imageContent->iContentType_2,
													imageContent->iR_2,
													imageContent->iG_2,
													imageContent->iB_2,
													imageContent->tolerance_2,
													aMetricParams);
	if(error != EErrorNone)
	{
		ALGO_Log_2("\t\t ERROR detected %s (%d)\n", __FUNCTION__, __LINE__);
		isThereAError++;
		if(EErrorNone == first_error)
		{
			first_error = error;
		}
	}

	return first_error;
}


TAlgoError ValidationMetricImageContent_AllocationOptimized( TAlgoImageParams* aImageParams,
									                    TAny* bufferEffectDisable, 
					                                    TAny* bufferEffectEnable,
					                             TAlgoMetricParams* aMetricParams )
{
	TAlgoImage aImageEffectDisable;
	TAlgoImage aImageEffectEnable;
	TUint32 memDisR=0,memDisG=0,memDisB=0,memEnR=0,memEnG=0,memEnB=0, memTempYUV=0;
	TUint32 aNbStatusLine = 0;
	TAlgoError error=EErrorNone;
	TAlgoParamImageContent *imageContent = (TAlgoParamImageContent*)aMetricParams->iParams;

	AlgoMemcpy(&aImageEffectDisable.iImageParams, aImageParams, sizeof(TAlgoImageParams));
	AlgoMemcpy(&aImageEffectEnable.iImageParams, aImageParams, sizeof(TAlgoImageParams));

	error = AllocateAndExtractRGBChannels( aImageParams,
										 &aImageEffectDisable,
										 &memDisR,
										 &memDisG,
										 &memDisB,
										 &memTempYUV,
										 &aNbStatusLine,
										 bufferEffectDisable, 
										 aMetricParams);
	if(error != EErrorNone)
		goto end;
		
	error = ValidationMetricImageContentProcess( &aImageEffectDisable, 
				                             	imageContent->iContentType_1,
												imageContent->iR_1,
												imageContent->iG_1,
												imageContent->iB_1,
												imageContent->tolerance_1,
												aMetricParams);
	if(error != EErrorNone)
	{
		ALGO_Log_2("\t\t ERROR detected %s (%d)\n", __FUNCTION__, __LINE__);
		goto end;
	}
	if(memDisR){AlgoFree((TAny*) memDisR);memDisR=0;}
	if(memDisG){AlgoFree((TAny*) memDisG);memDisG=0;}
	if(memDisB){AlgoFree((TAny*) memDisB);memDisB=0;}
	if(memTempYUV){AlgoFree((TAny*) memTempYUV);memTempYUV = 0;};

	error = AllocateAndExtractRGBChannels( aImageParams,
										 &aImageEffectEnable,
										 &memEnR,
										 &memEnG,
										 &memEnB,
										 &memTempYUV,
										 &aNbStatusLine,
										 bufferEffectEnable, 
										 aMetricParams);
	if(error != EErrorNone)
		goto end;

	error = ValidationMetricImageContentProcess( &aImageEffectEnable, 
					                             	imageContent->iContentType_2,
													imageContent->iR_2,
													imageContent->iG_2,
													imageContent->iB_2,
													imageContent->tolerance_2,
													aMetricParams);
	if(error != EErrorNone)
	{
		ALGO_Log_2("\t\t ERROR detected %s (%d)\n", __FUNCTION__, __LINE__);
		goto end;
	}
	if(memEnR){AlgoFree((TAny*) memEnR);memEnR=0;}
	if(memEnG){AlgoFree((TAny*) memEnG);memEnG=0;}
	if(memEnB){AlgoFree((TAny*) memEnB);memEnB=0;}
	if(memTempYUV){AlgoFree((TAny*) memTempYUV);memTempYUV = 0;};

end:
	if(memDisR){AlgoFree((TAny*) memDisR);memDisR=0;}
	if(memDisG){AlgoFree((TAny*) memDisG);memDisG=0;}
	if(memDisB){AlgoFree((TAny*) memDisB);memDisB=0;}
	if(memEnR){AlgoFree((TAny*) memEnR);memEnR=0;}
	if(memEnG){AlgoFree((TAny*) memEnG);memEnG=0;}
	if(memEnB){AlgoFree((TAny*) memEnB);memEnB=0;}
	if(memTempYUV){AlgoFree((TAny*) memTempYUV);memTempYUV = 0;};

	return error;
}
