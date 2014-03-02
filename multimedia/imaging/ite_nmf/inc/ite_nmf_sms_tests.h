/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __ITE_SMS_TESTS_H__
#define __ITE_SMS_TESTS_H__

/* Exports */
void Init_SMS_CmdList(void);
CMD_COMPLETION C_ite_dbg_SMSHelp_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_SMSSanityTest_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_SMSTest_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_SMSVerify_cmd(int a_nb_args, char ** ap_args);

#endif //__ITE_SMS_TESTS_H__
