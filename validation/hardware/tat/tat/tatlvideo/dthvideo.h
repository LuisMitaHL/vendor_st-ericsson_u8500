/*
* =====================================================================================
* 
*        Filename:  dthvideo.h
* 
*     Description:  
* 
*         Version:  1.0
*         Created:  27/08/2009 
*        Revision:  none
*        Compiler:  
* 
*          Author: GVO, 
*         � Copyright ST-Ericsson, 2009. All Rights Reserved
* 
* =====================================================================================
*/

#ifndef DTHVIDEO_H_
#define DTHVIDEO_H_

#include <dthsrvhelper/dthsrvhelper.h> /* for dth_element definition */

#if defined (TATLVIDEO_C)
#define GLOBAL 
#else
#define GLOBAL extern
#endif



GLOBAL int dth_init_service();
GLOBAL int Dth_ActVIDEO_exec(struct dth_element *elem);
GLOBAL int Dth_ActVIDEOParam_Set(struct dth_element *elem, void *value);
GLOBAL int Dth_ActVIDEOParam_Get(struct dth_element *elem, void *value);
GLOBAL int Dth_ElemVIDEOParam_Set(struct dth_element *elem, void *value);

#undef GLOBAL

#endif /* DTHVIDEO_H_ */
