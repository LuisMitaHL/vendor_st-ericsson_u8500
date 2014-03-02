/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "testcases.h"
#include "module_test_main.h"
#include "cn_client.h"
#include "cn_log.h"
#include "cn_ss_command_handling.h"


/*
 * SS DECODER TESTCASES
 */

/***************************************************************************************
 * TESTCASE: ss_decoder_positive_1                                                     *
 ***************************************************************************************
 *                                                                                     *
 * INPUT:                                                                              *
 * ======                                                                              *
 * SS pattern: "*SC*#"                                                                 *
 * SS string:  "*21*#"                                                                 *
 *                                                                                     *
 * EXPECTED OUTPUT:                                                                    *
 * ================                                                                    *
 * Procedure type: CN_SS_PROCEDURE_TYPE_ACTIVATION                                     *
 * Service code:   CN_SS_TYPE_CALL_FORWARDING_UNCONDITIONAL (21)                       *
 * Supplementary information A: N/A                                                    *
 * Supplementary information B: N/A                                                    *
 * Supplementary information C: N/A                                                    *
 *                                                                                     *
 ***************************************************************************************/
tc_result_t ss_decoder_positive_1() 
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_ss_string_compounds_t* command_p = NULL;
    char* ss_string_p = "*21*#";

    command_p = decode_ss_string(ss_string_p);
    TC_ASSERT(CN_SS_PROCEDURE_TYPE_ACTIVATION == command_p->procedure_type);
    TC_ASSERT(CN_SS_TYPE_CALL_FORWARDING_UNCONDITIONAL == command_p->ss_type);
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_a,""));
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_b,""));
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_c,""));

exit:
    if (command_p) { free(command_p); }
    return tc_result;
}

/***************************************************************************************
 * TESTCASE: ss_decoder_positive_2                                                     *
 ***************************************************************************************
 *                                                                                     *
 * INPUT:                                                                              *
 * ======                                                                              *
 * SS pattern: "#SC*SI#"                                                               *
 * SS string:  "#21*+4425993004126#"                                                   *
 *                                                                                     *
 * EXPECTED OUTPUT:                                                                    *
 * ================                                                                    *
 * Procedure type: CN_SS_PROCEDURE_TYPE_DEACTIVATION                                   *
 * Service code:   CN_SS_TYPE_CALL_FORWARDING_UNCONDITIONAL (21)                       *
 * Supplementary information A: "+4425993004126"                                       *
 * Supplementary information B: N/A                                                    *
 * Supplementary information C: N/A                                                    *
 *                                                                                     *
 ***************************************************************************************/
tc_result_t ss_decoder_positive_2() 
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_ss_string_compounds_t* command_p = NULL;
    char* ss_string_p = "#21*+4425993004126#";

    command_p = decode_ss_string(ss_string_p);
    TC_ASSERT(CN_SS_PROCEDURE_TYPE_DEACTIVATION == command_p->procedure_type);
    TC_ASSERT(CN_SS_TYPE_CALL_FORWARDING_UNCONDITIONAL == command_p->ss_type);
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_a,"+4425993004126"));
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_b,""));
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_c,""));

exit:
    if (command_p) { free(command_p); }
    return tc_result;
}

/***************************************************************************************
 * TESTCASE: ss_decoder_positive_3                                                     *
 ***************************************************************************************
 *                                                                                     *
 * INPUT:                                                                              *
 * ======                                                                              *
 * SS pattern: "*#SC*SI#"                                                              *
 * SS string:  "*#21*+4425993004126#"                                                  *
 *                                                                                     *
 * EXPECTED OUTPUT:                                                                    *
 * ================                                                                    *
 * Procedure type: CN_SS_PROCEDURE_TYPE_INTERROGATION                                  *
 * Service code:   CN_SS_TYPE_CALL_FORWARDING_UNCONDITIONAL (21)                       *
 * Supplementary information A: "+4425993004126"                                       *
 * Supplementary information B: N/A                                                    *
 * Supplementary information C: N/A                                                    *
 *                                                                                     *
 ***************************************************************************************/
tc_result_t ss_decoder_positive_3() 
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_ss_string_compounds_t* command_p = NULL;
    char* ss_string_p = "*#21*+4425993004126#";

    command_p = decode_ss_string(ss_string_p);
    TC_ASSERT(CN_SS_PROCEDURE_TYPE_INTERROGATION == command_p->procedure_type);
    TC_ASSERT(CN_SS_TYPE_CALL_FORWARDING_UNCONDITIONAL == command_p->ss_type);
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_a,"+4425993004126"));
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_b,""));
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_c,""));

exit:
    if (command_p) { free(command_p); }
    return tc_result;
}

/***************************************************************************************
 * TESTCASE: ss_decoder_positive_4                                                     *
 ***************************************************************************************
 *                                                                                     *
 * INPUT:                                                                              *
 * ======                                                                              *
 * SS pattern: "*SC*SI#"                                                               *
 * SS string:  "*21*+4425993004126#"                                                   *
 *                                                                                     *
 * EXPECTED OUTPUT:                                                                    *
 * ================                                                                    *
 * Procedure type: CN_SS_PROCEDURE_TYPE_REGISTRATION                                   *
 * Service code:   CN_SS_TYPE_CALL_FORWARDING_UNCONDITIONAL (21)                       *
 * Supplementary information A: "+4425993004126"                                       *
 * Supplementary information B: N/A                                                    *
 * Supplementary information C: N/A                                                    *
 *                                                                                     *
 ***************************************************************************************/
tc_result_t ss_decoder_positive_4() 
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_ss_string_compounds_t* command_p = NULL;
    char* ss_string_p = "*21*+4425993004126#";

    command_p = decode_ss_string(ss_string_p);
    TC_ASSERT(CN_SS_PROCEDURE_TYPE_REGISTRATION == command_p->procedure_type);
    TC_ASSERT(CN_SS_TYPE_CALL_FORWARDING_UNCONDITIONAL == command_p->ss_type);
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_a,"+4425993004126"));
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_b,""));
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_c,""));

exit:
    if (command_p) { free(command_p); }
    return tc_result;
}

/***************************************************************************************
 * TESTCASE: ss_decoder_positive_5                                                     *
 ***************************************************************************************
 *                                                                                     *
 * INPUT:                                                                              *
 * ======                                                                              *
 * SS pattern: "**SC*SI#"                                                              *
 * SS string:  "**21*+4425993004126#"                                                  *
 *                                                                                     *
 * EXPECTED OUTPUT:                                                                    *
 * ================                                                                    *
 * Procedure type: CN_SS_PROCEDURE_TYPE_REGISTRATION                                   *
 * Service code:   CN_SS_TYPE_CALL_FORWARDING_UNCONDITIONAL (21)                       *
 * Supplementary information A: "+4425993004126"                                       *
 * Supplementary information B: N/A                                                    *
 * Supplementary information C: N/A                                                    *
 *                                                                                     *
 ***************************************************************************************/
tc_result_t ss_decoder_positive_5()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_ss_string_compounds_t* command_p = NULL;
    char* ss_string_p = "**21*+4425993004126#";

    command_p = decode_ss_string(ss_string_p);
    TC_ASSERT(CN_SS_PROCEDURE_TYPE_REGISTRATION == command_p->procedure_type);
    TC_ASSERT(CN_SS_TYPE_CALL_FORWARDING_UNCONDITIONAL == command_p->ss_type);
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_a,"+4425993004126"));
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_b,""));
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_c,""));

exit:
    if (command_p) { free(command_p); }
    return tc_result;
}

/***************************************************************************************
 * TESTCASE: ss_decoder_positive_6                                                     *
 ***************************************************************************************
 *                                                                                     *
 * INPUT:                                                                              *
 * ======                                                                              *
 * SS pattern: "##SC*SI#"                                                              *
 * SS string:  "##21*+4425993004126#"                                                  *
 *                                                                                     *
 * EXPECTED OUTPUT:                                                                    *
 * ================                                                                    *
 * Procedure type: CN_SS_PROCEDURE_TYPE_ERASURE                                        *
 * Service code:   CN_SS_TYPE_CALL_FORWARDING_UNCONDITIONAL (21)                       *
 * Supplementary information A: "+4425993004126"                                       *
 * Supplementary information B: N/A                                                    *
 * Supplementary information C: N/A                                                    *
 *                                                                                     *
 ***************************************************************************************/
tc_result_t ss_decoder_positive_6() 
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_ss_string_compounds_t* command_p = NULL;
    char* ss_string_p = "##21*+4425993004126#";

    command_p = decode_ss_string(ss_string_p);
    TC_ASSERT(CN_SS_PROCEDURE_TYPE_ERASURE == command_p->procedure_type);
    TC_ASSERT(CN_SS_TYPE_CALL_FORWARDING_UNCONDITIONAL == command_p->ss_type);
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_a,"+4425993004126"));
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_b,""));
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_c,""));

exit:
    if (command_p) { free(command_p); }
    return tc_result;
}

/***************************************************************************************
 * TESTCASE: ss_decoder_positive_7                                                     *
 ***************************************************************************************
 *                                                                                     *
 * INPUT:                                                                              *
 * ======                                                                              *
 * SS pattern: "*SC*SIA*SIB*SIC#"                                                      *
 * SS string:  "*753*ABC*DEF*123#"                                                     *
 *                                                                                     *
 * EXPECTED OUTPUT:                                                                    *
 * ================                                                                    *
 * Procedure type: CN_SS_PROCEDURE_TYPE_REGISTRATION                                   *
 * Service code:   CN_SS_TYPE_ENHANCED_MULTILEVEL_PRECEDENCE_AND_PREEMPTION3 (753)     *
 * Supplementary information A: "ABC"                                                  *
 * Supplementary information B: "DEF"                                                  *
 * Supplementary information C: "123"                                                  *
 *                                                                                     *
 ***************************************************************************************/
tc_result_t ss_decoder_positive_7()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_ss_string_compounds_t* command_p = NULL;
    char* ss_string_p = "*753*ABC*DEF*123#";

    command_p = decode_ss_string(ss_string_p);
    TC_ASSERT(CN_SS_PROCEDURE_TYPE_REGISTRATION == command_p->procedure_type);
    TC_ASSERT(CN_SS_TYPE_ENHANCED_MULTILEVEL_PRECEDENCE_AND_PREEMPTION3 == command_p->ss_type);
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_a,"ABC"));
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_b,"DEF"));
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_c,"123"));

exit:
    if (command_p) { free(command_p); }
    return tc_result;
}

/***************************************************************************************
 * TESTCASE: ss_decoder_positive_8                                                     *
 ***************************************************************************************
 *                                                                                     *
 * INPUT:                                                                              *
 * ======                                                                              *
 * SS pattern: "*SC*SIA*SIB#"                                                          *
 * SS string:  "*752*AB*C#"                                                            *
 *                                                                                     *
 * EXPECTED OUTPUT:                                                                    *
 * ================                                                                    *
 * Procedure type: CN_SS_PROCEDURE_TYPE_REGISTRATION                                   *
 * Service code:   CN_SS_TYPE_ENHANCED_MULTILEVEL_PRECEDENCE_AND_PREEMPTION2 (752)     *
 * Supplementary information A: "AB"                                                   *
 * Supplementary information B: "C"                                                    *
 * Supplementary information C: N/A                                                    *
 *                                                                                     *
 ***************************************************************************************/
tc_result_t ss_decoder_positive_8() 
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_ss_string_compounds_t* command_p = NULL;
    char* ss_string_p = "*752*AB*C#";

    command_p = decode_ss_string(ss_string_p);
    TC_ASSERT(CN_SS_PROCEDURE_TYPE_REGISTRATION == command_p->procedure_type);
    TC_ASSERT(CN_SS_TYPE_ENHANCED_MULTILEVEL_PRECEDENCE_AND_PREEMPTION2 == command_p->ss_type);
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_a,"AB"));
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_b,"C"));
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_c,""));

exit:
    if (command_p) { free(command_p); }
    return tc_result;
}

/***************************************************************************************
 * TESTCASE: ss_decoder_positive_9                                                     *
 ***************************************************************************************
 *                                                                                     *
 * INPUT:                                                                              *
 * ======                                                                              *
 * SS pattern: "*SC*SIA**SIC#"                                                         *
 * SS string:  "*754*E**3456423545345435345#"                                          *
 *                                                                                     *
 * EXPECTED OUTPUT:                                                                    *
 * ================                                                                    *
 * Procedure type: CN_SS_PROCEDURE_TYPE_REGISTRATION                                   *
 * Service code:   CN_SS_TYPE_ENHANCED_MULTILEVEL_PRECEDENCE_AND_PREEMPTION4 (754)     *
 * Supplementary information A: "E"                                                    *
 * Supplementary information B: N/A                                                    *
 * Supplementary information C: "3456423545345435345"                                  *
 *                                                                                     *
 ***************************************************************************************/
tc_result_t ss_decoder_positive_9() 
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_ss_string_compounds_t* command_p = NULL;
    char* ss_string_p = "*754*E**3456423545345435345#";

    command_p = decode_ss_string(ss_string_p);
    TC_ASSERT(CN_SS_PROCEDURE_TYPE_REGISTRATION == command_p->procedure_type);
    TC_ASSERT(CN_SS_TYPE_ENHANCED_MULTILEVEL_PRECEDENCE_AND_PREEMPTION4 == command_p->ss_type);
    printf("A = %s, B=%s C=%s \n",command_p->supplementary_info_a,command_p->supplementary_info_b,command_p->supplementary_info_c);
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_a,"E"));
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_b,""));
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_c,"3456423545345435345"));

exit:
    if (command_p) { free(command_p); }
    return tc_result;
}

/***************************************************************************************
 * TESTCASE: ss_decoder_positive_10                                                    *
 ***************************************************************************************
 *                                                                                     *
 * INPUT:                                                                              *
 * ======                                                                              *
 * SS pattern: "*SC*SIA#"                                                              *
 * SS string:  "*75*7887287832873827836263652563656E235625#"                           *
 *                                                                                     *
 * EXPECTED OUTPUT:                                                                    *
 * ================                                                                    *
 * Procedure type: CN_SS_PROCEDURE_TYPE_REGISTRATION                                   *
 * Service code:   CN_SS_TYPE_ENHANCED_MULTILEVEL_PRECEDENCE_AND_PREEMPTION (75)       *
 * Supplementary information A: "7887287832873827836263652563656E235625"               *
 * Supplementary information B: N/A                                                    *
 * Supplementary information C: N/A                                                    *
 *                                                                                     *
 ***************************************************************************************/
tc_result_t ss_decoder_positive_10() 
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_ss_string_compounds_t* command_p = NULL;
    char* ss_string_p = "*75*7887287832873827836263652563656E235625#";

    command_p = decode_ss_string(ss_string_p);
    TC_ASSERT(CN_SS_PROCEDURE_TYPE_REGISTRATION == command_p->procedure_type);
    TC_ASSERT(CN_SS_TYPE_ENHANCED_MULTILEVEL_PRECEDENCE_AND_PREEMPTION == command_p->ss_type);
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_a,"7887287832873827836263652563656E235625"));
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_b,""));
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_c,""));

exit:
    if (command_p) { free(command_p); }
    return tc_result;
}

/***************************************************************************************
 * TESTCASE: ss_decoder_positive_11                                                    *
 ***************************************************************************************
 *                                                                                     *
 * INPUT:                                                                              *
 * ======                                                                              *
 * SS pattern: "*SC**SIB*SIC#"                                                         *
 * SS string:  "*75**B*C#"                                                             *
 *                                                                                     *
 * EXPECTED OUTPUT:                                                                    *
 * ================                                                                    *
 * Procedure type: CN_SS_PROCEDURE_TYPE_ACTIVATION                                     *
 * Service code:   CN_SS_TYPE_ENHANCED_MULTILEVEL_PRECEDENCE_AND_PREEMPTION (75)       *
 * Supplementary information A: N/A                                                    *
 * Supplementary information B: "B"                                                    *
 * Supplementary information C: "C"                                                    *
 *                                                                                     *
 ***************************************************************************************/
tc_result_t ss_decoder_positive_11() 
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_ss_string_compounds_t* command_p = NULL;
    char* ss_string_p = "*75**B*C#";

    command_p = decode_ss_string(ss_string_p);
    TC_ASSERT(CN_SS_PROCEDURE_TYPE_ACTIVATION == command_p->procedure_type);
    TC_ASSERT(CN_SS_TYPE_ENHANCED_MULTILEVEL_PRECEDENCE_AND_PREEMPTION == command_p->ss_type);
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_a,""));
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_b,"B"));
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_c,"C"));


exit:
    if (command_p) { free(command_p); }
    return tc_result;
}

/***************************************************************************************
 * TESTCASE: ss_decoder_positive_12                                                    *
 ***************************************************************************************
 *                                                                                     *
 * INPUT:                                                                              *
 * ======                                                                              *
 * SS pattern: "*SC**SIB#"                                                             *
 * SS string:  "*21**abcde#"                                                           *
 *                                                                                     *
 * EXPECTED OUTPUT:                                                                    *
 * ================                                                                    *
 * Procedure type: CN_SS_PROCEDURE_TYPE_ACTIVATION                                     *
 * Service code:   CN_SS_TYPE_CALL_FORWARDING_UNCONDITIONAL (21)                       *
 * Supplementary information A: N/A                                                    *
 * Supplementary information B: "B"                                                    *
 * Supplementary information C: "C"                                                    *
 *                                                                                     *
 ***************************************************************************************/
tc_result_t ss_decoder_positive_12()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_ss_string_compounds_t* command_p = NULL;
    char* ss_string_p = "*21**abcde#";

    command_p = decode_ss_string(ss_string_p);
    TC_ASSERT(CN_SS_PROCEDURE_TYPE_ACTIVATION == command_p->procedure_type);
    TC_ASSERT(CN_SS_TYPE_CALL_FORWARDING_UNCONDITIONAL == command_p->ss_type);
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_a,""));
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_b,"abcde"));
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_c,""));

exit:
    if (command_p) { free(command_p); }
    return tc_result;
}

/***************************************************************************************
 * TESTCASE: ss_decoder_positive_13                                                    *
 ***************************************************************************************
 *                                                                                     *
 * INPUT:                                                                              *
 * ======                                                                              *
 * SS pattern: "*SC***SIC#"                                                            *
 * SS string:  "*33***iuijhsjhjyew32h4j#"                                              *
 *                                                                                     *
 * EXPECTED OUTPUT:                                                                    *
 * ================                                                                    *
 * Procedure type: CN_SS_PROCEDURE_TYPE_ACTIVATION                                     *
 * Service code:   CN_SS_TYPE_CALL_BARRING_BAOC (33)                                   *
 * Supplementary information A: N/A                                                    *
 * Supplementary information B: N/A                                                    *
 * Supplementary information C: "iuijhsjhjyew32h4j"                                    *
 *                                                                                     *
 ***************************************************************************************/
tc_result_t ss_decoder_positive_13()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_ss_string_compounds_t* command_p = NULL;
    char* ss_string_p = "*33***iuijhsjhjyew32h4j#";

    command_p = decode_ss_string(ss_string_p);
    TC_ASSERT(CN_SS_PROCEDURE_TYPE_ACTIVATION == command_p->procedure_type);
    TC_ASSERT(CN_SS_TYPE_CALL_BARRING_BAOC == command_p->ss_type);
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_a,""));
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_b,""));
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_c,"iuijhsjhjyew32h4j"));

exit:
    if (command_p) { free(command_p); }
    return tc_result;
}

/***************************************************************************************
 * TESTCASE: ss_decoder_positive_14                                                    *
 ***************************************************************************************
 *                                                                                     *
 * INPUT:                                                                              *
 * ======                                                                              *
 * SS pattern: "*#SC#"                                                                 *
 * SS string:  "*#31#"                                                                 *
 *                                                                                     *
 * EXPECTED OUTPUT:                                                                    *
 * ================                                                                    *
 * Procedure type: CN_SS_PROCEDURE_TYPE_INTERROGATION                                  *
 * Service code:   CN_SS_TYPE_CALLING_LINE_IDENTIFICATION_RESTRICTION (31)             *
 * Supplementary information A: N/A                                                    *
 * Supplementary information B: N/A                                                    *
 * Supplementary information C: N/A                                                    *
 *                                                                                     *
 ***************************************************************************************/
tc_result_t ss_decoder_positive_14()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_ss_string_compounds_t* command_p = NULL;
    char* ss_string_p = "*#31#";

    command_p = decode_ss_string(ss_string_p);
    TC_ASSERT(CN_SS_PROCEDURE_TYPE_INTERROGATION == command_p->procedure_type);
    TC_ASSERT(CN_SS_TYPE_CALLING_LINE_IDENTIFICATION_RESTRICTION == command_p->ss_type);
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_a,""));
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_b,""));
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_c,""));

exit:
    if (command_p) { free(command_p); }
    return tc_result;
}

/***************************************************************************************
 * TESTCASE: ss_decoder_negative_1                                                     *
 ***************************************************************************************
 *                                                                                     *
 * INPUT:                                                                              *
 * ======                                                                              *
 * SS pattern: Incomplete SS string                                                    *
 * SS string:  "*33"                                                                   *
 *                                                                                     *
 * EXPECTED OUTPUT:                                                                    *
 * ================                                                                    *
 * Procedure type: CN_SS_PROCEDURE_TYPE_UNKNOWN                                        *
 * Service code:   CN_SS_TYPE_UNKNOWN                                                  *
 * Supplementary information A: N/A                                                    *
 * Supplementary information B: N/A                                                    *
 * Supplementary information C: N/A                                                    *
 *                                                                                     *
 ***************************************************************************************/
tc_result_t ss_decoder_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_ss_string_compounds_t* command_p = NULL;
    char* ss_string_p = "*33";

    command_p = decode_ss_string(ss_string_p);
    TC_ASSERT(CN_SS_PROCEDURE_TYPE_UNKNOWN == command_p->procedure_type);
    TC_ASSERT(CN_SS_TYPE_UNKNOWN == command_p->ss_type);
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_a,""));
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_b,""));
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_c,""));

exit:
    if (command_p) { free(command_p); }
    return tc_result;
}

/***************************************************************************************
 * TESTCASE: ss_decoder_negative_2                                                     *
 ***************************************************************************************
 *                                                                                     *
 * INPUT:                                                                              *
 * ======                                                                              *
 * SS pattern: Empty SS string                                                         *
 * SS string:  ""                                                                      *
 *                                                                                     *
 * EXPECTED OUTPUT:                                                                    *
 * ================                                                                    *
 * Procedure type: CN_SS_PROCEDURE_TYPE_UNKNOWN                                        *
 * Service code:   CN_SS_TYPE_UNKNOWN                                                  *
 * Supplementary information A: N/A                                                    *
 * Supplementary information B: N/A                                                    *
 * Supplementary information C: N/A                                                    *
 *                                                                                     *
 ***************************************************************************************/
tc_result_t ss_decoder_negative_2()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_ss_string_compounds_t* command_p = NULL;
    char* ss_string_p = "";

    command_p = decode_ss_string(ss_string_p);
    TC_ASSERT(CN_SS_PROCEDURE_TYPE_UNKNOWN == command_p->procedure_type);
    TC_ASSERT(CN_SS_TYPE_UNKNOWN == command_p->ss_type);
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_a,""));
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_b,""));
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_c,""));

exit:
    if (command_p) { free(command_p); }
    return tc_result;
}

/***************************************************************************************
 * TESTCASE: ss_decoder_negative_3                                                     *
 ***************************************************************************************
 *                                                                                     *
 * INPUT:                                                                              *
 * ======                                                                              *
 * SS pattern: Illegal SS string                                                       *
 * SS string:  "#g#**e#*d#*#"                                                          *
 *                                                                                     *
 * EXPECTED OUTPUT:                                                                    *
 * ================                                                                    *
 * Procedure type: CN_SS_PROCEDURE_TYPE_UNKNOWN                                        *
 * Service code:   CN_SS_TYPE_UNKNOWN                                                  *
 * Supplementary information A: N/A                                                    *
 * Supplementary information B: N/A                                                    *
 * Supplementary information C: N/A                                                    *
 *                                                                                     *
 ***************************************************************************************/
tc_result_t ss_decoder_negative_3()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_ss_string_compounds_t* command_p = NULL;
    char* ss_string_p = "#g#**e#*d#*#";

    command_p = decode_ss_string(ss_string_p);
    TC_ASSERT(CN_SS_PROCEDURE_TYPE_UNKNOWN == command_p->procedure_type);
    TC_ASSERT(CN_SS_TYPE_UNKNOWN == command_p->ss_type);
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_a,""));
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_b,""));
    TC_ASSERT(0 == strcmp(command_p->supplementary_info_c,""));

exit:
    if (command_p) { free(command_p); }
    return tc_result;
}

/***************************************************************************************
 * TESTCASE: ss_decoder_negative_4                                                     *
 ***************************************************************************************
 *                                                                                     *
 * INPUT:                                                                              *
 * ======                                                                              *
 * SS pattern: "*SC*SI#" with SC: Unknown service code (999)                           *
 * SS string:  "*SC*SI#"                                                               *
 *                                                                                     *
 * EXPECTED OUTPUT:                                                                    *
 * ================                                                                    *
 * Service code:   CN_SS_TYPE_UNKNOWN                                                  *
 *                                                                                     *
 ***************************************************************************************/
tc_result_t ss_decoder_negative_4()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_ss_string_compounds_t* command_p = NULL;
    char* ss_string_p = "*999*#";

    command_p = decode_ss_string(ss_string_p);
    TC_ASSERT(CN_SS_TYPE_UNKNOWN == command_p->ss_type);

exit:
    if (command_p) { free(command_p); }
    return tc_result;
}

/***************************************************************************************
 * TESTCASE: ss_decoder_negative_5                                                     *
 ***************************************************************************************
 *                                                                                     *
 * INPUT:                                                                              *
 * ======                                                                              *
 * decode_ss_string() with NULL parameter                                              *
 *                                                                                     *
 * EXPECTED OUTPUT:                                                                    *
 * ================                                                                    *
 * decode_ss_string returns NULL.
 *                                                                                     *
 ***************************************************************************************/
tc_result_t ss_decoder_negative_5()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_ss_string_compounds_t* command_p = NULL;

    command_p = decode_ss_string(NULL);
    TC_ASSERT(NULL == command_p);

exit:
    if (command_p) { free(command_p); }
    return tc_result;
}


/*
 * SS ENCODER TESTCASES
 */

/***************************************************************************************
 * TESTCASE: ss_encoder_positive_1                                                     *
 ***************************************************************************************
 *                                                                                     *
 * INPUT:                                                                              *
 * ======                                                                              *
 * Procedure type: CN_SS_PROCEDURE_TYPE_ACTIVATION                                     *
 * Service code:   CN_SS_TYPE_CALL_FORWARDING_UNCONDITIONAL (21)                       *
 * Supplementary information A: N/A                                                    *
 * Supplementary information B: N/A                                                    *
 * Supplementary information C: N/A                                                    *
 *                                                                                     *
 * EXPECTED OUTPUT:                                                                    *
 * ================                                                                    *
 * SS pattern: "*SC#"                                                                 *
 * SS string:  "*21#"                                                                 *
 *                                                                                     *
 ***************************************************************************************/
tc_result_t ss_encoder_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    char* ss_string_p = NULL;
    cn_ss_string_compounds_t supplementary_service;

    memset(&supplementary_service, 0, sizeof(supplementary_service));
    supplementary_service.procedure_type = CN_SS_PROCEDURE_TYPE_ACTIVATION;
    supplementary_service.ss_type = CN_SS_TYPE_CALL_FORWARDING_UNCONDITIONAL;

    ss_string_p = encode_ss_string(&supplementary_service);
    TC_ASSERT(0 == memcmp(ss_string_p, "*21#", strlen("*21#")+1));

exit:
    if (ss_string_p) { free(ss_string_p); }
    return tc_result;
}

/***************************************************************************************
 * TESTCASE: ss_encoder_positive_2                                                     *
 ***************************************************************************************
 *                                                                                     *
 * INPUT:                                                                              *
 * ======                                                                              *
 * Procedure type: CN_SS_PROCEDURE_TYPE_DEACTIVATION                                   *
 * Service code:   CN_SS_TYPE_CALL_FORWARDING_UNCONDITIONAL (21)                       *
 * Supplementary information A: "+4425993004126"                                       *
 * Supplementary information B: N/A                                                    *
 * Supplementary information C: N/A                                                    *
 *                                                                                     *
 * EXPECTED OUTPUT:                                                                    *
 * ================                                                                    *
 * SS pattern: "#SC*SI#"                                                               *
 * SS string:  "#21*+4425993004126#"                                                   *
 *                                                                                     *
 ***************************************************************************************/
tc_result_t ss_encoder_positive_2()
{
    tc_result_t tc_result = TC_RESULT_OK;
    char* ss_string_p = NULL;
    cn_ss_string_compounds_t supplementary_service;

    memset(&supplementary_service, 0, sizeof(supplementary_service));
    supplementary_service.procedure_type = CN_SS_PROCEDURE_TYPE_DEACTIVATION;
    supplementary_service.ss_type = CN_SS_TYPE_CALL_FORWARDING_UNCONDITIONAL;
    memmove(supplementary_service.supplementary_info_a, "+4425993004126", strlen("+4425993004126")+1);

    ss_string_p = encode_ss_string(&supplementary_service);
    TC_ASSERT(0 == memcmp(ss_string_p, "#21*+4425993004126#", strlen("#21*+4425993004126#")+1));

exit:
    if (ss_string_p) { free(ss_string_p); }
    return tc_result;
}

/***************************************************************************************
 * TESTCASE: ss_encoder_positive_3                                                     *
 ***************************************************************************************
 *                                                                                     *
 * INPUT:                                                                              *
 * ======                                                                              *
 * Procedure type: CN_SS_PROCEDURE_TYPE_INTERROGATION                                  *
 * Service code:   CN_SS_TYPE_CALL_FORWARDING_UNCONDITIONAL (21)                       *
 * Supplementary information A: "+4425993004126"                                       *
 * Supplementary information B: N/A                                                    *
 * Supplementary information C: N/A                                                    *
 *                                                                                     *
 * EXPECTED OUTPUT:                                                                    *
 * ================                                                                    *
 * SS pattern: "*#SC*SI#"                                                              *
 * SS string:  "*#21*+4425993004126#"                                                  *
 *                                                                                     *
 ***************************************************************************************/
tc_result_t ss_encoder_positive_3()
{
    tc_result_t tc_result = TC_RESULT_OK;
    char* ss_string_p = NULL;
    cn_ss_string_compounds_t supplementary_service;

    memset(&supplementary_service, 0, sizeof(supplementary_service));
    supplementary_service.procedure_type = CN_SS_PROCEDURE_TYPE_INTERROGATION;
    supplementary_service.ss_type = CN_SS_TYPE_CALL_FORWARDING_UNCONDITIONAL;
    memmove(supplementary_service.supplementary_info_a, "+4425993004126", strlen("+4425993004126")+1);

    ss_string_p = encode_ss_string(&supplementary_service);
    TC_ASSERT(0 == memcmp(ss_string_p, "*#21*+4425993004126#", strlen("*#21*+4425993004126#")+1));

exit:
    if (ss_string_p) { free(ss_string_p); }
    return tc_result;
}

/***************************************************************************************
 * TESTCASE: ss_encoder_positive_4                                                     *
 ***************************************************************************************
 *                                                                                     *
 * INPUT:                                                                              *
 * ======                                                                              *
 * Procedure type: CN_SS_PROCEDURE_TYPE_REGISTRATION                                   *
 * Service code:   CN_SS_TYPE_CALL_FORWARDING_UNCONDITIONAL (21)                       *
 * Supplementary information A: "+4425993004126"                                       *
 * Supplementary information B: N/A                                                    *
 * Supplementary information C: N/A                                                    *
 *                                                                                     *
 * EXPECTED OUTPUT:                                                                    *
 * ================                                                                    *
 * SS pattern: "**SC*SI#" or "*SC*SI#"                                                 *
 * SS string:  "**21*+4425993004126#" or "*21*+4425993004126#"                         *
 *                                                                                     *
 ***************************************************************************************/
tc_result_t ss_encoder_positive_4()
{
    tc_result_t tc_result = TC_RESULT_OK;
    char* ss_string_p = NULL;
    cn_ss_string_compounds_t supplementary_service;

    memset(&supplementary_service, 0, sizeof(supplementary_service));
    supplementary_service.procedure_type = CN_SS_PROCEDURE_TYPE_REGISTRATION;
    supplementary_service.ss_type = CN_SS_TYPE_CALL_FORWARDING_UNCONDITIONAL;
    memmove(supplementary_service.supplementary_info_a, "+4425993004126", strlen("+4425993004126")+1);

    ss_string_p = encode_ss_string(&supplementary_service);
    TC_ASSERT(0 == memcmp(ss_string_p, "**21*+4425993004126#", strlen("**21*+4425993004126#")+1) || /* "**SC*SI#" */
              0 == memcmp(ss_string_p, "*21*+4425993004126#", strlen("*21*+4425993004126#")+1));    /* "*SC*SI#"  */

exit:
    if (ss_string_p) { free(ss_string_p); }
    return tc_result;
}

/***************************************************************************************
 * TESTCASE: ss_encoder_positive_5                                                     *
 ***************************************************************************************
 *                                                                                     *
 * INPUT:                                                                              *
 * ======                                                                              *
 * Procedure type: CN_SS_PROCEDURE_TYPE_ERASURE                                        *
 * Service code:   CN_SS_TYPE_CALL_FORWARDING_UNCONDITIONAL (21)                       *
 * Supplementary information A: "+4425993004126"                                       *
 * Supplementary information B: N/A                                                    *
 * Supplementary information C: N/A                                                    *
 *                                                                                     *
 * EXPECTED OUTPUT:                                                                    *
 * ================                                                                    *
 * SS pattern: "##SC*SI#"                                                              *
 * SS string:  "##21*+4425993004126#"                                                  *
 *                                                                                     *
 ***************************************************************************************/
tc_result_t ss_encoder_positive_5()
{
    tc_result_t tc_result = TC_RESULT_OK;
    char* ss_string_p = NULL;
    cn_ss_string_compounds_t supplementary_service;

    memset(&supplementary_service, 0, sizeof(supplementary_service));
    supplementary_service.procedure_type = CN_SS_PROCEDURE_TYPE_ERASURE;
    supplementary_service.ss_type = CN_SS_TYPE_CALL_FORWARDING_UNCONDITIONAL;
    memmove(supplementary_service.supplementary_info_a, "+4425993004126", strlen("+4425993004126")+1);

    ss_string_p = encode_ss_string(&supplementary_service);
    TC_ASSERT(0 == memcmp(ss_string_p, "##21*+4425993004126#", strlen("##21*+4425993004126#")+1));

exit:
    if (ss_string_p) { free(ss_string_p); }
    return tc_result;
}

/***************************************************************************************
 * TESTCASE: ss_encoder_positive_6                                                     *
 ***************************************************************************************
 *                                                                                     *
 * INPUT:                                                                              *
 * ======                                                                              *
 * Procedure type: CN_SS_PROCEDURE_TYPE_REGISTRATION                                   *
 * Service code:   CN_SS_TYPE_ENHANCED_MULTILEVEL_PRECEDENCE_AND_PREEMPTION3 (753)     *
 * Supplementary information A: "ABC"                                                  *
 * Supplementary information B: "DEF"                                                  *
 * Supplementary information C: "123"                                                  *
 *                                                                                     *
 * EXPECTED OUTPUT:                                                                    *
 * ================                                                                    *
 * SS pattern: "*SC*SIA*SIB*SIC#" or "**SC*SIA*SIB*SIC#"                               *
 * SS string:  "*753*ABC*DEF*123#" or "**753*ABC*DEF*123#"                             *
 *                                                                                     *
 ***************************************************************************************/
tc_result_t ss_encoder_positive_6()
{
    tc_result_t tc_result = TC_RESULT_OK;
    char* ss_string_p = NULL;
    cn_ss_string_compounds_t supplementary_service;

    memset(&supplementary_service, 0, sizeof(supplementary_service));
    supplementary_service.procedure_type = CN_SS_PROCEDURE_TYPE_REGISTRATION;
    supplementary_service.ss_type = CN_SS_TYPE_ENHANCED_MULTILEVEL_PRECEDENCE_AND_PREEMPTION3;
    memmove(supplementary_service.supplementary_info_a, "ABC", strlen("ABC")+1);
    memmove(supplementary_service.supplementary_info_b, "DEF", strlen("DEF")+1);
    memmove(supplementary_service.supplementary_info_c, "123", strlen("123")+1);

    ss_string_p = encode_ss_string(&supplementary_service);
    TC_ASSERT(0 == memcmp(ss_string_p, "*753*ABC*DEF*123#",  strlen("*753*ABC*DEF*123#")+1) || /* "*SC*SIA*SIB*SIC#"  */
              0 == memcmp(ss_string_p, "**753*ABC*DEF*123#", strlen("**753*ABC*DEF*123#")+1)); /* "**SC*SIA*SIB*SIC#" */

exit:
    if (ss_string_p) { free(ss_string_p); }
    return tc_result;
}

/***************************************************************************************
 * TESTCASE: ss_encoder_positive_7                                                    *
 ***************************************************************************************
 *                                                                                     *
 * INPUT:                                                                              *
 * ======                                                                              *
 * Procedure type: CN_SS_PROCEDURE_TYPE_REGISTRATION                                   *
 * Service code:   CN_SS_TYPE_ENHANCED_MULTILEVEL_PRECEDENCE_AND_PREEMPTION2 (752)     *
 * Supplementary information A: "AB"                                                   *
 * Supplementary information B: "C"                                                    *
 * Supplementary information C: N/A                                                    *
 *                                                                                     *
 * EXPECTED OUTPUT:                                                                    *
 * ================                                                                    *
 * SS pattern: "*SC*SIA*SIB#" or "**SC*SIA*SIB#"                                    *
 * SS string:  "*752*AB*C#" or "**752*AB*C#"                                           *
 *                                                                                     *
 ***************************************************************************************/
tc_result_t ss_encoder_positive_7()
{
    tc_result_t tc_result = TC_RESULT_OK;
    char* ss_string_p = NULL;
    cn_ss_string_compounds_t supplementary_service;

    memset(&supplementary_service, 0, sizeof(supplementary_service));
    supplementary_service.procedure_type = CN_SS_PROCEDURE_TYPE_REGISTRATION;
    supplementary_service.ss_type = CN_SS_TYPE_ENHANCED_MULTILEVEL_PRECEDENCE_AND_PREEMPTION2;
    memmove(supplementary_service.supplementary_info_a, "AB", strlen("AB")+1);
    memmove(supplementary_service.supplementary_info_b, "C", strlen("C")+1);

    ss_string_p = encode_ss_string(&supplementary_service);
    TC_ASSERT(0 == memcmp(ss_string_p, "*752*AB*C#",  strlen("*752*AB*C#")+1) || /* "*SC*SIA*SIB#"  */
              0 == memcmp(ss_string_p, "**752*AB*C#", strlen("**752*AB*C#")+1)); /* "**SC*SIA*SIB#" */

exit:
    if (ss_string_p) { free(ss_string_p); }
    return tc_result;
}


/***************************************************************************************
 * TESTCASE: ss_encoder_positive_8                                                     *
 ***************************************************************************************
 *                                                                                     *
 * INPUT:                                                                              *
 * ======                                                                              *
 * Procedure type: CN_SS_PROCEDURE_TYPE_REGISTRATION                                   *
 * Service code:   CN_SS_TYPE_ENHANCED_MULTILEVEL_PRECEDENCE_AND_PREEMPTION4 (754)     *
 * Supplementary information A: "E"                                                    *
 * Supplementary information B: N/A                                                    *
 * Supplementary information C: "3456423545345435345"                                  *
 *                                                                                     *
 * EXPECTED OUTPUT:                                                                    *
 * ================                                                                    *
 * SS pattern: "*SC*SIA**SIC#" or "**SC*SIA**SIC#"                                     *
 * SS string:  "*754*E**3456423545345435345#" or "**754*E**3456423545345435345#"       *
 *                                                                                     *
 ***************************************************************************************/
tc_result_t ss_encoder_positive_8()
{
    tc_result_t tc_result = TC_RESULT_OK;
    char* ss_string_p = NULL;
    cn_ss_string_compounds_t supplementary_service;

    memset(&supplementary_service, 0, sizeof(supplementary_service));
    supplementary_service.procedure_type = CN_SS_PROCEDURE_TYPE_REGISTRATION;
    supplementary_service.ss_type = CN_SS_TYPE_ENHANCED_MULTILEVEL_PRECEDENCE_AND_PREEMPTION4;
    memmove(supplementary_service.supplementary_info_a, "E", strlen("E")+1);
    memmove(supplementary_service.supplementary_info_c, "3456423545345435345", strlen("3456423545345435345")+1);

    ss_string_p = encode_ss_string(&supplementary_service);
    TC_ASSERT(0 == memcmp(ss_string_p, "*754*E**3456423545345435345#",  strlen("*754*E**3456423545345435345#")+1) || /* "*SC*SIA**SIC#"  */
              0 == memcmp(ss_string_p, "**754*E**3456423545345435345#", strlen("**754*E**3456423545345435345#")+1)); /* "**SC*SIA**SIC#" */

exit:
    if (ss_string_p) { free(ss_string_p); }
    return tc_result;
}

/***************************************************************************************************************
 * TESTCASE: ss_encoder_positive_9                                                                             *
 ***************************************************************************************************************
 *                                                                                                             *
 * INPUT:                                                                                                      *
 * ======                                                                                                      *
 * Procedure type: CN_SS_PROCEDURE_TYPE_REGISTRATION                                                           *
 * Service code:   CN_SS_TYPE_ENHANCED_MULTILEVEL_PRECEDENCE_AND_PREEMPTION (75)                               *
 * Supplementary information A: "7887287832873827836263652563656E235625"                                       *
 * Supplementary information B: N/A                                                                            *
 * Supplementary information C: N/A                                                                            *
 *                                                                                                             *
 * EXPECTED OUTPUT:                                                                                            *
 * ================                                                                                            *
 * SS pattern: "*SC*SIA#" or "**SC*SIA#"                                                                       *
 * SS string:  "*75*7887287832873827836263652563656E235625#" or "**75*7887287832873827836263652563656E235625#" *
 *                                                                                                             *
 ***************************************************************************************************************/
tc_result_t ss_encoder_positive_9()
{
    tc_result_t tc_result = TC_RESULT_OK;
    char* ss_string_p = NULL;
    cn_ss_string_compounds_t supplementary_service;

    memset(&supplementary_service, 0, sizeof(supplementary_service));
    supplementary_service.procedure_type = CN_SS_PROCEDURE_TYPE_REGISTRATION;
    supplementary_service.ss_type = CN_SS_TYPE_ENHANCED_MULTILEVEL_PRECEDENCE_AND_PREEMPTION;
    memmove(supplementary_service.supplementary_info_a, "7887287832873827836263652563656E235625", strlen("7887287832873827836263652563656E235625")+1);

    ss_string_p = encode_ss_string(&supplementary_service);
    TC_ASSERT(0 == memcmp(ss_string_p, "*75*7887287832873827836263652563656E235625#",  strlen("*75*7887287832873827836263652563656E235625#")+1) || /* "*SC*SIA#"  */
              0 == memcmp(ss_string_p, "**75*7887287832873827836263652563656E235625#", strlen("**75*7887287832873827836263652563656E235625#")+1)); /* "**SC*SIA#" */

exit:
    if (ss_string_p) { free(ss_string_p); }
    return tc_result;
}

/***************************************************************************************
 * TESTCASE: ss_encoder_positive_10                                                    *
 ***************************************************************************************
 *                                                                                     *
 * INPUT:                                                                              *
 * ======                                                                              *
 * Procedure type: CN_SS_PROCEDURE_TYPE_REGISTRATION                                   *
 * Service code:   CN_SS_TYPE_CALL_FORWARDING_NO_REPLY (61)                            *
 * Supplementary information A: N/A                                                    *
 * Supplementary information B: "10"                                                   *
 * Supplementary information C: "15"                                                   *
 *                                                                                     *
 * EXPECTED OUTPUT:                                                                    *
 * ================                                                                    *
 * SS pattern: "*SC**SIB*SIC#" or "**SC**SIB*SIC#"                                     *
 * SS string:  "*61**10*15#" or "**61**10*15#"                                         *
 *                                                                                     *
 ***************************************************************************************/
tc_result_t ss_encoder_positive_10()
{
    tc_result_t tc_result = TC_RESULT_OK;
    char* ss_string_p = NULL;
    cn_ss_string_compounds_t supplementary_service;

    memset(&supplementary_service, 0, sizeof(supplementary_service));
    supplementary_service.procedure_type = CN_SS_PROCEDURE_TYPE_REGISTRATION;
    supplementary_service.ss_type = CN_SS_TYPE_CALL_FORWARDING_NO_REPLY;
    memmove(supplementary_service.supplementary_info_b, "10", strlen("10")+1);
    memmove(supplementary_service.supplementary_info_c, "15", strlen("15")+1);

    ss_string_p = encode_ss_string(&supplementary_service);
    TC_ASSERT(0 == memcmp(ss_string_p, "*61**10*15#",  strlen("*61**10*15#")+1) || /* "*SC**SIB*SIC#"  */
              0 == memcmp(ss_string_p, "**61**10*15#", strlen("**61**10*15#")+1)); /* "**SC**SIB*SIC#" */

exit:
    if (ss_string_p) { free(ss_string_p); }
    return tc_result;
}

/***************************************************************************************
 * TESTCASE: ss_encoder_positive_11                                                    *
 ***************************************************************************************
 *                                                                                     *
 * INPUT:                                                                              *
 * ======                                                                              *
 * Procedure type: CN_SS_PROCEDURE_TYPE_REGISTRATION                                   *
 * Service code:   CN_SS_TYPE_CALL_FORWARDING_UNCONDITIONAL (21)                       *
 * Supplementary information A: N/A                                                    *
 * Supplementary information B: "abcde"                                                *
 * Supplementary information C: N/A                                                    *
 *                                                                                     *
 * EXPECTED OUTPUT:                                                                    *
 * ================                                                                    *
 * SS pattern: "*21**SIB#" or "**21**SIB#"                                             *
 * SS string:  "*21**abcde#" or "**21**abcde#"                                         *
 *                                                                                     *
 ***************************************************************************************/
tc_result_t ss_encoder_positive_11()
{
    tc_result_t tc_result = TC_RESULT_OK;
    char* ss_string_p = NULL;
    cn_ss_string_compounds_t supplementary_service;

    memset(&supplementary_service, 0, sizeof(supplementary_service));
    supplementary_service.procedure_type = CN_SS_PROCEDURE_TYPE_REGISTRATION;
    supplementary_service.ss_type = CN_SS_TYPE_CALL_FORWARDING_UNCONDITIONAL;
    memmove(supplementary_service.supplementary_info_b, "10", strlen("10")+1);

    ss_string_p = encode_ss_string(&supplementary_service);
    TC_ASSERT(0 == memcmp(ss_string_p, "*21**10#",  strlen("*21**10#")+1) || /* "*SC**SIB*SIC#"  */
              0 == memcmp(ss_string_p, "**21**10#", strlen("**21**10#")+1)); /* "**SC**SIB*SIC#" */

exit:
    if (ss_string_p) { free(ss_string_p); }
    return tc_result;
}

/***************************************************************************************
 * TESTCASE: ss_encoder_positive_12                                                    *
 ***************************************************************************************
 *                                                                                     *
 * INPUT:                                                                              *
 * ======                                                                              *
 * Procedure type: CN_SS_PROCEDURE_TYPE_REGISTRATION                                   *
 * Service code:   CN_SS_TYPE_CALL_BARRING_BAOC (33)                                   *
 * Supplementary information A: N/A                                                    *
 * Supplementary information B: N/A                                                    *
 * Supplementary information C: "iuijhsjhjyew32h4j"                                    *
 *                                                                                     *
 * EXPECTED OUTPUT:                                                                    *
 * ================                                                                    *
 * SS pattern: "*SC***SIC#" or "**SC***SIC#"                                           *
 * SS string:  "*33***iuijhsjhjyew32h4j#" or "**33***iuijhsjhjyew32h4j#"               *
 *                                                                                     *
 ***************************************************************************************/
tc_result_t ss_encoder_positive_12()
{
    tc_result_t tc_result = TC_RESULT_OK;
    char* ss_string_p = NULL;
    cn_ss_string_compounds_t supplementary_service;

    memset(&supplementary_service, 0, sizeof(supplementary_service));
    supplementary_service.procedure_type = CN_SS_PROCEDURE_TYPE_REGISTRATION;
    supplementary_service.ss_type = CN_SS_TYPE_CALL_BARRING_BAOC;
    memmove(supplementary_service.supplementary_info_c, "iuijhsjhjyew32h4j", strlen("iuijhsjhjyew32h4j")+1);

    ss_string_p = encode_ss_string(&supplementary_service);
    TC_ASSERT(0 == memcmp(ss_string_p, "*33***iuijhsjhjyew32h4j#",  strlen("*33***iuijhsjhjyew32h4j#")+1) || /* "*SC***SIC#"  */
              0 == memcmp(ss_string_p, "**33***iuijhsjhjyew32h4j#", strlen("**33***iuijhsjhjyew32h4j#")+1)); /* "**SC***SIC#" */

exit:
    if (ss_string_p) { free(ss_string_p); }
    return tc_result;
}

/***************************************************************************************
 * TESTCASE: ss_encoder_negative_1                                                     *
 ***************************************************************************************
 *                                                                                     *
 * INPUT:                                                                              *
 * ======                                                                              *
 * Procedure type: CN_SS_PROCEDURE_TYPE_UNKNOWN                                        *
 * Service code:   CN_SS_TYPE_CALL_BARRING_BAOC                                        *
 * Supplementary information A: N/A                                                    *
 * Supplementary information B: N/A                                                    *
 * Supplementary information C: N/A                                                    *
 *                                                                                     *
 * EXPECTED OUTPUT:                                                                    *
 * ================                                                                    *
 * encode_ss_string returns NULL.
 *                                                                                     *
 ***************************************************************************************/
tc_result_t ss_encoder_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    char* ss_string_p = NULL;
    cn_ss_string_compounds_t supplementary_service;

    memset(&supplementary_service, 0, sizeof(supplementary_service));
    supplementary_service.procedure_type = CN_SS_PROCEDURE_TYPE_UNKNOWN;
    supplementary_service.ss_type = CN_SS_TYPE_CALL_BARRING_BAOC;

    ss_string_p = encode_ss_string(&supplementary_service);
    TC_ASSERT(NULL == ss_string_p);

exit:
    if (ss_string_p) { free(ss_string_p); }
    return tc_result;
}

/***************************************************************************************
 * TESTCASE: ss_encoder_negative_2                                                     *
 ***************************************************************************************
 *                                                                                     *
 * INPUT:                                                                              *
 * ======                                                                              *
 * Procedure type: CN_SS_PROCEDURE_TYPE_REGISTRATION                                   *
 * Service code:   CN_SS_TYPE_UNKNOWN                                                  *
 * Supplementary information A: N/A                                                    *
 * Supplementary information B: N/A                                                    *
 * Supplementary information C: N/A                                                    *
 *                                                                                     *
 * EXPECTED OUTPUT:                                                                    *
 * ================                                                                    *
 * encode_ss_string returns NULL.
 *                                                                                     *
 ***************************************************************************************/
tc_result_t ss_encoder_negative_2()
{
    tc_result_t tc_result = TC_RESULT_OK;
    char* ss_string_p = NULL;
    cn_ss_string_compounds_t supplementary_service;

    memset(&supplementary_service, 0, sizeof(supplementary_service));
    supplementary_service.procedure_type = CN_SS_PROCEDURE_TYPE_REGISTRATION;
    supplementary_service.ss_type = CN_SS_TYPE_UNKNOWN;

    ss_string_p = encode_ss_string(&supplementary_service);
    TC_ASSERT(NULL == ss_string_p);

exit:
    if (ss_string_p) { free(ss_string_p); }
    return tc_result;
}

/***************************************************************************************
 * TESTCASE: ss_encoder_negative_2                                                     *
 ***************************************************************************************
 *                                                                                     *
 * INPUT:                                                                              *
 * ======                                                                              *
 * encode_ss_string() with NULL parameter                                              *
 *                                                                                     *
 * EXPECTED OUTPUT:                                                                    *
 * ================                                                                    *
 * encode_ss_string returns NULL.
 *                                                                                     *
 ***************************************************************************************/
tc_result_t ss_encoder_negative_3()
{
    tc_result_t tc_result = TC_RESULT_OK;
    char* ss_string_p = NULL;

    ss_string_p = encode_ss_string(NULL);
    TC_ASSERT(NULL == ss_string_p);

exit:
    if (ss_string_p) { free(ss_string_p); }
    return tc_result;
}

