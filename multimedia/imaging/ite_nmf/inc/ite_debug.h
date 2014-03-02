/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef ITE_DEBUG_H_
#define ITE_DEBUG_H_

CMD_COMPLETION C_ite_dbg_StoreFOVX_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_PE_Log_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_Store_IMAGE_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_Display_Events_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_Clear_Stats_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_readPE_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_writePE_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_startvpip_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_stopvpip_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_sleepvpip_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_wakeupvpip_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_startenv_smiapp_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_startenv_SENSOR_TUNNING_cmd(int a_nb_args, char ** ap_args);

CMD_COMPLETION C_ite_dbg_startgrab_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_stopgrab_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_enablepipe_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_startenv_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_stopenv_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_loadfw_cmd( int a_nb_args, char **    ap_args );
CMD_COMPLETION C_ite_dbg_setusecase_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_bmsmode_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_bmsout_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_bmlin_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_getusecase_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_preview_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_still_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_video_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_LR_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_HR_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_LRHR_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_bms_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_bml_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_dispbuf_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_initbuf_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_cleanbuf_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_initlcdbuf_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_showgrabuf_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_colorbartest_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_autoframerate_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_manualframerate_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_getframerate_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_dz_prepare_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_dz_step_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_dz_reset_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_dz_test_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_sensoraccess_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_sensorread_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_wait_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_help_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_sensormode_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_buffercheck_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_writebmp_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_startstopstress_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_iris_cmd(int a_nb_args, char ** ap_args);
//CMD_COMPLETION C_ite_dbg_testverifbms_cmd(int a_nb_args, char ** ap_args);
//CMD_COMPLETION C_ite_dbg_testverifbml_cmd(int a_nb_args, char ** ap_args);
//CMD_COMPLETION C_ite_dbg_testveriflrhr_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_givergbvalue_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_testframerate_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_sleep_mode_xp70_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_uagemode_cmd(int a_nb_args, char ** ap_args);
#ifdef TEST_PERFORMANCE
CMD_COMPLETION C_ite_dbg_recordtime_cmd(int a_nb_args, char ** ap_args);
#endif
CMD_COMPLETION C_ite_dbg_mmio_test_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_create_all_buffers_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_free_all_buffers_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_sensor_select_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_nvm_raw_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_rand_floatrange_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_SetRxTestPattern_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_SetCursorPosition_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_SetInputModeSelection_cmd(int     a_nb_args, char    **ap_args);
CMD_COMPLETION C_ite_dbg_Setresolution_cmd(int     a_nb_args, char    **ap_args);
CMD_COMPLETION C_ite_dbg_ConvToUpper_cmd(int     a_nb_args, char    **ap_args);
CMD_COMPLETION C_ite_dbg_rand_Range_cmd(int a_nb_args,char ** ap_args);
CMD_COMPLETION C_ite_dbg_set_SensorParams_cmd(int a_nb_args,char ** ap_args);
CMD_COMPLETION C_ite_dbg_getresolution_cmd(int     a_nb_args, char    **ap_args);
CMD_COMPLETION C_ite_dbg_float_readPE_cmd(int a_nb_args, char ** ap_args);



void ITE_dbg_readPE(t_uint16 AddrPE);
void ITE_dbg_writePE(t_uint16 AddrPE,t_uint32 data);
void ITE_dbg_startenv(char * ap_test_id, char * ap_grabvpip_options);
void ITE_dbg_stopenv(void);
void ITE_dbg_loadfw(char * ap_test_id, char * ap_grabvpip_options);
int ITE_dbg_setusecase(char * ap_pipe, char * ap_size, char * ap_format);
void ITE_dbg_bmsmode(char * ap_format);
void ITE_dbg_bmsout(char * ap_format);
void ITE_dbg_bmlin(char * ap_format);
void ITE_dbg_getusecase(void);
void ITE_dbg_preview(char *ap_type,char * ap_mode, char * streamlength);
void ITE_dbg_still(char * ap_mode);
void ITE_dbg_video(char * ap_mode);
void ITE_dbg_LR(char * ap_mode);
void ITE_dbg_HR(char * ap_mode);
void ITE_dbg_LRHR(char * ap_mode);
void ITE_dbg_bms(char * ap_mode);
void ITE_dbg_bml(char * ap_mode);
void ITE_dbg_bml_stripe(char * ap_mode, char * ap_mode1);
void ITE_dbg_dispbuf(char * ap_pipe,int mode);
void ITE_dbg_initbuf(char * ap_pipe);
void ITE_dbg_cleanbuf(char * ap_pipe);
void ITE_dbg_showgrabuf(char * ap_pipe);
int  ITE_dbg_initlcdbuf(char * ap_size, char * ap_format);
void ITE_dbg_autoframerate(char * min, char * max);
void ITE_dbg_manualframerate(char * curr_fps , char * max_fps);
t_bool ITE_dbg_frameratedata(char * ap_pipe);
float ITE_dbg_getframerate(char * ap_pipe);
int ITE_dbg_testframerate(char * ap_pipe_test,char * ap_size, char * ap_format);
void ITE_dbg_dz_prepare(void);
void ITE_dbg_dz_step(char * step);
void ITE_dbg_dz_reset(void);
void ITE_dbg_dz_test(char * ap_pipe, char * step);
void ITE_dbg_sensoraccess(char * index, char *data);
void ITE_dbg_sensorread(char * index);
void ITE_dbg_wait(char * sec);
void ITE_dbg_colorbar_test(char * ap_pipe);
void ITE_dbg_colorbar_test(char * ap_pipe);
void ITE_dbg_sensormode_test(char *ap_mode);
void ITE_dbg_buffercheck_test(char * ap_pipe,char *ap_mode);
void ITE_dbg_writebmp(char *ap_pipe,char *filename,char *pelorder);
void ITE_dbg_previewvideo_startstop_test(void);
//void ITE_dbg_veriftestbms(char * ap_test_id, char * ap_grabvpip_options);
//void ITE_dbg_veriftestbml(char * ap_test_id, char * ap_grabvpip_options);
//void ITE_dbg_veriftestlrhr(char * ap_test_id, char * ap_grabvpip_options);
void ITE_dbg_givergbvalue(char *ap_pipe,char *Xpos,char *Ypos);
void ITE_dbg_sleepxp70(char* modeno);
void ITE_dbg_usagemode(char* modeno);
void ITE_dbg_startenv_SENSOR_TUNNING(char * ap_test_id, char * ap_grabvpip_options);
void ITE_dbg_startenv_smiapp(char * ap_test_id, char * ap_grabvpip_options);
#ifdef TEST_PERFORMANCE
void ITE_dbg_recordtime(char * type, char * usecase,char * comment, char * tag);
#endif
void ite_dbg_create_buffers(char *buffer_name);
void ite_dbg_free_buffers(char *buffer_name);
void ite_nvm_raw_data(char* enable);
float ite_rand_FloatRange(float min_no, float max_no);
void ITE_RxCursorPosition (char *x_pos, char *x_width, char *y_pos, char *y_width);
void ITE_RxTestPattern(char * ap_mode, char *Red, char *GR, char *Blue, char *BG);
void ITE_SetInputModeSelection(char *inp_sel);
int ITE_dbg_setutresolution(char * ap_pipe, char * ap_size, char * ap_format);

//ER 427679,new command added for setting ISP FW trace parameters
CMD_COMPLETION C_ite_dbg_SetTraceProperty_cmd (int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_SetGammaCurve_cmd (int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_TestValidFrameFlag_cmd (int a_nb_args, char ** ap_args);


#endif /* ITE_DEBUG_H_ */
