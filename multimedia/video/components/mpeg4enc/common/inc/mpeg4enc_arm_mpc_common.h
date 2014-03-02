/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __MPEG4ENC_ARM_MPC_COMMON_H
#define __MPEG4ENC_ARM_MPC_COMMON_H

#define TOTAL_PORT_COUNT 2

#define INPUT_BUFFER_COUNT 32
#define OUTPUT_BUFFER_COUNT 32
#define STAB_ARRAY_SIZE 32

#ifdef __MPEG4ENC_ARM_NMF //for ARM-NMF

    #define convfrom16bitmode(x) x
	#define SVP_SHAREDMEM_FROM16(x) x
	#define SVP_SHAREDMEM_TO16(x) x
	#define SVP_SHAREDMEM_FROM24(x) x
	#define SVP_SHAREDMEM_TO24(x) x

	#undef SHAREDMEM
	#define SHAREDMEM

	#define PRINT_VAR(x)
	#define PUT_PRAGMA

#else //for MPC build

	#if !(defined(__svp8500_ed__) || defined(__svp8500_v1__))
		#define SVP_SHAREDMEM_FROM16(x) convfrom16bitmode(x)
		#define SVP_SHAREDMEM_TO16(x) convto16bitmode(x)
		#define SVP_SHAREDMEM_FROM24(x) x
		#define SVP_SHAREDMEM_TO24(x) x
	#else
		#define SVP_SHAREDMEM_FROM16(x) ((((x)>>16)&0xFFFFul)|(((x)<<16)&0xFFFF0000ul))
		#define SVP_SHAREDMEM_TO16(x) ((((x)>>16)&0xFFFFul)|(((x)<<16)&0xFFFF0000ul))
		#define SVP_SHAREDMEM_FROM24(x) ((((x)>>24)&0xFFFFFFul)|(((x)<<24)&0xFFFFFF000000ul))
		#define SVP_SHAREDMEM_TO24(x) ((((x)>>24)&0xFFFFFFul)|(((x)<<24)&0xFFFFFF000000ul))
	#endif //for !(defined(__svp8500_ed__) || defined(__svp8500_v1__))

	#define PUT_PRAGMA #pragma force_dcumode

#endif // for __MPEG4ENC_ARM_NMF

typedef enum
{
	SVA_BUFFERING_NONE,
	SVA_BUFFERING_VBV,
	SVA_BUFFERING_HRD,
	SVA_BUFFERING_ANNEXG
} t_sva_brc_buffering_model;

/*
 * Define the structure to handle temporal value for SH
 */
typedef struct
{
	t_uint32 tr;
	t_sint32 cumulTimeSlot;
	t_uint32 slotDelay;
} t_sva_ec_mp4_sh_temporal;

/*
 * Define the structure to handle temporal value for SP
 */
typedef struct
{
	/*remain of previous division when convert diff from 90000 Hz to vopTimeIncrementResolution Hz*/
	t_uint32 remainForOffset;
	t_uint32 moduloTimeBase;/*temporal data value*/
	t_uint32 vopTimeIncrement;
	t_uint32 vopTimeIncrementBitSize;/*save vopTimeIncrementBitSize*/
} t_sva_ec_mp4_sp_temporal;

/*
 * Define the type that keep all data need for skip fifo
 */
typedef struct
{
	t_sva_timestamp_value pts;
	t_uint32 pictureNb;
	t_uint16 roundValue;
	/* short header specific */
	t_uint16 pictureCodingType;
	t_sva_ec_mp4_sh_temporal temporalSh;
	t_uint16 gobFrameId;
	/* simple profile specific */
	t_sva_ec_mp4_sp_temporal temporalSp;
} t_sva_ec_save;


#endif	//for __MPEG4ENC_ARM_MPC_COMMON_H
