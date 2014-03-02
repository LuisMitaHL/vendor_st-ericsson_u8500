#ifndef AIQ_IMAGE_CONVERSIONS_H
#define AIQ_IMAGE_CONVERSIONS_H

#include "AIQ_CommonTypes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void AIQ_UYVY422Itld_to_YUV420P     (AIQ_U8 *pu8_in,
                                     AIQ_U8 *pu8_out,
                                     AIQ_U32 u32_w,
                                     AIQ_U32 u32_h);
void AIQ_UYVY422Itld_to_YUV420SPNV12(AIQ_U8 *pu8_in,
                                     AIQ_U8 *pu8_out,
                                     AIQ_U32 u32_w,
                                     AIQ_U32 u32_h);
void AIQ_UYVY422Itld_to_YUV420SPNV21(AIQ_U8 *pu8_in,
                                     AIQ_U8 *pu8_out,
                                     AIQ_U32 u32_w,
                                     AIQ_U32 u32_h);
void AIQ_RGB565_to_YUV420P          (AIQ_U8 *pu8_in,
                                     AIQ_U8 *pu8_out,
                                     AIQ_U32 u32_w,
                                     AIQ_U32 u32_h);
void AIQ_RGB565_to_YUV420SPNV12     (AIQ_U8 *pu8_in,
                                     AIQ_U8 *pu8_out,
                                     AIQ_U32 u32_w,
                                     AIQ_U32 u32_h);
void AIQ_RGB565_to_YUV420SPNV21     (AIQ_U8 *pu8_in,
                                     AIQ_U8 *pu8_out,
                                     AIQ_U32 u32_w,
                                     AIQ_U32 u32_h);
void AIQ_YUV420MB_to_YUV420SPNV12   (AIQ_U8 *pu8_in,
                                     AIQ_U8 *pu8_out,
                                     AIQ_U32 u32_w,
                                     AIQ_U32 u32_h);
void YUV420SPNV12_to_AIQ_YUV420MB   (AIQ_U8 *pu8_in,
                                     AIQ_U8 *pu8_out,
                                     AIQ_U32 u32_w,
                                     AIQ_U32 u32_h);
void AIQ_MBUV_to_RawUV              (AIQ_U8 *pu8_inout,
                                     AIQ_U8 *pu8_dummy,
                                     AIQ_U32 u32_w,
                                     AIQ_U32 u32_h);
void AIQ_RawUV_to_MBUV              (AIQ_U8 *pu8_inout,
                                     AIQ_U8 *pu8_dummy,
                                     AIQ_U32 u32_w,
                                     AIQ_U32 u32_h);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* #ifndef AIQ_IMAGE_CONVERSIONS_H */
