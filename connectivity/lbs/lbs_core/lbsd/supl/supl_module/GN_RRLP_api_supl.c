
//****************************************************************************
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 
//
// Filename  GN_RRLP_api_supl.c
//
// $Header: X:/MKS Projects/prototype/prototype/supl_module/rcs/GN_RRLP_api_supl.c 1.13 2009/01/19 12:32:30Z grahama Rel $
// $Locker: $
//****************************************************************************

#ifdef __cplusplus
   extern "C" {
#endif

#include "pdu_defs.h"
#include "supl_interface.h"
#include "string.h"


//****************************************************************************
// File level documentation
/// \file
/// \brief
///      GloNav RRLP Externally Implemented Interface API Functions
//
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
   void*                Handle,              ///< [in] Opaque Handle used to coordinate requests.
   U2                   PDU_Size,            ///< [in] Size of data at p_PDU in bytes.
   U1*                  p_PDU,               ///< [in] Pointer to data.
   s_GN_AGPS_GAD_Data   *p_GN_AGPS_GAD_Data, ///< [in] Pointer to position solution GAD Format (NULL is not present).
   s_GN_GPS_Nav_Data    *p_GN_GPS_Nav_Data   ///< [in] Pointer to position solution NAV Data Format (NULL is not present).
)
{
   s_PDU_Buffer_Store *p_PDU_Buf = asn1_PDU_Buffer_Store_Alloc( PDU_Size );

   s_SUPL_Instance *p_SUPL_Instance = SUPL_Instance_From_POS_Handle( Handle );

   if ( p_PDU_Buf != NULL )
   {
      memcpy( p_PDU_Buf->PDU_Encoded.p_PDU_Data, p_PDU, PDU_Size );
      p_PDU_Buf->PDU_Encoded.Length = PDU_Size;
   }

   SUPL_Send_POS_Payload(
      p_SUPL_Instance,
      p_SUPL_Instance,
      p_SUPL_Instance->Handle,
      p_PDU_Buf,
      p_GN_AGPS_GAD_Data,
      p_GN_GPS_Nav_Data );
   return TRUE;
}

BL GN_RRLP_Sequence_End_Out(
   void*          Handle  ///< [in] Opaque Handle used to coordinate requests.
)
{
   s_SUPL_Instance *p_SUPL_Instance = SUPL_Instance_From_POS_Handle( Handle );

   if ( p_SUPL_Instance != NULL )
   {
      SUPL_Send_POS_Sequence_Complete( NULL,
                                       NULL,
                                       p_SUPL_Instance->Handle );
      return TRUE;
   }
   else
   {
      return FALSE;
   }
}

BL GN_RRLP_Sequence_Error_End_Out(
   void*          Handle  ///< [in] Opaque Handle used to coordinate requests.
)
{
   s_SUPL_Instance *p_SUPL_Instance = SUPL_Instance_From_POS_Handle( Handle );

   if ( p_SUPL_Instance != NULL )
   {
      SUPL_Send_POS_Sequence_Error( NULL,
                                       NULL,
                                       p_SUPL_Instance->Handle );
      return TRUE;
   }
   else
   {
      return FALSE;
   }
}

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
