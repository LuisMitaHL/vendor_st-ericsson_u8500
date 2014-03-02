/*
 *  Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 *  This code is ST-Ericsson proprietary and confidential.
 *  Any use of the code for whatever purpose is subject to
 *  specific written permission of ST-Ericsson SA.
 */

#ifndef GN_RRLP_API_H
#define GN_RRLP_API_H
#ifdef __cplusplus
   extern "C" {
#endif

//****************************************************************************
// File level documentation
/// \file
/// \brief
///      GloNav RRLP External Interface API Header
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
#include "GN_GPS_api.h"

//*****************************************************************************
/// \addtogroup GN_RRLP_api_I
/// \{

//-----------------------------------------------------------------------------
/// \brief
///      GN RRLP Core Position Request QoP Data.
/// \details
///      GN RRLP Core Library Quality of Position data for a Position Request.
typedef struct // GN_RRLP_QoP
{
   U1 horacc;           ///< Horizontal accuracy INTEGER(0..127).
   U1 veracc;           ///< Vertical accuracy INTEGER(0..127) OPTIONAL (0 = Not present).
   U2 maxLocAge;        ///< Maximum age of location INTEGER (0..65535) OPTIONAL (0 = Not present).
   I1 delay;            ///< Maximum permissable delay INTEGER(0..7) OPTIONAL (-1 = Not present).
} s_GN_RRLP_QoP;        // GN RRLP Core Library QoP Data

//*****************************************************************************
/// \brief
///      Initialises the RRLP_Handler.
/// \details
///      Sets up queues, instance data and memory allocations required.
///      <p> Called once at startup by the host to initialise the RRLP handler.
/// \returns
///      Nothing.
void GN_RRLP_Handler_Init( void );


//*****************************************************************************
/// \brief
///      DeInitialises the RRLP_Handler.
/// \details
///      Cleans up queues, instance data and memory allocations used.
///      <p> Called once at shutdown by the host to deinitialise the RRLP handler.
/// \returns
///      Nothing.
void GN_RRLP_Handler_DeInit( void );


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
void GN_RRLP_Handler( void );


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
   void**   p_Handle,      ///< [out] Opaque Handle used to coordinate requests.
   U2       PDU_Size,      ///< [in] Size of data at p_PDU in bytes.
   U1*      p_PDU,         ///< [in] Pointer to data.
   U4       PDU_TimeStamp, ///< [in] Time at which the PDU was received.
   s_GN_RRLP_QoP *p_GN_RRLP_QoP, ///< [in] QoP if available.
   BL        LoggingDisabled ///< [in] This is used to enable/disable logging from the RRLP module
);


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
   void*    Handle,        ///< [in] Opaque Handle used to coordinate requests.
   U2       PDU_Size,      ///< [in] Size of data at p_PDU in bytes.
   U1*      p_PDU,         ///< [in] Pointer to data.
   U4       PDU_TimeStamp  ///< [in] Time at which the PDU was received.
);


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
   void*    Handle,        ///< [in] Opaque Handle used to coordinate requests.
   e_GN_Status* p_Status,  ///< [in/out] Status of sequence termination and to be checked when return flag indicates failure.
   s_GN_AGPS_GAD_Data*
      p_GN_AGPS_GAD_Data   ///< [out] Data structure to be populated by the position solution.
);


/// \}

//*****************************************************************************
/// \addtogroup GN_RRLP_api_H
/// \{

//*****************************************************************************
/// \brief
///      RRLP PDU delivery out.
/// \details
///      Delivers an RRLP PDU to an external entity.
///      <H3>p_Status values returned:</H3>
///      <ul>
///         <li><var>#GN_SUCCESS \copydoc GN_SUCCESS</var>\n
///            p_Status should be set to this value when the return value
///            indicated success.</li>
///         <li><var>#GN_ERR_HANDLE_INVALID \copydoc GN_ERR_HANDLE_INVALID</var>\n
///            p_Status should be set to this value when the Handle
///            passed was not recognised by the host software.</li>
///         <li><var>#GN_ERR_NO_RESOURCE \copydoc GN_ERR_NO_RESOURCE</var>\n
///            p_Status should be set to this value when there are not
///            enough resources on the host to perform the requested function.</li>
///         <li><var>#GN_WRN_NOT_READY \copydoc GN_WRN_NOT_READY</var>\n
///            p_Status should be set to this value when the host is not
///            ready to perform the requested function.</li>
///      </ul>
/// \note
///      This function must be implemented by the Host software and will be
///      called by functions from within the GloNav Library.
/// \attention
///      The calling party is responsible for managing the memory referenced by
///      p_Status and p_PDU.
/// \returns
///      Flag to indicate success or failure of the posting of the request.
/// \retval #TRUE Flag indicating success.
/// \retval #FALSE Flag indicating failure and that the status should be checked.
BL GN_RRLP_PDU_Delivery_Out(
   void*    Handle,        ///< [in] Opaque Handle used to coordinate requests.
   U2       PDU_Size,      ///< [in] Size of data at p_PDU in bytes.
   U1*      p_PDU,         ///< [in] Pointer to data.
   s_GN_AGPS_GAD_Data *p_GN_AGPS_GAD_Data,  ///< [in] Pointer to position solution GAD Format (NULL is not present).
   s_GN_GPS_Nav_Data  *p_GN_GPS_Nav_Data    ///< [in] Pointer to position solution NAV Data Format (NULL is not present).
);


//*****************************************************************************
/// \brief
///      Calloc definition for GloNav library use.
/// \details
///      Platform implementation of the calloc() function returning a pointer
///      to a block of memory of (Number * Size) number of bytes.
///      The allocated memory area must be initialised to 0.
///      <p> If either of the parameters is 0 #GN_Calloc() will return #NULL.
///      <p> The memory can subsequently be deallocated using GN_Free().
/// \returns
///      Pointer to memory or NULL.
/// \retval
///      #NULL indicates an error occured while allocating memory.
/// \retval
///      Non_Null values are pointers to a block of successfully allocated memory.
void * GN_Calloc(
   U2 Number_Of_Elements,  ///< [in] Number of elements to allocate.
   U2 Size_Of_Elements     ///< [in] Size of elements to allocate.
);


//*****************************************************************************
/// \brief
///      Free definition for GloNav library use.
/// \details
///      Platform implementation of the free() function taking a pointer
///      to a block of memory to deallocate.
///      <p> The memory to deallocate will have been allocated by the GN_Calloc()
///      routine.
void GN_Free(
   void * p_mem  ///< [in] Pointer to the memory to deallocate.
);


//*****************************************************************************

/// \}

#ifdef __cplusplus
   }     // extern "C"
#endif

#endif   // GN_RRLP_API_H
