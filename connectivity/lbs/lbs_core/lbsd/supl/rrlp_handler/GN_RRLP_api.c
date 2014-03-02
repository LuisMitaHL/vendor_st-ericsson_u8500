
//****************************************************************************
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 
//
// Filename  GN_RRLP_api.c
//
// $Header: X:/MKS Projects/prototype/prototype/rrlp_handler/rcs/GN_RRLP_api.c 1.24 2009/01/14 14:27:48Z grahama Rel $
// $Locker: $
//****************************************************************************

#ifdef __cplusplus
   extern "C" {
#endif

#include <string.h>

#include "pdu_defs.h"
#include "rrlp_interface.h"

//****************************************************************************
// File level documentation
/// \file
/// \brief
///      GloNav RRLP Internally Implemented Interface API Functions
//
//-----------------------------------------------------------------------------
/// \ingroup   GloNav_RRLP
//
/// \defgroup  GN_RRLP_api_I GloNav RRLP Library - Internally Implemented RRLP API.
//
/// \brief
///      GloNav RRLP Library - RRLP API definitions for functions implemented internally.
//
/// \details
///      GloNav RRLP Library - GPS RRLP API definitions for the enumerated
///      data types structures and functions provided in the library and can be
///      called by the host software (ie inward called).
/// \addtogroup GN_RRLP_api_I
//
//-----------------------------------------------------------------------------
/// \ingroup   GloNav_RRLP
//
/// \defgroup  GN_RRLP_api_H GloNav RRLP Library - Host Implemented RRLP API.
//
/// \brief
///      GloNav RRLP Library - RRLP API definitions for functions implemented by the host.
//
/// \details
///      GloNav RRLP Library - RRLP API definitions for the enumerated data
///      types structures and functions called by the library and must be
///      implemented by the host software to suit the target platform OS and
///      hardware configuration (ie outward called).
///      All of these functions must be implemented in order to link a final
///      solution, even if only with a stub "{ return( 0 ); }" so say that the
///      particular action requested by the library is not supported.
/// \note
///      The functions in this group must be implemented in the
///      supporting code and linked with the library for the GPS to function
///      correctly.
/// \addtogroup GN_RRLP_api_H
//
//*****************************************************************************


#include "gps_ptypes.h"
#include "GN_Status.h"
#include "GN_AGPS_api.h"
#include "rrlp_interface.h"

//*****************************************************************************
/// \addtogroup GN_RRLP_api_I
/// \{

//*****************************************************************************
/// \brief
///      Initialises the RRLP_Handler.
/// \details
///      Sets up queues, instance data and memory allocations required.
///      <p> Called once at startup by the host to initialise the RRLP handler.
/// \returns
///      Nothing.
void GN_RRLP_Handler_Init( void )
{
   RRLP_Handler_Init();
}


//*****************************************************************************
/// \brief
///      DeInitialises the RRLP_Handler.
/// \details
///      Cleans up queues, instance data and memory allocations used.
///      <p> Called once at shutdown by the host to deinitialise the RRLP handler.
/// \returns
///      Nothing.
void GN_RRLP_Handler_DeInit( void )
{
   RRLP_Handler_DeInit();
}


//*****************************************************************************
/// \brief
///      RRLP_Handler process block.
/// \details
///      Called in the main processing loop to perform the RRLP related
///      processing functions.
///      <p> This module does the main RRLP processing is exercised to
///      enable the RRLP Handler to process RRLP related events.
/// \returns
///      Nothing.
void GN_RRLP_Handler( void )
{
   RRLP_Message_Handler();
}

//*****************************************************************************
/// \brief
///      RRLP Sequence Start in.
/// \details
///      Delivers the first RRLP PDU in the sequence from an external entity.
///      <p> This function is called in response to data being sent over an
///      RRLP Control Plane connection.
///      <p> For the RRLP Sequence Start the RRLP subsystem provides a
///      Handle which is used in subsequent exchanges to tie up the RRLP
///      sequence.
///      <ul>
///         <li><var>#GN_SUCCESS \copydoc GN_SUCCESS</var>\n
///            p_Status should be set to this value when the return value
///            indicated success.</li>
///         <li><var>#GN_ERR_NO_RESOURCE \copydoc GN_ERR_NO_RESOURCE</var>\n
///            p_Status should be set to this value when there are not
///            enough resources available to the library to perform the
///            requested function.</li>
///      </ul>
/// \attention
///      The calling party is responsible for managing the memory referenced by
///      p_Status and p_PDU.
/// \returns
///      Flag to indicate success or failure of the posting of the request.
/// \retval #TRUE Flag indicating success.
/// \retval #FALSE Flag indicating failure and that the status should be checked.
BL GN_RRLP_Sequence_Start_In(
   void           **p_Handle,    ///< [out] Opaque Handle used to coordinate requests.
   U2             PDU_Size,      ///< [in] Size of data at p_PDU in bytes.
   U1             *p_PDU,        ///< [in] Pointer to data.
   U4             PDU_TimeStamp, ///< [in] Timestamp of reception of PDU.
   s_GN_RRLP_QoP  *p_GN_RRLP_QoP, ///< [in] QoP if available.
   BL              LoggingDisabled ///< [in] This is used to enable/disable logging from the RRLP module
)
{
   s_RRLP_Instance   *p_RRLP_Instance;
   s_PDU_Encoded     *p_POS_Payload    = GN_Calloc( 1, sizeof( s_PDU_Encoded ) );
   s_GN_RRLP_QoP     *p_Temp_Qop;

   GN_RRLP_Log( "+GN_RRLP_Sequence_Start_In:" );
   GN_RRLP_Log( "GN_RRLP_api_call:    RRLP<=SUPL [label=\"GN_RRLP_Sequence_Start_In:\"];" );
   p_RRLP_Instance                  = RRLP_Instance_Request_New();
   *p_Handle                        = p_RRLP_Instance->Handle;
   p_RRLP_Instance->p_RRLP->State   = state_RRLP_Transaction_In_Progress;
   p_RRLP_Instance->p_RRLP->LoggingDisabled = LoggingDisabled;

   // POS_Handle not set, this is the first in the sequence.
   if ( p_GN_RRLP_QoP != NULL )
   {
      p_Temp_Qop = GN_Calloc( 1, sizeof( s_GN_RRLP_QoP ) );
      memcpy( p_Temp_Qop, p_GN_RRLP_QoP, sizeof( s_GN_RRLP_QoP ) );
   }
   else
   {
      p_Temp_Qop = NULL;
   }

   p_POS_Payload->Length      = PDU_Size;
   p_POS_Payload->p_PDU_Data  = GN_Calloc( 1, PDU_Size );

   memcpy( p_POS_Payload->p_PDU_Data, p_PDU, PDU_Size );

   RRLP_Send_PDU_Delivery( p_RRLP_Instance,
                           NULL,
                           p_RRLP_Instance->Handle,
                           p_POS_Payload,
                           PDU_TimeStamp,
                           p_Temp_Qop );
   if ( p_Temp_Qop != NULL )
   {
      GN_Free( p_Temp_Qop );
   }
   GN_RRLP_Log( "GN_RRLP_api_call:    RRLP>>SUPL [label=\"GN_RRLP_Sequence_Start_In: Handle=%p\"];", p_RRLP_Instance->Handle );
   GN_RRLP_Log( "-GN_RRLP_Sequence_Start_In:" );
   return TRUE;
}


//*****************************************************************************
/// \brief
///      RRLP PDU delivery in.
/// \details
///      Delivers a RRLP PDU from an external entity.
///      <p> This function is called in response to data being sent over an
///      RRLP Control Plane connection.
///      <p> For the RRLP PDU delivery the Host software provides a
///      Handle which is used in subsequent exchanges to tie up the RRLP
///      sequence.
///      <ul>
///         <li><var>#GN_SUCCESS \copydoc GN_SUCCESS</var>\n
///            p_Status should be set to this value when the return value
///            indicated success.</li>
///         <li><var>#GN_ERR_NO_RESOURCE \copydoc GN_ERR_NO_RESOURCE</var>\n
///            p_Status should be set to this value when there are not
///            enough resources available to the library to perform the
///            requested function.</li>
///      </ul>
/// \attention
///      The calling party is responsible for managing the memory referenced by
///      p_Status and p_PDU.
/// \returns
///      Flag to indicate success or failure of the posting of the request.
/// \retval #TRUE Flag indicating success.
/// \retval #FALSE Flag indicating failure and that the status should be checked.
BL GN_RRLP_PDU_Delivery_In(
   void        *Handle,       ///< [in] Opaque Handle used to coordinate requests.
   U2          PDU_Size,      ///< [in] Size of data at p_PDU in bytes.
   U1          *p_PDU,        ///< [in] Pointer to data.
   U4          PDU_TimeStamp  ///< [in] Timestamp of reception of PDU.
)
{
   s_RRLP_Instance   *p_RRLP_Instance;

   GN_RRLP_Log( "+GN_RRLP_PDU_Delivery_In:" );
   GN_RRLP_Log( "GN_RRLP_api_call:    RRLP<=SUPL [label=\"GN_RRLP_PDU_Delivery_In: Handle=%p\"];", Handle );
   p_RRLP_Instance = RRLP_Instance_From_Handle( Handle );
   if(p_RRLP_Instance != NULL)
   {
        s_PDU_Encoded     *p_POS_Payload    = GN_Calloc( 1, sizeof( s_PDU_Encoded ) );

        p_POS_Payload->Length = PDU_Size;
        p_POS_Payload->p_PDU_Data = GN_Calloc( 1, PDU_Size );

        memcpy( p_POS_Payload->p_PDU_Data, p_PDU, PDU_Size );

        RRLP_Send_PDU_Delivery( p_RRLP_Instance,
                               NULL,
                               p_RRLP_Instance->Handle,
                               p_POS_Payload,
                               PDU_TimeStamp,
                               NULL );
           GN_RRLP_Log( "GN_RRLP_api_call: PDU Delivery Successful" );

   }

   GN_RRLP_Log( "GN_RRLP_PDU_Delivery_In-:" );
   return TRUE;
}


//*****************************************************************************
/// \brief
///      RRLP Sequence End in.
/// \details
///      Terminates an RRLP sequence.
///      <p> This function is called in response to the completion of an
///      RRLP Control Plane exchange.
///      <p> For the RRLP PDU delivery the Host software provides a
///      Handle which is used in subsequent exchanges to tie up the RRLP
///      sequence.
///      <ul>
///         <li><var>#GN_SUCCESS \copydoc GN_SUCCESS</var>\n
///            p_Status should be set to this value when the return value
///            indicated success.</li>
///         <li><var>#GN_ERR_NO_RESOURCE \copydoc GN_ERR_NO_RESOURCE</var>\n
///            p_Status should be set to this value when there are not
///            enough resources available to the library to perform the
///            requested function.</li>
///      </ul>
/// \attention
///      The calling party is responsible for managing the memory referenced by
///      p_Status and p_GN_AGPS_GAD_Data.
/// \returns
///      Flag to indicate success or failure of the posting of the request.
/// \retval #TRUE Flag indicating success.
/// \retval #FALSE Flag indicating failure and that the status should be checked.
BL GN_RRLP_Sequence_End_In(
   void        *Handle,    ///< [in] Opaque Handle used to coordinate requests.
   e_GN_Status *p_Status,  ///< [in/out] Status of sequence termination and to be checked when return flag indicates failure.
   s_GN_AGPS_GAD_Data
      *p_GN_AGPS_GAD_Data  ///< [out] Data structure to be populated by the position solution.
)
{
   s_RRLP_Instance   *p_RRLP_Instance;
   BL                Status            = FALSE;
   U4                GAD_Ref_TOW;
   I2                GAD_Ref_TOW_Subms;     // GAD Data Reference GPS Time of Week Sub-millisecond part [range 0..9999 x 0.0001 ms,  -1 = Unknown]

   U4 temp = 0;

   p_RRLP_Instance = RRLP_Instance_From_Handle( Handle );

   if ( p_GN_AGPS_GAD_Data != NULL )
   {
      I4 EFSP_dT_us;
      Status = GN_AGPS_Get_GAD_Data( &temp, &GAD_Ref_TOW, &GAD_Ref_TOW_Subms, &EFSP_dT_us, p_GN_AGPS_GAD_Data );
      if ( Status == FALSE )
      {
         *p_Status = GN_WRN_NO_POSITION;
      }
   }
   else
   {
      *p_Status = GN_ERR_POINTER_INVALID;
   }

   RRLP_Send_Terminate_POS_Sequence( p_RRLP_Instance,
                                     Handle );
   return Status;
}

//*****************************************************************************

/// \}

#ifdef __cplusplus
   }     // extern "C"
#endif
