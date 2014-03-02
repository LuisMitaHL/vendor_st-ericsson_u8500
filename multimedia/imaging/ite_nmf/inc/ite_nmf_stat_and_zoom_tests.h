/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __ITE_ISPBLOCKS_TESTS_H__
#define __ITE_ISPBLOCKS_TESTS_H__

/* Exports */
void Init_STAT_AND_ZOOM_CmdList(void);
CMD_COMPLETION C_ite_dbg_StatAndZoomHelp_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_StatTest_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_AECStatCancelTest_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_AFStatCancelTest_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_StatAndZoomTest_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_FocusCancelTest_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_zoomtestforstatsonlyTest_cmd(int a_nb_args, char ** ap_args);

#endif //__ITE_ISPBLOCKS_TESTS_H__
