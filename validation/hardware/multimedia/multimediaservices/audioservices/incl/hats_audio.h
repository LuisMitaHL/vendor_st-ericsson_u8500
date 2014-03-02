/*!
	\file hats_audio.h

*/


/*! \defgroup audio_services Access to Audio services 
	\par	
		This part describes the C interface to provide access main audio services
		- Start/Stop PCM stream
		- Control volume levels, mute state
		- Select audio source and audio sink 	
*/

typedef struct {
	char ChunkID[4];			/* should be "RIFF*/
	uint32_t ChunkSize;
	char Format[4];				/* should be "fmt */
	char SubChunk1ID[4];
	uint32_t SubChunk1Size;
	uint16_t AudioFormat;		/* wav format : PCM, IEEE float, ... */
	uint16_t NumChannels;
	uint32_t SampleRate;
	uint32_t ByteRate;
	uint16_t BlockAlign;
	uint16_t BitsPerSample;
	char SubChunk2ID[4];
	uint32_t SubChunk2Size;
}e_HatsAudio_PCM_Header_t;

/*------------------------------------------------------*/
/*! 	\ingroup audio_services
	\def HATS_AUDIO_NO_ERROR
	\brief  code error returned meaning that the service has been executed without error
*/

#define  HATS_AUDIO_NO_ERROR  		((uint32_t) 0)

/*------------------------------------------------------*/
/*! 	\ingroup audio_services
	\def HATS_AUDIO_FAILED
	\brief  code error returned meaning that an error occurs during service execution
*/
#define  HATS_AUDIO_FAILED    		((uint32_t) 1)

/*------------------------------------------------------*/
/*! 	\ingroup audio_services
	\def HATS_AUDIO_DEVICE_ALREADY_USED
	\brief  code error returned meaning that the device is already in use
*/
#define  HATS_AUDIO_DEVICE_ALREADY_USED ((uint32_t) 2)

/*------------------------------------------------------*/
/*! 	\ingroup audio_services
	\def HATS_AUDIO_INVALID_PARAMETER
	\brief  code error returned meaning that a parameter is not valid ( details printed on console)
*/
#define  HATS_AUDIO_INVALID_PARAMETER   ((uint32_t) 3)

/*------------------------------------------------------*/
/*! 	\ingroup audio_services
	\def HATS_AUDIO_NOT_AVAILABLE
	\brief  code error returned meaning that service is not available ( details printed on console)
*/
#define  HATS_AUDIO_NOT_AVAILABLE   ((uint32_t) 4)


/* +PR_CAP_2532_001 */
/*------------------------------------------------------*/
/*! 	\ingroup audio_services
	\def HATS_AUDIO_FORMAT_NOT_SUPPORTED
	\brief  code error returned meaning that audio Format is not supported ( details printed on console)
*/
#define  HATS_AUDIO_FORMAT_NOT_SUPPORTED   ((uint32_t) 5)
/* - PR_CAP_2532_001 */

/*------------------------------------------------------*/
/*! 	\ingroup audio_services
	\def HATS_AUDIO_NO_SPACE_LEFT
	\brief  code error returned meaning that no space left in File system to perform action
*/
#define  HATS_AUDIO_NO_SPACE_LEFT   ((uint32_t) 6)

/*! 	\ingroup audio_services
	\enum e_HatsAudioMode_t
	\brief This control provides the various ways to send and receive data to/from audio codec.
*/
typedef enum{
	NORMAL_PLAYBACK_MODE  /**< In this mode ARM will send decoded audio packets to AB8500 audio codec and audio codec 30 ms FIFO will not be used. 
	                                                    This involves lot of memcopies from user space to kernel space. */
	,FM_PLAYBACK_MODE /**< This mode will provide FM listening feature. Audio codec will receive digital data from IF1 interface which is connected 
	                              to FM module and then data will be routed to some output device like Headset, Speaker, etc. */
	,FM_TX_MODE   /**< This mode will provide FM transmission feature. For instance, one would listen to mp3 streams stored in mobile phone on the car kit.
	                              Audio codec will receive PCM data from MSP1 through its IF0 interface and then will route this data to IF1 interface which is connected to FM module. FM module will then modulate and transmit the modulated data.*/
	,DIGITAL_LOOPBACK_MODE    /**< This mode provides digital loopback in STW8500. PCM data from MSP1 interface will be looped back to AB8500 device. */
	,NORMAL_CAPTURE_MODE    /**< This is the classical way of recording. ALSA driver will fetch PCM data from MSP1 connected with audio codec interface IF0 
	                                and will send to user space.*/
	,FM_CAPTURE_MODE  /**< This mode will provide FM recording capabilities. FM digital coming audio codec interface IF1 will be routed to audio codec interface IF0 
	                               and will come out through MSP1. ALSA driver will then fetch FM PCM data from MSP1 and will send to user space (APE).*/
	,ANALOG_LOOPBACK_MODE   /**< This mode provides analog loopback capability. More info regarding this feature is (TBD). */
#ifdef CR_CAP_73_001_STW4500_AUDIOLOOP
	,AB_DIGITAL_LOOPBACK_MODE    /**< This mode provides digital loopback in AB8500. PCM data from audio codec interface IF0 are looped back to DAC interface. */
#endif /* CR_CAP_73_001_STW4500_AUDIOLOOP */
	,NB_OF_DATA_MODE   /**< number of different mode available */
}e_HatsAudioMode_t;


/*! 	\ingroup audio_services
	\enum e_HatsAudioOutput_t
	\brief This control provides the selection of audio sink.
*/
typedef enum{
	CODEC_DEST_HEADSET 		/**< 	to select heaset organ */
	,CODEC_DEST_EARPIECE  		/**< 	to select handset orhan */
	,CODEC_DEST_HANDSFREE	 /**< to select hansfree mode */
	,CODEC_DEST_VIBRATOR1  	/**< to select vibrator 1 */
	,CODEC_DEST_VIBRATOR2  /**< to select vibrator 2 */
	,CODEC_DEST_FM_TX  /**< to select FM_TX */
	,CODEC_NB_OF_DEST   /**< number of different mode available */
}e_HatsAudioOutput_t;

/*! 	\ingroup audio_services
	\enum e_HatsAudioInput_t
	\brief This control provides the selection of audio source.
*/
typedef enum{
	CODEC_SRC_LINEIN		  /**< select analog line in*/
	,CODEC_SRC_MICROPHONE_1A   /**< select analog microphone 1A*/
	,CODEC_SRC_MICROPHONE_1B /**< select analog microphone 1*/
	,CODEC_SRC_MICROPHONE_2 /**< select analog microphone  2*/
	,CODEC_SRC_D_MICROPHONE_12  /**< select digital microphones  1 & 2*/
	,CODEC_SRC_D_MICROPHONE_34  /**< select digital microphones  3 & 4*/
	,CODEC_SRC_D_MICROPHONE_56  /**< select digital microphones  5 & 6 */
	,CODEC_SRC_FM_RX  /**< to select FM_RX */
	,CODEC_NB_OF_SRC   /**< number of different mode available */
	,CODEC_SRC_MICROPHONE_1 = CODEC_SRC_MICROPHONE_1B  /**< select analog microphone 1B*/
}e_HatsAudioInput_t;



/*! 	\ingroup audio_services
	\enum e_HatsAudioDevice_t
	\brief definitions of devices available
*/
typedef enum{
	AUDIO_DEVICE_0		  /**< main device ID*/
	,AUDIO_DEVICE_BT		  /**< Bluetooth device ID*/
	,AUDIO_DEVICE_HDMI		  /**< HDMI device ID*/
	,NB_OF_AUDIO_DEVICE   /**< number of device available*/
}e_HatsAudioDevice_t;

/*! 	\ingroup audio_services
	\enum e_HatsAudioCodecType_t
	\brief definitions of stream type
*/
typedef enum{
	CODEC_PLAYBACK	 /**< playback type*/
	,CODEC_CAPTURE   /**< record type*/
	,CODEC_MASTER    /**< master type ( used only to manage master volume and mute)*/
	,NB_OF_CODEC_TYPE   /**< number of codec Type available*/
}e_HatsAudioCodecType_t;

/*! 	\ingroup audio_services
	\enum e_HatsAudioDevice_t
	\brief definition of mute states
*/
typedef enum{
	STREAM_MUTED		  /**< stream is muted*/
	,STREAM_UNMUTED   /**< stream is  no muted*/
}e_HatsAudioMute_t;

/*! 	\ingroup audio_services
	\enum e_HatsAudioChannelMode_t
	\brief definition of multi channel mode
*/
typedef enum{
	MULTI_CHANNEL_DISABLE		  /**< multi channel is off only stereo 16 bit playback is available playback*/
	,MULTI_CHANNEL_ENABLE   /**< multi channel is on, 16 & 32 bits wave files are played in 20bit 8 ch (default)*/
	,NB_MULTICHANNEL_MODE   /**< nb of mode */
}e_HatsAudioChannelMode_t;


/*! 	\ingroup audio_services
	\enum e_HatsAudioDevice_t
	\brief definition of mute states
*/
typedef enum{
	AUDIO_CODEC_NOT_SUPPORTED		  /**< audio codec is not supported*/
	,AUDIO_CODEC_SUPPORTED   /**< audio codec is supported */
}e_HatsAudioCodecCapability_t;


/*! 	\ingroup audio_services
	\enum e_HatsAudioBurstMode_t
	\brief definition of burst mode
*/
typedef enum{
	BURST_MODE_DISABLE		  /**< burst is off dat is transfered to AB8500 et 480000 kHz (default)*/
	,BURST_MODE_ENABLE   /**< Burst mode is on,  data are transfered to AB8500 by 32 ms burst*/
	,NB_BURST_MODE   /**< nb of mode */
}e_HatsAudioBurstMode_t;

/*! 	\ingroup audio_services
	\struct HatsAudioScCallCodec_t
	\brief list of speech call codec. setted by \ref e_HatsAudioCodecCapability_t value
*/
typedef struct{
uint8_t  UMTS_AMR;
uint8_t  UMTS_AMR2;
uint8_t  UMTS_AMRWB;
uint8_t  HR_AMR;
uint8_t  FR_AMR;
uint8_t  FR_AMRWB;
uint8_t  GSM_HR;
uint8_t  GSM_FR;
uint8_t  GSM_EFR;
uint8_t  padding1;
uint16_t  padding2;
}HatsAudioScCallCodec_t;


/*! \ingroup audio_services
	\fn uint32_t StartPCMTransfer(e_HatsAudioDevice_t Device,e_HatsAudioCodecType_t Type,e_HatsAudioMode_t Mode,char * FilePath)
	\brief Service allowing to initiate a 48 kHz PCM transfer : playback or capture
	\param [in] Device Id \ref e_HatsAudioDevice_t
	\param [in] Type Transfer direction : Playback/Capture
	\param [in] Mode refer to \ref e_HatsAudioMode_t
	\param [in] FilePath string : file location
	\return 
		0 if no error
	\note 
		-	Only one playback at a time ; Only one capture at a time
		-	Start is done with default parameters hard coded in the provided driver
		- 	If function fails, return 0
*/
uint32_t StartPCMTransfer(e_HatsAudioDevice_t Device,e_HatsAudioCodecType_t Type,e_HatsAudioMode_t Mode,char * FilePath);


/*! \ingroup audio_services
	\fn uint32_t StopPCMTransfer(e_HatsAudioDevice_t Device,, e_HatsAudioCodecType_t Type)
	\brief Service allowing to stop a PCM transfer : playback or capture
	\param [in] Type Transfer direction : Playback/Capture
	\param [in] Device Id \ref e_HatsAudioDevice_t
	\return 	
		0 if no error
*/
uint32_t StopPCMTransfer(e_HatsAudioDevice_t Device, e_HatsAudioCodecType_t Type);


/*! \ingroup audio_services
	\fn uint32_t SetSink(e_HatsAudioDevice_t Device,e_HatsAudioOutput_t Output)
	\brief Service allowing to choose the output of the loop or of the playback PCM transfer
	\param [in] Device device Id \ref e_HatsAudioDevice_t
	\param [in] Output selected refer to \ref e_HatsAudioOutput_t
	\return 	
		0 if no error
	\warning
		- Output will be really configured when the playback will be start.
		- When Output is selected, hard coded configuration is applied (gain, HatsAudio outputs …).
*/
uint32_t SetSink(e_HatsAudioDevice_t Device,e_HatsAudioOutput_t Output);

/*! \ingroup audio_services
	\fn uint32_t GetSink(e_HatsAudioDevice_t Device,e_HatsAudioOutput_t* p_Output)
	\brief Service allowing to retrieve current selected output
	\param [in] Device device Id \ref e_HatsAudioDevice_t
	\param [out] p_Output selected output refer to \ref e_HatsAudioOutput_t
	\return 	
		0 if no error
*/
uint32_t GetSink(e_HatsAudioDevice_t Device,e_HatsAudioOutput_t* p_Output);


/*! \ingroup audio_services
	\fn uint32_t SetSrc(e_HatsAudioDevice_t Device,e_HatsAudioInput_t Input)
	\brief Service allowing to choose the input of the loop or of the capture PCM transfer
	\param [in] Device device Id \ref e_HatsAudioDevice_t
	\param [in] Input selected input refer to \ref e_HatsAudioInput_t
	\return 	
		0 if no error
	\warning
		- Input will be really configured when the capture will be start.
		- When input is selected, hard coded configuration is applied (gain, HatsAudio outputs …).
*/
uint32_t SetSrc(e_HatsAudioDevice_t Device,e_HatsAudioInput_t Input);

/*! \ingroup audio_services
	\fn uint32_t GetSrc(e_HatsAudioDevice_t Device, e_HatsAudioInput_t* p_input)
	\brief Service allowing to retrieve current selected input
	\param [in] Device device Id \ref e_HatsAudioDevice_t
	\param [out] p_input selected input refer to \ref e_HatsAudioInput_t
	\return 	
		0 if no error
*/
uint32_t GetSrc(e_HatsAudioDevice_t Device, e_HatsAudioInput_t* p_input);

/*! \ingroup audio_services
	\fn uint32_t SetVolume(e_HatsAudioDevice_t Device,e_HatsAudioCodecType_t Type, uint32_t LeftVolume, uint32_t RightVolume);
	\brief Service allowing to set the volume level of the specified PCM transfer
	\param [in] Device device Id \ref e_HatsAudioDevice_t
	\param [in] Type Transfer direction : Playback/Capture
	\param [in] LeftVolume 	Left volume integer [0-100] step 10
	\param [in] RightVolume 	Right volume integer [0-100] step 10
	\return 	
		0 if no error
*/
uint32_t SetVolume(e_HatsAudioDevice_t Device,e_HatsAudioCodecType_t Type, uint32_t LeftVolume, uint32_t RightVolume);

/*! \ingroup audio_services
	\fn uint32_t GetVolume(e_HatsAudioDevice_t Device,e_HatsAudioCodecType_t Type, uint32_t * p_LeftVolume, uint32_t * p_RightVolume);
	\brief Service allowing to retrieve the volume level of the specified PCM transfer
	\param [in] Device device Id \ref e_HatsAudioDevice_t
	\param [in] Type Transfer direction : Playback/Capture
	\param [out] p_LeftVolume 	Left volume integer [0-100] 
	\param [out] p_RightVolume 	Right volume integer [0-100]
	\return 	
		0 if no error
*/
uint32_t GetVolume(e_HatsAudioDevice_t Device,e_HatsAudioCodecType_t Type, uint32_t * p_LeftVolume, uint32_t * p_RightVolume);


/*! \ingroup audio_services
	\fn uint32_t SetMute(e_HatsAudioDevice_t Device,e_HatsAudioCodecType_t Type, e_HatsAudioMute_t State);
	\brief Service allowing to mute/unmute the specified PCM transfer
	\param [in] Device device Id \ref e_HatsAudioDevice_t
	\param [in] Type Transfer direction : Playback/Capture
	\param [in] State  mute state  \ref e_HatsAudioMute_t
	\return 	
		0 if no error
*/
uint32_t SetMute(e_HatsAudioDevice_t Device,e_HatsAudioCodecType_t Type, e_HatsAudioMute_t State);

/*! \ingroup audio_services
	\fn uint32_t GetMute(e_HatsAudioDevice_t Device,e_HatsAudioCodecType_t Type,e_HatsAudioMute_t* p_State);
	\brief Service allowing to retrieve the mute state of the specified PCM transfer
	\param [in] Device device Id \ref e_HatsAudioDevice_t
	\param [in] Type Transfer direction : Playback/Capture
	\param [out] p_State mute state  \ref e_HatsAudioMute_t
	\return 	
		0 if no error
*/
uint32_t GetMute(e_HatsAudioDevice_t Device,e_HatsAudioCodecType_t Type,e_HatsAudioMute_t*p_State);

/*! \ingroup audio_services
	\fn uint32_t SetMultichannelMode(e_HatsAudioDevice_t Device,e_HatsAudioChannelMode_t Mode);
	\brief Service allowing to retrieve the mute state of the specified PCM transfer
	\param [in] Device device Id \ref e_HatsAudioDevice_t
	\param [in] Mode Alsa behaviour mode \ref e_HatsAudioChannelMode_t
	\return 	
		0 if no error
*/
uint32_t SetMultichannelMode(e_HatsAudioDevice_t Device,e_HatsAudioChannelMode_t Mode);

/*! \ingroup audio_services
	\fn uint32_t StartSpeechCall(e_HatsAudioDevice_t Device);
	\brief Service allowing to start a 2G or 3G speech call ( audio part only)
	\param [in] Device device Id \ref e_HatsAudioDevice_t
	\return 	
		0 if no error
*/
uint32_t StartSpeechCall(e_HatsAudioDevice_t Device);

/*! \ingroup audio_services
	\fn uint32_t StopSpeechCall(e_HatsAudioDevice_t Device);
	\brief Service allowing to stop a 2G or 3G speech call ( audio part only)
	\param [in] Device device Id \ref e_HatsAudioDevice_t
	\return 	
		0 if no error
*/
uint32_t StopSpeechCall(e_HatsAudioDevice_t Device);
/*! \ingroup audio_services
	\fn uint32_t GetCsCallCapability(HatsAudioScCallCodec_t* pList);
	\brief Service allowing to retreive the list of supported audio codec
	\param [in] pList pointer to the list of audio codec ref audiocodec_t
	\return 	
		0 if no error
*/
uint32_t GetCsCallCapability(HatsAudioScCallCodec_t* pList);

/*! \ingroup audio_services
	\fn uint32_t ConvWavFile(const char * Infile, const char * outfile);
	\brief old Service allowing the convertion from 16 or 32 bits wav file to 20 bit 8ch wav file
	\param [in] Infile pointer to the name of the file to convert
	\param [in] outfile pointer to the name of the converted file
	\return 	
		0 if no error
*/
uint32_t ConvWavFile(const char * Infile, const char * outfile);

/*! \ingroup audio_services
	\fn uint32_t ConvWavFile(e_HatsAudioDevice_t Device,const char * Infile, const char * outfile);
	\brief Service allowing the convertion from 16 or 32 bits wav file to 20 bit 8ch wav file
	\param [in] Device device Id \ref e_HatsAudioDevice_t
	\param [in] Infile pointer to the name of the file to convert
	\param [in] outfile pointer to the name of the converted file
	\return 	
		0 if no error
*/
uint32_t ConvWaveFile(e_HatsAudioDevice_t Device,const char * Infile, const char * outfile);
