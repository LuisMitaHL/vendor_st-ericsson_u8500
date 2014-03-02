/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
* \file     algoextractionroutines.c
* \brief    Contains Algorithm Library Image Extraction Routines
* \author   ST Ericsson
*/

/*
 * Defines
 */
#define SWAP(a) (((a&0xFF)<<24)|((a&0xFF00)<<8)|((a&0xFF0000)>>8)|((a>>24)&0xFF))

/*
 * Includes 
 */
#include "algoextractionroutines.h"
#include "algomemoryroutines.h"


/**
 * Routine to convert a RGB565 Channel into seperate R, G and B Channels.
 *
 * @param aInputImage           [TUint8*] The input RGB565 image channel.
 * @param aOutputImage          [TAlgoImageDataRGB*] The seperate R, G and B channels.
 * @param aImageParams          [TAlgoImageParams*] The image parameters.
 *
 * @return                      [TAlgoError] EErrorNone if the conversion is successful.
 */
TAlgoError RGB565toRGBPlanes(TUint8* aInputImage, 
                    TAlgoImageDataRGB* aOutputImage, 
                    TAlgoImageParams* aImageParams)
    {
	TUint32 current_pixel_x = 0;
	TUint32 current_pixel_y = 0;
	TUint8 rgb2byte[2] = {0,0};
    TUint8* RChannel = aOutputImage->iRChannel;
    TUint8* GChannel = aOutputImage->iGChannel;
    TUint8* BChannel = aOutputImage->iBChannel;
	TInt32 nb_extra_due_to_stride = 0;

	ALGO_Log_1(" Color conversion %s \n", __FUNCTION__);

	if(	aImageParams->iImageWidth * 2 != aImageParams->iImageStride)
	{
		nb_extra_due_to_stride = aImageParams->iImageStride - (aImageParams->iImageWidth*2);
		ALGO_Log_1(" nb_extra_due_to_stride %d \n", nb_extra_due_to_stride);
	}

    aInputImage = aInputImage + aImageParams->iImageHeaderLength;
   

	ALGO_Log_0(" Prepare support of stride \n");
	for (current_pixel_y = 0; current_pixel_y < aImageParams->iImageHeight; current_pixel_y++) 
	{
		for (current_pixel_x = 0; current_pixel_x < aImageParams->iImageWidth; current_pixel_x++) 
		{
			rgb2byte[0] = *aInputImage;
			aInputImage++;
			rgb2byte[1] = *aInputImage;
			aInputImage++;

			*BChannel = (rgb2byte[0] & 0x1F);	// First 5 bits 
			*GChannel = (rgb2byte[0] >> 5) + ((rgb2byte[1] & 0x07) << 3);	// Middle 6 bits 
			*RChannel = rgb2byte[1] >> 3;			// Last 5 bits 

			// quantification on 8 bits
			*RChannel = *RChannel << 3;
			*GChannel = *GChannel << 2;
			*BChannel = *BChannel << 3;

			RChannel++;
			GChannel++;
			BChannel++;

		}
			aInputImage += nb_extra_due_to_stride;
	}

    return EErrorNone;
    }

/**
 * Routine to convert a BGR565 Channel into seperate R, G and B Channels.
 *
 * @param aInputImage           [TUint8*] The input BGR565 image channel.
 * @param aOutputImage          [TAlgoImageDataRGB*] The seperate R, G and B channels.
 * @param aImageParams          [TAlgoImageParams*] The image parameters.
 *
 * @return                      [TAlgoError] EErrorNone if the conversion is successful.
 */
TAlgoError BGR565toRGBPlanes(TUint8* aInputImage, 
                    TAlgoImageDataRGB* aOutputImage, 
                    TAlgoImageParams* aImageParams)
    {
	TUint32 current_pixel_x = 0;
	TUint32 current_pixel_y = 0;
	TUint8 rgb2byte[2] = {0,0};
    TUint8* RChannel = aOutputImage->iRChannel;
    TUint8* GChannel = aOutputImage->iGChannel;
    TUint8* BChannel = aOutputImage->iBChannel;
	TInt32 nb_extra_due_to_stride = 0;

	ALGO_Log_1(" Color conversion %s \n", __FUNCTION__);

	if(	aImageParams->iImageWidth * 2 != aImageParams->iImageStride)
	{
		nb_extra_due_to_stride = aImageParams->iImageStride - (aImageParams->iImageWidth*2);
		ALGO_Log_1(" nb_extra_due_to_stride %d \n", nb_extra_due_to_stride);
	}

	aInputImage = aInputImage + aImageParams->iImageHeaderLength;

	ALGO_Log_0(" Prepare support of stride \n");
	for (current_pixel_y = 0; current_pixel_y < aImageParams->iImageHeight; current_pixel_y++) 
	{
		for (current_pixel_x = 0; current_pixel_x < aImageParams->iImageWidth; current_pixel_x++) 
		{
			rgb2byte[0] = *aInputImage;
			aInputImage++;
			rgb2byte[1] = *aInputImage;
			aInputImage++;

			*RChannel = (rgb2byte[0] & 0x1F);	// First 5 bits 
			*GChannel = (rgb2byte[0] >> 5) + ((rgb2byte[1] & 0x07) << 3);	// Middle 6 bits 
			*BChannel = rgb2byte[1] >> 3;			// Last 5 bits 

			// quantification on 8 bits
			*RChannel = *RChannel << 3;
			*GChannel = *GChannel << 2;
			*BChannel = *BChannel << 3;

			RChannel++;
			GChannel++;
			BChannel++;

		}
 
			aInputImage += nb_extra_due_to_stride;
	}

    return EErrorNone;
    }


/**
 * Routine to convert a RGB24 Channel into seperate R, G and B Channels.
 *
 * @param aInputImage           [TUint8*] The input RGB24 image channel.
 * @param aOutputImage          [TAlgoImageDataRGB*] The seperate R, G and B channels.
 * @param aImageParams          [TAlgoImageParams*] The image parameters.
 *
 * @return                      [TAlgoError] EErrorNone if the conversion is successful.
 */
TAlgoError RGB24toRGBPlanes(TUint8* aInputImage, 
                    TAlgoImageDataRGB* aOutputImage, 
                    TAlgoImageParams* aImageParams)
    {
 	TUint32 current_pixel_x = 0;
	TUint32 current_pixel_y = 0;
    TUint8* RChannel = aOutputImage->iRChannel;
    TUint8* GChannel = aOutputImage->iGChannel;
    TUint8* BChannel = aOutputImage->iBChannel;
	TInt32 nb_extra_due_to_stride = 0;

	ALGO_Log_1(" Color conversion %s \n", __FUNCTION__);

	if(	aImageParams->iImageWidth * 3 != aImageParams->iImageStride)
	{
		nb_extra_due_to_stride = aImageParams->iImageStride - (aImageParams->iImageWidth*3);
		ALGO_Log_1(" nb_extra_due_to_stride %d \n", nb_extra_due_to_stride);
	}

	aInputImage = aInputImage + aImageParams->iImageHeaderLength;

	ALGO_Log_0(" Prepare support of stride \n");
	for (current_pixel_y = 0; current_pixel_y < aImageParams->iImageHeight; current_pixel_y++) 
	{
		for (current_pixel_x = 0; current_pixel_x < aImageParams->iImageWidth; current_pixel_x++) 
		{
			*BChannel = *aInputImage;
			aInputImage++;
			*GChannel = *aInputImage;
			aInputImage++;
			*RChannel = *aInputImage;
			aInputImage++;

			RChannel++;
			GChannel++;
			BChannel++;

		}
			aInputImage += nb_extra_due_to_stride;
	}

    return EErrorNone;
    }

/**
 * Routine to convert a BGR24 Channel into seperate R, G and B Channels.
 *
 * @param aInputImage           [TUint8*] The input BGR24 image channel.
 * @param aOutputImage          [TAlgoImageDataRGB*] The seperate R, G and B channels.
 * @param aImageParams          [TAlgoImageParams*] The image parameters.
 *
 * @return                      [TAlgoError] EErrorNone if the conversion is successful.
 */
TAlgoError BGR24toRGBPlanes(TUint8* aInputImage, 
                    TAlgoImageDataRGB* aOutputImage, 
                    TAlgoImageParams* aImageParams)
    {
 	TUint32 current_pixel_x = 0;
	TUint32 current_pixel_y = 0;
    TUint8* RChannel = aOutputImage->iRChannel;
    TUint8* GChannel = aOutputImage->iGChannel;
    TUint8* BChannel = aOutputImage->iBChannel;
	TInt32 nb_extra_due_to_stride = 0;

	ALGO_Log_1(" Color conversion %s \n", __FUNCTION__);

	if(	aImageParams->iImageWidth * 3 != aImageParams->iImageStride)
	{
		nb_extra_due_to_stride = aImageParams->iImageStride - (aImageParams->iImageWidth*3);
		ALGO_Log_1(" nb_extra_due_to_stride %d \n", nb_extra_due_to_stride);
	}

	aInputImage = aInputImage + aImageParams->iImageHeaderLength;

	ALGO_Log_0(" Prepare support of stride \n");
	for (current_pixel_y = 0; current_pixel_y < aImageParams->iImageHeight; current_pixel_y++) 
	{
		for (current_pixel_x = 0; current_pixel_x < aImageParams->iImageWidth; current_pixel_x++) 
		{
			*RChannel = *aInputImage;
			aInputImage++;
			*GChannel = *aInputImage;
			aInputImage++;
			*BChannel = *aInputImage;
			aInputImage++;

			RChannel++;
			GChannel++;
			BChannel++;

		}
			aInputImage += nb_extra_due_to_stride;
	}

    return EErrorNone;
    }



TAlgoError BMP_BGR24toRGBPlanes(TUint8* aInputImage, 
                    TAlgoImageDataRGB* aOutputImage, 
                    TAlgoImageParams* aImageParams)
{
 	TUint32 current_pixel_x = 0;
	TUint32 current_pixel_y = 0;
    TUint8* RChannel = aOutputImage->iRChannel;
    TUint8* GChannel = aOutputImage->iGChannel;
    TUint8* BChannel = aOutputImage->iBChannel;
	TUint32 nb_byte_of_padding_by_row = 0;

	ALGO_Log_1(" Color conversion %s \n", __FUNCTION__);
// see http://en.wikipedia.org/wiki/BMP_file_format
	if ((aImageParams->iImageWidth * 3) % 4)
	{
		nb_byte_of_padding_by_row = 4 - (aImageParams->iImageWidth * 3) % 4;
	}
	ALGO_Log_1(" nb_byte_of_padding_by_row %d \n", nb_byte_of_padding_by_row);

	aInputImage = aInputImage + aImageParams->iImageHeaderLength;
	// go to last stored row (which corresponds to first row of pixel from image)
	aInputImage += (aImageParams->iImageWidth * 3 + nb_byte_of_padding_by_row) * (aImageParams->iImageHeight - 1);

	for (current_pixel_y = 0; current_pixel_y < aImageParams->iImageHeight; current_pixel_y++) 
	{
		for (current_pixel_x = 0; current_pixel_x < aImageParams->iImageWidth; current_pixel_x++) 
		{
			*BChannel = *aInputImage;
			aInputImage++;
			*GChannel = *aInputImage;
			aInputImage++;
			*RChannel = *aInputImage;
			aInputImage++;

			RChannel++;
			GChannel++;
			BChannel++;

		}
		aInputImage += nb_byte_of_padding_by_row;
		aInputImage -= 2 * (aImageParams->iImageWidth * 3 + nb_byte_of_padding_by_row);
	}

    return EErrorNone;
}



/**
 * Routine to convert a ARGB4444 Channel into seperate R, G and B Channels.
 *
 * @param aInputImage           [TUint8*] The input ARGB4444 image channel.
 * @param aOutputImage          [TAlgoImageDataRGB*] The seperate R,G and B channels.
 * @param aImageParams          [TAlgoImageParams*] The image parameters.
 *
 * @return                      [TAlgoError] EErrorNone if the conversion is successful.
 */
TAlgoError ARGB4444toRGBPlanes(TUint8* aInputImage, 
                    TAlgoImageDataRGB* aOutputImage, 
                    TAlgoImageParams* aImageParams)
    {
 	TUint32 current_pixel_x = 0;
	TUint32 current_pixel_y = 0;
	TUint8* RChannel = aOutputImage->iRChannel;
	TUint8* GChannel = aOutputImage->iGChannel;
	TUint8* BChannel = aOutputImage->iBChannel;
	TInt32 nb_extra_due_to_stride = 0;

	ALGO_Log_1(" Color conversion %s \n", __FUNCTION__);

	if(	aImageParams->iImageWidth * 2 != aImageParams->iImageStride)
	{
		nb_extra_due_to_stride = aImageParams->iImageStride - (aImageParams->iImageWidth*2);
		ALGO_Log_1(" nb_extra_due_to_stride %d \n", nb_extra_due_to_stride);
	}

	aInputImage = aInputImage + aImageParams->iImageHeaderLength;

	for (current_pixel_y = 0; current_pixel_y < aImageParams->iImageHeight; current_pixel_y++) 
	{
		for (current_pixel_x = 0; current_pixel_x < aImageParams->iImageWidth; current_pixel_x++) 
		{
			*BChannel = (*aInputImage) & 0x0F;
			*GChannel = (*aInputImage) >> 4;
			aInputImage++;
			*RChannel = (*aInputImage) & 0x0F;
			aInputImage++;
			// quantification on 8 bits
			*RChannel = *RChannel << 4;
			*GChannel = *GChannel << 4;
			*BChannel = *BChannel << 4;
			RChannel++;
			GChannel++;
			BChannel++;

		}
			aInputImage += nb_extra_due_to_stride;
	}

	return EErrorNone;
    }

/**
 * Routine to convert a ARGB1555 Channel into seperate R, G and B Channels.
 *
 * @param aInputImage           [TUint8*] The input ARGB1555 image channel.
 * @param aOutputImage          [TAlgoImageDataRGB*] The seperate R,G and B channels.
 * @param aImageParams          [TAlgoImageParams*] The image parameters.
 *
 * @return                      [TAlgoError] EErrorNone if the conversion is successful.
 */
TAlgoError ARGB1555toRGBPlanes(TUint8* aInputImage, 
                    TAlgoImageDataRGB* aOutputImage, 
                    TAlgoImageParams* aImageParams)
    {
 	TUint32 current_pixel_x = 0;
	TUint32 current_pixel_y = 0;
	TUint8 byte_1, byte_2;
	TUint8* RChannel = aOutputImage->iRChannel;
	TUint8* GChannel = aOutputImage->iGChannel;
	TUint8* BChannel = aOutputImage->iBChannel;
	TInt32 nb_extra_due_to_stride = 0;

	ALGO_Log_1(" Color conversion %s \n", __FUNCTION__);

	if(	aImageParams->iImageWidth * 2 != aImageParams->iImageStride)
	{
		nb_extra_due_to_stride = aImageParams->iImageStride - (aImageParams->iImageWidth*2);
		ALGO_Log_1(" nb_extra_due_to_stride %d \n", nb_extra_due_to_stride);
	}

	aInputImage = aInputImage + aImageParams->iImageHeaderLength;

	for (current_pixel_y = 0; current_pixel_y < aImageParams->iImageHeight; current_pixel_y++) 
	{
		for (current_pixel_x = 0; current_pixel_x < aImageParams->iImageWidth; current_pixel_x++) 
		{
			byte_1 = *aInputImage;
			aInputImage++;
			byte_2 = *aInputImage;
			aInputImage++;

			*BChannel = byte_1 & 0x1F;
			*GChannel = ((byte_1 & 0xE0) >> 5) + ((byte_2 & 0x03) << 3);
			*RChannel = ((byte_2 & 0x7F) >> 2);

			*RChannel = *RChannel << 3;
			*GChannel = *GChannel << 3;
			*BChannel = *BChannel << 3;
			RChannel++;
			GChannel++;
			BChannel++;

		}
			aInputImage += nb_extra_due_to_stride;
	}

	return EErrorNone;
    }



/**
 * Routine to convert a ARGB8888 Channel into seperate R, G and B Channels.
 *
 * @param aInputImage           [TUint8*] The input ARGB8888 image channel.
 * @param aOutputImage          [TAlgoImageDataRGB*] The seperate R,G and B channels.
 * @param aImageParams          [TAlgoImageParams*] The image parameters.
 *
 * @return                      [TAlgoError] EErrorNone if the conversion is successful.
 */
TAlgoError ARGB8888toRGBPlanes(TUint8* aInputImage, 
                    TAlgoImageDataRGB* aOutputImage, 
                    TAlgoImageParams* aImageParams)
    {
 	TUint32 current_pixel_x = 0;
	TUint32 current_pixel_y = 0;
	TUint8* RChannel = aOutputImage->iRChannel;
	TUint8* GChannel = aOutputImage->iGChannel;
	TUint8* BChannel = aOutputImage->iBChannel;
	TInt32 nb_extra_due_to_stride = 0;

	ALGO_Log_1(" Color conversion %s \n", __FUNCTION__);

	if(	aImageParams->iImageWidth * 4 != aImageParams->iImageStride)
	{
		nb_extra_due_to_stride = aImageParams->iImageStride - (aImageParams->iImageWidth*4);
		ALGO_Log_1(" nb_extra_due_to_stride %d \n", nb_extra_due_to_stride);
	}

	aInputImage = aInputImage + aImageParams->iImageHeaderLength;

	for (current_pixel_y = 0; current_pixel_y < aImageParams->iImageHeight; current_pixel_y++) 
	{
		for (current_pixel_x = 0; current_pixel_x < aImageParams->iImageWidth; current_pixel_x++) 
		{

			*BChannel = *aInputImage;
			aInputImage++;
			*GChannel = *aInputImage;
			aInputImage++;
			*RChannel = *aInputImage;
			aInputImage++;

			aInputImage++; // skip A

			RChannel++;
			GChannel++;
			BChannel++;

		}
			aInputImage += nb_extra_due_to_stride;
	}

	return EErrorNone;
    }


/**
 * Routine to convert seperate R, G and B Channels into a RGB888 Channel.  
 *
 * @param aInputImage           [TAlgoImageDataRGB*] The seperate R,G and B channels.
 * @param aOutputImage          [TUint8*] The output RGB888 image channel.
 * @param aImageParams          [TAlgoImageParams*] The image parameters.
 *
 * @return                      [TAlgoError] EErrorNone if the conversion is successful.
 */
TAlgoError RGBPlanestoRGB888(TAlgoImageDataRGB* aInputImage, 
                             TUint8* aOutputImage, 
                             TAlgoImageParams* aImageParams)
    {
    TUint32 loop = 0;

    TUint8* RChannel = aInputImage->iRChannel;
    TUint8* GChannel = aInputImage->iGChannel;
    TUint8* BChannel = aInputImage->iBChannel;

	ALGO_Log_1(" Color conversion %s \n", __FUNCTION__);

	aOutputImage = aOutputImage + aImageParams->iImageHeaderLength;

    loop=0;
    do
	    {
        *aOutputImage = *RChannel;
        aOutputImage++;
        *aOutputImage = *GChannel;
        aOutputImage++;
        *aOutputImage = *BChannel;
        aOutputImage++;

        RChannel++;
        GChannel++;
        BChannel++;

        loop++;
	    }
        while(loop<( aImageParams->iImageWidth * aImageParams->iImageHeight ));

    return EErrorNone;
    }

/**
 * Routine to convert YUV(422) Interleaved buffer to seperate R, G and B channels.  
 *
 * @param aInputImage           [TUint8*] The input YUV(422) Planar image channel.
 * @param aOutputImage          [TAlgoImageDataRGB*] The seperate R,G and B channels.
 * @param aImageParams          [TAlgoImageParams*] The image parameters.
 *
 * @return                      [TAlgoError] EErrorNone if the conversion is successful.
 */

TAlgoError YUV422InterleavedtoRGBPlanes(TUint8* aInputImage, 
                                   TAlgoImageDataRGB* aOutputImage, 
                                   TAlgoImageParams* aImageParams)
    {   
    TUint32 x = 0;
    TUint32 vert = 0;
    TUint32 num_of_pixels_x = aImageParams->iImageWidth;
    TUint32 num_of_pixels_y = aImageParams->iImageHeight; 
    TUint8 u = 0;
    TUint8 y1 = 0;
    TUint8 v = 0;
    TUint8 y2 = 0;
    TInt32 current_pixel_R = 0;
    TInt32 current_pixel_G = 0;
    TInt32 current_pixel_B = 0;

    TUint8* im_Input = aInputImage;

    TUint8* RChannel = aOutputImage->iRChannel;
    TUint8* GChannel = aOutputImage->iGChannel;
    TUint8* BChannel = aOutputImage->iBChannel;

#ifdef  IMPLEMENTATION_OLD
    TInt32 C = 0;
    TInt32 D = 0;
    TInt32 E = 0;
		ALGO_Log_1(" Color conversion %s : implementation OLD \n", __FUNCTION__);
#else
		ALGO_Log_1(" Color conversion %s : implementation NEW \n", __FUNCTION__);
		ALGO_Log_0(" from (http://en.wikipedia.org/wiki/YUV)\n");
#endif
	if( (aImageParams->iImageWidth % 2) != 0 )
	{
		ALGO_Log_3(" Error, bad parameter, in fct %s, line %d, width%2 != 0 as it is %d\n", __FUNCTION__, __LINE__, aImageParams->iImageWidth);
	}

//	im_Input = im_Input + aImageParams->iImageHeaderLength;

   for(vert=0; vert<num_of_pixels_y; vert++)
    {
    for(x=0; x<num_of_pixels_x; x=x+2)
        {
        u  = *im_Input;
        im_Input++;
        y1 = *im_Input;
        im_Input++;
        v  = *im_Input;
        im_Input++;
        y2 = *im_Input;
        im_Input++;

#ifdef  IMPLEMENTATION_OLD
        C = y1 - 16;
        D = u - 128;
        E = v - 128;
        current_pixel_R = (( 298 * C           + 409 * E + 128) >> 8);
        current_pixel_G = (( 298 * C - 100 * D - 208 * E + 128) >> 8);
        current_pixel_B = (( 298 * C + 516 * D           + 128) >> 8);
#else
		current_pixel_R = (9535 * (y1 - 16) + 13074 * (v - 128)) >> 13;
		current_pixel_G = (9535 * (y1 - 16) -  6660 * (v - 128) - 3203 * (u - 128)) >> 13;
		current_pixel_B = (9535 * (y1 - 16) + 16531 * (u - 128)) >> 13; 
#endif

//		ALGO_Log_5(" i vert = %d, x = %d, y1 = %d, u = %d, v =%d \n", vert, x, y1, u, v);
//		ALGO_Log_5(" a vert = %d, x = %d, R = %d, G = %d, B =%d \n", vert, x, current_pixel_R, current_pixel_G, current_pixel_B);

		if (current_pixel_R < 0) current_pixel_R = 0;
        if (current_pixel_G < 0) current_pixel_G = 0;
		if (current_pixel_B < 0) current_pixel_B = 0;
			
		if (current_pixel_R > 255) current_pixel_R = 255;
		if (current_pixel_G > 255) current_pixel_G = 255;
		if (current_pixel_B > 255) current_pixel_B = 255;

//		ALGO_Log_5(" b vert = %d, x = %d, R = %d, G = %d, B =%d \n", vert, x, current_pixel_R, current_pixel_G, current_pixel_B);

		*RChannel = current_pixel_R;
        RChannel++;
        *GChannel = current_pixel_G;
        GChannel++;
        *BChannel = current_pixel_B;
        BChannel++;

#ifdef  IMPLEMENTATION_OLD
        C = y2 - 16;
        current_pixel_R = (( 298 * C           + 409 * E + 128) >> 8);
        current_pixel_G = (( 298 * C - 100 * D - 208 * E + 128) >> 8);
        current_pixel_B = (( 298 * C + 516 * D           + 128) >> 8);
#else
		current_pixel_R = (9535 * (y2 - 16) + 13074 * (v - 128)) >> 13;
		current_pixel_G = (9535 * (y2 - 16) -  6660 * (v - 128) - 3203 * (u - 128)) >> 13;
		current_pixel_B = (9535 * (y2 - 16) + 16531 * (u - 128)) >> 13; 
#endif

//		ALGO_Log_5(" i vert = %d, x = %d, y2 = %d, u = %d, v =%d \n", vert, x, y2, u, v);
//		ALGO_Log_5(" c vert = %d, x = %d, R = %d, G = %d, B =%d \n", vert, x, current_pixel_R, current_pixel_G, current_pixel_B);

		if (current_pixel_R < 0) current_pixel_R = 0;
        if (current_pixel_G < 0) current_pixel_G = 0;
		if (current_pixel_B < 0) current_pixel_B = 0;
			
		if (current_pixel_R > 255) current_pixel_R = 255;
		if (current_pixel_G > 255) current_pixel_G = 255;
		if (current_pixel_B > 255) current_pixel_B = 255;

//		ALGO_Log_5(" d vert = %d, x = %d, R = %d, G = %d, B =%d \n", vert, x, current_pixel_R, current_pixel_G, current_pixel_B);

        *RChannel = current_pixel_R;
        RChannel++;
        *GChannel = current_pixel_G;
        GChannel++;
        *BChannel = current_pixel_B;
        BChannel++;

        }
        im_Input+= (aImageParams->iImageStride - (aImageParams->iImageWidth*2));
      }

    return EErrorNone;
    }

/**
 * Routine to convert YUV(420) packed planar buffer to seperate R, G and B Channels.  
 *
 * @param aInputImage           [TUint8*] The input YUV420 image channel.
 * @param aOutputImage          [TAlgoImageDataRGB*] The seperate R,G and B channels.
 * @param aImageParams          [TAlgoImageParams*] The image parameters.
 *
 * @return                      [TAlgoError] EErrorNone if the conversion is successful.
 */

TAlgoError YUV420PackedPlanarI420toRGBPlanes(TUint8* aInputImage, 
                                   TAlgoImageDataRGB* aOutputImage, 
                                   TAlgoImageParams* aImageParams)
{
	TInt32 current_pixel_Y = 0; // current pixel YCbCr 4:2:0 components 
	TInt32 current_pixel_Cb = 0;
	TInt32 current_pixel_Cr = 0;
	TInt32 current_pixel_R = 0; // current pixel RGB 888 components 
	TInt32 current_pixel_G = 0;
	TInt32 current_pixel_B = 0;
	TUint32 current_pixel_x = 0;
	TUint32 current_pixel_y = 0;
    TUint32 parmaHeight = aImageParams->iImageSliceHeight;


#ifdef STRIDE_IMPLEMENTATION_CORRECT
// when camera will set correctly stride for OMX_COLOR_FormatYUV420PackedPlanar, activate next line
  	TInt32 im_stride_pels = (TInt32)((float)aImageParams->iImageStride);
#else
  	TInt32 im_stride_pels = (TInt32)(((float)aImageParams->iImageStride)/1.5);
#endif
	TInt32 picture_number_of_pixels = im_stride_pels * parmaHeight;
	TUint8* cb_start = aInputImage + picture_number_of_pixels;
	TUint8* cr_start = cb_start + (picture_number_of_pixels>>2);
	TInt32 cb_x = 0;
	TInt32 cb_y = 0;

    TUint8* RChannel = aOutputImage->iRChannel;
    TUint8* GChannel = aOutputImage->iGChannel;
    TUint8* BChannel = aOutputImage->iBChannel;
	
	ALGO_Log_1(" Color conversion %s \n", __FUNCTION__);

//	aInputImage = aInputImage + aImageParams->iImageHeaderLength;

	for (current_pixel_y = 0; current_pixel_y < aImageParams->iImageHeight; current_pixel_y++) 
	{
		for (current_pixel_x = 0; current_pixel_x < aImageParams->iImageWidth; current_pixel_x++) 
		{
			current_pixel_Y = (*(aInputImage + (current_pixel_y * im_stride_pels) + current_pixel_x)) & 0xFF; // get current pixel Y component 
			cb_x = current_pixel_x>>1;
			cb_y = current_pixel_y>>1;
			current_pixel_Cb = (*(cb_start + (cb_y*(im_stride_pels >> 1)) + cb_x)) & 0xFF; // get current pixel Cb component 
			current_pixel_Cr = (*(cr_start + (cb_y*(im_stride_pels >> 1)) + cb_x)) & 0xFF;
			
			current_pixel_R = ((16384 +  ((current_pixel_Y)<<15) + 45941*((current_pixel_Cr)-128))>>15);
			current_pixel_G = ((16384 +  ((current_pixel_Y)<<15) - 11272*((current_pixel_Cb)-128) - 23396*((current_pixel_Cr)-128))>>15);
			current_pixel_B = ((16384 +  ((current_pixel_Y)<<15) + 58065*((current_pixel_Cb)-128))>>15);
			
			if (current_pixel_R < 0) current_pixel_R = 0;
			if (current_pixel_G < 0) current_pixel_G = 0;
			if (current_pixel_B < 0) current_pixel_B = 0;
			
			if (current_pixel_R > 255) current_pixel_R = 255;
			if (current_pixel_G > 255) current_pixel_G = 255;
			if (current_pixel_B > 255) current_pixel_B = 255;

            *RChannel = current_pixel_R;
            RChannel++;
            *GChannel = current_pixel_G;
            GChannel++;
            *BChannel = current_pixel_B;
            BChannel++;
		}
	}

    return EErrorNone;
}


/**
 * Routine to convert a YUV420MBSemiPlanarFiltered buffer to YUV(420) Planar buffer.
 * This function is always followed by YUV420PackedPlanarI420toRGBPlanes, so the strategy
 * is to keep stride unchanged in this one and only remove windowing when converting to RGB  
 *
 * @param aInputImage           [TUint8*] The input YUV(420) MB SemiPlanar image channel.
 * @param aOutputImage          [TUint8*] The output YUV(420) Planar image channel.
 * @param aImageParams          [TAlgoImageParams*] The image parameters.
 *
 * @return                      [TAlgoError] EErrorNone if the conversion is successful.
 */
TAlgoError YUV420MBSemiPlanarFilteredtoYUV420Planar(TUint8* aInputImage, 
                                                    TUint8* aOutputImage, 
                                                    TAlgoImageParams* aImageParams)
    {
    TInt32 h,v,k;
	TInt32 im_stride_pels = (TInt32)(((float)aImageParams->iImageStride)/1.5);
    TInt32 im_h_mb = im_stride_pels/8;	/* horizontal size in words */
    TInt32 im_v_mb = 0; /* vertical size in macroblocks */
	
    TUint8* im_in;
    TUint8* im_out;
    TUint8* im_out_u;
    TUint8* im_out_v;
	TUint32 parmaHeight = aImageParams->iImageSliceHeight;
	
	ALGO_Log_1(" Color conversion %s \n", __FUNCTION__);

	ALGO_Log_4("param width  %d, height %d, stride %d, sliceheight %d\n", aImageParams->iImageWidth, aImageParams->iImageHeight, aImageParams->iImageStride, aImageParams->iImageSliceHeight);
	if( (aImageParams->iImageWidth % 8) != 0 )
	{
		ALGO_Log_3(" Error, bad parameter, in fct %s, line %d, width%8 != 0 as it is %d\n", __FUNCTION__, __LINE__, aImageParams->iImageWidth);
		return EErrorNotValidated;
	}

	if( (parmaHeight % 16) != 0 )
	{
		ALGO_Log_3(" Error, bad parameter, in fct %s, line %d, parmaHeight%16 != 0 as it is %d\n", __FUNCTION__, __LINE__, parmaHeight);
		return EErrorNotValidated;
	}
    im_v_mb = parmaHeight/16; 

	im_in = aInputImage;
//	im_in = im_in + aImageParams->iImageHeaderLength;
	im_out = aOutputImage;
    im_out_u = im_out + im_stride_pels * parmaHeight;    		/* pointer to U component */
    im_out_v = im_out_u + ((im_stride_pels * parmaHeight)>>2);	/* pointer to V component */

	/*	Y conversion */
	for(v=0;v<im_v_mb;v++)								/* loop on macroblocks */
	    {
		for(h=0;h<im_h_mb;h++)							/* loop on columns of words */
		    {
			for(k=0;k<16;k++)							/* loop on words */
			    {
				TUint8 tmp_int[8];
                AlgoMemcpy(tmp_int, im_in, 8);
                AlgoMemcpy(im_out, tmp_int, 8);
				
				im_in += 8;		    					/* next word in MB-tiled organization */
                im_out += im_stride_pels;	/* ...is on the next line in plane organization */
			    }
			
            im_out -= 16 * im_stride_pels; 	 /* go up 16 lines */
			im_out += 8;									
		    }
        im_out += 15 * im_stride_pels;		/* move to next macroblock line */
	    }

	/* UV conversion */ 
	for(v=0;v<(im_v_mb);v++)								/* loop on macroblocks */
	    {
		for(h=0;h<(im_h_mb);h++)						/* loop on columns of words */
		    {
			for(k=0;k<8;k++)							/* loop on word */
			    {    
				*im_out_u = *im_in;
                im_out_u++;
                im_in++;
                *im_out_v = *im_in;
                im_out_v++;
                im_in++;

                *im_out_u = *im_in;
                im_out_u++;
                im_in++;
                *im_out_v = *im_in;
                im_out_v++;
                im_in++;

                *im_out_u = *im_in;
                im_out_u++;
                im_in++;
                *im_out_v = *im_in;
                im_out_v++;
                im_in++;

                *im_out_u = *im_in;
                im_out_u++;
                im_in++;
                *im_out_v = *im_in;
                im_out_v++;
                im_in++;

                im_out_u += ((im_stride_pels>>1)-4);				
				im_out_v += ((im_stride_pels>>1)-4);
			    }	
            im_out_u-=8*(im_stride_pels>>1);					
			im_out_v-=8*(im_stride_pels>>1); 
			im_out_u+=4;								
			im_out_v+=4;
		    }
        im_out_u+=7*(im_stride_pels>>1);						
		im_out_v+=7*(im_stride_pels>>1);
	    }

    return EErrorNone;
    }



/**
 * Routine to convert a RAW8 Channel into seperate R, G and B Channels.
 *
 * @param aInputImage           [TUint8*] The input RAW8 image channel.
 * @param aOutputImage          [TAlgoImageDataRGB*] The seperate R,G and B channels.
 * @param aImageParams          [TAlgoImageParams*] The image parameters.
 *
 * @return                      [TAlgoError] EErrorNone if the conversion is successful.
 */
TAlgoError RAW8toRGBPlanes(TUint8* aInputImage, 
                    TAlgoImageDataRGB* aOutputImage, 
                    TAlgoImageParams* aImageParams,
					TUint32* aNbStatusLine)
{
	TUint32 current_pixel_x = 0;
	TUint32 current_pixel_y = 0;
	TUint8* RChannel = aOutputImage->iRChannel;
	TUint8* GChannel = aOutputImage->iGChannel;
	TUint8* BChannel = aOutputImage->iBChannel;
	TUint32 nb_status_line = aImageParams->iImageNbStatusLine;
	TInt32 nb_extra_due_to_stride = 0;
	TUint32 iImageHeightUsed = 0;

	ALGO_Log_1("\n Color conversion %s \n", __FUNCTION__);

	if(	aImageParams->iImageWidth * 1 != aImageParams->iImageStride)
	{
		nb_extra_due_to_stride = aImageParams->iImageStride - (aImageParams->iImageWidth*1);
	}

	aInputImage = aInputImage + aImageParams->iImageHeaderLength;
	
	ALGO_Log_1(" nb_status_line = %d\n", nb_status_line);
	ALGO_Log_1(" iImagePixelOrder = %d\n", aImageParams->iImagePixelOrder);
	//ALGO_Log_1("nb_extra_due_to_stride = %d\n", nb_extra_due_to_stride);

	if(	aImageParams->iImageWidth * 1 != aImageParams->iImageStride)
	{
		ALGO_Log_2("INFO : %d = width != stride = %d\n", aImageParams->iImageWidth, aImageParams->iImageStride);
	}

	aInputImage += nb_status_line * aImageParams->iImageStride;
	iImageHeightUsed = aImageParams->iImageHeight - nb_status_line;
	*aNbStatusLine = nb_status_line;
	ALGO_Log_4("working with width = %d, height = %d, stride = %d => equivalente width %d\n", aImageParams->iImageWidth, iImageHeightUsed, aImageParams->iImageStride, aImageParams->iImageStride*1/1);

	switch(aImageParams->iImagePixelOrder)
	{
	case 0:
		// GRGRGR...
		// BGBGBG...
		for (current_pixel_y = 0; current_pixel_y < iImageHeightUsed / 2; current_pixel_y++) 
		{
			for (current_pixel_x = 0; current_pixel_x < aImageParams->iImageWidth / 2; current_pixel_x++) 
			{
				GChannel[0] = aInputImage[0];
				RChannel[0] = aInputImage[1];
				BChannel[0] = aInputImage[0 + aImageParams->iImageStride];
				GChannel[1] = GChannel[0];
				RChannel[1] = RChannel[0];
				BChannel[1] = BChannel[0];

				GChannel[0 + aImageParams->iImageWidth] = aInputImage[1 + aImageParams->iImageStride];
				RChannel[0 + aImageParams->iImageWidth] = RChannel[0];
				BChannel[0 + aImageParams->iImageWidth] = BChannel[0];
				GChannel[1 + aImageParams->iImageWidth] = GChannel[0 + aImageParams->iImageWidth];
				RChannel[1 + aImageParams->iImageWidth] = RChannel[0];
				BChannel[1 + aImageParams->iImageWidth] = BChannel[0];

				aInputImage += 2;

				RChannel += 2;
				GChannel += 2;
				BChannel += 2;
			}

			RChannel += aImageParams->iImageWidth;
			GChannel += aImageParams->iImageWidth;
			BChannel += aImageParams->iImageWidth;
			aInputImage += aImageParams->iImageWidth * 1;
			if(	nb_extra_due_to_stride < 0)
			{
				ALGO_Log_0("ERROR bad STRIDE\n");
			}
			else
			{
				aInputImage += nb_extra_due_to_stride * 2; // * 2 because 2 lignes are processed simultaneously
			}
		}
		break;
	case 2:
		// GBGBGB...
		// RGRGRG...
		for (current_pixel_y = 0; current_pixel_y < iImageHeightUsed / 2; current_pixel_y++) 
		{
			for (current_pixel_x = 0; current_pixel_x < aImageParams->iImageWidth / 2; current_pixel_x++) 
			{
				GChannel[0] = aInputImage[0];
				BChannel[0] = aInputImage[1];
				RChannel[0] = aInputImage[0 + aImageParams->iImageStride];
				GChannel[1] = GChannel[0];
				BChannel[1] = BChannel[0];
				RChannel[1] = RChannel[0];

				GChannel[0 + aImageParams->iImageWidth] = aInputImage[1 + aImageParams->iImageStride];
				BChannel[0 + aImageParams->iImageWidth] = BChannel[0];
				RChannel[0 + aImageParams->iImageWidth] = RChannel[0];
				GChannel[1 + aImageParams->iImageWidth] = GChannel[0 + aImageParams->iImageWidth];
				BChannel[1 + aImageParams->iImageWidth] = BChannel[0];
				RChannel[1 + aImageParams->iImageWidth] = RChannel[0];

				aInputImage += 2;

				RChannel += 2;
				GChannel += 2;
				BChannel += 2;
			}

			RChannel += aImageParams->iImageWidth;
			GChannel += aImageParams->iImageWidth;
			BChannel += aImageParams->iImageWidth;
			aInputImage += aImageParams->iImageWidth * 1;
			if(	nb_extra_due_to_stride < 0)
			{
				ALGO_Log_0("ERROR bad STRIDE\n");
			}
			else
			{
				aInputImage += nb_extra_due_to_stride * 2; // * 2 because 2 lignes are processed simultaneously
			}
		}
		break;
	case 1:
		// RGRGRG...
		// GBGBGB...
		for (current_pixel_y = 0; current_pixel_y < iImageHeightUsed / 2; current_pixel_y++) 
		{
			for (current_pixel_x = 0; current_pixel_x < aImageParams->iImageWidth / 2; current_pixel_x++) 
			{
				GChannel[0] = aInputImage[1];
				RChannel[0] = aInputImage[0];
				BChannel[0] = aInputImage[1 + aImageParams->iImageStride];
				GChannel[1] = GChannel[0];
				RChannel[1] = RChannel[0];
				BChannel[1] = BChannel[0];

				GChannel[0 + aImageParams->iImageWidth] = aInputImage[0 + aImageParams->iImageStride];
				RChannel[0 + aImageParams->iImageWidth] = RChannel[0];
				BChannel[0 + aImageParams->iImageWidth] = BChannel[0];
				GChannel[1 + aImageParams->iImageWidth] = GChannel[0 + aImageParams->iImageWidth];
				RChannel[1 + aImageParams->iImageWidth] = RChannel[0];
				BChannel[1 + aImageParams->iImageWidth] = BChannel[0];

				aInputImage += 2;

				RChannel += 2;
				GChannel += 2;
				BChannel += 2;
			}

			RChannel += aImageParams->iImageWidth;
			GChannel += aImageParams->iImageWidth;
			BChannel += aImageParams->iImageWidth;
			aInputImage += aImageParams->iImageWidth * 1;
			if(	nb_extra_due_to_stride < 0)
			{
				ALGO_Log_0("ERROR bad STRIDE\n");
			}
			else
			{
				aInputImage += nb_extra_due_to_stride * 2; // * 2 because 2 lignes are processed simultaneously
			}
		}
		break;
	case 3:
		// BGBGBG...
		// GRGRGR...
		for (current_pixel_y = 0; current_pixel_y < iImageHeightUsed / 2; current_pixel_y++) 
		{
			for (current_pixel_x = 0; current_pixel_x < aImageParams->iImageWidth / 2; current_pixel_x++) 
			{
				GChannel[0] = aInputImage[1];
				BChannel[0] = aInputImage[0];
				RChannel[0] = aInputImage[1 + aImageParams->iImageStride];
				GChannel[1] = GChannel[0];
				BChannel[1] = BChannel[0];
				RChannel[1] = RChannel[0];

				GChannel[0 + aImageParams->iImageWidth] = aInputImage[0 + aImageParams->iImageStride];
				BChannel[0 + aImageParams->iImageWidth] = BChannel[0];
				RChannel[0 + aImageParams->iImageWidth] = RChannel[0];
				GChannel[1 + aImageParams->iImageWidth] = GChannel[0 + aImageParams->iImageWidth];
				BChannel[1 + aImageParams->iImageWidth] = BChannel[0];
				RChannel[1 + aImageParams->iImageWidth] = RChannel[0];

				aInputImage += 2;

				RChannel += 2;
				GChannel += 2;
				BChannel += 2;
			}

			RChannel += aImageParams->iImageWidth;
			GChannel += aImageParams->iImageWidth;
			BChannel += aImageParams->iImageWidth;
			aInputImage += aImageParams->iImageWidth * 1;
			if(	nb_extra_due_to_stride < 0)
			{
				ALGO_Log_0("ERROR bad STRIDE\n");
			}
			else
			{
				aInputImage += nb_extra_due_to_stride * 2; // * 2 because 2 lignes are processed simultaneously
			}
		}
		break;
	default:
		ALGO_Log_1(" Error bad PixelOrder = %d\n", aImageParams->iImagePixelOrder);
		return EErrorNotValidated;
	}

    return EErrorNone;
}

/**
 * Routine to convert a RAW12 Channel into seperate R, G and B Channels.
 *
 * @param aInputImage           [TUint8*] The input RAW12 image channel.
 * @param aOutputImage          [TAlgoImageDataRGB*] The seperate R,G and B channels.
 * @param aImageParams          [TAlgoImageParams*] The image parameters.
 *
 * @return                      [TAlgoError] EErrorNone if the conversion is successful.
 */
TAlgoError RAW12toRGBPlanes(TUint8* aInputImage, 
                    TAlgoImageDataRGB* aOutputImage, 
                    TAlgoImageParams* aImageParams,
					TUint32* aNbStatusLine)
{
	TUint32 current_pixel_x = 0;
	TUint32 current_pixel_y = 0;
	TUint8* RChannel = aOutputImage->iRChannel;
	TUint8* GChannel = aOutputImage->iGChannel;
	TUint8* BChannel = aOutputImage->iBChannel;
	TUint32 nb_status_line = aImageParams->iImageNbStatusLine;
	TInt32 nb_extra_due_to_stride = 0;
	TUint32 iImageHeightUsed = 0;

	ALGO_Log_1("\n Color conversion %s \n", __FUNCTION__);

	if(	aImageParams->iImageWidth*3/2 != aImageParams->iImageStride)
	{
		nb_extra_due_to_stride = aImageParams->iImageStride - (aImageParams->iImageWidth*3/2);
	}

	aInputImage = aInputImage + aImageParams->iImageHeaderLength;
	
	ALGO_Log_1(" nb_status_line = %d\n", nb_status_line);
	ALGO_Log_1(" iImagePixelOrder = %d\n", aImageParams->iImagePixelOrder);
	//ALGO_Log_1("nb_extra_due_to_stride = %d\n", nb_extra_due_to_stride);

	if(	aImageParams->iImageWidth * 3/2 != aImageParams->iImageStride)
	{
		ALGO_Log_2("INFO : %d = width*3/2 != stride = %d\n", aImageParams->iImageWidth*3/2, aImageParams->iImageStride);
	}

	aInputImage += nb_status_line * aImageParams->iImageStride;
	iImageHeightUsed = aImageParams->iImageHeight - nb_status_line;
	*aNbStatusLine = nb_status_line;
	ALGO_Log_4("working with width = %d, height = %d, stride = %d => equivalente width %d\n", aImageParams->iImageWidth, iImageHeightUsed, aImageParams->iImageStride, aImageParams->iImageStride*2/3);


	switch(aImageParams->iImagePixelOrder)
	{
	case 0:
		// GRxGRxGRx...
		// BGxBGxBGx...
		for (current_pixel_y = 0; current_pixel_y < aImageParams->iImageHeight / 2; current_pixel_y++) 
		{
			for (current_pixel_x = 0; current_pixel_x < aImageParams->iImageWidth / 2; current_pixel_x++) 
			{
				GChannel[0] = aInputImage[0];
				RChannel[0] = aInputImage[1];
				BChannel[0] = aInputImage[0 + aImageParams->iImageStride];
				GChannel[1] = GChannel[0];
				RChannel[1] = RChannel[0];
				BChannel[1] = BChannel[0];

				GChannel[0 + aImageParams->iImageWidth] = aInputImage[1 + aImageParams->iImageStride];
				RChannel[0 + aImageParams->iImageWidth] = RChannel[0];
				BChannel[0 + aImageParams->iImageWidth] = BChannel[0];
				GChannel[1 + aImageParams->iImageWidth] = GChannel[0 + aImageParams->iImageWidth];
				RChannel[1 + aImageParams->iImageWidth] = RChannel[0];
				BChannel[1 + aImageParams->iImageWidth] = BChannel[0];

				aInputImage += 3;

				RChannel += 2;
				GChannel += 2;
				BChannel += 2;
			}

			RChannel += aImageParams->iImageWidth;
			GChannel += aImageParams->iImageWidth;
			BChannel += aImageParams->iImageWidth;
			aInputImage += aImageParams->iImageWidth * 3/2;
			if(	nb_extra_due_to_stride < 0)
			{
				ALGO_Log_0("ERROR bad STRIDE\n");
			}
			else
			{
				aInputImage += nb_extra_due_to_stride * 2; // * 2 because 2 lignes are processed simultaneously
			}
		}
		break;
	case 2:
		// GBxGBxGBx...
		// RGxRGxRGx...
		for (current_pixel_y = 0; current_pixel_y < aImageParams->iImageHeight / 2; current_pixel_y++) 
		{
			for (current_pixel_x = 0; current_pixel_x < aImageParams->iImageWidth / 2; current_pixel_x++) 
			{
				GChannel[0] = aInputImage[0];
				BChannel[0] = aInputImage[1];
				RChannel[0] = aInputImage[0 + aImageParams->iImageStride];
				GChannel[1] = GChannel[0];
				BChannel[1] = BChannel[0];
				RChannel[1] = RChannel[0];

				GChannel[0 + aImageParams->iImageWidth] = aInputImage[1 + aImageParams->iImageStride];
				BChannel[0 + aImageParams->iImageWidth] = BChannel[0];
				RChannel[0 + aImageParams->iImageWidth] = RChannel[0];
				GChannel[1 + aImageParams->iImageWidth] = GChannel[0 + aImageParams->iImageWidth];
				BChannel[1 + aImageParams->iImageWidth] = BChannel[0];
				RChannel[1 + aImageParams->iImageWidth] = RChannel[0];

				aInputImage += 3;

				RChannel += 2;
				GChannel += 2;
				BChannel += 2;
			}

			RChannel += aImageParams->iImageWidth;
			GChannel += aImageParams->iImageWidth;
			BChannel += aImageParams->iImageWidth;
			aInputImage += aImageParams->iImageWidth * 3/2;
			if(	nb_extra_due_to_stride < 0)
			{
				ALGO_Log_0("ERROR bad STRIDE\n");
			}
			else
			{
				aInputImage += nb_extra_due_to_stride * 2; // * 2 because 2 lignes are processed simultaneously
			}
		}
		break;
	case 1:
		// RGxRGxRGx...
		// GBxGBxGBx...
		for (current_pixel_y = 0; current_pixel_y < aImageParams->iImageHeight / 2; current_pixel_y++) 
		{
			for (current_pixel_x = 0; current_pixel_x < aImageParams->iImageWidth / 2; current_pixel_x++) 
			{
				GChannel[0] = aInputImage[1];
				RChannel[0] = aInputImage[0];
				BChannel[0] = aInputImage[1 + aImageParams->iImageStride];
				GChannel[1] = GChannel[0];
				RChannel[1] = RChannel[0];
				BChannel[1] = BChannel[0];

				GChannel[0 + aImageParams->iImageWidth] = aInputImage[0 + aImageParams->iImageStride];
				RChannel[0 + aImageParams->iImageWidth] = RChannel[0];
				BChannel[0 + aImageParams->iImageWidth] = BChannel[0];
				GChannel[1 + aImageParams->iImageWidth] = GChannel[0 + aImageParams->iImageWidth];
				RChannel[1 + aImageParams->iImageWidth] = RChannel[0];
				BChannel[1 + aImageParams->iImageWidth] = BChannel[0];

				aInputImage += 3;

				RChannel += 2;
				GChannel += 2;
				BChannel += 2;
			}

			RChannel += aImageParams->iImageWidth;
			GChannel += aImageParams->iImageWidth;
			BChannel += aImageParams->iImageWidth;
			aInputImage += aImageParams->iImageWidth * 3/2;
			if(	nb_extra_due_to_stride < 0)
			{
				ALGO_Log_0("ERROR bad STRIDE\n");
			}
			else
			{
				aInputImage += nb_extra_due_to_stride * 2; // * 2 because 2 lignes are processed simultaneously
			}
		}
		break;
	case 3:
		// BGxBGxBGx...
		// GRxGRxGRx...
		for (current_pixel_y = 0; current_pixel_y < aImageParams->iImageHeight / 2; current_pixel_y++) 
		{
			for (current_pixel_x = 0; current_pixel_x < aImageParams->iImageWidth / 2; current_pixel_x++) 
			{
				GChannel[0] = aInputImage[1];
				BChannel[0] = aInputImage[0];
				RChannel[0] = aInputImage[1 + aImageParams->iImageStride];
				GChannel[1] = GChannel[0];
				BChannel[1] = BChannel[0];
				RChannel[1] = RChannel[0];

				GChannel[0 + aImageParams->iImageWidth] = aInputImage[0 + aImageParams->iImageStride];
				BChannel[0 + aImageParams->iImageWidth] = BChannel[0];
				RChannel[0 + aImageParams->iImageWidth] = RChannel[0];
				GChannel[1 + aImageParams->iImageWidth] = GChannel[0 + aImageParams->iImageWidth];
				BChannel[1 + aImageParams->iImageWidth] = BChannel[0];
				RChannel[1 + aImageParams->iImageWidth] = RChannel[0];

				aInputImage += 3;

				RChannel += 2;
				GChannel += 2;
				BChannel += 2;
			}

			RChannel += aImageParams->iImageWidth;
			GChannel += aImageParams->iImageWidth;
			BChannel += aImageParams->iImageWidth;
			aInputImage += aImageParams->iImageWidth * 3/2;
			if(	nb_extra_due_to_stride < 0)
			{
				ALGO_Log_0("ERROR bad STRIDE\n");
			}
			else
			{
				aInputImage += nb_extra_due_to_stride * 2; // * 2 because 2 lignes are processed simultaneously
			}
		}
		break;
	default:
		ALGO_Log_1(" Error bad PixelOrder = %d\n", aImageParams->iImagePixelOrder);
		return EErrorNotValidated;
	}

    return EErrorNone;
}


