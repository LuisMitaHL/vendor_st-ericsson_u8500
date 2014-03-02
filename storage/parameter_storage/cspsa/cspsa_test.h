/* **************************************************************************
 *
 * cspsa_test.h
 *
 * Copyright (C) 2010 ST-Ericsson SA
 *
 * This software is released either under the terms of the BSD-style
 * license accompanying CSPSA or a proprietary license obtained from
 * ST-Ericsson SA.
 *
 * Author: 2010, Martin Lundholm <martin.xa.lundholm@stericsson.com>
 *
 * DESCRIPTION:
 * Cross Platform Cross File System Test Implementation. See CSPSA_TEST_TC_Table
 * for brief test case descriptions. See also file "cspsa_ts.html".
 *
 * Note: TC = Test Case.
 *
 **************************************************************************** */

// =============================================================================
#ifndef INCLUSION_GUARD_CSPSA_TEST_H
#define INCLUSION_GUARD_CSPSA_TEST_H
// =============================================================================

// =============================================================================
//  Configurations
// =============================================================================

// - CFG_CSPSA_TEST_MEMORY_TYPE: Configures which memory type to use for tests.
//   See CSPSA_MemoryType_t in 'cspsa.h'.
#ifndef CFG_CSPSA_TEST_MEMORY_TYPE
#define CFG_CSPSA_TEST_MEMORY_TYPE       T_CSPSA_MEMORY_TYPE_UNDEFINED
#endif

// -----------------------------------------------------------------------------

// - Available test targets:
#define CFG_CSPSA_TEST_TARGET_LINUX_NOT_SET 0
#define CFG_CSPSA_TEST_TARGET_LINUX_US      1
#define CFG_CSPSA_TEST_TARGET_LINUX_BOOT    2
#define CFG_CSPSA_TEST_TARGET_OS_FREE       3

// - Available test variants:
#define CFG_CSPSA_TEST_VARIANT_NOT_SET      0
#define CFG_CSPSA_TEST_VARIANT_API          1
#define CFG_CSPSA_TEST_VARIANT_CORE         2

// - CFG_CSPSA_TEST_TARGET: Sets test target.
#ifndef CFG_CSPSA_TEST_TARGET
#define CFG_CSPSA_TEST_TARGET       CFG_CSPSA_TEST_TARGET_LINUX_NOT_SET
#endif

// - CFG_CSPSA_TEST_VARIANT: Sets test variant.
#ifndef CFG_CSPSA_TEST_VARIANT
#define CFG_CSPSA_TEST_VARIANT      CFG_CSPSA_TEST_VARIANT_NOT_SET
#endif

#if (CFG_CSPSA_TEST_TARGET == CFG_CSPSA_TEST_TARGET_LINUX_NOT_SET)
#error Set CFG_CSPSA_TEST_TARGET to a valid value!
#endif

#if (CFG_CSPSA_TEST_VARIANT == CFG_CSPSA_TEST_VARIANT_NOT_SET)
#error Set CFG_CSPSA_TEST_VARIANT_NOT_SET to a valid value!
#endif

// =============================================================================
//  Include Header Files
// =============================================================================

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// =============================================================================
//  Defines
// =============================================================================

// =============================================================================
//  Macros
// =============================================================================

// - M_CSPSA_TEST_TC_ENTRY: Macro for a test case entry in the test case table.
#define M_CSPSA_TEST_TC_ENTRY(Function, TcDescr) {Function, #Function, 0, T_CSPSA_TEST_TC_STATUS_UNDEFINED, TcDescr, {NULL}}

// =============================================================================
//  Types
// =============================================================================

// - CSPSA_TEST_TEST_TcStatus_t: A test case may be either "valid", "system",
//   "not implemented", not applicable  or "obsolete". If the test cases is
//   "system" a negative result does not mean a failure. It just means that the
//   test case did not apply.
typedef enum
{
  T_CSPSA_TEST_TC_STATUS_VALID = 0,
  T_CSPSA_TEST_TC_STATUS_SYSTEM,
  T_CSPSA_TEST_TC_STATUS_NOT_IMPLEMENTED,
  T_CSPSA_TEST_TC_STATUS_OBSOLETE,
  T_CSPSA_TEST_TC_STATUS_NOT_APPLICABLE,
  // - - -
  T_CSPSA_TEST_TC_STATUS_NUMBER_OF_ITEMS,
  T_CSPSA_TEST_TC_STATUS_UNDEFINED
} CSPSA_TEST_TEST_TcStatus_t;

// -----------------------------------------------------------------------------

/**
 * Type of media error (used in test cases that test media errors).
 *
 * @param T_CSPSA_MEDIA_ERROR_READ_FAILED Read operation generates error value
 * @param T_CSPSA_MEDIA_ERROR_WRITE_FAILED Write operation generates error value
 * @param T_CSPSA_MEDIA_ERROR_READ_CORRUPT_DATA Read operation generates "OK", but a byte is corrupt
 * @param T_CSPSA_MEDIA_ERROR_WRITE_CORRUPT_DATA Write operation generates "OK", but a byte is corrupt
 */
typedef enum
{
  T_CSPSA_MEDIA_ERROR_READ_FAILED,
  T_CSPSA_MEDIA_ERROR_WRITE_FAILED,
  T_CSPSA_MEDIA_ERROR_READ_CORRUPT_DATA,
  T_CSPSA_MEDIA_ERROR_WRITE_CORRUPT_DATA,
} CSPSA_TEST_MediaError_t;

// -----------------------------------------------------------------------------

/**
 * Command to induce a media error (used in test cases that test media errors).
 *
 * @param MediaError The type of media error to induce
 * @param Offset     The offset at which the media error must occur
 * @param AtBothEnds If true, the media error will occur both at Offset bytes from the
 *                   start and at the end of the storage area.
 * @param XorMask    The xor-mask that will be applied (only applicable in ...CORRUPT_DATA media errors)
 */
typedef struct
{
  CSPSA_TEST_MediaError_t MediaError;
  uint64_t Offset;
  bool AtBothEnds;
  int XorMask;
} CSPSA_TEST_InducedError_t;

// -----------------------------------------------------------------------------

// - CSPSA_TEST_TC_Function_t: Defines a test case function.
typedef void (*CSPSA_TEST_TC_Function_t) (uint32_t TestCaseTableIndex);

// -----------------------------------------------------------------------------

// - CSPSA_TEST_TC_ResultStringListObject_t: Defines a TC result list of strings.
typedef struct CSPSA_TEST_TC_ResultStringList_tt
{
  char* TC_ResultString_p;
  struct CSPSA_TEST_TC_ResultStringList_tt* next_p;
} CSPSA_TEST_TC_ResultStringListObject_t;
typedef CSPSA_TEST_TC_ResultStringListObject_t* CSPSA_TEST_TC_ResultStringListObjectP_t;
typedef CSPSA_TEST_TC_ResultStringListObjectP_t* CSPSA_TEST_TC_ResultStringListObjectPP_t;

// -----------------------------------------------------------------------------

// - CSPSA_TEST_TC_Result_t: Defines the test case result.
typedef struct
{
  CSPSA_TEST_TC_ResultStringListObject_t* TestCaseResultStringList;
} CSPSA_TEST_TC_Result_t;

// -----------------------------------------------------------------------------

// - CSPSA_TEST_TC_t: Struct representing one test case entry in the the test case table.
typedef struct
{
  CSPSA_TEST_TC_Function_t 		TestCaseFunction_p;     // - Function pointer to C impl of TC.
  char*                         	TestCaseName_p;         // - pFunction as a text-string.
  bool                          	TestOk;                 // - Determines if test is ok.
  CSPSA_TEST_TEST_TcStatus_t	TcStatus;               // - Determines the status of the test case.
  char*                         	TestCaseDescription_p;  // - Brief description of test case.
  CSPSA_TEST_TC_Result_t   		TestCaseResult;         // - Test case result.
} CSPSA_TEST_TC_t;

// =============================================================================
//  Global Function Declarations
// =============================================================================

// - CSPSA_TEST_Execute: Executes test case table.
int CSPSA_TEST_Execute(void);

// - CSPSA_TEST_GetConfig: Gets Config to use for testing
CSPSA_Config_t* CSPSA_TEST_GetConfig(void);

// - CSPSA_TEST_CanInduceMediaErrors: Checks if the media used during test can be used to induce errors
bool CSPSA_TEST_CanInduceMediaErrors(void);

// - CSPSA_TEST_InduceError: Induces a future media error. Set MediaError_p to NULL to clear error induction.
void CSPSA_TEST_InduceError(CSPSA_TEST_InducedError_t* MediaError_p);

// - CSPSA_TEST_RAM_Close: Closes RAM test.
void CSPSA_TEST_RAM_Close(void);

// =============================================================================
//  Test Case Table
// =============================================================================

// - CFG_MODULE_INSTANTIATE_TEST_CASE_TABLE: may only be defined by 'cspsa_test_ram.c'.
#ifdef CFG_MODULE_INSTANTIATE_TEST_CASE_TABLE

// - List of all test cases.
//   Note: It is possible to just comment out test cases in CSPSA_TEST_TC_Table that
//   one temporarily wants to skip without changing anywhere else.

CSPSA_TEST_TC_t CSPSA_TEST_TC_Table[] =
{
  M_CSPSA_TEST_TC_ENTRY(   CSPSA_TEST_TC_1,  "CSPSA_TC_1: Create a CSPSA image and verify it."),
  M_CSPSA_TEST_TC_ENTRY(   CSPSA_TEST_TC_2,  "CSPSA_TC_2: Delete objects in a CSPSA image and verify it."),
  M_CSPSA_TEST_TC_ENTRY(   CSPSA_TEST_TC_3,  "CSPSA_TC_3: Get error result if a CSPSA image cannot be written."),
  M_CSPSA_TEST_TC_ENTRY(   CSPSA_TEST_TC_4,  "CSPSA_TC_4: Manipulate one bit in the CSPSA data field and make sure CSPSA detects it."),
  M_CSPSA_TEST_TC_ENTRY(   CSPSA_TEST_TC_5,  "CSPSA_TC_5: Manipulate one bit in one header's CSPSA header fields (one field at a time) and make sure CSPSA detects it."),
  M_CSPSA_TEST_TC_ENTRY(   CSPSA_TEST_TC_6,  "CSPSA_TC_6: Manipulate one bit in both header's CSPSA header fields (one field at a time) and make sure CSPSA fails to open."),
  M_CSPSA_TEST_TC_ENTRY(   CSPSA_TEST_TC_7,  "CSPSA_TC_7: Test all API functions with bad input parameters."),
  M_CSPSA_TEST_TC_ENTRY(   CSPSA_TEST_TC_8,  "CSPSA_TC_8: Increase and decrease the size of a CSPSA image significantly and verify it."),
  M_CSPSA_TEST_TC_ENTRY(   CSPSA_TEST_TC_9,  "CSPSA_TC_9: Increase and decrease the existing parameters small amounts."),
  M_CSPSA_TEST_TC_ENTRY(   CSPSA_TEST_TC_10, "CSPSA_TC_10: Multiple clients test."),
  M_CSPSA_TEST_TC_ENTRY(   CSPSA_TEST_TC_11, "CSPSA_TC_11: Performance test (24 B x 4096 items)."),
  M_CSPSA_TEST_TC_ENTRY(   CSPSA_TEST_TC_12, "CSPSA_TC_12: Make sure CSPSA can be opened when there are two valid headers."),
  M_CSPSA_TEST_TC_ENTRY(   CSPSA_TEST_TC_13, "CSPSA_TC_13: Performance test of flash media."),
  M_CSPSA_TEST_TC_ENTRY(   CSPSA_TEST_TC_14, "CSPSA_TC_14: Create a CSPSA image and add some values to it."),
  M_CSPSA_TEST_TC_ENTRY(   CSPSA_TEST_TC_15, "CSPSA_TC_15: Randomly inserts/deletes values."),
  M_CSPSA_TEST_TC_ENTRY(   CSPSA_TEST_TC_16, "CSPSA_TC_16: Checks if induced read errors are detected."),
  M_CSPSA_TEST_TC_ENTRY(   CSPSA_TEST_TC_17, "CSPSA_TC_17: Check that image is stored alternating at beginning and end."),
  M_CSPSA_TEST_TC_ENTRY(   CSPSA_TEST_TC_18, "CSPSA_TC_18: Check flushing/opening when keys are stored at end of a block."),
  M_CSPSA_TEST_TC_ENTRY(   CSPSA_TEST_TC_19, "CSPSA_TC_19: Check fast parameters."),
};

#endif // - CFG_MODULE_INSTANTIATE_TEST_CASE_TABLE

// =============================================================================
#endif // INCLUSION_GUARD_CSPSA_TEST_H
// =============================================================================
