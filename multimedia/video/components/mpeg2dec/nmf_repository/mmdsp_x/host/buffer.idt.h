/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*****************************************************************************/
/**
 * This is a Nomadik Multiprocessing Framework generated file, please do not modify it.
 */

/* Generated buffer.idt defined type */
#if !defined(__BUFFER_H__)
#define __BUFFER_H__


#define BUFFERFLAG_MASK_FOR_DRAIN 0xFFFF00UL

#define DRAIN_ID_OFFSET 8

#define DRAIN_ID_LENGTH 16

typedef enum t_xyuv_BufferFlag_e {
  BUFFERFLAG_EOS=0x1,
  BUFFERFLAG_STARTTIME=0x2,
  BUFFERFLAG_DECODEONLY=0x4,
  BUFFERFLAG_DATACORRUPT=0x8,
  BUFFERFLAG_ENDOFRAME=0x10,
  BUFFERFLAG_SYNCFRAME=0x20,
  BUFFERFLAG_EXTRADATA=0x40,
  BUFFERFLAG_TIMESTAMP=0x400000L} BufferFlag_e;

typedef struct t_xyuv_Buffer_t {
  t_sword* data;
  t_uint24 byteInLastWord;
  t_uint32 allocLen;
  t_uint32 filledLen;
  BufferFlag_e flags;
  t_sint24 nTimeStamph;
  t_uint48 nTimeStampl;
  t_uint32 address;
} Buffer_t;

typedef Buffer_t* Buffer_p;

#endif
