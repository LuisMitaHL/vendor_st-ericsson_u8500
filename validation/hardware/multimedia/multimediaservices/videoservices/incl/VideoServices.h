/*****************************************************************************/
/**
*  Copyright (C) ST-Ericsson 2009
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Propose Video function for tests (Display (Main, Sub, TV-OUT), Camera)
* \author  ST-Ericsson
*/
/*****************************************************************************/

/*!
	\file VideoServices.h

*/


/*! \defgroup Display_services Access to Display services 
	\par	
		This part describes the C interface to provide access to main Video services for display
		- Initialise displays
		- Manage power ON/OFF of TV-OUT displays		
		- display a picture
		- Set PWM backlight value
		- Get ambiant light sensor value
*/

#ifndef _VIDSERV_H
#define _VIDSERV_H


/*------------------------------------------------------*/
/*! 	\ingroup Display_services
	\def VIDSERV_NO_ERROR
	\brief  code error returned meaning that the service has been executed without error
*/

#define  VIDSERV_NO_ERROR  		((unsigned int) 0)

/*------------------------------------------------------*/
/*! 	\ingroup Display_services
	\def VIDSERV_FAILED
	\brief  code error returned meaning that an error occurs during service execution
*/
#define  VIDSERV_FAILED    		((unsigned int) 1)

/*------------------------------------------------------*/
/*! 	\ingroup Display_services
	\def VIDSERV_DEVICE_ALREADY_USED
	\brief  code error returned meaning that the device is already in use
*/
#define  VIDSERV_DEVICE_ALREADY_USED ((unsigned int) 2)

/*------------------------------------------------------*/
/*! 	\ingroup Display_services
	\def VIDSERV_INVALID_PARAMETER
	\brief  code error returned meaning that a parameter is not valid
*/
#define  VIDSERV_INVALID_PARAMETER   ((unsigned int) 3)

/*------------------------------------------------------*/
/*! 	\ingroup Display_services
	\def VIDSERV_NOT_AVAILABLE
	\brief  code error returned meaning that service is not available/not yet implemented
*/
#define  VIDSERV_NOT_AVAILABLE   ((unsigned int) 4)


/*! 	\ingroup Display_services
	\enum e_VidServ_DisplayId_t
	\brief This control provides the selection of display
*/
typedef enum {
	DISPLAY_MAIN = 0,			/**< Main Display*/
	DISPLAY_SUB, 				/**< Sub Display*/
	DISPLAY_TV_OUT_CVBS_4500,	/**< Analog TV-OUT via AB8500*/
	DISPLAY_TV_OUT_CVBS_5810,	/**< Analog TV-OUT via AV8100*/
	DISPLAY_TV_OUT_HDMI		/**< Digital HDMI TV-OUT via AV8100*/
    ,DISPLAY_ID_LOC_FORCE_32BITS = 0x7FFFFFFF		/*Do not use ..*/
}e_VidServ_DisplayId_t;

/* keep interface but used AB8500 instead of 4500 and AV8100 instead of 5810 for videoservices */
#define DISPLAY_TV_OUT_CVBS_AB8500 DISPLAY_TV_OUT_CVBS_4500
#define DISPLAY_TV_OUT_CVBS_AV8100 DISPLAY_TV_OUT_CVBS_5810

/*! 	\ingroup Display_services
	\enum e_VidServ_PowerMode_t
	\brief This control provides the selection of power mode		   
*/
typedef enum {
	POWER_OFF=0,	/**< Power is OFF*/
	POWER_ON, 		/**< Power is ON*/
	LOW_POWER		/**< Power is low power*/
    ,POWER_MODE_LOC_FORCE_32BITS = 0x7FFFFFFF		/*Do not use ..*/
}e_VidServ_PowerMode_t;

/*! 	\ingroup Display_services
	\enum e_VidServ_PictureFormat_t
	\brief This control provides the selection picture format		   
*/
typedef enum {
  ARGB8888=0,
	RGB888, /**< RGB888 picture format */
	PNG, /**< PNG picture format */
	JPEG, /**< JPEG picture format */
    RAW, /**< Raw bayer picture format */
    RGB565 
    ,PICTURE_FORMAT_LOC_FORCE_32BITS = 0x7FFFFFFF		/*Do not use ..*/
}e_VidServ_PictureFormat_t;

/*! 	\ingroup Display_services
	\enum e_VidServ_LoopMode_t
	\brief This control provides the selection of the loop mode of the image to display		   
*/
typedef enum {
	NO_LOOP=0, /**< Image is displayed one time to the display */
	FIXED_RATE, /**< Image is displayed to the display with a fixed rate*/
	MAX_SPEED /**< Image is displayed as soon as previous display is done, to be checked if possible*/
    ,TOGGLE_LOOP /**< toggle between 2 images in loop */
    ,LOOP_MODE_LOC_FORCE_32BITS = 0x7FFFFFFF		/*Do not use ..*/
}e_VidServ_LoopMode_t;

/*! 	\ingroup Display_services
	\enum e_VidServ_Rotation_t
	\brief This control provides the selection of the rotation of the image to display		   
*/
typedef enum {
	ROTATE_UR=0, /**< 0° Rotation */
	ROTATE_CW, /**< 90° Rotation */
	ROTATE_UD, /**< 180° Rotation */
  ROTATE_CCW /**< 270° Rotation */
  ,ROTATE_LOC_FORCE_32BITS = 0x7FFFFFFF		/*Do not use ..*/
}e_VidServ_Rotation_t;

/*! 	\ingroup Display_services
	\enum e_VidServ_ScanMode_t
	\brief This control provides the selection of the display scan mode
*/
typedef enum {
	NONE=0, /**< To be used for Non-TV-OUT display (MAIN and SUB) */
	PROGRESSIVE, /**< Progressive scan mode */
	INTERLACED /**< Interlaced scan mode */
    ,SCAN_MODE_LOC_FORCE_32BITS = 0x7FFFFFFF		/*Do not use ..*/
}e_VidServ_ScanMode_t;

/*! 	\ingroup Display_services
	\enum e_VidServ_AVConnectorSource_t
	\brief This control provides the selection of the source to connect to AudioVideo connector
*/
typedef enum {
	VIDEO_SOURCE=0, /**< To be used to select Audio source to AudioVideo connector */
	AUDIO_SOURCE /**< To be used to select Video source to AudioVideo connector */
    ,AVSOURCE_LOC_FORCE_32BITS = 0x7FFFFFFF		/*Do not use ..*/
}e_VidServ_AVConnectorSource_t;

/*! 	\ingroup Display_services
	\enum e_VidServ_DisplayAV8100InputVideoFormat_t
	\brief This control provides the selection of the input video format of the AV8100 chip
*/
typedef enum{
	VID_AV8100_INPUT_PIX_RGB565, /**< To be used to select RGB565 as input format */
	VID_AV8100_INPUT_PIX_RGB666, /**< To be used to select RGB666 as input format */
	VID_AV8100_INPUT_PIX_RGB666P, /**< To be used to select RGB666P as input format */
	VID_AV8100_INPUT_PIX_RGB888, /**< To be used to select RGB888 as input format */
	VID_AV8100_INPUT_PIX_YCBCR422 /**< To be used to select YCBCR422 as input format */
    ,VID_AV8100INPUTFORMAT_LOC_FORCE_32BITS = 0x7FFFFFFF		/*Do not use ..*/
} e_VidServ_DisplayAV8100InputVideoFormat_t;

/*! 	\ingroup Display_services
	\struct t_VidServ_DisplayResolution
	\brief This structure provides the display resolution		   
*/
typedef struct {
	uint32_t Width; /**< width of the display resolution */
	uint32_t Height; /**< height of the display resolution */
	uint32_t Depth; /**< depth of the display */
  uint32_t Frequency; /**< frequency of the display, Applicable for Set properties */
	e_VidServ_ScanMode_t ScanMode; /**< refer to \ref e_VidServ_ScanMode_t */
}t_VidServ_DisplayResolution;

/*! 	\ingroup Display_services
	\struct t_VidServ_PictureResolution
	\brief This structure provides the resolution of the image to display  
*/
typedef struct {
	uint32_t Width; /**< width of the image to display */
	uint32_t Height; /**< height of the image to display */
}t_VidServ_PictureResolution;


/*! 	\ingroup Display_services
	\struct t_VidServ_ImageProperties
	\brief This structure provides the properties of the image to display  
	\note
		- BufferAddressOrFilePath buffer should be allocated/de-allocated by user
*/
typedef struct {
	uint32_t FromFile;	/**< FromFile=0 if a buffer with image data is provided\n
								 FromFile=1 if image has to read from a file*/
	uint8_t * BufferAddressOrFilePath; /**< if FromFile=0 Then pointer to buffer of image data\n
											      if FromFile=1 Then pointer to file path (Null Terminated)*/
	uint32_t BufferSize;	/**< if FromFile=0, size of the buffer to display \n
								 else Not Applicable*/
	e_VidServ_PictureFormat_t InputPictureFormat; /**< Format of the image to display, refer to \ref e_VidServ_PictureFormat_t */
	t_VidServ_PictureResolution InputPictureResolution; /**< Resolution of the picture to display, Applicable for RGB888 only */
} t_VidServ_ImageProperties;

/*! \ingroup Display_services
	\fn uint32_t VidServ_DisplayInit(e_VidServ_DisplayId_t DisplayId)
	\brief Service allowing to initialize a display
	\param [in] DisplayId Display to initialize, refer to \ref e_VidServ_DisplayId_t
	\return 	
		0 if no error
	\warning
        AB8500 CVBS and AV8100 CVBS output are exclusives, we can not itiliaze/use both
*/
uint32_t VidServ_DisplayInit(e_VidServ_DisplayId_t DisplayId);

/*! \ingroup Display_services
	\fn uint32_t VidServ_DisplayDeInit(e_VidServ_DisplayId_t DisplayId);
	\brief Service allowing to de-initialize a display
	\param [in] DisplayId Display to de-initialize, refer to \ref e_VidServ_DisplayId_t
	\return 	
		0 if no error
*/
uint32_t VidServ_DisplayDeInit(e_VidServ_DisplayId_t DisplayId);

/*! \ingroup Display_services
	\fn uint32_t VidServ_DisplaySetPowerMode(e_VidServ_DisplayId_t DisplayId, e_VidServ_PowerMode_t PowerMode);
	\brief Service allowing to set the power mode of a display
	\param [in] DisplayId  refer to \ref e_VidServ_DisplayId_t
	\param [in] PowerMode refer to \ref e_VidServ_PowerMode_t	
	\return 	
		0 if no error
	\warning
		- Step1: only applicable for TV-OUT displays (CVBS or HDMI)
		- Low Power to be checked if applicable for these displays
*/
uint32_t VidServ_DisplaySetPowerMode(e_VidServ_DisplayId_t DisplayId, e_VidServ_PowerMode_t PowerMode);

/*! \ingroup Display_services
	\fn uint32_t VidServ_DisplaySetProperties(e_VidServ_DisplayId_t DisplayId, t_VidServ_DisplayResolution * DisplayResolution);
	\brief Service allowing to set the properties of a display
	\param [in] DisplayId refer to \ref e_VidServ_DisplayId_t
	\param [in] DisplayResolution refer to \ref t_VidServ_DisplayResolution
	\return 	
		0 if no error
	\note
		- DisplayResolution structure should be allocated/de-allocated by user			
	\warning
		- Only applicable for TV-OUT displays (CVBS or HDMI)
*/	   
uint32_t VidServ_DisplaySetProperties(e_VidServ_DisplayId_t DisplayId, t_VidServ_DisplayResolution * DisplayResolution);

/*! \ingroup Display_services
	\fn uint32_t VidServ_DisplayGetProperties(e_VidServ_DisplayId_t DisplayId, t_VidServ_DisplayResolution * DisplayResolution);
	\brief Service allowing to get the properties of a display
	\param [in] DisplayId refer to \ref e_VidServ_DisplayId_t
	\param [out] DisplayResolution pointer of structure where the resolution is returned, refer to \ref t_VidServ_DisplayResolution
	\return 	
		0 if no error
	\note
		- DisplayResolution structure should be allocated/de-allocated by user
*/	   
uint32_t VidServ_DisplayGetProperties(e_VidServ_DisplayId_t DisplayId, t_VidServ_DisplayResolution * DisplayResolution);


/*! \ingroup Display_services
	\fn uint32_t VidServ_DisplayPicture(e_VidServ_DisplayId_t DisplayId, t_VidServ_ImageProperties * ImageProperties, e_VidServ_LoopMode_t LoopMode, uint32_t ImageDuration, t_VidServ_ImageProperties * Image2Properties, uint32_t Image2Duration);
	\brief Service allowing to display a picture on a display
	\param [in] DisplayId Display where the picture is displayed, refer to \ref e_VidServ_DisplayId_t
	\param [in] ImageProperties pointer to structure where are properties of the image to display, refer to \ref t_VidServ_ImageProperties
	\param [in] LoopMode refer to \ref e_VidServ_LoopMode_t
	\param [in] ImageDuration time in ms of the image1, applicable if LoopMode=FIXED_RATE or TOGGLE_LOOP
	\param [in] Image2Properties pointer to structure where are properties of the image2 to display, refer to \ref t_VidServ_ImageProperties, applicable if LoopMode=TOGGLE_LOOP
	\param [in] Image2Duration time in ms of the image2, applicable if LoopMode=TOGGLE_LOOP
	\return 	
		0 if no error
	\note
		- Use VidServ_StopDisplayPictureLoop() to stop the loop when a loop has be selected\n
		- ImageProperties structure should be allocated/de-allocated by user	
	\warning
		- Used display should have been initialised before
		- Restrictions for step1:\n
				- ImageProperties.FromFile should be 1 (or could be 0 for RGB888 buffer)\n
				- ImageProperties.InputFormat = PNG, RGB888\n
				- ImageProperties.InputResolution = Display Resolution (no upscaling/downscaling) for RGB888\n
*/	 
uint32_t VidServ_DisplayPicture(e_VidServ_DisplayId_t DisplayId, t_VidServ_ImageProperties * ImageProperties, e_VidServ_LoopMode_t LoopMode, uint32_t ImageDuration, e_VidServ_Rotation_t Image1Rotation, t_VidServ_ImageProperties * Image2Properties, uint32_t Image2Duration, e_VidServ_Rotation_t Image2Rotation);

/*! \ingroup Display_services
	\fn uint32_t VidServ_StopDisplayPictureLoop(e_VidServ_DisplayId_t DisplayId);
	\brief Service allowing to stop a image display loop
	\param [in] DisplayId Display where the picture loop should be stopped
	\return 	
		0 if no error
*/
uint32_t VidServ_StopDisplayPictureLoop(e_VidServ_DisplayId_t DisplayId);

/*! \ingroup Display_services
	\fn uint32_t VidServ_GetAlsValue(uint32_t* AlsValue);
	\brief Service allowing to get Ambiant Light Sensor value
	\param [in] AlsValue ALS value
	\return 	
		0 if no error	
	\note
		- AlsValue pointer should be allocated/de-allocated by user			
*/
uint32_t VidServ_GetAlsValue(uint32_t* AlsValue);

/*! \ingroup Display_services
	\fn uint32_t VidServ_BacklightSetPWMValue(e_VidServ_DisplayId_t DisplayId, uint32_t PwmValue);
	\brief Service allowing to set PWM backlight value of a display
	\param [in] DisplayId Display where to apply the PWM backlight value , refer to \ref e_VidServ_DisplayId_t
	\param [in] PwmValue PWM value to apply (minimum 0, maximum 1024)
	\return 	
		0 if no error
	\warning
		- Only applicable for Main or sub displays (not for TV-OUT)
	\note
		- PwmValue has a minimum of 0 and maximum of 1024	
*/
uint32_t VidServ_BacklightSetPWMValue(e_VidServ_DisplayId_t DisplayId, uint32_t PwmValue);

/*! \ingroup Display_services
	\fn uint32_t VidServ_BacklightGetPWMValue(e_VidServ_DisplayId_t DisplayId, uint32_t *PwmValue);
	\brief Service allowing to get PWM backlight value of a display
	\param [in] DisplayId Display where to get the PWM backlight value , refer to \ref e_VidServ_DisplayId_t
	\param [out] PwmValue PWM value (minimum 0, maximum 1024)
	\return 	
		0 if no error
	\warning
		- Only applicable for Main or sub displays (not for TV-OUT)			
*/
uint32_t VidServ_BacklightGetPWMValue(e_VidServ_DisplayId_t DisplayId, uint32_t *PwmValue);


/*! \ingroup Display_services
	\fn uint32_t VidServ_DisplayIsPlugged(e_VidServ_DisplayId_t DisplayId, uint32_t *IsPlugged);
	\brief Service allowing to know if a plug is detected on TV-OUT output
	\param [in] DisplayId refer to \ref e_VidServ_DisplayId_t
	\param [out] IsPlugged 1 if plug is detected, 0 if display plug is not detected (or unplug)
	\return 	
		0 if no error
	\warning
		- Only applicable for TV-OUT displays (CVBS or HDMI)
*/	   
uint32_t VidServ_DisplayIsPlugged(e_VidServ_DisplayId_t DisplayId, uint32_t *IsPlugged);

/*! \ingroup Display_services
	\fn uint32_t VidServ_AVConnectorSourceSelection(e_VidServ_AVConnectorSource_t SourceId);
	\brief Service allowing to select the source to connect to AudioVideo connector
	\param [in] SourceId refer to \ref e_VidServ_AVConnectorSource_t
	\return 	
		0 if no error
*/
uint32_t VidServ_AVConnectorSourceSelection(e_VidServ_AVConnectorSource_t SourceId);

/*! \ingroup Display_services
	\fn uint32_t VidServ_DisplayAV8100InputVideoFormat(e_VidServ_DisplayAV8100InputVideoFormat_t AV8100InputVideoFormat);
	\brief Service allowing to change the input video format of the AV8100 chip (data transferred on DSI Link 2)
	\param [in] AV8100InputVideoFormat refer to \ref e_VidServ_DisplayAV8100InputVideoFormat_t
	\return 	
		0 if no error
	\warning
		- Only applicable for AV8100 HDMI and CVBS displays, change will be apply for both displays
*/
uint32_t VidServ_DisplayAV8100InputVideoFormat(e_VidServ_DisplayAV8100InputVideoFormat_t AV8100InputVideoFormat);

#endif /* _VIDSERV_H */
