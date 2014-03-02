/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include "makefiles-tests-CC.h"

MakefileTest::MakefileTest()
{
    printf("MakefileTest::MakefileTest\n");
}

void MakefileTest::Hello()
{
    printf("MakefileTest::Hello\n");
}

void MakefileTest::staticHello()
{
    printf("MakefileTest::staticHello\n");
}

extern "C" {
    void MakefileTest_Hello()
    {
        MakefileTest::staticHello();
    }
}
