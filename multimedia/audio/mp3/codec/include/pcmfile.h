
 


#ifndef _pcmfile_h_
#define _pcmfile_h_

//#define USE_AUDIO_CLASS

#ifndef USE_AUDIO_CLASS

#include "audiolibs_common.h"
#include "vector.h"
#include "driver_readbuffer.h"
#include "driver_readfile.h"

/* different modes to open a RIFF file */
typedef enum {
    WAVE_NOT_SUPPORTED = 0,
    WAVE_PCM,
    WAVE_MP3,     /* MPEG-LAYER3 bitstream data in RIFF */
    WAVE_AAC      /* MPEG AAC bitstream data in RIFF */
} auMode;

/* audio information structure for conventional audio files */
typedef struct {
    unsigned int   ID;
    unsigned long  flags;
    unsigned int   blockSize;
    unsigned int   framesPerBlock;
    unsigned int   codecDelay;
} AudioInfoMP3;

/* audio information structure for mpeg-aac */
typedef struct  {
    unsigned int   ID;
    unsigned long  flags;
    unsigned int   profile;
    unsigned int   channelConfiguration;
    unsigned int   nPrograms;
    unsigned int   codecDelay;
}  AudioInfoAAC;

struct AudioInfo {
    auMode mode;
    unsigned long   bitsPerSample;
    unsigned long   sampleRate;
    unsigned long   nChannels;
    unsigned long   nSamples;
    unsigned long   isLittleEndian;
    double          fpScaleFactor;    /* for scaling floating point values */
	
    AudioInfoMP3  mp3info;
    AudioInfoAAC  aacinfo;
    
};

typedef void * hAuFile ;

#ifdef __cplusplus
#define MY_C "C"
#else
#define MY_C
#endif

/* pcm file decoder */
extern MY_C int  auOpenRead(hAuFile ha, FILE *f, struct AudioInfo *info);

extern MY_C long auReadFloat(hAuFile ha, float *s, long nSamples);

extern MY_C long auReadFract(hAuFile ha, Float *s, long nSamples);

extern MY_C void auClose(hAuFile ha);

extern MY_C int
auOpenWrite(FILE *f, struct AudioInfo *info);

extern MY_C long
auWriteFract(struct AudioInfo *info,FILE *f, Float *s, long nSamples);

#define AU_NO_ERROR  0
#define AU_ERROR     1

extern MY_C const char *auGetErrorText(int errorCode);

#endif /* USE_AUDIO_CLASS */

#endif /* Do not edit below this line */

