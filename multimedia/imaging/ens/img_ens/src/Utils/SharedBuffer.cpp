/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*****************************************************************************/
#include "SharedBuffer.h"

/* defines "memcpy" from libc */
#include <string.h>

SharedBuffer::SharedBuffer(OMX_U32 domain_id, t_cm_size byte_size, OMX_ERRORTYPE &error) 
	: mOmxHeader(0)
	, shared_buffer_arm(0)
	, shared_buffer_mpc(0)
	, header_memory_handle(0)
	, buffer_memory_handle(0)
	, mBufLogicalAddr(0)
	, mBufPhysicalAddr(0)
	, mMpcBufAddr(0)
	, mIsBufferAllocated(false) 
{
	MEMORY_TRACE_ENTER("SharedBuffer::SharedBuffer");

	mOmxHeader = new OMX_BUFFERHEADERTYPE;
	if (!mOmxHeader) 
	{
		error = OMX_ErrorInsufficientResources;
		return;
	}

	error = allocateArmSharedBuffer_t(domain_id);
	if (error != OMX_ErrorNone) 
	{
		delete mOmxHeader;
		return;
	}

	// allocate buffer
	error = ENS::allocMpcMemory(domain_id, CM_MM_MPC_SDRAM16, (byte_size / 2), CM_MM_ALIGN_2WORDS, &buffer_memory_handle);
	if (error != OMX_ErrorNone) 
	{
		delete mOmxHeader;
		ENS::freeMpcMemory(header_memory_handle);
		return;
	}
	mIsBufferAllocated = true;

	// init OMX buffer header
	t_cm_system_address sysAddr;
	t_cm_error cmErr = CM_OK;
	t_uint32 mpcBuffAddr = 0;
	cmErr = CM_GetMpcMemorySystemAddress(buffer_memory_handle, &sysAddr);
	if (cmErr != CM_OK)
	{
		delete mOmxHeader;
		ENS::freeMpcMemory(header_memory_handle);
		return;
	}

	mOmxHeader->nSize              = sizeof(OMX_BUFFERHEADERTYPE);
	mOmxHeader->pBuffer            = (OMX_U8*) sysAddr.logical;
	mOmxHeader->nAllocLen          = byte_size;
	mOmxHeader->nFilledLen         = 0;
	mOmxHeader->nOffset            = 0;
	mOmxHeader->pAppPrivate        = this;
	mOmxHeader->pInputPortPrivate  = this;
	mOmxHeader->pOutputPortPrivate = this;
	mOmxHeader->nFlags             = 0;

	// init buffer addresses
	mBufLogicalAddr = mOmxHeader->pBuffer;
	cmErr = CM_GetMpcMemoryMpcAddress(buffer_memory_handle, &mpcBuffAddr);
	if (cmErr != CM_OK) 
	{
		delete mOmxHeader;
		ENS::freeMpcMemory(header_memory_handle);
		return;
	}

	mBufPhysicalAddr = 0;
	mMpcBufAddr = mpcBuffAddr;

	// init headers
	initSharedBufferHeaders(byte_size);

	MEMORY_TRACE_LEAVE("SharedBuffer::SharedBuffer");
}

SharedBuffer::SharedBuffer(OMX_U32 domain_id, t_cm_size byte_size, t_uint8* pBuffer, t_uint32 bufferPhysicalAddr, t_uint32 bufferMpcAddr, void * bufferAllocInfo, OMX_ERRORTYPE &error) 
	: mOmxHeader(0)
	, shared_buffer_arm(0)
	, shared_buffer_mpc(0)
	, header_memory_handle(0)
	, buffer_memory_handle(0)
	, mBufLogicalAddr(0)
	, mBufPhysicalAddr(0)
	, mMpcBufAddr(0)
	, mIsBufferAllocated(false) 
{
	MEMORY_TRACE_ENTER("SharedBuffer::SharedBuffer");

	error = allocateArmSharedBuffer_t(domain_id);
	if (error != OMX_ErrorNone)
		return;

	// buffer must not be allocated
	buffer_memory_handle = (t_cm_memory_handle) bufferAllocInfo;
	mIsBufferAllocated = false;

	// omx buffer header will be initialized later
	mOmxHeader = 0;

	// init buffer addresses
	mBufLogicalAddr = pBuffer;
	IMGENS_ASSERT(bufferMpcAddr != 0 || bufferPhysicalAddr != 0);
	mBufPhysicalAddr = bufferPhysicalAddr;
	mMpcBufAddr = bufferMpcAddr;

	// init headers
	initSharedBufferHeaders(byte_size);

	MEMORY_TRACE_LEAVE("SharedBuffer::SharedBuffer");
}

SharedBuffer::~SharedBuffer(void) 
{
	OMX_ERRORTYPE error;
	error = ENS::freeMpcMemory(header_memory_handle);
	IMGENS_ASSERT(error == OMX_ErrorNone);
	if (mIsBufferAllocated) 
	{
		error = ENS::freeMpcMemory(buffer_memory_handle);
		IMGENS_ASSERT(error == OMX_ErrorNone);
		delete mOmxHeader;
	}
}

OMX_ERRORTYPE
SharedBuffer::setOMXHeader(OMX_BUFFERHEADERTYPE *omxHeader) 
{
	DBC_PRECONDITION(omxHeader!=0);
	mOmxHeader = omxHeader;
	return OMX_ErrorNone;
}

OMX_ERRORTYPE SharedBuffer::allocateArmSharedBuffer_t(OMX_U32 domain_id) 
{
	return ENS::allocMpcMemory(domain_id, CM_MM_MPC_SDRAM24,
			sizeof(ARMSharedBuffer_t) / 4, CM_MM_ALIGN_2WORDS,
			&header_memory_handle);
}

void SharedBuffer::initSharedBufferHeaders(t_cm_size byte_size) 
{
	t_cm_system_address sysAddr;
	t_uint32 mpcBuffAddr = 0;
	CM_GetMpcMemoryMpcAddress(header_memory_handle, &mpcBuffAddr);
	shared_buffer_mpc = mpcBuffAddr;
	CM_GetMpcMemorySystemAddress(header_memory_handle, &sysAddr);
	shared_buffer_arm = (ARMSharedBuffer_t *) sysAddr.logical;

	shared_buffer_arm->data            = mMpcBufAddr;
	shared_buffer_arm->byteInLastWord  = 0;
	shared_buffer_arm->allocLenh       = ((byte_size / 2) >> 24);
	shared_buffer_arm->allocLenl       = (((byte_size / 2) + (byte_size % 2)) & 0xFFFFFF);
	shared_buffer_arm->filledLenh      = 0;
	shared_buffer_arm->filledLenl      = 0;
	shared_buffer_arm->flags           = 0;
	shared_buffer_arm->nTimeStamph     = 0;
	shared_buffer_arm->nTimeStamplh    = 0;
	shared_buffer_arm->nTimeStampll    = 0;
	shared_buffer_arm->addressh        = mBufPhysicalAddr >> 24;
	shared_buffer_arm->addressl        = mBufPhysicalAddr & 0xffffff;
	shared_buffer_arm->hostHeaderAddrh = (((t_uint32) shared_buffer_arm) >> 24);
	shared_buffer_arm->hostHeaderAddrl = (((t_uint32) shared_buffer_arm) & 0xFFFFFF);
	shared_buffer_arm->hostClassAddr   = ((t_uint32) this);
}

IMG_ENS_API_EXPORT
void SharedBuffer::updateOMXHeader(bool enableCopy) 
{
	// filled Length
	OMX_U32 filledLen = (shared_buffer_arm->filledLenh << 24) | (shared_buffer_arm->filledLenl & 0xFFFFFF);
	filledLen = filledLen * 2;
	if (shared_buffer_arm->byteInLastWord == 1)
		filledLen--;
	mOmxHeader->nFilledLen = filledLen;

	// Offset
	if (mMpcBufAddr) 
	{
		mOmxHeader->nOffset = (shared_buffer_arm->data - mMpcBufAddr) * 2;
	} 
	else 
	{
		t_uint32 dataPhysicalAddress = (shared_buffer_arm->addressh << 24) | (shared_buffer_arm->addressl & 0xFFFFFF);
		mOmxHeader->nOffset = dataPhysicalAddress - mBufPhysicalAddr;
	}

	// Flags
	mOmxHeader->nFlags = shared_buffer_arm->flags;

	// Timestamp
	mOmxHeader->nTimeStamp = ((OMX_S64) shared_buffer_arm->nTimeStamph << 48) | ((OMX_S64) shared_buffer_arm->nTimeStamplh << 24) | (shared_buffer_arm->nTimeStampll & 0xffffff);

	// if DSP buffer is not the OMX buffer we need a memcpy
	if (enableCopy && mBufLogicalAddr != mOmxHeader->pBuffer)
	{
		::memcpy((char*) mOmxHeader->pBuffer + mOmxHeader->nOffset, (char*) mBufLogicalAddr + mOmxHeader->nOffset, mOmxHeader->nFilledLen);
	}
}

IMG_ENS_API_EXPORT
void SharedBuffer::updateMPCHeader(void)
{
	// if DSP buffer is not the same as OMX buffer we need a memcpy
	if (mBufLogicalAddr != mOmxHeader->pBuffer)
	{
		::memcpy((char*) mBufLogicalAddr + mOmxHeader->nOffset, (char*) mOmxHeader->pBuffer + mOmxHeader->nOffset, mOmxHeader->nFilledLen);
	}

	//Filled length
	t_uint32 filledLen = mOmxHeader->nFilledLen / 2 + mOmxHeader->nFilledLen % 2;
	shared_buffer_arm->filledLenh = filledLen >> 24;
	shared_buffer_arm->filledLenl = filledLen & 0xffffff;
	t_uint32 byteInLastWord = (mOmxHeader->nFilledLen % 2 == 0) ? 2 : 1;
	shared_buffer_arm->byteInLastWord = byteInLastWord;

	// Offset : set DSP address of the buffer
	if (mMpcBufAddr)
	{
		IMGENS_ASSERT((mOmxHeader->nOffset % 2) == 0);
		t_uint32 mpcBuffAddr = mMpcBufAddr + mOmxHeader->nOffset / 2;
		shared_buffer_arm->data = mpcBuffAddr;
	}
	else
	{
		shared_buffer_arm->data = 0;
	}

	// Offset : set physical address of the buffer
	if (mBufPhysicalAddr)
	{
		t_uint32 physicalAddr       = mBufPhysicalAddr + mOmxHeader->nOffset;
		shared_buffer_arm->addressh = ((t_uint32) physicalAddr) >> 24;
		shared_buffer_arm->addressl = ((t_uint32) physicalAddr) & 0xffffff;
	}
	else
	{
		shared_buffer_arm->addressh = 0;
		shared_buffer_arm->addressl = 0;
	}
	//Flags
	shared_buffer_arm->flags        = mOmxHeader->nFlags;

	// Timestamp
	shared_buffer_arm->nTimeStamph  = mOmxHeader->nTimeStamp >> 48;
	OMX_S64 timeStampOffset         = mOmxHeader->nTimeStamp & 0xffffffffffffLL;
	shared_buffer_arm->nTimeStamplh = timeStampOffset >> 24;
	shared_buffer_arm->nTimeStampll = timeStampOffset & 0xffffff;
}

