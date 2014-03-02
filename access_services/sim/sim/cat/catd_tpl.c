/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : catd_tpl.c
 * Description     : Terminal Profile download implementation source
 *
 * Author          : Jessica Nilsson <jessica.j.nilsson@stericsson.com>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdarg.h>
#include <assert.h>
#include <dirent.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "catd.h"
#include "catd_tpl.h"
#include "apdu.h"
#include "simd.h"
#include "catd_modem.h"

#define SAT_TERMINAL_PROFILE_FILE_HEADER "# Terminal Profile used by SIM\n\n"

//STRINGIZE is a standard macro for converting macro parameter into a string constant.
//The STRINGIZE_VALUE_OF will evaluate down to the final definition of a macro.
#define STRINGIZE(x) #x
#define STRINGIZE_VALUE_OF(x) STRINGIZE(x)
#define TERMINAL_PROFILE STRINGIZE_VALUE_OF(SET_TERMINAL_PROFILE)
#define ADAPT_TERMINAL_SUPPORT_TABLE STRINGIZE_VALUE_OF(SET_ADAPT_TERMINAL_SUPPORT_TABLE)

// The default terminal profile
catd_terminal_profile_download_t DEFAULT_TPDL;

// The default Adapt terminal support table file
const catd_adapt_terminal_support_table_t  DEFAULT_ADAPT_TERMINAL_SUPPORT_TABLE = {
    ADAPT_CFG_SCAN_CONTROL_APP_SUPPORTED,
    ADAPT_CFG_NETWORK_SELECTION_APP_SUPPORTED,
    ADAPT_CFG_ACTING_HPLMN_SUPPORTED,
    ADAPT_CFG_3G_SUPPORTED,
    ADAPT_CFG_RAT_BALANCING_SUPPORTED,
    ADAPT_CFG_BIP_UNSUPPORTED,
    ADAPT_CFG_LTE_UNSUPPORTED,
    ADAPT_CFG_RFU_NULL
};

static catd_terminal_profile_download_t *catd_main_load_tpdl() {
    FILE *pFile;
    char line[80];
    pFile = fopen(SAT_TERMINAL_PROFILE_PATH, "r");
    uint8_t values_found = 0;
    uint8_t *temp;
    unsigned int t = 0;
    catd_terminal_profile_download_t *tpdl_p = malloc(sizeof(catd_terminal_profile_download_t));

    if (pFile != NULL) {
        /* Initialise Terminal Profile */
        memset(tpdl_p, 0, sizeof(catd_terminal_profile_download_t));
        temp = (uint8_t *)tpdl_p;
        while (values_found < sizeof(catd_terminal_profile_download_t)) {
            if (fgets(line, 80, pFile) == NULL) {
                //EOF reached
                break;
            }

            if (sscanf(line, "%x", &t) == 1) {
                *temp++ = t;
                values_found++;
            }
        }
        fclose(pFile);

        if (values_found == 0) { //TODO add sanity check of >xx values??
            free(tpdl_p);
            return NULL;
        }

        return tpdl_p;

    } else {
        free(tpdl_p);
        return NULL;
    }
}

void catd_main_store_terminal_profile(uint8_t *profile_p, uint8_t profile_len)
{
    DIR *pDir;
    FILE *pFile;
    uint8_t *temp = (uint8_t *)profile_p;
    uint8_t i;

    if (profile_p == NULL) {
        catd_log_f(SIM_LOGGING_E, "No terminal profile to store");
        return;
    }

    //Check if path exists
    //TODO Smarter way??
    pDir = opendir(SAT_CONFIG_DIR);
    if (pDir == NULL) {
        // Assume /etc directory exists and we are root
        mkdir(SAT_CONFIG_DIR, (S_IRWXU|S_IRWXG|S_IRWXO));
    } else {
        closedir(pDir);
    }

    pFile = fopen(SAT_TERMINAL_PROFILE_PATH, "w");
    if (pFile != NULL) {
        fprintf(pFile, "%s\n", SAT_TERMINAL_PROFILE_FILE_HEADER);
        for (i = 0; i < profile_len; i++)
        {
            fprintf(pFile, "%x\n", *temp++);
        }
        fclose(pFile);
        catd_log_b(SIM_LOGGING_D, "tpdl : Stored terminal profile = ", profile_p, profile_len);
    }
}

//Internal method for copy the defined TERMINAL_PROFILE into the DEFAULT_TPDL
static void copy_tpdl() {

    int i;
    unsigned char value = 0; //The actual value that going to be copied
    char temp_byte[2]; //Used to temporary store byte by byte from TERMINAL_PROFILE
    unsigned char* default_tpdl_prt = (unsigned char*)&DEFAULT_TPDL; //Points to the memory for DEFAULT_TPDL
    memset(&DEFAULT_TPDL, 0, sizeof(DEFAULT_TPDL)); //memset for byte by byte

    catd_log_f(SIM_LOGGING_D, "catd : ENTER %s",__PRETTY_FUNCTION__);

    //Go through the TERMINAL_PROFILE and copy byte for byte
    for(i=0; i<(int)strlen(TERMINAL_PROFILE); i+=2) {
        memset(temp_byte, 0, sizeof(temp_byte));
        temp_byte[0] = TERMINAL_PROFILE[i];
        temp_byte[1] = TERMINAL_PROFILE[i+1];
        value = strtol(temp_byte, NULL, 16);
        *default_tpdl_prt = (unsigned char) value;
        default_tpdl_prt++;
    }

    //Now check if SAT_BIP_SUPPORT, SAT_PLI_LANGUAGE_SUPPORT or/and SAT_EVENT_LANGUAGE_SUPPORT
    //are set in the cat/Android.mk, if so then override the specific bits to 1 if not already
    //set by the customer.
#ifdef SAT_BIP_SUPPORT
    DEFAULT_TPDL.sixthbyte_event_driven_info_ext |= BIT_CFG_SAT_EVENT_DATA_AVAILABLE_SUPPORT;
    DEFAULT_TPDL.sixthbyte_event_driven_info_ext |= BIT_CFG_SAT_EVENT_CHANNEL_STATUS_SUPPORT;
    DEFAULT_TPDL.twelfthbyte |= BIT_CFG_SAT_CMD_OPEN_CHANNEL_SUPPORT;
    DEFAULT_TPDL.twelfthbyte |= BIT_CFG_SAT_CMD_CLOSE_CHANNEL_SUPPORT;
    DEFAULT_TPDL.twelfthbyte |= BIT_CFG_SAT_CMD_RECEIVE_DATA_SUPPORT;
    DEFAULT_TPDL.twelfthbyte |= BIT_CFG_SAT_CMD_SEND_DATA_SUPPORT;
    DEFAULT_TPDL.twelfthbyte |= BIT_CFG_SAT_CMD_GET_CHANNEL_STATUS_SUPPORT;
    DEFAULT_TPDL.thirteenthbyte |= MAX_CFG_SAT_BEARER_CHANNELS_SUPPORT_NUMBER;
    DEFAULT_TPDL.seventeenthbyte |= BIT_CFG_SAT_TCP_CLIENT_REMOTE_SUPPORT;
    DEFAULT_TPDL.seventeenthbyte |= BIT_CFG_SAT_UDP_CLIENT_REMOTE_SUPPORT;
    DEFAULT_TPDL.seventeenthbyte |= BIT_CFG_SAT_TCP_SERVER_SUPPORT;
    DEFAULT_TPDL.seventeenthbyte |= BIT_CFG_SAT_TCP_CLIENT_LOCAL_SUPPORT;
    DEFAULT_TPDL.seventeenthbyte |= BIT_CFG_SAT_UDP_CLIENT_LOCAL_SUPPORT;
#endif
#ifdef SAT_PLI_LANGUAGE_SUPPORT
    DEFAULT_TPDL.ninthbyte |= BIT_CFG_SAT_CMD_LOCAL_INFO_LANGUAGE_SUPPORT;
#endif
#ifdef SAT_EVENT_LANGUAGE_SUPPORT
    DEFAULT_TPDL.sixthbyte_event_driven_info_ext |= BIT_CFG_SAT_EVENT_LANGUAGE_SELECTION_SUPPORT;
#endif
    catd_log_f(SIM_LOGGING_D, "catd : EXIT %s",__PRETTY_FUNCTION__);
}

const catd_terminal_profile_download_t *catd_main_load_terminal_profile()
{
    catd_terminal_profile_download_t *persistent_tpdl = catd_main_load_tpdl();
    if (persistent_tpdl == NULL) {
        //Check so length for TERMINAL_PROFILE are the same as for DEFAULT_TPDL
        //Need to divide by 2 for TERMINAL_PROFILE cause its a string and not byte string
        if ((strlen(TERMINAL_PROFILE)/2) == sizeof(DEFAULT_TPDL)) {
            //Copy the TERMINAL_PROFILE and set the DEFAULT_TPDL
            copy_tpdl();
        } else {
            catd_log_f(SIM_LOGGING_D, "tpdl: Incorrect size of TERMINAL_PROFILE");
        }

        //Store the default profile to file, to have a "template" to edit on the file system
        catd_log_f(SIM_LOGGING_D, "tpdl: No terminal profile found, using default");
        catd_main_store_terminal_profile((uint8_t *) &DEFAULT_TPDL, sizeof(catd_terminal_profile_download_t));
        persistent_tpdl = catd_main_load_tpdl();
    }

    if (persistent_tpdl == NULL) {
        catd_log_f(SIM_LOGGING_E, "catd : unable to load terminal profile from file, using default");
        persistent_tpdl = malloc(sizeof(catd_terminal_profile_download_t));
        memset(persistent_tpdl, 0, sizeof(catd_terminal_profile_download_t));
        memcpy(persistent_tpdl, &DEFAULT_TPDL, sizeof(catd_terminal_profile_download_t));
    }

    catd_log_b(SIM_LOGGING_D, "tpdl : Loaded terminal profile = ",  persistent_tpdl, sizeof(catd_terminal_profile_download_t));

    return persistent_tpdl;
}

void catd_main_load_adapt_terminal_support_table(catd_adapt_terminal_support_table_t *table_p)
{
    catd_log_f(SIM_LOGGING_D, "catd : ENTER %s",__PRETTY_FUNCTION__);

    if (!table_p) {
        return;
    }

    if (!strlen(ADAPT_TERMINAL_SUPPORT_TABLE)) {
        memcpy(table_p, &DEFAULT_ADAPT_TERMINAL_SUPPORT_TABLE, sizeof(catd_adapt_terminal_support_table_t ));
    } else {
        catd_log_f(SIM_LOGGING_D, "catd : load adapt_terminal_support_table from Board Cfg");

        int i;
        unsigned int value = 0; //The actual value that going to be copied
        char temp_byte[2]; //Used to temporary store byte by byte from TERMINAL_SUPPORT_TABLE
        char* default_tst_prt = (char*)table_p; //Points to the memory for Deafult TST
        memset(table_p, 0, sizeof(catd_adapt_terminal_support_table_t)); //memset for byte by byte

        //Go through the ADAPT_TERMINAL_SUPPORT_TABLE and copy byte for byte
        for(i=0; i<(int)strlen(ADAPT_TERMINAL_SUPPORT_TABLE); i+=2) {
            memset(temp_byte, 0, sizeof(temp_byte));
            temp_byte[0] = ADAPT_TERMINAL_SUPPORT_TABLE[i];
            temp_byte[1] = ADAPT_TERMINAL_SUPPORT_TABLE[i+1];
            value = strtol(temp_byte, NULL, 16);
            *default_tst_prt = (unsigned char) value;
            default_tst_prt++;
        }
    }
    catd_log_f(SIM_LOGGING_D, "catd : EXIT %s",__PRETTY_FUNCTION__);

}

