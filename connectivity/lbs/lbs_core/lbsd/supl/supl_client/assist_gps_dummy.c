/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 


//*************************************************************************
///
/// \ingroup supl_client
///
/// \file
/// \brief
///      SUPL server assistance helper routines implementation.
/// \details
///      Stub to satisfy rrlp_helper.c function calls which are called
///      from the rrlp_handler module by server related code.
///
/// \todo
///      Reorganise the project so the encode routines which call these
///      functions are not compiled with the client.
//*************************************************************************

#include "assist_helper.h"
#include "GN_AGPS_api.h"
#include "GN_GPS_api.h"

//*****************************************************************************
/// \brief
///      GN GPS API Function to request the current GPS Navigation solution data.
/// \details
///      This function is equivalent to #GN_GPS_Get_Nav_Data but will always
///      return the current GPS Navigation solution data if valid.
/// \returns
///      Flag to indicate if there is valid GPS Navigation solution data
///      available.
/// \retval #TRUE if GPS Navigation solution data is available.
/// \retval #FALSE if GPS Navigation solution data is not available.
//*****************************************************************************
BL GN_GPS_Get_Nav_Data_Copy(
   s_GN_GPS_Nav_Data* p_Nav_Data ///< [in] Pointer to where the GPS Library should write the Nav Data to.
);

//*************************************************************************
//BL GN_Assist_Get_Net_Position
//(
//   s_LocationId*  p_LocationId,        ///< [in] Location ID required for the SUPL exchange.
//   s_GN_AGPS_GAD_Data *p_GAD           ///< [out] Pointer to the output GAD Data.
//)
//{
//   return TRUE;
//}

//*************************************************************************

//*****************************************************************************
/// \brief
///      Gets a reference time.
/// \details
///      Always return false as this is for linking purposes only.
/// \returns
///      Flag indicating success or failure.
/// \retval
///      #TRUE Indicates a reference time was obtainable.
/// \retval
///      #FALSE Indicates a reference time was not obtainable.
//*************************************************************************
BL GN_Assist_Get_RefTime(
   U2 *p_GPSWeek,                      ///< [out] GPS Week Number [inc roll-overs]
   U4 *p_GPSTOWMS                      ///< [out] GPS Time of Week [range 0..6047999999 ms]
)
{
   p_GPSWeek  = p_GPSWeek;
   p_GPSTOWMS = p_GPSTOWMS;
   return FALSE;
}

//*****************************************************************************
/// \brief
///      Gets a Ionospheric corrections.
/// \details
///      Always return false as this is for linking purposes only.
/// \returns
///      Flag indicating success or failure.
/// \retval
///      #TRUE Indicates Ionospheric corrections were obtainable.
/// \retval
///      #FALSE Indicates Ionospheric corrections were not obtainable.
//*************************************************************************
BL GN_Assist_Get_Ion(
   s_GN_AGPS_Ion *p_Ion                ///< [in] Pointer to where the Host software can get the Ionosphere model parameters from.
)
{
   p_Ion = p_Ion;
   return FALSE;
}

//*****************************************************************************
/// \brief
///      Gets an Almanac.
/// \details
///      Always return false as this is for linking purposes only.
/// \returns
///      Flag indicating success or failure.
/// \retval
///      #TRUE Indicates an Almanac was obtainable.
/// \retval
///      #FALSE Indicates an Almanac was not obtainable.
//*************************************************************************
BL GN_Assist_Get_Alm_List(
   s_GN_AGPS_Alm_El Alm_El_List[32]    ///< [out] Address of an array of Almanac Elements to populate.
)
{
   Alm_El_List = Alm_El_List;
   return FALSE;
}

//*****************************************************************************
/// \brief
///      Gets a set of ephemerides.
/// \details
///      This function is implemented to retrieve the ephemerides from the gps
///      core to facilitate logging.
/// \returns
///      Flag indicating success or failure.
/// \retval
///      #TRUE Indicates a set of ephemerides was obtainable.
/// \retval
///      #FALSE Indicates a set of ephemerides was not obtainable.
//*************************************************************************
BL GN_Assist_Get_Visible_Eph_List(
   s_GN_AGPS_Eph_El     Eph_El_List[32]      ///< [out] Address of an array of Ephemeris Elements to populate.
)
{
   BL Status = FALSE;
   U1 index;
   s_GN_GPS_Nav_Data GN_GPS_SUPL_Nav_Data;   // The latest Nav solution

   GN_GPS_Get_Nav_Data_Copy( &GN_GPS_SUPL_Nav_Data );

   for ( index = 0 ; ( index < 32 ) && ( GN_GPS_SUPL_Nav_Data.SatsInViewSVid[index] != 0 ) ; index++ )
   {
      // Set Status TRUE if any ephemerides available.
      Status  |=  GN_AGPS_Get_Eph_El( GN_GPS_SUPL_Nav_Data.SatsInViewSVid[index],
                                      &Eph_El_List[ GN_GPS_SUPL_Nav_Data.SatsInViewSVid[index] - 1] );
   }
   return Status;
}

//*****************************************************************************
/// \brief
///      Gets UTC Parameters.
/// \details
///      Always return false as this is for linking purposes only.
/// \returns
///      Flag indicating success or failure.
/// \retval
///      #TRUE Indicates UTC Parameters were obtainable.
/// \retval
///      #FALSE Indicates UTC Parameters were not obtainable.
//*************************************************************************
BL GN_Assist_Get_UTC(
   s_GN_AGPS_UTC* p_UTC                ///< [in] Pointer to where the Host software can get the UTC Correction model parameters from.
)
{
   p_UTC = p_UTC;
   return FALSE;
}

//*************************************************************************
