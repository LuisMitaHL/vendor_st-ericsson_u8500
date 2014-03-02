/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   SpeechProcHybridBuffer.cpp
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "SpeechProcHybridBuffer.h"
#include "ENS_Trace.h"

SpeechProcHybridBuffer::SpeechProcHybridBuffer(OMX_U32 domain_id, t_cm_size byte_size, sp_memory_type_t mem, OMX_ERRORTYPE &error)
    : mMpcHeaderFromArm(0),
      mMpcHeaderFromMpc(0),
      mMpcHeaderMemoryHandle(0),
      mMpcBufferMemoryHandle(0),
      mMpcBufferAddress(0)
{
  MEMORY_TRACE_ENTER("SpeechProcHybridBuffer::SpeechProcHybridBuffer");
  OMX_ERRORTYPE local_error = OMX_ErrorNone;
  t_cm_mpc_memory_type memory;

  // initialize mOmxHeader to please coverity
  mOmxHeader.nSize                = 0;
  mOmxHeader.nVersion.nVersion    = 0;
  mOmxHeader.pBuffer              = 0;
  mOmxHeader.nAllocLen            = 0;
  mOmxHeader.nFilledLen           = 0;
  mOmxHeader.nOffset              = 0;
  mOmxHeader.pAppPrivate          = 0;
  mOmxHeader.pPlatformPrivate     = 0;
  mOmxHeader.pInputPortPrivate    = 0;
  mOmxHeader.pOutputPortPrivate   = 0;
  mOmxHeader.hMarkTargetComponent = 0;
  mOmxHeader.pMarkData            = 0;
  mOmxHeader.nTickCount           = 0;
  mOmxHeader.nTimeStamp           = 0;
  mOmxHeader.nFlags               = 0;
  mOmxHeader.nOutputPortIndex     = 0;
  mOmxHeader.nInputPortIndex      = 0;

  // allocate MPC header in 24bits memory
  if(mem == SP_MEM_TCM)        { memory = CM_MM_MPC_TCM24_X;}
  else if(mem == SP_MEM_ESRAM) { memory = CM_MM_MPC_ESRAM24;}
  else                         { memory = CM_MM_MPC_SDRAM24;}

  error = ENS::allocMpcMemory(domain_id, memory, sizeof(MpcBuffer_t)/4,
                              CM_MM_ALIGN_2WORDS, &mMpcHeaderMemoryHandle);

  if (error != OMX_ErrorNone)
  {
    mMpcHeaderMemoryHandle = 0;
    return;
  }

  // allocate MPC buffer in 16bits memory if possible
  if (mem == SP_MEM_TCM)       { memory = CM_MM_MPC_TCM16_X;}
  else if(mem == SP_MEM_ESRAM) { memory = CM_MM_MPC_ESRAM16;}
  else                         { memory = CM_MM_MPC_SDRAM16;}

  error = ENS::allocMpcMemory(domain_id, memory, (byte_size / 2),
                              CM_MM_ALIGN_2WORDS, &mMpcBufferMemoryHandle);
  if (error != OMX_ErrorNone)
  {
    local_error = ENS::freeMpcMemory(mMpcHeaderMemoryHandle);
	DBC_ASSERT(local_error == OMX_ErrorNone);
    mMpcHeaderMemoryHandle = 0;
    mMpcBufferMemoryHandle = 0;
    return;
  }

  // init OMX buffer header
  t_cm_system_address sysAddr;
  t_cm_error cmErr = CM_OK;
  cmErr = CM_GetMpcMemorySystemAddress(mMpcBufferMemoryHandle, &sysAddr);
  if (cmErr != CM_OK)
  {
    error = OMX_ErrorUndefined;
    local_error = ENS::freeMpcMemory(mMpcHeaderMemoryHandle);
	DBC_ASSERT(local_error == OMX_ErrorNone);
    mMpcHeaderMemoryHandle = 0;
    mMpcBufferMemoryHandle = 0;
    return;
  }

  mOmxHeader.nSize              = sizeof(OMX_BUFFERHEADERTYPE);
  mOmxHeader.pBuffer            = (OMX_U8*)sysAddr.logical;
  mOmxHeader.nAllocLen          = byte_size;
  mOmxHeader.nFilledLen         = 0;
  mOmxHeader.nOffset            = 0;
  mOmxHeader.pAppPrivate        = this;
  mOmxHeader.pInputPortPrivate  = this;
  mOmxHeader.pOutputPortPrivate = this;
  mOmxHeader.nFlags             = 0;

  // init MPC header
  cmErr = CM_GetMpcMemoryMpcAddress(mMpcHeaderMemoryHandle, &mMpcHeaderFromMpc);
  if (cmErr != CM_OK)
  {
    error = OMX_ErrorUndefined;
    local_error = ENS::freeMpcMemory(mMpcHeaderMemoryHandle);
	DBC_ASSERT(local_error == OMX_ErrorNone);
    mMpcHeaderMemoryHandle = 0;
    mMpcBufferMemoryHandle = 0;
    return;
  }

  cmErr = CM_GetMpcMemorySystemAddress(mMpcHeaderMemoryHandle, &sysAddr);
  if (cmErr != CM_OK)
  {
    error = OMX_ErrorUndefined;
    local_error = ENS::freeMpcMemory(mMpcHeaderMemoryHandle);
	DBC_ASSERT(local_error == OMX_ErrorNone);
    mMpcHeaderMemoryHandle = 0;
    mMpcBufferMemoryHandle = 0;
    return;
  }
  mMpcHeaderFromArm = (MpcBuffer_t *)sysAddr.logical;

  cmErr = CM_GetMpcMemoryMpcAddress(mMpcBufferMemoryHandle, &mMpcBufferAddress);
  if (cmErr != CM_OK)
  {
    error = OMX_ErrorUndefined;
    local_error = ENS::freeMpcMemory(mMpcHeaderMemoryHandle);
	DBC_ASSERT(local_error == OMX_ErrorNone);
    mMpcHeaderMemoryHandle = 0;
    mMpcBufferMemoryHandle = 0;
    return;
  }

  mMpcHeaderFromArm->data            = mMpcBufferAddress;
  mMpcHeaderFromArm->byteInLastWord  = 0;
  mMpcHeaderFromArm->allocLenh       = ((byte_size / 2) >> 24);
  mMpcHeaderFromArm->allocLenl       = (((byte_size / 2)+(byte_size % 2)) & 0xFFFFFF);
  mMpcHeaderFromArm->filledLenh      = 0;
  mMpcHeaderFromArm->filledLenl      = 0;
  mMpcHeaderFromArm->flags           = 0;
  mMpcHeaderFromArm->nTimeStamph     = 0;
  mMpcHeaderFromArm->nTimeStamplh    = 0;
  mMpcHeaderFromArm->nTimeStampll    = 0;
  mMpcHeaderFromArm->addressh        = 0;
  mMpcHeaderFromArm->addressl        = 0;
  mMpcHeaderFromArm->hostClassAddrh  = (((t_uint32)this) >> 24);
  mMpcHeaderFromArm->hostClassAddrl  = (((t_uint32)this) & 0xFFFFFF);

  MEMORY_TRACE_LEAVE("SpeechProcHybridBuffer::SpeechProcHybridBuffer");
}


SpeechProcHybridBuffer::~SpeechProcHybridBuffer(void)
{
  OMX_ERRORTYPE error;

  if(mMpcBufferMemoryHandle)
  {
    error = ENS::freeMpcMemory(mMpcBufferMemoryHandle);
    DBC_ASSERT(error == OMX_ErrorNone);
  }

  if(mMpcHeaderMemoryHandle)
  {
    error = ENS::freeMpcMemory(mMpcHeaderMemoryHandle);
    DBC_ASSERT(error == OMX_ErrorNone);
  }
}


void SpeechProcHybridBuffer::updateOMXHeader(void)
{
  // filled Length
  OMX_U32 filledLen = (mMpcHeaderFromArm->filledLenh<< 24) | (mMpcHeaderFromArm->filledLenl & 0xFFFFFF);
  filledLen = filledLen * 2;
  if(mMpcHeaderFromArm->byteInLastWord == 1) filledLen--;
  mOmxHeader.nFilledLen = filledLen;

  // Offset
  mOmxHeader.nOffset = 0;

  // Flags
  mOmxHeader.nFlags = mMpcHeaderFromArm->flags;

  // Timestamp
  mOmxHeader.nTimeStamp =(((OMX_S64)mMpcHeaderFromArm->nTimeStamph  << 48)|
						  ((OMX_S64)mMpcHeaderFromArm->nTimeStamplh << 24)|
						  (mMpcHeaderFromArm->nTimeStampll & 0xffffff));
}


void SpeechProcHybridBuffer::updateMPCHeader(void)
{
  //Filled length
  t_uint32 filledLen = mOmxHeader.nFilledLen / 2 + mOmxHeader.nFilledLen % 2;
  mMpcHeaderFromArm->filledLenh = filledLen >> 24;
  mMpcHeaderFromArm->filledLenl = filledLen & 0xffffff;
  t_uint32 byteInLastWord = (mOmxHeader.nFilledLen % 2 == 0) ? 2 : 1;
  mMpcHeaderFromArm->byteInLastWord = byteInLastWord;

  // Offset : set DSP address of the buffer
  DBC_ASSERT((mOmxHeader.nOffset % 2) == 0);
  t_uint32 mpcBuffAddr    = mMpcBufferAddress + mOmxHeader.nOffset / 2;
  mMpcHeaderFromArm->data = mpcBuffAddr;

  //Flags
  mMpcHeaderFromArm->flags = mOmxHeader.nFlags;

  // Timestamp
  mMpcHeaderFromArm->nTimeStamph   = mOmxHeader.nTimeStamp >> 48;
  OMX_S64 timeStampOffset = mOmxHeader.nTimeStamp & 0xffffffffffffLL;
  mMpcHeaderFromArm->nTimeStamplh  = timeStampOffset >> 24;
  mMpcHeaderFromArm->nTimeStampll  = timeStampOffset & 0xffffff;
}

