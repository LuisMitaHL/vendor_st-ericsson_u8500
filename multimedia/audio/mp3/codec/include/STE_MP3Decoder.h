

#ifndef STE_MP3_DECODER_H_

#define STE_MP3_DECODER_H_

#include <media/stagefright/MediaSource.h>

    typedef int (*mp3_decode_init_malloc_t)(void *itf);
    typedef int (*mp3_decode_frame_t)(void *interface_fe);
    typedef void (*mp3_reset_t)(void *itf);
    typedef void (*mp3_close_t)(void *itf);
	typedef int  (*mp3_2_5_parse_header_t)(void *interface_fe);

   // typedef int  (*mp3_decode_getFrameReady_t)(void   *pAudioModuleInterface);
	// typedef int  (*mp3_decode_processFrame_t)(void   *pAudioModuleInterface);


namespace android {

struct MediaBufferGroup;

struct STE_MP3Decoder : public MediaSource {
    STE_MP3Decoder(const sp<MediaSource> &source);

    virtual status_t start(MetaData *params);
    virtual status_t stop();

    virtual sp<MetaData> getFormat();

    virtual status_t read(
            MediaBuffer **buffer, const ReadOptions *options);

	virtual int    mp3_decode_reset( void   *pAudioModuleItf );
	virtual int    mp3_decode_getFrameReady( void   *pAudioModuleItf );
	virtual int    mp3_decode_silenceInsertion( void * pAudioModuleItf );
	virtual int    mp3_decode_processFrame( void * pAudioModuleItf );

protected:
    virtual ~STE_MP3Decoder();

private:
    sp<MediaSource> mSource;
    sp<MetaData> mMeta;
    int32_t mNumChannels;

    bool mStarted;

    MediaBufferGroup *mBufferGroup;
	void *pAudioModuleItf;  // AUDIO_MODULE_INTERFACE_T
	int mFirstTime;
    bool bEOSReceived;
   
    void *mDecoderBuf;
    int64_t mAnchorTimeUs;
    int64_t mNumFramesOutput;

    MediaBuffer *mInputBuffer;

    void init();

    void *mp3_itf;
    void *mDLHandle;

    mp3_decode_init_malloc_t mp3_decode_init_malloc;
    mp3_decode_frame_t       mp3_decode_frame;
    mp3_reset_t              mp3_reset;
    mp3_close_t              mp3_close;
    mp3_2_5_parse_header_t   mp3_2_5_parse_header;

	//mp3_decode_getFrameReady_t mp3_decode_getFrameReady;
	//mp3_decode_processFrame_t  mp3_decode_processFrame;

    STE_MP3Decoder(const STE_MP3Decoder &);
    STE_MP3Decoder &operator=(const STE_MP3Decoder &);
};

}  // namespace android

#endif  // STE_MP3_DECODER_H_
