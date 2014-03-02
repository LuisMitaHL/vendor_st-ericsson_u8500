/*************************************************************************
 * Copyright ST-Ericsson 2012
 ************************************************************************/

#include <cops_test_framework.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>

#define TST_PRINT(ttype, str) \
{ \
    printf("%s: ", __progname); \
    if (ttype == TPASS) \
        printf("TPASS :"); \
    else if (ttype == TFAIL) \
        printf("TFAIL :"); \
    else if (ttype == TBROK) \
        printf("TBROK :"); \
    else if (ttype == TWARN) \
        printf("TWARN :"); \
    else if (ttype == TINFO) \
        printf("TINFO :"); \
    printf(" %s\n", str); \
}

#define EXPAND_VAR_ARGS(arg_fmt, str)     \
{                                         \
    va_list ap; /* varargs mechanism */   \
                                          \
    if (arg_fmt != NULL) {                \
        va_start(ap, arg_fmt);            \
        vsprintf(str, arg_fmt, ap);       \
        va_end(ap);                       \
    } else {                              \
       str[0] = '\0';                     \
    }                                     \
}

void tst_exit(void)
{
    exit(0);
}

void tst_tmpdir(void)
{
    if (mkdir("/tmp/cops_tmp", 0777) != 0) {
        printf("tmpdir failed\n");
    }
    chdir("/tmp/cops_tmp");
}

void tst_rmdir(void)
{
    system("rm -rf /tmp/cops_tmp");
}

void tst_brkm(int ttype, void (*func)(void), char *arg_fmt, ...)
{
    char str[256];
    EXPAND_VAR_ARGS(arg_fmt, str);
    TST_PRINT(ttype, str);
    func();
    tst_exit();
}

void tst_resm(int ttype, char *arg_fmt, ...)
{
    char str[256];
    EXPAND_VAR_ARGS(arg_fmt, str);
    TST_PRINT(ttype, str);
}
