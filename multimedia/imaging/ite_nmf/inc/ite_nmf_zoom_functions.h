/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef ITE_NMF_ZOOM_FUNCTIONS_H_
#define ITE_NMF_ZOOM_FUNCTIONS_H_



#include "VhcElementDefs.h"
#include "ite_testenv_utils.h"
#include "ite_sia_buffer.h"
#include "grab_types.idt.h"
#include "ite_main.h"
#include "cli.h"
#include "algotypedefs.h"
#include "algoerror.h"

float ITE_NMF_returnMaxDZforCurrentUsecase(void);
void ITE_NMF_DZ_Init(volatile float MaxDZ);
t_uint8 ITE_NMF_DZ_SendZoomCmd(ZoomCommand_te ZoomCmd);
void ITE_NMF_DZ_CheckCmdAbsorbed(t_uint8 DZCmdCount);
int ITE_NMF_DZ_WaitZoomComplete(void);
int ITE_NMF_DZ_SetFovX(float FOVX);
int ITE_NMF_DZ_SetCenter(t_sint16 Xoffset,t_sint16 Yoffset);
void ITE_NMF_DZ_Set(float DZ);
void ITE_NMF_DZ_ZoomSet(float DZ, t_bool toggle_bit);

TAlgoError ITE_NMF_DZ_Test(tps_siapicturebuffer p_refbuffer,tps_siapicturebuffer p_buffer,float DZref,float DZ);
TAlgoError ITE_NMF_ZoomTest(char *pipe, float DZ);
TAlgoError ITE_NMF_ZoomTest_FullRange(char *pipe,float MaxDZ);
TAlgoError ITE_NMF_VIDEOZoomTest(char *pipe, float DZ);
TAlgoError ITE_NMF_VIDEOZoomTest_FullRange(char *pipe,float MaxDZ);
TAlgoError ITE_NMF_STILLZoomTest(char *pipe, float DZ,t_uint32 stripe);
TAlgoError ITE_NMF_STILLZoomTest_FullRange(char *pipe,float MaxDZ);
void ITE_NMF_ZoomTest_with_range(float zoomstart,float zoomend,float zoomstep,char *Pipe,float MaxDZ);

void ITE_NMF_StoreRefBuffer(char *pipe);
void ITE_Zoom_stress_test(char *zstart,char *zend,char *zstep,char *Pipe);

CMD_COMPLETION C_ite_dbg_zoominit_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_zoomset_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_zoomhelp_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_storebuff_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_zoomtest_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_zoomtestfullrange_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_zoomtestbug1_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_zoomtestbug2_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_zoomtest_stress_cmd(int a_nb_args, char **ap_args);



void Init_dz_ITECmdList(void);

#endif /*ITE_NMF_ZOOM_FUNCTIONS_H_ */
