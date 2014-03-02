/**/

/*
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*/
#include "ImgConfig.h"
#include "cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ite_boardinfo.h"
#include "ite_platform.h"
#include "VhcElementDefs.h"
#include "ite_event.h"
#include "ite_nmf_zoom_functions.h"
#include "ite_vpip.h"
#include "ite_nmf_headers.h"
#include "ite_testenv_utils.h"
#include "ite_FocusControl_Module.h"

//#include "algoutilities.h"
//#include "hi_register_acces.h"
#include "ite_sia_buffer.h"

#include <cm/inc/cm_macros.h>
#include <test/api/test.h>
#include <los/api/los_api.h>

//#include <ilos/api/ilos_api.h>
// SPECIFYING THE HOST ADDRESS WHERE THE STATISTICS FOR AF WILL BE EXPORTED .
#define AFSTATS_HOST_ADDRESS_FOR_STATS_EXPORTING        0x00001e00
#define AFSTATS_HOST_ADDRESS_AT_T1_ADDRESSING           (AFSTATS_HOST_ADDRESS_FOR_STATS_EXPORTING + 0x00060000)
#define FLADRIVER_HOST_POS_FOR_HOR_INFINITY_HIGH_LEVEL  30
#define FLADRIVER_HOST_POS_FOR_INFINITY_HIGH_LEVEL      20
#define FLADRIVER_HOST_POS_FOR_MACRO_HIGH_LEVEL         180

#define ZONE_AREA1_X_BOUNDARY_START_PER                 5
#define ZONE_AREA1_X_BOUNDARY_END_PER                   45

#define ZONE_AREA1_Y_BOUNDARY_START_PER                 5
#define ZONE_AREA1_Y_BOUNDARY_END_PER                   90

#define ZONE_AREA2_X_BOUNDARY_START_PER                 50
#define ZONE_AREA2_X_BOUNDARY_END_PER                   95

#define ZONE_AREA2_Y_BOUNDARY_START_PER                 5
#define ZONE_AREA2_Y_BOUNDARY_END_PER                   90

#define ZONE_MIN_WIDTH_PER                              10
#define ZONE_MIN_HEIGHT_PER                             5

#define ZONE_MAX_WIDTH_PER                              20
#define ZONE_MAX_HEIGHT_PER                             10


// if the host zone setup is needed at the Pre boot time then this define is not neeeded .
#define _HOST_ZONE_SETUP_IN_PRE_BOOT                    0

#define _DEBUG_READ_AT_CANSOLE                          0
#define _PRINT_MEM_BYTES                                1

#define GLOBAL_DEFAULT_TOTAL_BYTE_PRINT                 416
#define GLOBAL_DEFAULT_NUMBER_BYTE_IN_SINGLE_LINE_PRINT 4
#define MAX_ACCEPTABLE_AFSTAT_VALID_LIMIT               3

#define U16_REGION_A_LOWER_LIMIT          0x8000
#define U16_REGION_A_UPPER_LIMIT          0xFFFF
#define U16_REGION_B_LOWER_LIMIT          0x0000
#define U16_REGION_E_LOWER_LIMIT          0x0400
#define U16_REGION_E_UPPER_LIMIT          0x7FFF


void            FLADriverManualFocusTest (void);
void            AFStatsZoneSetup (void);
void            FocusControl_Simulate_SW3A_Test (t_uint32 pMemoryAddress, t_uint32 pHostMemoryAddress);
void            FocusControl_FLADMove_and_Stats_Gather (t_uint32 pMemoryAddress, t_uint32 pHostMemoryAddress);
void            FocusControl_PrintBytesExtMemory (t_uint32 pMemoryAddress, t_uint32 pHostMemoryAddress);

void            FocusControl_PrintFormat (t_uint8 u8_byteinline, t_uint32 pMemoryAddress, t_uint16 u16_totalbyte);
void            FocusControl_SpecifyPrintFormat (t_uint32 pMemoryAddress, t_uint32 pHostMemoryAddress);

void            FLADriverMoveToInfinity (void);
void            FLADriverMoveToMacro (void);
void            FLADriverMoveStepToInfinity (t_uint16 StepSize);
void            FLADriverMoveToHorMacro (void);
void            FLADriverMoveStepToMacro (t_uint16 StepSize);
void            FLADriverMoveToTargetPos (t_uint16 TargetPos);
void            FLADriverMoveToRestPos (void);
void            FLADriverMoveToHorInfinityPos (void);
void            FLADriverMoveToHyperfocal (void);
void            AFStats_ZoneAbsDimensions (void);
Result_te       AFSTatsReadyTest (char *output, t_uint32 isp_addr, t_uint32 host_addr);
void            AFStats_FrameID_Test (t_uint32 pMemoryAddress, t_uint32 pHostMemoryAddress);

void            FocusControlInitialize (void);
void            AFStats_HostZoneWithZoom (void);
void            AFStats_HostZoneInPreBoot (void);

void            AFStats_HostFocusSystem (void);
void            AFStats_HostZoneSetup (void);
void            AFStats_OnlyZoomTest (void);
void            AFStats_ZoomBugTest (void);
void            ITE_NMF_waitForLensStop (void);
void            ITE_NMF_waitForAFStatsReady (void);
void            ITE_NMF_waitForLensStopWithAFStats(void);
void            AFStats_BugFindingTest (void);
void            FocusControl_BugTest_107051 (t_uint32 pMemoryAddress, t_uint32 pHostMemoryAddress);
void            AFStats_AFWindowSystemTest (void);
void            AFStats_AFCurrentReadStructure (void);
void            AFStats_WindowConfigReads (void);
void            AFStats_HostConfigReads (void);

long int        HostZoneStatisticsList[100];
t_uint32        g_u32_lens_event_count = 0;
t_uint32        g_test_count = 0;

typedef struct  _AFpagelement_
{
    t_uint16    PE_address;
    char        *PE_name;
}


ts_AFpageelement, *tps_AFpageelement;

ts_AFpageelement FocusControlParameters_tab[] =
{
    { FocusControl_Controls_e_FocusControl_LensCommand_Control_Byte0,
            "FocusControl_Controls.e_FocusControl_LensCommand_Control" },
    { FocusControl_Status_e_Flag_LensIsMovingAtTheSOF_Byte0, "FocusControl_Status.e_Flag_LensIsMovingAtTheSOF" },
    { FocusControl_Status_e_Flag_IsStable_Byte0, "FocusControl_Status.e_Flag_IsStable" },
    { FocusControl_Status_e_Flag_LensIsMovingAtTheSOF_Byte0, "FocusControl_Status.e_Flag_LensIsMovingAtTheSOF" },
    { FocusControl_Controls_e_FocusControl_LensCommand_Control_Byte0,
            "FocusControl_Controls.e_FocusControl_LensCommand_Control" },
    { AFStats_Status_u16_WOIWidth_Byte0, "AFStats_Status.u16_WOIWidth" },
    { AFStats_Status_u16_WOIHeight_Byte0, "AFStats_Status.u16_WOIHeight" },
    { AFStats_AFZoneInt_u16_INT00_AUTOFOCUS_Byte0, "AFStats_AFZoneInt.u16_INT00_AUTOFOCUS" },
    { AFStats_AFZoneInt_u16_INT01_AUTOFOCUS_Byte0, "AFStats_AFZoneInt.u16_INT01_AUTOFOCUS" },
    { AFStats_AFZoneInt_u16_INT02_AUTOFOCUS_Byte0, "AFStats_AFZoneInt.u16_INT02_AUTOFOCUS" },
    { AFStats_AFZoneInt_u16_INT03_AUTOFOCUS_Byte0, "AFStats_AFZoneInt.u16_INT03_AUTOFOCUS" },
    { AFStats_AFZoneInt_u16_INT04_AUTOFOCUS_Byte0, "AFStats_AFZoneInt.u16_INT04_AUTOFOCUS" },
    { AFStats_AFZoneInt_u16_INT05_AUTOFOCUS_Byte0, "AFStats_AFZoneInt.u16_INT05_AUTOFOCUS" },
    { AFStats_AFZoneInt_u16_INT06_AUTOFOCUS_Byte0, "AFStats_AFZoneInt.u16_INT06_AUTOFOCUS" },
    { AFStats_AFZoneInt_u16_INT07_AUTOFOCUS_Byte0, "AFStats_AFZoneInt.u16_INT07_AUTOFOCUS" },
    { AFStats_AFZoneInt_u16_INT08_AUTOFOCUS_Byte0, "AFStats_AFZoneInt.u16_INT08_AUTOFOCUS" },
    { AFStats_AFZoneInt_u16_INT09_AUTOFOCUS_Byte0, "AFStats_AFZoneInt.u16_INT09_AUTOFOCUS" },
    { FLADriver_Status_e_Flag_LensIsMoving_Byte0, "FLADriver_Status.e_Flag_LensIsMoving" },
    { FLADriver_Status_u16_Cycles_Byte0, "FLADriver_Status.u16_Cycles" },
    { Interrupts_Count_u16_INT10_STAT2_Auto_Focus_Byte0, "Interrupts_Count.u16_INT10_STAT2_Auto_Focus" },
    { FrameRateStatus_u16_DesiredFrameLength_lines_Byte0, "FrameRateStatus.u16_DesiredFrameLength_lines" }
};

ts_AFpageelement AFStatsAddressMemoryExport_tab[70] =
{
    { AFStats_HostZoneStatus_0_u32_Focus_Byte0, "AFStats_HostZoneStatus[0].u32_Focus" },
    { AFStats_HostZoneStatus_0_u32_AFZoneStartX_Byte0, "AFStats_HostZoneStatus[0].u32_AFZoneStartX" },
    { AFStats_HostZoneStatus_0_u32_AFZoneStartY_Byte0, "AFStats_HostZoneStatus[0].u32_AFZoneStartY" },
    { AFStats_HostZoneStatus_0_u32_AFZoneEndX_Byte0, "AFStats_HostZoneStatus[0].u32_AFZoneEndX" },
    { AFStats_HostZoneStatus_0_u32_AFZoneEndY_Byte0, "AFStats_HostZoneStatus[0].u32_AFZoneEndY" },
    { AFStats_HostZoneStatus_0_u32_AFZonesWidth_Byte0, "AFStats_HostZoneStatus[0].u32_AFZonesWidth" },
    { AFStats_HostZoneStatus_0_u32_AFZonesHeight_Byte0, "AFStats_HostZoneStatus[0].u32_AFZonesHeight" },
    { AFStats_HostZoneStatus_0_u32_Light_Byte0, "AFStats_HostZoneStatus[0].u32_Light" },
    { AFStats_HostZoneStatus_0_u32_WeightAssigned_Byte0, "AFStats_HostZoneStatus[0].u32_WeightAssigned" },
    { AFStats_HostZoneStatus_0_u32_Enabled_Byte0, "AFStats_HostZoneStatus[0].u32_Enabled" },
    { AFStats_HostZoneStatus_1_u32_Focus_Byte0, "AFStats_HostZoneStatus[1].u32_Focus" },
    { AFStats_HostZoneStatus_1_u32_AFZoneStartX_Byte0, "AFStats_HostZoneStatus[1].u32_AFZoneStartX" },
    { AFStats_HostZoneStatus_1_u32_AFZoneStartY_Byte0, "AFStats_HostZoneStatus[1].u32_AFZoneStartY" },
    { AFStats_HostZoneStatus_1_u32_AFZoneEndX_Byte0, "AFStats_HostZoneStatus[1].u32_AFZoneEndX" },
    { AFStats_HostZoneStatus_1_u32_AFZoneEndY_Byte0, "AFStats_HostZoneStatus[1].u32_AFZoneEndY" },
    { AFStats_HostZoneStatus_1_u32_AFZonesWidth_Byte0, "AFStats_HostZoneStatus[1].u32_AFZonesWidth" },
    { AFStats_HostZoneStatus_1_u32_AFZonesHeight_Byte0, "AFStats_HostZoneStatus[1].u32_AFZonesHeight" },
    { AFStats_HostZoneStatus_1_u32_Light_Byte0, "AFStats_HostZoneStatus[1].u32_Light" },
    { AFStats_HostZoneStatus_1_u32_WeightAssigned_Byte0, "AFStats_HostZoneStatus[1].u32_WeightAssigned" },
    { AFStats_HostZoneStatus_1_u32_Enabled_Byte0, "AFStats_HostZoneStatus[1].u32_Enabled" },
    { AFStats_HostZoneStatus_2_u32_Focus_Byte0, "AFStats_HostZoneStatus[2].u32_Focus" },
    { AFStats_HostZoneStatus_2_u32_AFZoneStartX_Byte0, "AFStats_HostZoneStatus[2].u32_AFZoneStartX" },
    { AFStats_HostZoneStatus_2_u32_AFZoneStartY_Byte0, "AFStats_HostZoneStatus[2].u32_AFZoneStartY" },
    { AFStats_HostZoneStatus_2_u32_AFZoneEndX_Byte0, "AFStats_HostZoneStatus[2].u32_AFZoneEndX" },
    { AFStats_HostZoneStatus_2_u32_AFZoneEndY_Byte0, "AFStats_HostZoneStatus[2].u32_AFZoneEndY" },
    { AFStats_HostZoneStatus_2_u32_AFZonesWidth_Byte0, "AFStats_HostZoneStatus[2].u32_AFZonesWidth" },
    { AFStats_HostZoneStatus_2_u32_AFZonesHeight_Byte0, "AFStats_HostZoneStatus[2].u32_AFZonesHeight" },
    { AFStats_HostZoneStatus_2_u32_Light_Byte0, "AFStats_HostZoneStatus[2].u32_Light" },
    { AFStats_HostZoneStatus_2_u32_WeightAssigned_Byte0, "AFStats_HostZoneStatus[2].u32_WeightAssigned" },
    { AFStats_HostZoneStatus_2_u32_Enabled_Byte0, "AFStats_HostZoneStatus[2].u32_Enabled" },
    { AFStats_HostZoneStatus_3_u32_Focus_Byte0, "AFStats_HostZoneStatus[3].u32_Focus" },
    { AFStats_HostZoneStatus_3_u32_AFZoneStartX_Byte0, "AFStats_HostZoneStatus[3].u32_AFZoneStartX" },
    { AFStats_HostZoneStatus_3_u32_AFZoneStartY_Byte0, "AFStats_HostZoneStatus[3].u32_AFZoneStartY" },
    { AFStats_HostZoneStatus_3_u32_AFZoneEndX_Byte0, "AFStats_HostZoneStatus[3].u32_AFZoneEndX" },
    { AFStats_HostZoneStatus_3_u32_AFZoneEndY_Byte0, "AFStats_HostZoneStatus[3].u32_AFZoneEndY" },
    { AFStats_HostZoneStatus_3_u32_AFZonesWidth_Byte0, "AFStats_HostZoneStatus[3].u32_AFZonesWidth" },
    { AFStats_HostZoneStatus_3_u32_AFZonesHeight_Byte0, "AFStats_HostZoneStatus[3].u32_AFZonesHeight" },
    { AFStats_HostZoneStatus_3_u32_Light_Byte0, "AFStats_HostZoneStatus[3].u32_Light" },
    { AFStats_HostZoneStatus_3_u32_WeightAssigned_Byte0, "AFStats_HostZoneStatus[3].u32_WeightAssigned" },
    { AFStats_HostZoneStatus_3_u32_Enabled_Byte0, "AFStats_HostZoneStatus[3].u32_Enabled" },
    { AFStats_HostZoneStatus_4_u32_Focus_Byte0, "AFStats_HostZoneStatus[4].u32_Focus" },
    { AFStats_HostZoneStatus_4_u32_AFZoneStartX_Byte0, "AFStats_HostZoneStatus[4].u32_AFZoneStartX" },
    { AFStats_HostZoneStatus_4_u32_AFZoneStartY_Byte0, "AFStats_HostZoneStatus[4].u32_AFZoneStartY" },
    { AFStats_HostZoneStatus_4_u32_AFZoneEndX_Byte0, "AFStats_HostZoneStatus[4].u32_AFZoneEndX" },
    { AFStats_HostZoneStatus_4_u32_AFZoneEndY_Byte0, "AFStats_HostZoneStatus[4].u32_AFZoneEndY" },
    { AFStats_HostZoneStatus_4_u32_AFZonesWidth_Byte0, "AFStats_HostZoneStatus[4].u32_AFZonesWidth" },
    { AFStats_HostZoneStatus_4_u32_AFZonesHeight_Byte0, "AFStats_HostZoneStatus[4].u32_AFZonesHeight" },
    { AFStats_HostZoneStatus_4_u32_Light_Byte0, "AFStats_HostZoneStatus[4].u32_Light" },
    { AFStats_HostZoneStatus_4_u32_WeightAssigned_Byte0, "AFStats_HostZoneStatus[4].u32_WeightAssigned" },
    { AFStats_HostZoneStatus_4_u32_Enabled_Byte0, "AFStats_HostZoneStatus[4].u32_Enabled" },
    { AFStats_HostZoneStatus_5_u32_Focus_Byte0, "AFStats_HostZoneStatus[5].u32_Focus" },
    { AFStats_HostZoneStatus_5_u32_AFZoneStartX_Byte0, "AFStats_HostZoneStatus[5].u32_AFZoneStartX" },
    { AFStats_HostZoneStatus_5_u32_AFZoneStartY_Byte0, "AFStats_HostZoneStatus[5].u32_AFZoneStartY" },
    { AFStats_HostZoneStatus_5_u32_AFZoneEndX_Byte0, "AFStats_HostZoneStatus[5].u32_AFZoneEndX" },
    { AFStats_HostZoneStatus_5_u32_AFZoneEndY_Byte0, "AFStats_HostZoneStatus[5].u32_AFZoneEndY" },
    { AFStats_HostZoneStatus_5_u32_AFZonesWidth_Byte0, "AFStats_HostZoneStatus[5].u32_AFZonesWidth" },
    { AFStats_HostZoneStatus_5_u32_AFZonesHeight_Byte0, "AFStats_HostZoneStatus[5].u32_AFZonesHeight" },
    { AFStats_HostZoneStatus_5_u32_Light_Byte0, "AFStats_HostZoneStatus[5].u32_Light" },
    { AFStats_HostZoneStatus_5_u32_WeightAssigned_Byte0, "AFStats_HostZoneStatus[5].u32_WeightAssigned" },
    { AFStats_HostZoneStatus_5_u32_Enabled_Byte0, "AFStats_HostZoneStatus[5].u32_Enabled" },
    { AFStats_HostZoneStatus_6_u32_Focus_Byte0, "AFStats_HostZoneStatus[6].u32_Focus" },
    { AFStats_HostZoneStatus_6_u32_AFZoneStartX_Byte0, "AFStats_HostZoneStatus[6].u32_AFZoneStartX" },
    { AFStats_HostZoneStatus_6_u32_AFZoneStartY_Byte0, "AFStats_HostZoneStatus[6].u32_AFZoneStartY" },
    { AFStats_HostZoneStatus_6_u32_AFZoneEndX_Byte0, "AFStats_HostZoneStatus[6].u32_AFZoneEndX" },
    { AFStats_HostZoneStatus_6_u32_AFZoneEndY_Byte0, "AFStats_HostZoneStatus[6].u32_AFZoneEndY" },
    { AFStats_HostZoneStatus_6_u32_AFZonesWidth_Byte0, "AFStats_HostZoneStatus[6].u32_AFZonesWidth" },
    { AFStats_HostZoneStatus_6_u32_AFZonesHeight_Byte0, "AFStats_HostZoneStatus[6].u32_AFZonesHeight" },
    { AFStats_HostZoneStatus_6_u32_Light_Byte0, "AFStats_HostZoneStatus[6].u32_Light" },
    { AFStats_HostZoneStatus_6_u32_WeightAssigned_Byte0, "AFStats_HostZoneStatus[6].u32_WeightAssigned" },
    { AFStats_HostZoneStatus_6_u32_Enabled_Byte0, "AFStats_HostZoneStatus[6].u32_Enabled" },
};

ts_AFpageelement AFStats_HostZoneConfigPercentageList[25] =
{
    { AFStats_HostZoneConfigPercentage_0_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte0,
            "AFStats_HostZoneConfigPercentage[0].f_HostAFZoneStartX_PER_wrt_WOIWidth" },
    { AFStats_HostZoneConfigPercentage_0_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte0,
            "AFStats_HostZoneConfigPercentage[0].f_HostAFZoneStartY_PER_wrt_WOIHeight" },
    { AFStats_HostZoneConfigPercentage_0_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte0,
            "AFStats_HostZoneConfigPercentage[0].f_HostAFZoneEndX_PER_wrt_WOIWidth" },
    { AFStats_HostZoneConfigPercentage_0_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte0,
            "AFStats_HostZoneConfigPercentage[0].f_HostAFZoneEndY_PER_wrt_WOIHeight" },
    { AFStats_HostZoneConfigPercentage_0_e_Flag_Enabled_Byte0, "AFStats_HostZoneConfigPercentage[0].e_Flag_Enabled" },
    { AFStats_HostZoneConfigPercentage_1_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte0,
            "AFStats_HostZoneConfigPercentage[1].f_HostAFZoneStartX_PER_wrt_WOIWidth" },
    { AFStats_HostZoneConfigPercentage_1_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte0,
            "AFStats_HostZoneConfigPercentage[1].f_HostAFZoneStartY_PER_wrt_WOIHeight" },
    { AFStats_HostZoneConfigPercentage_1_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte0,
            "AFStats_HostZoneConfigPercentage[1].f_HostAFZoneEndX_PER_wrt_WOIWidth" },
    { AFStats_HostZoneConfigPercentage_1_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte0,
            "AFStats_HostZoneConfigPercentage[1].f_HostAFZoneEndY_PER_wrt_WOIHeight" },
    { AFStats_HostZoneConfigPercentage_1_e_Flag_Enabled_Byte0, "AFStats_HostZoneConfigPercentage[1].e_Flag_Enabled" },
    { AFStats_HostZoneConfigPercentage_2_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte0,
            "AFStats_HostZoneConfigPercentage[2].f_HostAFZoneStartX_PER_wrt_WOIWidth" },
    { AFStats_HostZoneConfigPercentage_2_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte0,
            "AFStats_HostZoneConfigPercentage[2].f_HostAFZoneStartY_PER_wrt_WOIHeight" },
    { AFStats_HostZoneConfigPercentage_2_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte0,
            "AFStats_HostZoneConfigPercentage[2].f_HostAFZoneEndX_PER_wrt_WOIWidth" },
    { AFStats_HostZoneConfigPercentage_2_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte0,
            "AFStats_HostZoneConfigPercentage[2].f_HostAFZoneEndY_PER_wrt_WOIHeight" },
    { AFStats_HostZoneConfigPercentage_2_e_Flag_Enabled_Byte0, "AFStats_HostZoneConfigPercentage[2].e_Flag_Enabled" },
    { AFStats_HostZoneConfigPercentage_3_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte0,
            "AFStats_HostZoneConfigPercentage[3].f_HostAFZoneStartX_PER_wrt_WOIWidth" },
    { AFStats_HostZoneConfigPercentage_3_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte0,
            "AFStats_HostZoneConfigPercentage[3].f_HostAFZoneStartY_PER_wrt_WOIHeight" },
    { AFStats_HostZoneConfigPercentage_3_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte0,
            "AFStats_HostZoneConfigPercentage[3].f_HostAFZoneEndX_PER_wrt_WOIWidth" },
    { AFStats_HostZoneConfigPercentage_3_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte0,
            "AFStats_HostZoneConfigPercentage[3].f_HostAFZoneEndY_PER_wrt_WOIHeight" },
    { AFStats_HostZoneConfigPercentage_3_e_Flag_Enabled_Byte0, "AFStats_HostZoneConfigPercentage[3].e_Flag_Enabled" },
    { AFStats_HostZoneConfigPercentage_4_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte0,
            "AFStats_HostZoneConfigPercentage[4].f_HostAFZoneStartX_PER_wrt_WOIWidth" },
    { AFStats_HostZoneConfigPercentage_4_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte0,
            "AFStats_HostZoneConfigPercentage[4].f_HostAFZoneStartY_PER_wrt_WOIHeight" },
    { AFStats_HostZoneConfigPercentage_4_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte0,
            "AFStats_HostZoneConfigPercentage[4].f_HostAFZoneEndX_PER_wrt_WOIWidth" },
    { AFStats_HostZoneConfigPercentage_4_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte0,
            "AFStats_HostZoneConfigPercentage[4].f_HostAFZoneEndY_PER_wrt_WOIHeight" },
    { AFStats_HostZoneConfigPercentage_4_e_Flag_Enabled_Byte0, "AFStats_HostZoneConfigPercentage[4].e_Flag_Enabled" },
};

ts_AFpageelement AFStats_HostZoneConfigPercentageList1[25] =
{
    { AFStats_HostZoneConfigPercentage_5_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte0,
            "AFStats_HostZoneConfigPercentage[5].f_HostAFZoneStartX_PER_wrt_WOIWidth" },
    { AFStats_HostZoneConfigPercentage_5_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte0,
            "AFStats_HostZoneConfigPercentage[5].f_HostAFZoneStartY_PER_wrt_WOIHeight" },
    { AFStats_HostZoneConfigPercentage_5_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte0,
            "AFStats_HostZoneConfigPercentage[5].f_HostAFZoneEndX_PER_wrt_WOIWidth" },
    { AFStats_HostZoneConfigPercentage_5_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte0,
            "AFStats_HostZoneConfigPercentage[5].f_HostAFZoneEndY_PER_wrt_WOIHeight" },
    { AFStats_HostZoneConfigPercentage_5_e_Flag_Enabled_Byte0, "AFStats_HostZoneConfigPercentage[5].e_Flag_Enabled" },
    { AFStats_HostZoneConfigPercentage_6_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte0,
            "AFStats_HostZoneConfigPercentage[6].f_HostAFZoneStartX_PER_wrt_WOIWidth" },
    { AFStats_HostZoneConfigPercentage_6_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte0,
            "AFStats_HostZoneConfigPercentage[6].f_HostAFZoneStartY_PER_wrt_WOIHeight" },
    { AFStats_HostZoneConfigPercentage_6_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte0,
            "AFStats_HostZoneConfigPercentage[6].f_HostAFZoneEndX_PER_wrt_WOIWidth" },
    { AFStats_HostZoneConfigPercentage_6_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte0,
            "AFStats_HostZoneConfigPercentage[6].f_HostAFZoneEndY_PER_wrt_WOIHeight" },
    { AFStats_HostZoneConfigPercentage_6_e_Flag_Enabled_Byte0, "AFStats_HostZoneConfigPercentage[6].e_Flag_Enabled" },
    { AFStats_HostZoneConfigPercentage_7_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte0,
            "AFStats_HostZoneConfigPercentage[7].f_HostAFZoneStartX_PER_wrt_WOIWidth" },
    { AFStats_HostZoneConfigPercentage_7_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte0,
            "AFStats_HostZoneConfigPercentage[7].f_HostAFZoneStartY_PER_wrt_WOIHeight" },
    { AFStats_HostZoneConfigPercentage_7_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte0,
            "AFStats_HostZoneConfigPercentage[7].f_HostAFZoneEndX_PER_wrt_WOIWidth" },
    { AFStats_HostZoneConfigPercentage_7_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte0,
            "AFStats_HostZoneConfigPercentage[7].f_HostAFZoneEndY_PER_wrt_WOIHeight" },
    { AFStats_HostZoneConfigPercentage_7_e_Flag_Enabled_Byte0, "AFStats_HostZoneConfigPercentage[7].e_Flag_Enabled" },
    { AFStats_HostZoneConfigPercentage_8_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte0,
            "AFStats_HostZoneConfigPercentage[8].f_HostAFZoneStartX_PER_wrt_WOIWidth" },
    { AFStats_HostZoneConfigPercentage_8_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte0,
            "AFStats_HostZoneConfigPercentage[8].f_HostAFZoneStartY_PER_wrt_WOIHeight" },
    { AFStats_HostZoneConfigPercentage_8_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte0,
            "AFStats_HostZoneConfigPercentage[8].f_HostAFZoneEndX_PER_wrt_WOIWidth" },
    { AFStats_HostZoneConfigPercentage_8_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte0,
            "AFStats_HostZoneConfigPercentage[8].f_HostAFZoneEndY_PER_wrt_WOIHeight" },
    { AFStats_HostZoneConfigPercentage_8_e_Flag_Enabled_Byte0, "AFStats_HostZoneConfigPercentage[8].e_Flag_Enabled" },
    { AFStats_HostZoneConfigPercentage_9_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte0,
            "AFStats_HostZoneConfigPercentage[9].f_HostAFZoneStartX_PER_wrt_WOIWidth" },
    { AFStats_HostZoneConfigPercentage_9_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte0,
            "AFStats_HostZoneConfigPercentage[9].f_HostAFZoneStartY_PER_wrt_WOIHeight" },
    { AFStats_HostZoneConfigPercentage_9_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte0,
            "AFStats_HostZoneConfigPercentage[9].f_HostAFZoneEndX_PER_wrt_WOIWidth" },
    { AFStats_HostZoneConfigPercentage_9_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte0,
            "AFStats_HostZoneConfigPercentage[9].f_HostAFZoneEndY_PER_wrt_WOIHeight" },
    { AFStats_HostZoneConfigPercentage_9_e_Flag_Enabled_Byte0, "AFStats_HostZoneConfigPercentage[9].e_Flag_Enabled" },
};

ts_AFpageelement AFStats_HostZoneConfigAbs[50] =
{
    { AFStats_HostZoneConfig_0_u16_HostAFZoneStartX_Byte0, "AFStats_HostZoneConfig[0].u16_HostAFZoneStartX" },
    { AFStats_HostZoneConfig_0_u16_HostAFZoneStartY_Byte0, "AFStats_HostZoneConfig[0].u16_HostAFZoneStartY" },
    { AFStats_HostZoneConfig_0_u16_HostAFZoneWidth_Byte0, "AFStats_HostZoneConfig[0].u16_HostAFZoneWidth" },
    { AFStats_HostZoneConfig_0_u16_HostAFZoneHeight_Byte0, "AFStats_HostZoneConfig[0].u16_HostAFZoneHeight" },
    { AFStats_HostZoneConfig_0_e_Flag_Enabled_Byte0, "AFStats_HostZoneConfig[0].e_Flag_Enabled" },
    { AFStats_HostZoneConfig_1_u16_HostAFZoneStartX_Byte0, "AFStats_HostZoneConfig[1].u16_HostAFZoneStartX" },
    { AFStats_HostZoneConfig_1_u16_HostAFZoneStartY_Byte0, "AFStats_HostZoneConfig[1].u16_HostAFZoneStartY" },
    { AFStats_HostZoneConfig_1_u16_HostAFZoneWidth_Byte0, "AFStats_HostZoneConfig[1].u16_HostAFZoneWidth" },
    { AFStats_HostZoneConfig_1_u16_HostAFZoneHeight_Byte0, "AFStats_HostZoneConfig[1].u16_HostAFZoneHeight" },
    { AFStats_HostZoneConfig_1_e_Flag_Enabled_Byte0, "AFStats_HostZoneConfig[1].e_Flag_Enabled" },
    { AFStats_HostZoneConfig_2_u16_HostAFZoneStartX_Byte0, "AFStats_HostZoneConfig[2].u16_HostAFZoneStartX" },
    { AFStats_HostZoneConfig_2_u16_HostAFZoneStartY_Byte0, "AFStats_HostZoneConfig[2].u16_HostAFZoneStartY" },
    { AFStats_HostZoneConfig_2_u16_HostAFZoneWidth_Byte0, "AFStats_HostZoneConfig[2].u16_HostAFZoneWidth" },
    { AFStats_HostZoneConfig_2_u16_HostAFZoneHeight_Byte0, "AFStats_HostZoneConfig[2].u16_HostAFZoneHeight" },
    { AFStats_HostZoneConfig_2_e_Flag_Enabled_Byte0, "AFStats_HostZoneConfig[2].e_Flag_Enabled" },
    { AFStats_HostZoneConfig_3_u16_HostAFZoneStartX_Byte0, "AFStats_HostZoneConfig[3].u16_HostAFZoneStartX" },
    { AFStats_HostZoneConfig_3_u16_HostAFZoneStartY_Byte0, "AFStats_HostZoneConfig[3].u16_HostAFZoneStartY" },
    { AFStats_HostZoneConfig_3_u16_HostAFZoneWidth_Byte0, "AFStats_HostZoneConfig[3].u16_HostAFZoneWidth" },
    { AFStats_HostZoneConfig_3_u16_HostAFZoneHeight_Byte0, "AFStats_HostZoneConfig[3].u16_HostAFZoneHeight" },
    { AFStats_HostZoneConfig_3_e_Flag_Enabled_Byte0, "AFStats_HostZoneConfig[3].e_Flag_Enabled" },
    { AFStats_HostZoneConfig_4_u16_HostAFZoneStartX_Byte0, "AFStats_HostZoneConfig[4].u16_HostAFZoneStartX" },
    { AFStats_HostZoneConfig_4_u16_HostAFZoneStartY_Byte0, "AFStats_HostZoneConfig[4].u16_HostAFZoneStartY" },
    { AFStats_HostZoneConfig_4_u16_HostAFZoneWidth_Byte0, "AFStats_HostZoneConfig[4].u16_HostAFZoneWidth" },
    { AFStats_HostZoneConfig_4_u16_HostAFZoneHeight_Byte0, "AFStats_HostZoneConfig[4].u16_HostAFZoneHeight" },
    { AFStats_HostZoneConfig_4_e_Flag_Enabled_Byte0, "AFStats_HostZoneConfig[4].e_Flag_Enabled" },
    { AFStats_HostZoneConfig_5_u16_HostAFZoneStartX_Byte0, "AFStats_HostZoneConfig[5].u16_HostAFZoneStartX" },
    { AFStats_HostZoneConfig_5_u16_HostAFZoneStartY_Byte0, "AFStats_HostZoneConfig[5].u16_HostAFZoneStartY" },
    { AFStats_HostZoneConfig_5_u16_HostAFZoneWidth_Byte0, "AFStats_HostZoneConfig[5].u16_HostAFZoneWidth" },
    { AFStats_HostZoneConfig_5_u16_HostAFZoneHeight_Byte0, "AFStats_HostZoneConfig[5].u16_HostAFZoneHeight" },
    { AFStats_HostZoneConfig_5_e_Flag_Enabled_Byte0, "AFStats_HostZoneConfig[5].e_Flag_Enabled" },
    { AFStats_HostZoneConfig_6_u16_HostAFZoneStartX_Byte0, "AFStats_HostZoneConfig[6].u16_HostAFZoneStartX" },
    { AFStats_HostZoneConfig_6_u16_HostAFZoneStartY_Byte0, "AFStats_HostZoneConfig[6].u16_HostAFZoneStartY" },
    { AFStats_HostZoneConfig_6_u16_HostAFZoneWidth_Byte0, "AFStats_HostZoneConfig[6].u16_HostAFZoneWidth" },
    { AFStats_HostZoneConfig_6_u16_HostAFZoneHeight_Byte0, "AFStats_HostZoneConfig[6].u16_HostAFZoneHeight" },
    { AFStats_HostZoneConfig_6_e_Flag_Enabled_Byte0, "AFStats_HostZoneConfig[6].e_Flag_Enabled" },
    { AFStats_HostZoneConfig_7_u16_HostAFZoneStartX_Byte0, "AFStats_HostZoneConfig[7].u16_HostAFZoneStartX" },
    { AFStats_HostZoneConfig_7_u16_HostAFZoneStartY_Byte0, "AFStats_HostZoneConfig[7].u16_HostAFZoneStartY" },
    { AFStats_HostZoneConfig_7_u16_HostAFZoneWidth_Byte0, "AFStats_HostZoneConfig[7].u16_HostAFZoneWidth" },
    { AFStats_HostZoneConfig_7_u16_HostAFZoneHeight_Byte0, "AFStats_HostZoneConfig[7].u16_HostAFZoneHeight" },
    { AFStats_HostZoneConfig_7_e_Flag_Enabled_Byte0, "AFStats_HostZoneConfig[7].e_Flag_Enabled" },
    { AFStats_HostZoneConfig_8_u16_HostAFZoneStartX_Byte0, "AFStats_HostZoneConfig[8].u16_HostAFZoneStartX" },
    { AFStats_HostZoneConfig_8_u16_HostAFZoneStartY_Byte0, "AFStats_HostZoneConfig[8].u16_HostAFZoneStartY" },
    { AFStats_HostZoneConfig_8_u16_HostAFZoneWidth_Byte0, "AFStats_HostZoneConfig[8].u16_HostAFZoneWidth" },
    { AFStats_HostZoneConfig_8_u16_HostAFZoneHeight_Byte0, "AFStats_HostZoneConfig[8].u16_HostAFZoneHeight" },
    { AFStats_HostZoneConfig_8_e_Flag_Enabled_Byte0, "AFStats_HostZoneConfig[8].e_Flag_Enabled" },
    { AFStats_HostZoneConfig_9_u16_HostAFZoneStartX_Byte0, "AFStats_HostZoneConfig[9].u16_HostAFZoneStartX" },
    { AFStats_HostZoneConfig_9_u16_HostAFZoneStartY_Byte0, "AFStats_HostZoneConfig[9].u16_HostAFZoneStartY" },
    { AFStats_HostZoneConfig_9_u16_HostAFZoneWidth_Byte0, "AFStats_HostZoneConfig[9].u16_HostAFZoneWidth" },
    { AFStats_HostZoneConfig_9_u16_HostAFZoneHeight_Byte0, "AFStats_HostZoneConfig[9].u16_HostAFZoneHeight" },
    { AFStats_HostZoneConfig_9_e_Flag_Enabled_Byte0, "AFStats_HostZoneConfig[9].e_Flag_Enabled" },
};

ts_AFpageelement AFStatsWindowConfigAbs[50] =
{
    { AFStats_HostZoneStatus_0_u32_AFZoneStartX_Byte0, "AFStats_HostZoneStatus[0].u32_AFZoneStartX" },
    { AFStats_HostZoneStatus_0_u32_AFZoneStartY_Byte0, "AFStats_HostZoneStatus[0].u32_AFZoneStartY" },
    { AFStats_HostZoneStatus_0_u32_AFZoneEndX_Byte0, "AFStats_HostZoneStatus[0].u32_AFZoneEndX" },
    { AFStats_HostZoneStatus_0_u32_AFZoneEndY_Byte0, "AFStats_HostZoneStatus[0].u32_AFZoneEndY" },
    { AFStats_HostZoneStatus_0_u32_Enabled_Byte0, "AFStats_HostZoneStatus[0].u32_Enabled" },
    { AFStats_HostZoneStatus_1_u32_AFZoneStartX_Byte0, "AFStats_HostZoneStatus[1].u32_AFZoneStartX" },
    { AFStats_HostZoneStatus_1_u32_AFZoneStartY_Byte0, "AFStats_HostZoneStatus[1].u32_AFZoneStartY" },
    { AFStats_HostZoneStatus_1_u32_AFZoneEndX_Byte0, "AFStats_HostZoneStatus[1].u32_AFZoneEndX" },
    { AFStats_HostZoneStatus_1_u32_AFZoneEndY_Byte0, "AFStats_HostZoneStatus[1].u32_AFZoneEndY" },
    { AFStats_HostZoneStatus_1_u32_Enabled_Byte0, "AFStats_HostZoneStatus[1].u32_Enabled" },
    { AFStats_HostZoneStatus_2_u32_AFZoneStartX_Byte0, "AFStats_HostZoneStatus[2].u32_AFZoneStartX" },
    { AFStats_HostZoneStatus_2_u32_AFZoneStartY_Byte0, "AFStats_HostZoneStatus[2].u32_AFZoneStartY" },
    { AFStats_HostZoneStatus_2_u32_AFZoneEndX_Byte0, "AFStats_HostZoneStatus[2].u32_AFZoneEndX" },
    { AFStats_HostZoneStatus_2_u32_AFZoneEndY_Byte0, "AFStats_HostZoneStatus[2].u32_AFZoneEndY" },
    { AFStats_HostZoneStatus_2_u32_Enabled_Byte0, "AFStats_HostZoneStatus[2].u32_Enabled" },
    { AFStats_HostZoneStatus_3_u32_AFZoneStartX_Byte0, "AFStats_HostZoneStatus[3].u32_AFZoneStartX" },
    { AFStats_HostZoneStatus_3_u32_AFZoneStartY_Byte0, "AFStats_HostZoneStatus[3].u32_AFZoneStartY" },
    { AFStats_HostZoneStatus_3_u32_AFZoneEndX_Byte0, "AFStats_HostZoneStatus[3].u32_AFZoneEndX" },
    { AFStats_HostZoneStatus_3_u32_AFZoneEndY_Byte0, "AFStats_HostZoneStatus[3].u32_AFZoneEndY" },
    { AFStats_HostZoneStatus_3_u32_Enabled_Byte0, "AFStats_HostZoneStatus[3].u32_Enabled" },
    { AFStats_HostZoneStatus_4_u32_AFZoneStartX_Byte0, "AFStats_HostZoneStatus[4].u32_AFZoneStartX" },
    { AFStats_HostZoneStatus_4_u32_AFZoneStartY_Byte0, "AFStats_HostZoneStatus[4].u32_AFZoneStartY" },
    { AFStats_HostZoneStatus_4_u32_AFZoneEndX_Byte0, "AFStats_HostZoneStatus[4].u32_AFZoneEndX" },
    { AFStats_HostZoneStatus_4_u32_AFZoneEndY_Byte0, "AFStats_HostZoneStatus[4].u32_AFZoneEndY" },
    { AFStats_HostZoneStatus_4_u32_Enabled_Byte0, "AFStats_HostZoneStatus[4].u32_Enabled" },
    { AFStats_HostZoneStatus_5_u32_AFZoneStartX_Byte0, "AFStats_HostZoneStatus[5].u32_AFZoneStartX" },
    { AFStats_HostZoneStatus_5_u32_AFZoneStartY_Byte0, "AFStats_HostZoneStatus[5].u32_AFZoneStartY" },
    { AFStats_HostZoneStatus_5_u32_AFZoneEndX_Byte0, "AFStats_HostZoneStatus[5].u32_AFZoneEndX" },
    { AFStats_HostZoneStatus_5_u32_AFZoneEndY_Byte0, "AFStats_HostZoneStatus[5].u32_AFZoneEndY" },
    { AFStats_HostZoneStatus_5_u32_Enabled_Byte0, "AFStats_HostZoneStatus[5].u32_Enabled" },
    { AFStats_HostZoneStatus_6_u32_AFZoneStartX_Byte0, "AFStats_HostZoneStatus[6].u32_AFZoneStartX" },
    { AFStats_HostZoneStatus_6_u32_AFZoneStartY_Byte0, "AFStats_HostZoneStatus[6].u32_AFZoneStartY" },
    { AFStats_HostZoneStatus_6_u32_AFZoneEndX_Byte0, "AFStats_HostZoneStatus[6].u32_AFZoneEndX" },
    { AFStats_HostZoneStatus_6_u32_AFZoneEndY_Byte0, "AFStats_HostZoneStatus[6].u32_AFZoneEndY" },
    { AFStats_HostZoneStatus_6_u32_Enabled_Byte0, "AFStats_HostZoneStatus[6].u32_Enabled" },
    { AFStats_HostZoneStatus_7_u32_AFZoneStartX_Byte0, "AFStats_HostZoneStatus[7].u32_AFZoneStartX" },
    { AFStats_HostZoneStatus_7_u32_AFZoneStartY_Byte0, "AFStats_HostZoneStatus[7].u32_AFZoneStartY" },
    { AFStats_HostZoneStatus_7_u32_AFZoneEndX_Byte0, "AFStats_HostZoneStatus[7].u32_AFZoneEndX" },
    { AFStats_HostZoneStatus_7_u32_AFZoneEndY_Byte0, "AFStats_HostZoneStatus[7].u32_AFZoneEndY" },
    { AFStats_HostZoneStatus_7_u32_Enabled_Byte0, "AFStats_HostZoneStatus[7].u32_Enabled" },
    { AFStats_HostZoneStatus_8_u32_AFZoneStartX_Byte0, "AFStats_HostZoneStatus[8].u32_AFZoneStartX" },
    { AFStats_HostZoneStatus_8_u32_AFZoneStartY_Byte0, "AFStats_HostZoneStatus[8].u32_AFZoneStartY" },
    { AFStats_HostZoneStatus_8_u32_AFZoneEndX_Byte0, "AFStats_HostZoneStatus[8].u32_AFZoneEndX" },
    { AFStats_HostZoneStatus_8_u32_AFZoneEndY_Byte0, "AFStats_HostZoneStatus[8].u32_AFZoneEndY" },
    { AFStats_HostZoneStatus_8_u32_Enabled_Byte0, "AFStats_HostZoneStatus[8].u32_Enabled" },
    { AFStats_HostZoneStatus_9_u32_AFZoneStartX_Byte0, "AFStats_HostZoneStatus[9].u32_AFZoneStartX" },
    { AFStats_HostZoneStatus_9_u32_AFZoneStartY_Byte0, "AFStats_HostZoneStatus[9].u32_AFZoneStartY" },
    { AFStats_HostZoneStatus_9_u32_AFZoneEndX_Byte0, "AFStats_HostZoneStatus[9].u32_AFZoneEndX" },
    { AFStats_HostZoneStatus_9_u32_AFZoneEndY_Byte0, "AFStats_HostZoneStatus[9].u32_AFZoneEndY" },
    { AFStats_HostZoneStatus_9_u32_Enabled_Byte0, "AFStats_HostZoneStatus[9].u32_Enabled" }
};

void
ITE_NMF_waitForLensStop(void)
{

    union u_ITE_Event   event;

    // Added for event ISP_FLADRIVER_LENS_STOP check
    do
    {
        event = ITE_WaitEvent(ITE_EVT_ISPCTL_FOCUS_EVENTS);
        ITE_traceISPCTLevent(event);
        if (event.type == ITE_EVT_ISPCTL_ERROR)
        {
            break;
        }
    }while (!(event.ispctlInfo.info_id == ISP_FLADRIVER_LENS_STOP));


    LOS_Log("\nISP_FLADRIVER_LENS_STOP EVENT RECEIVED\n");
   // err = ITE_GetEventCount(Event0_Count_u16_EVENT0_21_FLA_DRIVER_LENS_STOP_Byte0);
   // return (err);
}


void
ITE_NMF_waitForAFStatsReady(void)
{

    union u_ITE_Event   event;
    do
    {
        event = ITE_WaitEvent(ITE_EVT_ISPCTL_FOCUS_EVENTS);
        ITE_traceISPCTLevent(event);
        if (event.type == ITE_EVT_ISPCTL_ERROR)
        {
            break;
        }
    } while (!(event.ispctlInfo.info_id == ISP_AUTOFOCUS_STATS_READY));
}




void
ITE_NMF_waitForLensStopWithAFStats(void)
{

    union u_ITE_Event   event;
    
    t_bool  lens_move_arrived = FALSE, af_stats_arrived  = FALSE;  //initialized to FALSE
    

     do
     {
         event = ITE_WaitEvent(ITE_EVT_ISPCTL_FOCUS_EVENTS);

         ITE_traceISPCTLevent(event);

         if (event.type == ITE_EVT_ISPCTL_ERROR)
         {
             break;
         }
         else if (event.ispctlInfo.info_id == ISP_FLADRIVER_LENS_STOP)
         {
             lens_move_arrived = TRUE;
         }
         else if (event.ispctlInfo.info_id == ISP_AUTOFOCUS_STATS_READY)
         {
             af_stats_arrived = TRUE;
         }
         
     } while ( !(lens_move_arrived & af_stats_arrived) );     
     
}



void
AFStatsZoneConfig(void)
{
    t_uint16        count = 0, array_size;
    ts_PageElement  tab_pe[25];

    //__NO_WARNING__
    for(array_size =0 ; array_size<=25 ; array_size++)
     {
      tab_pe[array_size].pe_addr = 0;
      tab_pe[array_size].pe_data = 0;
     }

    tab_pe[count].pe_addr = AFStats_HostZoneConfig_0_u16_HostAFZoneStartX_Byte0;
    tab_pe[count++].pe_data = 857;
    tab_pe[count].pe_addr = AFStats_HostZoneConfig_0_u16_HostAFZoneStartY_Byte0;
    tab_pe[count++].pe_data = 660;
    tab_pe[count].pe_addr = AFStats_HostZoneConfig_0_u16_HostAFZoneWidth_Byte0;
    tab_pe[count++].pe_data = 340;
    tab_pe[count].pe_addr = AFStats_HostZoneConfig_0_u16_HostAFZoneHeight_Byte0;
    tab_pe[count++].pe_data = 220;
    tab_pe[count].pe_addr = AFStats_HostZoneConfig_0_e_Flag_Enabled_Byte0;
    tab_pe[count++].pe_data = 1;

    tab_pe[count].pe_addr = AFStats_HostZoneConfig_1_u16_HostAFZoneStartX_Byte0;
    tab_pe[count++].pe_data = 514;
    tab_pe[count].pe_addr = AFStats_HostZoneConfig_1_u16_HostAFZoneStartY_Byte0;
    tab_pe[count++].pe_data = 660;
    tab_pe[count].pe_addr = AFStats_HostZoneConfig_1_u16_HostAFZoneWidth_Byte0;
    tab_pe[count++].pe_data = 340;
    tab_pe[count].pe_addr = AFStats_HostZoneConfig_1_u16_HostAFZoneHeight_Byte0;
    tab_pe[count++].pe_data = 220;
    tab_pe[count].pe_addr = AFStats_HostZoneConfig_1_e_Flag_Enabled_Byte0;
    tab_pe[count++].pe_data = 1;

    tab_pe[count].pe_addr = AFStats_HostZoneConfig_2_u16_HostAFZoneStartX_Byte0;
    tab_pe[count++].pe_data = 1200;
    tab_pe[count].pe_addr = AFStats_HostZoneConfig_2_u16_HostAFZoneStartY_Byte0;
    tab_pe[count++].pe_data = 660;
    tab_pe[count].pe_addr = AFStats_HostZoneConfig_2_u16_HostAFZoneWidth_Byte0;
    tab_pe[count++].pe_data = 340;
    tab_pe[count].pe_addr = AFStats_HostZoneConfig_2_u16_HostAFZoneHeight_Byte0;
    tab_pe[count++].pe_data = 220;
    tab_pe[count].pe_addr = AFStats_HostZoneConfig_2_e_Flag_Enabled_Byte0;
    tab_pe[count++].pe_data = 1;

    tab_pe[count].pe_addr = AFStats_HostZoneConfig_3_u16_HostAFZoneStartX_Byte0;
    tab_pe[count++].pe_data = 685;
    tab_pe[count].pe_addr = AFStats_HostZoneConfig_3_u16_HostAFZoneStartY_Byte0;
    tab_pe[count++].pe_data = 439;
    tab_pe[count].pe_addr = AFStats_HostZoneConfig_3_u16_HostAFZoneWidth_Byte0;
    tab_pe[count++].pe_data = 340;
    tab_pe[count].pe_addr = AFStats_HostZoneConfig_3_u16_HostAFZoneHeight_Byte0;
    tab_pe[count++].pe_data = 220;
    tab_pe[count].pe_addr = AFStats_HostZoneConfig_3_e_Flag_Enabled_Byte0;
    tab_pe[count++].pe_data = 1;

    tab_pe[count].pe_addr = AFStats_HostZoneConfig_4_u16_HostAFZoneStartX_Byte0;
    tab_pe[count++].pe_data = 1029;
    tab_pe[count].pe_addr = AFStats_HostZoneConfig_4_u16_HostAFZoneStartY_Byte0;
    tab_pe[count++].pe_data = 439;
    tab_pe[count].pe_addr = AFStats_HostZoneConfig_4_u16_HostAFZoneWidth_Byte0;
    tab_pe[count++].pe_data = 340;
    tab_pe[count].pe_addr = AFStats_HostZoneConfig_4_u16_HostAFZoneHeight_Byte0;
    tab_pe[count++].pe_data = 220;
    tab_pe[count].pe_addr = AFStats_HostZoneConfig_4_e_Flag_Enabled_Byte0;
    tab_pe[count].pe_data = 1;
    ITE_writeListPE(tab_pe, ++count);

    count = 0;
    tab_pe[count].pe_addr = AFStats_HostZoneConfig_5_u16_HostAFZoneStartX_Byte0;
    tab_pe[count++].pe_data = 685;
    tab_pe[count].pe_addr = AFStats_HostZoneConfig_5_u16_HostAFZoneStartY_Byte0;
    tab_pe[count++].pe_data = 881;
    tab_pe[count].pe_addr = AFStats_HostZoneConfig_5_u16_HostAFZoneWidth_Byte0;
    tab_pe[count++].pe_data = 340;
    tab_pe[count].pe_addr = AFStats_HostZoneConfig_5_u16_HostAFZoneHeight_Byte0;
    tab_pe[count++].pe_data = 220;
    tab_pe[count].pe_addr = AFStats_HostZoneConfig_5_e_Flag_Enabled_Byte0;
    tab_pe[count++].pe_data = 1;

    tab_pe[count].pe_addr = AFStats_HostZoneConfig_6_u16_HostAFZoneStartX_Byte0;
    tab_pe[count++].pe_data = 1029;
    tab_pe[count].pe_addr = AFStats_HostZoneConfig_6_u16_HostAFZoneStartY_Byte0;
    tab_pe[count++].pe_data = 881;
    tab_pe[count].pe_addr = AFStats_HostZoneConfig_6_u16_HostAFZoneWidth_Byte0;
    tab_pe[count++].pe_data = 340;
    tab_pe[count].pe_addr = AFStats_HostZoneConfig_6_u16_HostAFZoneHeight_Byte0;
    tab_pe[count++].pe_data = 220;
    tab_pe[count].pe_addr = AFStats_HostZoneConfig_6_e_Flag_Enabled_Byte0;
    tab_pe[count++].pe_data = 1;

    tab_pe[count].pe_addr = AFStats_HostZoneConfig_7_u16_HostAFZoneStartX_Byte0;
    tab_pe[count++].pe_data = 100;
    tab_pe[count].pe_addr = AFStats_HostZoneConfig_7_u16_HostAFZoneStartY_Byte0;
    tab_pe[count++].pe_data = 100;
    tab_pe[count].pe_addr = AFStats_HostZoneConfig_7_u16_HostAFZoneWidth_Byte0;
    tab_pe[count++].pe_data = 340;
    tab_pe[count].pe_addr = AFStats_HostZoneConfig_7_u16_HostAFZoneHeight_Byte0;
    tab_pe[count++].pe_data = 220;
    tab_pe[count].pe_addr = AFStats_HostZoneConfig_7_e_Flag_Enabled_Byte0;
    tab_pe[count++].pe_data = 1;

    tab_pe[count].pe_addr = AFStats_HostZoneConfig_8_u16_HostAFZoneStartX_Byte0;
    tab_pe[count++].pe_data = 1400;
    tab_pe[count].pe_addr = AFStats_HostZoneConfig_8_u16_HostAFZoneStartY_Byte0;
    tab_pe[count++].pe_data = 100;
    tab_pe[count].pe_addr = AFStats_HostZoneConfig_8_u16_HostAFZoneWidth_Byte0;
    tab_pe[count++].pe_data = 340;
    tab_pe[count].pe_addr = AFStats_HostZoneConfig_8_u16_HostAFZoneHeight_Byte0;
    tab_pe[count++].pe_data = 220;
    tab_pe[count].pe_addr = AFStats_HostZoneConfig_8_e_Flag_Enabled_Byte0;
    tab_pe[count++].pe_data = 1;

    tab_pe[count].pe_addr = AFStats_HostZoneConfig_9_u16_HostAFZoneStartX_Byte0;
    tab_pe[count++].pe_data = 1200;
    tab_pe[count].pe_addr = AFStats_HostZoneConfig_9_u16_HostAFZoneStartY_Byte0;
    tab_pe[count++].pe_data = 1200;
    tab_pe[count].pe_addr = AFStats_HostZoneConfig_9_u16_HostAFZoneWidth_Byte0;
    tab_pe[count++].pe_data = 340;
    tab_pe[count].pe_addr = AFStats_HostZoneConfig_9_u16_HostAFZoneHeight_Byte0;
    tab_pe[count++].pe_data = 220;
    tab_pe[count].pe_addr = AFStats_HostZoneConfig_9_e_Flag_Enabled_Byte0;
    tab_pe[count].pe_data = 1;

    ITE_writeListPE(tab_pe, ++count);

    ITE_writePE(AFStats_Controls_e_AFStats_WindowsSystem_Control_Byte0, AFStats_WindowsSystem_e_AF_HOST_SYSTEM);
    ITE_writePE(AFStats_Controls_e_Flag_HostZoneSetupInPercentage_Byte0, Flag_e_TRUE);
    LOS_Sleep(500);
    if (ITE_readPE(AFStats_Controls_e_Coin_ZoneConfigCmd_Byte0) == Coin_e_Tails)
    {
        ITE_writePE(AFStats_Controls_e_Coin_ZoneConfigCmd_Byte0, Coin_e_Heads);
    }
    else
    {
        ITE_writePE(AFStats_Controls_e_Coin_ZoneConfigCmd_Byte0, Coin_e_Tails);
    }


    LOS_Sleep(500);
}


/*********************************************************************************************************************************************
*********************************************************************************************************************************************/
void
FocusControlInitialize(void)
{
    LOS_Log("\nINITIALIZING FOCUS : NVM Initialization , Parameters of AFStats FLADriver and FocusControl \n");

    ITE_writePE(FLADriver_Controls_e_FLADriver_RangeDef_CtrlRange_Byte0, FLADriver_RangeDef_e_NVM_LEVEL_RANGE);
    ITE_writePE(AFStats_Controls_e_Flag_AbsSquareEnabled_Byte0, 1);
    ITE_writePE(AFStats_Controls_u8_CoringValue_Byte0, 1);
    ITE_writePE(AFStats_Controls_e_Flag_AutoRefresh_Byte0, 1);
    LOS_Sleep(500);
}


/*********************************************************************************************************************************************
*********************************************************************************************************************************************/
void
FocusControlHostInitialize(void)
{
    LOS_Log("\nINITIALIZING FOCUS : HOST Initialization , Parameters of AFStats FLADriver and FocusControl \n");

    ITE_writePE(FLADriver_Controls_e_FLADriver_RangeDef_CtrlRange_Byte0, FLADriver_RangeDef_e_HOST_DEFINED_RANGE);
    ITE_writePE(FLADriver_LLLCtrlStatusParam_u16_InfinityFarEndPos_Byte0, 350);
    ITE_writePE(FLADriver_LLLCtrlStatusParam_u16_MacroNearEndPos_Byte0, 500);
    ITE_writePE(FLADriver_LLLCtrlStatusParam_u16_HyperFocalPos_Byte0, 375);
    ITE_writePE(FLADriver_LLLCtrlStatusParam_u16_RestPos_Byte0, 350);
    ITE_writePE(FLADriver_LLLCtrlStatusParam_u16_MacroHorPos_Byte0, 478);
    ITE_writePE(FLADriver_LLLCtrlStatusParam_u16_ManualStepSize_Byte0, 14);
    ITE_writePE(AFStats_Controls_e_Flag_AbsSquareEnabled_Byte0, 1);
    ITE_writePE(AFStats_Controls_u8_CoringValue_Byte0, 1);
    ITE_writePE(AFStats_Controls_e_Flag_AutoRefresh_Byte0, 1);
    LOS_Sleep(500);
}


/*********************************************************************************************************************************************
*********************************************************************************************************************************************/
void
AFStatsZoneSetup(void)
{
    LOS_Log("SETTING AF ZONE CONFIGURATION - BY Default 7 Zone Eye Shaped Setup if Host Configuration is not there \n");
    AFStatsZoneConfig();
    LOS_Log("COMPLETED ---- ZONE Setup has been completed\n");
}


/*********************************************************************************************************************************************
*********************************************************************************************************************************************/
void
AFStats_HostFocusSystem(void)
{
    FocusControlHostInitialize();
    LOS_Sleep(500);
    FLADriverManualFocusTest();
}


void
AFStats_ZoneSetupRandom(void)
{
    t_uint8         Zone = 0;
    volatile float  PER_start_x;
    volatile float  PER_start_y;
    volatile float  PER_end_x;
    volatile float  PER_end_y;

    LOS_Log("HOST ZONE SETUP TEST STARTED\n\n");

    LOS_Log("RANDOM SELECTION OF SHAPE AND SIZE (in percentage)......\n");

    LOS_Log(
    "COMPLETE SCREEN IS DIVIDED INTO TWO PARTS,WHERE VERTICAL AXIS IS FIXED AND HORIZONTAL RANDOMLY VARIES..\n");
    LOS_Log("RANDOM SELECTION FOR 1ST PART....\n\n");

    PER_start_x = ZONE_AREA1_X_BOUNDARY_START_PER + (rand() % ((ZONE_AREA1_X_BOUNDARY_END_PER - ZONE_AREA1_X_BOUNDARY_START_PER) - (ZONE_MAX_WIDTH_PER)));
    PER_start_y = 5;
    PER_end_x = PER_start_x + ZONE_MIN_WIDTH_PER + (rand() % ZONE_MIN_WIDTH_PER);
    PER_end_y = PER_start_y + ZONE_MIN_HEIGHT_PER + (rand() % ZONE_MIN_HEIGHT_PER);

    LOS_Log("RANDOM SELECTION FOR 1ST PART : 1st zone....\n\n");

    ITE_writePE(AFStats_HostZoneConfigPercentageList[Zone * 5].PE_address, *( volatile t_uint32 * ) &PER_start_x);
    ITE_writePE(AFStats_HostZoneConfigPercentageList[Zone * 5 + 1].PE_address, *( volatile t_uint32 * ) &PER_start_y);
    ITE_writePE(AFStats_HostZoneConfigPercentageList[Zone * 5 + 2].PE_address, *( volatile t_uint32 * ) &PER_end_x);
    ITE_writePE(AFStats_HostZoneConfigPercentageList[Zone * 5 + 3].PE_address, *( volatile t_uint32 * ) &PER_end_y);
    ITE_writePE(AFStats_HostZoneConfigPercentageList[Zone * 5 + 4].PE_address, 1);

    LOS_Log("DONE : 1st PART : 1st ZONE....\n");

    for (Zone = 1; Zone < 5; Zone++)
    {
        LOS_Log("RANDOM SELECTION FOR 1ST PART : %dzone....\n\n", Zone);
        PER_start_x = ZONE_AREA1_X_BOUNDARY_START_PER + (rand() % ((ZONE_AREA1_X_BOUNDARY_END_PER - ZONE_AREA1_X_BOUNDARY_START_PER) - (ZONE_MAX_WIDTH_PER)));
        PER_start_y = (PER_end_y + 1);
        PER_end_x = PER_start_x + ZONE_MIN_WIDTH_PER + (rand() % ZONE_MIN_WIDTH_PER);
        PER_end_y = PER_start_y + ZONE_MIN_HEIGHT_PER + (rand() % ZONE_MIN_HEIGHT_PER);

        ITE_writePE(AFStats_HostZoneConfigPercentageList[Zone * 5].PE_address, *( volatile t_uint32 * ) &PER_start_x);
        ITE_writePE(
        AFStats_HostZoneConfigPercentageList[Zone * 5 + 1].PE_address,
        *( volatile t_uint32 * ) &PER_start_y);
        ITE_writePE(AFStats_HostZoneConfigPercentageList[Zone * 5 + 2].PE_address, *( volatile t_uint32 * ) &PER_end_x);
        ITE_writePE(AFStats_HostZoneConfigPercentageList[Zone * 5 + 3].PE_address, *( volatile t_uint32 * ) &PER_end_y);
        ITE_writePE(AFStats_HostZoneConfigPercentageList[Zone * 5 + 4].PE_address, 1);

        LOS_Log("DONE : 1st PART : %d ZONE....\n", Zone);
    }


    LOS_Log("RANDOM SELECTION FOR 2nd PART....\n");

    Zone = 0;
    PER_start_x = ZONE_AREA2_X_BOUNDARY_START_PER + (rand() % ((ZONE_AREA2_X_BOUNDARY_END_PER - ZONE_AREA2_X_BOUNDARY_START_PER) - (ZONE_MAX_WIDTH_PER)));
    PER_start_y = 5;

    PER_end_x = PER_start_x + ZONE_MIN_WIDTH_PER + (rand() % ZONE_MIN_WIDTH_PER);
    PER_end_y = PER_start_y + ZONE_MIN_HEIGHT_PER + (rand() % ZONE_MIN_HEIGHT_PER);

    ITE_writePE(AFStats_HostZoneConfigPercentageList1[Zone * 5].PE_address, *( volatile t_uint32 * ) &PER_start_x);
    ITE_writePE(AFStats_HostZoneConfigPercentageList1[Zone * 5 + 1].PE_address, *( volatile t_uint32 * ) &PER_start_y);
    ITE_writePE(AFStats_HostZoneConfigPercentageList1[Zone * 5 + 2].PE_address, *( volatile t_uint32 * ) &PER_end_x);
    ITE_writePE(AFStats_HostZoneConfigPercentageList1[Zone * 5 + 3].PE_address, *( volatile t_uint32 * ) &PER_end_y);

    ITE_writePE(AFStats_HostZoneConfigPercentageList1[Zone * 5 + 4].PE_address, 1);

    for (Zone = 1; Zone < 5; Zone++)
    {
        LOS_Log("RANDOM SELECTION FOR 2nd PART : %dzone....\n\n", Zone);
        PER_start_x = ZONE_AREA2_X_BOUNDARY_START_PER + (rand() % ((ZONE_AREA2_X_BOUNDARY_END_PER - ZONE_AREA2_X_BOUNDARY_START_PER) - (ZONE_MAX_WIDTH_PER)));
        PER_start_y = (PER_end_y + 1);
        PER_end_x = PER_start_x + ZONE_MIN_WIDTH_PER + (rand() % ZONE_MIN_WIDTH_PER);
        PER_end_y = PER_start_y + ZONE_MIN_HEIGHT_PER + (rand() % ZONE_MIN_HEIGHT_PER);

        ITE_writePE(AFStats_HostZoneConfigPercentageList1[Zone * 5].PE_address, *( volatile t_uint32 * ) &PER_start_x);
        ITE_writePE(
        AFStats_HostZoneConfigPercentageList1[Zone * 5 + 1].PE_address,
        *( volatile t_uint32 * ) &PER_start_y);
        ITE_writePE(
        AFStats_HostZoneConfigPercentageList1[Zone * 5 + 2].PE_address,
        *( volatile t_uint32 * ) &PER_end_x);
        ITE_writePE(
        AFStats_HostZoneConfigPercentageList1[Zone * 5 + 3].PE_address,
        *( volatile t_uint32 * ) &PER_end_y);
        ITE_writePE(AFStats_HostZoneConfigPercentageList1[Zone * 5 + 4].PE_address, 1);

        LOS_Log("DONE : 2nd PART : %d ZONE....\n", Zone);
    }


    LOS_Log("DISABLE ZONE 7 , 8 AND 9 : .\n");

    /*
        ITE_writePE(AFStats_HostZoneConfigPercentageList1[2*5+4].PE_address,0);
        ITE_writePE(AFStats_HostZoneConfigPercentageList1[3*5+4].PE_address,0);
        ITE_writePE(AFStats_HostZoneConfigPercentageList1[4*5+4].PE_address,0);
        */
    LOS_Log("FOR DEBUG USE ONLY : READING BACK THE ZONE PARAMETERS\n\n");
}


/*********************************************************************************************************************************************
*********************************************************************************************************************************************/
void
AFStats_DebugPERead(void)
{
#if _DEBUG_READ_AT_CANSOLE
    LOS_Log("\n\nZoomUpdateDone : %d\n", ITE_readPE(SystemConfig_Status_e_Flag_ZoomUpdateDone_Byte0));
    LOS_Log("\n\nWINDOW: %d\n", ITE_readPE(AFStats_Status_e_AFStats_WindowsSystem_Status_Byte0));
    LOS_Log("\n\nCONTROL:ZoneConfigCmd : %d\n", ITE_readPE(AFStats_Controls_e_Coin_ZoneConfigCmd_Byte0));
    LOS_Log("\n\nSTATUS:ZoneConfigStatus : %d\n", ITE_readPE(AFStats_Status_e_Coin_ZoneConfigStatus_Byte0));
    LOS_Log("\n\nPERCENTAGE : %d\n", ITE_readPE(AFStats_Controls_e_Flag_HostZoneSetupInPercentage_Byte0));
    LOS_Log("\n\nERROR : %d\n", ITE_readPE(AFStats_Status_e_AFStats_Error_Status_Byte0));
#endif //_DEBUG_READ_AT_CANSOLE
}


void
AFStats_DummyReadsSoftware(void)
{
#if _DEBUG_READ_AT_CANSOLE
    t_uint8 Zone = 0;
    for (Zone = 0; Zone < 5; Zone++)
    {
        LOS_Log("READ HOST SET PARAMETER FOR ZONE : %d\n\n\n", Zone);
        LOS_Log(
        "READ: SOFTWARE : PER_start_x : %d\n",
        ITE_readPE(AFStats_HostZoneConfigPercentageList[Zone * 5].PE_address));
        LOS_Log(
        "READ: SOFTWARE : PER_start_y : %d\n",
        ITE_readPE(AFStats_HostZoneConfigPercentageList[Zone * 5 + 1].PE_address));
        LOS_Log(
        "READ: SOFTWARE : PER_end_x : %d\n",
        ITE_readPE(AFStats_HostZoneConfigPercentageList[Zone * 5 + 2].PE_address));
        LOS_Log(
        "READ: SOFTWARE : PER_end_y : %d\n",
        ITE_readPE(AFStats_HostZoneConfigPercentageList[Zone * 5 + 3].PE_address));
        LOS_Log(
        "READ: SOFTWARE : ENABLE : %d\n",
        ITE_readPE(AFStats_HostZoneConfigPercentageList[Zone * 5 + 4].PE_address));
    }


    for (Zone = 0; Zone < 5; Zone++)
    {
        LOS_Log("READ HOST SET PARAMETER FOR ZONE : %d\n\n\n", Zone);
        LOS_Log(
        "READ: SOFTWARE : PER_start_x : %d\n",
        ITE_readPE(AFStats_HostZoneConfigPercentageList1[Zone * 5].PE_address));
        LOS_Log(
        "READ: SOFTWARE : PER_start_y : %d\n",
        ITE_readPE(AFStats_HostZoneConfigPercentageList1[Zone * 5 + 1].PE_address));
        LOS_Log(
        "READ: SOFTWARE : PER_end_x : %d\n",
        ITE_readPE(AFStats_HostZoneConfigPercentageList1[Zone * 5 + 2].PE_address));
        LOS_Log(
        "READ: SOFTWARE : PER_end_y : %d\n",
        ITE_readPE(AFStats_HostZoneConfigPercentageList1[Zone * 5 + 3].PE_address));
        LOS_Log(
        "READ: SOFTWARE : ENABLE : %d\n",
        ITE_readPE(AFStats_HostZoneConfigPercentageList1[Zone * 5 + 4].PE_address));
    }


    for (Zone = 0; Zone < 10; Zone++)
    {
        LOS_Log(
        "\n\nREAD: CONFIG - ABS VALUES :  START_X  :  %d\n\n",
        ITE_readPE(AFStats_HostZoneConfigAbs[Zone * 5].PE_address));
        LOS_Log(
        "\n\nREAD: CONFIG - ABS VALUES :  START_Y  :    %d\n\n",
        ITE_readPE(AFStats_HostZoneConfigAbs[Zone * 5 + 1].PE_address));
        LOS_Log(
        "\n\nREAD: CONFIG - ABS VALUES :  WIDTH_SIZE  :  %d\n\n",
        ITE_readPE(AFStats_HostZoneConfigAbs[Zone * 5 + 2].PE_address));
        LOS_Log(
        "\n\nREAD: CONFIG - ABS VALUES :  HEIGHT_SIZE  :    %d\n\n",
        ITE_readPE(AFStats_HostZoneConfigAbs[Zone * 5 + 3].PE_address));
        LOS_Log(
        "\n\nREAD: CONFIG - ABS VALUES :  ENABLE :  %d\n\n",
        ITE_readPE(AFStats_HostZoneConfigAbs[Zone * 5 + 4].PE_address));
    }


    LOS_Log("\n\n\n\n");
#endif //#if _DEBUG_READ_AT_CANSOLE
}


void
AFStats_BugFindingTest(void)
{
    // start bug finding test .
    LOS_Log("\n\nTEST TO FIND THE BUG : ");
    LOS_Log("\nBY CALLING ZOOM AFSTATS SETUP FLADRIVER STATS AND COMMAND STATS DUMP REQUEST ETC ECT\n\n");
}


void
AFStats_ZoomBugTest(void)
{
    float   DZ = 1.05000;
    t_uint8 count = 0;

    for (count = 0; count < 3; count++)
    {
        AFStats_ZoneAbsDimensions();
        LOS_Log("\n\nZOOM BUG TEST WITH NO FOCUS\n\n");
        LOS_Log("ZoomTest for DZ = %f\n", DZ);
        LOS_Log(
        "\n\nREAD: LENS PRESENT:  %d\n\n",
        ITE_readPE(FLADriver_NVMStoredData_e_Flag_NVMActuatorLensPresent_Byte0));
        ITE_NMF_ZoomTest("LR", DZ);
        LOS_Sleep(500);
        AFStats_ZoneAbsDimensions();
        LOS_Log("\n\nCALLING ZOOM BUG TEST  : DONE \n\n");
        DZ = 2.05000;

        LOS_Log("\n\nZOOM BUG TEST WITH NO FOCUS\n\n");
        LOS_Log("ZoomTest for DZ = %f\n", DZ);
        LOS_Log(
        "\n\nREAD: LENS PRESENT:  %d\n\n",
        ITE_readPE(FLADriver_NVMStoredData_e_Flag_NVMActuatorLensPresent_Byte0));
        ITE_NMF_ZoomTest("LR", DZ);
        LOS_Sleep(500);
        AFStats_ZoneAbsDimensions();
        LOS_Log("\n\nCALLING ZOOM BUG TEST  : DONE \n\n");

        DZ = 3.941500;
        LOS_Log("\n\nZOOM BUG TEST WITH NO FOCUS\n\n");
        LOS_Log("ZoomTest for DZ = %f\n", DZ);
        LOS_Log(
        "\n\nREAD: LENS PRESENT:  %d\n\n",
        ITE_readPE(FLADriver_NVMStoredData_e_Flag_NVMActuatorLensPresent_Byte0));
        ITE_NMF_ZoomTest("LR", DZ);
        LOS_Sleep(500);
        AFStats_ZoneAbsDimensions();
        LOS_Log("\n\nCALLING ZOOM BUG TEST  : DONE \n\n");

        //calling zone configuration test to see if any zoom bug happens due to change in the zone config .
        LOS_Log("\n\n\n\nRANDOM SETUP OF ZONE AFTER ZOOM DONE  : DONE \n\n\n\n");
        AFStats_HostZoneSetup();
        LOS_Sleep(1000);
    }


    // calling stats dump function .
}


void
AFStats_OnlyZoomTest(void)
{
    float   MaxDZ = 20.61;

    LOS_Log("\n\nZOOM TEST WITH NO FOCUS\n\n");
    LOS_Log("\nCALLING ZOOM TEST FOR FULL RANGE  : STARTED \n");

    //calling Zoom full range test .
    ITE_NMF_ZoomTest_FullRange("LR", MaxDZ);
    LOS_Log("\n\nCALLING ZOOM TEST FOR FULL RANGE  : DONE \n\n");
}


void
AFStats_HostZoneWithZoom(void)
{
    float   MaxDZ = 20.61;

    LOS_Log("\n\nHOST ZONE SETUP AND ZOOM OPERATION TEST\n\n");

    // setting up the host zone system for Focus .
    LOS_Log("\nSETTING HOST ZONE SETUP : STARTED\n");
    AFStats_HostZoneSetup();
    LOS_Log("\nHOST ZONE SETUP : DONE \n");

    LOS_Log("\nCALLING ZOOM TEST FOR FULL RANGE  : STARTED \n");

    //calling Zoom full range test .
    ITE_NMF_ZoomTest_FullRange("LR", MaxDZ);
    LOS_Log("\nCALLING ZOOM TEST FOR FULL RANGE  : DONE \n");

    // setting up the host zone system for Focus .
    LOS_Log("\nCALLING AGAIN : SETTING HOST ZONE SETUP : STARTED\n");
    AFStats_HostZoneSetup();
    LOS_Log("\nHOST ZONE SETUP : DONE \n");
}


void
AFStats_HostZoneInPreBoot(void)
{
#if _HOST_ZONE_SETUP_IN_PRE_BOOT
    t_uint8     Flag_var = 0;
    t_uint8     Elements = 0;
    t_uint32    Read;

    // Debug use only : AC
    LOS_Log("SETTING AF ZONE CONFIGURATION .... in terms of the percentage.\n\n");

    // dummy read software before Settings .
    ITE_writePE(AFStats_Controls_e_AFStats_WindowsSystem_Control_Byte0, 0);
    LOS_Sleep(500);
    ITE_writePE(AFStats_Controls_e_Flag_HostZoneSetupInPercentage_Byte0, Flag_e_TRUE);
    LOS_Sleep(500);

    // setting up  the zone parameter in percentage terms .
    AFStats_ZoneSetupRandom();
    LOS_Sleep(500);
    LOS_Sleep(500);

    LOS_Log("\n\nDEBUG READ BEFORE THE ZONE CHANGE TOGGLING ....\n\n");

    AFStats_HostConfigReads();

    LOS_Log("\n\nDEBUG READ DONE\n\n");
#endif //_HOST_ZONE_SETUP_IN_PRE_BOOT
}


void
AFStats_HostZoneSetup(void)
{
    t_uint8 Flag_var = 0;

    // Debug use only : AC
    // LOS_Log("\n\nDUMMY READ SOFTWARE : AFTER COIN TOGGLE n\n");
    // AFStats_DummyReadsSoftware();
    LOS_Log("SETTING AF ZONE CONFIGURATION .... in terms of the percentage.\n\n");

    // dummy read software before Settings .
    ITE_writePE(AFStats_Controls_e_AFStats_WindowsSystem_Control_Byte0, 0);
    LOS_Sleep(500);
    ITE_writePE(AFStats_Controls_e_Flag_HostZoneSetupInPercentage_Byte0, Flag_e_TRUE);
    LOS_Sleep(500);

    // setting up  the zone parameter in percentage terms .
    AFStats_ZoneSetupRandom();
    LOS_Sleep(500);
    LOS_Sleep(500);

    // LOS_Log("\n\nDUMMY READ SOFTWARE : AFTER SETTIGNS n\n");
    // AFStats_DummyReadsSoftware();
    LOS_Log("\n\nDEBUG READ BEFORE THE ZONE CHANGE TOGGLING ....\n\n");

    // AFStats_AFCurrentReadStructure();
    LOS_Log("\n\nDEBUG READ DONE\n\n");

    LOS_Log("SETUP OF THE af ZONE IN PERCENTAGE HAS BEEN COMPLETED ....\n\n");
    LOS_Log("REFLECT THE CHANGES INTO FW , SO THAT THE NEW ZONE CONFIGURATION IS SETUPED..\n\n");
    LOS_Log("TOGGLING THE COIN :CHNAGE WILL BE ACCEPTED BY THE FW..\n");

    //[PM]commented to make code and logs simpler. Currently AF doesn't fail in this check.
#if 0
    Flag_var = ITE_readPE(AFStats_Status_e_Coin_ZoneConfigStatus_Byte0);
    if (Flag_var == Coin_e_Tails)
    {
        Flag_var = Coin_e_Heads;
    }
    else
    {
        Flag_var = Coin_e_Tails;
    }


    LOS_Log("\n\nTOGGLING THE COIN ... \n\n");
    ITE_writePE(AFStats_Controls_e_Coin_ZoneConfigCmd_Byte0, Flag_var);
    while (ITE_readPE(AFStats_Status_e_Coin_ZoneConfigStatus_Byte0) != Flag_var)
    {
        LOS_Log("\nCOIN TOGGLED : INPROCESS \n");

        if (loop_count == 20)
        {
            Flag_var = ITE_readPE(AFStats_Status_e_AFStats_Error_Status_Byte0);
            if (Flag_var != 0)
            {
                LOS_Log("\n\nERROR IF ANY : YES ,ERROR NO :%d \n\n", Flag_var);
            }
            else
            {
                LOS_Log("\n\nERROR : NO \n\n");
            }


            break;
        }


        loop_count++;
    }


    LOS_Log("\n\nCOIN TOGGLED : SUCCESSFULLY \n\n");

    // Debug use only : AC
    // LOS_Log("\n\nDUMMY READ SOFTWARE : AFTER COIN TOGGLE n\n");
    // AFStats_DummyReadsSoftware();
    LOS_Sleep(500);
    LOS_Sleep(500);

    LOS_Log("\n\nDEBUG READ AFTER THE ZONE CHANGE DONE and toggling accepted....\n\n");
    AFStats_WindowConfigReads();
    LOS_Log("\n\nDEBUG READ DONE\n\n");

    Flag_var = ITE_readPE(AFStats_Status_e_AFStats_Error_Status_Byte0);
    if (Flag_var != 0)
    {
        LOS_Log("\n\nERROR IF ANY : YES ,ERROR NO :%d \n\n", Flag_var);
    }
    else
    {
        LOS_Log("\n\nERROR : NO \n\n");
    }


#else
    ITE_writePE(
    AFStats_Controls_e_Coin_ZoneConfigCmd_Byte0,
    !(ITE_readPE(AFStats_Status_e_Coin_ZoneConfigStatus_Byte0)));
    LOS_Sleep(500);
    LOS_Sleep(500);

    if
    (
        ITE_readPE(AFStats_Status_e_Coin_ZoneConfigStatus_Byte0) != ITE_readPE(
            AFStats_Controls_e_Coin_ZoneConfigCmd_Byte0)
    )
    {
        //should not reach here!!
        LOS_Log("*************UNDESIRED STATE**************");
        return;
    }


    //AFStats_WindowConfigReads();
    //LOS_Log("\n\nDEBUG READ DONE\n\n");
#endif

    //AFStats_AFCurrentReadStructure();
    Flag_var = ITE_readPE(FocusControl_Status_u16_Cycles_Byte0);
    LOS_Log("AF Stats ready Cycles :  %d\n\n", Flag_var);
    LOS_Log("AF ZONE SETUP DONE \n\n");
}


void
AFStats_AFCurrentReadStructure(void)
{
    LOS_Log("\n\n************************************************************************\n\n");
    LOS_Log("    CURRENT STATUS OF WINDOW SYSTEM AND ITS PARAMS \n\n");
    LOS_Log("\n\n************************************************************************\n\n");

    AFStats_ZoneAbsDimensions();

    LOS_Log("\n\n************************************************************************\n\n");
    LOS_Log("    CURRENT STATUS DONE \n\n");
    LOS_Log("\n\n************************************************************************\n\n");

    LOS_Log("\n\n\n\n");
}


void
AFStats_AFWindowSystemTest(void)
{
    t_uint8 count = 0;
    t_uint8 Flag_var = 0;

    AFStats_AFCurrentReadStructure();

    while (count < 2)
    {
        LOS_Log("\n\n************************************************************************\n\n");
        LOS_Log("    7 ZONE WINDOW SYSTEM SETUP  \n\n");
        LOS_Log("\n\n************************************************************************\n\n");

        LOS_Log("SETTING AF ZONE CONFIGURATION .... in terms of the percentage.\n\n");

        // dummy read software before Settings .
        ITE_writePE(AFStats_Controls_e_AFStats_WindowsSystem_Control_Byte0, 1);
        LOS_Sleep(500);
        ITE_writePE(AFStats_Controls_e_Flag_HostZoneSetupInPercentage_Byte0, Flag_e_FALSE);
        LOS_Sleep(500);
        LOS_Sleep(500);

        LOS_Log("SETUP OF AF 7 ZONE EYE SHAPED WINDOW SYSTEM COMPLETED ....\n\n");
        LOS_Log("REFLECT THE CHANGES INTO FW , SO THAT THE NEW ZONE CONFIGURATION IS SETUPED..\n\n");
        LOS_Log("TOGGLING THE COIN :CHNAGE WILL BE ACCEPTED BY THE FW..\n");
        Flag_var = ITE_readPE(AFStats_Status_e_Coin_ZoneConfigStatus_Byte0);
        if (Flag_var == Coin_e_Tails)
        {
            Flag_var = Coin_e_Heads;
        }
        else
        {
            Flag_var = Coin_e_Tails;
        }


        LOS_Log("\n\nTOGGLING THE COIN ... \n\n");
#if 0
        loop_count = 0;
        ITE_writePE(AFStats_Controls_e_Coin_ZoneConfigCmd_Byte0, Flag_var);
        while (ITE_readPE(AFStats_Status_e_Coin_ZoneConfigStatus_Byte0) != Flag_var)
        {
            LOS_Log("\nCOIN TOGGLED : INPROCESS \n");
            if (loop_count == 20)
            {
                Flag_var = ITE_readPE(AFStats_Status_e_AFStats_Error_Status_Byte0);
                if (Flag_var != 0)
                {
                    LOS_Log("\n\nERROR IF ANY : YES ,ERROR NO :%d \n\n", Flag_var);
                }
                else
                {
                    LOS_Log("\n\nERROR : NO \n\n");
                }


                break;
            }


            loop_count++;
        }


        LOS_Log("\n\nCOIN TOGGLED : SUCCESSFULLY \n\n");

        LOS_Sleep(500);
        LOS_Sleep(500);

        LOS_Log("\n\n READ AFTER THE 7 ZONE SETUP HAS BEEN TAKEN BY THE FW \n\n");
        AFStats_WindowConfigReads();

        Flag_var = ITE_readPE(AFStats_Status_e_AFStats_Error_Status_Byte0);
        if (Flag_var != 0)
        {
            LOS_Log("\n\nERROR IF ANY : YES ,ERROR NO :%d \n\n", Flag_var);
        }
        else
        {
            LOS_Log("\n\nERROR : NO \n\n");
        }


#else
        ITE_writePE(
        AFStats_Controls_e_Coin_ZoneConfigCmd_Byte0,
        !(ITE_readPE(AFStats_Status_e_Coin_ZoneConfigStatus_Byte0)));
        LOS_Sleep(500);
        LOS_Sleep(500);

        if
        (
            ITE_readPE(AFStats_Status_e_Coin_ZoneConfigStatus_Byte0) != ITE_readPE(
                AFStats_Controls_e_Coin_ZoneConfigCmd_Byte0)
        )
        {
            //should not reach here!!
            LOS_Log("*************UNDESIRED STATE**************");
            return;
        }


        LOS_Log("\n\n READ AFTER THE 7 ZONE SETUP HAS BEEN TAKEN BY THE FW \n\n");
        AFStats_WindowConfigReads();
#endif
        LOS_Log("\n\n************************************************************************\n\n");
        LOS_Log("    HOST ZONE WINDOW SYSTEM SETUP  \n\n");
        LOS_Log("\n\n************************************************************************\n\n");

        AFStats_HostZoneSetup();

        LOS_Log("\n\n************************************************************************\n\n");
        LOS_Log("    HOST ZONE SETUP DONE  \n\n");
        LOS_Log("\n\n************************************************************************\n\n\n\n");
        LOS_Log("\n\n READ AFTER THE HOST ZONE SETUP HAS BEEN TAKEN BY THE FW \n\n");

        AFStats_WindowConfigReads();

        // Loop count for the zone setup alternatly .
        count++;
    }
}


void
AFStats_WindowConfigReads(void)
{
    t_uint8     Elements = 0;
    t_uint32    Read = 0;
    float       f_FOVX = 0.0,
                f_FOVY = 0.0;

    LOS_Log("\n-***********************************-\n\n-*************************************-\n");
    LOS_Log("\nWOIWidth      :    %d", ITE_readPE(AFStats_Status_u16_WOIWidth_Byte0));
    LOS_Log("\nWOIHeight     :    %d", ITE_readPE(AFStats_Status_u16_WOIHeight_Byte0));
    LOS_Log("\n\n");
    LOS_Log("\n--------------------------------------\n");
    LOS_Log("\nAFZonesWidth      :    %d", ITE_readPE(AFStats_Status_u16_AFZonesWidth_Byte0));
    LOS_Log("\nAFZonesHeight     :    %d", ITE_readPE(AFStats_Status_u16_AFZonesHeight_Byte0));

    f_FOVX = ITE_readPE(Zoom_Status_f_FOVX_Byte0);
    f_FOVY = ITE_readPE(Zoom_Status_f_FOVY_Byte0);
    LOS_Log("\nFOV_X      :    %f", f_FOVX);
    LOS_Log("\nFOV_Y      :    %f", f_FOVY);
    LOS_Log("\n\n");

    LOS_Log("\n--------------------------------------\n");
    LOS_Log("\nERROR AF ZONE SETUP :    %d", ITE_readPE(AFStats_Status_e_AFStats_Error_Status_Byte0));
    LOS_Log("\nWINDOW AF IN USE     :    %d", ITE_readPE(AFStats_Status_e_AFStats_WindowsSystem_Status_Byte0));
    LOS_Log("\nZONE ENABLED     :    %d", ITE_readPE(AFStats_Status_u8_ActiveZonesCounter_Byte0));
    LOS_Log("\nZOOM UPDATE FLAG     :    %d", ITE_readPE(SystemConfig_Status_e_Flag_ZoomUpdateDone_Byte0));
    LOS_Log("\nZONE CONFIG STATUS     :    %d", ITE_readPE(AFStats_Status_e_Coin_ZoneConfigStatus_Byte0));
    LOS_Log("\nZONE CONFIG CONTROL     :    %d", ITE_readPE(AFStats_Controls_e_Coin_ZoneConfigCmd_Byte0));
    LOS_Log("\nPERCENTAGE : %d", ITE_readPE(AFStats_Controls_e_Flag_HostZoneSetupInPercentage_Byte0));
    LOS_Log("\n\n");

    LOS_Log("\n\n************************************************************************\n\n");
    LOS_Log("    CURRENT STATUS OF WINDOW SYSTEM  \n\n");
    LOS_Log("\n\n************************************************************************\n\n");

    while (Elements < 50)
    {
        Read = ITE_readPE(AFStatsWindowConfigAbs[Elements].PE_address);
        LOS_Log("\nPERCENTAGE ELEMENT : %d\n", Elements);
        LOS_Log("\nWIN : PAGE : %s : VALUE : %d\n", AFStatsWindowConfigAbs[Elements].PE_name, Read);
        LOS_Log("\n\n");

        Elements = Elements + 1;
    }


    LOS_Log("\n\n************************************************************************\n\n");
    LOS_Log("    STATUS READ DONE \n\n");
    LOS_Log("\n\n************************************************************************\n\n");

    LOS_Log("\n-***********************************-\n\n-*************************************-\n");
    LOS_Log("\nWOIWidth   :   %d", ITE_readPE(AFStats_Status_u16_WOIWidth_Byte0));
    LOS_Log("\nWOIHeight  :   %d", ITE_readPE(AFStats_Status_u16_WOIHeight_Byte0));
    LOS_Log("\n\n");
    LOS_Log("\n--------------------------------------\n");
    LOS_Log("\nAFZonesWidth   :   %d", ITE_readPE(AFStats_Status_u16_AFZonesWidth_Byte0));
    LOS_Log("\nAFZonesHeight  :   %d", ITE_readPE(AFStats_Status_u16_AFZonesHeight_Byte0));

    f_FOVX = ITE_readPE(Zoom_Status_f_FOVX_Byte0);
    f_FOVY = ITE_readPE(Zoom_Status_f_FOVY_Byte0);
    LOS_Log("\nFOV_X   :    %f", f_FOVX);
    LOS_Log("\nFOV_Y   :    %f", f_FOVX);
    LOS_Log("\n\n");

    LOS_Log("\n--------------------------------------\n");
    LOS_Log("\nERROR AF ZONE SETUP : %d", ITE_readPE(AFStats_Status_e_AFStats_Error_Status_Byte0));
    LOS_Log("\nWINDOW AF IN USE  :  %d", ITE_readPE(AFStats_Status_e_AFStats_WindowsSystem_Status_Byte0));
    LOS_Log("\nZONE ENABLED  :  %d", ITE_readPE(AFStats_Status_u8_ActiveZonesCounter_Byte0));
    LOS_Log("\nZOOM UPDATE FLAG  :  %d", ITE_readPE(SystemConfig_Status_e_Flag_ZoomUpdateDone_Byte0));
    LOS_Log("\nZONE CONFIG STATUS   :    %d", ITE_readPE(AFStats_Status_e_Coin_ZoneConfigStatus_Byte0));
    LOS_Log("\nZONE CONFIG CONTROL    : %d", ITE_readPE(AFStats_Controls_e_Coin_ZoneConfigCmd_Byte0));
    LOS_Log("\nPERCENTAGE : %d", ITE_readPE(AFStats_Controls_e_Flag_HostZoneSetupInPercentage_Byte0));
    LOS_Log("\n\n");
}


void
AFStats_HostConfigReads(void)
{
    t_uint8     Elements = 0;
    t_uint32    Read = 0;

    while (Elements < 25)
    {
        Read = ITE_readPE(AFStats_HostZoneConfigPercentageList[Elements].PE_address);
        LOS_Log("\n*************************************\n");
        LOS_Log("\n\n ELEMENT NO : %d\n\n", Elements);
        LOS_Log("\n*************************************\n");
        LOS_Log("\nPER : PAGE : %s : VALUE : %d\n", AFStats_HostZoneConfigPercentageList[Elements].PE_name, Read);
        LOS_Log("\n");

        Read = ITE_readPE(AFStats_HostZoneConfigAbs[Elements].PE_address);
        LOS_Log("\nABS : PAGE : %s : VALUE : %d\n", AFStats_HostZoneConfigAbs[Elements].PE_name, Read);
        LOS_Log("\n\n");

        Elements = Elements + 1;
    }


    Elements = 0;

    while (Elements < 25)
    {
        Read = ITE_readPE(AFStats_HostZoneConfigPercentageList1[Elements].PE_address);
        LOS_Log("\n*************************************\n");
        LOS_Log("\n\n ELEMENT NO : %d\n\n", (Elements + 25));
        LOS_Log("\n*************************************\n");
        LOS_Log("\nPER : PAGE : %s : VALUE : %d\n", AFStats_HostZoneConfigPercentageList1[Elements].PE_name, Read);
        LOS_Log("\n");

        Read = ITE_readPE(AFStats_HostZoneConfigAbs[Elements + 25].PE_address);
        LOS_Log("\nABS : PAGE : %s : VALUE : %d\n", AFStats_HostZoneConfigAbs[Elements].PE_name, Read);
        LOS_Log("\n\n");

        Elements = Elements + 1;
    }
}


/**********************************************************************************************************************************************/
void
AFStats_ZoneAbsDimensions(void)
{
#if _DEBUG_READ_AT_CANSOLE
    t_uint16    Elements;
    t_uint32    Read;
    float       f_FOVX,
                f_FOVY;

    Elements = 0;

    LOS_Log("\n-***********************************-\n\n-*************************************-\n");
    LOS_Log("\nWOIWidth      :    %d", ITE_readPE(AFStats_Status_u16_WOIWidth_Byte0));
    LOS_Log("\nWOIHeight     :    %d", ITE_readPE(AFStats_Status_u16_WOIHeight_Byte0));
    LOS_Log("\n\n");
    LOS_Log("\n--------------------------------------\n");
    LOS_Log("\nAFZonesWidth      :    %d", ITE_readPE(AFStats_Status_u16_AFZonesWidth_Byte0));
    LOS_Log("\nAFZonesHeight     :    %d", ITE_readPE(AFStats_Status_u16_AFZonesHeight_Byte0));

    f_FOVX = ITE_readPE(Zoom_Status_f_FOVX_Byte0);
    f_FOVY = ITE_readPE(Zoom_Status_f_FOVY_Byte0);
    LOS_Log("\nFOV_X      :    %f", f_FOVX);
    LOS_Log("\nFOV_Y      :    %f", f_FOVX);
    LOS_Log("\n\n");

    LOS_Log("\n--------------------------------------\n");
    LOS_Log("\nERROR AF ZONE SETUP :    %d", ITE_readPE(AFStats_Status_e_AFStats_Error_Status_Byte0));
    LOS_Log("\nWINDOW AF IN USE     :    %d", ITE_readPE(AFStats_Status_e_AFStats_WindowsSystem_Status_Byte0));
    LOS_Log("\nZONE ENABLED     :    %d", ITE_readPE(AFStats_Status_u8_ActiveZonesCounter_Byte0));
    LOS_Log("\nZOOM UPDATE FLAG     :    %d", ITE_readPE(SystemConfig_Status_e_Flag_ZoomUpdateDone_Byte0));
    LOS_Log("\nZONE CONFIG STATUS     :    %d", ITE_readPE(AFStats_Status_e_Coin_ZoneConfigStatus_Byte0));
    LOS_Log("\nZONE CONFIG CONTROL     :    %d", ITE_readPE(AFStats_Controls_e_Coin_ZoneConfigCmd_Byte0));
    LOS_Log("\nPERCENTAGE : %d", ITE_readPE(AFStats_Controls_e_Flag_HostZoneSetupInPercentage_Byte0));
    LOS_Log("\n\n");

    /*
        while(Elements < 5)
        {
        Read = ITE_readPE(AFStatsZoneConfigAbs[Elements].PE_address);
        LOS_Log("ELEMENT : %d  Page element : %s    Memory Read : %d\n",Elements,AFStatsZoneConfigAbs[Elements].PE_name,Read);
        LOS_Log("\n\n");
        Elements ++;
        }

        */

    // Reading Host Zone Configuration .
    AFStats_HostConfigReads();

    LOS_Log("\n-***********************************-\n\n-*************************************-\n");
    LOS_Log("\nWOIWidth   :   %d", ITE_readPE(AFStats_Status_u16_WOIWidth_Byte0));
    LOS_Log("\nWOIHeight  :   %d", ITE_readPE(AFStats_Status_u16_WOIHeight_Byte0));
    LOS_Log("\n\n");
    LOS_Log("\n--------------------------------------\n");
    LOS_Log("\nAFZonesWidth   :   %d", ITE_readPE(AFStats_Status_u16_AFZonesWidth_Byte0));
    LOS_Log("\nAFZonesHeight  :   %d", ITE_readPE(AFStats_Status_u16_AFZonesHeight_Byte0));

    f_FOVX = ITE_readPE(Zoom_Status_f_FOVX_Byte0);
    f_FOVY = ITE_readPE(Zoom_Status_f_FOVY_Byte0);
    LOS_Log("\nFOV_X   :    %f", f_FOVX);
    LOS_Log("\nFOV_Y   :    %f", f_FOVX);
    LOS_Log("\n\n");

    LOS_Log("\n--------------------------------------\n");
    LOS_Log("\nERROR AF ZONE SETUP : %d", ITE_readPE(AFStats_Status_e_AFStats_Error_Status_Byte0));
    LOS_Log("\nWINDOW AF IN USE  :  %d", ITE_readPE(AFStats_Status_e_AFStats_WindowsSystem_Status_Byte0));
    LOS_Log("\nZONE ENABLED  :  %d", ITE_readPE(AFStats_Status_u8_ActiveZonesCounter_Byte0));
    LOS_Log("\nZOOM UPDATE FLAG  :  %d", ITE_readPE(SystemConfig_Status_e_Flag_ZoomUpdateDone_Byte0));
    LOS_Log("\nZONE CONFIG STATUS   :    %d", ITE_readPE(AFStats_Status_e_Coin_ZoneConfigStatus_Byte0));
    LOS_Log("\nZONE CONFIG CONTROL    : %d", ITE_readPE(AFStats_Controls_e_Coin_ZoneConfigCmd_Byte0));
    LOS_Log("\nPERCENTAGE : %d", ITE_readPE(AFStats_Controls_e_Flag_HostZoneSetupInPercentage_Byte0));
    LOS_Log("\n\n");
#endif // _DEBUG_READ_AT_CANSOLE
}


/*
Bug : 107051 : Test:
AFStats_Status_e_Coin_AFStatsExportStatus changes even when
AFStats_Controls_e_Coin_AFStatsExportCmd has not been toggled by the host .
*/

/***************************************************************************************************/
void
FocusControl_BugTest_107051(
t_uint32    pMemoryAddress,
t_uint32    pHostMemoryAddress)
{
    t_uint8     Precoin = 0,
                Aftcoin = 0,
                Flag = 0,
                coin = 0;
    t_uint32    u32_Valid_flag = 3;

    if (pMemoryAddress == 0)
    {
        LOS_Log("ADDRESS NOT VALID , STOP EXECUTION ..\n");
        return;
    }


    LOS_Log("MOVE TO INFINITY COMMAND PROCESS\n\n");

    // issue the command to move to infinity 1st , so that the move and stats gathering can be started from infinity.
    FLADriverMoveToInfinity();
    LOS_Log("INFINITY POSITION IS REACHED.. \n\n");
    LOS_Log("NOW:-     1   : MOVE TO MACRO AND STATS GATHERING STEPS \n\n");
    LOS_Log("NOW:-     2   : MOVE TO MACRO cmd \n\n");
    LOS_Log("NOW:-     3   : e_Flag_StatsWithLensMove_Control :");
    LOS_Log("        TRUE : stats gathering and Lens move control\n\n");
    LOS_Log("NOW:-     4   : Set Export Memory Address and wash it\n\n");
    LOS_Log("NOW:-    5   : Check for the CURRENT Status VAR:[EXPOSED TO HOST ONLY] ");
    LOS_Log("        e_Coin_AFStatsExportStatus : \n\n");
    LOS_Log("NOW:-     6   : Toggling the command coin \n\n");
    LOS_Log("NOW:-    7   : Check for the Status of e_Flag_FW_LensWithStatsStatus");
    LOS_Log("        after COIN Toggle :\n\n");
    LOS_Log("NOW:-    8   : VALUE OF e_Flag_FW_LensWithStatsStatus MUST BE");
    LOS_Log("        Flag_e_TRUE befor AFStats NOTIFICATION.");
    LOS_Log("NOW:-    9   : if VALUE OF e_Flag_FW_LensWithStatsStatus is NOT");
    LOS_Log("        Flag_e_TRUE befor AFStats NOTIFICATION.:");
    LOS_Log("     TEST FAILS CHECK\n\n");
    LOS_Log("NOW:-     10  : Waiting for AFStats Notification  \n\n");
    LOS_Log("NOW:-    11  : Check for the Status of e_Coin_AFStatsExportStatus");
    LOS_Log("        after Getting NOTIFICATION : \n\n");
    LOS_Log("NOW:-     12  : if Flag valid - check for FLAD stop Notification\n\n");

    LOS_Log("\n\nPROCESS STARTED:\n\n");
    LOS_Log("\n\n\n");
    LOS_Log("CLEAR NOTIFICATION :FLADRIVER STOP : IF ANY .\n");
    ITE_CleanEvent(ITE_EVT_ISPCTL_FOCUS_EVENTS);
    LOS_Sleep(500);

    LOS_Log("\n\nISSUE COMMAND MOVE TO MACRO..\n\n");

    // once at infinity now move to Macro along with the stats gatheing command process is started.
    ITE_writePE(
    FocusControl_Controls_e_FocusControl_LensCommand_Control_Byte0,
    FocusControl_LensCommand_e_LA_CMD_GOTO_MACRO_NEAR_END);
    LOS_Log("\n\nCOMMAND ISSUED..\n\n");
    LOS_Log("\n\nMAKE FLAG: e_Flag_StatsWithLensMove_Control: TO TRUE..\n\n");
    ITE_writePE(FocusControl_Controls_e_Flag_StatsWithLensMove_Control_Byte0, Flag_e_TRUE);
    LOS_Log("\n\nMAKE FLAG: e_Flag_StatsWithLensMove_Control: DONE..\n\n");
    LOS_Log("\n\nSET ADDRESS ..\n\n");

    LOS_Log("SETTING HOST ADDRESS :%x \n", pMemoryAddress);
    ITE_writePE(AFStats_Controls_pu32_HostAssignedAddr_Byte0, pMemoryAddress);
    LOS_Log("WAITING SOME TIME ... \n\n");
    LOS_Sleep(500);
    LOS_Log("\n\nADDRESS SETTING: DONE..\n\n");

    //LOS_Log( "WRITING HOST ADDRESS WITH 0xFF TO 400 bytes LENGTH, TO ERASE DUMMY DATA... \n");
    //for (i=0;i<400;i++) (*((volatile unsigned char *)(pHostMemoryAddress+i))) = 0xff;
    //LOS_Log( "WRITING TO MEMORY COMPLETED ...\n\n");
    // clear if any notification is there .
    //LOS_Log( "\nCLEARING NOTIFICATION IF ANY : AFSTATS\n");
    //ITE_CleanEvent(ITE_EVT_ISPCTL_STATS_EVENTS);
    LOS_Log("\n\n\nNOW PRE TOGGLING COMMAND COIN PROCESS \n\n\n");

    LOS_Log("NOW:-    CHECK for the CURRENT Status VAR:");
    LOS_Log("[EXPOSED TO HOST ONLY] e_Coin_AFStatsExportStatus : \n\n");
    Precoin = ITE_readPE(AFStats_Status_e_Coin_AFStatsExportStatus_Byte0);
    if (Precoin == Coin_e_Heads)
    {
        LOS_Log("CURRENT VALUE OF \"e_Coin_AFStatsExportStatus");
        LOS_Log(": Coin_e_Heads : BEFORE COIN TOGGLE \n\n");
    }
    else
    {
        LOS_Log("CURRENT VALUE OF e_Coin_AFStatsExportStatus");
        LOS_Log(": Coin_e_Tails  : BEFORE COIN TOGGLE \n\n");
    }


    LOS_Log("NOW:-    CHECK for the CURRENT Status VAR: [EXPOSED TO FW ONLY]");
    LOS_Log(": e_Flag_FW_LensWithStatsStatus : \n\n");

    Flag = ITE_readPE(AFStats_Status_e_Flag_FW_LensWithStatsStatus_Byte0);

    if (Flag == Flag_e_FALSE)
    {
        LOS_Log("CURRENT VALUE OF e_Flag_FW_LensWithStatsStatus");
        LOS_Log(": Flag_e_FALSE : BEFORE COIN TOGGLE \n\n");
    }
    else
    {
        LOS_Log("CURRENT VALUE OF \"e_Flag_FW_LensWithStatsStatus");
        LOS_Log(": Flag_e_TRUE  : BEFORE COIN TOGGLE  \n\n");
        LOS_Log("\nTEST FAILS SOMEWHERE : PLS CHECK\n");
        return;
    }


    LOS_Log("Check for the Status of e_Flag_FW_LensWithStatsStatus");
    LOS_Log("after COIN Toggle : \n\n");
    LOS_Log("VALUE OF e_Flag_FW_LensWithStatsStatus MUST BE ");
    LOS_Log("Flag_e_TRUE befor AFStats NOTIFICATION. \n\n");
    LOS_Log("if VALUE OF e_Flag_FW_LensWithStatsStatus is NOT");
    LOS_Log("Flag_e_TRUE befor AFStats NOTIFICATION.: TEST FAILS \n\n");

    // Toggle the command coin so that the actual move withrespect to command is executed.
    LOS_Log("\nCOIN FLIP PROCESS STARTED ...FOR COMMAND MOVEMENT\n");
    ITE_RefreshEventCount(Event0_Count_u16_EVENT0_20_AF_STATS_READY_Byte0);
    ITE_RefreshEventCount(Event0_Count_u16_EVENT0_21_FLA_DRIVER_LENS_STOP_Byte0);
    coin = ITE_readPE(FocusControl_Status_e_Coin_Status_Byte0);
    if (coin == Coin_e_Tails)
    {
        coin = Coin_e_Heads;
    }
    else
    {
        coin = Coin_e_Tails;
    }


    LOS_Log("\nTOGGLING THE COIN  TO ... : %d\n", coin);

    // toggle the coin but do not wait for the status of the toggled coin , this has to be done after the notification ,
    // when lens stop , so that the command has been successfully absorbed .
    ITE_writePE(FocusControl_Controls_e_Coin_Control_Byte0, coin);

    //wait for the AFStats Notification.
    while ((ITE_readPE(FocusControl_Status_e_Coin_Status_Byte0)) != coin)
    {
        if (ITE_readPE(AFStats_Status_e_Flag_FW_LensWithStatsStatus_Byte0) == Flag_e_TRUE)
        {
            LOS_Log("\nFW HANDLING OF FLAG WORKING : BREAK HERE  ...: DURING PROCESS\n");
            break;
        }
    }


    LOS_Log("\nCOIN TOGGLED :SUCESSFULLY :\n");
    if ((ITE_readPE(AFStats_Status_e_Flag_FW_LensWithStatsStatus_Byte0)) == Flag_e_TRUE)
    {
        LOS_Log("\nFW HANDLING OF FLAG WORKING ++++++ ...: DURING PROCESS\n");
    }


    LOS_Log("\nWAITING FOR AFSTATS NOTIFICATION...\n");

    ITE_NMF_waitForAFStatsReady();
    ITE_GetEventCount(Event0_Count_u16_EVENT0_20_AF_STATS_READY_Byte0);
    LOS_Log("\nRECIEVED NOTIFICATION : STATS TO MEMORY EXPORT NOTIFICATION..\n");

    if (ITE_readPE(AFStats_Status_e_Flag_FW_LensWithStatsStatus_Byte0) != Flag_e_TRUE)
    {
        LOS_Log("\nFW HANDLING DONE : DURING PROCESS\n");
    }
    else
    {
        LOS_Log("\nCHECK THE PROCESS : e_Flag_FW_LensWithStatsStatus_Byte0\n");
        LOS_Log("\nMUST BE Flag_e_FALSE\n");
    }


    Aftcoin = ITE_readPE(AFStats_Status_e_Coin_AFStatsExportStatus_Byte0);
    if (Aftcoin != Precoin)
    {
        LOS_Log("\nCURRENT VALUE OF e_Coin_AFStatsExportStatus :\n");
        LOS_Log("\nFW HANDLING FAILED: COIN TOGGLED EVEN IF HOST DOES NOT WANT.\n\n");
        LOS_Log("\nTEST FAILS SOMEWHERE : PLS CHECK\n");
        return;
    }
    else
    {
        LOS_Log("\nNOW:-    6a:CURRENT VALUE OF e_Coin_AFStatsExportStatus :\n");
        LOS_Log("\nFW HANDLING PASSED : SUCCESSFULLY .\n\n");
    }


    if ((ITE_readPE(AFStats_Status_e_Flag_FW_LensWithStatsStatus_Byte0) == Flag_e_FALSE) && (Aftcoin == Precoin))
    {
        LOS_Log("\n\n\n\nBUG [107051]   FIXED         : SUCCESSFULLY .\n\n\n\n");
    }
    else
    {
        LOS_Log("\n\n\n\nBUG [107051]  NOT  FIXED     : FAILED  .\n\n\n\n");
        return;
    }


    u32_Valid_flag = (*(( volatile unsigned long int * ) (pHostMemoryAddress + 4 * 100)));

    if (u32_Valid_flag == 1)
    {
        LOS_Log("\n\nGOT THE VALUE of Valid flag : VALID   :%d\n\n", u32_Valid_flag);
    }
    else if (u32_Valid_flag == 0)
    {
        LOS_Log("\n\nGOT THE VALUE of Valid flag : INVALID   :%d\n\n", u32_Valid_flag);
    }
    else
    {
        LOS_Log("\n\nGOT THE VALUE : WRONG VALUE : %d\n\n", u32_Valid_flag);
    }


    LOS_Log("\n\nWAITING FOR THE NOTIFICATION : LENS STOP ............. \n");
    ITE_NMF_waitForLensStop();
    ITE_GetEventCount(Event0_Count_u16_EVENT0_21_FLA_DRIVER_LENS_STOP_Byte0);
    LOS_Log("\nRECIEVED NOTIFICATION : LENS STOP.....\n");

    LOS_Log("\n\nPROCESS FINISHED\n\n");
}


void
FocusControl_SpecifyPrintFormat(
t_uint32    pMemoryAddress,
t_uint32    pHostMemoryAddress)
{
    t_uint8     u8_byteinline = GLOBAL_DEFAULT_NUMBER_BYTE_IN_SINGLE_LINE_PRINT;
    t_uint16    u16_totalbyte = GLOBAL_DEFAULT_TOTAL_BYTE_PRINT;

    LOS_Log("************************************************************\n");
    LOS_Log("   PRINT INFORMATION      \n");
    LOS_Log("************************************************************\n");

    LOS_Log("BYTES PER LINE                  :%d \n", u8_byteinline);
    LOS_Log("TOTAL NO OF BYTES TO PRINT                :%d \n", u16_totalbyte);
    LOS_Log("MEMORY ADDRESS : pMemoryAddress                   :%x \n", pMemoryAddress);
    LOS_Log("MEMORY ADDRESS : pHostMemoryAddress               :%x \n", pHostMemoryAddress);

    LOS_Log("************************************************************\n");
    LOS_Log("   PRINT INFORMATION      \n");
    LOS_Log("************************************************************\n");

    FocusControl_PrintFormat(u8_byteinline, pHostMemoryAddress, u16_totalbyte);
}


void
FocusControl_PrintFormat(
t_uint8     u8_byteinline,
t_uint32    pMemoryAddress,
t_uint16    u16_totalbyte)
{
#if _PRINT_MEM_BYTES
    t_uint16    i = 0;
    t_uint8     value = 0;

    LOS_Log("************************************************************\n");
    LOS_Log("   BYTE PRINT PROCESS FROM MEMORY      \n");
    LOS_Log("************************************************************\n");

    LOS_Log("\n\n\n\n");

    LOS_Log("BYTES PER LINE                  :%d \n", u8_byteinline);
    LOS_Log("TOTAL NO OF BYTES TO PRINT                :%d \n", u16_totalbyte);
    LOS_Log("MEMORY ADDRESS FROM WHERE THE PRINT NEEDED        :%x \n", pMemoryAddress);
    LOS_Log("\n\n\n\n");

    while (i < u16_totalbyte)
    {
        value = (*(( volatile unsigned char * ) (pMemoryAddress + i)));
        LOS_Log(" %x ", value);
        i++;
        if ((i % u8_byteinline) == 0)
        {
            LOS_Log("\n");
        }
    }


#else
    LOS_Log("*****************************************************************\n");
    LOS_Log(" PRINT IS NOT SUPPORTED ,PLS ENABLE DEFINE (_PRINT_MEM_BYTES) \n");
    LOS_Log("*****************************************************************\n\n\n\n");
#endif //_PRINT_MEM_BYTES
}


void
FocusControl_PrintBytesExtMemory(
t_uint32    pMemoryAddress,
t_uint32    pHostMemoryAddress)
{
    t_sint32    Value = 0;
    t_uint32    i;
    t_uint8     loop_count = 0;

    LOS_Log("************************************************************\n");
    LOS_Log("     PRINT EXPORTED STATISTIC MEMORY BYTE     \n");
    LOS_Log("************************************************************\n");

    LOS_Log("------------------------------------------------------------\n");
    LOS_Log("\nCALLING HOST ZONE SETUP FOR ALL THE 10 ZONES\n");
    LOS_Log("------------------------------------------------------------\n");
    AFStats_HostZoneSetup();
    LOS_Log("------------------------------------------------------------\n");
    LOS_Log("  SETUP DONE FOR ZONES   \n");
    LOS_Log("------------------------------------------------------------\n");

    if (pMemoryAddress == 0)
    {
        LOS_Log("ADDRESS NOT VALID , STOP EXECUTION ..\n");
        return;
    }
    else
    {
        LOS_Log("Storing Address of the Page Element , which is exposed in Memory \n");
        LOS_Log("AUTOFOCUS STATS MEMORY EXPORTING TEST \n");
        LOS_Log(
        "Specifying Host Memory address , where Stats to be Dumped when Host issue a command and provide notification to host as soon as the stats becomes available \n\n");

        // setting memory address
        LOS_Log("pMemoryAddress :%x \n", pMemoryAddress);
        LOS_Log("pHostMemoryAddress :%x \n", pHostMemoryAddress);
        ITE_writePE(AFStats_Controls_pu32_HostAssignedAddr_Byte0, pMemoryAddress);

        LOS_Log("if HOST ADDRESS : 0 , STATS NOT EXPORTED , RESULT : RETURN  \n\n");

        // clear if any notification is there .
        LOS_Log("CLEARING NOTIFICATION IF ANY : AFSTATS\n");
        ITE_CleanEvent(ITE_EVT_ISPCTL_FOCUS_EVENTS);
        LOS_Log("NOTIFICATION AFSTATS : CLEARED...\n");
    }

    while (loop_count < 1)
    {
        LOS_Log("\nERASE DUMMY DATA IF ANY IN MEMORY\n");

        LOS_Log("\nWRITING HOST ADDRESS WITH 0xFF TO 400 bytes LENGTH, TO ERASE DUMMY DATA... \n");
        for (i = 0; i < 416; i++)
        {
            (*(( volatile unsigned char * ) (pHostMemoryAddress + i))) = 0xff;
        }


        LOS_Log("WRITING TO MEMORY COMPLETED ...\n\n");

        FocusControl_PrintFormat(4, pHostMemoryAddress, 416);

        LOS_Log("\nWAITING SOME TIME ... \n\n");
        LOS_Sleep(500);

        // issue Command for the Stats Req from Host Side .
        LOS_Log("GET STATS HOST CMD CONTROL ...\n");
        LOS_Log("CHANGING THE MODE TO ONCE MODE : Afstats once mode process...\n");
        Value = AFStats_HostCmd_e_REQ_STATS_ONCE;

        ITE_writePE(AFStats_Controls_e_AFStats_HostCmd_Ctrl_Byte0, Value);

        //LOS_Sleep(1000);
        // Value = ITE_readPE(AFStats_Status_e_AFStats_StatusHostCmd_Status_Byte0);
        // this is for the fw side debug only .
        while (1)
        {
            Value = ITE_readPE(AFStats_Status_e_AFStats_StatusHostCmd_Status_Byte0);
            LOS_Log("StatusHostCmd : %d\n", Value);
            if
            (
                (Value == AFStats_StatusHostCmd_e_STATS_COPY_DONE)
            ||  (Value == AFStats_StatusHostCmd_e_STATS_COPY_NOT_YET_DONE)
            )
            {
                break;
            }


            Value = ITE_readPE(AFStats_Controls_e_AFStats_HostCmd_Ctrl_Byte0);
            LOS_Log("CtrlHostCmd : %d\n", Value);
        }


        LOS_Log("STATS_ONCE MODE HAS BEEN SET SUCCESSFULLY ...\n");
        LOS_Sleep(500);
        LOS_Log("TOGGLING THE COIN FOR STARTING THE STATS EXPORT PROCESS IN ONCE MODE ...\n");
        ITE_RefreshEventCount(Event0_Count_u16_EVENT0_20_AF_STATS_READY_Byte0);
        Value = ITE_readPE(AFStats_Status_e_Coin_AFStatsExportStatus_Byte0);
        if (Value == Coin_e_Tails)
        {
            Value = Coin_e_Heads;
        }
        else
        {
            Value = Coin_e_Tails;
        }


        ITE_writePE(AFStats_Controls_e_Coin_AFStatsExportCmd_Byte0, Value);

        //LOS_Sleep(1000);
        LOS_Log("WAIT FOR STATS COPY TO EXTERNAL MEMORY  \n");
        while (1)
        {
            if ((ITE_readPE(AFStats_Status_e_Coin_AFStatsExportStatus_Byte0) == (t_uint16)Value))
            {
                break;
            }
        }


        LOS_Log("COIN TOGGLED : SUCCESSFULLY \n");
        LOS_Log("WAIT FOR THE CMD STATUS OF THE AFSTATS ....\n");

        // this is for the fw side debug only .
        LOS_Sleep(500);
        while (1)
        {
            Value = ITE_readPE(AFStats_Status_e_AFStats_StatusHostCmd_Status_Byte0);
            if
            (
                (Value == AFStats_StatusHostCmd_e_STATS_COPY_DONE)
            ||  (Value == AFStats_StatusHostCmd_e_STATS_COPY_NOT_YET_DONE)
            )
            {
                break;
            }
        }


        LOS_Log("STATS COPY DONE..\n");

        //LOS_Sleep(500);
        LOS_Log("WAITING FOR AFSTATS NOTIFICATION...Exporting stats to memory notification...\n");
        ITE_NMF_waitForAFStatsReady();
        ITE_GetEventCount(Event0_Count_u16_EVENT0_20_AF_STATS_READY_Byte0);
        LOS_Log("RECIEVED NOTIFICATION : Afstats exported to memory notification...\n\n\n");
        loop_count++;
    }   // for loop of the test

    //-------------------------------------------------------------------------------------------------------------
    LOS_Log("\n\nGETTING VALUE FROM THE MEMORY  & PRINTING IT ON SCREEN ON DESIRED FORMAT\n\n");
    LOS_Log("\n\n pHostMemoryAddress :%x \n", pHostMemoryAddress);
    FocusControl_PrintFormat(4, pHostMemoryAddress, 416);
    LOS_Log("\n\n pHostMemoryAddress :%x \n", pHostMemoryAddress);

    LOS_Log("\n\n PROCESS FINISHED\n\n");
}


/****************************************************************************************************/
void
FocusControl_FLADMove_and_Stats_Gather(
t_uint32    pMemoryAddress,
t_uint32    pHostMemoryAddress)
{
    t_uint8     coin = 0;
    t_uint32    u32_Valid_flag = 3;
    char        mess[256];

    MMTE_TEST_START(
    "FocusControl_FLAStats_Gather",
    "/ite_nmf/test_results_nreg/focus_tests",
    "TESTING FocusControl_FLAStats_Gather");
    MMTE_TEST_NEXT("FocusControl_FLAStats_Gather\n");

    if (pMemoryAddress == 0)
    {
        LOS_Log("ADDRESS NOT VALID , STOP EXECUTION ..\n");
        return;
    }


    if (0 == ITE_readPE(FLADriver_NVMStoredData_e_Flag_NVMActuatorLensPresent_Byte0))
    {
        MMTE_TEST_COMMENT("ACTUATOR LENS NOT PRESENT\n");
        MMTE_TEST_SKIPPED();
    }
    else
    {
        LOS_Log("MOVE TO INFINITY COMMAND PROCESS\n\n");

        // issue the command to move to infinity 1st , so that the move and stats gathering can be started from infinity.
        ITE_writePE(
        FocusControl_Controls_e_FocusControl_LensCommand_Control_Byte0,
        FocusControl_LensCommand_e_LA_CMD_GOTO_INFINITY_FAR_END);
        ITE_RefreshEventCount(Event0_Count_u16_EVENT0_21_FLA_DRIVER_LENS_STOP_Byte0);
        Toggle_focus_coin(&coin);
        ITE_NMF_waitForLensStop();
        ITE_GetEventCount(Event0_Count_u16_EVENT0_21_FLA_DRIVER_LENS_STOP_Byte0);

        LOS_Log("INFINITY POSITION IS REACHED.. \n\n");
        LOS_Log("NOW:-     MOVE TO MACRO AND STATS GATHERING STEPS \n\n");
        LOS_Log("NOW:-     1:MOVE TO MACRO cmd \n\n");
        LOS_Log("NOW:-     2:e_Flag_StatsWithLensMove_Control : TRUE : stats gathering and Lens move control \n\n");
        LOS_Log("NOW:-     3:Set Export Memory Address and wash it\n\n");
        LOS_Log("NOW:-     4:Toggling the command coin \n\n");
        LOS_Log("NOW:-     5:Waiting for AFStats Notification  \n\n");
        LOS_Log("NOW:-     6:check for the AFstats Validity Flag: should be INVALID  \n\n");
        LOS_Log("NOW:-     7:Again issue the stats request soon after the notification of Stats \n\n");
        LOS_Log("NOW:-     8:Wait for NOtification and check valid flag \n\n");
        LOS_Log("NOW:-     9:if Flag valid - check for FLAD stop Notification\n\n");

        LOS_Log("CLEAR NOTIFICATION :FLADRIVER STOP : IF ANY .\n");
        ITE_CleanEvent(ITE_EVT_ISPCTL_FOCUS_EVENTS);
        LOS_Sleep(500);

        LOS_Log("\n\nISSUE COMMAND MOVE TO MACRO..\n\n");

        // once at infinity now move to Macro along with the stats gatheing command process is started.
        ITE_writePE(
        FocusControl_Controls_e_FocusControl_LensCommand_Control_Byte0,
        FocusControl_LensCommand_e_LA_CMD_GOTO_MACRO_NEAR_END);

        ITE_writePE(FocusControl_Controls_e_Flag_StatsWithLensMove_Control_Byte0, Flag_e_TRUE);

        LOS_Log("SETTING HOST ADDRESS :%x \n", pMemoryAddress);
        ITE_writePE(AFStats_Controls_pu32_HostAssignedAddr_Byte0, pMemoryAddress);

        LOS_Sleep(500);

        //LOS_Log( "WRITING HOST ADDRESS WITH 0xFF TO 400 bytes LENGTH, TO ERASE DUMMY DATA... \n");
        //for (i=0;i<400;i++) (*((volatile unsigned char *)(pHostMemoryAddress+i))) = 0xff;
        //LOS_Log( "WRITING TO MEMORY COMPLETED ...\n\n");
        // clear if any notification is there .
        //LOS_Log( "\nCLEARING NOTIFICATION IF ANY : AFSTATS\n");
        //ITE_CleanEvent(ITE_EVT_ISPCTL_STATS_EVENTS);

        ITE_RefreshEventCount(Event0_Count_u16_EVENT0_20_AF_STATS_READY_Byte0);
        ITE_RefreshEventCount(Event0_Count_u16_EVENT0_21_FLA_DRIVER_LENS_STOP_Byte0);

        // Toggle the command coin so that the actual move withrespect to command is executed.
        Toggle_focus_coin(&coin);

        LOS_Log("WAITING FOR LENS STOP ALONG WITH AFSTATS NOTIFICATION...\n");
        ITE_NMF_waitForLensStopWithAFStats();

        ITE_GetEventCount(Event0_Count_u16_EVENT0_20_AF_STATS_READY_Byte0);
        ITE_GetEventCount(Event0_Count_u16_EVENT0_21_FLA_DRIVER_LENS_STOP_Byte0);

        u32_Valid_flag = (*(( volatile unsigned long int * ) (pHostMemoryAddress + 4 * 100)));

        if (u32_Valid_flag == 1)
        {
            LOS_Log("\n\nGOT THE VALUE of Valid flag : VALID   :%d\n\n", u32_Valid_flag);
        }
        else if (u32_Valid_flag == 0)
        {
            LOS_Log("\n\nGOT THE VALUE of Valid flag : INVALID   :%d\n\n", u32_Valid_flag);
        }
        else
        {
            LOS_Log("\n\nGOT THE VALUE : WRONG VALUE : %d\n\n", u32_Valid_flag);
        }


        //Only Stats Request Now
        ITE_RefreshEventCount(Event0_Count_u16_EVENT0_20_AF_STATS_READY_Byte0);
        coin = ITE_readPE(AFStats_Status_e_Coin_AFStatsExportStatus_Byte0);
        if (coin == Coin_e_Tails)
        {
            coin = Coin_e_Heads;
        }
        else
        {
            coin = Coin_e_Tails;
        }


        ITE_writePE(AFStats_Controls_e_Coin_AFStatsExportCmd_Byte0, coin);

        ITE_NMF_waitForAFStatsReady();
        ITE_GetEventCount(Event0_Count_u16_EVENT0_20_AF_STATS_READY_Byte0);
        u32_Valid_flag = (*(( volatile unsigned char * ) (pHostMemoryAddress + 4 * 100)));
        if (u32_Valid_flag == 1)
        {
            LOS_Log("\n\nGOT THE VALUE  : VALID :%d\n\n", ( int ) u32_Valid_flag);
            snprintf(
            mess,
            sizeof(mess),
            "PROCESS COMPLETED SUCCESSFULLY : GOT THE VALUE  : VALID :%d\n",
            ( int ) u32_Valid_flag);
            MMTE_TEST_COMMENT(mess);
            MMTE_TEST_PASSED();
        }
        else if (u32_Valid_flag == 0)
        {
            LOS_Log("\n\nGOT THE VALUE  : INVALID   :%d\n\n", ( int ) u32_Valid_flag);
            snprintf(mess, sizeof(mess), "GOT THE VALUE  : INVALID   :%d\n", ( int ) u32_Valid_flag);
            MMTE_TEST_COMMENT(mess);
            MMTE_TEST_FAILED();
        }
        else
        {
            LOS_Log("\n\nGOT THE VALUE  : WRONG VALUE  :%d\n\n", ( int ) u32_Valid_flag);
            snprintf(mess, sizeof(mess), "GOT THE VALUE  : INVALID   :%d\n", ( int ) u32_Valid_flag);
            MMTE_TEST_COMMENT(mess);
            MMTE_TEST_FAILED();
        }


        LOS_Log("\n\nPROCESS FINISHED\n\n");
    }
}


/****************************************************************************************************/
void
FocusControl_Simulate_SW3A_Test(
t_uint32    pMemoryAddress,
t_uint32    pHostMemoryAddress)
{
    t_uint8     coin = 0;
    t_uint8     u8_target_pos_random = 0,
                no_of_attempts = 0;
    t_uint16    u16_target_lens_pos = 0;
    t_sint16    LensPos = 0;
    t_sint16    s16_pos1=0, s16_pos2=0;
    t_uint32    u32_Valid_flag = 3;
    char        comment[256];
    static      t_uint32    u32_func_hit_counter = 0;

   UNUSED(pHostMemoryAddress);
/*

    U16_REGION_A_LOWER_LIMIT      =      0x8000
    U16_REGION_A_UPPER_LIMIT      =      0xFFFF
    U16_REGION_B_LOWER_LIMIT      =      0x0000
    U16_REGION_E_LOWER_LIMIT      =      0x0400
    U16_REGION_E_UPPER_LIMIT      =      0x7FFF


           Region A             Region B             Region C          Region D          Region E

       |<------------------|------------------|------------------|------------------|------------------>|
   0x8000               0xFFFF/0x0000      Inf Far End      Macro Near End        0x0400              0x7FFF

       (inner -case 1)        (inner-case 2)      (inner-case 0)     (inner-case 3)     (inner-case 4)

      If inner switch case 0 is hit, any random value from region C would be targeted
      If inner switch case 1 is hit, any random value from region A would be targeted
      If inner switch case 2 is hit, any random value from region B would be targeted
      If inner switch case 3 is hit, any random value from region D would be targeted
      If inner switch case 4 is hit, any random value from region E would be targeted

*/


    MMTE_TEST_START(
    "Focus_Simulate_SW3A",
    "/ite_nmf/test_results_nreg/focus_tests",
    "TESTING FocusControl_Simulate_SW3A_Test");

    MMTE_TEST_NEXT("Focus_Simulate_SW3A\n");

    if (pMemoryAddress == 0)
    {
        MMTE_TEST_COMMENT("ADDRESS NOT VALID , STOP EXECUTION ..\n");
        MMTE_TEST_FAILED();
        return;
    }


    if (0 == ITE_readPE(FLADriver_NVMStoredData_e_Flag_NVMActuatorLensPresent_Byte0))
    {
        MMTE_TEST_COMMENT("ACTUATOR LENS NOT PRESENT\n");
        MMTE_TEST_SKIPPED();
    }
    else
    {

        //CLEAR NOTIFICATION :FLADRIVER STOP : IF ANY
        ITE_CleanEvent(ITE_EVT_ISPCTL_FOCUS_EVENTS);
        u32_func_hit_counter++;

        LOS_Sleep(500);

        //randomize target position.
        if (u32_func_hit_counter % 2 == 0)              /* to hit outer case 0 on every alternate call */
        {    u8_target_pos_random = 0;    }

        else
        {    u8_target_pos_random = ((rand() * LOS_getSystemTime()) % 7) + 1;    }     /* to hit anything except outer case 0 */

        switch (u8_target_pos_random)                      /*outer switch*/

        {
            case 0:                /* Random Target */
                    if (u32_func_hit_counter % 4 != 0)             /* to hit inner-case 0 on every alternate call, and otherwise anything except inner-case 0  */
                        {u8_target_pos_random = ((rand() * LOS_getSystemTime()) % 4) + 1;}


                    switch (u8_target_pos_random)       /*inner switch*/
                            {
                                case 0:  /* Region C */
                                    MMTE_TEST_COMMENT("Random target position selected is between Inf Far End and Macro Near End.\n");
                                    s16_pos1 = (t_sint16)(ITE_readPE(FLADriver_LLLCtrlStatusParam_u16_MacroNearEndPos_Byte0));
                                    s16_pos2 = (t_sint16)(ITE_readPE(FLADriver_LLLCtrlStatusParam_u16_InfinityFarEndPos_Byte0));
                                    u16_target_lens_pos = rand() % ABS(s16_pos1 - s16_pos2) + MIN(s16_pos1, s16_pos2);
                                    //set target position.
                                    ITE_writePE(FLADriver_LLLCtrlStatusParam_u16_TarSetPos_Byte0, u16_target_lens_pos);

                                    break;

                                case 1: /* Region A */
                                    MMTE_TEST_COMMENT("Random target position selected is between 0x8000 and 0xFFFF.\n");
                                    s16_pos1 = (t_sint16)U16_REGION_A_UPPER_LIMIT;
                                    s16_pos2 = (t_sint16)U16_REGION_A_LOWER_LIMIT;
                                    u16_target_lens_pos = rand() % ABS(s16_pos1 - s16_pos2) + MIN(s16_pos1, s16_pos2);
                                    //set target position.
                                    ITE_writePE(FLADriver_LLLCtrlStatusParam_u16_TarSetPos_Byte0, u16_target_lens_pos);

                                    break;


                                case 2:  /* Region B */
                                    MMTE_TEST_COMMENT("Random target position selected is between 0x0000 and Inf Far End.\n");
                                    s16_pos1 = (t_sint16)(ITE_readPE(FLADriver_LLLCtrlStatusParam_u16_InfinityFarEndPos_Byte0));
                                    s16_pos2 = (t_sint16)U16_REGION_B_LOWER_LIMIT;
                                    u16_target_lens_pos = rand() % ABS(s16_pos1 - s16_pos2) + MIN(s16_pos1, s16_pos2);
                                    //set target position.
                                    ITE_writePE(FLADriver_LLLCtrlStatusParam_u16_TarSetPos_Byte0, u16_target_lens_pos);

                                    break;


                                case 3:  /* Region D */
                                   MMTE_TEST_COMMENT("Random target position selected is between Macro Near End and 0x0400.\n");
                                   s16_pos1 = (t_sint16)U16_REGION_E_LOWER_LIMIT;
                                   s16_pos2 = (t_sint16)(ITE_readPE(FLADriver_LLLCtrlStatusParam_u16_MacroNearEndPos_Byte0));
                                   u16_target_lens_pos = rand() % ABS(s16_pos1 - s16_pos2) + MIN(s16_pos1, s16_pos2);
                                   //set target position.
                                   ITE_writePE(FLADriver_LLLCtrlStatusParam_u16_TarSetPos_Byte0, u16_target_lens_pos);

                                   break;


                                case 4:  /* Region E */
                                   MMTE_TEST_COMMENT("Random target position selected is between 0x0400 and 0x7FFF.\n");
                                   s16_pos1 = (t_sint16)U16_REGION_E_UPPER_LIMIT;
                                   s16_pos2 = (t_sint16)U16_REGION_E_LOWER_LIMIT;
                                   u16_target_lens_pos = rand() % ABS(s16_pos1 - s16_pos2) + MIN(s16_pos1, s16_pos2);
                                   //set target position.
                                   ITE_writePE(FLADriver_LLLCtrlStatusParam_u16_TarSetPos_Byte0, u16_target_lens_pos);

                                    break;

                                default:
                                    break;
                            }//end inner-switch

                //reading target position after writing to know the actual (t_sint16) value accepted
                LensPos = ITE_readPE(FLADriver_LLLCtrlStatusParam_u16_TarSetPos_Byte0);
                sprintf(comment, "Target POSITION : %d\n", LensPos);
                MMTE_TEST_COMMENT(comment);

                //finally set the type of lens command.
                ITE_writePE(
                FocusControl_Controls_e_FocusControl_LensCommand_Control_Byte0,
                FocusControl_LensCommand_e_LA_CMD_GOTO_TARGET_POSITION);

                break;

            case 1:                    /*Macro*/
                MMTE_TEST_COMMENT("Random target position selected is: Macro.\n");
                ITE_writePE(
                FocusControl_Controls_e_FocusControl_LensCommand_Control_Byte0,
                FocusControl_LensCommand_e_LA_CMD_GOTO_MACRO_NEAR_END);

                break;

            case 2:                    /*HorInfinity*/
                MMTE_TEST_COMMENT("Random target position selected is: HorInfinity.\n");
                ITE_writePE(
                FocusControl_Controls_e_FocusControl_LensCommand_Control_Byte0,
                FocusControl_LensCommand_e_LA_CMD_GOTO_INFINITY_HOR);

                break;

            case 3:                    /*Infinity*/

                MMTE_TEST_COMMENT("Random target position selected is: Infinity.\n");
                ITE_writePE(
                FocusControl_Controls_e_FocusControl_LensCommand_Control_Byte0,
                FocusControl_LensCommand_e_LA_CMD_GOTO_INFINITY_FAR_END);

                break;

            case 4:                    /*MoveStepToInfinity*/
                //[PM][To be enabled after cleanup]
                MMTE_TEST_COMMENT("Random target position selected is:MoveStepToInfinity .\n");
                ITE_writePE(FLADriver_LLLCtrlStatusParam_u16_ManualStepSize_Byte0, 120);
                ITE_writePE(
                FocusControl_Controls_e_FocusControl_LensCommand_Control_Byte0,
                FocusControl_LensCommand_e_LA_CMD_MOVE_STEP_TO_INFINITY);

                break;

            case 5:                    /*MoveStepToMacro*/
                //[PM][To be enabled after cleanup]
                MMTE_TEST_COMMENT("Random target position selected is:MoveStepToMacro .\n");
                ITE_writePE(FLADriver_LLLCtrlStatusParam_u16_ManualStepSize_Byte0, 150);
                ITE_writePE(
                FocusControl_Controls_e_FocusControl_LensCommand_Control_Byte0,
                FocusControl_LensCommand_e_LA_CMD_MOVE_STEP_TO_MACRO);

                break;

            case 6:                    /*Hyperfocal*/
                MMTE_TEST_COMMENT("Random target position selected is: Hyperfocal.\n");
                ITE_writePE(
                FocusControl_Controls_e_FocusControl_LensCommand_Control_Byte0,
                FocusControl_LensCommand_e_LA_CMD_GOTO_HYPERFOCAL);

                break;

            case 7:                    /*HorMacro*/
                MMTE_TEST_COMMENT("Random target position selected is: HorMacro.\n");
                ITE_writePE(
                FocusControl_Controls_e_FocusControl_LensCommand_Control_Byte0,
                FocusControl_LensCommand_e_LA_CMD_GOTO_MACRO_HOR);

                break;


            default:
                break;
        }//end outer-switch


        ITE_writePE(FocusControl_Controls_e_Flag_StatsWithLensMove_Control_Byte0, Flag_e_TRUE);

        ITE_writePE(AFStats_Controls_pu32_HostAssignedAddr_Byte0, pMemoryAddress);

        LOS_Sleep(500);

        ITE_RefreshEventCount(Event0_Count_u16_EVENT0_20_AF_STATS_READY_Byte0);
        ITE_RefreshEventCount(Event0_Count_u16_EVENT0_21_FLA_DRIVER_LENS_STOP_Byte0);

        Toggle_focus_coin(&coin);

        //Although sw3A doesnot wait for Lens Stop, still in ite_nmf lets go the extra mile..
        LOS_Log("\nWAITING FOR LENS STOP ALONG WITH AFSTATS NOTIFICATION...\n");
        ITE_NMF_waitForLensStopWithAFStats();

        ITE_GetEventCount(Event0_Count_u16_EVENT0_20_AF_STATS_READY_Byte0);
        ITE_GetEventCount(Event0_Count_u16_EVENT0_21_FLA_DRIVER_LENS_STOP_Byte0);

        u32_Valid_flag = ITE_readPE(FrameParamStatus_Af_u32_AfStatsValid_Byte0);

        if (u32_Valid_flag == 1)
        {
            LOS_Log("\n\nGOT THE VALUE of Valid flag : VALID   :%d\n\n", u32_Valid_flag);
        }
        else if (u32_Valid_flag == 0)
        {
            LOS_Log("\n\nGOT THE VALUE of Valid flag : INVALID   :%d\n\n", u32_Valid_flag);
        }
        else
        {
            LOS_Log("\n\nUNEXPECTED VALUE of Valid flag : %d !!!!\n\n", u32_Valid_flag);
        }


        //Don't give further lens movement request; loop until valid stats are received.
        for
        (
            no_of_attempts = 1;
            ((u32_Valid_flag == 0) && (no_of_attempts <= (MAX_ACCEPTABLE_AFSTAT_VALID_LIMIT)));
            no_of_attempts++
        )
        {
            //coin toggled.
            ITE_RefreshEventCount(Event0_Count_u16_EVENT0_20_AF_STATS_READY_Byte0);
            coin = ITE_readPE(AFStats_Status_e_Coin_AFStatsExportStatus_Byte0);
            ITE_writePE(AFStats_Controls_e_Coin_AFStatsExportCmd_Byte0, !coin);

            LOS_Log("\nWAITING FOR AFSTATS NOTIFICATION...\n");
            ITE_NMF_waitForAFStatsReady();
            ITE_GetEventCount(Event0_Count_u16_EVENT0_20_AF_STATS_READY_Byte0);

            u32_Valid_flag = ITE_readPE(FrameParamStatus_Af_u32_AfStatsValid_Byte0);

            if (u32_Valid_flag == 1)
            {
                LOS_Log("\n\nGOT THE VALUE  : VALID    :%d\n\n", ( int ) u32_Valid_flag);
                snprintf(comment, sizeof(comment), "GOT THE VALUE  : VALID    :%d\n", ( int ) u32_Valid_flag);
                mmte_testComment(comment);
            }
            else if (u32_Valid_flag == 0)
            {
                LOS_Log("\n\nGOT THE VALUE  : INVALID   :%d\n\n", ( int ) u32_Valid_flag);
                snprintf(comment, sizeof(comment), "GOT THE VALUE  : INVALID   :%d\n", ( int ) u32_Valid_flag);
                mmte_testComment(comment);
            }
            else
            {
                LOS_Log("\n\nUNEXPECTED VALUE of Valid flag  :%d\n\n", ( int ) u32_Valid_flag);
                snprintf(comment, sizeof(comment), "UNEXPECTED VALUE of Valid flag  :%d\n", ( int ) u32_Valid_flag);
                mmte_testComment(comment);
            }
        }  //end-for


        if (no_of_attempts <= MAX_ACCEPTABLE_AFSTAT_VALID_LIMIT)
        {
            snprintf(comment, sizeof(comment), "no_of_attempts :%d\n", ( int ) no_of_attempts);
            MMTE_TEST_COMMENT(comment);
            MMTE_TEST_PASSED();
        }
        else if (u32_Valid_flag > 1)
        {
            MMTE_TEST_FAILED();
        }
        else
        {
            snprintf(
            comment,
            sizeof(comment),
            "**TEST FAILED**IDEAL MAX NO OF ATTEMPTS ACCEPTABLE :%d\t AFSTATS NOT VALID AFTER ATTEMPTS :%d\n ",
            ( int ) MAX_ACCEPTABLE_AFSTAT_VALID_LIMIT,
            ( int ) no_of_attempts);
            MMTE_TEST_COMMENT(comment);
            MMTE_TEST_FAILED();
        }
    }
}


void
AFStats_FrameID_Test(
t_uint32    pMemoryAddress,
t_uint32    pHostMemoryAddress)
{
    t_sint32    Value = 0;
    t_uint32    i;
    t_uint8     AFStats_FrameID = 0;
    t_uint32    u32_Valid_flag = 0;
    t_uint8     Pre_AFStats_FrameID = 0;
    t_uint8     loop_count = 0;

    LOS_Log("************************************************************\n");
    LOS_Log("     FRAME ID TEST STARTED\n");
    LOS_Log("************************************************************\n");

    LOS_Log("------------------------------------------------------------\n");
    LOS_Log("\nCALLING HOST ZONE SETUP FOR ALL THE 10 ZONES\n");
    LOS_Log("------------------------------------------------------------\n");
    AFStats_HostZoneSetup();
    LOS_Log("------------------------------------------------------------\n");
    LOS_Log("  SETUP DONE FOR ZONES   \n");

    LOS_Log("------------------------------------------------------------\n");
    LOS_Log("BEFORE TEST THE VALUE OF FRAME ID\n");
    Pre_AFStats_FrameID = ITE_readPE(AFStats_Status_u8_focus_stats_Frame_ID_Byte0);

    LOS_Log("\nFRAME ID(before test)    : %d\n", Pre_AFStats_FrameID);
    LOS_Log("------------------------------------------------------------\n");

    if (pMemoryAddress == 0)
    {
        LOS_Log("ADDRESS NOT VALID , STOP EXECUTION ..\n");
        return;
    }
    else
    {
        LOS_Log("Storing Address of the Page Element , which is exposed in Memory \n");
        LOS_Log("AUTOFOCUS STATS MEMORY EXPORTING TEST \n");
        LOS_Log(
        "Specifying Host Memory address , where Stats to be Dumped when Host issue a command and provide notification to host as soon as the stats becomes available \n\n");

        // setting memory address
        LOS_Log("pMemoryAddress :%x \n", pMemoryAddress);
        LOS_Log("pHostMemoryAddress :%x \n", pMemoryAddress);
        ITE_writePE(AFStats_Controls_pu32_HostAssignedAddr_Byte0, pMemoryAddress);

        LOS_Log("if HOST ADDRESS : 0 , STATS NOT EXPORTED , RESULT : RETURN  \n\n");

        // clear if any notification is there .
        LOS_Log("CLEARING NOTIFICATION IF ANY : AFSTATS\n");
        ITE_CleanEvent(ITE_EVT_ISPCTL_FOCUS_EVENTS);
        LOS_Log("NOTIFICATION AFSTATS : CLEARED...\n");
    }

    while (loop_count < 1)
    {
        LOS_Log("\nERASE DUMMY DATA IF ANY IN MEMORY\n");

        LOS_Log("\nWRITING HOST ADDRESS WITH 0xFF TO 400 bytes LENGTH, TO ERASE DUMMY DATA... \n");
        for (i = 0; i < 416; i++)
        {
            (*(( volatile unsigned char * ) (pHostMemoryAddress + i))) = 0xff;
        }


        LOS_Log("WRITING TO MEMORY COMPLETED ...\n\n");

        LOS_Log("WAITING SOME TIME ... \n\n");
        LOS_Sleep(500);

        // issue Command for the Stats Req from Host Side .
        LOS_Log("GET STATS HOST CMD CONTROL ...\n");
        LOS_Log("CHNAGING THE MODE TO ONCE MODE : Afstats once mode process...\n");
        Value = AFStats_HostCmd_e_REQ_STATS_ONCE;

        ITE_writePE(AFStats_Controls_e_AFStats_HostCmd_Ctrl_Byte0, Value);

        //LOS_Sleep(1000);
        // Value = ITE_readPE(AFStats_Status_e_AFStats_StatusHostCmd_Status_Byte0);
        // this is for the fw side debug only .
        while (1)
        {
            Value = ITE_readPE(AFStats_Status_e_AFStats_StatusHostCmd_Status_Byte0);
            LOS_Log("StatusHostCmd : %d\n", Value);
            if
            (
                (Value == AFStats_StatusHostCmd_e_STATS_COPY_DONE)
            ||  (Value == AFStats_StatusHostCmd_e_STATS_COPY_NOT_YET_DONE)
            )
            {
                break;
            }


            Value = ITE_readPE(AFStats_Controls_e_AFStats_HostCmd_Ctrl_Byte0);
            LOS_Log("CtrlHostCmd : %d\n", Value);
        }


        LOS_Log("STATS_ONCE MODE HAS BEEN SET SUCCESSFULLY ...\n");
        LOS_Sleep(500);
        LOS_Log("TOGGLING THE COIN FOR STARTING THE STATS EXPORT PROCESS IN ONCE MODE ...\n");
        ITE_RefreshEventCount(Event0_Count_u16_EVENT0_20_AF_STATS_READY_Byte0);
        Value = ITE_readPE(AFStats_Status_e_Coin_AFStatsExportStatus_Byte0);
        if (Value == Coin_e_Tails)
        {
            Value = Coin_e_Heads;
        }
        else
        {
            Value = Coin_e_Tails;
        }


        ITE_writePE(AFStats_Controls_e_Coin_AFStatsExportCmd_Byte0, Value);

        //LOS_Sleep(1000);
        LOS_Log("WAIT FOR STATS COPY TO EXTERNAL MEMORY  \n");
        while (1)
        {
            if ((ITE_readPE(AFStats_Status_e_Coin_AFStatsExportStatus_Byte0) == (t_uint16)Value))
            {
                break;
            }
        }


        LOS_Log("COIN TOGGLED : SUCCESSFULLY \n");
        LOS_Log("WAIT FOR THE CMD STATUS OF THE AFSTATS ....\n");

        // this is for the fw side debug only .
        LOS_Sleep(500);
        while (1)
        {
            Value = ITE_readPE(AFStats_Status_e_AFStats_StatusHostCmd_Status_Byte0);
            if
            (
                (Value == AFStats_StatusHostCmd_e_STATS_COPY_DONE)
            ||  (Value == AFStats_StatusHostCmd_e_STATS_COPY_NOT_YET_DONE)
            )
            {
                break;
            }
        }


        LOS_Log("STATS COPY DONE..\n");

        //LOS_Sleep(500);
        LOS_Log("WAITING FOR AFSTATS NOTIFICATION...Exporting stats to memory notification...\n");
        ITE_NMF_waitForAFStatsReady();
        ITE_GetEventCount(Event0_Count_u16_EVENT0_20_AF_STATS_READY_Byte0);
        LOS_Log("RECIEVED NOTIFICATION : Afstats exported to memory notification...\n\n\n");
        loop_count++;
    }   // for loop of the test

    //-------------------------------------------------------------------------------------------------------------
    LOS_Log("\n\nGETTING VALUE FROM THE MEMORY FOR FRAME ID  : READING MEMOERY ...\n\n");

    //4byte * 10 elements per zone * 10 zones + 4 byte valid flag + 4 byte Lens Position + 4 byte Frame ID
    u32_Valid_flag = (*(( volatile unsigned char * ) (pHostMemoryAddress + (4 * 10 * 10) + 4 + 4)));
    LOS_Log("RECIEVED THE VALUE OF THE FRAME ID ...\n");
    LOS_Log("\n\n\nCOMPARING FRAME ID WITH BOTH GRAB STRUCT & AFStats_Status FRAME ID ...\n\n\n");
    LOS_Log(
    "\n\nIN OTHER WORDS : COMPARING : FRAME ID (From Exp Mem) TO u8_AFFrameID & u32_EXPORTED_AF_STATISTICS_Frame_Id \n\n");

    AFStats_FrameID = ITE_readPE(AFStats_Status_u8_focus_stats_Frame_ID_Byte0);

    LOS_Log("\n\n           RESULTS            \n\n");

    LOS_Log("\nMEMORY READ FrameID       : %d\n", u32_Valid_flag);
    LOS_Log("\nFRAME ID (before test)    : %d\n", Pre_AFStats_FrameID);
    LOS_Log("\nFRAME ID (after test )    : %d\n", AFStats_FrameID);
    LOS_Log("\nLOOP COUNT (no of time the host request for exporting stats) : %d\n", loop_count);
    LOS_Log("\n\nPROCESS FINISHED\n\n");
}


Result_te
AFSTatsReadyTest(
char        *OutputFile,
t_uint32    pMemoryAddress,
t_uint32    pHostMemoryAddress)
{
    t_sint32    Value = 0;
    Result_te   Result = Result_e_Success;
    char        message[256];
    t_los_file  *FileMemoryDump;
    t_uint32    i;

    // issue Command for the Stats Req from Host Side .
    if (pMemoryAddress == 0)
    {
        LOS_Log("TEST CASE WILL FAIL BECAUSE ADDRESS NOT VALID. STOPPING EXECUTION ..\n");
        Result &= Result_e_Failure;
        return (Result);
    }
    else
    {
        LOS_Log("OPENING FILE FOR DUMPING STATS ....... \n\n");
        FileMemoryDump = LOS_fopen(OutputFile, "w");
        if (FileMemoryDump != 0)
        {
            LOS_Log("FILE OPENED : SUCESSFULLY OPENED THE FILE ... \n");
        }
    }


    LOS_Log("\nWRITING HOST ADDRESS WITH 0xFF TO 400 bytes LENGTH, TO ERASE DUMMY DATA... \n");
    for (i = 0; i < 416; i++)
    {
        (*(( volatile unsigned char * ) (pHostMemoryAddress + i))) = 0xff;
    }


    ITE_writePE(AFStats_Controls_pu32_HostAssignedAddr_Byte0, pMemoryAddress);

    ITE_writePE(AFStats_Controls_e_AFStats_HostCmd_Ctrl_Byte0, AFStats_HostCmd_e_REQ_STATS_ONCE);

    //toggle coin
    LOS_Log("TOGGLING THE COIN FOR STARTING THE STATS EXPORT PROCESS IN ONCE MODE ...\n");
    ITE_RefreshEventCount(Event0_Count_u16_EVENT0_20_AF_STATS_READY_Byte0);

    Value = !(ITE_readPE(AFStats_Status_e_Coin_AFStatsExportStatus_Byte0));
    ITE_writePE(AFStats_Controls_e_Coin_AFStatsExportCmd_Byte0, Value);

    LOS_Log("DUMPING AFSTATS IN A FILE \n");
    LOS_Log("FILE DUMP PROCESS STARTED , WAIT FOR SOME TIME\n");

    //dumping stats from memory into a file.
    sprintf(message, "#\n\n%x\n\n|", ( unsigned int ) pHostMemoryAddress);
    LOS_fwrite(message, strlen(message), 1, FileMemoryDump);
    LOS_Sleep(500);
    LOS_fclose(FileMemoryDump);

    Result = Result & (AFStats_Params_Check(pHostMemoryAddress, Value));

    if (Result == Result_e_Success)
    {
        Result = Result & Result_e_Success;
    }
    else
    {
        Result = Result & Result_e_Failure;
    }


    return (Result);
}


Result_te
AFStats_Params_Check(
t_uint32    pHostMemoryAddress,
t_sint32    Value)
{
    t_sint16    err = 0;
    t_uint32    u32_Valid_flag = 0;
    Result_te   Result = Result_e_Success;
    char        comment[256];

    //waiting for event
    LOS_Log("WAITING FOR AFSTATS NOTIFICATION...Exporting stats to memory notification...\n");
    ITE_NMF_waitForAFStatsReady();
    err = ITE_GetEventCount(Event0_Count_u16_EVENT0_20_AF_STATS_READY_Byte0);

    if (0 == err)
    {
        Result &= Result_e_Success;
        snprintf(comment, sizeof(comment), "AFStatsReady event received\n");
        MMTE_TEST_COMMENT(comment);
    }
    else
    {
        Result &= Result_e_Failure;
        snprintf(comment, sizeof(comment), "AFStatsReady event not received. Test case will FAIL.\n");
        MMTE_TEST_COMMENT(comment);
        return (Result);
    }


    //check status coin
    if (ITE_readPE(AFStats_Status_e_Coin_AFStatsExportStatus_Byte0) != (t_uint16)Value)
    {
        snprintf(comment, sizeof(comment), "***********UNDESIRED STATE************");
        MMTE_TEST_COMMENT(comment);
        Result &= Result_e_Failure;
        return (Result);
    }


    //check 1: check valid info exported in memory
    LOS_Log("\n\nGETTING VALUE FROM THE MEMORY FOR FRAME ID  : READING MEMOERY ...\n\n");

    //4byte * 10 elements per zone * 10 zones + 4 byte valid flag
    u32_Valid_flag = (*(( volatile unsigned char * ) (pHostMemoryAddress + (4 * 10 * 10))));

    LOS_Log("\nREADING MEMORY FOR Validity of Stats : %d\n", u32_Valid_flag);

    //Valid Byte = 1=> Valid
    //Valid Byte = 0 => Invalid
    if (u32_Valid_flag == 1)
    {
        Result = Result & Result_e_Success;
        snprintf(comment, sizeof(comment), "Valid stats exported in memory\n");
        MMTE_TEST_COMMENT(comment);
    }
    else
    {
        Result = Result & Result_e_Failure;
        snprintf(comment, sizeof(comment), "Invalid stats exported in memory. Test case will FAIL.\n");
        MMTE_TEST_COMMENT(comment);
    }


    //check 2: check valid bit in FrameParamStatus_Af
    //u32_AfStatsValid = 1 => Valid
    //u32_AfStatsValid = 0 => Invalid
    if (ITE_readPE(FrameParamStatus_Af_u32_AfStatsValid_Byte0) == 1)
    {
        Result = Result & Result_e_Success;
        snprintf(comment, sizeof(comment), "Valid stats exported as per FrameParamStatus_Af.AfStatsValid.\n");
        MMTE_TEST_COMMENT(comment);
    }
    else
    {
        Result = Result & Result_e_Failure;
        snprintf(
        comment,
        sizeof(comment),
        "Invalid stats exported as per FrameParamStatus_Af.AfStatsValid. Test case will FAIL.\n");
        MMTE_TEST_COMMENT(comment);
    }


    return (Result);
}


void
FLADriverMoveToInfinity(void)
{
    char        mess[256],
                comment[256];
    Result_te   result = Result_e_Success;
    t_sint16    LensPos = 0;

    MMTE_TEST_START(
    "FLADriverMoveToInfinity",
    "/ite_nmf/test_results_nreg/focus_tests",
    "Testing for FLADriverMoveToInfinity");

    sprintf(mess, "Testing for FLADriverMoveToInfinity");
    MMTE_TEST_NEXT(mess);

    if (0 == FLADriver_NVMStoredData_e_Flag_NVMActuatorLensPresent_Byte0)
    {
        MMTE_TEST_COMMENT("FOCUS NOT SUPPORTED AS NO ACTUATOR PRESENT\n");
        MMTE_TEST_SKIPPED();
    }
    else
    {
        //get current Lens position
        LensPos = ITE_readPE(FLADriver_LLLCtrlStatusParam_u16_CurrentPos_Byte0);
        sprintf(mess, "CURRENT LENS POSITION = %d\n ", LensPos);
        MMTE_TEST_COMMENT(mess);

        //desired lens position
        LensPos = ITE_readPE(FLADriver_LLLCtrlStatusParam_u16_InfinityFarEndPos_Byte0);
        sprintf(mess, "ISSUING - LensCommand GOTO INFINITY (= %d)\n", LensPos);
        MMTE_TEST_COMMENT(mess);

        // issue the command for the Macro movement and here do not wait for the command absorption untill the coin has been toggled
        result = Focus_Send_Command(FocusControl_LensCommand_e_LA_CMD_GOTO_INFINITY_FAR_END);

        if (result == Result_e_Success)
        {
            result = Focus_TargetPos_Check(LensPos);
        }
        else
        {
            snprintf(comment, sizeof(comment), "ERROR ENCOUNTERED");
            MMTE_TEST_COMMENT(comment);
        }


        if (Result_e_Success == result)
        {
            MMTE_TEST_PASSED();
        }
        else
        {
            MMTE_TEST_FAILED();
        }
    }
}


void
FLADriverMoveToMacro(void)
{
    char        mess[256];
    Result_te   result = Result_e_Success;
    t_sint16    LensPos = 0;

    MMTE_TEST_START(
    "FLADriverMoveToMacro",
    "/ite_nmf/test_results_nreg/focus_tests",
    "Testing for FLADriverMoveToMacro");

    sprintf(mess, "Testing for FLADriverMoveToMacro");
    mmte_testNext(mess);

    if (0 == ITE_readPE(FLADriver_NVMStoredData_e_Flag_NVMActuatorLensPresent_Byte0))
    {
        MMTE_TEST_COMMENT("FOCUS NOT SUPPORTED AS NO ACTUATOR PRESENT\n");
        MMTE_TEST_SKIPPED();
    }
    else
    {
        //get current Lens position
        LensPos = ITE_readPE(FLADriver_LLLCtrlStatusParam_u16_CurrentPos_Byte0);
        sprintf(mess, "CURRENT LENS POSITION = %d\n ", LensPos);
        MMTE_TEST_COMMENT(mess);

        //desired lens position
        LensPos = ITE_readPE(FLADriver_LLLCtrlStatusParam_u16_MacroNearEndPos_Byte0);
        sprintf(mess, "ISSUING - LensCommand GOTO MACRO (= %d)\n", LensPos);
        MMTE_TEST_COMMENT(mess);

        // issue the command for the lens movement and here do not wait for the command absorption untill the coin has been toggled .
        result = Focus_Send_Command(FocusControl_LensCommand_e_LA_CMD_GOTO_MACRO_NEAR_END);

        if (result == Result_e_Success)
        {
            result = Focus_TargetPos_Check(LensPos);
        }


        if (Result_e_Success == result)
        {
            MMTE_TEST_PASSED();
        }
        else
        {
            MMTE_TEST_FAILED();
        }
    }
}


void
FLADriverMoveStepToInfinity(
t_uint16    StepSize)
{       // StepSize = 100
    t_sint16    LensPos = 0,
                diff_from_target = 0,
                tolerance_level = 0;
    t_sint16    CurrLensPos = 0;
    t_uint16    IncPos = 0;
    char        mess[256],
                comment[256];
    Result_te   result = Result_e_Success;

    MMTE_TEST_START(
    "FLADriverMoveStepToInfinity",
    "/ite_nmf/test_results_nreg/focus_tests",
    "Testing for FLADriverMoveStepToInfinity");

    sprintf(mess, "Testing for FLADriverMoveStepToInfinity");
    mmte_testNext(mess);

    if (0 == ITE_readPE(FLADriver_NVMStoredData_e_Flag_NVMActuatorLensPresent_Byte0))
    {
        MMTE_TEST_COMMENT("FOCUS NOT SUPPORTED AS NO ACTUATOR PRESENT\n");
        MMTE_TEST_SKIPPED();
    }
    else
    {
        LOS_Log("STEP SIZE AT high level : %d Setting step size \n", StepSize);

        ITE_writePE(FLADriver_LLLCtrlStatusParam_u16_ManualStepSize_Byte0, StepSize);
        LOS_Sleep(500);

        //get current Lens position
        LensPos = ITE_readPE(FLADriver_LLLCtrlStatusParam_u16_CurrentPos_Byte0);
        sprintf(mess, "CURRENT LENS POSITION = %d\n ", LensPos);
        MMTE_TEST_COMMENT(mess);

        result = Focus_Send_Command(FocusControl_LensCommand_e_LA_CMD_MOVE_STEP_TO_INFINITY);

        if (Result_e_Success == result)
        {
            CurrLensPos = ITE_readPE(FLADriver_LLLCtrlStatusParam_u16_CurrentPos_Byte0);
            LOS_Log("LENS POSITION AFTER MOVEMENT :  %d\n", CurrLensPos);

            IncPos = abs(CurrLensPos - LensPos);
            LOS_Log("INCREMENT IN POSITION  : %d\n\n", IncPos);
            LOS_Log("DIFFERENCE FROM THE TARGET  :  %d\n\n", ITE_readPE(FLADriver_LensLLDParam_u16_DiffFromTarget_Byte0));

            if (IncPos == ITE_readPE(FLADriver_LLLCtrlStatusParam_u16_ManualStepSize_Byte0))
            {
                diff_from_target = ITE_readPE(FLADriver_LensLLDParam_u16_DiffFromTarget_Byte0);
                tolerance_level = ITE_readPE(FLADriver_LLLCtrlStatusParam_s16_ToleranceSize_Byte0);
                snprintf(
                comment,
                sizeof(comment),
                "Current Lens Position : %d\n Desired Lens Step :%d\n Position Difference is :%d\n Tolerence level set :%d\n",
                CurrLensPos,
                StepSize,
                diff_from_target,
                tolerance_level);
                MMTE_TEST_COMMENT(comment);
                snprintf(comment,sizeof(comment),"PASSED - LENS POSITION EXACTLY REACHED : %d\n", CurrLensPos);
                MMTE_TEST_COMMENT(comment);
                result &= Result_e_Success;
            }
            else if
                (
                    (ITE_readPE(FLADriver_LensLLDParam_u16_DiffFromTarget_Byte0)) <=
                        (ITE_readPE(FLADriver_LLLCtrlStatusParam_s16_ToleranceSize_Byte0))
                )
            {
                diff_from_target = ITE_readPE(FLADriver_LensLLDParam_u16_DiffFromTarget_Byte0);
                tolerance_level = ITE_readPE(FLADriver_LLLCtrlStatusParam_s16_ToleranceSize_Byte0);
                snprintf(
                comment,
                sizeof(comment),
                "Current Lens Position : %d\n Desired Lens Step :%d\n Position Difference is :%d\n Tolerence level set :%d\n",
                CurrLensPos,
                StepSize,
                diff_from_target,
                tolerance_level);
                MMTE_TEST_COMMENT(comment);
                snprintf(comment,sizeof(comment),"PASSED - LENS POSITION ACHIEVED WITH TOLERANCE : %d\n", CurrLensPos);
                MMTE_TEST_COMMENT(comment);
                result &= Result_e_Success;
            }
            else
            {
                diff_from_target = ITE_readPE(FLADriver_LensLLDParam_u16_DiffFromTarget_Byte0);
                tolerance_level = ITE_readPE(FLADriver_LLLCtrlStatusParam_s16_ToleranceSize_Byte0);
                snprintf(
                comment,
                sizeof(comment),
                "Current Lens Position : %d\n Desired Lens Step :%d\n Position Difference is :%d\n Tolerence level set :%d\n",
                CurrLensPos,
                StepSize,
                diff_from_target,
                tolerance_level);
                MMTE_TEST_COMMENT(comment);
                snprintf(comment,sizeof(comment),"FAIL - LENS POSITION NOT ACHIEVED: %d\n", CurrLensPos);
                MMTE_TEST_COMMENT(comment);
                result &= Result_e_Failure;
            }
        }
        else
        {
            snprintf(comment, sizeof(comment), "ERROR ENCOUNTERED");
            MMTE_TEST_COMMENT(comment);
            result &= Result_e_Failure;
        }


        if (Result_e_Success == result)
        {
            MMTE_TEST_PASSED();
        }
        else
        {
            MMTE_TEST_FAILED();
        }
    }
}


void
FLADriverMoveStepToMacro(
t_uint16    StepSize)
{       // StepSize = 50
    t_sint16    LensPos = 0,
                diff_from_target = 0,
                tolerance_level = 0;
    t_sint16    CurrLensPos = 0;
    t_uint16    IncPos = 0;
    char        mess[256],
                comment[256];
    Result_te   result = Result_e_Success;

    MMTE_TEST_START(
    "FLADriverMoveStepToMacro",
    "/ite_nmf/test_results_nreg/focus_tests",
    "Testing for FLADriverMoveStepToMacro");

    sprintf(mess, "Testing for FLADriverMoveStepToMacro");
    mmte_testNext(mess);

    if (0 == ITE_readPE(FLADriver_NVMStoredData_e_Flag_NVMActuatorLensPresent_Byte0))
    {
        MMTE_TEST_COMMENT("FOCUS NOT SUPPORTED AS NO ACTUATOR PRESENT\n");
        MMTE_TEST_SKIPPED();
    }
    else
    {
        LOS_Log("STEP SIZE  : %d Setting step size \n", StepSize);

        ITE_writePE(FLADriver_LLLCtrlStatusParam_u16_ManualStepSize_Byte0, StepSize);
        LOS_Sleep(500);

        //get current Lens position
        LensPos = ITE_readPE(FLADriver_LLLCtrlStatusParam_u16_CurrentPos_Byte0);
        sprintf(mess, "CURRENT LENS POSITION = %d\n ", LensPos);
        MMTE_TEST_COMMENT(mess);

        result = Focus_Send_Command(FocusControl_LensCommand_e_LA_CMD_MOVE_STEP_TO_MACRO);

        if (Result_e_Success == result)
        {
            CurrLensPos = ITE_readPE(FLADriver_LLLCtrlStatusParam_u16_CurrentPos_Byte0);
            LOS_Log("LENS POSITION AFTER MOVEMENT :  %d\n", CurrLensPos);

            IncPos = abs(CurrLensPos - LensPos);
            LOS_Log("INCREMENT IN POSITION  : %d\n\n", IncPos);
            LOS_Log("DIFFERENCE FROM THE TARGET  :  %d\n\n", ITE_readPE(FLADriver_LensLLDParam_u16_DiffFromTarget_Byte0));


            if (IncPos == ITE_readPE(FLADriver_LLLCtrlStatusParam_u16_ManualStepSize_Byte0))
            {
                diff_from_target = ITE_readPE(FLADriver_LensLLDParam_u16_DiffFromTarget_Byte0);
                tolerance_level = ITE_readPE(FLADriver_LLLCtrlStatusParam_s16_ToleranceSize_Byte0);
                snprintf(
                comment,
                sizeof(comment),
                "Current Lens Position : %d\n Desired Lens Step :%d\n Position Difference is :%d\n Tolerence level set :%d\n",
                CurrLensPos,
                StepSize,
                diff_from_target,
                tolerance_level);
                MMTE_TEST_COMMENT(comment);
                snprintf(comment,sizeof(comment),"PASSED - LENS POSITION EXACTLY REACHED : %d\n", CurrLensPos);
                MMTE_TEST_COMMENT(comment);
                result &= Result_e_Success;
            }
            else if
                (
                    (ITE_readPE(FLADriver_LensLLDParam_u16_DiffFromTarget_Byte0)) <=
                        (ITE_readPE(FLADriver_LLLCtrlStatusParam_s16_ToleranceSize_Byte0))
                )
            {
                diff_from_target = ITE_readPE(FLADriver_LensLLDParam_u16_DiffFromTarget_Byte0);
                tolerance_level = ITE_readPE(FLADriver_LLLCtrlStatusParam_s16_ToleranceSize_Byte0);
                snprintf(
                comment,
                sizeof(comment),
                "Current Lens Position : %d\n Desired Lens Step :%d\n Position Difference is :%d\n Tolerence level set :%d\n",
                CurrLensPos,
                StepSize,
                diff_from_target,
                tolerance_level);
                MMTE_TEST_COMMENT(comment);
                snprintf(comment,sizeof(comment),"PASSED - LENS POSITION ACHIEVED WITH TOLERANCE : %d\n", CurrLensPos);
                MMTE_TEST_COMMENT(comment);
                result &= Result_e_Success;
            }
            else
            {
                diff_from_target = ITE_readPE(FLADriver_LensLLDParam_u16_DiffFromTarget_Byte0);
                tolerance_level = ITE_readPE(FLADriver_LLLCtrlStatusParam_s16_ToleranceSize_Byte0);
                snprintf(
                comment,
                sizeof(comment),
                "Current Lens Position : %d\n Desired Lens Step :%d\n Position Difference is :%d\n Tolerence level set :%d\n",
                CurrLensPos,
                StepSize,
                diff_from_target,
                tolerance_level);
                MMTE_TEST_COMMENT(comment);
                snprintf(comment,sizeof(comment),"FAIL - LENS POSITION NOT ACHIEVED: %d\n", CurrLensPos);
                MMTE_TEST_COMMENT(comment);
                result &= Result_e_Failure;
            }
        }
        else
        {
            snprintf(comment, sizeof(comment), "ERROR ENCOUNTERED");
            MMTE_TEST_COMMENT(comment);
            result &= Result_e_Failure;
        }


        if (Result_e_Success == result)
        {
            MMTE_TEST_PASSED();
        }
        else
        {
            MMTE_TEST_FAILED();
        }
    }
}


void
FLADriverMoveToTargetPos(
t_uint16    TargetPos)
{       //TargetPos = 255
    Result_te   result = Result_e_Success;
    char        mess[256],
                comment[256],
                sensorname[30],
                filename[256],
                pathname[256];
    t_sint16    LensPos = 0;


    // To generate multiple ".out" files we use mmte api instead of the macros
    if (usecase.sensor==0)
     {
      sprintf(sensorname,"Cam0_");
     }
    else
     {
      sprintf(sensorname,"Cam1_");
     }

     snprintf(filename,sizeof(filename),"%sFLADriverMoveToTargetPos_%d",sensorname,(int)g_test_count);

     snprintf(pathname,sizeof(pathname),IMAGING_PATH"/ite_nmf/test_results_nreg/focus_tests");
     strcat(g_out_path,pathname);
     init_test_results_nreg(g_out_path);
     mmte_testStart(filename,"FLADriverMoveToTargetPos",g_out_path);

    sprintf(mess, "Testing for FLADriverMoveToTargetPos");
    mmte_testNext(mess);

    if (0 == ITE_readPE(FLADriver_NVMStoredData_e_Flag_NVMActuatorLensPresent_Byte0))
    {
        sprintf(mess,"FOCUS NOT SUPPORTED AS NO ACTUATOR PRESENT\n");
        mmte_testComment(mess);
        mmte_testResult(TEST_SKIPPED);
        mmte_testEnd();
    }
    else
    {
        ITE_writePE(FLADriver_LLLCtrlStatusParam_u16_TarSetPos_Byte0, TargetPos);
        LOS_Sleep(500);

        //get current Lens position
        LensPos = ITE_readPE(FLADriver_LLLCtrlStatusParam_u16_CurrentPos_Byte0);
        sprintf(mess, "CURRENT LENS POSITION = %d\n ", LensPos);
        mmte_testComment(mess);

        //desired lens position
        LensPos = ITE_readPE(FLADriver_LLLCtrlStatusParam_u16_TarSetPos_Byte0);
        sprintf(mess, "ISSUING - LensCommand GOTO TARGET POSITION (= %d)\n", LensPos);
        mmte_testComment(mess);

        // issue the command for the lens movement and here do not wait for the command absorption untill the coin has been toggled .
        result = Focus_Send_Command(FocusControl_LensCommand_e_LA_CMD_GOTO_TARGET_POSITION);

        if (result == Result_e_Success)
        {
            result = Focus_TargetPos_Check(LensPos);
        }
        else
        {
            snprintf(comment, sizeof(comment), "ERROR ENCOUNTERED");
            mmte_testComment(comment);
        }


        if (Result_e_Success == result)
        {
            mmte_testResult(TEST_PASSED);
            mmte_testEnd();
        }
        else
        {
            mmte_testResult(TEST_FAILED);
            mmte_testEnd();
        }
        memset ( g_out_path, 0, KlogDirectoryLentgh*sizeof (char) );
        g_test_count++;
    }
}


void
FLADriverMoveToHorInfinityPos(void)
{
    char        mess[256],
                comment[256];
    Result_te   result = Result_e_Success;
    t_sint16    LensPos = 0;
    MMTE_TEST_START(
    "FLADriverMoveToHorInfinityPos",
    "/ite_nmf/test_results_nreg/focus_tests",
    "Testing for FLADriverMoveToHorInfinityPos");

    sprintf(mess, "Testing for FLADriverMoveToHorInfinityPos");
    mmte_testNext(mess);

    if (0 == ITE_readPE(FLADriver_NVMStoredData_e_Flag_NVMActuatorLensPresent_Byte0))
    {
        MMTE_TEST_COMMENT("FOCUS NOT SUPPORTED AS NO ACTUATOR PRESENT\n");
        MMTE_TEST_SKIPPED();
    }
    else
    {
        //get current Lens position
        LensPos = ITE_readPE(FLADriver_LLLCtrlStatusParam_u16_CurrentPos_Byte0);
        sprintf(mess, "CURRENT LENS POSITION = %d\n ", LensPos);
        MMTE_TEST_COMMENT(mess);

        //desired lens position
        LensPos = ITE_readPE(FLADriver_LLLCtrlStatusParam_u16_InfinityHorPos_Byte0);
        sprintf(mess, "ISSUING - LensCommand GOTO HOR_INFINITY (= %d)\n", LensPos);
        MMTE_TEST_COMMENT(mess);

        // issue the command for the Macro movement and here do not wait for the command absorption untill the coin has been toggled .
        result = Focus_Send_Command(FocusControl_LensCommand_e_LA_CMD_GOTO_INFINITY_HOR);

        if (result == Result_e_Success)
        {
            result = Focus_TargetPos_Check(LensPos);
        }
        else
        {
            snprintf(comment, sizeof(comment), "ERROR ENCOUNTERED");
            MMTE_TEST_COMMENT(comment);
        }


        if (Result_e_Success == result)
        {
            MMTE_TEST_PASSED();
        }
        else
        {
            MMTE_TEST_FAILED();
        }
    }
}


void
FLADriverMoveToRestPos(void)
{
    char        mess[256],
                comment[256];
    Result_te   result = Result_e_Success;
    t_sint16    LensPos = 0;

    MMTE_TEST_START(
    "FLADriverMoveToRestPos",
    "/ite_nmf/test_results_nreg/focus_tests",
    "Testing for FLADriverMoveToRestPos");

    sprintf(mess, "Testing for FLADriverMoveToRestPos");
    mmte_testNext(mess);

    if (0 == ITE_readPE(FLADriver_NVMStoredData_e_Flag_NVMActuatorLensPresent_Byte0))
    {
        MMTE_TEST_COMMENT("FOCUS NOT SUPPORTED AS NO ACTUATOR PRESENT\n");
        MMTE_TEST_SKIPPED();
    }
    else
    {
        //get current Lens position
        LensPos = ITE_readPE(FLADriver_LLLCtrlStatusParam_u16_CurrentPos_Byte0);
        sprintf(mess, "CURRENT LENS POSITION = %d\n ", LensPos);
        MMTE_TEST_COMMENT(mess);

        //desired lens position
        LensPos = ITE_readPE(FLADriver_LLLCtrlStatusParam_u16_RestPos_Byte0);
        sprintf(mess, "ISSUING - LensCommand GOTO REST_POSITION : %d\n", LensPos);
        MMTE_TEST_COMMENT(mess);

        // issue the command for the Macro movement and here do not wait for the command absorption untill the coin has been toggled .
        result = Focus_Send_Command(FocusControl_LensCommand_e_LA_CMD_GOTO_REST);

        if (result == Result_e_Success)
        {
            result = Focus_TargetPos_Check(LensPos);
        }
        else
        {
            snprintf(comment, sizeof(comment), "ERROR ENCOUNTERED");
            MMTE_TEST_COMMENT(comment);
        }


        if (Result_e_Success == result)
        {
            MMTE_TEST_PASSED();
        }
        else
        {
            MMTE_TEST_FAILED();
        }
    }
}


void
FLADriverMoveToHorMacro(void)
{
    char        mess[256],
                comment[256];
    Result_te   result = Result_e_Success;
    t_sint16    LensPos = 0;

    MMTE_TEST_START(
    "FLADriverMoveToHorMacro",
    "/ite_nmf/test_results_nreg/focus_tests",
    "Testing for FLADriverMoveToHorMacro");

    sprintf(mess, "Testing for FLADriverMoveToHorMacro");
    mmte_testNext(mess);

    if (0 == ITE_readPE(FLADriver_NVMStoredData_e_Flag_NVMActuatorLensPresent_Byte0))
    {
        MMTE_TEST_COMMENT("FOCUS NOT SUPPORTED AS NO ACTUATOR PRESENT\n");
        MMTE_TEST_SKIPPED();
    }
    else
    {
        //get current Lens position
        LensPos = ITE_readPE(FLADriver_LLLCtrlStatusParam_u16_CurrentPos_Byte0);
        sprintf(mess, "CURRENT LENS POSITION = %d\n ", LensPos);
        MMTE_TEST_COMMENT(mess);

        //desired lens position
        LensPos = ITE_readPE(FLADriver_LLLCtrlStatusParam_u16_MacroHorPos_Byte0);
        sprintf(mess, "ISSUING - LensCommand GOTO HOR_MACRO (= %d)\n", LensPos);
        MMTE_TEST_COMMENT(mess);

        // issue the command for the Macro movement and here do not wait for the command absorption untill the coin has been toggled .
        result = Focus_Send_Command(FocusControl_LensCommand_e_LA_CMD_GOTO_MACRO_HOR);

        if (result == Result_e_Success)
        {
            result = Focus_TargetPos_Check(LensPos);
        }
        else
        {
            snprintf(comment, sizeof(comment), "ERROR ENCOUNTERED");
            MMTE_TEST_COMMENT(comment);
        }


        if (Result_e_Success == result)
        {
            MMTE_TEST_PASSED();
        }
        else
        {
            MMTE_TEST_FAILED();
        }
    }
}


void
FLADriverMoveToHyperfocal(void)
{
    char        mess[256],
                comment[256];
    Result_te   result = Result_e_Success;
    t_sint16    LensPos = 0;

    MMTE_TEST_START(
    "FLADriverMoveToHyperfocal",
    "/ite_nmf/test_results_nreg/focus_tests",
    "Testing for FLADriverMoveToHyperfocal");

    sprintf(mess, "Testing for FLADriverMoveToHyperfocal");
    mmte_testNext(mess);

    if (0 == ITE_readPE(FLADriver_NVMStoredData_e_Flag_NVMActuatorLensPresent_Byte0))
    {
        MMTE_TEST_COMMENT("FOCUS NOT SUPPORTED AS NO ACTUATOR PRESENT\n");
        MMTE_TEST_SKIPPED();
    }
    else
    {
        //get current Lens position
        LensPos = ITE_readPE(FLADriver_LLLCtrlStatusParam_u16_CurrentPos_Byte0);
        sprintf(mess, "CURRENT LENS POSITION = %d\n ", LensPos);
        MMTE_TEST_COMMENT(mess);

        //desired lens position
        LensPos = ITE_readPE(FLADriver_LLLCtrlStatusParam_u16_HyperFocalPos_Byte0);
        sprintf(mess, "ISSUING - LensCommand GOTO HYPERFOCAL (= %d)\n", LensPos);
        MMTE_TEST_COMMENT(mess);

        // issue the command for the Macro movement and here do not wait for the command absorption untill the coin has been toggled .
        result = Focus_Send_Command(FocusControl_LensCommand_e_LA_CMD_GOTO_HYPERFOCAL);

        if (result == Result_e_Success)
        {
            result = Focus_TargetPos_Check(LensPos);
        }
        else
        {
            snprintf(comment, sizeof(comment), "ERROR ENCOUNTERED");
            MMTE_TEST_COMMENT(comment);
        }


        if (Result_e_Success == result)
        {
            MMTE_TEST_PASSED();
        }
        else
        {
            MMTE_TEST_FAILED();
        }
    }
}


/* This function sends the lens movement command by toggling the focus coin,
  * waits for lens movement complete, and checks whether command takes
  * less than MAX_ACCEPTABLE_AFSTAT_VALID_LIMIT frames to complete.
  */
Result_te
Focus_Send_Command(
FocusControl_LensCommand_te command)
{
    volatile t_uint16   frameid1 = 0,
                        frameid2 = 0,
                        diff = 0;
    t_sint16            err = 0;
    t_uint8             coin;
    char                comment[256];
    unsigned long long  timestart = 0,
                        timestop = 0;
    Result_te           result = Result_e_Success;

    //send command
    ITE_writePE(FocusControl_Controls_e_FocusControl_LensCommand_Control_Byte0, command);

    //Fetch event count
    ITE_RefreshEventCount(Event0_Count_u16_EVENT0_21_FLA_DRIVER_LENS_STOP_Byte0);

    if (ITE_readPE(DataPathControl_e_Flag_Pipe0Enable_Byte0) == Flag_e_TRUE)
    {
        //Pipe 0
        frameid1 = ITE_readPE(PipeStatus_0_u8_FramesStreamedOutOfPipeLastRun_Byte0);
    }
    else if (ITE_readPE(DataPathControl_e_Flag_Pipe1Enable_Byte0) == Flag_e_TRUE)
    {
        //Pipe 1
        frameid1 = ITE_readPE(PipeStatus_1_u8_FramesStreamedOutOfPipeLastRun_Byte0);
    }


    timestart = LOS_getSystemTime();

    // Toggle the command coin so that the actual move withrespect to command is executed.
    Toggle_focus_coin(&coin);

    // Added for event ISP_FLADRIVER_LENS_STOP check
    ITE_NMF_waitForLensStop();
    err = ITE_GetEventCount(Event0_Count_u16_EVENT0_21_FLA_DRIVER_LENS_STOP_Byte0);
    if (0 == err)
    {
        result &= Result_e_Success;
    }
    else
    {
        result &= Result_e_Failure;
        return (result);
    }


    timestop = LOS_getSystemTime();

    if (ITE_readPE(FocusControl_Status_e_Coin_Status_Byte0) != ITE_readPE(FocusControl_Controls_e_Coin_Control_Byte0))
    {
        //should not reach here!!
        LOS_Log("*************UNDESIRED STATE**************");
        result &= Result_e_Failure;
        return (result);
    }


    //wait for the command to absorbed .
    if
    (
        (FocusControl_LensCommand_te) ITE_readPE(FocusControl_Status_e_FocusControl_LensCommand_Status_Byte0) != command
    )
    {
        //should not reach here!!
        LOS_Log("Did not absorb command. \n");
        result &= Result_e_Failure;
        return (result);
    };

    if (ITE_readPE(DataPathControl_e_Flag_Pipe0Enable_Byte0) == Flag_e_TRUE)
    {
        //Pipe 0
        frameid2 = ITE_readPE(PipeStatus_0_u8_FramesStreamedOutOfPipeLastRun_Byte0);
    }
    else if (ITE_readPE(DataPathControl_e_Flag_Pipe1Enable_Byte0) == Flag_e_TRUE)
    {
        //Pipe 1
        frameid2 = ITE_readPE(PipeStatus_1_u8_FramesStreamedOutOfPipeLastRun_Byte0);
    }


    diff = frameid2 - frameid1;
    snprintf(
    comment,
    sizeof(comment),
    "Time taken by the command = %llu \tRequested in FrameID:%d Command completed in FrameID:%d\n",
    (timestop - timestart),
    ( int ) frameid1,
    ( int ) frameid2);
    MMTE_TEST_COMMENT(comment);

    if (diff <= MAX_ACCEPTABLE_AFSTAT_VALID_LIMIT)
    {
        result &= Result_e_Success;
    }
    else
    {
        snprintf(
        comment,
        sizeof(comment),
        "Test case will fail because lens move command took %d frames to converge.\n",
        ( int ) diff);
        MMTE_TEST_COMMENT(comment);

        result &= Result_e_Failure;
    }

    return (result);
}


/* This function verifies whether lens has moved to the desired position on completion
  * of the last lens movement command, within tolerance limits defined by FLADriver_LLLCtrlStatusParam_s16_ToleranceSize_Byte0
  */
Result_te
Focus_TargetPos_Check(
t_sint16    target_pos_addr)
{
    t_sint16    current_lens_pos = 0;
    t_uint16    diff_from_target = 0;
    t_uint16    tolerance_level  = 0;

    Result_te   result = Result_e_Success;
    char        comment[256];

    current_lens_pos = (t_sint16)ITE_readPE(FLADriver_LLLCtrlStatusParam_u16_CurrentPos_Byte0);
    diff_from_target = (t_uint16)ITE_readPE(FLADriver_LensLLDParam_u16_DiffFromTarget_Byte0);
    tolerance_level  = (t_uint16)ITE_readPE(FLADriver_LLLCtrlStatusParam_s16_ToleranceSize_Byte0);

    snprintf(
    comment,
    sizeof(comment),
    "Current Lens Position : %d\tDesired Lens Position :%d\tPosition Difference is :%d\tTolerence level set :%d\n",
    current_lens_pos,
    target_pos_addr,
    diff_from_target,
    tolerance_level);
    MMTE_TEST_COMMENT(comment);

    if (current_lens_pos == target_pos_addr)
    {
        snprintf(comment, sizeof(comment), "TEST WILL PASS BECAUSE LENS POSITION EXACTLY REACHED : %d\n", current_lens_pos);
        MMTE_TEST_COMMENT(comment);

        result &= Result_e_Success;
    }
    else if (diff_from_target <= tolerance_level)
    {
        snprintf(
        comment,
        sizeof(comment),
        "TEST WILL PASS BECAUSE LENS POSITION ACHIEVED WITH TOLERANCE : %d\n",
        current_lens_pos);
        MMTE_TEST_COMMENT(comment);

        result &= Result_e_Success;
    }
    else
    {
        snprintf(comment, sizeof(comment), "TEST WILL FAIL BECAUSE LENS POSITION NOT ACHIEVED: %d\n", current_lens_pos);
        mmte_testComment(comment);

        result &= Result_e_Failure;
    }

    return (result);
}


// Toggle the command coin so that the actual move withrespect to command is executed.
void
Toggle_focus_coin(
t_uint8 *focus_coin)
{
    *focus_coin = ITE_readPE(FocusControl_Status_e_Coin_Status_Byte0);

    if (*focus_coin == Coin_e_Tails)
    {
        *focus_coin = Coin_e_Heads;
    }
    else
    {
        *focus_coin = Coin_e_Tails;
    }


    // toggle the coin but do not wait for the status of the toggled coin , this has to be done after the notification ,
    // when lens stop , so that the command has been successfully absorbed .
    ITE_writePE(FocusControl_Controls_e_Coin_Control_Byte0, *focus_coin);
}


void
FLADriverManualFocusTest(void)
{
    t_uint16    LensPos = 0;

    if (0 == ITE_readPE(FLADriver_NVMStoredData_e_Flag_NVMActuatorLensPresent_Byte0))
    {
        MMTE_TEST_START(
        "FLADriverManualFocusTest",
        "/ite_nmf/test_results_nreg/focus_tests",
        "TESTING FLADriverManualFocusTest");
        MMTE_TEST_NEXT("FLADriverManualFocusTest\n");
        MMTE_TEST_COMMENT("ACTUATOR LENS NOT PRESENT\n");
        MMTE_TEST_SKIPPED();
    }
    else
    {
        LOS_Log("MANUAL FOCUS TEST ------ various manual focus test are performed inside it \n");
        LOS_Log("SETTING MODE TO MANUAL MODE \n");

        LOS_Sleep(100);

        LOS_Log("TEST DONE UNDER MANUAL FOCUS ARE GIVEN BELOW  \n");

        LOS_Log(
        "1: MOVE TO REST POSITION         : %d\n",
        ITE_readPE(FLADriver_LLLCtrlStatusParam_u16_RestPos_Byte0));
        LOS_Log(
        "2: MOVE TO MACRO                 :  %d\n",
        ITE_readPE(FLADriver_LLLCtrlStatusParam_u16_MacroNearEndPos_Byte0));
        LOS_Log(
        "3: MOVE TO INFINITY              : %d\n",
        ITE_readPE(FLADriver_LLLCtrlStatusParam_u16_InfinityFarEndPos_Byte0));
        LOS_Log(
        "4: MOVE TO TARGET POSITION       : %d\n",
        ITE_readPE(FLADriver_LLLCtrlStatusParam_u16_TarSetPos_Byte0));
        LOS_Log(
        "5: MOVE TO HOR INFINITY POSITION     : %d\n",
        ITE_readPE(FLADriver_LLLCtrlStatusParam_u16_InfinityHorPos_Byte0));
        LOS_Log(
        "6: MOVE SINGLE STEP TO MACRO:   STEP : %d\n",
        ITE_readPE(FLADriver_LLLCtrlStatusParam_u16_ManualStepSize_Byte0));
        LOS_Log(
        "7: MOVE SINGLE STEP TO INFINITY STEP : %d\n",
        ITE_readPE(FLADriver_LLLCtrlStatusParam_u16_ManualStepSize_Byte0));
        LOS_Log(
        "8: MOVE TO HOR MACRO POSITION    : %d\n",
        ITE_readPE(FLADriver_LLLCtrlStatusParam_u16_MacroHorPos_Byte0));
        LOS_Log(
        "9: MOVE TO HYPERFOCAL POSITION   : %d\n",
        ITE_readPE(FLADriver_LLLCtrlStatusParam_u16_HyperFocalPos_Byte0));

        LOS_Log("CLEAR NOTIFICATION  : IF ANY .\n");
        ITE_CleanEvent(ITE_EVT_ISPCTL_FOCUS_EVENTS);
        LOS_Sleep(500);

        LOS_Log("\nTEST 1 : FLADriverMoveToRestPos STARTED\n");
        FLADriverMoveToRestPos();

        LOS_Log("\nTEST 2 : FLADriverMoveToMacro STARTED\n");
        FLADriverMoveToMacro();

        LOS_Log("\nTEST 3 : FLADriverMoveToInfinity STARTED\n");
        FLADriverMoveToInfinity();

        LOS_Log("\nTEST 4 : FLADriverMoveToTargetPos STARTED\n");
        LensPos = ITE_readPE(FLADriver_LLLCtrlStatusParam_u16_MacroNearEndPos_Byte0);
        LensPos = (LensPos + ITE_readPE(FLADriver_LLLCtrlStatusParam_u16_InfinityFarEndPos_Byte0)) / 2;
        FLADriverMoveToTargetPos(LensPos);

        LOS_Log("\nTEST 5 : FLADriverMoveToHorInfinityPos STARTED\n");
        FLADriverMoveToHorInfinityPos();
        LOS_Log("\n\n\n");

        LOS_Log("\nTEST 6 : FLADriverMoveStepToMacro STARTED\n");
        FLADriverMoveStepToMacro(150);

        LOS_Log("\nTEST 7 : FLADriverMoveStepToInfinity STARTED\n");
        FLADriverMoveStepToInfinity(120);

        LOS_Log("\nTEST 8 : FLADriverMoveToHorMacro STARTED\n");
        FLADriverMoveToHorMacro();

        LOS_Log("\nTEST 9 : FLADriverMoveToHyperfocal STARTED\n");
        FLADriverMoveToHyperfocal();

        LOS_Log("\nALL THE TEST UNDER MANUAL TEST HAS BEEN FINISHED\n");
    }
}

