/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "damper_func.h"
#include <string.h> // required for memcpy

CDamperFunc::CDamperFunc()
{
   bConfigured = false;
   uiDim_X0    = 0;
   uiDim_X1    = 0;
   damperType  = CDAMPER_CONSTANT;
   afX0[0]     = 0.0;
   afX1[0]     = 0.0;
   afY[0]      = 0.0;
}

int CDamperFunc::Configure(const unsigned int uiDim_X0p, const unsigned int uiDim_X1p, const float *pfX0, const float *pfX1, const float *pfY)
{
   if((uiDim_X0p >  CDAMPER_CTRLP_MAX_DIM) || (uiDim_X1p >  CDAMPER_CTRLP_MAX_DIM))
   {
      return -1;
   }
   if((uiDim_X0p <= 1)&&(uiDim_X1p <= 1))  // Constant damper case
   {
      damperType = CDAMPER_CONSTANT;
      uiDim_X0   = 1;
      uiDim_X1   = 1;
   }
   else if (uiDim_X0p <= 1)                // 1-D damper case in X1 axis
   {
      damperType = CDAMPER_1D_X1;
      uiDim_X0   = 1;
      uiDim_X1   = uiDim_X1p;
      memcpy(&afX1,pfX1,uiDim_X1*sizeof(float));
   }
   else if (uiDim_X1p <= 1)                // 1-D damper case in X0 axis
   {
      damperType = CDAMPER_1D_X0;
      uiDim_X0   = uiDim_X0p;
      uiDim_X1   = 1;
      memcpy(&afX0,pfX0,uiDim_X0*sizeof(float));   
   }
   else                                    // 2-D damper
   {
      damperType = CDAMPER_2D;
      uiDim_X0   = uiDim_X0p;
      uiDim_X1   = uiDim_X1p;
      memcpy(&afX1,pfX1,uiDim_X1*sizeof(float));
      memcpy(&afX0,pfX0,uiDim_X0*sizeof(float));   
   }

   memcpy(&afY, pfY, uiDim_X0*uiDim_X1*sizeof(float));
   bConfigured = true;
   return 0;
}

int CDamperFunc::Evaluate(float *pfY, const float fX0, const float fX1)
{
   unsigned int  clubbedIndex_X0;
   unsigned int  clubbedIndex_X1;
   unsigned char u8Index0_X0,u8Index1_X0,u8Index0_X1,u8Index1_X1;
   float        weight_X0;
   float        weight_X1;
   if(bConfigured == false)
   {
      return -1;
   }
   
   switch(damperType)
   {
   case CDAMPER_CONSTANT:
      *pfY = afY[0];
      break;
   case CDAMPER_1D_X0:
      clubbedIndex_X0 = getIndex(fX0,CDAMPER_1D_X0);
      weight_X0       = getWeight(fX0,clubbedIndex_X0,CDAMPER_1D_X0);
      u8Index0_X0     = (unsigned char)(clubbedIndex_X0 & 0xff);
      u8Index1_X0     = (unsigned char)((clubbedIndex_X0 >> 8) & 0xff);
      *pfY            =   (1.0f - weight_X0) * afY[u8Index0_X0]
                        + weight_X0          * afY[u8Index1_X0];
      break;
   case CDAMPER_1D_X1:
      clubbedIndex_X1 = getIndex(fX1,CDAMPER_1D_X1);
      weight_X1       = getWeight(fX1,clubbedIndex_X1,CDAMPER_1D_X1);
      u8Index0_X1     = (unsigned char)(clubbedIndex_X1 & 0xff);
      u8Index1_X1     = (unsigned char)((clubbedIndex_X1 >> 8) & 0xff);
      *pfY            =   (1.0f - weight_X1) * afY[u8Index0_X1]
                        + weight_X1          * afY[u8Index1_X1];
      break;
   case CDAMPER_2D:
      clubbedIndex_X0 = getIndex(fX0,CDAMPER_1D_X0);
      clubbedIndex_X1 = getIndex(fX1,CDAMPER_1D_X1);
      weight_X0       = getWeight(fX0,clubbedIndex_X0,CDAMPER_1D_X0);
      weight_X1       = getWeight(fX1,clubbedIndex_X1,CDAMPER_1D_X1);
      u8Index0_X0     = (unsigned char)(clubbedIndex_X0 & 0xff);
      u8Index1_X0     = (unsigned char)((clubbedIndex_X0 >> 8) & 0xff);
      u8Index0_X1     = (unsigned char)(clubbedIndex_X1 & 0xff);
      u8Index1_X1     = (unsigned char)((clubbedIndex_X1 >> 8) & 0xff);
      *pfY            =   (1.0f - weight_X0) * (1.0f - weight_X1) * afY[u8Index0_X0*uiDim_X1+u8Index0_X1]
                        + weight_X0          * (1.0f - weight_X1) * afY[u8Index1_X0*uiDim_X1+u8Index0_X1]
                        + (1.0f - weight_X0) * weight_X1          * afY[u8Index0_X0*uiDim_X1+u8Index1_X1]
                        + weight_X0          * weight_X1          * afY[u8Index1_X0*uiDim_X1+u8Index1_X1];
      break;
   }
   
   return 0;
}

unsigned int CDamperFunc::getIndex(const float fXvalue, CDamper_Type axis)
{
   unsigned char u8Index=0,u8Index0=0,u8Index1=0;
   unsigned int  uiCurDim = 0;
   float        *pfCurX = NULL;
   switch(axis)
   {
   case CDAMPER_1D_X0:
      uiCurDim = uiDim_X0;
      pfCurX   = afX0;
      break;
   case CDAMPER_1D_X1:
      uiCurDim = uiDim_X1;
      pfCurX   = afX1;
      break;
   // Handle CDAMPER_CONSTANT and CDAMPER_2D to make compiler happy
   case CDAMPER_CONSTANT:
   case CDAMPER_2D:
      return 0;
   }

   // check particular case: left padding
   if (fXvalue <= pfCurX[0])
   {
      return 0;
   }
   // check particular case: right padding
   if (fXvalue > pfCurX[uiCurDim-1])
   {
      u8Index0 = u8Index1 = uiCurDim-1;
      return (u8Index0 | (u8Index1 << 8));
   }
   // nominal case: scan through the entire array to find the indices
   for (u8Index=1; u8Index<uiCurDim; u8Index++)
   {
      if (fXvalue <= pfCurX[u8Index])
      {
         u8Index1 = u8Index;
         break;
      }
   }
   u8Index0 = (unsigned char)(u8Index1 - 1);
   return (u8Index0 | (u8Index1 << 8));
}

float CDamperFunc::getWeight(const float fXvalue, unsigned int clubbedIndex, CDamper_Type axis)
{
   unsigned char u8Index0=0,u8Index1=0;
   float        *pfCurX=NULL;
   float         fWeight=0.0;
   switch(axis)
   {
   case CDAMPER_1D_X0:
      pfCurX = afX0;
      break;
   case CDAMPER_1D_X1:
      pfCurX = afX1;
      break;
   // Handle CDAMPER_CONSTANT and CDAMPER_2D to make compiler happy
   case CDAMPER_CONSTANT:
   case CDAMPER_2D:
      return 0.0;
   }
   
   // particular case: left padding
   if(clubbedIndex == 0)
   {
      return 0.0;
   }

   // particular case: right padding
   u8Index0 = (unsigned char)(clubbedIndex & 0xff);
   u8Index1 = (unsigned char)((clubbedIndex >> 8) & 0xff);
   if(u8Index0 == u8Index1)
   {
      return 1.0;
   }

   // nominal case
   fWeight = (fXvalue - pfCurX[u8Index0]) / (pfCurX[u8Index1]-pfCurX[u8Index0]);
   return fWeight;
}
