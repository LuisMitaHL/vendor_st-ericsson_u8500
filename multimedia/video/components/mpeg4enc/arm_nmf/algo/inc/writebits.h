/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef WRITEBITS_H
#define WRITEBITS_H

#include "ast_defines.h"
#include "ast_types.h"

/* commands definition */
#define NO_CMD        0    /* no action -- writes the bits passed as an argument     */
#define ALGN_BYTE     1    /* align the bitstream to the byte                        */
#define STFF_BIT      2    /* insert stuffing bits inside the bitstream              */
#define WRITE_MODE    5    /* disable the writing of bitstream to allow pre-analisys */
#define RST_BUF       6    /* reset the buffer -- used for Data partitioning         */
#define SET_BUF       7    /* set the buffer where data is written                   */
#define CPY_BUF       8    /* copy buffer contents into another buffer               */
#define PIC_BIT       9    /* shows the bits contained in a picture                  */
#define SLC_BIT      10    /* shows the bits contained in the current slice          */
#define VPK_BIT      11    /* shows the bits for the current Video Packet            */
#define MRK_BUF      12    /* called at the end of each macroblock                   */
#define BCK_BUF      13    /* go back to the position sed by MRK_BUF                 */

/* Buffers definition */
#define NORMAL_BUFFER 0
#define HDR__1_BUFFER 1
#define HDR__2_BUFFER 2
#define DATA___BUFFER 3

#ifdef VP_STORE_POSITION
  #define VP_STORE_NEW_SEQ   0
  #define VP_STORE_NEW_FRAME 1
  #define VP_STORE_NEW_VP    2
  #define VP_STORE_WRITE     3
  #define VP_STORE_SET_SL    4
  #define VP_STORE_RES_SL    5
#endif

#define BITSTREAM_BUF_LENGTH 4*32768*4
#define HEADERS_1_BUF_LENGTH (4*250+1200)       /* FP: inserted space into buffer to avoid overflow with stuffing bits */
#define HEADERS_2_BUF_LENGTH (4*250+1200)       /* FP: inserted space into buffer to avoid overflow with stuffing bits */
#define DATA_B_BUFFER_LENGTH 4*1250


struct Buffer {
  unsigned char *buf;                    /* pointer to the temporary memory area were bits are stored */
  unsigned int   buffer_bits;            /* number of bits used in the current buffer                 */
  unsigned int   buffer;                 /* 8-bit buffer containing the current word                  */
  unsigned int   buf_length;             /* amount of bits available in the 8-bit buffer              */
  unsigned int   sl_buffer_bits;         /* Slice pre-analisys amount of bits used by current buffer  */
  unsigned int   marker_buffer_bits;     /* index of the amount of bits used by the previous mb       */
  unsigned int   first_vol_hdr_bits;     /* enzo: n. of bits used to write first vol header           */
};



struct BitStreamData 
{
  struct Buffer         bitstream;
  struct Buffer         Header_1;
  struct Buffer         Header_2;	
  struct Buffer         Data_block;
  struct Buffer         *s_buffer;
  unsigned int          modality; /* WRITE = 0 (writes data on the bitstream); SIMULATE = 1 (writes only # of bits); */
  int FrameStuffBits;
};
/* Normal buffers and error resilience buffer */

unsigned int mp4e_IF_WRITEBITS(unsigned int command, unsigned int nbits, unsigned int value);
void mp4e_WRITE_BITS(unsigned int nbits, unsigned int value);

void sl_setreset_bitstream(int mode);
void mp4e_ResetBuffer(int buf_num);
void WriteStuffingBytes(ui32_t ui_stbytes);
void mp4e_ResetFirstVOLHeaderBits(void);



#ifdef VP_STORE_POSITION
 void SaveVPdatavalues(int command);
 int GetVPnum();
 void GetVPposition(ui32_t * out);
#endif /*VP_STORE_POSITION*/


void mp4e_INIT_BITS(int sizeofbitstream);

void mp4e_Flush(void);
unsigned int mp4e_BitCountBRC(void);
void mp4e_CountFirstVOLHeaderBits(void);
ui32_t mp4e_WriteStuffingMB(si32_t si_stufbits, int inter);
unsigned int mp4e_READ_bitstream_bits(void);
void mp4e_WRITE_bitstream_bits(unsigned int value);

#ifdef STATISTIC
#include "bitstream.h"
void SaveVideoPacketStatistic();
void SaveMBStatistic(int mode, int command, struct VLC_WriteData *vlc);
#endif /* STATISTIC */



#endif /* WRITEBITS_H */
