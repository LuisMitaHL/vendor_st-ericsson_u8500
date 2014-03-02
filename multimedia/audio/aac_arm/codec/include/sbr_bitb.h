/*
  Bitbuffer management
*/
#ifndef __SBR_BITBUF_H
#define __SBR_BITBUF_H

#include "arm_porting.h"

#ifdef USE_3GPP_BITBUF

typedef struct
{
  unsigned char *char_ptr;
  unsigned char buffered_bits;
  unsigned short buffer_word;
  unsigned long nrBitsRead;
  unsigned long bufferLen;
  /* for debug */
  /* unsigned char *init_buffer_ptr; */
}
BIT_BUFFER;

typedef BIT_BUFFER *HANDLE_BIT_BUFFER;

void initBitBuffer (HANDLE_BIT_BUFFER hBitBuf,
		    unsigned char *start_ptr, unsigned long bufferLen);

unsigned long getbits (HANDLE_BIT_BUFFER hBitBuf, int n);
unsigned long GetNrBitsRead (HANDLE_BIT_BUFFER hBitBuf);
long GetNrBitsAvailable (HANDLE_BIT_BUFFER hBitBuf);

void CopyBitbufferState (HANDLE_BIT_BUFFER hBitBuf,
			 HANDLE_BIT_BUFFER hBitBufDest);

#else
#include "bitstream_lib_proto.h"

#define BIT_BUFFER BS_STRUCT_T
typedef BIT_BUFFER *HANDLE_BIT_BUFFER;

#define initBitBuffer(a,b,c)  	   bs_init((BS_STRUCT_T *) a,b,c,WORDSIZE)

#define getbits(a,b) 			bs_read_bit((BS_STRUCT_T *) a,b)

long GetNrBitsAvailable (HANDLE_BIT_BUFFER hBitBuf);

void CopyBitbufferState (HANDLE_BIT_BUFFER hBitBuf,
			 HANDLE_BIT_BUFFER hBitBufDest);

#endif
#endif
