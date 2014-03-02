/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _MAKEFILES_TESTS_CC_H_
#define _MAKEFILES_TESTS_CC_H

#ifdef __cplusplus

class MakefileTest {
 public:
  MakefileTest();
  
  void Hello();
  static void staticHello();
};

#endif /* __cplusplus */

/********************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  void MakefileTest_Hello();

  void MakefileTest_cplusplus();
#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif  /* _MAKEFILES_TESTS_CC_H */
