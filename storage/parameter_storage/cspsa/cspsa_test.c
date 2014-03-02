/* **************************************************************************
 *
 * cspsa_test_ram.c
 *
 * Copyright (C) 2009 ST-Ericsson SA
 *
 * This software is released either under the terms of the BSD-style
 * license accompanying CSPSA or a proprietary license obtained from
 * ST-Ericsson SA.
 *
 * Author: 2010, Martin Lundholm <martin.xa.lundholm@stericsson.com>
 *
 * DESCRIPTION:
 * Crash Safe Parameter Storage Area (CSPSA) test implementation for RAM.
 * See file "cspsa_ts.html".
 *
 * Note: TC = Test Case.
 *
 **************************************************************************** */

// =============================================================================
//  Configurations
// =============================================================================

// - CFG_CSPSA_TEST_DURATION_TEST_ITERATIONS: Sets the number of iterations used
//   in duration tests.
#ifndef CFG_CSPSA_TEST_DURATION_TEST_ITERATIONS
#define CFG_CSPSA_TEST_DURATION_TEST_ITERATIONS       1
#endif

// - CFG_CSPSA_TEST_AREA_SIZE: Sets the size of CSPSA.
#ifndef CFG_CSPSA_TEST_AREA_SIZE
#define CFG_CSPSA_TEST_AREA_SIZE              (512*1024)
#endif

// - CFG_CSPSA_LBP: Builds is for Linux Base Platform.
#ifndef CFG_CSPSA_LBP
#define CFG_CSPSA_LBP                         0
#endif

// - CFG_CSPSA_ANDROID: Builds is for Android.
#ifndef CFG_CSPSA_ANDROID
#define CFG_CSPSA_ANDROID                     0
#endif

// =============================================================================
//  Include Header Files
// =============================================================================

#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <errno.h>
#include <sys/time.h>
#include <time.h>
#include "cspsa.h"
#include "cspsa_core.h"
#include "cspsa_ll.h"
#include "crc32.h"
#include "cspsa_plugin.h"

#if (CFG_CSPSA_TEST_TARGET != CFG_CSPSA_TEST_TARGET_OS_FREE)
#include <pthread.h>
#include "cspsa_ll_ram.h"
#include "cspsa_ll_dev.h"
#include "cspsa_api_linux_socket_server.h"
#endif

// =============================================================================
//  Global Test Case Function Declarations
// =============================================================================

// - Test cases. See CSPSA_TEST_TC_Table in 'cspsa_test_ram.h'.
extern void CSPSA_TEST_TC_1(const uint32_t TestCaseTableIndex);
extern void CSPSA_TEST_TC_2(const uint32_t TestCaseTableIndex);
extern void CSPSA_TEST_TC_3(const uint32_t TestCaseTableIndex);
extern void CSPSA_TEST_TC_4(const uint32_t TestCaseTableIndex);
extern void CSPSA_TEST_TC_5(const uint32_t TestCaseTableIndex);
extern void CSPSA_TEST_TC_6(const uint32_t TestCaseTableIndex);
extern void CSPSA_TEST_TC_7(const uint32_t TestCaseTableIndex);
extern void CSPSA_TEST_TC_8(const uint32_t TestCaseTableIndex);
extern void CSPSA_TEST_TC_9(const uint32_t TestCaseTableIndex);
extern void CSPSA_TEST_TC_10(const uint32_t TestCaseTableIndex);
extern void CSPSA_TEST_TC_11(const uint32_t TestCaseTableIndex);
extern void CSPSA_TEST_TC_12(const uint32_t TestCaseTableIndex);
extern void CSPSA_TEST_TC_13(const uint32_t TestCaseTableIndex);
extern void CSPSA_TEST_TC_14(const uint32_t TestCaseTableIndex);
extern void CSPSA_TEST_TC_15(const uint32_t TestCaseTableIndex);
extern void CSPSA_TEST_TC_16(const uint32_t TestCaseTableIndex);
extern void CSPSA_TEST_TC_17(const uint32_t TestCaseTableIndex);
extern void CSPSA_TEST_TC_18(const uint32_t TestCaseTableIndex);
extern void CSPSA_TEST_TC_19(const uint32_t TestCaseTableIndex);

// -----------------------------------------------------------------------------

#define CFG_MODULE_INSTANTIATE_TEST_CASE_TABLE
#include "cspsa_test.h"
#undef CFG_MODULE_INSTANTIATE_TEST_CASE_TABLE

// -----------------------------------------------------------------------------

// - These include files depends on settings in "cspsa_test.h":
#if (CFG_CSPSA_TEST_TARGET != CFG_CSPSA_TEST_TARGET_OS_FREE)
#include <pthread.h>
#include "cspsa_ll_ram.h"
#include "cspsa_ll_dev.h"
#include "cspsa_api_linux_socket_server.h"
#endif

// =============================================================================
//  Defines
// =============================================================================

#if (CFG_CSPSA_TEST_TARGET == CFG_CSPSA_TEST_TARGET_OS_FREE)
// - In OS free, main is somewhere else, and must run with external test driver.
#define CFG_CSPSA_TEST_NO_MAIN
#define CFG_CSPSA_TEST_EXTERNAL_CONFIG
#define CFG_CSPSA_TEST_NO_TIMEOFDAY
#endif

// - D_CSPSA_TEST_MODULE_HEADER: Printout header.
#define D_CSPSA_TEST_MODULE_HEADER                  "\n[CSPSA]: "
// - An Excel row: [EXCEL,<Sheet name>,<TEXT|NUMBER>]:
#define D_CSPSA_TEST_EXCEL_TC_SHEET                 "\n[EXCEL,%s,%s]: "
#define D_CSPSA_TEST_EXCEL_INFO_SHEET               "\n[EXCEL,CSPSA_TEST_TC_INFO,TEXT]: "
#define D_CSPSA_TEST_EXCEL_TC_TABLE_SHEET           "\n[EXCEL,CSPSA_TEST_TC_TABLE,TEXT]: "
#define D_CSPSA_TEST_EXCEL_ERROR                    "\n[EXCEL,CSPSA_ERROR,TEXT]: "

// - D_CSPSA_TEST_TEST_DATA_SIZE: Size of test data buffer. Must not be smaller than 4kB.
#define D_CSPSA_TEST_TEST_DATA_SIZE                 	(16*1024)

#if  (CFG_CSPSA_TEST_TARGET == CFG_CSPSA_TEST_TARGET_LINUX_US) && (CFG_CSPSA_TEST_VARIANT == CFG_CSPSA_TEST_VARIANT_API)

// - D_CSPSA_TEST_TEST_TC10_NBR_OF_THREADS: Number of read threads in TC 10.
#define D_CSPSA_TEST_TEST_TC_10_NBR_OF_READ_THREADS 	(2)

// - D_CSPSA_TEST_TEST_TC_10_NBR_OF_WRITE_THREADS: Number of write threads in TC 10.
#define D_CSPSA_TEST_TEST_TC_10_NBR_OF_WRITE_THREADS	(2)

#endif

// - D_CSPSA_TEST_TEST_TC_10_DURATION: Duration for threads to run in TC 10.
#define D_CSPSA_TEST_TEST_TC_10_DURATION 				(30)

// - D_CSPSA_TEST_MAX_SIMPLE_LIST_SIZE: Max allowed nr elements in simple list.
#define D_CSPSA_TEST_MAX_SIMPLE_LIST_SIZE               (100)

// =============================================================================
//  Macros
// =============================================================================

// - M_CSPSA_TEST_COMPILE_TIME_ASSERT: Compile time assert.
#define M_CSPSA_TEST_COMPILE_TIME_ASSERT(expr) (void)(1/(int)(expr))

// -----------------------------------------------------------------------------

// - M_CSPSA_TEST_ASSERT: Run time assert for test cases.
#define M_CSPSA_TEST_ASSERT(_result_,_line_,_value_) \
  do { \
    if (!(_result_)) { \
      F_CSPSA_TEST_AppendTestCaseResult(TestCaseTableIndex, "Internal Error."); \
      F_CSPSA_TEST_PrintString(V_CSPSA_TEST_TempString_p); \
      F_CSPSA_TEST_ResetString(V_CSPSA_TEST_TempString_p); \
      sprintf(V_CSPSA_TEST_TempString_p, C_CSPSA_TEST_ERROR_STRING, _line_, _value_); \
      F_CSPSA_TEST_AppendTestCaseResult(TestCaseTableIndex, V_CSPSA_TEST_TempString_p); \
      F_CSPSA_TEST_AssertFailed(); \
      F_CSPSA_TEST_ResetString(V_CSPSA_TEST_TempString_p); \
    } \
  } while (false)

// -----------------------------------------------------------------------------

// - M_CSPSA_TEST_ASSERT_ERROR_LINE: Run time assert that stores error line in V_CSPSA_TEST_ErrorLine.
#define M_CSPSA_TEST_ASSERT_ERROR_LINE(n) \
  do {if (!(n)) { V_CSPSA_TEST_ErrorLine = __LINE__; F_CSPSA_TEST_AssertFailed(); goto ErrorExit; }} while (false)

// -----------------------------------------------------------------------------

// - M_CSPSA_TEST_ALIGN32: Aligns a value to be a 32 bit value.
#define M_CSPSA_TEST_ALIGN32(s) (((s) + 3) & ~3)

// =============================================================================
//  Local Types
// =============================================================================

// - CSPSA_TEST_DataItem_t: The structure of a data item.
typedef struct
{
  CSPSA_Key_t Key;
  CSPSA_Size_t Size;
  uint8_t Data_p[];
} CSPSA_TEST_DataItem_t;

// -----------------------------------------------------------------------------

// - CSPSA_TEST_HeaderFlags_t: Enumeration of possible CSPSA flags.
typedef enum
{
  T_CSPSA_TEST_HEADERFLAGS_DATA_AT_END   = (1 << 0),
  // - - -
  T_CSPSA_TEST_HEADERFLAGS_END_MARK
} CSPSA_TEST_HeaderFlags_t;

// -----------------------------------------------------------------------------

// - CSPSA_TEST_TestType_t: Enumeration of possible key/size test combinations.
typedef enum
{
  T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_CONST = 0,
  T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_INC,
  T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_DEC,
  T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_RND,
  T_CSPSA_TEST_TEST_TYPE_KEY_DEC_SIZE_INC,
  T_CSPSA_TEST_TEST_TYPE_KEY_DEC_SIZE_DEC,
  T_CSPSA_TEST_TEST_TYPE_KEY_DEC_SIZE_RND,
  T_CSPSA_TEST_TEST_TYPE_KEY_RND_SIZE_INC,
  T_CSPSA_TEST_TEST_TYPE_KEY_RND_SIZE_DEC,
  T_CSPSA_TEST_TEST_TYPE_KEY_RND_SIZE_RND,
  // - - -
  T_CSPSA_TEST_TEST_TYPE_NBR_OF_ITEMS,
  T_CSPSA_TEST_TEST_TYPE_UNDEFINED,
} CSPSA_TEST_TestType_t;

// -----------------------------------------------------------------------------

// - CSPSA_TEST_FUNC_t: Enumeration of CSPSA functions (for time measurement).
typedef enum
{
  T_CSPSA_TEST_FUNC_OPEN,
  T_CSPSA_TEST_FUNC_FLUSH,
  T_CSPSA_TEST_FUNC_CLOSE,
  T_CSPSA_TEST_FUNC_WRITE_1ST,
  T_CSPSA_TEST_FUNC_WRITE_2ND,
  T_CSPSA_TEST_FUNC_READ,
  T_CSPSA_TEST_FUNC_FORMAT,
  T_CSPSA_TEST_FUNC_CREATE,
  T_CSPSA_TEST_FUNC_DELETE,
  // - - -
  T_CSPSA_TEST_FUNC_NBR_OF_ITEMS,
  T_CSPSA_TEST_FUNC_UNDEFINED,
} CSPSA_TEST_FUNC_t;

// -----------------------------------------------------------------------------

// - CSPSA_TEST_ThreadArg_t: Structure of thread argument.
typedef struct
{
  CSPSA_Handle_t Handle;
  CSPSA_TEST_TestType_t TestType;
  uint32_t TotalTestDataSize;
  uint32_t DurationInS;
  uint32_t Offset;
  uint32_t ThreadIndex;
  uint32_t Sleep;
} CSPSA_TEST_ThreadArg_t;

// -----------------------------------------------------------------------------

// - CSPSA_TEST_KeyValuePair_t: Key/value type used in simple list.
typedef struct
{
  CSPSA_Key_t Key;
  int Value;
} CSPSA_TEST_KeyValuePair_t;

// =============================================================================
//  Local Constants
// =============================================================================

// - C_CSPSA_TEST_NumberOfTestCases: Determines the total number of testcases.
static const uint32_t C_CSPSA_TEST_NumberOfTestCases = sizeof(CSPSA_TEST_TC_Table) / sizeof(CSPSA_TEST_TC_t);

// - C_CSPSA_TEST_ERROR_STRING: Defines a commmon error string for internal errors.
static const char C_CSPSA_TEST_ERROR_STRING[] = D_CSPSA_TEST_EXCEL_ERROR "Internal error at line (%u);Value: %u.\n";

// - C_CSPSA_TEST_SYSTEM_TEST_YES_STRING: Common 'Yes.' string.
static const char C_CSPSA_TEST_SYSTEM_TEST_YES_STRING[] = "Yes.";

// - C_CSPSA_TEST_SYSTEM_TEST_NO_STRING: Common 'No.' string.
static const char C_CSPSA_TEST_SYSTEM_TEST_NO_STRING[] = "No.";

// - C_CSPSA_TEST_HeaderMagic: Magic header value.
static const uint32_t C_CSPSA_TEST_HeaderMagic = 0x00415350;

// =============================================================================
//  Local Variables
// =============================================================================

// - V_CSPSA_TEST_TempString_p: General purpose string.
static char V_CSPSA_TEST_TempString_p[10*1024];

// - V_CSPSA_TEST_ReadData_p: Common read data buffer.
static uint8_t V_CSPSA_TEST_ReadData_p[D_CSPSA_TEST_TEST_DATA_SIZE];
// - V_CSPSA_TEST_WriteData_p: Common write data buffer.
static uint8_t V_CSPSA_TEST_WriteData_p[D_CSPSA_TEST_TEST_DATA_SIZE];

// - V_CSPSA_TEST_ErrorLine: Indicates on which line in this file an error occurred.
static uint32_t V_CSPSA_TEST_ErrorLine;

// - V_CSPSA_TEST_Config: Memory media configuration.
static CSPSA_Config_t V_CSPSA_TEST_Config;

//- V_CSPSA_TEST_Handle: A CSPSA handle.
static CSPSA_Handle_t V_CSPSA_TEST_Handle;

//- V_CSPSA_TEST_ConstSize: Constant size for test of type
//  T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_CONST.
static uint32_t V_CSPSA_TEST_ConstSize = 1;

//- Common variables for time measurment.
static struct timeval V_CSPSA_TEST_StartTime;
static struct timeval V_CSPSA_TEST_ElapsedTime;
static struct timeval V_CSPSA_TEST_TotalStartTime;
static struct timeval V_CSPSA_TEST_TotalElapsedTime;

// - TimeMeasurement: Structure for function call duration measurement.
static struct timeval V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_NBR_OF_ITEMS];

//- V_CSPSA_TEST_TC10_DataCounter_p: Assisting TC10 to count how much data
//  has been transferred.
static uint32_t V_CSPSA_TEST_TC10_DataCounter_p[5];

// - V_CSPSA_TEST_SessionInfo: Local session info struct to be able to call
//   CSPSA_LL_Open() without opening a real session.
static CSPSA_SessionInfo_t V_CSPSA_TEST_SessionInfo;

// - V_CSPSA_TEST_Seed1: Seed 1 used in random number generation
static uint32_t V_CSPSA_TEST_Seed1 = 960131;    /* must not be zero */
// - V_CSPSA_TEST_Seed2: Seed 2 used in random number generation
static uint32_t V_CSPSA_TEST_Seed2 = 1000003;    /* must not be zero */

// - V_CSPSA_TEST_SimpleList: Simple key-value list used in some test cases to check
//                            if CSPSA contents are correct
static CSPSA_TEST_KeyValuePair_t V_CSPSA_TEST_SimpleList[D_CSPSA_TEST_MAX_SIMPLE_LIST_SIZE];

// - V_CSPSA_TEST_SimpleListSize: Size of the simple list
static uint32_t V_CSPSA_TEST_SimpleListSize = 0;

// =============================================================================
//  Global External Variables
// =============================================================================

// =============================================================================
//  Local Function Declarations
// =============================================================================

static void F_CSPSA_TEST_AssertFailed(void);
static void F_CSPSA_TEST_AppendTestCaseResult(const uint32_t TestCaseTableIndex, const char* const NewTC_ResultString_p);
static void F_CSPSA_TEST_CleanUp(void);
static bool F_CSPSA_TEST_Init(void);
static uint8_t* F_CSPSA_TEST_GetWriteData(void);
static void F_CSPSA_TEST_PrintString(const char* const PrintString_p);
#if 0
static char* F_CSPSA_TEST_PrintTestType(const CSPSA_TEST_TestType_t TestType);
#endif
static char* F_CSPSA_TEST_PrintMemoryType(const CSPSA_MemoryType_t MemoryType);
static bool F_CSPSA_TEST_PrintTestCaseResults(void);
static void F_CSPSA_TEST_ResetString(char* const TempString_p);
static void F_CSPSA_TEST_TC_NotPerformed(const uint32_t TestCaseTableIndex);

static CSPSA_Result_t F_CSPSA_TEST_ReadAndVerify(
  const CSPSA_Handle_t Handle,
  const uint32_t TotalTestDataSize,
  const CSPSA_TEST_TestType_t TestType
);
static CSPSA_Result_t F_CSPSA_TEST_InitFlashMemory(
	CSPSA_Config_t* const Config_p
);
static CSPSA_Result_t F_CSPSA_TEST_UnInitFlashMemory(
	CSPSA_Config_t* const Config_p
);
#if 0
static CSPSA_Result_t F_CSPSA_TEST_VerifyRamImage(
  const CSPSA_Handle_t Handle,
  const uint32_t TotalTestDataSize,
  const CSPSA_TEST_TestType_t TestType
);
#endif
static CSPSA_Result_t F_CSPSA_TEST_WriteValues(
  const CSPSA_Handle_t Handle,
  const uint32_t TotalTestDataSize,
  const CSPSA_TEST_TestType_t TestType
);
static CSPSA_Result_t F_CSPSA_TEST_GetInitalParameterValues(
  const CSPSA_Handle_t Handle,
  const CSPSA_TEST_TestType_t TestType,
  const uint32_t TotalTestDataSize,
  uint32_t* const Key_p,
  uint32_t* const Size_p
);
static CSPSA_Result_t F_CSPSA_TEST_GetNextParameterValues(
  const CSPSA_Handle_t Handle,
  const CSPSA_TEST_TestType_t TestType,
  const uint32_t Index,
  uint32_t* const Key_p,
  uint32_t* const Size_p
);

#if D_CSPSA_TEST_TEST_TC_10_NBR_OF_READ_THREADS
static void* F_CSPSA_TEST_ReadThread(void *arg);
#endif
#if D_CSPSA_TEST_TEST_TC_10_NBR_OF_WRITE_THREADS
static void* F_CSPSA_TEST_WriteThread(void *arg);
#endif

static void F_CSPSA_TEST_TimeAccumulate(struct timeval *AccumulatedTime, struct timeval *NewTime);
static int F_CSPSA_TEST_TimeElapsed(struct timeval *start, struct timeval *elapsed);
#if 1
static uint64_t F_CSPSA_TEST_TimeInUs(struct timeval *TimeValue);
#endif
static uint64_t F_CSPSA_TEST_TimeInMs(struct timeval *TimeValue);
static int F_CSPSA_TEST_TimeStart(struct timeval *start);

// =============================================================================
//  External Declarations
// =============================================================================

// =============================================================================
//  Local Function Definitions
// =============================================================================

// - F_CSPSA_TEST_AssertFailed: Called when an assert fails (it can be useful to set a breakpoint at this function when debugging).

static void F_CSPSA_TEST_AssertFailed(void)
{
}

// - F_CSPSA_TEST_AppendTestCaseResult: Appends a test case result string to the test case.

static void F_CSPSA_TEST_AppendTestCaseResult(const uint32_t TestCaseTableIndex, const char* const NewTC_ResultString_p)
{
  CSPSA_TEST_TC_t* CurrentTestCase_p = &(CSPSA_TEST_TC_Table[TestCaseTableIndex]);
  CSPSA_TEST_TC_ResultStringListObject_t* TestCaseResultStringListObject_p = NULL;
  CSPSA_TEST_TC_ResultStringListObjectPP_t TestCaseResultStringListObject_pp = &(CurrentTestCase_p->TestCaseResult.TestCaseResultStringList);
  CSPSA_TEST_TC_ResultStringListObject_t* TempTestCaseResultStringListObject_p = *TestCaseResultStringListObject_pp;

  TestCaseResultStringListObject_p = (CSPSA_TEST_TC_ResultStringListObject_t*) malloc(sizeof(CSPSA_TEST_TC_ResultStringListObject_t));
  TestCaseResultStringListObject_p->next_p = NULL;

  TestCaseResultStringListObject_p->TC_ResultString_p = (char*) malloc(strlen(NewTC_ResultString_p)+2);
  sprintf(TestCaseResultStringListObject_p->TC_ResultString_p, "%s\n", NewTC_ResultString_p);

  if (*TestCaseResultStringListObject_pp == NULL)
  {
    // - This is the first string.
    *TestCaseResultStringListObject_pp = TestCaseResultStringListObject_p;
  }
  else
  {
    // - Find the last string, and append the string.
    while (TempTestCaseResultStringListObject_p->next_p != NULL)
    {
      TempTestCaseResultStringListObject_p = TempTestCaseResultStringListObject_p->next_p;
    }
    TempTestCaseResultStringListObject_p->next_p = TestCaseResultStringListObject_p;
  }

}

// -----------------------------------------------------------------------------

// - F_CSPSA_TEST_CleanUp: Cleans up after a test case run.

static void F_CSPSA_TEST_CleanUp()
{
  uint32_t TestCaseTableIndex;

  for (TestCaseTableIndex = 0; TestCaseTableIndex < C_CSPSA_TEST_NumberOfTestCases; TestCaseTableIndex++)
  {
    CSPSA_TEST_TC_t* CurrentTestCase_p = &(CSPSA_TEST_TC_Table[TestCaseTableIndex]);
    CSPSA_TEST_TC_ResultStringListObjectPP_t TestCaseResultStringListObject_pp = &(CurrentTestCase_p->TestCaseResult.TestCaseResultStringList);
    CSPSA_TEST_TC_ResultStringListObject_t* TempTestCaseResultStringListObject_p = *TestCaseResultStringListObject_pp;
    CSPSA_TEST_TC_ResultStringListObjectP_t NextTestCaseResultStringListObject_p;
    CurrentTestCase_p->TcStatus = T_CSPSA_TEST_TC_STATUS_UNDEFINED;
    CurrentTestCase_p->TestOk = false;

    while (TempTestCaseResultStringListObject_p != NULL)
    {
      NextTestCaseResultStringListObject_p = TempTestCaseResultStringListObject_p->next_p;
      free(TempTestCaseResultStringListObject_p->TC_ResultString_p);
      free(TempTestCaseResultStringListObject_p);
      TempTestCaseResultStringListObject_p = NextTestCaseResultStringListObject_p;
    }
    *TestCaseResultStringListObject_pp = NULL;
  }
}

// -----------------------------------------------------------------------------

// - F_CSPSA_TEST_GetInitalParameterValues: Generates initial key and size values based on TestType.

static CSPSA_Result_t F_CSPSA_TEST_GetInitalParameterValues(
  const CSPSA_Handle_t Handle,
  const CSPSA_TEST_TestType_t TestType,
  const uint32_t TotalTestDataSize,
  uint32_t* const Key_p,
  uint32_t* const Size_p
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_UNDEFINED;

  M_CSPSA_PARAMETER_NOT_USED(Handle);

  switch (TestType)
  {
    case T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_CONST: *Key_p = 1;                             *Size_p = V_CSPSA_TEST_ConstSize; break;
    case T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_INC:   *Key_p = 1;                             *Size_p = 1;                      break;
    case T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_DEC:   *Key_p = 1;                             *Size_p = TotalTestDataSize / 6;  break;
    case T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_RND:   *Key_p = 1;                             *Size_p = TotalTestDataSize / 6;  break;
    case T_CSPSA_TEST_TEST_TYPE_KEY_DEC_SIZE_INC:   *Key_p = 0x12345678;                    *Size_p = 1;                      break;
    case T_CSPSA_TEST_TEST_TYPE_KEY_DEC_SIZE_DEC:   *Key_p = 0x12345678;                    *Size_p = TotalTestDataSize / 6;  break;
    case T_CSPSA_TEST_TEST_TYPE_KEY_DEC_SIZE_RND:   *Key_p = 0x12345678;                    *Size_p = TotalTestDataSize / 6;  break;
    case T_CSPSA_TEST_TEST_TYPE_KEY_RND_SIZE_INC:   *Key_p = 1000 + TotalTestDataSize / 6;  *Size_p = 1;                      break;
    case T_CSPSA_TEST_TEST_TYPE_KEY_RND_SIZE_DEC:   *Key_p = 1000 + TotalTestDataSize / 6;  *Size_p = TotalTestDataSize / 6;  break;
    case T_CSPSA_TEST_TEST_TYPE_KEY_RND_SIZE_RND:   *Key_p = 1000 + TotalTestDataSize / 6;  *Size_p = TotalTestDataSize / 6;  break;
    default:
      goto ErrorExit;
  }

  Result = T_CSPSA_RESULT_OK;

ErrorExit:

  return Result;
}

// -----------------------------------------------------------------------------

// - F_CSPSA_TEST_GetNextParameterValues: Generates next key and size values based on TestType.

static CSPSA_Result_t F_CSPSA_TEST_GetNextParameterValues(
  const CSPSA_Handle_t Handle,
  const CSPSA_TEST_TestType_t TestType,
  const uint32_t Index,
  uint32_t* const Key_p,
  uint32_t* const Size_p
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_UNDEFINED;
  int32_t RndIndex = ((Index & 1) > 0) ? -Index : Index ;

  M_CSPSA_PARAMETER_NOT_USED(Handle);

  switch (TestType)
  {
    case T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_CONST: *Key_p += 1;                        *Size_p = V_CSPSA_TEST_ConstSize; break;
    case T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_INC:   *Key_p += 1;                        *Size_p += 1;                     break;
    case T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_DEC:   *Key_p += 1;                        *Size_p -= 1;                     break;
    case T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_RND:   *Key_p += 1;                        *Size_p = *Size_p * 3 - RndIndex; break;
    case T_CSPSA_TEST_TEST_TYPE_KEY_DEC_SIZE_INC:   *Key_p -= 1;                        *Size_p += 1;                     break;
    case T_CSPSA_TEST_TEST_TYPE_KEY_DEC_SIZE_DEC:   *Key_p -= 1;                        *Size_p -= 1;                     break;
    case T_CSPSA_TEST_TEST_TYPE_KEY_DEC_SIZE_RND:   *Key_p -= 1;                        *Size_p = *Size_p * 3 - RndIndex; break;
    case T_CSPSA_TEST_TEST_TYPE_KEY_RND_SIZE_INC:   *Key_p += *Key_p * RndIndex + Index; *Size_p += 1;                     break;
    case T_CSPSA_TEST_TEST_TYPE_KEY_RND_SIZE_DEC:   *Key_p += *Key_p * RndIndex + Index; *Size_p -= 1;                     break;
    case T_CSPSA_TEST_TEST_TYPE_KEY_RND_SIZE_RND:   *Key_p += *Key_p * RndIndex + Index; *Size_p = *Size_p * 3 - RndIndex; break;
    default:
      goto ErrorExit;
  }

  if (*Size_p <= 0)
  {
    *Size_p = 1;
  }

  Result = T_CSPSA_RESULT_OK;

ErrorExit:

  return Result;
}

// -----------------------------------------------------------------------------

// - F_CSPSA_TEST_TC_NotPerformed: If the test cases is not executed, this
//   function is called instead.

static void F_CSPSA_TEST_TC_NotPerformed(const uint32_t TestCaseTableIndex)
{
  CSPSA_TEST_TC_t* CurrentTestCase_p = &(CSPSA_TEST_TC_Table[TestCaseTableIndex]);

  switch (CurrentTestCase_p->TcStatus)
  {
    case T_CSPSA_TEST_TC_STATUS_SYSTEM:
      if (CurrentTestCase_p->TestOk)
      {
        F_CSPSA_TEST_AppendTestCaseResult(TestCaseTableIndex, C_CSPSA_TEST_SYSTEM_TEST_YES_STRING);
      }
      else
      {
        F_CSPSA_TEST_AppendTestCaseResult(TestCaseTableIndex, C_CSPSA_TEST_SYSTEM_TEST_NO_STRING);
      }
      break;
    case T_CSPSA_TEST_TC_STATUS_NOT_IMPLEMENTED:
      CurrentTestCase_p->TcStatus = T_CSPSA_TEST_TC_STATUS_NOT_IMPLEMENTED;
      F_CSPSA_TEST_AppendTestCaseResult(TestCaseTableIndex, "Not implemented.");
      break;
    case T_CSPSA_TEST_TC_STATUS_OBSOLETE:
      CurrentTestCase_p->TcStatus = T_CSPSA_TEST_TC_STATUS_OBSOLETE;
      F_CSPSA_TEST_AppendTestCaseResult(TestCaseTableIndex, "Obsolete.");
      break;
    case T_CSPSA_TEST_TC_STATUS_NOT_APPLICABLE:
      CurrentTestCase_p->TcStatus = T_CSPSA_TEST_TC_STATUS_NOT_APPLICABLE;
      F_CSPSA_TEST_AppendTestCaseResult(TestCaseTableIndex, "Not applicable.");
      break;
    default:
      break;
  }
}

// -----------------------------------------------------------------------------

// - F_CSPSA_TEST_ResetString: Resets a string.

static void F_CSPSA_TEST_ResetString(char* const TempString_p)
{
  strcpy(TempString_p, "\0");
}

// -----------------------------------------------------------------------------

#if D_CSPSA_TEST_TEST_TC_10_NBR_OF_READ_THREADS
static void* F_CSPSA_TEST_ReadThread(void *arg)
{
	CSPSA_Result_t CSPSA_Result;
	CSPSA_TEST_ThreadArg_t* ThreadArg_p = (CSPSA_TEST_ThreadArg_t*) arg;
	struct timeval TotalStartTime;
	struct timeval TotalElapsedTime;
	uint64_t TotalElapsedTimeInMs = 0;
  CSPSA_Key_t Key;
  CSPSA_Size_t Size;
  uint32_t Index = 0;

	memset(&TotalElapsedTime, 0, sizeof(TotalElapsedTime));
	F_CSPSA_TEST_TimeStart(&TotalStartTime);
  fprintf(stderr, "F_CSPSA_TEST_ReadThread(%u)\n", __LINE__);

  CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &ThreadArg_p->Handle);
  if (CSPSA_Result != T_CSPSA_RESULT_OK) goto ErrorExit;

  CSPSA_Result = F_CSPSA_TEST_GetInitalParameterValues(ThreadArg_p->Handle, ThreadArg_p->TestType, ThreadArg_p->TotalTestDataSize, &Key, &Size);
  if (CSPSA_Result != T_CSPSA_RESULT_OK) goto ErrorExit;

	while (TotalElapsedTimeInMs < ThreadArg_p->DurationInS * 1000)
	{
    CSPSA_Result = CSPSA_ReadValue(ThreadArg_p->Handle, Key, Size, &V_CSPSA_TEST_ReadData_p[ThreadArg_p->Offset]);
    if (CSPSA_Result >= T_CSPSA_RESULT_ERRORS)
    {
    	Index = 0;
  		CSPSA_Result = F_CSPSA_TEST_GetInitalParameterValues(ThreadArg_p->Handle, ThreadArg_p->TestType, ThreadArg_p->TotalTestDataSize, &Key, &Size);
    }
    else
    {
    	V_CSPSA_TEST_TC10_DataCounter_p[ThreadArg_p->ThreadIndex] += Size;
    	usleep(ThreadArg_p->Sleep);
    }

    CSPSA_Result = F_CSPSA_TEST_GetNextParameterValues(ThreadArg_p->Handle, ThreadArg_p->TestType, Index++, &Key, &Size);
    if (CSPSA_Result != T_CSPSA_RESULT_OK) goto ErrorExit;

	  F_CSPSA_TEST_TimeElapsed(&TotalStartTime, &TotalElapsedTime);
	  TotalElapsedTimeInMs = F_CSPSA_TEST_TimeInMs(&TotalElapsedTime);
	}

ErrorExit:

  fprintf(stderr, "F_CSPSA_TEST_ReadThread(%u)\n", __LINE__);
  if (ThreadArg_p->Handle != 0)
  {
    fprintf(stderr, "F_CSPSA_TEST_ReadThread(%u)\n", __LINE__);
  	CSPSA_Result = CSPSA_Close(&ThreadArg_p->Handle);
  }

  if (CSPSA_Result == T_CSPSA_RESULT_OK)
  {
	  return NULL;
	}
	else
	{
	  return (void*)(-1);
	}
}
#endif // D_CSPSA_TEST_TEST_TC_10_NBR_OF_READ_THREADS

// -----------------------------------------------------------------------------

#if D_CSPSA_TEST_TEST_TC_10_NBR_OF_WRITE_THREADS
static void* F_CSPSA_TEST_WriteThread(void *arg)
{
	CSPSA_Result_t CSPSA_Result;
	CSPSA_TEST_ThreadArg_t* ThreadArg_p = (CSPSA_TEST_ThreadArg_t*) arg;
	struct timeval TotalStartTime;
	struct timeval TotalElapsedTime;
	uint64_t TotalElapsedTimeInMs = 0;
  CSPSA_Key_t Key;
  CSPSA_Size_t Size;
  uint32_t Index = 0;

	memset(&TotalElapsedTime, 0, sizeof(TotalElapsedTime));
	F_CSPSA_TEST_TimeStart(&TotalStartTime);

  CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &ThreadArg_p->Handle);
  if (CSPSA_Result != T_CSPSA_RESULT_OK) goto ErrorExit;

  CSPSA_Result = F_CSPSA_TEST_GetInitalParameterValues(ThreadArg_p->Handle, ThreadArg_p->TestType, ThreadArg_p->TotalTestDataSize, &Key, &Size);
  if (CSPSA_Result != T_CSPSA_RESULT_OK) goto ErrorExit;

	while (TotalElapsedTimeInMs < ThreadArg_p->DurationInS * 1000)
	{
    CSPSA_Result = CSPSA_DeleteValue(ThreadArg_p->Handle, Key);
    if (CSPSA_Result >= T_CSPSA_RESULT_ERRORS)
    {
    	Index = 0;
  		CSPSA_Result = F_CSPSA_TEST_GetInitalParameterValues(ThreadArg_p->Handle, ThreadArg_p->TestType, ThreadArg_p->TotalTestDataSize, &Key, &Size);
    }
    else
    {
    	V_CSPSA_TEST_TC10_DataCounter_p[ThreadArg_p->ThreadIndex] += Size;
    	usleep(ThreadArg_p->Sleep);
    }

    CSPSA_Result = CSPSA_WriteValue(ThreadArg_p->Handle, Key, Size, &V_CSPSA_TEST_WriteData_p[ThreadArg_p->Offset]);
    if (CSPSA_Result >= T_CSPSA_RESULT_ERRORS) goto ErrorExit;

    CSPSA_Result = F_CSPSA_TEST_GetNextParameterValues(ThreadArg_p->Handle, ThreadArg_p->TestType, Index++, &Key, &Size);
    if (CSPSA_Result != T_CSPSA_RESULT_OK) goto ErrorExit;

	  F_CSPSA_TEST_TimeElapsed(&TotalStartTime, &TotalElapsedTime);
	  TotalElapsedTimeInMs = F_CSPSA_TEST_TimeInMs(&TotalElapsedTime);
	}

ErrorExit:

  if (ThreadArg_p->Handle != 0)
  {
  	CSPSA_Result = CSPSA_Close(&ThreadArg_p->Handle);
  }

  if (CSPSA_Result == T_CSPSA_RESULT_OK)
  {
	  return NULL;
	}
	else
	{
	  return (void*)(-1);
	}
}
#endif

// -----------------------------------------------------------------------------

// - F_CSPSA_TEST_TimeAccumulate: Accumulates time.

static void F_CSPSA_TEST_TimeAccumulate(struct timeval *AccumulatedTime, struct timeval *NewTime)
{
  AccumulatedTime->tv_usec += NewTime->tv_usec;
  AccumulatedTime->tv_sec += NewTime->tv_sec;
  while (AccumulatedTime->tv_usec > 1000000LL)
  {
    AccumulatedTime->tv_usec -= 1000000LL;
    AccumulatedTime->tv_sec++;
  }
}

// -----------------------------------------------------------------------------

// - F_CSPSA_TEST_TimeInUs: Returns time in microseconds.

#if 1
static uint64_t F_CSPSA_TEST_TimeInUs(struct timeval *TimeValue)
{
  return TimeValue->tv_sec * 1000000ULL + TimeValue->tv_usec;
}
#endif

// -----------------------------------------------------------------------------

// - F_CSPSA_TEST_TimeInMs: Returns time in milliseconds.

static uint64_t F_CSPSA_TEST_TimeInMs(struct timeval *TimeValue)
{
  return TimeValue->tv_sec * 1000ULL + TimeValue->tv_usec / 1000;
}

// -----------------------------------------------------------------------------

// - F_CSPSA_TEST_TimeStart: Returns start time.

static int F_CSPSA_TEST_TimeStart(struct timeval *start)
{
#ifdef CFG_CSPSA_TEST_NO_TIMEOFDAY
  // - On some OS-free configurations gettimeofday is not available.
  (void)start;
  return 0;
#else
  return gettimeofday(start, NULL);
#endif
}

// -----------------------------------------------------------------------------

// - F_CSPSA_TEST_TimeElapsed: Returns elapsed time.

static int F_CSPSA_TEST_TimeElapsed(struct timeval *start, struct timeval *elapsed)
{
  int Result = F_CSPSA_TEST_TimeStart(elapsed);
  if (Result == 0)
  {
    elapsed->tv_sec -= start->tv_sec;
    if (elapsed->tv_usec < start->tv_usec)
    {
      elapsed->tv_sec--;
      elapsed->tv_usec += 1000000ULL;
    }
    elapsed->tv_usec -= start->tv_usec;
  }
  return Result;
}

// -----------------------------------------------------------------------------

// - F_CSPSA_TEST_PrintString: Wrapper for printing a string.

static void F_CSPSA_TEST_PrintString(const char* const PrintString_p)
{
  printf("%s", PrintString_p);
}

// -----------------------------------------------------------------------------

// - F_CSPSA_TEST_PrintTestCaseResults: Prints all test case results.

static bool F_CSPSA_TEST_PrintTestCaseResults()
{
  uint32_t TestCaseTableIndex;
  bool AllTestCasesOk = true;

  // - Traverse through all test cases and note the overall result.
  for (TestCaseTableIndex = 0; TestCaseTableIndex < C_CSPSA_TEST_NumberOfTestCases; TestCaseTableIndex++)
  {
    CSPSA_TEST_TC_t* CurrentTestCase_p = &(CSPSA_TEST_TC_Table[TestCaseTableIndex]);
    if (CurrentTestCase_p->TcStatus == T_CSPSA_TEST_TC_STATUS_VALID)
    {
      AllTestCasesOk =
        AllTestCasesOk
        &&
        (
          ((CurrentTestCase_p->TcStatus == T_CSPSA_TEST_TC_STATUS_VALID) && CurrentTestCase_p->TestOk)
          ||
          (CurrentTestCase_p->TcStatus == T_CSPSA_TEST_TC_STATUS_SYSTEM)
          ||
          (CurrentTestCase_p->TcStatus == T_CSPSA_TEST_TC_STATUS_NOT_IMPLEMENTED)
          ||
          (CurrentTestCase_p->TcStatus == T_CSPSA_TEST_TC_STATUS_OBSOLETE)
          ||
          (CurrentTestCase_p->TcStatus == T_CSPSA_TEST_TC_STATUS_NOT_APPLICABLE)
        );
    }
  }

  sprintf(V_CSPSA_TEST_TempString_p, D_CSPSA_TEST_EXCEL_INFO_SHEET "Test Case Info\n");
  sprintf(V_CSPSA_TEST_TempString_p+strlen(V_CSPSA_TEST_TempString_p), D_CSPSA_TEST_EXCEL_INFO_SHEET "Storage type;%s\n", F_CSPSA_TEST_PrintMemoryType(V_CSPSA_TEST_Config.MemoryType));
  if (AllTestCasesOk)
  {
    sprintf(V_CSPSA_TEST_TempString_p+strlen(V_CSPSA_TEST_TempString_p), D_CSPSA_TEST_EXCEL_INFO_SHEET "Module test result;Ok.\n");
  }
  else
  {
    sprintf(V_CSPSA_TEST_TempString_p+strlen(V_CSPSA_TEST_TempString_p), D_CSPSA_TEST_EXCEL_INFO_SHEET "Module test result;Failed.\n");
  }
  F_CSPSA_TEST_PrintString(V_CSPSA_TEST_TempString_p);

  sprintf(V_CSPSA_TEST_TempString_p, D_CSPSA_TEST_EXCEL_TC_TABLE_SHEET "Test case id;Test case description;Result");
  {
    if (CFG_CSPSA_TEST_DURATION_TEST_ITERATIONS > 1)
    {
      uint32_t iterations = 1;
      while (iterations <= CFG_CSPSA_TEST_DURATION_TEST_ITERATIONS)
      {
        sprintf(V_CSPSA_TEST_TempString_p+strlen(V_CSPSA_TEST_TempString_p), ";Duration #%u", iterations);
        iterations++;
      }
    }
    else
    {
      sprintf(
        V_CSPSA_TEST_TempString_p+strlen(V_CSPSA_TEST_TempString_p),
        ";Test Duration [ms];Open [ms];Close [ms];Read [ms];Write(empty) [ms];Write(rewrite) [ms];Flush [ms];Format [ms];Create [ms];Delete [ms]\n"
      );
    }
  }
  F_CSPSA_TEST_PrintString(V_CSPSA_TEST_TempString_p);

  for (TestCaseTableIndex = 0; TestCaseTableIndex < C_CSPSA_TEST_NumberOfTestCases; TestCaseTableIndex++)
  {
    CSPSA_TEST_TC_t* CurrentTestCase_p = &(CSPSA_TEST_TC_Table[TestCaseTableIndex]);
    CSPSA_TEST_TC_ResultStringListObjectPP_t TestCaseResultStringListObject_pp = &(CurrentTestCase_p->TestCaseResult.TestCaseResultStringList);
    CSPSA_TEST_TC_ResultStringListObjectP_t TempTestCaseResultStringListObject_p;

    F_CSPSA_TEST_ResetString(V_CSPSA_TEST_TempString_p);
    sprintf(V_CSPSA_TEST_TempString_p, D_CSPSA_TEST_EXCEL_TC_TABLE_SHEET "%s;%s;", CurrentTestCase_p->TestCaseName_p, CurrentTestCase_p->TestCaseDescription_p);

    if (*TestCaseResultStringListObject_pp == NULL)
    {
      printf(D_CSPSA_TEST_MODULE_HEADER "Internal error line %d.\n", __LINE__);
      return false;
    }

    TempTestCaseResultStringListObject_p = *TestCaseResultStringListObject_pp;
    while (TempTestCaseResultStringListObject_p != NULL)
    {
      sprintf(V_CSPSA_TEST_TempString_p+strlen(V_CSPSA_TEST_TempString_p), "%s ", TempTestCaseResultStringListObject_p->TC_ResultString_p);
      TempTestCaseResultStringListObject_p = TempTestCaseResultStringListObject_p->next_p;
    }

    F_CSPSA_TEST_PrintString(V_CSPSA_TEST_TempString_p);

  }

  return AllTestCasesOk;
}

// -----------------------------------------------------------------------------

// - F_CSPSA_TEST_PrintTestType: Converts a variable of type
//   CSPSA_TEST_TestType_t to a string.

#if 0
static char* F_CSPSA_TEST_PrintTestType(const CSPSA_TEST_TestType_t TestType)
{
  char* Result_p = NULL;

  #define M_MACRO_TO_STRING(__n) case __n: Result_p = #__n;break;
  switch (TestType)
  {
    M_MACRO_TO_STRING(T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_CONST )
    M_MACRO_TO_STRING(T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_INC   )
    M_MACRO_TO_STRING(T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_DEC   )
    M_MACRO_TO_STRING(T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_RND   )
    M_MACRO_TO_STRING(T_CSPSA_TEST_TEST_TYPE_KEY_DEC_SIZE_INC   )
    M_MACRO_TO_STRING(T_CSPSA_TEST_TEST_TYPE_KEY_DEC_SIZE_DEC   )
    M_MACRO_TO_STRING(T_CSPSA_TEST_TEST_TYPE_KEY_DEC_SIZE_RND   )
    M_MACRO_TO_STRING(T_CSPSA_TEST_TEST_TYPE_KEY_RND_SIZE_INC   )
    M_MACRO_TO_STRING(T_CSPSA_TEST_TEST_TYPE_KEY_RND_SIZE_DEC   )
    M_MACRO_TO_STRING(T_CSPSA_TEST_TEST_TYPE_KEY_RND_SIZE_RND   )
    M_MACRO_TO_STRING(T_CSPSA_TEST_TEST_TYPE_NBR_OF_ITEMS       )
    M_MACRO_TO_STRING(T_CSPSA_TEST_TEST_TYPE_UNDEFINED          )
    default:
      Result_p = "Unknown";
      break;
  }
  #undef M_MACRO_TO_STRING
  return Result_p;
}
#endif

// -----------------------------------------------------------------------------

// - F_CSPSA_TEST_PrintMemoryType: Converts a variable of type
//   CSPSA_MemoryType_t to a string.

static char* F_CSPSA_TEST_PrintMemoryType(const CSPSA_MemoryType_t MemoryType)
{
  char* Result_p = NULL;

  #define M_MACRO_TO_STRING(__n) case __n: Result_p = #__n;break;
  switch (MemoryType)
  {
    M_MACRO_TO_STRING(T_CSPSA_MEMORY_TYPE_DEV_MTD           )
    M_MACRO_TO_STRING(T_CSPSA_MEMORY_TYPE_DEV_MMC           )
    M_MACRO_TO_STRING(T_CSPSA_MEMORY_TYPE_EMMC              )
    M_MACRO_TO_STRING(T_CSPSA_MEMORY_TYPE_MMC               )
    M_MACRO_TO_STRING(T_CSPSA_MEMORY_TYPE_NAND_RAW          )
    M_MACRO_TO_STRING(T_CSPSA_MEMORY_TYPE_NAND_MANAGED      )
    M_MACRO_TO_STRING(T_CSPSA_MEMORY_TYPE_NOR               )
    M_MACRO_TO_STRING(T_CSPSA_MEMORY_TYPE_RAM               )
    M_MACRO_TO_STRING(T_CSPSA_MEMORY_TYPE_NUMBER_OF_ITEMS   )
    M_MACRO_TO_STRING(T_CSPSA_MEMORY_TYPE_UNDEFINED         )
    default:
      Result_p = "Unknown";
      break;
  }
  #undef M_MACRO_TO_STRING
  return Result_p;
}

// -----------------------------------------------------------------------------

// - F_CSPSA_TEST_Init: Initializes module test.

static bool F_CSPSA_TEST_Init()
{
  bool Result = false;

  // - Initialize write data with a pattern.
  (void) F_CSPSA_TEST_GetWriteData();

  // - Initialize CSPSA.
#ifdef CFG_CSPSA_TEST_EXTERNAL_CONFIG
  // Get configuration from external test driver
  V_CSPSA_TEST_Config = *CSPSA_TEST_GetConfig();
#endif

  Result = true;

//ErrorExit:

  return Result;
}

// -----------------------------------------------------------------------------

// - F_CSPSA_TEST_GetWriteData: Scrambles write data and returns pointer to write data.

static uint8_t* F_CSPSA_TEST_GetWriteData()
{
  static uint32_t Seed = 1749;
  int i;

  // - Initialize write data with a pattern.
  for (i = 0; i < D_CSPSA_TEST_TEST_DATA_SIZE; i++)
  {
    V_CSPSA_TEST_WriteData_p[i] = (uint8_t)(Seed - Seed*~i + ((Seed*Seed*i) >> 8));
    //V_CSPSA_TEST_WriteData_p[i] = i+1;
  }

  Seed = V_CSPSA_TEST_WriteData_p[0] * V_CSPSA_TEST_WriteData_p[1] + V_CSPSA_TEST_WriteData_p[2];

  // - Initialize CSPSA.

  return V_CSPSA_TEST_WriteData_p;
}

// -----------------------------------------------------------------------------

// - F_CSPSA_TEST_InitFlashMemory: Initializes flash memory.

static CSPSA_Result_t F_CSPSA_TEST_InitFlashMemory(
	CSPSA_Config_t* const Config_p
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_UNDEFINED;

#ifndef CFG_CSPSA_TEST_EXTERNAL_CONFIG
  V_CSPSA_TEST_Config.MemoryType = CFG_CSPSA_TEST_MEMORY_TYPE;
  V_CSPSA_TEST_Config.SizeInBytes = CFG_CSPSA_TEST_AREA_SIZE;

  switch (Config_p->MemoryType)
  {
    case T_CSPSA_MEMORY_TYPE_DEV_MTD:
      break;
    case T_CSPSA_MEMORY_TYPE_DEV_MMC:
    {
      CSPSA_LL_DEV_Init();
      Config_p->StartOffsetInBytes = 128 * 1024 * 1024;
      Config_p->BlockSize = 4 * 1024;
      Config_p->SectorSize = 512;
      Config_p->Attributes = 0;
      strncpy(Config_p->Name, "test", D_CSPSA_MAX_NAME_SIZE);
      {
        CSPSA_LinuxDevice_t* LinuxDevice_p = (CSPSA_LinuxDevice_t*) Config_p->Extra_p;
        int FileHandle = -1;

        if (LinuxDevice_p == NULL)
        {
          LinuxDevice_p = (CSPSA_LinuxDevice_t*) malloc(sizeof(CSPSA_LinuxDevice_t));
          memset(LinuxDevice_p, 0, sizeof(CSPSA_LinuxDevice_t));
          Config_p->Extra_p = LinuxDevice_p;
        }

#if (CFG_CSPSA_LBP)
        if (FileHandle == -1)
        {
          LinuxDevice_p->DeviceName_p = "/dev/mmcblk0p3";  // - LBP
          FileHandle = open(LinuxDevice_p->DeviceName_p, O_RDONLY, 0);
        }
        if (FileHandle == -1)
        {
          LinuxDevice_p->DeviceName_p = "/dev/sdc2";         // - Ubuntu (use dmesg right after inserting sd card).
          FileHandle = open(LinuxDevice_p->DeviceName_p, O_RDONLY, 0);
        }
#endif

#if (CFG_CSPSA_ANDROID)
        if (FileHandle == -1)
        {
          LinuxDevice_p->DeviceName_p = "/dev/block/mmcblk0p3";  // - Android
          FileHandle = open(LinuxDevice_p->DeviceName_p, O_RDONLY, 0);
        }
#endif

        if (FileHandle == -1)
        {
          printf("[%s] Internal error at line (%u);Value: 0x%08X. Device '%s' is not accessible.\n", __FILE__, __LINE__, FileHandle, LinuxDevice_p->DeviceName_p);
          Result = T_CSPSA_RESULT_E_MEDIA_INACCESSIBLE;
          break;
        }
        else
        {
          close(FileHandle);
        }

        Config_p->SizeOfExtra = sizeof(*LinuxDevice_p);
      }
      Result = T_CSPSA_RESULT_OK;
      break;
    }
    case T_CSPSA_MEMORY_TYPE_EMMC:
      break;
    case T_CSPSA_MEMORY_TYPE_MMC:
      break;
    case T_CSPSA_MEMORY_TYPE_NAND_RAW:
      break;
    case T_CSPSA_MEMORY_TYPE_NAND_MANAGED:
      break;
    case T_CSPSA_MEMORY_TYPE_NOR:
      break;
    case T_CSPSA_MEMORY_TYPE_RAM:
    {
      CSPSA_LL_RAM_Init();
      if (Config_p->StartOffsetInBytes == 0)
      {
        Config_p->StartOffsetInBytes = (uint64_t)(ptrdiff_t) malloc(Config_p->SizeInBytes); // - Double casting to supress warning.
        if (Config_p->StartOffsetInBytes == 0) break;
      }
      Config_p->BlockSize = 16 * 1024;
      Config_p->SectorSize = 2 * 1024;
      Config_p->Attributes = 0;
      strncpy(Config_p->Name, "test", D_CSPSA_MAX_NAME_SIZE);
      {
        CSPSA_LinuxDevice_t* LinuxDevice_p = (CSPSA_LinuxDevice_t*) Config_p->Extra_p;

        if (LinuxDevice_p == NULL)
        {
          LinuxDevice_p = (CSPSA_LinuxDevice_t*) malloc(sizeof(CSPSA_LinuxDevice_t));
          memset(LinuxDevice_p, 0, sizeof(CSPSA_LinuxDevice_t));
          Config_p->Extra_p = LinuxDevice_p;
        }
        LinuxDevice_p->DeviceName_p = "/cspsa/ram"; // - Needed for client~server code in Linux user space.
        Config_p->SizeOfExtra = sizeof(*LinuxDevice_p);
      }
      Result = T_CSPSA_RESULT_OK;
      break;
    }
    default:
      break;
  }
#endif // CFG_CSPSA_TEST_EXTERNAL_CONFIG
  if (Result == T_CSPSA_RESULT_OK)
  {
    (void)CSPSA_AddParameterArea(Config_p);
  }
  return Result;

}

// -----------------------------------------------------------------------------

// - F_CSPSA_TEST_UnInitFlashMemory: Free any resources.

static CSPSA_Result_t F_CSPSA_TEST_UnInitFlashMemory(CSPSA_Config_t* const Config_p)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_UNDEFINED;

  switch (Config_p->MemoryType)
  {
    case T_CSPSA_MEMORY_TYPE_DEV_MTD:
      break;
    case T_CSPSA_MEMORY_TYPE_DEV_MMC:
    {
      Result = T_CSPSA_RESULT_OK;
      break;
    }
    case T_CSPSA_MEMORY_TYPE_EMMC:
      break;
    case T_CSPSA_MEMORY_TYPE_MMC:
      break;
    case T_CSPSA_MEMORY_TYPE_NAND_RAW:
      break;
    case T_CSPSA_MEMORY_TYPE_NAND_MANAGED:
      break;
    case T_CSPSA_MEMORY_TYPE_NOR:
      break;
    case T_CSPSA_MEMORY_TYPE_RAM:
    {
      if (Config_p->StartOffsetInBytes != 0)
      {
		    free((uint8_t*)(ptrdiff_t)Config_p->StartOffsetInBytes);
		    Config_p->StartOffsetInBytes = 0;
		  }
      Result = T_CSPSA_RESULT_OK;
      break;
    }
    default:
      break;
  }

  if (Config_p->Extra_p != NULL)
  {
    free(Config_p->Extra_p);
    Config_p->Extra_p = NULL;
  }

  CSPSA_RemoveParameterArea(&V_CSPSA_TEST_Config);

  return Result;

}

// -----------------------------------------------------------------------------

// - F_CSPSA_TEST_ReadAndVerify: Reads values from CSPSA and verifies size
//   and content. See F_CSPSA_TEST_WriteValues().

static CSPSA_Result_t F_CSPSA_TEST_ReadAndVerify(
  const CSPSA_Handle_t Handle,
  const uint32_t TotalTestDataSize,
  const CSPSA_TEST_TestType_t TestType
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_UNDEFINED;
  uint32_t DataLeft = TotalTestDataSize;
  CSPSA_Key_t Key;
  CSPSA_Size_t Size;
  uint32_t TempSize;
  uint32_t Index = 0;

  // - Verify what F_CSPSA_TEST_WriteValues() wrote!

  Result = F_CSPSA_TEST_GetInitalParameterValues(Handle, TestType, TotalTestDataSize, &Key, &Size);
  if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;

  do
  {
    if ((Size == 0) || (Size > DataLeft))
    {
      Size = DataLeft;
    }

    // - It's not really necessary to get size of the key value as we have the formula, but do it to
    //   verify the CSPSA_GetSizeOfValue() function.
    Result = CSPSA_GetSizeOfValue(Handle, Key, &TempSize);
    if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;

    if (TempSize != Size)
    {
      Result = T_CSPSA_RESULT_E_NO_VALID_IMAGE;
      goto ErrorExit;
    }

    memset(V_CSPSA_TEST_ReadData_p, 0, sizeof(V_CSPSA_TEST_ReadData_p));
    Result = CSPSA_ReadValue(Handle, Key, Size, V_CSPSA_TEST_ReadData_p);
    if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;

    if (memcmp(V_CSPSA_TEST_ReadData_p, V_CSPSA_TEST_WriteData_p, Size) != 0)
    {
      Result = T_CSPSA_RESULT_E_NO_VALID_IMAGE;
      goto ErrorExit;
    }

    DataLeft -= Size;
    Index++;
    Result = F_CSPSA_TEST_GetNextParameterValues(Handle, TestType, Index, &Key, &Size);
    if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;
  } while (DataLeft > 0);

ErrorExit:

  return Result;
}

// -----------------------------------------------------------------------------

// - F_CSPSA_TEST_VerifyRamImage: Verfies CSPSA's stored images.
#if 0
static CSPSA_Result_t F_CSPSA_TEST_VerifyRamImage(
  const CSPSA_Handle_t Handle,
  const uint32_t TotalTestDataSize,
  const CSPSA_TEST_TestType_t TestType
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_NO_VALID_IMAGE;
  uint32_t StartOfCspsaImageArea = (uint32_t) V_CSPSA_TEST_Config.StartOffsetInBytes;
  CSPSA_CORE_Header_t* Header_p = (CSPSA_CORE_Header_t*) StartOfCspsaImageArea;
  uint32_t NbrOfValidHeaders = 0;
  uint32_t BlockSize = V_CSPSA_TEST_Config.BlockSize;
  uint32_t i;

  // - We need to examine what CSPSA actually wrote to RAM as images after a flush or when CSPSA is
  //   closed. Result needs to be consistent with data written in function
  //   F_CSPSA_TEST_WriteValues().

  // - If only one valid header is found return T_CSPSA_RESULT_W_INVALID_HEADER_FOUND, if both
  //   headers are invalid return T_CSPSA_RESULT_E_NO_VALID_IMAGE.

  // - Check two headers.
  for (i = 0; i < 2; i++)
  {
    uint32_t HeaderSize = sizeof(*Header_p) + Header_p->BbtSize;
    // - A header is found. Examine it before examining data.
    if (Header_p->Magic != C_CSPSA_TEST_HeaderMagic) break;
    if (Header_p->Version != 1) break;
    if (HeaderSize < sizeof(*Header_p)) break;
    if (HeaderSize > V_CSPSA_TEST_Config.SizeInBytes) break;
    if (Header_p->DataSize > V_CSPSA_TEST_Config.SizeInBytes) break;
    if (Header_p->DataSize <= sizeof(CSPSA_TEST_DataItem_t)) break;
    if (HeaderSize + Header_p->DataSize > V_CSPSA_TEST_Config.SizeInBytes) break;
    if (Header_p->Flags >= T_CSPSA_TEST_HEADERFLAGS_END_MARK) break;

    // - Check Crc32Header.
    {
      uint32_t CrcValue = C_CSPSA_TEST_HeaderMagic;

      CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->Magic, sizeof(Header_p->Magic));
      CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->Version, sizeof(Header_p->Version));
      CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->Index, sizeof(Header_p->Index));
      CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->Flags, sizeof(Header_p->Flags));
      CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->DataSize, sizeof(Header_p->DataSize));
      CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->Crc32Data, sizeof(Header_p->Crc32Data));
      CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->BbtSize, sizeof(Header_p->BbtSize));
      if (Header_p->BbtSize > 0)
      {
        CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->Bbt, Header_p->BbtSize);
      }
      if (Header_p->Crc32Header != CrcValue) break;
    }

    // - Check Crc32Data.
    {
      uint32_t CrcValue = C_CSPSA_TEST_HeaderMagic;

      // - If data is at end, it is stored sector/block wise backwards.
      if ((Header_p->Flags & T_CSPSA_TEST_HEADERFLAGS_DATA_AT_END) != 0)
      {
        uint32_t Offset;
        uint32_t ChunkSize;
        uint32_t DataLeft = Header_p->DataSize;

        // - Data is stored backwards.
        Offset = V_CSPSA_TEST_Config.SizeInBytes - BlockSize;
        ChunkSize = (DataLeft > BlockSize - HeaderSize) ? BlockSize - HeaderSize : DataLeft;
        CrcValue = CRC32_Calc(CrcValue, (uint8_t*) StartOfCspsaImageArea + Offset + HeaderSize, ChunkSize);
        DataLeft -= ChunkSize;
        Offset -= BlockSize;
        while (DataLeft > 0)
        {
          ChunkSize = (DataLeft > BlockSize) ? BlockSize : DataLeft;
          CrcValue = CRC32_Calc(CrcValue, (uint8_t*) StartOfCspsaImageArea + Offset, ChunkSize);
          DataLeft -= ChunkSize;
          Offset -= BlockSize;
        }
      }
      else
      {
        CrcValue = CRC32_Calc(CrcValue, (uint8_t*) StartOfCspsaImageArea + HeaderSize, Header_p->DataSize);
      }
      if (CrcValue != Header_p->Crc32Data) break;
    }

    // - Now check data keys and values. Use same algorithm as in function
    //   F_CSPSA_TEST_WriteValues().
    {
      CSPSA_TEST_DataItem_t* DataItem_p = (CSPSA_TEST_DataItem_t*) ((uint8_t*) StartOfCspsaImageArea + HeaderSize);
      uint32_t Index = 0;
      CSPSA_Key_t Key;
      CSPSA_Size_t Size;
      uint32_t DataLeft = Header_p->DataSize;
      uint8_t* TempBuffer_p = NULL;
      uint32_t AmountOfData = TotalTestDataSize;

      if ((Header_p->Flags & T_CSPSA_TEST_HEADERFLAGS_DATA_AT_END) != 0)
      {
        // - Data is stored backwards :-( To avoid a mind-bottling algorithm just
        //   create a temporary image to hold the entire data, copy data and read forward
        //   in a normal, human fashion.
        uint8_t* ReadPointer_p = (uint8_t*)StartOfCspsaImageArea + V_CSPSA_TEST_Config.SizeInBytes - BlockSize + HeaderSize;
        uint32_t ChunkSize;
        uint32_t TempDataLeft = Header_p->DataSize;

        TempBuffer_p = (uint8_t*) malloc(Header_p->DataSize);
        DataItem_p = (CSPSA_TEST_DataItem_t*) TempBuffer_p;

        ChunkSize = (TempDataLeft > BlockSize - HeaderSize) ? BlockSize - HeaderSize : TempDataLeft;
        memcpy(TempBuffer_p, ReadPointer_p, ChunkSize);
        TempDataLeft -= ChunkSize;
        TempBuffer_p += ChunkSize;
        ReadPointer_p -= (HeaderSize + BlockSize);

        while (TempDataLeft > 0)
        {
          ChunkSize = (TempDataLeft > BlockSize) ? BlockSize : TempDataLeft;
          memcpy(TempBuffer_p, ReadPointer_p, ChunkSize);
          TempDataLeft -= ChunkSize;
          TempBuffer_p += ChunkSize;
          ReadPointer_p -= ChunkSize;
        }
        TempBuffer_p = (uint8_t*) DataItem_p;
      }

      Result = F_CSPSA_TEST_GetInitalParameterValues(Handle, TestType, TotalTestDataSize, &Key, &Size);
      if (Result != T_CSPSA_RESULT_OK) break;

      while (DataLeft > 0)
      {
        uint32_t Size32Aligned;

        if ((Size == 0) || (Size > AmountOfData))
        {
          Size = AmountOfData;
        }
        if (DataItem_p->Key != Key) break;
        if (DataItem_p->Size != Size) break;
        if (memcmp(V_CSPSA_TEST_WriteData_p, DataItem_p->Data_p, Size) != 0) break;
        Size32Aligned = M_CSPSA_TEST_ALIGN32(sizeof(*DataItem_p) + DataItem_p->Size);
        DataLeft -= Size32Aligned;
        AmountOfData -= DataItem_p->Size;
        DataItem_p = (CSPSA_TEST_DataItem_t*)((uint8_t*)DataItem_p + Size32Aligned);
        Index++;
        Result = F_CSPSA_TEST_GetNextParameterValues(Handle, TestType, Index, &Key, &Size);
        if (Result != T_CSPSA_RESULT_OK) break;
      }

      if (TempBuffer_p != NULL)
      {
        free(TempBuffer_p);
      }
      if (DataLeft > 0)
      {
        break;
      }
    }

    // - First image was fine. Now check the second.
    NbrOfValidHeaders++;
    Header_p = (CSPSA_CORE_Header_t*) ((uint8_t*)StartOfCspsaImageArea + V_CSPSA_TEST_Config.SizeInBytes - BlockSize);
  }

ErrorExit:

  if (NbrOfValidHeaders == 2)
  {
    Result = T_CSPSA_RESULT_OK;
  }
  else
  if (NbrOfValidHeaders == 1)
  {
    Result = T_CSPSA_RESULT_W_INVALID_HEADER_FOUND;
  }
  else
  {
    Result = T_CSPSA_RESULT_E_NO_VALID_IMAGE;
  }

  return Result;
}
#endif

// -----------------------------------------------------------------------------

// - F_CSPSA_TEST_WriteValues: Writes test data to CSPSA.

static CSPSA_Result_t F_CSPSA_TEST_WriteValues(
  const CSPSA_Handle_t Handle,
  const uint32_t TotalTestDataSize,
  const CSPSA_TEST_TestType_t TestType
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_UNDEFINED;
  uint32_t DataLeft = TotalTestDataSize;
  CSPSA_Key_t Key;
  CSPSA_Size_t Size;
  uint32_t Index = 0;
  uint8_t* WriteData_p = F_CSPSA_TEST_GetWriteData();

  Result = F_CSPSA_TEST_GetInitalParameterValues(Handle, TestType, TotalTestDataSize, &Key, &Size);
  if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;

  do
  {
    if ((Size > DataLeft) || (Size == 0))
    {
      Size = DataLeft;
    }

    Result = CSPSA_WriteValue(Handle, Key, Size, WriteData_p);
    if (Result >= T_CSPSA_RESULT_ERRORS) goto ErrorExit;

    DataLeft -= Size;
    Index++;
    Result = F_CSPSA_TEST_GetNextParameterValues(Handle, TestType, Index, &Key, &Size);
    if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;

  } while (DataLeft > 0);

ErrorExit:

	if (DataLeft == 0)
  {
    Result = T_CSPSA_RESULT_OK;
  }
  else
  if (Result == T_CSPSA_RESULT_OK)
  {
  	Result = T_CSPSA_RESULT_NUMBER_OF_ITEMS;
  }

  return Result;
}

// -----------------------------------------------------------------------------

// - F_CSPSA_TEST_InitTestCase: Initializes a test case.
static void F_CSPSA_TEST_InitTestCase(const uint32_t TestCaseTableIndex)
{
  (void)(TestCaseTableIndex);
  // - Clear all time measurment values.
  memset((uint8_t*)&V_CSPSA_TEST_TimeMeasurement, 0, sizeof(V_CSPSA_TEST_TimeMeasurement));
  F_CSPSA_TEST_ResetString(V_CSPSA_TEST_TempString_p);
  F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_TotalStartTime);
}

// -----------------------------------------------------------------------------

// - F_CSPSA_TEST_FinishTestCase: Can be called at the end of a test case.
static void F_CSPSA_TEST_FinishTestCase(const uint32_t TestCaseTableIndex)
{
  CSPSA_TEST_TC_t* CurrentTestCase_p = &(CSPSA_TEST_TC_Table[TestCaseTableIndex]);
  F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_TotalStartTime, &V_CSPSA_TEST_TotalElapsedTime);
#if 0
  (void) F_CSPSA_TEST_UnInitFlashMemory(V_CSPSA_TEST_Config_p);
#endif
  if (CurrentTestCase_p->TcStatus != T_CSPSA_TEST_TC_STATUS_NOT_IMPLEMENTED)
  {
    if (CurrentTestCase_p->TestOk)
    {
      sprintf(V_CSPSA_TEST_TempString_p, "Ok.");
      sprintf(V_CSPSA_TEST_TempString_p+strlen(V_CSPSA_TEST_TempString_p),
        ";%lu\n",
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TotalElapsedTime)
      );
    }
    else
    {
      sprintf(
        V_CSPSA_TEST_TempString_p,
        "Failed;Line %u",
        V_CSPSA_TEST_ErrorLine
      );
    }
    F_CSPSA_TEST_AppendTestCaseResult(TestCaseTableIndex, V_CSPSA_TEST_TempString_p);
  }
}

// -----------------------------------------------------------------------------

#ifndef CFG_CSPSA_TEST_READ_ERRORS
// - CSPSA_TEST_CanInduceMediaErrors: Dummy implementations of test driver functions.
bool CSPSA_TEST_CanInduceMediaErrors(void)
{
  return false;
}
#endif

// -----------------------------------------------------------------------------

#ifndef CFG_CSPSA_TEST_READ_ERRORS
// - CSPSA_TEST_RAM_Close: Dummy function.
void CSPSA_TEST_RAM_Close(void)
{
  ;
}
#endif

// -----------------------------------------------------------------------------

#ifndef CFG_CSPSA_TEST_READ_ERRORS
void CSPSA_TEST_InduceError(CSPSA_TEST_InducedError_t* MediaError_p)
{
  (void)MediaError_p;
}

#endif

// -----------------------------------------------------------------------------

/**
 * Returns total size of all parameters (excluding size of the keys).
 */
static uint32_t F_CSPSA_TEST_GetSizeOfAllParameters(const CSPSA_Handle_t Handle)
{
  CSPSA_Key_t Key;
  CSPSA_Size_t Size = 0;
  CSPSA_Size_t TotalSize = 0;
  CSPSA_Result_t Result;

  Result = CSPSA_GetSizeOfFirstValue(Handle, &Key, &Size);
  while (Result == T_CSPSA_RESULT_OK)
  {
    TotalSize += Size;
    Result = CSPSA_GetSizeOfNextValue(Handle, &Key, &Size);
  }
  return TotalSize;
}

// -----------------------------------------------------------------------------

static void F_CSPSA_TEST_ResetRandSeed(void)
{
  V_CSPSA_TEST_Seed1 = 960131;    /* must not be zero */
  V_CSPSA_TEST_Seed2 = 1000003;    /* must not be zero */
}

// -----------------------------------------------------------------------------

/** Simple random generator */
static int F_CSPSA_TEST_GetRand(uint32_t Max)
{
  uint32_t Result;

  V_CSPSA_TEST_Seed2 = 36969 * (V_CSPSA_TEST_Seed2 & 65535) + (V_CSPSA_TEST_Seed2 >> 16);
  V_CSPSA_TEST_Seed1 = 18000 * (V_CSPSA_TEST_Seed1 & 65535) + (V_CSPSA_TEST_Seed1 >> 16);
  Result = (V_CSPSA_TEST_Seed2 << 16) + V_CSPSA_TEST_Seed1;  /* 32-bit result */
  return Result % Max;
}



// =============================================================================
//   Simple list implementation (to check consistency with CSPSA)
// =============================================================================

// - Extremely simple "key/value" implementation.

static void F_CSPSA_TEST_ClearSimpleList(void)
{
  V_CSPSA_TEST_SimpleListSize = 0;
}

// -----------------------------------------------------------------------------

static void F_CSPSA_TEST_InsertInSimpleList(CSPSA_Key_t Key, int Value)
{
  uint32_t i;

  for (i = 0; i < V_CSPSA_TEST_SimpleListSize; ++i)
  {
    if (V_CSPSA_TEST_SimpleList[i].Key == Key)
    {
      // - Replace existing value and return.
      V_CSPSA_TEST_SimpleList[i].Value = Value;
      return;
    }
    else if (V_CSPSA_TEST_SimpleList[i].Key > Key)
    {
      // - Insert at index i to keep simple list sorted.
      break;
    }
  }
  if (V_CSPSA_TEST_SimpleListSize < D_CSPSA_TEST_MAX_SIMPLE_LIST_SIZE)
  {
    // - Insert at index i to keep simple list sorted; move existing elements 1 step.
    uint32_t j;
    for (j = V_CSPSA_TEST_SimpleListSize; j > i; --j)
    {
      V_CSPSA_TEST_SimpleList[j] = V_CSPSA_TEST_SimpleList[j-1];
    }
    V_CSPSA_TEST_SimpleList[i].Key = Key;
    V_CSPSA_TEST_SimpleList[i].Value = Value;
    ++V_CSPSA_TEST_SimpleListSize;
  }
}

// -----------------------------------------------------------------------------

static void F_CSPSA_TEST_DeleteFromSimpleList(CSPSA_Key_t Key)
{
  uint32_t i;

  for (i = 0; i < V_CSPSA_TEST_SimpleListSize; ++i)
  {
    if (V_CSPSA_TEST_SimpleList[i].Key == Key)
    {
      // - Found key; move all elements after this element 1 step.
      uint32_t j;
      for (j = i+1; j < V_CSPSA_TEST_SimpleListSize; ++j)
      {
        V_CSPSA_TEST_SimpleList[j-1] = V_CSPSA_TEST_SimpleList[j];
      }
      --V_CSPSA_TEST_SimpleListSize;
    }
  }
}

// -----------------------------------------------------------------------------

static void F_CSPSA_TEST_PrintList(void)
{
  uint32_t i;

  printf("List size: %u; elements: { ", V_CSPSA_TEST_SimpleListSize);
  for (i = 0; i < V_CSPSA_TEST_SimpleListSize; ++i)
  {
    if (i > 0)
    {
      printf(", ");
    }
    if (i%20 == 0) printf("\n  ");
    printf("(key %u, val %d)", V_CSPSA_TEST_SimpleList[i].Key, V_CSPSA_TEST_SimpleList[i].Value);
  }
  printf(" }\n");
}

// -----------------------------------------------------------------------------

// - F_CSPSA_TEST_CheckItems: Checks that items in CSPSA are consistent with items in simple list.
static int F_CSPSA_TEST_CheckItems(void)
{
  uint32_t i = 0;
  int Ok = 0;
  CSPSA_Result_t Result;
  CSPSA_Key_t RealKey = 0;
  CSPSA_Size_t Size;

  if (V_CSPSA_TEST_SimpleListSize == 0)
  {
    Result = CSPSA_GetSizeOfFirstValue(V_CSPSA_TEST_Handle, &RealKey, &Size);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(RealKey == (CSPSA_Key_t) (-D_CSPSA_CORE_NBR_OF_FAST_PARAMETERS));
  }
  else
  {
    for (i = 0 ; i < V_CSPSA_TEST_SimpleListSize; i++ )
    {
      CSPSA_Key_t ExpectedKey = V_CSPSA_TEST_SimpleList[i].Key;
      int ExpectedValue = V_CSPSA_TEST_SimpleList[i].Value;
      CSPSA_Size_t ExpectedSize = (CSPSA_Size_t)((ExpectedValue+1)*32);
      int ExpectedChar = 'A' + (ExpectedValue % 25);
      CSPSA_Data_t* Buf_p;
      uint32_t j;

      if (i == 0)
      {
        Result = CSPSA_GetSizeOfFirstValue(V_CSPSA_TEST_Handle, &RealKey, &Size);
      }
      else
      {
        Result = CSPSA_GetSizeOfNextValue(V_CSPSA_TEST_Handle, &RealKey, &Size);
      }
      M_CSPSA_TEST_ASSERT_ERROR_LINE(Result == T_CSPSA_RESULT_OK);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(RealKey == ExpectedKey);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(Size == ExpectedSize);
      Buf_p = malloc(Size);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(Buf_p != NULL);
      Result = CSPSA_ReadValue(V_CSPSA_TEST_Handle, RealKey, Size, Buf_p);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(Result == T_CSPSA_RESULT_OK);
      for (j = 0; j < Size; ++j)
      {
        M_CSPSA_TEST_ASSERT_ERROR_LINE(ExpectedChar == Buf_p[j]);
      }
      free(Buf_p);
    }
    // - Check that we have reached the end.
    Result = CSPSA_GetSizeOfNextValue(V_CSPSA_TEST_Handle, &RealKey, &Size);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(RealKey == (CSPSA_Key_t) (-D_CSPSA_CORE_NBR_OF_FAST_PARAMETERS));
  }

  Ok = 1;

ErrorExit:
  if (!Ok)
  {
    printf("F_CSPSA_TEST_CheckItems: error in element index %u, key %u\n", i, V_CSPSA_TEST_SimpleList[i].Key);
    printf("Failed at line %u, Result=%d\n", V_CSPSA_TEST_ErrorLine, Result);
    F_CSPSA_TEST_PrintList();
  }
  return Ok;
}

// -----------------------------------------------------------------------------

// - F_CSPSA_TEST_InsertItem: Adds an item to CSPSA, as well as to the simple list. If
// 	 the item already existed it is replaced. At the end it is checked whether the simple
//   list and CSPSA are still consistent.
static int F_CSPSA_TEST_InsertItem(const CSPSA_Key_t Key, int Value)
{
  int Ok = 0;
  CSPSA_Result_t Result = T_CSPSA_RESULT_UNDEFINED;
  uint32_t Size = (Value+1)*32;
  uint32_t ExistingSize;
  char* Buf_p = malloc(Size);
  uint32_t i;

  //printf("F_CSPSA_TEST_InsertItem(%u, %d)\n", Key, Value);
  M_CSPSA_TEST_ASSERT_ERROR_LINE(Buf_p != NULL);
  for (i = 0; i < Size; ++i)
  {
    Buf_p[i] = 'A' + (Value % 25);
  }
  // - Check if the item already exists.
  Result = CSPSA_GetSizeOfValue(V_CSPSA_TEST_Handle, Key, &ExistingSize);
  M_CSPSA_TEST_ASSERT_ERROR_LINE(Result == T_CSPSA_RESULT_OK || Result == T_CSPSA_RESULT_E_INVALID_KEY);
  if (Result == T_CSPSA_RESULT_E_INVALID_KEY)
  {
    // - Key did not exist.
    Result = CSPSA_WriteValue(V_CSPSA_TEST_Handle, Key, Size, (CSPSA_Data_t*)Buf_p);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(Result < T_CSPSA_RESULT_ERRORS);
  }
  else
  {
    Result = CSPSA_WriteValue(V_CSPSA_TEST_Handle, Key, Size, (CSPSA_Data_t*)Buf_p);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(Result < T_CSPSA_RESULT_ERRORS);
  }
  F_CSPSA_TEST_InsertInSimpleList(Key, Value);
  Ok = F_CSPSA_TEST_CheckItems();

ErrorExit:

  if (Buf_p != NULL)
  {
    free(Buf_p);
  }
  if (!Ok)
  {
    printf("F_CSPSA_TEST_InsertItem(%u, %d): failed at line %d, Result = %d\n", Key, Value, V_CSPSA_TEST_ErrorLine, Result);
    F_CSPSA_TEST_PrintList();
  }
  return Ok;
}

// -----------------------------------------------------------------------------

// - Deletes an item from CSPSA, as well as from the simple list. At the end it is
//   checked whether the simple list and CSPSA are still consistent.
static int F_CSPSA_TEST_DeleteItem(const CSPSA_Key_t Key)
{
  int Ok = 0;
  CSPSA_Result_t Result;
  uint32_t ExistingSize;

  //printf("F_CSPSA_TEST_DeleteItem(%u)\n", Key);
  // check if the item already exists
  Result = CSPSA_GetSizeOfValue(V_CSPSA_TEST_Handle, Key, &ExistingSize);
  M_CSPSA_TEST_ASSERT_ERROR_LINE(Result == T_CSPSA_RESULT_OK);
  Result = CSPSA_DeleteValue(V_CSPSA_TEST_Handle, Key);
  M_CSPSA_TEST_ASSERT_ERROR_LINE(Result == T_CSPSA_RESULT_OK);
  F_CSPSA_TEST_DeleteFromSimpleList(Key);
  Ok = F_CSPSA_TEST_CheckItems();
ErrorExit:
  if (!Ok)
  {
    printf("F_CSPSA_TEST_DeleteItem(%u): failed at line %d, Result = %d\n", Key, V_CSPSA_TEST_ErrorLine, Result);
    F_CSPSA_TEST_PrintList();
  }
  return Ok;
}

// -----------------------------------------------------------------------------

// - Flushes CSPSA, closes, and opens it again. At the end it is checked whether
//   the simple list and CSPSA are still consistent.
static int F_CSPSA_TEST_FlushCloseReopen(bool CheckItems)
{
  CSPSA_Result_t CSPSA_Result;
  int Ok = 0;

  //printf("F_CSPSA_TEST_FlushCloseReopen()\n");
  // - Flush.
  CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
  // - Close.
  CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
  // - Open again.
  CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
  if (CheckItems)
  {
    Ok = F_CSPSA_TEST_CheckItems();
    M_CSPSA_TEST_ASSERT_ERROR_LINE(Ok);
  }
  else
  {
    Ok = 1;
  }

ErrorExit:

  return Ok;
}

// -----------------------------------------------------------------------------

// - F_CSPSA_TEST_WriteBlock: Writes a trim area parameter of the given size, with all bytes set to the given character
static bool F_CSPSA_TEST_WriteBlock(const CSPSA_Handle_t Handle, const CSPSA_Key_t Key, const char c, uint32_t Size)
{
  char* Buffer_p = malloc(Size);
  bool Ok = Buffer_p != NULL;
  CSPSA_Result_t Result;

  M_CSPSA_TEST_ASSERT_ERROR_LINE(Ok);
  memset(Buffer_p, c, Size);
  Result = CSPSA_WriteValue(Handle, Key, Size, (CSPSA_Data_t*)Buffer_p);
  free(Buffer_p);
  M_CSPSA_TEST_ASSERT_ERROR_LINE(Result == T_CSPSA_RESULT_OK);
  Ok = true;
ErrorExit:
  return Ok;
}

// =============================================================================
//   main()
// =============================================================================

// - main: Calls CSPSA_TEST_Execute().

#ifndef CFG_CSPSA_TEST_NO_MAIN
int main(int argc, char **argv)
{
  M_CSPSA_PARAMETER_NOT_USED(argc);
  M_CSPSA_PARAMETER_NOT_USED(argv);

  return CSPSA_TEST_Execute();

}
#endif

// =============================================================================
//   Global Function Definitions
// =============================================================================

// - CSPSA_TEST_Execute: Executes all test cases in the test case table.

int CSPSA_TEST_Execute()
{
  int Result = 0;
  uint32_t TestCaseTableIndex;
  static uint32_t ExecutionCounter = 1;
  bool AllTestCasesOk = false;
  CSPSA_Result_t CSPSA_Result;

  // - Initalize socket server.
  CSPSA_Result = F_CSPSA_TEST_InitFlashMemory(&V_CSPSA_TEST_Config);
  if (CSPSA_Result != T_CSPSA_RESULT_OK) goto ErrorExit;

#if (CFG_CSPSA_TEST_TARGET == CFG_CSPSA_TEST_TARGET_LINUX_US) && (CFG_CSPSA_TEST_VARIANT == CFG_CSPSA_TEST_VARIANT_API)
  CSPSA_API_LINUX_SOCKET_SERVER_Create(&V_CSPSA_TEST_Config);
#endif

  if ( !F_CSPSA_TEST_Init() )
  {
    goto ErrorExit;
  }

  F_CSPSA_TEST_ResetString(V_CSPSA_TEST_TempString_p);

  printf(D_CSPSA_TEST_MODULE_HEADER "Module test started.\n");

  // - Process each test case.
  for (TestCaseTableIndex = 0; TestCaseTableIndex < C_CSPSA_TEST_NumberOfTestCases; TestCaseTableIndex++)
  {
    CSPSA_TEST_TC_t* CurrentTestCase_p = &(CSPSA_TEST_TC_Table[TestCaseTableIndex]);

    printf(D_CSPSA_TEST_MODULE_HEADER "Executing test case '%s'.\n", CurrentTestCase_p->TestCaseName_p);

    CurrentTestCase_p->TestCaseFunction_p(TestCaseTableIndex);
    F_CSPSA_TEST_TC_NotPerformed(TestCaseTableIndex);
    // - A test case must result in test result strings.
    M_CSPSA_TEST_ASSERT(CurrentTestCase_p->TestCaseResult.TestCaseResultStringList != NULL, __LINE__, TestCaseTableIndex);
  }

  printf(D_CSPSA_TEST_MODULE_HEADER "Module test ended. Printing test case results.\n");
  printf(D_CSPSA_TEST_MODULE_HEADER "Run 'TcTableToExcel' with this log file to get result in Excel format.\n");

  AllTestCasesOk = F_CSPSA_TEST_PrintTestCaseResults();

  F_CSPSA_TEST_CleanUp();

  ExecutionCounter++;

ErrorExit:

  F_CSPSA_TEST_InitFlashMemory(&V_CSPSA_TEST_Config);

#if (CFG_CSPSA_TEST_TARGET == CFG_CSPSA_TEST_TARGET_LINUX_US) && (CFG_CSPSA_TEST_VARIANT == CFG_CSPSA_TEST_VARIANT_API)
  CSPSA_API_LINUX_SOCKET_SERVER_Destroy(&V_CSPSA_TEST_Config);
#endif

  F_CSPSA_TEST_UnInitFlashMemory(&V_CSPSA_TEST_Config);

  F_CSPSA_TEST_PrintString("\n");

  Result = AllTestCasesOk ? 0 : -1;

  return Result;

}

// =============================================================================
//   Test Cases
// =============================================================================

// -----------------------------------------------------------------------------

// - CSPSA_TEST_TC_1
//
//   § Create a CSPSA image and verify it
//   § All CSPSA function call durations should be summed up and presented as a test result
//   § A summed duration time for each type of function call should be presented (that is separate
//     sums for open, flush, close, read and write operations)
//
//  (*) A CSPSA image is created in many different ways. See type CSPSA_TEST_TestType_t
//      for all variants that is tested.

void CSPSA_TEST_TC_1(const uint32_t TestCaseTableIndex)
{
  CSPSA_TEST_TC_t* CurrentTestCase_p = &(CSPSA_TEST_TC_Table[TestCaseTableIndex]);
  uint32_t TestDataSize = 0;
  CSPSA_Result_t CSPSA_Result;
  uint32_t TestSizeTuples[][2] = {{1,12}, {512-80,512+80}, {4*1024-80,4*1024+80}};
  uint32_t TupleIx;
  CSPSA_TEST_TestType_t TestType = 0;

  // - Clear all time measurment values.
  memset((uint8_t*)&V_CSPSA_TEST_TimeMeasurement, 0, sizeof(V_CSPSA_TEST_TimeMeasurement));

  // - Initialize.

  CurrentTestCase_p->TcStatus = T_CSPSA_TEST_TC_STATUS_VALID;
  V_CSPSA_TEST_ErrorLine = 0;
  F_CSPSA_TEST_ResetString(V_CSPSA_TEST_TempString_p);
  if (CurrentTestCase_p->TcStatus == T_CSPSA_TEST_TC_STATUS_NOT_IMPLEMENTED)
  {
    goto ErrorExit;
  }

  F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_TotalStartTime);

  // - Now create a CSPSA image in different ways determined by CSPSA_TEST_TestType_t. Clear everything between
  //   test runs.

  V_CSPSA_TEST_ConstSize = 1;
  for (TestType = (CSPSA_TEST_TestType_t) 0; TestType < T_CSPSA_TEST_TEST_TYPE_NBR_OF_ITEMS; TestType++)
  {
    for (TupleIx = 0; TupleIx < sizeof(TestSizeTuples)/sizeof(TestSizeTuples[0]); TupleIx++)
    {
      for (TestDataSize = TestSizeTuples[TupleIx][0]; TestDataSize <= TestSizeTuples[TupleIx][1]; TestDataSize += 1)
      {
        //printf(D_CSPSA_TEST_MODULE_HEADER "TestType(%s)/TestDataSize(%u).\n", F_CSPSA_TEST_PrintTestType(TestType), TestDataSize);

        // - Create CSPSA in cache.
        F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
        CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
        F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
        F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CREATE], &V_CSPSA_TEST_ElapsedTime);

        // - Write values.
        F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
        CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
        F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
        F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_1ST], &V_CSPSA_TEST_ElapsedTime);

        // - Flush.
        F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
        CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
        F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
        F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FLUSH], &V_CSPSA_TEST_ElapsedTime);

#if 0
        // - TBD: There is a problem with F_CSPSA_TEST_VerifyRamImage(). It needs the verification data
        //   orderered as the image is written ordered.
        if
        (
          (V_CSPSA_TEST_Config.MemoryType == T_CSPSA_MEMORY_TYPE_RAM)
          &&
          (
            (TestType == T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_CONST)
            ||
            (TestType == T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_INC)
            ||
            (TestType == T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_DEC)
            ||
            (TestType == T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_RND)
          )
        )
        {
          CSPSA_Result = F_CSPSA_TEST_VerifyRamImage(V_CSPSA_TEST_Handle, TestDataSize, TestType);
          M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
        }
#endif

        // - Close.
        F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
        CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
        F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
        F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);

        // - Open.
        F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
        CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
        F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
        F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_OPEN], &V_CSPSA_TEST_ElapsedTime);

        // - Read values.
        F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
        CSPSA_Result = F_CSPSA_TEST_ReadAndVerify(V_CSPSA_TEST_Handle, TestDataSize, TestType);
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
        F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
        F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_READ], &V_CSPSA_TEST_ElapsedTime);

        // - Write values.
        F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
        CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
        F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
        F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_2ND], &V_CSPSA_TEST_ElapsedTime);

        // - Flush.
        F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
        CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
        F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
        F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FLUSH], &V_CSPSA_TEST_ElapsedTime);

#if 0
        // - TBD: There is a problem with F_CSPSA_TEST_VerifyRamImage(). It needs the verification data
        //   orderered as the image is written ordered.
        if
        (
          (V_CSPSA_TEST_Config.MemoryType == T_CSPSA_MEMORY_TYPE_RAM)
          &&
          (
            (TestType == T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_CONST)
            ||
            (TestType == T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_INC)
            ||
            (TestType == T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_DEC)
            ||
            (TestType == T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_RND)
          )
        )
        {
          CSPSA_Result = F_CSPSA_TEST_VerifyRamImage(V_CSPSA_TEST_Handle, TestDataSize, TestType);
          M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
        }
#endif

        // - Close.
        F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
        CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
        F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
        F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);

        // - Open.
        F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
        CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
        F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
        F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_OPEN], &V_CSPSA_TEST_ElapsedTime);

        // - Read values.
        F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
        CSPSA_Result = F_CSPSA_TEST_ReadAndVerify(V_CSPSA_TEST_Handle, TestDataSize, TestType);
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
        F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
        F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_READ], &V_CSPSA_TEST_ElapsedTime);

        // - Format.
        F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
        CSPSA_Result = CSPSA_Format(V_CSPSA_TEST_Handle);
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
        F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
        F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FORMAT], &V_CSPSA_TEST_ElapsedTime);

        // - Close.
        F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
        CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
        F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
        F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);

      }
    }
  }
  F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_TotalStartTime, &V_CSPSA_TEST_TotalElapsedTime);

  CurrentTestCase_p->TestOk = true;

ErrorExit:

  if (CurrentTestCase_p->TcStatus != T_CSPSA_TEST_TC_STATUS_NOT_IMPLEMENTED)
  {
    if (CurrentTestCase_p->TestOk)
    {
      sprintf(V_CSPSA_TEST_TempString_p, "Ok.");
      sprintf(V_CSPSA_TEST_TempString_p+strlen(V_CSPSA_TEST_TempString_p),
        ";%lu;%lu;%lu;%lu;%lu;%lu;%lu;%lu;%lu;%lu\n",
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TotalElapsedTime),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_OPEN]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_READ]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_1ST]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_2ND]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FLUSH]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FORMAT]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CREATE]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_DELETE])
      );
    }
    else
    {
      sprintf(
        V_CSPSA_TEST_TempString_p,
        "Failed;Line %u;TestDataSize=%u;TestType=%u",
        V_CSPSA_TEST_ErrorLine,
        TestDataSize,
        TestType
      );
      if (V_CSPSA_TEST_Handle != 0)
      {
        (void) CSPSA_Close(&V_CSPSA_TEST_Handle);
      }
    }

    F_CSPSA_TEST_AppendTestCaseResult(TestCaseTableIndex, V_CSPSA_TEST_TempString_p);

  }
}

// -----------------------------------------------------------------------------

// - CSPSA_TEST_TC_2
//
//   § Delete objects in a CSPSA image and verify it
//   § All CSPSA function call durations should be summed up and presented as a test result
//
//  (a) Delete some values at beginning, end and in middle.
//  (b) Update some of the values that are to be deleted so that they end up in item list.
//  (c) Try to remove all items.

void CSPSA_TEST_TC_2(const uint32_t TestCaseTableIndex)
{
  CSPSA_TEST_TC_t* CurrentTestCase_p = &(CSPSA_TEST_TC_Table[TestCaseTableIndex]);
  uint32_t TestDataSize = 0;
  CSPSA_Result_t CSPSA_Result;
  uint32_t TestSizeTuples[][2] = {{4*1024-80,4*1024+80}};
  uint32_t TupleIx;
  CSPSA_TEST_TestType_t TestType;
  CSPSA_Key_t TestKey = 0;
  CSPSA_Size_t TestSize = 0;
  CSPSA_Key_t ValueKey[9];
  CSPSA_Size_t ValueSize[9];
  uint32_t i;

  // - Clear all time measurment values.
  memset((uint8_t*)&V_CSPSA_TEST_TimeMeasurement, 0, sizeof(V_CSPSA_TEST_TimeMeasurement));
  memset((uint8_t*)ValueKey, 0, sizeof(ValueKey));
  memset((uint8_t*)ValueSize, 0, sizeof(ValueSize));

  // - Initialize.

  CurrentTestCase_p->TcStatus = T_CSPSA_TEST_TC_STATUS_VALID;
  V_CSPSA_TEST_ErrorLine = 0;
  F_CSPSA_TEST_ResetString(V_CSPSA_TEST_TempString_p);
  if (CurrentTestCase_p->TcStatus == T_CSPSA_TEST_TC_STATUS_NOT_IMPLEMENTED)
  {
    goto ErrorExit;
  }

  F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_TotalStartTime);

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (a) Delete some values at beginning, end and in middle.
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  V_CSPSA_TEST_ConstSize = 1;
  for (TestType = (CSPSA_TEST_TestType_t) 0; TestType < 1; TestType++)
  {
    for (TupleIx = 0; TupleIx < sizeof(TestSizeTuples)/sizeof(TestSizeTuples[0]); TupleIx++)
    {
      for (TestDataSize = TestSizeTuples[TupleIx][0]; TestDataSize <= TestSizeTuples[TupleIx][1]; TestDataSize += 1)
      {
        //printf(D_CSPSA_TEST_MODULE_HEADER "TestType(%s)/TestDataSize(%u).\n", F_CSPSA_TEST_PrintTestType(TestType), TestDataSize);

        // - Create CSPSA in cache.
        F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
        CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
        F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
        F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CREATE], &V_CSPSA_TEST_ElapsedTime);

        // - Write values.
        F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
        CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
        F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
        F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_1ST], &V_CSPSA_TEST_ElapsedTime);

        // - Flush.
        F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
        CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
        F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
        F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FLUSH], &V_CSPSA_TEST_ElapsedTime);

        // - Close.
        F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
        CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
        F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
        F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);

        // - Open.
        F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
        CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
        F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
        F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_OPEN], &V_CSPSA_TEST_ElapsedTime);

        // - Get first three keys.
        i = 0;
        CSPSA_Result = CSPSA_GetSizeOfFirstValue(V_CSPSA_TEST_Handle, &TestKey, &ValueSize[i]);
        ValueKey[i++] = TestKey;
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
        CSPSA_Result = CSPSA_GetSizeOfNextValue(V_CSPSA_TEST_Handle, &TestKey, &ValueSize[i]);
        ValueKey[i++] = TestKey;
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
        CSPSA_Result = CSPSA_GetSizeOfNextValue(V_CSPSA_TEST_Handle, &TestKey, &ValueSize[i]);
        ValueKey[i++] = TestKey;
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
        // - Skip one item.
        CSPSA_Result = CSPSA_GetSizeOfNextValue(V_CSPSA_TEST_Handle, &TestKey, &ValueSize[i]);
        CSPSA_Result = CSPSA_GetSizeOfNextValue(V_CSPSA_TEST_Handle, &TestKey, &ValueSize[i]);
        ValueKey[i++] = TestKey;
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
        CSPSA_Result = CSPSA_GetSizeOfNextValue(V_CSPSA_TEST_Handle, &TestKey, &ValueSize[i]);
        ValueKey[i++] = TestKey;
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
        CSPSA_Result = CSPSA_GetSizeOfNextValue(V_CSPSA_TEST_Handle, &TestKey, &ValueSize[i]);
        ValueKey[i++] = TestKey;
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
        // - Get last three items.
        CSPSA_Result = CSPSA_GetSizeOfNextValue(V_CSPSA_TEST_Handle, &TestKey, &ValueSize[i]);
        ValueKey[i++] = TestKey;
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
        CSPSA_Result = CSPSA_GetSizeOfNextValue(V_CSPSA_TEST_Handle, &TestKey, &ValueSize[i]);
        ValueKey[i++] = TestKey;
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
        CSPSA_Result = CSPSA_GetSizeOfNextValue(V_CSPSA_TEST_Handle, &TestKey, &ValueSize[i]);
        ValueKey[i++] = TestKey;
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
        while (TestKey < D_CSPSA_CORE_FIRST_FAST_PARAMETER_KEY)
        {
          ValueKey[6] = ValueKey[7];
          ValueKey[7] = ValueKey[8];
          ValueKey[8] = TestKey;
          ValueSize[6] = ValueSize[7];
          ValueSize[7] = ValueSize[8];
          ValueSize[8] = TestSize;
          CSPSA_Result = CSPSA_GetSizeOfNextValue(V_CSPSA_TEST_Handle, &TestKey, &TestSize);
        }

        // - Read values.
        F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
        CSPSA_Result = F_CSPSA_TEST_ReadAndVerify(V_CSPSA_TEST_Handle, TestDataSize, TestType);
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
        F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
        F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_READ], &V_CSPSA_TEST_ElapsedTime);

        // - Delete values {1, 3, 2}, {last, last-2, last-1}, {x, x+2, x+1}.
        F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
        CSPSA_Result = CSPSA_DeleteValue(V_CSPSA_TEST_Handle, ValueKey[0]);
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
        CSPSA_Result = CSPSA_DeleteValue(V_CSPSA_TEST_Handle, ValueKey[2]);
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
        CSPSA_Result = CSPSA_DeleteValue(V_CSPSA_TEST_Handle, ValueKey[1]);
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
        CSPSA_Result = CSPSA_DeleteValue(V_CSPSA_TEST_Handle, ValueKey[3]);
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
        CSPSA_Result = CSPSA_DeleteValue(V_CSPSA_TEST_Handle, ValueKey[5]);
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
        CSPSA_Result = CSPSA_DeleteValue(V_CSPSA_TEST_Handle, ValueKey[4]);
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
        CSPSA_Result = CSPSA_DeleteValue(V_CSPSA_TEST_Handle, ValueKey[8]);
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
        CSPSA_Result = CSPSA_DeleteValue(V_CSPSA_TEST_Handle, ValueKey[6]);
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
        CSPSA_Result = CSPSA_DeleteValue(V_CSPSA_TEST_Handle, ValueKey[7]);
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
        F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
        F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_DELETE], &V_CSPSA_TEST_ElapsedTime);

        // - Flush.
        F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
        CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
        F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
        F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FLUSH], &V_CSPSA_TEST_ElapsedTime);

        // - Close.
        F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
        CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
        F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
        F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);

        // - Open.
        F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
        CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
        F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
        F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_OPEN], &V_CSPSA_TEST_ElapsedTime);

        // - Read values {1, 3, 2}, {last, last-2, last-1}, {x, x+2, x+1}.
        {
          uint32_t i;

          F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
          for (i = 0; i < sizeof(ValueKey)/sizeof(ValueKey[0]); i++)
          {
            CSPSA_Result = CSPSA_ReadValue(V_CSPSA_TEST_Handle, ValueKey[i], ValueSize[i], V_CSPSA_TEST_ReadData_p);
            M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result != T_CSPSA_RESULT_OK); // - NOTE: Result should be NOT ok!
          }
          F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
          F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_READ], &V_CSPSA_TEST_ElapsedTime);
        }

        // - Format.
        F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
        CSPSA_Result = CSPSA_Format(V_CSPSA_TEST_Handle);
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
        F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
        F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FORMAT], &V_CSPSA_TEST_ElapsedTime);

        // - Close.
        F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
        CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
        F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
        F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);
      }
    }
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (b) Update some of the values that are to be deleted so that they end up in item list.
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  TestDataSize = 4*1024;
  V_CSPSA_TEST_ConstSize = 43;
  TestType = T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_CONST;
  {
    //printf(D_CSPSA_TEST_MODULE_HEADER "TestType(%s)/TestDataSize(%u).\n", F_CSPSA_TEST_PrintTestType(TestType), TestDataSize);

    // - Create CSPSA in cache.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CREATE], &V_CSPSA_TEST_ElapsedTime);

    // - Write values.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_1ST], &V_CSPSA_TEST_ElapsedTime);

    // - Flush.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FLUSH], &V_CSPSA_TEST_ElapsedTime);

    // - Close.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);

    // - Open.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_OPEN], &V_CSPSA_TEST_ElapsedTime);

    // - Get first three keys.
    i = 0;
    CSPSA_Result = CSPSA_GetSizeOfFirstValue(V_CSPSA_TEST_Handle, &TestKey, &ValueSize[i]);
    ValueKey[i++] = TestKey;
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    CSPSA_Result = CSPSA_GetSizeOfNextValue(V_CSPSA_TEST_Handle, &TestKey, &ValueSize[i]);
    ValueKey[i++] = TestKey;
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    CSPSA_Result = CSPSA_GetSizeOfNextValue(V_CSPSA_TEST_Handle, &TestKey, &ValueSize[i]);
    ValueKey[i++] = TestKey;
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    // - Skip one item.
    CSPSA_Result = CSPSA_GetSizeOfNextValue(V_CSPSA_TEST_Handle, &TestKey, &ValueSize[i]);
    CSPSA_Result = CSPSA_GetSizeOfNextValue(V_CSPSA_TEST_Handle, &TestKey, &ValueSize[i]);
    ValueKey[i++] = TestKey;
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    CSPSA_Result = CSPSA_GetSizeOfNextValue(V_CSPSA_TEST_Handle, &TestKey, &ValueSize[i]);
    ValueKey[i++] = TestKey;
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    CSPSA_Result = CSPSA_GetSizeOfNextValue(V_CSPSA_TEST_Handle, &TestKey, &ValueSize[i]);
    ValueKey[i++] = TestKey;
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    // - Get last three items.
    CSPSA_Result = CSPSA_GetSizeOfNextValue(V_CSPSA_TEST_Handle, &TestKey, &ValueSize[i]);
    ValueKey[i++] = TestKey;
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    CSPSA_Result = CSPSA_GetSizeOfNextValue(V_CSPSA_TEST_Handle, &TestKey, &ValueSize[i]);
    ValueKey[i++] = TestKey;
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    CSPSA_Result = CSPSA_GetSizeOfNextValue(V_CSPSA_TEST_Handle, &TestKey, &ValueSize[i]);
    ValueKey[i++] = TestKey;
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    while (TestKey < D_CSPSA_CORE_FIRST_FAST_PARAMETER_KEY)
    {
      ValueKey[6] = ValueKey[7];
      ValueKey[7] = ValueKey[8];
      ValueKey[8] = TestKey;
      ValueSize[6] = ValueSize[7];
      ValueSize[7] = ValueSize[8];
      ValueSize[8] = TestSize;
      CSPSA_Result = CSPSA_GetSizeOfNextValue(V_CSPSA_TEST_Handle, &TestKey, &TestSize);
    }

    // - Read values.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = F_CSPSA_TEST_ReadAndVerify(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_READ], &V_CSPSA_TEST_ElapsedTime);

    // - Delete values {1, 3, 2}, {last, last-2, last-1}, {x, x+2, x+1}.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_DeleteValue(V_CSPSA_TEST_Handle, ValueKey[0]);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    CSPSA_Result = CSPSA_DeleteValue(V_CSPSA_TEST_Handle, ValueKey[2]);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    CSPSA_Result = CSPSA_DeleteValue(V_CSPSA_TEST_Handle, ValueKey[1]);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    CSPSA_Result = CSPSA_DeleteValue(V_CSPSA_TEST_Handle, ValueKey[3]);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    CSPSA_Result = CSPSA_DeleteValue(V_CSPSA_TEST_Handle, ValueKey[5]);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    CSPSA_Result = CSPSA_DeleteValue(V_CSPSA_TEST_Handle, ValueKey[4]);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    CSPSA_Result = CSPSA_DeleteValue(V_CSPSA_TEST_Handle, ValueKey[8]);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    CSPSA_Result = CSPSA_DeleteValue(V_CSPSA_TEST_Handle, ValueKey[6]);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    CSPSA_Result = CSPSA_DeleteValue(V_CSPSA_TEST_Handle, ValueKey[7]);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_DELETE], &V_CSPSA_TEST_ElapsedTime);

    // - Flush.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FLUSH], &V_CSPSA_TEST_ElapsedTime);

    // - Close.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);

    // - Open.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_OPEN], &V_CSPSA_TEST_ElapsedTime);

    // - Read values {1, 3, 2}, {last, last-2, last-1}, {x, x+2, x+1}.
    {
      uint32_t i;

      F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
      for (i = 0; i < sizeof(ValueKey)/sizeof(ValueKey[0]); i++)
      {
        CSPSA_Result = CSPSA_ReadValue(V_CSPSA_TEST_Handle, ValueKey[i], ValueSize[i], V_CSPSA_TEST_ReadData_p);
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result != T_CSPSA_RESULT_OK); // - NOTE: Result should NOT be ok!
      }
      F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
      F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_READ], &V_CSPSA_TEST_ElapsedTime);
    }

    // - Format.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Format(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FORMAT], &V_CSPSA_TEST_ElapsedTime);

    // - Close.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (c) Try to remove all items.
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  TestDataSize = 4*1024;
  V_CSPSA_TEST_ConstSize = 43;
  TestType = T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_CONST;
  CSPSA_Result =
    F_CSPSA_TEST_GetInitalParameterValues(
      V_CSPSA_TEST_Handle,
      TestType,
      TestDataSize,
      &ValueKey[0],
      &ValueSize[0]
    );
  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
  {
    //printf(D_CSPSA_TEST_MODULE_HEADER "TestType(%s)/TestDataSize(%u).\n", F_CSPSA_TEST_PrintTestType(TestType), TestDataSize);

    // - Create CSPSA in cache.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CREATE], &V_CSPSA_TEST_ElapsedTime);

    // - Write values.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_1ST], &V_CSPSA_TEST_ElapsedTime);

    // - Flush.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FLUSH], &V_CSPSA_TEST_ElapsedTime);

    // - Close.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);

    // - Open.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_OPEN], &V_CSPSA_TEST_ElapsedTime);

    // - Read values.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = F_CSPSA_TEST_ReadAndVerify(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_READ], &V_CSPSA_TEST_ElapsedTime);

    // - Delete all items.
    CSPSA_Result = CSPSA_GetSizeOfFirstValue(V_CSPSA_TEST_Handle, &TestKey, &TestSize);
    while ((CSPSA_Result == T_CSPSA_RESULT_OK) && (TestKey < D_CSPSA_CORE_FIRST_FAST_PARAMETER_KEY))
    {
      F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
      CSPSA_Result = CSPSA_DeleteValue(V_CSPSA_TEST_Handle, TestKey);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
      F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
      F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_DELETE], &V_CSPSA_TEST_ElapsedTime);
      TestKey = ValueKey[0];
      CSPSA_Result = CSPSA_GetSizeOfFirstValue(V_CSPSA_TEST_Handle, &TestKey, &TestSize);
    }

    // - Flush.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FLUSH], &V_CSPSA_TEST_ElapsedTime);

    // - Close.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);

    // - Open.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_OPEN], &V_CSPSA_TEST_ElapsedTime);

    // - Read values {1, 3, 2}, {last, last-2, last-1}, {x, x+2, x+1}.
    {
      uint32_t i;

      F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
      for (i = 0; i < sizeof(ValueKey)/sizeof(ValueKey[0]); i++)
      {
        CSPSA_Result = CSPSA_ReadValue(V_CSPSA_TEST_Handle, ValueKey[i], ValueSize[i], V_CSPSA_TEST_ReadData_p);
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result != T_CSPSA_RESULT_OK); // - NOTE: Result should be NOT ok!
      }
      F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
      F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_READ], &V_CSPSA_TEST_ElapsedTime);
    }

    // - Write values.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_1ST], &V_CSPSA_TEST_ElapsedTime);

    // - Format.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Format(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FORMAT], &V_CSPSA_TEST_ElapsedTime);

    // - Close.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);
  }
  F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_TotalStartTime, &V_CSPSA_TEST_TotalElapsedTime);

  CurrentTestCase_p->TestOk = true;

ErrorExit:

  if (CurrentTestCase_p->TcStatus != T_CSPSA_TEST_TC_STATUS_NOT_IMPLEMENTED)
  {
    if (CurrentTestCase_p->TestOk)
    {
      sprintf(V_CSPSA_TEST_TempString_p, "Ok.");
      sprintf(V_CSPSA_TEST_TempString_p+strlen(V_CSPSA_TEST_TempString_p),
        ";%lu;%lu;%lu;%lu;%lu;%lu;%lu;%lu;%lu;%lu\n",
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TotalElapsedTime),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_OPEN]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_READ]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_1ST]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_2ND]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FLUSH]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FORMAT]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CREATE]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_DELETE])
      );
    }
    else
    {
      sprintf(
        V_CSPSA_TEST_TempString_p,
        "Failed;Line %u;TestDataSize=%u;TestType=%u",
        V_CSPSA_TEST_ErrorLine,
        TestDataSize,
        TestType
      );
      if (V_CSPSA_TEST_Handle != 0)
      {
        (void) CSPSA_Close(&V_CSPSA_TEST_Handle);
      }
    }

    F_CSPSA_TEST_AppendTestCaseResult(TestCaseTableIndex, V_CSPSA_TEST_TempString_p);

  }
}

// -----------------------------------------------------------------------------

// - CSPSA_TEST_TC_3
//
//   § Get error result if a CSPSA image cannot be written
//   § All CSPSA function call durations should be summed up and presented as a test result
//
//  (a) Try to write an image that is larger than half the storage area that is not read-only.
//  (b) Try to create an image that is larger than half the storage area that is  read-only.

void CSPSA_TEST_TC_3(const uint32_t TestCaseTableIndex)
{
  CSPSA_TEST_TC_t* CurrentTestCase_p = &(CSPSA_TEST_TC_Table[TestCaseTableIndex]);
  uint32_t TestDataSize;
  CSPSA_Result_t CSPSA_Result;
  CSPSA_TEST_TestType_t TestType;

  // - Clear all time measurment values.
  memset((uint8_t*)&V_CSPSA_TEST_TimeMeasurement, 0, sizeof(V_CSPSA_TEST_TimeMeasurement));

  // - Initialize.

#if (CFG_CSPSA_TEST_TARGET == CFG_CSPSA_TEST_TARGET_LINUX_US) && (CFG_CSPSA_TEST_VARIANT == CFG_CSPSA_TEST_VARIANT_CORE)
  CurrentTestCase_p->TcStatus = T_CSPSA_TEST_TC_STATUS_VALID;
#else
  CurrentTestCase_p->TcStatus = T_CSPSA_TEST_TC_STATUS_NOT_APPLICABLE;
#endif
  V_CSPSA_TEST_ErrorLine = 0;
  F_CSPSA_TEST_ResetString(V_CSPSA_TEST_TempString_p);
  if (CurrentTestCase_p->TcStatus == T_CSPSA_TEST_TC_STATUS_NOT_APPLICABLE)
  {
    goto ErrorExit;
  }

  F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_TotalStartTime);

  V_CSPSA_TEST_ConstSize = D_CSPSA_TEST_TEST_DATA_SIZE;
  TestDataSize = V_CSPSA_TEST_Config.SizeInBytes / 2;
  TestType = T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_CONST;

  // - Create CSPSA in cache.
  F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
  F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
  F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CREATE], &V_CSPSA_TEST_ElapsedTime);

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (a) Try to write an image that is larger than half the storage area that is not read-only.
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - First write an image that can be flushed to an empty area.
  {
    //printf(D_CSPSA_TEST_MODULE_HEADER "TestType(%s)/TestDataSize(%u).\n", F_CSPSA_TEST_PrintTestType(TestType), TestDataSize);

    // - Write values.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_1ST], &V_CSPSA_TEST_ElapsedTime);

    // - Flush.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result != T_CSPSA_RESULT_OK); // - Failure expected.
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FLUSH], &V_CSPSA_TEST_ElapsedTime);

    // - Format.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Format(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FORMAT], &V_CSPSA_TEST_ElapsedTime);

    // - Close.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);
  }

  F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_TotalStartTime, &V_CSPSA_TEST_TotalElapsedTime);

  CurrentTestCase_p->TestOk = true;

ErrorExit:

  if (CurrentTestCase_p->TcStatus != T_CSPSA_TEST_TC_STATUS_NOT_APPLICABLE)
  {
    if (CurrentTestCase_p->TestOk)
    {
      sprintf(V_CSPSA_TEST_TempString_p, "Ok.");
      sprintf(V_CSPSA_TEST_TempString_p+strlen(V_CSPSA_TEST_TempString_p),
        ";%lu;%lu;%lu;%lu;%lu;%lu;%lu;%lu;%lu;%lu\n",
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TotalElapsedTime),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_OPEN]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_READ]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_1ST]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_2ND]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FLUSH]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FORMAT]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CREATE]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_DELETE])
      );
    }
    else
    {
      sprintf(
        V_CSPSA_TEST_TempString_p,
        "Failed;Line %u;TestDataSize=%u;TestType=%u",
        V_CSPSA_TEST_ErrorLine,
        TestDataSize,
        TestType
      );
    }

    F_CSPSA_TEST_AppendTestCaseResult(TestCaseTableIndex, V_CSPSA_TEST_TempString_p);

  }
}

// -----------------------------------------------------------------------------

// - CSPSA_TEST_TC_4
//
//   § Manipulate one bit in the CSPSA data field and make sure CSPSA detects it
//   § All CSPSA function call durations should be summed up and presented as a test result
//
//  (a) Manipulate one bit in data that is not a key or size field.
//  (b) Manipulate one bit in data that is a key field.
//  (c) Manipulate one bit in data that is a size field.
//  (d) Manipulate one bit in data that is a size field.

void CSPSA_TEST_TC_4(const uint32_t TestCaseTableIndex)
{
  CSPSA_TEST_TC_t* CurrentTestCase_p = &(CSPSA_TEST_TC_Table[TestCaseTableIndex]);
  uint32_t TestDataSize;
  CSPSA_Result_t CSPSA_Result;
  CSPSA_TEST_TestType_t TestType;
  uint32_t BlockSize = V_CSPSA_TEST_Config.BlockSize;
  uint32_t SectorSize = V_CSPSA_TEST_Config.BlockSize;

  // - Clear all time measurment values.
  memset((uint8_t*)&V_CSPSA_TEST_TimeMeasurement, 0, sizeof(V_CSPSA_TEST_TimeMeasurement));

  // - Initialize.

#if (CFG_CSPSA_TEST_TARGET == CFG_CSPSA_TEST_TARGET_LINUX_US) && (CFG_CSPSA_TEST_VARIANT == CFG_CSPSA_TEST_VARIANT_CORE)
  CurrentTestCase_p->TcStatus = T_CSPSA_TEST_TC_STATUS_VALID;
#else
  CurrentTestCase_p->TcStatus = T_CSPSA_TEST_TC_STATUS_NOT_APPLICABLE;
#endif
  V_CSPSA_TEST_ErrorLine = 0;
  F_CSPSA_TEST_ResetString(V_CSPSA_TEST_TempString_p);
  if (CurrentTestCase_p->TcStatus == T_CSPSA_TEST_TC_STATUS_NOT_APPLICABLE)
  {
    goto ErrorExit;
  }

  F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_TotalStartTime);

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (a) Manipulate one bit in data that is not a key or size field.
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  {
	  // - Create CSPSA in cache.
	  F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
	  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
	  F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
	  F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CREATE], &V_CSPSA_TEST_ElapsedTime);

	  V_CSPSA_TEST_ConstSize = 12;
	  TestType = T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_CONST;
	  TestDataSize = 4 * 1024;

    // - Write values.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_1ST], &V_CSPSA_TEST_ElapsedTime);

    // - Flush.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FLUSH], &V_CSPSA_TEST_ElapsedTime);

    // - Close.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);

    // - Manipulate one bit in data.
    {
      CSPSA_CORE_Header_t* Header_p = NULL;
      uint32_t Offset = 0;
      bool ReadBackwards = false;
  		uint32_t HeaderSize;
  		CSPSA_TEST_DataItem_t* DataItem_p;
  		uint8_t* TempData_p;
    	uint32_t Size32Aligned;

    	// - Use a fake session info struct to be able to open ll driver.
    	V_CSPSA_TEST_SessionInfo.Config = V_CSPSA_TEST_Config;
    	V_CSPSA_TEST_Handle = (CSPSA_Handle_t) &V_CSPSA_TEST_SessionInfo;
      F_CSPSA_TEST_InitFlashMemory(&V_CSPSA_TEST_SessionInfo.Config);

    	// - Open a low level driver.
    	CSPSA_Result = CSPSA_LL_Open(V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

      memset(V_CSPSA_TEST_ReadData_p, 0, sizeof(V_CSPSA_TEST_ReadData_p));

      CSPSA_Result = CSPSA_LL_Read(V_CSPSA_TEST_Handle, Offset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

  		Header_p = (CSPSA_CORE_Header_t*) (ptrdiff_t) V_CSPSA_TEST_ReadData_p;
  		HeaderSize = sizeof(*Header_p) + Header_p->BbtSize;
      M_CSPSA_TEST_ASSERT_ERROR_LINE((HeaderSize >= sizeof(*Header_p)) && (HeaderSize <= CFG_CSPSA_CORE_STORAGE_IMAGE_HEADER_MAX_SIZE * SectorSize));
  		DataItem_p = (CSPSA_TEST_DataItem_t*) ((uint8_t*) Header_p + HeaderSize);
  		Size32Aligned = M_CSPSA_TEST_ALIGN32(sizeof(*DataItem_p) + DataItem_p->Size);

      // - Verify header.
      {
        uint32_t CrcValue = C_CSPSA_TEST_HeaderMagic;

        CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->Magic, sizeof(Header_p->Magic));
        CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->Version, sizeof(Header_p->Version));
        CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->Index, sizeof(Header_p->Index));
        CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->Flags, sizeof(Header_p->Flags));
        CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->DataSize, sizeof(Header_p->DataSize));
        CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->Crc32Data, sizeof(Header_p->Crc32Data));
        CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->FastParameter, sizeof(Header_p->FastParameter));
        CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->Reserved, sizeof(Header_p->Reserved));
        CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->BbtSize, sizeof(Header_p->BbtSize));
        if (Header_p->BbtSize > 0)
        {
          CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->Bbt, Header_p->BbtSize);
        }
        M_CSPSA_TEST_ASSERT_ERROR_LINE(Header_p->Crc32Header == CrcValue);
      }

    	if ((Header_p->Flags & T_CSPSA_TEST_HEADERFLAGS_DATA_AT_END) != 0)
    	{
    	  ReadBackwards = true;
    	  Offset = V_CSPSA_TEST_Config.SizeInBytes - BlockSize + HeaderSize;
        memset(V_CSPSA_TEST_ReadData_p, 0, sizeof(V_CSPSA_TEST_ReadData_p));

        CSPSA_Result = CSPSA_LL_Read(V_CSPSA_TEST_Handle, Offset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

  		  DataItem_p = (CSPSA_TEST_DataItem_t*) ((uint8_t*) Header_p + HeaderSize);
    	}

    	// - "Randomly" pick the third item. Assume it's within the BlockSize we read
    	//   out from flash.
    	DataItem_p = (CSPSA_TEST_DataItem_t*)((uint8_t*)DataItem_p + Size32Aligned);
    	Size32Aligned = M_CSPSA_TEST_ALIGN32(sizeof(*DataItem_p) + DataItem_p->Size);
    	DataItem_p = (CSPSA_TEST_DataItem_t*)((uint8_t*)DataItem_p + Size32Aligned);
    	TempData_p = &DataItem_p->Data_p[0];
    	// - "Randomly" XOR bit #3.
    	*TempData_p = *TempData_p ^ 0x08;

    	// - Write back the faulty data.
      CSPSA_Result = CSPSA_LL_Write(V_CSPSA_TEST_Handle, Offset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    	// - Close the low level driver.
    	CSPSA_Result = CSPSA_LL_Close(V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    }

    // - Open.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_E_NO_VALID_IMAGE); // - Failure is expected.
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_OPEN], &V_CSPSA_TEST_ElapsedTime);
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (b) Manipulate one bit in data that is a key field.
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  {
	  // - Create CSPSA in cache.
	  F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
	  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
	  F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
	  F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CREATE], &V_CSPSA_TEST_ElapsedTime);

	  V_CSPSA_TEST_ConstSize = 12;
	  TestType = T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_CONST;
	  TestDataSize = 4 * 1024;

    // - Write values.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_1ST], &V_CSPSA_TEST_ElapsedTime);

    // - Flush.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FLUSH], &V_CSPSA_TEST_ElapsedTime);

    // - Close.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);

    // - Manipulate one bit in key.
    {
      CSPSA_CORE_Header_t* Header_p = NULL;
      uint32_t Offset = 0;
      bool ReadBackwards = false;
  		uint32_t HeaderSize;
  		CSPSA_TEST_DataItem_t* DataItem_p;
  		CSPSA_Key_t* TempData_p;
    	uint32_t Size32Aligned;

    	// - Use a fake session info struct to be able to open ll driver.
    	V_CSPSA_TEST_SessionInfo.Config = V_CSPSA_TEST_Config;
    	V_CSPSA_TEST_Handle = (CSPSA_Handle_t) &V_CSPSA_TEST_SessionInfo;
      F_CSPSA_TEST_InitFlashMemory(&V_CSPSA_TEST_SessionInfo.Config);

    	// - Open a low level driver.
    	CSPSA_Result = CSPSA_LL_Open(V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

      memset(V_CSPSA_TEST_ReadData_p, 0, sizeof(V_CSPSA_TEST_ReadData_p));

      CSPSA_Result = CSPSA_LL_Read(V_CSPSA_TEST_Handle, Offset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

  		Header_p = (CSPSA_CORE_Header_t*) (ptrdiff_t) V_CSPSA_TEST_ReadData_p;
  		HeaderSize = sizeof(*Header_p) + Header_p->BbtSize;
  		DataItem_p = (CSPSA_TEST_DataItem_t*) ((uint8_t*) Header_p + HeaderSize);
  		Size32Aligned = M_CSPSA_TEST_ALIGN32(sizeof(*DataItem_p) + DataItem_p->Size);

    	if ((Header_p->Flags & T_CSPSA_TEST_HEADERFLAGS_DATA_AT_END) != 0)
    	{
    	  ReadBackwards = true;
    	  Offset = V_CSPSA_TEST_Config.SizeInBytes - BlockSize + HeaderSize;
        memset(V_CSPSA_TEST_ReadData_p, 0, sizeof(V_CSPSA_TEST_ReadData_p));

        CSPSA_Result = CSPSA_LL_Read(V_CSPSA_TEST_Handle, Offset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

  		  DataItem_p = (CSPSA_TEST_DataItem_t*) ((uint8_t*) Header_p + HeaderSize);
    	}

    	// - "Randomly" pick the third item. Assume it's within the BlockSize we read
    	//   out from flash.
    	DataItem_p = (CSPSA_TEST_DataItem_t*)((uint8_t*)DataItem_p + Size32Aligned);
    	Size32Aligned = M_CSPSA_TEST_ALIGN32(sizeof(*DataItem_p) + DataItem_p->Size);
    	DataItem_p = (CSPSA_TEST_DataItem_t*)((uint8_t*)DataItem_p + Size32Aligned);
    	TempData_p = (CSPSA_Key_t*) &DataItem_p->Key;
    	// - "Randomly" XOR bit #3.
    	*TempData_p = *TempData_p ^ 0x08;

    	// - Write back the faulty data.
      CSPSA_Result = CSPSA_LL_Write(V_CSPSA_TEST_Handle, Offset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    	CSPSA_Result = CSPSA_LL_Close(V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    }

    // - Open.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_E_NO_VALID_IMAGE); // - Failure is expected.
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_OPEN], &V_CSPSA_TEST_ElapsedTime);
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (c) Manipulate one bit in data that is a size field.
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  {
	  // - Create CSPSA in cache.
	  F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
	  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
	  F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
	  F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CREATE], &V_CSPSA_TEST_ElapsedTime);

	  V_CSPSA_TEST_ConstSize = 12;
	  TestType = T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_CONST;
	  TestDataSize = 4 * 1024;

    // - Write values.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_1ST], &V_CSPSA_TEST_ElapsedTime);

    // - Flush.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FLUSH], &V_CSPSA_TEST_ElapsedTime);

    // - Close.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);

    // - Manipulate one bit in size.
    {
      CSPSA_CORE_Header_t* Header_p = NULL;
      uint32_t Offset = 0;
      bool ReadBackwards = false;
  		uint32_t HeaderSize;
  		CSPSA_TEST_DataItem_t* DataItem_p;
  		CSPSA_Size_t* TempData_p;
    	uint32_t Size32Aligned;

    	// - Use a fake session info struct to be able to open ll driver.
    	V_CSPSA_TEST_SessionInfo.Config = V_CSPSA_TEST_Config;
    	V_CSPSA_TEST_Handle = (CSPSA_Handle_t) &V_CSPSA_TEST_SessionInfo;
      F_CSPSA_TEST_InitFlashMemory(&V_CSPSA_TEST_SessionInfo.Config);

    	// - Open a low level driver.
    	CSPSA_Result = CSPSA_LL_Open(V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

      memset(V_CSPSA_TEST_ReadData_p, 0, sizeof(V_CSPSA_TEST_ReadData_p));

      CSPSA_Result = CSPSA_LL_Read(V_CSPSA_TEST_Handle, Offset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

  		Header_p = (CSPSA_CORE_Header_t*) (ptrdiff_t) V_CSPSA_TEST_ReadData_p;
  		HeaderSize = sizeof(*Header_p) + Header_p->BbtSize;
  		DataItem_p = (CSPSA_TEST_DataItem_t*) ((uint8_t*) Header_p + HeaderSize);
  		Size32Aligned = M_CSPSA_TEST_ALIGN32(sizeof(*DataItem_p) + DataItem_p->Size);

    	if ((Header_p->Flags & T_CSPSA_TEST_HEADERFLAGS_DATA_AT_END) != 0)
    	{
    	  ReadBackwards = true;
    	  Offset = V_CSPSA_TEST_Config.SizeInBytes - BlockSize + HeaderSize;
        memset(V_CSPSA_TEST_ReadData_p, 0, sizeof(V_CSPSA_TEST_ReadData_p));

        CSPSA_Result = CSPSA_LL_Read(V_CSPSA_TEST_Handle, Offset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

  		  DataItem_p = (CSPSA_TEST_DataItem_t*) ((uint8_t*) Header_p + HeaderSize);
    	}

    	// - "Randomly" pick the third item. Assume it's within the BlockSize we read
    	//   out from flash.
    	DataItem_p = (CSPSA_TEST_DataItem_t*)((uint8_t*)DataItem_p + Size32Aligned);
    	Size32Aligned = M_CSPSA_TEST_ALIGN32(sizeof(*DataItem_p) + DataItem_p->Size);
    	DataItem_p = (CSPSA_TEST_DataItem_t*)((uint8_t*)DataItem_p + Size32Aligned);
    	TempData_p = (CSPSA_Size_t*) &DataItem_p->Size;
    	// - "Randomly" make size very large. See if the image loader can cope with that without crashing...
    	*TempData_p = *TempData_p | 0x80000000;

    	// - Write back the faulty data.
      CSPSA_Result = CSPSA_LL_Write(V_CSPSA_TEST_Handle, Offset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    	CSPSA_Result = CSPSA_LL_Close(V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    }

    // - Open.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_E_NO_VALID_IMAGE); // - Failure is expected.
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_OPEN], &V_CSPSA_TEST_ElapsedTime);

  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (d) Manipulate one bit in data that is a size field.
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  {
	  // - Create CSPSA in cache.
	  F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
	  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
	  F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
	  F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CREATE], &V_CSPSA_TEST_ElapsedTime);

	  V_CSPSA_TEST_ConstSize = 12;
	  TestType = T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_CONST;
	  TestDataSize = 4 * 1024;

    // - Write values.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_1ST], &V_CSPSA_TEST_ElapsedTime);

    // - Flush.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FLUSH], &V_CSPSA_TEST_ElapsedTime);

    // - Close.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);

    // - Manipulate one bit in size.
    {
      CSPSA_CORE_Header_t* Header_p = NULL;
      uint32_t Offset = 0;
      bool ReadBackwards = false;
  		uint32_t HeaderSize;
  		CSPSA_TEST_DataItem_t* DataItem_p;
  		CSPSA_Size_t* TempData_p;
    	uint32_t Size32Aligned;

    	// - Use a fake session info struct to be able to open ll driver.
    	V_CSPSA_TEST_SessionInfo.Config = V_CSPSA_TEST_Config;
    	V_CSPSA_TEST_Handle = (CSPSA_Handle_t) &V_CSPSA_TEST_SessionInfo;
      F_CSPSA_TEST_InitFlashMemory(&V_CSPSA_TEST_SessionInfo.Config);

    	// - Open a low level driver.
    	CSPSA_Result = CSPSA_LL_Open(V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

      memset(V_CSPSA_TEST_ReadData_p, 0, sizeof(V_CSPSA_TEST_ReadData_p));

      CSPSA_Result = CSPSA_LL_Read(V_CSPSA_TEST_Handle, Offset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

  		Header_p = (CSPSA_CORE_Header_t*) (ptrdiff_t) V_CSPSA_TEST_ReadData_p;
  		HeaderSize = sizeof(*Header_p) + Header_p->BbtSize;
  		DataItem_p = (CSPSA_TEST_DataItem_t*) ((uint8_t*) Header_p + HeaderSize);
  		Size32Aligned = M_CSPSA_TEST_ALIGN32(sizeof(*DataItem_p) + DataItem_p->Size);

    	if ((Header_p->Flags & T_CSPSA_TEST_HEADERFLAGS_DATA_AT_END) != 0)
    	{
    	  ReadBackwards = true;
    	  Offset = V_CSPSA_TEST_Config.SizeInBytes - BlockSize + HeaderSize;
        memset(V_CSPSA_TEST_ReadData_p, 0, sizeof(V_CSPSA_TEST_ReadData_p));

        CSPSA_Result = CSPSA_LL_Read(V_CSPSA_TEST_Handle, Offset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

  		  DataItem_p = (CSPSA_TEST_DataItem_t*) ((uint8_t*) Header_p + HeaderSize);
    	}

    	// - "Randomly" pick the third item. Assume it's within the BlockSize we read
    	//   out from flash.
    	DataItem_p = (CSPSA_TEST_DataItem_t*)((uint8_t*)DataItem_p + Size32Aligned);
    	Size32Aligned = M_CSPSA_TEST_ALIGN32(sizeof(*DataItem_p) + DataItem_p->Size);
    	DataItem_p = (CSPSA_TEST_DataItem_t*)((uint8_t*)DataItem_p + Size32Aligned);
    	TempData_p = (CSPSA_Size_t*) &DataItem_p->Size;
    	// - "Randomly" XOR bit #3.
    	*TempData_p = *TempData_p ^ 0x08;

    	// - Write back the faulty data.
      CSPSA_Result = CSPSA_LL_Write(V_CSPSA_TEST_Handle, Offset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    	CSPSA_Result = CSPSA_LL_Close(V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    }

    // - Open.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_E_NO_VALID_IMAGE); // - Failure is expected.
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_OPEN], &V_CSPSA_TEST_ElapsedTime);

  }

  F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_TotalStartTime, &V_CSPSA_TEST_TotalElapsedTime);
  CurrentTestCase_p->TestOk = true;

ErrorExit:

  if (CurrentTestCase_p->TcStatus == T_CSPSA_TEST_TC_STATUS_VALID)
  {
    if (CurrentTestCase_p->TestOk)
    {
      sprintf(V_CSPSA_TEST_TempString_p, "Ok.");
      sprintf(V_CSPSA_TEST_TempString_p+strlen(V_CSPSA_TEST_TempString_p),
        ";%lu;%lu;%lu;%lu;%lu;%lu;%lu;%lu;%lu;%lu\n",
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TotalElapsedTime),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_OPEN]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_READ]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_1ST]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_2ND]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FLUSH]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FORMAT]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CREATE]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_DELETE])
      );
    }
    else
    {
      sprintf(
        V_CSPSA_TEST_TempString_p,
        "Failed;Line %u;TestDataSize=%u;TestType=%u",
        V_CSPSA_TEST_ErrorLine,
        TestDataSize,
        TestType
      );
      if (V_CSPSA_TEST_Handle != 0)
      {
        (void) CSPSA_Close(&V_CSPSA_TEST_Handle);
      }
    }

    F_CSPSA_TEST_AppendTestCaseResult(TestCaseTableIndex, V_CSPSA_TEST_TempString_p);
  }
}

// -----------------------------------------------------------------------------

// - CSPSA_TEST_TC_5
//
//   § Manipulate one bit in one of the CSPSA header fields except in CRC32
//     fields and make sure CSPSA recovers
//   § All CSPSA function call durations should be summed up and presented as a test result
//
//  (a) Manipulate one bit in header "Magic" field at start of storage area.
//  (b) Manipulate one bit in header "Magic" field at end of storage area.
//  (c) Manipulate one bit in "Version" field at start of storage area.
//  (d) Manipulate one bit in "Index" field at start of storage area.
//  (e) Manipulate one bit in "Index" field at end of storage area.
//  (f) Manipulate one bit in "Flags" field at start of storage area.
//  (g) Manipulate one "small" bit in "DataSize" field at start of storage area.
//  (h) Manipulate one "large" bit in "DataSize" field at end of storage area.
//  (i) Manipulate one bit in "Crc32Data" field at start of storage area.
//  (j) Manipulate one bit in "Crc32Data" field at end of storage area.
//  (k) Manipulate one bit in "Crc32Header" field at start of storage area.
//  (l) Manipulate one bit in "Crc32Header" field at end of storage area.
//  (m) Manipulate one bit in "BbtSize" field at start of storage area.

void CSPSA_TEST_TC_5(const uint32_t TestCaseTableIndex)
{
  CSPSA_TEST_TC_t* CurrentTestCase_p = &(CSPSA_TEST_TC_Table[TestCaseTableIndex]);
  uint32_t TestDataSize;
  CSPSA_Result_t CSPSA_Result;
  CSPSA_TEST_TestType_t TestType;
  uint32_t BlockSize = V_CSPSA_TEST_Config.BlockSize;

  // - Clear all time measurment values.
  memset((uint8_t*)&V_CSPSA_TEST_TimeMeasurement, 0, sizeof(V_CSPSA_TEST_TimeMeasurement));

  // - Initialize.

#if (CFG_CSPSA_TEST_TARGET == CFG_CSPSA_TEST_TARGET_LINUX_US) && (CFG_CSPSA_TEST_VARIANT == CFG_CSPSA_TEST_VARIANT_CORE)
  CurrentTestCase_p->TcStatus = T_CSPSA_TEST_TC_STATUS_VALID;
#else
  CurrentTestCase_p->TcStatus = T_CSPSA_TEST_TC_STATUS_NOT_APPLICABLE;
#endif
  V_CSPSA_TEST_ErrorLine = 0;
  F_CSPSA_TEST_ResetString(V_CSPSA_TEST_TempString_p);
  if (CurrentTestCase_p->TcStatus == T_CSPSA_TEST_TC_STATUS_NOT_APPLICABLE)
  {
    goto ErrorExit;
  }

  F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_TotalStartTime);

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (a) Manipulate one bit in header "Magic" field at start of storage area.
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  {
	  // - Create CSPSA in cache.
	  F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
	  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
	  F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
	  F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CREATE], &V_CSPSA_TEST_ElapsedTime);

	  V_CSPSA_TEST_ConstSize = 12;
	  TestType = T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_CONST;
	  TestDataSize = 4 * 1024;

    // - Write values.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_1ST], &V_CSPSA_TEST_ElapsedTime);

    // - Flush.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FLUSH], &V_CSPSA_TEST_ElapsedTime);

    // - Close.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);

    // - Manipulate one bit in header "Magic" field at start of storage area.
    {
      CSPSA_CORE_Header_t* Header_p = NULL;
      uint32_t Offset = 0;
      bool ReadBackwards = false;
  		uint32_t* TempData_p;

    	// - Use a fake session info struct to be able to open ll driver.
    	V_CSPSA_TEST_SessionInfo.Config = V_CSPSA_TEST_Config;
    	V_CSPSA_TEST_Handle = (CSPSA_Handle_t) &V_CSPSA_TEST_SessionInfo;
      F_CSPSA_TEST_InitFlashMemory(&V_CSPSA_TEST_SessionInfo.Config);

    	// - Open a low level driver.
    	CSPSA_Result = CSPSA_LL_Open(V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

      memset(V_CSPSA_TEST_ReadData_p, 0, sizeof(V_CSPSA_TEST_ReadData_p));

      CSPSA_Result = CSPSA_LL_Read(V_CSPSA_TEST_Handle, Offset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

  		Header_p = (CSPSA_CORE_Header_t*) (ptrdiff_t) V_CSPSA_TEST_ReadData_p;

    	TempData_p = &Header_p->Magic;
    	// - "Randomly" XOR bit #3.
    	*TempData_p = *TempData_p ^ 0x08;

    	// - Write back the faulty data.
      CSPSA_Result = CSPSA_LL_Write(V_CSPSA_TEST_Handle, Offset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    	CSPSA_Result = CSPSA_LL_Close(V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    }

    // - Open.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
#if CFG_CSPSA_CORE_ENABLE_EXTRA_HEADER
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_W_INVALID_HEADER_FOUND); // - Failure is expected.
#else
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_E_NO_VALID_IMAGE); // - Failure is expected.
#endif
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_OPEN], &V_CSPSA_TEST_ElapsedTime);

    if (CSPSA_Result < T_CSPSA_RESULT_ERRORS)
    {
      // - Format.
      F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
      CSPSA_Result = CSPSA_Format(V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
      F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
      F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FORMAT], &V_CSPSA_TEST_ElapsedTime);

      // - Close.
      F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
      CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
      F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
      F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);
    }
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (b) Manipulate one bit in header "Magic" field at end of storage area.
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  if (CSPSA_Result < T_CSPSA_RESULT_ERRORS)
  {
	  // - Create CSPSA in cache.
	  F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
	  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
	  F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
	  F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CREATE], &V_CSPSA_TEST_ElapsedTime);

	  V_CSPSA_TEST_ConstSize = 12;
	  TestType = T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_CONST;
	  TestDataSize = 4 * 1024;

    // - Write values.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_1ST], &V_CSPSA_TEST_ElapsedTime);

    // - Flush.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FLUSH], &V_CSPSA_TEST_ElapsedTime);

    // - Close.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);

    // - Manipulate one bit in header "Magic" field at end of storage area.
    {
      CSPSA_CORE_Header_t* Header_p = NULL;
      uint32_t Offset = V_CSPSA_TEST_Config.SizeInBytes - BlockSize;
      bool ReadBackwards = true;
  		uint32_t HeaderSize;
  		CSPSA_TEST_DataItem_t* DataItem_p;
  		uint32_t* TempData_p;
    	uint32_t Size32Aligned;

    	// - Use a fake session info struct to be able to open ll driver.
    	V_CSPSA_TEST_SessionInfo.Config = V_CSPSA_TEST_Config;
    	V_CSPSA_TEST_Handle = (CSPSA_Handle_t) &V_CSPSA_TEST_SessionInfo;
      F_CSPSA_TEST_InitFlashMemory(&V_CSPSA_TEST_SessionInfo.Config);

    	// - Open a low level driver.
    	CSPSA_Result = CSPSA_LL_Open(V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

      memset(V_CSPSA_TEST_ReadData_p, 0, sizeof(V_CSPSA_TEST_ReadData_p));

      CSPSA_Result = CSPSA_LL_Read(V_CSPSA_TEST_Handle, Offset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

  		Header_p = (CSPSA_CORE_Header_t*) (ptrdiff_t) V_CSPSA_TEST_ReadData_p;
  		HeaderSize = sizeof(*Header_p) + Header_p->BbtSize;
  		DataItem_p = (CSPSA_TEST_DataItem_t*) ((uint8_t*) Header_p + HeaderSize);
  		Size32Aligned = M_CSPSA_TEST_ALIGN32(sizeof(*DataItem_p) + DataItem_p->Size);

    	TempData_p = &Header_p->Magic;
    	// - "Randomly" XOR bit #3.
    	*TempData_p = *TempData_p ^ 0x08;

    	// - Write back the faulty data.
      CSPSA_Result = CSPSA_LL_Write(V_CSPSA_TEST_Handle, Offset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    	CSPSA_Result = CSPSA_LL_Close(V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    }

    // - Open.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_W_INVALID_HEADER_FOUND); // - Failure is expected.
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_OPEN], &V_CSPSA_TEST_ElapsedTime);

    // - Format.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Format(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FORMAT], &V_CSPSA_TEST_ElapsedTime);

    // - Close.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (c) Manipulate one bit in "Version" field at start of storage area.
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  {
	  // - Create CSPSA in cache.
	  F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
	  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
	  F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
	  F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CREATE], &V_CSPSA_TEST_ElapsedTime);

	  V_CSPSA_TEST_ConstSize = 12;
	  TestType = T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_CONST;
	  TestDataSize = 4 * 1024;

    // - Write values.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_1ST], &V_CSPSA_TEST_ElapsedTime);

    // - Flush.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FLUSH], &V_CSPSA_TEST_ElapsedTime);

    // - Close.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);

    // - Manipulate one bit in "Version" field at start of storage area.
    {
      CSPSA_CORE_Header_t* Header_p = NULL;
      uint32_t Offset = 0;
      bool ReadBackwards = false;
  		uint32_t* TempData_p;

    	// - Use a fake session info struct to be able to open ll driver.
    	V_CSPSA_TEST_SessionInfo.Config = V_CSPSA_TEST_Config;
    	V_CSPSA_TEST_Handle = (CSPSA_Handle_t) &V_CSPSA_TEST_SessionInfo;
      F_CSPSA_TEST_InitFlashMemory(&V_CSPSA_TEST_SessionInfo.Config);

    	// - Open a low level driver.
    	CSPSA_Result = CSPSA_LL_Open(V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

      memset(V_CSPSA_TEST_ReadData_p, 0, sizeof(V_CSPSA_TEST_ReadData_p));

      CSPSA_Result = CSPSA_LL_Read(V_CSPSA_TEST_Handle, Offset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

  		Header_p = (CSPSA_CORE_Header_t*) (ptrdiff_t) V_CSPSA_TEST_ReadData_p;

    	TempData_p = &Header_p->Version;
    	// - "Randomly" XOR bit #3.
    	*TempData_p = *TempData_p ^ 0x08;

    	// - Write back the faulty data.
      CSPSA_Result = CSPSA_LL_Write(V_CSPSA_TEST_Handle, Offset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    	CSPSA_Result = CSPSA_LL_Close(V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    }

    // - Open.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
#if CFG_CSPSA_CORE_ENABLE_EXTRA_HEADER
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_W_INVALID_HEADER_FOUND); // - Failure is expected.
#else
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_E_NO_VALID_IMAGE); // - Failure is expected.
#endif
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_OPEN], &V_CSPSA_TEST_ElapsedTime);

    if (CSPSA_Result < T_CSPSA_RESULT_ERRORS)
    {
      // - Format.
      F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
      CSPSA_Result = CSPSA_Format(V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
      F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
      F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FORMAT], &V_CSPSA_TEST_ElapsedTime);

      // - Close.
      F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
      CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
      F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
      F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);
    }
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (d) Manipulate one bit in "Index" field at start of storage area.
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  {
	  // - Create CSPSA in cache.
	  F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
	  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
	  F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
	  F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CREATE], &V_CSPSA_TEST_ElapsedTime);

	  V_CSPSA_TEST_ConstSize = 12;
	  TestType = T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_CONST;
	  TestDataSize = 4 * 1024;

    // - Write values.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_1ST], &V_CSPSA_TEST_ElapsedTime);

    // - Flush.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FLUSH], &V_CSPSA_TEST_ElapsedTime);

    // - Close.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);

    // - Manipulate one bit in "Index" field at start of storage area.
    {
      CSPSA_CORE_Header_t* Header_p = NULL;
      uint32_t Offset = 0;
      bool ReadBackwards = false;
  		uint32_t* TempData_p;

    	// - Use a fake session info struct to be able to open ll driver.
    	V_CSPSA_TEST_SessionInfo.Config = V_CSPSA_TEST_Config;
    	V_CSPSA_TEST_Handle = (CSPSA_Handle_t) &V_CSPSA_TEST_SessionInfo;
      F_CSPSA_TEST_InitFlashMemory(&V_CSPSA_TEST_SessionInfo.Config);

    	// - Open a low level driver.
    	CSPSA_Result = CSPSA_LL_Open(V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

      memset(V_CSPSA_TEST_ReadData_p, 0, sizeof(V_CSPSA_TEST_ReadData_p));

      CSPSA_Result = CSPSA_LL_Read(V_CSPSA_TEST_Handle, Offset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

  		Header_p = (CSPSA_CORE_Header_t*) (ptrdiff_t) V_CSPSA_TEST_ReadData_p;

    	TempData_p = &Header_p->Index;
    	// - "Randomly" XOR bit #3.
    	*TempData_p = *TempData_p ^ 0x08;

    	// - Write back the faulty data.
      CSPSA_Result = CSPSA_LL_Write(V_CSPSA_TEST_Handle, Offset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    	CSPSA_Result = CSPSA_LL_Close(V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    }

    // - Open.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
#if CFG_CSPSA_CORE_ENABLE_EXTRA_HEADER
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_W_INVALID_HEADER_FOUND); // - Failure is expected.
#else
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_E_NO_VALID_IMAGE); // - Failure is expected.
#endif
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_OPEN], &V_CSPSA_TEST_ElapsedTime);

    if (CSPSA_Result < T_CSPSA_RESULT_ERRORS)
    {
      // - Format.
      F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
      CSPSA_Result = CSPSA_Format(V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
      F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
      F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FORMAT], &V_CSPSA_TEST_ElapsedTime);

      // - Close.
      F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
      CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
      F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
      F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);
    }
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (e) Manipulate one bit in "Index" field at end of storage area.
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  {
	  // - Create CSPSA in cache.
	  F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
	  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
	  F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
	  F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CREATE], &V_CSPSA_TEST_ElapsedTime);

	  V_CSPSA_TEST_ConstSize = 12;
	  TestType = T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_CONST;
	  TestDataSize = 4 * 1024;

    // - Write values.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_1ST], &V_CSPSA_TEST_ElapsedTime);

    // - Flush.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FLUSH], &V_CSPSA_TEST_ElapsedTime);

    // - Close.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);

    // - Manipulate one bit in "Index" field at end of storage area.
    {
      CSPSA_CORE_Header_t* Header_p = NULL;
      uint32_t Offset = V_CSPSA_TEST_Config.SizeInBytes - BlockSize;
      bool ReadBackwards = true;
  		uint32_t HeaderSize;
  		CSPSA_TEST_DataItem_t* DataItem_p;
  		uint32_t* TempData_p;
    	uint32_t Size32Aligned;

    	// - Use a fake session info struct to be able to open ll driver.
    	V_CSPSA_TEST_SessionInfo.Config = V_CSPSA_TEST_Config;
    	V_CSPSA_TEST_Handle = (CSPSA_Handle_t) &V_CSPSA_TEST_SessionInfo;
      F_CSPSA_TEST_InitFlashMemory(&V_CSPSA_TEST_SessionInfo.Config);

    	// - Open a low level driver.
    	CSPSA_Result = CSPSA_LL_Open(V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

      memset(V_CSPSA_TEST_ReadData_p, 0, sizeof(V_CSPSA_TEST_ReadData_p));

      CSPSA_Result = CSPSA_LL_Read(V_CSPSA_TEST_Handle, Offset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

  		Header_p = (CSPSA_CORE_Header_t*) (ptrdiff_t) V_CSPSA_TEST_ReadData_p;
  		HeaderSize = sizeof(*Header_p) + Header_p->BbtSize;
  		DataItem_p = (CSPSA_TEST_DataItem_t*) ((uint8_t*) Header_p + HeaderSize);
  		Size32Aligned = M_CSPSA_TEST_ALIGN32(sizeof(*DataItem_p) + DataItem_p->Size);

    	TempData_p = &Header_p->Index;
    	// - "Randomly" XOR bit #3.
    	*TempData_p = *TempData_p ^ 0x08;

    	// - Write back the faulty data.
      CSPSA_Result = CSPSA_LL_Write(V_CSPSA_TEST_Handle, Offset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    	CSPSA_Result = CSPSA_LL_Close(V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    }

    // - Open.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
#if CFG_CSPSA_CORE_ENABLE_EXTRA_HEADER
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_W_INVALID_HEADER_FOUND); // - Failure is expected.
#else
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
#endif
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_OPEN], &V_CSPSA_TEST_ElapsedTime);

    if (CSPSA_Result < T_CSPSA_RESULT_ERRORS)
    {
      // - Format.
      F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
      CSPSA_Result = CSPSA_Format(V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
      F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
      F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FORMAT], &V_CSPSA_TEST_ElapsedTime);

      // - Close.
      F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
      CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
      F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
      F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);
    }
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (f) Manipulate one bit in "Flags" field at start of storage area.
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  {
	  // - Create CSPSA in cache.
	  F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
	  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
	  F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
	  F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CREATE], &V_CSPSA_TEST_ElapsedTime);

	  V_CSPSA_TEST_ConstSize = 12;
	  TestType = T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_CONST;
	  TestDataSize = 4 * 1024;

    // - Write values.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_1ST], &V_CSPSA_TEST_ElapsedTime);

    // - Flush.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FLUSH], &V_CSPSA_TEST_ElapsedTime);

    // - Close.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);

    // - Manipulate one bit in "Flags" field at start of storage area.
    {
      CSPSA_CORE_Header_t* Header_p = NULL;
      uint32_t Offset = 0;
      bool ReadBackwards = false;
  		uint32_t* TempData_p;

    	// - Use a fake session info struct to be able to open ll driver.
    	V_CSPSA_TEST_SessionInfo.Config = V_CSPSA_TEST_Config;
    	V_CSPSA_TEST_Handle = (CSPSA_Handle_t) &V_CSPSA_TEST_SessionInfo;
      F_CSPSA_TEST_InitFlashMemory(&V_CSPSA_TEST_SessionInfo.Config);

    	// - Open a low level driver.
    	CSPSA_Result = CSPSA_LL_Open(V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

      memset(V_CSPSA_TEST_ReadData_p, 0, sizeof(V_CSPSA_TEST_ReadData_p));

      CSPSA_Result = CSPSA_LL_Read(V_CSPSA_TEST_Handle, Offset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

  		Header_p = (CSPSA_CORE_Header_t*) (ptrdiff_t) V_CSPSA_TEST_ReadData_p;

    	TempData_p = &Header_p->Flags;
    	// - "Randomly" XOR bit #3.
    	*TempData_p = *TempData_p ^ 0x08;

    	// - Write back the faulty data.
      CSPSA_Result = CSPSA_LL_Write(V_CSPSA_TEST_Handle, Offset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    	CSPSA_Result = CSPSA_LL_Close(V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    }

    // - Open.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
#if CFG_CSPSA_CORE_ENABLE_EXTRA_HEADER
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_W_INVALID_HEADER_FOUND); // - Failure is expected.
#else
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_E_NO_VALID_IMAGE); // - Failure is expected.
#endif
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_OPEN], &V_CSPSA_TEST_ElapsedTime);

    if (CSPSA_Result < T_CSPSA_RESULT_ERRORS)
    {
      // - Format.
      F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
      CSPSA_Result = CSPSA_Format(V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
      F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
      F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FORMAT], &V_CSPSA_TEST_ElapsedTime);

      // - Close.
      F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
      CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
      F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
      F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);
    }
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (g) Manipulate one "small" bit in "DataSize" field at start of storage area.
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  {
	  // - Create CSPSA in cache.
	  F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
	  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
	  F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
	  F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CREATE], &V_CSPSA_TEST_ElapsedTime);

	  V_CSPSA_TEST_ConstSize = 12;
	  TestType = T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_CONST;
	  TestDataSize = 4 * 1024;

    // - Write values.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_1ST], &V_CSPSA_TEST_ElapsedTime);

    // - Flush.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FLUSH], &V_CSPSA_TEST_ElapsedTime);

    // - Close.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);

    // - Manipulate one "small" bit in "DataSize" field at start of storage area.
    {
      CSPSA_CORE_Header_t* Header_p = NULL;
      uint32_t Offset = 0;
      bool ReadBackwards = false;
  		uint32_t* TempData_p;

    	// - Use a fake session info struct to be able to open ll driver.
    	V_CSPSA_TEST_SessionInfo.Config = V_CSPSA_TEST_Config;
    	V_CSPSA_TEST_Handle = (CSPSA_Handle_t) &V_CSPSA_TEST_SessionInfo;
      F_CSPSA_TEST_InitFlashMemory(&V_CSPSA_TEST_SessionInfo.Config);

    	// - Open a low level driver.
    	CSPSA_Result = CSPSA_LL_Open(V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

      memset(V_CSPSA_TEST_ReadData_p, 0, sizeof(V_CSPSA_TEST_ReadData_p));

      CSPSA_Result = CSPSA_LL_Read(V_CSPSA_TEST_Handle, Offset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

  		Header_p = (CSPSA_CORE_Header_t*) (ptrdiff_t) V_CSPSA_TEST_ReadData_p;

    	TempData_p = &Header_p->DataSize;
    	// - "Randomly" XOR bit #3.
    	*TempData_p = *TempData_p ^ 0x08;

    	// - Write back the faulty data.
      CSPSA_Result = CSPSA_LL_Write(V_CSPSA_TEST_Handle, Offset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    	CSPSA_Result = CSPSA_LL_Close(V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    }

    // - Open.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
#if CFG_CSPSA_CORE_ENABLE_EXTRA_HEADER
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_W_INVALID_HEADER_FOUND); // - Failure is expected.
#else
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_E_NO_VALID_IMAGE); // - Failure is expected.
#endif
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_OPEN], &V_CSPSA_TEST_ElapsedTime);

    if (CSPSA_Result < T_CSPSA_RESULT_ERRORS)
    {
      // - Format.
      F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
      CSPSA_Result = CSPSA_Format(V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
      F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
      F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FORMAT], &V_CSPSA_TEST_ElapsedTime);

      // - Close.
      F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
      CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
      F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
      F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);
    }
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (h) Manipulate one "large" bit in "DataSize" field at end of storage area.
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  if (CSPSA_Result < T_CSPSA_RESULT_ERRORS)
  {
	  // - Create CSPSA in cache.
	  F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
	  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
	  F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
	  F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CREATE], &V_CSPSA_TEST_ElapsedTime);

	  V_CSPSA_TEST_ConstSize = 12;
	  TestType = T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_CONST;
	  TestDataSize = 4 * 1024;

    // - Write values.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_1ST], &V_CSPSA_TEST_ElapsedTime);

    // - Flush.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FLUSH], &V_CSPSA_TEST_ElapsedTime);

    // - Close.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);

    // - Manipulate one "large" bit in "DataSize" field at end of storage area.
    {
      CSPSA_CORE_Header_t* Header_p = NULL;
      uint32_t Offset = V_CSPSA_TEST_Config.SizeInBytes - BlockSize;
      bool ReadBackwards = true;
  		uint32_t HeaderSize;
  		CSPSA_TEST_DataItem_t* DataItem_p;
  		uint32_t* TempData_p;
    	uint32_t Size32Aligned;

    	// - Use a fake session info struct to be able to open ll driver.
    	V_CSPSA_TEST_SessionInfo.Config = V_CSPSA_TEST_Config;
    	V_CSPSA_TEST_Handle = (CSPSA_Handle_t) &V_CSPSA_TEST_SessionInfo;
      F_CSPSA_TEST_InitFlashMemory(&V_CSPSA_TEST_SessionInfo.Config);

    	// - Open a low level driver.
    	CSPSA_Result = CSPSA_LL_Open(V_CSPSA_TEST_Handle);

      memset(V_CSPSA_TEST_ReadData_p, 0, sizeof(V_CSPSA_TEST_ReadData_p));
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

      CSPSA_Result = CSPSA_LL_Read(V_CSPSA_TEST_Handle, Offset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

  		Header_p = (CSPSA_CORE_Header_t*) (ptrdiff_t) V_CSPSA_TEST_ReadData_p;
  		HeaderSize = sizeof(*Header_p) + Header_p->BbtSize;
  		DataItem_p = (CSPSA_TEST_DataItem_t*) ((uint8_t*) Header_p + HeaderSize);
  		Size32Aligned = M_CSPSA_TEST_ALIGN32(sizeof(*DataItem_p) + DataItem_p->Size);

    	TempData_p = &Header_p->DataSize;
    	// - "Randomly" XOR bit #3.
    	*TempData_p = *TempData_p ^ 0x80000000;

    	// - Write back the faulty data.
      CSPSA_Result = CSPSA_LL_Write(V_CSPSA_TEST_Handle, Offset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    	CSPSA_Result = CSPSA_LL_Close(V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    }

    // - Open.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
#if CFG_CSPSA_CORE_ENABLE_EXTRA_HEADER
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_W_INVALID_HEADER_FOUND); // - Failure is expected.
#else
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_E_NO_VALID_IMAGE); // - Failure is expected.
#endif
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_OPEN], &V_CSPSA_TEST_ElapsedTime);

    if (CSPSA_Result < T_CSPSA_RESULT_ERRORS)
    {
      // - Format.
      F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
      CSPSA_Result = CSPSA_Format(V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
      F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
      F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FORMAT], &V_CSPSA_TEST_ElapsedTime);

      // - Close.
      F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
      CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
      F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
      F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);
    }
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (i) Manipulate one bit in "Crc32Data" field at start of storage area.
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  {
	  // - Create CSPSA in cache.
	  F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
	  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
	  F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
	  F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CREATE], &V_CSPSA_TEST_ElapsedTime);

	  V_CSPSA_TEST_ConstSize = 12;
	  TestType = T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_CONST;
	  TestDataSize = 4 * 1024;

    // - Write values.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_1ST], &V_CSPSA_TEST_ElapsedTime);

    // - Flush.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FLUSH], &V_CSPSA_TEST_ElapsedTime);

    // - Close.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);

    // - Manipulate one bit in "Crc32Data" field at start of storage area.
    {
      CSPSA_CORE_Header_t* Header_p = NULL;
      uint32_t Offset = 0;
      bool ReadBackwards = false;
  		uint32_t* TempData_p;

    	// - Use a fake session info struct to be able to open ll driver.
    	V_CSPSA_TEST_SessionInfo.Config = V_CSPSA_TEST_Config;
    	V_CSPSA_TEST_Handle = (CSPSA_Handle_t) &V_CSPSA_TEST_SessionInfo;
      F_CSPSA_TEST_InitFlashMemory(&V_CSPSA_TEST_SessionInfo.Config);

    	// - Open a low level driver.
    	CSPSA_Result = CSPSA_LL_Open(V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

      memset(V_CSPSA_TEST_ReadData_p, 0, sizeof(V_CSPSA_TEST_ReadData_p));

      CSPSA_Result = CSPSA_LL_Read(V_CSPSA_TEST_Handle, Offset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

  		Header_p = (CSPSA_CORE_Header_t*) (ptrdiff_t) V_CSPSA_TEST_ReadData_p;

    	TempData_p = &Header_p->Crc32Data;
    	// - "Randomly" XOR bit #3.
    	*TempData_p = *TempData_p ^ 0x08;

    	// - Write back the faulty data.
      CSPSA_Result = CSPSA_LL_Write(V_CSPSA_TEST_Handle, Offset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    	CSPSA_Result = CSPSA_LL_Close(V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    }

    // - Open.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
#if CFG_CSPSA_CORE_ENABLE_EXTRA_HEADER
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_W_INVALID_HEADER_FOUND); // - Failure is expected.
#else
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_E_NO_VALID_IMAGE); // - Failure is expected.
#endif
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_OPEN], &V_CSPSA_TEST_ElapsedTime);

    if (CSPSA_Result < T_CSPSA_RESULT_ERRORS)
    {
      // - Format.
      F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
      CSPSA_Result = CSPSA_Format(V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
      F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
      F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FORMAT], &V_CSPSA_TEST_ElapsedTime);

      // - Close.
      F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
      CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
      F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
      F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);
    }
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (j) Manipulate one bit in "Crc32Data" field at end of storage area.
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  if (CSPSA_Result < T_CSPSA_RESULT_ERRORS)
  {
	  // - Create CSPSA in cache.
	  F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
	  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
	  F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
	  F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CREATE], &V_CSPSA_TEST_ElapsedTime);

	  V_CSPSA_TEST_ConstSize = 12;
	  TestType = T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_CONST;
	  TestDataSize = 4 * 1024;

    // - Write values.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_1ST], &V_CSPSA_TEST_ElapsedTime);

    // - Flush.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FLUSH], &V_CSPSA_TEST_ElapsedTime);

    // - Close.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);

    // - Manipulate one bit in "Crc32Data" field at end of storage area.
    {
      CSPSA_CORE_Header_t* Header_p = NULL;
      uint32_t Offset = V_CSPSA_TEST_Config.SizeInBytes - BlockSize;
      bool ReadBackwards = true;
  		uint32_t* TempData_p;

    	// - Use a fake session info struct to be able to open ll driver.
    	V_CSPSA_TEST_SessionInfo.Config = V_CSPSA_TEST_Config;
    	V_CSPSA_TEST_Handle = (CSPSA_Handle_t) &V_CSPSA_TEST_SessionInfo;
      F_CSPSA_TEST_InitFlashMemory(&V_CSPSA_TEST_SessionInfo.Config);

    	// - Open a low level driver.
    	CSPSA_Result = CSPSA_LL_Open(V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

      memset(V_CSPSA_TEST_ReadData_p, 0, sizeof(V_CSPSA_TEST_ReadData_p));

      CSPSA_Result = CSPSA_LL_Read(V_CSPSA_TEST_Handle, Offset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

  		Header_p = (CSPSA_CORE_Header_t*) (ptrdiff_t) V_CSPSA_TEST_ReadData_p;

    	TempData_p = &Header_p->Crc32Data;
    	// - "Randomly" XOR bit #3.
    	*TempData_p = *TempData_p ^ 0x08;

    	// - Write back the faulty data.
      CSPSA_Result = CSPSA_LL_Write(V_CSPSA_TEST_Handle, Offset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    	CSPSA_Result = CSPSA_LL_Close(V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    }

    // - Open.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_W_INVALID_HEADER_FOUND); // - Failure is expected.
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_OPEN], &V_CSPSA_TEST_ElapsedTime);

    // - Format.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Format(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FORMAT], &V_CSPSA_TEST_ElapsedTime);

    // - Close.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (k) Manipulate one bit in "Crc32Header" field at start of storage area.
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  {
	  // - Create CSPSA in cache.
	  F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
	  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
	  F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
	  F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CREATE], &V_CSPSA_TEST_ElapsedTime);

	  V_CSPSA_TEST_ConstSize = 12;
	  TestType = T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_CONST;
	  TestDataSize = 4 * 1024;

    // - Write values.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_1ST], &V_CSPSA_TEST_ElapsedTime);

    // - Flush.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FLUSH], &V_CSPSA_TEST_ElapsedTime);

    // - Close.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);

    // - Manipulate one bit in "Crc32Header" field at start of storage area.
    {
      CSPSA_CORE_Header_t* Header_p = NULL;
      uint32_t Offset = 0;
      bool ReadBackwards = false;
  		uint32_t* TempData_p;

    	// - Use a fake session info struct to be able to open ll driver.
    	V_CSPSA_TEST_SessionInfo.Config = V_CSPSA_TEST_Config;
    	V_CSPSA_TEST_Handle = (CSPSA_Handle_t) &V_CSPSA_TEST_SessionInfo;
      F_CSPSA_TEST_InitFlashMemory(&V_CSPSA_TEST_SessionInfo.Config);

    	// - Open a low level driver.
    	CSPSA_Result = CSPSA_LL_Open(V_CSPSA_TEST_Handle);

      memset(V_CSPSA_TEST_ReadData_p, 0, sizeof(V_CSPSA_TEST_ReadData_p));
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

      CSPSA_Result = CSPSA_LL_Read(V_CSPSA_TEST_Handle, Offset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

  		Header_p = (CSPSA_CORE_Header_t*) (ptrdiff_t) V_CSPSA_TEST_ReadData_p;

    	TempData_p = &Header_p->Crc32Header;
    	// - "Randomly" XOR bit #3.
    	*TempData_p = *TempData_p ^ 0x08;

    	// - Write back the faulty data.
      CSPSA_Result = CSPSA_LL_Write(V_CSPSA_TEST_Handle, Offset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    	CSPSA_Result = CSPSA_LL_Close(V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    }

    // - Open.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
#if CFG_CSPSA_CORE_ENABLE_EXTRA_HEADER
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_W_INVALID_HEADER_FOUND); // - Failure is expected.
#else
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_E_NO_VALID_IMAGE); // - Failure is expected.
#endif
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_OPEN], &V_CSPSA_TEST_ElapsedTime);

    if (CSPSA_Result < T_CSPSA_RESULT_ERRORS)
    {
      // - Format.
      F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
      CSPSA_Result = CSPSA_Format(V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
      F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
      F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FORMAT], &V_CSPSA_TEST_ElapsedTime);

      // - Close.
      F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
      CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
      F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
      F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);
    }
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (l) Manipulate one bit in "Crc32Header" field at end of storage area.
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  if (CSPSA_Result < T_CSPSA_RESULT_ERRORS)
  {
	  // - Create CSPSA in cache.
	  F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
	  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
	  F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
	  F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CREATE], &V_CSPSA_TEST_ElapsedTime);

	  V_CSPSA_TEST_ConstSize = 12;
	  TestType = T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_CONST;
	  TestDataSize = 4 * 1024;

    // - Write values.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_1ST], &V_CSPSA_TEST_ElapsedTime);

    // - Flush.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FLUSH], &V_CSPSA_TEST_ElapsedTime);

    // - Close.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);

    // - Manipulate one bit in "Crc32Header" field at end of storage area.
    {
      CSPSA_CORE_Header_t* Header_p = NULL;
      uint32_t Offset = V_CSPSA_TEST_Config.SizeInBytes - BlockSize;
      bool ReadBackwards = true;
  		uint32_t* TempData_p;

    	// - Use a fake session info struct to be able to open ll driver.
    	V_CSPSA_TEST_SessionInfo.Config = V_CSPSA_TEST_Config;
    	V_CSPSA_TEST_Handle = (CSPSA_Handle_t) &V_CSPSA_TEST_SessionInfo;
      F_CSPSA_TEST_InitFlashMemory(&V_CSPSA_TEST_SessionInfo.Config);

    	// - Open a low level driver.
    	CSPSA_Result = CSPSA_LL_Open(V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

      memset(V_CSPSA_TEST_ReadData_p, 0, sizeof(V_CSPSA_TEST_ReadData_p));

      CSPSA_Result = CSPSA_LL_Read(V_CSPSA_TEST_Handle, Offset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

  		Header_p = (CSPSA_CORE_Header_t*) (ptrdiff_t) V_CSPSA_TEST_ReadData_p;

    	TempData_p = &Header_p->Crc32Header;
    	// - "Randomly" XOR bit #3.
    	*TempData_p = *TempData_p ^ 0x08;

    	// - Write back the faulty data.
      CSPSA_Result = CSPSA_LL_Write(V_CSPSA_TEST_Handle, Offset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    	CSPSA_Result = CSPSA_LL_Close(V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    }

    // - Open.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_W_INVALID_HEADER_FOUND); // - Failure is expected.
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_OPEN], &V_CSPSA_TEST_ElapsedTime);

    // - Format.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Format(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FORMAT], &V_CSPSA_TEST_ElapsedTime);

    // - Close.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (m) Manipulate one bit in "BbtSize" field at start of storage area.
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  {
	  // - Create CSPSA in cache.
	  F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
	  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
	  F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
	  F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CREATE], &V_CSPSA_TEST_ElapsedTime);

	  V_CSPSA_TEST_ConstSize = 12;
	  TestType = T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_CONST;
	  TestDataSize = 4 * 1024;

    // - Write values.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_1ST], &V_CSPSA_TEST_ElapsedTime);

    // - Flush.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FLUSH], &V_CSPSA_TEST_ElapsedTime);

    // - Close.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);

    // - Manipulate one bit in "BbtSize" field at start of storage area.
    {
      CSPSA_CORE_Header_t* Header_p = NULL;
      uint32_t Offset = 0;
      bool ReadBackwards = false;
  		uint32_t* TempData_p;

    	// - Use a fake session info struct to be able to open ll driver.
    	V_CSPSA_TEST_SessionInfo.Config = V_CSPSA_TEST_Config;
    	V_CSPSA_TEST_Handle = (CSPSA_Handle_t) &V_CSPSA_TEST_SessionInfo;
      F_CSPSA_TEST_InitFlashMemory(&V_CSPSA_TEST_SessionInfo.Config);

    	// - Open a low level driver.
    	CSPSA_Result = CSPSA_LL_Open(V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

      memset(V_CSPSA_TEST_ReadData_p, 0, sizeof(V_CSPSA_TEST_ReadData_p));

      CSPSA_Result = CSPSA_LL_Read(V_CSPSA_TEST_Handle, Offset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

  		Header_p = (CSPSA_CORE_Header_t*) (ptrdiff_t) V_CSPSA_TEST_ReadData_p;

    	TempData_p = &Header_p->BbtSize;
    	// - "Randomly" XOR bit #3.
    	*TempData_p = *TempData_p ^ 0x08;

    	// - Write back the faulty data.
      CSPSA_Result = CSPSA_LL_Write(V_CSPSA_TEST_Handle, Offset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    	CSPSA_Result = CSPSA_LL_Close(V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    }

    // - Open.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
#if CFG_CSPSA_CORE_ENABLE_EXTRA_HEADER
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_W_INVALID_HEADER_FOUND); // - Failure is expected.
#else
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_E_NO_VALID_IMAGE); // - Failure is expected.
#endif
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_OPEN], &V_CSPSA_TEST_ElapsedTime);

    if (CSPSA_Result < T_CSPSA_RESULT_ERRORS)
    {
      // - Format.
      F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
      CSPSA_Result = CSPSA_Format(V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
      F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
      F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FORMAT], &V_CSPSA_TEST_ElapsedTime);

      // - Close.
      F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
      CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
      F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
      F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);
    }
  }

  F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_TotalStartTime, &V_CSPSA_TEST_TotalElapsedTime);
  CurrentTestCase_p->TestOk = true;

ErrorExit:

  if (CurrentTestCase_p->TcStatus != T_CSPSA_TEST_TC_STATUS_NOT_APPLICABLE)
  {
    if (CurrentTestCase_p->TestOk)
    {
      sprintf(V_CSPSA_TEST_TempString_p, "Ok.");
      sprintf(V_CSPSA_TEST_TempString_p+strlen(V_CSPSA_TEST_TempString_p),
        ";%lu;%lu;%lu;%lu;%lu;%lu;%lu;%lu;%lu;%lu\n",
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TotalElapsedTime),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_OPEN]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_READ]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_1ST]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_2ND]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FLUSH]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FORMAT]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CREATE]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_DELETE])
      );
    }
    else
    {
      sprintf(
        V_CSPSA_TEST_TempString_p,
        "Failed;Line %u;TestDataSize=%u;TestType=%u",
        V_CSPSA_TEST_ErrorLine,
        TestDataSize,
        TestType
      );
      if (V_CSPSA_TEST_Handle != 0)
      {
        (void) CSPSA_Close(&V_CSPSA_TEST_Handle);
      }
    }

    F_CSPSA_TEST_AppendTestCaseResult(TestCaseTableIndex, V_CSPSA_TEST_TempString_p);

  }
}

// -----------------------------------------------------------------------------

// - CSPSA_TEST_TC_6
//
//   § Manipulate one bit in both header's CSPSA header fields (one field at a time)
//     and make sure CSPSA fails to open
//   § All CSPSA function call durations should be summed up and presented as a test result
//
//  (a) Manipulate one bit in header "DataSize" field in both headers.

void CSPSA_TEST_TC_6(const uint32_t TestCaseTableIndex)
{
  CSPSA_TEST_TC_t* CurrentTestCase_p = &(CSPSA_TEST_TC_Table[TestCaseTableIndex]);
  uint32_t TestDataSize;
  CSPSA_Result_t CSPSA_Result;
  CSPSA_TEST_TestType_t TestType;
  uint32_t BlockSize = V_CSPSA_TEST_Config.BlockSize;

  // - Clear all time measurment values.
  memset((uint8_t*)&V_CSPSA_TEST_TimeMeasurement, 0, sizeof(V_CSPSA_TEST_TimeMeasurement));

  // - Initialize.

#if (CFG_CSPSA_TEST_TARGET == CFG_CSPSA_TEST_TARGET_LINUX_US) && (CFG_CSPSA_TEST_VARIANT == CFG_CSPSA_TEST_VARIANT_CORE)
  if (CFG_CSPSA_CORE_ENABLE_EXTRA_HEADER)
  {
    CurrentTestCase_p->TcStatus = T_CSPSA_TEST_TC_STATUS_VALID;
  }
  else
  {
    CurrentTestCase_p->TcStatus = T_CSPSA_TEST_TC_STATUS_OBSOLETE;
  }
#else
  CurrentTestCase_p->TcStatus = T_CSPSA_TEST_TC_STATUS_NOT_APPLICABLE;
#endif
  V_CSPSA_TEST_ErrorLine = 0;
  F_CSPSA_TEST_ResetString(V_CSPSA_TEST_TempString_p);
  if ((CurrentTestCase_p->TcStatus == T_CSPSA_TEST_TC_STATUS_NOT_APPLICABLE) || (CurrentTestCase_p->TcStatus == T_CSPSA_TEST_TC_STATUS_OBSOLETE))
  {
    goto ErrorExit;
  }

  F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_TotalStartTime);

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (a) Manipulate one bit in header "DataSize" field in both headers.
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  {
	  // - Create CSPSA in cache.
	  F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
	  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
	  F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
	  F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CREATE], &V_CSPSA_TEST_ElapsedTime);

	  V_CSPSA_TEST_ConstSize = 12;
	  TestType = T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_CONST;
	  TestDataSize = 4 * 1024;

    // - Write values.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_1ST], &V_CSPSA_TEST_ElapsedTime);

    // - Flush.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FLUSH], &V_CSPSA_TEST_ElapsedTime);

    // - Close.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);

    // - Manipulate one bit in header "DataSize" field in both headers.
    {
      CSPSA_CORE_Header_t* Header_p = NULL;
      uint32_t Offset = 0;
      bool ReadBackwards = false;
  		uint32_t* TempData_p;

    	// - Use a fake session info struct to be able to open ll driver.
    	V_CSPSA_TEST_SessionInfo.Config = V_CSPSA_TEST_Config;
    	V_CSPSA_TEST_Handle = (CSPSA_Handle_t) &V_CSPSA_TEST_SessionInfo;
      F_CSPSA_TEST_InitFlashMemory(&V_CSPSA_TEST_SessionInfo.Config);

    	// - Open a low level driver.
    	CSPSA_Result = CSPSA_LL_Open(V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

      memset(V_CSPSA_TEST_ReadData_p, 0, sizeof(V_CSPSA_TEST_ReadData_p));

      CSPSA_Result = CSPSA_LL_Read(V_CSPSA_TEST_Handle, Offset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

  		Header_p = (CSPSA_CORE_Header_t*) (ptrdiff_t) V_CSPSA_TEST_ReadData_p;

    	TempData_p = &Header_p->Magic;
    	// - "Randomly" XOR bit #3.
    	*TempData_p = *TempData_p ^ 0x08;

    	// - Write back the faulty data.
      CSPSA_Result = CSPSA_LL_Write(V_CSPSA_TEST_Handle, Offset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

      Offset = V_CSPSA_TEST_Config.SizeInBytes - BlockSize;
      ReadBackwards = true;

      CSPSA_Result = CSPSA_LL_Read(V_CSPSA_TEST_Handle, Offset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

  		Header_p = (CSPSA_CORE_Header_t*) (ptrdiff_t) V_CSPSA_TEST_ReadData_p;

    	TempData_p = &Header_p->Magic;
    	// - "Randomly" XOR bit #3.
    	*TempData_p = *TempData_p ^ 0x08;

    	// - Write back the faulty data.
      CSPSA_Result = CSPSA_LL_Write(V_CSPSA_TEST_Handle, Offset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    	CSPSA_Result = CSPSA_LL_Close(V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    }

    // - Open.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_E_NO_VALID_IMAGE); // - Failure is expected.
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_OPEN], &V_CSPSA_TEST_ElapsedTime);

  }

  F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_TotalStartTime, &V_CSPSA_TEST_TotalElapsedTime);
  CurrentTestCase_p->TestOk = true;

ErrorExit:

  if (CurrentTestCase_p->TcStatus == T_CSPSA_TEST_TC_STATUS_VALID)
  {
    if (CurrentTestCase_p->TestOk)
    {
      sprintf(V_CSPSA_TEST_TempString_p, "Ok.");
      sprintf(V_CSPSA_TEST_TempString_p+strlen(V_CSPSA_TEST_TempString_p),
        ";%lu;%lu;%lu;%lu;%lu;%lu;%lu;%lu;%lu;%lu\n",
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TotalElapsedTime),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_OPEN]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_READ]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_1ST]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_2ND]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FLUSH]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FORMAT]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CREATE]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_DELETE])
      );
    }
    else
    {
      sprintf(
        V_CSPSA_TEST_TempString_p,
        "Failed;Line %u;TestDataSize=%u;TestType=%u",
        V_CSPSA_TEST_ErrorLine,
        TestDataSize,
        TestType
      );
      if (V_CSPSA_TEST_Handle != 0)
      {
        (void) CSPSA_Close(&V_CSPSA_TEST_Handle);
      }
    }

    F_CSPSA_TEST_AppendTestCaseResult(TestCaseTableIndex, V_CSPSA_TEST_TempString_p);

  }
}

// -----------------------------------------------------------------------------

// - CSPSA_TEST_TC_7
//
//   § Test all API functions with bad input parameters
//
//  (a) CSPSA_Open()
//  (b) CSPSA_Close()
//  (c) CSPSA_GetSizeOfValue()
//  (d) CSPSA_GetSizeOfNextValue()
//  (e) CSPSA_ReadValue()
//  (f) CSPSA_WriteValue()
//  (g) CSPSA_DeleteValue()
//  (h) CSPSA_Flush()
//  (i) CSPSA_Format()
//  (j) CSPSA_Create()

void CSPSA_TEST_TC_7(const uint32_t TestCaseTableIndex)
{
  CSPSA_TEST_TC_t* CurrentTestCase_p = &(CSPSA_TEST_TC_Table[TestCaseTableIndex]);
  uint32_t TestDataSize;
  CSPSA_Result_t CSPSA_Result;
  CSPSA_TEST_TestType_t TestType;

  // - Initialize.

  CurrentTestCase_p->TcStatus = T_CSPSA_TEST_TC_STATUS_VALID;
  V_CSPSA_TEST_ErrorLine = 0;
  F_CSPSA_TEST_ResetString(V_CSPSA_TEST_TempString_p);
  if (CurrentTestCase_p->TcStatus == T_CSPSA_TEST_TC_STATUS_NOT_IMPLEMENTED)
  {
    goto ErrorExit;
  }

  F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_TotalStartTime);

  V_CSPSA_TEST_ConstSize = 12;
  TestType = T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_CONST;
  TestDataSize = 4 * 1024;

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (a) CSPSA_Open()
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  //printf(D_CSPSA_TEST_MODULE_HEADER "CSPSA_TEST_TC_7(%d).\n", __LINE__);
  {
	  // - Create CSPSA in cache.
	  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Write values.
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Flush.
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

		// - Start testing CSPSA_Open() with illegal input parameters.
    CSPSA_Result = CSPSA_Open(NULL, &V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_E_BAD_PARAMETER); // - Failure is expected.

    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, NULL);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_E_BAD_PARAMETER); // - Failure is expected.
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (b) CSPSA_Close()
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  //printf(D_CSPSA_TEST_MODULE_HEADER "CSPSA_TEST_TC_7(%d).\n", __LINE__);
  {
	  // - Create CSPSA in cache.
	  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Write values.
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Flush.
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

		// - Start testing CSPSA_Close() with illegal input parameters.
    CSPSA_Result = CSPSA_Close(NULL);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_E_BAD_PARAMETER);

    // - Format.
    CSPSA_Result = CSPSA_Format(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (c) CSPSA_GetSizeOfValue()
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  //printf(D_CSPSA_TEST_MODULE_HEADER "CSPSA_TEST_TC_7(%d).\n", __LINE__);
  {
  	CSPSA_Key_t Key;
  	CSPSA_Size_t Size;

  	CSPSA_Result =
	  	F_CSPSA_TEST_GetInitalParameterValues(
			  V_CSPSA_TEST_Handle,
			  TestType,
			  TestDataSize,
			  &Key,
			  &Size
			);

	  // - Create CSPSA in cache.
	  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Write values.
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Flush.
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Open
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

		// - Start testing CSPSA_GetSizeOfValue() with illegal input parameters.

    CSPSA_Result = CSPSA_GetSizeOfValue(0, Key, &Size);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_E_BAD_PARAMETER); // - Failure is expected.

    CSPSA_Result = CSPSA_GetSizeOfValue(V_CSPSA_TEST_Handle, Key - 1, &Size);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_E_INVALID_KEY); // - Failure is expected.

    CSPSA_Result = CSPSA_GetSizeOfValue(V_CSPSA_TEST_Handle, Key, NULL);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_E_BAD_PARAMETER); // - Failure is expected.

    // - Format.
    CSPSA_Result = CSPSA_Format(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (d) CSPSA_GetSizeOfNextValue()
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  //printf(D_CSPSA_TEST_MODULE_HEADER "CSPSA_TEST_TC_7(%d).\n", __LINE__);
  {
  	CSPSA_Key_t Key;
  	CSPSA_Size_t Size;

  	CSPSA_Result =
	  	F_CSPSA_TEST_GetInitalParameterValues(
			  V_CSPSA_TEST_Handle,
			  TestType,
			  TestDataSize,
			  &Key,
			  &Size
			);

	  // - Create CSPSA in cache.
	  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Write values.
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Flush.
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Open
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    CSPSA_Result = CSPSA_GetSizeOfValue(V_CSPSA_TEST_Handle, Key, &Size);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

		// - Start testing CSPSA_GetSizeOfValue() with illegal input parameters.

    CSPSA_Result = CSPSA_GetSizeOfNextValue(0, &Key, &Size);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_E_BAD_PARAMETER); // - Failure is expected.

    CSPSA_Result = CSPSA_GetSizeOfNextValue(V_CSPSA_TEST_Handle, NULL, &Size);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_E_BAD_PARAMETER); // - Failure is expected.

    CSPSA_Result = CSPSA_GetSizeOfNextValue(V_CSPSA_TEST_Handle, &Key, NULL);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_E_BAD_PARAMETER); // - Failure is expected.

    do
    {
    	CSPSA_Result = CSPSA_GetSizeOfNextValue(V_CSPSA_TEST_Handle, &Key, &Size);
    } while (CSPSA_Result == T_CSPSA_RESULT_OK);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_E_END_OF_DATA); // - Failure is expected.

    // - Format.
    CSPSA_Result = CSPSA_Format(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (e) CSPSA_ReadValue()
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  //printf(D_CSPSA_TEST_MODULE_HEADER "CSPSA_TEST_TC_7(%d).\n", __LINE__);
  {
  	CSPSA_Key_t Key;
  	CSPSA_Size_t Size;

  	CSPSA_Result =
	  	F_CSPSA_TEST_GetInitalParameterValues(
			  V_CSPSA_TEST_Handle,
			  TestType,
			  TestDataSize,
			  &Key,
			  &Size
			);

	  // - Create CSPSA in cache.
	  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Write values.
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Flush.
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Open
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

		// - Start testing CSPSA_ReadValue() with illegal input parameters.

    CSPSA_Result = CSPSA_ReadValue(0, Key, Size, V_CSPSA_TEST_ReadData_p);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_E_BAD_PARAMETER); // - Failure is expected.

    CSPSA_Result = CSPSA_ReadValue(V_CSPSA_TEST_Handle, Key - 1, Size, V_CSPSA_TEST_ReadData_p);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_E_INVALID_KEY); // - Failure is expected.

    CSPSA_Result = CSPSA_ReadValue(V_CSPSA_TEST_Handle, Key, Size + 1, V_CSPSA_TEST_ReadData_p);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_W_SIZE); // - Failure is expected.

    CSPSA_Result = CSPSA_ReadValue(V_CSPSA_TEST_Handle, Key, Size, NULL);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_E_BAD_PARAMETER); // - Failure is expected.

    // - Format.
    CSPSA_Result = CSPSA_Format(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (f) CSPSA_WriteValue()
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  //printf(D_CSPSA_TEST_MODULE_HEADER "CSPSA_TEST_TC_7(%d).\n", __LINE__);
  {
  	CSPSA_Key_t Key;
  	CSPSA_Size_t Size;

  	CSPSA_Result =
	  	F_CSPSA_TEST_GetInitalParameterValues(
			  V_CSPSA_TEST_Handle,
			  TestType,
			  TestDataSize,
			  &Key,
			  &Size
			);

	  // - Create CSPSA in cache.
	  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Write values.
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Flush.
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Open
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

		// - Start testing CSPSA_WriteValue() with illegal input parameters.

    CSPSA_Result = CSPSA_WriteValue(0, Key, Size, V_CSPSA_TEST_WriteData_p);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_E_BAD_PARAMETER); // - Failure is expected.

    CSPSA_Result = CSPSA_WriteValue(V_CSPSA_TEST_Handle, Key - 1, Size, V_CSPSA_TEST_WriteData_p);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_W_NEW_KEY); // - Failure is expected.

    CSPSA_Result = CSPSA_WriteValue(V_CSPSA_TEST_Handle, Key, Size + 1, V_CSPSA_TEST_WriteData_p);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_W_SIZE); // - Failure is expected.

    CSPSA_Result = CSPSA_WriteValue(V_CSPSA_TEST_Handle, Key, Size, NULL);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_E_BAD_PARAMETER); // - Failure is expected.

    // - Format.
    CSPSA_Result = CSPSA_Format(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (g) CSPSA_DeleteValue()
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  //printf(D_CSPSA_TEST_MODULE_HEADER "CSPSA_TEST_TC_7(%d).\n", __LINE__);
  {
  	CSPSA_Key_t Key;
  	CSPSA_Size_t Size;

  	CSPSA_Result =
	  	F_CSPSA_TEST_GetInitalParameterValues(
			  V_CSPSA_TEST_Handle,
			  TestType,
			  TestDataSize,
			  &Key,
			  &Size
			);

	  // - Create CSPSA in cache.
	  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Write values.
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Flush.
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Open
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

		// - Start testing CSPSA_DeleteValue() with illegal input parameters.

    CSPSA_Result = CSPSA_DeleteValue(0, Key);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_E_BAD_PARAMETER); // - Failure is expected.

    CSPSA_Result = CSPSA_DeleteValue(V_CSPSA_TEST_Handle, Key - 1);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_E_INVALID_KEY); // - Failure is expected.

    // - Format.
    CSPSA_Result = CSPSA_Format(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (h) CSPSA_Flush()
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  //printf(D_CSPSA_TEST_MODULE_HEADER "CSPSA_TEST_TC_7(%d).\n", __LINE__);
  {
  	CSPSA_Key_t Key;
  	CSPSA_Size_t Size;

  	CSPSA_Result =
	  	F_CSPSA_TEST_GetInitalParameterValues(
			  V_CSPSA_TEST_Handle,
			  TestType,
			  TestDataSize,
			  &Key,
			  &Size
			);

	  // - Create CSPSA in cache.
	  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Write values.
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

		// - Start testing CSPSA_Flush() with illegal input parameters.

    CSPSA_Result = CSPSA_Flush(0);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_E_BAD_PARAMETER); // - Failure is expected.

    // - Format.
    CSPSA_Result = CSPSA_Format(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (i) CSPSA_Format()
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  //printf(D_CSPSA_TEST_MODULE_HEADER "CSPSA_TEST_TC_7(%d).\n", __LINE__);
  {
  	CSPSA_Key_t Key;
  	CSPSA_Size_t Size;

  	CSPSA_Result =
	  	F_CSPSA_TEST_GetInitalParameterValues(
			  V_CSPSA_TEST_Handle,
			  TestType,
			  TestDataSize,
			  &Key,
			  &Size
			);

	  // - Create CSPSA in cache.
	  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

		// - Start testing CSPSA_Format() with illegal input parameters.

    CSPSA_Result = CSPSA_Format(0);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_E_BAD_PARAMETER); // - Failure is expected.

    // - Format.
    CSPSA_Result = CSPSA_Format(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (j) CSPSA_Create()
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  //printf(D_CSPSA_TEST_MODULE_HEADER "CSPSA_TEST_TC_7(%d).\n", __LINE__);
  {
  	CSPSA_Key_t Key;
  	CSPSA_Size_t Size;

  	CSPSA_Result =
	  	F_CSPSA_TEST_GetInitalParameterValues(
			  V_CSPSA_TEST_Handle,
			  TestType,
			  TestDataSize,
			  &Key,
			  &Size
			);

		// - Start testing CSPSA_Create() with illegal input parameters.

	  CSPSA_Result = CSPSA_Create(NULL, &V_CSPSA_TEST_Handle);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_E_BAD_PARAMETER); // - Failure is expected.

	  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, NULL);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_E_BAD_PARAMETER); // - Failure is expected.
  }

  F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_TotalStartTime, &V_CSPSA_TEST_TotalElapsedTime);

  CurrentTestCase_p->TestOk = true;

ErrorExit:

  if (CurrentTestCase_p->TcStatus != T_CSPSA_TEST_TC_STATUS_NOT_IMPLEMENTED)
  {
    if (CurrentTestCase_p->TestOk)
    {
      sprintf(V_CSPSA_TEST_TempString_p, "Ok.;%lu", (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TotalElapsedTime));
    }
    else
    {
      sprintf(
        V_CSPSA_TEST_TempString_p,
        "Failed;Line %u;TestDataSize=%u;TestType=%u",
        V_CSPSA_TEST_ErrorLine,
        TestDataSize,
        TestType
      );
      if (V_CSPSA_TEST_Handle != 0)
      {
        (void) CSPSA_Close(&V_CSPSA_TEST_Handle);
      }
    }

    F_CSPSA_TEST_AppendTestCaseResult(TestCaseTableIndex, V_CSPSA_TEST_TempString_p);

  }
}

// -----------------------------------------------------------------------------

// - CSPSA_TEST_TC_8
//
//   § Increase and decrease the size of a CSPSA image significantly and verify it
//
//  (a) Add a large item after first flush.
//  (b) Replace with a large item after first flush.
//  (c) Add a large item before first flush.
//  (d) Replace a large item before first flush.
//  (e) Add and delete a large item before first flush.
//  (f) Replace and delete a large item before first flush.
//  (g) Replace and delete a large item before first flush.
//  (h) Replace and delete a large item (with second Key value).

void CSPSA_TEST_TC_8(const uint32_t TestCaseTableIndex)
{
  CSPSA_TEST_TC_t* CurrentTestCase_p = &(CSPSA_TEST_TC_Table[TestCaseTableIndex]);
  uint32_t TestDataSize;
  CSPSA_Result_t CSPSA_Result;
  CSPSA_TEST_TestType_t TestType;

  // - Initialize.

  CurrentTestCase_p->TcStatus = T_CSPSA_TEST_TC_STATUS_VALID;
  V_CSPSA_TEST_ErrorLine = 0;
  F_CSPSA_TEST_ResetString(V_CSPSA_TEST_TempString_p);
  if (CurrentTestCase_p->TcStatus == T_CSPSA_TEST_TC_STATUS_NOT_IMPLEMENTED)
  {
    goto ErrorExit;
  }

  F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_TotalStartTime);

  V_CSPSA_TEST_ConstSize = 12;
  TestType = T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_CONST;
  TestDataSize = 4 * 1024;

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (a) Add a large item after first flush.
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  {
  	CSPSA_Key_t Key;
  	CSPSA_Size_t Size;

  	CSPSA_Result =
	  	F_CSPSA_TEST_GetInitalParameterValues(
			  V_CSPSA_TEST_Handle,
			  TestType,
			  TestDataSize,
			  &Key,
			  &Size
			);

	  // - Create CSPSA in cache.
	  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Write values.
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Flush.
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Open
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

		// - Add a large item.
    CSPSA_Result = CSPSA_WriteValue(V_CSPSA_TEST_Handle, Key - 1, D_CSPSA_TEST_TEST_DATA_SIZE, F_CSPSA_TEST_GetWriteData());
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result < T_CSPSA_RESULT_ERRORS);

    // - Flush.
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Open
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (b) Replace with a large item after first flush.
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  {
  	CSPSA_Key_t Key;
  	CSPSA_Size_t Size;

  	CSPSA_Result =
	  	F_CSPSA_TEST_GetInitalParameterValues(
			  V_CSPSA_TEST_Handle,
			  TestType,
			  TestDataSize,
			  &Key,
			  &Size
			);

	  // - Create CSPSA in cache.
	  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Write values.
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Flush.
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Open
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

		// - Replace with a large item.
    CSPSA_Result = CSPSA_WriteValue(V_CSPSA_TEST_Handle, Key, D_CSPSA_TEST_TEST_DATA_SIZE, F_CSPSA_TEST_GetWriteData());
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result < T_CSPSA_RESULT_ERRORS);

    // - Flush.
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Open
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (c) Add a large item before first flush.
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  {
  	CSPSA_Key_t Key;
  	CSPSA_Size_t Size;

  	CSPSA_Result =
	  	F_CSPSA_TEST_GetInitalParameterValues(
			  V_CSPSA_TEST_Handle,
			  TestType,
			  TestDataSize,
			  &Key,
			  &Size
			);

	  // - Create CSPSA in cache.
	  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Write values.
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

		// - Add a large item before first flush.
    CSPSA_Result = CSPSA_WriteValue(V_CSPSA_TEST_Handle, Key - 1, D_CSPSA_TEST_TEST_DATA_SIZE, F_CSPSA_TEST_GetWriteData());
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result < T_CSPSA_RESULT_ERRORS);

    // - Flush.
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Open
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (d) Replace a large item before first flush.
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  {
  	CSPSA_Key_t Key;
  	CSPSA_Size_t Size;

  	CSPSA_Result =
	  	F_CSPSA_TEST_GetInitalParameterValues(
			  V_CSPSA_TEST_Handle,
			  TestType,
			  TestDataSize,
			  &Key,
			  &Size
			);

	  // - Create CSPSA in cache.
	  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Write values.
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

		// - Replace with a large item before first flush.
    CSPSA_Result = CSPSA_WriteValue(V_CSPSA_TEST_Handle, Key, D_CSPSA_TEST_TEST_DATA_SIZE, F_CSPSA_TEST_GetWriteData());
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result < T_CSPSA_RESULT_ERRORS);

    // - Flush.
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Open
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (e) Add and delete a large item before first flush.
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  {
  	CSPSA_Key_t Key;
  	CSPSA_Size_t Size;

  	CSPSA_Result =
	  	F_CSPSA_TEST_GetInitalParameterValues(
			  V_CSPSA_TEST_Handle,
			  TestType,
			  TestDataSize,
			  &Key,
			  &Size
			);

	  // - Create CSPSA in cache.
	  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Write values.
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

		// - Add a large item before first flush.
    CSPSA_Result = CSPSA_WriteValue(V_CSPSA_TEST_Handle, Key - 1, D_CSPSA_TEST_TEST_DATA_SIZE, F_CSPSA_TEST_GetWriteData());
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result < T_CSPSA_RESULT_ERRORS);

    // - Delete the newly added value.
    CSPSA_Result = CSPSA_DeleteValue(V_CSPSA_TEST_Handle, Key - 1);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result < T_CSPSA_RESULT_ERRORS);

    // - Flush.
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Open
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (f) Replace and delete a large item before first flush.
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  {
  	CSPSA_Key_t Key;
  	CSPSA_Size_t Size;

  	CSPSA_Result =
	  	F_CSPSA_TEST_GetInitalParameterValues(
			  V_CSPSA_TEST_Handle,
			  TestType,
			  TestDataSize,
			  &Key,
			  &Size
			);

	  // - Create CSPSA in cache.
	  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Write values.
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

		// - Replace with a large item before first flush.
    CSPSA_Result = CSPSA_WriteValue(V_CSPSA_TEST_Handle, Key, D_CSPSA_TEST_TEST_DATA_SIZE, F_CSPSA_TEST_GetWriteData());
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result < T_CSPSA_RESULT_ERRORS);

    // - Delete the newly added value.
    CSPSA_Result = CSPSA_DeleteValue(V_CSPSA_TEST_Handle, Key);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result < T_CSPSA_RESULT_ERRORS);

    // - Flush.
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Open
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (g) Replace and delete a large item before first flush.
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  {
  	CSPSA_Key_t Key;
  	CSPSA_Size_t Size;

  	CSPSA_Result =
	  	F_CSPSA_TEST_GetInitalParameterValues(
			  V_CSPSA_TEST_Handle,
			  TestType,
			  TestDataSize,
			  &Key,
			  &Size
			);

	  // - Create CSPSA in cache.
	  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Write values.
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

  	CSPSA_Result =
	  	F_CSPSA_TEST_GetNextParameterValues(
			  V_CSPSA_TEST_Handle,
			  TestType,
			  TestDataSize,
			  &Key,
			  &Size
			);

		// - Replace with a large item before first flush.
    CSPSA_Result = CSPSA_WriteValue(V_CSPSA_TEST_Handle, Key, D_CSPSA_TEST_TEST_DATA_SIZE, F_CSPSA_TEST_GetWriteData());
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result < T_CSPSA_RESULT_ERRORS);

    // - Delete the newly added value.
    CSPSA_Result = CSPSA_DeleteValue(V_CSPSA_TEST_Handle, Key);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result < T_CSPSA_RESULT_ERRORS);

    // - Flush.
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Open
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (h) Replace and delete a large item (with second Key value).
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  {
  	CSPSA_Key_t Key;
  	CSPSA_Size_t Size;

  	CSPSA_Result =
	  	F_CSPSA_TEST_GetInitalParameterValues(
			  V_CSPSA_TEST_Handle,
			  TestType,
			  TestDataSize,
			  &Key,
			  &Size
			);

	  // - Create CSPSA in cache.
	  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Write values.
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

		// - Replace with a large item before first flush.
    CSPSA_Result = CSPSA_WriteValue(V_CSPSA_TEST_Handle, Key, D_CSPSA_TEST_TEST_DATA_SIZE, F_CSPSA_TEST_GetWriteData());
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result < T_CSPSA_RESULT_ERRORS);

    // - Flush.
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Open
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

  	CSPSA_Result =
	  	F_CSPSA_TEST_GetNextParameterValues(
			  V_CSPSA_TEST_Handle,
			  TestType,
			  TestDataSize,
			  &Key,
			  &Size
			);

		// - Replace with a large item.
    CSPSA_Result = CSPSA_WriteValue(V_CSPSA_TEST_Handle, Key, D_CSPSA_TEST_TEST_DATA_SIZE, F_CSPSA_TEST_GetWriteData());
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result < T_CSPSA_RESULT_ERRORS);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
  }

  F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_TotalStartTime, &V_CSPSA_TEST_TotalElapsedTime);

  CurrentTestCase_p->TestOk = true;

ErrorExit:

  if (CurrentTestCase_p->TcStatus != T_CSPSA_TEST_TC_STATUS_NOT_IMPLEMENTED)
  {
    if (CurrentTestCase_p->TestOk)
    {
      sprintf(V_CSPSA_TEST_TempString_p, "Ok.;%lu", (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TotalElapsedTime));
    }
    else
    {
      sprintf(
        V_CSPSA_TEST_TempString_p,
        "Failed;Line %u;TestDataSize=%u;TestType=%u",
        V_CSPSA_TEST_ErrorLine,
        TestDataSize,
        TestType
      );
      if (V_CSPSA_TEST_Handle != 0)
      {
        (void) CSPSA_Close(&V_CSPSA_TEST_Handle);
      }
    }

    F_CSPSA_TEST_AppendTestCaseResult(TestCaseTableIndex, V_CSPSA_TEST_TempString_p);

  }
}

// -----------------------------------------------------------------------------

// - CSPSA_TEST_TC_9
//
//   § Increase and decrease the existing parameters small amounts
//
//  (a) Add a large item after first flush.
//  (b) Replace with a large item after first flush.
//  (c) Add a large item before first flush.
//  (d) Replace a large item before first flush.
//  (e) Add and delete a large item before first flush.
//  (f) Replace and delete a large item before first flush.
//  (g) Replace and delete a large item before first flush.
//  (h) Replace and delete a large item (with second Key value).

void CSPSA_TEST_TC_9(const uint32_t TestCaseTableIndex)
{
  CSPSA_TEST_TC_t* CurrentTestCase_p = &(CSPSA_TEST_TC_Table[TestCaseTableIndex]);
  uint32_t TestDataSize;
  CSPSA_Result_t CSPSA_Result;
  CSPSA_TEST_TestType_t TestType;

  // - Initialize.

  CurrentTestCase_p->TcStatus = T_CSPSA_TEST_TC_STATUS_VALID;
  V_CSPSA_TEST_ErrorLine = 0;
  F_CSPSA_TEST_ResetString(V_CSPSA_TEST_TempString_p);
  if (CurrentTestCase_p->TcStatus == T_CSPSA_TEST_TC_STATUS_NOT_IMPLEMENTED)
  {
    goto ErrorExit;
  }

  F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_TotalStartTime);

  V_CSPSA_TEST_ConstSize = 12;
  TestType = T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_CONST;
  TestDataSize = 4 * 1024;

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (a) Add a large item after first flush.
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  {
  	CSPSA_Key_t Key;
  	CSPSA_Size_t Size;

  	CSPSA_Result =
	  	F_CSPSA_TEST_GetInitalParameterValues(
			  V_CSPSA_TEST_Handle,
			  TestType,
			  TestDataSize,
			  &Key,
			  &Size
			);

	  // - Create CSPSA in cache.
	  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Write values.
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Flush.
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Open
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

		// - Add a large item.
    CSPSA_Result = CSPSA_WriteValue(V_CSPSA_TEST_Handle, Key - 1, V_CSPSA_TEST_ConstSize / 2 + 1, F_CSPSA_TEST_GetWriteData());
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result < T_CSPSA_RESULT_ERRORS);

    // - Flush.
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Open
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (b) Replace with a large item after first flush.
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  {
  	CSPSA_Key_t Key;
  	CSPSA_Size_t Size;

  	CSPSA_Result =
	  	F_CSPSA_TEST_GetInitalParameterValues(
			  V_CSPSA_TEST_Handle,
			  TestType,
			  TestDataSize,
			  &Key,
			  &Size
			);

	  // - Create CSPSA in cache.
	  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Write values.
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Flush.
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Open
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

		// - Replace with a large item.
    CSPSA_Result = CSPSA_WriteValue(V_CSPSA_TEST_Handle, Key, V_CSPSA_TEST_ConstSize / 2 + 1, F_CSPSA_TEST_GetWriteData());
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result < T_CSPSA_RESULT_ERRORS);

    // - Flush.
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Open
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (c) Add a large item before first flush.
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  {
  	CSPSA_Key_t Key;
  	CSPSA_Size_t Size;

  	CSPSA_Result =
	  	F_CSPSA_TEST_GetInitalParameterValues(
			  V_CSPSA_TEST_Handle,
			  TestType,
			  TestDataSize,
			  &Key,
			  &Size
			);

	  // - Create CSPSA in cache.
	  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Write values.
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

		// - Add a large item before first flush.
    CSPSA_Result = CSPSA_WriteValue(V_CSPSA_TEST_Handle, Key - 1, V_CSPSA_TEST_ConstSize / 2 + 1, F_CSPSA_TEST_GetWriteData());
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result < T_CSPSA_RESULT_ERRORS);

    // - Flush.
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Open
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (d) Replace a large item before first flush.
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  {
  	CSPSA_Key_t Key;
  	CSPSA_Size_t Size;

  	CSPSA_Result =
	  	F_CSPSA_TEST_GetInitalParameterValues(
			  V_CSPSA_TEST_Handle,
			  TestType,
			  TestDataSize,
			  &Key,
			  &Size
			);

	  // - Create CSPSA in cache.
	  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Write values.
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

		// - Replace with a large item before first flush.
    CSPSA_Result = CSPSA_WriteValue(V_CSPSA_TEST_Handle, Key, V_CSPSA_TEST_ConstSize / 2 + 1, F_CSPSA_TEST_GetWriteData());
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result < T_CSPSA_RESULT_ERRORS);

    // - Flush.
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Open
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (e) Add and delete a large item before first flush.
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  {
  	CSPSA_Key_t Key;
  	CSPSA_Size_t Size;

  	CSPSA_Result =
	  	F_CSPSA_TEST_GetInitalParameterValues(
			  V_CSPSA_TEST_Handle,
			  TestType,
			  TestDataSize,
			  &Key,
			  &Size
			);

	  // - Create CSPSA in cache.
	  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Write values.
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

		// - Add a large item before first flush.
    CSPSA_Result = CSPSA_WriteValue(V_CSPSA_TEST_Handle, Key - 1, V_CSPSA_TEST_ConstSize / 2 + 1, F_CSPSA_TEST_GetWriteData());
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result < T_CSPSA_RESULT_ERRORS);

    // - Delete the newly added value.
    CSPSA_Result = CSPSA_DeleteValue(V_CSPSA_TEST_Handle, Key - 1);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result < T_CSPSA_RESULT_ERRORS);

    // - Flush.
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Open
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (d) Replace and delete a large item before first flush.
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  {
  	CSPSA_Key_t Key;
  	CSPSA_Size_t Size;

  	CSPSA_Result =
	  	F_CSPSA_TEST_GetInitalParameterValues(
			  V_CSPSA_TEST_Handle,
			  TestType,
			  TestDataSize,
			  &Key,
			  &Size
			);

	  // - Create CSPSA in cache.
	  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Write values.
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

		// - Replace with a large item before first flush.
    CSPSA_Result = CSPSA_WriteValue(V_CSPSA_TEST_Handle, Key, V_CSPSA_TEST_ConstSize / 2 + 1, F_CSPSA_TEST_GetWriteData());
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result < T_CSPSA_RESULT_ERRORS);

    // - Delete the newly added value.
    CSPSA_Result = CSPSA_DeleteValue(V_CSPSA_TEST_Handle, Key);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result < T_CSPSA_RESULT_ERRORS);

    // - Flush.
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Open
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (f) Replace and delete a large item before first flush.
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  {
  	CSPSA_Key_t Key;
  	CSPSA_Size_t Size;

  	CSPSA_Result =
	  	F_CSPSA_TEST_GetInitalParameterValues(
			  V_CSPSA_TEST_Handle,
			  TestType,
			  TestDataSize,
			  &Key,
			  &Size
			);

	  // - Create CSPSA in cache.
	  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Write values.
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

  	CSPSA_Result =
	  	F_CSPSA_TEST_GetNextParameterValues(
			  V_CSPSA_TEST_Handle,
			  TestType,
			  TestDataSize,
			  &Key,
			  &Size
			);

		// - Replace with a large item before first flush.
    CSPSA_Result = CSPSA_WriteValue(V_CSPSA_TEST_Handle, Key, V_CSPSA_TEST_ConstSize / 2 + 1, F_CSPSA_TEST_GetWriteData());
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result < T_CSPSA_RESULT_ERRORS);

    // - Delete the newly added value.
    CSPSA_Result = CSPSA_DeleteValue(V_CSPSA_TEST_Handle, Key);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result < T_CSPSA_RESULT_ERRORS);

    // - Flush.
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Open
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (g) Replace and delete a large item (with second Key value).
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  {
  	CSPSA_Key_t Key;
  	CSPSA_Size_t Size;

  	CSPSA_Result =
	  	F_CSPSA_TEST_GetInitalParameterValues(
			  V_CSPSA_TEST_Handle,
			  TestType,
			  TestDataSize,
			  &Key,
			  &Size
			);

	  // - Create CSPSA in cache.
	  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Write values.
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

		// - Replace with a large item before first flush.
    CSPSA_Result = CSPSA_WriteValue(V_CSPSA_TEST_Handle, Key, V_CSPSA_TEST_ConstSize / 2 + 1, F_CSPSA_TEST_GetWriteData());
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result < T_CSPSA_RESULT_ERRORS);

    // - Flush.
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Open
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

  	CSPSA_Result =
	  	F_CSPSA_TEST_GetNextParameterValues(
			  V_CSPSA_TEST_Handle,
			  TestType,
			  TestDataSize,
			  &Key,
			  &Size
			);

		// - Replace with a large item.
    CSPSA_Result = CSPSA_WriteValue(V_CSPSA_TEST_Handle, Key, V_CSPSA_TEST_ConstSize / 2 + 1, F_CSPSA_TEST_GetWriteData());
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result < T_CSPSA_RESULT_ERRORS);

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
  }

  F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_TotalStartTime, &V_CSPSA_TEST_TotalElapsedTime);

  CurrentTestCase_p->TestOk = true;

ErrorExit:

  if (CurrentTestCase_p->TcStatus != T_CSPSA_TEST_TC_STATUS_NOT_IMPLEMENTED)
  {
    if (CurrentTestCase_p->TestOk)
    {
      sprintf(V_CSPSA_TEST_TempString_p, "Ok.;%lu", (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TotalElapsedTime));
    }
    else
    {
      sprintf(
        V_CSPSA_TEST_TempString_p,
        "Failed;Line %u;TestDataSize=%u;TestType=%u",
        V_CSPSA_TEST_ErrorLine,
        TestDataSize,
        TestType
      );
      if (V_CSPSA_TEST_Handle != 0)
      {
        (void) CSPSA_Close(&V_CSPSA_TEST_Handle);
      }
    }

    F_CSPSA_TEST_AppendTestCaseResult(TestCaseTableIndex, V_CSPSA_TEST_TempString_p);

  }
}

// -----------------------------------------------------------------------------
//  - CSPSA_TEST_TC_10:
//
//    § Multiple clients test
//
//   (a) Create a CSPSA.
//   (b) Start five threads of which three are reading and two are writing to
//       the same CSPSA, and let threads run for > 30 s.

void CSPSA_TEST_TC_10(const uint32_t TestCaseTableIndex)
{
  CSPSA_TEST_TC_t* CurrentTestCase_p = &(CSPSA_TEST_TC_Table[TestCaseTableIndex]);
  uint32_t TestDataSize = 0;
  CSPSA_TEST_TestType_t TestType = T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_CONST;

  // - Initialize.

#if  (CFG_CSPSA_TEST_TARGET != CFG_CSPSA_TEST_TARGET_LINUX_US) || (CFG_CSPSA_TEST_VARIANT != CFG_CSPSA_TEST_VARIANT_API)
  CurrentTestCase_p->TcStatus = T_CSPSA_TEST_TC_STATUS_NOT_APPLICABLE;
#else
  // - V_CSPSA_TEST_ThreadArg_p: Read/Write thread arguments.
  CSPSA_TEST_ThreadArg_t V_CSPSA_TEST_ThreadArg_p[D_CSPSA_TEST_TEST_TC_10_NBR_OF_READ_THREADS + D_CSPSA_TEST_TEST_TC_10_NBR_OF_WRITE_THREADS];
  CSPSA_Result_t CSPSA_Result;

  CurrentTestCase_p->TcStatus = T_CSPSA_TEST_TC_STATUS_VALID;
  V_CSPSA_TEST_ErrorLine = 0;
  F_CSPSA_TEST_ResetString(V_CSPSA_TEST_TempString_p);
  memset(V_CSPSA_TEST_TC10_DataCounter_p, 0, sizeof(V_CSPSA_TEST_TC10_DataCounter_p));
  if (CurrentTestCase_p->TcStatus != T_CSPSA_TEST_TC_STATUS_VALID)
  {
    goto ErrorExit;
  }

  F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_TotalStartTime);

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (a) Create a CSPSA
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  // - Create CSPSA in cache.
  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

  V_CSPSA_TEST_ConstSize = 12;
  TestType = T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_CONST;
  TestDataSize = 4 * 1024;

  // - Write values.
  CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

  // - Flush.
  CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

  // - Close.
  CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

  // - Open.
  CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// - (b) Start five threads of which three are reading and two are writing to
	//       the same CSPSA, and let threads run for > 30 s.
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  {
  	int PthreadResult;
  	pthread_t thread_p[D_CSPSA_TEST_TEST_TC_10_NBR_OF_READ_THREADS + D_CSPSA_TEST_TEST_TC_10_NBR_OF_WRITE_THREADS];
  	int NbrOfThreads = 0;
  	uint32_t i;

    memset(V_CSPSA_TEST_ReadData_p, 0, sizeof(V_CSPSA_TEST_ReadData_p));

	  V_CSPSA_TEST_ThreadArg_p[NbrOfThreads].Offset = 0;

#if D_CSPSA_TEST_TEST_TC_10_NBR_OF_WRITE_THREADS
	  for (i = 0; i < D_CSPSA_TEST_TEST_TC_10_NBR_OF_WRITE_THREADS; i++)
	  {
		  V_CSPSA_TEST_ThreadArg_p[NbrOfThreads].Handle = 0;
		  V_CSPSA_TEST_ThreadArg_p[NbrOfThreads].TestType = T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_INC;
		  V_CSPSA_TEST_ThreadArg_p[NbrOfThreads].TotalTestDataSize = TestDataSize;
		  V_CSPSA_TEST_ThreadArg_p[NbrOfThreads].DurationInS = D_CSPSA_TEST_TEST_TC_10_DURATION;
		  V_CSPSA_TEST_ThreadArg_p[NbrOfThreads].Offset = TestDataSize;
		  V_CSPSA_TEST_ThreadArg_p[NbrOfThreads].ThreadIndex = NbrOfThreads;
		  V_CSPSA_TEST_ThreadArg_p[NbrOfThreads].Sleep = 50000;
	  	PthreadResult = pthread_create(&thread_p[NbrOfThreads], NULL, F_CSPSA_TEST_WriteThread, &V_CSPSA_TEST_ThreadArg_p[NbrOfThreads]);
	    M_CSPSA_TEST_ASSERT_ERROR_LINE(PthreadResult == 0);
	  	NbrOfThreads++;
	  }
#endif

	  V_CSPSA_TEST_ThreadArg_p[NbrOfThreads].Offset = 0;

#if D_CSPSA_TEST_TEST_TC_10_NBR_OF_READ_THREADS
	  for (i = 0; i < D_CSPSA_TEST_TEST_TC_10_NBR_OF_READ_THREADS; i++)
	  {
		  V_CSPSA_TEST_ThreadArg_p[NbrOfThreads].Handle = 0;
		  V_CSPSA_TEST_ThreadArg_p[NbrOfThreads].TestType = T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_INC;
		  V_CSPSA_TEST_ThreadArg_p[NbrOfThreads].TotalTestDataSize = TestDataSize;
		  V_CSPSA_TEST_ThreadArg_p[NbrOfThreads].DurationInS = D_CSPSA_TEST_TEST_TC_10_DURATION;
		  V_CSPSA_TEST_ThreadArg_p[NbrOfThreads].Offset = TestDataSize;
		  V_CSPSA_TEST_ThreadArg_p[NbrOfThreads].ThreadIndex = NbrOfThreads;
		  V_CSPSA_TEST_ThreadArg_p[NbrOfThreads].Sleep = 5000 * NbrOfThreads;
	  	PthreadResult = pthread_create(&thread_p[NbrOfThreads], NULL, F_CSPSA_TEST_ReadThread, &V_CSPSA_TEST_ThreadArg_p[NbrOfThreads]);
	    M_CSPSA_TEST_ASSERT_ERROR_LINE(PthreadResult == 0);
	  	NbrOfThreads++;
	  }
#endif

		while (NbrOfThreads > 0)
		{
      PthreadResult = pthread_join(thread_p[NbrOfThreads - 1], NULL);
      NbrOfThreads--;
		}
  }

  // - Close.
  CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

  F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_TotalStartTime, &V_CSPSA_TEST_TotalElapsedTime);
  CurrentTestCase_p->TestOk = true;

ErrorExit:
#endif // - End of Linux specific code.

  if (CurrentTestCase_p->TcStatus == T_CSPSA_TEST_TC_STATUS_VALID)
  {
    if (CurrentTestCase_p->TestOk)
    {
    	uint32_t TotalDataTransferred = 0;
    	uint32_t i;
    	for (i = 0; i < sizeof(V_CSPSA_TEST_TC10_DataCounter_p) / sizeof(V_CSPSA_TEST_TC10_DataCounter_p[0]); i++)
    	{
    		TotalDataTransferred += V_CSPSA_TEST_TC10_DataCounter_p[i];
    	}
      sprintf(
      	V_CSPSA_TEST_TempString_p,
      	"Ok. A total of %u [B] of data was transferred: {w1(%u),w2(%u),r1(%u),r2(%u),r3(%u)};%lu",
      	TotalDataTransferred,
      	V_CSPSA_TEST_TC10_DataCounter_p[0],
      	V_CSPSA_TEST_TC10_DataCounter_p[1],
      	V_CSPSA_TEST_TC10_DataCounter_p[2],
      	V_CSPSA_TEST_TC10_DataCounter_p[3],
      	V_CSPSA_TEST_TC10_DataCounter_p[4],
      	(unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TotalElapsedTime)
      );
    }
    else
    {
      sprintf(
        V_CSPSA_TEST_TempString_p,
        "Failed;Line %u;TestDataSize=%u;TestType=%u",
        V_CSPSA_TEST_ErrorLine,
        TestDataSize,
        TestType
      );
      if (V_CSPSA_TEST_Handle != 0)
      {
        (void) CSPSA_Close(&V_CSPSA_TEST_Handle);
      }
    }

    F_CSPSA_TEST_AppendTestCaseResult(TestCaseTableIndex, V_CSPSA_TEST_TempString_p);

  }
}

// -----------------------------------------------------------------------------
//  - CSPSA_TEST_TC_11
//
//    § Performance test (12 B x 4096 items).
//
//   (a) Write, flush and read 12 B x 4096 items.

void CSPSA_TEST_TC_11(const uint32_t TestCaseTableIndex)
{
  CSPSA_TEST_TC_t* CurrentTestCase_p = &(CSPSA_TEST_TC_Table[TestCaseTableIndex]);
  uint32_t TestDataSize;
  CSPSA_Result_t CSPSA_Result;
  CSPSA_TEST_TestType_t TestType;

  // - Clear all time measurment values.
  memset((uint8_t*)&V_CSPSA_TEST_TimeMeasurement, 0, sizeof(V_CSPSA_TEST_TimeMeasurement));

  // - Initialize.

  CurrentTestCase_p->TcStatus = T_CSPSA_TEST_TC_STATUS_VALID;
  V_CSPSA_TEST_ErrorLine = 0;
  F_CSPSA_TEST_ResetString(V_CSPSA_TEST_TempString_p);
  if (CurrentTestCase_p->TcStatus == T_CSPSA_TEST_TC_STATUS_NOT_APPLICABLE)
  {
    goto ErrorExit;
  }

  F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_TotalStartTime);

  // - Create CSPSA in cache.
  F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
  F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
  F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CREATE], &V_CSPSA_TEST_ElapsedTime);

  // - Now create a CSPSA image in different ways determined by CSPSA_TEST_TestType_t. Clear everything between
  //   test runs.

  V_CSPSA_TEST_ConstSize = 24;
  TestType = T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_CONST;
  TestDataSize = 4096 * V_CSPSA_TEST_ConstSize;

  {
    //printf(D_CSPSA_TEST_MODULE_HEADER "TestType(%s)/TestDataSize(%u).\n", F_CSPSA_TEST_PrintTestType(TestType), TestDataSize);

    // - Write values.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_1ST], &V_CSPSA_TEST_ElapsedTime);

    // - Flush.
    // - NOTE: Exclude this flush from time measurement since the previous write was done to an empty cache. It
    //   would not be representative for normal flushes.
    //F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    //F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    //F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FLUSH], &V_CSPSA_TEST_ElapsedTime);

    // - Close.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);

    // - Open.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_OPEN], &V_CSPSA_TEST_ElapsedTime);

    // - Read values.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = F_CSPSA_TEST_ReadAndVerify(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_READ], &V_CSPSA_TEST_ElapsedTime);

    // - Write values.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_2ND], &V_CSPSA_TEST_ElapsedTime);

    // - Flush.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FLUSH], &V_CSPSA_TEST_ElapsedTime);

    // - Close.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);

    // - Open.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_OPEN], &V_CSPSA_TEST_ElapsedTime);

    // - Read values.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = F_CSPSA_TEST_ReadAndVerify(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_READ], &V_CSPSA_TEST_ElapsedTime);

    // - Close.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);

  }
  F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_TotalStartTime, &V_CSPSA_TEST_TotalElapsedTime);

  CurrentTestCase_p->TestOk = true;

ErrorExit:

  if (CurrentTestCase_p->TcStatus != T_CSPSA_TEST_TC_STATUS_NOT_IMPLEMENTED)
  {
    if (CurrentTestCase_p->TestOk)
    {
      sprintf(V_CSPSA_TEST_TempString_p, "Ok.");
      sprintf(V_CSPSA_TEST_TempString_p+strlen(V_CSPSA_TEST_TempString_p),
        ";%lu;%lu;%lu;%lu;%lu;%lu;%lu;%lu;%lu;%lu\n",
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TotalElapsedTime),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_OPEN]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_READ]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_1ST]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_2ND]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FLUSH]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FORMAT]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CREATE]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_DELETE])
      );
    }
    else
    {
      sprintf(
        V_CSPSA_TEST_TempString_p,
        "Failed;Line %u;TestDataSize=%u;TestType=%u",
        V_CSPSA_TEST_ErrorLine,
        TestDataSize,
        TestType
      );
      if (V_CSPSA_TEST_Handle != 0)
      {
        (void) CSPSA_Close(&V_CSPSA_TEST_Handle);
      }
    }

    F_CSPSA_TEST_AppendTestCaseResult(TestCaseTableIndex, V_CSPSA_TEST_TempString_p);

  }
}

// -----------------------------------------------------------------------------
//  - CSPSA_TEST_TC_12
//
//    § Make sure CSPSA can be opened when there are two valid headers
//
//   (a) Both headers and data is valid
//   (b) Both headers are valid, but the latest written data has a bit error

void CSPSA_TEST_TC_12(const uint32_t TestCaseTableIndex)
{
  CSPSA_TEST_TC_t* CurrentTestCase_p = &(CSPSA_TEST_TC_Table[TestCaseTableIndex]);
  uint32_t TestDataSize;
  CSPSA_Result_t CSPSA_Result;
  CSPSA_TEST_TestType_t TestType;
  uint32_t BlockSize = V_CSPSA_TEST_Config.BlockSize;
  uint32_t SectorSize = V_CSPSA_TEST_Config.SectorSize;

  // - Clear all time measurment values.
  memset((uint8_t*)&V_CSPSA_TEST_TimeMeasurement, 0, sizeof(V_CSPSA_TEST_TimeMeasurement));

  // - Initialize.

#if (CFG_CSPSA_TEST_TARGET == CFG_CSPSA_TEST_TARGET_LINUX_US) && (CFG_CSPSA_TEST_VARIANT == CFG_CSPSA_TEST_VARIANT_CORE)
  CurrentTestCase_p->TcStatus = T_CSPSA_TEST_TC_STATUS_VALID;
#else
  CurrentTestCase_p->TcStatus = T_CSPSA_TEST_TC_STATUS_NOT_APPLICABLE;
#endif
  V_CSPSA_TEST_ErrorLine = 0;
  F_CSPSA_TEST_ResetString(V_CSPSA_TEST_TempString_p);
  if (CurrentTestCase_p->TcStatus == T_CSPSA_TEST_TC_STATUS_NOT_APPLICABLE)
  {
    goto ErrorExit;
  }

  F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_TotalStartTime);

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (a) Both headers and data is valid
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  {
	  // - Create CSPSA in cache.
	  F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
	  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
	  F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
	  F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CREATE], &V_CSPSA_TEST_ElapsedTime);

	  V_CSPSA_TEST_ConstSize = 12;
	  TestType = T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_CONST;
	  TestDataSize = 4 * 1024;

    // - Write values.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_1ST], &V_CSPSA_TEST_ElapsedTime);

    // - Flush.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FLUSH], &V_CSPSA_TEST_ElapsedTime);

    // - Close.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);

    // - Copy the valid image and put at other end.
    {
      CSPSA_CORE_Header_t* Header_p = NULL;
      uint32_t ReadOffset = 0;
      uint32_t WriteOffset = 0;
      bool ReadBackwards = false;
  		uint32_t HeaderSize;
    	uint32_t SizeBlockAligned;

    	// - Use a fake session info struct to be able to open ll driver.
    	V_CSPSA_TEST_SessionInfo.Config = V_CSPSA_TEST_Config;
    	V_CSPSA_TEST_Handle = (CSPSA_Handle_t) &V_CSPSA_TEST_SessionInfo;
      F_CSPSA_TEST_InitFlashMemory(&V_CSPSA_TEST_SessionInfo.Config);

    	// - Open a low level driver.
    	CSPSA_Result = CSPSA_LL_Open(V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

      memset(V_CSPSA_TEST_ReadData_p, 0, sizeof(V_CSPSA_TEST_ReadData_p));

      CSPSA_Result = CSPSA_LL_Read(V_CSPSA_TEST_Handle, ReadOffset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

  		Header_p = (CSPSA_CORE_Header_t*) (ptrdiff_t) V_CSPSA_TEST_ReadData_p;
  		HeaderSize = sizeof(*Header_p) + Header_p->BbtSize;
      M_CSPSA_TEST_ASSERT_ERROR_LINE((HeaderSize >= sizeof(*Header_p)) && (HeaderSize <= CFG_CSPSA_CORE_STORAGE_IMAGE_HEADER_MAX_SIZE * SectorSize));
  		SizeBlockAligned = M_CSPSA_TEST_ALIGN32(HeaderSize + Header_p->DataSize);
      SizeBlockAligned = SizeBlockAligned + V_CSPSA_TEST_Config.BlockSize - 1;
      SizeBlockAligned &= ~(V_CSPSA_TEST_Config.BlockSize - 1);

      // - Verify header.
      {
        uint32_t CrcValue = C_CSPSA_TEST_HeaderMagic;

        CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->Magic, sizeof(Header_p->Magic));
        CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->Version, sizeof(Header_p->Version));
        CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->Index, sizeof(Header_p->Index));
        CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->Flags, sizeof(Header_p->Flags));
        CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->DataSize, sizeof(Header_p->DataSize));
        CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->Crc32Data, sizeof(Header_p->Crc32Data));
        CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->FastParameter, sizeof(Header_p->FastParameter));
        CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->Reserved, sizeof(Header_p->Reserved));
        CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->BbtSize, sizeof(Header_p->BbtSize));
        if (Header_p->BbtSize > 0)
        {
          CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->Bbt, Header_p->BbtSize);
        }
        M_CSPSA_TEST_ASSERT_ERROR_LINE(Header_p->Crc32Header == CrcValue);
      }

      // - Start copying area.
    	{
    	  ReadBackwards = (Header_p->Flags & T_CSPSA_TEST_HEADERFLAGS_DATA_AT_END);
    	  ReadOffset = (ReadBackwards) ? V_CSPSA_TEST_Config.SizeInBytes - BlockSize : 0;
    	  WriteOffset = (ReadBackwards) ? 0 : V_CSPSA_TEST_Config.SizeInBytes - BlockSize;
        memset(V_CSPSA_TEST_ReadData_p, 0, sizeof(V_CSPSA_TEST_ReadData_p));

        while (SizeBlockAligned > 0)
        {
          CSPSA_Result = CSPSA_LL_Read(V_CSPSA_TEST_Handle, ReadOffset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
          M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

          // - Write back the data at opposite end.
          CSPSA_Result = CSPSA_LL_Write(V_CSPSA_TEST_Handle, WriteOffset, V_CSPSA_TEST_ReadData_p, BlockSize, !ReadBackwards);
          M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

          if (ReadBackwards)
          {
            ReadOffset -= BlockSize;
            WriteOffset += BlockSize;
          }
          else
          {
            ReadOffset += BlockSize;
            WriteOffset -= BlockSize;
          }
          SizeBlockAligned -= BlockSize;
        }
      }
      // - Increment header index and re-calc crc32.
      ReadBackwards = true;
      ReadOffset = (ReadBackwards) ? V_CSPSA_TEST_Config.SizeInBytes - BlockSize : 0;
      CSPSA_Result = CSPSA_LL_Read(V_CSPSA_TEST_Handle, ReadOffset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
  		Header_p = (CSPSA_CORE_Header_t*) (ptrdiff_t) V_CSPSA_TEST_ReadData_p;
      Header_p->Index++;
      {
        uint32_t CrcValue = C_CSPSA_TEST_HeaderMagic;

        CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->Magic, sizeof(Header_p->Magic));
        CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->Version, sizeof(Header_p->Version));
        CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->Index, sizeof(Header_p->Index));
        CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->Flags, sizeof(Header_p->Flags));
        CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->DataSize, sizeof(Header_p->DataSize));
        CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->Crc32Data, sizeof(Header_p->Crc32Data));
        CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->FastParameter, sizeof(Header_p->FastParameter));
        CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->Reserved, sizeof(Header_p->Reserved));
        CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->BbtSize, sizeof(Header_p->BbtSize));
        if (Header_p->BbtSize > 0)
        {
          CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->Bbt, Header_p->BbtSize);
        }
        Header_p->Crc32Header = CrcValue;
      }
        // - Write back the new header at opposite end of the last one.
        WriteOffset = ReadOffset;
        CSPSA_Result = CSPSA_LL_Write(V_CSPSA_TEST_Handle, WriteOffset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    	// - Close the low level driver.
    	CSPSA_Result = CSPSA_LL_Close(V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    }

    // - Now CSPSA_Open() should cope with the two images and choose the one with the lowest index. After
    //   close there should be only one image (re-written).

    // - Open.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_OPEN], &V_CSPSA_TEST_ElapsedTime);

    // - Close.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);

    // - Check that there is only one header.
    {
      CSPSA_CORE_Header_t* Header_p = NULL;
      bool ReadBackwards = true;
      uint32_t ReadOffset = (ReadBackwards) ? V_CSPSA_TEST_Config.SizeInBytes - BlockSize : 0;
  		uint32_t HeaderSize;
    	uint32_t SizeBlockAligned;

    	// - Use a fake session info struct to be able to open ll driver.
    	V_CSPSA_TEST_SessionInfo.Config = V_CSPSA_TEST_Config;
    	V_CSPSA_TEST_Handle = (CSPSA_Handle_t) &V_CSPSA_TEST_SessionInfo;
      F_CSPSA_TEST_InitFlashMemory(&V_CSPSA_TEST_SessionInfo.Config);

    	// - Open a low level driver.
    	CSPSA_Result = CSPSA_LL_Open(V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

      memset(V_CSPSA_TEST_ReadData_p, 0, sizeof(V_CSPSA_TEST_ReadData_p));

      CSPSA_Result = CSPSA_LL_Read(V_CSPSA_TEST_Handle, ReadOffset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

  		Header_p = (CSPSA_CORE_Header_t*) (ptrdiff_t) V_CSPSA_TEST_ReadData_p;
  		HeaderSize = sizeof(*Header_p) + Header_p->BbtSize;
      M_CSPSA_TEST_ASSERT_ERROR_LINE((HeaderSize >= sizeof(*Header_p)) && (HeaderSize <= CFG_CSPSA_CORE_STORAGE_IMAGE_HEADER_MAX_SIZE * SectorSize));
  		SizeBlockAligned = M_CSPSA_TEST_ALIGN32(HeaderSize + Header_p->DataSize);
      SizeBlockAligned = SizeBlockAligned + V_CSPSA_TEST_Config.BlockSize - 1;
      SizeBlockAligned &= ~(V_CSPSA_TEST_Config.BlockSize - 1);

      // - Verify header at start of area (where it ougth to be).
      M_CSPSA_TEST_ASSERT_ERROR_LINE(Header_p->Index == 2);
      {
        uint32_t CrcValue = C_CSPSA_TEST_HeaderMagic;

        CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->Magic, sizeof(Header_p->Magic));
        CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->Version, sizeof(Header_p->Version));
        CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->Index, sizeof(Header_p->Index));
        CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->Flags, sizeof(Header_p->Flags));
        CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->DataSize, sizeof(Header_p->DataSize));
        CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->Crc32Data, sizeof(Header_p->Crc32Data));
        CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->FastParameter, sizeof(Header_p->FastParameter));
        CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->Reserved, sizeof(Header_p->Reserved));
        CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->BbtSize, sizeof(Header_p->BbtSize));
        if (Header_p->BbtSize > 0)
        {
          CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->Bbt, Header_p->BbtSize);
        }
        M_CSPSA_TEST_ASSERT_ERROR_LINE(Header_p->Crc32Header == CrcValue);
      }

      // - Verify header at end of area (where it ougth not to be).
      do
      {
        ReadBackwards = false;
        ReadOffset = (ReadBackwards) ? V_CSPSA_TEST_Config.SizeInBytes - BlockSize : 0;
        CSPSA_Result = CSPSA_LL_Read(V_CSPSA_TEST_Handle, ReadOffset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
        M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

        Header_p = (CSPSA_CORE_Header_t*) (ptrdiff_t) V_CSPSA_TEST_ReadData_p;
        HeaderSize = sizeof(*Header_p) + Header_p->BbtSize;
        if ((HeaderSize < sizeof(*Header_p)) || (HeaderSize > CFG_CSPSA_CORE_STORAGE_IMAGE_HEADER_MAX_SIZE * SectorSize))
        {
          // - Seems like no header. That's fine so bail out.
          break;
        }
        SizeBlockAligned = M_CSPSA_TEST_ALIGN32(HeaderSize + Header_p->DataSize);
        SizeBlockAligned = SizeBlockAligned + V_CSPSA_TEST_Config.BlockSize - 1;
        SizeBlockAligned &= ~(V_CSPSA_TEST_Config.BlockSize - 1);

        // - Verify header at start of area (where it ougth to be).
        {
          uint32_t CrcValue = C_CSPSA_TEST_HeaderMagic;

          CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->Magic, sizeof(Header_p->Magic));
          CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->Version, sizeof(Header_p->Version));
          CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->Index, sizeof(Header_p->Index));
          CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->Flags, sizeof(Header_p->Flags));
          CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->DataSize, sizeof(Header_p->DataSize));
          CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->Crc32Data, sizeof(Header_p->Crc32Data));
          CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->FastParameter, sizeof(Header_p->FastParameter));
          CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->Reserved, sizeof(Header_p->Reserved));
          CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->BbtSize, sizeof(Header_p->BbtSize));
          if (Header_p->BbtSize > 0)
          {
            CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &Header_p->Bbt, Header_p->BbtSize);
          }
          // - If CRC32 is correct something is wrong!
          M_CSPSA_TEST_ASSERT_ERROR_LINE(Header_p->Crc32Header == CrcValue);
        }
      } while (false);
    }
  }

  CurrentTestCase_p->TestOk = true;

ErrorExit:

  if (CurrentTestCase_p->TcStatus == T_CSPSA_TEST_TC_STATUS_VALID)
  {
    if (CurrentTestCase_p->TestOk)
    {
      sprintf(V_CSPSA_TEST_TempString_p, "Ok.");
      sprintf(V_CSPSA_TEST_TempString_p+strlen(V_CSPSA_TEST_TempString_p),
        ";%lu;%lu;%lu;%lu;%lu;%lu;%lu;%lu;%lu;%lu\n",
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TotalElapsedTime),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_OPEN]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_READ]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_1ST]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_2ND]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FLUSH]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FORMAT]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CREATE]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_DELETE])
      );
    }
    else
    {
      sprintf(
        V_CSPSA_TEST_TempString_p,
        "Failed;Line %u;TestDataSize=%u;TestType=%u",
        V_CSPSA_TEST_ErrorLine,
        TestDataSize,
        TestType
      );
      if (V_CSPSA_TEST_Handle != 0)
      {
        (void) CSPSA_Close(&V_CSPSA_TEST_Handle);
      }
    }

    F_CSPSA_TEST_AppendTestCaseResult(TestCaseTableIndex, V_CSPSA_TEST_TempString_p);
  }
}

// -----------------------------------------------------------------------------
//  - CSPSA_TEST_TC_13
//
//    § Performance test of flash media
//
//   (a) Write 8 x 128 kB
//   (b) Read 8 x 128 kB

void CSPSA_TEST_TC_13(const uint32_t TestCaseTableIndex)
{
  CSPSA_TEST_TC_t* CurrentTestCase_p = &(CSPSA_TEST_TC_Table[TestCaseTableIndex]);
  CSPSA_Result_t CSPSA_Result;
  uint8_t* WriteData_p = NULL;
  uint32_t Offset = 0;
  bool WriteBackwards = false;
  bool ReadBackwards = false;
  uint32_t Seed = 1749;
  uint32_t i;
  uint32_t Laps = 500;
  uint64_t WriteThroughPut = 0;
  uint64_t ReadThroughPut = 0;

  // - Clear all time measurment values.
  memset((uint8_t*)&V_CSPSA_TEST_TimeMeasurement, 0, sizeof(V_CSPSA_TEST_TimeMeasurement));

#if (CFG_CSPSA_TEST_TARGET == CFG_CSPSA_TEST_TARGET_LINUX_US) && (CFG_CSPSA_TEST_VARIANT == CFG_CSPSA_TEST_VARIANT_CORE)
  CurrentTestCase_p->TcStatus = T_CSPSA_TEST_TC_STATUS_VALID;
#else
  CurrentTestCase_p->TcStatus = T_CSPSA_TEST_TC_STATUS_NOT_APPLICABLE;
#endif
  V_CSPSA_TEST_ErrorLine = 0;
  F_CSPSA_TEST_ResetString(V_CSPSA_TEST_TempString_p);
  if (CurrentTestCase_p->TcStatus == T_CSPSA_TEST_TC_STATUS_NOT_APPLICABLE)
  {
    goto ErrorExit;
  }

  V_CSPSA_TEST_Config.MemoryType = CFG_CSPSA_TEST_MEMORY_TYPE;

	V_CSPSA_TEST_SessionInfo.Config = V_CSPSA_TEST_Config;
	V_CSPSA_TEST_Handle = (CSPSA_Handle_t) &V_CSPSA_TEST_SessionInfo;
  F_CSPSA_TEST_InitFlashMemory(&V_CSPSA_TEST_SessionInfo.Config);

  WriteData_p = (uint8_t*) malloc(CFG_CSPSA_TEST_AREA_SIZE / 2);
  // - Initialize write data with a pattern.
  for (i = 0; i < CFG_CSPSA_TEST_AREA_SIZE / 2; i++)
  {
    WriteData_p[i] = (uint8_t)(Seed - Seed*~i + ((Seed*Seed*i) >> 8));
  }

  F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_TotalStartTime);

  CSPSA_Result = CSPSA_LL_Open(V_CSPSA_TEST_Handle);
  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

  //  - Low level write.
  F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
  for (i = 0; i < Laps; i++)
  {
    CSPSA_Result = CSPSA_LL_Write(V_CSPSA_TEST_Handle, Offset, WriteData_p, CFG_CSPSA_TEST_AREA_SIZE / 2, WriteBackwards);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
  }
  F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
  F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_1ST], &V_CSPSA_TEST_ElapsedTime);

  if (F_CSPSA_TEST_TimeInUs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_1ST]) != 0)
  {
    WriteThroughPut = (uint64_t)Laps * ((uint64_t)CFG_CSPSA_TEST_AREA_SIZE / 2LL) * 1000000LL / F_CSPSA_TEST_TimeInUs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_1ST]);
    //printf("Laps(%u), Size(%u), Duration(%lu)\n", Laps, (CFG_CSPSA_TEST_AREA_SIZE / 2), F_CSPSA_TEST_TimeInUs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_1ST]));
  }
  else
  {
    WriteThroughPut = 0;
  }
  WriteThroughPut = WriteThroughPut / 1024 ;

  //  - Low level read.
  F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
  for (i = 0; i < Laps; i++)
  {
    CSPSA_Result = CSPSA_LL_Read(V_CSPSA_TEST_Handle, Offset, WriteData_p, CFG_CSPSA_TEST_AREA_SIZE / 2, ReadBackwards);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
  }
  F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
  F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_READ], &V_CSPSA_TEST_ElapsedTime);

  if (F_CSPSA_TEST_TimeInUs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_READ]) != 0)
  {
    ReadThroughPut = (uint64_t)Laps * ((uint64_t)CFG_CSPSA_TEST_AREA_SIZE / 2LL) * 1000000LL / F_CSPSA_TEST_TimeInUs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_READ]);
    //printf("Laps(%u), Size(%u), Duration(%lu)\n", Laps, (CFG_CSPSA_TEST_AREA_SIZE / 2), F_CSPSA_TEST_TimeInUs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_READ]));
  }
  else
  {
    ReadThroughPut = 0;
  }
  ReadThroughPut = ReadThroughPut / 1024 ;

  CSPSA_Result = CSPSA_LL_Close(V_CSPSA_TEST_Handle);
  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

  F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_TotalStartTime, &V_CSPSA_TEST_TotalElapsedTime);

  CurrentTestCase_p->TestOk = true;

ErrorExit:

  if (WriteData_p != NULL)
  {
    free(WriteData_p);
    WriteData_p = NULL;
  }

  if (CurrentTestCase_p->TcStatus != T_CSPSA_TEST_TC_STATUS_NOT_APPLICABLE)
  {
    if (CurrentTestCase_p->TestOk)
    {
      sprintf(V_CSPSA_TEST_TempString_p, "Ok. Write @ %lu [kB/s]. Read @ %lu [kB/s]", (unsigned long int) WriteThroughPut, (unsigned long int) ReadThroughPut);
      sprintf(V_CSPSA_TEST_TempString_p+strlen(V_CSPSA_TEST_TempString_p),
        ";%lu\n",
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TotalElapsedTime)
      );
    }
    else
    {
      sprintf(
        V_CSPSA_TEST_TempString_p,
        "Failed;Line %u",
        V_CSPSA_TEST_ErrorLine
      );
      // - Don't close CSPSA as we're using a fake session.
      V_CSPSA_TEST_Handle = 0;
    }

    F_CSPSA_TEST_AppendTestCaseResult(TestCaseTableIndex, V_CSPSA_TEST_TempString_p);

  }
}

// -----------------------------------------------------------------------------

// - CSPSA_TEST_TC_14: Simple test case that inserts/deletes a few values.
void CSPSA_TEST_TC_14(const uint32_t TestCaseTableIndex)
{
  CSPSA_TEST_TC_t* CurrentTestCase_p = &(CSPSA_TEST_TC_Table[TestCaseTableIndex]);
  CSPSA_Result_t CSPSA_Result;
  int Ok;

  // - Initialize.
  F_CSPSA_TEST_InitTestCase(TestCaseTableIndex);
  F_CSPSA_TEST_ClearSimpleList();
  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
  Ok = F_CSPSA_TEST_CheckItems();
  M_CSPSA_TEST_ASSERT_ERROR_LINE(Ok);

  Ok = F_CSPSA_TEST_InsertItem(3, 3);
  M_CSPSA_TEST_ASSERT_ERROR_LINE(Ok);
  Ok = F_CSPSA_TEST_InsertItem(7, 8);
  M_CSPSA_TEST_ASSERT_ERROR_LINE(Ok);
  Ok = F_CSPSA_TEST_InsertItem(5, 20);
  M_CSPSA_TEST_ASSERT_ERROR_LINE(Ok);
  // - Overwrite key 3.
  Ok = F_CSPSA_TEST_InsertItem(3, 40);
  M_CSPSA_TEST_ASSERT_ERROR_LINE(Ok);
  // - Delete key 5.
  Ok = F_CSPSA_TEST_DeleteItem(5);
  M_CSPSA_TEST_ASSERT_ERROR_LINE(Ok);

  // - Flush.
  Ok = F_CSPSA_TEST_FlushCloseReopen(true);
  M_CSPSA_TEST_ASSERT_ERROR_LINE(Ok);

  // - Insert more items.
  Ok = F_CSPSA_TEST_InsertItem(5, 10);
  M_CSPSA_TEST_ASSERT_ERROR_LINE(Ok);
  Ok = F_CSPSA_TEST_InsertItem(3, 10);
  M_CSPSA_TEST_ASSERT_ERROR_LINE(Ok);
  // - Close.
  CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);

  CurrentTestCase_p->TestOk = true;

ErrorExit:

  if (V_CSPSA_TEST_Handle != 0)
  {
    (void) CSPSA_Close(&V_CSPSA_TEST_Handle);
  }

  F_CSPSA_TEST_FinishTestCase(TestCaseTableIndex);
}

// -----------------------------------------------------------------------------

// - CSPSA_TEST_TC_15: Tests randomly inserting/deleting values into the list
//   (and flushing/reopening).

void CSPSA_TEST_TC_15(const uint32_t TestCaseTableIndex)
{
  CSPSA_TEST_TC_t* CurrentTestCase_p = &(CSPSA_TEST_TC_Table[TestCaseTableIndex]);
  CSPSA_Result_t CSPSA_Result;
  int Ok;
  uint32_t Rnd;
  uint32_t Count;

  // - Initialize.
  F_CSPSA_TEST_InitTestCase(TestCaseTableIndex);
  F_CSPSA_TEST_ResetRandSeed();
  F_CSPSA_TEST_ClearSimpleList();
  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
  CSPSA_Result = CSPSA_Format(V_CSPSA_TEST_Handle);
  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
  Ok = F_CSPSA_TEST_CheckItems();
  M_CSPSA_TEST_ASSERT_ERROR_LINE(Ok);
  for (Count = 0; Count < 10000; ++Count)
  {
    //printf("Count = %u\n", Count);
    Rnd = F_CSPSA_TEST_GetRand(8);
    if (Rnd == 0)
    {
      Ok = F_CSPSA_TEST_FlushCloseReopen(true);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(Ok);
    }
    else if ((Rnd < 4 && V_CSPSA_TEST_SimpleListSize > 1) || V_CSPSA_TEST_SimpleListSize >= D_CSPSA_TEST_MAX_SIMPLE_LIST_SIZE)
    {
      // - Delete a random element from the list.
      uint32_t Index = F_CSPSA_TEST_GetRand(V_CSPSA_TEST_SimpleListSize);
      Ok = F_CSPSA_TEST_DeleteItem(V_CSPSA_TEST_SimpleList[Index].Key);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(Ok);
    }
    else
    {
      // - Insert a random element in the list.
      uint32_t Key = F_CSPSA_TEST_GetRand(400);
      int Value = (int) F_CSPSA_TEST_GetRand(30);
      Ok = F_CSPSA_TEST_InsertItem(Key, Value);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(Ok);
    }
  }
  // - Close.
  CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);

  CurrentTestCase_p->TestOk = true;

ErrorExit:

  if (V_CSPSA_TEST_Handle != 0)
  {
    (void) CSPSA_Close(&V_CSPSA_TEST_Handle);
  }

  F_CSPSA_TEST_FinishTestCase(TestCaseTableIndex);
}

// -----------------------------------------------------------------------------

void CSPSA_TEST_TC_16(const uint32_t TestCaseTableIndex)
{
  CSPSA_TEST_TC_t* CurrentTestCase_p = &(CSPSA_TEST_TC_Table[TestCaseTableIndex]);
  CSPSA_Result_t CSPSA_Result;
  CSPSA_TEST_InducedError_t InducedError;
  uint32_t i;
  const char* AreaName_p = "TEST_RAM_CSPSA";

  // - How many blocks we should fill with data.
  uint32_t BlocksToFill = 2;

  // - Initialize.
  F_CSPSA_TEST_InitTestCase(TestCaseTableIndex);
  if (!CSPSA_TEST_CanInduceMediaErrors())
  {
    // - It is not possible to induce read/write errors; abort this test case.
    CurrentTestCase_p->TcStatus = T_CSPSA_TEST_TC_STATUS_NOT_APPLICABLE;
    goto ErrorExit;
  }
  CurrentTestCase_p->TcStatus = T_CSPSA_TEST_TC_STATUS_VALID;
  F_CSPSA_TEST_ClearSimpleList();
  CSPSA_Result = CSPSA_Create(AreaName_p, &V_CSPSA_TEST_Handle);
  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
  // - Insert some items in the list until at least 2 blocks are filled.
  for (i = 0; F_CSPSA_TEST_GetSizeOfAllParameters(V_CSPSA_TEST_Handle)
                        < BlocksToFill * V_CSPSA_TEST_Config.BlockSize; ++i)
  {
    bool Ok = F_CSPSA_TEST_InsertItem(i, i);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(Ok);
  }
  // - Flush and close.
  CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
  // - Close.
  CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
  // - Induce read errors.
  InducedError.MediaError = T_CSPSA_MEDIA_ERROR_READ_FAILED;
  InducedError.AtBothEnds = true;
  // - Loop to induce a read error on every block.
  for (i = 0; i < BlocksToFill; ++i)
  {
    InducedError.Offset = i * V_CSPSA_TEST_Config.BlockSize;
    CSPSA_TEST_InduceError(&InducedError);
    // - Open (should result in read error).
    CSPSA_Result = CSPSA_Open(AreaName_p, &V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_E_READ_ERROR);
  }
  // - Loop to induce reading of corrupt data in every 32-bit word of the first 2 blocks.
  InducedError.MediaError = T_CSPSA_MEDIA_ERROR_READ_CORRUPT_DATA;
  for (i = 0; i < V_CSPSA_TEST_Config.BlockSize/2; ++i)
  {
    InducedError.Offset = 4*i;// + (i&0x3); // alternate byte.
    InducedError.XorMask = 1 << ((i*5) & 0x7); // single bit error (alternate which bit).
    CSPSA_TEST_InduceError(&InducedError);
    // - Open (should result in CRC check to fail).
    CSPSA_Result = CSPSA_Open(AreaName_p, &V_CSPSA_TEST_Handle);
    //printf("Open after corrupt data on %lld: %d\n", InducedError.Offset, CSPSA_Result);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result != T_CSPSA_RESULT_OK);
  }
  CSPSA_TEST_RAM_Close();
  CurrentTestCase_p->TestOk = true;

ErrorExit:

  if (CSPSA_TEST_CanInduceMediaErrors())
  {
    // - Stop inducing errors.
    CSPSA_TEST_InduceError(NULL);
    F_CSPSA_TEST_FinishTestCase(TestCaseTableIndex);
    CSPSA_TEST_RAM_Close();
  }

  if (V_CSPSA_TEST_Handle != 0)
  {
    (void) CSPSA_Close(&V_CSPSA_TEST_Handle);
  }

}

// -----------------------------------------------------------------------------
//  - CSPSA_TEST_TC_17
//
//    § Check that image is stored alternating at beginning and end.
//
//   (a) Write value and flush seven times and check that image is stored alternating at
//       beginning and end.

void CSPSA_TEST_TC_17(const uint32_t TestCaseTableIndex)
{
  CSPSA_TEST_TC_t* CurrentTestCase_p = &(CSPSA_TEST_TC_Table[TestCaseTableIndex]);
  uint32_t TestDataSize;
  CSPSA_Result_t CSPSA_Result;
  CSPSA_TEST_TestType_t TestType;
  uint32_t Iterations;
  bool DataAtEnd = true;
  uint32_t ExpectedIndex = 0;
  uint32_t BlockSize = V_CSPSA_TEST_Config.BlockSize;

  // - Clear all time measurment values.
  memset((uint8_t*)&V_CSPSA_TEST_TimeMeasurement, 0, sizeof(V_CSPSA_TEST_TimeMeasurement));

  // - Initialize.

#if (CFG_CSPSA_TEST_TARGET == CFG_CSPSA_TEST_TARGET_LINUX_US) && (CFG_CSPSA_TEST_VARIANT == CFG_CSPSA_TEST_VARIANT_CORE)
  CurrentTestCase_p->TcStatus = T_CSPSA_TEST_TC_STATUS_VALID;
#else
  CurrentTestCase_p->TcStatus = T_CSPSA_TEST_TC_STATUS_NOT_APPLICABLE;
#endif
  V_CSPSA_TEST_ErrorLine = 0;
  F_CSPSA_TEST_ResetString(V_CSPSA_TEST_TempString_p);
  if (CurrentTestCase_p->TcStatus == T_CSPSA_TEST_TC_STATUS_NOT_APPLICABLE)
  {
    goto ErrorExit;
  }

  F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_TotalStartTime);

  // - Create CSPSA in cache.
  F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
  F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
  F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CREATE], &V_CSPSA_TEST_ElapsedTime);

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// - (a) Write value and flush seven times and check that image is stored alternating at
  //       beginning and end.
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  V_CSPSA_TEST_ConstSize = 23;
  TestType = T_CSPSA_TEST_TEST_TYPE_KEY_INC_SIZE_CONST;
  TestDataSize = 31 * V_CSPSA_TEST_ConstSize;
  Iterations = 7;

  while (Iterations-- > 0)
  {
    DataAtEnd = !DataAtEnd;
    ExpectedIndex++;

    // - Write values.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_1ST], &V_CSPSA_TEST_ElapsedTime);

    // - Flush.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FLUSH], &V_CSPSA_TEST_ElapsedTime);

    // - Check header.
    {
      CSPSA_CORE_Header_t* Header_p = NULL;
      uint32_t Offset = (DataAtEnd) ? V_CSPSA_TEST_Config.SizeInBytes - BlockSize : 0;
      bool ReadBackwards = DataAtEnd;

      memset(V_CSPSA_TEST_ReadData_p, 0, sizeof(V_CSPSA_TEST_ReadData_p));

      CSPSA_Result = CSPSA_LL_Read(V_CSPSA_TEST_Handle, Offset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

  		Header_p = (CSPSA_CORE_Header_t*) (ptrdiff_t) V_CSPSA_TEST_ReadData_p;
      M_CSPSA_TEST_ASSERT_ERROR_LINE(Header_p->Magic == C_CSPSA_TEST_HeaderMagic) ;
      M_CSPSA_TEST_ASSERT_ERROR_LINE(Header_p->Index == ExpectedIndex) ;
    }
    // - Check !header.
    {
      CSPSA_CORE_Header_t* Header_p = NULL;
      uint32_t Offset = (DataAtEnd) ? 0 : V_CSPSA_TEST_Config.SizeInBytes - BlockSize;
      bool ReadBackwards = !DataAtEnd;

      memset(V_CSPSA_TEST_ReadData_p, 0, sizeof(V_CSPSA_TEST_ReadData_p));

      CSPSA_Result = CSPSA_LL_Read(V_CSPSA_TEST_Handle, Offset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

  		Header_p = (CSPSA_CORE_Header_t*) (ptrdiff_t) V_CSPSA_TEST_ReadData_p;
      M_CSPSA_TEST_ASSERT_ERROR_LINE(Header_p->Magic != C_CSPSA_TEST_HeaderMagic) ;
    }

    DataAtEnd = !DataAtEnd;
    ExpectedIndex++;

    // - Write values.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = F_CSPSA_TEST_WriteValues(V_CSPSA_TEST_Handle, TestDataSize, TestType);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_1ST], &V_CSPSA_TEST_ElapsedTime);

    // - Flush.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FLUSH], &V_CSPSA_TEST_ElapsedTime);

    // - Close.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);

    // - Check header.
    {
      CSPSA_CORE_Header_t* Header_p = NULL;
      uint32_t Offset = (DataAtEnd) ? V_CSPSA_TEST_Config.SizeInBytes - BlockSize : 0;
      bool ReadBackwards = DataAtEnd;

    	// - Use a fake session info struct to be able to open ll driver.
    	V_CSPSA_TEST_SessionInfo.Config = V_CSPSA_TEST_Config;
    	V_CSPSA_TEST_Handle = (CSPSA_Handle_t) &V_CSPSA_TEST_SessionInfo;
      F_CSPSA_TEST_InitFlashMemory(&V_CSPSA_TEST_SessionInfo.Config);

    	// - Open a low level driver.
    	CSPSA_Result = CSPSA_LL_Open(V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

      memset(V_CSPSA_TEST_ReadData_p, 0, sizeof(V_CSPSA_TEST_ReadData_p));

      CSPSA_Result = CSPSA_LL_Read(V_CSPSA_TEST_Handle, Offset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

  		Header_p = (CSPSA_CORE_Header_t*) (ptrdiff_t) V_CSPSA_TEST_ReadData_p;
      M_CSPSA_TEST_ASSERT_ERROR_LINE(Header_p->Magic == C_CSPSA_TEST_HeaderMagic) ;
      M_CSPSA_TEST_ASSERT_ERROR_LINE(Header_p->Index == ExpectedIndex) ;

    	CSPSA_Result = CSPSA_LL_Close(V_CSPSA_TEST_Handle);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    }

    // - Open.
    F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
    CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
    F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_OPEN], &V_CSPSA_TEST_ElapsedTime);

    // - Check header.
    {
      CSPSA_CORE_Header_t* Header_p = NULL;
      uint32_t BlockSize = V_CSPSA_TEST_Config.BlockSize;
      uint32_t Offset = (DataAtEnd) ? V_CSPSA_TEST_Config.SizeInBytes - BlockSize : 0;
      bool ReadBackwards = false;

      memset(V_CSPSA_TEST_ReadData_p, 0, sizeof(V_CSPSA_TEST_ReadData_p));

      CSPSA_Result = CSPSA_LL_Read(V_CSPSA_TEST_Handle, Offset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

  		Header_p = (CSPSA_CORE_Header_t*) (ptrdiff_t) V_CSPSA_TEST_ReadData_p;
      M_CSPSA_TEST_ASSERT_ERROR_LINE(Header_p->Magic == C_CSPSA_TEST_HeaderMagic) ;
      M_CSPSA_TEST_ASSERT_ERROR_LINE(Header_p->Index == ExpectedIndex) ;
    }
    // - Check !header.
    {
      CSPSA_CORE_Header_t* Header_p = NULL;
      uint32_t BlockSize = V_CSPSA_TEST_Config.BlockSize;
      uint32_t Offset = (DataAtEnd) ? 0 : V_CSPSA_TEST_Config.SizeInBytes - BlockSize;
      bool ReadBackwards = false;

      memset(V_CSPSA_TEST_ReadData_p, 0, sizeof(V_CSPSA_TEST_ReadData_p));

      CSPSA_Result = CSPSA_LL_Read(V_CSPSA_TEST_Handle, Offset, V_CSPSA_TEST_ReadData_p, BlockSize, ReadBackwards);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

  		Header_p = (CSPSA_CORE_Header_t*) (ptrdiff_t) V_CSPSA_TEST_ReadData_p;
      M_CSPSA_TEST_ASSERT_ERROR_LINE(Header_p->Magic != C_CSPSA_TEST_HeaderMagic) ;
    }
  }

  // - Close.
  F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
  CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
  F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
  F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE], &V_CSPSA_TEST_ElapsedTime);

  F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_TotalStartTime, &V_CSPSA_TEST_TotalElapsedTime);

  CurrentTestCase_p->TestOk = true;

ErrorExit:

  if (CurrentTestCase_p->TcStatus == T_CSPSA_TEST_TC_STATUS_VALID)
  {
    if (CurrentTestCase_p->TestOk)
    {
      sprintf(V_CSPSA_TEST_TempString_p, "Ok.");
      sprintf(V_CSPSA_TEST_TempString_p+strlen(V_CSPSA_TEST_TempString_p),
        ";%lu;%lu;%lu;%lu;%lu;%lu;%lu;%lu;%lu;%lu\n",
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TotalElapsedTime),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_OPEN]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CLOSE]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_READ]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_1ST]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_WRITE_2ND]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FLUSH]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_FORMAT]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CREATE]),
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_DELETE])
      );
    }
    else
    {
      sprintf(
        V_CSPSA_TEST_TempString_p,
        "Failed;Line %u;TestDataSize=%u;TestType=%u",
        V_CSPSA_TEST_ErrorLine,
        TestDataSize,
        TestType
      );
      if (V_CSPSA_TEST_Handle != 0)
      {
        (void) CSPSA_Close(&V_CSPSA_TEST_Handle);
      }
    }

    F_CSPSA_TEST_AppendTestCaseResult(TestCaseTableIndex, V_CSPSA_TEST_TempString_p);

  }
}

// -----------------------------------------------------------------------------
//  - CSPSA_TEST_TC_18
//
//    § Check handling of situation where key is at end of a block, and values can use more
//      than 1 block.
//

void CSPSA_TEST_TC_18(const uint32_t TestCaseTableIndex)
{
  CSPSA_TEST_TC_t* CurrentTestCase_p = &(CSPSA_TEST_TC_Table[TestCaseTableIndex]);
  uint32_t BlockSize = V_CSPSA_TEST_Config.BlockSize;
  CSPSA_Result_t CSPSA_Result;
  bool Ok;
  uint32_t Size;

  // - Clear all time measurment values.
  memset((uint8_t*)&V_CSPSA_TEST_TimeMeasurement, 0, sizeof(V_CSPSA_TEST_TimeMeasurement));

  // - Initialize.

  CurrentTestCase_p->TcStatus = T_CSPSA_TEST_TC_STATUS_VALID;
  V_CSPSA_TEST_ErrorLine = 0;
  F_CSPSA_TEST_ResetString(V_CSPSA_TEST_TempString_p);
  if (CurrentTestCase_p->TcStatus == T_CSPSA_TEST_TC_STATUS_NOT_APPLICABLE)
  {
    goto ErrorExit;
  }

  F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_TotalStartTime);

  // - Create CSPSA in cache.
  F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_StartTime);
  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
  F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_StartTime, &V_CSPSA_TEST_ElapsedTime);
  F_CSPSA_TEST_TimeAccumulate(&V_CSPSA_TEST_TimeMeasurement[T_CSPSA_TEST_FUNC_CREATE], &V_CSPSA_TEST_ElapsedTime);

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// - a. Write few values; key 1 and 2 will end up at very end of their block.
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  // - First block.
  Size = BlockSize - sizeof (CSPSA_CORE_Header_t) - sizeof (CSPSA_DataItem_t) - 4;
  // - Parameter 0 will take whole first block except 4 bytes.
  Ok = F_CSPSA_TEST_WriteBlock(V_CSPSA_TEST_Handle, 0, '0', Size);
  M_CSPSA_TEST_ASSERT_ERROR_LINE(Ok);
  Size = BlockSize - sizeof (CSPSA_DataItem_t);
  // - Key of parameter 1 is in first block, size + data in second block + a bit of third block.
  Ok = F_CSPSA_TEST_WriteBlock(V_CSPSA_TEST_Handle, 1, '1', Size);
  M_CSPSA_TEST_ASSERT_ERROR_LINE(Ok);
  // - Key of parameter 2 is in second block, size + data in third block + a bit of fourth block.
  Ok = F_CSPSA_TEST_WriteBlock(V_CSPSA_TEST_Handle, 2, '2', Size + 31);
  M_CSPSA_TEST_ASSERT_ERROR_LINE(Ok);
  // - Parameter 3 is in fourth block.
  Ok = F_CSPSA_TEST_WriteBlock(V_CSPSA_TEST_Handle, 3, '3', Size - 32);
  M_CSPSA_TEST_ASSERT_ERROR_LINE(Ok);
  // - Parameter 4: key is in fourth block, value in 5th.
  Ok = F_CSPSA_TEST_WriteBlock(V_CSPSA_TEST_Handle, 4, '4', 4);
  M_CSPSA_TEST_ASSERT_ERROR_LINE(Ok);
  Ok = F_CSPSA_TEST_FlushCloseReopen(false);
  M_CSPSA_TEST_ASSERT_ERROR_LINE(Ok);
  F_CSPSA_TEST_TimeElapsed(&V_CSPSA_TEST_TotalStartTime, &V_CSPSA_TEST_TotalElapsedTime);
  // - Close.
  CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

  CurrentTestCase_p->TestOk = true;

ErrorExit:

  if (CurrentTestCase_p->TcStatus == T_CSPSA_TEST_TC_STATUS_VALID)
  {
    if (CurrentTestCase_p->TestOk)
    {
      sprintf(V_CSPSA_TEST_TempString_p, "Ok.");
      sprintf(V_CSPSA_TEST_TempString_p+strlen(V_CSPSA_TEST_TempString_p),
        ";%lu\n",
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TotalElapsedTime)
      );
    }
    else
    {
      sprintf(
        V_CSPSA_TEST_TempString_p,
        "Failed;Line %u",
        V_CSPSA_TEST_ErrorLine
      );
      if (V_CSPSA_TEST_Handle != 0)
      {
        (void) CSPSA_Close(&V_CSPSA_TEST_Handle);
      }
    }

    F_CSPSA_TEST_AppendTestCaseResult(TestCaseTableIndex, V_CSPSA_TEST_TempString_p);

  }
}

// -----------------------------------------------------------------------------
//  - CSPSA_TEST_TC_19
//
//    § Check that the fast parameter feature works.
//
//   (a) Write different 32-bit values to the different fast parameters and check result.
//   (b) Negative tests. Try writing an item with different size than 4 bytes, and try deleting
//       a fast parameter.

void CSPSA_TEST_TC_19(const uint32_t TestCaseTableIndex)
{
  CSPSA_TEST_TC_t* CurrentTestCase_p = &(CSPSA_TEST_TC_Table[TestCaseTableIndex]);
  CSPSA_Result_t CSPSA_Result;

  // - Initialize.

  CurrentTestCase_p->TcStatus = T_CSPSA_TEST_TC_STATUS_VALID;
  V_CSPSA_TEST_ErrorLine = 0;
  F_CSPSA_TEST_ResetString(V_CSPSA_TEST_TempString_p);
  if (CurrentTestCase_p->TcStatus == T_CSPSA_TEST_TC_STATUS_NOT_IMPLEMENTED)
  {
    goto ErrorExit;
  }

  F_CSPSA_TEST_TimeStart(&V_CSPSA_TEST_TotalStartTime);

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (a) Write different 32-bit values to the different fast parameters and check result.
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  {
    CSPSA_Key_t Key;
    CSPSA_Size_t Size = 4;
    uint32_t Data;

	  // - Create CSPSA in cache.
	  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Write values.
    for (Key = D_CSPSA_CORE_FIRST_FAST_PARAMETER_KEY; Key < D_CSPSA_CORE_FIRST_FAST_PARAMETER_KEY + D_CSPSA_CORE_NBR_OF_FAST_PARAMETERS - 1; Key++)
    {
      CSPSA_Result = CSPSA_WriteValue(V_CSPSA_TEST_Handle, Key, Size, (CSPSA_Data_t*) &Key);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
    }

    // - Read values before flush.
    for (Key = D_CSPSA_CORE_FIRST_FAST_PARAMETER_KEY; Key < D_CSPSA_CORE_FIRST_FAST_PARAMETER_KEY + D_CSPSA_CORE_NBR_OF_FAST_PARAMETERS - 1; Key++)
    {
      CSPSA_Result = CSPSA_ReadValue(V_CSPSA_TEST_Handle, Key, Size, (CSPSA_Data_t*) &Data);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(Key == Data);
    }

    // - Flush.
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Read values after flush.
    for (Key = D_CSPSA_CORE_FIRST_FAST_PARAMETER_KEY; Key < D_CSPSA_CORE_FIRST_FAST_PARAMETER_KEY + D_CSPSA_CORE_NBR_OF_FAST_PARAMETERS - 1; Key++)
    {
      CSPSA_Result = CSPSA_ReadValue(V_CSPSA_TEST_Handle, Key, Size, (CSPSA_Data_t*) &Data);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(Key == Data);
    }

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

	  // - Open CSPSA again.
	  CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Read values after close~open.
    for (Key = D_CSPSA_CORE_FIRST_FAST_PARAMETER_KEY; Key < D_CSPSA_CORE_FIRST_FAST_PARAMETER_KEY + D_CSPSA_CORE_NBR_OF_FAST_PARAMETERS - 1; Key++)
    {
      CSPSA_Result = CSPSA_ReadValue(V_CSPSA_TEST_Handle, Key, Size, (CSPSA_Data_t*) &Data);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(Key == Data);
    }

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // - (b) Negative tests. Try writing an item with different size than 4 bytes, and try deleting
  //       a fast parameter.
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  {
    CSPSA_Key_t Key;
    CSPSA_Size_t Size = 1;
    uint32_t Data;

	  // - Create CSPSA in cache.
	  CSPSA_Result = CSPSA_Create(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Write data of illegal sizes.
    for (Key = D_CSPSA_CORE_FIRST_FAST_PARAMETER_KEY; Key < D_CSPSA_CORE_FIRST_FAST_PARAMETER_KEY + D_CSPSA_CORE_NBR_OF_FAST_PARAMETERS - 1; Key++)
    {
      CSPSA_Result = CSPSA_WriteValue(V_CSPSA_TEST_Handle, Key, Size, (CSPSA_Data_t*) &Key);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_E_INVALID_SIZE);
      Size++;
      if (Size == sizeof(V_CSPSA_TEST_SessionInfo.LastStorageImageHeader_p->FastParameter[0]))
      {
        Size++;
      }
    }

    // - Read values before flush.
    for (Key = D_CSPSA_CORE_FIRST_FAST_PARAMETER_KEY; Key < D_CSPSA_CORE_FIRST_FAST_PARAMETER_KEY + D_CSPSA_CORE_NBR_OF_FAST_PARAMETERS - 1; Key++)
    {
      CSPSA_Result = CSPSA_ReadValue(V_CSPSA_TEST_Handle, Key, sizeof(V_CSPSA_TEST_SessionInfo.LastStorageImageHeader_p->FastParameter[0]), (CSPSA_Data_t*) &Data);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(0 == Data);
    }

    // - Flush.
    CSPSA_Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Write data of illegal sizes.
    for (Key = D_CSPSA_CORE_FIRST_FAST_PARAMETER_KEY; Key < D_CSPSA_CORE_FIRST_FAST_PARAMETER_KEY + D_CSPSA_CORE_NBR_OF_FAST_PARAMETERS - 1; Key++)
    {
      CSPSA_Result = CSPSA_WriteValue(V_CSPSA_TEST_Handle, Key, Size, (CSPSA_Data_t*) &Key);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_E_INVALID_SIZE);
      Size++;
      if (Size == sizeof(V_CSPSA_TEST_SessionInfo.LastStorageImageHeader_p->FastParameter[0]))
      {
        Size++;
      }
    }

    // - Read values after flush.
    for (Key = D_CSPSA_CORE_FIRST_FAST_PARAMETER_KEY; Key < D_CSPSA_CORE_FIRST_FAST_PARAMETER_KEY + D_CSPSA_CORE_NBR_OF_FAST_PARAMETERS - 1; Key++)
    {
      CSPSA_Result = CSPSA_ReadValue(V_CSPSA_TEST_Handle, Key, sizeof(V_CSPSA_TEST_SessionInfo.LastStorageImageHeader_p->FastParameter[0]), (CSPSA_Data_t*) &Data);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(0 == Data);
    }

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

	  // - Open CSPSA again.
	  CSPSA_Result = CSPSA_Open(V_CSPSA_TEST_Config.Name, &V_CSPSA_TEST_Handle);
	  M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);

    // - Write data of illegal sizes.
    for (Key = D_CSPSA_CORE_FIRST_FAST_PARAMETER_KEY; Key < D_CSPSA_CORE_FIRST_FAST_PARAMETER_KEY + D_CSPSA_CORE_NBR_OF_FAST_PARAMETERS - 1; Key++)
    {
      CSPSA_Result = CSPSA_WriteValue(V_CSPSA_TEST_Handle, Key, Size, (CSPSA_Data_t*) &Key);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_E_INVALID_SIZE);
      Size++;
      if (Size == sizeof(V_CSPSA_TEST_SessionInfo.LastStorageImageHeader_p->FastParameter[0]))
      {
        Size++;
      }
    }

    // - Read values after close~open.
    for (Key = D_CSPSA_CORE_FIRST_FAST_PARAMETER_KEY; Key < D_CSPSA_CORE_FIRST_FAST_PARAMETER_KEY + D_CSPSA_CORE_NBR_OF_FAST_PARAMETERS - 1; Key++)
    {
      CSPSA_Result = CSPSA_ReadValue(V_CSPSA_TEST_Handle, Key, sizeof(V_CSPSA_TEST_SessionInfo.LastStorageImageHeader_p->FastParameter[0]), (CSPSA_Data_t*) &Data);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
      M_CSPSA_TEST_ASSERT_ERROR_LINE(0 == Data);
    }

    // - Close.
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    M_CSPSA_TEST_ASSERT_ERROR_LINE(CSPSA_Result == T_CSPSA_RESULT_OK);
  }

  CurrentTestCase_p->TestOk = true;

ErrorExit:

  if (CurrentTestCase_p->TcStatus != T_CSPSA_TEST_TC_STATUS_NOT_IMPLEMENTED)
  {
    if (CurrentTestCase_p->TestOk)
    {
      sprintf(V_CSPSA_TEST_TempString_p, "Ok.");
      sprintf(V_CSPSA_TEST_TempString_p+strlen(V_CSPSA_TEST_TempString_p),
        ";%lu\n",
        (unsigned long int) F_CSPSA_TEST_TimeInMs(&V_CSPSA_TEST_TotalElapsedTime)
      );
    }
    else
    {
      sprintf(
        V_CSPSA_TEST_TempString_p,
        "Failed;Line %u",
        V_CSPSA_TEST_ErrorLine
      );
      if (V_CSPSA_TEST_Handle != 0)
      {
        (void) CSPSA_Close(&V_CSPSA_TEST_Handle);
      }
    }

    F_CSPSA_TEST_AppendTestCaseResult(TestCaseTableIndex, V_CSPSA_TEST_TempString_p);

  }
}

// -----------------------------------------------------------------------------
//  - CSPSA_TEST_TC_X
//
//    § ???
//
//   (a) ???

//void CSPSA_TEST_TC_X(const uint32_t TestCaseTableIndex)
//{
//  CSPSA_TEST_TC_t* CurrentTestCase_p = &(CSPSA_TEST_TC_Table[TestCaseTableIndex]);
//
//  CurrentTestCase_p->TcStatus = T_CSPSA_TEST_TC_STATUS_NOT_IMPLEMENTED;
//  V_CSPSA_TEST_ErrorLine = 0;
//  F_CSPSA_TEST_ResetString(V_CSPSA_TEST_TempString_p);
//
//  if (CurrentTestCase_p->TcStatus == T_CSPSA_TEST_TC_STATUS_NOT_IMPLEMENTED)
//  {
//    goto ErrorExit;
//  }
//
//  CurrentTestCase_p->TestOk = true;
//
//ErrorExit:
//
//  if (CurrentTestCase_p->TcStatus != T_CSPSA_TEST_TC_STATUS_NOT_IMPLEMENTED)
//  {
//    if (!CurrentTestCase_p->TestOk)
//    {
//      sprintf(
//        V_CSPSA_TEST_TempString_p,
//        "Failed;Line %u",
//        V_CSPSA_TEST_ErrorLine
//      );
//      if (V_CSPSA_TEST_Handle != 0)
//      {
//        (void) CSPSA_Close(&V_CSPSA_TEST_Handle);
//      }
//    }
//
//    F_CSPSA_TEST_AppendTestCaseResult(TestCaseTableIndex, V_CSPSA_TEST_TempString_p);
//
//  }
//}

// ================================================================================
//   End of file
// =============================================================================
