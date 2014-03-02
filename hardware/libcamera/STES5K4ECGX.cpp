/* 
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 *
 * Code adapted for usage of OMX components. All functions
 * using OMX functionality are under copyright of ST-Ericsson
 *
 *  This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 *  This code is provided as example. Up to customer to
 * modify it according to camera interlace algorithm
*/


//System includes
#include <semaphore.h>

//Multimedia includes
#undef CAM_LOG_TAG
#define CAM_LOG_TAG DBGT_TAG
#define DBGT_LAYER 2
#define DBGT_PREFIX "Deintl"

//Internal includes
#include "STECamTrace.h"
#include "STECamUtils.h"
#include "STECamOmxUtils.h"
#include "STES5K4ECGX.h"
#include "STEExtIspCamera.h"
#include "STECamMemoryHeapBase.h"
#include <linux/hwmem.h>
#include "STEArmIvProc.h"

namespace android {

bool Deinterlace::deinterlacingYuvThumbnailFromJpeg(
        char* pJpegBuf,
        int*  pJpegBufSize,
        char* pYuv422IThumbBuf,
        int   Yuv422IThumbBufSize,
        int   embeddedThumbWidth,
        int   embeddedThumbHeight)
{
    DBGT_PROLOG("");

    /* WARNING!!!!
     * -----------
     * De-interlacing function must be in line with the camera
     * thumbnail configuration.
     * Implementation of the deinterlacing algorithm is tighly link to
     * the camera used. So do not forget to update this algorithm for a
     * new camera implementation. */
    int  ret = 0;
    bool isThumbinterlaced = false;

    /* 1/ Define what is the thumbnail size.
     * WARNING!!!!!
     * The yuv thumbnail size to de-interlaced need to be equal to the preview
     * size.
     * So camera driver that define the thumbnail size should be in line with
     * the preview size defined. */
    int yuvThumbWidth  = embeddedThumbWidth;
    int yuvThumbHeight = embeddedThumbHeight;

    /* 2/ Allocate temporary memory for the YUV thumbnail according to the
     * thumbnail size and the yuv pixel format. */
    int yuvThumbBufSize = yuvThumbWidth * yuvThumbHeight * 2; /* YUV422 */

    /* 3/ Parse pJpegBuf, extract the yuv thumbnail (if present) and
     * store it in the temporary buffer previously allocated and update
     * pJpegBufSize */
    ret = DecodeInterleaveData(
            (uint8_t*)pJpegBuf,              // (IN/OUT) Pointer on Interleave Data is same for receiving JPEG data
            pJpegBufSize,		             // (IN/OUT) Pointer on JPEG data size
            (uint8_t*)pYuv422IThumbBuf,      // (OUT)    Pointer on the buffer for receiving YUV data
            yuvThumbBufSize,                 // (IN)     Size of the buffer receiving YUV data
            yuvThumbWidth,                   // (IN)     YUV thumbnail width
            yuvThumbHeight);                 // (IN)     YUV thumbnail height

    /* 4/ If there is no yuv interlaced present in the pJpegBuf, just
     * return the updated pJpegBufSize to have the exact Jpeg size. */
    if(ret == 1)
    {
        DBGT_PTRACE("Jpeg does not contains YUV stream => Nothing to deinterlace.");
        isThumbinterlaced = false;
    }

    /* 4bis/ The YUV thumbnail has been de-interlaced.
     * The yuv thumbnail format need to be converted from YUV422Itld
     * into YUV420MB format. */
    else if(ret == 0)
    {
        DBGT_PTRACE("Jpeg and YUV data successfully deinterlaced.");
        isThumbinterlaced = true;
    }

    /* 4ter/ Error while de-interlacing the thumbnail. */
    else
    {
        DBGT_CRITICAL("Error in DecodeInterleaveData function.");

	//WARNING, return is done outside else in order to
	//free temporary thumbnail memory
	//        return isThumbinterlaced;
    }

    DBGT_EPILOG("");

    return isThumbinterlaced;
}

//
// Decode Interleave Data and output Jpeg Data and YUV data seperately.
//
int Deinterlace::DecodeInterleaveData(
        uint8_t *pInterleaveData,  // (IN/OUT) Pointer on Interleave Data is same for receiving JPEG data
        int *pJpegSize,            // (IN/OUT) Pointer on JPEG data size
        uint8_t *pYuvData,	       // (OUT)    Pointer on the buffer for receiving YUV data
        int yuvThumbBufSize,       // (IN)     Size of the buffer receiving YUV data
        int yuvThumbWidth,         // (IN)     YUV thumbnail width
        int yuvThumbHeight)        // (IN)     YUV thumbnail height
{
    /* This function should perform the following action:
     * 1/ deinterlace the YUV stream and the Jpeg stream if YUV stream is
     * present.
     * 2/ give back the right Jpeg size.
     *
     * return -1 if error occurs.
     * return  0 if Jpeg and YUV have been well deinterlaced.
     * return  1 if the Jpeg does not contains YUV data. */

    DBGT_PROLOG("");

    /* Following algorithm implementation de-interlace YUV thumbnail from
     * Jpeg data for sensor samsung_s5k4ecgx.
     * In spoof mode, the YUV stream interlaced has always the same size
     * wich is the yuv thumbnail width * 2.
     * Further, the Start Of Embedded Image marker (SOEI 0xFFBE) is always
     * aligned on u16 so parsing will be done by reading every 2 Bytes.
     * No need to search for the End Of Embedded Image marker (EOEI
     * 0xFFBF) because the YUV enterlaced stream has always the same size.
     * Once all YUV data have been recover we finaly search for End Of
     * Image marker 0xFFD9. THe exact Jpeg size will be tehn updated after
     * all the buffer has been parsed. */
    int      ret = 1;
    int      interleaveDataSize;
    uint8_t  *interleave_ptr;
    uint8_t  *jpegData_ptr;
    uint8_t  *yuvData_ptr;
    uint8_t  *ffbf_ptr;
    uint8_t  *ffbe_ptr;
    uint8_t  *ffd9_ptr;
    int      jpeg_size;
    int      jpeg_chunck;
    int      yuv_size;
    int      yuv_chunck;
    int      yuv_chunck_nb;
    int      remainingSize;
    int      i = 0;

    yuvData_ptr        = pYuvData;
    jpegData_ptr       = NULL;
    ffbf_ptr           = NULL;
    ffbe_ptr           = NULL;
    ffd9_ptr           = NULL;
    jpeg_size          = 0; /* in Bytes */
    jpeg_chunck        = 0; /* in Bytes */
    yuv_size           = 0; /* in Bytes */
    yuv_chunck         = yuvThumbWidth * 2; /* in Bytes */
    yuv_chunck_nb      = 0;
    remainingSize      = *pJpegSize;
    interleaveDataSize = *pJpegSize;

    interleave_ptr    = (uint8_t*)pInterleaveData;

    if(pInterleaveData == NULL)
    {
        DBGT_PTRACE("Jpeg buffer pointer is NULL!");
        return -1;
    }

    if((*((uint16_t*)interleave_ptr)) != 0xD8FF)
    {
        DBGT_PTRACE("Jpeg header marker (0xFFD8) not found!");
        return -1;
    }

    DBGT_PDEBUG("0xFFD8 marker found @0x%x",
            (unsigned int)interleave_ptr);

    /* 1/
     * Find the EOS (0xFFD9 marker) position by dichotomy. And check if the
     * Jpeg has YUV embedded data. */
    remainingSize   = remainingSize >> 1; /* divide by 2 the remanining size */
    interleave_ptr += remainingSize;
    while(remainingSize > 64)
    {
        /* Perform dichotomy until the remaining size is less than 64 Bytes */
        DBGT_PDEBUG("Dichotomy interleave_ptr = 0x%x, remainingSize = %d, value = 0x%x",
                (unsigned int)interleave_ptr - (unsigned int)pInterleaveData,
                remainingSize,
                (*((uint32_t*)interleave_ptr)));

        if((*((uint32_t*)interleave_ptr)) != 0x00000000)
        {
            /* Search forward */
            remainingSize   = remainingSize >> 1; /* divide by 2 the remanining size */
            interleave_ptr += remainingSize;
        }
        else
        {
            /* Search backward */
            remainingSize   = remainingSize >> 1; /* divide by 2 the remanining size */
            interleave_ptr -= remainingSize;
        }

        if(remainingSize <= 64)
        {
            /* Last step */

            if((*((uint32_t*)interleave_ptr)) != 0x00000000)
            {
                /* If *interleaved_ptr is not null we add remainingSize to the
                 * pointer in order to always search backward for 0xFFD9
                 * marker. */
                interleave_ptr += remainingSize;
            }

            DBGT_PDEBUG("Dichotomy interleave_ptr = 0x%x, remainingSize = %d, value = 0x%x (last step)",
                    (unsigned int)interleave_ptr - (unsigned int)pInterleaveData,
                    remainingSize,
                    (*((uint32_t*)interleave_ptr)));
        }

    }

    /* Always search backward the 0xFFD9 marker. */
    for(i = (remainingSize * 2) ; i > 0 ; i--)
    {
        DBGT_PDEBUG("Search for 0xFFD9 = 0x%x, i = %d, value = 0x%x",
                (unsigned int)interleave_ptr - (unsigned int)pInterleaveData,
                i,
                *((uint16_t*)(interleave_ptr)));

        if((*((uint16_t*)(interleave_ptr))) == 0xD9FF)
        {
            DBGT_PDEBUG("0xFFD9 marker found @0x%x",
                    (unsigned int)interleave_ptr);
            jpeg_size = (interleave_ptr - (uint8_t*)pInterleaveData) + 2;
            ffd9_ptr  = interleave_ptr;
            break;
        }

        if(i == 1)
        {
            DBGT_CRITICAL("0xFFD9 marker not found!!");
            return -1;
        }
        /* Increment/Decrement pointer */
        interleave_ptr --;
    }

    /* 2/
     * Check if the Jpeg has YUV embedded data. */

    i = 0;
    for(i = 0 ; i < 10 ; i++)
    {
        /* If the JPEG8 marker 0xFFBC marker is found in the 10 bytes following
         * the 0xFFD9 marker, it implies that YUV data are embedded into the
         * Jpeg file. */
        if((*((uint16_t*)(interleave_ptr))) == 0xBCFF)
        {
            DBGT_PDEBUG("0xBCD9 marker found => Jpeg contains embedded YUV data");
            ret = 0; /* ret = 0 means YUV data embedded into Jpeg file */

            /* Re-adjust the final Jpeg size */
            jpeg_size = jpeg_size - ((yuv_chunck + 4) * yuvThumbHeight);

            break;
        }

        /* Increment pointer */
        interleave_ptr ++;
    }


    /* 3/
     * If YUV data are embedded in the Jpeg de-interlaced them. */

    if(ret == 0)
    {
        /* Reinitialize pointer and index */
        interleave_ptr = (uint8_t*)pInterleaveData;
        i              = 0;

        while((yuv_size < yuvThumbBufSize) && (i < interleaveDataSize))
        {
            /* Now we know that yuv embedded data are present in
             * the jpeg file. Parsing could be speed up.
             * Search only for OxFFBE (start of YUV stream) until
             * the yuv buffer is filled. */

            if((*((uint16_t*)interleave_ptr)) == 0xBEFF)
            {
                /* Mark the ffbe_ptr position */
                ffbe_ptr = pInterleaveData + i;
                DBGT_PDEBUG("0xFFBE marker position is ffbe_ptr = 0x%x.",
                        (unsigned int)ffbe_ptr);
            }

            /* Increment pointer and index */
            interleave_ptr += 2;
            i += 2;

            if(ffbe_ptr !=NULL)
            {
                ret = 0; /* ret = 0 means YUV embedded data are present */

                /* Each time the 0xFFBE marker is found, we
                 * copy the following yuv_chunck into the yuv
                 * output buffer. */
                ffbe_ptr += 2;
                DBGT_PDEBUG("Copy yuv data in yuv buffer   (yuvData_ptr = 0x%x, ffbe_ptr = 0x%x, yuv_chunck = %d) => chunk#%d",
                        (unsigned int)yuvData_ptr,
                        (unsigned int)ffbe_ptr,
                        yuv_chunck,
                        yuv_chunck_nb);

                /* Increment the chunck number */
                yuv_chunck_nb ++;

                if((yuvData_ptr + yuv_chunck) > (pYuvData + yuvThumbBufSize))
                {
                    DBGT_CRITICAL("Buffer to store the YUV thumbnail is too small!");
                }
                else
                {
                    memcpy(yuvData_ptr, ffbe_ptr, yuv_chunck);
                }

                if(yuv_size != 0)
                {
                    if(ffbe_ptr - ffbf_ptr == 4)
                    {
                        DBGT_PDEBUG("0xFFBF marker is immediatly followed by 0xFFBE marker => No Jpeg data to copy!");
                    }
                    else
                    {
                        /* We copy the jpeg_chunck into the output
                         * jpeg buffer. */
                        ffbf_ptr += 2;
                        jpeg_chunck = ffbe_ptr - ffbf_ptr - 2;

                        DBGT_PDEBUG("Copy Jpeg data in Jpeg buffer (jpegData_ptr = 0x%x, ffbf_ptr = 0x%x, jpeg_chunck = %d)",
                                (unsigned int)jpegData_ptr,
                                (unsigned int)ffbf_ptr,
                                jpeg_chunck);

                        if((jpegData_ptr + jpeg_chunck) > (pInterleaveData + interleaveDataSize))
                        {
                            DBGT_CRITICAL("Buffer to store the Jpeg is too small!");
                        }
                        else
                        {
                            memcpy(jpegData_ptr, ffbf_ptr, jpeg_chunck);
                        }

                        /* Increment yuvData pointer */
                        jpegData_ptr += jpeg_chunck;

                        /* Reinitialize jpeg_chunck */
                        jpeg_chunck = 0;
                    }
                }
                else
                {
                    /* Specific case:
                     * The first 0xFFBE marker has been found.*/

                    /* Mark the jpegData_ptr position. */
                    jpegData_ptr = ffbe_ptr -2;
                }

                /* Increment yuvData pointer */
                yuvData_ptr += yuv_chunck;

                /* Increment pointer and index */
                interleave_ptr += yuv_chunck;
                i += yuv_chunck;

                /* Reinitialise ffbe_ptr */
                ffbe_ptr = NULL;

                /* Mark the ffbf_ptr position */
                ffbf_ptr = interleave_ptr;
                DBGT_PDEBUG("0xFFBF marker position is ffbf_ptr = 0x%x.",
                        (unsigned int)ffbf_ptr);

                /* Update the yuv_size */
                yuv_size += yuv_chunck;
            }
        }
    }

    if(ret == 0)
    {
        /* We copy the last jpeg_chunck into the output jpeg buffer. */
        ffbf_ptr += 2;
        jpeg_chunck = (ffd9_ptr - ffbf_ptr) + 2;

        DBGT_PDEBUG("Copy Jpeg data in Jpeg buffer (jpegData_ptr = 0x%x, ffbf_ptr = 0x%x, jpeg_chunck = %d)",
                (unsigned int)jpegData_ptr,
                (unsigned int)ffbf_ptr,
                jpeg_chunck);

        if((jpegData_ptr + jpeg_chunck) > (pInterleaveData + interleaveDataSize))
        {
            DBGT_CRITICAL("Buffer to store the Jpeg is too small!");
        }
        else
        {
            memcpy(jpegData_ptr, ffbf_ptr, jpeg_chunck);
        }
    }

    /* Return the value of the real Jpeg size */
    if(jpeg_size > 0)
    {
        *pJpegSize = jpeg_size;
    }

    DBGT_EPILOG("");

    return ret;
}
}; // namespace android
