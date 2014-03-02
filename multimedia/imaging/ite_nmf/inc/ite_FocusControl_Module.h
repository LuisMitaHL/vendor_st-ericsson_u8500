/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef ITE_FOCUSCONTROL_MODULE_H_
#define ITE_FOCUSCONTROL_MODULE_H_



#include "VhcElementDefs.h"
#include "ite_testenv_utils.h"
#include "ite_sia_buffer.h"
#include "grab_types.idt.h"
#include "ite_main.h"
#include "cli.h"




extern void FLADriverManualFocusTest(void);
extern void AFStatsZoneSetup(void);
extern void FLADriverMoveToInfinity(void);
extern void FLADriverMoveToMacro(void);
extern void FLADriverMoveStepToInfinity(t_uint16 StepSize);
extern void FLADriverMoveToHorMacro(void);
extern void FocusControl_FLADMove_and_Stats_Gather(t_uint32 pMemoryAddress ,t_uint32 pHostMemoryAddress);
extern void FocusControl_PrintBytesExtMemory(t_uint32 pMemoryAddress ,t_uint32 pHostMemoryAddress);
extern void FocusControl_SpecifyPrintFormat(t_uint32 pMemoryAddress ,t_uint32 pHostMemoryAddress);
extern void FocusControl_PrintFormat(t_uint8 u8_byteinline , t_uint32 pMemoryAddress , t_uint16 u16_totalbyte );
extern void FLADriverMoveStepToMacro(t_uint16 StepSize);
extern void FLADriverMoveToTargetPos(t_uint16 TargetPos);
extern void FLADriverMoveToRestPos(void);
extern void FLADriverMoveToHorInfinityPos(void);
extern void FLADriverMoveToHyperfocal(void);
extern void AFStats_ZoneAbsDimensions(void);
extern Result_te  AFSTatsReadyTest(char * output,t_uint32 isp_addr,t_uint32 host_addr);
extern void AFStats_FrameID_Test(t_uint32 pMemoryAddress, t_uint32 pHostMemoryAddress);

extern void FocusControlInitialize(void);
extern void AFStats_HostFocusSystem(void);
extern void AFStats_HostZoneSetup(void);
extern void  ITE_NMF_waitForLensStop(void);
extern void  ITE_NMF_waitForAFStatsReady(void);
extern void AFStats_HostZoneWithZoom(void);
extern void AFStats_HostZoneInPreBoot(void);

extern void AFStats_OnlyZoomTest(void);
extern void AFStats_ZoomBugTest(void);
extern void AFStats_BugFindingTest(void);

extern void FocusControl_BugTest_107051(t_uint32 pMemoryAddress ,t_uint32 pHostMemoryAddress);
extern void AFStats_AFWindowSystemTest(void);
extern void AFStats_AFCurrentReadStructure(void);
extern void AFStats_WindowConfigReads(void);
extern void AFStats_HostConfigReads(void);

void Toggle_focus_coin (t_uint8* focus_coin) ;
void FocusControl_Simulate_SW3A_Test( t_uint32    pMemoryAddress,t_uint32    pHostMemoryAddress);

Result_te Focus_Send_Command(FocusControl_LensCommand_te endpos);
Result_te Focus_TargetPos_Check(t_sint16 target_pos_addr);
Result_te AFStats_Params_Check(t_uint32 pHostMemoryAddress,t_sint32 Value);



#endif /*ITE_FOCUSCONTROL_MODULE_H_ */
