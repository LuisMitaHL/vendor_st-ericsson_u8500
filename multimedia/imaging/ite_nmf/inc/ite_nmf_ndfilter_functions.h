/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#ifndef ITE_NMF_NDFILTER_FUNCTIONS_H_
#define ITE_NMF_NDFILTER_FUNCTIONS_H_

#include "VhcElementDefs.h"
#include "ite_testenv_utils.h"
#include "ite_sia_buffer.h"
#include "grab_types.idt.h"
#include "ite_main.h"
#include "cli.h"

void Init_NDFilter_ITECmdList(void);
CMD_COMPLETION C_ite_dbg_NDFilter_help_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_TestNDFilterSupport_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_Enable_Disable_NDFilter_cmd(int a_nb_args, char ** ap_args);

#endif /* ITE_NMF_NDFILTER_FUNCTIONS_H_ */
