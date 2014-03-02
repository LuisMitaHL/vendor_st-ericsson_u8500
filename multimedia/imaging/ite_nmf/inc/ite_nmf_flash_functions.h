/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#ifndef ITE_NMF_FLASH_FUNCTIONS_H_
#define ITE_NMF_FLASH_FUNCTIONS_H_


void Init_Flash_ITECmdList(void);
CMD_COMPLETION C_ite_dbg_FlashHelp_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_TestFlashSupport_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_FlashSanityTest_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_ConfigureFlash_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_VerifyFlashTriggered_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_BMSFlashTest_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_Flash_enable_pre_streaming_cmd (int a_nb_args, char ** ap_args);
#if FLASH_DRIVER_INCLUDE
CMD_COMPLETION C_ite_dbg_Flash_TestLitFrame_cmd(int     a_nb_args,char    **ap_args);
#endif
#endif /* ITE_NMF_FLASH_FUNCTIONS_H_ */
