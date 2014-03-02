
//****************************************************************************
// GPS IP Centre, ST-Ericsson (UK) Ltd.
// Copyright (C) 2007-2012 ST-Ericsson (UK) Ltd.
// 15-16 Cottesbrooke Park, Heartlands Business Park, Daventry, NN11 8YL, UK.
// All rights reserved.
//
// Filename  GN_EE_api_stubs.c
//
// $Header: S:/GN_49xxPE_MKS/49xxPE_API/rcs/GN_EE_api_stubs.c 1.5 2012/03/02 15:39:42Z geraintf Rel $
// $Locker: $
//****************************************************************************

//****************************************************************************
//
//  GN Extended Ephemeris (EE) Interface API Stubs
//
//*****************************************************************************

#include "gps_ptypes.h"
#include "GN_EE_api.h"


//*****************************************************************************
//
//  GN Extended Ephemeris - Integration API Functions.
//
//  These API functions are called by the main GN_GPS_api.h functions, ie from:
//     GN_GPS_Initialise(), GN_GPS_Update() and GN_GPS_Shutdown().
//
//  If Extended Ephemeris functionality is not required, then these GN_EE_???()
//  API's can be simply replaced with the following stubs.
//
//*****************************************************************************


//*****************************************************************************
/// \brief
///      GN EE API Function that performs tall the GN Extended Ephemeris Software
///      Initialisation activities at the GNSS sub-system Initialisation.
/// \details
///      GN EE API Function that performs tall the GN Extended Ephemeris Software
///      Initialisation activities at the GNSS sub-system Initialisation.
///      This API function is called at end of GN_GPS_Initialise().
/// \returns
///      None.
void GN_EE_Initialise( void )
{
   // Extended Ephemeris is not supported so there is nothing to do.
   return;
}


//*****************************************************************************
/// \brief
///     GN EE Function that requests a new GPS Extended Ephemeris Element record
///     input for the specified GPS (or QZSS) satellite PRN number.
/// \details
///     GN EE Function that requests a new GPS Extended Ephemeris Element record
///     input for the specified GPS (or QZSS) satellite PRN number.
///     <p> The EE data fields are scaled as they appear in the broadcast
///     GPS (or QZSS) Navigation Message subframe.
///     <p>This function is called from GN_GPS_Update(), but only if neither a
///     current valid Broadcast or Assistance ephemeris is available internally.
/// \attention
///     The current GPS Time is provided as an aid to help decide which Extended
///     Ephemeris should be provided.  This should be one with an ephemeris
///     reference time toe/toc a period in the future consistent with the
///     validity period of the ephemeris, so that it is used in the
///     optimal window either side of the toe/toc.   For Rx-Networks P-GPS
///     Extended Ephemeris, the target toe/toc should be 2 minutes in the future.
/// \attention
///     This extended ephemeris data will stop being used if a valid Broadcast
///     ephemeris or Assistance ephemeris becomes available.
/// \returns
///     Flag to indicate whether the requested extended ephemeris is provided.
/// \retval #TRUE if extended ephemeris is provided.
/// \retval #FALSE if extended ephemeris is not available.
BL GN_EE_Get_GPS_Eph_El(
   U1 SV,                           ///< [in] GPS (or QZSS) Satellite identification (PRN) number [range 1..32 GPS, or 193..197 QZSS].
   U2 WeekNo,                       ///< [in] Current full GPS Week No (including 1024 roll overs) [range 0..4095 weeks].
   U4 TOW_s,                        ///< [in] Current GPS Time of Week [range 0..604799 s].
   s_GN_EE_GPS_Eph_El *p_Eph_El )   ///< [in] Pointer to destination GPS (or QZSS) Extended Ephemeris data Elements.
{
   // GPS (or QZSS) Extended Ephemeris is not supported so ignore the ephemeris request.

   // Remove compiler Warnings!
   SV       = SV;
   WeekNo   = WeekNo;
   TOW_s    = TOW_s;
   p_Eph_El = p_Eph_El;

   return( FALSE );
}


//*****************************************************************************
/// \brief
///     GN EE Function that provides a new GPS (or QZSS) Broadcast or A-GPS
///     (or A-QZSS) Assistance Ephemeris to the Extended Ephemeris provider.
/// \details
///     GN EE Function that provides a new GPS (or QZSS) Broadcast or A-GPS
///     (or A-QZSS) Assistance Ephemeris to the Extended Ephemeris provider so
///     that it can be used to aid the generation of future extended ephemeris data.
///     <p> The Ephemeris data elements fields are scaled as they appear in the
///     broadcast GPS (or A-QZSS) Navigation Message subframe.
///     <p>This function is called from GN_GPS_Update() when a new Broadcast
///     or Assistance Ephemeris becomes available internally.
/// \returns
///     Flag to indicate whether setting the ephemeris was accepted.
/// \retval #TRUE if accepted.
/// \retval #FALSE if not accepted.
BL GN_EE_Set_GPS_Eph_El(
   s_GN_EE_GPS_Eph_El *p_Eph_El )   ///< [in] Pointer to source GPS (or QZSS) Extended Ephemeris data Elements.
{
   // GPS (or QZSS) Extended Ephemeris is not supported so ignore the ephemeris data provided.

   // Remove compiler Warnings!
   p_Eph_El = p_Eph_El;

   return( FALSE );
}


//*****************************************************************************
///     GN EE Function that requests a new Glonass Extended Ephemeris Element
///     record input for the specified Glonass satellite slot number.
/// \details
///     GN EE Function that requests a new Glonass Extended Ephemeris Element
///     record input for the specified Glonass satellite slot number.
///     <p> The EE data fields are scaled as they appear in the broadcast
///     Glonass Navigation Message subframe.
///     <p>This function is called from GN_GPS_Update(), but only if neither a
///     current valid Broadcast or A-GLONASS ephemeris is available internally.
/// \attention
///     This extended ephemeris data will stop being used if a valid broadcast
///     ephemeris or A-GLONASS ephemeris becomes available.
/// \returns
///     Flag to indicate whether the requested extended ephemeris is provided.
/// \retval #TRUE if extended ephemeris is provided.
/// \retval #FALSE if extended ephemeris is not available.
BL GN_EE_Get_GLON_Eph_El(
   U1 SV,                           ///< [in] Glonass Satellite Slot number [range 1..24].
   U1 N4,                           ///< [in] Current Glonass 4yr Block Number [range 1..15].
   U2 NT,                           ///< [in] Current Glonass Day Number [range 1..1461 days within 4 year period].
   U4 TOD_s,                        ///< [in] Current Glonass Time of Day (UTC SU) [range 0..86399 s].
   s_GN_EE_GLON_Eph_El *p_Eph_El )  ///< [in] Pointer to destination Glonass Extended Ephemeris data Elements.
{
   // GLONASS Extended Ephemeris is not supported so ignore the ephemeris request.

   // Remove compiler Warnings!
   SV       = SV;
   N4       = N4;
   NT       = NT;
   TOD_s    = TOD_s;
   p_Eph_El = p_Eph_El;

   return( FALSE );
}


//*****************************************************************************
/// \brief
///     GN EE Function that provides a new Glonass Broadcast or A-GLONASS
///     Assistance Ephemeris to the Extended Ephemeris provider.
/// \details
///     GN EE Function that provides a new Glonass Broadcast or A-GLONASS
///     Assistance Ephemeris to the Extended Ephemeris provider so that it can
///     be used to aid the generation of future extended ephemeris data.
///     <p> The Ephemeris data elements fields are scaled as they appear in the
///     broadcast Glonass Navigation Message subframe.
///     <p>This function is called from GN_GPS_Update() when a new Glonass
///     Broadcast or A-Glonass Assistance Ephemeris becomes available internally.
/// \returns
///     Flag to indicate whether setting the ephemeris was accepted.
/// \retval #TRUE if accepted.
/// \retval #FALSE if not accepted.
BL GN_EE_Set_GLON_Eph_El(
   s_GN_EE_GLON_Eph_El *p_Eph_El )  ///< [in] Pointer to source GLONASS Extended Ephemeris data Elements.
{
   // GLONASS Extended Ephemeris is not supported so ignore the ephemeris data provided.

   // Remove compiler Warnings!
   p_Eph_El = p_Eph_El;

   return( FALSE );
}


//*****************************************************************************
/// \brief
///      GN EE Function that performs all the GN Extended Ephemeris Software
///      Shutdown activities at the GNSS sub-system Shutdown.
/// \details
///      GN EE Function that performs all the GN Extended Ephemeris Software
///      Shutdown activities at the GPS sub-system Shutdown.
///      This API function is called near the end of GN_GPS_Shutdown().
/// \returns
///      None.

void GN_EE_Shutdown( void )
{
   // Extended Ephemeris is not supported so there is nothing to do.
   return;
}


//*****************************************************************************



