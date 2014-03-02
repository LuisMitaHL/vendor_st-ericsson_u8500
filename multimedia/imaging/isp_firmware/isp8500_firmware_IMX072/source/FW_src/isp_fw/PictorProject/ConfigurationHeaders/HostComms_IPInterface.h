/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/**
 \file HostComms_IPInterface.h
 \brief Resolves the Input Dependencies Required by the Host Comms Module.
 \ingroup HostComms
*/
#ifndef HOSTCOMMS_IPINTERFACE_H_
#   define HOSTCOMMS_IPINTERFACE_H_

#   include "PictorhwReg.h"
#   include "VirtualRegisterList.h"

/// Should return TRUE if HOST has asked for a WRITE Operation and FALSE otherwise.
#   define HostComms_IsWriteOperation()    (Get_USER_IF_USER_IF_OPC_opcode() == 1)

/// Should return the Page Element Index from where the current Read/Write operation is being attempted.
#   define HostComms_GetAddressValue() Get_USER_IF_USER_IF_PAGE_ELEMENT_page_element()

/// Should return the value being written in case of a write operation by the host.
#   define HostComms_GetDataValue()    Get_USER_IF_USER_IF_WR_DATA_wr_data()

/// Programs the Data Register of Host Comms Register bank to "value" in case of a read operation.
#   define HostComms_SetDataValue(value)   Set_USER_IF_USER_IF_RD_DATA_rd_data(value)

/// Acknowledge the host comms operation complete
#   define HostComms_SetStatusValue(value) Set_USER_IF_USER_IF_ACK_word(Get_USER_IF_USER_IF_OPC())

/// Should return the following page information for the page 'u16_PageNumber':
/// ptrgu32_PageBaseAddr: Base address of the page.
/// u8_SizeOf32BitData: Size of 4 byte aligned segment of the page
/// u8_SizeOf16BitData: Size of 2 byte aligned segment of the page
/// bo_ReadOnly: TRUE if the page is read only FALSE otherwise
/// bo_ModeStaticWrite: TRUE if the page is mode static FALSE otherwise
/// u8_DirectHWAccess : 0 indicates IMG page address table, 1 indicates non-IMG page address table
#define HostComms_GetPageInfo(u8_DirectHWAccess, u16_PageNumber,ptrgu32_PageBaseAddr,u8_SizeOf32BitData,u8_SizeOf16BitData,bo_ReadOnly,bo_ModeStaticWrite)		VirtualRegister_GetPageInfo(u8_DirectHWAccess, u16_PageNumber,ptrgu32_PageBaseAddr,u8_SizeOf32BitData,u8_SizeOf16BitData,bo_ReadOnly,bo_ModeStaticWrite)


#endif /*HOSTCOMMS_IPINTERFACE_H_*/

