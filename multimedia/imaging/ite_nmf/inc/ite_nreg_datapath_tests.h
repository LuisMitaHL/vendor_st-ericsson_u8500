/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
  * This code is ST-Ericsson proprietary and confidential.
   * Any use of the code for whatever purpose is subject to
    * specific written permission of ST-Ericsson SA.
     */
      
#ifndef ITE_NREG_DATAPATH_TESTS_H_
#define ITE_NREG_DATAPATH_TESTS_H_

CMD_COMPLETION C_ite_nreg_datapath_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_nreg_LRdatapath_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_nreg_HRdatapath_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_nreg_LRHRdatapath_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_nreg_BMSdatapath_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_nreg_VideoBMSdatapath_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_nreg_LRHRBMSdatapath_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_nreg_BMLdatapath_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_nreg_VideoBMLdatapath_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_nreg_ZoomBMLdatapath_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_nreg_ZoomStreaming_cmd(int a_nb_args, char ** ap_args);



int ITE_nreg_Datapath(char * ap_test_id, char * ap_grabvpip_options,char * sensor_tunning);

CMD_COMPLETION C_ite_test_bug97581_cmd(int a_nb_args, char ** ap_args);
void ITE_test_bug97581(char * ap_test_id, char * ap_grabvpip_options);

CMD_COMPLETION C_ite_test_bug98168_cmd(int a_nb_args, char ** ap_args);
void ITE_test_bug98168(char * ap_test_id, char * ap_grabvpip_options);

CMD_COMPLETION C_ite_basic_bml_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_datapath_HR_SmiaPP_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_datapath_HR_SENSOR_TUNNING_Cmd(int a_nb_args, char ** ap_args);
int ITE_testBasicBML(void);
void ITE_TestExpTime_datapath(float framerate,char pipe);
void ITE_testLRdatapath(enum e_grabFormat grbformat);
void ITE_testHRdatapath(enum e_grabFormat grbformat);
void ITE_testLRHRdatapath(enum e_grabFormat lrgrbformat,enum e_grabFormat hrgrbformat,t_uint8 sameAR);
int ITE_testVideoBMSdatapath_alloutput(enum e_grabFormat lrgrbformat,enum e_grabFormat hrgrbformat);
int ITE_testLRHRBMSdatapath(
     enum e_grabFormat   lrgrbformat,
     enum e_grabFormat   hrgrbformat,
     t_uint8 sameAR,
     t_uint8 glaceonly,
     t_uint8 Zoom,
     t_uint8 statinzoom);
int ITE_testZoomBMLStillForSensoroutputModes(enum e_grabFormat lrgrbformat,enum e_grabFormat hrgrbformat,t_uint32 stripe);
void ITE_testBMSdatapathForSensoroutputModes(void);
int ITE_testZoomstreaming(enum e_grabFormat lrgrbformat,enum e_grabFormat hrgrbformat,char * Pipe);
int ITE_testBMLdatapathForSensoroutputModes(enum e_grabFormat lrgrbformat,enum e_grabFormat hrgrbformat);
#endif /* ITE_NREG_DATAPATH_TESTS_H_ */
