
#ifndef _H_chandefs
#define _H_chandefs

#ifdef ARM
/* include aac specific defines file */
//#include "defines.h"
#endif

#ifdef BIGDECODER

    /*
     * channels for big decoder configuration (modify for any desired
     * decoder configuration)
     */
#define    FChans  15		/* front channels: left, center, right */
#define    FCenter  1 		/* 1 if decoder has front center channel */
#define    SChans  18		/* side channels: */
#define    BChans  15		/* back channels: left surround, right
				 * surround */
#define    BCenter  1		/* 1 if decoder has back center channel */
#define    LChans   1		/* LFE channels */
#define    XChans   2		/* scratch space for parsing unused channels */

#else /* BIGDECODER */

#ifndef STEREO_ONLY
    /*
     * channels for 5.1 main profile configuration (modify for any desired
     * decoder configuration)
     */
#define    FChans  3		/* front channels: left, center, right */
#define    FCenter 1		/* 1 if decoder has front center channel */
#define    SChans  0		/* side channels: */
#define    BChans  2		/* back channels: left surround, right surround */
#define    BCenter 0		/* 1 if decoder has back center channel */
#define    LChans  1		/* LFE channels */
#define    XChans  2		/* scratch space for parsing unused channels */

#else

/* channels for stereo decoder */

#ifdef AAC_DEFAULT_CHANNEL_CONFIG
/* We need to force 3 channels, as the default order is center, left,
   right.  As as result stereo bitstreams result in segmentation
   violations */
#define    FChans  3		/* front channels: left, center, right */
#define    FCenter 1		/* 1 if decoder has front center channel */
#else
#ifdef MC
#define    FChans  3		/* front channels: left, center, right */
#define    FCenter 1		/* 1 if decoder has front center channel */
#else
#define    FChans  2		/* front channels: left, center, right */
#define    FCenter 0		/* 1 if decoder has front center channel */
#endif /*MC*/
#endif /* AAC_DEFAULT_CHANNEL_CONFIG*/



#ifdef MC
#define    SChans  2		/* side channels: */
#define    BChans  2		/* back channels: left surround, right surround */
#define    BCenter 0		/* 1 if decoder has back center channel */
#define    LChans  1		/* LFE channels */
#define    XChans  2		/* scratch space for parsing unused channels */
#else
/* if this is chosed then following patch is to be used for SBR in aac_local.h"
#ifndef SBR
	Float           aac_output_buffer_aligned[(Chans - XChans) * (LN2+4)];
	Float           *aac_output_buffer;
#else // SBR compiled

	Float           aac_output_buffer_aligned[(Chans - XChans) * (LN+4)];//__attribute__((aligned(16)));
	Float           *aac_output_buffer;//[(Chans - XChans) * (LN+4)];//__attribute__((aligned(16)));
#endif
	*/
#define    SChans  0		/* side channels: */
#define    BChans  0		/* back channels: left surround, right surround */
#define    BCenter 0		/* 1 if decoder has back center channel */
#define    LChans  0		/* LFE channels */
#define    XChans  2		/* scratch space for parsing unused channels */
#endif

#endif /* STEREO_ONLY */
#endif /* BIGDECODER */



#ifdef MC
#define    Chans  (FChans + SChans + BChans + LChans + XChans)-BChans
/* -2 to map side and back channels at the same location */
#else
#define    Chans  (FChans + SChans + BChans + LChans + XChans)
#endif







#ifdef AAC_DEFAULT_CHANNEL_CONFIG

#define SideChannelsBaseIndex  FChans
#define BackChannelsBaseIndex  (SideChannelsBaseIndex+SChans)
#define LFEChannelsBaseIndex   (BackChannelsBaseIndex+BChans)

#define FrontCenterIndex        0
#define FrontLeftIndex          1

#else 

#ifdef MC
#define LFEChannelsBaseIndex    FChans
#define SideChannelsBaseIndex   (LFEChannelsBaseIndex+LChans)
#define BackChannelsBaseIndex   (LFEChannelsBaseIndex+LChans)
#define FrontCenterIndex        0
#define FrontLeftIndex          1

/**

* Enumerated type for each of the speakers.

*/

typedef enum 
{
	AAC_SPEAKER_CENTRE = 0, /**< Centre */
	AAC_SPEAKER_LEFT = 1, /**< Left */
	AAC_SPEAKER_RIGHT = 2, /**< Right */
	AAC_SPEAKER_LFE1 = 3,/**< Low Frequency Effects 1 */
	AAC_SPEAKER_LS = 4,/**< Left Surround */
	AAC_SPEAKER_RS = 5,/**< Right Surround */
	AAC_SPEAKER_Cs = 6,/**< Center Surround */
	AAC_SPEAKER_Lsr = 7,/**< Left Surround in Rear */
	AAC_SPEAKER_Rsr = 8,/**< Right Surround in Rear */
	AAC_SPEAKER_Lss = 9,/**< Left Surround on Side */
	AAC_SPEAKER_Rss = 10,/**< Right Surround on Side */
	AAC_SPEAKER_Lc = 11,/**< Between Left and Centre in front */
	AAC_SPEAKER_Rc = 12,/**< Between Right and Centre in front */
	AAC_SPEAKER_Lh = 13,/**< Left Height in front */
	AAC_SPEAKER_Ch = 14,/**< Centre Height in Front */
	AAC_SPEAKER_Rh = 15,/**< Right Height in front */
	AAC_SPEAKER_LFE2 = 16,/**< Low Frequency Effects 2 */
	AAC_SPEAKER_Lw = 17,/**< Left on side in front */
	AAC_SPEAKER_Rw = 18,/**< Right on side in front */
	AAC_SPEAKER_Oh = 19,/**< Over the listeners Head */
	AAC_SPEAKER_Lhs = 20,/**< Left Height on Side */
	AAC_SPEAKER_Rhs = 21,/**< Right Height on Side */
	AAC_SPEAKER_Chr = 22,/**< Centre Height in Rear */
	AAC_SPEAKER_Lhr = 23,/**< Left Height in Rear */
	AAC_SPEAKER_Rhr = 24,/**< Right Height in Rear */
	AAC_SPEAKER_Clf = 25,/**< Low Center in Front */
	AAC_SPEAKER_Llf = 26,/**< Low Left in Front */
	AAC_SPEAKER_Rlf = 27,/**< Low Right in Front */
	AAC_SPEAKER_Lt = 28,
	AAC_SPEAKER_Rt = 29,
	AAC_SPEAKER_NONE = 30, /* dummy used for stereo output with only one channel */
	AAC_SPEAKER_MAX_SPEAKERS /**< This must always be the last entry on the list */
} aacDecoderSpeakers ;

extern const int AacDecSpeakerOutMap[AAC_SPEAKER_MAX_SPEAKERS+1];

#else

#define LFEChannelsBaseIndex    FChans
#define SideChannelsBaseIndex   (LFEChannelsBaseIndex+LChans)
#define BackChannelsBaseIndex   (SideChannelsBaseIndex+SChans)

#define FrontCenterIndex        (FChans-1)
#define FrontLeftIndex          0

#endif //MC

#endif /* AAC_DEFAULT_CHANNEL_CONFIG */

#endif /* Do not edit below this line */

