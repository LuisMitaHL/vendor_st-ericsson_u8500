/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/**
 \if 		INCLUDE_IN_HTML_ONLY
 \file 		VirtualRegisterList.h

 \brief		This file contains definition of the firmware page table structure.

 \ingroup 	Miscellaneous
 \endif
*/
#ifndef _VIRTUAL_REGISTER_LIST_H_
#   define _VIRTUAL_REGISTER_LIST_H_

#include "FwVersion.h"
#include "DeviceParameter.h"
#include "SystemConfig.h"
#include "HostComms_OPInterface.h"
#include "Platform.h"
#include "HostInterface.h"
#include "MasterI2C_op_interface.h"
#include "ColourMatrix_op_interface.h"
#include "RgbToYuvCoder_op_interface.h"
#include "BootManager.h"
#include "FrameDimension_op_interface.h"
#include "FrameRate_op_interface.h"
#include "Zoom_OPInterface.h"
#include "ZoomTop.h"
#include "awb_statistics_op_interface.h"
#include "ITM.h"
#include "CRM.h"
#include "weighted_statistics_processor_op_interface.h"
#include "channel_gains_op_interface.h"
#include "RSO_OPInterface.h"
#include "exposure_statistics_op_interface.h"
#include "minimum_weighted_statistics_processor_op_interface.h"
#include "GPIOManager_OpInterface.h"
#include "histogram_op_interface.h"
#include "Glace_OPInterface.h"
#include "Gamma_op_interface.h"
#include "VirtualRegisterList_WMM.h"
#include "lla_abstraction.h"
#include "AFStats.h"
#include "FLADriver.h"
#include "FocusControl.h"
#include "Adsoc.h"
#include "Babylon.h"
#include "Scorpio.h"
#include "BinningRepair.h"
#include "Gridiron_op_interface.h"
#include "Duster_op_interface.h"
#include "SDL_op_interface.h"
#include "HostComms.h"
#include "ErrorHandler.h"

/// Specifies the page property to be read only
#   define READ_ONLY_PAGE  1

/// Specifies the page property to be read write
#   define READ_WRITE_PAGE 0

/// Specifies the page property to be mode static.
/// Any change to a mode static element will only take effect
/// when streaming is started the next time.
#   define MODE_STATIC_PAGE    1

/// Specifies the page property to be non mode static.
/// Any change to a non mode static element will only take effect immediately
#   define NON_MODE_STATIC_PAGE    0

/// Page Element Info Structure
/// ---------------------------
/// This structure is used in the page element table (declared in VirtualRegisterList.c).
/// It is used to store all of the information needed to access a particular page.
typedef struct
{
    /// Specifies the base address of the page
    volatile void    *ptrv_PageAddr;

    /// Specifies the size of 32 bit segment within the page
    uint8_t u8_SizeOf32BitData;

    /// Specifies the size of 16 bit segment within the page
    uint8_t u8_SizeOf16BitData;

    /// Specifies if a page is mode static in nature.
    /// If set to MODE_STATIC_PAGE, then the page is mode static
    /// If set to NON_MODE_STATIC_PAGE, then the page is non mode static
    uint8_t u8_ModeStatic;

    /// Specifies if a page is read only in nature.
    /// If set to READ_ONLY_PAGE, then the page is read only
    /// If set to READ_WRITE_PAGE, then the page is read write
    uint8_t u8_ReadOnly;
} PageElementInfo_ts;


typedef struct
{
	/// dummy page variable for placing a page
	uint8_t u8_dummy;
}Reserved_ts;

/// Returns the page information of a page table entry u16_PageNumber.
/// The information returned is the base address of the page, the size of
/// 32 and 16 bit data segments and read only and mode static property of
/// the page.
#define VirtualRegister_GetPageInfo(u8_DirectHWAccess, u16_PageNumber, ptru32_PageBaseAddr, u8_SizeOf32BitData, u8_SizeOf16BitData, bo_ReadOnly, bo_ModeStaticWrite)    \
	{																																									\
		if (u8_DirectHWAccess == 0)/*access of IMG pages*/																												\
		{																																								\
			ptru32_PageBaseAddr = (uint32_t  *)(g_PageElementInfo[u16_PageNumber].ptrv_PageAddr);																						\
			u8_SizeOf32BitData = g_PageElementInfo[u16_PageNumber].u8_SizeOf32BitData;																					\
			u8_SizeOf16BitData = g_PageElementInfo[u16_PageNumber].u8_SizeOf16BitData;																					\
			bo_ReadOnly = g_PageElementInfo[u16_PageNumber].u8_ReadOnly;																								\
			bo_ModeStaticWrite = g_PageElementInfo[u16_PageNumber].u8_ModeStatic;																						\
		}																																								\
		else					  /*access of WMM pages*/																												\
		{																																								\
			ptru32_PageBaseAddr = (uint32_t  *)(g_PageElementInfo_WMM[u16_PageNumber].ptrv_PageAddr);																					\
			u8_SizeOf32BitData = g_PageElementInfo_WMM[u16_PageNumber].u8_SizeOf32BitData;																				\
			u8_SizeOf16BitData = g_PageElementInfo_WMM[u16_PageNumber].u8_SizeOf16BitData;																				\
			bo_ReadOnly = g_PageElementInfo_WMM[u16_PageNumber].u8_ReadOnly;																							\
			bo_ModeStaticWrite = g_PageElementInfo_WMM[u16_PageNumber].u8_ModeStatic;																					\
		}																																								\
	}

extern const PageElementInfo_ts    g_PageElementInfo[];
extern const PageElementInfo_ts    g_PageElementInfo_WMM[];
extern Reserved_ts                 g_Reserved;

#endif // _VIRTUAL_REGISTER_LIST_H_

