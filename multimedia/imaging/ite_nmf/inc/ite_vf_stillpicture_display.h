/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef ITE_VF_STILLPICTURE_DISPLAY_H_
#define ITE_VF_STILLPICTURE_DISPLAY_H_

CMD_COMPLETION C_ite_VfStillDisplay_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_RAWDump_cmd(int a_nb_args, char ** ap_args);

void ITE_VfStillDisplay(char * ap_test_id, char * ap_grabvpip_options);
void ITE_LRDatapath(char * ap_test_id, char * ap_grabvpip_options);
void ITE_HRDatapath(char * ap_test_id, char * ap_grabvpip_options);
void ITE_LRHRDatapath(char * ap_test_id, char * ap_grabvpip_options);
void ITE_BMSDatapath(char * ap_test_id, char * ap_grabvpip_options);
int ITE_RAWDump(char * ap_test_id, char * ap_grabvpip_options);

#endif /*ITE_VF_STILLPICTURE_DISPLAY_H_ */

