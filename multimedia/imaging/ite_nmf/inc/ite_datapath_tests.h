/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#ifndef ITE_DATAPATH_TESTS_H_
#define ITE_DATAPATH_TESTS_H_

CMD_COMPLETION C_ite_datapath_LR_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_datapath_HR_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_datapath_LRHR_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_datapath_BMS_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_datapath_BMS_FLASH_cmd(int a_nb_args, char ** ap_args);

void ITE_HRDatapath_SmiaPP(char * ap_test_id, char * ap_grabvpip_options);
void ITE_HRDatapath_SENSOR_TUNNING(char * ap_test_id, char * ap_grabvpip_options);
void ITE_LRDatapath_SENSOR_TUNNING(char * ap_test_id, char * ap_grabvpip_options);
void ITE_VfStillDisplay(char * ap_test_id, char * ap_grabvpip_options);
void ITE_LRDatapath(char * ap_test_id, char * ap_grabvpip_options);
void ITE_HRDatapath(char * ap_test_id, char * ap_grabvpip_options);
void ITE_LRHRDatapath(char * ap_test_id, char * ap_grabvpip_options);
void ITE_BMSDatapath(char * ap_test_id, char * ap_grabvpip_options);
void ITE_FLASH_BMSDatapath(char * ap_test_id, char * ap_grabvpip_options);

extern volatile tps_siaMetaDatabuffer pOutpuModeBuffer;

#endif /*ITE_DATAPATH_TESTS_H_ */
