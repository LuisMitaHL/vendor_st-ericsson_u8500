/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef STECAMSWROUTINES_H
#define STECAMSWROUTINES_H

//System includes
#include <dlfcn.h>

//Multimedia includes
#include <AIQ_CommonTypes.h>

namespace android {

typedef void (*swRoutines)(AIQ_U8 *aInBuffer,
                           AIQ_U8 *aOutBuffer,
                           AIQ_U32 aWidth,
                           AIQ_U32 aHeight);


class CamSwRoutines
{

public:

 enum Type
     {
         EConUYVY422ItldToYUV420P, /**< YUV422I -> YUV420P */
         EConUYVY422ItldToYUV420SPNV12, /**< YUV422I -> YUV420SPNV12 */
         EConUYVY422ItldToYUV420SPNV21, /**< YUV422I -> YUV420SPNV21 */
         EConRGB565ToYUV420P, /**< RGB565 -> YUV420P */
         EConRGB565ToYUV420SPNV12, /**< RGB565 -> YUV420SPNV12 */
         EConRGB565ToYUV420SPNV21, /**< RGB565 -> YUV420SPNV21 */
         ERotYUV420MB180DEG, /**< YUV420MB 180 rotation */
         ERotUYVY422Itld90DEG, /**< YUV422Itld 90 rotation */
         ERotUYVY422Itld270DEG, /**< YUV422Itld 270 rotation */
         EMaxType /**< WaterMark */
     };

  /*init to load symbols*/
  static status_t init();

  /*Deinit to unload symbols*/
  static inline void deinit();

  /* YUV422I -> YUV420P */
  static void UYVY422Itld_to_YUV420P(AIQ_U8 *aInBuffer,
                                     AIQ_U8 *aOutBuffer,
                                     AIQ_U32 aWidth,
                                     AIQ_U32 aHeight);

  /* YUV422I -> YUV420SPNV12 */
  static void UYVY422Itld_to_YUV420SP_NV12(AIQ_U8 *aInBuffer,
                                           AIQ_U8 *aOutBuffer,
                                           AIQ_U32 aWidth,
                                           AIQ_U32 aHeight);

  /* YUV422I -> YUV420SPNV21 */
  static void UYVY422Itld_to_YUV420SP_NV21(AIQ_U8 *aInBuffer,
                                           AIQ_U8 *aOutBuffer,
                                           AIQ_U32 aWidth,
                                           AIQ_U32 aHeight);

  /* RGB565 -> YUV420P */
  static void RGB565_to_YUV420P(AIQ_U8 *aInBuffer,
                                AIQ_U8 *aOutBuffer,
                                AIQ_U32 aWidth,
                                AIQ_U32 aHeight);

  /* RGB565 -> YUV420SPNV12 */
  static void RGB565_to_YUV420SP_NV12(AIQ_U8 *aInBuffer,
                                     AIQ_U8 *aOutBuffer,
                                     AIQ_U32 aWidth,
                                     AIQ_U32 aHeight);

  /* RGB565 -> YUV420SPNV21 */
  static void RGB565_to_YUV420SP_NV21(AIQ_U8 *aInBuffer,
                                     AIQ_U8 *aOutBuffer,
                                     AIQ_U32 aWidth,
                                     AIQ_U32 aHeight);

  /* YUV420 MB rotations */
  static void YUV420MB_180_rotation_inplace(AIQ_U8 *aInBuffer,
                                            AIQ_U8 *aDummyBuffer,
                                            AIQ_U32 aWidth,
                                            AIQ_U32 aHeight);

  /* UYUV422Itld rotations */
  static void UYVY422Itld_90_rotation_inplace(AIQ_U8 *aInBuffer,
                                              AIQ_U8 *aTempBuffer,
                                              AIQ_U32 aWidth,
                                              AIQ_U32 aHeight);

  static void UYVY422Itld_270_rotation_inplace(AIQ_U8 *aInBuffer,
                                               AIQ_U8 *aTempBuffer,
                                               AIQ_U32 aWidth,
                                               AIQ_U32 aHeight);

private:
  inline static void process(Type aType,
                             AIQ_U8 *aInBuffer,
                             AIQ_U8 *aTempBuffer,
                             AIQ_U32 aWidth,
                             AIQ_U32 aHeight);

private:
  static void  *mLibHandle; /**< Lib Handle */
  static const char* mSwRoutinesNames[]; /**< Routine names */
  static swRoutines mSwRoutines[EMaxType]; /* *< Symbols */

};

#include "STECamSwRoutines.inl"

}

#endif // STECAMSWROUTINES_H
