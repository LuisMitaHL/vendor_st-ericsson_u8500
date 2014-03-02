/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "admRequest.h"
#include <stdlib.h>
#include "ste_adm_client.h"
AdmRequest::AdmRequest()
{
    tagBeforeUpdateDbtString = "<UpdateDatabase ";
    tagAfterUpdateDbtString = "/>";
    tagBeforeUpdateEffectStringConfig = "<SetConfig ";
    tagAfterUpdateEffectStringConfig = "/>";
    tagBeforeUpdateEffectStringParam = "<SetParam ";
    tagAfterUpdateEffectStringParam = "/>" ;
    tagBeforeUpdateVoiceChain = "<VoiceChain ";
    tagAfterUpdateVoiceChain = "/>" ;
    tagBeforePingRequest = "<PingRequest ";
    tagAfterPingRequest = "/>";
    tagBeforeReopenDB = "<ReopenDB ";
    tagAfterReopenDB = "/>";
}

void AdmRequest::run_adm_rescan_config_db(char *rcvText)
{
    ALOGD("AdmRequest::run_adm_rescan_config_db\n");

    if (rcvText != NULL) {
        /* The receive text is in the form
         * BegintagDb + updateDbString + endTagDb + begintagEffect + updateEffectString +endtagEffect
         * extract updateEffectString  and call adm function for updating the running effect with new values
         */
        int lengthOfString = (strlen(rcvText) + 1);
        //This text point to begintagEffect
        char *beginTag = strstr(rcvText, tagBeforeUpdateEffectStringConfig);
        if(beginTag == NULL) {
            //printf("sequence for finding updating effect config not founded \n");
            return;
        }
        // more than enough, will be released at the end of this function
        char *updateEffectStringConfig = (char *) malloc(lengthOfString);
        memset(updateEffectStringConfig, 0, lengthOfString);

        //this text point to updateEffectString
        char *pointToDesiredText = &beginTag[strlen(tagBeforeUpdateEffectStringConfig)];
        memcpy(updateEffectStringConfig, pointToDesiredText, strlen(pointToDesiredText) - strlen(tagAfterUpdateEffectStringConfig));
        //here we should call the appropriate adm function, for now print text for test purposes
        //printf("%s\n", updateEffectString);
        char *token;
        // this pointer keeps the elements needed to send to ste_adm_rescan_config function
        char *elements[3];
        int k = 0;
        // split the updateEffectString into tokens
        token = strtok(updateEffectStringConfig, " ");
        elements[k] = token;
        k++;

        while (token != NULL && k < 3) {
            token = strtok(NULL, " ");

            if (token != NULL) {
                elements[k] = token;
                k++;
            }
        }

        ste_adm_rescan_config(elements[0], (ste_adm_effect_chain_type_t) atoi(elements[1]), elements[2]);

        free(updateEffectStringConfig);
    } else {
        ALOGE("Wrong format on AdmRequest::run_adm_rescan_config_db will exit \n");
        exit(EXIT_FAILURE);
    }
}

void AdmRequest::run_adm_update_db(char *rcvText)
{
    ALOGD("AdmRequest::run_adm_update_db called \n");

    if (rcvText != NULL) {
        /* The receive text is in the form
         * BegintagDb + updateDbString + endTagDb + begintagEffect + updateEffectString +endtagEffect
         * extract updateDbString  and call adm function for updating db
         */
        int lengthOfBeginTag = strlen(tagBeforeUpdateDbtString);
        int lengthOfEndTag = strlen(tagAfterUpdateDbtString);
        int nextPosToPoint = 0;
        char *nextPlaceToPoint = NULL;
        int pos = 0;
        // check if rcvText contains BegintagDb if not it doesn't contain a sql statement, terminate then
        char *findBeginTag = NULL ;
        char *findendTag = NULL ;

        findBeginTag = strstr(rcvText, tagBeforeUpdateDbtString) ;

        if (findBeginTag != NULL) {
            findendTag = strstr(findBeginTag, tagAfterUpdateDbtString) ;
        }

        if (findBeginTag == NULL || findendTag == NULL) {
            //printf("no update sql statement \n");
            return;
        }

        // ok we found begin tag
        char *sqlStatement = NULL;
        int length = strlen(rcvText) + 1;
        sqlStatement = (char *) malloc(length);

        while (findBeginTag != NULL && findendTag != NULL) {
            // allocate more than enough
            memset(sqlStatement, 0, length);
            nextPlaceToPoint = &findBeginTag[strlen(tagBeforeUpdateDbtString)];

            //memcpy(sqlStatement, nextPlaceToPoint, findendTag - findBeginTag -strlen(tagBeforeUpdateDbtString));
            memcpy(sqlStatement, nextPlaceToPoint, strlen(findBeginTag) - strlen(findendTag) - strlen(tagBeforeUpdateDbtString));
            findBeginTag = strstr(nextPlaceToPoint, tagBeforeUpdateDbtString);

            if (findBeginTag != NULL) {
                findendTag = strstr(findBeginTag, tagAfterUpdateDbtString);
            }

            //printf("Sending SQL statement: %s\n", sqlStatement);
            ALOGD("Calling ste_adm_client_execute_sql: (%s)", sqlStatement);

            if (STE_ADM_RES_OK != ste_adm_client_execute_sql(sqlStatement)) {
                ALOGE("run_adm_update_db: ste_adm_client_execute_sql returned error\n");
            }
        }

        free(sqlStatement);
    } else {
        ALOGE("Wrong format on AdmRequest::run_adm_update_db will exit \n");
        exit(EXIT_FAILURE);
    }
}


void AdmRequest::run_adm_rescan_param_db(char *rcvText)
{
    ALOGD("AdmRequest::run_adm_rescan_param_db called \n");

    if (rcvText != NULL) {
        /* The receive text is in the form
         * BegintagDb + updateDbString + endTagDb + begintagEffect + updateEffectString +endtagEffect
         * extract updateEffectString  and call adm function for updating the running effect with new values
         */
        int lengthOfString = (strlen(rcvText) + 1);
        // more than enough, will be released at the end of this function
        char *updateEffectStringParam = (char *) malloc(lengthOfString);
        memset(updateEffectStringParam, 0, lengthOfString);
        // This text point to begintagEffect
        char *beginTag = strstr(rcvText, tagBeforeUpdateEffectStringParam);

        if (beginTag == NULL) {
            //printf("no rescan command sended \n");
            free(updateEffectStringParam);
            return;
        }

        //this text point to updateEffectString
        char *pointToDesiredText = &beginTag[strlen(tagBeforeUpdateEffectStringParam)];
        memcpy(updateEffectStringParam, pointToDesiredText, strlen(pointToDesiredText) - strlen(tagAfterUpdateEffectStringParam));

        //   printf("%s\n", updateEffectString);

        ste_adm_reload_device_settings();

        free(updateEffectStringParam);
    } else {
        ALOGE("Wrong format on AdmRequest::run_adm_rescan_param_db will exit \n");
        exit(EXIT_FAILURE);
    }
}

void AdmRequest::run_adm_rescan_voice_chain(char *rcvText)
{
    ALOGD("run_adm_rescan_voice_chain called \n");

    if (rcvText != NULL) {
        /* The receive text is in the form
         * BegintagDb + updateDbString + endTagDb + begintagEffect + updateEffectString +endtagEffect
         * extract updateDbString  and call adm function for updating db
         */
        //allocate more than enough
        char *updateDbString = (char *) malloc(strlen(rcvText) + 1);
        memset(updateDbString, 0, strlen(rcvText) + 1);
        //this array will point to the beginning of updateDbString until the end
        char *beginDbString = &rcvText[strlen(tagBeforeUpdateVoiceChain)];

        if (strstr(rcvText, tagBeforeUpdateVoiceChain) == NULL) {
            //printf("No voice chain update \n");
            free(updateDbString);
            return;
        }

        // this array will point to the beginning of endTagDb
        char *endTagDb = strstr(rcvText, tagAfterUpdateVoiceChain);

        if (endTagDb == NULL) {
            //printf("No voice chain update \n");
            free(updateDbString);
            return;
        }

        memcpy(updateDbString, beginDbString, strlen(beginDbString) - strlen(endTagDb));
        ste_adm_rescan_speechproc();
        free(updateDbString);
    } else {
        ALOGE("Wrong format on AdmRequest::run_adm_rescan_voice_chain will exit \n");
        exit(EXIT_FAILURE);
    }
}

char *AdmRequest::run_scan_ping_request(char *rcvText)
{
    char *res = NULL;

    if (NULL != rcvText) {
        char *startTag = strstr(rcvText, tagBeforePingRequest);
        char *endTag;

        if (startTag && (endTag = strstr(startTag, tagAfterPingRequest)) != NULL) {
            int len = endTag - startTag - strlen(tagBeforePingRequest) + 1;
            res = (char *) malloc(len * sizeof(char));
            memcpy(res, startTag + strlen(tagBeforePingRequest), len - 1);
            res[len - 1] = '\0';
        }
    }

    return res;
}

int AdmRequest::run_scan_reopen_db(char *rcvText, int *admRes)
{
    int res = 0;
    ALOGD("AdmRequest::run_scan_reopen_db\n");

    if (NULL != rcvText) {
        char *startTag = strstr(rcvText, tagBeforeReopenDB);
        if (startTag != NULL) {
            char *endTag = strstr(startTag, tagAfterReopenDB);
            if (startTag && endTag)  {
                ALOGI("Reopening DB\n");
                *admRes = ste_adm_reopen_db();
                res = 1;
            }
        }
    }

    return res;

}

void AdmRequest::checkForResidualText(char *buf, char **resText)
{
    if (resText == NULL) {
        return;
    }

    if (*resText != NULL) {
        free(*resText);
        *resText = NULL;
    }

    char *endTag = strstr(buf, "/>");
    char *dummy = buf;

    while (NULL != (dummy = strstr(dummy + 1, "/>"))) {
        endTag = dummy;
    }

    if (endTag != NULL) {
        if (strlen(endTag) > 2) {
            *resText = (char *) malloc(strlen(endTag) - 2 + 1);
            strcpy(*resText, endTag + 2);
        }
    } else {
        // No end tag found. Save whole message!
        *resText = (char *) malloc(strlen(buf) + 1);
        strcpy(*resText, buf);
    }

    if (*resText != NULL) {
        // Clear new-line characters
        char *tmp = *resText;

        while (*tmp != 0) {
            if (*tmp == '\n') {
                *tmp = ' ';
            }

            tmp++;
        }

        if (strlen(*resText) <= 10) {
            ALOGD("Found residual text: '%s'\n", *resText);
        } else {
            ALOGD("Found residual text. Size = %d chars \n", strlen(*resText));
        }
    } else {
        ALOGD("Found no residual text\n");
    }
}

