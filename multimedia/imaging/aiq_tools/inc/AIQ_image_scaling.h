#ifndef AIQ_IMAGE_SCALING_H
#define AIQ_IMAGE_SCALING_H

#include "AIQ_CommonTypes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


void AIQ_UYVY422Itld_scaling(const AIQ_U8         *pu8_in,
                             const AIQ_Dimensions *pDim_in,
                                   AIQ_U8         *pu8_out,
                             const AIQ_Dimensions *pDim_out,
                                   AIQ_U8         *pu8_dummy);
void AIQ_RGB565_scaling     (const AIQ_U8         *pu8_in,
                             const AIQ_Dimensions *pDim_in,
                                   AIQ_U8         *pu8_out,
                             const AIQ_Dimensions *pDim_out,
                                   AIQ_U8         *pu8_dummy);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* #ifndef AIQ_IMAGE_SCALING_H */
