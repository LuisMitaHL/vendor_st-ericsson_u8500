/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <archi.h>
#include <inc/archi-wrapper.h>
#include "osttrace_p.h"
#include "ste_ost_group_map.h"
#include <osttrace/mmdsp.nmf>


t_uint16 ATTR(trace_mode) = 0;

/********************** MMDSP types ***********************
  Type       Arithmetic size     Storage size     Alignment
  (bits, mode 16/24)       (MAUs)         (MAUs)
  -----------------------------------------------------------
  char              8                 1               1
  short           16/24               1               1
  int             16/24               1               1
  long            32/48               2               1
  long long       40/56               3               1
  pointer         16/24               1               1
  float             32                2               1
  double            32                2               1
 **********************************************************/

#if 0
#define STM_DSP_SAA_OSMO_BASE                   0xF81000UL
#define STM_DSP_SVA_OSMO_BASE                   0xF81000UL
#define STM_DSP_SIA_OSMO_BASE                   0xF81800UL

#define ll_trace_16 write16
#define ll_tracet_16 write16T

#define ll_trace_32 write32
#define ll_tracet_32 write32
#endif

#define ll_trace_16 traceWrite16
#define ll_tracet_16 traceWrite16T

#define ll_trace_32 traceWrite32
#define ll_tracet_32 traceWrite32T

#define SAA_CORE 1
#define SVA_CORE 2
#define SIA_CORE 3
#define UNK_CORE 255


#ifdef __mode16__
#define DATA24_DSP_BASE_ADDR        0x10000UL
#define DATA16_DSP_BASE_ADDR        0x800000UL
#define MMIO_DSP_BASE_ADDR          0xF80000UL
#else /* __mode16__ -> __mode24__ */
#define DATA24_DSP_BASE_ADDR        0x10000U
#define DATA16_DSP_BASE_ADDR        0x800000U
#define MMIO_DSP_BASE_ADDR          0xF80000U
#endif /* __mode24__ */


#define SWAP_32(x, y) *x++ = y & 0x0000FFu; \
					  *x++ = (y & 0x00FF00u) >> 8; \
					  *x++ = (y & 0xFF0000u) >> 16; \
					  *x++ = (y & 0xFF000000ul) >> 24

#define MAKELONG(a, b, c, d) \
	(unsigned long) (    (((unsigned long) (a)) << 24)  | (((unsigned long) (b)) << 16)  | (((unsigned long) (c)) << 8)  | (((unsigned long) (d))))

#define MAKELONG_2(a, b, c, d) \
	(unsigned long) (    (((unsigned long) (a)) << 32)  | (((unsigned long) (b)) << 24)  | (((unsigned long) (c)) << 8)  | (((unsigned long) (d))))

#define MAX_OST_FRAME_SIZE	256
// 25 (ost header basis) + 4 (extended btrace header) + 4 (multi part extra) + 8 (OMX handle and ID) + 4 (array size)
#define MAX_OST_HEADER_SIZE	45

#define MAX_THREAD_PRIORITY	2

#define MPC_LOAD_IDLE_STATE                 0
#define MPC_LOAD_LOAD_STATE                 1

#define PRCMU_TIMER_4_DOWNCOUNT             ((0x57454>>1) + MMIO_DSP_BASE_ADDR)
#if ((defined(__STN_8500) && (__STN_8500 >= 30)) || defined(__STN_9540))
    #define PRCMU_SH_VAR_4                  ((0xB8DE4>>1) + MMIO_DSP_BASE_ADDR)
#elif defined(__STN_8500) && (__STN_8500 <= 20)
    #define PRCMU_SH_VAR_4                  ((0x5FDE4>>1) + MMIO_DSP_BASE_ADDR)
#endif

#define MULTIPART_ID	0x111111111111uL
unsigned long multipart_id = MULTIPART_ID;
const unsigned char t_xti_channel[3] = {0x43, 0x44, 0x45};

int myCoreId = UNK_CORE;


static unsigned char st_timestamp [8] = { 0xd0, 0x00, 0x03, 0x2f, 0x3e, 0x89, 0x6e, 0x3b };
static unsigned short ost_group_id [16] = { 
		TRACE_GROUP_BIT_MASK_0,
		TRACE_GROUP_BIT_MASK_1,
		TRACE_GROUP_BIT_MASK_2,
		TRACE_GROUP_BIT_MASK_3,
		TRACE_GROUP_BIT_MASK_4,
		TRACE_GROUP_BIT_MASK_5,
		TRACE_GROUP_BIT_MASK_6,
		TRACE_GROUP_BIT_MASK_7,
		TRACE_GROUP_BIT_MASK_8,
		TRACE_GROUP_BIT_MASK_9,
		TRACE_GROUP_BIT_MASK_10,
		TRACE_GROUP_BIT_MASK_11,
		TRACE_GROUP_BIT_MASK_12,
		TRACE_GROUP_BIT_MASK_13,
		TRACE_GROUP_BIT_MASK_14,
		TRACE_GROUP_BIT_MASK_15,
		};



static int write_ost_chunk(unsigned char *buffer, unsigned int size, unsigned char enable_ts, unsigned char *is_remain_byte, unsigned char* remain_byte);
static int write_ost_chunk_ext(unsigned int *buffer, unsigned int size, unsigned char *is_remain_byte, unsigned char* remain_byte);
static t_uint56 getCurrentTimerValue(void);


/* PRIVATE function */
/* get current timer value */
static t_uint56 getCurrentTimerValue()
{
    t_uint32 msbBefore;
    t_uint32 lsb;
    t_uint32 msbAfter;
    t_uint56 res;

    /* read prcmu timers */
    msbBefore = ~(*((__EXTMMIO volatile t_uint32 *)PRCMU_SH_VAR_4));
    lsb = ~(*((__EXTMMIO volatile t_uint32 *)PRCMU_TIMER_4_DOWNCOUNT));
    msbAfter = ~(*((__EXTMMIO volatile t_uint32 *)PRCMU_SH_VAR_4));

    lsb = (lsb & 0xffffffffuL); // Not a 32 bit value but a 48 bits
    /* handle rollover test case */
    // NOTE : there is still a window in prcmu side between counter rollover and prcmu interrupt handling
    // to update msb register => this can lead to erroneous value return here
    if (msbBefore == msbAfter || lsb >= 0x80000000UL)
        res = ((msbBefore & 0xffffffUL) << 32) + lsb;
    else
        res = ((msbAfter & 0xffffffUL) << 32) + lsb;

    return res;
}


asm unsigned int wgetGmode(void)
{
	xmv	Flag, @{}
}


/***************************************************************************/
t_nmf_error METH(construct)(void)
{
	unsigned char myid;

	myid = HOST [0x02]; 

	if ((myid & 0x80) == 0) {
		if      (HOST[4] == 0x47) myCoreId = SAA_CORE;
		else if (HOST[4] == 0x57) myCoreId = SVA_CORE;
		else myCoreId = UNK_CORE;
	} else {
		if      (myid == 0x8A) myCoreId = SAA_CORE;
		else if (myid == 0x9A) myCoreId = SVA_CORE;
		else if (myid == 0xAA) myCoreId = SIA_CORE;
		else myCoreId = UNK_CORE;
	}	
	return NMF_OK;
}

#pragma force_dcumode
////////////////////////////////////////////////////////////////////////////
// add_ost_header
////////////////////////////////////////////////////////////////////////////
static int add_ost_header (t_nml_hdr *p_header, TRACEMSGID_TYPE componentId, TRACEMSGID_TYPE tracemsgId, unsigned short has_handle, unsigned long has_array_size, unsigned long has_multipart, unsigned int aComponentHandle, TRACEMSGID_TYPE aParentHandle, unsigned int mode)
{
	unsigned char *p;
	unsigned int i;
	unsigned int size = 0;
	unsigned int group;
	unsigned int Eheader2;
//  t_uint56 timerValue = getCurrentTimerValue();


	// populate the header
	p_header->master_id    = MASTER_TRACE_ID;
	p_header->version_byte = VERSION_BYTE;
	p_header->entity_id    = OST_TRACE_ENTITY;
	p_header->protocol_id  = PROTOCOL_ID_BINARY;
//	header.length_byte  = sizeof (header) - 5 /* [master_id..length_byte] */ + 0 /* data bytes*/;
 	p_header->timestamp [0] = 0;
	for (i = 1; i < 8; ++i)
		p_header->timestamp [i] = st_timestamp [i];
// 		p_header->timestamp [i] = (timerValue >> (56-8*i)) & 0xFFu;

//	NmfPrint1(EE_DEBUG_LEVEL_INFO, "mode: 0x%x\n", mode);

	// We are called from Mode16 
	if (mode & 0x80)
	{
		p_header->component_id [0] = (componentId & 0xFF00000000ul) >> 32;
		//NmfPrint1(EE_DEBUG_LEVEL_INFO, "Component_id Byte 0: 0x%x\n", p_header->component_id [0]);
		p_header->component_id [1] = (componentId & 0xFF000000ul) >> 24;
		//NmfPrint1(EE_DEBUG_LEVEL_INFO, "Component_id Byte 1: 0x%x\n", p_header->component_id [1]);

		group = (tracemsgId & 0xFFFF000000ul) >> 24;
		//NmfPrint1(EE_DEBUG_LEVEL_INFO, "group 1: 0x%x\n", group);
		group = wedge(group);
		group = 23 - group - 1;
		//NmfPrint1(EE_DEBUG_LEVEL_INFO, "group 2: 0x%x\n", group);
	}
	else
	{
		p_header->component_id [0] = (componentId & 0xFF000000ul) >> 24;
		p_header->component_id [1] = (componentId & 0xFF0000ul) >> 16;
		group = (tracemsgId & 0xFFFF0000ul) >> 16;
		group = wedge(group);
		group = 23 - group - 1;
	}

	p_header->component_id [2] = (componentId & 0x00FF00ul) >> 8;
	//NmfPrint1(EE_DEBUG_LEVEL_INFO, "Component_id Byte 2: 0x%x\n", p_header->component_id [2]);
	p_header->component_id [3] = (componentId & 0x0000FFul);
	//NmfPrint1(EE_DEBUG_LEVEL_INFO, "Component_id Byte 3: 0x%x\n", p_header->component_id [3]);



	group = ost_group_id[group];
	//NmfPrint1(EE_DEBUG_LEVEL_INFO, "group 3: 0x%x\n", group);

	p_header->group_id [0] = (group & 0xFF00u) >> 8;
	p_header->group_id [1] = (group & 0xFFu);

	// we consider only the 16 LS bits
	p_header->trace_id [0] = (tracemsgId & 0xFF00u) >> 8;
	p_header->trace_id [1] = (tracemsgId & 0x00FFu);
	//NmfPrint1(EE_DEBUG_LEVEL_INFO, "trace ID: 0x%x\n", (unsigned int)(tracemsgId & 0xFFFFu));		


	// btrace_header combines length, flags?, category and subcategory
	p_header->btrace_header [0] = 4; // length
	p_header->btrace_header [1] = 0; // flags == "no other field than data is present"
	p_header->btrace_header [2] = (group & 0xFFu);
	p_header->btrace_header [3] = KBTraceSubCategoryOpenSystemTrace & 0xFFu;
	
	size = 25;
	p = (unsigned char*)(p_header->payload);

	if (has_multipart >> 2)
	{
		// Update btrace size to cope with 4 bytes Eheader2 + 4 bytes EExtra data
		p_header->btrace_header [0] += 8; // length

		// Update header with EExtraPresent and EHeader2Present
		p_header->btrace_header [1] |= 0x21; 

		// Add Eheader2
		SWAP_32(p, has_multipart & 3);

		// Add Multi part ID
		SWAP_32(p, has_multipart >> 2);

		size += 8;
	}
	

	if (has_handle)
	{
		// Add Parent Handle
		*p++ = (aParentHandle & 0xFF000000ul) >> 24;
		*p++ = (aParentHandle & 0xFF00000000ul) >> 32;
		*p++ = (aParentHandle & 0x0000FFul);
	 	*p++ = (aParentHandle & 0x00FF00ul) >> 8;

		// Add This Handle
		*p++ = (aComponentHandle & 0x00FFu);
		*p++ = (aComponentHandle & 0xFF00u) >> 8;

		if (mode & 0x80)
		{
			*p++ = 0;
		}
		else
		{	
			*p++ = (aComponentHandle & 0xFF0000u) >> 16;
		}
		*p++ = 0;

		// Update length with Parent Handle and ID		
		p_header->btrace_header [0] += 8; // length
		size += 8;

		// To be compatible with OST SF we need to provide a size information when
		// 1 or more 32-bit word is followed by a pointe
		// The size of the pointer must then be provided just before the data pointed
		if (has_array_size)
		{
			// Update btrace size to cope with 2 more 32-bit word	
			p_header->btrace_header [0] += 4; // length

			// Add array_size	
			SWAP_32(p, has_array_size);
			size += 4;
		}
		
	}

	return size;

}

#pragma force_dcumode
////////////////////////////////////////////////////////////////////////////
// add_ost_header
////////////////////////////////////////////////////////////////////////////
static int add_ost_header_ext (t_nml_hdr_ext *p_header, TRACEMSGID_TYPE componentId, TRACEMSGID_TYPE tracemsgId, unsigned short has_handle, unsigned long has_array_size, unsigned int aComponentHandle, TRACEMSGID_TYPE aParentHandle, unsigned int mode)
{
	unsigned char *p;
	unsigned int i;
	unsigned int size = 0;
	unsigned int group;
	unsigned int Eheader2;
//  t_uint56 timerValue = getCurrentTimerValue();


	// populate the header
	p_header->master_id    = MASTER_TRACE_ID;
	p_header->version_byte = VERSION_BYTE;
	p_header->entity_id    = OST_TRACE_ENTITY;
	p_header->protocol_id  = PROTOCOL_ID_BINARY;
	p_header->length_byte  = 0; 	/* Shall be set to zero in case extended length is available */ 
 	p_header->timestamp [0] = 0;
	for (i = 1; i < 8; ++i)
		p_header->timestamp [i] = st_timestamp [i];
// 		p_header->timestamp [i] = (timerValue >> (56-8*i)) & 0xFFu;

//	NmfPrint1(EE_DEBUG_LEVEL_INFO, "mode: 0x%x\n", mode);

	// We are called from Mode16 
	if (mode & 0x80)
	{
		p_header->component_id [0] = (componentId & 0xFF00000000ul) >> 32;
		//NmfPrint1(EE_DEBUG_LEVEL_INFO, "Component_id Byte 0: 0x%x\n", p_header->component_id [0]);
		p_header->component_id [1] = (componentId & 0xFF000000ul) >> 24;
		//NmfPrint1(EE_DEBUG_LEVEL_INFO, "Component_id Byte 1: 0x%x\n", p_header->component_id [1]);

		group = (tracemsgId & 0xFFFF000000ul) >> 24;
		//NmfPrint1(EE_DEBUG_LEVEL_INFO, "group 1: 0x%x\n", group);
		group = wedge(group);
		group = 23 - group - 1;
		//NmfPrint1(EE_DEBUG_LEVEL_INFO, "group 2: 0x%x\n", group);
	}
	else
	{
		p_header->component_id [0] = (componentId & 0xFF000000ul) >> 24;
		p_header->component_id [1] = (componentId & 0xFF0000ul) >> 16;
		group = (tracemsgId & 0xFFFF0000ul) >> 16;
		group = wedge(group);
		group = 23 - group - 1;
	}

	p_header->component_id [2] = (componentId & 0x00FF00ul) >> 8;
	//NmfPrint1(EE_DEBUG_LEVEL_INFO, "Component_id Byte 2: 0x%x\n", p_header->component_id [2]);
	p_header->component_id [3] = (componentId & 0x0000FFul);
	//NmfPrint1(EE_DEBUG_LEVEL_INFO, "Component_id Byte 3: 0x%x\n", p_header->component_id [3]);



	group = ost_group_id[group];
	//NmfPrint1(EE_DEBUG_LEVEL_INFO, "group 3: 0x%x\n", group);

	p_header->group_id [0] = (group & 0xFF00u) >> 8;
	p_header->group_id [1] = (group & 0xFFu);

	// we consider only the 16 LS bits
	p_header->trace_id [0] = (tracemsgId & 0xFF00u) >> 8;
	p_header->trace_id [1] = (tracemsgId & 0x00FFu);
	//NmfPrint1(EE_DEBUG_LEVEL_INFO, "trace ID: 0x%x\n", (unsigned int)(tracemsgId & 0xFFFFu));


	// btrace_header combines length, flags?, category and subcategory
	p_header->btrace_header [0] = 0xFF; /* Set to 0xFF in case of extended length */
	p_header->btrace_header [1] = 0x0; /* Update header with EExtraPresent and EHeader2Present */
	p_header->btrace_header [2] = (group & 0xFFu);
	p_header->btrace_header [3] = KBTraceSubCategoryOpenSystemTrace & 0xFFu;
	
	size = 29;
	p = (unsigned char*)(p_header->payload);


	// EHeader2Present is made of EMultipartFlagMask = 3<<0 and ECpuIdMask = 0xfff<<20
	// Add Eheader2 with multipart flag set to zero for extended length
	//SWAP_32(p, (myCoreId <<20));

	// Add Extra with multipart ID
	//SWAP_32(p, multipart_id);

	// take into account EHeader2Present + Extra field
	//size += 8;
	

	if (has_handle)
	{
		//Parent Handle
		*p++ = (aParentHandle & 0xFF000000ul) >> 24;
		*p++ = (aParentHandle & 0xFF00000000ul) >> 32;

 		*p++ = (aParentHandle & 0x0000FFul);
	 	*p++ = (aParentHandle & 0x00FF00ul) >> 8;
	
		// Add This Handle
		*p++ = (aComponentHandle & 0x00FFu);
		*p++ = (aComponentHandle & 0xFF00u) >> 8;
	
		if (mode & 0x80)
		{
			*p++ = 0;
		}
		else
		{
			*p++ = (aComponentHandle & 0xFF0000u) >> 16;
		}
		*p++ = 0;

		size += 8;

		// To be compatible with OST SF we need to provide a size information when
		// 1 or more 32-bit word is followed by a pointe
		// The size of the pointer must then be provided just before the data pointed
		if (has_array_size)
		{
			// Add array_size
			SWAP_32(p, has_array_size);
			size += 4;
		}
		
	}

	return size;

}
#pragma force_dcumode
void OSTTrace0_arm (TRACEMSGID_TYPE componentId, TRACEMSGID_TYPE tracemsgId, unsigned short has_handle, unsigned int aComponentHandle, TRACEMSGID_TYPE aParentHandle, unsigned int mode)
{
	unsigned int group;

	struct t_nmf_trace t_ost_to_arm;
	if (mode & 0x80)
	{

		group = (tracemsgId & 0xFFFF000000ul) >> 24;
		group = wedge(group);
		group = 23 - group - 1;
		group = ost_group_id[group];
		t_ost_to_arm.traceId = (tracemsgId & 0xFFFFul) | ((unsigned long)group << 24);
		t_ost_to_arm.componentId = componentId;
		t_ost_to_arm.componentHandle = aComponentHandle;
		t_ost_to_arm.parentHandle = aParentHandle;
		t_ost_to_arm.parentHandle = ((aParentHandle & 0xFFFFul) << 24) | ((aParentHandle >> 24));
	}
	else
	{
		group = (tracemsgId & 0xFFFF0000ul) >> 16;
		group = wedge(group);
		group = 23 - group - 1;
		group = ost_group_id[group];
		t_ost_to_arm.traceId = (tracemsgId & 0xFFFFul) | ((unsigned long)group << 24);
		t_ost_to_arm.componentId = (componentId & 0xFFFFul) | ((componentId >> 16) << 24);
		t_ost_to_arm.componentHandle = ((((unsigned long) aComponentHandle) >> 16) << 24) | (aComponentHandle & 0xFFFFul);
		t_ost_to_arm.parentHandle = ((aParentHandle & 0xFFFFul) << 24) | ((aParentHandle >> 24));
	}

	t_ost_to_arm.paramOpt = ((unsigned long)has_handle << 24);

	NmfOstTrace(&t_ost_to_arm, (t_bool) has_handle, 0);
}

#pragma force_dcumode
void OSTTrace1_arm (TRACEMSGID_TYPE componentId, TRACEMSGID_TYPE tracemsgId, unsigned short has_handle, unsigned int aComponentHandle, TRACEMSGID_TYPE aParentHandle, unsigned int mode, unsigned int param)
{
	unsigned int group;

	struct t_nmf_trace t_ost_to_arm;
	if (mode & 0x80)
	{

		group = (tracemsgId & 0xFFFF000000ul) >> 24;
		group = wedge(group);
		group = 23 - group - 1;
		group = ost_group_id[group];
		t_ost_to_arm.traceId = (tracemsgId & 0xFFFFul) | ((unsigned long)group << 24);
		t_ost_to_arm.componentId = componentId;
		t_ost_to_arm.componentHandle = aComponentHandle;
		t_ost_to_arm.parentHandle = ((aParentHandle & 0xFFFFul) << 24) | ((aParentHandle >> 24));
		t_ost_to_arm.params[0] = param;
	}
	else
	{
		group = (tracemsgId & 0xFFFF0000ul) >> 16;
		group = wedge(group);
		group = 23 - group - 1;
		group = ost_group_id[group];
		t_ost_to_arm.traceId = (tracemsgId & 0xFFFFul) | ((unsigned long)group << 24);
		t_ost_to_arm.componentId = (componentId & 0xFFFFul) | ((componentId >> 16) << 24);
		t_ost_to_arm.componentHandle = ((((unsigned long) aComponentHandle) >> 16) << 24) | (aComponentHandle & 0xFFFFul);
		t_ost_to_arm.parentHandle = ((aParentHandle & 0xFFFFul) << 24) | ((aParentHandle >> 24));
		t_ost_to_arm.params[0] = ((((unsigned long) param) >> 16) << 24) | (param & 0xFFFFul);
	}

	t_ost_to_arm.paramOpt = (1ul) | ((unsigned long)has_handle << 24);

	NmfOstTrace(&t_ost_to_arm, (t_bool) has_handle, 1);
}

#pragma force_dcumode
void OSTTrace2_arm (TRACEMSGID_TYPE componentId, TRACEMSGID_TYPE tracemsgId, unsigned short has_handle, unsigned int aComponentHandle, TRACEMSGID_TYPE aParentHandle, unsigned int mode, unsigned int param1, unsigned int param2)
{
	unsigned int group;

	struct t_nmf_trace t_ost_to_arm;
	if (mode & 0x80)
	{

		group = (tracemsgId & 0xFFFF000000ul) >> 24;
		group = wedge(group);
		group = 23 - group - 1;
		group = ost_group_id[group];
		t_ost_to_arm.traceId = (tracemsgId & 0xFFFFul) | ((unsigned long)group << 24);
		t_ost_to_arm.componentId = componentId;
		t_ost_to_arm.componentHandle = aComponentHandle;
		t_ost_to_arm.parentHandle = ((aParentHandle & 0xFFFFul) << 24) | ((aParentHandle >> 24));
		t_ost_to_arm.params[0] = param1;
		t_ost_to_arm.params[1] = param2;
	}
	else
	{
		group = (tracemsgId & 0xFFFF0000ul) >> 16;
		group = wedge(group);
		group = 23 - group - 1;
		group = ost_group_id[group];
		t_ost_to_arm.traceId = (tracemsgId & 0xFFFFul) | ((unsigned long)group << 24);
		t_ost_to_arm.componentId = (componentId & 0xFFFFul) | ((componentId >> 16) << 24);
		t_ost_to_arm.componentHandle = ((((unsigned long) aComponentHandle) >> 16) << 24) | (aComponentHandle & 0xFFFFul);
		t_ost_to_arm.parentHandle = ((aParentHandle & 0xFFFFul) << 24) | ((aParentHandle >> 24));
		t_ost_to_arm.params[0] = ((((unsigned long) param1) >> 16) << 24) | (param1 & 0xFFFFul);
		t_ost_to_arm.params[1] = ((((unsigned long) param2) >> 16) << 24) | (param2 & 0xFFFFul);
	}

	t_ost_to_arm.paramOpt = (2ul) | ((unsigned long)has_handle << 24);
	NmfOstTrace(&t_ost_to_arm, (t_bool) has_handle, 2);
}

#pragma force_dcumode
void OSTTrace3_arm (TRACEMSGID_TYPE componentId, TRACEMSGID_TYPE tracemsgId, unsigned short has_handle, unsigned int aComponentHandle, TRACEMSGID_TYPE aParentHandle, unsigned int mode, unsigned int param1, unsigned int param2, unsigned int param3)
{
	unsigned int group;

	struct t_nmf_trace t_ost_to_arm;
	if (mode & 0x80)
	{

		group = (tracemsgId & 0xFFFF000000ul) >> 24;
		group = wedge(group);
		group = 23 - group - 1;
		group = ost_group_id[group];
		t_ost_to_arm.traceId = (tracemsgId & 0xFFFFul) | ((unsigned long)group << 24);
		t_ost_to_arm.componentId = componentId;
		t_ost_to_arm.componentHandle = aComponentHandle;
		t_ost_to_arm.parentHandle = ((aParentHandle & 0xFFFFul) << 24) | ((aParentHandle >> 24));
		t_ost_to_arm.params[0] = param1;
		t_ost_to_arm.params[1] = param2;
		t_ost_to_arm.params[2] = param3;
	}
	else
	{
		group = (tracemsgId & 0xFFFF0000ul) >> 16;
		group = wedge(group);
		group = 23 - group - 1;
		group = ost_group_id[group];
		t_ost_to_arm.traceId = (tracemsgId & 0xFFFFul) | ((unsigned long)group << 24);
		t_ost_to_arm.componentId = (componentId & 0xFFFFul) | ((componentId >> 16) << 24);
		t_ost_to_arm.componentHandle = ((((unsigned long) aComponentHandle) >> 16) << 24) | (aComponentHandle & 0xFFFFul);
		t_ost_to_arm.parentHandle = ((aParentHandle & 0xFFFFul) << 24) | ((aParentHandle >> 24));
		t_ost_to_arm.params[0] = ((((unsigned long) param1) >> 16) << 24) | (param1 & 0xFFFFul);
		t_ost_to_arm.params[1] = ((((unsigned long) param2) >> 16) << 24) | (param2 & 0xFFFFul);
		t_ost_to_arm.params[2] = ((((unsigned long) param3) >> 16) << 24) | (param3 & 0xFFFFul);
	}

	t_ost_to_arm.paramOpt = (3ul) | ((unsigned long)has_handle << 24);
	NmfOstTrace(&t_ost_to_arm, (t_bool) has_handle, 3);
}

#pragma force_dcumode
void OSTTrace4_arm (TRACEMSGID_TYPE componentId, TRACEMSGID_TYPE tracemsgId, unsigned short has_handle, unsigned int aComponentHandle, TRACEMSGID_TYPE aParentHandle, unsigned int mode, unsigned int param1, unsigned int param2, unsigned int param3, unsigned int param4)
{
	unsigned int group;

	struct t_nmf_trace t_ost_to_arm;
	if (mode & 0x80)
	{

		group = (tracemsgId & 0xFFFF000000ul) >> 24;
		group = wedge(group);
		group = 23 - group - 1;
		group = ost_group_id[group];
		t_ost_to_arm.traceId = (tracemsgId & 0xFFFFul) | ((unsigned long)group << 24);
		t_ost_to_arm.componentId = componentId;
		t_ost_to_arm.componentHandle = aComponentHandle;
		t_ost_to_arm.parentHandle = ((aParentHandle & 0xFFFFul) << 24) | ((aParentHandle >> 24));
		t_ost_to_arm.params[0] = param1;
		t_ost_to_arm.params[1] = param2;
		t_ost_to_arm.params[2] = param3;
		t_ost_to_arm.params[3] = param4;
	}
	else
	{
		group = (tracemsgId & 0xFFFF0000ul) >> 16;
		group = wedge(group);
		group = 23 - group - 1;
		group = ost_group_id[group];
		t_ost_to_arm.traceId = (tracemsgId & 0xFFFFul) | ((unsigned long)group << 24);
		t_ost_to_arm.componentId = (componentId & 0xFFFFul) | ((componentId >> 16) << 24);
		t_ost_to_arm.componentHandle = ((((unsigned long) aComponentHandle) >> 16) << 24) | (aComponentHandle & 0xFFFFul);
		t_ost_to_arm.parentHandle = ((aParentHandle & 0xFFFFul) << 24) | ((aParentHandle >> 24));
		t_ost_to_arm.params[0] = ((((unsigned long) param1) >> 16) << 24) | (param1 & 0xFFFFul);
		t_ost_to_arm.params[1] = ((((unsigned long) param2) >> 16) << 24) | (param2 & 0xFFFFul);
		t_ost_to_arm.params[2] = ((((unsigned long) param3) >> 16) << 24) | (param3 & 0xFFFFul);
		t_ost_to_arm.params[3] = ((((unsigned long) param4) >> 16) << 24) | (param4 & 0xFFFFul);
	}

	t_ost_to_arm.paramOpt = (4ul) | ((unsigned long)has_handle << 24);
	NmfOstTrace(&t_ost_to_arm, (t_bool) has_handle, 4);
}



////////////////////////////////////////////////////////////////////////////
// OSTTrace0
////////////////////////////////////////////////////////////////////////////
void OSTTrace0 (TRACEMSGID_TYPE componentId, TRACEMSGID_TYPE tracemsgId, unsigned short has_handle, unsigned int aComponentHandle, TRACEMSGID_TYPE aParentHandle)
{
	unsigned int mode = wgetGmode();

	if (ATTR(trace_mode))
	{
		OSTTrace0_arm(componentId, tracemsgId, has_handle, aComponentHandle, aParentHandle, mode);
	}
	else
	{

	// for this simple type of OST trace, we know from the beginning
	// that a normal header is enough
	t_nml_hdr buffer;
	unsigned int size = 0;
	unsigned char is_remain_byte = 0;
	unsigned char remain_byte;

 	size = add_ost_header (&buffer, componentId, tracemsgId, has_handle, 0, 0, aComponentHandle, aParentHandle, mode);
	buffer.length_byte  = size - 5;

	//ENTER_CRITICAL_SECTION;

	write_ost_chunk ((unsigned char*) &buffer, size, 1, &is_remain_byte, &remain_byte);

	//EXIT_CRITICAL_SECTION;

	}
}


////////////////////////////////////////////////////////////////////////////
// OSTTrace1
////////////////////////////////////////////////////////////////////////////
void OSTTrace1 (TRACEMSGID_TYPE componentId, TRACEMSGID_TYPE tracemsgId, unsigned int param, unsigned short has_handle, unsigned int aComponentHandle, TRACEMSGID_TYPE aParentHandle)
{
	unsigned int mode = wgetGmode();

	if (ATTR(trace_mode))
	{
		OSTTrace1_arm(componentId, tracemsgId, has_handle, aComponentHandle, aParentHandle, mode, param);
	}
	else
	{

	// for this simple type of OST trace, we know from the beginning
	// that a normal header is enough
	t_nml_hdr buffer;
	unsigned char *p;
	unsigned int size = 0;
	unsigned char is_remain_byte = 0;
	unsigned char remain_byte;
	

	size = add_ost_header (&buffer, componentId, tracemsgId, has_handle, 0, 0, aComponentHandle, aParentHandle, mode);
	buffer.btrace_header [0] += 4; // length
	// Adding 4 bytes from Param
	buffer.length_byte  = (size + 4) - 5;

	// Transmit OST header
	//ENTER_CRITICAL_SECTION;
	write_ost_chunk ((unsigned char*) &buffer, size, 0, &is_remain_byte, &remain_byte);
	//EXIT_CRITICAL_SECTION;

	// Reuse buffer since header already send
	p = ((unsigned char*)&buffer);
	SWAP_32(p, param);
	size = 4;
	if (is_remain_byte)
		size++;

	// Transmit 32-bit param
	//ENTER_CRITICAL_SECTION;
	write_ost_chunk ((unsigned char*) &buffer, size, 1, &is_remain_byte, &remain_byte);	
	//EXIT_CRITICAL_SECTION;
	}

}

////////////////////////////////////////////////////////////////////////////
// OSTTrace2
////////////////////////////////////////////////////////////////////////////
void OSTTrace2 (TRACEMSGID_TYPE componentId, TRACEMSGID_TYPE tracemsgId, unsigned int param1, unsigned int param2, unsigned short has_handle, unsigned int aComponentHandle, TRACEMSGID_TYPE aParentHandle)
{
	unsigned int mode = wgetGmode();

	if (ATTR(trace_mode))
	{
		OSTTrace2_arm(componentId, tracemsgId, has_handle, aComponentHandle, aParentHandle, mode, param1, param2);
	}
	else
	{

	t_nml_hdr buffer;
	unsigned char *p;
	unsigned int size = 0;
	unsigned char is_remain_byte = 0;
	unsigned char remain_byte;
	

	size = add_ost_header (&buffer, componentId, tracemsgId, has_handle, 0, 0, aComponentHandle, aParentHandle, mode);
	buffer.btrace_header [0] += 8; // length

	// Adding 8 bytes from Param
	buffer.length_byte  = (size + 8) - 5;

	// Transmit OST header
	//ENTER_CRITICAL_SECTION;
	write_ost_chunk ((unsigned char*) &buffer, size, 0, &is_remain_byte, &remain_byte);
	//EXIT_CRITICAL_SECTION;

	// Reuse buffer since header already send
	p = ((unsigned char*)&buffer);
	SWAP_32(p, param1);
	SWAP_32(p, param2);
	size = 8;
	if (is_remain_byte)
		size++;

	// Transmit 32-bit param1 and 32-bit param2
	//ENTER_CRITICAL_SECTION;
	write_ost_chunk ((unsigned char*) &buffer, size, 1, &is_remain_byte, &remain_byte);	
	//EXIT_CRITICAL_SECTION;
	}
}

////////////////////////////////////////////////////////////////////////////
// OSTTrace3
////////////////////////////////////////////////////////////////////////////
void OSTTrace3 (TRACEMSGID_TYPE componentId, TRACEMSGID_TYPE tracemsgId, unsigned int param1, unsigned int param2, unsigned int param3, unsigned short has_handle, unsigned int aComponentHandle, TRACEMSGID_TYPE aParentHandle)
{
	unsigned int mode = wgetGmode();

	if (ATTR(trace_mode))
	{
		OSTTrace3_arm(componentId, tracemsgId, has_handle, aComponentHandle, aParentHandle, mode, param1, param2, param3);
	}
	else
	{
	t_nml_hdr buffer;
	unsigned char *p;
	unsigned int size = 0;
	unsigned char is_remain_byte = 0;
	unsigned char remain_byte;

	size = add_ost_header (&buffer, componentId, tracemsgId, has_handle, 0, 0, aComponentHandle, aParentHandle, mode);
	buffer.btrace_header [0] += 12; // length

	// Adding 12 bytes from Param
	buffer.length_byte  = (size + 12) - 5;

	// Transmit OST header
	//ENTER_CRITICAL_SECTION;
	write_ost_chunk ((unsigned char*) &buffer, size, 0, &is_remain_byte, &remain_byte);
	//EXIT_CRITICAL_SECTION;

	// Reuse buffer since header already send
	p = ((unsigned char*)&buffer);
	SWAP_32(p, param1);
	SWAP_32(p, param2);
	SWAP_32(p, param3);
	size = 12;
	if (is_remain_byte)
		size++;

	// Transmit 32-bit param1, param2, param3
	//ENTER_CRITICAL_SECTION;
	write_ost_chunk ((unsigned char*) &buffer, size, 1, &is_remain_byte, &remain_byte);	
	//EXIT_CRITICAL_SECTION;
	}
}

////////////////////////////////////////////////////////////////////////////
// OSTTrace4
////////////////////////////////////////////////////////////////////////////
void OSTTrace4 (TRACEMSGID_TYPE componentId, TRACEMSGID_TYPE tracemsgId, unsigned int param1, unsigned int param2, unsigned int param3, unsigned int param4, unsigned short has_handle, unsigned int aComponentHandle, TRACEMSGID_TYPE aParentHandle)
{
	unsigned int mode = wgetGmode();

	if (ATTR(trace_mode))
	{
		OSTTrace4_arm(componentId, tracemsgId, has_handle, aComponentHandle, aParentHandle, mode, param1, param2, param3, param4);
	}
	else
	{
	
	t_nml_hdr buffer;
	unsigned char *p;
	unsigned int size = 0;
	unsigned char is_remain_byte = 0;
	unsigned char remain_byte;

	size = add_ost_header (&buffer, componentId, tracemsgId, has_handle, 0, 0, aComponentHandle, aParentHandle, mode);
	buffer.btrace_header [0] += 16; // length

	// Adding 16 bytes from Param	
	buffer.length_byte  = (size + 16) - 5;

	// Transmit OST header
	write_ost_chunk ((unsigned char*) &buffer, size, 0, &is_remain_byte, &remain_byte);

	// Reuse buffer since header already send
	p = ((unsigned char*)&buffer);
	SWAP_32(p, param1);
	SWAP_32(p, param2);
	SWAP_32(p, param3);
	SWAP_32(p, param4);
	size = 16;
	if (is_remain_byte)
		size++;

	// Transmit 32-bit param1, param2, param3
	write_ost_chunk ((unsigned char*) &buffer, size, 1, &is_remain_byte, &remain_byte);	
	}
}

////////////////////////////////////////////////////////////////////////////
// OSTTraceData
////////////////////////////////////////////////////////////////////////////
void OSTTraceData (TRACEMSGID_TYPE componentId, TRACEMSGID_TYPE tracemsgId, unsigned char *ptr, unsigned int length, unsigned short has_handle, unsigned int aComponentHandle, TRACEMSGID_TYPE aParentHandle)
{

	t_nml_hdr_ext buffer;
	unsigned char *p;
	unsigned int size = 0;
	unsigned char is_remain_byte = 0;
	unsigned char remain_byte;
	unsigned int ost_length;
	unsigned int mode = wgetGmode();

	// By default all word are handle as 32-bit
	ost_length = (length << 2);

	size = add_ost_header_ext (&buffer, componentId, tracemsgId, has_handle, ost_length, aComponentHandle, aParentHandle, mode);
	
	// Update ost payload size
	p = &buffer.ext_length[0];
	SWAP_32(p, (size + ost_length) - 5);

	// Transmit OST header
	write_ost_chunk ((unsigned char*) &buffer, size, 0, &is_remain_byte, &remain_byte);
	
	// Transmit OST payload
	write_ost_chunk_ext (ptr, ost_length, &is_remain_byte, &remain_byte);	

}



#pragma force_dcumode
static int write_ost_chunk(unsigned char *buffer, unsigned int size, unsigned char enable_ts, unsigned char *is_remain_byte, unsigned char *remain_byte)
{
	unsigned char *p, c1, c2, c3, c4;
	unsigned int first_time = 1;
	unsigned int priority;
	unsigned char channel;

	priority = (unsigned int) getRunningComponentPriority();
	// Should never happen, but in case of...
	if (priority > MAX_THREAD_PRIORITY) priority = MAX_THREAD_PRIORITY;

	channel = t_xti_channel [priority];

	// send off the header less the last byte
	p = buffer;
	if (*is_remain_byte)
	 	c1 = *remain_byte;
	else
		c1 = *p++;

	*is_remain_byte = 0;
	*remain_byte = 0;

	while (size > 0)
	{
		if (size > 4) {
			if (first_time) {
				c2 = *p++; c3 = *p++; c4 = *p++; 
			} else {
				c1 = *p++; c2 = *p++; c3 = *p++; c4 = *p++; 
			}
			ll_trace_32 (channel, MAKELONG (c1, c2, c3, c4));
			size -= 4;
		} else {
			if (size == 4) {
				if (first_time) {
					c2 = *p++; c3 = *p++; c4 = *p++; 
				} else {
					c1 = *p++; c2 = *p++; c3 = *p++; c4 = *p++; 
				}
				if (enable_ts)
					ll_tracet_32 (channel, MAKELONG (c1, c2, c3, c4));
				else
					ll_trace_32 (channel, MAKELONG (c1, c2, c3, c4));
				size -= 4;
				return 0;
			}
			if (size > 2) {
				if (first_time)				
					c2 = *p++; 
				else {
					c1 = *p++; c2 = *p++;
				}
				ll_trace_16 (channel, ((unsigned int) c1 << 8) | c2);				
				size -= 2;		
			} 
			if (size == 2) {
				if (first_time)				
					c2 = *p++; 
				else {
					c1 = *p++; c2 = *p++;				
				}
				if (enable_ts)				
					ll_tracet_16 (channel, ((unsigned int) c1 << 8) | c2);		
				else
					ll_trace_16 (channel, ((unsigned int) c1 << 8) | c2);				
				size -= 2;
				return 0;
			}		
			if (size /* == 1 */) {
				if (first_time)				
					c2 = 0; 
				else {
					c1 = *p++; c2 = 0;
				}
				if (enable_ts)								
				{
					ll_tracet_16 (channel, ((unsigned int) c1 << 8) | c2);
					return 0;			
				}
				else {
					*remain_byte = c1;
					*is_remain_byte = 1;					
					return 0;								
				}
			}
		}
		first_time = 0;
	}
	return 0;
}

#pragma force_dcumode
static int write_ost_chunk_ext(unsigned int *buffer, unsigned int size, unsigned char *is_remain_byte, unsigned char *remain_byte)
{
	unsigned int *p, c1;
	unsigned int first_time = 1;
	unsigned int priority;
	unsigned char channel;
	unsigned long tmp;

	priority = (unsigned int) getRunningComponentPriority();
	// Should never happen, but in case of...
	if (priority > MAX_THREAD_PRIORITY) priority = MAX_THREAD_PRIORITY;

	channel = t_xti_channel [priority];

	// send off the header less the last byte
	p = buffer;

	if (*is_remain_byte)
	 	c1 = *remain_byte;
	else
		c1 = 0;

	*is_remain_byte = 0;
	*remain_byte = 0;

#ifdef OST_DEBUG
	NmfPrint1(EE_DEBUG_LEVEL_INFO, "is_remain_byte: %d\n", *is_remain_byte);
	NmfPrint1(EE_DEBUG_LEVEL_INFO, "size: %d\n", size);
#endif

	while (size)
	{
		tmp = (((unsigned long)c1) << 24);
		tmp |= (((((unsigned long)*p) >> 16) & 0xFF) << 8);
		tmp |= ((((unsigned long)*p) >> 8)  & 0xFF);

		//NmfPrint1(EE_DEBUG_LEVEL_INFO, "tmp_H: 0x%x\n", (unsigned int)((tmp >> 16)& 0xFFFF));
		//NmfPrint1(EE_DEBUG_LEVEL_INFO, "tmp_L: 0x%x\n", (unsigned int)(tmp & 0xFFFF));
		//NmfPrint1(EE_DEBUG_LEVEL_INFO, "remain_byte: 0x%x\n", *remain_byte);

		c1 = (*p & 0xFF);


		ll_trace_32 (channel, (unsigned long)tmp);
		size -= 4;
		*p++;
	}
	
	ll_tracet_16 (channel, (unsigned int)(c1 << 8));
	*is_remain_byte = 0;

	return 0;
}
















