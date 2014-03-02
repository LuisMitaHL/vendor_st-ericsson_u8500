/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : modem_simulator.c
 * Description     : modem simulator main file
 *
 * Author          : Haiyuan Bu <haiyuan.bu@stericsson.com>
 *
 */



#include "signal.h"
#include "sim_stub.h"
#include "func_trace.h"

#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>

#ifndef HAVE_ANDROID_OS
#ifndef HOST_FS_PATH
#define HOST_FS_PATH ""
#endif
static const char       socket_name[] = HOST_FS_PATH "/tmp/sim_stub_a";
#else
static const char       socket_name[] = "/dev/socket/sim_stub_a";
#endif

#define CLOSED_FD       (-1)
#define MODEM_CT        (0x0)

typedef const char     *test_func_t();

#define MAX_NAME_SIZE   (64)

/**
 * Table of mapping test names to test functions.
 * Just expand the list with more items in no particular order.
 */
struct test_entry
{
    char                   *test_name;
    test_func_t            *test_func;
};

//####################################################################################
//functions declaration
//####################################################################################
const char             *test_pc_displaytext();
const char             *test_pc_getinkey();
const char             *test_pc_setupmenu();
const char             *test_pc_selectitem();
const char             *test_pc_providelocalinfo_imei();
const char             *test_pc_providelocalinfo_imeisv();
const char             *test_pc_providelocalinfo_battery();
const char             *test_pc_providelocalinfo_location();
const char             *test_pc_providelocalinfo_nw_searchmode();
const char             *test_pc_providelocalinfo_timing_advance();
const char             *test_pc_providelocalinfo_access_technology();
const char             *test_pc_providelocalinfo_nmr();
const char             *test_pc_providelocalinfo_nmr_01();
const char             *test_pc_providelocalinfo_datetime();
const char             *test_pc_providelocalinfo_language();
const char             *test_pc_providelocalinfo_default();
const char             *test_pc_getinput();
const char             *test_pc_setupeventlist();
const char             *test_pc_setupeventlist_01();
const char             *test_pc_sendsms();
const char             *test_pc_sendsms_01();
const char             *test_pc_sendsms_02();
const char             *test_pc_sendsms_03();
const char             *test_pc_sendsms_04();
const char             *test_pc_senddtmf();
const char             *test_pc_senddtmf_01();
const char             *test_pc_senddtmf_02();
const char             *test_pc_sendss();
const char             *test_pc_sendss_01();
const char             *test_pc_sendss_02();
const char             *test_pc_sendss_03();
const char             *test_pc_sendss_04();
const char             *test_pc_sendss_05();
const char             *test_pc_sendss_06();
const char             *test_pc_sendss_07();
const char             *test_pc_sendss_08();
const char             *test_pc_sendss_09();
const char             *test_pc_sendussd();
const char             *test_pc_sendussd_01();
const char             *test_pc_sendussd_02();
const char             *test_pc_sendussd_03();
const char             *test_pc_sendussd_04();
const char             *test_pc_sendussd_05();
const char             *test_pc_sendussd_06();
const char             *test_pc_sendussd_07();
const char             *test_pc_sendussd_08();
const char             *test_pc_sendussd_09();
const char             *test_pc_setupcall();
const char             *test_pc_setupcall_01();
const char             *test_pc_setupcall_02();
const char             *test_pc_setupcall_03();
const char             *test_pc_setupcall_04();
const char             *test_pc_setupecall();
const char             *test_pc_setupcall_disconnect_others();
const char             *test_pc_setupcall_disconnect_others_01();
const char             *test_pc_setupcall_disconnect_others_02();
const char             *test_pc_setupcall_disconnect_others_03();
const char             *test_pc_setupcall_disconnect_others_04();
const char             *test_pc_setupcall_disconnect_others_05();
const char             *test_pc_setupcall_disconnect_others_06();
const char             *test_pc_setupcall_disconnect_others_07();
const char             *test_pc_setupcall_disconnect_others_08();
const char             *test_pc_setupcall_hold_others();
const char             *test_pc_setupcall_hold_others_01();
const char             *test_pc_setupcall_hold_others_02();
const char             *test_pc_setupcall_00();
const char             *test_pc_pollinterval();
const char             *test_pc_pollinterval_01();
const char             *test_pc_pollinterval_02();
const char             *test_pc_pollingoff();
const char             *test_pc_pollingoff_01();
const char             *test_pc_pollingoff_02();
const char             *test_pc_moretime();
const char             *test_pc_timermanagement_01();
const char             *test_pc_timermanagement_02();
const char             *test_pc_timermanagement_03();
const char             *test_pc_timermanagement_04();
const char             *test_pc_timermanagement_05();
const char             *test_pc_timermanagement_06();
const char             *test_pc_timermanagement_07();
const char             *test_pc_timermanagement_08();
const char             *test_pc_timermanagement_09();
const char             *test_pc_timermanagement_10();
const char             *test_pc_timermanagement_11();
const char             *test_pc_timermanagement_12();
const char             *test_pc_timermanagement_13();
const char             *test_pc_timermanagement_14();
const char             *test_pc_timermanagement_15();
const char             *test_pc_timermanagement_16();
const char             *test_ind_modem_reset_ind();
const char             *test_cat_not_ready();
const char             *test_cat_not_supported();
const char             *test_card_removed_ind();
const char             *test_card_shutdown();
const char             *test_set_uicc_fail_tests();
const char             *test_reset_uicc_fail_tests();
const char             *test_pc_refresh();
const char             *test_pc_refresh_01();
const char             *test_pc_refresh_02();
const char             *test_pc_refresh_03();
const char             *test_pc_refresh_04();
const char             *test_pc_language_notification();
const char             *test_pc_refresh_reset();
const char             *test_pc_refresh_reset_01();
const char             *test_pc_refresh_reset_02();
const char             *test_pc_refresh_full_file_change();
const char             *test_pc_refresh_full_file_change_01();
const char             *test_pc_refresh_full_file_change_02();
const char             *test_pc_refresh_full_file_change_03();
const char             *test_refresh_reset_ind();
const char             *test_card_fallback();
const char             *test_pc_open_channel();
const char             *test_pc_close_channel();
const char             *test_pc_send_data();
const char             *test_pc_send_data_01();
const char             *test_pc_receive_data();
const char             *test_send_cat_ready();
const char             *test_send_uicc_ready();

char * sim_stub_enc(char *dst, const void *vsrc, size_t n);

const char * sim_stub_dec(const char *src, void *vdst, size_t n, const char *smax);

void sim_stub_i_safe_copy(char *dst, const char *src, size_t n);

int sim_stub_send_generic(int fd, uint16_t cmd, const char *payload, size_t n, uintptr_t client_tag);

//####################################################################################
//global varibale
//####################################################################################
static int socket_fd;

// The following define is just to fool indent not to produce odd indentation!
#define TENTRY(N,F) {N,F}

static const struct test_entry tests[] =
{
    TENTRY("pc_displaytext",                        test_pc_displaytext),
    TENTRY("pc_getinkey",                           test_pc_getinkey),
    TENTRY("pc_setupmenu",                          test_pc_setupmenu),
    TENTRY("pc_selectitem",                         test_pc_selectitem),
    TENTRY("pc_getinput",                           test_pc_getinput),
    TENTRY("pc_providelocalinfo_imei",              test_pc_providelocalinfo_imei),
    TENTRY("pc_providelocalinfo_imeisv",            test_pc_providelocalinfo_imeisv),
    TENTRY("pc_providelocalinfo_battery",           test_pc_providelocalinfo_battery),
    TENTRY("pc_providelocalinfo_location",          test_pc_providelocalinfo_location),
    TENTRY("pc_providelocalinfo_nw_searchmode",     test_pc_providelocalinfo_nw_searchmode),
    TENTRY("pc_providelocalinfo_timing_advance",    test_pc_providelocalinfo_timing_advance),
    TENTRY("pc_providelocalinfo_access_technology", test_pc_providelocalinfo_access_technology),
    TENTRY("pc_providelocalinfo_nmr",               test_pc_providelocalinfo_nmr),
    TENTRY("pc_providelocalinfo_nmr_01",               test_pc_providelocalinfo_nmr_01),
    TENTRY("pc_providelocalinfo_datetime",          test_pc_providelocalinfo_datetime),
    TENTRY("pc_providelocalinfo_language",          test_pc_providelocalinfo_language),
    TENTRY("pc_providelocalinfo_default",           test_pc_providelocalinfo_default),
    TENTRY("pc_setupeventlist",                     test_pc_setupeventlist),
    TENTRY("pc_setupeventlist_01",                  test_pc_setupeventlist_01),
    TENTRY("pc_sendsms",                            test_pc_sendsms),
    TENTRY("pc_sendsms_01",                         test_pc_sendsms_01),
    TENTRY("pc_sendsms_02",                         test_pc_sendsms_02),
    TENTRY("pc_sendsms_03",                         test_pc_sendsms_03),
    TENTRY("pc_sendsms_04",                         test_pc_sendsms_04),
    TENTRY("pc_senddtmf",                           test_pc_senddtmf),
    TENTRY("pc_senddtmf_01",                        test_pc_senddtmf_01),
    TENTRY("pc_senddtmf_02",                        test_pc_senddtmf_02),
    TENTRY("pc_sendss",                             test_pc_sendss),
    TENTRY("pc_sendss_01",                          test_pc_sendss_01),
    TENTRY("pc_sendss_02",                          test_pc_sendss_02),
    TENTRY("pc_sendss_03",                          test_pc_sendss_03),
    TENTRY("pc_sendss_04",                          test_pc_sendss_04),
    TENTRY("pc_sendss_05",                          test_pc_sendss_05),
    TENTRY("pc_sendss_06",                          test_pc_sendss_06),
    TENTRY("pc_sendss_07",                          test_pc_sendss_07),
    TENTRY("pc_sendss_08",                          test_pc_sendss_08),
    TENTRY("pc_sendss_09",                          test_pc_sendss_09),
    TENTRY("pc_sendussd",                           test_pc_sendussd),
    TENTRY("pc_sendussd_01",                        test_pc_sendussd_01),
    TENTRY("pc_sendussd_02",                        test_pc_sendussd_02),
    TENTRY("pc_sendussd_03",                        test_pc_sendussd_03),
    TENTRY("pc_sendussd_04",                        test_pc_sendussd_04),
    TENTRY("pc_sendussd_05",                        test_pc_sendussd_05),
    TENTRY("pc_sendussd_06",                        test_pc_sendussd_06),
    TENTRY("pc_sendussd_07",                        test_pc_sendussd_07),
    TENTRY("pc_sendussd_08",                        test_pc_sendussd_08),
    TENTRY("pc_sendussd_09",                        test_pc_sendussd_09),
    TENTRY("pc_setupcall",                          test_pc_setupcall),
    TENTRY("pc_setupcall_01",                       test_pc_setupcall_01),
    TENTRY("pc_setupcall_02",                       test_pc_setupcall_02),
    TENTRY("pc_setupcall_03",                       test_pc_setupcall_03),
    TENTRY("pc_setupcall_04",                       test_pc_setupcall_04),
    TENTRY("pc_setupecall",                         test_pc_setupecall),
    TENTRY("pc_setupcall_disconnect",               test_pc_setupcall_disconnect_others),
    TENTRY("pc_setupcall_disconnect_01",            test_pc_setupcall_disconnect_others_01),
    TENTRY("pc_setupcall_disconnect_02",            test_pc_setupcall_disconnect_others_02),
    TENTRY("pc_setupcall_disconnect_03",            test_pc_setupcall_disconnect_others_03),
    TENTRY("pc_setupcall_disconnect_04",            test_pc_setupcall_disconnect_others_04),
    TENTRY("pc_setupcall_disconnect_05",            test_pc_setupcall_disconnect_others_05),
    TENTRY("pc_setupcall_disconnect_06",            test_pc_setupcall_disconnect_others_06),
    TENTRY("pc_setupcall_disconnect_07",            test_pc_setupcall_disconnect_others_07),
    TENTRY("pc_setupcall_disconnect_08",            test_pc_setupcall_disconnect_others_08),
    TENTRY("pc_setupcall_hold",                     test_pc_setupcall_hold_others),
    TENTRY("pc_setupcall_hold_01",                  test_pc_setupcall_hold_others_01),
    TENTRY("pc_setupcall_hold_02",                  test_pc_setupcall_hold_others_02),
    TENTRY("pc_setupcall_00",                       test_pc_setupcall_00),
    TENTRY("pc_pollinterval",                       test_pc_pollinterval),
    TENTRY("pc_pollinterval_01",                    test_pc_pollinterval_01),
    TENTRY("pc_pollinterval_02",                    test_pc_pollinterval_02),
    TENTRY("pc_pollingoff",                         test_pc_pollingoff),
    TENTRY("pc_pollingoff_01",                      test_pc_pollingoff_01),
    TENTRY("pc_pollingoff_02",                      test_pc_pollingoff_02),
    TENTRY("pc_moretime",                           test_pc_moretime),
    TENTRY("pc_timermanagement_01",                 test_pc_timermanagement_01),
    TENTRY("pc_timermanagement_02",                 test_pc_timermanagement_02),
    TENTRY("pc_timermanagement_03",                 test_pc_timermanagement_03),
    TENTRY("pc_timermanagement_04",                 test_pc_timermanagement_04),
    TENTRY("pc_timermanagement_05",                 test_pc_timermanagement_05),
    TENTRY("pc_timermanagement_06",                 test_pc_timermanagement_06),
    TENTRY("pc_timermanagement_07",                 test_pc_timermanagement_07),
    TENTRY("pc_timermanagement_08",                 test_pc_timermanagement_08),
    TENTRY("pc_timermanagement_09",                 test_pc_timermanagement_09),
    TENTRY("pc_timermanagement_10",                 test_pc_timermanagement_10),
    TENTRY("pc_timermanagement_11",                 test_pc_timermanagement_11),
    TENTRY("pc_timermanagement_12",                 test_pc_timermanagement_12),
    TENTRY("pc_timermanagement_13",                 test_pc_timermanagement_13),
    TENTRY("pc_timermanagement_14",                 test_pc_timermanagement_14),
    TENTRY("pc_timermanagement_15",                 test_pc_timermanagement_15),
    TENTRY("pc_timermanagement_16",                 test_pc_timermanagement_16),
    TENTRY("modem_reset_ind",                       test_ind_modem_reset_ind),
    TENTRY("pc_refresh",                            test_pc_refresh),
    TENTRY("pc_refresh_01",                         test_pc_refresh_01),
    TENTRY("pc_refresh_02",                         test_pc_refresh_02),
    TENTRY("pc_refresh_03",                         test_pc_refresh_03),
    TENTRY("pc_refresh_04",                         test_pc_refresh_04),
    TENTRY("pc_language_notification",              test_pc_language_notification),
    TENTRY("pc_refresh_reset",                      test_pc_refresh_reset),
    TENTRY("pc_refresh_reset_01",                   test_pc_refresh_reset_01),
    TENTRY("pc_refresh_reset_02",                   test_pc_refresh_reset_02),
    TENTRY("pc_refresh_full_change",                test_pc_refresh_full_file_change),
    TENTRY("pc_refresh_full_change_01",             test_pc_refresh_full_file_change_01),
    TENTRY("pc_refresh_full_change_02",             test_pc_refresh_full_file_change_02),
    TENTRY("pc_refresh_full_change_03",             test_pc_refresh_full_file_change_03),
    TENTRY("pc_open_channel",                       test_pc_open_channel),
    TENTRY("pc_close_channel",                      test_pc_close_channel),
    TENTRY("pc_send_data",                          test_pc_send_data),
    TENTRY("pc_send_data_01",                       test_pc_send_data_01),
    TENTRY("pc_receive_data",                       test_pc_receive_data),
    TENTRY("send_refresh_reset",                    test_refresh_reset_ind),
    TENTRY("send_cat_not_ready",                    test_cat_not_ready),
    TENTRY("send_cat_not_supported",                test_cat_not_supported),
    TENTRY("send_card_removed",                     test_card_removed_ind),
    TENTRY("send_cat_ready",                        test_send_cat_ready),
    TENTRY("send_uicc_ready",                       test_send_uicc_ready),
    TENTRY("set_uicc_fail_tests",                   test_set_uicc_fail_tests),
    TENTRY("reset_uicc_fail_tests",                 test_reset_uicc_fail_tests),
    TENTRY("card_fallback",                         test_card_fallback),
    TENTRY("card_shutdown",                         test_card_shutdown),
    TENTRY(0, 0)                                    /* Terminator */
};

/*
 * Put all test names here in the order you would like them to run when you
 * call ./modem_simulator with no test names
 */
static char      *all_tests[] =
{
    "pc_displaytext",
    "pc_getinkey",
    "pc_getinput",
    "pc_setupeventlist",
    "pc_setupeventlist_01",
    "pc_providelocalinfo_imei",
    "pc_providelocalinfo_imeisv",
    "pc_providelocalinfo_battery",
    "pc_providelocalinfo_location",
    "pc_providelocalinfo_nw_searchmode",
    "pc_providelocalinfo_timing_advance",
    "pc_providelocalinfo_access_technology",
    "pc_providelocalinfo_nmr",
    "pc_providelocalinfo_nmr_01",
    "pc_providelocalinfo_datetime",
    "pc_providelocalinfo_language",
    "pc_providelocalinfo_default",
    "pc_setupmenu",
    "pc_selectitem",
    "pc_sendsms",
    "pc_sendsms_01",
    "pc_sendsms_02",
    "pc_sendsms_03",
    "pc_sendsms_04",
    "pc_senddtmf",
    "pc_senddtmf_01",
    "pc_senddtmf_02",
    "pc_sendss",
    "pc_sendss_01",
    "pc_sendss_02",
    "pc_sendss_03",
    "pc_sendss_04",
    "pc_sendss_05",
    "pc_sendss_06",
    "pc_sendss_07",
    "pc_sendss_08",
    "pc_sendss_09",
    "pc_sendussd",
    "pc_sendussd_01",
    "pc_sendussd_02",
    "pc_sendussd_03",
    "pc_sendussd_04",
    "pc_sendussd_05",
    "pc_sendussd_06",
    "pc_sendussd_07",
    "pc_sendussd_08",
    "pc_sendussd_09",
    "pc_setupcall",
/*    "pc_setupcall_01",
    "pc_setupcall_02",
    "pc_setupcall_03",
    "pc_setupcall_hold",
    "pc_setupcall_disconnect",
    "pc_setupcall_disconnect_01",
    "pc_setupcall_disconnect_02",
    "pc_setupcall_disconnect_03",
    "pc_setupcall_disconnect_04",
    "pc_setupcall_disconnect_05",
    "pc_setupcall_disconnect_06",
*/// Should be executed for tcat client
    "pc_pollinterval",
    "pc_pollinterval_01",
    "pc_pollinterval_02",
    "pc_pollingoff",
    "pc_pollingoff_01",
    "pc_pollingoff_02",
    "pc_moretime",
    "pc_timermanagement_01",
    "pc_timermanagement_02",
    "pc_timermanagement_03",
    "pc_timermanagement_04",
    "pc_timermanagement_05",
    "pc_timermanagement_06",
    "pc_timermanagement_07",
    "pc_timermanagement_08",
    "pc_timermanagement_09",
    "pc_timermanagement_10",
    "pc_timermanagement_11",
    "pc_timermanagement_12",
    "pc_timermanagement_13",
    "pc_timermanagement_14",
    "pc_timermanagement_15",
    "pc_timermanagement_16",
    "pc_language_notification",
    "pc_refresh",
    "pc_refresh_01",
    "pc_refresh_02",
    "pc_refresh_03",
    "pc_refresh_04",
    "pc_refresh_full_change",
    "pc_refresh_full_change_01",
    "pc_refresh_full_change_02",
    "pc_refresh_full_change_03",
    "pc_refresh_reset",
    "pc_refresh_reset_01",
    "pc_refresh_reset_02"
};


//####################################################################################
//functions definition
//####################################################################################

//####################################################################################
//Function Name: test_pc_displaytext
//Decription:
// Triggers sending of Display Text Proactive command
//####################################################################################
const char             *test_pc_displaytext()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_DISPLAY_TEXT, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_displaytext failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_getinkey
//Decription:
// Triggers sending of Get Inkey Proactive command
//####################################################################################
const char             *test_pc_getinkey()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_GET_INKEY, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_getinkey failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_setupmenu
//Decription:
// Triggers sending of Setup Menu Proactive command
//####################################################################################
const char             *test_pc_setupmenu()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SETUP_MENU, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_setupmenu failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_selectitem
//Decription:
// Triggers sending of Select Item Proactive command
//####################################################################################
const char             *test_pc_selectitem()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SELECT_ITEM, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_setupmenu failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_getinput
//Decription:
// Triggers sending of Get Input Proactive command
//####################################################################################
const char             *test_pc_getinput()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_GET_INPUT, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_getinput failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_setupeventlist
//Decription:
// Triggers sending of Setup Event List Proactive command
//####################################################################################
const char             *test_pc_setupeventlist()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SETUP_EVENT_LIST, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_setupeventlist failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_setupeventlist_01
//Decription:
// Triggers sending of Setup Event List Proactive command
// Event List is absent
//####################################################################################
const char             *test_pc_setupeventlist_01()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SETUP_EVENT_LIST_01, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_setupeventlist_01 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_providelocalinfo_imei
//Decription:
// Triggers sending of Provide Local Info(IMEI) Proactive command
//####################################################################################
const char             *test_pc_providelocalinfo_imei()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_PROVIDE_LOCAL_INFO_IMEI, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_providelocalinfo_imei failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_providelocalinfo_imeisv
//Decription:
// Triggers sending of Provide Local Info(IMEISV) Proactive command
//####################################################################################
const char             *test_pc_providelocalinfo_imeisv()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_PROVIDE_LOCAL_INFO_IMEISV, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_providelocalinfo_imeisv failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_providelocalinfo_battery
//Decription:
// Triggers sending of Provide Local Info(Battery) Proactive command
//####################################################################################
const char             *test_pc_providelocalinfo_battery()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_PROVIDE_LOCAL_INFO_BATTERY, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_providelocalinfo_battery failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_providelocalinfo_location
//Decription:
// Triggers sending of Provide Local Info(Location) Proactive command
//####################################################################################
const char             *test_pc_providelocalinfo_location()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_PROVIDE_LOCAL_INFO_LOCATION, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_providelocalinfo_location failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_providelocalinfo_nw_searchmode
//Decription:
// Triggers sending of Provide Local Info(Search Mode) Proactive command
//####################################################################################
const char* test_pc_providelocalinfo_nw_searchmode()
{
    int rc = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_PROVIDE_LOCAL_INFO_NW_SEARCH_MODE, NULL, 0, MODEM_CT);
    return rc ? "test_pc_providelocalinfo_nw_searchmode failed" : NULL;
}

//####################################################################################
//Function Name: test_pc_providelocalinfo_access_technology
//Decription:
// Triggers sending of Provide Local Info(Access Technology) Proactive command
//####################################################################################
const char* test_pc_providelocalinfo_access_technology()
{
    int rc = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_PROVIDE_LOCAL_INFO_ACCESS_TECHNOLOGY, NULL, 0, MODEM_CT);
    return rc ? "test_pc_providelocalinfo_access_technology failed" : NULL;
}

//####################################################################################
//Function Name: test_pc_providelocalinfo_timing_advance
//Decription:
// Triggers sending of Provide Local Info(Timing Advance) Proactive command
//####################################################################################
const char* test_pc_providelocalinfo_timing_advance()
{
    int rc = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_PROVIDE_LOCAL_INFO_TIMING_ADVANCE, NULL, 0, MODEM_CT);
    return rc ? "test_pc_providelocalinfo_location failed" : NULL;
}

//####################################################################################
//Function Name: test_pc_providelocalinfo_nmr
//Decription:
// Triggers sending of Provide Local Info(Network Measurement Result) Proactive command
//####################################################################################
const char* test_pc_providelocalinfo_nmr()
{
    int rc = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_PROVIDE_LOCAL_INFO_NMR, NULL, 0, MODEM_CT);
    return rc ? "test_pc_providelocalinfo_nmr failed" : NULL;
}

//####################################################################################
//Function Name: test_pc_providelocalinfo_nmr_01
//Decription:
// Triggers sending of Provide Local Info(Network Measurement Result) Proactive command
//####################################################################################
const char* test_pc_providelocalinfo_nmr_01()
{
    int rc = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_PROVIDE_LOCAL_INFO_NMR, NULL, 0, MODEM_CT);
    return rc ? "test_pc_providelocalinfo_nmr_01 failed" : NULL;
}


//####################################################################################
//Function Name: test_pc_providelocalinfo_datetime
//Decription:
// Triggers sending of Provide Local Info(Date and Time) Proactive command
//####################################################################################
const char* test_pc_providelocalinfo_datetime()
{
    int rc = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_PROVIDE_LOCAL_INFO_DATE_TIME, NULL, 0, MODEM_CT);
    return rc ? "test_pc_providelocalinfo_datetime failed" : NULL;
}

//####################################################################################
//Function Name: test_pc_providelocalinfo_language
//Decription:
// Triggers sending of Provide Local Info(Language) Proactive command
//####################################################################################
const char* test_pc_providelocalinfo_language()
{
    int rc = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_PROVIDE_LOCAL_INFO_LANGUAGE, NULL, 0, MODEM_CT);
    return rc ? "test_pc_providelocalinfo_datetime failed" : NULL;
}

//####################################################################################
//Function Name: test_pc_providelocalinfo_default
//Decription:
// Triggers sending of Provide Local Info(Rubbish) Proactive command. This will send a value not present in the listed values for this
// Command
//####################################################################################
const char* test_pc_providelocalinfo_default()
{
    int rc = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_PROVIDE_LOCAL_INFO_DEFAULT, NULL, 0, MODEM_CT);
    return rc ? "test_pc_providelocalinfo_datetime failed" : NULL;
}

//####################################################################################
//Function Name: test_pc_sendsms
//Decription:
// Triggers sending of Send SMS Proactive command
// Already packed TPDU is sent
//####################################################################################
const char             *test_pc_sendsms()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SEND_SHORT_MESSAGE, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_sendsms failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_sendsms_01
//Decription:
// Triggers sending of Send SMS Proactive command.
// The SMS TPDU needs to be packed by ME.
//####################################################################################
const char             *test_pc_sendsms_01()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SEND_SHORT_MESSAGE_01, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_sendsms_01 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_sendsms_02
//Decription:
// Triggers sending of Send SMS Proactive command.
// Call Control Failure is tested
//####################################################################################
const char             *test_pc_sendsms_02()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SEND_SHORT_MESSAGE_02, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_sendsms_02 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_sendsms_03
//Decription:
// Triggers sending of Send SMS Proactive command.
// Failure in Sending SMS is tested
//####################################################################################
const char             *test_pc_sendsms_03()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SEND_SHORT_MESSAGE_03, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_sendsms_03 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_sendsms_04
//Decription:
// Triggers sending of Send SMS Proactive command.
// Wrong DCS is sent in Packing Required APDU
//####################################################################################
const char             *test_pc_sendsms_04()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SEND_SHORT_MESSAGE_04, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_sendsms_04 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_senddtmf
//Decription:
// Triggers sending of Send DTMF Proactive command.
//####################################################################################
const char             *test_pc_senddtmf()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SEND_DTMF, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_sendtmf failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_senddtmf_01
//Decription:
// Triggers sending of Send DTMF Proactive command.
// No DTMF string is sent in APDU.
//####################################################################################
const char             *test_pc_senddtmf_01()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SEND_DTMF_01, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_sendtmf_01 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_senddtmf_02
//Decription:
// Triggers sending of Send DTMF Proactive command.
// No DTMF string is sent in APDU.
//####################################################################################
const char             *test_pc_senddtmf_02()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SEND_DTMF_02, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_sendtmf_02 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_sendss
//Decription:
// Triggers sending of Send SS Proactive command.
//####################################################################################
const char             *test_pc_sendss()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SEND_SS, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_sendss failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_sendss_01
//Decription:
// Triggers sending of Send SS Proactive command.
// Call control failure is tested
//####################################################################################
const char             *test_pc_sendss_01()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SEND_SS_01, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_sendss_01 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_sendss_02
//Decription:
// Triggers sending of Send SS Proactive command.
// Temporary Call Control Failure is tested
//####################################################################################
const char             *test_pc_sendss_02()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SEND_SS_02, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_sendss_02 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_sendss_03
//Decription:
// Triggers sending of Send SS Proactive command.
// Temporary SS busy Failure is tested
//####################################################################################
const char             *test_pc_sendss_03()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SEND_SS_03, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_sendss_03 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_sendss_04
//Decription:
// Triggers sending of Send SS Proactive command.
// Temporary SS not available failure is tested
//####################################################################################
const char             *test_pc_sendss_04()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SEND_SS_04, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_sendss_04 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_sendss_05
//Decription:
// Triggers sending of Send SS Proactive command.
// Failure due to USSD busy is tested
//####################################################################################
const char             *test_pc_sendss_05()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SEND_SS_05, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_sendss_05 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_sendss_06
//Decription:
// Triggers sending of Send SS Proactive command.
// Failure due to MM/network errors is tested
//####################################################################################
const char             *test_pc_sendss_06()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SEND_SS_06, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_sendss_06 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_sendss_07
//Decription:
// Triggers sending of Send SS Proactive command.
// Failure due to CC/network errors is tested
//####################################################################################
const char             *test_pc_sendss_07()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SEND_SS_07, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_sendss_07 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_sendss_08
//Decription:
// Triggers sending of Send SS Proactive command.
// General error case
//####################################################################################
const char             *test_pc_sendss_08()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SEND_SS_08, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_sendss_08 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_sendss_09
//Decription:
// Triggers sending of Send SS Proactive command.
// SS String is not present
//####################################################################################
const char             *test_pc_sendss_09()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SEND_SS_09, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_sendss_09 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_sendussd
//Decription:
// Triggers sending of Send USSD Proactive command.
//####################################################################################
const char *test_pc_sendussd()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SEND_USSD, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_sendussd failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_sendussd_01
//Decription:
// Triggers sending of Send USSD Proactive command.
// Call control failure is tested
//####################################################################################
const char *test_pc_sendussd_01()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SEND_USSD_01, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_sendussd_01 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_sendussd_02
//Decription:
// Triggers sending of Send USSD Proactive command.
// Temporary Call control failure is tested
//####################################################################################
const char *test_pc_sendussd_02()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SEND_USSD_02, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_sendussd_02 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_sendussd_03
//Decription:
// Triggers sending of Send USSD Proactive command.
// Temporary SS busy Failure is tested
//####################################################################################
const char *test_pc_sendussd_03()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SEND_USSD_03, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_sendussd_03 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_sendussd_04
//Decription:
// Triggers sending of Send USSD Proactive command.
// Failure due to USSD busy is tested
//####################################################################################
const char *test_pc_sendussd_04()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SEND_USSD_04, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_sendussd_04 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_sendussd_05
//Decription:
// Triggers sending of Send USSD Proactive command.
// Temporary SS not available failure is tested
//####################################################################################
const char *test_pc_sendussd_05()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SEND_USSD_05, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_sendussd_05 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_sendussd_06
//Decription:
// Triggers sending of Send USSD Proactive command.
// Failure due to MM/network errors is tested
//####################################################################################
const char *test_pc_sendussd_06()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SEND_USSD_06, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_sendussd_06 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_sendussd_07
//Decription:
// Triggers sending of Send USSD Proactive command.
// Failure due to CC/network errors is tested
//####################################################################################
const char *test_pc_sendussd_07()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SEND_USSD_07, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_sendussd_07 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_sendussd_08
//Decription:
// Triggers sending of Send USSD Proactive command.
// General error case
//####################################################################################
const char *test_pc_sendussd_08()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SEND_USSD_08, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_sendussd_08 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_sendussd_09
//Decription:
// Triggers sending of Send USSD Proactive command.
// USS String is not present
//####################################################################################
const char *test_pc_sendussd_09()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SEND_USSD_09, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_sendussd_09 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_setupcall
//Decription:
// Triggers sending of Setup Call ( If not Busy ) Proactive command.
//####################################################################################
const char             *test_pc_setupcall()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SETUP_CALL, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_setupcall failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_setupcall_00
//Decription:
// Triggers sending of Setup Call ( If not Busy ) Proactive command.
//####################################################################################
const char             *test_pc_setupcall_00()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SETUP_CALL_00, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_setupcall_00 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_setupcall_01
//Decription:
// Triggers sending of Setup Call ( If not Busy ) Proactive command.
// Setup Contains Sub-Address.
//####################################################################################
const char             *test_pc_setupcall_01()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SETUP_CALL_01, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_setupcall_01 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_setupcall_02
//Decription:
// Triggers sending of Setup Call ( If not Busy ) Proactive command.
// CN Request failure is tested.
//####################################################################################
const char             *test_pc_setupcall_02()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SETUP_CALL_02, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_setupcall_02 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_setupcall_03
//Decription:
// Triggers sending of Setup Call ( If not Busy ) Proactive command.
// CN Request failure (dial) is tested.
//####################################################################################
const char             *test_pc_setupcall_03()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SETUP_CALL_03, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_setupcall_03 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_setupcall_04
//Decription:
// Triggers sending of Setup Call ( If not Busy ) Proactive command.
// CN Response failure (dial) is tested.
//####################################################################################
const char             *test_pc_setupcall_04()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SETUP_CALL_04, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_setupcall_04 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_setupecall
//Decription:
// Triggers sending of Emergency Setup Call ( Disconnect Others ) Proactive command.
//####################################################################################
const char             *test_pc_setupecall()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SETUP_ECALL, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_setupecall failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_setupcall_disconnect_others
//Decription:
// Triggers sending of Setup Call ( Disconnect Others ) Proactive command.
//####################################################################################
const char             *test_pc_setupcall_disconnect_others()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SETUP_CALL_DISCONNECT, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_setupcall_disconnect_others failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_setupcall_disconnect_others_01
//Decription:
// Triggers sending of Setup Call ( Disconnect Others ) Proactive command.
// Call Control Rejected case id tested.
//####################################################################################
const char             *test_pc_setupcall_disconnect_others_01()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SETUP_CALL_DISCONNECT_01, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_setupcall_disconnect_others_01 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_setupcall_disconnect_others_02
//Decription:
// Triggers sending of Setup Call ( Disconnect Others ) Proactive command.
// CN failure response is tested.
//####################################################################################
const char             *test_pc_setupcall_disconnect_others_02()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SETUP_CALL_DISCONNECT_02, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_setupcall_disconnect_others_02 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_setupcall_disconnect_others_03
//Decription:
// Triggers sending of Setup Call ( Disconnect Others ) Proactive command.
// Converts Setup Call to Send USSD after call control - Error case.
//####################################################################################
const char             *test_pc_setupcall_disconnect_others_03()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SETUP_CALL_DISCONNECT_03, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_setupcall_disconnect_others_03 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_setupcall_disconnect_others_04
//Decription:
// Triggers sending of Setup Call ( Disconnect Others ) Proactive command.
// Converts Setup Call to Send USSD after call control.
//####################################################################################
const char             *test_pc_setupcall_disconnect_others_04()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SETUP_CALL_DISCONNECT_04, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_setupcall_disconnect_others_04 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_setupcall_disconnect_others_05
//Decription:
// Triggers sending of Setup Call ( Disconnect Others ) Proactive command.
// Setup Call does not contain the address.
//####################################################################################
const char             *test_pc_setupcall_disconnect_others_05()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SETUP_CALL_DISCONNECT_05, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_setupcall_disconnect_others_05 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_setupcall_disconnect_others_06
//Decription:
// Triggers sending of Setup Call ( Disconnect Others ) Proactive command.
// CN request failure ( hangup ) is tested.
//####################################################################################
const char             *test_pc_setupcall_disconnect_others_06()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SETUP_CALL_DISCONNECT_06, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_setupcall_disconnect_others_06 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_setupcall_disconnect_others_07
//Decription:
// Triggers sending of Setup Call ( Disconnect Others ) Proactive command.
// CN response failure ( hangup ) is tested.
//####################################################################################
const char             *test_pc_setupcall_disconnect_others_07()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SETUP_CALL_DISCONNECT_07, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_setupcall_disconnect_others_07 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_setupcall_disconnect_others_08
//Decription:
// Triggers sending of Setup Call ( Disconnect Others ) Proactive command.
// CN request failure ( get call list ) is tested.
//####################################################################################
const char             *test_pc_setupcall_disconnect_others_08()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SETUP_CALL_DISCONNECT_08, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_setupcall_disconnect_others_08 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_setupcall_hold_others
//Decription:
// Triggers sending of Setup Call ( Hold Others ) Proactive command.
//####################################################################################
const char             *test_pc_setupcall_hold_others()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SETUP_CALL_HOLD, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_setupcall_hold_others failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_setupcall_hold_others_01
//Decription:
// Triggers sending of Setup Call ( Hold Others ) Proactive command.
// CN request failure ( hold ) is tested.
//####################################################################################
const char             *test_pc_setupcall_hold_others_01()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SETUP_CALL_HOLD_01, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_setupcall_hold_others_01 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_setupcall_hold_others_02
//Decription:
// Triggers sending of Setup Call ( Hold Others ) Proactive command.
// CN response failure ( hold ) is tested.
//####################################################################################
const char             *test_pc_setupcall_hold_others_02()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SETUP_CALL_HOLD_02, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_setupcall_hold_others_02 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_pollinterval
//Decription:
// Triggers sending of Poll Interval Proactive command.
//####################################################################################
const char             *test_pc_pollinterval()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_POLL_INTERVAL, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_pollinterval failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_pollinterval_01
//Decription:
// Triggers sending of Poll Interval Proactive command.
// Modem request failure is tested.
//####################################################################################
const char             *test_pc_pollinterval_01()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_POLL_INTERVAL_01, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_pollinterval_01 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_pollinterval_02
//Decription:
// Triggers sending of Poll Interval Proactive command.
// Modem response failure is tested.
//####################################################################################
const char             *test_pc_pollinterval_02()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_POLL_INTERVAL_02, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_pollinterval_02 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_pollingoff
//Decription:
// Triggers sending of Poll Off Proactive command.
//####################################################################################
const char             *test_pc_pollingoff()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_POLLING_OFF, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_pollingoff failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_pollingoff_01
//Decription:
// Triggers sending of Poll Off Proactive command.
// Modem request failure is tested.
//####################################################################################
const char             *test_pc_pollingoff_01()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_POLLING_OFF_01, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_pollingoff_01 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_pollingoff_02
//Decription:
// Triggers sending of Poll Off Proactive command.
// Failure due to non-zero poll interval in response is tested.
//####################################################################################
const char             *test_pc_pollingoff_02()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_POLLING_OFF_02, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_pollingoff_01 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_moretime
//Decription:
// Triggers sending of More Time Proactive command.
//####################################################################################
const char             *test_pc_moretime()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_MORE_TIME, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_moretime failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_timermanagement_01
//Decription:
// Triggers sending of Timer Management Proactive command.
// Deactivate the timer without starting it first.
//####################################################################################
const char             *test_pc_timermanagement_01()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_TIMER_MANAGEMENT_01, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_timermanagement_01 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_timermanagement_02
//Decription:
// Triggers sending of Timer Management Proactive command.
// Issue get time without starting the timer.
//####################################################################################
const char             *test_pc_timermanagement_02()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_TIMER_MANAGEMENT_02, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_timermanagement_02 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_timermanagement_03
//Decription:
// Triggers sending of Timer Management Proactive command.
// Start timer with ID=6 with timeout 0.
//####################################################################################
const char             *test_pc_timermanagement_03()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_TIMER_MANAGEMENT_03, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_timermanagement_03 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_timermanagement_04
//Decription:
// Triggers sending of Timer Management Proactive command.
// Start timer with ID=4 with timeout of 25hrs.
//####################################################################################
const char             *test_pc_timermanagement_04()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_TIMER_MANAGEMENT_04, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_timermanagement_04 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_timermanagement_05
//Decription:
// Triggers sending of Timer Management Proactive command.
// Start timer with ID=4 with timeout of 24hrs 1min.
//####################################################################################
const char             *test_pc_timermanagement_05()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_TIMER_MANAGEMENT_05, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_timermanagement_05 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_timermanagement_06
//Decription:
// Triggers sending of Timer Management Proactive command.
// Start timer with ID=4 with timeout of 24hrs 1sec.
//####################################################################################
const char             *test_pc_timermanagement_06()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_TIMER_MANAGEMENT_06, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_timermanagement_06 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_timermanagement_07
//Decription:
// Triggers sending of Timer Management Proactive command.
// Start, get time and deactivate timer with ID=5.
//####################################################################################
const char             *test_pc_timermanagement_07()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_TIMER_MANAGEMENT_07, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_timermanagement_07 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_timermanagement_08
//Decription:
// Triggers sending of Timer Management Proactive command.
// Start timer with ID=0.
//####################################################################################
const char             *test_pc_timermanagement_08()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_TIMER_MANAGEMENT_08, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_timermanagement_08 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_timermanagement_09
//Decription:
// Triggers sending of Timer Management Proactive command.
// Deactivate timer with ID=0.
//####################################################################################
const char             *test_pc_timermanagement_09()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_TIMER_MANAGEMENT_09, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_timermanagement_09 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_timermanagement_10
//Decription:
// Triggers sending of Timer Management Proactive command.
// Issue get time for ID = 0.
//####################################################################################
const char             *test_pc_timermanagement_10()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_TIMER_MANAGEMENT_10, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_timermanagement_10 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_timermanagement_11
//Decription:
// Triggers sending of Timer Management Proactive command.
// Start all 8 timers, and wait for expiry in correct order.
//####################################################################################
const char             *test_pc_timermanagement_11()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_TIMER_MANAGEMENT_11, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_timermanagement_11 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_timermanagement_12
//Decription:
// Triggers sending of Timer Management Proactive command.
// Send 93,00 for expiry envelope and wait for response success to resend.
//####################################################################################
const char             *test_pc_timermanagement_12()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_TIMER_MANAGEMENT_12, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_timermanagement_12 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_timermanagement_13
//Decription:
// Triggers sending of Timer Management Proactive command.
// Start 3 timers, send 93,00 to first expiry. Wait for OK response for sending other timer expiry messages.
//####################################################################################
const char             *test_pc_timermanagement_13()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_TIMER_MANAGEMENT_13, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_timermanagement_13 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_timermanagement_14
//Decription:
// Triggers sending of Timer Management Proactive command.
// Send 93,00 when timer expiry envelope is sent. Then deactivate timer.
//####################################################################################
const char             *test_pc_timermanagement_14()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_TIMER_MANAGEMENT_14, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_timermanagement_14 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_timermanagement_15
//Decription:
// Triggers sending of Timer Management Proactive command.
// Timer ID is not present.
//####################################################################################
const char             *test_pc_timermanagement_15()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_TIMER_MANAGEMENT_15, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_timermanagement_15 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_timermanagement_16
//Decription:
// Triggers sending of Timer Management Proactive command.
// Send TR before sending Timer expiry envelope response.
//####################################################################################
const char             *test_pc_timermanagement_16()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_TIMER_MANAGEMENT_16, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_timermanagement_16 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_cat_not_ready
//Decription:
// Triggers sending of cat not ready.
//####################################################################################
const char             *test_cat_not_ready()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_CAT_NOT_READY_IND, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_cat_not_ready failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_cat_not_supported
//Decription:
// Triggers sending of cat not supported.
//####################################################################################
const char             *test_cat_not_supported()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_CAT_NOT_SUPPORTED, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_cat_not_supported failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_card_removed_ind
//Decription:
// Triggers sending of card removed ind
//####################################################################################
const char             *test_card_removed_ind()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_CARD_REMOVED_IND, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_card_removed_ind failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_card_shutdown
//Decription:
// Triggers sending of card shutdown
//####################################################################################
const char             *test_card_shutdown()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_CARD_SHUTDOWN, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_card_shutdown failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_set_uicc_fail_tests
//Decription:
// Triggers
//####################################################################################
const char             *test_set_uicc_fail_tests()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_SET_UICC_FAIL_TESTS, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_set_uicc_fail_tests failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_reset_uicc_fail_tests
//Decription:
// Triggers
//####################################################################################
const char             *test_reset_uicc_fail_tests()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_RESET_UICC_FAIL_TESTS, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_reset_uicc_fail_tests failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_refresh
//Decription:
// Triggers sending of Refresh Proactive command.
//####################################################################################
const char             *test_pc_refresh()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_REFRESH, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_refresh failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_refresh
//Decription:
// Triggers sending of Refresh ind.
//####################################################################################
const char             *test_refresh_reset_ind()
{
    int rv;

    printf("test_refresh_reset_ind\n");
    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_REFRESH_RESET_IND, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_refresh_reset_ind failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_refresh_01
//Decription:
// Triggers sending of Refresh Proactive command.
// Modem request failure is tested.
//####################################################################################
const char             *test_pc_refresh_01()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_REFRESH_01, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_refresh_01 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_refresh_02
//Decription:
// Triggers sending of Refresh Proactive command.
// Modem response failure is tested.
//####################################################################################
const char             *test_pc_refresh_02()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_REFRESH_02, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_refresh_02 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_refresh_03
//Decription:
// Triggers sending of Refresh Proactive command.
// AID is not present.
//####################################################################################
const char             *test_pc_refresh_03()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_REFRESH_03, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_refresh_03 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_refresh_04
//Decription:
// Triggers sending of Refresh Proactive command.
// File list is not present.
//####################################################################################
const char             *test_pc_refresh_04()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_REFRESH_04, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_refresh_04 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_refresh_reset
//Decription:
// Triggers sending of Refresh (RESET) Proactive command.
//####################################################################################
const char             *test_pc_refresh_reset()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_REFRESH_RESET, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_refresh_reset failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_refresh_reset_01
//Decription:
// Triggers sending of Refresh (RESET) Proactive command.
// Modem request failure is tested.
//####################################################################################
const char             *test_pc_refresh_reset_01()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_REFRESH_RESET_01, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_refresh_reset_01 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_refresh_reset_02
//Decription:
// Triggers sending of Refresh (RESET) Proactive command.
// Modem response failure is tested.
//####################################################################################
const char             *test_pc_refresh_reset_02()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_REFRESH_RESET_02, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_refresh_reset_02 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_refresh_full_file_change
//Decription:
// Triggers sending of Refresh (Full File Change) Proactive command.
//####################################################################################
const char             *test_pc_refresh_full_file_change()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_REFRESH_FULL_CHANGE_NOTIFICATION, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_refresh_full_file_change failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_refresh_full_file_change_01
//Decription:
// Triggers sending of Refresh (Full File Change) Proactive command.
// Modem request failure is tested.
//####################################################################################
const char             *test_pc_refresh_full_file_change_01()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_REFRESH_FULL_CHANGE_NOTIFICATION_01, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_refresh_full_file_change_01 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_refresh_full_file_change_02
//Decription:
// Triggers sending of Refresh (Full File Change) Proactive command.
// Modem response failure is tested.
//####################################################################################
const char             *test_pc_refresh_full_file_change_02()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_REFRESH_FULL_CHANGE_NOTIFICATION_02, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_refresh_full_file_change_02 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_refresh_full_file_change_03
//Decription:
// Triggers sending of Refresh (Full File Change) Proactive command.
// Contains AID.
//####################################################################################
const char             *test_pc_refresh_full_file_change_03()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_REFRESH_FULL_CHANGE_NOTIFICATION_03, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_refresh_full_file_change_03 failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_ind_modem_reset_ind
//Decription:
// Triggers sending of Modem Reset.
//####################################################################################
const char             *test_ind_modem_reset_ind()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_IND_MODEM_RESET, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_ind_modem_reset_ind failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_pc_language_notification
//Decription:
// Triggers sending of Language Notification Proactive command.
//####################################################################################
const char             *test_pc_language_notification()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_LANGUAGE_NOTIFICATION, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_language_notification failed";
    }

    return 0;
}

//####################################################################################
//Function Name: test_card_fallback
//Decription:
// Triggers sending of Card fallback indication.
//####################################################################################
const char             *test_card_fallback()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_CARD_FALLBACK, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_card_fallback failed";
    }

    return 0;

}

//####################################################################################
//Function Name: test_pc_open_channel
//Decription:
// Triggers sending of Open Channel Proactive Command
//####################################################################################
const char             *test_pc_open_channel()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_OPEN_CHANNEL, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_open_channel failed";
    }
    return 0;
}

//####################################################################################
//Function Name: test_pc_close_channel
//Decription:
// Triggers sending of Close Channel Proactive Command
//####################################################################################
const char             *test_pc_close_channel()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_CLOSE_CHANNEL, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_close_channel failed";
    }
    return 0;
}

//####################################################################################
//Function Name: test_pc_send_data
//Decription:
// Triggers sending of Send Data Proactive Command
//####################################################################################
const char             *test_pc_send_data()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SEND_DATA, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_send_data failed";
    }
    return 0;
}

//####################################################################################
//Function Name: test_pc_send_data_01
//Decription:
// Triggers sending of Send Data Proactive Command
//The CAT Response for TR is delayed
//####################################################################################
const char             *test_pc_send_data_01()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_SEND_DATA_01, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_send_data_01 failed";
    }
    return 0;
}

//####################################################################################
//Function Name: test_pc_receive_data
//Decription:
// Triggers sending of Receive Data Proactive Command
//####################################################################################
const char             *test_pc_receive_data()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_PC_RECEIVE_DATA, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_pc_receive_data failed";
    }
    return 0;
}

//####################################################################################
//Function Name: test_send_cat_ready
//Decription:
// Triggers sending of UICC Cat Indication for Modem Ready
//####################################################################################
const char             *test_send_cat_ready()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_SEND_CAT_READY, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_send_cat_ready failed";
    }
    return 0;
}

//####################################################################################
//Function Name: test_send_uicc_ready
//Decription:
// Triggers sending of UICC Card Indication for Card Ready
//####################################################################################
const char             *test_send_uicc_ready()
{
    int rv;

    rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_SEND_UICC_READY, NULL, 0, MODEM_CT);

    if (0 != rv)
    {
        return "test_send_uicc_ready failed";
    }
    return 0;
}

test_func_t            *find_test_func(const char *name)
{
    int                     i;
    for (i = 0; tests[i].test_name; i++)
    {
        if (strcmp(name, tests[i].test_name) == 0)
        {
            return tests[i].test_func;
        }
    }
    return 0;
}


void run_tests(int nr_of_test, char **tc_name)
{
    int i;
    fd_set read_set;
    struct timeval timeout;

    printf("#\tmodem_simulator: entering run_tests\n");

    while (nr_of_test)
    {
        test_func_t            *f;
        const char             *s;

        f = find_test_func(*tc_name);
        if (!f) {
            printf("#\tmodem_simulator: No such test function: %s\n", *tc_name);
            return;
        }

        printf("# \n");
        printf("#\tstarting test %s\n", *tc_name);
        s = f();
        if (s)
        {
            printf("#\t%s failed to initiate test\n", *tc_name);
        }
        else
        {
            printf("#\t%s initiated (does not mean that the test is complete or passed)\n", *tc_name);
        }

        FD_ZERO(&read_set);
        FD_SET(socket_fd, &read_set);
        timeout.tv_sec = 60;
        timeout.tv_usec = 0;

        printf("#\twaiting for test to complete\n");
        i = select(socket_fd + 1, &read_set, 0, 0, &timeout);
        if (i > 0) {
            if (FD_ISSET(socket_fd, &read_set)) {
                char buf[1024];
                memset(buf, 0, 1024);
                int n = read(socket_fd, buf, 1023);
                if (n > 0) {
                    printf("#\ttest completed with the following data: %s\n", buf);
                } else {
                    printf("#\ttest completed, failed to receive data: %d\n", n);
                }
            }
        } else if (i == 0) {
            // Timeout
            printf("#\ttest timed out.\n");
        } else {
            // Error
            int err = errno;
            printf("Error waiting for test to complete : %d\n", err);
        }

        --nr_of_test;
        ++tc_name;
        sleep(3);
    }
}

void print_avalible_tests(int nr_of_test, char **tc_name)
{
    printf("#\tAvailable test cases:\n");

    while (nr_of_test)
    {
        test_func_t            *f;

        f = find_test_func(*tc_name);
        if (f) {
            printf("#\t %s\n", *tc_name);
        }

        --nr_of_test;
        ++tc_name;
    }
    printf("\n");
}


//#####################################################################################
//  Lib Functions
//#####################################################################################

char * sim_stub_enc(char *dst, const void *vsrc, size_t n)
{
    const char             *src = (const char *) vsrc;
    while (n)
    {
        *dst = *src;
        ++dst;
        ++src;
        --n;
    }
    return dst;
}


const char * sim_stub_dec(const char *src, void *vdst, size_t n, const char *smax)
{
    char                   *dst = (char *) vdst;
    while (src && dst && n)
    {
        if (src < smax)
        {
            *dst = *src;
            ++dst;
            ++src;
            --n;
        }
        else
        {
            return 0;
        }
    }
    return src;
}

void sim_stub_i_safe_copy(char *dst, const char *src, size_t n)
{
    strncpy(dst, src, n);
    if (n)
    {
        dst[n - 1] = '\0';
    }
}

static void *sim_stub_send_create_buffer ( uint16_t cmd, const char *payload, size_t n, uintptr_t client_tag, size_t *buffsize )
{
    char       *used_buffer = NULL;
    char       *p = NULL;
    uint16_t    len;
    size_t      bsize;

    // check payload/n consistency
    // NULL,0 is good, as is not-null,not-zero.
    if ( (payload == NULL && n != 0) || (payload != NULL && n == 0) )
    {
        return NULL;
    }

    if (n + sizeof(client_tag) + sizeof(cmd) > 0x0ffff)        /* too large! */
    {
        return NULL;
    }

    len = n + sizeof(client_tag) + sizeof(cmd);
    bsize = len + sizeof(len);

    used_buffer = malloc(bsize);
    if (!used_buffer)
    {
        return NULL;
    }

    p = used_buffer;
    p = sim_stub_enc (p, &len, sizeof(len));
    p = sim_stub_enc (p, &cmd, sizeof(cmd));
    p = sim_stub_enc (p, &client_tag, sizeof(client_tag));
    p = sim_stub_enc (p, payload, n);
    assert( bsize == (size_t)(p - used_buffer) );

    *buffsize = bsize;

    return used_buffer;
}

int sim_stub_send_generic(int fd, uint16_t cmd, const char *payload, size_t n, uintptr_t client_tag)
{
    char                   *used_buffer = NULL;
    ssize_t                 bytes_written;
    size_t                  bytes_to_write;
    int                     ret_val = -1;

    used_buffer = sim_stub_send_create_buffer( cmd, payload, n, client_tag, &bytes_to_write );

    if ( !used_buffer )
    {
        return -1;
    }

    bytes_written = write(fd, used_buffer, bytes_to_write);

    if (bytes_written == -1)
    {
        ret_val = -1;
    }
    else if ((size_t) bytes_written != bytes_to_write)
    {
        ret_val = -1;
    }
    else
    {
        /* Success */
        ret_val = 0;
    }

    free(used_buffer);

    return ret_val;
}

static int modem_simulator_connect(const char *path)
{
    struct sockaddr_un      addr;
    int                     i;
    int                     fd;

    printf("modem_simulator: connect to stub socket thread\n");
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    sim_stub_i_safe_copy(addr.sun_path, path, sizeof(addr.sun_path));

    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0)
    {
        printf("modem_simulator: failed to create socket\n");
        return CLOSED_FD;
    }

    i = connect(fd, (struct sockaddr *) &addr, sizeof(addr));
    if (i < 0)
    {
        printf("modem_simulator: failed to connect\n");
        close(fd);
        return CLOSED_FD;
    }

    i = fcntl(fd, F_SETFL, O_NONBLOCK);
    if (i < 0)
    {
        printf("modem_simulator: failed to set non-blocking\n");
        return CLOSED_FD;
    }

    return fd;
}

static void modem_simulator_disconnect()
{
    printf("modem_simulator: disconnect from stub socket thread\n");
    if (socket_fd != -1)
    {
        int rv;

        rv = sim_stub_send_generic(socket_fd, SIM_MODEM_CMD_MODEM_DISCONNECT, NULL, 0, MODEM_CT);
        if (0 != rv)
        {
            return;
        }
        close(socket_fd);
        socket_fd = -1;
    }
}

void modem_main_loop(int argc, const char **argv)
{
    char     typed_str[100];
    int      rv;
    char    *tc_name_p = typed_str;

    printf("MODEM SIMULATOR: ENTER MAIN LOOP\n");
    if ( argc == 2 && strcmp(argv[1],"all") == 0 ) {
        int  n = sizeof(all_tests) / sizeof(char *);
        run_tests(n, all_tests);
    }
    else
    for (;;)
    {
        printf("#-------------------------------------------------------------------\n");
        printf("#\tEnter help for available tests cases!\n\n");
        printf("#\tEnter your test name: ");
        rv = scanf ("%s", tc_name_p);
        if ( rv == 0 || rv == EOF ) break;
        if (0 == strncmp("exit", tc_name_p, 4))
        {
            goto MODEM_DONE;
        }
        else if (0 == strncmp("all", tc_name_p, 3))
        {                /* Do all */
            int  n = sizeof(all_tests) / sizeof(char *);
            run_tests(n, all_tests);
        }
        else if (0 == strncmp("help", tc_name_p, 4))
        {                /* Print name of available test cases */
            int  n = sizeof(all_tests) / sizeof(char *);
            print_avalible_tests(n, all_tests);
        }
        else
        {
            run_tests(1, (char**)&tc_name_p);
        }
    }

MODEM_DONE:
    printf("MODEM SIMULATOR: EXIT MAIN LOOP\n");
}



void modem_simulator(int argc, const char **argv)
{
    if (signal((int)SIGPIPE, SIG_IGN) == SIG_ERR) {
        printf("modem_simulator : SIG_ERR when ignoring SIGPIPE\n");
        abort();
    }

    socket_fd = modem_simulator_connect(socket_name);
    if (socket_fd == -1)
    {
        printf("modem_simulator : connect to server failed\n");
        return;
    }
    modem_main_loop(argc,argv);
    //disconnect
    modem_simulator_disconnect();
}



// -----------------------------------------------------------------------------


int main(int argc, const char **argv)
{
    printf("====================================================================\n");
    setProcName(argv[0]);
    setbuf(stdout, 0);

    if (argc > 2)
    {
        exit(EXIT_FAILURE);
    }
    modem_simulator(argc,argv);

    printf("====================================================================\n");
    exit(EXIT_SUCCESS);
}




