/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef __INC_ITE_BUFFER_MANAGEMENT_H
#define __INC_ITE_BUFFER_MANAGEMENT_H

#if !( defined(__ARM_LINUX) || defined(__ARM_SYMBIAN))
#include "hcl_defs.h"
#else
#include <inc/type.h>
#endif

#include "ite_sia_buffer.h"
#include "algoutilities.h"

#define COLOR_BAR__DEFAULT_TESTING_POSITION 10

typedef struct _PelRGB888_ {
	t_uint8 RedValue;
	t_uint8 GreenValue;
	t_uint8 BlueValue;
} ts_PelRGB888, *tps_PelRGB888;

	
typedef enum {
   ITE_FILE_RAW = 0,
   ITE_FILE_GAM
} e_ITE_FileType;

typedef enum {
 YELLOW_COLOR_E,
 CYAN_COLOR_E,
 MAGENTA_COLOR_E,
 BLACK_COLOR_E,
 WHITE_COLOR_E,
 RED_COLOR_E,
 BLUE_COLOR_E,
 GREEN_COLOR_E
} e_basic_color;


#ifdef __cplusplus
extern "C"
{
#endif

/*--------------------------------------------------------------------------*
 * Functions                                                                *
 *--------------------------------------------------------------------------*/
t_uint8 ITE_StoreBufferInFile(char* out_filename,ts_siapicturebuffer* Buffer, e_ITE_FileType type);
void ITE_FillBufferWith(t_uint32 adr_Buffer, t_uint32 buffer_size, t_uint8 value);
void ITE_CleanBuffer(ts_siapicturebuffer* p_Buffer);
t_uint8 ITE_CheckGrabInBuffer(t_uint32 adr_Buffer, t_uint32 grab_size, t_uint8 default_value);
t_uint8 ITE_CheckBufsamevalue(t_uint32 adr_Buffer, t_uint32 grab_size);
t_uint8 ITE_CompareBuffers(t_uint32  adr_Buffer0,t_uint32 adr_Buffer1, t_uint32 buffer_size);
void ITE_DuplicateBuffer(tps_siapicturebuffer p_sourcebuffer, tps_siapicturebuffer p_destbuffer);
t_uint8 ITE_LoadBufferFromFile(char* filename,ts_siapicturebuffer* p_Buffer);
t_uint16 ITE_GetRGBvalue(ts_siapicturebuffer* p_Buffer,t_uint32 Xcoord,t_uint32 Ycoord,ts_PelRGB888 *Pel);
t_uint16 TestDZbuffer(ts_siapicturebuffer* p_Buffer); 
t_uint16 TestColorBarbuffer(ts_siapicturebuffer* p_Buffer, t_uint16 border_testing_offset); 
t_uint16 TestColorBarbufferGeneric(ts_siapicturebuffer* p_Buffer, t_uint16 border_testing_offset,  t_uint32 full_fov, t_uint32 fov_x);
t_uint16 TestColoredbuffer(ts_siapicturebuffer* p_Buffer,e_basic_color color);
t_uint8 IsRed(ts_PelRGB888 Pixel);
t_uint8 IsGreen(ts_PelRGB888 Pixel);
t_uint8 IsBlue(ts_PelRGB888 Pixel);
t_uint8 IsBlack(ts_PelRGB888 Pixel);
t_uint8 IsWhite(ts_PelRGB888 Pixel);
t_uint8 IsYellow(ts_PelRGB888 Pixel);
t_uint8 IsCyan(ts_PelRGB888 Pixel);
t_uint8 IsMagenta(ts_PelRGB888 Pixel);
t_uint8 ITE_StoreinBMPFile(char* filename,ts_siapicturebuffer* p_Buffer);
	
#ifdef __cplusplus
}
#endif


#endif /* __INC_ITE_BUFFER_H */
