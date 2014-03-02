/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : apdu_test.c
 * Description     : test program for apdu
 *
 * Author          : Mats Bergstrom <mats.m.bergstrom@stericsson.com>
 *
 */

// FIXME: Change names to conform to linux rules.

#include "apdu.h"
#include "func_trace.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>
#include <string.h>

//##############################################################
//function declaration
//##############################################################
static void             printHexCharString(const unsigned char
                                           *SourceBuf_p, uint8_t length);
const char             *SAT_APDU_TC_HandleDisplayText();
const char             *SAT_APDU_TC_HandleProvideIMEI();
const char             *SAT_APDU_TC_HandleSetupEventList();
const char             *SAT_APDU_TC_HandleSetupMenu();
const char             *SAT_APDU_TC_HandleMenuSelection();
const char             *SAT_APDU_TC_HandleGetInkey();
const char             *SAT_APDU_TC_HandleGetInput();
const char             *SAT_APDU_TC_HandleMoreTime();
const char             *SAT_APDU_TC_HandlePollingOff();
const char             *SAT_APDU_TC_HandlePollInterval();
const char             *SAT_APDU_TC_HandleTimerManagement();
const char             *SAT_APDU_TC_HandleSendUssd();
const char             *SAT_APDU_TC_HandleSendSS();
const char             *SAT_APDU_TC_HandleSetupCall();
const char             *SAT_APDU_TC_HandleTimerManagement();
const char             *SAT_APDU_TC_HandleRefresh();
const char             *SAT_APDU_TC_HandleSendDTMF();
const char             *SAT_APDU_TC_HandleSendSMS();
const char             *SAT_APDU_TC_HandleLanguageNotification();
const char             *SAT_APDU_TC_HandleWrongAPDU();
void                    run_tests(int argc, const char *argv[]);

//##############################################################
//function definition
//##############################################################

/*************************************************************************
 * Function:      printHexCharString
 *
 * Description:
 *
 *************************************************************************/
static void printHexCharString(const unsigned char *SourceBuf_p,
                               uint8_t StrLength)
{
    unsigned char          *TempBuf_p;
    unsigned char          *dest_p;
    int                     i;

    TempBuf_p = malloc(StrLength * 2 + 1);
    memset(TempBuf_p, 0, StrLength * 2 + 1);
    dest_p = TempBuf_p;

    for (i = 0; i < StrLength; i++) {
        sprintf((char *) TempBuf_p, "%02x", *SourceBuf_p);
        SourceBuf_p++;
        TempBuf_p++;
        TempBuf_p++;
    }
    printf("%s \n", dest_p);
    free(dest_p);
}


static void printHexAPDU(const ste_apdu_t * apdu)
{
    printHexCharString(ste_apdu_get_raw(apdu),
                       ste_apdu_get_raw_length(apdu));
}


/*************************************************************************
 * Function:      SAT_APDU_TC_HandleDisplayText
 *
 * Description:
 *
 *************************************************************************/
const char             *SAT_APDU_TC_HandleDisplayText()
{
    unsigned char           APDU_DisplayText_01[] = {
        0xD0, // Proactive Command
        0x1E, // Length
        0x81, 0x03, 0x01, 0x21, 0x00, // Command Details
        0x82, 0x02, 0x81, 0x02, // Device Id
        0x8D, 0x04, 0x04, 0x43, 0x41, 0x54, // Text String
        0x1E, 0x02, 0x01, 0x01,  //Icon id 1
        0x50, 0x04, 0x01, 0x02, 0x03, 0x04,  //Text Attribute 1
        0x2B,0x00, // Immediate response
        0x68, 0x01, 0x01,  //Frame id
        0x84, 0x02, 0x00, 0x01 // Duration in Min
    };
    unsigned char           APDU_DisplayText_02[] = {
        0xD0, // Proactive Command
        0x13, // Length
        0x81, 0x03, 0x01, 0x21, 0x00, // Command Details
        0x82, 0x02, 0x81, 0x02, // Device Id
        0x8D, 0x04, 0x04, 0x43, 0x41, 0x54, // Text String
        0x84, 0x02, 0x01, 0x50 // Duration in Sec
    };
    unsigned char           APDU_DisplayText_03[] = {
        0xD0, // Proactive Command
        0x13, // Length
        0x81, 0x03, 0x01, 0x21, 0x00, // Command Details
        0x82, 0x02, 0x81, 0x02, // Device Id
        0x8D, 0x04, 0x04, 0x43, 0x41, 0x54, // Text String
        0x84, 0x02, 0x02, 0x80 // Duration in Tenths of Sec
    };
    unsigned char           APDU_DisplayText_04[] = {
        0xD0, // Proactive Command
        0x15, // Length
        0x81, 0x03, 0x01, 0x21, 0x00, // Command Details
        0x82, 0x02, 0x81, 0x02, // Device Id
        0x8D, 0x04, 0x04, 0x43, 0x41, 0x54, // Text String
        0xC8, 0x04, 0x01, 0x02, 0x03, 0x04 // Unknown Tag ( CDMA SMS )
    };

    ste_parsed_apdu_t      *parsed_apdu_info_p = NULL;
    int                     APDU_Kind = STE_APDU_KIND_NONE;
    int                     APDU_Type = STE_APDU_CMD_TYPE_NONE;
    int                     current_apdu = 0;
    ste_command_result_t    ResponseData;
    unsigned char           AdditionalInfo[20] = "Additional Info OK.";

    ste_apdu_t             *apdu;
    ste_apdu_t             *unparsed_apdu;
    ste_apdu_t             *tr_apdu;

start_loop:
    switch( current_apdu ) {
        case 0:
            apdu = ste_apdu_new(APDU_DisplayText_01, sizeof(APDU_DisplayText_01));
            break;
        case 1:
            apdu = ste_apdu_new(APDU_DisplayText_02, sizeof(APDU_DisplayText_02));
            break;
        case 2:
            apdu = ste_apdu_new(APDU_DisplayText_03, sizeof(APDU_DisplayText_03));
            break;
        case 3:
            apdu = ste_apdu_new(APDU_DisplayText_04, sizeof(APDU_DisplayText_04));
            break;

        default:
            goto End;
    }
    current_apdu++;
    if (!apdu)
        return "ste_apdu_new failed";

    printf("\n");
    printf
        ("############ Test SAT_APDU_TC_HandleDisplayText -%02d ############\n", current_apdu);
    //Parse an APDU string
    if (STE_SAT_APDU_ERROR_NONE !=
        ste_apdu_parse(apdu, &parsed_apdu_info_p)) {
        printf("FAILED : TC Display Text: Parse APDU failed.\n");
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        return "ste_apdu_parse failed.";
    } else {
        //print some useful information here
        printf("Result : TC Display Text: Parse APDU: OK\n");
    }

    //Get Kind
    APDU_Kind = ste_parsed_apdu_get_kind(parsed_apdu_info_p);
    if (STE_APDU_KIND_NONE == APDU_Kind) {
        printf("FAILED : TC Display Text: Get APDU Kind failed.\n");
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        return "ste_apdu_get_kind failed.";
    } else {
        //print some useful information here
        printf("The Kind of APDU is: 0x%x \n", APDU_Kind);
    }

    //Get Type
    APDU_Type = ste_parsed_apdu_get_type(parsed_apdu_info_p);
    if (STE_APDU_CMD_TYPE_NONE == APDU_Type) {
        printf("FAILED : TC Display Text: Get APDU Type failed.\n");
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        return "ste_parsed_apdu_get_type failed.";
    } else {
        //print some useful information here
        printf("The Type of APDU is: 0x%x \n", APDU_Type);
    }


    //Create a new APDU string
    if (STE_SAT_APDU_ERROR_NONE !=
        ste_apdu_unparse(parsed_apdu_info_p, &unparsed_apdu)) {
        printf("FAILED : TC Display Text: Create APDU failed.\n");
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        return "ste_apdu_unparse failed.";
    } else {
        //print some useful information here
        printf("Created Result APDU: ");
        printHexAPDU(unparsed_apdu);
        printf("Length of APDU binary data: %zd \n",
               ste_apdu_get_raw_length(unparsed_apdu));
    }

    //terminal response
    ResponseData.general_result = SAT_RES_CMD_PERFORMED_SUCCESSFULLY;
    ResponseData.command_status = STE_SAT_PC_CMD_STATUS_COMPLETED;
    ResponseData.additional_info_size = 20;
    ResponseData.additional_info_p = AdditionalInfo;
    ResponseData.other_data_type = STE_CMD_RESULT_NOTHING;
    ResponseData.other_data.imei_p = NULL;

    if (STE_SAT_APDU_ERROR_NONE !=
        ste_apdu_terminal_response(parsed_apdu_info_p, &ResponseData,
                                   &tr_apdu)) {
        printf("FAILED : TC Display Text: Create response APDU failed.\n");
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        return "ste_apdu_terminal_response failed.";
    } else {
        //print some useful information here
        printf("Created Response APDU: ");
        printHexAPDU(tr_apdu);
        printf("Length of Response APDU binary data: %zd \n",
               ste_apdu_get_raw_length(tr_apdu));
    }

    //delete the info structure
    ste_parsed_apdu_delete(parsed_apdu_info_p);
    parsed_apdu_info_p = NULL;

    ste_apdu_delete(apdu);
    ste_apdu_delete(unparsed_apdu);
    ste_apdu_delete(tr_apdu);
    goto start_loop;

End:
    return 0;
}

/*************************************************************************
 * Function:      SAT_APDU_TC_HandleMenuSelection
 *
 * Description:
 *
 *************************************************************************/
const char             *SAT_APDU_TC_HandleMenuSelection()
{
    unsigned char           APDU_MenuSelection[] =
        { 0xD3, 0x07, 0x82, 0x02, 0x01, 0x81, 0x90, 0x01, 0x12
    };
    ste_parsed_apdu_t      *parsed_apdu_info_p;
    int                     APDU_Kind = STE_APDU_KIND_NONE;
    ste_apdu_t             *apdu;

    apdu = ste_apdu_new(APDU_MenuSelection, sizeof(APDU_MenuSelection));
    if (!apdu)
        return "ste_apdu_new failed";

    printf("\n");
    printf
        ("############ Test SAT_APDU_TC_HandleMenuSelection ############\n");
    //Parse an APDU string
    if (STE_SAT_APDU_ERROR_NONE !=
        ste_apdu_parse(apdu, &parsed_apdu_info_p)) {
        printf("FAILED : TC Menu Selection: Parse APDU failed.\n");
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        ste_apdu_delete(apdu);
        return "ste_apdu_parse failed.";
    } else {
        //print some useful information here
        printf("Result : TC Menu Selection: Parse APDU: OK\n");
    }

    //Get Kind
    APDU_Kind = ste_parsed_apdu_get_kind(parsed_apdu_info_p);
    if (STE_APDU_KIND_NONE == APDU_Kind) {
        printf("FAILED : TC Menu Selection: Get APDU Kind failed.\n");
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        ste_apdu_delete(apdu);
        return "ste_apdu_get_kind failed.";
    } else {
        //print some useful information here
        printf("The Kind of APDU is: 0x%x \n", APDU_Kind);
    }

    //delete the info structure
    ste_parsed_apdu_delete(parsed_apdu_info_p);

    ste_apdu_delete(apdu);

    return 0;
}

/*************************************************************************
 * Function:      SAT_APDU_TC_HandleProvideIMEI
 *
 * Description:
 *
 *************************************************************************/
const char             *SAT_APDU_TC_HandleProvideIMEI()
{
    unsigned char           APDU_ProvideIMEI_01[] = {
        0xD0, // Proactive Command
        0x09, // Len
        0x81, 0x03, 0x01, 0x26, 0x01, // Command Details
        0x82, 0x02, 0x81, 0x82 // Device ID
    };
    unsigned char           APDU_ProvideIMEI_02[] = {
        0xD0, // Proactive Command
        0x0f, // Len
        0x81, 0x03, 0x01, 0x26, 0x01, // Command Details
        0x82, 0x02, 0x81, 0x82, // Device ID
        0x8D, 0x04, 0x04, 0x43, 0x41, 0x54 // Unknown Tag ( CDMA SMS )
    };
    ste_parsed_apdu_t      *parsed_apdu_info_p = NULL;
    int                     APDU_Kind = STE_APDU_KIND_NONE;
    int                     APDU_Type = STE_APDU_CMD_TYPE_NONE;
    int                     current_apdu = 0;
    ste_command_result_t    ResponseData;
    unsigned char           AdditionalInfo[20] = "Additional Info OK.";
    static IMEI_t           DefaultIMEI = {
        0x0, 0x9, 0x9, {0x9, 0x9, 0x9, 0x0, 0x0,
                        0x1, 0x2, 0x3, 0x4, 0x5,
                        0x6, 0x0, 0x0, 0x0, 0x0}
    };
    ste_apdu_t             *apdu;
    ste_apdu_t             *unparsed_apdu;
    ste_apdu_t             *tr_apdu;

start_loop:
    switch(current_apdu) {
        case 0:
            apdu = ste_apdu_new(APDU_ProvideIMEI_01, sizeof(APDU_ProvideIMEI_01));
            break;
        case 1:
            apdu = ste_apdu_new(APDU_ProvideIMEI_02, sizeof(APDU_ProvideIMEI_02));
            break;
        default: goto End;
    }
    if (!apdu)
        return "ste_apdu_new failed";

    current_apdu++;
    printf("\n");
    printf
        ("############ Test SAT_APDU_TC_HandleProvideIMEI - %02d ############\n", current_apdu);

    //Parse an APDU string
    if (STE_SAT_APDU_ERROR_NONE !=
        ste_apdu_parse(apdu,
                       (ste_parsed_apdu_t **) (&parsed_apdu_info_p))) {
        printf("FAILED : TC Provide IMEI: Parse APDU failed.\n");
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        return "ste_apdu_parse failed.";
    } else {
        //print some useful information here
        printf("Result : TC Provide IMEI: Parse APDU: OK\n");
    }

    //Get Kind
    APDU_Kind = ste_parsed_apdu_get_kind(parsed_apdu_info_p);
    if (STE_APDU_KIND_NONE == APDU_Kind) {
        printf("FAILED : TC Provide IMEI: Get APDU Kind failed.\n");
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        return "ste_apdu_get_kind failed.";
    } else {
        //print some useful information here
        printf("The Kind of APDU is: 0x%x \n", APDU_Kind);
    }

    //Get Type
    APDU_Type = ste_parsed_apdu_get_type(parsed_apdu_info_p);
    if (STE_APDU_CMD_TYPE_NONE == APDU_Type) {
        printf("FAILED : TC Provide IMEI: Get APDU Type failed.\n");
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        return "ste_parsed_apdu_get_type failed.";
    } else {
        //print some useful information here
        printf("The Type of APDU is: 0x%x \n", APDU_Type);
    }

    //Create a new APDU string
    if (STE_SAT_APDU_ERROR_NONE !=
        ste_apdu_unparse(parsed_apdu_info_p, &unparsed_apdu)) {
        printf("FAILED : TC Provide IMEI: Create APDU failed.\n");
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        return "ste_apdu_unparse failed.";
    } else {
        //print some useful information here
        printf("Created Result APDU: ");
        printHexAPDU(unparsed_apdu);
        printf("Length of APDU binary data: %zd \n",
               ste_apdu_get_raw_length(unparsed_apdu));
    }

    //terminal response
    ResponseData.general_result = SAT_RES_CMD_PERFORMED_SUCCESSFULLY;
    ResponseData.command_status = STE_SAT_PC_CMD_STATUS_COMPLETED;
    ResponseData.additional_info_size = 20;
    ResponseData.additional_info_p = AdditionalInfo;
    ResponseData.other_data_type = STE_CMD_RESULT_IMEI;
    ResponseData.other_data.imei_p = &DefaultIMEI;

    if (STE_SAT_APDU_ERROR_NONE !=
        ste_apdu_terminal_response(parsed_apdu_info_p, &ResponseData,
                                   &tr_apdu)) {
        printf("FAILED : TC Provide IMEI: Create response APDU failed\n");
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        return "ste_apdu_terminal_response failed.";
    } else {
        //print some useful information here
        printf("Created Response APDU: ");
        printHexAPDU(tr_apdu);
        printf("Length of Response APDU binary data: %zd \n",
               ste_apdu_get_raw_length(tr_apdu));
    }


    //delete the info structure
    ste_parsed_apdu_delete(parsed_apdu_info_p);
    parsed_apdu_info_p = NULL;

    ste_apdu_delete(apdu);
    ste_apdu_delete(unparsed_apdu);
    ste_apdu_delete(tr_apdu);
    goto start_loop;

End:
    return 0;
}

/*************************************************************************
 * Function:      SAT_APDU_TC_HandleSetupEventList
 *
 * Description:
 *
 *************************************************************************/
const char             *SAT_APDU_TC_HandleSetupEventList()
{
    unsigned char           APDU_SetupEventList_01[] = {
        0xD0, // Proactive Command
        0x11, // Length
        0x81, 0x03, 0x01, 0x05, 0x00, // Command Details
        0x82, 0x02, 0x81, 0x82, // Device Id
        0x99, 0x06, 0x00, 0x05, 0x02, 0x0F, 0x10, 0x13 // Event List
    }; //totally 6 event items
    unsigned char           APDU_SetupEventList_02[] = {
        0xD0, // Proactive Command
        0x17, // Length
        0x81, 0x03, 0x01, 0x05, 0x00, // Command Details
        0x82, 0x02, 0x81, 0x82, // Device Id
        0x99, 0x06, 0x00, 0x05, 0x02, 0x0F, 0x10, 0x13, // Event List
        0xC8, 0x04, 0x01, 0x02, 0x03, 0x04 // Unknown Tag ( CDMA SMS )
    }; //totally 6 event items
    ste_parsed_apdu_t      *parsed_apdu_info_p = NULL;
    int                     APDU_Kind = STE_APDU_KIND_NONE;
    int                     APDU_Type = STE_APDU_CMD_TYPE_NONE;
    int                     current_apdu = 0;
    ste_command_result_t    ResponseData;
    unsigned char           AdditionalInfo[20] = "Additional Info OK.";

    ste_apdu_t             *apdu;
    ste_apdu_t             *unparsed_apdu;
    ste_apdu_t             *tr_apdu;

start_loop:
    switch( current_apdu ) {
        case 0:
            apdu = ste_apdu_new(APDU_SetupEventList_01, sizeof(APDU_SetupEventList_01));
            break;
        case 1:
            apdu = ste_apdu_new(APDU_SetupEventList_02, sizeof(APDU_SetupEventList_02));
            break;
        default: goto End;
    }
    if (!apdu)
        return "ste_apdu_new failed";

    current_apdu++;
    printf("\n");
    printf
        ("############ Test SAT_APDU_TC_HandleSetupEventList - %02d ############\n", current_apdu);
    //Parse an APDU string
    if (STE_SAT_APDU_ERROR_NONE !=
        ste_apdu_parse(apdu, &parsed_apdu_info_p)) {
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        printf("FAILED : TC Set Up Event List: Parse APDU failed.\n");
        return "ste_apdu_parse failed.";
    } else {
        //print some useful information here
        printf("Result : TC Set Up Event List: Parse APDU: OK\n");
    }

    //Get Kind
    APDU_Kind = ste_parsed_apdu_get_kind(parsed_apdu_info_p);
    if (STE_APDU_KIND_NONE == APDU_Kind) {
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        printf("FAILED : TC Set Up Event List: Get APDU Kind failed.\n");
        return "ste_apdu_get_kind failed.";
    } else {
        //print some useful information here
        printf("The Kind of APDU is: 0x%x \n", APDU_Kind);
    }

    //Get Type
    APDU_Type = ste_parsed_apdu_get_type(parsed_apdu_info_p);
    if (STE_APDU_CMD_TYPE_NONE == APDU_Type) {
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        printf("FAILED : TC Set Up Event List: Get APDU Type failed.\n");
        return "ste_parsed_apdu_get_type failed.";
    } else {
        //print some useful information here
        printf("The Type of APDU is: 0x%x \n", APDU_Type);
    }

    //Create a new APDU string
    if (STE_SAT_APDU_ERROR_NONE !=
        ste_apdu_unparse(parsed_apdu_info_p, &unparsed_apdu)) {
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        printf("FAILED : TC Set Up Event List: Create APDU failed.\n");
        return "ste_apdu_unparse failed.";
    } else {
        //print some useful information here
        printf("Created Result APDU: ");
        printHexAPDU(unparsed_apdu);
        printf("Length of APDU binary data: %zd \n",
               ste_apdu_get_raw_length(unparsed_apdu));
    }

    //terminal response
    ResponseData.general_result = SAT_RES_CMD_PERFORMED_SUCCESSFULLY;
    ResponseData.command_status = STE_SAT_PC_CMD_STATUS_COMPLETED;
    ResponseData.additional_info_size = 20;
    ResponseData.additional_info_p = AdditionalInfo;
    ResponseData.other_data_type = STE_CMD_RESULT_NOTHING;
    ResponseData.other_data.imei_p = NULL;

    if (STE_SAT_APDU_ERROR_NONE !=
        ste_apdu_terminal_response(parsed_apdu_info_p, &ResponseData,
                                   &tr_apdu)) {
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        printf
            ("FAILED : TC Set Up Event List: Create response APDU failed.\n");
        return "ste_apdu_terminal_response failed.";
    } else {
        //print some useful information here
        printf("Created Response APDU: ");
        printHexAPDU(tr_apdu);
        printf("Length of Response APDU binary data: %zd \n",
               ste_apdu_get_raw_length(tr_apdu));
    }

    //delete the info structure
    ste_parsed_apdu_delete(parsed_apdu_info_p);
    parsed_apdu_info_p = NULL;

    ste_apdu_delete(apdu);
    ste_apdu_delete(unparsed_apdu);
    ste_apdu_delete(tr_apdu);
    goto start_loop;

End:
    return 0;
}

/*************************************************************************
 * Function:      SAT_APDU_TC_HandleSetupMenu
 *
 * Description:
 *
 *************************************************************************/
const char             *SAT_APDU_TC_HandleSetupMenu()
{
    unsigned char           APDU_SetUpMenu_01[] ={
          0xD0, // Proactive Command
          0x81, // Two byte Length Ind
          0x83, // Length
          0x81, 0x03, 0x01, 0x25, 0x00, // Command Details
          0x82, 0x02, 0x81, 0x02, // Device Identities
          0x85, 0x0C, 0x54, 0x6F, 0x6F, 0x6C, 0x6B, 0x69, 0x74, 0x20, 0x4D, // Alpha Id
          0x65, 0x6E, 0x75,
          0x8F, 0x07, 0x01, 0x49, 0x74, 0x65, 0x6D, 0x20, 0x31, // Item
          0x8F, 0x07, 0x02, 0x49, 0x74, 0x65, 0x6D, 0x20, 0x32, // Item
          0x8F, 0x07, 0x03, 0x49, 0x74, 0x65, 0x6D, 0x20, 0x33, // Item
          0x8F, 0x07, 0x04, 0x49, 0x74, 0x65, 0x6D, 0x20, 0x34, // Item
          0x8F, 0x07, 0x04, 0x49, 0x74, 0x65, 0x6D, 0x20, 0x35, // Item
          0x8F, 0x07, 0x04, 0x49, 0x74, 0x65, 0x6D, 0x20, 0x36, // Item
          0x18, 0x04, 0x13, 0x10, 0x15, 0x26, // Next Action Ind
          0x1E, 0x02, 0x01, 0x01, // Icon ID
          0x1f, 0x07, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, // Icon Id List
          0x50, 0x04, 0x01, 0x02, 0x03, 0x04, // Text Attribute
          0x51, 0x18, 0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04, // Item Text Attribute List
          0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04, 0x01, 0x02,
          0x03, 0x04, 0x01, 0x02, 0x03, 0x04,
          0x68, 0x01, 0x01, // Frame Id
    };
    unsigned char           APDU_SetUpMenu_02[] = {
        0xD0, // Proactive Command
        0x47, // Length
        0x81, 0x03, 0x01, 0x25, 0x00, // Command Details
        0x82, 0x02, 0x81, 0x02, // Device Identities
        0x85, 0x0C, 0x54, 0x6F, 0x6F, 0x6C, 0x6B, 0x69, 0x74, 0x20, 0x4D, 0x65, 0x6E, 0x75, // Alpha Id
        0x8F, 0x07, 0x01, 0x49, 0x74, 0x65, 0x6D, 0x20, 0x31, // Item
        0x8F, 0x07, 0x02, 0x49, 0x74, 0x65, 0x6D, 0x20, 0x32, // Item
        0x8F, 0x07, 0x03, 0x49, 0x74, 0x65, 0x6D, 0x20, 0x33, // Item
        0x8F, 0x07, 0x04, 0x49, 0x74, 0x65, 0x6D, 0x20, 0x34, // Item
        0x18, 0x04, 0x13, 0x10, 0x15, 0x26, // Next Action Ind
        0xC8, 0x04, 0x01, 0x02, 0x03, 0x04 // Unknown Tag ( CDMA SMS )
    };
    ste_parsed_apdu_t      *parsed_apdu_info_p = NULL;
    int                     APDU_Kind = STE_APDU_KIND_NONE;
    int                     APDU_Type = STE_APDU_CMD_TYPE_NONE;
    int                     current_apdu = 0;
    ste_command_result_t    ResponseData;
    unsigned char           AdditionalInfo[20] = "Additional Info OK.";

    ste_apdu_t             *apdu;
    ste_apdu_t             *unparsed_apdu;
    ste_apdu_t             *tr_apdu;

start_loop:
    switch( current_apdu ) {
        case 0:
            apdu = ste_apdu_new(APDU_SetUpMenu_01, sizeof(APDU_SetUpMenu_01));
            break;
        case 1:
            apdu = ste_apdu_new(APDU_SetUpMenu_02, sizeof(APDU_SetUpMenu_02));
            break;
        default: goto End;
    }
    if (!apdu)
        return "ste_apdu_new failed";

    current_apdu++;
    printf("\n");
    printf("############ Test SAT_APDU_TC_HandleSetupMenu -%02d ############\n", current_apdu);
    //Parse an APDU string
    if (STE_SAT_APDU_ERROR_NONE !=
        ste_apdu_parse(apdu, &parsed_apdu_info_p)) {
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        printf("FAILED : TC Setup Menu: Parse APDU failed.\n");
        return "ste_apdu_parse failed.";
    } else {
        //print some useful information here
        printf("Result : TC SetUp Menu: Parse APDU: OK\n");
    }

    //Get Kind
    APDU_Kind = ste_parsed_apdu_get_kind(parsed_apdu_info_p);
    if (STE_APDU_KIND_NONE == APDU_Kind) {
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        printf("FAILED : TC SetUp Menu: Get APDU Kind failed.\n");
        return "ste_apdu_get_kind failed.";
    } else {
        //print some useful information here
        printf("The Kind of APDU is: 0x%x \n", APDU_Kind);
    }

    //Get Type
    APDU_Type = ste_parsed_apdu_get_type(parsed_apdu_info_p);
    if (STE_APDU_CMD_TYPE_NONE == APDU_Type) {
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        printf("FAILED : TC SetUp Menu: Get APDU Type failed.\n");
        return "ste_parsed_apdu_get_type failed.";
    } else {
        //print some useful information here
        printf("The Type of APDU is: 0x%x \n", APDU_Type);
    }

    //Create a new APDU string
    if (STE_SAT_APDU_ERROR_NONE !=
        ste_apdu_unparse(parsed_apdu_info_p, &unparsed_apdu)) {
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        ste_apdu_delete(unparsed_apdu);
        printf("FAILED : TC SetUp Menu: Create APDU failed.\n");
        return "ste_apdu_unparse failed.";
    } else {
        //print some useful information here
        printf("Created Result APDU: ");
        printHexAPDU(unparsed_apdu);
        printf("Length of APDU binary data: %zd \n",
               ste_apdu_get_raw_length(unparsed_apdu));
    }

    //terminal response
    ResponseData.general_result = SAT_RES_CMD_PERFORMED_SUCCESSFULLY;
    ResponseData.command_status = STE_SAT_PC_CMD_STATUS_COMPLETED;
    ResponseData.additional_info_size = 20;
    ResponseData.additional_info_p = AdditionalInfo;
    ResponseData.other_data_type = STE_CMD_RESULT_NOTHING;
    ResponseData.other_data.imei_p = NULL;

    if (STE_SAT_APDU_ERROR_NONE !=
        ste_apdu_terminal_response(parsed_apdu_info_p, &ResponseData,
                                   &tr_apdu)) {
        printf("FAILED : TC SetUp Menu: Create response APDU failed.\n");
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        ste_apdu_delete(tr_apdu);
        ste_apdu_delete(unparsed_apdu);
        return "ste_apdu_terminal_response failed.";
    } else {
        //print some useful information here
        printf("Created Response APDU: ");
        printHexAPDU(tr_apdu);
        printf("Length of Response APDU binary data: %zd \n",
               ste_apdu_get_raw_length(tr_apdu));
    }

    //delete the info structure
    ste_parsed_apdu_delete(parsed_apdu_info_p);
    parsed_apdu_info_p = NULL;

    ste_apdu_delete(apdu);
    ste_apdu_delete(unparsed_apdu);
    ste_apdu_delete(tr_apdu);
    goto start_loop;

End:
    return 0;
}


/*************************************************************************
 * Function:      SAT_APDU_TC_HandleGetInkey
 *
 * Description:
 *
 *************************************************************************/
const char             *SAT_APDU_TC_HandleGetInkey()
{
    unsigned char           APDU_GetInkey_01[] = {
        0xD0, // Proactive Command
        0x2B, // Length
        0x81, 0x03, 0x01, 0x22, 0x02, // Command Details
        0x82, 0x02, 0x81, 0x02, // Device Id
        0x8D, 0x0F, 0x04, 0x45, 0x6E, 0x74, 0x65, 0x72, 0x20, 0x61, // Text String
        0x20, 0x64, 0x69, 0x67, 0x69, 0x74, 0x3A,
        0x1E, 0x02, 0x00, 0x08, // Icon ID
        0x84, 0x02, 0x00, 0x01, // Duration in Min
        0x50, 0x04, 0x01, 0x02, 0x03, 0x04, // Text Attribute
        0x68, 0x01, 0x01
    };
    unsigned char           APDU_GetInkey_02[] = {
        0xD0, // Proactive Command
        0x22, // Length
        0x81, 0x03, 0x01, 0x22, 0x02, // Command Details
        0x82, 0x02, 0x81, 0x02, // Device Id
        0x8D, 0x0F, 0x04, 0x45, 0x6E, 0x74, 0x65, 0x72, 0x20, 0x61, // Text String
        0x20, 0x64, 0x69, 0x67, 0x69, 0x74, 0x3A,
        0x1E, 0x02, 0x00, 0x08, // Icon ID
        0x84, 0x02, 0x01, 0x10 // Duration in Sec
    };
    unsigned char           APDU_GetInkey_03[] = {
        0xD0, // Proactive Command
        0x22, // Length
        0x81, 0x03, 0x01, 0x22, 0x02, // Command Details
        0x82, 0x02, 0x81, 0x02, // Device Id
        0x8D, 0x0F, 0x04, 0x45, 0x6E, 0x74, 0x65, 0x72, 0x20, 0x61, // Text String
        0x20, 0x64, 0x69, 0x67, 0x69, 0x74, 0x3A,
        0x1E, 0x02, 0x00, 0x08, // Icon ID
        0x84, 0x02, 0x02, 0x50 // Duration in Tenths of Sec
    };
    unsigned char           APDU_GetInkey_04[] = {
        0xD0, // Proactive Command
        0x24, // Length
        0x81, 0x03, 0x01, 0x22, 0x02, // Command Details
        0x82, 0x02, 0x81, 0x02, // Device Id
        0x8D, 0x0F, 0x04, 0x45, 0x6E, 0x74, 0x65, 0x72, 0x20, 0x61, // Text String
        0x20, 0x64, 0x69, 0x67, 0x69, 0x74, 0x3A,
        0x1E, 0x02, 0x00, 0x08, // Icon ID
        0xC8, 0x04, 0x01, 0x02, 0x03, 0x04 // Unknown Tag ( CDMA SMS )
    };
    unsigned char           APDU_GetInkey_05[] = {
        0xD0, // Proactive Command
        0x24, // Length
        0x81, 0x03, 0x01, 0x22, 0x02, // Command Details
        0x82, 0x02, 0x81, 0x02, // Device Id
        0x8D, 0x0F, 0x04, 0x45, 0x6E, 0x74, 0x65, 0x72, 0x20, 0x61, // Text String
        0x20, 0x64, 0x69, 0x67, 0x69, 0x74, 0x3A,
        0x1E, 0x02, 0x00, 0x08, // Icon ID
        0x50, 0x04, 0x01, 0x02, 0x03, 0x04 // Text Attribute
    };

    ste_parsed_apdu_t      *parsed_apdu_info_p = NULL;
    int                     APDU_Kind = STE_APDU_KIND_NONE;
    int                     APDU_Type = STE_APDU_CMD_TYPE_NONE;
    int                     current_apdu = 0;
    ste_command_result_t    ResponseData;
    unsigned char           AdditionalInfo[20] = "Additional Info OK.";
    static ste_apdu_text_string_t TextStr;

    ste_apdu_t             *apdu;
    ste_apdu_t             *unparsed_apdu;
    ste_apdu_t             *tr_apdu;

start_loop:
    switch( current_apdu ) {
        case 0:
            apdu = ste_apdu_new(APDU_GetInkey_01, sizeof(APDU_GetInkey_01));
            break;
        case 1:
            apdu = ste_apdu_new(APDU_GetInkey_02, sizeof(APDU_GetInkey_02));
            break;
        case 2:
            apdu = ste_apdu_new(APDU_GetInkey_03, sizeof(APDU_GetInkey_03));
            break;
        case 3:
            apdu = ste_apdu_new(APDU_GetInkey_04, sizeof(APDU_GetInkey_04));
            break;
        case 4:
             apdu = ste_apdu_new(APDU_GetInkey_05, sizeof(APDU_GetInkey_05));
             break;
        default: goto End;
    }
    if (!apdu)
        return "ste_apdu_new failed";

    current_apdu++;
    printf("\n");
    printf("############ Test SAT_APDU_TC_HandleGetInkey - %02d ############\n", current_apdu );
    //Parse an APDU string
    if (STE_SAT_APDU_ERROR_NONE !=
        ste_apdu_parse(apdu, &parsed_apdu_info_p)) {
        printf("FAILED : TC Get Inkey: Parse APDU failed.\n");
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        ste_apdu_delete(apdu);
        return "ste_apdu_parse failed.";
    } else {
        //print some useful information here
        printf("Result : TC Get Inkey: Parse APDU: OK\n");
    }

    //Get Kind
    APDU_Kind = ste_parsed_apdu_get_kind(parsed_apdu_info_p);
    if (STE_APDU_KIND_NONE == APDU_Kind) {
        printf("FAILED : TC Get Inkey: Get APDU Kind failed.\n");
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        ste_apdu_delete(apdu);
        return "ste_apdu_get_kind failed.";
    } else {
        //print some useful information here
        printf("The Kind of APDU is: 0x%x \n", APDU_Kind);
    }

    //Get Type
    APDU_Type = ste_parsed_apdu_get_type(parsed_apdu_info_p);
    if (STE_APDU_CMD_TYPE_NONE == APDU_Type) {
        printf("FAILED : TC Get Inkey: Get APDU Type failed.\n");
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        ste_apdu_delete(apdu);
        return "ste_parsed_apdu_get_type failed.";
    } else {
        //print some useful information here
        printf("The Type of APDU is: 0x%x \n", APDU_Type);
    }

    //Create a new APDU string
    if (STE_SAT_APDU_ERROR_NONE !=
        ste_apdu_unparse(parsed_apdu_info_p, &unparsed_apdu)) {
        printf("FAILED : TC Get Inkey: Create APDU failed.\n");
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        ste_apdu_delete(apdu);
        return "ste_apdu_unparse failed.";
    } else {
        //print some useful information here
        printf("Created Result APDU: ");
        printHexAPDU(unparsed_apdu);
        printf("Length of APDU binary data: %zd \n",
               ste_apdu_get_raw_length(unparsed_apdu));
    }

    //terminal response
    TextStr.text_length = 20;
    TextStr.coding_scheme = 0x01;       //TODO: needs to be changed, do not know what is a valid coding scheme value
    TextStr.text_string_p = AdditionalInfo;

    ResponseData.general_result = SAT_RES_CMD_PERFORMED_SUCCESSFULLY;
    ResponseData.command_status = STE_SAT_PC_CMD_STATUS_COMPLETED;
    ResponseData.additional_info_size = 20;
    ResponseData.additional_info_p = AdditionalInfo;
    ResponseData.other_data_type = STE_CMD_RESULT_TEXT_STRING;
    ResponseData.other_data.text_p = &TextStr;

    if (STE_SAT_APDU_ERROR_NONE !=
        ste_apdu_terminal_response(parsed_apdu_info_p, &ResponseData,
                                   &tr_apdu)) {
        printf("FAILED : TC Get Inkey: Create response APDU failed\n");
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        ste_apdu_delete(apdu);
        ste_apdu_delete(tr_apdu);
        return "ste_apdu_terminal_response failed.";
    } else {
        //print some useful information here
        printf("Created Response APDU: ");
        printHexAPDU(tr_apdu);
        printf("Length of Response APDU binary data: %zd \n",
               ste_apdu_get_raw_length(tr_apdu));
    }

    //delete the info structure
    ste_parsed_apdu_delete(parsed_apdu_info_p);
    parsed_apdu_info_p = NULL;
    ste_apdu_delete(tr_apdu);
    ste_apdu_delete(unparsed_apdu);
    ste_apdu_delete(apdu);
    goto start_loop;

End:
    return 0;
}

/*************************************************************************
 * Function:      SAT_APDU_TC_HandleGetInput
 *
 * Description:
 *
 *************************************************************************/
const char             *SAT_APDU_TC_HandleGetInput()
{
    unsigned char           APDU_GetInput_01[] = {
        0xD0, // Proactive Command
        0x20, // Length
        0x81, 0x03, 0x01, 0x23, 0x00, // Command Details
        0x82, 0x02, 0x81, 0x82, // Device Identities
        0x8D, 0x0C, 0x04, 0x45, 0x6E, 0x74, 0x65, 0x72, 0x20, 0x31, 0x32, // Text String
        0x33, 0x34, 0x35,
        0x91, 0x02, 0x05, 0x05, // Response Length
        0x17, 0x02, 0x00, 0x65, // Default Text
        0x1E, 0x02, 0x01, 0x01, //Icon id
        0x50, 0x04, 0x01, 0x02, 0x03, 0x04, // Text Attribute
        0x68, 0x01, 0x01 //Frame id
    };
    unsigned char           APDU_GetInput_02[] = {
        0xD0, // Proactive Command
        0x32, // Length
        0x81, 0x03, 0x01, 0x23, 0x00, // Command Details
        0x82, 0x02, 0x81, 0x82, // Device Identities
        0x8D, 0x0C, 0x04, 0x45, 0x6E, 0x74, 0x65, 0x72, 0x20, 0x31, 0x32, // Text String
        0x33, 0x34, 0x35,
        0x91, 0x02, 0x05, 0x05, // Response Length
        0x17, 0x02, 0x00, 0x65, // Default Text
        0x1E, 0x02, 0x01, 0x01, //Icon id
        0x50, 0x04, 0x01, 0x02, 0x03, 0x04, // Text Attribute
        0x68, 0x01, 0x01, //Frame id
        0xC8, 0x04, 0x01, 0x02, 0x03, 0x04 // Unknown Tag ( CDMA SMS )
    };
    ste_parsed_apdu_t      *parsed_apdu_info_p = NULL;
    int                     APDU_Kind = STE_APDU_KIND_NONE;
    int                     APDU_Type = STE_APDU_CMD_TYPE_NONE;
    int                     current_apdu = 0;
    ste_command_result_t    ResponseData;
    unsigned char           AdditionalInfo[20] = "Additional Info OK.";
    static ste_apdu_text_string_t TextStr;

    ste_apdu_t             *apdu;
    ste_apdu_t             *unparsed_apdu;
    ste_apdu_t             *tr_apdu;

start_loop:
    switch( current_apdu ) {
        case 0:
            apdu = ste_apdu_new(APDU_GetInput_01, sizeof(APDU_GetInput_01));
            break;
        case 1:
            apdu = ste_apdu_new(APDU_GetInput_02, sizeof(APDU_GetInput_02));
            break;
        default: goto End;
    }

    if (!apdu)
        return "ste_apdu_new failed";

    current_apdu++;
    printf("\n");
    printf("############ Test SAT_APDU_TC_HandleGetInput - %02d ############\n", current_apdu);
    //Parse an APDU string
    if (STE_SAT_APDU_ERROR_NONE !=
        ste_apdu_parse(apdu, &parsed_apdu_info_p)) {
        printf("FAILED : TC Get Input: Parse APDU failed.\n");
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        ste_apdu_delete(apdu);
        return "ste_apdu_parse failed.";
    } else {
        //print some useful information here
        printf("Result : TC Get Input: Parse APDU: OK\n");
    }

    //Get Kind
    APDU_Kind = ste_parsed_apdu_get_kind(parsed_apdu_info_p);
    if (STE_APDU_KIND_NONE == APDU_Kind) {
        printf("FAILED : TC Get Input: Get APDU Kind failed.\n");
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        ste_apdu_delete(apdu);
        return "ste_apdu_get_kind failed.";
    } else {
        //print some useful information here
        printf("The Kind of APDU is: 0x%x \n", APDU_Kind);
    }

    //Get Type
    APDU_Type = ste_parsed_apdu_get_type(parsed_apdu_info_p);
    if (STE_APDU_CMD_TYPE_NONE == APDU_Type) {
        printf("FAILED : TC Get Input: Get APDU Type failed.\n");
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        ste_apdu_delete(apdu);
        return "ste_parsed_apdu_get_type failed.";
    } else {
        //print some useful information here
        printf("The Type of APDU is: 0x%x \n", APDU_Type);
    }

    //Create a new APDU string
    if (STE_SAT_APDU_ERROR_NONE !=
        ste_apdu_unparse(parsed_apdu_info_p, &unparsed_apdu)) {
        printf("FAILED : TC Get Input: Create APDU failed.\n");
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        ste_apdu_delete(apdu);
        return "ste_apdu_unparse failed.";
    } else {
        //print some useful information here
        printf("Created Result APDU: ");
        printHexAPDU(unparsed_apdu);
        printf("Length of APDU binary data: %zd \n",
               ste_apdu_get_raw_length(unparsed_apdu));
    }

    //terminal response
    TextStr.text_length = 20;
    TextStr.coding_scheme = 0x01;       //TODO: needs to be changed, do not know what is a valid coding scheme value
    TextStr.text_string_p = AdditionalInfo;

    ResponseData.general_result = SAT_RES_CMD_PERFORMED_SUCCESSFULLY;
    ResponseData.command_status = STE_SAT_PC_CMD_STATUS_COMPLETED;
    ResponseData.additional_info_size = 20;
    ResponseData.additional_info_p = AdditionalInfo;
    ResponseData.other_data_type = STE_CMD_RESULT_TEXT_STRING;
    ResponseData.other_data.text_p = &TextStr;

    if (STE_SAT_APDU_ERROR_NONE !=
        ste_apdu_terminal_response(parsed_apdu_info_p, &ResponseData,
                                   &tr_apdu)) {
        printf("FAILED : TC Get Input: Create response APDU failed\n");
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        ste_apdu_delete(apdu);
        ste_apdu_delete(tr_apdu);
        return "ste_apdu_terminal_response failed.";
    } else {
        //print some useful information here
        printf("Created Response APDU: ");
        printHexAPDU(tr_apdu);
        printf("Length of Response APDU binary data: %zd \n",
               ste_apdu_get_raw_length(tr_apdu));
    }

    //delete the info structure
    ste_parsed_apdu_delete(parsed_apdu_info_p);
    parsed_apdu_info_p = NULL;
    ste_apdu_delete(tr_apdu);
    ste_apdu_delete(apdu);
    ste_apdu_delete(unparsed_apdu);
    goto start_loop;

End:
    return 0;
}

/*************************************************************************
 * Function:      SAT_APDU_TC_HandleMoreTime
 *
 * Description:
 *
 *************************************************************************/
const char             *SAT_APDU_TC_HandleMoreTime()
{
    unsigned char           APDU_MoreTime_01[] = {
        0xD0, // Proactive Command
        0x09, // Length
        0x81, 0x03, 0x01, 0x02, 0x00, // Command Details
        0x82, 0x02, 0x81, 0x82 // Device Id
    };
    unsigned char           APDU_MoreTime_02[] = {
        0xD0, // Proactive Command
        0x0f, // Length
        0x81, 0x03, 0x01, 0x02, 0x00, // Command Details
        0x82, 0x02, 0x81, 0x82, // Device Id
        0xC8, 0x04, 0x01, 0x02, 0x03, 0x04 // Unknown Tag ( CDMA SMS )
    };
    ste_parsed_apdu_t      *parsed_apdu_info_p;
    int                     APDU_Kind = STE_APDU_KIND_NONE;
    int                     APDU_Type = STE_APDU_CMD_TYPE_NONE;
    int                     current_apdu = 0;
    ste_command_result_t    ResponseData;

    ste_apdu_t             *apdu;
    ste_apdu_t             *unparsed_apdu;
    ste_apdu_t             *tr_apdu;

start_loop:
    switch( current_apdu ) {
        case 0:
            apdu = ste_apdu_new(APDU_MoreTime_01, sizeof(APDU_MoreTime_01));
            break;
        case 1:
            apdu = ste_apdu_new(APDU_MoreTime_02, sizeof(APDU_MoreTime_02));
            break;
        default: goto End;
    }
    if (!apdu)
        return "ste_apdu_new failed";

    current_apdu++;
    printf("\n");
    printf("############ Test SAT_APDU_TC_HandleMoreTime - %02d ############\n", current_apdu);
    //Parse an APDU string
    if (STE_SAT_APDU_ERROR_NONE !=
        ste_apdu_parse(apdu, &parsed_apdu_info_p)) {
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        printf("FAILED : TC More Time: Parse APDU failed.\n");
        return "ste_apdu_parse failed.";
    } else {
        //print some useful information here
        printf("Result : TC More Time: Parse APDU: OK\n");
    }

    //Get Kind
    APDU_Kind = ste_parsed_apdu_get_kind(parsed_apdu_info_p);
    if (STE_APDU_KIND_NONE == APDU_Kind) {
        printf("FAILED : TC More Time: Get APDU Kind failed.\n");
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        return "ste_apdu_get_kind failed.";
    } else {
        //print some useful information here
        printf("The Kind of APDU is: 0x%x \n", APDU_Kind);
    }

    //Get Type
    APDU_Type = ste_parsed_apdu_get_type(parsed_apdu_info_p);
    if (STE_APDU_CMD_TYPE_NONE == APDU_Type) {
        printf("FAILED : TC More Time: Get APDU Type failed.\n");
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        return "ste_parsed_apdu_get_type failed.";
    } else {
        //print some useful information here
        printf("The Type of APDU is: 0x%x \n", APDU_Type);
    }

    //Create a new APDU string
    if (STE_SAT_APDU_ERROR_NONE !=
        ste_apdu_unparse(parsed_apdu_info_p, &unparsed_apdu)) {
        printf("FAILED : TC More Time: Create APDU failed.\n");
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        return "ste_apdu_unparse failed.";
    } else {
        //print some useful information here
        printf("Created Result APDU: ");
        printHexAPDU(unparsed_apdu);
        printf("Length of APDU binary data: %zd \n",
               ste_apdu_get_raw_length(unparsed_apdu));
    }

    ResponseData.general_result = SAT_RES_CMD_PERFORMED_SUCCESSFULLY;
    ResponseData.command_status = STE_SAT_PC_CMD_STATUS_COMPLETED;
    ResponseData.additional_info_size = 0;
    ResponseData.additional_info_p = NULL;
    ResponseData.other_data_type = STE_CMD_RESULT_NOTHING;
    ResponseData.other_data.text_p = NULL;

    if (STE_SAT_APDU_ERROR_NONE !=
        ste_apdu_terminal_response(parsed_apdu_info_p, &ResponseData,
                                   &tr_apdu)) {
        printf("FAILED : TC More Time: Create response APDU failed\n");
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        return "ste_apdu_terminal_response failed.";
    } else {
        //print some useful information here
        printf("Created Response APDU: ");
        printHexAPDU(tr_apdu);
        printf("Length of Response APDU binary data: %zd \n",
               ste_apdu_get_raw_length(tr_apdu));
    }

    //delete the info structure
    ste_parsed_apdu_delete(parsed_apdu_info_p);
    ste_apdu_delete(tr_apdu);
    ste_apdu_delete(apdu);
    ste_apdu_delete(unparsed_apdu);
    goto start_loop;

End:
    return 0;
}


/*************************************************************************
 * Function:      SAT_APDU_TC_HandlePollingOff
 *
 * Description:
 *
 *************************************************************************/
const char             *SAT_APDU_TC_HandlePollingOff()
{
    unsigned char           APDU_PollingOff_01[] = {
        0xD0, // Proactive Command
        0x09, // Length
        0x81, 0x03, 0x01, 0x04, 0x00, // Command Details
        0x82, 0x02, 0x81, 0x82 // Device Id
    };
    unsigned char           APDU_PollingOff_02[] = {
        0xD0, // Proactive Command
        0x0f, // Length
        0x81, 0x03, 0x01, 0x04, 0x00, // Command Details
        0x82, 0x02, 0x81, 0x82, // Device Id
        0xC8, 0x04, 0x01, 0x02, 0x03, 0x04 // Unknown Tag ( CDMA SMS )
    };
    ste_parsed_apdu_t      *parsed_apdu_info_p;
    int                     APDU_Kind = STE_APDU_KIND_NONE;
    int                     APDU_Type = STE_APDU_CMD_TYPE_NONE;
    int                     current_apdu = 0;
    ste_command_result_t    ResponseData;

    ste_apdu_t             *apdu;
    ste_apdu_t             *unparsed_apdu;
    ste_apdu_t             *tr_apdu;

start_loop:
    switch( current_apdu ) {
        case 0:
            apdu = ste_apdu_new(APDU_PollingOff_01, sizeof(APDU_PollingOff_01));
            break;
        case 1:
            apdu = ste_apdu_new(APDU_PollingOff_02, sizeof(APDU_PollingOff_02));
            break;
        default:
            goto End;
    }
    if (!apdu)
        return "ste_apdu_new failed";

    current_apdu++;
    printf("\n");
    printf
        ("############ Test SAT_APDU_TC_HandlePollingOff - %02d ############\n", current_apdu );
    //Parse an APDU string
    if (STE_SAT_APDU_ERROR_NONE !=
        ste_apdu_parse(apdu, &parsed_apdu_info_p)) {
        printf("FAILED : TC Polling Off: Parse APDU failed.\n");
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        return "ste_apdu_parse failed.";
    } else {
        //print some useful information here
        printf("Result : TC Polling Off: Parse APDU: OK\n");
    }

    //Get Kind
    APDU_Kind = ste_parsed_apdu_get_kind(parsed_apdu_info_p);
    if (STE_APDU_KIND_NONE == APDU_Kind) {
        printf("FAILED : TC Polling Off: Get APDU Kind failed.\n");
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        return "ste_apdu_get_kind failed.";
    } else {
        //print some useful information here
        printf("The Kind of APDU is: 0x%x \n", APDU_Kind);
    }

    //Get Type
    APDU_Type = ste_parsed_apdu_get_type(parsed_apdu_info_p);
    if (STE_APDU_CMD_TYPE_NONE == APDU_Type) {
        printf("FAILED : TC Polling Off: Get APDU Type failed.\n");
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        return "ste_parsed_apdu_get_type failed.";
    } else {
        //print some useful information here
        printf("The Type of APDU is: 0x%x \n", APDU_Type);
    }

    //Create a new APDU string
    if (STE_SAT_APDU_ERROR_NONE !=
        ste_apdu_unparse(parsed_apdu_info_p, &unparsed_apdu)) {
        printf("FAILED : TC Polling Off: Create APDU failed.\n");
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        return "ste_apdu_unparse failed.";
    } else {
        //print some useful information here
        printf("Created Result APDU: ");
        printHexAPDU(unparsed_apdu);
        printf("Length of APDU binary data: %zd \n",
               ste_apdu_get_raw_length(unparsed_apdu));
    }

    ResponseData.general_result = SAT_RES_CMD_PERFORMED_SUCCESSFULLY;
    ResponseData.command_status = STE_SAT_PC_CMD_STATUS_COMPLETED;
    ResponseData.additional_info_size = 0;
    ResponseData.additional_info_p = NULL;
    ResponseData.other_data_type = STE_CMD_RESULT_NOTHING;
    ResponseData.other_data.text_p = NULL;

    if (STE_SAT_APDU_ERROR_NONE !=
        ste_apdu_terminal_response(parsed_apdu_info_p, &ResponseData,
                                   &tr_apdu)) {
        printf("FAILED : TC Polling Off: Create response APDU failed\n");
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        return "ste_apdu_terminal_response failed.";
    } else {
        //print some useful information here
        printf("Created Response APDU: ");
        printHexAPDU(tr_apdu);
        printf("Length of Response APDU binary data: %zd \n",
               ste_apdu_get_raw_length(tr_apdu));
    }

    //delete the info structure
    ste_parsed_apdu_delete(parsed_apdu_info_p);
    ste_apdu_delete(tr_apdu);
    ste_apdu_delete(apdu);
    ste_apdu_delete(unparsed_apdu);
    goto start_loop;

End:
    return 0;
}

/*************************************************************************
 * Function:      SAT_APDU_TC_HandlePollInterval
 *
 * Description:
 *
 *************************************************************************/
const char             *SAT_APDU_TC_HandlePollInterval()
{
    unsigned char           APDU_PollInterval_01[] = {
        0xD0, // proactive Command
        0x0D, // Lengtj
        0x81, 0x03, 0x01, 0x03, 0x00, // Command Details
        0x82, 0x02, 0x81, 0x82, // Device Id
        0x84, 0x02, 0x00, 0x01 // Duration
    };
    unsigned char           APDU_PollInterval_02[] = {
        0xD0, // proactive Command
        0x13, // Lengtj
        0x81, 0x03, 0x01, 0x03, 0x00, // Command Details
        0x82, 0x02, 0x81, 0x82, // Device Id
        0x84, 0x02, 0x00, 0x01, // Duration
        0xC8, 0x04, 0x01, 0x02, 0x03, 0x04 // Unknown Tag ( CDMA SMS )
    };
    ste_parsed_apdu_t      *parsed_apdu_info_p;
    int                     APDU_Kind = STE_APDU_KIND_NONE;
    int                     APDU_Type = STE_APDU_CMD_TYPE_NONE;
    int                     current_apdu = 0;
    ste_command_result_t    ResponseData;

    ste_apdu_t             *apdu;
    ste_apdu_t             *unparsed_apdu;
    ste_apdu_t             *tr_apdu;

start_loop:
    switch( current_apdu ) {
        case 0:
            apdu = ste_apdu_new(APDU_PollInterval_01, sizeof(APDU_PollInterval_01));
            break;
        case 1:
            apdu = ste_apdu_new(APDU_PollInterval_02, sizeof(APDU_PollInterval_02));
            break;
        default: goto End;
    }
    if (!apdu)
        return "ste_apdu_new failed";

    current_apdu++;
    printf("\n");
    printf
        ("############ Test SAT_APDU_TC_HandlePollInterval - %02d ############\n", current_apdu );
    //Parse an APDU string
    if (STE_SAT_APDU_ERROR_NONE !=
        ste_apdu_parse(apdu, &parsed_apdu_info_p)) {
        printf("FAILED : TC Poll Interval: Parse APDU failed.\n");
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        ste_apdu_delete(apdu);
        return "ste_apdu_parse failed.";
    } else {
        //print some useful information here
        printf("Result : TC Poll Interval: Parse APDU: OK\n");
    }

    //Get Kind
    APDU_Kind = ste_parsed_apdu_get_kind(parsed_apdu_info_p);
    if (STE_APDU_KIND_NONE == APDU_Kind) {
        printf("FAILED : TC Poll Interval: Get APDU Kind failed.\n");
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        ste_apdu_delete(apdu);
        return "ste_apdu_get_kind failed.";
    } else {
        //print some useful information here
        printf("The Kind of APDU is: 0x%x \n", APDU_Kind);
    }

    //Get Type
    APDU_Type = ste_parsed_apdu_get_type(parsed_apdu_info_p);
    if (STE_APDU_CMD_TYPE_NONE == APDU_Type) {
        printf("FAILED : TC Poll Interval: Get APDU Type failed.\n");
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        ste_apdu_delete(apdu);
        return "ste_parsed_apdu_get_type failed.";
    } else {
        //print some useful information here
        printf("The Type of APDU is: 0x%x \n", APDU_Type);
    }

    //Create a new APDU string
    if (STE_SAT_APDU_ERROR_NONE !=
        ste_apdu_unparse(parsed_apdu_info_p, &unparsed_apdu)) {
        printf("FAILED : TC Poll Interval: Create APDU failed.\n");
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        ste_apdu_delete(apdu);
        return "ste_apdu_unparse failed.";
    } else {
        //print some useful information here
        printf("Created Result APDU: ");
        printHexAPDU(unparsed_apdu);
        printf("Length of APDU binary data: %zd \n",
               ste_apdu_get_raw_length(unparsed_apdu));
    }

    ResponseData.general_result = SAT_RES_CMD_PERFORMED_SUCCESSFULLY;
    ResponseData.command_status = STE_SAT_PC_CMD_STATUS_COMPLETED;
    ResponseData.additional_info_size = 0;
    ResponseData.additional_info_p = NULL;
    ResponseData.other_data_type = STE_CMD_RESULT_NOTHING;

    if (STE_SAT_APDU_ERROR_NONE !=
        ste_apdu_terminal_response(parsed_apdu_info_p, &ResponseData,
                                   &tr_apdu)) {
        printf("FAILED : TC Poll Interval: Create response APDU failed\n");
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        ste_apdu_delete(apdu);
        return "ste_apdu_terminal_response failed.";
    } else {
        //print some useful information here
        printf("Created Response APDU: ");
        printHexAPDU(tr_apdu);
        printf("Length of Response APDU binary data: %zd \n",
               ste_apdu_get_raw_length(tr_apdu));
    }

    //delete the info structure
    ste_parsed_apdu_delete(parsed_apdu_info_p);

    ste_apdu_delete(apdu);
    ste_apdu_delete(unparsed_apdu);
    ste_apdu_delete(tr_apdu);
    goto start_loop;

End:
    return 0;
}

/*************************************************************************
 * Function:      SAT_APDU_TC_HandleTimerManagement
 *
 * Description:
 *
 *************************************************************************/
const char             *SAT_APDU_TC_HandleTimerManagement()
{
    unsigned char           APDU_TimerManagement_01[] = /* Start Timer */
    {
        0xD0, // Proactive Command
        0x11, // Length
        0x81, 0x03, 0x01, 0x27, 0x00, // Command Details
        0x82, 0x02, 0x81, 0x02, // Device Id
        0xA4, 0x01, 0x01, // Timer Id
        0xA5, 0x03, 0x00, 0x50, 0x00 // Timer Val
    };
    unsigned char           APDU_TimerManagement_02[] = /* Get Timer */
    {
        0xD0, // Proactive Command
        0x11, // Length
        0x81, 0x03, 0x01, 0x27, 0x02, // Command Details
        0x82, 0x02, 0x81, 0x02, // Device Id
        0xA4, 0x01, 0x01, // Timer Id
        0xA5, 0x03, 0x00, 0x50, 0x00 // Timer Val
    };
    unsigned char           APDU_TimerManagement_03[] = /* Deactivate Timer */
    {
        0xD0, // Proactive Command
        0x11, // Length
        0x81, 0x03, 0x01, 0x27, 0x01, // Command Details
        0x82, 0x02, 0x81, 0x02, // Device Id
        0xA4, 0x01, 0x01, // Timer Id
        0xA5, 0x03, 0x00, 0x50, 0x00 // Timer Val
    };
    unsigned char           APDU_TimerManagement_04[] = /* Start Timer - No timer value */
    {
        0xD0, // Proactive Command
        0x0c, // Length
        0x81, 0x03, 0x01, 0x27, 0x01, // Command Details
        0x82, 0x02, 0x81, 0x02, // Device Id
        0xA4, 0x01, 0x01 // Timer Id
    };
    unsigned char           APDU_TimerManagement_05[] = /* Start Timer */
    {
        0xD0, // Proactive Command
        0x17, // Length
        0x81, 0x03, 0x01, 0x27, 0x00, // Command Details
        0x82, 0x02, 0x81, 0x02, // Device Id
        0xA4, 0x01, 0x01, // Timer Id
        0xA5, 0x03, 0x00, 0x50, 0x00, // Timer Val
        0xC8, 0x04, 0x01, 0x02, 0x03, 0x04 // Unknown Tag ( CDMA SMS )
    };
    ste_parsed_apdu_t      *parsed_apdu_info_p = 0;
    int                     APDU_Kind = STE_APDU_KIND_NONE;
    int                     APDU_Type = STE_APDU_CMD_TYPE_NONE;
    int                     current_apdu = 0;
    ste_command_result_t    ResponseData;

    ste_apdu_t             *apdu;
    ste_apdu_t             *unparsed_apdu;
    ste_apdu_t             *tr_apdu;

start_loop:
    switch( current_apdu ) {
        case 0:
            apdu =
                ste_apdu_new(APDU_TimerManagement_01, sizeof(APDU_TimerManagement_01));
            break;

        case 1:
            apdu =
                ste_apdu_new(APDU_TimerManagement_02, sizeof(APDU_TimerManagement_02));
            break;

        case 2:
            apdu =
                ste_apdu_new(APDU_TimerManagement_03, sizeof(APDU_TimerManagement_03));
            break;

        case 3:
            apdu =
                ste_apdu_new(APDU_TimerManagement_04, sizeof(APDU_TimerManagement_04));
            break;

        case 4:
            apdu =
                ste_apdu_new(APDU_TimerManagement_05, sizeof(APDU_TimerManagement_05));
            break;

        default: goto End;
    }
    if (!apdu)
        return "ste_apdu_new failed";

    current_apdu++;
    printf("\n");
    printf
        ("############ Test SAT_APDU_TC_HandleTimerManagement - %02d ############\n", current_apdu );
    //Parse an APDU string
    if (STE_SAT_APDU_ERROR_NONE !=
        ste_apdu_parse(apdu, &parsed_apdu_info_p)) {
        printf("FAILED : TC Timer Management: Parse APDU failed.\n");
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        return "ste_apdu_parse failed.";
    } else {
        //print some useful information here
        printf("Result : TC Timer Management: Parse APDU: OK\n");
    }

    //Get Kind
    APDU_Kind = ste_parsed_apdu_get_kind(parsed_apdu_info_p);
    if (STE_APDU_KIND_NONE == APDU_Kind) {
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        printf("FAILED : TC Timer Management: Get APDU Kind failed.\n");
        return "ste_apdu_get_kind failed.";
    } else {
        //print some useful information here
        printf("The Kind of APDU is: 0x%x \n", APDU_Kind);
    }

    //Get Type
    APDU_Type = ste_parsed_apdu_get_type(parsed_apdu_info_p);
    if (STE_APDU_CMD_TYPE_NONE == APDU_Type) {
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        printf("FAILED : TC Timer Management: Get APDU Type failed.\n");
        return "ste_parsed_apdu_get_type failed.";
    } else {
        //print some useful information here
        printf("The Type of APDU is: 0x%x \n", APDU_Type);
    }

    //Create a new APDU string
    if (STE_SAT_APDU_ERROR_NONE !=
        ste_apdu_unparse(parsed_apdu_info_p, &unparsed_apdu)) {
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        printf("FAILED : TC Timer Management: Create APDU failed.\n");
        return "ste_apdu_unparse failed.";
    } else {
        //print some useful information here
        printf("Created Result APDU: ");
        printHexAPDU(unparsed_apdu);
        printf("Length of APDU binary data: %zd \n",
               ste_apdu_get_raw_length(unparsed_apdu));
    }

    ResponseData.general_result = SAT_RES_CMD_PERFORMED_SUCCESSFULLY;
    ResponseData.command_status = STE_SAT_PC_CMD_STATUS_COMPLETED;
    ResponseData.additional_info_size = 0;
    ResponseData.additional_info_p = NULL;
    ResponseData.other_data_type = STE_CMD_RESULT_NOTHING;

    if (STE_SAT_APDU_ERROR_NONE !=
        ste_apdu_terminal_response(parsed_apdu_info_p, &ResponseData,
                                   &tr_apdu)) {
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        printf
            ("FAILED : TC Timer Management: Create response APDU failed\n");
        return "ste_apdu_terminal_response failed.";
    } else {
        //print some useful information here
        printf("Created Response APDU: ");
        printHexAPDU(tr_apdu);
        printf("Length of Response APDU binary data: %zd \n",
               ste_apdu_get_raw_length(tr_apdu));
    }

    //delete the info structure
    ste_parsed_apdu_delete(parsed_apdu_info_p);
    ste_apdu_delete(apdu);
    ste_apdu_delete(unparsed_apdu);
    ste_apdu_delete(tr_apdu);
    goto start_loop;

End:
    return 0;
}

/*************************************************************************
 * Function:      SAT_APDU_TC_HandleSendUssd
 *
 * Description:
 *
 *************************************************************************/
const char             *SAT_APDU_TC_HandleSendUssd()
{
    unsigned char           APDU_SendUssd_01[] = {
        0xD0, // Proactive Command
        0x13, // Length
        0x81, 0x03, 0x01, 0x12, 0x00, // Command Details
        0x82, 0x02, 0x81, 0x83, // Device Id
        0x8A, 0x08, 0x0F, 0xAA, 0x98, 0x6D, 0xA6, 0x8A, 0xC5, 0x46 // USSD String
    };
    unsigned char           APDU_SendUssd_02[] = {
        0xD0, // Proactive Command
        0x19, // Length
        0x81, 0x03, 0x01, 0x12, 0x00, // Command Details
        0x82, 0x02, 0x81, 0x83, // Device Id
        0x8A, 0x08, 0x0F, 0xAA, 0x98, 0x6D, 0xA6, 0x8A, 0xC5, 0x46, // USSD String
        0xC8, 0x04, 0x01, 0x02, 0x03, 0x04 // Unknown Tag ( CDMA SMS )
    };
    ste_parsed_apdu_t      *parsed_apdu_info_p = 0;
    int                     APDU_Kind = STE_APDU_KIND_NONE;
    int                     APDU_Type = STE_APDU_CMD_TYPE_NONE;
    int                     current_apdu = 0;
    ste_command_result_t    ResponseData;
    ste_apdu_t             *apdu;
    ste_apdu_t             *tr_apdu;

start_loop:
    switch( current_apdu ) {
        case 0:
            apdu = ste_apdu_new(APDU_SendUssd_01, sizeof(APDU_SendUssd_01));
            break;
        case 1:
            apdu = ste_apdu_new(APDU_SendUssd_02, sizeof(APDU_SendUssd_02));
            break;
        default: goto End;
    }
    if (!apdu)
        return "ste_apdu_new failed";

    current_apdu++;
    printf("\n");
    printf
        ("############ Test SAT_APDU_TC_HandleSendUssd - %02d ############\n", current_apdu);
    //Parse an APDU string
    if (STE_SAT_APDU_ERROR_NONE !=
        ste_apdu_parse(apdu, &parsed_apdu_info_p)) {
        printf("FAILED : TC Send USSD: Parse APDU failed.\n");
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        return "ste_apdu_parse failed.";
    } else {
        //print some useful information here
        printf("Result : TC Send USSD: Parse APDU: OK\n");
    }

    //Get Kind
    APDU_Kind = ste_parsed_apdu_get_kind(parsed_apdu_info_p);
    if (STE_APDU_KIND_NONE == APDU_Kind) {
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        printf("FAILED : TC Send USSD: Get APDU Kind failed.\n");
        return "ste_apdu_get_kind failed.";
    } else {
        //print some useful information here
        printf("The Kind of APDU is: 0x%x \n", APDU_Kind);
    }

    //Get Type
    APDU_Type = ste_parsed_apdu_get_type(parsed_apdu_info_p);
    if (STE_APDU_CMD_TYPE_NONE == APDU_Type) {
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        printf("FAILED : TC Send USSD: Get APDU Type failed.\n");
        return "ste_parsed_apdu_get_type failed.";
    } else {
        //print some useful information here
        printf("The Type of APDU is: 0x%x \n", APDU_Type);
    }

    ResponseData.general_result = SAT_RES_CMD_PERFORMED_SUCCESSFULLY;
    ResponseData.command_status = STE_SAT_PC_CMD_STATUS_COMPLETED;
    ResponseData.additional_info_size = 0;
    ResponseData.additional_info_p = NULL;
    ResponseData.other_data_type = STE_CMD_RESULT_NOTHING;

    if (STE_SAT_APDU_ERROR_NONE !=
        ste_apdu_terminal_response(parsed_apdu_info_p, &ResponseData,
                                   &tr_apdu)) {
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        printf
            ("FAILED : TC Send USSD: Create response APDU failed\n");
        return "ste_apdu_terminal_response failed.";
    } else {
        //print some useful information here
        printf("Created Response APDU: ");
        printHexAPDU(tr_apdu);
        printf("Length of Response APDU binary data: %zd \n",
               ste_apdu_get_raw_length(tr_apdu));
    }

    //delete the info structure
    ste_parsed_apdu_delete(parsed_apdu_info_p);
    ste_apdu_delete(apdu);
    ste_apdu_delete(tr_apdu);
    goto start_loop;

End:
    return 0;
}

/*************************************************************************
 * Function:      SAT_APDU_TC_HandleSendSS
 *
 * Description:
 *
 *************************************************************************/
const char             *SAT_APDU_TC_HandleSendSS()
{
    unsigned char           APDU_SendSS_01[] = {
        0xD0, // Proactive Command
        0x12, // Length
        0x81, 0x03, 0x01, 0x11, 0x00, // Command Details
        0x82, 0x02, 0x81, 0x83, // Device Id
        0x89, 0x07, 0x11, 0x2a, 0x23, 0x33, 0x33, 0x2a, 0x23 // SS String
    };
    unsigned char           APDU_SendSS_02[] = {
        0xD0, // Proactive Command
        0x18, // Length
        0x81, 0x03, 0x01, 0x11, 0x00, // Command Details
        0x82, 0x02, 0x81, 0x83, // Device Id
        0x89, 0x07, 0x11, 0x2a, 0x23, 0x33, 0x33, 0x2a, 0x23, // SS String
        0xC8, 0x04, 0x01, 0x02, 0x03, 0x04 // Unknown Tag ( CDMA SMS )
    };
    ste_parsed_apdu_t      *parsed_apdu_info_p = 0;
    int                     APDU_Kind = STE_APDU_KIND_NONE;
    int                     APDU_Type = STE_APDU_CMD_TYPE_NONE;
    int                     current_apdu = 0;
    ste_command_result_t    ResponseData;
    ste_apdu_t             *apdu;
    ste_apdu_t             *tr_apdu;

start_loop:
    switch( current_apdu ) {
        case 0:
            apdu = ste_apdu_new(APDU_SendSS_01, sizeof(APDU_SendSS_01));
            break;
        case 1:
            apdu = ste_apdu_new(APDU_SendSS_02, sizeof(APDU_SendSS_02));
            break;
        default: goto End;
    }
    if (!apdu)
        return "ste_apdu_new failed";

    current_apdu ++;
    printf("\n");
    printf
        ("############ Test SAT_APDU_TC_HandleSendSS - %02d ############\n", current_apdu );
    //Parse an APDU string
    if (STE_SAT_APDU_ERROR_NONE !=
        ste_apdu_parse(apdu, &parsed_apdu_info_p)) {
        printf("FAILED : TC Send SS: Parse APDU failed.\n");
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        return "ste_apdu_parse failed.";
    } else {
        //print some useful information here
        printf("Result : TC Send SS: Parse APDU: OK\n");
    }

    //Get Kind
    APDU_Kind = ste_parsed_apdu_get_kind(parsed_apdu_info_p);
    if (STE_APDU_KIND_NONE == APDU_Kind) {
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        printf("FAILED : TC Send SS: Get APDU Kind failed.\n");
        return "ste_apdu_get_kind failed.";
    } else {
        //print some useful information here
        printf("The Kind of APDU is: 0x%x \n", APDU_Kind);
    }

    //Get Type
    APDU_Type = ste_parsed_apdu_get_type(parsed_apdu_info_p);
    if (STE_APDU_CMD_TYPE_NONE == APDU_Type) {
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        printf("FAILED : TC Send SS: Get APDU Type failed.\n");
        return "ste_parsed_apdu_get_type failed.";
    } else {
        //print some useful information here
        printf("The Type of APDU is: 0x%x \n", APDU_Type);
    }

    ResponseData.general_result = SAT_RES_CMD_PERFORMED_SUCCESSFULLY;
    ResponseData.command_status = STE_SAT_PC_CMD_STATUS_COMPLETED;
    ResponseData.additional_info_size = 0;
    ResponseData.additional_info_p = NULL;
    ResponseData.other_data_type = STE_CMD_RESULT_NOTHING;

    if (STE_SAT_APDU_ERROR_NONE !=
        ste_apdu_terminal_response(parsed_apdu_info_p,
                                   &ResponseData,
                                   &tr_apdu)) {
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        printf
            ("FAILED : TC Send SS: Create response APDU failed\n");
        return "ste_apdu_terminal_response failed.";
    } else {
        //print some useful information here
        printf("Created Response APDU: ");
        printHexAPDU(tr_apdu);
        printf("Length of Response APDU binary data: %zd \n",
               ste_apdu_get_raw_length(tr_apdu));
    }
    //delete the info structure
    ste_parsed_apdu_delete(parsed_apdu_info_p);
    ste_apdu_delete(apdu);
    ste_apdu_delete(tr_apdu);
    goto start_loop;

End:
    return 0;
}

/*************************************************************************
 * Function:      SAT_APDU_TC_HandleRefresh
 *
 * Description:
 *
 *************************************************************************/
const char             *SAT_APDU_TC_HandleRefresh()
{
    unsigned char           APDU_Refresh_01[] = {
        0xD0, // Proactive Command
        0x33, // Length
        0x81, 0x03, 0x01, 0x01, 0x02, //Command Details
        0x82, 0x02, 0x81, 0x82, //Device Id
        0x92, 0x0f, 0x02, 0x3F, 0x00, 0x7f, 0x66, 0x5F, 0x30, // File List
        0x4F, 0x36, 0x3F, 0x00, 0x7F, 0x20, 0x6F, 0x46,
        0xAF, 0x01, 0x01, //AID
        0x05,0x07,0x52,0x65,0x66,0x72,0x65,0x73,0x68, // Alpha Id
        0x1E,0x02,0x01,0x01, // Icon Id
        0x50,0x04,0x01,0x02,0x03,0x04, // Text Attribute
        0x68,0x01,0x01 //Frame id
    };
    unsigned char           APDU_Refresh_02[] = {
        0xD0, // Proactive Command
        0x39, // Length
        0x81, 0x03, 0x01, 0x01, 0x02, //Command Details
        0x82, 0x02, 0x81, 0x82, //Device Id
        0x92, 0x0f, 0x02, 0x3F, 0x00, 0x7f, 0x66, 0x5F, 0x30, // File List
        0x4F, 0x36, 0x3F, 0x00, 0x7F, 0x20, 0x6F, 0x46,
        0xAF, 0x01, 0x01, //AID
        0x05,0x07,0x52,0x65,0x66,0x72,0x65,0x73,0x68, // Alpha Id
        0x1E,0x02,0x01,0x01, // Icon Id
        0x50,0x04,0x01,0x02,0x03,0x04, // Text Attribute
        0x68,0x01,0x01, //Frame id
        0xC8, 0x04, 0x01, 0x02, 0x03, 0x04 // Unknown Tag ( CDMA SMS )
    };
    ste_parsed_apdu_t      *parsed_apdu_info_p;
    int                     APDU_Kind = STE_APDU_KIND_NONE;
    int                     APDU_Type = STE_APDU_CMD_TYPE_NONE;
    int                     current_apdu = 0;
    ste_command_result_t    ResponseData;

    ste_apdu_t             *apdu;
    ste_apdu_t             *tr_apdu;

start_loop:
    switch( current_apdu ) {
        case 0:
            apdu = ste_apdu_new(APDU_Refresh_01, sizeof(APDU_Refresh_01));
            break;
        case 1:
            apdu = ste_apdu_new(APDU_Refresh_02, sizeof(APDU_Refresh_02));
            break;
        default:
            goto End;
    }
    if (!apdu)
        return "ste_apdu_new failed";

    current_apdu++;
    printf("\n");
    printf
        ("############ Test SAT_APDU_TC_HandleRefresh - %02d ############\n", current_apdu );
    //Parse an APDU string
    if (STE_SAT_APDU_ERROR_NONE !=
        ste_apdu_parse(apdu, &parsed_apdu_info_p)) {
        printf("FAILED : TC Refresh: Parse APDU failed.\n");
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        return "ste_apdu_parse failed.";
    } else {
        //print some useful information here
        printf("Result : TC Refresh: Parse APDU: OK\n");
    }

    //Get Kind
    APDU_Kind = ste_parsed_apdu_get_kind(parsed_apdu_info_p);
    if (STE_APDU_KIND_NONE == APDU_Kind) {
        printf("FAILED : TC Refresh: Get APDU Kind failed.\n");
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        return "ste_apdu_get_kind failed.";
    } else {
        //print some useful information here
        printf("The Kind of APDU is: 0x%x \n", APDU_Kind);
    }

    //Get Type
    APDU_Type = ste_parsed_apdu_get_type(parsed_apdu_info_p);
    if (STE_APDU_CMD_TYPE_NONE == APDU_Type) {
        printf("FAILED : TC Refresh: Get APDU Type failed.\n");
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        return "ste_parsed_apdu_get_type failed.";
    } else {
        //print some useful information here
        printf("The Type of APDU is: 0x%x \n", APDU_Type);
    }

    ResponseData.general_result = SAT_RES_CMD_PERFORMED_SUCCESSFULLY;
    ResponseData.command_status = STE_SAT_PC_CMD_STATUS_COMPLETED;
    ResponseData.additional_info_size = 0;
    ResponseData.additional_info_p = NULL;
    ResponseData.other_data_type = STE_CMD_RESULT_NOTHING;
    ResponseData.other_data.text_p = NULL;

    if (STE_SAT_APDU_ERROR_NONE !=
        ste_apdu_terminal_response(parsed_apdu_info_p, &ResponseData,
                                   &tr_apdu)) {
        printf("FAILED : TC Refresh: Create response APDU failed\n");
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        return "ste_apdu_terminal_response failed.";
    } else {
        //print some useful information here
        printf("Created Response APDU: ");
        printHexAPDU(tr_apdu);
        printf("Length of Response APDU binary data: %zd \n",
               ste_apdu_get_raw_length(tr_apdu));
    }

    //delete the info structure
    ste_parsed_apdu_delete(parsed_apdu_info_p);
    ste_apdu_delete(tr_apdu);
    ste_apdu_delete(apdu);
    goto start_loop;

End:
    return 0;
}

/*************************************************************************
 * Function:      SAT_APDU_TC_HandleSetupCall
 *
 * Description:
 *
 *************************************************************************/
const char             *SAT_APDU_TC_HandleSetupCall()
{
    unsigned char           APDU_SetupCall_01[] = {
        0xD0, // Proactive Command
        0x1E, // Length
        0x81, 0x03, 0x01, 0x10, 0x00, // Command Details
        0x82, 0x02, 0x81, 0x83, //Device Id
        0x85, 0x08, 0x4E, 0x6F, 0x74, 0x20, 0x62, 0x75, 0x73, 0x79, // Alpha Id
        0x86, 0x09, 0x91, 0x10, 0x32, 0x04, 0x21, 0x43, 0x65, 0x1C, 0x2C // Address
    };
    unsigned char           APDU_SetupCall_02[] = {
        0xD0, // Proactive Command
        0x24, // Length
        0x81, 0x03, 0x01, 0x10, 0x00, // Command Details
        0x82, 0x02, 0x81, 0x83, //Device Id
        0x85, 0x08, 0x4E, 0x6F, 0x74, 0x20, 0x62, 0x75, 0x73, 0x79, // Alpha Id
        0x86, 0x09, 0x91, 0x10, 0x32, 0x04, 0x21, 0x43, 0x65, 0x1C, 0x2C, // Address
        0xC8, 0x04, 0x01, 0x02, 0x03, 0x04 // Unknown Tag ( CDMA SMS )
    };
    unsigned char           APDU_SetupCall_03[] = {
        0xD0, // Proactive Command
        0x47, // Length
        0x81, 0x03, 0x01, 0x10, 0x00, // Command Details
        0x82, 0x02, 0x81, 0x83, //Device Id
        0x85, 0x08, 0x4E, 0x6F, 0x74, 0x20, 0x62, 0x75, 0x73, 0x79, // Alpha Id
        0x1E, 0x02, 0x01, 0x01,  //Icon id 1
        0x85, 0x08, 0x4E, 0x6F, 0x74, 0x20, 0x62, 0x75, 0x73, 0x79, // Alpha Id
        0x1E, 0x02, 0x01, 0x02,  //Icon id 2
        0x86, 0x09, 0x91, 0x10, 0x32, 0x04, 0x21, 0x43, 0x65, 0x1C, 0x2C, // Address
        0x87, 0x02, 0x01, 0x20, // CC
        0x08, 0x02, 0x21, 0x43,  //sub address
        0x50, 0x04, 0x01, 0x02, 0x03, 0x04,  //Text Attribute 1
        0x50, 0x04, 0x01, 0x02, 0x03, 0x04,  //Text Attribute 2
        0x68, 0x01, 0x01  //Frame id
    };

    ste_parsed_apdu_t      *parsed_apdu_info_p;
    int                     APDU_Kind = STE_APDU_KIND_NONE;
    int                     APDU_Type = STE_APDU_CMD_TYPE_NONE;
    int                     current_apdu = 0;
    ste_command_result_t    ResponseData;

    ste_apdu_t             *apdu;
    ste_apdu_t             *tr_apdu;

start_loop:
    switch( current_apdu ) {
        case 0:
            apdu = ste_apdu_new(APDU_SetupCall_01, sizeof(APDU_SetupCall_01));
            break;
        case 1:
            apdu = ste_apdu_new(APDU_SetupCall_02, sizeof(APDU_SetupCall_02));
            break;
        case 2:
            apdu = ste_apdu_new(APDU_SetupCall_03, sizeof(APDU_SetupCall_03));
            break;

        default:
            goto End;
    }
    if (!apdu)
        return "ste_apdu_new failed";

    current_apdu++;
    printf("\n");
    printf
        ("############ Test SAT_APDU_TC_HandleSetupCall - %02d ############\n", current_apdu );
    //Parse an APDU string
    if (STE_SAT_APDU_ERROR_NONE !=
        ste_apdu_parse(apdu, &parsed_apdu_info_p)) {
        printf("FAILED : TC Setup Call: Parse APDU failed.\n");
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        return "ste_apdu_parse failed.";
    } else {
        //print some useful information here
        printf("Result : TC Setup Call: Parse APDU: OK\n");
    }

    //Get Kind
    APDU_Kind = ste_parsed_apdu_get_kind(parsed_apdu_info_p);
    if (STE_APDU_KIND_NONE == APDU_Kind) {
        printf("FAILED : TC Setup Call: Get APDU Kind failed.\n");
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        return "ste_apdu_get_kind failed.";
    } else {
        //print some useful information here
        printf("The Kind of APDU is: 0x%x \n", APDU_Kind);
    }

    //Get Type
    APDU_Type = ste_parsed_apdu_get_type(parsed_apdu_info_p);
    if (STE_APDU_CMD_TYPE_NONE == APDU_Type) {
        printf("FAILED : TC Setup Call: Get APDU Type failed.\n");
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        return "ste_parsed_apdu_get_type failed.";
    } else {
        //print some useful information here
        printf("The Type of APDU is: 0x%x \n", APDU_Type);
    }

    ResponseData.general_result = SAT_RES_CMD_PERFORMED_SUCCESSFULLY;
    ResponseData.command_status = STE_SAT_PC_CMD_STATUS_COMPLETED;
    ResponseData.additional_info_size = 0;
    ResponseData.additional_info_p = NULL;
    ResponseData.other_data_type = STE_CMD_RESULT_NOTHING;
    ResponseData.other_data.text_p = NULL;

    if (STE_SAT_APDU_ERROR_NONE !=
        ste_apdu_terminal_response(parsed_apdu_info_p, &ResponseData,
                                   &tr_apdu)) {
        printf("FAILED : TC Setup Call: Create response APDU failed\n");
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        return "ste_apdu_terminal_response failed.";
    } else {
        //print some useful information here
        printf("Created Response APDU: ");
        printHexAPDU(tr_apdu);
        printf("Length of Response APDU binary data: %zd \n",
               ste_apdu_get_raw_length(tr_apdu));
    }

    //delete the info structure
    ste_parsed_apdu_delete(parsed_apdu_info_p);
    ste_apdu_delete(tr_apdu);
    ste_apdu_delete(apdu);
    goto start_loop;

End:
    return 0;
}

/*************************************************************************
 * Function:      SAT_APDU_TC_HandleSendDTMF
 *
 * Description:
 *
 *************************************************************************/
const char             *SAT_APDU_TC_HandleSendDTMF()
{
    unsigned char           APDU_SendDTMF_01[] = {
        0xD0, // Proactive Command
        0x22, // Length
        0x81, 0x03, 0x01, 0x14, 0x00, // Command Details
        0x82, 0x02, 0x81, 0x83, // Device Id
        0x05, 0x03, 0x12, 0x34, 0x56, // Alpha Id
        0x2C, 0x05, 0xC1, 0xD2, 0xE3, 0x54, 0x6F, // Dtmf String (somewhat bad, should result in DTMF 1p2345 (6 should be truncated by 'bad' end marker)
        0x1E, 0x02, 0x01, 0x01, // Icon Id
        0x50, 0x04, 0x01, 0x02, 0x03, 0x04, // Text Attribute
        0x68, 0x01, 0x01 // Frame Id
    };
    unsigned char           APDU_SendDTMF_02[] = {
        0xD0, // Proactive Command
        0x28, // Length
        0x81, 0x03, 0x01, 0x14, 0x00, // Command Details
        0x82, 0x02, 0x81, 0x83, // Device Id
        0x05, 0x03, 0x12, 0x34, 0x56, // Alpha Id
        0x2C, 0x05, 0xC1, 0xD2, 0xE3, 0x54, 0x6F, // Dtmf String (somewhat bad, should result in DTMF 1p2345 (6 should be truncated by 'bad' end marker)
        0x1E, 0x02, 0x01, 0x01, // Icon Id
        0x50, 0x04, 0x01, 0x02, 0x03, 0x04, // Text Attribute
        0x68, 0x01, 0x01, // Frame Id
        0xC8, 0x04, 0x01, 0x02, 0x03, 0x04 // Unknown Tag ( CDMA SMS )
    };
    ste_parsed_apdu_t      *parsed_apdu_info_p;
    int                     APDU_Kind = STE_APDU_KIND_NONE;
    int                     APDU_Type = STE_APDU_CMD_TYPE_NONE;
    int                     current_apdu = 0;
    ste_command_result_t    ResponseData;

    ste_apdu_t             *apdu;
    ste_apdu_t             *tr_apdu;

start_loop:
    switch( current_apdu ) {
        case 0:
            apdu = ste_apdu_new(APDU_SendDTMF_01, sizeof(APDU_SendDTMF_01));
            break;
        case 1:
            apdu = ste_apdu_new(APDU_SendDTMF_02, sizeof(APDU_SendDTMF_02));
            break;
        default:
            goto End;
    }
    if (!apdu)
        return "ste_apdu_new failed";

    current_apdu++;
    printf("\n");
    printf
        ("############ Test SAT_APDU_TC_HandleSendDTMF - %02d ############\n", current_apdu );
    //Parse an APDU string
    if (STE_SAT_APDU_ERROR_NONE !=
        ste_apdu_parse(apdu, &parsed_apdu_info_p)) {
        printf("FAILED : TC Send DTMF: Parse APDU failed.\n");
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        return "ste_apdu_parse failed.";
    } else {
        //print some useful information here
        printf("Result : TC Send DTMF: Parse APDU: OK\n");
    }

    //Get Kind
    APDU_Kind = ste_parsed_apdu_get_kind(parsed_apdu_info_p);
    if (STE_APDU_KIND_NONE == APDU_Kind) {
        printf("FAILED : TC Send DTMF: Get APDU Kind failed.\n");
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        return "ste_apdu_get_kind failed.";
    } else {
        //print some useful information here
        printf("The Kind of APDU is: 0x%x \n", APDU_Kind);
    }

    //Get Type
    APDU_Type = ste_parsed_apdu_get_type(parsed_apdu_info_p);
    if (STE_APDU_CMD_TYPE_NONE == APDU_Type) {
        printf("FAILED : TC Send DTMF: Get APDU Type failed.\n");
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        return "ste_parsed_apdu_get_type failed.";
    } else {
        //print some useful information here
        printf("The Type of APDU is: 0x%x \n", APDU_Type);
    }

    ResponseData.general_result = SAT_RES_CMD_PERFORMED_SUCCESSFULLY;
    ResponseData.command_status = STE_SAT_PC_CMD_STATUS_COMPLETED;
    ResponseData.additional_info_size = 0;
    ResponseData.additional_info_p = NULL;
    ResponseData.other_data_type = STE_CMD_RESULT_NOTHING;
    ResponseData.other_data.text_p = NULL;

    if (STE_SAT_APDU_ERROR_NONE !=
        ste_apdu_terminal_response(parsed_apdu_info_p, &ResponseData,
                                   &tr_apdu)) {
        printf("FAILED : TC Send DTMF: Create response APDU failed\n");
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        return "ste_apdu_terminal_response failed.";
    } else {
        //print some useful information here
        printf("Created Response APDU: ");
        printHexAPDU(tr_apdu);
        printf("Length of Response APDU binary data: %zd \n",
               ste_apdu_get_raw_length(tr_apdu));
    }

    //delete the info structure
    ste_parsed_apdu_delete(parsed_apdu_info_p);
    ste_apdu_delete(tr_apdu);
    ste_apdu_delete(apdu);
    goto start_loop;

End:
    return 0;
}

/*************************************************************************
 * Function:      SAT_APDU_TC_HandleSendSMS
 *
 * Description:
 *
 *************************************************************************/
const char             *SAT_APDU_TC_HandleSendSMS()
{
    unsigned char           APDU_SendSMS_01[] = {
        0xD0, // Proactive Command
        0x23, // Length
        0x81, 0x03, 0x01, 0x13, 0x01, // Command Details
        0x82, 0x02, 0x81, 0x83, // Device Identities
        0x8B, 0x18, 0x01, 0x00, 0x09, 0x91, 0x10, 0x32, 0x54, 0x76, 0xF8, 0x40, 0xF4, 0x0C, 0x54, // SMS TPDU
        0x65, 0x73, 0x74, 0x20, 0x4D, 0x65, 0x73, 0x73, 0x61, 0x67, 0x65
    };
    unsigned char           APDU_SendSMS_02[] = {
        0xD0, // Proactive Command
        0x0f, // Length
        0x81, 0x03, 0x01, 0x13, 0x01, // Command Details
        0x82, 0x02, 0x81, 0x83, // Device Identities
        0xC8, 0x04, 0x01, 0x02, 0x03, 0x04 // CDMA SMS
    };
    unsigned char           APDU_SendSMS_03[] = {
        0xD0, // Proactive Command
        0x23, // Length
        0x81, 0x03, 0x01, 0x13, 0x01, // Command Details
        0x82, 0x02, 0x81, 0x83, // Device Identities
        0x8B, 0x18, 0x01, 0x00, 0x09, 0x91, 0x10, 0x32, 0x54, 0x76, 0xF8, 0x40, 0xF4, 0x0C, 0x54, // SMS TPDU
        0x65, 0x73, 0x74, 0x20, 0x4D, 0x65, 0x73, 0x73, 0x61, 0x67, 0x65,
        0x89, 0x07, 0x11, 0x2a, 0x23, 0x33, 0x33, 0x2a, 0x23 // Unknown Tag (SS String)
    };
    ste_parsed_apdu_t      *parsed_apdu_info_p;
    int                     APDU_Kind = STE_APDU_KIND_NONE;
    int                     APDU_Type = STE_APDU_CMD_TYPE_NONE;
    int                     current_apdu = 0;
    ste_command_result_t    ResponseData;

    ste_apdu_t             *apdu;
    ste_apdu_t             *tr_apdu;

start_loop:
    switch( current_apdu ) {
        case 0:
            apdu = ste_apdu_new(APDU_SendSMS_01, sizeof(APDU_SendSMS_01));
            break;
        case 1:
            apdu = ste_apdu_new(APDU_SendSMS_02, sizeof(APDU_SendSMS_02));
            break;
        case 2:
            apdu = ste_apdu_new(APDU_SendSMS_03, sizeof(APDU_SendSMS_03));
            break;
        default:
            goto End;
    }
    if (!apdu)
        return "ste_apdu_new failed";

    current_apdu++;
    printf("\n");
    printf
        ("############ Test SAT_APDU_TC_HandleSendSMS - %02d ############\n", current_apdu );
    //Parse an APDU string
    if (STE_SAT_APDU_ERROR_NONE !=
        ste_apdu_parse(apdu, &parsed_apdu_info_p)) {
        printf("FAILED : TC Send SMS: Parse APDU failed.\n");
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        return "ste_apdu_parse failed.";
    } else {
        //print some useful information here
        printf("Result : TC Send SMS: Parse APDU: OK\n");
    }

    //Get Kind
    APDU_Kind = ste_parsed_apdu_get_kind(parsed_apdu_info_p);
    if (STE_APDU_KIND_NONE == APDU_Kind) {
        printf("FAILED : TC Send SMS: Get APDU Kind failed.\n");
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        return "ste_apdu_get_kind failed.";
    } else {
        //print some useful information here
        printf("The Kind of APDU is: 0x%x \n", APDU_Kind);
    }

    //Get Type
    APDU_Type = ste_parsed_apdu_get_type(parsed_apdu_info_p);
    if (STE_APDU_CMD_TYPE_NONE == APDU_Type) {
        printf("FAILED : TC Send SMS: Get APDU Type failed.\n");
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        return "ste_parsed_apdu_get_type failed.";
    } else {
        //print some useful information here
        printf("The Type of APDU is: 0x%x \n", APDU_Type);
    }

    ResponseData.general_result = SAT_RES_CMD_PERFORMED_SUCCESSFULLY;
    ResponseData.command_status = STE_SAT_PC_CMD_STATUS_COMPLETED;
    ResponseData.additional_info_size = 0;
    ResponseData.additional_info_p = NULL;
    ResponseData.other_data_type = STE_CMD_RESULT_NOTHING;
    ResponseData.other_data.text_p = NULL;

    if (STE_SAT_APDU_ERROR_NONE !=
        ste_apdu_terminal_response(parsed_apdu_info_p, &ResponseData,
                                   &tr_apdu)) {
        printf("FAILED : TC Send SMS: Create response APDU failed\n");
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        return "ste_apdu_terminal_response failed.";
    } else {
        //print some useful information here
        printf("Created Response APDU: ");
        printHexAPDU(tr_apdu);
        printf("Length of Response APDU binary data: %zd \n",
               ste_apdu_get_raw_length(tr_apdu));
    }

    //delete the info structure
    ste_parsed_apdu_delete(parsed_apdu_info_p);
    ste_apdu_delete(tr_apdu);
    ste_apdu_delete(apdu);
    goto start_loop;

End:
    return 0;
}

/*************************************************************************
 * Function:      SAT_APDU_TC_HandleLanguageNotification
 *
 * Description:
 *
 *************************************************************************/
const char             *SAT_APDU_TC_HandleLanguageNotification()

{
    unsigned char           APDU_LanguageNotification_01[] = {
        0xD0, // Proactive Command
        0x0D, // Length
        0x81, 0x03, 0x01, 0x35, 0x01, // Command Details
        0x82, 0x02, 0x81, 0x82, // Device ID
        0xAD, 0x02, 0x7A, 0x68 // Language
    };
    unsigned char           APDU_LanguageNotification_02[] = {
        0xD0, // Proactive Command
        0x13, // Length
        0x81, 0x03, 0x01, 0x35, 0x01, // Command Details
        0x82, 0x02, 0x81, 0x82, // Device ID
        0xAD, 0x02, 0x7A, 0x68, // Language
        0xC8, 0x04, 0x01, 0x02, 0x03, 0x04 // Unknown Tag ( CDMA SMS )
    };
    ste_parsed_apdu_t      *parsed_apdu_info_p;
    int                     APDU_Kind = STE_APDU_KIND_NONE;
    int                     APDU_Type = STE_APDU_CMD_TYPE_NONE;
    int                     current_apdu = 0;
    ste_command_result_t    ResponseData;

    ste_apdu_t             *apdu;
    ste_apdu_t             *tr_apdu;

start_loop:
    switch( current_apdu ) {
        case 0:
            apdu = ste_apdu_new(APDU_LanguageNotification_01, sizeof(APDU_LanguageNotification_01));
            break;
        case 1:
            apdu = ste_apdu_new(APDU_LanguageNotification_02, sizeof(APDU_LanguageNotification_02));
            break;
        default:
            goto End;
    }
    if (!apdu)
        return "ste_apdu_new failed";

    current_apdu++;
    printf("\n");
    printf
        ("############ Test SAT_APDU_TC_HandleLanguageNotification - %02d ############\n", current_apdu );
    //Parse an APDU string
    if (STE_SAT_APDU_ERROR_NONE !=
        ste_apdu_parse(apdu, &parsed_apdu_info_p)) {
        printf("FAILED : TC Language Notify: Parse APDU failed.\n");
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        return "ste_apdu_parse failed.";
    } else {
        //print some useful information here
        printf("Result : TC Language Notify: Parse APDU: OK\n");
    }

    //Get Kind
    APDU_Kind = ste_parsed_apdu_get_kind(parsed_apdu_info_p);
    if (STE_APDU_KIND_NONE == APDU_Kind) {
        printf("FAILED : TC Language Notify: Get APDU Kind failed.\n");
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        return "ste_apdu_get_kind failed.";
    } else {
        //print some useful information here
        printf("The Kind of APDU is: 0x%x \n", APDU_Kind);
    }

    //Get Type
    APDU_Type = ste_parsed_apdu_get_type(parsed_apdu_info_p);
    if (STE_APDU_CMD_TYPE_NONE == APDU_Type) {
        printf("FAILED : TC Language Notify: Get APDU Type failed.\n");
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        return "ste_parsed_apdu_get_type failed.";
    } else {
        //print some useful information here
        printf("The Type of APDU is: 0x%x \n", APDU_Type);
    }

    ResponseData.general_result = SAT_RES_CMD_PERFORMED_SUCCESSFULLY;
    ResponseData.command_status = STE_SAT_PC_CMD_STATUS_COMPLETED;
    ResponseData.additional_info_size = 0;
    ResponseData.additional_info_p = NULL;
    ResponseData.other_data_type = STE_CMD_RESULT_NOTHING;
    ResponseData.other_data.text_p = NULL;

    if (STE_SAT_APDU_ERROR_NONE !=
        ste_apdu_terminal_response(parsed_apdu_info_p, &ResponseData,
                                   &tr_apdu)) {
        printf("FAILED : TC Language Notify: Create response APDU failed\n");
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        return "ste_apdu_terminal_response failed.";
    } else {
        //print some useful information here
        printf("Created Response APDU: ");
        printHexAPDU(tr_apdu);
        printf("Length of Response APDU binary data: %zd \n",
               ste_apdu_get_raw_length(tr_apdu));
    }

    //delete the info structure
    ste_parsed_apdu_delete(parsed_apdu_info_p);
    ste_apdu_delete(tr_apdu);
    ste_apdu_delete(apdu);
    goto start_loop;

End:
    return 0;
}

/*************************************************************************
 * Function:      SAT_APDU_TC_HandleWrongAPDU
 *
 * Description:
 *
 *************************************************************************/
const char             *SAT_APDU_TC_HandleWrongAPDU()
{
    unsigned char           APDU_01[] = { /* Wrong Len */
        0xD0, // Proactive Command
        0x82, // Wrong Length
        0x81, 0x03, 0x01, 0x21, 0x00, // Command Details
        0x82, 0x02, 0x81, 0x02, // Device Id
        0x8D, 0x04, 0x04, 0x43, 0x41, 0x54, // Text String
    };
    unsigned char           APDU_02[] = { /* Wrong Two Byte Len */
        0xD0, // Proactive Command
        0x81, // Two Byte ind
        0x00, // Length
        0x81, 0x03, 0x01, 0x21, 0x00, // Command Details
        0x82, 0x02, 0x81, 0x02, // Device Id
        0x8D, 0x04, 0x04, 0x43, 0x41, 0x54, // Text String
    };
    unsigned char           APDU_03[] = { /* No Command Details */
        0xD0, // Proactive Command
        0x0a, // Length
        0x82, 0x02, 0x81, 0x02, // Device Id
        0x8D, 0x04, 0x04, 0x43, 0x41, 0x54, // Text String
    };
    unsigned char           APDU_04[] = { /* Wrong Len Command Details */
        0xD0, // Proactive Command
        0x0e, // Length
        0x81, 0x02, 0x01, 0x21, // Command Details
        0x82, 0x02, 0x81, 0x02, // Device Id
        0x8D, 0x04, 0x04, 0x43, 0x41, 0x54, // Text String
    };
    unsigned char           APDU_05[] = { /* Incomplete Display Text */
        0xD0, // Proactive Command
        0x05, // Length
        0x81, 0x03, 0x01, 0x21, 0x00 // Command Details
    };
    unsigned char           APDU_06[] = { /* Wrong Dev ID Len in Display Text */
        0xD0, // Proactive Command
        0x0f, // Length
        0x81, 0x03, 0x01, 0x21, 0x00, // Command Details
        0x82, 0x82, 0x81, 0x02, // Device Id
        0x8D, 0x04, 0x04, 0x43, 0x41, 0x54, // Text String
    };
    unsigned char           APDU_07[] = { /* Wrong Dev Id in Display Text */
        0xD0, // Proactive Command
        0x0f, // Length
        0x81, 0x03, 0x01, 0x21, 0x00, // Command Details
        0x82, 0x02, 0x81, 0x82, // Device Id
        0x8D, 0x04, 0x04, 0x43, 0x41, 0x54, // Text String
    };
    unsigned char           APDU_08[] = { /* Incomplete Provide IMEI */
        0xD0, // Proactive Command
        0x05, // Len
        0x81, 0x03, 0x01, 0x26, 0x01, // Command Details
    };
    unsigned char           APDU_09[] = { /* Wrong Dev Id Len in Provide IMEI */
        0xD0, // Proactive Command
        0x09, // Len
        0x81, 0x03, 0x01, 0x26, 0x01, // Command Details
        0x82, 0x82, 0x81, 0x82 // Device ID
    };
    unsigned char           APDU_10[] = { /* Wrong Dev ID in Provide IMEI */
        0xD0, // Proactive Command
        0x09, // Len
        0x81, 0x03, 0x01, 0x26, 0x01, // Command Details
        0x82, 0x02, 0x81, 0x02 // Device ID
    };
    unsigned char           APDU_11[] = { /* Incomplete Setup Event List */
        0xD0, // Proactive Command
        0x05, // Length
        0x81, 0x03, 0x01, 0x05, 0x00, // Command Details
    };
    unsigned char           APDU_12[] = { /* Wrong Dev Id Len in Setup Event List */
        0xD0, // Proactive Command
        0x11, // Length
        0x81, 0x03, 0x01, 0x05, 0x00, // Command Details
        0x82, 0x82, 0x81, 0x82, // Device Id
        0x99, 0x06, 0x00, 0x05, 0x02, 0x0F, 0x10, 0x13 // Event List
    };
    unsigned char           APDU_13[] = { /* Wrong Dev Id in Setup Event List */
        0xD0, // Proactive Command
        0x11, // Length
        0x81, 0x03, 0x01, 0x05, 0x00, // Command Details
        0x82, 0x02, 0x81, 0x02, // Device Id
        0x99, 0x06, 0x00, 0x05, 0x02, 0x0F, 0x10, 0x13 // Event List
    };
    unsigned char           APDU_14[] = { /* Incomplete Setup Menu */
        0xD0, // Proactive Command
        0x05, // Length
        0x81, 0x03, 0x01, 0x25, 0x00 // Command Details
    };
    unsigned char           APDU_15[] = { /* Wrong Dev Id Len in Setup Menu */
        0xD0, // Proactive Command
        0x2f, // Length
        0x81, 0x03, 0x01, 0x25, 0x00, // Command Details
        0x82, 0x82, 0x81, 0x02, // Device Id
        0x85, 0x0C, 0x54, 0x6F, 0x6F, 0x6C, 0x6B, 0x69, 0x74, 0x20, 0x4D, 0x65, 0x6E, 0x75, // Alpha Id
        0x8F, 0x07, 0x01, 0x49, 0x74, 0x65, 0x6D, 0x20, 0x31, // Item
        0x8F, 0x07, 0x02, 0x49, 0x74, 0x65, 0x6D, 0x20, 0x32, // Item
        0x18, 0x04, 0x13, 0x10, 0x15, 0x26 // Next Action Ind
    };
    unsigned char           APDU_16[] = { /* Wrong Dev Id in Setup Menu */
        0xD0, // Proactive Command
        0x2f, // Length
        0x81, 0x03, 0x01, 0x25, 0x00, // Command Details
        0x82, 0x02, 0x81, 0x82, // Device Id
        0x85, 0x0C, 0x54, 0x6F, 0x6F, 0x6C, 0x6B, 0x69, 0x74, 0x20, 0x4D, 0x65, 0x6E, 0x75, // Alpha Id
        0x8F, 0x07, 0x01, 0x49, 0x74, 0x65, 0x6D, 0x20, 0x31, // Item
        0x8F, 0x07, 0x02, 0x49, 0x74, 0x65, 0x6D, 0x20, 0x32, // Item
        0x18, 0x04, 0x13, 0x10, 0x15, 0x26 // Next Action Ind
    };
    unsigned char           APDU_17[] = { /* Incomplete Get Inkey */
        0xD0, // Proactive Command
        0x05, // Length
        0x81, 0x03, 0x01, 0x22, 0x02, // Command Details
    };
    unsigned char           APDU_18[] = { /* Wrong Dev Len in Get Inkey */
        0xD0, // Proactive Command
        0x22, // Length
        0x81, 0x03, 0x01, 0x22, 0x02, // Command Details
        0x82, 0x82, 0x81, 0x02, // Device Id
        0x8D, 0x0F, 0x04, 0x45, 0x6E, 0x74, 0x65, 0x72, 0x20, 0x61, // Text String
        0x20, 0x64, 0x69, 0x67, 0x69, 0x74, 0x3A,
        0x1E, 0x02, 0x00, 0x08, // Icon ID
        0x84, 0x02, 0x00, 0x01 // Duration in Min
    };
    unsigned char           APDU_19[] = { /* Wrong Device Id in Get Inkey */
        0xD0, // Proactive Command
        0x22, // Length
        0x81, 0x03, 0x01, 0x22, 0x02, // Command Details
        0x82, 0x02, 0x81, 0x82, // Device Id
        0x8D, 0x0F, 0x04, 0x45, 0x6E, 0x74, 0x65, 0x72, 0x20, 0x61, // Text String
        0x20, 0x64, 0x69, 0x67, 0x69, 0x74, 0x3A,
        0x1E, 0x02, 0x00, 0x08, // Icon ID
        0x84, 0x02, 0x00, 0x01 // Duration in Min
    };
    unsigned char           APDU_20[] = { /* Incomplete Get Input */
        0xD0, // Proactive Command
        0x05, // Length
        0x81, 0x03, 0x01, 0x23, 0x00, // Command Details
    };
    unsigned char           APDU_21[] = { /* Wrong Device Len in Get Input */
        0xD0, // Proactive Command
        0x2a, // Length
        0x81, 0x03, 0x01, 0x23, 0x00, // Command Details
        0x82, 0x82, 0x81, 0x82, // Device Identities
        0x8D, 0x0C, 0x04, 0x45, 0x6E, 0x74, 0x65, 0x72, 0x20, 0x31, 0x32, // Text String
        0x33, 0x34, 0x35,
        0x91, 0x02, 0x05, 0x05, // Response Length
        0x17, 0x02, 0x00, 0x65, // Default Text
        0x1E, 0x02, 0x01, 0x01, //Icon id
        0x50, 0x04, 0x01, 0x02, 0x03, 0x04, // Text Attribute
        0x68, 0x01, 0x01 //Frame id
    };
    unsigned char           APDU_22[] = { /* Wrong Device Id in Get Input */
        0xD0, // Proactive Command
        0x2a, // Length
        0x81, 0x03, 0x01, 0x23, 0x00, // Command Details
        0x82, 0x02, 0x81, 0x02, // Device Identities
        0x8D, 0x0C, 0x04, 0x45, 0x6E, 0x74, 0x65, 0x72, 0x20, 0x31, 0x32, // Text String
        0x33, 0x34, 0x35,
        0x91, 0x02, 0x05, 0x05, // Response Length
        0x17, 0x02, 0x00, 0x65, // Default Text
        0x1E, 0x02, 0x01, 0x01, //Icon id
        0x50, 0x04, 0x01, 0x02, 0x03, 0x04, // Text Attribute
        0x68, 0x01, 0x01 //Frame id
    };
    unsigned char           APDU_23[] = { /* Incomplete More Time */
        0xD0, // Proactive Command
        0x05, // Length
        0x81, 0x03, 0x01, 0x02, 0x00, // Command Details
    };
    unsigned char           APDU_24[] = { /* Wrong Device Len in More Time */
        0xD0, // Proactive Command
        0x09, // Length
        0x81, 0x03, 0x01, 0x02, 0x00, // Command Details
        0x82, 0x82, 0x81, 0x82 // Device Id
    };
    unsigned char           APDU_25[] = { /* Wrong Device Id in More Time */
        0xD0, // Proactive Command
        0x09, // Length
        0x81, 0x03, 0x01, 0x02, 0x00, // Command Details
        0x82, 0x02, 0x81, 0x02 // Device Id
    };
    unsigned char           APDU_26[] = { /* Incomplete Poll Interval */
        0xD0, // proactive Command
        0x05, // Length
        0x81, 0x03, 0x01, 0x03, 0x00, // Command Details
    };
    unsigned char           APDU_27[] = { /* Wrong Device Len in Poll Interval */
        0xD0, // proactive Command
        0x0D, // Length
        0x81, 0x03, 0x01, 0x03, 0x00, // Command Details
        0x82, 0x82, 0x81, 0x82, // Device Id
        0x84, 0x02, 0x00, 0x01 // Duration
    };
    unsigned char           APDU_28[] = { /* Wrong Device Id in Poll Interval */
        0xD0, // proactive Command
        0x0D, // Length
        0x81, 0x03, 0x01, 0x03, 0x00, // Command Details
        0x82, 0x02, 0x81, 0x02, // Device Id
        0x84, 0x02, 0x00, 0x01 // Duration
    };
    unsigned char           APDU_29[] = { /* Incomplete Polling Off */
        0xD0, // Proactive Command
        0x05, // Length
        0x81, 0x03, 0x01, 0x04, 0x00, // Command Details
    };
    unsigned char           APDU_30[] = { /* Wrong Device Len in Polling Off */
        0xD0, // Proactive Command
        0x09, // Length
        0x81, 0x03, 0x01, 0x04, 0x00, // Command Details
        0x82, 0x82, 0x81, 0x82 // Device Id
    };
    unsigned char           APDU_31[] = { /* Wrong Device Id in Polling Off */
        0xD0, // Proactive Command
        0x09, // Length
        0x81, 0x03, 0x01, 0x04, 0x00, // Command Details
        0x82, 0x02, 0x81, 0x02 // Device Id
    };
    unsigned char           APDU_32[] = /* Incomplete Timer Management - Start Timer */
    {
        0xD0, // Proactive Command
        0x05, // Length
        0x81, 0x03, 0x01, 0x27, 0x00, // Command Details
    };
    unsigned char           APDU_33[] = /* Wrong Device Len in Timer Management - Start Timer */
    {
        0xD0, // Proactive Command
        0x11, // Length
        0x81, 0x03, 0x01, 0x27, 0x00, // Command Details
        0x82, 0x82, 0x81, 0x02, // Device Id
        0xA4, 0x01, 0x01, // Timer Id
        0xA5, 0x03, 0x00, 0x50, 0x00 // Timer Val
    };
    unsigned char           APDU_34[] = /* Wrong Device ID in Timer Management - Start Timer */
    {
        0xD0, // Proactive Command
        0x11, // Length
        0x81, 0x03, 0x01, 0x27, 0x00, // Command Details
        0x82, 0x02, 0x81, 0x82, // Device Id
        0xA4, 0x01, 0x01, // Timer Id
        0xA5, 0x03, 0x00, 0x50, 0x00 // Timer Val
    };
    unsigned char           APDU_35[] = { /* Incomplete Refresh */
        0xD0, // Proactive Command
        0x05, // Length
        0x81, 0x03, 0x01, 0x01, 0x02, //Command Details
    };
    unsigned char           APDU_36[] = { /* Wrong Device Len in Refresh */
        0xD0, // Proactive Command
        0x33, // Length
        0x81, 0x03, 0x01, 0x01, 0x02, //Command Details
        0x82, 0x82, 0x81, 0x82, //Device Id
        0x92, 0x0f, 0x02, 0x3F, 0x00, 0x7f, 0x66, 0x5F, 0x30, // File List
        0x4F, 0x36, 0x3F, 0x00, 0x7F, 0x20, 0x6F, 0x46,
        0xAF, 0x01, 0x01, //AID
        0x05,0x07,0x52,0x65,0x66,0x72,0x65,0x73,0x68, // Alpha Id
        0x1E,0x02,0x01,0x01, // Icon Id
        0x50,0x04,0x01,0x02,0x03,0x04, // Text Attribute
        0x68,0x01,0x01 //Frame id
    };
    unsigned char           APDU_37[] = { /* Wrong Device Id in Refresh */
        0xD0, // Proactive Command
        0x33, // Length
        0x81, 0x03, 0x01, 0x01, 0x02, //Command Details
        0x82, 0x02, 0x81, 0x02, //Device Id
        0x92, 0x0f, 0x02, 0x3F, 0x00, 0x7f, 0x66, 0x5F, 0x30, // File List
        0x4F, 0x36, 0x3F, 0x00, 0x7F, 0x20, 0x6F, 0x46,
        0xAF, 0x01, 0x01, //AID
        0x05,0x07,0x52,0x65,0x66,0x72,0x65,0x73,0x68, // Alpha Id
        0x1E,0x02,0x01,0x01, // Icon Id
        0x50,0x04,0x01,0x02,0x03,0x04, // Text Attribute
        0x68,0x01,0x01 //Frame id
    };
    unsigned char           APDU_38[] = { /* Incomplete Setup Call */
        0xD0, // Proactive Command
        0x05, // Length
        0x81, 0x03, 0x01, 0x10, 0x00, // Command Details
    };
    unsigned char           APDU_39[] = { /* Wrong Device Len in Setup Call */
        0xD0, // Proactive Command
        0x1E, // Length
        0x81, 0x03, 0x01, 0x10, 0x00, // Command Details
        0x82, 0x82, 0x81, 0x83, //Device Id
        0x85, 0x08, 0x4E, 0x6F, 0x74, 0x20, 0x62, 0x75, 0x73, 0x79, // Alpha Id
        0x86, 0x09, 0x91, 0x10, 0x32, 0x04, 0x21, 0x43, 0x65, 0x1C, 0x2C // Address
    };
    unsigned char           APDU_40[] = { /* Wrong Device Id in Setup Call */
        0xD0, // Proactive Command
        0x1E, // Length
        0x81, 0x03, 0x01, 0x10, 0x00, // Command Details
        0x82, 0x02, 0x81, 0x03, //Device Id
        0x85, 0x08, 0x4E, 0x6F, 0x74, 0x20, 0x62, 0x75, 0x73, 0x79, // Alpha Id
        0x86, 0x09, 0x91, 0x10, 0x32, 0x04, 0x21, 0x43, 0x65, 0x1C, 0x2C // Address
    };
    unsigned char           APDU_41[] = { /* Incomplete Send SS */
        0xD0, // Proactive Command
        0x05, // Length
        0x81, 0x03, 0x01, 0x11, 0x00, // Command Details
    };
    unsigned char           APDU_42[] = { /* Wrong Device Len in Send SS */
        0xD0, // Proactive Command
        0x12, // Length
        0x81, 0x03, 0x01, 0x11, 0x00, // Command Details
        0x82, 0x82, 0x81, 0x83, // Device Id
        0x89, 0x07, 0x11, 0x2a, 0x23, 0x33, 0x33, 0x2a, 0x23 // SS String
    };
    unsigned char           APDU_43[] = { /* Wrong Device ID in Send SS */
        0xD0, // Proactive Command
        0x12, // Length
        0x81, 0x03, 0x01, 0x11, 0x00, // Command Details
        0x82, 0x02, 0x81, 0x03, // Device Id
        0x89, 0x07, 0x11, 0x2a, 0x23, 0x33, 0x33, 0x2a, 0x23 // SS String
    };
    unsigned char           APDU_44[] = { /* Incomplete Send USSD */
        0xD0, // Proactive Command
        0x05, // Length
        0x81, 0x03, 0x01, 0x12, 0x00 // Command Details
    };
    unsigned char           APDU_45[] = { /* Wrong Device Len in Send USSD */
        0xD0, // Proactive Command
        0x13, // Length
        0x81, 0x03, 0x01, 0x12, 0x00, // Command Details
        0x82, 0x82, 0x81, 0x83, // Device Id
        0x8A, 0x08, 0x0F, 0xAA, 0x98, 0x6D, 0xA6, 0x8A, 0xC5, 0x46 // USSD String
    };
    unsigned char           APDU_46[] = { /* Wrong Device ID in Send USSD */
        0xD0, // Proactive Command
        0x13, // Length
        0x81, 0x03, 0x01, 0x12, 0x00, // Command Details
        0x82, 0x02, 0x81, 0x03, // Device Id
        0x8A, 0x08, 0x0F, 0xAA, 0x98, 0x6D, 0xA6, 0x8A, 0xC5, 0x46 // USSD String
    };
    unsigned char           APDU_47[] = { /* Incomplete Send SMS */
        0xD0, // Proactive Command
        0x05, // Length
        0x81, 0x03, 0x01, 0x13, 0x01, // Command Details
    };
    unsigned char           APDU_48[] = { /* Wrong Device Len in Send SMS */
        0xD0, // Proactive Command
        0x23, // Length
        0x81, 0x03, 0x01, 0x13, 0x01, // Command Details
        0x82, 0x82, 0x81, 0x83, // Device Identities
        0x8B, 0x18, 0x01, 0x00, 0x09, 0x91, 0x10, 0x32, 0x54, 0x76, 0xF8, 0x40, 0xF4, 0x0C, 0x54, // SMS TPDU
        0x65, 0x73, 0x74, 0x20, 0x4D, 0x65, 0x73, 0x73, 0x61, 0x67, 0x65
    };
    unsigned char           APDU_49[] = { /* Wrong Device ID in Send SMS */
        0xD0, // Proactive Command
        0x23, // Length
        0x81, 0x03, 0x01, 0x13, 0x01, // Command Details
        0x82, 0x02, 0x81, 0x03, // Device Identities
        0x8B, 0x18, 0x01, 0x00, 0x09, 0x91, 0x10, 0x32, 0x54, 0x76, 0xF8, 0x40, 0xF4, 0x0C, 0x54, // SMS TPDU
        0x65, 0x73, 0x74, 0x20, 0x4D, 0x65, 0x73, 0x73, 0x61, 0x67, 0x65
    };
    unsigned char           APDU_50[] = { /* Incomplete Send DTMF */
        0xD0, // Proactive Command
        0x05, // Length
        0x81, 0x03, 0x01, 0x14, 0x00, // Command Details
    };
    unsigned char           APDU_51[] = { /* Wrong Device Len in Send DTMF */
        0xD0, // Proactive Command
        0x22, // Length
        0x81, 0x03, 0x01, 0x14, 0x00, // Command Details
        0x82, 0x82, 0x81, 0x83, // Device Id
        0x05, 0x03, 0x12, 0x34, 0x56, // Alpha Id
        0x2C, 0x05, 0xC1, 0xD2, 0xE3, 0x54, 0x6F, // Dtmf String (somewhat bad, should result in DTMF 1p2345 (6 should be truncated by 'bad' end marker)
        0x1E, 0x02, 0x01, 0x01, // Icon Id
        0x50, 0x04, 0x01, 0x02, 0x03, 0x04, // Text Attribute
        0x68, 0x01, 0x01 // Frame Id
    };
    unsigned char           APDU_52[] = { /* Wrong Device ID in Send DTMF */
        0xD0, // Proactive Command
        0x22, // Length
        0x81, 0x03, 0x01, 0x14, 0x00, // Command Details
        0x82, 0x02, 0x81, 0x03, // Device Id
        0x05, 0x03, 0x12, 0x34, 0x56, // Alpha Id
        0x2C, 0x05, 0xC1, 0xD2, 0xE3, 0x54, 0x6F, // Dtmf String (somewhat bad, should result in DTMF 1p2345 (6 should be truncated by 'bad' end marker)
        0x1E, 0x02, 0x01, 0x01, // Icon Id
        0x50, 0x04, 0x01, 0x02, 0x03, 0x04, // Text Attribute
        0x68, 0x01, 0x01 // Frame Id
    };
    unsigned char           APDU_53[] = { /* Incomplete Language Notification */
        0xD0, // Proactive Command
        0x05, // Length
        0x81, 0x03, 0x01, 0x35, 0x01, // Command Details
    };
    unsigned char           APDU_54[] = { /* Wrong Device Len in Language Notification */
        0xD0, // Proactive Command
        0x0D, // Length
        0x81, 0x03, 0x01, 0x35, 0x01, // Command Details
        0x82, 0x82, 0x81, 0x82, // Device ID
        0xAD, 0x02, 0x7A, 0x68 // Language
    };
    unsigned char           APDU_55[] = { /* Wrong Device ID in Language Notification */
        0xD0, // Proactive Command
        0x0D, // Length
        0x81, 0x03, 0x01, 0x35, 0x01, // Command Details
        0x82, 0x02, 0x81, 0x02, // Device ID
        0xAD, 0x02, 0x7A, 0x68 // Language
    };
    unsigned char           APDU_56[] = { /* Wrong Command Tag */
        0x52,
        0x04,
        0x82, 0x02, 0x81, 0x02
    };
    ste_parsed_apdu_t      *parsed_apdu_info_p = NULL;
    ste_apdu_t             *apdu;
    ste_sat_apdu_error_t   ret_code = STE_SAT_APDU_ERROR_NONE;
    int                    current_apdu = 0;
    char                   *TC_Ids[] = {
        "SAT_APDU_TC_HandleProactiveCommand_Wrong_Len",
        "SAT_APDU_TC_HandleProactiveCommand_Wrong_TwoByte_Len",
        "SAT_APDU_TC_HandleProactiveCommand_No_Command_Details",
        "SAT_APDU_TC_HandleProactiveCommand_Command_Details_with_WrongLen",
        "SAT_APDU_TC_HandleDisplayText_IncompletePDU",
        "SAT_APDU_TC_HandleDisplayText_WrongDevLen",
        "SAT_APDU_TC_HandleDisplayText_Wrong_DevID",
        "SAT_APDU_TC_HandleProvideIMEI_IncompletePDU",
        "SAT_APDU_TC_HandleProvideIMEI_WrongDevLen",
        "SAT_APDU_TC_HandleProvideIMEI_Wrong_DevID",
        "SAT_APDU_TC_HandleSetupEventList_IncompletePDU",
        "SAT_APDU_TC_HandleSetupEventList_WrongDevLen",
        "SAT_APDU_TC_HandleSetupEventList_Wrong_DevID",
        "SAT_APDU_TC_HandleSetupMenu_IncompletePDU",
        "SAT_APDU_TC_HandleSetupMenu_WrongDevLen",
        "SAT_APDU_TC_HandleSetupMenu_Wrong_DevID",
        "SAT_APDU_TC_HandleGetInkey_IncompletePDU",
        "SAT_APDU_TC_HandleGetInkey_WrongDevLen",
        "SAT_APDU_TC_HandleGetInkey_WrongDevID",
        "SAT_APDU_TC_HandleGetInput_IncompletePDU",
        "SAT_APDU_TC_HandleGetInput_WrongDevLen",
        "SAT_APDU_TC_HandleGetInput_WrongDevID",
        "SAT_APDU_TC_HandleMoreTime_IncompletePDU",
        "SAT_APDU_TC_HandleMoreTime_WrongDevLen",
        "SAT_APDU_TC_HandleMoreTime_WrongDevID",
        "SAT_APDU_TC_HandlePollInterval_IncompletePDU",
        "SAT_APDU_TC_HandlePollInterval_WrongDevLen",
        "SAT_APDU_TC_HandlePollInterval_WrongDevID",
        "SAT_APDU_TC_HandlePollingOff_IncompletePDU",
        "SAT_APDU_TC_HandlePollingOff_WrongDevLen",
        "SAT_APDU_TC_HandlePollingOff_WrongDevID",
        "SAT_APDU_TC_HandleTimerManagement_IncompletePDU",
        "SAT_APDU_TC_HandleTimerManagement_WrongDevLen",
        "SAT_APDU_TC_HandleTimerManagement_WrongDevID",
        "SAT_APDU_TC_HandleRefresh_IncompletePDU",
        "SAT_APDU_TC_HandleRefresh_WrongDevLen",
        "SAT_APDU_TC_HandleRefresh_WrongDevID",
        "SAT_APDU_TC_HandleSetupCall_IncomlpetePDU",
        "SAT_APDU_TC_HandleSetupCall_WrongDevLen",
        "SAT_APDU_TC_HandleSetupCall_WrongDevID",
        "SAT_APDU_TC_HandleSendSS_IncomlpetePDU",
        "SAT_APDU_TC_HandleSendSS_WrongDevLen",
        "SAT_APDU_TC_HandleSendSS_WrongDevID",
        "SAT_APDU_TC_HandleSendUssd_IncompletePDU",
        "SAT_APDU_TC_HandleSendUssd_WrongDevLen",
        "SAT_APDU_TC_HandleSendUssd_WrongDevID",
        "SAT_APDU_TC_HandleSendSMS_IncompletePDU",
        "SAT_APDU_TC_HandleSendSMS_WrongDevLen",
        "SAT_APDU_TC_HandleSendSMS_WrongDevID",
        "SAT_APDU_TC_HandleSendDTMF_IncompletePDU",
        "SAT_APDU_TC_HandleSendDTMF_WrongDevLen",
        "SAT_APDU_TC_HandleSendDTMF_WrongDevID",
        "SAT_APDU_TC_HandleLanguageNotification_IncompletePDU",
        "SAT_APDU_TC_HandleLanguageNotification_WrongDevLen",
        "SAT_APDU_TC_HandleLanguageNotification_WrongDevID",
        "SAT_APDU_TC_HandleWrongCommandTag"
    };
    char                   *TC_Names[] = {
        "TC Proactive Command Wrong Len",
        "TC Proactive Command Wrong Two Byte Len",
        "TC Proactive Command No Command Details",
        "TC Proactive Command Wrong Command Details Len",
        "TC Display Text Incomplete PDU",
        "TC Display Text Wrong Dev Len",
        "TC Display Text Wrong Dev ID",
        "TC Provide IMEI Incomplete PDU",
        "TC Provide IMEI Wrong Dev Len",
        "TC Provide IMEI Wrong Dev ID",
        "TC Set Up Event List Incomplete PDU",
        "TC Set Up Event List Wrong Dev Len",
        "TC Set Up Event List Wrong Dev ID",
        "TC SetUp Menu Incomplete PDU",
        "TC SetUp Menu Wrong Dev Len",
        "TC SetUp Menu Wrong Dev ID",
        "TC Get Inkey Incomplete PDU",
        "TC Get Inkey Wrong Dev Len",
        "TC Get Inkey Wrong Dev ID",
        "TC Get Input Incomplete PDU",
        "TC Get Input Wrong Dev Len",
        "TC Get Input Wrong Dev ID",
        "TC More Time Incomplete PDU",
        "TC More Time Wrong Dev Len",
        "TC More Time Wrong Dev ID",
        "TC Poll Interval Incomplete PDU",
        "TC Poll Interval Wrong Dev Len",
        "TC Poll Interval Wrong Dev ID",
        "TC Polling Off Incomplete PDU",
        "TC Polling Off Wrong Dev Len",
        "TC Polling Off Wrong Dev ID",
        "TC Timer Management Incomplete PDU",
        "TC Timer Management Wrong Dev Len",
        "TC Timer Management Wrong Dev ID",
        "TC Refresh Incomplete PDU",
        "TC Refresh Wrong Dev Len",
        "TC Refresh Wrong Dev ID",
        "TC Setup Call Incomplete PDU",
        "TC Setup Call Wrong Dev Len",
        "TC Setup Call Wrong Dev ID",
        "TC Send SS Incomlpete PDU",
        "TC Send SS Wrong Dev Len",
        "TC Send SS Wrong Dev ID",
        "TC Send USSD Incomlpete PDU",
        "TC Send USSD Wrong Dev Len",
        "TC Send USSD Wrong Dev ID",
        "TC Send SMS Incomplete PDU",
        "TC Send SMS Wrong Dev Len",
        "TC Send SMS Wrong Dev ID",
        "TC Send DTMF Incomplete PDU",
        "TC Send DTMF Wrong Dev Len",
        "TC Send DTMF Wrong Dev ID",
        "TC Language Notify Incomplete PDU",
        "TC Language Notify Wrong Dev Len",
        "TC Language Notify Wrong Dev ID",
        "TC Wrong Command Tag"
    };

start_loop:
    switch( current_apdu ) {
        case 0:
            apdu = ste_apdu_new(APDU_01, sizeof(APDU_01));
            ret_code = STE_SAT_APDU_ERROR_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME;
            break;
        case 1:
            apdu = ste_apdu_new(APDU_02, sizeof(APDU_02));
            ret_code = STE_SAT_APDU_ERROR_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME;
            break;
        case 2:
            apdu = ste_apdu_new(APDU_03, sizeof(APDU_03));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            break;
        case 3:
            apdu = ste_apdu_new(APDU_04, sizeof(APDU_04));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            break;
        case 4:
            apdu = ste_apdu_new(APDU_05, sizeof(APDU_05));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            break;
        case 5:
            apdu = ste_apdu_new(APDU_06, sizeof(APDU_06));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            break;
        case 6:
            apdu = ste_apdu_new(APDU_07, sizeof(APDU_07));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_INCORRECT;
            break;
        case 7:
            apdu = ste_apdu_new(APDU_08, sizeof(APDU_08));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            break;
        case 8:
            apdu = ste_apdu_new(APDU_09, sizeof(APDU_09));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            break;
        case 9:
            apdu = ste_apdu_new(APDU_10, sizeof(APDU_10));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_INCORRECT;
            break;
        case 10:
            apdu = ste_apdu_new(APDU_11, sizeof(APDU_11));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            break;
        case 11:
            apdu = ste_apdu_new(APDU_12, sizeof(APDU_12));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            break;
        case 12:
            apdu = ste_apdu_new(APDU_13, sizeof(APDU_13));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_INCORRECT;
            break;
        case 13:
            apdu = ste_apdu_new(APDU_14, sizeof(APDU_14));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            break;
        case 14:
            apdu = ste_apdu_new(APDU_15, sizeof(APDU_15));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            break;
        case 15:
            apdu = ste_apdu_new(APDU_16, sizeof(APDU_16));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_INCORRECT;
            break;
        case 16:
            apdu = ste_apdu_new(APDU_17, sizeof(APDU_17));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            break;
        case 17:
            apdu = ste_apdu_new(APDU_18, sizeof(APDU_18));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            break;
        case 18:
#if 0 /* Wrong Device ID in Get Inkey not checked as of now */
            apdu = ste_apdu_new(APDU_19, sizeof(APDU_19));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_INCORRECT;
            break;
#else
            current_apdu++; goto start_loop;
#endif
        case 19:
            apdu = ste_apdu_new(APDU_20, sizeof(APDU_20));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            break;
        case 20:
            apdu = ste_apdu_new(APDU_21, sizeof(APDU_21));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            break;
        case 21:
#if 0 /* Wrong Device ID in Get Input not checked as of now */
            apdu = ste_apdu_new(APDU_22, sizeof(APDU_22));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_INCORRECT;
            break;
#else
            current_apdu++; goto start_loop;
#endif
        case 22:
            apdu = ste_apdu_new(APDU_23, sizeof(APDU_23));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            break;
        case 23:
            apdu = ste_apdu_new(APDU_24, sizeof(APDU_24));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            break;
        case 24:
#if 0 /* Wrong Device ID in More Time not checked as of now */
            apdu = ste_apdu_new(APDU_25, sizeof(APDU_25));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_INCORRECT;
            break;
#else
            current_apdu++; goto start_loop;
#endif
        case 25:
            apdu = ste_apdu_new(APDU_26, sizeof(APDU_26));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            break;
        case 26:
            apdu = ste_apdu_new(APDU_27, sizeof(APDU_27));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            break;
        case 27:
#if 0 /* Wrong Device ID in Poll Interval not checked as of now */
            apdu = ste_apdu_new(APDU_28, sizeof(APDU_28));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_INCORRECT;
            break;
#else
            current_apdu++; goto start_loop;
#endif
        case 28:
            apdu = ste_apdu_new(APDU_29, sizeof(APDU_29));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            break;
        case 29:
            apdu = ste_apdu_new(APDU_30, sizeof(APDU_30));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            break;
        case 30:
#if 0 /* Wrong Device ID in Polling Off not checked as of now */
            apdu = ste_apdu_new(APDU_31, sizeof(APDU_31));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_INCORRECT;
            break;
#else
            current_apdu++; goto start_loop;
#endif
        case 31:
            apdu = ste_apdu_new(APDU_32, sizeof(APDU_32));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            break;
        case 32:
            apdu = ste_apdu_new(APDU_33, sizeof(APDU_33));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            break;
        case 33:
#if 0 /* Wrong Device ID in Timer Management not checked as of now */
            apdu = ste_apdu_new(APDU_34, sizeof(APDU_34));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_INCORRECT;
            break;
#else
            current_apdu++; goto start_loop;
#endif
        case 34:
            apdu = ste_apdu_new(APDU_35, sizeof(APDU_35));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            break;
        case 35:
            apdu = ste_apdu_new(APDU_36, sizeof(APDU_36));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            break;
        case 36:
            apdu = ste_apdu_new(APDU_37, sizeof(APDU_37));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_INCORRECT;
            break;
        case 37:
            apdu = ste_apdu_new(APDU_38, sizeof(APDU_38));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            break;
        case 38:
            apdu = ste_apdu_new(APDU_39, sizeof(APDU_39));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            break;
        case 39:
            apdu = ste_apdu_new(APDU_40, sizeof(APDU_40));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_INCORRECT;
            break;
        case 40:
            apdu = ste_apdu_new(APDU_41, sizeof(APDU_41));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            break;
        case 41:
            apdu = ste_apdu_new(APDU_42, sizeof(APDU_42));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            break;
        case 42:
            apdu = ste_apdu_new(APDU_43, sizeof(APDU_43));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_INCORRECT;
            break;
        case 43:
            apdu = ste_apdu_new(APDU_44, sizeof(APDU_44));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            break;
        case 44:
            apdu = ste_apdu_new(APDU_45, sizeof(APDU_45));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            break;
        case 45:
            apdu = ste_apdu_new(APDU_46, sizeof(APDU_46));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_INCORRECT;
            break;
        case 46:
            apdu = ste_apdu_new(APDU_47, sizeof(APDU_47));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            break;
        case 47:
            apdu = ste_apdu_new(APDU_48, sizeof(APDU_48));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            break;
        case 48:
            apdu = ste_apdu_new(APDU_49, sizeof(APDU_49));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_INCORRECT;
            break;
        case 49:
            apdu = ste_apdu_new(APDU_50, sizeof(APDU_50));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            break;
        case 50:
            apdu = ste_apdu_new(APDU_51, sizeof(APDU_51));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            break;
        case 51:
            apdu = ste_apdu_new(APDU_52, sizeof(APDU_52));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_INCORRECT;
            break;
        case 52:
            apdu = ste_apdu_new(APDU_53, sizeof(APDU_53));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            break;
        case 53:
            apdu = ste_apdu_new(APDU_54, sizeof(APDU_54));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            break;
        case 54:
#if 0 /* Wrong Device ID in Language Notification not checked as of now */
            apdu = ste_apdu_new(APDU_55, sizeof(APDU_55));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_INCORRECT;
            break;
#else
            current_apdu++; goto start_loop;
#endif
        case 55:
            apdu = ste_apdu_new(APDU_56, sizeof(APDU_56));
            ret_code = STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            break;

        default:
            goto End;
    }
    if (!apdu)
        return "ste_apdu_new failed";

    printf("\n");
    printf
        ("############ Test %s ############\n", TC_Ids[current_apdu]);
    //Parse an APDU string
    if (ret_code !=
        ste_apdu_parse(apdu, &parsed_apdu_info_p)) {
        printf("FAILED : %s: Parse APDU failed.\n", TC_Names[current_apdu]);
        ste_apdu_delete(apdu);
        ste_parsed_apdu_delete(parsed_apdu_info_p);
        return "ste_apdu_parse failed.";
    } else {
        //print some useful information here
        printf("Result : %s: Parse APDU: OK\n", TC_Names[current_apdu]);
    }
    ste_parsed_apdu_delete(parsed_apdu_info_p);
    parsed_apdu_info_p = NULL;
    ste_apdu_delete(apdu);
    current_apdu++;
    goto start_loop;

End:
    return 0;
}

typedef const char     *test_func_t();

/**
 * Table of mapping test names to test functions.
 * Just expand the list with more items in no particular order.
 */
struct test_entry {
    char                   *test_name;
    test_func_t            *test_func;
};

// The following define is just to fool indent not to produce odd indentation!
#define TENTRY(N,F) {N,F}
static const struct test_entry tests[] = {
    TENTRY("DisplayText",SAT_APDU_TC_HandleDisplayText),
    TENTRY("ProvideIMEI",SAT_APDU_TC_HandleProvideIMEI),
    TENTRY("SetupEventList",SAT_APDU_TC_HandleSetupEventList),
    TENTRY("SetupMenu",SAT_APDU_TC_HandleSetupMenu),
    TENTRY("MenuSelection",SAT_APDU_TC_HandleMenuSelection),
    TENTRY("GetInkey",SAT_APDU_TC_HandleGetInkey),
    TENTRY("GetInput",SAT_APDU_TC_HandleGetInput),
    TENTRY("MoreTime",SAT_APDU_TC_HandleMoreTime),
    TENTRY("PollingOff",SAT_APDU_TC_HandlePollingOff),
    TENTRY("SendUssd", SAT_APDU_TC_HandleSendUssd),
    TENTRY("SendSS", SAT_APDU_TC_HandleSendSS),
    TENTRY("TimerManagement", SAT_APDU_TC_HandleTimerManagement),
    TENTRY("PollInterval", SAT_APDU_TC_HandlePollInterval),
    TENTRY("Refresh", SAT_APDU_TC_HandleRefresh),
    TENTRY("SetupCall", SAT_APDU_TC_HandleSetupCall),
    TENTRY("SendDTMF", SAT_APDU_TC_HandleSendDTMF),
    TENTRY("SendSMS", SAT_APDU_TC_HandleSendSMS),
    TENTRY("LanguageNotify", SAT_APDU_TC_HandleLanguageNotification),
    TENTRY("WrongApdu", SAT_APDU_TC_HandleWrongAPDU)
};
/*
 * Put all test names here in the order you would like them to run when you
 * call ./tapdu with no test names
 */
static const char      *all_tests[] = {
    "DisplayText",
    "ProvideIMEI",
    "SetupEventList",
    "SetupMenu",
    "MenuSelection",
    "GetInkey",
    "GetInput",
    "MoreTime",
    "PollingOff",
    "PollInterval",
    "SendUssd",
    "SendSS",
    "TimerManagement",
    "SetupCall",
    "SendDTMF",
    "Refresh",
    "SendSMS",
    "LanguageNotify",
    "WrongApdu"
};

test_func_t            *find_test_func(const char *name)
{
    int                     i;
    for (i = 0; tests[i].test_name; i++) {
        if (strcmp(name, tests[i].test_name) == 0) {
            return tests[i].test_func;
        }
    }
    return 0;
}

/*************************************************************************
 * Function:      run_tests
 *
 * Description:
 *
 *************************************************************************/
void run_tests(int argc, const char *argv[])
{
    int                     numFailed = 0;
    int                     numPassed = 0;

    while (argc) {
        test_func_t            *f = find_test_func(*argv);
        const char             *s;

        if (!f) {
            printf("No such test function: %s\n", *argv);
            exit(EXIT_FAILURE);
        }

        s = f();
        if (s) {
            numFailed++;
            printf("FAILED : %s : %s\n", *argv, s);
        } else {
            numPassed++;
            printf("OK     : %s\n", *argv);
        }
        printf
            ("########################## TC END ##########################\n");

        --argc;
        ++argv;
    }

    // Don't change the printed text!
    printf("\n########################## SUMMARY ##########################\n");
    printf("TEST CASES EXECUTED: %d\n", numFailed + numPassed);
    printf("PASSED: %d\n", numPassed);
    printf("FAILED: %d\n", numFailed);
}

/*************************************************************************
 * Function:      main
 *
 * Description:
 *
 *************************************************************************/
int main(int argc, const char *argv[])
{
    setProcName(argv[0]);
    setbuf(stdout, 0);

    --argc;
    ++argv;

    if (!argc) {                /* Do all */
        int                     FuncIndex =
            sizeof(all_tests) / sizeof(*all_tests);
        run_tests(FuncIndex, all_tests);
    } else {
        run_tests(argc, argv);
    }

    return EXIT_SUCCESS;
}
