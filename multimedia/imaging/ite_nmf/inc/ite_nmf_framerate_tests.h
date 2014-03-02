/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#ifndef ITE_NMF_FRAMERATE_TESTS_H_
#define ITE_NMF_FRAMERATE_TESTS_H_

#include "ite_nmf_framerate.h"
#include "ite_sia_buffer.h"
#include "algotypedefs.h"
#include "ite_main.h"
#include "algoerror.h"

CMD_COMPLETION C_ite_FramerateTests_cmd(int a_nb_args, char ** ap_args);

void ITE_FramerateTest(char * ap_test_id, char * ap_grabvpip_options);
void ITE_FramerateTest1080p(char * ap_test_id, char * ap_grabvpip_options);
Result_te ITE_unitaryTestStaticFramerate(float manualFr, enum e_grabPipeID pipeId);

#endif /*ITE_NMF_FRAMERATE_TESTS_H_ */
