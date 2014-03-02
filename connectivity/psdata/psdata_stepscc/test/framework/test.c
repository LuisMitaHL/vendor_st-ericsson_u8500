/*
 * Copyright (C) 2011, ST-Ericsson
 *
 * File name       : test.c
 * Description     : testframework file
 *
 * Author          : Christian Nilsson <christian.c.nilsson@stericsson.com>
 *
 */

#include <stdio.h>
#include "tc_main.h"
#include "util.h"

int nbrOfTests;
int nbrOfPass = 0;
int nbrOfFail = 0;
/* if anyone fails, we return -1 */
int anyFail = 0;

/********************************/
/* Private function declarations */
/********************************/
int runtests();
void printResults();

/***********************************/
/* Test framework Implementation */
/***********************************/

int main() {
  int runtestresult = 0;

  printf("\n");
  printf("####################\n");
  printf("# Starting test\n");
  printf("####################\n");

  /* init the listening sockets */
  if (createSockets(1) < 0) {
    printf("Could not create sockets\n");
    return 0;
  }
  printf("Starting test execution\n");
  nbrOfTests = sizeof(testcases) / sizeof(testcase_t);
  printf("Number of tests: %d\n", nbrOfTests);
  printf("\n");

  runtestresult = runtests();
  printf("\n");
  printf("Testrun finished\n");
  closeSockets();
  if (runtestresult == -1) {
    printf("Testframework error\n");
  } else {
    printResults();
  }
  return anyFail;
}

/***********************************/
/* Private function implementation */
/***********************************/

/**
 * Method used for executing tests and checking results
 */
int runtests() {
  int i;
  psccd_test_state_type_t result;
  for (i = 0; i < nbrOfTests; i++) {
    printf("# Testcase: %d - %s\n", i + 1,
        testcases[i].test_string);
    /* flush printf */
    fflush(stdout);
    result = testcases[i].psccd_test_fp(0);
    switch (result) {
    case PSCCD_TEST_SUCCESS:
      nbrOfPass++;
      printf("PASS\n");
      break;
    case PSCCD_TEST_FAIL:
      nbrOfFail++;
      anyFail = -1;
      printf("FAIL\n");
      break;
    default:
      printf("ERROR: invalid result from testcase: %d\n", result);
      return -1;
    }
  }
  return 0;
}

/*
 * Method used to print results
 */
void printResults() {
  printf("####################\n");
  printf("# Summary:\n");
  printf("####################\n");
  printf("Number of tests: %d\n", nbrOfTests);
  if (nbrOfTests > 0)
    printf("Number of tests passed: %d (%d percent)\n", nbrOfPass, (int) (100
        * nbrOfPass / nbrOfTests));
  printf("####################\n");
  printf("\n");
  return;
}
