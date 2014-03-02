/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/*!
 * \defgroup mmte_TESTS_TOOLS Nomadik Test API
 */
#ifndef mmte_TEST_H
#define mmte_TEST_H

/*!
 * \brief Test status
 * \ingroup mmte_TESTS_TOOLS
 */
typedef enum {
    TEST_PASSED  = 1, //!< OK
    TEST_FAILED  = 2, //!< NOK
    TEST_SKIPPED = 3, //!< SKIPPED
} t_test_status;

typedef enum {
    TEST_CONFIG_TRACE_LEVEL = 1,
} t_test_config;


#ifdef __cplusplus
    extern "C" {
#endif

/*!
 * \brief test start and initialization
 *
 * Report file of current test function is created and opened for writing.
 *
 * \param testName name of the current test
 * \param testDesc short description of the test
 * \param logDir   directory where the all output is going to be created (absolute or relative..)
 * \ingroup mmte_TESTS_TOOLS
 */
IMPORT_SHARED void mmte_testStart(const char* testName, const char* testDesc, const char* logDir);

/*!
 * \brief test comment
 *
 * Additional comment to the test.
 *
 * \param testComm comment
 * \ingroup mmte_TESTS_TOOLS
 */
IMPORT_SHARED void mmte_testComment(const char* testComm);

/*!
 * \brief test property
 *
 * Properties of the test.
 *
 * \param key property name
 * \param value property value
 * \ingroup mmte_TESTS_TOOLS
 */
IMPORT_SHARED void mmte_testProperty(const char* key, const char* value);

/*!
 * \brief starts next test step
 *
 * \param stepDescription string describing next test step
 * \ingroup mmte_TESTS_TOOLS
 */
IMPORT_SHARED void mmte_testNext(const char* stepDescription);

/*!
 * \brief test step result
 *
 * status of the current step:
 *   "Failed" in case of failure
 *   "Passed" in case of success
 *
 * \param status condition determining step status
 * \ingroup mmte_TESTS_TOOLS
 */
IMPORT_SHARED void mmte_testResult(t_test_status status);

/*!
 * \brief end of test
 *
 * Terminates the test.
 * \ingroup mmte_TESTS_TOOLS
 */
IMPORT_SHARED void mmte_testEnd(void);

/*!
 * \brief Get global test result
 *
 * \ingroup mmte_TESTS_TOOLS
 */
IMPORT_SHARED void mmte_testGlobalEnd(int* _testSuccessesGlobal, int *_testFailuresGlobal, int *_testSkippedGlobal);

/*!
 * \brief change trace configuration at run-time
 * Available Commands:
 *   TEST_CONFIG_TRACE_LEVEL
 *
 * For the moment, value is passed as-is
 *
 * \ingroup mmte_TESTS_TOOLS
 */
IMPORT_SHARED void mmte_testConfig(t_test_config cmd, t_sint8 value);


#ifdef __cplusplus
    }
#endif

#endif
