/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "tee_test.h"

int main(int argc, char *argv[])
{
    TEEC_Result result;

    (void)argc;
    (void)argv;

    result = TEE_Test_Run_All(void);

    return result;
}
