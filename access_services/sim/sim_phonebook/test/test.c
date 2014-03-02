/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Description     : testframework file
 *
 * Author          : Christian Nilsson <christian.c.nilsson@stericsson.com>
 *                   Dag Gullberg <dag.xd.gullberg@stericsson.com>
 *
 */

#include <stdio.h>
#include <test.h>

#include "tc.h"
#include "sim_stub/sim_response_data.h"
#include "simpb_internal.h"

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sim.h>

/*
 * Global variables
 */
int nbrOfTests;
int nbrOfPass = 0;
int nbrOfFail = 0;
int stubfd_ts = -1;
int stubfd_st = -1;

/*
 * Private function declarations
 */

int runtests();
int connectToStub();
void closeSockets();
int waitForStubEvent(int timeout);
int sendToStub(int writesize, void *data_p);
int readFromStub(int *readsize_p, void **data_pp);
void printResults();
simpb_test_state_type_t setup(int state, ste_simpb_t **ste_simpb_pp);
simpb_test_state_type_t take_down(int state, ste_simpb_t **ste_simpb_pp);


/* callback parameters */
ste_simpb_callback_t ste_simpb_callback_data;

/*
 * Test case list
 */
const testcase_t testcases[] = {
    { "setup", setup }, /* do not remove. must be first */

    { "test_read_record", test_read_record},
    { "test_update_record", test_update_record},
    { "test_get_file_info", test_get_file_info},
    { "test_get_supported_pbs", test_get_supported_pbs},
    { "test_set_pb", test_set_pb},
    { "test_get_pb", test_get_pb},

    { "test_unsolicted_events_CAT_PC_REFRESH_INDs", test_unsolicted_events_CAT_PC_REFRESH_INDs},
    { "test_unsolicted_events_SIM_STATUS_STATE_CHANGED_IND",test_unsolicted_events_SIM_STATUS_STATE_CHANGED_IND},

    { "take_down", take_down } /* do not remove. must be last */
};


/*
 * Tests part of framework.
 */
simpb_test_state_type_t setup(int state, ste_simpb_t **ste_simpb_pp)
{
    ste_simpb_status_t status;
    ste_sim_closure_t closure;
    int stub_fd = 0;
    int fd;
    void *data_p;
    int size;
    uintptr_t ct = 144;
    ste_simpb_t *ste_simpb_p = NULL;

    /* connect, no calls are done to sim stub*/
    TESTSIMPB(ste_simpb_connect(ste_simpb_pp, &fd, &status, (ste_simpb_cb_t) ste_simpb_cb, NULL));
    ste_simpb_p = *ste_simpb_pp;

    /* Role: Client of SIMPB */
    /* startup */
    TESTSIMPB(ste_simpb_startup(ste_simpb_p, ct));

    /* Role: SIM server serving SIMPBs requests*/

    /*
       SIMPBD_TEST_STE_SIM_NEW_ST
       SIMPBD_TEST_STE_SIM_CONNECT
       SIMPBD_TEST_STE_SIM_FD

       are actions requested from the client served synchronously by the sim lib.

       Hence there are no actions involving those requests here where we emulate the sim daemon
       */

    /* wait for stub socket to send us the get sim state request from pb */
    {
        simpbd_test_ste_uicc_sim_get_state_req_t req;
        simpbd_test_ste_sim_read_resp_t *response_p;
        ste_uicc_get_sim_state_response_t *rdata_p = NULL;
        int size = 0;
        int size_data = 0;

        TESTCHECK(getStubEvent(1000, SIMPBD_TEST_STE_UICC_SIM_GET_STATE, &data_p, &size));

        memcpy(&req, data_p, sizeof(req));
        free(data_p);
        ct = req.client_tag;

        size_data = sizeof(ste_uicc_get_sim_state_response_t);
        rdata_p = (ste_uicc_get_sim_state_response_t *)calloc(size_data, sizeof(uint8_t));
        rdata_p->state = SIM_STATE_READY;
        rdata_p->error_cause = STE_UICC_STATUS_CODE_OK;

        size = size_data + sizeof(simpbd_test_ste_sim_read_resp_t);
        response_p = (simpbd_test_ste_sim_read_resp_t *)calloc(size, sizeof(uint8_t));

        response_p->size = size_data;
        memcpy(&(response_p->data), (void *)rdata_p, size_data);
        free(rdata_p);

        response_p->retval = UICC_REQUEST_STATUS_OK;
        response_p->cause = STE_UICC_CAUSE_REQ_GET_SIM_STATE;
        response_p->client_tag = ct;

        TESTCHECK(sendStubResponse(SIMPBD_TEST_STE_UICC_SIM_GET_STATE, response_p, size));
        free(response_p);
    }

    /* simulating that SIM has sent unsolicited event that it is now ready for command and Client
     * is setting the status to initialized. */
    ste_simpb_p->status = STE_SIMPB_STATUS_INITIALIZED;

    /*  ** ste_simpb_cache_pbr ** */

    /* Role: Client of SIMPB */
    TESTSIMPB(ste_simpb_cache_pbr(ste_simpb_p, ct));

    /* Role: SIM server serving SIMPBs requests*/
    /* wait for stub socket to send us the get card status request from pb */
    {
        simpbd_test_ste_uicc_sim_card_status_req_t req;
        simpbd_test_ste_sim_read_resp_t *response_p;
        ste_uicc_sim_card_status_response_t *rdata_p = NULL;
        int size = 0;
        int size_data = 0;

        TESTCHECK(getStubEvent(1000, SIMPBD_TEST_STE_UICC_SIM_CARD_STATUS, &data_p, &size));

        memcpy(&req, data_p, sizeof(req));
        free(data_p);
        ct = req.client_tag;

        size_data = sizeof(ste_uicc_sim_card_status_response_t);
        rdata_p = (ste_uicc_sim_card_status_response_t *)calloc(size_data, sizeof(uint8_t));
        rdata_p->card_type = SIM_CARD_TYPE_UICC;
        rdata_p->status = STE_UICC_STATUS_CODE_OK;
        rdata_p->num_apps = 1;
        rdata_p->card_state = STE_UICC_CARD_STATE_READY;
        rdata_p->upin_state = STE_UICC_PIN_STATE_ENABLED_VERIFIED;

        size = size_data + sizeof(simpbd_test_ste_sim_read_resp_t);
        response_p = (simpbd_test_ste_sim_read_resp_t *)calloc(size, sizeof(uint8_t));

        response_p->size = size_data;
        memcpy(&(response_p->data), (void *)rdata_p, size_data);

        response_p->retval = UICC_REQUEST_STATUS_OK;
        response_p->cause = STE_UICC_CAUSE_REQ_CARD_STATUS;
        response_p->client_tag = ct;

        TESTCHECK(sendStubResponse(SIMPBD_TEST_STE_UICC_SIM_CARD_STATUS, response_p, size));
        free(rdata_p);
        free(response_p);
    }

    /* wait for stub socket to send us the get app info request from pb */
    {
        simpbd_test_ste_uicc_get_app_info_req_t req;
        simpbd_test_ste_sim_read_resp_t *response_p;
        ste_uicc_sim_app_info_response_t *rdata_p = NULL;
        int size = 0;
        int size_data = 0;

        TESTCHECK(getStubEvent(1000, SIMPBD_TEST_STE_UICC_GET_APP_INFO, &data_p, &size));

        memcpy(&req, data_p, sizeof(req));
        free(data_p);
        ct = req.client_tag;

        size_data = sizeof(ste_uicc_sim_app_info_response_t);
        rdata_p = (ste_uicc_sim_app_info_response_t *)calloc(size_data, sizeof(uint8_t));
        rdata_p->app_type = SIM_APP_USIM;
        rdata_p->status = STE_UICC_STATUS_CODE_OK;

        size = size_data + sizeof(simpbd_test_ste_sim_read_resp_t);
        response_p = (simpbd_test_ste_sim_read_resp_t *)calloc(size, sizeof(uint8_t));

        response_p->size = size_data;
        memcpy(&(response_p->data), (void *)rdata_p, size_data);
        free(rdata_p);

        response_p->retval = UICC_REQUEST_STATUS_OK;
        response_p->cause = STE_UICC_CAUSE_REQ_GET_APP_INFO;
        response_p->client_tag = ct;

        TESTCHECK(sendStubResponse(SIMPBD_TEST_STE_UICC_GET_APP_INFO, response_p, size));
        free(response_p);
    }

    /* wait for stub socket to send us the get ste_uicc_sim_get_file_information request from pb  GLOBAL phonebook */
    {
        simpbd_test_ste_uicc_sim_file_get_information_req_t req;
        simpbd_test_ste_sim_read_resp_t *response_p;
        ste_uicc_get_file_information_response_t *rdata_p = NULL;
        int size = 0;
        int size_data = 0;

        TESTCHECK(getStubEvent(1000, SIMPBD_TEST_STE_UICC_SIM_FILE_INFORMATION_GET, &data_p, &size));

        memcpy(&req, data_p, sizeof(req));
        free(data_p);
        ct = req.client_tag;

        size_data = sizeof(ste_uicc_get_file_information_response_t) + sim_fcp_data.length;
        rdata_p = (ste_uicc_get_file_information_response_t *)calloc(size_data, sizeof(uint8_t));
        rdata_p->uicc_status_code = STE_UICC_STATUS_CODE_OK;
        rdata_p->uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        rdata_p->status_word.sw1 = sim_fcp_data.sw1;
        rdata_p->status_word.sw2 = sim_fcp_data.sw2;;
        rdata_p->fcp = 0;
        memcpy((uint8_t *)(&(rdata_p->length) + 1), sim_file_information_data[0].data, sim_file_information_data[0].length);
        rdata_p->length = sim_file_information_data[0].length;

        size = size_data + sizeof(simpbd_test_ste_sim_read_resp_t);
        response_p = (simpbd_test_ste_sim_read_resp_t *)calloc(size, sizeof(uint8_t));

        response_p->size = size_data;
        memcpy(&(response_p->data), (void *)rdata_p, size_data);
        free(rdata_p);

        response_p->retval = UICC_REQUEST_STATUS_OK;
        response_p->cause = STE_UICC_CAUSE_REQ_GET_FILE_INFORMATION;
        response_p->client_tag = ct;

        TESTCHECK(sendStubResponse(SIMPBD_TEST_STE_UICC_SIM_FILE_INFORMATION_GET, response_p, size));
        free(response_p);
    }

    /* wait for stub socket to send us the get ste_uicc_sim_get_file_information request from pb LOCAL phonebook */
    {
        simpbd_test_ste_uicc_sim_file_get_information_req_t req;
        simpbd_test_ste_sim_read_resp_t *response_p;
        ste_uicc_get_file_information_response_t *rdata_p = NULL;
        int size = 0;
        int size_data = 0;

        TESTCHECK(getStubEvent(1000, SIMPBD_TEST_STE_UICC_SIM_FILE_INFORMATION_GET, &data_p, &size));

        memcpy(&req, data_p, sizeof(req));
        free(data_p);
        ct = req.client_tag;

        size_data = sizeof(ste_uicc_get_file_information_response_t) + sim_fcp_data.length;
        rdata_p = (ste_uicc_get_file_information_response_t *)calloc(size_data, sizeof(uint8_t));
        rdata_p->uicc_status_code = STE_UICC_STATUS_CODE_OK;
        rdata_p->uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        rdata_p->status_word.sw1 = sim_fcp_data.sw1;
        rdata_p->status_word.sw2 = sim_fcp_data.sw2;;
        rdata_p->fcp = 0;
        memcpy((uint8_t *)(&(rdata_p->length) + 1), sim_file_information_data[0].data, sim_file_information_data[0].length);
        rdata_p->length = sim_file_information_data[0].length;

        size = size_data + sizeof(simpbd_test_ste_sim_read_resp_t);
        response_p = (simpbd_test_ste_sim_read_resp_t *)calloc(size, sizeof(uint8_t));

        response_p->size = size_data;
        memcpy(&(response_p->data), (void *)rdata_p, size_data);
        free(rdata_p);

        response_p->retval = UICC_REQUEST_STATUS_OK;
        response_p->cause = STE_UICC_CAUSE_REQ_GET_FILE_INFORMATION;
        response_p->client_tag = ct;

        TESTCHECK(sendStubResponse(SIMPBD_TEST_STE_UICC_SIM_FILE_INFORMATION_GET, response_p, size));
        free(response_p);
    }

    /* wait for stub socket to send us the get pbr info request from pb STE_SIMPB_REQUEST_ID_CACHE_PBR*/
    {
        simpbd_test_ste_uicc_sim_file_read_record_req_t req;
        simpbd_test_ste_sim_read_resp_t *response_p = NULL;
        ste_uicc_sim_file_read_record_response_t *data_p = NULL;
        int size = 0;
        int size_data = 0;

        TESTCHECK(getStubEvent(1000, SIMPBD_TEST_STE_UICC_SIM_FILE_READ_RECORD, &data_p, &size))

        memcpy(&req, data_p, sizeof(req));
        free(data_p);
        ct = req.client_tag;
        /* need to send the response back */

        size_data = sizeof(ste_uicc_sim_file_read_record_response_t) + sim_pbr_data.length;
        data_p = (ste_uicc_sim_file_read_record_response_t *)calloc(size_data, sizeof(uint8_t));
        data_p->uicc_status_code = STE_UICC_STATUS_CODE_OK;
        data_p->uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        data_p->status_word.sw1 = sim_pbr_data.sw1;
        data_p->status_word.sw2 = sim_pbr_data.sw2;
        data_p->data = 0;  /* 0 address as data is piggy-backed in packet */
        memcpy((uint8_t *)(&(data_p->length) + 1), sim_pbr_data.data, sim_pbr_data.length);
        data_p->length = sim_pbr_data.length;

        size = size_data + sizeof(simpbd_test_ste_sim_read_resp_t);
        response_p = (simpbd_test_ste_sim_read_resp_t *)calloc(size, sizeof(uint8_t));
        response_p->size = size_data;
        memcpy(response_p->data, (void *)data_p, size_data);

        response_p->retval = UICC_REQUEST_STATUS_OK;
        response_p->cause = STE_UICC_CAUSE_REQ_READ_SIM_FILE_RECORD;
        response_p->client_tag = ct;

        TESTCHECK(sendStubResponse(SIMPBD_TEST_STE_UICC_SIM_FILE_READ_RECORD, response_p, size));
    }


    /* wait for stub socket to send us the get pbr info request from pb STE_SIMPB_REQUEST_ID_CACHE_PBR*/
    {
        simpbd_test_ste_uicc_sim_file_read_record_req_t req;
        simpbd_test_ste_sim_read_resp_t *response_p = NULL;
        ste_uicc_sim_file_read_record_response_t *data_p = NULL;
        int size = 0;
        int size_data = 0;

        TESTCHECK(getStubEvent(1000, SIMPBD_TEST_STE_UICC_SIM_FILE_READ_RECORD, &data_p, &size))

        memcpy(&req, data_p, sizeof(req));
        free(data_p);
        ct = req.client_tag;
        /* need to send the response back */

        size_data = sizeof(ste_uicc_sim_file_read_record_response_t) + sim_pbr_data.length;
        data_p = (ste_uicc_sim_file_read_record_response_t *)calloc(size_data, sizeof(uint8_t));
        data_p->uicc_status_code = STE_UICC_STATUS_CODE_OK;
        data_p->uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        data_p->status_word.sw1 = sim_pbr_data.sw1;
        data_p->status_word.sw2 = sim_pbr_data.sw2;
        data_p->data = 0;  /* 0 address as data is piggy-backed in packet */
        memcpy((uint8_t *)(&(data_p->length) + 1), sim_pbr_data.data, sim_pbr_data.length);
        data_p->length = sim_pbr_data.length;

        size = size_data + sizeof(simpbd_test_ste_sim_read_resp_t);
        response_p = (simpbd_test_ste_sim_read_resp_t *)calloc(size, sizeof(uint8_t));
        response_p->size = size_data;
        memcpy(response_p->data, (void *)data_p, size_data);

        response_p->retval = UICC_REQUEST_STATUS_OK;
        response_p->cause = STE_UICC_CAUSE_REQ_READ_SIM_FILE_RECORD;
        response_p->client_tag = ct;

        TESTCHECK(sendStubResponse(SIMPBD_TEST_STE_UICC_SIM_FILE_READ_RECORD, response_p, size));
    }

    /* wait for stub socket to send us the get pbr info request from pb STE_SIMPB_REQUEST_ID_CACHE_PBR*/
    {
        simpbd_test_ste_uicc_sim_file_read_record_req_t req;
        simpbd_test_ste_sim_read_resp_t *response_p = NULL;
        ste_uicc_sim_file_read_record_response_t *data_p = NULL;
        int size = 0;
        int size_data = 0;

        TESTCHECK(getStubEvent(1000, SIMPBD_TEST_STE_UICC_SIM_FILE_READ_RECORD, &data_p, &size))

        memcpy(&req, data_p, sizeof(req));
        free(data_p);
        ct = req.client_tag;
        /* need to send the response back */

        size_data = sizeof(ste_uicc_sim_file_read_record_response_t) + sim_pbr_data.length;
        data_p = (ste_uicc_sim_file_read_record_response_t *)calloc(size_data, sizeof(uint8_t));
        data_p->uicc_status_code = STE_UICC_STATUS_CODE_FAIL;
        data_p->uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        data_p->status_word.sw1 = 0x63;
        data_p->status_word.sw2 = 0x40;
        data_p->data = 0;  /* 0 address as data is piggy-backed in packet */
        data_p->length = 0;

        size = size_data + sizeof(simpbd_test_ste_sim_read_resp_t);
        response_p = (simpbd_test_ste_sim_read_resp_t *)calloc(size, sizeof(uint8_t));
        response_p->size = size_data;
        memcpy(response_p->data, (void *)data_p, size_data);

        response_p->retval = UICC_REQUEST_STATUS_OK;
        response_p->cause = STE_UICC_CAUSE_REQ_READ_SIM_FILE_RECORD;
        response_p->client_tag = ct;

        TESTCHECK(sendStubResponse(SIMPBD_TEST_STE_UICC_SIM_FILE_READ_RECORD, response_p, size));
    }

    {
        /* CLIENT ACTION Data returning*/

        ste_simpb_cb_read_data_t *read_cb_data_p = NULL;
        uint8_t *read_data_p = NULL;
        int data_size = 0;
        int i = 0;
        int fd = ste_simpb_fd_get(ste_simpb_p);

        waitForEvent(1000, fd);
        TESTSIMPB(ste_simbp_receive(ste_simpb_p));
    }

    return SIMPB_TEST_SUCCESS;

error:

    return SIMPB_TEST_FAIL;
}

simpb_test_state_type_t take_down(int state, ste_simpb_t **ste_simpb_pp)
{
    void *data_p;
    int size;
    uintptr_t ct = 144;
    ste_simpb_t *ste_simpb_p = *ste_simpb_pp;

    TESTSIMPB(ste_simpb_shutdown(ste_simpb_p, 0));

    {
        simpbd_test_ste_sim_disconnect_req_t req;
        simpbd_test_ste_sim_disconnect_resp_t response;
        TESTCHECK(getStubEvent(1000, SIMPBD_TEST_STE_SIM_DISCONNECT, &data_p, &size));
        memcpy(&req, data_p, sizeof(req));
        free(data_p);
        ct = req.client_tag;
        /* need to send the response back */
        response.retval = 0;
        TESTCHECK(sendStubResponse(SIMPBD_TEST_STE_SIM_DISCONNECT, &response, sizeof(response)));
    }

    {
        /* CLIENT ACTION Data returning*/

        ste_simpb_cb_read_data_t *read_cb_data_p = NULL;
        uint8_t *read_data_p = NULL;
        int data_size = 0;
        int i = 0;
        int fd = ste_simpb_fd_get(ste_simpb_p);

        waitForEvent(1000, fd);
        TESTSIMPB(ste_simbp_receive(ste_simpb_p));
    }

    return SIMPB_TEST_SUCCESS;

error:

    return SIMPB_TEST_FAIL;
}

/*
 * call-back function
 */

void ste_simpb_cb(ste_simpb_cause_t     cause,
                  uintptr_t             client_tag, /* client_tag == 0 is UNSOLICITED EVENT */
                  simpbd_request_id_t   request_id,
                  void                 *data_p,
                  ste_simpb_result_t    result,
                  void                 *client_context)
{
    printf("TEST: Callback function ste_simpb_cb called - Client is done with request/response \n");
    ste_simpb_callback_data.cb_cause = cause;
    ste_simpb_callback_data.cb_client_tag = client_tag;
    ste_simpb_callback_data.cb_request_id = request_id;
    ste_simpb_callback_data.cb_data_p = data_p; // since we share memory with libsimpb the data pointer is valid
    ste_simpb_callback_data.cb_result = result;
    ste_simpb_callback_data.cb_client_context = client_context;

    return;
}

/*
 * Test framework Implementation
 */

int main()
{
    int runtestresult = 0;
    printf("####################\n");
    printf("# Starting test\n");
    printf("####################\n");

    /* init the listening sockets */
    if (connectToStub() < 0) {
        printf("Could not connect to stub\n");
        return 0;
    }

    printf("Starting test execution\n");
    nbrOfTests = sizeof(testcases) / sizeof(testcase_t);
    printf("Number of tests: %x\n", nbrOfTests);

    runtestresult = runtests();
    printf("Testrun finished\n");
    closeSockets();

    if (runtestresult == -1) {
        printf("Testframework error");
    } else {
        printResults();
    }

    return 0;
}

/*
 * Method used for executing tests and checking results
 */
int runtests()
{
    int i;
    simpb_test_state_type_t result;
    ste_simpb_t *ste_simpb_p = NULL;

    for (i = 0; i < nbrOfTests; i++) {
        printf("########### Running testcase:%d - %s #############\n", i + 1,
               testcases[i].test_string);

        result = testcases[i].simpb_test_fp(0, &ste_simpb_p);
        sleep(1);

        switch (result) {
        case SIMPB_TEST_SUCCESS:
            nbrOfPass++;
            printf("######### test PASS ###########\n");
            break;
        case SIMPB_TEST_FAIL:
            nbrOfFail++;
            printf("######### test FAIL ###########\n");
            break;
        default:
            printf("ERROR: invalid result from testcase: %d\n", result);
            return -1;
        }
    }

    return 0;
}

/*
 * Internal method used to setup connections towards the stub
 */
int connectToStub()
{
    struct sockaddr_un addr;
    int fd = -1;

    if (stubfd_ts < 0) {
        /*Setting up connection test -> stub */
        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, simpbd_test_socket_name_ts, sizeof(addr.sun_path));
        fd = socket(AF_UNIX, SOCK_DGRAM, 0);

        if (fd < 0) {
            printf("Failed to create socket\n");
            goto error_socket;
        }

        stubfd_ts = fd;
    }

    if (stubfd_st < 0) {
        /* Setting up connectio stub -> test */
        fd = socket(AF_UNIX, SOCK_DGRAM, 0);

        if (fd < 0) {
            printf("Failed to create socket\n");
            goto error_socket;
        }

        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, simpbd_test_socket_name_st, sizeof(addr.sun_path));

        /* Bind the socket */
        unlink(addr.sun_path);

        if (bind(fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
            printf("bind() returns error(%d)=\"%s\"\n", errno, strerror(errno));
            return -1;
        }

        stubfd_st = fd;
    }

    /* here both sockets are setup */
    return 0;

error_socket:

    if (stubfd_ts > -1) {
        close(stubfd_ts);
    }

    if (stubfd_st > -1) {
        close(stubfd_st);
    }

    return -1;
}

/*
 *  This should be called when all tests are done to shut down test sockets
 */
void closeSockets()
{
    printf("Closing test and stub sockets\n");

    if (stubfd_ts > -1) {
        close(stubfd_ts);
    }

    if (stubfd_st > -1) {
        close(stubfd_st);
    }

    stubfd_st = -1;
    stubfd_ts = -1;
    return;
}

/*
 * This method is used to send responses to the stub
 */
int sendStubResponse(simpbd_test_request_id_t request_id, void *data_p,
                     int datasize)
{
    simpb_test_message_t simpb_test_message;
    void *message_data = NULL;
    int result = -1;

    printf("[%d] [test.c] Enter sendStubResponse with request_id = %d\n", getpid(), request_id);
    message_data = malloc(datasize + sizeof(simpb_test_message_t));
    memset(&simpb_test_message, 0, sizeof(simpb_test_message));
    memset(message_data, 0, sizeof(*message_data));
    simpb_test_message.request_id = request_id;
    memcpy(message_data, &simpb_test_message, sizeof(simpb_test_message_t));
    memcpy(message_data + sizeof(simpb_test_message_t), data_p, datasize);

    if (sendToStub(datasize + sizeof(simpb_test_message_t), message_data) < 0) {
        printf("Failed to send response to stub\n");
    } else {
        result = 0;
    }

    free(message_data);
    return result;
}

/*
 * Used to send callback indications and data to the daemon
 */
int sendCallback(simpbd_test_request_id_t request_id, void *data_p, int datasize)
{
    return sendStubResponse(request_id, data_p, datasize);
}

/*
 * Wait for a event from stub. If received before timeout, the data is also read.
 * Note that *data_pp needs to be freed upon successful return
 */
int getStubEvent(int timeout, simpbd_test_request_id_t request_id,
                 void **data_pp, int *size_p)
{
    void *testData_p;
    int size;

    /* First wait for an event from stub */
    if (waitForStubEvent(timeout) < 0) {
        printf("[%d] [test.c] getStubEvent with request_id = %d timed out!\n", getpid(), request_id);
        return -1;
    }

    /* read from stub socket */
    if (readFromStub(&size, &testData_p) < 0) {
        return -1;
    } else {
        simpb_test_message_t message;
        /* extract message header */
        memcpy(&message, testData_p, sizeof(simpb_test_message_t));

        /* check if it was the correct event */
        if (message.request_id != request_id) {
            printf("[%d] [test.c] getStubEvent, Wrong response received, expected %d, got %d\n",  getpid(), request_id,
                   message.request_id);
            free(testData_p);
            return -1;
        }

        /* move the memory pointer to point to the response data */
        *data_pp = malloc(size - sizeof(simpb_test_message_t));
        memcpy(*data_pp, testData_p + sizeof(simpb_test_message_t), size
               - sizeof(simpb_test_message_t));
        *size_p = size - sizeof(simpb_test_message_t);
        free(testData_p);
        return 0;
    }
}

/**
 *  Method used to wait for an activity on the stub->testapp filedescriptor
 */
int waitForStubEvent(int timeout)
{
    return waitForEvent(timeout, stubfd_st);
}

/*

 * General method for listening to a file descriptor
 */
int waitForEvent(int timeout, int fd)
{
    fd_set rfds;
    struct timeval tv, *tvp;
    int result = 0;

    if (fd < 0) {
        printf("Error fd <0 ");
        return -1;
    }

    /* Set up timeout structure */
    if (timeout < 0) {
        tvp = NULL;
    } else {
        tv.tv_sec = timeout / 1000;
        tv.tv_usec = (timeout % 1000) * 1000;
        tvp = &tv;
    }

    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);

    result = select(fd + 1, &rfds, NULL, NULL, tvp);

    if (result == -EINTR) {
        printf("select returned because of a signal\n");
        return -1;
    } else if (result == 0) {
        printf("Timeout waiting for event\n");
        return -1;
    } else if (FD_ISSET(fd, &rfds)) {
        /* we got an event before timeout */
        return 0;
    } else {
        printf("Error, select returned without fd set");
        return -1;
    }
}

/*
 * Method for sending data to the stub
 */
int sendToStub(int writesize, void *data_p)
{

    int n = 0;
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, simpbd_test_socket_name_ts, sizeof(addr.sun_path));

    n = sendto(stubfd_ts, data_p, writesize, 0, (struct sockaddr *) &addr,
               sizeof(addr));

    if (n < 0) {
        printf("[%d] [%s] [%s] [%d] sendToStub returns error(%d)=\"%s\" (n=%d)\n", getpid(), __FILE__, __FUNCTION__, errno, strerror(errno), n);
    }

    return 0;
}

/**
 * Method used for reading data from the stub filedescriptor
 * if successful return data_p must be freed
 */
int readFromStub(int *readsize_p, void **data_pp)
{
    int n = 0;
    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;

    if (data_pp == NULL) {
        printf("data_pp == NULL");
        return -1;
    }

    *data_pp = malloc(MAX_READ_BUFFER_SIZE);
    memset(*data_pp, 0, MAX_READ_BUFFER_SIZE);
    memset(&addr, 0, sizeof(addr));

    strncpy(addr.sun_path, simpbd_test_socket_name_st, sizeof(addr.sun_path));
    int addrlen = sizeof(addr);

    n = recvfrom(stubfd_st, *data_pp, MAX_READ_BUFFER_SIZE, 0,
                 (struct sockaddr *) &addr, (socklen_t *) &addrlen);

    if (n < 0) {
        printf("recvfrom returns error(%d)=\"%s\"\n", errno, strerror(errno));
        free(*data_pp);
        return -1;
    }

    if (n == MAX_READ_BUFFER_SIZE) {
        printf("Warning, read full buffer. Extend buffersize.\n");
    }

    *readsize_p = n;
    return 0;
}

/*
 * Method used to print results
 */
void printResults()
{
    printf("####################\n");
    printf("# Summary:\n");
    printf("####################\n");
    printf("Number of tests: %d\n", nbrOfTests);
    printf("Number of tests passed: %d (%d percent)\n", nbrOfPass, (int)(100
            * nbrOfPass / nbrOfTests));
    printf("####################\n");
}

