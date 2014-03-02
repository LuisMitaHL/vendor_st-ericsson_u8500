/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef INCLUSION_GUARD_VIDEOCODECMOTIONCOMPENSATION_H
#define INCLUSION_GUARD_VIDEOCODECMOTIONCOMPENSATION_H

#ifdef __cplusplus
extern "C" {
#endif

/*************************************************************************
* Includes
**************************************************************************/
#include "ast_types.h"

/*************************************************************************
* Types, constants and external variables
**************************************************************************/


typedef struct
{
  t_sint16 x;   // X Coordinate 
  t_sint16 y;   // Y Coordinate
} Vector_t;

typedef struct
{
  uint8* Src_p;
  int    SrcLineWidth;
  int    SrcDispWidth;  
  int    SrcHeight;
} MC_Params_t;



typedef struct 
{
  uint16            SourceWidth;            // The exact size of input/output picture 
  uint16            SourceHeight;
  MC_Params_t       SourceImage[3];             
} LayerData_t;


typedef enum 
{
  Y  = 0, 
  CB = 1, 
  CR = 2
} YUV_Component_t;

#define MSIGN(a)              ((a) < 0 ? -1 : 1)

#define GET_Y_PTR(img, x, y)  ((img)->Y_p  + (x) + (y) * (img)->Y_LineWidth)
#define GET_CB_PTR(img, x, y) ((img)->Cb_p + (x) + (y) * (img)->C_LineWidth)
#define GET_CR_PTR(img, x, y) ((img)->Cr_p + (x) + (y) * (img)->C_LineWidth)

/*************************************************************************
* Declarations of functions
**************************************************************************/

#ifndef EMP_DEBUG_MC
void mp4e_MotionCompensateEncoder_planar(
                             unsigned char * RESTRICT inputframe,
                             unsigned char * RESTRICT reconframe,
                             int             xpos,
                             int             ypos,
                             int             xdim,
                             int             ydim,
                             int             vector_hpx,
                             int             vector_hpy,
							 int			 umv);

void mp4e_MotionCompensateEncoder_semiplanar(
                             unsigned char * RESTRICT inputframe,
                             unsigned char * RESTRICT reconframe,
                             int             xpos,
                             int             ypos,
                             int             xdim,
                             int             ydim,
                             int             vector_hpx,
                             int             vector_hpy,
							 int			 umv);
#else
void MotionCompensateEncoder_planar(
                             unsigned char * RESTRICT inputframe,
                             unsigned char * RESTRICT reconframe,
                             unsigned char * RESTRICT predictedmb,
                             int             xpos,
                             int             ypos,
                             int             xdim,
                             int             ydim,
                             int             vector_hpx,
                             int             vector_hpy,
							 int			 umv);
void MotionCompensateEncoder_semiplanar(
                             unsigned char * RESTRICT inputframe,
                             unsigned char * RESTRICT reconframe,
                             unsigned char * RESTRICT predictedmb,
                             int             xpos,
                             int             ypos,
                             int             xdim,
                             int             ydim,
                             int             vector_hpx,
                             int             vector_hpy,
							 int			 umv);
#endif


/*************************************************************************/                                  
#ifdef __cplusplus
}
#endif

#endif //INCLUSION_GUARD_VIDEOCODECMOTIONCOMPENSATION_H
