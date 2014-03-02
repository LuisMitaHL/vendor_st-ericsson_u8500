/*****************************************************************************
 *  ST-Ericsson GPS Technology
 *  Copyright (C) ST-Ericsson SA 2009. All rights reserved.
 *  This code is ST-Ericsson proprietary and confidential.
 *  Any use of the code for whatever purpose is subject to
 *  specific written permission of ST-Ericsson.
 *
 *  Author: Balaji Natakala
 *  Email : balaji.natakala@stericsson.com
 *****************************************************************************/
#define SIMCLIENT_SOCKETHANDLING_C

#include "amil.h"
#include "simClient.h"
#include "sim.h"

#define K_SIMCLIENT_UNDEFINED_SOCKET          (-1)

//#define K_SIMCLIENT_UICC_INIT_THREAD_ID_VALUE (0)
#define SIMCLIENT_CT_CAUSE_CONNECT            (0)
#define SIMCLIENT_CT_CAUSE_READ_IMSI          (1)

#define INVALID_CELLINFOTYPE                  (-1)
#define INVALID_REGSTATE                      (-1)

typedef struct {
    int fd;
    ste_sim_t *ste_sim_p;
} sim_client_t;

static int v_SimReqRespSckt         = K_SIMCLIENT_UNDEFINED_SOCKET;
static sim_client_t sim_client      = { 0, NULL };

//static void *simClient1_2SocketHandlerThread(void *threadid);
static void simClient1_3CreateSimSession(void);
static void simClient1_4UpdateIMSI(char *imsi);
static void simClient1_5SimCallBackFunction(int cause, uintptr_t client_tag, void *data, void *user_data);
static bool simClient1_7HandleSocketSelect(void* pp_FileDes, int32_t vl_SelectReturn, int8_t vp_handle);


/**
 * Initializes SIM client parameters
 */

#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 1

bool simClient1_1Init(int8_t vp_handle) {
    bool vl_Error = FALSE;

    if (v_SimReqRespSckt == K_SIMCLIENT_UNDEFINED_SOCKET) {
        // Create session with the SIM server
        simClient1_3CreateSimSession();
    }

    if (v_SimReqRespSckt != K_SIMCLIENT_UNDEFINED_SOCKET)
    {
        Amil1_06RegisterForListen(v_SimReqRespSckt, &simClient1_7HandleSocketSelect, vp_handle);
    }
    else
    {
        vl_Error = TRUE;
        ERR("SIM Socket creation failed %d\n", v_SimReqRespSckt);
    }

    return vl_Error;
}

#if 0
/**
 * @brief Thread created for the SIM Client. Creates a sessions with
 *        the SIM Server. It then starts infinite receive loop.
 *
 * @param threadid  Thread ID passed in from pthread_create(). It's not currently
 *                   used.
 */

#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 2

static void *simClient1_2SocketHandlerThread(void *threadid)
{
    //cn_error_code_t cn_result;
    INF("%s:SocketHandlerThread Started",__func__);

    if (!sim_sync_p) {
        // Create session with Sim UICC server
        simClient1_3CreateSimSession();
    }

    pthread_exit(NULL);
    return NULL;
}
#endif

/**
 * @brief Creates the SIM Server session and requests for IMSI read
 */

#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 3

static void simClient1_3CreateSimSession(void)
{
    ste_sim_closure_t sim_closure;
    int result;

    INF("%s: called\n",__func__);

    sim_closure.func = simClient1_5SimCallBackFunction;
    sim_closure.user_data = &sim_client;

    sim_client.ste_sim_p = ste_sim_new_st(&sim_closure);

    if (sim_client.ste_sim_p == NULL) {
        ERR("%s: ste_sim_new_st() failed!\n", __func__);
        goto error;
    }

    result = ste_sim_connect(sim_client.ste_sim_p, SIMCLIENT_CT_CAUSE_CONNECT);

    if (STE_SIM_SUCCESS != result) {
        ERR("%s: ste_sim_connect failed, result %d!", __func__, result);
        goto error;
    }

    sim_client.fd = ste_sim_fd(sim_client.ste_sim_p);

    if (sim_client.fd < 0) {
        ERR("%s: ste_sim_fd failed, fd %d!", __func__, sim_client.fd);
        goto error;
    }
    v_SimReqRespSckt = sim_client.fd;

    INF("%s: done\n", __func__);
    return;

error:
    if (sim_client.ste_sim_p) {
        ste_sim_delete(sim_client.ste_sim_p, SIMCLIENT_CT_CAUSE_CONNECT);
        INF("%s: ste_sim_delete() Called\n", __func__);
        sim_client.ste_sim_p = NULL;
    }

    return;
}


/**
 * @brief Waits for data on Callnet server sockets. When data is detected
 *        it is passed to the appropriate handler functions.
 */
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 4

void simClient1_4UpdateIMSI(char *imsi)
{
    s_gnsSUPL_MobileInfo vl_MobileInfo;

    INF("%s: called\n", __func__);

    memset (&vl_MobileInfo, 0, sizeof(vl_MobileInfo));
    vl_MobileInfo.v_CellInfoType = INVALID_CELLINFOTYPE;
    vl_MobileInfo.v_RegistrationState = INVALID_REGSTATE;
    if( imsi[0] != '\0' )
    {
        memcpy( vl_MobileInfo.a_IMSI,  imsi , sizeof(vl_MobileInfo.a_IMSI) );
    }

    GNS_SuplMobileInfoInd(&vl_MobileInfo);

    INF("%s: done\n", __func__);
}

/**
 * @brief Waits for data on Callnet server sockets. When data is detected
 *        it is passed to the appropriate handler functions.
 */

#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 5
static void simClient1_5SimCallBackFunction(int cause, uintptr_t client_tag, void *data, void *user_data)
{
    ste_uicc_sim_file_read_imsi_response_t *imsi_p = NULL;
    sim_client_t *sim_client_p = (sim_client_t *)user_data;
    int i;
    static int imsi_read_done = 0;

    switch (cause) {
        case STE_SIM_CAUSE_CONNECT:
        {
            int result = ste_uicc_sim_get_state(sim_client.ste_sim_p, SIMCLIENT_CT_CAUSE_CONNECT);

            if (STE_SIM_SUCCESS != result) {
                ERR("%s: ste_uicc_sim_get_state failed, result %d!", __func__, result);
            }
        }
        break;
        case STE_UICC_CAUSE_NOT_READY:
            INF("%s: Got STE_UICC_CAUSE_NOT_READY!\n", __func__);
            break;
        case STE_UICC_CAUSE_SIM_STATE_CHANGED:
        {
            ste_uicc_sim_state_changed_t *state_change = (ste_uicc_sim_state_changed_t *) data;
            INF("%s: STE_UICC_CAUSE_SIM_STATE_CHANGED, state %d\n", __func__, state_change->state);

            if (state_change && state_change->state == SIM_STATE_READY && imsi_read_done != 1) {
                uicc_request_status_t uicc_request_status;

                uicc_request_status = ste_uicc_sim_file_read_imsi(sim_client.ste_sim_p, SIMCLIENT_CT_CAUSE_READ_IMSI);

                if (UICC_REQUEST_STATUS_OK != uicc_request_status) {
                    ERR("%s: ste_uicc_sim_file_read_imsi failed, result = %d\n", __func__, uicc_request_status);
                }
            }
        }
        break;
        case STE_UICC_CAUSE_REQ_GET_SIM_STATE:
        {
            ste_uicc_get_sim_state_response_t *sim_state = (ste_uicc_get_sim_state_response_t *) data;
            uicc_request_status_t uicc_request_status;
            INF("%s: STE_UICC_CAUSE_REQ_GET_SIM_STATE, state %d\n", __func__, sim_state->state);

            if (sim_state && sim_state->error_cause == 0 && sim_state->state == SIM_STATE_READY && imsi_read_done != 1) {
                uicc_request_status = ste_uicc_sim_file_read_imsi(sim_client.ste_sim_p, SIMCLIENT_CT_CAUSE_READ_IMSI);

                if (UICC_REQUEST_STATUS_OK != uicc_request_status) {
                    ERR("%s: ste_uicc_sim_file_read_imsi failed, result = %d\n", __func__, uicc_request_status);
                }
            }
        }
        break;
        case STE_UICC_CAUSE_REQ_SIM_FILE_READ_IMSI:
        {
            imsi_p = (ste_uicc_sim_file_read_imsi_response_t *)(data);
            INF("%s: STE_UICC_CAUSE_REQ_SIM_FILE_READ_IMSI\n", __func__);
            INF("#\tREAD UICC STATUS CODE = %d (success=%d)\n", imsi_p->uicc_status_code, STE_UICC_STATUS_CODE_OK);
            INF("#\tREAD UICC STATUS CODE DETAILS = %d,\tREAD STATUS WORD 1 = %d,\tREAD STATUS WORD 2 = %d\n", imsi_p->uicc_status_code_fail_details,imsi_p->status_word.sw1,imsi_p->status_word.sw2);
            if ( imsi_p->uicc_status_code == STE_UICC_STATUS_CODE_OK ) {
                INF("#\tRaw IMSI:");
                for ( i = 0 ; i < SIM_EF_IMSI_LEN ; i++ ) {
                    INF( " %02x", imsi_p->raw_imsi[i] );
                }
                INF("#\tIMSI String:%s\n", imsi_p->imsi );
                simClient1_4UpdateIMSI(imsi_p->imsi);

                imsi_read_done = 1;  // Done!
            }
        }
        break;
    }
}



#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 6

void simClient1_6DeInit(int8_t vp_handle)

{
    INF("%s: called\n", __func__);

    if (v_SimReqRespSckt != K_SIMCLIENT_UNDEFINED_SOCKET)
    {
        Amil1_07DeRegister(v_SimReqRespSckt, vp_handle);
        v_SimReqRespSckt = K_SIMCLIENT_UNDEFINED_SOCKET;
    }

    if (sim_client.ste_sim_p)
    {
        // This will disconnect and then delete the ste_sim_t SIM object.
        ste_sim_delete(sim_client.ste_sim_p, SIMCLIENT_CT_CAUSE_CONNECT);
        INF("%s: ste_sim_disconnect() Called\n", __func__);
        sim_client.ste_sim_p = NULL;
    }
    else
    {
        INF("%s: SIM object pointer is NULL\n", __func__);
    }

    INF("%s: done\n", __func__);
}


/**
 * @brief Waits for data on the SIM server socket. When data is detected
 *        it is passed to the appropriate handler functions.
 */
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 7
static bool simClient1_7HandleSocketSelect(void* pp_FileDes, int32_t vl_SelectReturn, int8_t vp_handle)
{
    bool vl_closed = FALSE;
    int status;

    if (vl_SelectReturn > 0)
    {
        if (sim_client.ste_sim_p)
        {
            status = ste_sim_read(sim_client.ste_sim_p);
            if (status < 0)
            {
                // Contact lost with SIM server, close and clean-up at this end
                ERR("%s: SIM server has disconnected!\n", __func__);
                vl_closed = TRUE;
            }
        }
        else
        {
            ERR("%s: SIM object pointer is NULL!\n", __func__);
            vl_closed = TRUE;
        }
    }

    return vl_closed;
}
