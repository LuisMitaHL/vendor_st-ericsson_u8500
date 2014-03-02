/*
  independent channel concealment
*/

#ifndef CONCEAL_H
#define CONCEAL_H

#define MAXSFB  51            /* maximum number of scalefactor bands  */
#define FRAME_SIZE 1024
#define MAX_WINDOWS   8

typedef enum{
  aacConcealment_Ok,
  aacConcealment_FadeOut,
  aacConcealment_Mute,
  aacConcealment_FadeIn
} aacConcealmentState;

typedef enum {
  aacConcealment_NoExpand,
  aacConcealment_Expand,
  aacConcealment_Compress
} aacConcealmentExpandType;

typedef struct
{
  char  WindowShape;
  char  WindowSequence;
  Float SpectralCoefficient[FRAME_SIZE];
  int   iRandomPhase;
  int   prevFrameOk[2];
  int   cntConcealFrame;
  int   ConcealState;
  int   nFadeInFrames;
  int   nValidFrames;
  int   nFadeOutFrames;

  int   badframe_cnt;

} aacConcealmentInfo;

void aac_conceal_channelconfig(MC_Info *aac_mc_info_conceal,						  
						   MC_Info           *aac_mc_info,
						   unsigned int      last_frame,
						   int               *scratch_buffer,
						   int               concealment_on						   
						   );

extern void aacConcealment_Init(aacConcealmentInfo *pConcealmentInfo);

#ifdef ARM
extern void aacConcealment_Apply(aacConcealmentInfo *pConcealmentInfo, 
                                 int            *pSpectralCoefficient,
                                 UCHAR            *WindowSequence,
                                 UCHAR            *WindowShape,
                                 int               SamplingRateIndex,
                                 char              badFrameIndicator,
								 int               concealment_on,
								 int               decim_enbl);
#else

extern void aacConcealment_Apply(aacConcealmentInfo *pConcealmentInfo, 
                                 Float            *pSpectralCoefficient,
                                 UCHAR            *WindowSequence,
                                 UCHAR            *WindowShape,
                                 int               SamplingRateIndex,
                                 char              badFrameIndicator,
								 int               concealment_on);
#endif
#define AAC_NF_NO_RANDOM_VAL  512
extern const Float EXTERN aacConceal_randomPhase[AAC_NF_NO_RANDOM_VAL][2];
extern const Float EXTERN aacConceal_fadeFacTable[];
extern const Float EXTERN conceal_invtab[64];

Float EXTERN *fn_conceal_invtab(void);

#endif /* #ifndef CONCEAL_H */
