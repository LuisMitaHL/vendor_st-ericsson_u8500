/**
 \file Duster.h
 Filename : Duster.h
 Creation Date: 18th March 2010
 COPYRIGHT       : STMicroelectronics, 2005

    This document contains proprietary and confidential information of the
                         STMicroelectronics Group.

 \brief  This file is a part of the release code. It contains the data
        structures, macros, enums and function declarations used by the module.
 \ingroup Duster
*/

/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 

#ifndef _DUSTER_H_
#define _DUSTER_H_

#include "Damper.h"
#include "Duster_PlatformSpecific.h"


/**********************************************************************************/

/*  Exported FUNCTIONS                                                            */

/**********************************************************************************/

extern void Duster_Commit(void)TO_EXT_DDR_PRGM_MEM;
extern void Duster_UpdateGaussian(void)TO_EXT_DDR_PRGM_MEM;


#endif // _DUSTER_H_
