/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 *
 * Code adapted for usage of OMX components. All functions
 * using OMX functionality are under copyright of ST-Ericsson
 *
 *  This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#undef CAM_LOG_TAG
#define CAM_LOG_TAG DBGT_TAG
#define DBGT_LAYER 2
#define DBGT_PREFIX "ExfMng"

#include "STECamTrace.h"

//Internal includes
#include "STECamOmxUtils.h"
#include "STEExtIspCamera.h"
#include "STEExifMng.h"

#ifndef ENABLE_EXIF_MIXER
/****************************JHEAD******************************/
#include "STECameraConfig.h"

extern "C" {
#include <jhead.h>
}

#include <math.h>
#if defined(__SYMBIAN32__) || defined(ANDROID)
// In Symbian and Android, log2 not in libc
#define M_LOG2_E  0.693147180559945309417
#define log2f(x)  (logf(x) / (float)M_LOG2_E)
#endif

//--------------------------------------------------------------------------
// Describes tag values

#define TAG_DIR_COUNT              0xFFFF

#define TAG_INTEROP_INDEX          0x0001
#define TAG_INTEROP_VERSION        0x0002
#define TAG_IMAGE_WIDTH            0x0100
#define TAG_IMAGE_LENGTH           0x0101
#define TAG_BITS_PER_SAMPLE        0x0102
#define TAG_COMPRESSION            0x0103
#define TAG_PHOTOMETRIC_INTERP     0x0106
#define TAG_FILL_ORDER             0x010A
#define TAG_DOCUMENT_NAME          0x010D
#define TAG_IMAGE_DESCRIPTION      0x010E
#define TAG_MAKE                   0x010F
#define TAG_MODEL                  0x0110
#define TAG_SRIP_OFFSET            0x0111
#define TAG_ORIENTATION            0x0112
#define TAG_SAMPLES_PER_PIXEL      0x0115
#define TAG_ROWS_PER_STRIP         0x0116
#define TAG_STRIP_BYTE_COUNTS      0x0117
#define TAG_X_RESOLUTION           0x011A
#define TAG_Y_RESOLUTION           0x011B
#define TAG_PLANAR_CONFIGURATION   0x011C
#define TAG_RESOLUTION_UNIT        0x0128
#define TAG_TRANSFER_FUNCTION      0x012D
#define TAG_SOFTWARE               0x0131
#define TAG_DATETIME               0x0132
#define TAG_ARTIST                 0x013B
#define TAG_WHITE_POINT            0x013E
#define TAG_PRIMARY_CHROMATICITIES 0x013F
#define TAG_TRANSFER_RANGE         0x0156
#define TAG_JPEG_PROC              0x0200
#define TAG_THUMBNAIL_OFFSET       0x0201
#define TAG_THUMBNAIL_LENGTH       0x0202
#define TAG_Y_CB_CR_COEFFICIENTS   0x0211
#define TAG_Y_CB_CR_SUB_SAMPLING   0x0212
#define TAG_Y_CB_CR_POSITIONING    0x0213
#define TAG_REFERENCE_BLACK_WHITE  0x0214
#define TAG_RELATED_IMAGE_WIDTH    0x1001
#define TAG_RELATED_IMAGE_LENGTH   0x1002
#define TAG_CFA_REPEAT_PATTERN_DIM 0x828D
#define TAG_CFA_PATTERN1           0x828E
#define TAG_BATTERY_LEVEL          0x828F
#define TAG_COPYRIGHT              0x8298
#define TAG_EXPOSURETIME           0x829A
#define TAG_FNUMBER                0x829D
#define TAG_IPTC_NAA               0x83BB
#define TAG_EXIF_OFFSET            0x8769
#define TAG_INTER_COLOR_PROFILE    0x8773
#define TAG_EXPOSURE_PROGRAM       0x8822
#define TAG_SPECTRAL_SENSITIVITY   0x8824
#define TAG_GPSINFO                0x8825
#define TAG_ISO_EQUIVALENT         0x8827
#define TAG_OECF                   0x8828
#define TAG_EXIF_VERSION           0x9000
#define TAG_DATETIME_ORIGINAL      0x9003
#define TAG_DATETIME_DIGITIZED     0x9004
#define TAG_COMPONENTS_CONFIG      0x9101
#define TAG_CPRS_BITS_PER_PIXEL    0x9102
#define TAG_SHUTTERSPEED           0x9201
#define TAG_APERTURE               0x9202
#define TAG_BRIGHTNESS_VALUE       0x9203
#define TAG_EXPOSURE_BIAS          0x9204
#define TAG_MAXAPERTURE            0x9205
#define TAG_SUBJECT_DISTANCE       0x9206
#define TAG_METERING_MODE          0x9207
#define TAG_LIGHT_SOURCE           0x9208
#define TAG_FLASH                  0x9209
#define TAG_FOCALLENGTH            0x920A
#define TAG_MAKER_NOTE             0x927C
#define TAG_USERCOMMENT            0x9286
#define TAG_SUBSEC_TIME            0x9290
#define TAG_SUBSEC_TIME_ORIG       0x9291
#define TAG_SUBSEC_TIME_DIG        0x9292

#define TAG_WINXP_TITLE            0x9c9b // Windows XP - not part of exif standard.
#define TAG_WINXP_COMMENT          0x9c9c // Windows XP - not part of exif standard.
#define TAG_WINXP_AUTHOR           0x9c9d // Windows XP - not part of exif standard.
#define TAG_WINXP_KEYWORDS         0x9c9e // Windows XP - not part of exif standard.
#define TAG_WINXP_SUBJECT          0x9c9f // Windows XP - not part of exif standard.

#define TAG_FLASH_PIX_VERSION      0xA000
#define TAG_COLOR_SPACE            0xA001
#define TAG_EXIF_IMAGEWIDTH        0xA002
#define TAG_EXIF_IMAGELENGTH       0xA003
#define TAG_RELATED_AUDIO_FILE     0xA004
#define TAG_INTEROP_OFFSET         0xA005
#define TAG_FLASH_ENERGY           0xA20B
#define TAG_SPATIAL_FREQ_RESP      0xA20C
#define TAG_FOCAL_PLANE_XRES       0xA20E
#define TAG_FOCAL_PLANE_YRES       0xA20F
#define TAG_FOCAL_PLANE_UNITS      0xA210
#define TAG_SUBJECT_LOCATION       0xA214
#define TAG_EXPOSURE_INDEX         0xA215
#define TAG_SENSING_METHOD         0xA217
#define TAG_FILE_SOURCE            0xA300
#define TAG_SCENE_TYPE             0xA301
#define TAG_CFA_PATTERN            0xA302
#define TAG_CUSTOM_RENDERED        0xA401
#define TAG_EXPOSURE_MODE          0xA402
#define TAG_WHITEBALANCE           0xA403
#define TAG_DIGITALZOOMRATIO       0xA404
#define TAG_FOCALLENGTH_35MM       0xA405
#define TAG_SCENE_CAPTURE_TYPE     0xA406
#define TAG_GAIN_CONTROL           0xA407
#define TAG_CONTRAST               0xA408
#define TAG_SATURATION             0xA409
#define TAG_SHARPNESS              0xA40A
#define TAG_DISTANCE_RANGE         0xA40C

//--------------------------------------------------------------------------
// new tags for GPS exif field

#define TAG_GPS_VERS_ID            0
#define TAG_GPS_LAT_REF            1
#define TAG_GPS_LAT                2
#define TAG_GPS_LONG_REF           3
#define TAG_GPS_LONG               4
#define TAG_GPS_ALT_REF            5
#define TAG_GPS_ALT                6
#define TAG_GPS_TIMESTAMP          7
#define TAG_GPS_SATELLITES         8
#define TAG_GPS_RECEIVER_STATUS    9

#define TAG_GPS_PROCESSING_METHOD 0x1B
#define TAG_GPS_DATESTAMP         0x1D
#define TAG_GPS_MAPDATUM          0x12

namespace android {
#define STR_MAX 32
#undef _CNAME_
#define _CNAME_ SteExifMng

    SteExifMng::SteExifMng() {
        DBGT_PROLOG("");

        // reset exif jpeg library
        ResetJpgfile();

        DBGT_EPILOG("");
    };

    SteExifMng::~SteExifMng(){
        DBGT_PROLOG("");

        // discard data to avoid memory leak
        DiscardData();

        DBGT_EPILOG("");
    }

    int SteExifMng::UpdateExifField(
        const CameraParameters &params,
        char* inBuffer,
        int inBufSize,
        char* outBuffer,
        int outBufSize,
        char* inThumbBuffer,
        int inThumbBufSize,
        int cameraId){

        DBGT_PROLOG("");
        ExifElement_t exifElementTable[64];

        // send jpeg to exif library
        if( ReadJpegSectionsFromBuffer ((unsigned char*)inBuffer, inBufSize, READ_ALL)!= true){
            DBGT_CRITICAL("can't read jpeg buffer");
        }

        int exifTagCount = 0;
        int gpsTagCount = 0;
        int hasDateTimeTag = 0;
        int TagIndex = 0;

        /* *********************** Tags used by IFD0 (main image) *************************** */

        int width, height = 0;
        params.getPictureSize(&width, &height);
        char widthStr[STR_MAX],heightStr[STR_MAX];
        snprintf(widthStr, sizeof(widthStr), "%d", width);
        snprintf(heightStr, sizeof(heightStr), "%d", height);

        //TAG_IMAGE_WIDTH
        exifElementTable[TagIndex].Tag = TAG_IMAGE_WIDTH;
        exifElementTable[TagIndex].Format = FMT_USHORT;
        exifElementTable[TagIndex].Value = (char*)widthStr;
        exifElementTable[TagIndex].DataLength = 1;
        exifElementTable[TagIndex].GpsTag = FALSE;
        TagIndex++;
        exifTagCount++;

        //TAG_IMAGE_LENGTH
        exifElementTable[TagIndex].Tag = TAG_IMAGE_LENGTH;
        exifElementTable[TagIndex].Format = FMT_USHORT;
        exifElementTable[TagIndex].Value = (char*)heightStr;
        exifElementTable[TagIndex].DataLength = 1;
        exifElementTable[TagIndex].GpsTag = FALSE;
        TagIndex++;
        exifTagCount++;

        //TAG_MAKE
        char makeStr[STR_MAX] = EXIF_DEFAULT_MAKE;
        if (params.get("Make") != NULL) {
            strncpy(makeStr, params.get("Make"), sizeof(makeStr));
        }

        exifElementTable[TagIndex].Tag = TAG_MAKE;
        exifElementTable[TagIndex].Format = FMT_STRING;
        exifElementTable[TagIndex].Value = (char*)makeStr;
        exifElementTable[TagIndex].DataLength = 1;
        exifElementTable[TagIndex].GpsTag = FALSE;
        TagIndex++;
        exifTagCount++;

        //TAG_MODEL
        char modelStr[STR_MAX] = EXIF_DEFAULT_MODEL;
        if (params.get("models") != NULL) {
            strncpy(makeStr, params.get("models"), sizeof(modelStr));
        }
        exifElementTable[TagIndex].Tag = TAG_MODEL;
        exifElementTable[TagIndex].Format = FMT_STRING;
        exifElementTable[TagIndex].Value = (char*)modelStr;
        exifElementTable[TagIndex].DataLength = 1;
        exifElementTable[TagIndex].GpsTag = FALSE;
        TagIndex++;
        exifTagCount++;

#if 1
        //TAG_ORIENTATION
        char orientationStr[STR_MAX] = "1";
        if (params.get(CameraParameters::KEY_ROTATION) != NULL){
            int rotation = atoi(params.get(CameraParameters::KEY_ROTATION));

	DBGT_CRITICAL(" [dyron debug] capter picture rotation = %d", rotation);

            rotation = (rotation%360);

            if(rotation==270){
                // bottom left side  270°
                strncpy(orientationStr, "8", STR_MAX);
            }else{
                if(rotation==90){
                    // top right side 90°
                    strncpy(orientationStr, "6", STR_MAX);
                }else{
                    if(rotation==180){
                        // bottom right side 180°
                        strncpy(orientationStr, "3", STR_MAX);
                    }else{
                        // top left side 0°
                        strncpy(orientationStr, "1", STR_MAX);
                    }
                }
            }
        }

        exifElementTable[TagIndex].Tag = TAG_ORIENTATION;
        exifElementTable[TagIndex].Format = FMT_USHORT;
        exifElementTable[TagIndex].Value = (char*)orientationStr;
        exifElementTable[TagIndex].DataLength = 1;
        exifElementTable[TagIndex].GpsTag = FALSE;
        TagIndex++;
        exifTagCount++;
#endif
        //TAG_X_RESOLUTION
        char xresolStr[] = EXIF_DEFAULT_XYRESOL;
        exifElementTable[TagIndex].Tag = TAG_X_RESOLUTION;
        exifElementTable[TagIndex].Format = FMT_URATIONAL;
        exifElementTable[TagIndex].Value = (char*)xresolStr;
        exifElementTable[TagIndex].DataLength = 1;
        exifElementTable[TagIndex].GpsTag = FALSE;
        TagIndex++;
        exifTagCount++;

        //TAG_Y_RESOLUTION
        char yresolStr[] = EXIF_DEFAULT_XYRESOL;
        exifElementTable[TagIndex].Tag = TAG_Y_RESOLUTION;
        exifElementTable[TagIndex].Format = FMT_URATIONAL;
        exifElementTable[TagIndex].Value = (char*)yresolStr;
        exifElementTable[TagIndex].DataLength = 1;
        exifElementTable[TagIndex].GpsTag = FALSE;
        TagIndex++;
        exifTagCount++;

        //TAG_RESOLUTION_UNIT
        char resoUnitStr[] = EXIF_DEFAULT_RESO_UNIT; //default = inch
        exifElementTable[TagIndex].Tag = TAG_RESOLUTION_UNIT;
        exifElementTable[TagIndex].Format = FMT_USHORT;
        exifElementTable[TagIndex].Value = (char*)resoUnitStr;
        exifElementTable[TagIndex].DataLength = 1;
        exifElementTable[TagIndex].GpsTag = FALSE;
        TagIndex++;
        exifTagCount++;

        //TAG_SOFTWARE
        char softwareStr[STR_MAX] = EXIF_DEFAULT_SOFTWARE ;
        if (params.get("software") != NULL) {
            strncpy(softwareStr, params.get("software"),sizeof(softwareStr));
        }
        exifElementTable[TagIndex].Tag = TAG_SOFTWARE;
        exifElementTable[TagIndex].Format = FMT_STRING;
        exifElementTable[TagIndex].Value = (char*)softwareStr;
        exifElementTable[TagIndex].DataLength = 1;
        exifElementTable[TagIndex].GpsTag = FALSE;
        TagIndex++;
        exifTagCount++;

        //TAG_DATETIME
        time_t Tval = time(NULL);
        struct tm *OurT = localtime(&Tval);
        snprintf( ::ImageInfo.DateTime, STR_MAX, "%d:%02d:%02d %02d:%02d:%02d",
                OurT->tm_year+1900 ,
                OurT->tm_mon+1,
                OurT->tm_mday,
                OurT->tm_hour,
                OurT->tm_min,
                OurT->tm_sec);

        ::ImageInfo.numDateTimeTags = 1;

        //TAG_Y_CB_CR_POSITIONING
        char ycbcrPositioningStr[] = EXIF_DEFAULT_YCBCR_POSITIONING; //default = co-sited
        exifElementTable[TagIndex].Tag = TAG_Y_CB_CR_POSITIONING;
        exifElementTable[TagIndex].Format = FMT_USHORT;
        exifElementTable[TagIndex].Value = (char*)ycbcrPositioningStr;
        exifElementTable[TagIndex].DataLength = 1;
        exifElementTable[TagIndex].GpsTag = FALSE;
        TagIndex++;
        exifTagCount++;

        /* *********************** Tags used by Exif SubIFD *************************** */

        //TAG_EXPOSURETIME
        char exposureTimeStr[STR_MAX]="1/10";
        if(params.getPreviewFrameRate()!=0){
            snprintf( exposureTimeStr, STR_MAX, "1/%d", (int)params.getPreviewFrameRate() );
        }
        exifElementTable[TagIndex].Tag = TAG_EXPOSURETIME;
        exifElementTable[TagIndex].Format = FMT_URATIONAL;
        exifElementTable[TagIndex].Value = (char*)exposureTimeStr;
        exifElementTable[TagIndex].DataLength = 1;
        exifElementTable[TagIndex].GpsTag = FALSE;
        TagIndex++;
        exifTagCount++;

        //TAG_FNUMBER
        char ApertureFNumberStr[STR_MAX]="3/5";
        int Fnumber =100;
        if (params.get("fnumber") != NULL){
            Fnumber= 100 * atof(params.get("fnumber"));
        }else{
            Fnumber= 100 * EXIF_DEFAULT_APERTURE_FNUMBER;
        }
        snprintf( ApertureFNumberStr, STR_MAX, "%d/%d", Fnumber, 100 );
        exifElementTable[TagIndex].Tag = TAG_FNUMBER;
        exifElementTable[TagIndex].Format = FMT_URATIONAL;
        exifElementTable[TagIndex].Value = (char*)ApertureFNumberStr;
        exifElementTable[TagIndex].DataLength = 1;
        exifElementTable[TagIndex].GpsTag = FALSE;
        TagIndex++;
        exifTagCount++;

        //TAG_ISO_EQUIVALENT
        char ISOequivalentStr[STR_MAX] ="";
        if (params.get("sensitivity") != NULL){
            int ISOequivalent = atoi( params.get("sensitivity") );

            // check validity of iso speed rating
            if( (ISOequivalent<100)||(ISOequivalent>1600) ){
                snprintf( ISOequivalentStr, STR_MAX, "%d", EXIF_DEFAULT_ISO_EQUIVALENT );
            }else{
                snprintf( ISOequivalentStr, STR_MAX, "%d", ISOequivalent );
            }
        }else{
            snprintf( ISOequivalentStr, STR_MAX, "%d", EXIF_DEFAULT_ISO_EQUIVALENT );
        }
        exifElementTable[TagIndex].Tag = TAG_ISO_EQUIVALENT;
        exifElementTable[TagIndex].Format = FMT_USHORT;
        exifElementTable[TagIndex].Value = (char*)ISOequivalentStr;
        exifElementTable[TagIndex].DataLength = 1;
        exifElementTable[TagIndex].GpsTag = FALSE;
        TagIndex++;
        exifTagCount++;

        //TAG_EXIF_VERSION
        char exifVersionStr[]=EXIF_DEFAULT_EXIF_VERSION;
        exifElementTable[TagIndex].Tag = TAG_EXIF_VERSION;
        exifElementTable[TagIndex].Format = FMT_UNDEFINED;
        exifElementTable[TagIndex].Value = (char*)exifVersionStr;
        exifElementTable[TagIndex].DataLength = 4;
        exifElementTable[TagIndex].GpsTag = FALSE;
        TagIndex++;
        exifTagCount++;

        //TAG_DATETIME_DIGITIZED
        exifElementTable[TagIndex].Tag = TAG_DATETIME_DIGITIZED;
        exifElementTable[TagIndex].Format = FMT_STRING;
        exifElementTable[TagIndex].Value = (char*)::ImageInfo.DateTime;
        exifElementTable[TagIndex].DataLength = 20;
        exifElementTable[TagIndex].GpsTag = FALSE;
        TagIndex++;
        exifTagCount++;

        //TAG_DATETIME_ORIGINAL
        exifElementTable[TagIndex].Tag = TAG_DATETIME_ORIGINAL;
        exifElementTable[TagIndex].Format = FMT_STRING;
        exifElementTable[TagIndex].Value = (char*)::ImageInfo.DateTime;
        exifElementTable[TagIndex].DataLength = 20;
        exifElementTable[TagIndex].GpsTag = FALSE;
        TagIndex++;
        exifTagCount++;

        //TAG_COMPONENTS_CONFIG
        char components_configurationStr[]= EXIF_DEFAULT_COMPONENTS_CONFIG;
        exifElementTable[TagIndex].Tag = TAG_COMPONENTS_CONFIG;
        exifElementTable[TagIndex].Format = FMT_BYTE;
        exifElementTable[TagIndex].Value = (char*)components_configurationStr;
        exifElementTable[TagIndex].DataLength = 4;
        exifElementTable[TagIndex].GpsTag = FALSE;
        TagIndex++;
        exifTagCount++;

        //TAG_SHUTTERSPEED
        char shutter_speedStr [STR_MAX]="100/100";
        if(params.getPreviewFrameRate()!=0){
            float shutter_speed = 0;
            /*
             * EXIF ShutterSpeedValue values
             * The unit is the APEX unit (Additive System of Photographic Exposure).
             * ShutterSpeedValue = - log2 (exposure time)
             */
            float expotime = (float)(1/(float)params.getPreviewFrameRate());
            shutter_speed = (OMX_S32)-log2f(expotime); // keep the integer part only (see APEX tables)
            snprintf(shutter_speedStr, STR_MAX, "%d/%d", (int) (shutter_speed * 100), 100);
        }
        exifElementTable[TagIndex].Tag = TAG_SHUTTERSPEED;
        exifElementTable[TagIndex].Format = FMT_SRATIONAL;
        exifElementTable[TagIndex].Value = (char*)shutter_speedStr;
        exifElementTable[TagIndex].DataLength = 1;
        exifElementTable[TagIndex].GpsTag = FALSE;
        TagIndex++;
        exifTagCount++;

        //TAG_EXPOSURE_BIAS
        char exposure_biasStr [STR_MAX]="0/100";
        snprintf(exposure_biasStr, STR_MAX, "%d/%d", (int) (EXIF_DEFAULT_EXPOSURE_BIAS * 100), 100);
        exifElementTable[TagIndex].Tag = TAG_EXPOSURE_BIAS;
        exifElementTable[TagIndex].Format = FMT_SRATIONAL;
        exifElementTable[TagIndex].Value = (char*)exposure_biasStr;
        exifElementTable[TagIndex].DataLength = 1;
        exifElementTable[TagIndex].GpsTag = FALSE;
        TagIndex++;
        exifTagCount++;

        //TAG_METERING_MODE
        char MeteringModeStr[STR_MAX] = EXIF_DEFAULT_METERING_MODE;
        if (params.get("exposure") != NULL) {
            if (!strcmp(params.get("exposure"), "matrix"))
                snprintf(MeteringModeStr, STR_MAX, "5");
            if (!strcmp(params.get("exposure"), "center_weighted"))
                snprintf(MeteringModeStr, STR_MAX, "2");
            if (!strcmp(params.get("exposure"), "spot"))
                snprintf(MeteringModeStr, STR_MAX, "3");
            if (!strcmp(params.get("exposure"), "average"))
                snprintf(MeteringModeStr, STR_MAX, "1");
        }
        exifElementTable[TagIndex].Tag = TAG_METERING_MODE;
        exifElementTable[TagIndex].Format = FMT_USHORT;
        exifElementTable[TagIndex].Value = (char*)MeteringModeStr;
        exifElementTable[TagIndex].DataLength = 1;
        exifElementTable[TagIndex].GpsTag = FALSE;
        TagIndex++;
        exifTagCount++;

        //TAG_MAXAPERTURE
        char maxApertureValueStr [STR_MAX]="0/100";
        snprintf(maxApertureValueStr,STR_MAX,"%d/%d",(int)EXIF_DEFAULT_MAX_APERTURE_NUM,(int)EXIF_DEFAULT_MAX_APERTURE_DENOM);
        exifElementTable[TagIndex].Tag = TAG_MAXAPERTURE;
        exifElementTable[TagIndex].Format = FMT_URATIONAL;
        exifElementTable[TagIndex].Value = (char*)maxApertureValueStr;
        exifElementTable[TagIndex].DataLength = 1;
        exifElementTable[TagIndex].GpsTag = FALSE;
        TagIndex++;
        exifTagCount++;

        //TAG_LIGHT_SOURCE
        char LightSourceStr[STR_MAX] = EXIF_DEFAULT_LIGHT_SOURCE; //Default: Unknown light source
        if(NULL != params.get(CameraParameters::KEY_WHITE_BALANCE))
        {
            if (!strcmp(params.get(CameraParameters::KEY_WHITE_BALANCE), CameraParameters::WHITE_BALANCE_DAYLIGHT))
                snprintf(LightSourceStr,STR_MAX,"1");
            if (!strcmp(params.get(CameraParameters::KEY_WHITE_BALANCE), CameraParameters::WHITE_BALANCE_CLOUDY_DAYLIGHT))
                snprintf(LightSourceStr,STR_MAX,"10");
            if (!strcmp(params.get(CameraParameters::KEY_WHITE_BALANCE), CameraParameters::WHITE_BALANCE_SHADE))
                snprintf(LightSourceStr,STR_MAX,"11");
            if (!strcmp(params.get(CameraParameters::KEY_WHITE_BALANCE), CameraParameters::WHITE_BALANCE_FLUORESCENT))
                snprintf(LightSourceStr,STR_MAX,"14");
            if (!strcmp(params.get(CameraParameters::KEY_WHITE_BALANCE), CameraParameters::WHITE_BALANCE_INCANDESCENT))
                snprintf(LightSourceStr,STR_MAX,"3");
        }
        exifElementTable[TagIndex].Tag = TAG_LIGHT_SOURCE;
        exifElementTable[TagIndex].Format = FMT_USHORT;
        exifElementTable[TagIndex].Value = (char*)LightSourceStr;
        exifElementTable[TagIndex].DataLength = 1;
        exifElementTable[TagIndex].GpsTag = FALSE;
        TagIndex++;
        exifTagCount++;

        //TAG_FLASH
        char FlashUsedStr[STR_MAX] = EXIF_DEFAULT_FLASH_USED; //default = Flash did not fire
        exifElementTable[TagIndex].Tag = TAG_FLASH;
        exifElementTable[TagIndex].Format = FMT_USHORT;
        exifElementTable[TagIndex].Value = (char*)FlashUsedStr;
        exifElementTable[TagIndex].DataLength = 1;
        exifElementTable[TagIndex].GpsTag = FALSE;
        TagIndex++;
        exifTagCount++;

        //TAG_FOCALLENGTH
        char focal_lengthStr [STR_MAX]="0/100";
        // Get the focal length parameter as read from the driver and set by the HAL
        snprintf(focal_lengthStr, STR_MAX, "%f/%d", 100*params.getFloat(CameraParameters::KEY_FOCAL_LENGTH),100);
        exifElementTable[TagIndex].Tag = TAG_FOCALLENGTH;
        exifElementTable[TagIndex].Format = FMT_URATIONAL;
        exifElementTable[TagIndex].Value = (char*)focal_lengthStr;
        exifElementTable[TagIndex].DataLength = 1;
        exifElementTable[TagIndex].GpsTag = FALSE;
        TagIndex++;
        exifTagCount++;

        //TAG_USERCOMMENT
        char userCommentsStr[STR_MAX] = EXIF_DEFAULT_USER_COMMENTS;
        exifElementTable[TagIndex].Tag = TAG_USERCOMMENT;
        exifElementTable[TagIndex].Format = FMT_STRING;
        exifElementTable[TagIndex].Value = (char*)userCommentsStr;
        exifElementTable[TagIndex].DataLength = -1;
        exifElementTable[TagIndex].GpsTag = FALSE;
        TagIndex++;
        exifTagCount++;

        //TAG_FLASH_PIX_VERSION
        char flash_pix_versionStr[STR_MAX] = EXIF_DEFAULT_FLASH_PIX_VERSION;
        exifElementTable[TagIndex].Tag = TAG_FLASH_PIX_VERSION;
        exifElementTable[TagIndex].Format = FMT_UNDEFINED;
        exifElementTable[TagIndex].Value = (char*)flash_pix_versionStr;
        exifElementTable[TagIndex].DataLength = 4;
        exifElementTable[TagIndex].GpsTag = FALSE;
        TagIndex++;
        exifTagCount++;

        //TAG_COLOR_SPACE
        char ColorSpaceStr[STR_MAX] = EXIF_DEFAULT_COLOR_SPACE; //default = sRGB
        exifElementTable[TagIndex].Tag = TAG_COLOR_SPACE;
        exifElementTable[TagIndex].Format = FMT_USHORT;
        exifElementTable[TagIndex].Value = (char*)ColorSpaceStr;
        exifElementTable[TagIndex].DataLength = 1;
        exifElementTable[TagIndex].GpsTag = FALSE;
        TagIndex++;
        exifTagCount++;

        //TAG_EXIF_IMAGEWIDTH
        exifElementTable[TagIndex].Tag = TAG_EXIF_IMAGEWIDTH;
        exifElementTable[TagIndex].Format = FMT_ULONG;
        exifElementTable[TagIndex].Value = (char*)widthStr;
        exifElementTable[TagIndex].DataLength = 1;
        exifElementTable[TagIndex].GpsTag = FALSE;
        TagIndex++;
        exifTagCount++;

        //TAG_EXIF_IMAGELENGTH
        exifElementTable[TagIndex].Tag = TAG_EXIF_IMAGELENGTH;
        exifElementTable[TagIndex].Format = FMT_ULONG;
        exifElementTable[TagIndex].Value = (char*)heightStr;
        exifElementTable[TagIndex].DataLength = 1;
        exifElementTable[TagIndex].GpsTag = FALSE;
        TagIndex++;
        exifTagCount++;

        //TAG_CUSTOM_RENDERED
        char CustomRenderedStr[STR_MAX] = EXIF_DEFAULT_CUSTOM_RENDERED; //default = Normal process
        exifElementTable[TagIndex].Tag = TAG_CUSTOM_RENDERED;
        exifElementTable[TagIndex].Format = FMT_USHORT;
        exifElementTable[TagIndex].Value = (char*)CustomRenderedStr;
        exifElementTable[TagIndex].DataLength = 1;
        exifElementTable[TagIndex].GpsTag = FALSE;
        TagIndex++;
        exifTagCount++;

        //TAG_EXPOSURE_MODE
        char ExposureModeStr[STR_MAX] = EXIF_DEFAULT_EXPOSURE_MODE; //default = Auto Exposure
        exifElementTable[TagIndex].Tag = TAG_EXPOSURE_MODE;
        exifElementTable[TagIndex].Format = FMT_USHORT;
        exifElementTable[TagIndex].Value = (char*)ExposureModeStr;
        exifElementTable[TagIndex].DataLength = 1;
        exifElementTable[TagIndex].GpsTag = FALSE;
        TagIndex++;
        exifTagCount++;

        //TAG_WHITEBALANCE
        char WhitebalanceStr[STR_MAX] = "";
        if ((NULL != params.get(CameraParameters::KEY_WHITE_BALANCE)) &&
                ( strcmp(params.get(CameraParameters::KEY_WHITE_BALANCE),
                         CameraParameters::WHITE_BALANCE_AUTO ) == 0 )){
            snprintf(WhitebalanceStr,STR_MAX,"0");
        }else{
            snprintf(WhitebalanceStr,STR_MAX,"1"); //else is manual white balance
        }
        exifElementTable[TagIndex].Tag = TAG_WHITEBALANCE;
        exifElementTable[TagIndex].Format = FMT_USHORT;
        exifElementTable[TagIndex].Value = (char*)WhitebalanceStr;
        exifElementTable[TagIndex].DataLength = 1;
        exifElementTable[TagIndex].GpsTag = FALSE;
        TagIndex++;
        exifTagCount++;

        //TAG_DIGITALZOOMRATIO
        char digitalZoomRatioStr [STR_MAX]="";
        if (params.getInt("zoom") != 0) {
            snprintf(digitalZoomRatioStr, STR_MAX, "%d/%d",(int)((float)params.getInt("zoom")*10) , 1000 );
        }else{
            snprintf(digitalZoomRatioStr, STR_MAX, "%d/%d",(int)(EXIF_DEFAULT_ZOOM_RATIO*1000) , 1000 );
        }
        exifElementTable[TagIndex].Tag = TAG_DIGITALZOOMRATIO;
        exifElementTable[TagIndex].Format = FMT_URATIONAL;
        exifElementTable[TagIndex].Value = (char*)digitalZoomRatioStr;
        exifElementTable[TagIndex].DataLength = 1;
        exifElementTable[TagIndex].GpsTag = FALSE;
        TagIndex++;
        exifTagCount++;

        //TAG_SCENE_CAPTURE_TYPE
        char SceneTypeStr[STR_MAX] = EXIF_DEFAULT_SCENE_TYPE; //default = standard
        if (NULL != params.get(CameraParameters::KEY_SCENE_MODE)){
            if (strcmp(params.get(CameraParameters::KEY_SCENE_MODE),
                        CameraParameters::SCENE_MODE_LANDSCAPE ) == 0 ){
                snprintf(SceneTypeStr, STR_MAX, "1");
            }else if(strcmp(params.get(CameraParameters::KEY_SCENE_MODE),
                        CameraParameters::SCENE_MODE_PORTRAIT ) == 0 ){
                snprintf(SceneTypeStr, STR_MAX, "2");
            }else if(strcmp(params.get(CameraParameters::KEY_SCENE_MODE),
                        CameraParameters::SCENE_MODE_NIGHT ) == 0 ){
                snprintf(SceneTypeStr, STR_MAX, "3");
            }else{
                snprintf(SceneTypeStr, STR_MAX, "0");            }
        }
        exifElementTable[TagIndex].Tag = TAG_SCENE_CAPTURE_TYPE;
        exifElementTable[TagIndex].Format = FMT_USHORT;
        exifElementTable[TagIndex].Value = (char*)SceneTypeStr;
        exifElementTable[TagIndex].DataLength = 1;
        exifElementTable[TagIndex].GpsTag = FALSE;
        TagIndex++;
        exifTagCount++;

        //TAG_DISTANCE_RANGE
        char DistanceRangeStr[STR_MAX] = EXIF_DEFAULT_DISTANCE_RANGE; //default = unknown
        exifElementTable[TagIndex].Tag = TAG_DISTANCE_RANGE;
        exifElementTable[TagIndex].Format = FMT_USHORT;
        exifElementTable[TagIndex].Value = (char*)DistanceRangeStr;
        exifElementTable[TagIndex].DataLength = 1;
        exifElementTable[TagIndex].GpsTag = FALSE;
        TagIndex++;
        exifTagCount++;

        //TAG_EXPOSURE_PROGRAM
        char ExposureProgramStr[STR_MAX] = EXIF_DEFAULT_EXPOSURE_PROGRAM; //default = Aperture priority
        exifElementTable[TagIndex].Tag = TAG_EXPOSURE_PROGRAM;
        exifElementTable[TagIndex].Format = FMT_USHORT;
        exifElementTable[TagIndex].Value = (char*)ExposureProgramStr;
        exifElementTable[TagIndex].DataLength = 1;
        exifElementTable[TagIndex].GpsTag = FALSE;
        TagIndex++;
        exifTagCount++;

        /* *********************** Tags used by GPS IFD *************************** */

        if ((params.get(CameraParameters::KEY_GPS_LONGITUDE) != NULL)
                && (params.get(CameraParameters::KEY_GPS_LONGITUDE) != NULL)
                && (params.get(CameraParameters::KEY_GPS_ALTITUDE) != NULL)
                && (params.get(CameraParameters::KEY_GPS_TIMESTAMP) != NULL)) {
            DBGT_PTRACE("Add GPS tags");

            float longitude  = atof( params.get(CameraParameters::KEY_GPS_LONGITUDE) );
            float latitude  = atof( params.get(CameraParameters::KEY_GPS_LATITUDE) );
            int altitude  = atoi( params.get(CameraParameters::KEY_GPS_ALTITUDE) );
            int nbsec, degree, min, sec;
            float totalSec;
            time_t rawtime = atol(params.get(CameraParameters::KEY_GPS_TIMESTAMP));
            struct tm * ptm;
            char GpsLatRef[STR_MAX] = "";
            char GpsLat[STR_MAX] = "";
            char GpsLongRef[STR_MAX] = "";
            char GpsLong[STR_MAX] = "";
            char GpsAltRef[STR_MAX] = "";
            char GpsAlt[STR_MAX] = "";
            char GpsTimeStamp[STR_MAX] = "";
            char GpsDateStamp[STR_MAX] = "";
            char GpsStatus[STR_MAX] = "";
            char GpsMapDatum[STR_MAX] = "";
            char GpsProcessingMethod[STR_MAX] = "";

            if(longitude<0){
                degree = (int)(longitude * (-1));
                nbsec = (int)(longitude*(-3600)-(float)degree*3600);
                totalSec = longitude*(-60)-(float)degree*60;

            }else{
                degree = (int)longitude;
                nbsec = (int)(longitude*3600-(float)degree*3600);
                totalSec = longitude*60-(float)degree*60;

            }
            min = nbsec/60;
            //sec = nbsec-60*min;
            sec = (int)((totalSec -min) * 60 * 1000);

            snprintf(GpsLong, 31,  "%d/%d,%d/%d,%d/%d", degree, 1, min, 1, sec, 1000 );
            if(latitude<0){
                degree = (int)(latitude* (-1));
                nbsec = (int)(latitude*(-3600)-(float)degree*3600);
                totalSec = latitude*(-60)-(float)degree*60;
            }else{
                degree = (int)latitude;
                nbsec = (int)(latitude*3600-(float)degree*3600);
                totalSec = latitude*60-(float)degree*60;
            }
            min = nbsec/60;
            sec = (int)((totalSec -min) * 60 * 1000);
            snprintf(GpsLat, 31, "%d/%d,%d/%d,%d/%d", degree, 1, min, 1, sec, 1000 );
            snprintf(GpsAlt, 20, "%d/1", altitude);

            if(longitude>=0){
                strncpy(GpsLongRef, "E", 2);
            }else{
                strncpy(GpsLongRef, "W", 2);
            }
            if(latitude>=0){
                strncpy(GpsLatRef, "N", 2);
            }else{
                strncpy(GpsLatRef, "S", 2);
            }
            if(altitude>=0){
                strncpy(GpsAltRef, "0", 2);
            }else{
                strncpy(GpsAltRef, "1", 2);
            }

            if (params.get(CameraParameters::KEY_GPS_TIMESTAMP) != NULL) {
                 ptm = gmtime(&rawtime);
                 snprintf( GpsDateStamp, 11, "%d:%02d:%02d",
                        ptm->tm_year+1900 ,
                        ptm->tm_mon+1,
                        ptm->tm_mday );
            } else {
                snprintf( GpsDateStamp, 11, "%d:%02d:%02d",
                        OurT->tm_year+1900 ,
                        OurT->tm_mon+1,
                        OurT->tm_mday );
            }
            snprintf( GpsTimeStamp, STR_MAX,
                    "%d/%d,%d/%d,%d/%d",
                    OurT->tm_hour, 1,
                    OurT->tm_min, 1,
                    OurT->tm_sec, 1 );

            strncpy(GpsMapDatum, EXIF_DEFAULT_GPS_MAPDATUM, strlen(EXIF_DEFAULT_GPS_MAPDATUM) + 1);

            strncpy(GpsStatus, "V", 2);

            //TAG_GPS_VERS_ID
            char gps_version_id[STR_MAX] = EXIF_DEFAULT_GPS_VERSION;
            exifElementTable[TagIndex].Tag = TAG_GPS_VERS_ID;
            exifElementTable[TagIndex].Format = FMT_BYTE;
            exifElementTable[TagIndex].Value = (char*)gps_version_id;
            exifElementTable[TagIndex].DataLength = 4;
            exifElementTable[TagIndex].GpsTag = TRUE;
            TagIndex++;
            gpsTagCount++;

            //TAG_GPS_LAT_REF
            exifElementTable[TagIndex].Tag = TAG_GPS_LAT_REF;
            exifElementTable[TagIndex].Format = FMT_STRING;
            exifElementTable[TagIndex].Value = (char*)GpsLatRef;
            exifElementTable[TagIndex].DataLength = 2;
            exifElementTable[TagIndex].GpsTag = TRUE;
            TagIndex++;
            gpsTagCount++;

            //TAG_GPS_LAT
            exifElementTable[TagIndex].Tag = TAG_GPS_LAT;
            exifElementTable[TagIndex].Format = FMT_URATIONAL;
            exifElementTable[TagIndex].Value = (char*)GpsLat;
            exifElementTable[TagIndex].DataLength = 3;
            exifElementTable[TagIndex].GpsTag = TRUE;
            TagIndex++;
            gpsTagCount++;

            //TAG_GPS_LONG_REF
            exifElementTable[TagIndex].Tag = TAG_GPS_LONG_REF;
            exifElementTable[TagIndex].Format = FMT_STRING;
            exifElementTable[TagIndex].Value = (char*)GpsLongRef;
            exifElementTable[TagIndex].DataLength = 2;
            exifElementTable[TagIndex].GpsTag = TRUE;
            TagIndex++;
            gpsTagCount++;

            //TAG_GPS_LONG
            exifElementTable[TagIndex].Tag = TAG_GPS_LONG;
            exifElementTable[TagIndex].Format = FMT_URATIONAL;
            exifElementTable[TagIndex].Value = (char*)GpsLong;
            exifElementTable[TagIndex].DataLength = 3;
            exifElementTable[TagIndex].GpsTag = TRUE;
            TagIndex++;
            gpsTagCount++;

            //TAG_GPS_ALT_REF
            exifElementTable[TagIndex].Tag = TAG_GPS_ALT_REF;
            exifElementTable[TagIndex].Format = FMT_BYTE;
            exifElementTable[TagIndex].Value = (char*)GpsAltRef;
            exifElementTable[TagIndex].DataLength = 1;
            exifElementTable[TagIndex].GpsTag = TRUE;
            TagIndex++;
            gpsTagCount++;

            //TAG_GPS_ALT
            exifElementTable[TagIndex].Tag = TAG_GPS_ALT;
            exifElementTable[TagIndex].Format = FMT_SRATIONAL;
            exifElementTable[TagIndex].Value = (char*)GpsAlt;
            exifElementTable[TagIndex].DataLength = 1;
            exifElementTable[TagIndex].GpsTag = TRUE;
            TagIndex++;
            gpsTagCount++;

            //TAG_GPS_TIMESTAMP
            exifElementTable[TagIndex].Tag = TAG_GPS_TIMESTAMP;
            exifElementTable[TagIndex].Format = FMT_URATIONAL;
            exifElementTable[TagIndex].Value = (char*)GpsTimeStamp;
            exifElementTable[TagIndex].DataLength = 3;
            exifElementTable[TagIndex].GpsTag = TRUE;
            TagIndex++;
            gpsTagCount++;

            //TAG_GPS_DATESTAMP
            exifElementTable[TagIndex].Tag = TAG_GPS_DATESTAMP;
            exifElementTable[TagIndex].Format = FMT_STRING;
            exifElementTable[TagIndex].Value = (char*)GpsDateStamp;
            exifElementTable[TagIndex].DataLength = 11;
            exifElementTable[TagIndex].GpsTag = TRUE;
            TagIndex++;
            gpsTagCount++;

            //TAG_GPS_RECEIVER_STATUS
            exifElementTable[TagIndex].Tag = TAG_GPS_RECEIVER_STATUS;
            exifElementTable[TagIndex].Format = FMT_STRING;
            exifElementTable[TagIndex].Value = (char*)GpsStatus;
            exifElementTable[TagIndex].DataLength = 2;
            exifElementTable[TagIndex].GpsTag = TRUE;
            TagIndex++;
            gpsTagCount++;

            //TAG_GPS_MAPDATUM
            exifElementTable[TagIndex].Tag = TAG_GPS_MAPDATUM;
            exifElementTable[TagIndex].Format = FMT_STRING;
            exifElementTable[TagIndex].Value = (char*)GpsMapDatum;
            exifElementTable[TagIndex].DataLength = -1;
            exifElementTable[TagIndex].GpsTag = TRUE;
            TagIndex++;
            gpsTagCount++;

            //TAG_GPS_PROCESSING_METHOD
            if (params.get(CameraParameters::KEY_GPS_PROCESSING_METHOD) != NULL) {
                strncpy(GpsProcessingMethod,
                        params.get(CameraParameters::KEY_GPS_PROCESSING_METHOD),
                        strlen(params.get(CameraParameters::KEY_GPS_PROCESSING_METHOD))+1);

                exifElementTable[TagIndex].Tag = TAG_GPS_PROCESSING_METHOD;
                exifElementTable[TagIndex].Format = FMT_STRING;
                exifElementTable[TagIndex].Value = (char*)GpsProcessingMethod;
                exifElementTable[TagIndex].DataLength = -1;
                exifElementTable[TagIndex].GpsTag = TRUE;
                TagIndex++;
                gpsTagCount++;
            }
        }

        /* *********************** Tags used by IFD1 (Thumbnail) *************************** */

        if((inThumbBufSize>0)&&(inThumbBuffer!=NULL)){

            ::ImageInfo.ThumbnailSize = inThumbBufSize;
            if(NULL != params.get( CameraParameters::KEY_JPEG_THUMBNAIL_WIDTH)) {
                ::ImageInfo.ThumbnailWidth = params.getInt( CameraParameters::KEY_JPEG_THUMBNAIL_WIDTH);
                DBGT_PTRACE("KEY_JPEG_THUMBNAIL_WIDTH value set = %d", ::ImageInfo.ThumbnailWidth);
            }else{
                DBGT_PTRACE("KEY_JPEG_THUMBNAIL_WIDTH not set");
                ::ImageInfo.ThumbnailWidth = 0;
            }
            if(NULL != params.get( CameraParameters::KEY_JPEG_THUMBNAIL_HEIGHT)) {
                ::ImageInfo.ThumbnailHeight = params.getInt( CameraParameters::KEY_JPEG_THUMBNAIL_HEIGHT);
                DBGT_PTRACE("KEY_JPEG_THUMBNAIL_HEIGHT value set = %d", ::ImageInfo.ThumbnailHeight);
            }else{
                DBGT_PTRACE("KEY_JPEG_THUMBNAIL_HEIGHT not set");
                ::ImageInfo.ThumbnailHeight = 0;
            }
        }else{
            ::ImageInfo.ThumbnailSize = 0;
        }

        /* *********************** Generate Exif header ******************************** */
        DBGT_PTRACE("create_EXIF");
        create_EXIF(exifElementTable, exifTagCount,gpsTagCount,hasDateTimeTag);

        if((inThumbBufSize>0)&&(inThumbBuffer!=NULL)){
            DBGT_PTRACE("Add thumbnail to EXIF header");
            ReplaceThumbnailFromBuffer(inThumbBuffer, inThumbBufSize);
        }

        // Write image data back to a raw buffer.
        if(WriteJpegToBuffer((unsigned char*)outBuffer, outBufSize) == false){
            DBGT_CRITICAL("Fail to write back");
            return -1;
        }

        DBGT_EPILOG("");
        return 0;
    };

}; //namespace android


#else
/* **************************EXIF_MIXER**************************** */

#include "STECamOmxILExtIndex.h"
#include "STECamUtils.h"
#include <OMX_Symbian_ComponentExt_Ste.h>

namespace android {
        OMX_ERRORTYPE ExifMngEventHandler(
                        OMX_HANDLETYPE hComponent,OMX_PTR pAppData,
                        OMX_EVENTTYPE eEvent,OMX_U32 nData1,
                        OMX_U32 nData2, OMX_PTR pEventData);

        OMX_ERRORTYPE ExifMngFillBufferDone(
                        OMX_IN OMX_HANDLETYPE hComponent,
                        OMX_IN OMX_PTR pAppData,
                        OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);

        OMX_ERRORTYPE ExifMngEmptyBufferDone(
                        OMX_IN OMX_HANDLETYPE hComponent,
                        OMX_IN OMX_PTR pAppData,
                        OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);

#undef _CNAME_
#define _CNAME_ SteExifMng

        SteExifMng::SteExifMng(
                        char* inThumbBuffer,
                        int inThumbSize,
                        int inThumbWidth,
                        int inThumbHeight,
                        char* inPictBuffer,
                        int inPictSize,
                        int inPictWidth,
                        int inPictHeight,
                        const CameraParameters &aParams,
                        char* &outBuffer,
                        int* size):
                        pExifInThumbBuffer(NULL),
                        pExifInPictBuffer(NULL),
                        pExifOutBuffer(NULL)
        {
                OMX_BOOL IsThumbnail = OMX_TRUE;
                DBGT_PROLOG("");
                char exifmixer[] = "OMX.ST.VFM.EXIFMixer";
                OMX_ERRORTYPE err = OMX_ErrorNone;

                //initialize omxutils
                err = mOmxUtils.init();
                if(err != OMX_ErrorNone){
                        DBGT_CRITICAL("can't initialize omx utils");
                        return;
                }

                err = (mOmxUtils.interface()->GetpOMX_Init())();
                if(err != OMX_ErrorNone){
                        DBGT_CRITICAL("can't initialize omx utils interface");
                        return;
                }

                mExifCallback.EmptyBufferDone  = ExifMngEmptyBufferDone;
                mExifCallback.FillBufferDone   = ExifMngFillBufferDone;
                mExifCallback.EventHandler     = ExifMngEventHandler;

                err = (mOmxUtils.interface()->GetpOMX_GetHandle())(&mExifMixer, exifmixer, this, &mExifCallback);
                if(err == OMX_ErrorNone &&  mExifMixer != 0){
                        DBGT_PTRACE("mExifMixer GetHandle Done for EXIFMIXER ");
                }else{
                        DBGT_CRITICAL("mExifMixer GetHandle failed err= %d (0x%x) ", (OMX_ERRORTYPE) err,(OMX_ERRORTYPE) err );
                }

                sem_init(&mStateEXIF_sem , 0 , 0);
                sem_init(&mExif_sem , 0 , 0);
                sem_init(&mExif_eos_sem , 0 , 0);

                mInThumbWidth = inThumbWidth;
                mInThumbHeight = inThumbHeight;
                mInPictWidth = inPictWidth;
                mInPictHeight = inPictHeight;
                mInThumbBufSize = inThumbSize;
                mInPictBufSize =inPictSize;

                if((mInThumbWidth == 0) || (mInThumbHeight == 0) || (mInThumbBufSize == 0)){
                    IsThumbnail = OMX_FALSE;
                }
                OmxUtils::StructWrapper<OMX_PARAM_PORTDEFINITIONTYPE>::init(paramEXIFinput1);
                OmxUtils::StructWrapper<OMX_PARAM_PORTDEFINITIONTYPE>::init(paramEXIFinput2);
                OmxUtils::StructWrapper<OMX_PARAM_PORTDEFINITIONTYPE>::init(paramEXIFoutput);

                /****************************************************************************/
                //   Initialize EXIF input 1
                /****************************************************************************/
                if(IsThumbnail == OMX_TRUE){
                OmxUtils::initialize(paramEXIFinput1, OMX_PortDomainImage, 0);
                err = OMX_GetParameter(mExifMixer, OMX_IndexParamPortDefinition, &paramEXIFinput1);
                if(err == OMX_ErrorNone){
                        DBGT_PTRACE("OMX_GetParameter OMX_IndexParamPortDefinition port 0 EXIFMIXER passed");
                }else{
                        DBGT_CRITICAL("OMX_GetParameter OMX_IndexParamPortDefinition port 0 EXIFMIXER failed err = 0x%x", err);
                }

                paramEXIFinput1.nBufferCountActual = 1;     // the index of the input port. Should be modified.
                // Here, the min number of buffers to be used is retrieved
                OMX_IMAGE_PORTDEFINITIONTYPE    *pt_exifinput1 = &(paramEXIFinput1.format.image);
                pt_exifinput1->cMIMEType = (OMX_STRING)"";
                pt_exifinput1->pNativeRender = (OMX_NATIVE_DEVICETYPE)NULL;
                pt_exifinput1->nFrameWidth = (OMX_U32) mInThumbWidth;
                pt_exifinput1->nFrameHeight = (OMX_U32) mInThumbHeight;
                pt_exifinput1->nStride = (OMX_U32) (mInThumbWidth * 3 / 2);
                pt_exifinput1->bFlagErrorConcealment  = (OMX_BOOL)OMX_FALSE;
                pt_exifinput1->eCompressionFormat  = (OMX_IMAGE_CODINGTYPE)OMX_IMAGE_CodingJPEG;
                pt_exifinput1->eColorFormat = (OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatUnused;
                pt_exifinput1->pNativeWindow = (OMX_NATIVE_DEVICETYPE)NULL;
                err = OMX_SetParameter(mExifMixer, OMX_IndexParamPortDefinition, &paramEXIFinput1);
                if(err == OMX_ErrorNone){
                        DBGT_PTRACE("OMX_SetParameter OMX_IndexParamPortDefinition port 0 EXIFMIXER passed ");
                }else{
                        DBGT_CRITICAL("OMX_SetParameter OMX_IndexParamPortDefinition port 0 EXIFMIXER failed err = 0x%x", err);
                }

                err = OMX_GetParameter(mExifMixer, OMX_IndexParamPortDefinition, &paramEXIFinput1);
                if(err == OMX_ErrorNone){
                        DBGT_PTRACE("OMX_GetParameter OMX_IndexParamPortDefinition port 0 EXIFMIXER passed");
                }else{
                        DBGT_CRITICAL("OMX_GetParameter OMX_IndexParamPortDefinition port 0 EXIFMIXER failed err = 0x%x", err);
                }

                DBGT_PTRACE("Dumping params for paramEXIFinput1");
                OmxUtils::dump(paramEXIFinput1);
                }else{
                    err = OMX_SendCommand(mExifMixer, OMX_CommandPortDisable, 0, NULL);
                    if(err == OMX_ErrorNone){
                        DBGT_PTRACE("OMX_SendCommand Port Disable 0 mExifMixer ");
                    }else{
                        DBGT_CRITICAL("OMX_SendCommand Port Disable 0 mExifMixer failed err = 0x%x", err);
                    }
                }

                /****************************************************************************/
                //   Initialize EXIF input 2
                /****************************************************************************/
                OmxUtils::initialize(paramEXIFinput2, OMX_PortDomainImage, 1);
                err = OMX_GetParameter(mExifMixer, OMX_IndexParamPortDefinition, &paramEXIFinput2);
                if(err == OMX_ErrorNone){
                        DBGT_PTRACE("OMX_GetParameter OMX_IndexParamPortDefinition port 1 EXIFMIXER passed");
                }else{
                        DBGT_CRITICAL("OMX_GetParameter OMX_IndexParamPortDefinition port 1 EXIFMIXER failed err = 0x%x", err);
                }

                paramEXIFinput2.nBufferCountActual = 1;     // the index of the input port. Should be modified.
                // Here, the min number of buffers to be used is retrieved
                OMX_IMAGE_PORTDEFINITIONTYPE    *pt_exifinput2 = &(paramEXIFinput2.format.image);
                pt_exifinput2->cMIMEType = (OMX_STRING)"";
                pt_exifinput2->pNativeRender = (OMX_NATIVE_DEVICETYPE)NULL;
                pt_exifinput2->nFrameWidth = (OMX_U32)mInPictWidth;
                pt_exifinput2->nFrameHeight = (OMX_U32)mInPictHeight;
                pt_exifinput2->bFlagErrorConcealment  = (OMX_BOOL)OMX_FALSE;
                pt_exifinput2->eCompressionFormat  = (OMX_IMAGE_CODINGTYPE)OMX_IMAGE_CodingEXIF;
                pt_exifinput2->eColorFormat = (OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatUnused;
                pt_exifinput2->pNativeWindow = (OMX_NATIVE_DEVICETYPE)NULL;
                err = OMX_SetParameter(mExifMixer, OMX_IndexParamPortDefinition, &paramEXIFinput2);
                if(err == OMX_ErrorNone){
                        DBGT_PTRACE("OMX_SetParameter OMX_IndexParamPortDefinition port 1 EXIFMIXER passed ");
                }else{
                        DBGT_CRITICAL("OMX_SetParameter OMX_IndexParamPortDefinition port 1 EXIFMIXER failed err = 0x%x", err);
                }

                err = OMX_GetParameter(mExifMixer, OMX_IndexParamPortDefinition, &paramEXIFinput2);
                if(err == OMX_ErrorNone){
                        DBGT_PTRACE("OMX_GetParameter OMX_IndexParamPortDefinition port 1 EXIFMIXER passed");
                }else{
                        DBGT_CRITICAL("OMX_GetParameter OMX_IndexParamPortDefinition port 1 EXIFMIXER failed err = 0x%x", err);
                }
                DBGT_PTRACE("Dumping params for paramEXIFinput2");
                OmxUtils::dump(paramEXIFinput2);

                /****************************************************************************/
                //   Initialize EXIF output
                /****************************************************************************/
                OmxUtils::initialize(paramEXIFoutput, OMX_PortDomainImage, 2);
                err = OMX_GetParameter(mExifMixer, OMX_IndexParamPortDefinition, &paramEXIFoutput);
                if(err == OMX_ErrorNone){
                        DBGT_PTRACE("OMX_GetParameter OMX_IndexParamPortDefinition port 2 EXIFMIXER passed");
                }else{
                        DBGT_CRITICAL("OMX_GetParameter OMX_IndexParamPortDefinition port 2 EXIFMIXER failed err = 0x%x", err);
                }

                paramEXIFoutput.nBufferCountActual = 1;     // the index of the input port. Should be modified.
                // Here, the min number of buffers to be used is retrieved
                OMX_IMAGE_PORTDEFINITIONTYPE    *pt_exifoutput = &(paramEXIFoutput.format.image);
                pt_exifoutput->cMIMEType = (OMX_STRING)"";
                pt_exifoutput->pNativeRender = (OMX_NATIVE_DEVICETYPE)NULL;
                pt_exifoutput->nFrameWidth = (OMX_U32)mInPictWidth;
                pt_exifoutput->nFrameHeight = (OMX_U32)mInPictHeight;
                pt_exifoutput->bFlagErrorConcealment  = (OMX_BOOL)OMX_FALSE;
                pt_exifoutput->eCompressionFormat  = (OMX_IMAGE_CODINGTYPE)OMX_IMAGE_CodingEXIF;
                pt_exifoutput->eColorFormat = (OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatUnused;
                pt_exifoutput->pNativeWindow = (OMX_NATIVE_DEVICETYPE)NULL;
                err = OMX_SetParameter(mExifMixer, OMX_IndexParamPortDefinition, &paramEXIFoutput);
                if(err == OMX_ErrorNone){
                        DBGT_PTRACE("OMX_SetParameter OMX_IndexParamPortDefinition port 2 EXIFMIXER passed ");
                }else{
                        DBGT_CRITICAL("OMX_SetParameter OMX_IndexParamPortDefinition port 2 EXIFMIXER failed err = 0x%x", err);
                }

                err = OMX_GetParameter(mExifMixer, OMX_IndexParamPortDefinition, &paramEXIFoutput);
                if(err == OMX_ErrorNone){
                        DBGT_PTRACE("OMX_GetParameter OMX_IndexParamPortDefinition port 2 EXIFMIXER passed");
                }else{
                        DBGT_CRITICAL("OMX_GetParameter OMX_IndexParamPortDefinition port 2 EXIFMIXER failed err = 0x%x", err);
                }

                DBGT_PTRACE("Dumping params for paramEXIFoutput");
                OmxUtils::dump(paramEXIFoutput);


                err =OMX_SendCommand(mExifMixer,OMX_CommandStateSet,OMX_StateIdle,NULL);
                if(err == OMX_ErrorNone){
                        DBGT_PTRACE("Sending WAIT_STATE_IDLE passed for EXIFMIXER");
                }

                /* Allocate input buffers */
                if(IsThumbnail == OMX_TRUE){
                err = OMX_AllocateBuffer(mExifMixer, &pExifInThumbBuffer, 0,NULL , (OMX_U32)mInThumbBufSize);
                if(err == OMX_ErrorNone && pExifInThumbBuffer->pBuffer != 0){
                        DBGT_PTRACE("Allocate input buffer done for EXIFMIXER thumb size:%lu",pExifInThumbBuffer->nAllocLen);
                }else{
                        DBGT_CRITICAL("Allocate input  thumb buffer err= %d (0x%x) ", (OMX_ERRORTYPE) err,(OMX_ERRORTYPE) err );
                    }
                }

                err = OMX_AllocateBuffer(mExifMixer, &pExifInPictBuffer, 1, NULL , (OMX_U32)mInPictBufSize);
                if(err == OMX_ErrorNone && pExifInPictBuffer->pBuffer != 0){
                        DBGT_PTRACE("Allocate input buffer done for EXIFMIXER pict size:%lu",pExifInPictBuffer->nAllocLen);
                }else{
                        DBGT_CRITICAL("Allocate input pict buffer err= %d (0x%x) ", (OMX_ERRORTYPE) err,(OMX_ERRORTYPE) err );
                }

                if(IsThumbnail == OMX_TRUE){
                pExifInThumbBuffer->nFilledLen = pExifInThumbBuffer->nAllocLen;
                }
                pExifInPictBuffer->nFilledLen = pExifInPictBuffer->nAllocLen;

                /* Allocate output buffer */
                mOutputBufSize = paramEXIFoutput.nBufferSize;
                err = OMX_AllocateBuffer(mExifMixer, &pExifOutBuffer, 2, NULL, (OMX_U32)mOutputBufSize);
                if(err == OMX_ErrorNone &&  pExifOutBuffer->pBuffer != 0){
                        DBGT_PTRACE("Allocate out buffer done for EXIFMIXER size:%lu",pExifOutBuffer->nAllocLen);
                }else{
                        DBGT_CRITICAL("Allocate output buffer err= %d (0x%x) ", (OMX_ERRORTYPE) err,(OMX_ERRORTYPE) err );
                }
                pExifOutBuffer->nFilledLen = pExifOutBuffer->nAllocLen;
                camera_sem_wait(&mStateEXIF_sem, SEM_WAIT_TIMEOUT);

                err =OMX_SendCommand(mExifMixer,OMX_CommandStateSet,OMX_StateExecuting,NULL);
                if(err == OMX_ErrorNone){
                        DBGT_PTRACE("Sending WAIT_STATE_EXECUTING passed for EXIFMIXER");
                }else{
                        DBGT_CRITICAL("OMX_SendCommand OMX_StateExecuting failed for EXIFMIXER 0x%x",err);
                }
                camera_sem_wait(&mStateEXIF_sem, SEM_WAIT_TIMEOUT);

                // recopy frames to input buffers
                if(IsThumbnail == OMX_TRUE){
                memcpy(pExifInThumbBuffer->pBuffer, inThumbBuffer, mInThumbBufSize );
                }
                memcpy(pExifInPictBuffer->pBuffer, inPictBuffer, mInPictBufSize );

                err = setGPSTags(aParams, mExifMixer, 2);
                if(err ==OMX_ErrorNone ){
                        DBGT_PTRACE("setGPSTags Passed");
                } else {
                        DBGT_CRITICAL("setGPSTags Failed");
                }

                err =OMX_FillThisBuffer(mExifMixer,pExifOutBuffer);
                if(err ==OMX_ErrorNone ){
                        DBGT_PTRACE("OMX_FillThisBuffer Done 0x%x  ", (unsigned int) pExifOutBuffer->pBuffer);
                } else {
                        DBGT_CRITICAL("OMX_FillThisBuffer error: %s", OmxUtils::name(err));
                }

                if(IsThumbnail == OMX_TRUE){
                err =OMX_EmptyThisBuffer(mExifMixer,pExifInThumbBuffer);
                if(err ==OMX_ErrorNone ){
                        DBGT_PTRACE("OMX_EmptyThisBuffer Done 0x%x  ", (unsigned int) pExifInThumbBuffer->pBuffer);
                } else {
                        DBGT_CRITICAL("OMX_EmptyThisBuffer error: %s", OmxUtils::name(err));
                    }
                }

                err =OMX_EmptyThisBuffer(mExifMixer,pExifInPictBuffer);
                if(err ==OMX_ErrorNone ){
                        DBGT_PTRACE("OMX_EmptyThisBuffer Done 0x%x  ", (unsigned int) pExifInPictBuffer->pBuffer);
                } else {
                        DBGT_CRITICAL("OMX_EmptyThisBuffer error: %s", OmxUtils::name(err));
                }
                camera_sem_wait(&mExif_sem, SEM_WAIT_TIMEOUT);

                outBuffer = (char*)pExifOutBuffer->pBuffer;
                *size = pExifOutBuffer->nFilledLen;

                DBGT_EPILOG("");
        }

        SteExifMng::~SteExifMng()
        {
                DBGT_PROLOG("");
                OMX_ERRORTYPE err = OMX_ErrorNone;

                err =OMX_SendCommand(mExifMixer,OMX_CommandStateSet,OMX_StateIdle,NULL);
                if(err == OMX_ErrorNone){
                        DBGT_PTRACE("Sending WAIT_STATE_IDLE passed for EXIFMIXER");
                }else{
                        DBGT_CRITICAL("OMX_SendCommand OMX_StateIdle failed for EXIFMIXER 0x%x",err);
                }
                camera_sem_wait(&mStateEXIF_sem, SEM_WAIT_TIMEOUT);

                err =OMX_SendCommand(mExifMixer,OMX_CommandStateSet,OMX_StateLoaded,NULL);
                if(err == OMX_ErrorNone){
                        DBGT_PTRACE("Sending WAIT_STATE_LOADED passed for EXIFMIXER");
                }else{
                        DBGT_CRITICAL("OMX_SendCommand OMX_StateLoaded failed for EXIFMIXER 0x%x",err);
                }

                if(pExifInThumbBuffer != NULL){
                err =OMX_FreeBuffer(mExifMixer,(OMX_U32)0, pExifInThumbBuffer);
                if(err == OMX_ErrorNone){
                        DBGT_PTRACE("OMX_FreeBuffer passed for Thumb Jpeg");
                    }
                }
                if(pExifInPictBuffer != NULL){
                err =OMX_FreeBuffer(mExifMixer,(OMX_U32)1, pExifInPictBuffer);
                if(err == OMX_ErrorNone){
                        DBGT_PTRACE("OMX_FreeBuffer passed for Pict Jpeg");
                    }
                }
                if(pExifOutBuffer != NULL){
                err =OMX_FreeBuffer(mExifMixer,  (OMX_U32)2, pExifOutBuffer);
                if(err == OMX_ErrorNone){
                        DBGT_PTRACE("OMX_FreeBuffer passed for Out Jpeg");
                    }
                }

                camera_sem_wait(&mStateEXIF_sem, SEM_WAIT_TIMEOUT);

                sem_destroy(&mStateEXIF_sem);
                sem_destroy(&mExif_sem);
                sem_destroy(&mExif_eos_sem);


                DBGT_PTRACE("Free handle Exif mixer");
                err = mOmxUtils.interface()->GetpOMX_FreeHandle()(mExifMixer);
                if(err == OMX_ErrorNone){
                        DBGT_PTRACE("In Exif Mixer destructor GetpOMX_FreeHandle camera passed ");
                }else{
                        DBGT_CRITICAL("In Exif Mixer destructor GetpOMX_FreeHandle camera failed ");
                }

                err = (mOmxUtils.interface()->GetpOMX_Deinit())();
                if(err == OMX_ErrorNone){
                    DBGT_PTRACE("In Exif Mixer destructor GetpOMX_Deinit  passed ");
                }else{
                    DBGT_CRITICAL("In Exif Mixer destructor GetpOMX_Deinit failed ");
                }

                DBGT_EPILOG("");
        }

/*****************************************************************************/
//  GPS keys provide decimal values which are to converted to
//  GPS degree/minute/seconds.  Say GPS latitude key value is 23.19543.
//  ** The whole units of degrees = 23
//  Multiply the decimal by 60 (0.19543 * 60 = 11.7258)
//  ** The whole number provides the minutes  =  11
//  Again multiply the remaining decimal by 60 (0.7258* 60 = 43.548)
//  ** The resulting number becomes the seconds (43.548).
//  Seconds field can have decimal values till 3 points after decimal.
//  These values appear as rationals in the exif viewer as:
//       23/1 11/1 43548/1000
// For the OMX_U64 values, the first 32 bits are the numerator and the last 32
// the denominator .
/*****************************************************************************/
        OMX_BOOL SteExifMng::convertToDegrees(OMX_U64 &aDegrees, OMX_U64 &aMinutes, OMX_U64 &aSeconds, char const *const aGPSKey )
        {
                DBGT_PROLOG("GPS key %s", aGPSKey);
                float tmpVal;
                OMX_U32 absVal = 0;
                OMX_BOOL referenceDir = OMX_TRUE;

                if ( NULL != aGPSKey ) {
                        tmpVal = atof(aGPSKey);

                        if ( 0 > tmpVal) {
                                // If the valur of key is negative , then the direction is opposite to
                                // whatever the reference direction is.
                                DBGT_PTRACE("Direction opposite of reference direction");
                                referenceDir = OMX_FALSE;
                                tmpVal = tmpVal * (-1);
                        }

                        aDegrees = ((OMX_U64)tmpVal<<32)|1;
                        aMinutes = ((OMX_U64)abs(extractSubUnitLocation(tmpVal)) << 32) | 1;
                        aSeconds = (extractSubUnitLocation(tmpVal) * 1000) ;
                        aSeconds = ((aSeconds<< 32)|(0x3E8));
                }

                DBGT_EPILOG("");
                return referenceDir;
        }

        /**********************************************************************************/
        //  KEY_GPS_TIMESTAMP provides time in the format of seconds since January 1, 1970.
        //  This has to be converted to real time which can be done as  follows:
        //  Say GPS time stamp value is tS= 1420724859.
        //  (tS %( 24x60x60)) leaves us with the absolute number of seconds elapsed today.
        //  Hence,ts now is 49659. This can be converted easily to hours , minutes and seconds.
        //  These values appear as rationals in the exif viewer as:
        //     13/1 47/1 39/1
        //  For the OMX_U64 values, the first 32 bits are the numerator and the last 32
        //  the denominator.
        /*****************************************************************************/
        void SteExifMng::convertUTCToTime(OMX_U64& aHours, OMX_U64& aMinutes, OMX_U64& aSeconds, char const* const aGPSKey )
        {
            DBGT_PROLOG("UTC time %s", aGPSKey);
            OMX_U64 tmpTime;
            if ( NULL != aGPSKey )
            {
                tmpTime = atoi(aGPSKey);
                tmpTime = (tmpTime % 86400);

                aSeconds = ((OMX_U64)extractSubUnitTime(tmpTime)<<32)| 1;
                aMinutes = ((OMX_U64)extractSubUnitTime(tmpTime)<<32)| 1;
                aHours =  (tmpTime<<32)| 1;
            }
            DBGT_EPILOG("");
        }


        OMX_U64 SteExifMng::extractSubUnitTime(OMX_U64 &aTime)
        {
            DBGT_PROLOG("Time Value %lld", aTime);
            OMX_U64 tmp;
            tmp = aTime % 60;
            aTime = aTime / 60;
            DBGT_EPILOG("");
            return tmp;
        }

        float SteExifMng::extractSubUnitLocation(float &aWholeValue)
        {
            DBGT_PROLOG("Whole Value %f", aWholeValue);

            aWholeValue -= abs(aWholeValue);
            aWholeValue *= 60;

            DBGT_EPILOG("");
            return aWholeValue;
        }

        OMX_ERRORTYPE SteExifMng::setGPSTags(const CameraParameters &aParams, OMX_HANDLETYPE aHandle, int aPort)
        {
            DBGT_PROLOG("Latitude: %s Longitude: %s Altitude: %s TimeStamp: %s", aParams.get(CameraParameters::KEY_GPS_LATITUDE),
                    aParams.get(CameraParameters::KEY_GPS_LONGITUDE), aParams.get(CameraParameters::KEY_GPS_ALTITUDE),
                    aParams.get(CameraParameters::KEY_GPS_TIMESTAMP));

            DBGT_ASSERT(NULL != aHandle, "Handle is Null");
            OMX_ERRORTYPE err = OMX_ErrorNone;
            OMX_INDEXTYPE index;

            if( (NULL != aParams.get(CameraParameters::KEY_GPS_LATITUDE))  || (NULL != aParams.get(CameraParameters::KEY_GPS_LONGITUDE)) ||
                    (NULL != aParams.get(CameraParameters::KEY_GPS_ALTITUDE)) || (NULL != aParams.get(CameraParameters::KEY_GPS_TIMESTAMP)) ){

                char gpsIndex[] = "OMX.Symbian.Index.Config.GpsLocation";

                DBGT_PTRACE("Setting GPS tags");
                OmxUtils::StructContainer<OMX_SYMBIAN_CONFIG_GPSLOCATIONTYPE> gpsLocation;
                err = OMX_GetExtensionIndex(aHandle, gpsIndex, &index);
                if(err == OMX_ErrorNone){
                    DBGT_PTRACE("GPS tag OMX_GetExtensionIndex passed ");
                }else{
                    DBGT_CRITICAL("GPS tag OMX_GetExtensionIndex failed");
                }

                gpsLocation.ptr()->bLatitudeRefNorth = convertToDegrees(
                        gpsLocation.ptr()->nLatitudeDegrees,
                        gpsLocation.ptr()->nLatitudeMinutes,
                        gpsLocation.ptr()->nLatitudeSeconds,
                        aParams.get(CameraParameters::KEY_GPS_LATITUDE));

                gpsLocation.ptr()->bLongitudeRefEast = convertToDegrees(
                        gpsLocation.ptr()->nLongitudeDegrees,
                        gpsLocation.ptr()->nLongitudeMinutes,
                        gpsLocation.ptr()->nLongitudeSeconds,
                        aParams.get(CameraParameters::KEY_GPS_LONGITUDE));

                if (NULL != aParams.get(CameraParameters::KEY_GPS_ALTITUDE) ) {

                    gpsLocation.ptr()->nAltitudeMeters = atof(aParams.get(CameraParameters::KEY_GPS_ALTITUDE));

                    if ( 0 < gpsLocation.ptr()->nAltitudeMeters ) {
                        gpsLocation.ptr()->bAltitudeRefAboveSea = OMX_FALSE;
                    } else {
                        gpsLocation.ptr()->bAltitudeRefAboveSea = OMX_TRUE;
                        gpsLocation.ptr()->nAltitudeMeters *= -1;
                    }
                    gpsLocation.ptr()->nAltitudeMeters = (((gpsLocation.ptr()->nAltitudeMeters * 1000)<<32) | (0x3E8));
                }

                convertUTCToTime(gpsLocation.ptr()->nHours, gpsLocation.ptr()->nMinutes, gpsLocation.ptr()->nSeconds, aParams.get(CameraParameters::KEY_GPS_TIMESTAMP));

                gpsLocation.ptr()->bLocationKnown = OMX_TRUE;
                gpsLocation.ptr()->nPortIndex = aPort;

                err = OMX_SetConfig(aHandle, index , gpsLocation.ptr());
                if(err == OMX_ErrorNone){
                    DBGT_PTRACE("OMX_SetConfig for GPS passed ");
                }else{
                    DBGT_CRITICAL("OMX_SetConfig for GPS failed");
                }
            } else {
                DBGT_PTRACE("No GPS info available");
            }

            DBGT_EPILOG("");
            return err;
        }

        OMX_ERRORTYPE ExifMngEventHandler(
                        OMX_HANDLETYPE hComponent,OMX_PTR pAppData,
                        OMX_EVENTTYPE eEvent,OMX_U32 nData1,
                        OMX_U32 nData2, OMX_PTR pEventData)
        {
                SteExifMng* pExifMng = (SteExifMng*) pAppData;

                DBGT_PROLOG("Handle: %p AppData: %p", hComponent, pAppData);
                DBGT_PTRACE(    "Event: %s(%d) Data1: %lu Data2: %lu EventData: %p",
                                OmxUtils::name(eEvent), eEvent, nData1, nData2, pEventData);

                DBGT_ASSERT(NULL != hComponent, "Invalid handle" );

                switch (eEvent) {
                case OMX_EventCmdComplete:
                        {
                                OMX_COMMANDTYPE cmd = (OMX_COMMANDTYPE) nData1;
                                DBGT_PTRACE("Cmd: %s(%d)", OmxUtils::name(cmd), cmd);

                                if (cmd == OMX_CommandStateSet) {
                                        OMX_STATETYPE newState = (OMX_STATETYPE) nData2;
                                        DBGT_PTRACE("State: %s(%d)", OmxUtils::name(newState), newState);
                                        sem_post(&pExifMng->mStateEXIF_sem);
                                }else if (OMX_CommandPortEnable == cmd)
                                        sem_post(&pExifMng->mStateEXIF_sem);
                                else if (OMX_CommandPortDisable == cmd)
                                        sem_post(&pExifMng->mStateEXIF_sem);

                                break;
                        }
                case OMX_EventError:
                        {
                                DBGT_CRITICAL(    "Error detected by OMX event handler : %d (0x%x) - %d (0x%x) ",
                                                (OMX_ERRORTYPE) nData1,(OMX_ERRORTYPE) nData1,
                                                (OMX_ERRORTYPE) nData2,(OMX_ERRORTYPE) nData2);
                                break;
                        }
                case OMX_EventPortSettingsChanged:
                        {
                                DBGT_PTRACE("component is reported a port settings change");
                                break;
                        }
                case OMX_EventBufferFlag:
                        {
                                DBGT_PTRACE("component has detected an EOS");
                                if ((nData1 == 2) &&  (nData2 & OMX_BUFFERFLAG_EOS)) {
                                        sem_post(&pExifMng->mExif_eos_sem);
                                }
                                break;
                        }
                default:
                        DBGT_PTRACE("Unknown event: %d", eEvent);
                }

                DBGT_EPILOG("");
                return OMX_ErrorNone;

        };

        OMX_ERRORTYPE ExifMngEmptyBufferDone(
                        OMX_IN OMX_HANDLETYPE hComponent,
                        OMX_IN OMX_PTR pAppData,
                        OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
        {
                SteExifMng* pExifMng = (SteExifMng*) pAppData;

                DBGT_PROLOG("Handle: %p, AppData: %p, BuffHeader: %p", hComponent, pAppData, pBuffer);

                DBGT_EPILOG("");
                return OMX_ErrorNone;
        };

        OMX_ERRORTYPE ExifMngFillBufferDone(
                        OMX_IN OMX_HANDLETYPE hComponent,
                        OMX_IN OMX_PTR pAppData,
                        OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
        {
                SteExifMng* pExifMng = (SteExifMng*) pAppData;
                DBGT_PROLOG("Handle: %p, AppData: %p, BuffHeader: %p", hComponent, pAppData, pBuffer);
                //NULL checking
                DBGT_ASSERT(NULL != hComponent, "Invalid handle");
                DBGT_ASSERT(NULL != pBuffer, "Invalid buffer");
                DBGT_PTRACE("Size: %lu\n", pBuffer->nFilledLen);
                sem_post(&pExifMng->mExif_sem);
                DBGT_PTRACE("EXIF FIll buffer Size: %lu\n", pBuffer->nFilledLen);
                DBGT_EPILOG("");
                return OMX_ErrorNone;
        };

}; //namespace android

#endif //#ifndef ENABLE_EXIF_MIXER
