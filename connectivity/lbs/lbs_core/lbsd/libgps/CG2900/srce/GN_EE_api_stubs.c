
//****************************************************************************
// GPS IP Centre, ST-Ericsson (UK) Ltd.
// Copyright (C) 2007-2009 ST-Ericsson (UK) Ltd.
// 15-16 Cottesbrooke Park, Heartlands Business Park, Daventry, NN11 8YL, UK.
// All rights reserved.
//
// Filename  GN_EE_api_stubs.c
//
// $Header: S:/GN_49xxPE_MKS/49xxPE_API/rcs/GN_EE_api_stubs.c 1.3 2011/06/28 10:25:03Z pfleming Rel $
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
//  These API functions are called by the main GN_GPS_api.h functions but only
//  Extended Ephemeris enabled GN_GPS Library build,  ie from:
//     GN_GPS_Initialise(), GN_GPS_Update() and GN_GPS_Shutdown().
//
//  If Extended Ephemeris functionality is not required, then these GN_EE_???()
//  API's can be simply replaced with stubs.
//
//*****************************************************************************


//*****************************************************************************
// GN EE Function that performs all the GN Extended Ephemeris Software
// Initialisation activities at the GPS sub-system Initialisation.
// This function is called from the end of GN_GPS_Initialise() in EE enabled
// GN_GPS Library builds.

void GN_EE_Initialise( void )
{
   // Extended Ephemeris is not supported so there is nothing to do.
   return;
}


//*****************************************************************************
// GN EE Function that requests a new EE GPS Ephemeris Element record input
// for the specified satellite PRN number.
// This function is called from GN_GPS_Update() in EE enabled GN_GPS Library
// builds, if neither a current valid Broadcast or A-GPS ephemeris is
// available internally.
// Returns TRUE if a valid EE GPS Ephemeris Element record is returned.

BL GN_EE_Get_GPS_Eph_El(
   U1 SV,                           // i  - Satellite identification (PRN)
                                    //         number [range 1..32].
   U2 WeekNo,                       // i  - Current full GPS Week No (including
                                    //         1024 roll overs) [range 0..4095].
   U4 TOW_s,                        // i  - Current GPS Time of Week
                                    //         [range 0..604799].
   s_GN_EE_GPS_Eph_El *p_Eph_El )   //  o - Pointer to where the EE module can
                                    //         write the GPS Ephemeris Elements to.
{
   // Extended Ephemeris is not supported so ignore the ephemeris request.

   // Remove compiler Warnings!
   SV       = SV;
   WeekNo   = WeekNo;
   TOW_s    = TOW_s;
   p_Eph_El = p_Eph_El;

   return( FALSE );
}

//*****************************************************************************
// GN EE Function that outputs a new Broadcast or A-GPS Ephemeris Element
// record for the specified satellite PRN number, so that the EE module may
// use it to improve upon its Extended Ephemeris.
// This function is called from GN_GPS_Update() in EE enabled GN_GPS Library
// builds, whenever a new Broadcast or A-GPS ephemeris record becomes
// available internally.
// Returns TRUE if a EE GPS Ephemeris Element record was accepted.

BL GN_EE_Set_GPS_Eph_El(
   s_GN_EE_GPS_Eph_El *p_Eph_El )   // i  - Pointer to where the EE module
                                    //         can read the GPS Ephemeris
                                    //         Elements from.
{
   // Extended Ephemeris is not supported so ignore the ephemeris data provided.

   // Remove compiler Warnings!
   p_Eph_El = p_Eph_El;

   return( FALSE );
}


//*****************************************************************************
// GN EE Function that performs all the GN Extended Ephemeris Software
// Shutdown activities at the GPS sub-system Shutdown.
// This is called from the end of GN_GPS_Shutdown() in EE enabled builds.

void GN_EE_Shutdown( void )
{
   // Extended Ephemeris is not supported so there is nothing to do.
   return;
}


//*****************************************************************************



