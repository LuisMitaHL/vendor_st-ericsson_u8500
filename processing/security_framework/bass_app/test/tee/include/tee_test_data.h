/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef NW_TO_TEE_TEST_DATA_H
#define NW_TO_TEE_TEST_DATA_H

#include <tee_test_common.h>

int getSessionCommandInfo(enum TestTypes tt, SessionCommandInfo *sci);

int cleanUpSessionCommandInfo(enum TestTypes tt,
                              SessionCommandInfo *sci);

int compareResult(enum TestTypes tt, OperationInfo *opInfo);

#endif
