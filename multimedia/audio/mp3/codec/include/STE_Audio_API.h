

#ifndef _STE_AUDIO_API_H_
#define _STE_AUDIO_API_H_

/*  

WARNING: Do not use any external header for the sake of easier segregation of algo source & API

This API file is for external frameworks / wrappers to integrate the codec library
There should not be any need of algo source / header files to be delivered to customer. 
This API file and algo lib (algo API + lib) should fulfill the requirement

*/

#ifndef __cplusplus
#define bool int
//#define true 1
//#define false 0
#endif

#define ONE_MILI_SEC 1000  // one mili second is 1000 micro second


typedef enum 
{
    STE_AUDIO_CHANNEL_NONE   = 0,  /**< Unused or empty */
    STE_AUDIO_CHANNEL_LEFT   = 1,  /**< Left  */
    STE_AUDIO_CHANNEL_RIGHT  = 2,  /**< Right  */
    STE_AUDIO_CHANNEL_CENTRE = 3,  /**< Centre */
    STE_AUDIO_CHANNEL_LS     = 4,  /**< Left Surround */
    STE_AUDIO_CHANNEL_RS     = 5,  /**< Right Surround  */
    STE_AUDIO_CHANNEL_LFE1   = 6,  /**< Low Frequency Effects 1 */
    STE_AUDIO_CHANNEL_Cs     = 7,  /**< Center Surround */
    STE_AUDIO_CHANNEL_Lsr    = 8,  /**< Left Surround in Rear  */
    STE_AUDIO_CHANNEL_Rsr    = 9,  /**< Right Surround in Rear  */
    STE_AUDIO_CHANNEL_Lss    = 10, /**< Left Surround on Side */
    STE_AUDIO_CHANNEL_Rss    = 11, /**< Right Surround on Side  */
    STE_AUDIO_CHANNEL_Lc     = 12, /**< Between Left and Centre in front  */
    STE_AUDIO_CHANNEL_Rc     = 13, /**< Between Right and Centre in front  */
    STE_AUDIO_CHANNEL_Lh     = 14, /**< Left Height in front */
    STE_AUDIO_CHANNEL_Ch     = 15, /**< Centre Height in Front  */
    STE_AUDIO_CHANNEL_Rh     = 16, /**< Right Height in front  */
    STE_AUDIO_CHANNEL_LFE2   = 17, /**< Low Frequency Effects 2 */
    STE_AUDIO_CHANNEL_Lw     = 18, /**< Left on side in front */
    STE_AUDIO_CHANNEL_Rw     = 19, /**< Right on side in front  */
    STE_AUDIO_CHANNEL_Oh     = 20, /**< Over the listeners Head */
    STE_AUDIO_CHANNEL_Lhs    = 21, /**< Left Height on Side */
    STE_AUDIO_CHANNEL_Rhs    = 22, /**< Right Height on Side  */
    STE_AUDIO_CHANNEL_Chr    = 23, /**< Centre Height in Rear  */
    STE_AUDIO_CHANNEL_Lhr    = 24, /**< Left Height in Rear */
    STE_AUDIO_CHANNEL_Rhr    = 25, /**< Right Height in Rear  */
    STE_AUDIO_CHANNEL_Clf    = 26, /**< Low Center in Front */
    STE_AUDIO_CHANNEL_Llf    = 27, /**< Low Left in Front */
    STE_AUDIO_CHANNEL_Rlf    = 28, /**< Low Right in Front */
    STE_AUDIO_CHANNEL_Lt     = 29,
    STE_AUDIO_CHANNEL_Rt     = 30,
    STE_AUDIO_CHANNEL_MAX_CHANNELS /**< This must always be the last entry on the list */
} STE_AUDIO_CHANNEL_MAPPING ;

typedef enum {
    AUDIO_ERROR_NONE, 
    AUDIO_ERROR_WARNING,
    AUDIO_ERROR_INIT_FAIL,
    AUDIO_ERROR_ERROR,
    AUDIO_ERROR_UNDEFINED, 
    AUDIO_ERROR_STREAM_CORRUPT,
    AUDIO_ERROR_INSUFFICIENT_RESOURCES,
    AUDIO_ERROR_UNSUPPORTED_PARAM_TYPE
} AUDIO_ERROR_STATUS; 

typedef struct { 
    char *pBuffer;            // buffer allocated and filled by framework
    unsigned int nAllocLen;          /**< size of the buffer allocated, in bytes */ 
    unsigned int nFilledLen;         /**< number of bytes currently in the buffer */
    unsigned int nOffset;    /* start offset of valid data in bytes from the start of the buffer */
    unsigned int nBufferStatusFlags;    /* Flags to pass info about the status of buffer usage  */
    unsigned int nFrameworkSpecificFlags;        /*< buffer specific flags  from framework*/
    unsigned int nInputSpecificFlags;             /*  flags to indicate the input status */
    long long nTimeStamp;    // Presentation Timestamp of data in buffer
} AUDIO_INPUT_STRUCT_T;


typedef struct { 
    char *pBuffer;            // buffer allocated by framework and filled by audio module
    unsigned int nAllocLen;          /**< size of the buffer allocated, in bytes */ 	 
    unsigned int nFilledLen;         /**< number of bytes currently in the buffer */
    unsigned int nOffset;    /* start offset of valid data in bytes from the start of the buffer */
    unsigned nBufferStatusFlags;    /* Flags to pass info about the status of buffer usage */
    unsigned int nFrameworkSpecificFlags;           /**< buffer specific flags to framework */
    unsigned int nOutputSpecificFlags;             /*  flags to indicate the output status */
    long long nTimeStamp; // Presentation Timestamp of data in buffer 
} AUDIO_OUTPUT_STRUCT_T;

typedef struct {

    int mChunksForThisFrame;
    int mFramesInThisChunk;
    int mBytesConsumed;   // It is the number of bytes consumed/copied/used by decoder from Input buffer

    long long mFirstChunkTimeStamp;
    long long mLastChunkTimeStamp;
    long long mNextChunkTimeStamp;

}AUDIO_INPUT_CHUNK_INFO_T;

typedef struct {

    /*
    All these values should be updated properly 
    at the end of every decode frame by decoder algorithm
    */

    long mCurrentFrameDuration;

    long long mPrevFrameOutTimeStamp;
    long long mCurrFrameOutTimeStamp;	
    long long mNextFrameOutTimeStamp;

}AUDIO_TIMESTAMP_INFO_T;


/* nBufferStatusFlags  */
#define STE_AUDIO_BUFFER_IN_USE                                      0X00000001
#define STE_AUDIO_BUFFER_FREE                                        0X00000002
#define STE_AUDIO_BUFFER_READY                                       0X00000004
#define STE_AUDIO_BUFFER_INVALID                                     0X00000008
#define STE_AUDIO_BUFFER_NEW                                         0X00000010

/* nInputSpecificFlags */
#define STE_AUDIO_INPUT_FIRST_BUFFER                                 0X00000001
#define STE_AUDIO_INPUT_NEW_BUFFER                                   0X00000002
#define STE_AUDIO_INPUT_GET_NEW_FRAME                                0X00000004
#define STE_AUDIO_INPUT_AFTER_SEEK                                   0X00000008
#define STE_AUDIO_INPUT_CONFIG_CHANGED                               0X00000010
#define STE_AUDIO_INPUT_SYNC_LOST                                    0X00000020
#define STE_AUDIO_INPUT_RUN_INPLACE                                  0X00000040
#define STE_AUDIO_INPUT_PARAM_CHANGED                                0X00000080


/* nOutputSpecificFlags  */  
#define STE_AUDIO_OUTPUT_FIRST_TIME                                  0X00000001
#define STE_AUDIO_OUTPUT_BAD_FRAME                                   0X00000002
#define STE_AUDIO_OUTPUT_ENABLE                                      0X00000004
#define STE_AUDIO_OUTPUT_EMIT_BUFFERS                                0X00000008
#define STE_AUDIO_STREAM_PARAM_CHANGED                               0X00000010
#define STE_AUDIO_OUTPUT_PARAM_CHANGED                               0X00000020
#define STE_AUDIO_OUTPUT_COMFORT_NOISE                               0X00000040
#define STE_AUDIO_ERROR_CONCEALMENT_APPLIED                          0X00000080


/* nFrameworkSpecificFlags */ 
#define STE_AUDIO_BUFFERFLAG_EOS                                     0X00000001 
#define STE_AUDIO_BUFFERFLAG_STARTTIME                               0X00000002
#define STE_AUDIO_BUFFERFLAG_DECODEONLY                              0X00000004
#define STE_AUDIO_BUFFERFLAG_DATACORRUPT                             0X00000008
#define STE_AUDIO_BUFFERFLAG_ENDOFFRAME                              0X00000010
#define STE_AUDIO_BUFFERFLAG_SYNCFRAME                               0X00000020
#define STE_AUDIO_BUFFERFLAG_EXTRADATA                               0X00000040
#define STE_AUDIO_BUFFERFLAG_CODECCONFIG                             0X00000080



typedef enum{

    AUDIO_DECODER_TYPE,
    AUDIO_ENCODER_TYPE,
    AUDIO_EFFECT_TYPE,
    AUDIO_TRANSCODER_TYPE

}AUDIO_MODULE_TYPE_T;


typedef enum {

    UNKNOWN_AUDIO = 0,

    MPEG1_LAYER1,  /*  ISO compliant */
    MPEG1_LAYER2,   /* ISO compliant */
    MPEG1_LAYER25,  /* ISO compliant + fhg extension */
    MPEG1_LAYER3,  /*  ISO compliant */
    MPEG2_LAYER1,  /*  ISO compliant */
    MPEG2_LAYER2,  /*  ISO compliant */
    MPEG2_LAYER25, /*  ISO compliant + fhg extension */
    MPEG2_LAYER3,  /*  ISO compliant */

    MPEG2_AAC_ADTS,
    MPEG2_AAC_ADIF,
    MPEG2_AAC_RAW,
    MPEG4_AAC_ADTS,
    MPEG4_AAC_ADIF,
    MPEG4_AAC_RAW,

    /* More modes will be defined for types of modes supported */

}AUDIO_MODULE_MODE_T;


typedef struct {
    AUDIO_MODULE_MODE_T     frameType;
    bool mFrameReady;      // True indictates that frame is ready to be decoded
    bool mCompleteFrame;   // This is Complete Frame  
    bool mFirstFrame;      // This is the First frame to be decoded
    int  mFrameLength;     // this is the length of the encoded frame to be decoded
    int  mFrameDuration;   // this is expected frame duration of the decoded pcm samples
    int  nBitRate;         //  bitrate of the stream
    int  nSamplingFreq;    //  sampling frequency of the decoded pcm samples
    int  nChannels;        //  number of channels
    int  nBitsPerSampleRef;   // the number of bits per sample in the stream
    long long nThisFrameOutTimeStamp;    // This is the Time stamp to be attached with This frame, valid if mFrameReady == true
} AUDIO_FRAME_INFO_T; 


typedef struct {

    /* All of following variables are internal to silence insertion module */
    bool activateSilenceInsertion;  // T
    int  mFramesTobeInserted;
    int  mTotalSilenceFrames;
    int  mSilenceFrameSize;
    int  mChannels;
    int  mSampleSize;
    int  mSilenceFrameDuration;

}AUDIO_SILENCE_INFO_T;


typedef struct {
    unsigned int   nDstChannelMask; 
    unsigned int   bitsPerSampleToOutput ; 
    unsigned int   DRCPercent ;
    int            enableDownmix;
    int            lfeMixedToFront;
    int            enableDialNorm ;
    int            enableSilenceInsertion;
    bool           bSamplesInterleaved;   // if the samples should be interleaved or not
    bool           bEndian;               // write output samples in little or big endian
    bool           bFrameAlignedInput;    /* Information to Audio Module */
    bool           bFirstTime;             /* true if This is first time call */
    int            eChannelMapArray[STE_AUDIO_CHANNEL_MAX_CHANNELS];   // channel layout 
} AUDIO_PLAYER_COMMAND_T;


typedef struct {

    /* Input to Decoder */
    AUDIO_PLAYER_COMMAND_T     audioPlayerCommand;
    AUDIO_INPUT_STRUCT_T       inputDataStruct;

    /* Output from Decoder */
    AUDIO_OUTPUT_STRUCT_T      outputDataStruct;   
    AUDIO_FRAME_INFO_T         audioFrameInfo;
    AUDIO_INPUT_CHUNK_INFO_T   inputAudioChunkInfo;
    AUDIO_TIMESTAMP_INFO_T     audioTimeStampInfo;
    AUDIO_SILENCE_INFO_T       audioSilenceInfo;
    int                        eErrorNumber;
    void                      *pAudioCodecInterface;

} AUDIO_MODULE_INTERFACE_T; 

#endif   // _STE_AUDIO_API_H_

