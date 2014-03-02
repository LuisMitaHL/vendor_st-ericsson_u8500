/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cn_log.h"
#include "cn_macros.h"

#include "str_convert.h"
#include "request_handling.h"
#include "sim_client.h"

#include "plmn_list.h"
#include "plmn_sim.h"

/* Macros */
#define EXTRACT_MCC(x)      (((x >> 8) & 0x00F) | (x & 0x0F0) | ((x << 8) & 0xF00))
#define EXTRACT_MNC(x)      (((x >> 12) & 0x00F) | ((x >> 16) & 0x0F0) | (x >> 8 & 0xF00))
#define SWAP_LAC(x)         ((x << 8) | (x >> 8))

/* Constants */
static const struct {
    const char *file_path;
} sim_apps[] = {
    {""},             /* SIM_APP_UNKNOWN */
    {"3F007F20"},     /* SIM_APP_GSM */
    {"3F007FFF"},     /* SIM_APP_USIM */
    {""},             /* SIM_APP_ISIM */
};

typedef enum {
    EF_FIRST = 0,
    EF_PNN   = EF_FIRST,
    EF_OPL,
    EF_COUNT,
} plmn_file_t;

static const struct {
    const char    *name_p;
    const uint16_t id;                  /* NOTE: Store in network byte order */
    size_t         length;
} sim_ef_files[] = {
    {"EF-PNN", CN_HTONS(0x6FC5), sizeof(uint16_t)},
    {"EF-OPL", CN_HTONS(0x6FC6), sizeof(uint16_t)},
};

typedef enum {                          /* IEI - Information element identifier */
    IEI_NETWORK_NAME_FULL   = 0x43,     /* Network name 10.5.3.5a */
    IEI_NETWORK_NAME_SHORT  = 0x45,     /* Network name 10.5.3.5a */
    IEI_LOCAL_TZ            = 0x46,     /* Local time zone 10.5.3.8 */
    IEI_UT_AND_LOCAL_TZ     = 0x47,     /* Universal time and local time zone 10.5.3.9 */
    IEI_LSA_IDENTITY        = 0x48,     /* LSA Identifier 10.5.3.11 */
    IEI_NETWORK_DST         = 0x49,     /* Network daylight saving time 10.5.3.11 */
    IEI_PLMN_ADDITIONAL_INFO = 0x80,    /* PLMN Additional information 10.5.3.5a */
} iei_t;

typedef enum {
    PLMN_REQUEST_APP_STATE = REQUEST_STATE_REQUEST,
    PLMN_RESPONSE_APP_STATE,
    PLMN_RESPONSE_SERVICE_PNN,
    PLMN_RESPONSE_SERVICE_OPL,
    PLMN_REQUEST_READ_EF_OPL,
    PLMN_RESPONSE_READ_EF_OPL,
    PLMN_RESPONSE_READ_EF_PNN,
    PLMN_REQUEST_STATUS_DONE,
} plmn_sim_state_t;

typedef struct {
    sim_app_type_t app_type;
    ste_uicc_service_status_t pnn;
    ste_uicc_service_status_t opl;
    int opl_record;
    int pnn_record;
    uint16_t mcc;
    uint16_t mnc;
    uint16_t lac1;
    uint16_t lac2;
    int added;
} plmn_sim_context_t;


/* Local data */
static bool load_active = false;


/**
 * Initiate loading of operator list(s) from (U)SIM
 */
int plmn_sim_load()
{
    request_record_t *record_p = NULL;
    request_status_t status = REQUEST_STATUS_ERROR;


    /* Set load active flag to prevent multiple simulatenous attempts to load PLMN operator list from SIM */
    load_active = true;

    /* Allocate a request to handle reading from (U)SIM */
    record_p = request_record_create(CN_REQUEST_READ_OPL_FROM_SIM, 0 /* client_tag */, 0 /* client_id */);

    if (!record_p) {
        CN_LOG_E("ERROR, not possible to create request record!");
        goto error;
    }

    /* Allocate memory to hold context specific information */
    record_p->request_data_p = calloc(1, sizeof(plmn_sim_context_t));

    if (!record_p->request_data_p) {
        CN_LOG_E("ERROR, not possible to allocate memory!");
        goto cleanup;
    }

    /* Take the first step up the transaction ladder */
    record_p->state = PLMN_REQUEST_APP_STATE;
    record_p->request_handler_p = plmn_sim_handle_request;
    status = plmn_sim_handle_request(NULL, record_p);

    /* Check status, it should be pending, if not, there's a problem */
    if (REQUEST_STATUS_PENDING != status) {
        CN_LOG_E("ERROR, unexpected status %d!", status);
        goto error;
    }

    return 0;

cleanup:
    if (record_p) {
        free(record_p->request_data_p);
        record_p->request_data_p = NULL;
        request_record_free(record_p);
        load_active = false;
    }

error:
    return -1;
}


bool plmn_sim_load_active()
{
    return load_active;
}


/**
 * Check if the last part of the path matches the path of any of the files we monitor.
 */
bool plmn_sim_path_match(sim_path_t *path_p)
{
    bool result = false;
    int offset, i;

    for (i = EF_FIRST; i < EF_COUNT; i++) {

        /* Calculate offset from beginning of path to begin comparison */
        offset = path_p->pathlen - sim_ef_files[i].length;

        if (0 < offset) {

            /*
             * The SIM file path is reported in network byte order. This is also
             * the order in which the array of EF file id's are stored in.
             */
            if (memcmp(&path_p->path[offset], &sim_ef_files[i].id, sim_ef_files[i].length) == 0) {
                CN_LOG_D("Found matching path for file: %s.", sim_ef_files[i].name_p);
                result = true;
                break;
            }
        }
    }

    return result;
}


request_status_t plmn_sim_handle_request(void *UNUSED(data_p), request_record_t *record_p)
{
    plmn_sim_context_t *request_data_p;
    uicc_request_status_t status;


    REQUIRE_VALID_RECORD(record_p);

    request_data_p = (plmn_sim_context_t *)record_p->request_data_p;

    REQUIRE_VALID_REQUEST_DATA(request_data_p);

eval_state:
    switch (record_p->state) {
    case PLMN_REQUEST_APP_STATE: {
        CN_LOG_D("ste_uicc_get_app_info -> request");
        status = ste_uicc_get_app_info(sim_client_get_handle(),
                                       (uintptr_t)request_record_get_modem_tag(record_p));

        if (UICC_REQUEST_STATUS_OK != status) {
            CN_LOG_E("ste_uicc_get_app_info request returns error %d!", status);
            break;
        }

        record_p->state = PLMN_RESPONSE_APP_STATE;

        return REQUEST_STATUS_PENDING;
    }

    case PLMN_RESPONSE_APP_STATE: {
        ste_uicc_sim_app_info_response_t *response_data_p =
            (ste_uicc_sim_app_info_response_t *)record_p->response_data_p;
        CN_LOG_D("ste_uicc_get_app_info <- response");

        REQUIRE_VALID_RESPONSE_DATA(response_data_p);

        if (STE_UICC_STATUS_CODE_OK != response_data_p->status) {
            CN_LOG_E("ste_uicc_get_app_info response status %d!", response_data_p->status);
            break;
        }

        /* Copy the SIM app-type so it is easily accessible when needed */
        request_data_p->app_type = response_data_p->app_type;

        /* Request service state for PLMN Network Name - PNN */
        CN_LOG_D("ste_uicc_get_service_availability(PNN) -> request");
        status = ste_uicc_get_service_availability(sim_client_get_handle(),
                 (uintptr_t)request_record_get_modem_tag(record_p),
                 SIM_SERVICE_TYPE_PNN);

        if (UICC_REQUEST_STATUS_OK != status) {
            CN_LOG_E("ste_uicc_get_service_availability request returns error %d!", status);
            break;
        }

        record_p->state = PLMN_RESPONSE_SERVICE_PNN;

        return REQUEST_STATUS_PENDING;
    }

    case PLMN_RESPONSE_SERVICE_PNN: {
        ste_uicc_get_service_availability_response_t *response_data_p =
            (ste_uicc_get_service_availability_response_t *)record_p->response_data_p;
        CN_LOG_D("ste_uicc_get_service_availability(PNN) <- response");

        REQUIRE_VALID_RESPONSE_DATA(response_data_p);

        if (STE_UICC_STATUS_CODE_OK != response_data_p->uicc_status_code) {
            CN_LOG_E("ste_uicc_get_service_availability response status %d!", response_data_p->uicc_status_code);
            break;
        }

        /* Copy the PNN service availability so it is easily accessible when needed */
        request_data_p->pnn = response_data_p->service_availability;

        /* Request service state for Operator PLMN List - OPL */
        CN_LOG_D("ste_uicc_get_service_availability(OPL) -> request");
        status = ste_uicc_get_service_availability(sim_client_get_handle(),
                 (uintptr_t)request_record_get_modem_tag(record_p),
                 SIM_SERVICE_TYPE_OPL);

        if (UICC_REQUEST_STATUS_OK != status) {
            CN_LOG_E("ste_uicc_get_service_availability request returns error %d!", status);
            break;
        }

        record_p->state = PLMN_RESPONSE_SERVICE_OPL;

        return REQUEST_STATUS_PENDING;
    }

    case PLMN_RESPONSE_SERVICE_OPL: {
        ste_uicc_get_service_availability_response_t *response_data_p =
            (ste_uicc_get_service_availability_response_t *)record_p->response_data_p;
        CN_LOG_D("ste_uicc_get_service_availability(OPL) <- response");

        REQUIRE_VALID_RESPONSE_DATA(response_data_p);

        if (STE_UICC_STATUS_CODE_OK != response_data_p->uicc_status_code) {
            CN_LOG_E("ste_uicc_get_service_availability response status %d!", response_data_p->uicc_status_code);
            break;
        }

        /* Copy the OPL service availability so it is easily accessible when needed */
        request_data_p->opl = response_data_p->service_availability;

        /* Both OPL and PNN services must be available, or there is no operator list on SIM */
        if (STE_UICC_SERVICE_AVAILABLE != request_data_p->opl ||
                STE_UICC_SERVICE_AVAILABLE != request_data_p->pnn) {
            CN_LOG_W("Services OPL(%d) and/or PNN(%d) are not available. No operator list available on (U)SIM.",
                     request_data_p->opl,
                     request_data_p->pnn);
            record_p->state = PLMN_REQUEST_STATUS_DONE;
            goto eval_state;
        }

        /* Set things up for reading EF-OPL */
        request_data_p->opl_record = SIM_RECORD_ID_ABSOLUTE_FIRST;
        record_p->state = PLMN_REQUEST_READ_EF_OPL;
        goto eval_state;
    }

    case PLMN_REQUEST_READ_EF_OPL: {
        CN_LOG_D("ste_uicc_sim_file_read_record(EF-OPL:%d) -> request", request_data_p->opl_record);
        status = ste_uicc_sim_file_read_record(sim_client_get_handle(),
                                               (uintptr_t)request_record_get_modem_tag(record_p),
                                               CN_NTOHS(sim_ef_files[EF_OPL].id),
                                               request_data_p->opl_record,
                                               0 /* length */,
                                               sim_apps[request_data_p->app_type].file_path);

        if (UICC_REQUEST_STATUS_OK != status) {
            CN_LOG_E("ste_uicc_sim_file_read_record(EF-OPL) request returns error %d!", status);
            break;
        }

        record_p->state = PLMN_RESPONSE_READ_EF_OPL;

        return REQUEST_STATUS_PENDING;
    }

    case PLMN_RESPONSE_READ_EF_OPL: {
        ste_uicc_sim_file_read_record_response_t *response_data_p =
            (ste_uicc_sim_file_read_record_response_t *)record_p->response_data_p;
        CN_LOG_D("ste_uicc_sim_file_read_record(EF-OPL:%d) <- response", request_data_p->opl_record);
        uint32_t mcc_mnc;
        uint16_t mcc, mnc, lac1, lac2;
        uint8_t pnn;
        int pos;

        if (STE_UICC_STATUS_CODE_OK != response_data_p->uicc_status_code) {
            if (STE_UICC_STATUS_CODE_FAIL == response_data_p->uicc_status_code &&
                    STE_UICC_STATUS_CODE_FAIL_DETAILS_INVALID_PARAMETERS == response_data_p->uicc_status_code_fail_details) {
                /* End of file reached. End the operation */
                CN_LOG_D("End-of-file reached reading EF-OPL record %d.",
                         request_data_p->opl_record);
                record_p->state = PLMN_REQUEST_STATUS_DONE;
                goto eval_state;
            } else {
                CN_LOG_E("ste_uicc_sim_file_read_record(EF-OPL:%d) response status %d!",
                         request_data_p->opl_record, response_data_p->uicc_status_code);
                break;
            }
        }

        /* Check record length */
        if (response_data_p->length < 8) {
            CN_LOG_E("ste_uicc_sim_file_read_record(EF-OPL) record length (%d) less than 8 response status %d!",
                     response_data_p->length);
            request_data_p->opl_record++;
            record_p->state = PLMN_REQUEST_READ_EF_OPL;
            goto eval_state;
        }

        /* 3 first bytes hold MCC & MNC */
        mcc_mnc = (uint32_t)response_data_p->data[0];
        pos = sizeof(uint8_t);
        mcc_mnc |= (uint32_t)response_data_p->data[pos] << 8;
        pos += sizeof(uint8_t);
        mcc_mnc |= (uint32_t)response_data_p->data[pos] << 16;
        pos += sizeof(uint8_t);

        /* Mobile Country Code - MCC */
        mcc = EXTRACT_MCC(mcc_mnc);

        /* Mobile Network Code - MNC */
        mnc = EXTRACT_MNC(mcc_mnc);

        /* Loction Area Code - LAC */
        lac1 = *(uint16_t *)&response_data_p->data[pos];
        pos += sizeof(uint16_t);
        lac1 = SWAP_LAC(lac1);
        lac2 = *(uint16_t *)&response_data_p->data[pos];
        pos += sizeof(uint16_t);
        lac2 = SWAP_LAC(lac2);

        pnn = response_data_p->data[pos];
        pos += sizeof(uint8_t);

#ifdef CN_DEBUG_ENABLED
        {
            char datastr[100] = { '\0' };
            char str[10];

            for (pos = 0; pos < 30 && pos < response_data_p->length; pos++) {
                sprintf(str, "%02X ", response_data_p->data[pos]);
                strcat(datastr, str);
            }

            CN_LOG_D("EF-OPL record=%d, length=%d, data=%s, mcc_mnc=%06X, mcc=%03X, mnc=%03X, lac=%04X-%04X, pnn=%02X",
                     request_data_p->opl_record,
                     response_data_p->length,
                     datastr,
                     mcc_mnc, mcc, mnc, lac1, lac2, pnn);
        }
#endif /* CN_DEBUG_ENABLED */

        /* Detect end-of-file */
        if (0xFFFFFF == mcc_mnc) {
            record_p->state = PLMN_REQUEST_STATUS_DONE;
            goto eval_state;
        }

        /* Trim any trailing 0xF from MNC */
        if (0xF == (mnc & 0xF)) {
            mnc >>= 4;
        }

        /* Read corresponding record from EF-PNN */
        request_data_p->mcc = mcc;
        request_data_p->mnc = mnc;
        request_data_p->lac1 = lac1;
        request_data_p->lac2 = lac2;
        request_data_p->pnn_record = pnn;
        CN_LOG_D("ste_uicc_sim_file_read_record(EF-PNN:%d) -> request", request_data_p->pnn_record);
        status = ste_uicc_sim_file_read_record(sim_client_get_handle(),
                                               (uintptr_t)request_record_get_modem_tag(record_p),
                                               CN_NTOHS(sim_ef_files[EF_PNN].id),
                                               request_data_p->pnn_record,
                                               0 /* length */,
                                               sim_apps[request_data_p->app_type].file_path);

        if (UICC_REQUEST_STATUS_OK != status) {
            CN_LOG_E("ste_uicc_sim_file_read_record(EF-PNN) request returns error %d!", status);
            break;
        }

        record_p->state = PLMN_RESPONSE_READ_EF_PNN;

        return REQUEST_STATUS_PENDING;
    }

    case PLMN_RESPONSE_READ_EF_PNN: {
        ste_uicc_sim_file_read_record_response_t *response_data_p =
            (ste_uicc_sim_file_read_record_response_t *)record_p->response_data_p;
        int added = 0;
        int pos, encoding;
        iei_t iei;
        int spn_res, mnn_res;
        size_t in_length, out_length, spare_bits;
        bool add_ci;
        char spn[64];
        char mnn[32];
        CN_LOG_D("ste_uicc_sim_file_read_record(EF-PNN:%d) <- response", request_data_p->pnn_record);

        if (STE_UICC_STATUS_CODE_OK != response_data_p->uicc_status_code) {
            CN_LOG_E("ste_uicc_sim_file_read_record(EF-PNN:%d) response status %d!",
                     request_data_p->pnn_record, response_data_p->uicc_status_code);
            break;
        }

#ifdef CN_DEBUG_ENABLED
        {
            char datastr[100] = { '\0' };
            char str[10];
            int i;

            for (i = 0; i < 30 && i < response_data_p->length; i++) {
                sprintf(str, "%02X ", response_data_p->data[i]);
                strcat(datastr, str);
            }

            CN_LOG_D("EF-PNN record=%d, length=%d, data=%s",
                     request_data_p->pnn_record,
                     response_data_p->length,
                     datastr);
        }
#endif/* CN_DEBUG_ENABLED */

        spn[0] = mnn[0] = '\0';
        spn_res = mnn_res = 0;

        for (pos = 0; pos < response_data_p->length; pos += (in_length + 2)) {
            iei = (iei_t)response_data_p->data[pos];
            in_length = response_data_p->data[pos + 1];
            add_ci = (response_data_p->data[pos + 2] & 0x8) != 0;
            encoding = (response_data_p->data[pos + 2] >> 4) & 0x7;
            spare_bits = response_data_p->data[pos + 2] & 0x7;

            switch (iei) {
            case IEI_NETWORK_NAME_FULL:
                /* Full name for network */
                out_length = sizeof(spn) - 1;
                spn_res = str_convert_string(encoding, in_length - 1,
                                             &response_data_p->data[pos + 3], spare_bits,
                                             CHARSET_UTF_8, &out_length, (uint8_t *)spn);
                break;

            case IEI_NETWORK_NAME_SHORT:
                /* Short name for network */
                out_length = sizeof(mnn) - 1;
                mnn_res = str_convert_string(encoding, in_length - 1,
                                             &response_data_p->data[pos + 3], spare_bits,
                                             CHARSET_UTF_8, &out_length, (uint8_t *)mnn);
                break;

            case IEI_PLMN_ADDITIONAL_INFO:
                /* PLMN additional information */
                break;

            default:
                CN_LOG_E("Unknown IEI 0x%X in EF-PNN record %d !",
                         iei, request_data_p->pnn_record);
                break;
            }
        }

        if (spn_res >= 0 && mnn_res >= 0) {
            CN_LOG_D("Adding mcc=0x%03X, mnc=0x%02X, lac=0x%X-0x%X, spn=\"%s\", mnn=\"%s\"",
                     request_data_p->mcc,  request_data_p->mnc, request_data_p->lac1, request_data_p->lac2, spn, mnn);
            /* Add entry to list if names have been converted successfully */
            added = plmn_list_add_entry(PLMN_SIM_FILE,
                                        request_data_p->mcc,
                                        request_data_p->mnc,
                                        request_data_p->lac1,
                                        request_data_p->lac2,
                                        spn, mnn);

            /* Keep count on number of addded entries */
            request_data_p->added += added;
        } else {
            CN_LOG_E("Error when converting SPN/MNN names for EF-PNN record=%d (mcc=%03X, mnc=%02X, lac=%X-%X) !",
                     request_data_p->pnn_record,
                     request_data_p->mcc,
                     request_data_p->mnc,
                     request_data_p->lac1,
                     request_data_p->lac2);
        }

        /* Read next record from EF-OPL */
        request_data_p->opl_record++;
        record_p->state = PLMN_REQUEST_READ_EF_OPL;
        goto eval_state;
    }

    case PLMN_REQUEST_STATUS_DONE:
        CN_LOG_D("Read %d (U)SIM entries into PLMN operator list.", request_data_p->added);
        free(record_p->request_data_p);
        record_p->request_data_p = NULL;

        /* Reset load active flag */
        load_active = false;

        return REQUEST_STATUS_DONE;

    default:
        CN_LOG_E("Unhandled state %d!", record_p->state);
        break;
    }

    /* Some error has occurred, clean up and exit */
    free(record_p->request_data_p);
    record_p->request_data_p = NULL;

    /* Reset load active flag */
    load_active = false;

    return REQUEST_STATUS_ERROR;
}

