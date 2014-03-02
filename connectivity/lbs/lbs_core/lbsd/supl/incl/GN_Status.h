/*
 *  Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 *  This code is ST-Ericsson proprietary and confidential.
 *  Any use of the code for whatever purpose is subject to
 *  specific written permission of ST-Ericsson SA.
 */

#ifndef GN_STATUS_H
#define GN_STATUS_H

//****************************************************************************
/// \file
/// \brief
///     GloNav API returned Status Code definitions.
//
/// \defgroup  GN_Status  GloNav API Returned Status Code Definitions.
//
/// \brief
///     GloNav API returned Status Code definitions.
//
/// \details
///     GloNav API returned Status Code definitions for those functions that
///     provide additional error or warning information in addition to the
///     Boolean Success / Fail return paramater.
//
//*****************************************************************************

/// \addtogroup GN_Status
/// \{

//*****************************************************************************

//*****************************************************************************

#define  GN_ERR  0x100        ///< 0x100 = Mask used for all "Error" GN Status Codes.
#define  GN_WRN  0x200        ///< 0x200 = Mask used for all "Warning" GN Status Codes.

//*****************************************************************************
/// \brief
///      GN API returned Status code definitions.
/// \details
///      GloNav API returned Status Code definitions for those functions that
///      provide additional error or warning information in addition to the
///      Boolean Success / Fail return paramater.
typedef enum // GN_Status
{
   GN_SUCCESS                 =  0x000,            ///< = 0x000 : GN Success Status.

   // Error Return State Codes

   GN_ERR_HANDLE_INVALID      = (GN_ERR | 0x01),   ///< = 0x101 : GN Error Status - Handle provided was not recognised.

   GN_ERR_POINTER_INVALID     = (GN_ERR | 0x02),   ///< = 0x102 : GN Error Status - Pointer provided was not set (ie NULL).

   GN_ERR_PARAMETER_INVALID   = (GN_ERR | 0x03),   ///< = 0x103 : GN Error Status - Parameter provided was not valid (ie invalid value).

   GN_ERR_NO_RESOURCE         = (GN_ERR | 0x04),   ///< = 0x104 : GN Error Status - No resources available to perform action.

   GN_ERR_CONN_SHUTDOWN       = (GN_ERR | 0x05),   ///< = 0x105 : GN Error Status - Connection was shutdown spontaneously.

   GN_ERR_CONN_REJECTED       = (GN_ERR | 0x06),   ///< = 0x106 : GN Error Status - Connection attempt was rejected.

   GN_ERR_CONN_TIMEOUT        = (GN_ERR | 0x07),   ///< = 0x107 : GN Error Status - Connection attempt timed out.

   // Warning Return State Codes

   GN_WRN_NOT_READY           = (GN_WRN | 0x01),   ///< = 0x201 : GN Warning Status - Destination not ready.

   GN_WRN_NO_POSITION         = (GN_WRN | 0x02)    ///< = 0x201 : GN Warning Status - No position available.

}  e_GN_Status;


//*****************************************************************************
/// \}

#endif   // GN_STATUS_H

// end of file
