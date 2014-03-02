/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <mpeg4dec/arm_nmf/algo_deblock.nmf>
#include "videocodecProfile_three_Filter.h"
#include "stdlib.h"
#include "stdio.h"
/*************************************************************************
* Declarations of internal functions
*************************************************************************/
void mp4d_FilterOneBlockVerticalEdge_P3(t_uint8* EdgePos_p, int LineWidth, t_uint8 QP);
void mp4d_FilterOneBlockHorizontalEdge_P3(t_uint8* EdgePos_p, int LineWidth, t_uint8 QP);

/*************************************************************************
* Static variables
*************************************************************************/
static const t_uint8 Strength[32] = { 0,  1,  1,  2,  2,  3,  3,  4,  4,  4,  5,  5,  6,  6,  7,  7,
                                     7,  8,  8,  8,  9,  9,  9, 10, 10, 10, 11, 11, 11, 12, 12, 12};

/*************************************************************************
* Definition of internal functions
*************************************************************************/

/*************************************************************************
*
* Name:         FilterOneBlockVerticalEdge
*
* Parameters:   EdgePos_p [IN/OUT]
*               LineWidth [IN]
*               QP        [IN]
*
* Returns:      void
*
* Description:  Filter along vertical left block edge.
*               H263 Annex J
*
**************************************************************************/
void mp4d_FilterOneBlockVerticalEdge_P3(t_uint8* EdgePos_p, int LineWidth, t_uint8 QP)
{
  int abs_d1;
  int d1;
  int d2;
  int d;
  int Ah;
  int Bh;
  int Ch;
  int Dh;
  int i;
  int abs_d;
 // Edges on the left of the block
  for (i = 0; i < 8; i++)
  {
    Ah = *(EdgePos_p - 2);
    Bh = *(EdgePos_p - 1);
    Ch = *(EdgePos_p);
    Dh = *(EdgePos_p + 1);

    d             = (Ah - (4 * Bh) + (4 * Ch) - Dh) / 8;
    abs_d         = MABS(d);
    abs_d1        = MMAX(0, abs_d - MMAX(0, 2 * (abs_d - Strength[QP])));
    d1            = MSIGN(d) * abs_d1;
    d2            = MCLIP((Ah - Dh) / 4, -(abs_d1 >> 1), (abs_d1 >> 1));

    *(EdgePos_p - 2) = (t_uint8) (Ah - d2);
    *(EdgePos_p + 1) = (t_uint8) (Dh + d2);
    *(EdgePos_p - 1) = (t_uint8) MCLIP(Bh + d1, 0, 255);
    *(EdgePos_p)     = (t_uint8) MCLIP(Ch - d1, 0, 255);

    EdgePos_p += LineWidth;
  }
}


/*************************************************************************
*
* Name:         FilterOneBlockHorizontalEdge
*
* Parameters:   EdgePos_p [IN/OUT]
*               LineWidth [IN]
*               QP        [IN]
*
* Returns:      void
*
* Description:  Filter along upper horizontal block edge.
*               H263 Annex J
*
**************************************************************************/
void mp4d_FilterOneBlockHorizontalEdge_P3(t_uint8* EdgePos_p, int LineWidth,       t_uint8 QP)
{
  int abs_d1;
  int d;
  int d1;
  int d2;
  int Av;
  int Bv;
  int Cv;
  int Dv;
  int i;
  int abs_d;
  // Edges on top of the block
  for (i = 0; i < 8; i++)
  {
    Av = *(EdgePos_p - (2 * LineWidth));
    Bv = *(EdgePos_p - LineWidth);
    Cv = *(EdgePos_p);
    Dv = *(EdgePos_p + LineWidth);

    d      = (Av - (4 * Bv) + (4 * Cv) - Dv) / 8;
    abs_d  = MABS(d);
    abs_d1 = MMAX(0, abs_d  - MMAX(0, 2 * (abs_d - Strength[QP])));
    d1     = MSIGN(d) * abs_d1;
    d2     = MCLIP((Av - Dv) / 4, -(abs_d1 >> 1), (abs_d1 >> 1));

    *(EdgePos_p - (2 * LineWidth)) = (t_uint8) (Av - d2);
    *(EdgePos_p + LineWidth)       = (t_uint8) (Dv + d2);
    *(EdgePos_p - LineWidth)       = (t_uint8) MCLIP(Bv + d1, 0, 255);
    *(EdgePos_p)                   = (t_uint8) MCLIP(Cv - d1, 0, 255);

    (void)*EdgePos_p++;
  }
}

/*************************************************************************
* Definition of exported functions
**************************************************************************/

/*************************************************************************
*
* Name:         mp4d_DeblockingFilter
*
* Parameters:   Image_p     [OUT]
*               MB_Data_p   [IN]
*               LayerData_p [IN]
*
* Returns:      void
*
* Description:  H263 Annex J
*
**************************************************************************/
void mp4d_DeblockingFilter_P3(YUV_Image_dblk* Image_p, PostFilter_State_t *State_p)
{
  t_uint8 LumQuantStep;     // quantization step for current MB
  t_uint8 ChromQuantStep;   // quantization step for chrominace components
  int   Y_Coordinate;     // up_left corner Coordinates of the MB
  int   X_Coordinate;
  int   i;
  const t_uint8* QP_p;
  QP_p = State_p->QP_p;
  //Coding_type_p = State_p->Coding_type_p;
  // Do the horizontal filtering first
  for (i = 0; i < State_p->NrMB; i++)
  {
    // Get Coordinates
    Y_Coordinate = i/State_p->NrMB_Horizontal;
    X_Coordinate = i%State_p->NrMB_Horizontal;

    //printf("Y_Coordinate::%x X_Coordinate::%x Coding_type_p[i]::%x",Y_Coordinate,X_Coordinate,Coding_type_p[i]);

    // Filtering
    if (QP_p[2*i+1]) // Filter if this macroblock is coded
    {
      // Get Quant
      LumQuantStep = QP_p[2*i];

      if (Y_Coordinate > 0)
      {
        //ChromQuantStep = (t_uint8)LumQuantStep;
        ChromQuantStep = (t_uint8) (State_p->MQ_Mode ? ChrominanceQuantTable_P3[LumQuantStep] : LumQuantStep);
        mp4d_FilterOneBlockHorizontalEdge_P3(GET_Y_PTR(Image_p, X_Coordinate, Y_Coordinate), Image_p->Y_LineWidth, LumQuantStep);
        mp4d_FilterOneBlockHorizontalEdge_P3(GET_Y_PTR(Image_p, X_Coordinate + 8, Y_Coordinate), Image_p->Y_LineWidth, LumQuantStep);
        mp4d_FilterOneBlockHorizontalEdge_P3(GET_CB_PTR(Image_p, X_Coordinate >> 1, Y_Coordinate >> 1), Image_p->C_LineWidth, ChromQuantStep);
        mp4d_FilterOneBlockHorizontalEdge_P3(GET_CR_PTR(Image_p, X_Coordinate >> 1, Y_Coordinate >> 1), Image_p->C_LineWidth, ChromQuantStep);
      }
      mp4d_FilterOneBlockHorizontalEdge_P3(GET_Y_PTR(Image_p, X_Coordinate, Y_Coordinate + 8), Image_p->Y_LineWidth, LumQuantStep);
      mp4d_FilterOneBlockHorizontalEdge_P3(GET_Y_PTR(Image_p, X_Coordinate + 8, Y_Coordinate + 8), Image_p->Y_LineWidth, LumQuantStep);
    }
    else if ((Y_Coordinate > 0) && QP_p[2*(i - State_p->NrMB_Horizontal)+1])
    {
      // Filter if above macroblock is coded
      // Get Quant
      LumQuantStep   = QP_p[2*(i - State_p->NrMB_Horizontal)];
      ChromQuantStep = (t_uint8) (State_p->MQ_Mode ? ChrominanceQuantTable_P3[LumQuantStep] : LumQuantStep);

      mp4d_FilterOneBlockHorizontalEdge_P3(GET_Y_PTR(Image_p, X_Coordinate, Y_Coordinate), Image_p->Y_LineWidth, LumQuantStep);
      mp4d_FilterOneBlockHorizontalEdge_P3(GET_Y_PTR(Image_p, X_Coordinate + 8, Y_Coordinate), Image_p->Y_LineWidth, LumQuantStep);
      mp4d_FilterOneBlockHorizontalEdge_P3(GET_CB_PTR(Image_p, X_Coordinate >> 1, Y_Coordinate >> 1), Image_p->C_LineWidth, ChromQuantStep);
      mp4d_FilterOneBlockHorizontalEdge_P3(GET_CR_PTR(Image_p, X_Coordinate >> 1, Y_Coordinate >> 1), Image_p->C_LineWidth, ChromQuantStep);
    }
  }

  // Then do the vertical filtering
  for (i = 0; i < State_p->NrMB; i++)
  {
    // Get Coordinates
    Y_Coordinate = i/State_p->NrMB_Horizontal;
    X_Coordinate = i%State_p->NrMB_Horizontal;

    // Filtering
    if (QP_p[2*i+1])
    {
      // Get Quant
      LumQuantStep = QP_p[2*i];

      if (X_Coordinate > 0)
      {
        ChromQuantStep = (t_uint8) (State_p->MQ_Mode ? ChrominanceQuantTable_P3[LumQuantStep] : LumQuantStep);

        mp4d_FilterOneBlockVerticalEdge_P3(GET_Y_PTR(Image_p, X_Coordinate, Y_Coordinate), Image_p->Y_LineWidth, LumQuantStep);
        mp4d_FilterOneBlockVerticalEdge_P3(GET_Y_PTR(Image_p, X_Coordinate, Y_Coordinate + 8),Image_p->Y_LineWidth, LumQuantStep);
        mp4d_FilterOneBlockVerticalEdge_P3(GET_CB_PTR(Image_p, X_Coordinate >> 1,Y_Coordinate >> 1), Image_p->C_LineWidth, ChromQuantStep);
        mp4d_FilterOneBlockVerticalEdge_P3(GET_CR_PTR(Image_p, X_Coordinate >> 1,Y_Coordinate >> 1), Image_p->C_LineWidth, ChromQuantStep);
      }
      mp4d_FilterOneBlockVerticalEdge_P3(GET_Y_PTR(Image_p, X_Coordinate + 8, Y_Coordinate), Image_p->Y_LineWidth, LumQuantStep);
      mp4d_FilterOneBlockVerticalEdge_P3(GET_Y_PTR(Image_p, X_Coordinate + 8, Y_Coordinate + 8), Image_p->Y_LineWidth, LumQuantStep);
    }
    else if ((X_Coordinate > 0) && QP_p[2*(i - 1)+1])
    {
      // Get Quant
      LumQuantStep   = QP_p[2*(i - 1)];
      ChromQuantStep = (t_uint8) (State_p->MQ_Mode ? ChrominanceQuantTable_P3[LumQuantStep] : LumQuantStep);

      mp4d_FilterOneBlockVerticalEdge_P3(GET_Y_PTR(Image_p, X_Coordinate, Y_Coordinate), Image_p->Y_LineWidth, LumQuantStep);
      mp4d_FilterOneBlockVerticalEdge_P3(GET_Y_PTR(Image_p, X_Coordinate, Y_Coordinate + 8), Image_p->Y_LineWidth, LumQuantStep);
      mp4d_FilterOneBlockVerticalEdge_P3(GET_CB_PTR(Image_p, X_Coordinate >> 1, Y_Coordinate >> 1), Image_p->C_LineWidth, ChromQuantStep);
      mp4d_FilterOneBlockVerticalEdge_P3(GET_CR_PTR(Image_p, X_Coordinate >> 1, Y_Coordinate >> 1), Image_p->C_LineWidth, ChromQuantStep);
    }
  }
}



