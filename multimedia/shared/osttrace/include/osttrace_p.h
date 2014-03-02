/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __OSTTRACE_P_H__
#define __OSTTRACE_P_H__

#ifdef __ARM_LINUX
#include "stm-trace.h"
#endif

/* RVCT and GCC compilers defines __arm__
   X86 GCC defines __linux__ */
#if (defined(__arm__) || defined(__linux__))

#define MASK_INTERRUPTS		do { ; /* nothing */ } while (0)
#define UNMASK_INTERRUPTS	do { ; /* nothing */ } while (0)

#else

#include <inc/typedef.h>
#define MASK_INTERRUPTS		do { EMU_unit_maskit = 1; } while (0)
#define UNMASK_INTERRUPTS	do { EMU_unit_maskit = 0; } while (0)

#endif

#define MAX_TRACE_CHUNK_SIZE 256

typedef struct {
	unsigned char master_id;
	unsigned char version_byte;
	unsigned char entity_id;
	unsigned char protocol_id;
	unsigned char length_byte;
	unsigned char timestamp [8];
	unsigned char component_id [4];
	unsigned char group_id [2];
	unsigned char trace_id [2];
	unsigned char btrace_header [4];
	unsigned char payload [20]; 
} t_nml_hdr;

typedef struct {
	unsigned char master_id;
	unsigned char version_byte;
	unsigned char entity_id;
	unsigned char protocol_id;
	unsigned char length_byte;
	unsigned char ext_length [4];
	unsigned char timestamp [8];
	unsigned char component_id [4];
	unsigned char group_id [2];
	unsigned char trace_id [2];
	unsigned char btrace_header [4];
	unsigned char payload [20]; 
} t_nml_hdr_ext;


#if (defined(__arm__) || defined(__linux__))

  #define MASTER_TRACE_ID       0x08	// for OST

  #define VERSION_BYTE		0x05	// we've been told that!

  #define PROTOCOL_ID_ASCII	0x02	// for bare ASCII messages data
  #define PROTOCOL_ID_BINARY	0x03	// OST is a binary protocol

  #define STM_BASE_ADDR         0x80100000
  #define STM_NUMBER_OF_CHANNEL 256

  #ifdef __ARM_THINK

typedef struct
{
	unsigned int    osmo;
	unsigned int    reserved_0; /* 64 bits padding */
	unsigned int    osmot;
	unsigned int    reserved_1; /* 64 bits padding */
} t_stm_msg_reg;

typedef struct
{
	t_stm_msg_reg channel[STM_NUMBER_OF_CHANNEL];
} t_stm_channel;

static t_stm_channel *volatile pStmMsgReg = (t_stm_channel *)(STM_BASE_ADDR);

#endif /* __ARM_THINK */

#else /* #if (defined(__arm__) || defined(__linux__)) */

/* MMDSP case */

#define STM_NUM_CHANNEL 256

typedef struct {
	t_shared_reg osmo[2];
	t_shared_reg osmot[2];
} t_stm_msg_reg;

typedef struct
{
	t_stm_msg_reg channel[STM_NUM_CHANNEL];
} t_stm_channel;

#define MASTER_TRACE_ID		0x08	// for OST

#define VERSION_BYTE		0x05	// we've been told that!

#define PROTOCOL_ID_ASCII	0x02	// for bare ASCII messages data
#define PROTOCOL_ID_BINARY	0x03	// OST is a binary protocol

#endif /* #if (defined(__arm__) || defined(__linux__)) */

/**************************** FUNCTIONS ***************************/
#if (defined(__arm__) || defined(__linux__))

#ifdef	__ARM_THINK
static __inline void STM_Init_BaseAddr(unsigned int base_addr)
{
	pStmMsgReg  = (t_stm_channel *) base_addr;
}
#endif


static __inline void XtiSend_64(const unsigned int channel, const unsigned long long val)
{
#ifdef	__ARM_THINK
	pStmMsgReg->channel[channel].osmo = val;
#error Not supported by think	
#elif defined (__ARM_LINUX)
	stm_trace_64(channel, val);
#endif
}

static __inline void XtiSend_64T(const unsigned int channel, const unsigned long long val)
{
#ifdef	__ARM_THINK		
	pStmMsgReg->channel[channel].osmot = val;
#error Not supported by think		
#elif defined (__ARM_LINUX)
	stm_tracet_64(channel, val);
#endif

}

static __inline void XtiSend_32(const unsigned int channel, const unsigned int val)
{
#ifdef	__ARM_THINK
	pStmMsgReg->channel[channel].osmo = val;
#elif defined (__ARM_LINUX)
	stm_trace_32(channel, val);
#endif
}

static __inline void XtiSend_32T(const unsigned int channel, const unsigned int val)
{
#ifdef	__ARM_THINK
	pStmMsgReg->channel[channel].osmot = val;
#elif defined (__ARM_LINUX)
	stm_tracet_32(channel, val);
#endif

}

static __inline void XtiSend_16(const unsigned int channel, const unsigned short val)
{
#ifdef	__ARM_THINK
	*((volatile unsigned short*) &(pStmMsgReg->channel[channel].osmo)) = val;
#elif defined (__ARM_LINUX)
	stm_trace_16(channel, val);
#endif	
}

static __inline void XtiSend_16T(const unsigned int channel, const unsigned short val)
{
#ifdef	__ARM_THINK
	*((volatile unsigned short*) &(pStmMsgReg->channel[channel].osmot)) = val;
#elif defined (__ARM_LINUX)
	stm_tracet_16(channel, val);
#endif		
}

static __inline void XtiSend_8(const  int channel, const unsigned char val)
{
#ifdef	__ARM_THINK
	*((volatile unsigned char*) &(pStmMsgReg->channel[channel].osmo)) = val;
#elif defined (__ARM_LINUX)
	stm_trace_8(channel, val);
#endif
}

static __inline void XtiSend_8T(const  int channel, const unsigned char val)
{
#ifdef	__ARM_THINK
	*((volatile unsigned char*) &(pStmMsgReg->channel[channel].osmot)) = val;
#elif defined (__ARM_LINUX)
	stm_tracet_8(channel, val);
#endif
}

#endif /* #if (defined(__arm__) || defined(__linux__)) */

#endif /* __NOST_IMPL_H__ */
