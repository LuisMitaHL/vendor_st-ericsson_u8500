/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include <stdlib.h>

#include "makefiles-tests.h"
#include "makefiles-tests-CC.h"

#define REPORT_STRING_FMT "%-30s: %s\n"
#define OK_STR "OK"
#define KO_STR "FAILED"

int main()
{
    int nb_errors = 0;
    float float_a, float_b, float_c;
    double double_a, double_b, double_c;
    
    printf("Starting makefiles tests\n");
    
    double_a = 1.0;
    double_b = 2.0;
    double_c = 4.0;
    
    float_a = 1.0;
    float_b = 2.0;
    float_c = 4.0;
    
    if((float_a+float_b) != makefiles_tests_float_add(float_a,float_b)) {
	printf(REPORT_STRING_FMT, "makefiles_tests_float_add", KO_STR);
	nb_errors ++;
    } else {
	printf(REPORT_STRING_FMT, "makefiles_tests_float_add", OK_STR);	
    }
    if((float_a-float_b) != makefiles_tests_float_sub(float_a,float_b)) {
	printf(REPORT_STRING_FMT, "makefiles_tests_float_sub", KO_STR);
	nb_errors ++;
    } else {
	printf(REPORT_STRING_FMT, "makefiles_tests_float_sub", OK_STR);	
    }
    
    if((double_a+double_b) != makefiles_tests_double_add(double_a,double_b)) {
	printf(REPORT_STRING_FMT, "makefiles_tests_double_add", KO_STR);
	nb_errors ++;
    } else {
	printf(REPORT_STRING_FMT, "makefiles_tests_double_add", OK_STR);	
    }
    if((double_a-double_b) != makefiles_tests_double_sub(double_a,double_b)) {
	printf(REPORT_STRING_FMT, "makefiles_tests_double_sub", KO_STR);
	nb_errors ++;
    } else {
	printf(REPORT_STRING_FMT, "makefiles_tests_double_sub", OK_STR);	
    }

    MakefileTest_Hello();
    MakefileTest_cplusplus();
    
    printf("makefiles tests: %d errors\n", nb_errors);

    return nb_errors;
}
