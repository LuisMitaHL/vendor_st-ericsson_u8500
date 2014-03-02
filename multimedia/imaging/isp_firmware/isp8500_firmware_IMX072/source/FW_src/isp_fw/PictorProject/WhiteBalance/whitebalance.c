/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \file      whitebalance.c
 \brief     Declare and defines functions required for White Balance alogorithm.
 \ingroup   WhiteBalance
*/
#include "whitebalance_opinterface.h"
#include "whitebalance_ipinterface.h"

WhiteBalanceControl_ts  g_WhiteBalanceControl =
{
    WHITEBALANCE_DEFAULT_MANUAL_RED_GAIN,						// f_RedManualGain
    WHITEBALANCE_DEFAULT_MANUAL_GREEN_GAIN,						// f_GreenManualGain
    WHITEBALANCE_DEFAULT_MANUAL_BLUE_GAIN               // f_BlueManualGain
};

WhiteBalanceStatus_ts   g_WhiteBalanceStatus =
{
    WHITEBALANCE_DEFAULT_MANUAL_RED_GAIN,               // f_RedGain
    WHITEBALANCE_DEFAULT_MANUAL_RED_GAIN,               // f_GreenGain
    WHITEBALANCE_DEFAULT_MANUAL_RED_GAIN                // f_BlueGain
};

/**
 \fn        void WhiteBalance_Main(void)
 \brief
 \details
 \return    void
 \callgraph
 \callergraph
 \ingroup WhiteBalance
*/
void
WhiteBalance_Main(void)
{
    g_WhiteBalanceStatus.f_RedGain = g_WhiteBalanceControl.f_RedManualGain;
    g_WhiteBalanceStatus.f_GreenGain = g_WhiteBalanceControl.f_GreenManualGain;
    g_WhiteBalanceStatus.f_BlueGain = g_WhiteBalanceControl.f_BlueManualGain;

    return;
}

