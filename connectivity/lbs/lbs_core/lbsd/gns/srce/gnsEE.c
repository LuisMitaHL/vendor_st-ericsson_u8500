/*****************************************************************************
 *  ST-Ericsson GPS Technology
 *  Copyright (C) 2009 ST-Ericsson.
 *  All rights reserved
 *  Date: 08-06-2012
 *  Author: Mohan Babu
 *  Email : mohanbabu.n@stericsson.com
 *****************************************************************************/
#include "gns.h"
#include "gnsInternal.h"
#include "gnsFsm.h"


#include "gnsEE.h"
#include "lsimee.h"

static t_GnsEEClientCallback            vg_GnsEECallback;
static t_GnsEERefLocationCallback       vg_GnsEERefLocationCallback;
static t_GnsEERefTimeCallback           vg_GnsEERefTimeCallback;
static t_GnsEEDeleteSeedCallback        vg_GnsEEDeleteSeedCallback;
static t_GnsEEBCEDataCallback           vg_GnsBCECallback;


void GNS_EEGetEphemerisRequest(t_gnsEE_ExtendedEphInd *pp_EERequest)
{
    if( NULL != vg_GnsEECallback )
    {
        vg_GnsEECallback(pp_EERequest);
    }
}

void GNS_EEGetRefLocationRequest()
{
    if( NULL != vg_GnsEERefLocationCallback )
    {
        vg_GnsEERefLocationCallback();
    }
}

void GNS_EEGetRefTimeRequest()
{
    if( NULL != vg_GnsEERefTimeCallback )
    {
        vg_GnsEERefTimeCallback();
    }
}


void GNS_EEDeleteSeedRequest()
{
    if( NULL != vg_GnsEEDeleteSeedCallback )
    {
        vg_GnsEEDeleteSeedCallback();
    }
}

void GNS_EEFeedBCEInd(t_gnsEE_NavDataBCE *pp_BCE)
{
    if( NULL != vg_GnsBCECallback )
    {
        vg_GnsBCECallback(pp_BCE);
    }
}

void GNS_EEGetEphemerisRsp(s_gnsEE_NavDataList* pp_NavDataList)
{
    lsim8_1EEGetEphemerisRsp(pp_NavDataList);
}


void GNS_EEGetRefLocationRsp(t_gnsEE_RefPosition* pp_RefPosition)
{
    lsim8_2EEGetRefLocationRsp(pp_RefPosition);
}


bool GNS_ExtendedEphemerisRegister( t_GnsEEClientCallback  v_Callback )
{
    if( v_Callback != NULL )
    {
        vg_GnsEECallback = v_Callback;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


bool GNS_ReferencePositionRegister( t_GnsEERefLocationCallback   v_Callback )
{
    if( v_Callback != NULL )
    {
        vg_GnsEERefLocationCallback = v_Callback;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


bool GNS_ReferenceTimeRegister( t_GnsEERefTimeCallback   v_Callback )
{
    if( v_Callback != NULL )
    {
        vg_GnsEERefTimeCallback = v_Callback;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


bool GNS_DeleteSeedRegister( t_GnsEEDeleteSeedCallback   v_Callback )
{
    if( v_Callback != NULL )
    {
        vg_GnsEEDeleteSeedCallback = v_Callback;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

bool GNS_BCEDataRegister( t_GnsEEBCEDataCallback   v_Callback )
{
    if( v_Callback != NULL )
    {
        vg_GnsBCECallback = v_Callback;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
