/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _DAMPER_FUNC_H_
#define _DAMPER_FUNC_H_

#define CDAMPER_CTRLP_MAX_DIM   8

class CDamperFunc
{
public:
   CDamperFunc();
   int           Configure(const unsigned int uiDim_X0p, const unsigned int uiDim_X1p, const float *pfX0, const float *pfX1, const float *pfY);
   int           Evaluate (float *pfY, const float fX0, const float fX1);

private:

   typedef enum
   {
      CDAMPER_CONSTANT = 0,                                        // damper is 1x1, i.e. constant output value
      CDAMPER_1D_X0    = 1,                                        // damper is uiDim_X0x1, i.e. 1-D damper along the X0 axis
      CDAMPER_1D_X1    = 2,                                        // damper is 1xuiDim_X1, i.e. 1-D damper along the X1 axis
      CDAMPER_2D       = 3                                         // damper is uiDim_X0xuiDim_X1; i.e. 2-D general case
   } CDamper_Type;

   bool          bConfigured;
   CDamper_Type  damperType;                                        // type of the damper; see above the enum definition
   unsigned int  uiDim_X0;                                          // actual number of control points along the first dimension
   unsigned int  uiDim_X1;                                          // actual number of control points along the second dimension
   float         afX0[CDAMPER_CTRLP_MAX_DIM];                       // coordinates of the control points along the first dimension
                                                                    // filled part of the array is limited to uiDim_X0 elements
   float         afX1[CDAMPER_CTRLP_MAX_DIM];                       // coordinates of the control points along the second dimension
                                                                    // filled part of the array is limited to uiDim_X1 elements
   float         afY [CDAMPER_CTRLP_MAX_DIM*CDAMPER_CTRLP_MAX_DIM]; // values of the control points
                                                                    // filled part of the array is limited to uiDim_X0*uiDim_X0 elements
                                                                    // not a 2D array, for fast copy purpose

   unsigned int  getIndex (const float fXvalue, CDamper_Type axis);
   float         getWeight(const float fXvalue, unsigned int clubbedIndex, CDamper_Type axis);

};

#endif // _DAMPER_FUNC_H_
