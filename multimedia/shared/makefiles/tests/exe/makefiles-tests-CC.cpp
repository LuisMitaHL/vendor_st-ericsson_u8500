/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include "makefiles-tests-CC.h"

class MakefileTest2 : public MakefileTest
{
public:
    MakefileTest2();
  
    void Hello();
};

MakefileTest testInstance;

extern "C" {
    void MakefileTest_cplusplus()
    {
        printf("MakefileTest_cplusplus\n");
        MakefileTest * testInstance2 = new MakefileTest2();
        testInstance.Hello();
        testInstance2->Hello();
        delete testInstance2;
    }
}

MakefileTest2::MakefileTest2()
{
    printf("MakefileTest2::MakefileTest2\n");
}

void MakefileTest2::Hello()
{
    MakefileTest::Hello();
    printf("MakefileTest2::Hello\n");
}
