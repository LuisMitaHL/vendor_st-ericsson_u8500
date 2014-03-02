/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * Defines
 */


#ifndef __INCLUDE_ALGO_EXTRACTION_ROUTINES_H__
#define __INCLUDE_ALGO_EXTRACTION_ROUTINES_H__

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
 * Routine to convert a RGB565 Channel into seperate R, G and B Channels.
 *
 * @param aInputImage           [TUint8*] The input RGB565 image channel.
 * @param aOutputImage          [TAlgoImageDataRGB*] The seperate R,G and B channels.
 * @param aImageParams          [TAlgoImageParams*] The image parameters.
 *
 * @return                      [TAlgoError] EErrorNone if the conversion is successful.
 */
TAlgoError RGB565toRGBPlanes(TUint8* aInputImage, 
                    TAlgoImageDataRGB* aOutputImage, 
                    TAlgoImageParams* aImageParams);

/**
 * Routine to convert a BGR565 Channel into seperate R, G and B Channels.
 *
 * @param aInputImage           [TUint8*] The input BGR565 image channel.
 * @param aOutputImage          [TAlgoImageDataRGB*] The seperate R,G and B channels.
 * @param aImageParams          [TAlgoImageParams*] The image parameters.
 *
 * @return                      [TAlgoError] EErrorNone if the conversion is successful.
 */
TAlgoError BGR565toRGBPlanes(TUint8* aInputImage, 
                    TAlgoImageDataRGB* aOutputImage, 
                    TAlgoImageParams* aImageParams);

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
                    TAlgoImageParams* aImageParams);

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
                    TAlgoImageParams* aImageParams);

TAlgoError BMP_BGR24toRGBPlanes(TUint8* aInputImage, 
                    TAlgoImageDataRGB* aOutputImage, 
                    TAlgoImageParams* aImageParams);

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
                    TAlgoImageParams* aImageParams);

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
                    TAlgoImageParams* aImageParams);



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
                    TAlgoImageParams* aImageParams);

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
                             TAlgoImageParams* aImageParams);


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
                                   TAlgoImageParams* aImageParams);

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
                                   TAlgoImageParams* aImageParams);

/**
 * Routine to convert a YUV420MBSemiPlanarFiltered buffer to YUV(420) Planar buffer.
 *
 * @param aInputImage           [TUint8*] The input YUV(420) MB SemiPlanar image channel.
 * @param aOutputImage          [TUint8*] The output YUV(420) Planar image channel.
 * @param aImageParams          [TAlgoImageParams*] The image parameters.
 *
 * @return                      [TAlgoError] EErrorNone if the conversion is successful.
 */
TAlgoError YUV420MBSemiPlanarFilteredtoYUV420Planar(TUint8* aInputImage, 
                                                    TUint8* aOutputImage, 
                                                    TAlgoImageParams* aImageParams);

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
					TUint32* aNbStatusLine);

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
					TUint32* aNbStatusLine);



#ifdef __cplusplus
}
#endif 

#endif	//__INCLUDE_ALGO_EXTRACTION_ROUTINES_H__


