/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#ifndef ITE_NONREG_RW_LIST_PE_H_
#define ITE_NONREG_RW_LIST_PE_H_

CMD_COMPLETION C_ite_NonRegTest_PE_RW_List_cmd(int a_nb_args, char ** ap_args);

void ITE_NonReg_RWListPe_InVfStillDisplay(char * ap_test_id, char * ap_grabvpip_options);
void ITE_NonReg_ListPE(int xSnapshot, int ySnapshot);
void ITE_NonReg_PollingPE(int xSnapshot, int ySnapshot);

#endif /*ITE_NONREG_RW_LIST_PE_H_ */
