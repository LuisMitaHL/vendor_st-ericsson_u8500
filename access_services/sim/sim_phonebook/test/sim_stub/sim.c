/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Description     : sim stub
 *
 * Author          : Christian Nilsson <christian.c.nilsson@stericsson.com>
 *
 */

#include <sim.h>
#include <stdio.h>
#include "test.h"
#include <sys/socket.h>
#include <errno.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#define SIM_STUB_PRINT(a) \
    printf("[%d] [%s] [%s] %s\n", getpid(), __FILE__, __FUNCTION__, a);

static ste_sim_closure_t sim_closure;

int stubfd_st = -1;
int stubfd_ts = -1;

static ste_sim_t *ste_sim_p;

#define SIM_STUB_REQUEST_PHASE 0
#define SIM_STUB_RESPONSE_PHASE 1

static sim_stub_cmd_phase = SIM_STUB_REQUEST_PHASE;

/********************************/
/* Private function declarations */
/********************************/
int stubConnection();
void closeSockets();
int sendTestRequest(simpbd_test_request_id_t request_id, void *data_p,
                    int datasize);
int sendToTest(int datasize, void *data_p);
int getTestEvent(int timeout, simpbd_test_request_id_t request_id,
                 void **data_pp, int *size_p);
int waitForTestEvent(int timeout);
int readFromTest(int *readsize_p, void **data_pp);

/*******************/
/* Implementation */
/*******************/

ste_sim_t *ste_sim_new_st(const ste_sim_closure_t *closure)
{
    void *data_p = NULL;
    int size;
    SIM_STUB_PRINT("enter: ste_sim_new_st");
    memcpy(&sim_closure,closure, sizeof(ste_sim_closure_t));

    TESTSTUB(stubConnection());

    ste_sim_p = malloc(sizeof(ste_sim_t));
    return ste_sim_p;
error:
    return NULL;
}

void ste_sim_delete(ste_sim_t *sim, uintptr_t client_tag)
{
    (void)client_tag;
    free(sim);
    return;
}

int ste_sim_connect(ste_sim_t *sim, uintptr_t client_tag)
{
    (void)sim;
    (void)client_tag;
    SIM_STUB_PRINT("enter: ste_sim_connect");
    return 0;
}

int ste_sim_fd(const ste_sim_t *sim)
{
    (void)sim;
    SIM_STUB_PRINT("enter: ste_sim_fd");
    return stubfd_ts;
}

int ste_sim_disconnect(ste_sim_t *sim, uintptr_t client_tag)
{
    void *data_p = NULL;
    int size;
    simpbd_test_ste_sim_disconnect_req_t request;
    simpbd_test_ste_sim_disconnect_resp_t response;
    SIM_STUB_PRINT("enter: ste_sim_disconnect");

    /* send request */
    memset(&request, 0, sizeof(request));
    request.client_tag = client_tag;
    TESTSTUB(sendTestRequest(SIMPBD_TEST_STE_SIM_DISCONNECT, &request, sizeof(request)));

    /* wait for the response */
    TESTSTUB(getTestEvent(1000, SIMPBD_TEST_STE_SIM_DISCONNECT, &data_p, &size));

    memcpy(&response, data_p, sizeof(response));
    free(data_p);

    return response.retval;
error:
    return -1;
}

/*
 * This function is called whenever the simpbd gets indication on the sim fd (used for events)
 */



/* ptr_p is a double pointer to data struct */
uint8_t handle_incoming_on_function(simpbd_test_ste_sim_read_resp_t *response_p)
{
    uint8_t result = 0;

    if (NULL != response_p && NULL != response_p->data) {
        switch(response_p->cause) {
        case STE_UICC_CAUSE_REQ_READ_SIM_FILE_RECORD:

            printf("[%d] [%s] [%s] handle_incoming_on_function size = %d,  sizeof(ste_uicc_sim_file_read_record_response_t) = %d\n", getpid(),__FILE__,__FUNCTION__,
                    response_p->size, sizeof(ste_uicc_sim_file_read_record_response_t));
            /* special handling required to handle READ as data is a blob in a blob, and data need to be formated to expected struct's */
            ste_uicc_sim_file_read_record_response_t *read_record_data_p = (ste_uicc_sim_file_read_record_response_t *)response_p->data;
            if (sizeof(ste_uicc_sim_file_read_record_response_t) < response_p->size) {
               read_record_data_p->data = (uint8_t*)((uint8_t *)(&(read_record_data_p->length))+1);
               /* Note: tricky solution, require the last member in the struct to be "length" */
            }
            break;
        case STE_UICC_CAUSE_REQ_GET_FILE_INFORMATION:

            printf("[%d] [%s] [%s] handle_incoming_on_function size = %d,  sizeof(ste_uicc_get_file_information_response_t) = %d\n", getpid(),__FILE__,__FUNCTION__,
                    response_p->size, sizeof(ste_uicc_get_file_information_response_t));
            /* special handling required to handle READ as data is a blob in a blob, and data need to be formated to expected struct's */
            ste_uicc_get_file_information_response_t *get_file_info_data_p = (ste_uicc_get_file_information_response_t *)response_p->data;
            if (sizeof(ste_uicc_get_file_information_response_t) < response_p->size) {
                get_file_info_data_p->fcp = (uint8_t*)((uint8_t *)(&get_file_info_data_p->length+1));
               /* Note: tricky solution, require the last member in the struct to be "length" */
            }

            {
                int i=0;
                uint8_t* datap = get_file_info_data_p->fcp;
                printf("[%d] [%s] [%s] TEST -> FCP_DATA:\n[%d] [%s] [%s] ",
                        getpid(), __FILE__, __FUNCTION__,getpid(), __FILE__, __FUNCTION__);
                for (i=0; i < get_file_info_data_p->length; i++) {
                    printf("%02X", (uint8_t)datap[i]);
                    if (!((i+1)%8)) {
                        printf("\n[%d] [%s] [%s] ", getpid(), __FILE__, __FUNCTION__);
                    }
                }
                printf("\n");
            }
            break;
        default:
            break;

        }
    }

    return result;
}


static simpbd_test_request_id_t current_req_id = 0 ;

int ste_sim_read(ste_sim_t *sim)
{
    void *data_p = NULL;
    int size;
    int result = -1;
    void *response_data_p = NULL;

    SIM_STUB_PRINT("enter: ste_sim_read");

    simpbd_test_ste_sim_read_resp_t* response_p;

    TESTSTUB(getTestEvent(1000, current_req_id, &data_p, &size));

    response_p = (simpbd_test_ste_sim_read_resp_t *)calloc(1, size);
    memcpy(response_p, data_p, size);

    free(data_p);

    response_data_p = response_p->data;

    (void)handle_incoming_on_function(response_p);

    if (sim_closure.func != NULL) {
        sim_closure.func(response_p->cause, response_p->client_tag, response_data_p, sim_closure.user_data);
    } else {
        printf("error: cb_func_p == NULL");
        goto error;
    }

    result = response_p->retval;

error:
    free(response_p);
    return result;
}

uicc_request_status_t ste_uicc_sim_get_state(ste_sim_t *uicc,
        uintptr_t client_tag)
{
    void *data_p = NULL;
    int size;
    simpbd_test_ste_uicc_sim_get_state_req_t request;
    SIM_STUB_PRINT("enter: ste_uicc_sim_get_state");

    /* send request */
    memset(&request, 0, sizeof(request));
    request.client_tag = client_tag;
    current_req_id = SIMPBD_TEST_STE_UICC_SIM_GET_STATE;
    TESTSTUB(sendTestRequest(SIMPBD_TEST_STE_UICC_SIM_GET_STATE, &request, sizeof(request)));

    return UICC_REQUEST_STATUS_OK;

error:
    return UICC_REQUEST_STATUS_FAILED;
}

uicc_request_status_t ste_uicc_card_status(ste_sim_t * uicc,
                                           uintptr_t client_tag)
{
    void *data_p = NULL;
    int size;
    simpbd_test_ste_uicc_sim_card_status_req_t request;
    SIM_STUB_PRINT("enter: ste_uicc_card_status");

    /* send request */
    memset(&request, 0, sizeof(request));
    request.client_tag = client_tag;
    current_req_id = SIMPBD_TEST_STE_UICC_SIM_CARD_STATUS;
    TESTCHECK(sendTestRequest(SIMPBD_TEST_STE_UICC_SIM_CARD_STATUS, &request, sizeof(request)));

    return UICC_REQUEST_STATUS_OK;

error:
    return UICC_REQUEST_STATUS_FAILED;
}

uicc_request_status_t ste_uicc_get_app_info(ste_sim_t *uicc, uintptr_t client_tag)
{
    void *data_p = NULL;
    int size;
    simpbd_test_ste_uicc_get_app_info_req_t request;

    SIM_STUB_PRINT("enter: ste_uicc_get_app_info");

    /* send request */
    memset(&request, 0, sizeof(request));
    request.client_tag = client_tag;
    current_req_id = SIMPBD_TEST_STE_UICC_GET_APP_INFO;
    TESTSTUB(sendTestRequest(SIMPBD_TEST_STE_UICC_GET_APP_INFO, &request, sizeof(request)));

    return UICC_REQUEST_STATUS_OK;

error:
    return UICC_REQUEST_STATUS_FAILED;
}

uicc_request_status_t ste_uicc_sim_file_read_record(ste_sim_t *uicc,
        uintptr_t client_tag,
        int file_id,
        int offset,
        int length,
        const char *file_path)
{
    void *data_p = NULL;
    int size;
    simpbd_test_ste_uicc_sim_file_read_record_req_t request;

    SIM_STUB_PRINT("enter: ste_uicc_sim_file_read_record");

    /* send request */
    memset(&request, 0, sizeof(request));
    request.client_tag = client_tag;
    request.file_id = file_id;
    request.file_path = file_path;
    current_req_id = SIMPBD_TEST_STE_UICC_SIM_FILE_READ_RECORD;
    TESTSTUB(sendTestRequest(SIMPBD_TEST_STE_UICC_SIM_FILE_READ_RECORD, &request, sizeof(request)));

    return UICC_REQUEST_STATUS_OK;
error:
    return UICC_REQUEST_STATUS_FAILED;
}

uicc_request_status_t ste_uicc_sim_get_file_information(ste_sim_t *uicc,
        uintptr_t client_tag,
        int file_id,
        const char *file_path,
        ste_uicc_sim_get_file_info_type_t type)
{
    void *data_p = NULL;
    int size = 0;
    simpbd_test_ste_uicc_sim_file_get_information_req_t request;

    SIM_STUB_PRINT("enter: ste_uicc_sim_get_file_information");

    /* send request */
    memset(&request, 0, sizeof(request));
    request.client_tag = client_tag;
    request.file_id = file_id;

    current_req_id = SIMPBD_TEST_STE_UICC_SIM_FILE_INFORMATION_GET;
    TESTSTUB(sendTestRequest(SIMPBD_TEST_STE_UICC_SIM_FILE_INFORMATION_GET, &request, sizeof(request)));


    return UICC_REQUEST_STATUS_OK;
error:
    return UICC_REQUEST_STATUS_FAILED;

}

uicc_request_status_t ste_uicc_sim_file_update_record(ste_sim_t *uicc,
        uintptr_t client_tag, int file_id, int record_id, int length,
        const char *file_path, const uint8_t *data)
{
    void *data_p = NULL;
    int size;
    simpbd_test_ste_uicc_sim_file_update_record_req_t *request_p;

    SIM_STUB_PRINT("enter: ste_uicc_sim_file_update_record");

    /* send request */
    request_p = (simpbd_test_ste_uicc_sim_file_update_record_req_t *)calloc(1, sizeof(simpbd_test_ste_uicc_sim_file_update_record_req_t)+length);
    request_p->client_tag = client_tag;
    request_p->file_id = file_id;
    request_p->length = length;
    request_p->record_id = record_id;
    request_p->data = data;
    request_p->file_path, file_path;

    current_req_id = SIMPBD_TEST_STE_UICC_SIM_FILE_UPDATE_RECORD;
    TESTSTUB(sendTestRequest(SIMPBD_TEST_STE_UICC_SIM_FILE_UPDATE_RECORD, request_p, sizeof(*request_p)));

    free(request_p);
    return UICC_REQUEST_STATUS_OK;
error:
    return UICC_REQUEST_STATUS_FAILED;
}

uicc_request_status_t ste_uicc_sim_file_update_binary(ste_sim_t *uicc,
                              uintptr_t client_tag,
                              int file_id,
                              int offset,
                              int length,
                              const char *file_path,
                              const uint8_t *data)
{
    (void)uicc;
    (void)client_tag;
    (void)file_id;
    (void)offset;
    (void)length;
    (void)file_path;
    (void)data;

    printf("SIM_STUB: enter: ste_uicc_sim_file_update_binary (not implemented in stub)");
    return UICC_REQUEST_STATUS_FAILED;
}

uicc_request_status_t ste_uicc_sim_file_get_format(ste_sim_t *uicc,
        uintptr_t client_tag,
        int file_id,
        const char *file_path)
{
    (void)uicc;
    (void)client_tag;
    (void)file_id;
    (void)file_path;

    SIM_STUB_PRINT("enter: ste_uicc_sim_file_get_format (not implemented in stub)");
    return UICC_REQUEST_STATUS_FAILED;
}

uicc_request_status_t ste_uicc_sim_file_read_binary(ste_sim_t *uicc,
                                                    uintptr_t client_tag,
                                                    int file_id,
                                                    int offset,
                                                    int length,
                                                    const char *file_path)
{
    (void)uicc;
    (void)client_tag;
    (void)file_id;
    (void)offset;
    (void)length;
    (void)file_path;

    SIM_STUB_PRINT("enter: ste_uicc_sim_file_read_binary (not implemented in stub)");
    return UICC_REQUEST_STATUS_FAILED;
}

/**********************************************************************************/
/************************** Stub connection handling *****************************/
/**********************************************************************************/
/*
 * Method for setting up the sockets used between test and stub process
 */
int stubConnection()
{
    struct sockaddr_un addr;
    int fd = -1;

    if (stubfd_st < 0) {
        /* Setting up connection stub -> test */
        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, simpbd_test_socket_name_st, sizeof(addr.sun_path));
        fd = socket(AF_UNIX, SOCK_DGRAM, 0);

        if (fd < 0) {
            SIM_STUB_PRINT("Failed to create socket\n");
            goto error_socket;
        }

        stubfd_st = fd;
    }

    if (stubfd_ts < 0) {
        /* Setting up connectio test -> stub */
        fd = socket(AF_UNIX, SOCK_DGRAM, 0);

        if (fd < 0) {
            SIM_STUB_PRINT("Failed to create socket\n");
            goto error_socket;
        }

        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, simpbd_test_socket_name_ts, sizeof(addr.sun_path));

        /* Bind the socket */
        unlink(addr.sun_path);

        if (bind(fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
            printf("[%d] [sim.c] stubConnection: bind() returns error(%d)=\"%s\"\n", getpid(), errno, strerror(errno));
            return -1;
        }

        stubfd_ts = fd;
    }

    printf("[%d] [sim.c] stubConnection: stubfd_ts= %d, stubfd_st=%d\n", getpid(), stubfd_ts, stubfd_st);
    /* Both sockets are setup */
    return 0;

error_socket:

    if (stubfd_st > -1) {
        close(stubfd_st);
    }

    if (stubfd_ts > -1) {
        close(stubfd_ts);
    }

    return -1;
}

/*
 * This method should be called when all tests are done to close test sockets
 */
void closeSockets()
{
    SIM_STUB_PRINT("closeSockets: Closing test and stub sockets\n");

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
 * When a request comes down to simstub this method should be used to forward the request to testapplication
 */
int sendTestRequest(simpbd_test_request_id_t request_id, void *data_p,
                    int datasize)
{
    simpb_test_message_t simpb_test_message;
    void *message_data = NULL;

    SIM_STUB_PRINT(" enter: sendTestRequest");
    simpb_test_message.request_id = request_id;
    message_data = malloc(datasize + sizeof(simpb_test_message_t));
    memset(message_data, 0, sizeof(message_data));
    memcpy(message_data, &simpb_test_message, sizeof(simpb_test_message_t));
    memcpy(message_data + sizeof(simpb_test_message_t), data_p, datasize);

    if (sendToTest(datasize + sizeof(simpb_test_message_t), message_data) < 0) {
        SIM_STUB_PRINT("sendTestRequest: Failed to send request to test app\n");
        free(message_data);
        return -1;
    } else {
        free(message_data);
    }

    return 0;
}

/*
 * Method used internally to send data to the testapplication
 */
int sendToTest(int datasize, void *data_p)
{
    int n = 0;
    struct sockaddr_un addr;

    SIM_STUB_PRINT(" enter: sendToTest");

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, simpbd_test_socket_name_st, sizeof(addr.sun_path));

    n = sendto(stubfd_st, data_p, datasize, 0, (struct sockaddr *) &addr,
               sizeof(addr));

    if (n < 0) {
        printf("[%d] [sim.c] sendToTest: sendto returns error(%d)=\"%s\"\n", getpid(), errno, strerror(errno));
    }

    return 0;
}

/*
 * Waits for and unpacks the event. If another event comes first the method returns error
 * Upon successful return *data_pp needs to be freed
 */
int getTestEvent(int timeout, simpbd_test_request_id_t request_id,
                 void **data_pp, int *size_p)
{
    void *testData_p;
    int size = 0;

    SIM_STUB_PRINT(" enter: getTestEvent");
    /* First wait for an event from test */
    if (waitForTestEvent(timeout) < 0) {
        return -1;
    }

    /* read from test socket */
    if (readFromTest(&size, &testData_p) < 0) {
        return -1;
    } else {
        simpb_test_message_t message;
        /* extract message header */
        memcpy(&message, testData_p, sizeof(simpb_test_message_t));

        /* check if it was the correct event */
        if (message.request_id != request_id && SIMPBD_TEST_STE_SIM_URC != message.request_id) {
            printf("[%d] [sim.c] getTestEvent: Wrong response received, expected %d, got %d\n", getpid(), request_id,
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

/*
 * Waits for an indication on the test filedescriptor. If indication occurs 0 is returned. Otherwise -1.
 */
int waitForTestEvent(int timeout)
{
    fd_set rfds;
    struct timeval tv, *tvp;
    int result = 0;

    SIM_STUB_PRINT(" enter: waitForTestEvent");

    if (stubfd_ts < 0) {
        printf("Error stubfd_ts <0 ");
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
    FD_SET(stubfd_ts, &rfds);

    result = select(stubfd_ts + 1, &rfds, NULL, NULL, tvp);

    if (result == -EINTR) {
        printf("[%d] [sim.c] waitForTestEvent: select returned because of a signal\n", getpid());
        return -1;
    }

    else if (result == 0) {
        printf("[%d] [sim.c] waitForTestEvent: Timeout waiting for event\n", getpid());
        return -1;
    }

    if (FD_ISSET(stubfd_ts, &rfds)) {
        /* we got an event */
        return 0;
    } else {
        printf("[%d] [sim.c] waitForTestEvent: Error, select returned without fd set", getpid());
        return -1;
    }
}

/**
 * Method used for reading data from the stub filedescriptor
 * if successful return *data_pp must be freed
 */
int readFromTest(int *readsize_p, void **data_pp)
{
    int n = 0;
    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;

    SIM_STUB_PRINT("enter: readFromTest");

    if (data_pp == NULL) {
        printf("data_pp == NULL");
        return -1;
    }

    *data_pp = malloc(MAX_READ_BUFFER_SIZE);
    memset(*data_pp, 0, MAX_READ_BUFFER_SIZE);
    memset(&addr, 0, sizeof(addr));

    strncpy(addr.sun_path, simpbd_test_socket_name_ts, sizeof(addr.sun_path));
    int addrlen = sizeof(addr);

    n = recvfrom(stubfd_ts, *data_pp, MAX_READ_BUFFER_SIZE, 0,
                 (struct sockaddr *) &addr, (socklen_t *) &addrlen);

    if (n < 0) {
        printf("[%d] [sim.c] readFromTest: recvfrom returns error(%d)=\"%s\"\n", getpid(), errno, strerror(errno));
        free(*data_pp);
        return -1;
    }

    if (n == MAX_READ_BUFFER_SIZE) {
        SIM_STUB_PRINT("readFromTest: Warning, read full buffer. Extend buffersize.\n");
    }

    *readsize_p = n;
    return 0;
}
