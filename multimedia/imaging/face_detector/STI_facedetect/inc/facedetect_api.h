/*******************************************************************************
 *                            C STMicroelectronics
 *    Reproduction and Communication of this document is strictly prohibited
 *      unless specifically authorized in writing by STMicroelectronics.
 *------------------------------------------------------------------------------
 *                              Imaging Division
 *------------------------------------------------------------------------------
 * $Id: facedetect_api.h.rca 1.4 Wed Jul 28 14:05:39 2010 sebastien.cleyet-merle@st.com Experimental $
 ******************************************************************************/
#ifndef __FACEDETECT_API_H__
#define __FACEDETECT_API_H__

#include "face_detector.h" /* for AIQ_faces and AIQ common types */
//#include "AIQ_CommonTypes.h"

typedef void* STI_facedetect_param_t;
typedef void* STI_RoiList_t;
typedef void* STI_Roi_t;
typedef void* STI_IMG_image_t;
typedef void* STI_RectList_t;

/**
* Algorithm parameters.
*/

AIQ_Error copy_input_image(STI_IMG_image_t inImage_v,  
                           AIQ_Float downsamplingFactor,
                           AIQ_Frame *p_Input);

void facedetect_and_convert(STI_IMG_image_t yImage_v, 
                            STI_IMG_image_t RGBImage_v , 
                            STI_facedetect_param_t param_v, 
                            STI_RectList_t pFaceList_v, 
                            STI_RoiList_t pRoi_v,
                            AIQ_Faces *faces, 
                            AIQ_Float downsamplingFactor);

#ifdef FD_EXTENDED_CFG
AIQ_Error facedetect_init_all_params(STI_facedetect_param_t *p_param,
                                   STI_RectList_t *pList_v,
                                   STI_RoiList_t *pRoiList_v,
                                   AIQ_U32 u32_width, 
                                   AIQ_U32 u32_height,
                                   STI_IMG_image_t *inImage_v,
                                   AIQ_Float *pdownsamplingFactor,
                                   AIQ_U32 u32_algo_type, 
                                   AIQ_U32 u32_period
                                   );
#else
AIQ_Error facedetect_init_all_params(STI_facedetect_param_t *p_param,
                                   STI_RectList_t *pList_v,
                                   STI_RoiList_t *pRoiList_v,
                                   AIQ_U32 u32_width, 
                                   AIQ_U32 u32_height,
                                   STI_IMG_image_t *inImage_v,
                                   AIQ_Float *pdownsamplingFactor
                                   );
#endif /* FD_EXTENDED_CFG */
void facedetect_release_all_params(STI_facedetect_param_t p_param,
                                   STI_RectList_t pList_v,
                                   STI_RoiList_t pRoiList_v,
                                   STI_IMG_image_t pinImage_v);

#endif // __FACEDETECT_API_H__
