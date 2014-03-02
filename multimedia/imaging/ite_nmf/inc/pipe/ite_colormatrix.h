/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef ITE_COLORMATRIX_H_
#define ITE_COLORMATRIX_H_

//#include <share/inc/type.h>
#include "VhcElementDefs.h"
#include "ite_pageelements.h"

#include <cm/inc/cm_macros.h>



typedef struct s_page_element {
   t_uint16 addr;
   t_uint16 value;
   t_uint16 type;
} ts_page_element, *pts_page_element;


#ifdef __cplusplus
extern "C"
{
#endif

void ITE_SetColorMatrix(t_uint16 pipe, t_uint16 sensor);

#ifdef __cplusplus
}
#endif

#endif /*ITE_COLORMATRIX_H_*/
