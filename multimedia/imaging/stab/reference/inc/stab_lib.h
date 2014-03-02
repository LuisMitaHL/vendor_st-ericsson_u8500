/*****************************************************************************
 *                            C STMicroelectronics
 *    Reproduction and Communication of this document is strictly prohibited
 *      unless specifically authorized in writing by STMicroelectronics.
 *----------------------------------------------------------------------------
 * 	                             WMM/MCP/MMD/
 *****************************************************************************/
 /**
 * \file 	stab_lib.h
 * \brief 	header file for stab_lib.c
 * \author 	Jean-Marc VOLLE
 * \date 	4 Dec 2008
 *
 *
 **/
/*****************************************************************************/

#ifndef _STAB_LIB_
#define _STAB_LIB_
#include <stdio.h>
#include "stab_common.h"

/* IMPORT_C and EXPORT_C defines are used in Symbian to define symbol visibility. */
#ifndef IMPORT_C
#define IMPORT_C 
#endif
#ifndef EXPORT_C
#define EXPORT_C
#endif

#ifdef __cplusplus
extern "C" {
#endif 

IMPORT_C void stablib_stabilize(t_bool is_first_frame,
                   t_uint32 frame_width,
                   t_uint32 frame_height,
                   t_sint32 search_x,
                   t_sint32 search_y,
                   t_sint32 cropping_x,
                   t_sint32 cropping_y,
                   t_sint32 frame_rate,
                   t_sint32 *x_curve,
                   t_sint32 *y_curve
    );


IMPORT_C void stablib_report_croping_vectors (t_sint16 *p_vector_x, t_sint16 *p_vector_y);
IMPORT_C void stablib_destroy(void);
IMPORT_C void stablib_create(t_uint16 frame_width,
                  t_uint16 frame_height);

#ifdef __cplusplus
}; /* extern "C" */
#endif


#endif /* _STAB_LIB_ */
