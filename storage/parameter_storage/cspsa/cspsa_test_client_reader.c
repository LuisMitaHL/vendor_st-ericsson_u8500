/* **************************************************************************
 *
 * cspsa_test_client_reader.c
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
 *
 * Simple executable to read from CSPSA:
 *
 *  - argv[1]: Area name string
 *  - argv[2]: Timeout in [s]
 *  - argv[3]: Sleep time in [ms] between reads. Can be set to zero.
 *
 **************************************************************************** */

// =============================================================================
//  Configurations
// =============================================================================

// - CFG_CSPSA_TEST_CLIENT_READER_XXX: Brief.
#ifndef CFG_CSPSA_TEST_CLIENT_READER_XX_YY
#define CFG_CSPSA_TEST_CLIENT_READER_XX_YY                      (1)
#endif

// =============================================================================
//  Include Header Files
// =============================================================================

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#include "cspsa.h"

// =============================================================================
//  Defines
// =============================================================================

// - D_CSPSA_TEST_CLIENT_READER_TEST_DATA_SIZE: Size of test data buffer. Must not be smaller than 4kB.
#define D_CSPSA_TEST_CLIENT_READER_TEST_DATA_SIZE                 	(16*1024)

// =============================================================================
//  Macros
// =============================================================================

// - M_CSPSA_TEST_CLIENT_READER_FREE: Wrapper to free a pointer.
#define M_CSPSA_TEST_CLIENT_READER_FREE(Pointer) \
  do                          \
  {                           \
    free(Pointer);            \
    Pointer = NULL;           \
  } while (0)

// =============================================================================
//  Local Types
// =============================================================================

// =============================================================================
//  Local Constants
// =============================================================================

// =============================================================================
//  Local Variables
// =============================================================================

// - V_CSPSA_TEST_ReadData_p: Common read data buffer.
static uint8_t V_CSPSA_TEST_CLIENT_READER_ReadData_p[D_CSPSA_TEST_CLIENT_READER_TEST_DATA_SIZE];

// =============================================================================
//  Global External Variables
// =============================================================================

// =============================================================================
//  Local Function Declarations
// =============================================================================

static uint64_t F_CSPSA_TEST_CLIENT_READER_TimeInMs(struct timeval *TimeValue);
static int F_CSPSA_TEST_CLIENT_READER_TimeStart(struct timeval *start);
static int F_CSPSA_TEST_CLIENT_READER_TimeElapsed(struct timeval *start, struct timeval *elapsed);

// =============================================================================
//  External Declarations
// =============================================================================

// =============================================================================
//  Local Function Definitions
// =============================================================================

// - F_CSPSA_TEST_CLIENT_READER_TimeInMs: Returns time in milliseconds.

static uint64_t F_CSPSA_TEST_CLIENT_READER_TimeInMs(struct timeval *TimeValue)
{
  return TimeValue->tv_sec * 1000ULL + TimeValue->tv_usec / 1000;
}

// -----------------------------------------------------------------------------

// - F_CSPSA_TEST_CLIENT_READER_TimeStart: Returns start time.

static int F_CSPSA_TEST_CLIENT_READER_TimeStart(struct timeval *start)
{
  return gettimeofday(start, NULL);
}

// -----------------------------------------------------------------------------

// - F_CSPSA_TEST_CLIENT_READER_TimeElapsed: Returns elapsed time.

static int F_CSPSA_TEST_CLIENT_READER_TimeElapsed(struct timeval *start, struct timeval *elapsed)
{
  int Result = F_CSPSA_TEST_CLIENT_READER_TimeStart(elapsed);
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

// =============================================================================
//   Global Function Definitions
// =============================================================================

// =============================================================================
//   Processes()
// =============================================================================

// - CSPSA_TEST_CLIENT_READER_Result_t P_CSPSA_TEST_CLIENT_READER_Zyx(void){}

// =============================================================================
//   main()
// =============================================================================

// - main(): Open CSPSA area and read for.

int main(int argc, char *argv[])
{
  const char DefaultAreaName_p[] = "CSPSA0";
  char* AreaName_p = NULL;
  CSPSA_Result_t CSPSA_Result;
  CSPSA_Handle_t CSPSA_Handle = (CSPSA_Handle_t) (-1);
	struct timeval TotalStartTime;
	struct timeval TotalElapsedTime;
	uint64_t TotalElapsedTimeInMs = 0;
  CSPSA_Key_t Key;
  CSPSA_Size_t Size;
  uint32_t DurationInS;
  uint32_t SleepTimeBetweenReadsInMs;

  // - Get area name.
  if (argc > 1)
  {
    AreaName_p = (char*) malloc(strlen(argv[1]) + 1);
    strcpy(AreaName_p, argv[1]);
  }
  else
  {
    AreaName_p = (char*) malloc(strlen(DefaultAreaName_p) + 1);
    strcpy(AreaName_p, DefaultAreaName_p);
  }

  // - Get duration in [s].
  if (argc > 2)
  {
    DurationInS = atoi(argv[2]);
  }
  else
  {
    DurationInS = 30;
  }
  // - Get sleep time between reads in [s].
  if (argc > 3)
  {
    SleepTimeBetweenReadsInMs = atoi(argv[2]);
  }
  else
  {
    SleepTimeBetweenReadsInMs = 50;
  }

  CSPSA_Result = CSPSA_Open(AreaName_p, &CSPSA_Handle);
  if (CSPSA_Result >= T_CSPSA_RESULT_ERRORS) goto ErrorExit;

	CSPSA_Result = CSPSA_GetSizeOfFirstValue(CSPSA_Handle, &Key, &Size);
  if (CSPSA_Result >= T_CSPSA_RESULT_ERRORS) goto ErrorExit;

	memset(&TotalElapsedTime, 0, sizeof(TotalElapsedTime));
	F_CSPSA_TEST_CLIENT_READER_TimeStart(&TotalStartTime);

  printf("Starting reading from '%s'\n", AreaName_p);

	while (TotalElapsedTimeInMs / 1000 < DurationInS)
	{
    CSPSA_Result = CSPSA_ReadValue(CSPSA_Handle, Key, Size, V_CSPSA_TEST_CLIENT_READER_ReadData_p);
    if (CSPSA_Result >= T_CSPSA_RESULT_ERRORS) goto ErrorExit;

    usleep(SleepTimeBetweenReadsInMs);

		CSPSA_Result = CSPSA_GetSizeOfNextValue(CSPSA_Handle, &Key, &Size);
	  if (CSPSA_Result == T_CSPSA_RESULT_E_END_OF_DATA)
	  {
			CSPSA_Result = CSPSA_GetSizeOfFirstValue(CSPSA_Handle, &Key, &Size);
		  if (CSPSA_Result >= T_CSPSA_RESULT_ERRORS) goto ErrorExit;
	  }
    else
	  if (CSPSA_Result >= T_CSPSA_RESULT_ERRORS)
    {
      goto ErrorExit;
    }

	  F_CSPSA_TEST_CLIENT_READER_TimeElapsed(&TotalStartTime, &TotalElapsedTime);
	  TotalElapsedTimeInMs = F_CSPSA_TEST_CLIENT_READER_TimeInMs(&TotalElapsedTime);
	}

  printf("Stopped reading from '%s'\n", AreaName_p);

ErrorExit:

  if (CSPSA_Handle != (CSPSA_Handle_t) (-1))
  {
    (void) CSPSA_Close(&CSPSA_Handle);
  }
  M_CSPSA_TEST_CLIENT_READER_FREE(AreaName_p);

  return CSPSA_Result;
}

// =============================================================================
//   End of file
// =============================================================================
