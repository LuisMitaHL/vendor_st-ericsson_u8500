/*****************************************************************************/
/*
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*/
/*****************************************************************************/
#ifndef __SHAREDBUFFERHPP
#define __SHAREDBUFFERHPP

#if 0

#include "ENS_Redefine_Class.h"

#include "ENS_Nmf.h"
#include "ENS_macros.h"

typedef struct {
	t_uint32 data;
	t_uint32 byteInLastWord;
	t_uint32 allocLenh;
	t_uint32 allocLenl;
	t_uint32 filledLenh;
	t_uint32 filledLenl;
	t_uint32 flags;
	t_uint32 nTimeStamph; //high part of the timestamp (16 most significant bits).
	t_uint32 nTimeStamplh; //high part of the timestamp low part (48 least significant bits).
	t_uint32 nTimeStampll; //low part of the timestamp high part (48 least significant bits).
	t_uint32 addressh;
	t_uint32 addressl;
	t_uint32 hostHeaderAddrh;
	t_uint32 hostHeaderAddrl;
	t_uint32 hostClassAddr;
} ARMSharedBuffer_t;


class SharedBuffer{
public:
	ENS_API_IMPORT SharedBuffer(OMX_U32 domain,
		t_cm_size byte_size,
		OMX_ERRORTYPE &error);
	ENS_API_IMPORT SharedBuffer(OMX_U32 domain,
		t_cm_size byte_size,
		t_uint8* pBuffer,
		t_uint32 bufferPhysicalAddr,
		t_uint32 bufferMpcAddr,
		void * bufferAllocInfo,
		OMX_ERRORTYPE &error);
	ENS_API_IMPORT ~SharedBuffer(void);

	inline OMX_BUFFERHEADERTYPE *getOMXHeader(void) {return mOmxHeader;}
	inline t_uint32             getMPCHeader(void) {return shared_buffer_mpc;}
	inline ARMSharedBuffer_t    *getSharedHeader(void) {return shared_buffer_arm;}
	inline t_cm_memory_handle   getBufferAllocInfo(void) {return buffer_memory_handle;}
	ENS_API_IMPORT void updateOMXHeader(bool enableCopy);
	ENS_API_IMPORT void updateMPCHeader(void);
	ENS_API_IMPORT OMX_ERRORTYPE setOMXHeader(OMX_BUFFERHEADERTYPE *omxHeader);

private:
	OMX_BUFFERHEADERTYPE *mOmxHeader;
	ARMSharedBuffer_t    *shared_buffer_arm;
	t_uint32              shared_buffer_mpc;
	t_cm_memory_handle    header_memory_handle;
	t_cm_memory_handle    buffer_memory_handle;
	t_uint8 *               mBufLogicalAddr;
	t_uint32                mBufPhysicalAddr;
	t_uint32                mMpcBufAddr;
	bool                    mIsBufferAllocated;

	OMX_ERRORTYPE allocateArmSharedBuffer_t(OMX_U32 domain_id);
	void initSharedBufferHeaders(t_cm_size byte_size);
};

#endif
#endif // __SHAREDBUFFERHPP
