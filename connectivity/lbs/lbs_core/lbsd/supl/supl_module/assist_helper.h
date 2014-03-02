/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 


//
// Filename assist_helper.h
//
// $Header: X:/MKS Projects/prototype/prototype/supl_module/rcs/assist_helper.h 1.4 2008/05/08 11:14:13Z grahama Rel $
// $Locker: $
//*************************************************************************
///
/// \ingroup supl_server
///
/// \file
/// \brief
///     Assistance data helper routines declarations.
///
//*************************************************************************

#ifndef ASSIST_HELPER_H
#define ASSIST_HELPER_H

#include <stdio.h>

#include "gps_ptypes.h"
#include "GN_AGPS_api.h"
#include "GN_SUPL_api.h"

BL GN_Assist_Get_Net_Position(s_LocationId*  p_LocationId, s_GN_AGPS_GAD_Data *p_GAD);
BL GN_Assist_Get_RefTime(U2 *p_GPSWeek, U4 *p_GPSTOWMS);
BL GN_Assist_Get_Ion(s_GN_AGPS_Ion* p_Ion);
BL GN_Assist_Get_Alm_List(s_GN_AGPS_Alm_El Alm_El_List[32]);
BL GN_Assist_Get_Visible_Eph_List(s_GN_AGPS_Eph_El Eph_El_List[32]);
BL GN_Assist_Get_UTC(s_GN_AGPS_UTC* p_UTC);

#endif // ASSIST_HELPER_H
