/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#ifndef ITE_NMF_EFFECTS_TESTS_H_
#define ITE_NMF_EFFECTS_TESTS_H_

#include "ite_sia_buffer.h"
#include "algotypedefs.h"
#include "ite_main.h"
#include "algoerror.h"

CMD_COMPLETION C_ite_EffectsTests_cmd(int a_nb_args, char ** ap_args);

void ITE_EffectsTests(char * ap_test_id, char * ap_grabvpip_options);
TAlgoError ITE_performMetricNegative(ts_siapicturebuffer FramePositive, ts_siapicturebuffer FrameNegative, TAlgoMetricErrorTolerance metricTolerance);
void ITE_generateFrameNoEffectLR(ts_sia_usecase usecase);
void ITE_generateFrameNegativeEffectLR(ts_sia_usecase usecase);
void ITE_generateFrameNoEffectHR(ts_sia_usecase usecase);
void ITE_generateFrameNegativeEffectHR(ts_sia_usecase usecase);
void ITE_generateFrameSolarizeEffectLR(ts_sia_usecase usecase);
void ITE_generateFrameSolarizeEffectHR(ts_sia_usecase usecase);
void ITE_generateFrameEmbossEffectLR(ts_sia_usecase usecase);
void ITE_generateFrameEmbossEffectHR(ts_sia_usecase usecase);
#endif /*ITE_NMF_EFFECTS_TESTS_H_ */
