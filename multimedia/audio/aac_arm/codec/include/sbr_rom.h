/*
  Declaration of constant tables
*/
#ifndef __rom_H
#define __rom_H


#include "sbrdecsettings.h" /* for MAXNRSBRCHANNELS */
#include "sbrdecoder.h"
#include "env_extr.h"
#include "qmf_dec.h"
#include "ps_dec.h"


#ifdef __flexcc2__
#define SBR_MEM   __EXTERN
#define PS_MEM 	  __EXTERN
#define HYBRID_MEM __EXTERN //YMEM
#define LPP_MEM   __EXTERN
#else
#define SBR_MEM
#define PS_MEM
#define HYBRID_MEM 
#define LPP_MEM
#endif

#define INV_INT_TABLE_SIZE     55
#define SBR_NF_NO_RANDOM_VAL  512

extern const SBR_MEM unsigned char sbr_start_freq_16[16];
extern const SBR_MEM unsigned char sbr_start_freq_22[16];
extern const SBR_MEM unsigned char sbr_start_freq_24[16];
extern const SBR_MEM unsigned char sbr_start_freq_32[16];
extern const SBR_MEM unsigned char sbr_start_freq_44[16];
extern const SBR_MEM unsigned char sbr_start_freq_48[16];

#define NUM_WHFACTOR_TABLE_ENTRIES  9
extern const unsigned short LPP_MEM sbr_whFactorsIndex[NUM_WHFACTOR_TABLE_ENTRIES];
extern const Float LPP_MEM sbr_whFactorsTable[NUM_WHFACTOR_TABLE_ENTRIES][6];

extern const Float SBR_MEM sbr_limGains[4];
extern const Float SBR_MEM sbr_limGains_e[4];
extern const Float SBR_MEM sbr_limiterBandsPerOctave[4];
extern const Float SBR_MEM sbr_smoothFilter[4];
extern const Float SBR_MEM sbr_invIntTable[INV_INT_TABLE_SIZE];
extern const Float SBR_MEM sbr_randomPhase[SBR_NF_NO_RANDOM_VAL][2];


extern const FRAME_INFO SBR_MEM sbr_frame_info1_16;
extern const FRAME_INFO SBR_MEM sbr_frame_info2_16;
extern const FRAME_INFO SBR_MEM sbr_frame_info4_16;
extern const FRAME_INFO SBR_MEM sbr_frame_info8_16;

extern const SBR_HEADER_DATA SBR_MEM sbr_defaultHeader;

extern const char HUF_MEM sbr_huffBook_EnvLevel10T[120][2];
extern const char HUF_MEM sbr_huffBook_EnvLevel10F[120][2];
extern const char HUF_MEM sbr_huffBook_EnvBalance10T[48][2];
extern const char HUF_MEM sbr_huffBook_EnvBalance10F[48][2];
extern const char HUF_MEM sbr_huffBook_EnvLevel11T[62][2];
extern const char HUF_MEM sbr_huffBook_EnvLevel11F[62][2];
extern const char HUF_MEM sbr_huffBook_EnvBalance11T[24][2];
extern const char HUF_MEM sbr_huffBook_EnvBalance11F[24][2];
extern const char HUF_MEM sbr_huffBook_NoiseLevel11T[62][2];
extern const char HUF_MEM sbr_huffBook_NoiseBalance11T[24][2];


extern const Float PS_MEM aRevLinkDecaySer[NO_SERIAL_ALLPASS_LINKS];
extern const int PS_MEM aRevLinkDelaySer[];

extern const int  PS_MEM ps_groupBorders[NO_IID_GROUPS + 1];
extern const int  PS_MEM ps_delta_groupBorders[NO_IID_GROUPS + 1];
extern const short PS_MEM ps_bins2groupMap[NO_IID_GROUPS];
extern const short PS_MEM ps_delta_bins2groupMap[NO_IID_GROUPS];
extern const short PS_MEM ps_bins2groupMap_2[NO_IID_GROUPS];

extern const Float PS_MEM aFractDelayPhaseFactorReQmf[NO_QMF_ALLPASS_CHANNELS];
extern const Float PS_MEM aFractDelayPhaseFactorImQmf[NO_QMF_ALLPASS_CHANNELS];
extern const Float PS_MEM aFractDelayPhaseFactorReSubQmf[SUBQMF_GROUPS];
extern const Float PS_MEM aFractDelayPhaseFactorImSubQmf[SUBQMF_GROUPS];

#ifdef ARM
extern const Float PS_MEM * const PS_MEM aaFractDelayPhaseFactorSerReQmf[3];
extern const Float PS_MEM * const PS_MEM aaFractDelayPhaseFactorSerImQmf[3];
extern const Float PS_MEM * const PS_MEM aaFractDelayPhaseFactorSerReSubQmf[3];
extern const Float PS_MEM * const PS_MEM aaFractDelayPhaseFactorSerImSubQmf[3];
#else
extern const Float PS_MEM *  PS_MEM aaFractDelayPhaseFactorSerReQmf[3];
extern const Float PS_MEM *  PS_MEM aaFractDelayPhaseFactorSerImQmf[3];
extern const Float PS_MEM *  PS_MEM aaFractDelayPhaseFactorSerReSubQmf[3];
extern const Float PS_MEM *  PS_MEM aaFractDelayPhaseFactorSerImSubQmf[3];
#endif

extern const Float PS_MEM ps_scaleFactors[NO_IID_LEVELS];
extern const Float PS_MEM ps_scaleFactorsFine[NO_IID_LEVELS_FINE];
extern const Float PS_MEM ps_alphas[NO_ICC_LEVELS];
extern const Float PS_MEM cos_table[1025];

extern const char  HUF_MEM aBookPsIidTimeDecode[28][2];
extern const char  HUF_MEM aBookPsIidFreqDecode[28][2];
extern const char  HUF_MEM aBookPsIccTimeDecode[14][2];
extern const char  HUF_MEM aBookPsIccFreqDecode[14][2];
extern const char  HUF_MEM aBookPsIidFineTimeDecode[60][2];
extern const char  HUF_MEM aBookPsIidFineFreqDecode[60][2];

extern const Float HYBRID_MEM ps_p2_6[6];
extern const Float HYBRID_MEM ps_p8_13[13];

#endif

