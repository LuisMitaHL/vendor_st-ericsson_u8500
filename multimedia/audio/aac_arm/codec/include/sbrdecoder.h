/*
  SBR decoder frontend prototypes and definitions
*/

#ifndef __SBRDECODER_H
#define __SBRDECODER_H


#define SBR_EXTENSION          13  /* 1101 */
#define SBR_EXTENSION_CRC      14  /* 1110 */

#ifdef SBR_MC
#define MAXNRELEMENTS6 6
#define MAXNRSBRCHANNELS6 MAXNRELEMENTS6
#define MAXNRQMFCHANNELS6 6
#define MAXNRELEMENTS 6
#else
#define MAXNRELEMENTS 2
#endif
#define MAXNRSBRCHANNELS MAXNRELEMENTS

#ifdef MONO_ONLY
#define MAXNRQMFCHANNELS 1
#else
#define MAXNRQMFCHANNELS MAXNRSBRCHANNELS
#endif

#define MAXSBRBYTES 269 // it used to be 128        /* have to be correctly determinated */
#ifdef ARM
#define MAXSBRBYTES_INT32 75   
#else /* ARM */
#define MAXSBRBYTES_INT24 100 // is used to be 43   /* MAXSBRBYTES / 3 for int24 mode */
#endif /* ARM */ 
#define RIGHT_CHANNEL 1

typedef enum
{
  SBRDEC_OK = 0,
  SBRDEC_CONCEAL,
  SBRDEC_NOSYNCH,
  SBRDEC_ILLEGAL_PROGRAM,
  SBRDEC_ILLEGAL_TAG,
  SBRDEC_ILLEGAL_CHN_CONFIG,
  SBRDEC_ILLEGAL_SECTION,
  SBRDEC_ILLEGAL_SCFACTORS,
  SBRDEC_ILLEGAL_PULSE_DATA,
  SBRDEC_MAIN_PROFILE_NOT_IMPLEMENTED,
  SBRDEC_GC_NOT_IMPLEMENTED,
  SBRDEC_ILLEGAL_PLUS_ELE_ID,
  SBRDEC_CREATE_ERROR,
  SBRDEC_NOT_INITIALIZED
}
SBR_ERROR;

typedef enum
{
    SBR_INIT_OK,
    SBR_INIT_FAILED,
    SBR_REINIT
}
SBR_INIT_STATUS;

typedef enum
{
  SBR_ID_SCE = 0,
  SBR_ID_CPE,
  SBR_ID_CCE,
  SBR_ID_LFE,
  SBR_ID_DSE,
  SBR_ID_PCE,
  SBR_ID_FIL,
  SBR_ID_END
}
SBR_ELEMENT_ID;

typedef struct
{
    int ElementID;
    int ExtensionType;
    int Payload;
#ifdef USE_3GPP_BITBUF
    unsigned char Data[MAXSBRBYTES];
#else
#ifdef ARM
    unsigned int Data[MAXSBRBYTES_INT32];
#else /* ARM */
    unsigned int Data[MAXSBRBYTES_INT24];
#endif /* ARM */
#endif
#ifdef SBR_MC
    int FirstChannel;
	int LastChannel;
	int sbr_stream_element_index; // to get 
	int LeftChannelIndex;
	int RightChannelIndex;
#endif

}
SBR_ELEMENT_STREAM;

typedef struct
{
  int NrElements;
    //int previous_NrElements;
    //int NrElementsCore;
  SBR_ELEMENT_STREAM sbrElement[MAXNRELEMENTS]; /* for the delayed frame */
}
SBRBITSTREAM;

#include "sbr_dec.h"

typedef struct
{
    SBR_CHANNEL      SbrChannel[MAXNRSBRCHANNELS];
#ifdef SBR_MC
    SBR_HEADER_DATA  sbr_header[MAXNRELEMENTS];
#else
    SBR_HEADER_DATA  sbr_header;
#endif
    struct PS_DEC ParametricStereoDec;
    SBR_CONCEAL_DATA SbrConcealData;
    Float *sav_scratch_addr;
}
SBR_DECODER_INSTANCE;

#ifdef ARM
typedef  SBR_DECODER_INSTANCE *SBRDECODER;
#else
typedef struct SBR_DECODER_INSTANCE *SBRDECODER;
#endif


SBR_INIT_STATUS openSBR (SBR_DECODER_INSTANCE *self, int sampleRate, int samplesPerFrame, int bDownSample, int bApplyQmfLp, int num_chans) ;

SBR_ERROR applySBR (CODEC_INTERFACE_T *interface_fe,
                    SBRBITSTREAM * Bitstr,
                    Float *TimeData,
                    int *numChannels,
                    int frameOK,
                    int bDownSample,
                    int bBitstreamDownMix);
#endif
