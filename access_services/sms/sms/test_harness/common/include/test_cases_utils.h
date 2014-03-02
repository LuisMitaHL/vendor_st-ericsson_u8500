/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef INCLUSION_GUARD_TEST_CASES_UTILS_H
#define INCLUSION_GUARD_TEST_CASES_UTILS_H

#ifdef SMS_TEST_HARNESS_MEMORY_CHECK_ENABLED

/********************************************************************/
/**
 *
 * @function     Request_SMS_TestUtil_MemoryStatusGet
 *
 * @description
 * This is a test harness function which is used when additional memory
 * checking is enabled. Calling this function sends a signal to the
 * SMS Server which causes it to dump the current memory allocation
 * status to the log.  There is no response signal.
 *
 * This function is needed because the test harness cannot access the
 * SMS Server data structures directly.
 *
 * @param [in] RequestCtrl_p    Contains information to send data to
 *                              the SMS Server.
 *
 * @return     SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t Request_SMS_TestUtil_MemoryStatusGet(
    const SMS_RequestControlBlock_t * const RequestCtrl_p);

#endif                          // SMS_TEST_HARNESS_MEMORY_CHECK_ENABLED



#ifdef SMS_TEST_HARNESS_CAT_REFRESH_SIMULATION_ENABLED

#include "g_sms.h"              // Need to know the internal data structures used in signals so test harness can pass in data.

/********************************************************************/
/**
 *
 * @function     Request_SMS_Test_CAT_Cause_PC_RefreshInd
 *
 * @description
 * This is a test harness function which is used to simulate a CAT
 * Refresh. It causes a signal to be sent to the SMS Process which is
 * then handled as if it had come from the UICC CAT Server callback.
 *
 * This function simulates the CAT_CAUSE_PC_REFRESH_IND.
 *
 * This function is needed because it is not possible to stub out the
 * UICC CAT server which is needed for other test cases.
 *
 * There is no response signal.
 *
 * @param [in] RequestCtrl_p    Contains information to send data to
 *                              the SMS Server.
 * @param [in] RefreshIndData_p CAT Refresh data to send to SMS Server.
 *
 * @return     SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t Request_SMS_Test_CAT_Cause_PC_RefreshInd(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const SMS_Test_copy_ste_cat_pc_refresh_ind_t * const RefreshIndData_p);

/********************************************************************/
/**
 *
 * @function     Request_SMS_Test_CAT_Cause_PC_RefreshFileInd
 *
 * @description
 * This is a test harness function which is used to simulate a CAT
 * Refresh. It causes a signal to be sent to the SMS Process which is
 * then handled as if it had come from the UICC CAT Server callback.
 *
 * This function simulates the CAT_CAUSE_PC_REFRESH_FILE_IND.
 *
 * This function is needed because it is not possible to stub out the
 * UICC CAT server which is needed for other test cases.
 *
 * There is no response signal.
 *
 * @param [in] RequestCtrl_p    Contains information to send data to
 *                              the SMS Server.
 * @param [in] RefreshFileIndData_p CAT Refresh data to send to SMS Server.
 *
 * @return     SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t Request_SMS_Test_CAT_Cause_PC_RefreshFileInd(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const SMS_Test_copy_ste_cat_pc_refresh_file_ind_t * const RefreshFileIndData_p);

#endif                          // SMS_TEST_HARNESS_CAT_REFRESH_SIMULATION_ENABLED

#endif                          // INCLUSION_GUARD_TEST_CASES_UTILS_H
