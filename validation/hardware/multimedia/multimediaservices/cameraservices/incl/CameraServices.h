/*****************************************************************************/
/**
*  Copyright (C) ST-Ericsson 2009
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Propose IQT function for tests 
* \author  ST-Ericsson
*/
/*****************************************************************************/



#ifndef _CAMERASERV_H
#define _CAMERASERV_H


/*------------------------------------------------------*/
/*! 	\ingroup Display_services
	\def CAMERASERV_NO_ERROR
	\brief  code error returned meaning that the service has been executed without error
*/

#define  CAMERASERV_NO_ERROR  		((unsigned int) 0)

/*------------------------------------------------------*/
/*! 	\ingroup Display_services
	\def CAMERASERV_FAILED
	\brief  code error returned meaning that an error occurs during service execution
*/
#define  CAMERASERV_FAILED    		((unsigned int) 1)

/*------------------------------------------------------*/
/*! 	\ingroup Display_services
	\def CAMERASERV_DEVICE_ALREADY_USED
	\brief  code error returned meaning that the device is already in use
*/
#define  CAMERASERV_DEVICE_ALREADY_USED ((unsigned int) 2)

/*------------------------------------------------------*/
/*! 	\ingroup Display_services
	\def CAMERASERV_INVALID_PARAMETER
	\brief  code error returned meaning that a parameter is not valid
*/
#define  CAMERASERV_INVALID_PARAMETER   ((unsigned int) 3)

/*------------------------------------------------------*/
/*! 	\ingroup Display_services
	\def CAMERASERV_NOT_AVAILABLE
	\brief  code error returned meaning that service is not available/not yet implemented
*/
#define  CAMERASERV_NOT_AVAILABLE   ((unsigned int) 4)


uint32_t CameraServ_InitPipe(char *input_pipe,char *output_pipe,char *output_MMTE_task_pipe,int *fd_ip,int *fd_op,int *fd_op_mmte_task,int pts); 
void CameraServ_WaitAcknowledge(int fd,char *trigger,int timeout);

uint32_t CameraServ_SetMmteAlias(int fd_in,int fd_out,char *mmte_command,int timeout);
uint32_t CameraServ_InitMmte(int fd_in,int fd_out,char *trigger,int timeout);
uint32_t CameraServ_Init2Mmte(int fd_in,int fd_out,char *trigger,int timeout);

uint32_t CameraServ_StartPreview(int fd_in,int fd_out,char *trigger,int timeout);
uint32_t CameraServ_StartIdle(int fd_in,int fd_out,char *trigger,int timeout);
uint32_t CameraServ_StartPause(int fd_in,int fd_out,char *trigger,int timeout);
uint32_t CameraServ_StartResume(int fd_in,int fd_out,char *trigger,int timeout);
uint32_t CameraServ_StartStillPictureCapture(int fd_in,int fd_out,char *trigger,int timeout);
uint32_t CameraServ_StartRecord(int fd_in,int fd_out,char *trigger,int timeout);
uint32_t CameraServ_StopRecord(int fd_in,int fd_out,char *trigger,int timeout);
uint32_t CameraServ_StartStillBurstPictureCapture(int fd_in,int fd_out,char *trigger,int timeout);
uint32_t CameraServ_StopStillBurstPictureCapture(int fd_in,int fd_out,char *trigger,int timeout);
uint32_t CameraServ_StopIdle(int fd_in,int fd_out,char *trigger,int timeout);
uint32_t CameraServ_StopPreview(int fd_in,int fd_out,char *trigger,int timeout);

uint32_t CameraServ_StartMmte(char *pipe_input,char *pipe_output,int fd_out,char *trigger,int timeout);
uint32_t CameraServ_StopMmte(int fd_in,int fd_out,char *trigger,int timeout);
uint32_t CameraServ_CloseFileDescriptors(void);


/********************************************************************************/
/*                             Camera Services                                  */
/********************************************************************************/


/*! 	\ingroup Camera_services
	\enum e_CameraServ_SensorId_t
	\brief This control provides the selection of the sensor
*/
typedef enum {
	SENSOR_PRIMARY=0,				/**< primary sensor*/
	SENSOR_SECONDARY 				/**< secondary sensor */
    ,SENSOR_ID_LOC_FORCE_32BITS = 0x7FFFFFFF		/*Do not use ..*/
}e_CameraServ_SensorId_t;

/*! 	\ingroup Camera_services
	\enum e_CameraServ_FlashMode_t
	\brief This control provides the selection of the flash mode
*/
typedef enum {
	FLASH_ON=0, /**< Strobe at every shot */
	FLASH_OFF, /**< Strobe off */
	FLASH_AUTO, /**< Strobe according to environment light */
	FLASH_RED_EYE_REDUCTION, /**< Pre-shot strobes */
	FLASH_TORCH /**< Flash is always on */
    ,FLASH_MODE_LOC_FORCE_32BITS = 0x7FFFFFFF		/*Do not use ..*/
}e_CameraServ_FlashMode_t;

/*! 	\ingroup Camera_services
	\enum e_CameraServ_FlashType_t
	\brief This control provides the selection of the type of flash
*/
typedef enum {
	XENON_FLASH=0, /**< select the xenon flash */
	LED_FLASH /**< select the Led flash */
    ,FLASH_TYPE_LOC_FORCE_32BITS = 0x7FFFFFFF		/*Do not use ..*/
}e_CameraServ_FlashType_t;

/*! 	\ingroup Camera_services
	\enum e_CameraServ_ZoomType_t
	\brief This control provides the selection of the type of zoom
*/
typedef enum {
	DIGITAL_ZOOM=0, /**< select the digital zoom */
	OPTICAL_ZOOM /**< select the optical zoom */
    ,ZOOM_TYPE_LOC_FORCE_32BITS = 0x7FFFFFFF		/*Do not use ..*/
}e_CameraServ_ZoomType_t;

/*! 	\ingroup Camera_services
	\enum e_CameraServ_FocusMode_t
	\brief This control provides the selection of the type of focus
*/
typedef enum {
	FOCUS_OFF=0, /**< Focus off */
	FOCUS_AUTO /**< Focus adjustments are being performed automatically and continuously by the component until a capture request is issued */
    ,FOCUS_MODE_LOC_FORCE_32BITS = 0x7FFFFFFF		/*Do not use ..*/
}e_CameraServ_FocusMode_t;
/*! 	\ingroup Camera_services
	\enum e_CameraServ_ShotMode_t
	\brief This control provides the selection of the mode of shot
*/
typedef enum {
	SINGLE_SHOT=0, /**< only one shot when take a picture */
	BURST_SHOT /**<  take picture with burst mode */
    ,SHOT_MODE_LOC_FORCE_32BITS = 0x7FFFFFFF		/*Do not use ..*/
}e_CameraServ_ShotMode_t;



/*! 	\ingroup Camera_services
	\enum e_CameraServ_bFrameLimited_t
	\brief This control provides the selection of the mode of burst
*/
typedef enum {
	FINITE_BURST=0, /**< only one shot when take a picture */
	INFINITE_BURST /**<  take picture with burst mode */
    ,BURST_MODE_LOC_FORCE_32BITS = 0x7FFFFFFF		/*Do not use ..*/
}e_CameraServ_bFrameLimited_t;

/*! 	\ingroup Camera_services
	\struct t_CameraServ_PreviewResolution
	\brief This structure provides the resolution of the preview  
*/
typedef struct {
	uint32_t Width; /**< width of the preview */
	uint32_t Height; /**< height of the preview */
}t_CameraServ_PreviewResolution;


/*! 	\ingroup Camera_services
	\struct t_CameraServ_FlashProperties
	\brief This structure provides the flash properties  
*/
typedef struct {
	e_CameraServ_FlashType_t FlashType; /**< type of flash to use, see \ref  e_CameraServ_FlashType_t */
	e_CameraServ_FlashMode_t FlashMode; /**< mode of flash to use, see \ref  e_CameraServ_FlashMode_t */
}t_CameraServ_FlashProperties;

/*! 	\ingroup Camera_services
	\struct t_CameraServ_ZoomProperties
	\brief This structure provides the zoom properties  
*/
typedef struct {
	e_CameraServ_ZoomType_t ZoomType; /**< type of zoom to use, see \ref  e_CameraServ_ZoomType_t */
	uint32_t ZoomValue; /**< value of zoom to use */
}t_CameraServ_ZoomProperties;


/*! 	\ingroup Camera_services
	\struct t_CameraServ_SensorProperties
	\brief This structure provides the sensor properties  
*/
typedef struct {
	uint32_t* FrameRate; /**< set the frame rate of the sensor (in frame per second) */
    t_CameraServ_FlashProperties* FlashProperties; /**< flash type and mode to apply, refer to \ref t_CameraServ_FlashProperties */
    e_CameraServ_FocusMode_t* FocusMode; /**< focus mode to apply, refer to \ref e_CameraServ_FocusMode_t */
}t_CameraServ_SensorProperties;


/*! 	\ingroup Camera_services
	\struct t_CameraServ_BurstProperties
	\brief This structure provides the burst properties when take a shot in burst mode
*/
typedef struct {
	e_CameraServ_bFrameLimited_t bFrameLimited; /**< set the burst mode in finite or infinite, refer to \ref  e_CameraServ_bFrameLimited_t */
    uint32_t nFrameLimited; /**< number of frame to take during finite burst shot */
}t_CameraServ_BurstProperties;



/*! \ingroup Camera_services
	\fn uint32_t CameraServ_CameraSensorInit(e_CameraServ_SensorId_t SensorId);
	\brief Service allowing to initialize a sensor
	\param [in] SensorId Sensor to initialize, refer to \ref e_CameraServ_SensorId_t
	\return 	
		0 if no error
*/
uint32_t CameraServ_CameraSensorInit(e_CameraServ_SensorId_t SensorId);

/*! \ingroup Camera_services
	\fn uint32_t CameraServ_CameraSetPreviewProp(e_CameraServ_DisplayId_t* DisplayId, t_CameraServ_PreviewResolution* PreviewResolution);
	\brief Service allowing to set the properties of the preview for camera use cases
	\param [in] DisplayId select to the display of the preview, refer to \ref e_CameraServ_DisplayId_t
    \param [in] PreviewResolution resolution of the preview, refer to \ref t_CameraServ_PreviewResolution
	\return 	
		0 if no error
	\note
        Parameters should be allocated/de-allocated by the user of the interface
		If the pointer on a setting  is Null (sensorID or settings in t_CameraServ_SensorProperties structure), this means that the corresponding setting is unchanged
*/
uint32_t CameraServ_CameraSetPreviewProp(uint32_t* DisplayId, t_CameraServ_PreviewResolution* PreviewResolution);

/*! \ingroup Camera_services
	\fn uint32_t CameraServ_CameraGetPreviewProp(e_CameraServ_DisplayId_t* DisplayId, t_CameraServ_PreviewResolution* PreviewResolution);
	\brief Service allowing to get the properties of the preview for camera use cases
	\param [out] DisplayId get display of the preview, refer to \ref e_CameraServ_DisplayId_t
    \param [out] PreviewResolution get the resolution of the preview, refer to \ref t_CameraServ_PreviewResolution
	\return 	
		0 if no error
	\note
		Parameters should be allocated/de-allocated by the user of the interface
*/
uint32_t CameraServ_CameraGetPreviewProp(uint32_t* DisplayId, t_CameraServ_PreviewResolution* PreviewResolution);

/*! \ingroup Camera_services
	\fn uint32_t CameraServ_CameraSetPictureProp(t_CameraServ_PictureResolution* PictureResolution, e_CameraServ_PictureFormat_t* PictureFormat);
	\brief Service allowing to set the properties of the picture to take for camera use cases
    \param [in] PictureResolution resolution of the captured picture, refer to \ref t_CameraServ_PictureResolution
    \param [in] PictureFormat format of the captured picture, refer to \ref e_CameraServ_PictureFormat_t
	\return 	
		0 if no error
	\note
        Parameters should be allocated/de-allocated by the user of the interface
		If the pointer on a setting  is Null (sensorID or settings in t_CameraServ_SensorProperties structure), this means that the corresponding setting is unchanged
*/
uint32_t CameraServ_CameraSetPictureProp(uint32_t* PictureResolution, uint32_t* PictureFormat);

/*! \ingroup Camera_services
	\fn uint32_t CameraServ_CameraGetPictureProp(t_CameraServ_PictureResolution* PictureResolution, e_CameraServ_PictureFormat_t* PictureFormat);
	\brief Service allowing to get the properties of the picture for camera use cases
    \param [out] PictureResolution resolution of the captured picture, refer to \ref t_CameraServ_PictureResolution
    \param [out] PictureFormat format of the captured picture, refer to \ref e_CameraServ_PictureFormat_t
	\return 	
		0 if no error
	\note
		Parameters should be allocated/de-allocated by the user of the interface
*/
uint32_t CameraServ_CameraGetPictureProp(uint32_t* PictureResolution, uint32_t* PictureFormat);

/*! \ingroup Camera_services
	\fn uint32_t CameraServ_CameraSetSensorProp(e_CameraServ_SensorId_t* SensorId, t_CameraServ_SensorProperties* SensorProperties);
	\brief Service allowing to set the properties of the sensor for camera use cases
    \param [in] SensorId sensor selection, refer to \ref e_CameraServ_SensorId_t
    \param [in] SensorProperties sensor properties, refer to \ref t_CameraServ_SensorProperties
	\return 	
		0 if no error
	\note
        Parameters should be allocated/de-allocated by the user of the interface
		If the pointer on a setting  is Null (sensorID or settings in t_CameraServ_SensorProperties structure), this means that the corresponding setting is unchanged
*/
uint32_t CameraServ_CameraSetSensorProp(e_CameraServ_SensorId_t* SensorId, t_CameraServ_SensorProperties* SensorProperties);

/*! \ingroup Camera_services
	\fn uint32_t CameraServ_CameraGetSensorProp(e_CameraServ_SensorId_t* SensorId, t_CameraServ_SensorProperties* SensorProperties, t_CameraServ_PictureResolution* MaxSensorResolution);
	\brief Service allowing to get the properties of the sensor for camera use cases
    \param [out] SensorId selected sensor, refer to \ref e_CameraServ_SensorId_t
    \param [out] SensorProperties sensor properties, refer to \ref t_CameraServ_SensorProperties
    \param [out] MaxSensorResolution maximum resolution supported by the selected sensor, refer to \ref t_CameraServ_PictureResolution
	\return 	
		0 if no error
	\note
		Parameters should be allocated/de-allocated by the user of the interface
*/
uint32_t CameraServ_CameraGetSensorProp(e_CameraServ_SensorId_t* SensorId, t_CameraServ_SensorProperties* SensorProperties, uint32_t* MaxSensorResolution);

/*! \ingroup Camera_services
	\fn uint32_t CameraServ_CameraSetSensorZoom(t_CameraServ_ZoomProperties* ZoomProperties);
	\brief Service allowing to set the zoom of the sensor for camera use cases
    \param [in] ZoomProperties zoom to apply, refer to \ref t_CameraServ_ZoomProperties
	\return 	
		0 if no error
	\note
		Parameters should be allocated/de-allocated by the user of the interface
*/
uint32_t CameraServ_CameraSetSensorZoom(t_CameraServ_ZoomProperties* ZoomProperties);

/*! \ingroup Camera_services
	\fn uint32_t CameraServ_CameraGetSensorZoom(t_CameraServ_ZoomProperties* ZoomProperties);
	\brief Service allowing to get the zoom of the sensor for camera use cases
    \param [in] ZoomProperties applied zoom, refer to \ref t_CameraServ_ZoomProperties
	\return 	
		0 if no error
	\note
		Parameters should be allocated/de-allocated by the user of the interface
*/
uint32_t CameraServ_CameraGetSensorZoom(t_CameraServ_ZoomProperties* ZoomProperties);

/*! \ingroup Camera_services
	\fn uint32_t CameraServ_CameraPreviewStart(void);
	\brief Service allowing to start the preview
	\return 	
		0 if no error
*/
uint32_t CameraServ_CameraPreviewStart(void);

/*! \ingroup Camera_services
	\fn uint32_t CameraServ_CameraPreviewStop(void);
	\brief Service allowing to stop the preview
	\return 	
		0 if no error
*/
uint32_t CameraServ_CameraPreviewStop(void);


/*! \ingroup Camera_services
	\fn uint32_t CameraServ_CameraTakeAShot(int8_t *outputFilePath, e_CameraServ_ShotMode_t ShotMode, t_CameraServ_BurstProperties* BurstProperties);
	\brief Service allowing to take a picture
    \param [in] outputFilePath location where the image will be stored
    \param [in] ShotMode shot mode selection, refer to \ref e_CameraServ_ShotMode_t
    \param [in] BurstProperties burst mode parameters, refer to \ref t_CameraServ_BurstProperties, applicable if burst mode is selected else can be NULL
	\return 	
		0 if no error
*/
uint32_t CameraServ_CameraTakeAShot(int8_t *outputFilePath, e_CameraServ_ShotMode_t ShotMode, t_CameraServ_BurstProperties* BurstProperties);

/*! \ingroup Camera_services
	\fn uint32_t CameraServ_CameraStopShot(void);
	\brief Service allowing to stop the infinite burst shot
	\return 	
		0 if no error
*/
uint32_t CameraServ_CameraStopShot(void);



#endif /* _CAMERASERV_H */
