//*************************************************************************
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 
//
// Filename pdu_defs.h
//
// $Header: X:/MKS Projects/prototype/prototype/os_abstraction/rcs/pdu_defs.h 1.40 2009/01/13 14:09:40Z grahama Rel $
// $Locker: $
//*************************************************************************
///
/// \ingroup os_abstraction
///
/// \file
/// \brief
///      PDU handling declarations.
///
//*************************************************************************

#ifndef PDU_DEFS_H
#define PDU_DEFS_H

#include <stddef.h>

#include "gps_ptypes.h"

/// Definition for level of logging.
#define PDU_LOGGING_LEV_INF  (1<<2)
#define PDU_LOGGING_LEV_DBG  (1<<3)

#define PDU_LOGGING PDU_LOGGING_LEV_DBG //Setting value to 8, corresponding to (1<<3) as per e_lbsconfig_LogSeverity.K_LBSCFG_LOG_LEV_DBG

/// Definitions for sizes to use when encoding PDUs
#define PDU_SIZE_MAX    2048

/// Structure containing PER encoded data.
typedef struct PDU_Encoded {
    U2  Length;             ///< Length of encoded data.
    U1  *p_PDU_Data;        ///< PDU in encoded form.
} s_PDU_Encoded;

/// Structure containing information for PDUs received or created.
typedef struct PDU_Buffer_Store {
    BL               PDU_Complete;  ///< Flag to indicate when the PDU has been completely read.
    U2               AvailSize;     ///< Available size of the associated PDU store.
    s_PDU_Encoded    PDU_Encoded;   ///< Structure containing length and pointer to raw asn1 data.
} s_PDU_Buffer_Store;

void asn1_PDU_Buffer_Store_Free( s_PDU_Buffer_Store **p_p_PDU_Buffer_Store );

s_PDU_Buffer_Store  *asn1_PDU_Buffer_Store_Alloc( U2 Raw_PDU_Size );


#endif // PDU_DEFS_H
