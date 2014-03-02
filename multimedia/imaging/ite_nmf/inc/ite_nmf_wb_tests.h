/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#ifndef ITE_NMF_WB_TESTS_H_
#define ITE_NMF_WB_TESTS_H_

#include "ite_nmf_wb.h"

CMD_COMPLETION C_ite_WBTests_cmd(int a_nb_args, char ** ap_args);

void ITE_WBTests(char * ap_test_id, char * ap_grabvpip_options);
void ITE_NMF_GenerateFrameWithWBPreset(e_Constrained_WBPreset WBPreset);

#endif /*ITE_NMF_WB_TESTS_H_ */
