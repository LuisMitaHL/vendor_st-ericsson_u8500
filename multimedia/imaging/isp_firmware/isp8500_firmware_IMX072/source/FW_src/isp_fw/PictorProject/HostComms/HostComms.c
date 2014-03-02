/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \if INCLUDE_IN_HTML_ONLY
 \file HostComms.c

 \brief This file is a part of the release code of the host comms module.
    It is the main file which implements the host comms mechanism.
    The current implementation is based on a mailbox mechanism.

 \ingroup HostComms
 \endif
*/


#include "HostComms.h"
#include "HostCommsPlatformSpecific.h"
#include "HostComms_IPInterface.h"


/**
 \if INCLUDE_IN_HTML_ONLY
 \union HostComms_EncodedIndex_tu
 \brief Union for the Address Encoding Scheme used in the HostComms module.
 \ingroup HostComms
 \endif
*/
typedef union
{
    /// The Encoded Index sent by the HOST.
    uint16_t u16_EncodedIndex;
    struct
    {
        /// byte offset within the page
        uint8_t u8_PageOffset           :HOST_COMMS_PAGE_OFFSET_WIDTH;
        /// page number
        uint16_t u16_PageNumber         :HOST_COMMS_PAGE_NUMBER_WIDTH;
        /// tells if Fw access or direct HW access, 0 => FW access , 1 => Direct HW access.
        uint8_t u8_DirectHWAccess       :1;

    }fields_s;
} HostComms_EncodedIndex_tu;




/// Base address of the current page being
/// accessed by the host.
uint32_t  * ptrgu32_PageBaseAddr;

/// Base address of the current element being
/// accessed by the host.
uint32_t  * ptrgu32_ElementBaseAddr;

/// Indicates whether the Page Element Access
/// corresponds to IMG(0) or WMM(1)
/// Extracted by decoding the current address being accessed.
uint8_t  gu8_DirectHWAccess;

/// Size of the element being accessed. Extracted
/// by decoding the current address being accessed.
uint8_t  gu8_ElementSize;

/// Number of the page being currently accessed.
/// Extracted by decoding the current address
/// being accessed.
uint16_t  gu16_PageNumber;

/// Byte offset within the page being currently accessed.
/// Extracted by decoding the current address
/// being accessed.
uint8_t   gu8_PageOffset;

/// Set to TRUE if the current access is to a read only page.
bool_t   gbo_ReadOnly;

/// Set to TRUE if the current access is to a mode static page.
bool_t   gbo_ModeStaticWrite;

/// Status page of the Host Comms Module.
/// Contains information about the last
/// operation attempted by the host.
HostComms_Status_ts  g_HostComms_Status;


HostComms_PEDump_ts g_HostComms_PEDump;
#if HOST_COMMS_DEBUG
PageReg_ts g_PageRegDump[10];
uint8_t g_count = 0;
#endif

void DecodeAddress ( void );
void ReadData ( void );
void WriteData ( void );


/**
  \fn void HostComms_ISRs (void)
  \brief    It is the top level function which must be called in context of the
            host comms read or write interrupt.
  \return   void
  \callgraph
  \callergraph
  \ingroup  HostComms
*/

void HostComms_ISRs (void)
{
  DecodeAddress();

  if (HostComms_IsWriteOperation())
  {
      WriteData();
#if HOST_COMMS_DEBUG
      g_PageRegDump[g_count].data = Get_USER_IF_USER_IF_WR_DATA();
#endif
    }
  else
  {
      ReadData();
#if HOST_COMMS_DEBUG
      g_PageRegDump[g_count].data = Get_USER_IF_USER_IF_RD_DATA();
#endif
  }

  // update the status Page
  g_HostComms_Status.u32_PageNumberAndOffset = (uint32_t)((uint32_t)gu16_PageNumber << 16) | gu8_PageOffset;

  // update the hardware status
  HostComms_SetStatusValue(g_HostComms_Status.e_HostComms_Status);

#if HOST_COMMS_DEBUG
  g_PageRegDump[g_count].page = Get_USER_IF_USER_IF_PAGE_ELEMENT();
  g_PageRegDump[g_count].opc = Get_USER_IF_USER_IF_OPC();
  g_PageRegDump[g_count].ack =  Get_USER_IF_USER_IF_ACK();

  g_count++;
  if(g_count >9)
      g_count = 0;
#endif
}// end of function HostComms_ISRs ()


/**
 \if INCLUDE_IN_HTML_ONLY
 \fn void DecodeAddress (void)
 \brief     This function decodes the current address being accessed. It
            computes the page number and the byte offset within the page
            corresponding to the current read/write operation.
 \return    void
 \callgraph
 \callergraph
 \ingroup   HostComms
 \endif
*/
void DecodeAddress ( void )
{
    HostComms_EncodedIndex_tu EncodedIndex;


    uint8_t u8_SizeOf16BitData, u8_SizeOf32BitData;

    // fetch the encoded index
    EncodedIndex.u16_EncodedIndex = (uint16_t)(HostComms_GetAddressValue());

    // fetch the decoded index to avoid decoding multiple times
    gu16_PageNumber = EncodedIndex.fields_s.u16_PageNumber;
    gu8_PageOffset = EncodedIndex.fields_s.u8_PageOffset;
    gu8_DirectHWAccess = EncodedIndex.fields_s.u8_DirectHWAccess;

    // fetch the page information
    HostComms_GetPageInfo(gu8_DirectHWAccess, gu16_PageNumber, ptrgu32_PageBaseAddr, u8_SizeOf32BitData, u8_SizeOf16BitData, gbo_ReadOnly, gbo_ModeStaticWrite);

    //Bring PageOffset to Access Boundary
    if (gu8_PageOffset < u8_SizeOf32BitData)
    {
        gu8_PageOffset &= 0xFC;
        gu8_ElementSize = 4;
    }
    else if (gu8_PageOffset < (u8_SizeOf32BitData + u8_SizeOf16BitData))
    {
        gu8_PageOffset &= 0xFE;
        gu8_ElementSize = 2;
    }
    else
    {
        gu8_ElementSize = 1;
    }

    ptrgu32_ElementBaseAddr = (uint32_t *)((int8_t *)ptrgu32_PageBaseAddr + gu8_PageOffset);
}


/**
 \if INCLUDE_IN_HTML_ONLY
 \fn void ReadData (void )
 \brief     This function performs the actual read operation and
            returns the value read to the host. The DecodeAddress
            function must be invoked prior to calling this function
            to ensure that the current address being accessed has
            been decoded.
 \return    void
 \callgraph
 \callergraph
 \ingroup   HostComms
 \endif
*/
void ReadData ( void )
{
    if (4 == gu8_ElementSize)
    {
        HostComms_SetDataValue(*ptrgu32_ElementBaseAddr);
    }
    else if (2 == gu8_ElementSize)
    {
        HostComms_SetDataValue(*(uint16_t *)ptrgu32_ElementBaseAddr);
    }
    else
    {
        HostComms_SetDataValue(*(uint8_t *)ptrgu32_ElementBaseAddr);
    }

    //Updating the Status Register Value
    g_HostComms_Status.e_HostComms_Status = HostComms_Status_e_ReadComplete;

}// end of function ReadData()

/**
 \if INCLUDE_IN_HTML_ONLY
 \fn void WriteData (void)
 \brief     This function performs the actual write operation. The
            DecodeAddress function must be invoked prior to calling
            this function to ensure that the current address being
            accessed has been decoded.
 \return    void
 \callgraph
 \callergraph
 \ingroup   HostComms
 \endif

*/

void WriteData ( void )
{
    uint32_t u32_DataValue;

    if (gbo_ReadOnly)
    {
        // Updating the Status Register Value
        g_HostComms_Status.e_HostComms_Status = HostComms_Status_e_ReadOnlyWriteDenied;
    }
    else
    {
        u32_DataValue   =   HostComms_GetDataValue();
        if (4 == gu8_ElementSize)
        {
            *ptrgu32_ElementBaseAddr = u32_DataValue;
        }
        else if ((2 == gu8_ElementSize))
        {
            *(uint16_t *)ptrgu32_ElementBaseAddr = (uint16_t)u32_DataValue;
        }
        else
        {
            *(uint8_t *)ptrgu32_ElementBaseAddr = (uint8_t)u32_DataValue;
        }

        if (gbo_ModeStaticWrite)
        {
            g_HostComms_Status.bo_ModeStaticSetupChanged = TRUE;
        }

        // Updating the Status Register Value
        g_HostComms_Status.e_HostComms_Status = HostComms_Status_e_WriteComplete;
    }
} // end of function WriteData()

