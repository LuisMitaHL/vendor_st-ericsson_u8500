#ifndef AIQ_IMAGE_CROPPING_H
#define AIQ_IMAGE_CROPPING_H

#include "AIQ_CommonTypes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void AIQ_UYVY422Itld_cropping (const AIQ_U8         *pu8_in,
                               const AIQ_Dimensions *pDim_in,
                                     AIQ_U8         *pu8_out,
                               const AIQ_CropParams *pCropParam);
void AIQ_RGB565_cropping      (const AIQ_U8         *pu8_in,
                               const AIQ_Dimensions *pDim_in,
                                     AIQ_U8         *pu8_out,
                               const AIQ_CropParams *pCropParam);
void AIQ_YUV420SPNV21_cropping(const AIQ_U8         *pu8_in,
                               const AIQ_Dimensions *pDim_in,
                                     AIQ_U8         *pu8_out,
                               const AIQ_CropParams *pCropParam);
void AIQ_YUV420SPNV12_cropping(const AIQ_U8         *pu8_in,
                               const AIQ_Dimensions *pDim_in,
                                     AIQ_U8         *pu8_out,
                               const AIQ_CropParams *pCropParam);
void AIQ_YUV420P_cropping     (const AIQ_U8         *pu8_in,
                               const AIQ_Dimensions *pDim_in,
                                     AIQ_U8         *pu8_out,
                               const AIQ_CropParams *pCropParam);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* #ifndef AIQ_IMAGE_CROPPING_H */
