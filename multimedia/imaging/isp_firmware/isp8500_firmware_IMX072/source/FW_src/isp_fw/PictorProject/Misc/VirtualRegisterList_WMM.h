/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/**
 \if 		INCLUDE_IN_HTML_ONLY
 \file 		VirtualRegisterList_WMM.h

 \brief		This file contains definition of the firmware page table structure.

 \ingroup 	Miscellaneous
 \endif
*/
#ifndef _VIRTUAL_REGISTER_LIST_WMM_H_
#   define _VIRTUAL_REGISTER_LIST_WMM_H_

#include "VirtualRegisterList.h"
# include "run_mode_ctrl.h"
# include "Aperture.h"
#include "Sensor_Tuning.h"
#include "Flash.h"
#include "osttrace.h"
#include "Mozart.h"
#include "Norcos.h"
#include "HDR.h"
#if 0
typedef struct
{
    uint8_t u8_SizeOf32BitData;
    uint8_t u8_SizeOf16BitData;
    uint8_t u8_ModeStatic;
    uint8_t u8_ReadOnly;
} Dummy_ts;

extern Dummy_ts g_Dummy0;
extern Dummy_ts g_Dummy1;
extern Dummy_ts g_Dummy2;
extern Dummy_ts g_Dummy3;
#endif

//extern const PageElementInfo_ts    g_PageElementInfo_WMM[];

#endif // _VIRTUAL_REGISTER_LIST_WMM_H_

