/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * Defines
 */


#ifndef __INCLUDE_ALGO_TYPEDEFS_H__
#define __INCLUDE_ALGO_TYPEDEFS_H__

/** For Filter matrix. */
#define N			3
#define NUM_FILTERS		10

/** Configuration File Path. */
//#define CONFIG_PATH "./metricconfigfile.dat"

#ifdef __SYMBIAN32__
#  include <e32def.h> 
#else
#undef NULL 
#define NULL    ((void *)0)

/** For General typedef for datatypes. */
typedef signed char TInt8;
typedef unsigned char TUint8;
typedef signed short TInt16;
typedef unsigned short TUint16;
typedef signed int TInt32;
typedef unsigned int TUint32;
typedef signed long long TInt64;
typedef unsigned long long TUint64;
typedef float TReal32;
typedef double TReal64;
typedef void TAny;
#endif


/** 
 * An enum TAlgoType.
 * Supported Algorithm Metrices Enumerations in Algorithm Library. 
 */
typedef enum
    {
    ETypeUnknown = 0,			/**< enum value ETypeUnknown		-   For init. */
    ETypeNegativeImage,			/**< enum value ETypeNegativeImage  -   For Negative Image Metrices. */
    ETypeBlackAndWhite,			/**< enum value ETypeBlackAndWhite  -   For BlackAndWhite Image Metrices. */
    ETypeAutoFocus,				/**< enum value ETypeAutoFocus      -   For Auto Focus Image Metrices. */
    ETypeSepia,					/**< enum value ETypeSepia          -   For Sepia Image Metrices. */
    ETypeWhiteBalance,			/**< enum value ETypeWhiteBalance   -   For White Balance Image Metrices. */
    ETypeSpatialSimilar,		/**< enum value ETypeSpatialSimilar -   For Spatial Similar Image Metrices. */
    ETypeEmboss,				/**< enum value ETypeEmboss         -   For Emboss Image Metrices. */
    ETypeSolarize,				/**< enum value ETypeSolarize       -   For Solarize Image Metrices. */
    ETypeSolarize_basic,		/**< enum value ETypeSolarize_basic -   For Solarize Image Metrices  (basic check). */
    ETypeColorTone,				/**< enum value ETypeColorTone      -   For Color Tone Image Metrices. */
	ETypeZoom,					/**< enum value ETypeZoom           -   For Zoom Image Metrices. */
    ETypeContrast,				/**< enum value ETypeContrast       -   For Contrast Image Metrices. */
    ETypeSaturation,			/**< enum value ETypeSaturation     -   For Saturation Image Metrices. */
    ETypeBrightness,			/**< enum value ETypeBrightness     -   For Brightness Image Metrices. */
    ETypeSharpness,			/**< enum value ETypeSharpness     -   For Sharpness Image Metrices. */
    ETypeExposureValue,			/**< enum value ETypeExposureValue  -   For ExposureValue Image Metrices. */    
    ETypeExposurePreset,		/**< enum value ETypeExposurePreset  -   For ExposurePreset Image Metrices. */    
    ETypeGamma,					/**< enum value ETypeGamma          -   For Gamma Image Metrices. */
    ETypeGamma_basic,			/**< enum value ETypeGamma_basic    -   For Gamma Image Metrices (basic check). */
	ETypeVerticalMirroring,		/**< enum value ETypeVerticalMirroring		-   For Vertical Mirroring Image Metrices: pixel at 1,0 is swapped with pixel 1,H */
	ETypeHorizontalMirroring,	/**< enum value ETypeHorizontalMirroring	-   For Horizontal Mirroring Image Metrices: pixel at 0,1 is swapped with pixel W,1 */
	ETypeBothMirroring,			/**< enum value ETypeBothMirroring			-   For Both Mirroring Image Metrices: pixel at 0, 0 is swapped with pixel W,H  */
	EType0degreesRotation,		/**< enum value EType0degreesRotation		-   For 0degrees Rotation Image Metrices. */
	EType90degreesRotation,		/**< enum value EType90degreesRotation		-   For 90degrees Rotation Image Metrices. */
	EType180degreesRotation,	/**< enum value EType180degreesRotation		-   For 180degrees Rotation Image Metrices. */
	EType270degreesRotation,	/**< enum value EType270degreesRotation		-   For 270degrees Rotation Image Metrices. */
    ETypeImageContent,			/**< enum value ETypeImageContent    -   For testing image content. */
	ETypeLuminanceMean, 		/**< enum value ETypeLuminanceMean    -   For testing image luminance mean Metrices. */
	ETypeCenterFieldOfView,		/**< enum value ETypeCenterFieldOfView    -   For testing image pantilt Metrices. */
	ETypeMaxNb, 	         /**< enum value ETypeMaxNb			 -   Number of values for this enum */

    }TAlgoType;


/** 
 * An enum TAlgoImageFormat.
 * Supported AImage Formats Enumerations in Algorithm Library. 
 */
typedef enum
    {
    EImageFormatUnknown = 0,        /**< enum value EImageFormatUnknown	- For init */
    EImageFormat16bitRGB565,		/**< enum value EImageFormat16bitRGB565		- 16 bits per pixel RGB  format with colors stored as Red  15:11, Green 10:5, and Blue 4:0. */
    EImageFormat16bitBGR565,		/**< enum value EImageFormat16bitRGB565		- 16 bits per pixel RGB  format with colors stored as Blue 15:11, Green 10:5, and Red  4:0. */
    EImageFormat24bitRGB888,		/**< enum value EImageFormat24bitRGB888		- 24 bits per pixel RGB  format with colors stored as Red  23:16, Green 15:8, and Blue 7:0. */
    EImageFormat24bitBGR888,		/**< enum value EImageFormat24bitBGR888		- 24 bits per pixel BGR  format with colors stored as Blue 23:16, Green 15:8, and Red  7:0. */
    EImageFormat16bitARGB4444,		/**< enum value EImageFormat16bitARGB4444	- 16 bits per pixel ARGB format with colors stored as Alpha 15:12,	Red 11:8,	Green 7:4,	and Blue 3:0. */
    EImageFormat16bitARGB1555,		/**< enum value EImageFormat16bitARGB1555	- 16 bits per pixel ARGB format with colors stored as Alpha 15,		Red 14:10,	Green 9:5,	and Blue 4:0. */
    EImageFormat32bitARGB8888,		/**< enum value EImageFormat32bitARGB8888	- 24 bits per pixel ABGR format with colors stored as Alpha 31:24,	Blue 23:16, Green 15:8,	and Red  7:0. */
    EImageFormatBMP,			    /**< enum value EImageFormatBMP			        -   For BMP Format. */
    EImageFormatYUV422Interleaved,			/**< enum value EImageFormatYUV422Interleaved   -   For YUV422 Interleaved Format. */
    EImageFormatYUV420PackedPlanarI420,		/**< enum value EImageFormatYUV420PackedPlanarI420  -   For YUV420 Packed Planar Format. */
    EImageFormatYUV420MBSemiPlanarFiltered,	/**< enum value EImageFormatYUV420MBSemiPlanarFiltered  -   For YUV420MB Semi Planar Filtered. */
    EImageFormatRAW8,				/**< enum value EImageFormatRAW8 -   For RAW8 Format. */
    EImageFormatRAW12,				/**< enum value EImageFormatRAW12 -   For RAW12 Format. */
    EImageMaxNb, 				    /**< enum value EImageMaxNb				        -   Number of values for this enum */
   
    }TAlgoImageFormat;

/** 
 * An struct TAlgoImageDataRGB.
 * Contains the RGB data in three seperate channels in Algorithm Library. 
 */
typedef struct
    {
    TUint8* iRChannel;  /**< Contains the R Channel Data. */
    TUint8* iGChannel;  /**< Contains the G Channel Data. */
    TUint8* iBChannel;  /**< Contains the b Channel Data. */

    }TAlgoImageDataRGB;

/** 
 * An struct TAlgoImageParams.
 * Contains the Image related information parameters in Algorithm Library. 
 */
typedef struct 
    {
	TUint32 iImageWidth;            /**< Contains the Image width in pixels. */
    TUint32 iImageHeight;           /**< Contains the Image height in pixels. */
 	TUint32 iImageStride;           /**< Contains the Image stride in bytes. */
    TUint32 iImageSliceHeight;      /**< Contains the Image sliceheight in pixels. */
	TUint32 iImageNbStatusLine;     /**< Contains the Image NbStatusLine in bytes. */
	TUint32 iImagePixelOrder;       /**< Contains the Image PixelOrder in bytes. */
    TUint32 iImageHeaderLength;     /**< Contains the Image Header Length. */
    TAlgoImageFormat iImageFormat;  /**< Contains the Image Format. */
    TUint8 iBitMaxR;                /**< Contains the number of bits in R channel. */
    TUint8 iBitMaxG;                /**< Contains the number of bits in G channel. */
    TUint8 iBitMaxB;                /**< Contains the number of bits in B channel. */
    TAny* iParams;                  /**< Contains the Image extra params. */

    }TAlgoImageParams;

typedef enum
    {
    EObjectTypeUnknown = 0,
    EObjectTypeFace,
    EObjectTypeObject,
    EObjectTypeFocusedArea,   
    }TAlgoObjectType;

typedef struct 
    {
	TInt32 sRect_sTopLeft_nX;
	TInt32 sRect_sTopLeft_nY;
	TInt32 sRect_sSize_nWidth;
	TInt32 sRect_sSize_nHeight;
	TInt32 sReference_nWidth;
	TInt32 sReference_nHeight;
	TAlgoObjectType eObjectType;
	}TAlgo_RELATIVERECTTYPE;

#define MAX_NUMBER_OF_ROIS 10
typedef struct 
    {
	TUint32 nNumberOfROIs;      /**< Number of ROIs included in this config */
    TAlgo_RELATIVERECTTYPE sROI[MAX_NUMBER_OF_ROIS];
    }TAlgoROIOBJECTINFOTYPE;


/** 
 * An struct TAlgoImage.
 * Contains the Image in Algorithm Library. 
 */
typedef struct
    {
    TAlgoImageDataRGB iImageData;   /**< Contains the Image Data in R, G and B channels. */
    TAlgoImageParams iImageParams;  /**< Contains the Image related information parametrs. */

    }TAlgoImage;

/** 
 * An struct TAlgoMetricRange.
 * Contains the Algorithm Metric Range for different Channels in Image. 
 */
typedef struct
    {
    TInt8 iRangeR;  /**< Contains the Range for R Channel. */
    TInt8 iRangeG;  /**< Contains the Range for G Channel. */
    TInt8 iRangeB;  /**< Contains the Range for B Channel. */

    }TAlgoMetricRange;

/** 
 * An struct TAlgoMetricErrorTolerance.
 * Contains the Algorithm Metric Error Tolerance for different Channels in Image. 
 */
typedef struct
    {
    TReal32 iErrorToleranceR;  /**< Contains the Error Tolerance for R Channel. */
    TReal32 iErrorToleranceG;  /**< Contains the Error Tolerance for G Channel. */
    TReal32 iErrorToleranceB;  /**< Contains the Error Tolerance for B Channel. */

    }TAlgoMetricErrorTolerance;

/** 
 * An struct TAlgoImageTestCoordinates.
 * Contains the Image test area coordinates in Algorithm Library. 
 */
typedef struct
    {
    TUint32	iStartIndexX;                               /**< Contains the Start X Index of rectangle on which metrice will apply. */
    TUint32	iStartIndexY;                               /**< Contains the Start Y Index of rectangle on which metrice will apply. */
	TUint32 iPixelsToGrabX;                             /**< Contains the Number of pixels to grab starting from iStartIndexX in X direction. */
	TUint32 iPixelsToGrabY;                             /**< Contains the Number of pixels to grab starting from iStartIndexX in Y direction. */

    }TAlgoImageTestCoordinates;

/** 
 * An struct TAlgoMetricParams.
 * Contains the Algorithm Metric Parameters in Algorithm Library. 
 */
typedef struct 
    {
    TAlgoType iType;                                    /**< Contains the Algorithm Metric type. */
    TAlgoMetricErrorTolerance iErrorTolerance;          /**< Contains the Algorithm Metric tolerance. */
    TAlgoMetricRange iMetricRange;                      /**< Contains the Algorithm Metric Range. */
    TAlgoImageTestCoordinates iTestCoordinates;         /**< Contains the Image Coordinates under consideration. */
    TAny* iParams;                                      /**< Contains the Algorithm Metric extra params. */

    }TAlgoMetricParams;

/** 
 * An struct TAlgoImageGain.
 * Contains the Algorithm Image Gains for different Channels in Image. 
 */
typedef struct
    {
    TReal64 iGainR_Disable;  /**< Contains the Gain for R Channel for disabled image. */
    TReal64 iGainG_Disable;  /**< Contains the Gain for G Channel for disabled image. */
    TReal64 iGainB_Disable;  /**< Contains the Gain for B Channel for disabled image. */
    TReal64 iGainR_Enable;   /**< Contains the Gain for R Channel for enabled image. */
    TReal64 iGainG_Enable;   /**< Contains the Gain for R Channel for enabled image. */
    TReal64 iGainB_Enable;   /**< Contains the Gain for R Channel for enabled image. */

    }TAlgoImagesGain;

/** 
 * An struct TAlgoParamZoomFactor.
 * Contains the zoom param. 
 */
typedef struct
    {
    TReal64 iZoomFactor;  /**< Contains the zoom factor. */
    }TAlgoParamZoomFactor;

/** 
 * An struct TAlgoParamGamma.
 * Contains the gamma param. 
 */
typedef struct
    {
    TUint32 iPrGamma_1;  /**< Contains the gamma param 1. */
    TUint32 iPrGamma_2;  /**< Contains the gamma param 2. */
    }TAlgoParamGamma;

/** 
 * An enum TAlgoImageContent.
 * type of content of the image. 
 */
typedef enum
    {
    EImageContentUnknown = 0,			/**< enum value EImageContentUnknown	-   For init. */
    EImageContentNormal,				/**< enum value EImageContentNormal. */
    EImageContentColorBar,				/**< enum value EImageContentColorBar. */
    EImageContentSolidBarWhite,			/**< enum value EImageContentSolidBarWhite. */
    EImageContentSolidBarBlack,			/**< enum value EImageContentSolidBarBlack. */
    EImageContentSolidBarRed,			/**< enum value EImageContentSolidBarRed. */
    EImageContentSolidBarGreen,			/**< enum value EImageContentSolidBarGreen. */
    EImageContentSolidBarBlue,			/**< enum value EImageContentSolidBarBlue. */
    EImageContentSolidBarYellow,		/**< enum value EImageContentSolidBarYellow. */
    EImageContentSolidBarCyan,			/**< enum value EImageContentSolidBarCyan. */
    EImageContentSolidBarMagenta,		/**< enum value EImageContentSolidBarMagenta. */
    EImageContentSolidBarRGBspecified,	/**< enum value EImageContentSolidBarRGBspecified. */
    EImageContentSolidBarRorGorBorBorW,	/**< enum value EImageContentSolidBarRorGorBorBorW. */
    EImageContentMaxNb,					/**< enum value EImageContent			 -   Number of values for this enum */
    }TAlgoImageContent;

/** 
 * An struct TAlgoParamImageContent.
 * Contains the ImageContent param. 
 */
typedef struct
    {
	TAlgoImageContent iContentType_1;
    TUint32 iR_1;  /**< Contains param R. */
    TUint32 iG_1;  /**< Contains param G. */
    TUint32 iB_1;  /**< Contains param B. */
    TUint32 tolerance_1;  /**< tolerance. */
 	TAlgoImageContent iContentType_2;
    TUint32 iR_2;  /**< Contains param R. */
    TUint32 iG_2;  /**< Contains param G. */
    TUint32 iB_2;  /**< Contains param B. */
    TUint32 tolerance_2;  /**< tolerance. */
    TUint32 tolerance_R;  /**< tolerance. */
    TUint32 tolerance_G;  /**< tolerance. */
    TUint32 tolerance_B;  /**< tolerance. */
   }TAlgoParamImageContent;

/** 
 * An struct TAlgoParamBrightness.
 * Contains the Brightness param. 
 */
typedef struct
    {
    TUint32 iPrBrightness_1;  /**< Contains the Brightness param 1. */
    TUint32 iPrBrightness_2;  /**< Contains the Brightness param 2. */
    }TAlgoParamBrightness;
/** 
 * An struct TAlgoParamSharpness.
 * Contains the Sharpness param. 
 */
typedef struct
    {
    TUint32 iPrSharpness_1;  /**< Contains the Sharpness param 1. */
    TUint32 iPrSharpness_2;  /**< Contains the Sharpness param 2. */
    }TAlgoParamSharpness;
/** 
 * An struct TAlgoParamContrast.
 * Contains the Contrast param. 
 */
typedef struct
    {
    TInt32 iPrContrast_1;  /**< Contains the Contrast param 1. */
    TInt32 iPrContrast_2;  /**< Contains the Contrast param 2. */
    }TAlgoParamContrast;

/** 
 * An struct TAlgoParamSaturation.
 * Contains the Saturation param. 
 */
typedef struct
    {
    TUint32 iPrSaturation_1;  /**< Contains the Saturation param 1. */
    TUint32 iPrSaturation_2;  /**< Contains the Saturation param 2. */
    }TAlgoParamSaturation;

/** 
 * An enum TAlgoExpoValue.
 * type of the exposure value of the image. 
 */
typedef enum
    {
    EExpoValueUnknown = 0,			/**< enum value EExpoValue Unknown	-   For init. */
    EExpoValue_Off,				    /**< enum value EExpoValue_Off. */
    EExpoValue_Metering,			/**< enum value EExpoValue_Metering. */
    EExpoValue_EVCompensation,  	/**< enum value EExpoValue_EVCompensation. */
    EExpoValue_ApertureFNumber,	    /**< enum value EExpoValue_ApertureFNumber. */
	EExpoValue_ApertureAuto,		/**< enum value EExpoValue_ApertureAuto. */
    EExpoValue_ShutterSpeedMsec,    /**< enum value EExpoValue_ShutterSpeedMsec. */
	EExpoValue_ShutterSpeedAuto,	/**< enum value EExpoValue_ShutterSpeedAuto. */
    EExpoValue_Senstivity,			/**< enum value EExpoValue_Senstivity. */
	EExpoValue_SenstivityAuto,		/**< enum value EExpoValue_SenstivityAuto. */
    EExpoValueMaxNb,				/**< enum value EExpoValueMaxNb			 -   Number of values for this enum */
    }TAlgoExpoValueType;

/** 
 * An struct TAlgoParamCenterFieldOfView.
 * Enables to check that pantilt.
 */
typedef struct
    {
	TInt32 iX;				/**< Center pos X. */
    TInt32 iY;				/**< Center pos Y. */
	TUint32 iTolerance;
    }TAlgoParamCenterFieldOfView;	

/** 
 * An struct TAlgoParamLuminanceMeanValue.
 * Enables to check that MeanLuminanceValue division is in a certain range between MinFactor and MaxFactor.
 * Min or Max test can be discarded by programming 0 
 */
typedef struct
    {
	TUint8 iIsFirstImageDarker;          /**< iIsFirstImageDarker. */
	TUint32 iMinFactor;							  /**< Contains the Minimun Enable/Disable value for test to pass , if 0 , test criteria discarded*/
	TUint32 iMaxFactor;							  /**< Contains the Maximum Enable/Disable value for test to pass , if 0 , test criteria discarded*/
	TUint32 iTolerance;
    }TAlgoParamLuminanceMeanValue;	
	
/** 
 * An struct TAlgoParamExposureValue.
 * Contains the ExposureValue param. 
 */
typedef struct
    {
	TAlgoExpoValueType iExpoValueType;		/**< Contains the type of Exposure Value. */
    TReal64 iExposureValue_Disable;          /**< Contains the ExposureValue param 1. */
    TReal64 iExposureValue_Enable;           /**< Contains the ExposureValue param 2. */
	TUint32 iTolerance;
    }TAlgoParamExposureValue;

/** 
 * An enum TAlgoImageContent.
 * type of content of the image. 
 */
typedef enum
    {
    EExpoPresetUnknown = 0,			/**< enum value EExpoPresetUnknown	-   For init. */
    EExpoPreset_Off,				/**< enum value EExpoPreset_Off. */
    EExpoPreset_Auto,				/**< enum value EExpoPreset_Auto. */
    EExpoPreset_Night,				/**< enum value EExpoPreset_Night. */
    EExpoPreset_BackLight,			/**< enum value EExpoPreset_BackLight. */
    EExpoPreset_SpotLight,			/**< enum value EExpoPreset_SpotLight. */
    EExpoPreset_Sports,				/**< enum value EExpoPreset_Sports. */
    EExpoPreset_Snow,				/**< enum value EExpoPreset_Snow. */
    EExpoPreset_Beach,				/**< enum value EExpoPreset_Beach. */
    EExpoPresetMaxNb,				/**< enum value EExpoPresetMaxNb			 -   Number of values for this enum */
    }TAlgoExpoPreset;

/** 
 * An struct TAlgoParamImageContent.
 * Contains the ImageContent param. 
 */
typedef struct
    {
	TAlgoExpoPreset iExpoPreset_1;
 	TAlgoExpoPreset iExpoPreset_2;
	TUint32			iTolerance;
  }TAlgoParamExposurePreset;


/** 
 * An struct TAlgoMatrix.
 * Contains the Algorithm Matrix. 
 */
typedef struct
    {
	TReal32 iMatrix[N][N];	    /**< Contains the matrix. */
	
    }TAlgoMatrix;


/** 
 * White Balance control type 
 *
 * STRUCT MEMBERS:
 *  
 *  
 */
typedef enum  {
    EWhiteBalControlOff = 0,
    EWhiteBalControlAuto,
    EWhiteBalControlSunLight,
    EWhiteBalControlCloudy,
    EWhiteBalControlShade,
    EWhiteBalControlTungsten,
    EWhiteBalControlFluorescent,
    EWhiteBalControlIncandescent,
    EWhiteBalControlFlash,
    EWhiteBalControlHorizon
} TAlgoWBType;


/** 
 * An enum TAlgoFilterType.
 * Supported Algorithm Filters in Algorithm Library. 
 */
typedef enum
    {
    EFiltertUnknown = 0,			/**< enum value EFiltertUnknown				-   For init. */
    EFilterConvolution,             /**< enum value EFilterConvolution			-   For Convolution Filter. */
    EFilterDilate,                  /**< enum value EFilterDilate				-   For Dilate Filter. */
    EFilterErosion,                 /**< enum value EFilterErosion				-   For Erosion Filter. */
    EFilterBrightness,              /**< enum value EFilterBrightness			-   For Brightness Filter. */
	EFilterGamma,                   /**< enum value EFilterGamma				-   For Gamma Filter. */
	EFilterNegative,                /**< enum value EFilterNegative				-   For Negative Filter. */
	EFilterRotation,                /**< enum value EFilterRotation				-   For Rotation Filter. */
	EFilterGrayScaleConversion,     /**< enum value EFilterGrayScaleConversion  -   For GrayScaleConversion Filter. */
	EFilterResize,                  /**< enum value EFilterResize				-   For Resize Filter. */
	EFilterExposure,                /**< enum value EFilterExposure				-   For Exposure Filter. */
	EFilterWhiteBalance,            /**< enum value EFilterWhiteBalance			-   For WhiteBalance Filter. */
	EFilterAutoFocus1,              /**< enum value EFilterAutoFocus			-   For AutoFocus Filter. */
	EFilterContrast,                /**< enum value EFilterContrast				-   For Contrast Filter. */
    EFilterEdgeDetection,           /**< enum value EFilterEdgeDetection		-   For Edge Detection Filter. */
    EFilterApplyColorTone,          /**< enum value EFilterApplyColorTone		-   For Color tone 3*3 Filter. */
    EFilterFindSepiaColor,          /**< enum value EFindSepiaColor				-   For Sepia Filter. */
    EFilterMaxNb, 					/**< enum value EFilterMaxNb				-   Number of values for this enum */

    }TAlgoFilterType;

/** 
 * An enum TConvFilterType.
 * Supported Convolution Filters in Algorithm Library. 
 */
typedef enum
    {
    EConvFiltertUnknown = 0,	/**< enum value EConvFiltertUnknown			-   For init. */
	EConvFilterBlur,            /**< enum value EConvFilterBlur				-   For Convolution Blur Filter. */
	EConvFilterSharpness,       /**< enum value EConvFilterSharpness		-   For Convolution Sharpness Filter. */
	EConvFilterEmboss,          /**< enum value EConvFilterEmboss			-   For Convolution Emboss Filter. */
	EConvFilterContrast,        /**< enum value EConvFilterContrast			-   For Convolution Contrast Filter. */
	EConvFilterEdgeDetection,   /**< enum value EConvFilterEdgeDetection    -   For Convolution Edge Detection Filter. */
    EConvFilterMaxNb, 			/**< enum value EConvFilterMaxNb			-   Number of values for this enum */

    }TConvFilterType;

/** 
 * An struct TAlgoConvFilterParams.
 * Contains the Convolution Filter Parameters in Algorithm Library. 
 */
typedef struct
    {
	TConvFilterType      	iConvFilterType;	/**< Contains the Convolution Filter type. */	
	TReal64					iMatrix[N][N];	    /**< Contains the Convolution matrix. */
	//TReal32					iParam;	/* Param 1: Brightness / Contrast / Gamma strength */	
		
    }TAlgoConvFilterParams;

/** 
 * An struct TAlgoGenericFilterParams.
 * Contains the Generic Filter Parameters in Algorithm Library. 
 */
typedef struct 
    {
    TAlgoImageParams iImageParams;      /**< Contains the Image Parameters. */
	TUint32	iStartIndexX;               /**< Contains the Start X Index of rectange on which filter will apply. */
    TUint32	iStartIndexY;               /**< Contains the Start Y Index of rectange on which filter will apply. */
	TUint32 iPixelsToGrabX;             /**< Contains the Number of pixels to grab starting from iStartIndexX in X direction. */
	TUint32 iPixelsToGrabY;             /**< Contains the Number of pixels to grab starting from iStartIndexX in Y direction. */
	TAlgoFilterType	iFilterType;        /**< Contains the IType of Filter to be applied. */
    TReal32 iParam;                     /**< Contains the Param 1: Brightness / Contrast / Gamma strength */

    }TAlgoGenericFilterParams;

#pragma pack(2) /*2 byte packing */ 
/** 
 * An struct TAlgoBMPHeader.
 * Contains the BMP Header. 
 */
typedef struct 
    { 
    TUint16 iType; 
    TUint32 iSize; 
    TUint16 iReserved1, iReserved2; 
    TUint32 iOffset; 

    }TAlgoBMPHeader; 

#pragma pack() /* Default packing */ 
/** 
 * An struct TAlgoBMPHeader.
 * Contains the BMP Header. 
 */
typedef struct 
    { 
    TUint32 iSize; 
    TInt32  iWidth, iHeight; 
    TUint16 iPlanes; //vineet 
    TUint16 iBits; 
    TUint32 iCompression; 
    TUint32 iImageSize; 
    TInt32  iXResolution, iYResolution; 
    TUint32 iNColors; 
    TUint32 iImportantColors; 

    }TAlgoBMPInfoheader; 


/** 
 * Test struct init.
 */
typedef struct 
    { 
	TReal32 iErrorToleranceR;
    TReal32 iErrorToleranceG;
    TReal32 iErrorToleranceB;
    TReal64 iGainRdisable;
    TReal64 iGainGdisable;
    TReal64 iGainBdisable;
    TReal64 iGainRenable;
    TReal64 iGainGenable;
    TReal64 iGainBenable;
    }TAlgoTestInit; 


typedef struct 
    {
	TUint8 iMetricName[20];
	TUint8 iInputfilename[256];
	TUint8 iOutputfilename[256];
	TAlgoImageTestCoordinates iTestCoordinates;
    TAlgoMetricErrorTolerance iErrorTolerance;

    }TAlgoTestFileParams;

#endif	//__INCLUDE_TYPEDEFS_H__

