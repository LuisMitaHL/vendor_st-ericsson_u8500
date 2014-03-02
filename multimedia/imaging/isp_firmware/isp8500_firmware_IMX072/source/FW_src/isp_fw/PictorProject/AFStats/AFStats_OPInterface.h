/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 * \file    AFStats_OPInterface.h
 * \brief   Header File containing the output interface provided by the AFStats Module.
 * \ingroup AFStats
 */
#ifndef AFSTATS_OPINTERFACE_H_
#   define AFSTATS_OPINTERFACE_H_
#   include "AFStats.h"

extern void AFStats_SetupStats (void);
extern void AFStats_ProcessingHostCmd (void);
extern void HandlePendingAFRequest (void) TO_EXT_DDR_PRGM_MEM;
extern void AFStats_Cancel (void) TO_EXT_DDR_PRGM_MEM;
extern void AFStats_CompleteStatsNotify (void) TO_EXT_DDR_PRGM_MEM;
extern void AFStats_StatsIntEnable (void) TO_EXT_DDR_PRGM_MEM;
extern void AFStats_StatsIntDisableAll (void) TO_EXT_DDR_PRGM_MEM;
extern void AFStats_StatsIntClearAll (void) TO_EXT_DDR_PRGM_MEM;

extern volatile Flag_te g_AFStatsNotifyPending;

#   define AFStats_GetForcedAFIrq()        (g_AFStats_Status.e_Flag_ForcedAFStatsIrq)
#   define AFStats_SetForcedAFIrq(Value)   (g_AFStats_Status.e_Flag_ForcedAFStatsIrq = Value)
#   define AFStats_TriggerStatsInterrupt()                      \
    {                                                           \
        g_AFStats_Status.e_Flag_ForcedAFStatsIrq = Flag_e_TRUE; \
        g_AFStats_IntCtrl1.u8_Byte = 0xff;                      \
    }


#   define AFStats_SetStatsHostcmdCtrl(x)          (g_AFStats_Controls.e_AFStats_HostCmd_Ctrl = x)
#   define AFStats_GetStatsExportStatus()          (g_AFStats_Status.e_Coin_AFStatsExportStatus)
#   define AFStats_SetStatsExportCtrl(x)           (g_AFStats_Controls.e_Coin_AFStatsExportCmd = x)
#   define AFStats_GetAutoRefreshCtrl()            (g_AFStats_Controls.e_Flag_AutoRefresh)
#   define AFStats_SetAutoRefreshCtrl(x)           (g_AFStats_Controls.e_Flag_AutoRefresh = x)
#   define AFStats_GetActiveZonesNumber()          (g_AFStats_Status.u8_ActiveZonesCounter)
#   define AFStats_GetMaxFocusMeasurePerPixel()    (g_AFStats_Status.u32_MaxFocusMeasurePerPixel)
#   define AFStats_GetZoneWidth()                  (g_AFStats_Status.u16_AFZonesWidth)
#   define AFStats_GetZoneHeight()                 (g_AFStats_Status.u16_AFZonesHeight)
#   define AFStats_GetStartingAFZoneLine()         (g_AFStats_Status.u16_StartingAFZoneLine)
#   define AFStats_GetMaxNoOfZoneSupport()         AFS_HW_STATS_ZONE_NUMBER

#   define AFStats_GetStatsWithLensMoveFWStatus()  (g_AFStats_Status.e_Flag_FW_LensWithStatsStatus)
#   define AFStats_SetStatsWithLensMoveFWStatus(x) (g_AFStats_Status.e_Flag_FW_LensWithStatsStatus = x)
#   define AFStats_SetStatsCancelStatus(status)    (g_isAFStatsCancelled = (status))
#   define AFStats_IsStatsRequestCancelled()       (Flag_e_TRUE == g_isAFStatsCancelled)
#endif /*AFSTATS_OPINTERFACE_H_*/

