/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef ADMREQUEST_H
#define ADMREQUEST_H
#include <stdio.h>
#include <string.h>
#include "tuningServer_log.h"

class AdmRequest
{

    /**
     * Constructor for the class
     */
public:
    AdmRequest();

    /**
     * This function updates the db
     * @param str String wich contains a correct SQL-statement in order to update the functionality
     */
    void run_adm_update_db(char *ch);

    /**
     * This method get the needed parameters needed to send to adm_set_config
     * from the sql statements we get from client
     */
    void run_adm_rescan_config_db(char *ch);

    /**
     * This method get the needed parameters needed to send to adm_set_param
     * from the sql statements we get from client
     */

    void run_adm_rescan_param_db(char *ch);

    /**
    * This method rescan the voice
    * cain if needed
    * @param     *ch the string we get from client
    */

    void run_adm_rescan_voice_chain(char *ch);

    /**
    * Scan buffer for ping request tag. Return string between start and end tag if found. NULL otherwise
    *
    **/
    char *run_scan_ping_request(char *ch);

    /**
    * Scan for requests to open/close adm.sqlite DB used by ADM
    */
    int run_scan_reopen_db(char *ch, int *admResult);

    /**
    * This method checks 'buf' for unterminates tags. Return an allocated buffer 'resText' containing it
    * If *resText is not null it will be freed first and reallocated.
    **/
    void checkForResidualText(char *buf, char **resText);


public:
    const char *tagBeforeUpdateDbtString;
    const char *tagAfterUpdateDbtString ;
    const char *tagBeforeUpdateEffectStringConfig;
    const char *tagAfterUpdateEffectStringConfig;
    const char *tagBeforeUpdateEffectStringParam;
    const char *tagAfterUpdateEffectStringParam;
    const char *tagBeforeUpdateVoiceChain;
    const char *tagAfterUpdateVoiceChain;
    const char *tagBeforePingRequest;
    const char *tagAfterPingRequest;
    const char *tagBeforeReopenDB;
    const char *tagAfterReopenDB;

private:
};
#endif
