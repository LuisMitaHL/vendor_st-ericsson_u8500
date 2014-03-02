/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#ifndef ITE_NMF_SENSOR_TUNNING_FUNCTIONS_H_
#define ITE_NMF_SENSOR_TUNNING_FUNCTIONS_H_

#include "cli.h"
#include "grab_types.idt.h"

void Init_SENSOR_TUNNING_ITECmdList(void);
CMD_COMPLETION C_ite_dbg_SENSOR_TUNNING_help_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_Test_SENSOR_TUNNING_Support_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_Query_SENSOR_TUNNING_Idx_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_QuerySensorInfo_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_Configure_SENSOR_TUNNING_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION ITE_Query_SENSOR_TUNNING_Indices();
CMD_COMPLETION ITE_Configure_SENSOR_TUNNING();
CMD_COMPLETION C_ite_dbg_Before_Boot_SENSOR_TUNNING_help_cmd(int     a_nb_args, char    **ap_args);
CMD_COMPLETION C_ite_dbg_Before_Boot_Test_SENSOR_TUNNING_Support_cmd(int     a_nb_args, char    **ap_args);
CMD_COMPLETION C_ite_dbg_Before_Boot_Query_SENSOR_TUNNING_Idx_cmd( int     a_nb_args, char    **ap_args);
CMD_COMPLETION C_ite_dbg_Before_Boot_Configure_SENSOR_TUNNING_cmd( int     a_nb_args, char    **ap_args);
CMD_COMPLETION C_ite_dbg_Before_Boot_QuerySensorInfo_cmd( int     a_nb_args, char    **ap_args);

#endif /* ITE_NMF_SENSOR_TUNNING_FUNCTIONS_H_ */
