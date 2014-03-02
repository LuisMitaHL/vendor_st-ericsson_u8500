/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#ifndef ITE_NMF_DZ_FUNCTIONS_H_
#define ITE_NMF_DZ_FUNCTIONS_H_

#include "hcl_defs.h"
#include <los/api/los_api.h>
#include "VhcElementDefs.h"


typedef struct _dz_param_ { 	
t_uint16 		prescalefactor;
ZoomPrescale_e 		prescaletype;
t_uint16 		PipeLRMinStep;
t_uint16 		PipeLRMaxStep;
t_uint16 		PipeHRMinStep;
t_uint16 		PipeHRMaxStep;
t_uint16 		maxzoomfactor;
float	 		LRscalelowlimit;
float 	 		HRscalelowlimit ;
} ts_dz_param, *tps_dz_param;

void ITE_DZ_PREPARE(void);
int ITE_DZ_STEP(int Step);
void ITE_DZ_RESET(void);

void ITE_DZ_GiveZoomInfo(void);

#endif /* ITE_NMF_DZ_FUNCTIONS_H_ */
