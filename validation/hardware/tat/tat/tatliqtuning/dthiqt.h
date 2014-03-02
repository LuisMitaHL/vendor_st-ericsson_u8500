/*
* =====================================================================================
* 
*        Filename:  dthiqt.h
* 
*     Description:  
* 
*         Version:  1.0
*         Created:  27/08/2009 
*        Revision:  none
*        Compiler:  
* 
*          Author: GVO, 
*         © Copyright ST-Ericsson, 2009. All Rights Reserved
* 
* =====================================================================================
*/

#ifndef DTHVIDEO_H_
#define DTHVIDEO_H_

#include <dthsrvhelper/dthsrvhelper.h> /* for dth_element definition */

#if defined (TATLIQT_C)
#define GLOBAL 
#else
#define GLOBAL extern
#endif



GLOBAL int dth_init_service();
GLOBAL int Dth_ActIQT_exec(struct dth_element *elem);
GLOBAL int Dth_ActIQTParam_Set(struct dth_element *elem, void *value);
GLOBAL int Dth_ActIQTParam_Get(struct dth_element *elem, void *value);
GLOBAL int Dth_ElemIQTParam_Set(struct dth_element *elem, void *value);
GLOBAL int Dth_ActIQTPreActionFile_GetVF(struct dth_element *elem, void *value);
GLOBAL int Dth_ActIQTPreActionFile_GetStill(struct dth_element *elem, void *value);
GLOBAL int Dth_ActIQTPreActionFile_GetVideo(struct dth_element *elem, void *value);

GLOBAL int Dth_ActPe_exec(struct dth_element *elem);
GLOBAL int Dth_ActPeParam_Set(struct dth_element *elem, void *Value);
GLOBAL int Dth_ActPeParam_Get(struct dth_element *elem, void *Value);
GLOBAL int Dth_ActIQTPreActionFile_GetRawStill(struct dth_element *elem, void *value);
GLOBAL int Dth_ActIQTPreActionFile_GetHrStill(struct dth_element *elem, void *value);
GLOBAL int Dth_ActIQTParam_SetResolutionVF(struct dth_element *elem, void *value); 
#undef GLOBAL

#endif /* DTHVIDEO_H_ */
