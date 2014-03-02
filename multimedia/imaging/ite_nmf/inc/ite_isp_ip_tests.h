/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#ifndef ITE_ISP_IP_TESTS_H_
#define ITE_ISP_IP_TESTS_H_

//#include "hcl_defs.h"
#include "grab_types.idt.h"


// Declaration of all the functions

CMD_COMPLETION C_ite_ip_tests_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_Flip_Mirror_cmd(int a_nb_args, char ** ap_args);

void ITE_ip_tests(char * ap_test_id, char * ap_grabvpip_options);

#endif /* ITE_ISP_IP_TESTS_H_ */
