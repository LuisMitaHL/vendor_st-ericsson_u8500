/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*vbv.h
Header file of the Video Buffer Verifier*/

#ifndef VBV_H
#define VBV_H

#include "ast_defines.h"
#include "ast_types.h"

#define MAX_VBV_BUFFER_SIZE 40

/* #define DEBUG_VBV */
/*comment if debug is not required*/

void mp4e_vbv_init(mp4_parameters * mp4_par);
void mp4e_vbv_config_info(int bits);
#ifdef VBV_USEFUL_FUNCTIONS
ui32_t mp4e_vbv_control_T(ui32_t T);
ui8_t mp4e_vbv_update_picture(ui32_t S, ui32_t * nstuff, int wantskip);
ui32_t mp4e_vbv_MB_stuff_ctrl(int VPoccupancy, int MBbits, int MBx, int MBy, int inter, ui32_t VPstuffMB);
#endif
#ifdef VBV_WRITE_REPORT /*defined in ast_defines.h*/
  #include <stdio.h>
  void mp4e_vbv_closure (mp4_parameters * mp4_par, int nframes);
#endif

typedef struct
{
  ui32_t Bs; /*VB dimension; 30+12 bit length variable should be used, but for the SP@L3 32 bits are good enough*/
  ui32_t vbv_occupancy;
  si32_t B;  /*istantaneous VB fullness (bits)*/
  ui32_t Rp; /*bits to be removed from the buffer per picture*/
  ui32_t Rp_frac;
  ui32_t ui_StuffBits;
  ui8_t  b_vbv_panic_mode;
  ui32_t ui_Tmin;
  ui32_t ui_Tmax; /*minimum and maximum bits for the current frame*/
  ui32_t ui_GLOBAL_Tmax; /*minimum and maximum bits for every frame*/
  //static ui32_t ui_GLOBAL_minfullness; /*minimum fullness for the buffer*/
  unsigned int VPsize;
  unsigned int totalMBs;
  unsigned int MBwidth;
  unsigned int max_vp_size;
  ui8_t vbv_useHRD_flag;
  ui8_t firstframe;
  //static ui8_t framerate;
}VBVData;

#endif /* VBV_H */
