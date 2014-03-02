
#ifndef _cil_aac_params_h_
#define _cil_aac_params_h_

#include "audiolibs_types.h"

/* aac dec */
typedef struct {
  t_uint16        iSyntax;	// STATIC range 0(ADTS),1(ADIF),2(RAW),3(RAW_STREAMING), 4(AutoDetect ADIF or ADTS), 5(AutoDetect ADIF/ADTS/RAW) ( default 0(ADTS) )
  t_uint16        iObjectType;	// STATIC: values 2(LC),4(LTP), default 2(LC) - Value 1(Main) is not supported. However the Bit 7 may be used to force Main Profile as LC (value 0x81) - autodetection in case of Autodetect Syntax
  t_uint16        iFrequency;	// STATIC:  8->96kHz (t_saa_sample_freq enumeration) - autodetection in case of ADIF/ADTS, anyway initialization must reach authorized range of values!!
  //t_uint16        iMaxInputChannels;	// STATIC range [1,6], default 2(stereo) - NOT USED
  t_uint16	iMemoryPreset;  	// STATIC 
  // 0(MEM_DEFAULT)  1(MEM_ALL_DDR) 2(MEM_ALL_TCM)
  // 8(MEM_ALL_ESRAM) 9(MEM_MIX_ESRAM_DDR) 11(MEM_MIX_ESRAM_OTHER_1)
//  t_uint16        iMaxBlocksPerFrame;	 STATIC (needed for ADTS syntax only) range [1,4], default 1 - NOT USED
  t_uint16		  icrc_ignore; // STATIC default 1 crc errors are ignored instead of muting the output
  t_uint16        iDownSample;	// STATIC range [0,1], default 0, down_sample (decimate by 2) if needed for freqs > 48kHz
  t_uint16        iEnableSBR;	// STATIC range [0,1,2,3,4,5,6,7] xxxxxxxxxxxxxLAE (default 0)
  								// Bit 0(E) = EnableSBR
								// Bit 1(A) = Enable_SBR_Auto_Decimation_for_fs_above_24kHz
								// Bit 2(L) = Enable_SBR_Low_Power
  								// ---------------------------------------------------------------------------------------
  								// E=0=sbr not decoded with fs_out=fs_aac_core if iDownSample=0  
  								// else fs_out=fs_aac_core/2 if iDownSample=1;
								// ---------------------------------------------------------------------------------------
								// E=1=sbr decoded with fs_out=2*fs_aac_core if iDownSample=0 else 1*fs_aac_core if 
								// iDownSample=1
								// ---------------------------------------------------------------------------------------
								// ------------------------------------------------------------------------------------------
								// A=1=sbr decoded with fs_out=2*fs_aac_core only if (fs_aac_core<=24kHz and iDownSample ==0)
								// ------------------------------------------------------------------------------------------
								// L=1=sbr decoded in Low Power mode for non_parametric stereo stereo streams
								// ------------------------------------------------------------------------------------------
  t_uint16        iErrorConcealment;	// STATIC values 0(off), 1(mute), 2(repeat), 3(adaptive), 4(3gpp), default 0(off)
  t_uint16        ibsac_on;	// STATCC rangw[0,1], default 0
  t_uint16        ibsac_nch;	// STATIC (needed if bsac_on = 1) : range [1, 2]
  t_uint16        ibsac_layer;	// STATIC (needed if bsac_on and bsac_use_max_layer=0): range [0, 48]
  t_uint16        ibsac_usemaxlayer;	// STATIC (needed if bsac_on) : range [0, 1]
} t_dec_aac_params;


#define ESAA_AAC_MAX_BITSTREAM_SIZE_IN_BIT                      (15544)  // theory is 12288 but we need to increase size to pass some corrupted stream
#define ESAA_AAC_SAMPLE_FRAME_SIZE_IN_WORD                      1024*2
#define ESAA_EAACPLUS_SAMPLE_FRAME_SIZE_IN_WORD                 2048*2

#endif
