/*************************************************************************
 * Copyright ST-Ericsson 2012
 ************************************************************************/
#ifndef COPS_TEST_FRAMEWORK_H
#define COPS_TEST_FRAMEWORK_H

#include <stdarg.h>

#ifndef PATH_MAX
#ifdef MAXPATHLEN
#define PATH_MAX  MAXPATHLEN
#else
#define PATH_MAX  1024
#endif
#endif

#define TPASS      0    /* Test passed flag */
#define TFAIL      1    /* Test failed flag */
#define TBROK      2    /* Test broken flag */
#define TWARN      3    /* Test warning flag */
#define TINFO      4    /* Test information flag */

#ifndef MIN
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#endif

extern const char *__progname;

/* print result from test operation */
void tst_resm(int ttype, char *arg_fmt, ...)
    __attribute__ ((format (printf, 2, 3)));

/* break test suite and print msg */
void tst_brkm(int ttype, void (*func)(void), char *arg_fmt, ...)
    __attribute__ ((format (printf, 3, 4)));

/* exit test suite */
void tst_exit(void);

/* create temporary directory and step into it */
void tst_tmpdir(void);

/* remove temporary directory */
void tst_rmdir(void);

#endif
