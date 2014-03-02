/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Author: 
 * 
 */

/**
 \defgroup  PageElement Debug interface

 \details   This module provides an debug interface for reading/writing page elements.
 \n         This used fixed address struct for communication between Host and FW
            
*/

#include "PEDebug.h"
#include "HostComms_IPInterface.h"
#include "osttrace.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "PictorProject_PEDebug_PEDebugTraces.h"
#endif



#ifdef PEDEBUG_ENABLE

//Initialize the structure
volatile DEBUG_PEInterface_ts g_DebugPEInterface = 
{
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,		
};

//Process the Debug PE request (if pending)
void DebugPE_ProcessUserInterfaceReq(void)
{
	//Most of the code here borrowed from UserIf code
	unsigned char u8_MapSel = 0, u8_SizeOf32BitData, u8_SizeOf16BitData,u8_ElementSize;
	unsigned short u16_PageNumber = 0;
	unsigned short u16_PageOffset = 0;
	Flag_te   bReadOnly;
	Flag_te   bModeStaticWrite;
	uint32_t  * ptru32_ElementBaseAddr;
	uint32_t  * ptru32_PageBaseAddr;
	
	if(g_DebugPEInterface.u32_PEControl !=g_DebugPEInterface.u32_PEStatus)
	{
		//There is a new request 

		OstTraceInt3(TRACE_ERROR, "DebugPE_ProcessUserInterfaceReq: PE Address = %x, control = %d, status = %d",g_DebugPEInterface.u32_PEAddress, g_DebugPEInterface.u32_PEControl, g_DebugPEInterface.u32_PEStatus);
		u8_MapSel = (unsigned char)((g_DebugPEInterface.u32_PEAddress & 0x8000) >> 15);
		u16_PageNumber = (unsigned short)((g_DebugPEInterface.u32_PEAddress & 0x7FC0) >> 6);
		u16_PageOffset = (unsigned short)((g_DebugPEInterface.u32_PEAddress & 0x003F));

		//Get the PE array ref
		HostComms_GetPageInfo(u8_MapSel, u16_PageNumber, ptru32_PageBaseAddr, u8_SizeOf32BitData, u8_SizeOf16BitData, bReadOnly, bModeStaticWrite);
		
	    	//Bring PageOffset to Access Boundary
	    	if (u16_PageOffset < u8_SizeOf32BitData)
	    	{
	        	u16_PageOffset &= 0xFC;
       		u8_ElementSize = 4;
	    	}
	    	else if (u16_PageOffset < (u8_SizeOf32BitData + u8_SizeOf16BitData))
	    	{
	        	u16_PageOffset &= 0xFE;
       		u8_ElementSize = 2;
	    	}
		else
		{
			u8_ElementSize = 1;
		}

		ptru32_ElementBaseAddr = (uint32_t *)((int8_t *)ptru32_PageBaseAddr + u16_PageOffset);

		if(g_DebugPEInterface.u32_PEAccess == 0x0)	
		{
			//Read
			if (4 == u8_ElementSize)
			{
				g_DebugPEInterface.u32_PEVal = *ptru32_ElementBaseAddr;
			}
			else if (2 == u8_ElementSize)
			{
				g_DebugPEInterface.u32_PEVal = *(uint16_t *)ptru32_ElementBaseAddr;
			}
			else
			{
				g_DebugPEInterface.u32_PEVal = *(uint8_t *)ptru32_ElementBaseAddr;
			 }

			OstTraceInt3(TRACE_ERROR, "DebugPE_ProcessUserInterfaceReq: PE Address = %x, PEVal = %x, baseAddr = %x\n",g_DebugPEInterface.u32_PEAddress, g_DebugPEInterface.u32_PEVal, (unsigned int)ptru32_ElementBaseAddr);
		}
		else
		{
			//Write
			if (bReadOnly)
		      	{
		      		//Do nothing
		      	}
			else
			{
				if (4 == u8_ElementSize)
				{
					*ptru32_ElementBaseAddr = g_DebugPEInterface.u32_PEVal;
				}
				else if ((2 == u8_ElementSize))
				{
					*(uint16_t *)ptru32_ElementBaseAddr = (uint16_t) g_DebugPEInterface.u32_PEVal;
				}
				else
				{
					*(uint8_t *)ptru32_ElementBaseAddr = (uint8_t) g_DebugPEInterface.u32_PEVal;
				}

				OstTraceInt3(TRACE_ERROR, "DebugPE_ProcessUserInterfaceReq:=WRITE = PE Address = %x, PEVal = %x, baseAddr = %x\n",g_DebugPEInterface.u32_PEAddress, g_DebugPEInterface.u32_PEVal, (unsigned int)ptru32_ElementBaseAddr);
			}
		}

		//Equal Status and control coin, that signifies the end of processing from FW point of view  	
		g_DebugPEInterface.u32_PEStatus = g_DebugPEInterface.u32_PEControl;
	}

	return;
}

#endif //#if (PEDEBUG_ENABLE == 1)

