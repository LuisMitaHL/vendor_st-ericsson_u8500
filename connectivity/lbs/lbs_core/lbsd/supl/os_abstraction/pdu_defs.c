//*************************************************************************
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 
//
// Filename pdu_defs.c
//
// $Header: X:/MKS Projects/prototype/prototype/os_abstraction/rcs/pdu_defs.c 1.17 2009/01/13 14:09:40Z grahama Rel $
// $Locker: $
//*************************************************************************
///
/// \ingroup os_abstraction
///
/// \file
/// \brief
///      PDU handling functions.
///
//*************************************************************************

#include "GN_SUPL_api.h"
#include "pdu_defs.h"

//*****************************************************************************
/// \brief
///      Function for freeing up and clearing a PDU Buffer Store.
//*****************************************************************************
void asn1_PDU_Buffer_Store_Free
(
   s_PDU_Buffer_Store **p_p_PDU_Buffer_Store   ///< Address of the PDU_Buffer_Store pointer to free.
)
{
   s_PDU_Buffer_Store *p_PDU_Buffer_Store = *p_p_PDU_Buffer_Store;

   if ( p_PDU_Buffer_Store != NULL )
   {
      if( p_PDU_Buffer_Store->PDU_Encoded.p_PDU_Data != NULL )
      {
         GN_Free( p_PDU_Buffer_Store->PDU_Encoded.p_PDU_Data );
      }
      GN_Free( p_PDU_Buffer_Store );
      *p_p_PDU_Buffer_Store = NULL;
   }
}

//*****************************************************************************
/// \brief
///      Allocates memory for a Buffer store and initialises variables.
/// \returns
///      Pointer to #s_PDU_Buffer_Store
//*****************************************************************************
s_PDU_Buffer_Store *asn1_PDU_Buffer_Store_Alloc
(
   U2 Raw_PDU_Size ///< size to allocate for the encoded PDU.
)
{
   s_PDU_Buffer_Store *p_PDU_Buffer_Store;

   p_PDU_Buffer_Store = GN_Calloc( 1, sizeof( s_PDU_Buffer_Store ) );
   if ( Raw_PDU_Size != 0 )
   {
      p_PDU_Buffer_Store->PDU_Encoded.p_PDU_Data = GN_Calloc( 1, Raw_PDU_Size );
   }
   p_PDU_Buffer_Store->AvailSize = Raw_PDU_Size;
   p_PDU_Buffer_Store->PDU_Encoded.Length = 0;

   return p_PDU_Buffer_Store;
}

