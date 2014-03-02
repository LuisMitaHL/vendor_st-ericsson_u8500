
//****************************************************************************
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 
//
// Filename  GN_RRLP_server_api.h
//
// $Header: X:/MKS Projects/prototype/prototype/rrlp_handler/rcs/GN_RRLP_server_api.h 1.5 2009/01/07 16:17:00Z grahama Rel $
// $Locker: $
//****************************************************************************

#ifndef GN_RRLP_SERVER_API_H
#define GN_RRLP_SERVER_API_H
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
#include "supl_ptypes.h"
#include "GN_Status.h"
#include "GN_AGPS_api.h"

//*****************************************************************************
/// \addtogroup GN_RRLP_api_I
/// \{

//*****************************************************************************
/// \brief
///      RRLP Server Start in.
/// \details
///      Starts an RRLP sequence in the RRLP Server entity.
///      <p> This function is called in response to a SUPL-POS-INIT being
///      received over the SUPL connection.
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
BL GN_RRLP_Server_Start_In(
   void                       **p_Handle,                ///< [out] Opaque Handle used to coordinate requests.
   e_GN_Status                *p_Status,                 ///< [in/out] Status of disconnect and to be checked when return flag indicates failure.
   s_GN_AGPS_GAD_Data         *p_GN_AGPS_GAD_Data,       ///< [in] Reference Location in GAD format.
   e_GN_PrefMethod            GN_PrefMethod,             ///< [in] Positioning Method the Positioning subsystem should use.
   s_GN_RequestedAssistData   *s_GN_RequestedAssistData  ///< [in] Assistance data required.
);

//*****************************************************************************
/// \brief
///      RRLP Sequence End Out.
/// \details
///      Terminates an RRLP sequence in the RRLP Server entity.
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
BL GN_RRLP_Sequence_End_Out(
   void        *Handle    ///< [in] Opaque Handle used to coordinate requests.
);

//*****************************************************************************
/// \brief
///      RRLP Sequence End Out.
/// \details
///      Terminates an RRLP sequence in the RRLP Server entity due to error.
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
BL GN_RRLP_Sequence_Error_End_Out(
   void        *Handle    ///< [in] Opaque Handle used to coordinate requests.
);

/// \}

#ifdef __cplusplus
   }     // extern "C"
#endif

#endif   // GN_RRLP_SERVER_API_H
