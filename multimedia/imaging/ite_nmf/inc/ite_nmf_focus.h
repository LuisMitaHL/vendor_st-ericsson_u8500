/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#ifndef ITE_NMF_FOCUS_H_
#define ITE_NMF_FOCUS_H_

#include "VhcElementDefs.h"

typedef enum
{
	NMF_FOCUS_REGION_ALL,
	NMF_FOCUS_REGION_CENTER,
	NMF_FOCUS_REGION_LEFT,
	NMF_FOCUS_REGION_RIGHT,
	NMF_FOCUS_REGION_TOP,
	NMF_FOCUS_REGION_BOTTOM,
	NMF_FOCUS_REGION_TOPLEFT,
	NMF_FOCUS_REGION_TOPRIGHT,
	NMF_FOCUS_REGION_BOTTOMLEFT,
	NMF_FOCUS_REGION_BOTTOMRIGHT	
} e_nmf_Focus_Region;

t_uint8  ITE_NMF_InitFocus(void);
t_uint8  ITE_NMF_ConfigureSAF(e_nmf_Focus_Region region);
t_uint8  ITE_NMF_TrigSAF(void);

#endif /* ITE_NMF_FOCUS_H_ */
