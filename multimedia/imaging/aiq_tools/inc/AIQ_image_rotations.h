#ifndef AIQ_IMAGE_ROTATIONS_H
#define AIQ_IMAGE_ROTATIONS_H

#include "AIQ_CommonTypes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* YUV420 MB rotations */
void AIQ_YUV420MB_180_rotation             (AIQ_U8 *pu8_in,
                                            AIQ_U8 *pu8_out,
                                            AIQ_U32 u32_w,
                                            AIQ_U32 u32_h);
void AIQ_YUV420MB_180_rotation_inplace     (AIQ_U8 *pu8_inout,
                                            AIQ_U8 *pu8_dummy,
                                            AIQ_U32 u32_w,
                                            AIQ_U32 u32_h);
void AIQ_YUV420MB_180_rotation_inplace_neon(AIQ_U8 *pu8_inout,
                                            AIQ_U8 *pu8_dummy,
                                            AIQ_U32 u32_w,
                                            AIQ_U32 u32_h);
/* UYUV422Itld rotations */
void AIQ_UYVY422Itld_180_rotation_inplace  (AIQ_U8 *pu8_inout,
                                            AIQ_U32 u32_w,
                                            AIQ_U32 u32_h);
void AIQ_UYVY422Itld_90_rotation_inplace   (AIQ_U8 *pu8_inout,
                                            AIQ_U8 *pu8_temp,
                                            AIQ_U32 u32_w,
                                            AIQ_U32 u32_h);
void AIQ_UYVY422Itld_270_rotation_inplace  (AIQ_U8 *pu8_inout,
                                            AIQ_U8 *pu8_temp,
                                            AIQ_U32 u32_w,
                                            AIQ_U32 u32_h);

/* YUV420P rotations */
void AIQ_YUV420P_90_rotation               (AIQ_U8 *pu8_in,
                                            AIQ_U8 *pu8_out,
                                            AIQ_U32 u32_w_in,
                                            AIQ_U32 u32_h_in);
void AIQ_YUV420P_270_rotation              (AIQ_U8 *pu8_inout,
                                            AIQ_U8 *pu8_temp,
                                            AIQ_U32 u32_w_in,
                                            AIQ_U32 u32_h_in);
void AIQ_YUV420P_90_rotation_neon          (AIQ_U8 *pu8_in,
                                            AIQ_U8 *pu8_out,
                                            AIQ_U32 u32_w_in,
                                            AIQ_U32 u32_h_in);
void AIQ_YUV420P_270_rotation_neon         (AIQ_U8 *pu8_inout,
                                            AIQ_U8 *pu8_temp,
                                            AIQ_U32 u32_w_in,
                                            AIQ_U32 u32_h_in);

/* YUV420SP rotations */
void AIQ_YUV420SPNV12_90_rotation_neon     (AIQ_U8 *pu8_in,
                                            AIQ_U8 *pu8_out,
                                            AIQ_U32 u32_w_in,
                                            AIQ_U32 u32_h_in);
void AIQ_YUV420SPNV12_270_rotation_neon    (AIQ_U8 *pu8_in,
                                            AIQ_U8 *pu8_out,
                                            AIQ_U32 u32_w_in,
                                            AIQ_U32 u32_h_in);
void AIQ_YUV420SPNV21_90_rotation_neon     (AIQ_U8 *pu8_in,
                                            AIQ_U8 *pu8_out,
                                            AIQ_U32 u32_w_in,
                                            AIQ_U32 u32_h_in);
void AIQ_YUV420SPNV21_270_rotation_neon    (AIQ_U8 *pu8_in,
                                            AIQ_U8 *pu8_out,
                                            AIQ_U32 u32_w_in,
                                            AIQ_U32 u32_h_in);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* #ifndef AIQ_IMAGE_ROTATIONS_H */
