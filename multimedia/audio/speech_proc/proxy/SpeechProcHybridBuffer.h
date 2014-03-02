/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   SpeechProcHybridBuffer.h
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef __SPEECH_PROC_BUFFER_H_
#define __SPEECH_PROC_BUFFER_H_

#include "ENS_Nmf.h"
#include "speech_proc_config.h"

class SpeechProcHybridBuffer{
public:
  SpeechProcHybridBuffer(OMX_U32 domain, t_cm_size byte_size, sp_memory_type_t mem, OMX_ERRORTYPE &error);
  ~SpeechProcHybridBuffer(void);

  inline OMX_BUFFERHEADERTYPE *getOMXHeader(void){return &mOmxHeader;}
  inline t_uint32              getMPCHeader(void){return mMpcHeaderFromMpc;}
  void updateOMXHeader(void);
  void updateMPCHeader(void);

private:
  // map from ARM side buffer_t on dsp 24bits memory + 2 extra fields
  typedef struct {
    t_uint32     data;
    t_uint32	 byteInLastWord;
    t_uint32	 allocLenh;
    t_uint32	 allocLenl;
    t_uint32	 filledLenh;
    t_uint32	 filledLenl;
    t_uint32	 flags;
    t_uint32     nTimeStamph;  //high part of the timestamp (16 most significant bits).
    t_uint32     nTimeStamplh; //high part of the timestamp low part (48 least significant bits).
    t_uint32     nTimeStampll; //low part of the timestamp high part (48 least significant bits).
    t_uint32	 addressh;
    t_uint32	 addressl;
    t_uint32	 hostClassAddrh;
	t_uint32	 hostClassAddrl;
  } MpcBuffer_t;

  OMX_BUFFERHEADERTYPE	 mOmxHeader;
  MpcBuffer_t	        *mMpcHeaderFromArm;
  t_uint32         	     mMpcHeaderFromMpc;
  t_cm_memory_handle	 mMpcHeaderMemoryHandle;
  t_cm_memory_handle	 mMpcBufferMemoryHandle;
  t_uint32               mMpcBufferAddress;
};

#endif
