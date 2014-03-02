/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "atc_string.h"
#include "exe_p.h"
#include "exe_last_fail_cause.h"

#include "whitebox.h"
#ifdef EXE_USE_ATC_CUSTOMER_EXTENSIONS
#include "wb_extended_whitebox.h"
#endif

#define ETZV_CHECK_STRING_1 "*ETZV: \"+02\""
#define ETZV_CHECK_STRING_2 "*ETZV: \"+02\", \"2010/05/20,10:45:00\", \"1970/01/24,05:02:46\""
#define ETZV_CHECK_STRING_3 "*ETZV: \"+02\", \"2010/05/20,10:45:00\", \"1970/01/24,05:02:46\", \"1\""
#define ETZV_CHECK_STRING_4 "*ETZV: \"+02\", \"2010/05/20,10:45:00\", \"1970/01/24,05:02:46\", \"0\""
#define ECAV_CHECK_STRING "*ECAV: 0,3,1,,"
#define CREG_CHECK_STRING "+CREG: 5,\"003D\",\"0085AB54\""
#define CGREG_CHECK_STRING "+CGREG: 5,\"003D\",\"0085AB54\""
#define CSSI_CHECK_STRING_1 "+CSSI: 2"
#define CSSI_CHECK_STRING_2 "+CSSI: 4,3"
#define CSSU_CHECK_STRING_1 "+CSSU: 2,,\"+4612345678\",145"
#define CSSU_CHECK_STRING_2 "+CSSU: 1,3,\"+4612345678\",145"
#define CSSU_CHECK_STRING_3 "+CSSU: 3,,\"12345678\",90"
#define ETTY_READ_STRING "*ETTY: 1"
#define ETTY_TEST_STRING "*ETTY: (0,1)"
#define CMGF_COMMAND "+CMGF?"
#define CLCC_TEST_STRING_1 "+CLCC: 1,0,0,0,0,\"+4646143000\",145,,,0"
#define CLCC_TEST_STRING_2 "+CLCC: 1,0,0,0,0,\"+4646143000\",145,,,0"
#define CLCC_TEST_STRING_3 "+CLCC: 2,1,4,0,0,\"046143099\",129,,,1"
#define CHLD_TEST_STRING_1 "+CHLD: (0,1,1x,2,2x,3,4)"
#define EGNCI_CHECK_STRING_1 "OK"
#define EGNCI_CHECK_STRING_2 "*EGNCI: \"43342\",\"4321\",\"0000ABCD\",,,10"
#define EGNCI_CHECK_STRING_3 "*EGNCI: \"40445\",\"0123\",\"00000987\",,,35"
#define EGNCI_CHECK_STRING_4 "*EGNCI: \"532125\",\"5665\",\"000000A5\",,,63"
#define EWNCI_CHECK_STRING_1 "OK"
#define EWNCI_CHECK_STRING_2 "*EWNCI: 10780,414,85,40,62"
#define EWNCI_CHECK_STRING_3 "*EWNCI: 10780,424,91,45,64"
#define EWNCI_CHECK_STRING_4 "*EWNCI: 10773,434,86,32,66"
#define EGSCI_CHECK_STRING_1 "OK"
#define EGSCI_CHECK_STRING_2 "*EGSCI: 106,31,30,120"
#define EGSCI_CHECK_STRING_3 "ERROR"
#define EWSCI_CHECK_STRING_1 "OK"
#define EWSCI_CHECK_STRING_2 "*EWSCI: 10780,411,91,33,66"
#define EWSCI_CHECK_STRING_3 "ERROR"


extern int state;

const char wb_test_string_ucs2[] = {0, '*', 0, '1', 0, '2', 0, '3', 0, '*', 0, '1', 0, '1', 0, '#'};
const char wb_test_string_ucs2_hex[] = "002A003100320033002A003100310023";
const char wb_test_string_utf8[] = "*123*11#";
const char wb_test_string_gsm8[] = " Ringt f|r: 5,62 kr";
const char wb_test_string_hex[] = "2A3132332A313123";
const char wb_test_string_alphabet[] = "abcdefghijklmnopqrstuvxyzABCDEFGHIJKLMNOPQRSTUVXYZ1234567890 !$@#%&/()={}*-_[].,:;^\"<>|+~?\\";

const char wb_test_gsm8_pad[] = "ABC\rDEF";

const char wb_test_string_gsm7[] = {0xAA, 0x98, 0x6C, 0xA6, 0x8A, 0xC5, 0x46, 0};

void wb_testfcn_string_conversion(char *at_string_p)
{
    uint8_t converted_string[500];
    uint8_t *test_string_p = NULL;
    uint8_t test_string[500];
    size_t dest_length = 0;
    size_t num_chars = 0;
    int i;

    /* alphabet utf8 -> gsm7 -> hex -> gsm7 -> utf8 */
    memset(&converted_string, 0, sizeof(converted_string));
    memset(&test_string, 0, sizeof(test_string));

    num_chars = atc_string_convert(ATC_CHARSET_UTF8, wb_test_string_alphabet, strlen(wb_test_string_alphabet),
                                   ATC_CHARSET_GSM7, (uint8_t *)&converted_string, &dest_length);

    memmove(&test_string, &converted_string, dest_length);
    memset(&converted_string, 0, sizeof(converted_string));

    num_chars = atc_string_convert(ATC_CHARSET_GSM7, (uint8_t *)&test_string, dest_length,
                                   ATC_CHARSET_HEX, (uint8_t *)&converted_string, &dest_length);

    memmove(&test_string, &converted_string, dest_length);
    memset(&converted_string, 0, sizeof(converted_string));

    num_chars = atc_string_convert(ATC_CHARSET_HEX, (uint8_t *)&test_string, dest_length,
                                   ATC_CHARSET_GSM7, (uint8_t *)&converted_string, &dest_length);

    memmove(&test_string, &converted_string, dest_length);
    memset(&converted_string, 0, sizeof(converted_string));

    num_chars = atc_string_convert(ATC_CHARSET_GSM7, (uint8_t *)&test_string, dest_length,
                                   ATC_CHARSET_UTF8, (uint8_t *)&converted_string, &dest_length);

    WB_CHECK(check_for_string((char *)&converted_string, wb_test_string_alphabet));

    /* utf8 -> gsm7 -> utf8 */
    memset(&converted_string, 0, sizeof(converted_string));
    memset(&test_string, 0, sizeof(test_string));

    num_chars = atc_string_convert(ATC_CHARSET_UTF8, wb_test_string_utf8, strlen(wb_test_string_utf8),
                                   ATC_CHARSET_GSM7, (uint8_t *)&converted_string, &dest_length);

    memmove(&test_string, &converted_string, dest_length);
    WB_CHECK(check_for_string((char *)&converted_string, (char *)&wb_test_string_gsm7));

    memset(&converted_string, 0, sizeof(converted_string));
    num_chars = atc_string_convert(ATC_CHARSET_GSM7, (uint8_t *)&test_string, strlen((char *)&test_string),
                                   ATC_CHARSET_UTF8, (uint8_t *)&converted_string, &dest_length);

    WB_CHECK(check_for_string((char *)&converted_string, wb_test_string_utf8));


    /* gsm7 -> hex -> gsm7 */
    memset(&converted_string, 0, sizeof(converted_string));
    memset(&test_string, 0, sizeof(test_string));

    atc_string_convert(ATC_CHARSET_GSM7, (uint8_t *)&wb_test_string_gsm7, strlen(wb_test_string_gsm7),
                       ATC_CHARSET_HEX, (uint8_t *)&converted_string, &dest_length);

    memmove(&test_string, &converted_string, dest_length);
    WB_CHECK(check_for_string((char *)&converted_string, wb_test_string_hex));

    memset(&converted_string, 0, sizeof(converted_string));
    atc_string_convert(ATC_CHARSET_HEX, (uint8_t *)&test_string, strlen((char *)&test_string),
                       ATC_CHARSET_GSM7, (uint8_t *)&converted_string, &dest_length);

    WB_CHECK(check_for_string((char *)&converted_string, (char *)&wb_test_string_gsm7));


    /* hex -> gsm7 -> utf8 */
    memset(&converted_string, 0, sizeof(converted_string));
    memset(&test_string, 0, sizeof(test_string));

    atc_string_convert(ATC_CHARSET_HEX, wb_test_string_hex, strlen(wb_test_string_hex),
                       ATC_CHARSET_GSM7, (uint8_t *)&converted_string, &dest_length);

    memmove(&test_string, &converted_string, dest_length);
    WB_CHECK(check_for_string((char *)&converted_string, (char *)&wb_test_string_gsm7));

    memset(&converted_string, 0, sizeof(converted_string));
    atc_string_convert(ATC_CHARSET_GSM7, (uint8_t *)&test_string, strlen((char *)&test_string),
                       ATC_CHARSET_UTF8, (uint8_t *)&converted_string, &dest_length);

    WB_CHECK(check_for_string((char *)&converted_string, wb_test_string_utf8));


    /* utf8 -> gsm7 -> hex */
    memset(&converted_string, 0, sizeof(converted_string));
    memset(&test_string, 0, sizeof(test_string));

    atc_string_convert(ATC_CHARSET_UTF8, wb_test_string_utf8, strlen(wb_test_string_utf8),
                       ATC_CHARSET_GSM7, (uint8_t *)&converted_string, &dest_length);

    memmove(&test_string, &converted_string, dest_length);
    WB_CHECK(check_for_string((char *)&converted_string, (char *)&wb_test_string_gsm7));

    memset(&converted_string, 0, sizeof(converted_string));
    atc_string_convert(ATC_CHARSET_GSM7, (uint8_t *)&test_string, strlen((char *)&test_string),
                       ATC_CHARSET_HEX, (uint8_t *)&converted_string, &dest_length);

    WB_CHECK(check_for_string((char *)&converted_string, wb_test_string_hex));


    /* gsm8 -> gsm7 -> gsm8 */
    /* This case specifically tests CR padding, and CR pad removal */
    memset(&converted_string, 0, sizeof(converted_string));
    memset(&test_string, 0, sizeof(test_string));

    num_chars = atc_string_convert(ATC_CHARSET_GSM8, wb_test_gsm8_pad, (strlen(wb_test_gsm8_pad) / 7) * 7,
                                   ATC_CHARSET_GSM7, (uint8_t *)&converted_string, &dest_length);

    memmove(&test_string, &converted_string, dest_length);

    /* String should have a CR padded to the end of it, increasing the string length by 1 character */
    WB_CHECK(num_chars == 1 + ((strlen(wb_test_gsm8_pad) / 7) * 7));
    WB_CHECK(dest_length == (num_chars * 7) / 8);

    memset(&converted_string, 0, sizeof(converted_string));
    num_chars = atc_string_convert(ATC_CHARSET_GSM7, (uint8_t *)&test_string, dest_length,
                                   ATC_CHARSET_GSM8, (uint8_t *)&converted_string, &dest_length);

    WB_CHECK(check_for_string((char *)&converted_string, wb_test_gsm8_pad));
    /* String should have a CR stripped from the end of it, decreasing the string length by 1 character */
    WB_CHECK(num_chars == (strlen(wb_test_gsm8_pad) / 7) * 7);
    WB_CHECK(dest_length == num_chars);


    /* hex -> utf8 -> hex*/
    memset(&converted_string, 0, sizeof(converted_string));
    memset(&test_string, 0, sizeof(test_string));

    atc_string_convert(ATC_CHARSET_HEX, wb_test_string_ucs2_hex, strlen(wb_test_string_ucs2_hex),
                       ATC_CHARSET_UTF8, (uint8_t *)&converted_string, &dest_length);

    memmove(&test_string, &converted_string, dest_length);
    WB_CHECK(check_for_string((char *)&converted_string, (char *)&wb_test_string_utf8));

    memset(&converted_string, 0, sizeof(converted_string));
    atc_string_convert(ATC_CHARSET_UTF8, (uint8_t *)&test_string, strlen((char *)&test_string),
                       ATC_CHARSET_HEX, (uint8_t *)&converted_string, &dest_length);

    WB_CHECK(check_for_string((char *)&converted_string, wb_test_string_ucs2_hex));


    /* ucs2 -> utf8 */
    memset(&converted_string, 0, sizeof(converted_string));
    atc_string_convert(ATC_CHARSET_UCS2, (uint8_t *)&wb_test_string_ucs2, sizeof(wb_test_string_ucs2),
                       ATC_CHARSET_UTF8, (uint8_t *)&converted_string, &dest_length);

    WB_CHECK(check_for_string((char *)&converted_string, wb_test_string_utf8));


    /* ucs2 -> hex -> ucs2 */
    memset(&converted_string, 0, sizeof(converted_string));
    atc_string_convert(ATC_CHARSET_UCS2, (uint8_t *)&wb_test_string_ucs2, sizeof(wb_test_string_ucs2),
                       ATC_CHARSET_HEX, (uint8_t *)&converted_string, &dest_length);

    memmove(&test_string, &converted_string, dest_length);
    WB_CHECK(check_for_string((char *)&converted_string, wb_test_string_ucs2_hex));

    memset(&converted_string, 0, sizeof(converted_string));
    atc_string_convert(ATC_CHARSET_HEX, (uint8_t *)&test_string, strlen((char *)&test_string),
                       ATC_CHARSET_UCS2, (uint8_t *)&converted_string, &dest_length);

    WB_CHECK(check_for_string((char *)&converted_string, (char *)&wb_test_string_ucs2));
}

/* Test functions for ME CONTROL */
void wb_testfcn_cfun_99(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p); /* AT+CFUN=99 */

    for (state = 0; state < 2 && PARSER_p->IsPending; state++) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cfun_100(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p); /* AT+CFUN=99 */

    EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_cfun_5(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p); /* AT+CFUN=5 */

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_cfun_6(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p); /* AT+CFUN=6 */

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_cfun_5_6(char *at_string)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string, strlen(at_string), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_cfun_failure(char *at_string_p)
{
    /* Make sure CME ERROR codes are enabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    for (state = 0; state < 2 && PARSER_p->IsPending; state++) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 4"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}


void wb_testfcn_cfun_rf_off(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CFUN: 4"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_cfun_rf_transition(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_cfun_rf_preferred_wcdma(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    for (state = 0; state < 2 && PARSER_p->IsPending; state++) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CFUN: 7"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_cfun_rf_gsm_only(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    for (state = 0; state < 2 && PARSER_p->IsPending; state++) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CFUN: 5"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_cfun_rf_wcdma_only(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    for (state = 0; state < 2 && PARSER_p->IsPending; state++) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CFUN: 6"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_cfun_rf_gsm_wcdma_auto(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    for (state = 0; state < 2 && PARSER_p->IsPending; state++) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_cfun_rf_cdma_evdo_auto(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    for (state = 0; state < 2 && PARSER_p->IsPending; state++) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_cfun_rf_cdma_only(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    for (state = 0; state < 2 && PARSER_p->IsPending; state++) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_cfun_rf_evdo_only(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    for (state = 0; state < 2 && PARSER_p->IsPending; state++) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_cfun_rf_all_auto(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    for (state = 0; state < 2 && PARSER_p->IsPending; state++) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_cfun_read_failure(char *at_string_p)
{
    for (state = 0; state < 2; state++) {
        RESPONSE_BUFFER_LENGTH = 0;
        Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

        if (PARSER_p->IsPending) {
            EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
        }

        WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
        WB_CHECK(!leftovers(RESPONSE_BUFFER));
    }
}

void wb_testfcn_gcap_do(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    size_t at_string_length = strlen(at_string_p);
    Parser_AT(at_string_p, at_string_length, AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+GCAP: +CGSM"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_gcap_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    size_t at_string_length = strlen(at_string_p);
    Parser_AT(at_string_p, at_string_length, AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cgsn_do(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "012345678987654"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_cgsn_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_csq(char *at_string_p)
{
    /* Make sure CME ERROR codes are enabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    switch (CURRENT_TEST_CASE) {
    case wb_testcase_csq_do: {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CSQ: 0,99"));
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
        break;
    }
    case wb_testcase_csq_do_fail_mal: {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 3"));
        break;
    }
    case wb_testcase_csq_test: {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CSQ: (0-31,99),(0-7,99)"));
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
        break;
    }
    }

    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}


void wb_testfcn_etzr(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_etzv(char *at_string_p)
{
#ifdef EXE_USE_ATC_CUSTOMER_EXTENSIONS
    unsigned int i;

    wb_extended_disable_parser_flags();
#endif

    RESPONSE_BUFFER_LENGTH = 0;
    wb_testfcn_etzr(at_string_p);
    EXECUTOR.cn_selector_callback_event(FD_CN_EVENT, NULL); /* CN_EVENT_TIME_INFO */

    if (CURRENT_TEST_CASE == wb_testcase_etzv_1) {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, ETZV_CHECK_STRING_1));
    } else if (CURRENT_TEST_CASE == wb_testcase_etzv_2) {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, ETZV_CHECK_STRING_2));
    } else if (CURRENT_TEST_CASE == wb_testcase_etzv_3) {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, ETZV_CHECK_STRING_3));
    } else if (CURRENT_TEST_CASE == wb_testcase_etzv_4) {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, ETZV_CHECK_STRING_1));
    } else if (CURRENT_TEST_CASE == wb_testcase_etzv_5) {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, ETZV_CHECK_STRING_2));
    } else if (CURRENT_TEST_CASE == wb_testcase_etzv_6) {
        /* No event expected, leftovers() below will cover the case */
    } else if (CURRENT_TEST_CASE == wb_testcase_etzv_7) {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, ETZV_CHECK_STRING_4));
    }

    WB_CHECK(!leftovers(RESPONSE_BUFFER));

#ifdef EXE_USE_ATC_CUSTOMER_EXTENSIONS

    for (i = 0; i < MAX_NO_OF_OPEN_CHANNELS; i++) {
        Parser_ResetCommandSettings(ParserStateTable_GetParserState_ByEntry(i));
    }

#endif
}


void wb_testfcn_ecav(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    EXECUTOR.cn_selector_callback_event(FD_CN_EVENT, NULL); /* CN_EVENT_CALL_STATE_CHANGED */
    WB_CHECK(check_for_string(RESPONSE_BUFFER, ECAV_CHECK_STRING));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

}


void wb_testfcn_cssi(char *at_string_p)
{
    char response[20];

    state = 0;
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL); /* CN_RESPONSE_SET_CSSN */
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    state = 1;
    RESPONSE_BUFFER_LENGTH = 0;

    switch (CURRENT_TEST_CASE) {
    case wb_testcase_cssi_1:
        strcpy(response, CSSI_CHECK_STRING_1);
        break;

    case wb_testcase_cssi_2:
        strcpy(response, CSSI_CHECK_STRING_2);
        break;
    }

    EXECUTOR.cn_selector_callback_event(FD_CN_EVENT, NULL); /* CN_EVENT_CALL_SUPP_SVC_NOTIFICATION */
    WB_CHECK(check_for_string(RESPONSE_BUFFER, response));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


static bool wb_testfcn_cusd_hex_charset(char **at_string_pp);
static void wb_testfcn_cscstest_cusd_send_ussd_helper(char *at_string_p, bool expecting_good_conversion);

static bool wb_testfcn_cusd_hex_charset(char **at_string_pp)
{
    bool return_value = false;
    state = 0;

    return return_value;
}

const char *cusd_ussd_string_hex_p = "757373645F737472";  /* Input data pattern encoded in hexadecimal notation. */
const char *cusd_ussd_string_ascii_p = "ussd_str";  /* Input data pattern encoded in ASCII notation. */
const char gsm7string[] = {0xF5, 0xF9, 0x9C, 0xFC, 0x9D, 0xD3, 0xE5, 0};  /* Input data pattern encoded in GSM 7 notation. */

void wb_testfcn_cscstest_cusd_send_ussd_failure(char *at_string_p)
{
    wb_testfcn_cscstest_cusd_send_ussd_helper(at_string_p, false);
}


void wb_testfcn_cscstest_cusd_send_ussd(char *at_string_p)
{
    wb_testfcn_cscstest_cusd_send_ussd_helper(at_string_p, true);
}


static void wb_testfcn_cscstest_cusd_send_ussd_helper(char *at_string_p, bool expecting_good_conversion)
{
    int repetition_count = 0;
    const char *cusd_ussd_ptr = gsm7string; /* Use GSM 7 bit default alphapet until UCS2 is added */
    char *cusd_textbuffer_ptr;
    char *fcnname;

    if (ATC_CHARSET_HEX == PARSER_p->cscs_charset) {
        cusd_ussd_ptr = cusd_ussd_string_hex_p;
    }

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    /* Check the ussd_str that was left by the EXE */
    cusd_textbuffer_ptr = EXECUTOR.wb_test_ussd_str;

    int i = 0;

    switch (CURRENT_TEST_CASE) {
    case wb_testcase_cusd_hex:
        WB_CHECK(check_for_string(cusd_textbuffer_ptr, gsm7string));
        break;
    case wb_testcase_cusd_hex_fail:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
        break;
    default:
        WB_CHECK(check_for_string(cusd_textbuffer_ptr, cusd_ussd_ptr));
        break;
    }

    if (expecting_good_conversion) {
        WB_CHECK(!leftovers(cusd_textbuffer_ptr));

    } else {
        WB_CHECK(0 == repetition_count);
    }
}


void wb_testfcn_cusd_send_ussd(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_cusd_send_ussd_failure(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    /* now check the response buffer */
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_cusd_cancel_ussd(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_cusd_get_settings(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CUSD: 0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_cusd_get_supported_n(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CUSD: (0-2)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_cusd_unsolicited(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    /* Enable +CUSD result code presentation */
    Parser_AT("+CUSD=1", strlen("+CUSD=1"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    EXECUTOR.cn_selector_callback_event(FD_CN_EVENT, NULL); /* CN_EVENT_USSD */

    switch (CURRENT_TEST_CASE) {
    case wb_testcase_cusd_unsolicited:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CUSD: 0,\" Ringt\",15"));
        break;
    case wb_testcase_cusd_unsolicited_gsm8:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CUSD: 0,\" Ringt för: 5,62 kr\",68"));
        break;
    case wb_testcase_cusd_unsolicited_ucs2:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CUSD: 0,\" Ri\\\"n\\\"g\",72"));
        break;
    }

    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cusd_unsolicited_hex(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    /* Enable +CUSD result code presentation */
    Parser_AT("+CUSD=1", strlen("+CUSD=1"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    /* Enable HEX */
    Parser_AT("+CSCS=\"HEX\"", strlen("+CSCS=\"HEX\""), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    EXECUTOR.cn_selector_callback_event(FD_CN_EVENT, NULL); /* CN_EVENT_USSD */
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CUSD: 0,\"2052696E67\",1"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    Parser_AT("+CSCS=\"UTF8\"", strlen("+CSCS=\"UTF8\""), AT_DATA_COMMAND, PARSER_p);

}

void wb_testfcn_ccfc_set_1(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_ccfc_set_2(char *at_string_p)
{
    /* Make sure CME ERROR codes are enabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    /* Test */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 50"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_ccfc_set_query(char *at_string_p)
{
    /* Test with three classes */
    state = 0;
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CCFC: 1,1,\"+01555123123\",145"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CCFC: 1,2,\"555234234\",129,,,15"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Test with no classes in MAL response */
    state++;
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CCFC: 0,1"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CCFC: 0,4"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CCFC: 0,16"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CCFC: 0,32"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CCFC: 0,64"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CCFC: 0,128"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_ccfc_set_register(char *at_string_p)
{
    /* Make sure CME ERROR codes are enabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    /* TEST MAL_SUCCESS*/
    state = 0;
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* TEST MAL_NOT_SUPPORTED*/
    state++;
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 4"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* TEST MAL_FAILED*/
    state++;
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 100"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}


void wb_testfcn_ccfc_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CCFC: (0-5)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_ccwa_set_1(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_ccwa_set_neg(char *at_string_p)
{
    /* Make sure CME ERROR codes are enabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 50"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}


void wb_testfcn_ccwa_set_query(char *at_string_p)
{
    /* Make sure CME ERROR codes are enabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    switch (CURRENT_TEST_CASE) {
    case wb_testcase_ccwa_set_query:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CCWA: 1,1"));
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CCWA: 1,4"));
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
        break;
    case wb_testcase_ccwa_query_fail1:
        /* Tests FDN blocked response */
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 213"));
        break;
    case wb_testcase_ccwa_query_fail2:
        /* Tests failure response */
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 100"));
        break;
    case wb_testcase_ccwa_query_fail3:
        /* Tests success response, but without an response data */
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 100"));
        break;
    }
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

void wb_testfcn_ccwa_read(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CCWA: 0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_ccwa_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CCWA: (0,1)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_ccwa_abort(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    /* Sending another (in this case not abortable) command to interrupt the running CCWA command */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(CMGF_COMMAND, strlen(CMGF_COMMAND), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK")); /* This OK is from aborting CCWA. */
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMGF: 0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


#define CCWA_CHECK_STRING "+CCWA: \"+4612345678\",145,1"
void wb_testfcn_ccwa_unsolicited(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    EXECUTOR.cn_selector_callback_event(FD_CN_EVENT, NULL); /* CN_EVENT_RING_WAITING */

    WB_CHECK(check_for_string(RESPONSE_BUFFER, CCWA_CHECK_STRING));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_cnap_set(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    switch (CURRENT_TEST_CASE) {
    case wb_testcase_cnap_set_0:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
        break;
    case wb_testcase_cnap_set_1:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
        break;
    case wb_testcase_cnap_set_2: /* Invalid value */
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
        break;
    }

    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_cnap_read(char *at_string_p)
{
    int cnap_read = 0;
    char cnap_resp[20];

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    switch (CURRENT_TEST_CASE) {
    case wb_testcase_cnap_read_0:
        sprintf(cnap_resp, "+CNAP: 0,%d", cnap_read);
        WB_CHECK(check_for_string(RESPONSE_BUFFER, cnap_resp));
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
        break;
    case wb_testcase_cnap_read_1:
        sprintf(cnap_resp, "+CNAP: 1,%d", cnap_read);
        WB_CHECK(check_for_string(RESPONSE_BUFFER, cnap_resp));
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
        break;
    }

    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cnap_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CNAP: (0-1)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cnap_unsolicited(char *at_string_p)
{
    int entry;
    AT_ParserState_s *parser_p;

    /* Turn off +CLIP */
    for (entry = 0; entry < MAX_NO_OF_OPEN_CHANNELS; entry++) {
        parser_p = ParserStateTable_GetParserState_ByEntry(entry);

        if (parser_p->CLIP) {
            parser_p->CLIP = 0;
        }

        if (parser_p->ECAM) {
            parser_p->ECAM = 0;
        }
    }

    RESPONSE_BUFFER_LENGTH = 0;
    EXECUTOR.cn_selector_callback_event(FD_CN_EVENT, NULL); /* CN_EVENT_CALL_CNAP */

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CNAP: \"No Name\",0"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_colr_do(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+COLR: 1"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_colr_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


#define COLP_READ           "+COLP?"
#define COLP_SET_NO_VAL     "+COLP="
#define COLP_SET_0          "+COLP=0"
#define COLP_SET_1          "+COLP=1"
void wb_testfcn_colp(char *at_string_p)
{
    /* SET DISABLE */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(COLP_SET_0, strlen(COLP_SET_0), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* READ  */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(COLP_READ, strlen(COLP_READ), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+COLP: 0,2"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* SET NO VAL */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(COLP_SET_NO_VAL, strlen(COLP_SET_NO_VAL), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* READ  */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(COLP_READ, strlen(COLP_READ), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+COLP: 0,2"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_colp_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+COLP: (0,1)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_cmod_set(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_cmod_set_neg(char *at_string_p)
{
    /* Make sure CME ERROR codes are enabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    if (wb_testcase_cmod_set_neg_1 == CURRENT_TEST_CASE) {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 4"));
    } else {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 3"));
    }

    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}


void wb_testfcn_cmod_read(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMOD: 0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_cmod_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMOD: (0)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_a_do(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    for (state = 0; state < 2 && PARSER_p->IsPending; state++) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_a_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_chld_1(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (wb_testcase_chld_0 == CURRENT_TEST_CASE) {
        state = -1;
    } else {
        state = 0;
    }

    for (; state < 3 && PARSER_p->IsPending; state++) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_chld_2(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (wb_testcase_chld_1 == CURRENT_TEST_CASE) {
        state = -1;
    } else {
        state = 0;
    }

    for (; state < 3 && PARSER_p->IsPending; state++) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_chld_failure(char *at_string_p)
{
    /* Make sure CME ERROR codes are enabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    state = 0;
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 3"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}


void wb_testfcn_chld_read(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_chld_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, CHLD_TEST_STRING_1));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_clcc_0(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_clcc_1(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, CLCC_TEST_STRING_1));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_clcc_2(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, CLCC_TEST_STRING_2));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CLCC_TEST_STRING_3));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_creg_ereg_cgreg_registration_presentation(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;

    PARSER_p->CGREG = NET_REG_IND_DISABLED;
    PARSER_p->CREG = NET_REG_IND_DISABLED;
    PARSER_p->EREG = NET_REG_IND_DISABLED;

    state = 0; /* SET state */

    /* Set network registration presentation preference */
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Read network registration presentation settings */
    RESPONSE_BUFFER_LENGTH = 0;
    state = 1; /* READ state */

    switch (CURRENT_TEST_CASE) {
    case wb_testcase_creg_reg_presentation_1:
    case wb_testcase_creg_reg_presentation_2:
    case wb_testcase_creg_reg_loc_presentation_1:
    case wb_testcase_creg_reg_loc_presentation_2:
    case wb_testcase_creg_reg_loc_presentation_3:
        at_string_p = "+CREG?";
        break;
    case wb_testcase_ereg_reg_presentation_1:
    case wb_testcase_ereg_reg_presentation_2:
    case wb_testcase_ereg_reg_ext_presentation_1:
    case wb_testcase_ereg_reg_ext_presentation_2:
    case wb_testcase_ereg_reg_ext_presentation_3:
        at_string_p = "*EREG?";
        break;
    case wb_testcase_cgreg_reg_presentation_1:
    case wb_testcase_cgreg_reg_presentation_2:
    case wb_testcase_cgreg_reg_loc_presentation_1:
    case wb_testcase_cgreg_reg_loc_presentation_2:
    case wb_testcase_cgreg_reg_loc_presentation_3:
        at_string_p = "+CGREG?";
        break;
    default:
        wb_test_check(0, __FILE__, __LINE__);
    }

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    switch (CURRENT_TEST_CASE) {
    case wb_testcase_creg_reg_presentation_1:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CREG: 1,1"));
        break;
    case wb_testcase_creg_reg_presentation_2:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CREG: 1,0"));
        break;
    case wb_testcase_creg_reg_loc_presentation_1:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CREG: 2,1,\"003D\",\"0085AB54\""));
        break;
    case wb_testcase_creg_reg_loc_presentation_2:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CREG: 2,5,\"003D\",\"0085AB54\""));
        break;
    case wb_testcase_creg_reg_loc_presentation_3:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CREG: 2,0"));
        break;
    case wb_testcase_ereg_reg_presentation_1:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "*EREG: 1,1"));
        break;
    case wb_testcase_ereg_reg_presentation_2:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "*EREG: 1,0"));
        break;
    case wb_testcase_ereg_reg_ext_presentation_1:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "*EREG: 2,1,\"003D\",\"0085AB54\",2"));
        break;
    case wb_testcase_ereg_reg_ext_presentation_2:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "*EREG: 2,5,\"003D\",\"0085AB54\",2"));
        break;
    case wb_testcase_ereg_reg_ext_presentation_3:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "*EREG: 2,0"));
        break;
    case wb_testcase_cgreg_reg_presentation_1:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGREG: 1,1"));
        break;
    case wb_testcase_cgreg_reg_presentation_2:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGREG: 1,0"));
        break;
    case wb_testcase_cgreg_reg_loc_presentation_1:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGREG: 2,1,\"003D\",\"0085AB54\",2"));
        break;
    case wb_testcase_cgreg_reg_loc_presentation_2:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGREG: 2,5,\"003D\",\"0085AB54\",2"));
        break;
    case wb_testcase_cgreg_reg_loc_presentation_3:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGREG: 2,3"));
        break;
    default:
        wb_test_check(0, __FILE__, __LINE__);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_creg_ereg_cgreg_is_registration_presentation_supported(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;

    PARSER_p->CGREG = NET_REG_IND_DISABLED;
    PARSER_p->CREG = NET_REG_IND_DISABLED;
    PARSER_p->EREG = NET_REG_IND_DISABLED;

    /* Is registration presentation supported? */
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    if (CURRENT_TEST_CASE == wb_testcase_creg_is_supported) {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CREG: (0-2)"));
    } else if (CURRENT_TEST_CASE == wb_testcase_ereg_is_supported) {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "*EREG: (0-2)"));
    } else {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGREG: (0-2)"));
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_creg_ereg_cgreg_unsolicited_registration_presentation(char *at_string_p)
{
    PARSER_p->CGREG = NET_REG_IND_DISABLED;
    PARSER_p->CREG = NET_REG_IND_DISABLED;
    PARSER_p->EREG = NET_REG_IND_DISABLED;

    /* Enable network presentation */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Verify unsolicited responses */
    RESPONSE_BUFFER_LENGTH = 0;

    EXECUTOR.cn_selector_callback_event(FD_CN_EVENT, NULL); /* CN_EVENT_MODEM_REGISTRATION_STATUS */

    switch (CURRENT_TEST_CASE) {
    case wb_testcase_creg_unsolicited_1:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CREG: 1"));
        break;
    case wb_testcase_creg_unsolicited_2:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CREG: 0"));
        break;
    case wb_testcase_creg_unsolicited_3:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CREG: 1,\"003D\",\"0085AB54\""));
        break;
    case wb_testcase_creg_unsolicited_4:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CREG: 5,\"003D\",\"0085AB54\""));
        break;
    case wb_testcase_creg_unsolicited_5:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CREG: 4"));
        break;
    case wb_testcase_ereg_unsolicited_1:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "*EREG: 1"));
        break;
    case wb_testcase_ereg_unsolicited_2:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "*EREG: 0"));
        break;
    case wb_testcase_ereg_unsolicited_3:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "*EREG: 1,\"003D\",\"0085AB54\",2"));
        break;
    case wb_testcase_ereg_unsolicited_4:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "*EREG: 5,\"003D\",\"0085AB54\",2"));
        break;
    case wb_testcase_ereg_unsolicited_5:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "*EREG: 4"));
        break;
    case wb_testcase_cgreg_unsolicited_1:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGREG: 1"));
        break;
    case wb_testcase_cgreg_unsolicited_2:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGREG: 0"));
        break;
    case wb_testcase_cgreg_unsolicited_3:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGREG: 1,\"003D\",\"0085AB54\",3"));
        break;
    case wb_testcase_cgreg_unsolicited_4:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGREG: 5,\"003D\",\"0085AB54\",3"));
        break;
    case wb_testcase_cgreg_unsolicited_5:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGREG: 0"));
        break;
    default:
        wb_test_check(0, __FILE__, __LINE__);
    }

    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_egnci(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    switch (CURRENT_TEST_CASE) {
    case wb_testcase_egnci_1:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, EGNCI_CHECK_STRING_1));
        break;

    case wb_testcase_egnci_2:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, EGNCI_CHECK_STRING_2));
        WB_CHECK(check_for_string(RESPONSE_BUFFER, EGNCI_CHECK_STRING_3));
        WB_CHECK(check_for_string(RESPONSE_BUFFER, EGNCI_CHECK_STRING_4));
        WB_CHECK(check_for_string(RESPONSE_BUFFER, EGNCI_CHECK_STRING_1));
        break;
    }

    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_egsci(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    switch (CURRENT_TEST_CASE) {
    case wb_testcase_egsci_1:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, EGSCI_CHECK_STRING_3));
        break;

    case wb_testcase_egsci_2:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, EGSCI_CHECK_STRING_2));
        WB_CHECK(check_for_string(RESPONSE_BUFFER, EGSCI_CHECK_STRING_1));
        break;
    }

    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_ewnci(char *at_string_p)
{
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    switch (CURRENT_TEST_CASE) {
    case wb_testcase_ewnci_1:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, EWNCI_CHECK_STRING_1));
        break;

    case wb_testcase_ewnci_2:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, EWNCI_CHECK_STRING_2));
        WB_CHECK(check_for_string(RESPONSE_BUFFER, EWNCI_CHECK_STRING_3));
        WB_CHECK(check_for_string(RESPONSE_BUFFER, EWNCI_CHECK_STRING_4));
        WB_CHECK(check_for_string(RESPONSE_BUFFER, EWNCI_CHECK_STRING_1));
        break;
    }

    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_ewsci(char *at_string_p)
{
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    switch (CURRENT_TEST_CASE) {
    case wb_testcase_ewsci_1:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, EWSCI_CHECK_STRING_3));
        break;

    case wb_testcase_ewsci_2:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, EWSCI_CHECK_STRING_2));
        WB_CHECK(check_for_string(RESPONSE_BUFFER, EWSCI_CHECK_STRING_1));
        break;
    }

    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_clip_failure(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_clip_0(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_clip_1(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_clip_read(char *at_string_p)
{
    int clip_read = 0;
    char clip_resp[20];

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    sprintf(clip_resp, "+CLIP: 1,%d", clip_read);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, clip_resp));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_clip_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CLIP: (0-1)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_clir_read(char *at_string_p)
{
    exe_clir_read_t clir_read;
    clir_read.n = CN_CLIR_SETTING_INVOCATION;
    clir_read.m = CN_CLIR_STATUS_UNKNOWN;
    char clir_resp[20];

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL); /* CN_RESPONSE_CLIR_STATUS */
    }

    sprintf(clir_resp, "+CLIR: %d,%d", clir_read.n, clir_read.m);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, clir_resp));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_clir_set(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL); /* CN_RESPONSE_SET_CLIR */
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


#define CSSN_SET_SUPPORTED "+CSSN=1,1"
#define CSSN_SET_NOT_SUPPORTED "+CSSN=1,2"
#define CSSN_READ "+CSSN?"
#define CSSN_TEST "+CSSN=?"
void wb_testfcn_cssn(char *at_string)
{
    char SET_CLEAN_UP[CLEAN_UP_BUFFER_SIZE];

    (void)at_string;

    /* Make sure CME ERROR codes are enabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    /* First read and save current value for further clean up */
    state = 0;
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(CSSN_READ, strlen(CSSN_READ), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    if (WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"))) {

        /* Change "+CSSN: default_value" to "+CSSN= default value"
         * so it can be used as set command when clean-up */
        char *pch_p = strstr(RESPONSE_BUFFER, "+CSSN: ");

        if (pch_p) {
            strncpy(SET_CLEAN_UP, pch_p, 10);
            SET_CLEAN_UP[10] = '\0';
            pch_p = strchr(SET_CLEAN_UP, ':');
            *pch_p = '=';
        }

        /* Positive Test Cases */
        state++;
        RESPONSE_BUFFER_LENGTH = 0;
        Parser_AT(CSSN_SET_SUPPORTED, strlen(CSSN_SET_SUPPORTED), AT_DATA_COMMAND, PARSER_p);

        if (PARSER_p->IsPending) {
            EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
        }

        if (WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"))) {
            WB_CHECK(!leftovers(RESPONSE_BUFFER));

            /* Negative Test Cases */
            state++;
            RESPONSE_BUFFER_LENGTH = 0;
            Parser_AT(CSSN_SET_NOT_SUPPORTED, strlen(CSSN_SET_NOT_SUPPORTED), AT_DATA_COMMAND, PARSER_p);

            if (PARSER_p->IsPending) {
                EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
            }

            WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 4"));
            WB_CHECK(!leftovers(RESPONSE_BUFFER));

            /* Now read previously set value */
            state++;
            RESPONSE_BUFFER_LENGTH = 0;
            Parser_AT(CSSN_READ, strlen(CSSN_READ), AT_DATA_COMMAND, PARSER_p);

            if (PARSER_p->IsPending) {
                EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
            }

            WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
            WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CSSN: 1,1"));
            WB_CHECK(!leftovers(RESPONSE_BUFFER));

            /* Test Mode */
            state++;
            RESPONSE_BUFFER_LENGTH = 0;
            Parser_AT(CSSN_TEST, strlen(CSSN_TEST), AT_DATA_COMMAND, PARSER_p);

            if (PARSER_p->IsPending) {
                EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
            }

            WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
            WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CSSN: (0,1),(0,1)"));
            WB_CHECK(!leftovers(RESPONSE_BUFFER));
        }
    }

    /* Clean Up */
    state++;
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(SET_CLEAN_UP, strlen(SET_CLEAN_UP), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}


void wb_testfcn_cpwd_call_barring(char *at_string_p)
{
    wb_testfcn_cmee_configure("+CMEE=2");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    wb_testfcn_cmee_configure("+CMEE=0");
}


void wb_testfcn_clck_call_barring_lock(char *at_string_p)
{
    wb_testfcn_cmee_configure("+CMEE=2");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    wb_testfcn_cmee_configure("+CMEE=0");
}


void wb_testfcn_clck_call_barring_unlock(char *at_string_p)
{
    wb_testfcn_cmee_configure("+CMEE=2");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    wb_testfcn_cmee_configure("+CMEE=0");
}


void wb_testfcn_clck_call_barring_query(char *at_string_p)
{
    /* Do the QUERY */
    wb_testfcn_cmee_configure("+CMEE=2");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CLCK: 1,1"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CLCK: 1,4"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    wb_testfcn_cmee_configure("+CMEE=0");
}


void wb_testfcn_cssu(char *at_string_p)
{
    char response[50];

    state = 0;
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL); /* CN_RESPONSE_SET_CSSN */
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    state = 1;
    RESPONSE_BUFFER_LENGTH = 0;

    switch (CURRENT_TEST_CASE) {
    case wb_testcase_cssu_1:
        strcpy(response, CSSU_CHECK_STRING_1);
        break;

    case wb_testcase_cssu_2:
        strcpy(response, CSSU_CHECK_STRING_2);
        break;

    case wb_testcase_cssu_3:
        strcpy(response, CSSU_CHECK_STRING_3);
        break;
    }

    EXECUTOR.cn_selector_callback_event(FD_CN_EVENT, NULL); /* CN_EVENT_CALL_SUPP_SVC_NOTIFICATION */
    WB_CHECK(check_for_string(RESPONSE_BUFFER, response));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_vtd_set(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_vtd_set_neg(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_vtd_read(char *at_string_p)
{
    wb_testfcn_vtd_set("+VTD=55");
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "55"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_vtd_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "(0-10000000)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_d(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    PARSER_p->connecting_voice_call = false;
}


void wb_testfcn_d_failure(char *at_string_p)
{

    int cmee = PARSER_p->CMEE;
    PARSER_p->CMEE = 1;

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    if (CURRENT_TEST_CASE == wb_testcase_d_failure_1) {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 212"));
    } else {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 27"));
    }

    WB_CHECK(!leftovers(RESPONSE_BUFFER));
    PARSER_p->CMEE = cmee;
}

void wb_testfcn_d_busy(char *at_string_p)
{
    PARSER_p->ECAM = 0;

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;

    EXECUTOR.cn_selector_callback_event(FD_CN_EVENT, NULL); /* CN_EVENT_CALL_STATE_CHANGED */

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "BUSY"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_d_no_answer(char *at_string_p)
{
    PARSER_p->ECAM = 0;

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;

    EXECUTOR.cn_selector_callback_event(FD_CN_EVENT, NULL); /* CN_EVENT_CALL_STATE_CHANGED */

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "NO ANSWER"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_d_no_response(char *at_string_p)
{
    PARSER_p->ECAM = 0;

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;

    EXECUTOR.cn_selector_callback_event(FD_CN_EVENT, NULL); /* CN_EVENT_CALL_STATE_CHANGED */

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "NO CARRIER"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_d_no_dialtone(char *at_string_p)
{
    PARSER_p->ECAM = 0;

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;

    EXECUTOR.cn_selector_callback_event(FD_CN_EVENT, NULL); /* CN_EVENT_CALL_STATE_CHANGED */

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "NO DIALTONE"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_dtmf_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_dtmf(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_dtmf_neg(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_z_do(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    for (state = 0; state < 5 && PARSER_p->IsPending; state++) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_z_set(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

#define AT_RESET "V1E"
void wb_testfcn_sequence(char *at_string_p)
{

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "0"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
    Parser_AT(AT_RESET, strlen(AT_RESET), AT_DATA_COMMAND, PARSER_p);
}

void wb_testfcn_h(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    for (state = 0; state < 2 && PARSER_p->IsPending; state++) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_h_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "H: (0)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_etty_set(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cops_network_manual_registration_with_automatic_fallback(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
* Extended Error Reporting Test Functions                          *
** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
*/

static char *correct_default_string = "+CEER: ";  /* from init_call_fail_cause() */

/*******************************************************/
static void init_call_fail_cause(void);
/*
 * static void init_call_fail_cause(void)
 *  /n my version of exe_last_fail_cause_init()
 *
 */

static void init_call_fail_cause(void)
{
    struct exe_s *my_exe_p = exe_get_exe();
    exe_last_fail_cause_t *last_fail_p = NULL;

    last_fail_p = &(my_exe_p->last_fail);
    exe_last_fail_cause_init(last_fail_p);
}

/* Test program to simulate dialup failure  OK */
void wb_testfcn_ceer_d_failure(char *at_string)
{
    char *correct_string = correct_default_string;
    wb_testutil_disable_realtime_reporting();
    RESPONSE_BUFFER_LENGTH = 0;
    state = 0;

    /* first call of AT+CEER should return default value */
    init_call_fail_cause();
    Parser_AT("+CEER", strlen("+CEER"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, correct_default_string));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
    RESPONSE_BUFFER_LENGTH = 0;

    /* setup expected output */
    if (wb_testcase_ceer_d_busy == CURRENT_TEST_CASE) {
        correct_string = "+CEER: Exit Cause: CC17 (User busy)";
    } else if (wb_testcase_ceer_d_fcn_bad == CURRENT_TEST_CASE) {
        correct_string = "+CEER: Exit Cause: CS20 (Fixed Dialing Number check)";
    } else {
        assert(0);
    }

    /* solicited response from CN */
    Parser_AT(at_string, strlen(at_string), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
    EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL); /* CN_RESPONSE_DIAL */
    state++;
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
    RESPONSE_BUFFER_LENGTH = 0;
    /* unsolicited response from CN */
    EXECUTOR.cn_selector_callback_event(FD_CN_EVENT, NULL); /* CN_EVENT_CALL_STATE_CHANGED */
    if (wb_testcase_ceer_d_busy == CURRENT_TEST_CASE) {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "BUSY"));
    } else {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "NO CARRIER"));
    }
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* second call of AT+CEER should return correct string for simulated response */
    Parser_AT("+CEER", strlen("+CEER"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, correct_string));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
    init_call_fail_cause();

    /* unsolicited callback from CN */
    EXECUTOR.cn_selector_callback_event(FD_CN_REQUEST, NULL); /* CN_EVENT_CALL_STATE_CHANGED */
    state++;
    WB_CHECK(!leftovers(RESPONSE_BUFFER)); /* realtime reporting is disabled */
    RESPONSE_BUFFER_LENGTH = 0;

    /* third call of AT+CEER should return correct string for simulated event */
    Parser_AT("+CEER", strlen("+CEER"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, correct_string));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

/*******************************************************/

/* Test program to simulate external disconnection OK */
void wb_testfcn_ceer_disconnect(char *at_string)
{
    char *correct_string = correct_default_string;
    wb_testutil_disable_realtime_reporting();
    RESPONSE_BUFFER_LENGTH = 0;
    state = 0;

    /* first call of AT+CEER should return default value */
    init_call_fail_cause();
    Parser_AT("+CEER", strlen("+CEER"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, correct_default_string));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
    RESPONSE_BUFFER_LENGTH = 0;

    /* setup callback context and expected output */
    if (wb_testcase_ceer_disc_normal == CURRENT_TEST_CASE) {
        correct_string = "+CEER: Exit Cause: CC16 (Normal call clearing)";
    } else if (wb_testcase_ceer_disc_channel_lost == CURRENT_TEST_CASE) {
        correct_string = "+CEER: Exit Cause: CS19 (Call re-establishment failed)";
    } else {
        assert(0);
    }

    /* unsolicited callback from CN */
    EXECUTOR.cn_selector_callback_event(FD_CN_REQUEST, NULL); /* CN_EVENT_CALL_STATE_CHANGED */
    state++;
    WB_CHECK(!leftovers(RESPONSE_BUFFER)); /* realtime reporting is disabled */
    RESPONSE_BUFFER_LENGTH = 0;

    /* second call of AT+CEER should return correct string for simulated event */
    Parser_AT("+CEER", strlen("+CEER"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, correct_string));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

}

/*******************************************************/

/* Test program to simulate hangup failure */
void wb_testfcn_ceer_h_failure(char *at_string)
{
    char *correct_string = correct_default_string;
    wb_testutil_disable_realtime_reporting();
    RESPONSE_BUFFER_LENGTH = 0;
    state = 0;

    /* first call of AT+CEER should return default value */
    init_call_fail_cause();
    Parser_AT("+CEER", strlen("+CEER"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, correct_default_string));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
    RESPONSE_BUFFER_LENGTH = 0;

    /* setup expected output */
    if (wb_testcase_ceer_h_no_call == CURRENT_TEST_CASE) {
        correct_string = "+CEER: Exit Cause: CS1 (Call does not exist)";
    } else {
        assert(0);
    }

    state = 0;
    Parser_AT(at_string, strlen(at_string), AT_DATA_COMMAND, PARSER_p);

    /* cn_request_hangup() advances state to 1, so callback will skip sending call list */
    for (; state < 2 && PARSER_p->IsPending; state++) {
        WB_CHECK(!leftovers(RESPONSE_BUFFER));
        RESPONSE_BUFFER_LENGTH = 0;
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
    RESPONSE_BUFFER_LENGTH = 0;

    /* second call of AT+CEER should return correct string for simulated response */
    Parser_AT("+CEER", strlen("+CEER"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, correct_string));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


/* Test program to simulate hangup success OK */
void wb_testfcn_ceer_h_ok(char *at_string)
{
    char *correct_string = correct_default_string;
    wb_testutil_disable_realtime_reporting();
    RESPONSE_BUFFER_LENGTH = 0;
    state = 0;

    /* first call of AT+CEER should return default value */
    init_call_fail_cause();
    Parser_AT("+CEER", strlen("+CEER"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, correct_default_string));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
    RESPONSE_BUFFER_LENGTH = 0;

    /* setup expected output */
    if (wb_testcase_ceer_h_ok == CURRENT_TEST_CASE) {
        correct_string = "+CEER: Exit Cause: CC16 (Normal call clearing)";
    } else if (wb_testcase_ceer_h_ok_unspec == CURRENT_TEST_CASE) {
        correct_string = "+CEER: Exit Cause: CC31 (Normal unspecified)";
    } else {
        assert(0);
    }

    state = 0;
    Parser_AT(at_string, strlen(at_string), AT_DATA_COMMAND, PARSER_p);

    /* cn_request_hangup() advances state to 1, so callback will skip sending call list */
    for (; state < 2 && PARSER_p->IsPending; state++) {
        WB_CHECK(!leftovers(RESPONSE_BUFFER));
        RESPONSE_BUFFER_LENGTH = 0;
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
    RESPONSE_BUFFER_LENGTH = 0;

    /* Hangup command did not fail, so CEER has not been updated yet */
    Parser_AT("+CEER", strlen("+CEER"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, correct_default_string));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
    RESPONSE_BUFFER_LENGTH = 0;

    /* unsolicited callback from CN */
    state = 2; /* CN_EVENT_CALL_STATE_CHANGED */
    EXECUTOR.cn_selector_callback_event(FD_CN_REQUEST, NULL);
    WB_CHECK(!leftovers(RESPONSE_BUFFER)); /* realtime reporting is disabled */
    RESPONSE_BUFFER_LENGTH = 0;

    /* second call of AT+CEER should return correct string for simulated event */
    Parser_AT("+CEER", strlen("+CEER"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, correct_string));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

/* Test program to simulate answer failure */
void wb_testfcn_ceer_a_failure(char *at_string)
{
    char *correct_string = correct_default_string;
    wb_testutil_disable_realtime_reporting();
    RESPONSE_BUFFER_LENGTH = 0;
    state = 0;

    /* first call of AT+CEER should return default value */
    init_call_fail_cause();
    Parser_AT("+CEER", strlen("+CEER"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, correct_default_string));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
    RESPONSE_BUFFER_LENGTH = 0;

    /* setup expected output */
    if (wb_testcase_ceer_a_barred_cug == CURRENT_TEST_CASE) {
        correct_string = "+CEER: Exit Cause: CC55 (Incoming call barred within CUG)";
    } else {
        assert(0);
    }

    /* solicited response from CN */
    Parser_AT(at_string, strlen(at_string), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
    EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL); /* CN_RESPONSE_CURRENT_CALL_LIST */
    state++;
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
    EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL); /* CN_RESPONSE_ANSWER_CALL */
    state++;
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
    RESPONSE_BUFFER_LENGTH = 0;

    /* second call of AT+CEER should return correct string for simulated response */
    Parser_AT("+CEER", strlen("+CEER"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, correct_string));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


/*******************************************************/

/* Test program to simulate external network error events */
void wb_testfcn_ceer_netfail_1(char *at_string)
{
    static int counter = 0;
    char *correct_string = correct_default_string;
    wb_testutil_disable_realtime_reporting();
    RESPONSE_BUFFER_LENGTH = 0;
    state = 0;

    /* first call of AT+CEER should return default value */
    if (0 == counter) {
        init_call_fail_cause();
        Parser_AT("+CEER", strlen("+CEER"), AT_DATA_COMMAND, PARSER_p);
        WB_CHECK(check_for_string(RESPONSE_BUFFER, correct_default_string));
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
        WB_CHECK(!leftovers(RESPONSE_BUFFER));
        RESPONSE_BUFFER_LENGTH = 0;
    }

    counter++;

    /* setup callback context and expected output */
    if (wb_testcase_ceer_unsol_gsm == CURRENT_TEST_CASE) {
        correct_string = "+CEER: Detach Cause: GMM09 (MS identity cannot be derived by the network)";
    } else if (wb_testcase_ceer_unsol_ss_fail == CURRENT_TEST_CASE) {
        correct_string = "+CEER: Detach Cause: GMM09 (MS identity cannot be derived by the network)";
    } else {
        assert(0);
    }

    /* unsolicited callback from CN */
    EXECUTOR.cn_selector_callback_event(FD_CN_REQUEST, NULL); /* CN_EVENT_CALL_STATE_CHANGED */
    state++;
    WB_CHECK(!leftovers(RESPONSE_BUFFER)); /* realtime reporting is disabled */
    RESPONSE_BUFFER_LENGTH = 0;

    /* second call of AT+CEER should return correct string for simulated event */
    Parser_AT("+CEER", strlen("+CEER"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, correct_string));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

/*******************************************************/

/* Test program to simulate external network error events */
void wb_testfcn_ceer_pscc_1(char *at_string)
{
    char *correct_string = correct_default_string;
    wb_testutil_disable_realtime_reporting();
    RESPONSE_BUFFER_LENGTH = 0;
    state = 0;

    /* setup callback context and expected output */
    if (wb_testcase_ceer_unsol_pscc == CURRENT_TEST_CASE) {
        correct_string = "+CEER: Deactivate Cause: SM42 (Syntactical error in the TFT operation)";
    } else {
        assert(0);
    }

    /* unsolicited callback already happened in wb_testcase_cgact_set_3gpp_failure testcase */

    /* call of AT+CEER should return correct string for simulated event */
    Parser_AT("+CEER", strlen("+CEER"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, correct_string));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_ceer_test(char *at_string)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string, strlen(at_string), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testutil_disable_realtime_reporting(void)
{
    Parser_AT("*ECAM=0", strlen("*ECAM=0"), AT_DATA_COMMAND, PARSER_p);
    RESPONSE_BUFFER_LENGTH = 0;
#ifdef EXE_USE_ATC_CUSTOMER_EXTENSIONS
    wb_testutil_extended_disable_realtime_reporting();
#endif
}


void wb_testutil_enable_realtime_reporting(void)
{
    Parser_AT("*ECAM=1", strlen("*ECAM=1"), AT_DATA_COMMAND, PARSER_p);
    RESPONSE_BUFFER_LENGTH = 0;
#ifdef EXE_USE_ATC_CUSTOMER_EXTENSIONS
    wb_testutil_extended_enable_realtime_reporting();
#endif
}


void wb_testfcn_ecme_set(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    switch (CURRENT_TEST_CASE) {
    case wb_testcase_ecme_set_0:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
        break;
    case wb_testcase_ecme_set_1:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
        break;
    case wb_testcase_ecme_set_2: /* Invalid value */
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
        break;
    }

    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_ecme_read(char *at_string_p)
{
    int ecme_read = 0;
    char ecme_resp[20];

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    switch (CURRENT_TEST_CASE) {
    case wb_testcase_ecme_read_0:
        sprintf(ecme_resp, "*ECME: 0", ecme_read);
        WB_CHECK(check_for_string(RESPONSE_BUFFER, ecme_resp));
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
        break;
    case wb_testcase_ecme_read_1:
        sprintf(ecme_resp, "*ECME: 1", ecme_read);
        WB_CHECK(check_for_string(RESPONSE_BUFFER, ecme_resp));
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
        break;
    }

    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_ecme_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "*ECME: (0-1)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_ecme_unsolicited(char *at_string_p)
{
    int entry;

    RESPONSE_BUFFER_LENGTH = 0;
    EXECUTOR.cn_selector_callback_event(FD_CN_EVENT, NULL); /* CN_EVENT_NET_DETAILED_FAIL_CAUSE */

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "*ECME: 102,\"Detach Cause: GMM02 (IMSI unknown in HLR)\""));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_elat_set(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    switch (CURRENT_TEST_CASE) {
    case wb_testcase_elat_set_0:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
        break;
    case wb_testcase_elat_set_1:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
        break;
    case wb_testcase_elat_set_2: /* Invalid value */
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
        break;
    }

    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_elat_read(char *at_string_p)
{
    int elat_read = 0;
    char elat_resp[20];

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    switch (CURRENT_TEST_CASE) {
    case wb_testcase_elat_read_0:
        sprintf(elat_resp, "*ELAT: 0");
        WB_CHECK(check_for_string(RESPONSE_BUFFER, elat_resp));
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
        break;
    case wb_testcase_elat_read_1:
        sprintf(elat_resp, "*ELAT: 1");
        WB_CHECK(check_for_string(RESPONSE_BUFFER, elat_resp));
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
        break;
    }

    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_elat_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "*ELAT: (0-1)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_elat_unsolicited(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    EXECUTOR.cn_selector_callback_event(FD_CN_EVENT, NULL); /* CN_EVENT_GENERATE_LOCAL_COMFORT_TONES */

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "*ELAT: 1"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_elin_set(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;

    /* Make sure CME ERROR codes are enabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    switch (CURRENT_TEST_CASE) {
    case wb_testcase_elin_set_0: /* Fallthrough */
    case wb_testcase_elin_set_3: {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 50"));
        break;
    }
    case wb_testcase_elin_set_2_no_support: {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 3"));
        break;
    }
    default:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
        break;
    }

    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

void wb_testfcn_elin_read(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);

    if (wb_testcase_elin_read_1 == CURRENT_TEST_CASE) {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "*ELIN: 1"));
    } else {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "*ELIN: 2"));
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_elin_test(char *at_string_p)
{
    unsigned int i;

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);

    if (wb_testcase_elin_test_no_support == CURRENT_TEST_CASE) {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "*ELIN: (1)"));
    } else {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "*ELIN: (1,2)"));
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_ecrat_set(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL); /* CN_RESPONSE_SET_EVENT_REPORTING */
    }

    switch (CURRENT_TEST_CASE) {
    case wb_testcase_ecrat_set_1: /* *ECRAT=0  positive test: unsubscribe */
    case wb_testcase_ecrat_set_2: /* *ECRAT=1  positive test: subscribe   */
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
        break;
    case wb_testcase_ecrat_set_3: /* *ECRAT      negative test: DO cmd           */
    case wb_testcase_ecrat_set_4: /* *ECRAT=     negative test: no param         */
    case wb_testcase_ecrat_set_5: /* *ECRAT=-1   negative test: negative param   */
    case wb_testcase_ecrat_set_6: /* *ECRAT=2    negative test: legal value +1   */
    case wb_testcase_ecrat_set_7: /* *ECRAT=67   negative test: way out of range */
    case wb_testcase_ecrat_set_8: /* *ECRAT=0,1  negative test: two params       */
    case wb_testcase_ecrat_set_9: /* *ECRAT=0    negative test: CNS failure      */
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
        break;
    default:
        printf("%s invoked with wrong testcase!", __func__);
        WB_CHECK(0);
        break;
    }

    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_ecrat_read(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;

    PARSER_p->ECRAT = 1;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL); /* CN_RESPONSE_RAT_NAME */
    }

    switch (CURRENT_TEST_CASE) {
    case wb_testcase_ecrat_read_1: /* *ECRAT? positive test: UNKNOWN */
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "*ECRAT: 1,0"));
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
        break;
    case wb_testcase_ecrat_read_2: /* *ECRAT? positive test: GSM     */
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "*ECRAT: 1,1"));
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
        break;
    case wb_testcase_ecrat_read_3: /* *ECRAT? positive test: UMTS    */
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "*ECRAT: 1,2"));
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
        break;
    case wb_testcase_ecrat_read_4: /* *ECRAT? positive test: EPS     */
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "*ECRAT: 1,4"));
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
        break;
    case wb_testcase_ecrat_read_5: /* *ECRAT? negative test: CNS failure */
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
        break;
    default:
        printf("%s invoked with wrong testcase!", __func__);
        WB_CHECK(0);
        break;
    }

    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    PARSER_p->ECRAT = 0;
}

void wb_testfcn_ecrat_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!PARSER_p->IsPending);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "*ECRAT: (0,1)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));

    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_ecrat_unsolicited(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;

    PARSER_p->ECRAT = 1;

    EXECUTOR.cn_selector_callback_event(FD_CN_EVENT, NULL); /* CN_EVENT_RAT_NAME */

    switch (CURRENT_TEST_CASE) {
    case wb_testcase_ecrat_unsolicited_1: /* positive test: EPS */
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "*ECRAT: 4"));
        break;
    case wb_testcase_ecrat_unsolicited_2: /* positive test: UMTS */
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "*ECRAT: 2"));
        break;
    case wb_testcase_ecrat_unsolicited_3: /* positive test: GSM */
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "*ECRAT: 1"));
        break;
    case wb_testcase_ecrat_unsolicited_4: /* positive test: UNKNOWN */
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "*ECRAT: 0"));
        break;
    default:
        printf("%s invoked with wrong testcase!", __func__);
        WB_CHECK(0);
        break;
    }

    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    PARSER_p->ECRAT = 0;
}

void wb_testfcn_ennir_set(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL); /* CN_RESPONSE_SET_EVENT_REPORTING */
    }

    switch (CURRENT_TEST_CASE) {
    case wb_testcase_ennir_set_1: /* *ENNIR=0  positive test: unsubscribe */
    case wb_testcase_ennir_set_2: /* *ENNIR=1  positive test: subscribe   */
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
        break;
    case wb_testcase_ennir_set_3: /* *ENNIR=2 negative test   */
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
        break;
    default:
        printf("%s invoked with wrong testcase!", __func__);
        WB_CHECK(0);
        break;
    }

    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_ennir_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!PARSER_p->IsPending);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "*ENNIR: (0,1)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));

    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_ennir_read(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!PARSER_p->IsPending);
    switch (CURRENT_TEST_CASE) {
    case wb_testcase_ennir_read_1:
         WB_CHECK(check_for_string(RESPONSE_BUFFER, "*ENNIR: 0"));
         WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
         break;
    case wb_testcase_ennir_read_2:
         WB_CHECK(check_for_string(RESPONSE_BUFFER, "*ENNIR: 1"));
         WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
         break;
    default:
        printf("%s invoked with wrong testcase!", __func__);
        WB_CHECK(0);
        break;
    }

    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_ennir_unsolicited(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    EXECUTOR.cn_selector_callback_event(FD_CN_EVENT, NULL);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "*ENNIR: \"Airtel\",\"At\",\"12345\""));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

}



void wb_testfcn_epwrred_set(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL); /* CN_RESPONSE_SEND_TX_BACK_OFF_EVENT */
    }

    switch (CURRENT_TEST_CASE) {
    case wb_testcase_epwrred_set_1: /* positive case, first value    */
    case wb_testcase_epwrred_set_2: /* positive case, third value    */
    case wb_testcase_epwrred_set_3: /* positive case, middle value   */
    case wb_testcase_epwrred_set_4: /* positive case, last value -1  */
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
        break;
    case wb_testcase_epwrred_set_5: /* negative case, negative value     */
    case wb_testcase_epwrred_set_6: /* negative case, negative value */
    case wb_testcase_epwrred_set_7:  /* negative case, last value + 1               */
    case wb_testcase_epwrred_set_8:  /* negative case, no value                     */
    case wb_testcase_epwrred_set_9:  /* negative case, DO cmd                       */
    case wb_testcase_epwrred_set_10: /* negative case, second parameter             */
    case wb_testcase_epwrred_set_11: /* negative case, string parameter             */
    case wb_testcase_epwrred_set_12: /* negative case, second parameter is a string */
    case wb_testcase_epwrred_set_13: /* positive case, first value                  */
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
        break;
    default:
        printf("%s invoked with wrong testcase!", __func__);
        WB_CHECK(0);
        break;
    }

    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_epwrred_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!PARSER_p->IsPending);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "*EPWRRED: (0-16)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));

    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_eempage_read(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_eempage_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "*EEMPAGE: (0-3),(1-5),(1-60)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_eempage_set_error_par(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT("+CMEE=1", strlen("+CMEE=1"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 50")); /* invalid parameter */
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT("+CMEE=0", strlen("+CMEE=0"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_eempage_set_error_op(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT("+CMEE=1", strlen("+CMEE=1"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 3")); /* operation not allowed */
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT("+CMEE=0", strlen("+CMEE=0"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_eempage_set_ok(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_eempage_unsolicited(char *at_string_p)
{
    RESPONSE_BUFFER[0] = 0;
    EXECUTOR.cn_selector_callback_event(FD_CN_EVENT, NULL);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "*EEMPAGE: 123,456"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_esvn_read(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL); /* CN_RESPONSE_GET_PP_FLAG */
    }

    switch (CURRENT_TEST_CASE) {
    case wb_testcase_esvn_read:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "*ESVN: 4"));
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
        break;
    case wb_testcase_esvn_error1:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
        break;
    case wb_testcase_esvn_error2:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
        break;
    case wb_testcase_esvn_set:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
        break;
    }

    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

