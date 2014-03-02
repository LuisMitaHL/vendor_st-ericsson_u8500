/*************************************************************************
 * Copyright ST-Ericsson 2010
 ************************************************************************/
#include <cops_tapp_common.h>
#include <cops_common.h>
#include <cops_gstate.h>
#include <cops_shared_util.h>
#include <r_sim.h>
#include <t_sim_si.h>
#include <stdlib.h>

/**
 * cops_tapp_local must support the following functions in order to be a
 * trusted application replacement:
 *
 * x Create correct SIPC messages
 * x Insert and verify the counters of all messages
 * x Verify sipc messages
 * x Initialize the counters to 0
 * x Invoke - which basically does all above
 */

#define STATIC_COPS_MAX_IMSI_LEN   (15)
#define STATIC_COPS_MAX_GID_LEVEL  (2)
#define STATIC_COPS_MAX_NBR_OF_CNL_ENRTIES (42)
#define STATIC_COPS_MAX_PLMN_LEN   (3)

typedef struct {
    uint32_t error_code;
    uint8_t  imsi[STATIC_COPS_MAX_IMSI_LEN]; /* 15 */
    uint8_t  gid[STATIC_COPS_MAX_GID_LEVEL]; /* gid[0] = gid1, gid[1] = gid2 */
    uint8_t  nbr_of_cnl_entries;
    uint8_t  padding[2]; /* For 4-byte alignment */
} cops_sim_data_t;

typedef struct {
    uint8_t plmn[STATIC_COPS_MAX_PLMN_LEN];
    uint8_t ns;
    uint8_t sp;
    uint8_t corporate;
} cops_cnl_entry_t;

static bool init = false;

static cops_return_code_t cops_setup_counters(void);

static cops_return_code_t
cops_util_get_cnl(uint8_t *nbr_of_cnl_entries_p,
                  size_t *cnl_data_size_p,
                  cops_cnl_entry_t **const cnl_data_pp);

static cops_return_code_t
cops_read_sim_data(cops_sipc_message_t *msg_in, cops_sipc_message_t **msg_out);

static cops_return_code_t
cops_util_get_cnl(uint8_t *nbr_of_cnl_entries_p,
                  size_t *cnl_data_size_p,
                  cops_cnl_entry_t **const cnl_data_pp)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    SIM_CNL_t *sim_cnl_p = NULL;
    RequestStatus_t request_status;
    SIM_ISO_ErrorCause_t sim_iso_error_cause;

    COPS_DEBUG_CHK_ASSERTION(nbr_of_cnl_entries_p != NULL);
    COPS_DEBUG_CHK_ASSERTION(cnl_data_size_p != NULL);
    COPS_DEBUG_CHK_ASSERTION(cnl_data_pp != NULL);
    COPS_DEBUG_CHK_ASSERTION(*cnl_data_pp == NULL);

    /* Init values */
    *nbr_of_cnl_entries_p = 0;
    *cnl_data_size_p = 0;

    sim_cnl_p = malloc(sizeof(SIM_CNL_t));
    COPS_CHK_ALLOC(sim_cnl_p);

    /* Init cnl data */
    memset((void *)sim_cnl_p, 0, sizeof(SIM_CNL_t));

    request_status = Request_SIM_CoopNetworkList_Read(WAIT_RESPONSE,
                                                    (UICC_ServiceHandle_t)65536,
                                                    sim_cnl_p,
                                                    &sim_iso_error_cause);

    /* Check if we can rely on sim_iso_error_cause */
    if (request_status != REQUEST_OK &&
        request_status != REQUEST_FAILED_APPLICATION) {

        COPS_SET_RC(COPS_RC_SIM_ERROR,
                    "Failed to read cnl. Result=0x%x, ErrorCause=0x%x",
                    request_status,
                    sim_iso_error_cause);
    }

    if (sim_iso_error_cause != SIM_ISO_NORMAL_COMPLETION) {

        COPS_SET_RC(COPS_RC_SIM_ERROR,
                    "Failed to read cnl. sim_iso_error_cause=0x%x",
                    sim_iso_error_cause);
    }

    /* Set cnl data to sim data */
    *nbr_of_cnl_entries_p = sim_cnl_p->NumberOfNetworks;

    if (sim_cnl_p->NumberOfNetworks > 0) {
        size_t alignment_rest;
        uint8_t i;

        /* Get the size of the CNL data */
        *cnl_data_size_p =
            (size_t)(sizeof(cops_cnl_entry_t) * sim_cnl_p->NumberOfNetworks);

        /* Padding for 4-byte alignment? */
        alignment_rest = (*cnl_data_size_p) % 4;

        if (alignment_rest > 0) {
            *cnl_data_size_p += 4 - alignment_rest;
        }

        *cnl_data_pp = malloc(*cnl_data_size_p);
        COPS_CHK_ALLOC(*cnl_data_pp);

        /* Init data */
        memset((void *)*cnl_data_pp, 0, *cnl_data_size_p);

        for (i = 0; i < sim_cnl_p->NumberOfNetworks ; i++) {
            /* Copy packed bitfields */
            memcpy((void *)(*cnl_data_pp)[i].plmn,
                   (const void *)&(sim_cnl_p->CoopNetwork[i].PLMN),
                   sizeof((*cnl_data_pp)[0].plmn));

            memcpy((void *)(&(*cnl_data_pp)[i].ns),
                   (const void *)&(sim_cnl_p->CoopNetwork[i].Subset),
                   sizeof(uint8_t));

            memcpy((void *)(&(*cnl_data_pp)[i].sp),
                   (const void *)&(sim_cnl_p->CoopNetwork[i].SP),
                   sizeof(uint8_t));

            memcpy((void *)(&(*cnl_data_pp)[i].corporate),
                   (const void *)&(sim_cnl_p->CoopNetwork[i].Corporate),
                   sizeof(uint8_t));
        }
    }

function_exit:
    free(sim_cnl_p);
    return ret_code;
}

static cops_return_code_t cops_setup_counters(void)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    if (!init) {
        cops_sipc_session_t *gstate_session;

        gstate_session = ISSWAPI_GSTATE_ALLOC(TEE_GID_COPS_SIPC_MODEM_COUNTER,
                                              sizeof(*gstate_session));
        COPS_CHK_ALLOC(gstate_session);
        gstate_session = 0;

        gstate_session = ISSWAPI_GSTATE_ALLOC(TEE_GID_COPS_SIPC_APE_COUNTER,
                                              sizeof(*gstate_session));
        COPS_CHK_ALLOC(gstate_session);
        gstate_session = 0;

        init = true;
    }
function_exit:
    return ret_code;
}

cops_return_code_t
cops_tapp_invoke(struct cops_state *state, cops_tapp_io_t **tapp_io,
                 cops_taf_id_t taf_id)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_sipc_message_t *msg = NULL;
    cops_sipc_session_t *modem_session = NULL;

    (void) state;
    (void) taf_id;

    COPS_CHK_RC(cops_setup_counters());

    COPS_CHK_RC(cops_data_get_sipc_message(&(*tapp_io)->args, &msg));

    /* If message originated from swbp/api protect it, otherwise verify it */
    if (COPS_SIPC_IS_REQUEST(msg) &&
        COPS_SENDER_UNSECURE == cops_sipc_get_sender_from_msg(msg)) {
        modem_session = ISSWAPI_GSTATE_GET(TEE_GID_COPS_SIPC_MODEM_COUNTER);
        COPS_CHK(modem_session != NULL, COPS_RC_ISSW_ERROR);
        COPS_CHK_RC(cops_sipc_increment_session(modem_session));
        cops_sipc_clear_sender_in_msg(msg);
        COPS_CHK_RC(cops_sipc_protect_message(modem_session, msg, true));
    } else {
        bool mac_ok = false;
        cops_sipc_session_t session = { 0 };
        cops_sipc_message_t *msg_pp = NULL;

        COPS_CHK_RC(cops_sipc_verify_message(msg, &session, &mac_ok, false));
        COPS_CHK_ASSERTION(mac_ok);

        if (COPS_SIPC_READ_SIM_DATA_HREQ == msg->msg_type) {
            COPS_CHK_RC(cops_read_sim_data(msg, &msg_pp));
            cops_sipc_free_message(&msg);
            msg = msg_pp;
            COPS_CHK_RC(cops_sipc_protect_message(&session, msg, mac_ok));
        }
    }

function_exit:

    if (COPS_RC_OK != ret_code) {
        cops_sipc_message_t *temp_msg = NULL;
        size_t offset = 0;

        if (NULL != msg && COPS_SIPC_IS_REQUEST(msg)) {
            (void) cops_sipc_alloc_message(ret_code, sizeof(uint32_t),
                                        (cops_sipc_mtype_t)(msg->msg_type + 1),
                                        COPS_SENDER_SECURE, &temp_msg);
            cops_sipc_free_message(&msg);
            msg = temp_msg;
        }

        if (NULL != msg) {
            (void) cops_tapp_sipc_set_uint32(msg, &offset, ret_code);
        } else {
            ret_code = COPS_RC_INTERNAL_ERROR;
        }
    }

    if (NULL != msg) {
        (void) cops_data_set_sipc_message(&(*tapp_io)->rvs, msg);
        cops_sipc_free_message(&msg);
    }

    return ret_code;
}

static cops_return_code_t
cops_read_sim_data(cops_sipc_message_t *msg_in, cops_sipc_message_t **msg_out)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    bool include_cnl;
    size_t offset = 0;
    RequestStatus_t request_status;
    SIM_ISO_ErrorCause_t sim_iso_error_cause;
    cops_sim_data_t sim_data;
    size_t sim_data_size = sizeof(cops_sim_data_t);
    uint8_t sim_imsi[SIM_IMSI_LEN];
    uint8_t gid_level;
    SIM_GID_t sim_gid;
    uint8_t n;
    cops_cnl_entry_t *cnl_data_p = NULL;
    size_t cnl_data_size = 0;

    COPS_DEBUG_CHK_ASSERTION(msg_in != NULL);
    COPS_DEBUG_CHK_ASSERTION(msg_out != NULL);
    COPS_DEBUG_CHK_ASSERTION(*msg_out == NULL);

    /* Init static sim data */
    memset((void *)&sim_data, 0, sim_data_size);

    /* Read GID1 and GID2 */
    for (n = 0; n < (uint8_t)STATIC_COPS_MAX_GID_LEVEL; n++) {

        gid_level = n + 1;  /* gid_level 1 or 2 */

        /* Init data */
        memset((void *)&sim_gid, 0, sizeof(SIM_GID_t));

        request_status = Request_SIM_GroupIdentifiers_Read(WAIT_RESPONSE,
                                                    (UICC_ServiceHandle_t)65536,
                                                    &gid_level,
                                                    &sim_gid,
                                                    &sim_iso_error_cause);

        /* Check if sim_iso_error_cause is valid */
        if (request_status != REQUEST_OK &&
            request_status != REQUEST_FAILED_APPLICATION) {
            COPS_LOG(LOG_WARNING, "Failed to read gid%d, Result=0x%x, "
                     "using default (0xFF)\n", n + 1, request_status);
        }

        if (sim_iso_error_cause != SIM_ISO_NORMAL_COMPLETION) {
            COPS_LOG(LOG_WARNING, "Failed to read gid%d, "
                     "sim_iso_error_cause=0x%x, using default (0xFF)\n",
                     n + 1, sim_iso_error_cause);
        }

        /* We are only intressed in the first byte of the GID1 and GID2 file */
        if (sim_gid.NumberOfIdentifiers > 0) {
            sim_data.gid[n] = sim_gid.Id[0];
        } else {
            sim_data.gid[n] = (uint8_t)0xFF;
        }
      }

    /* Read imsi */
    memset(sim_imsi, 0, (size_t)SIM_IMSI_LEN);

    request_status = Request_SIM_IMSI_Read(WAIT_RESPONSE,
                                           (UICC_ServiceHandle_t)65536,
                                           sim_imsi,
                                           &sim_iso_error_cause);

    /* Check if we can rely on sim_iso_error_cause */
    if (request_status != REQUEST_OK &&
        request_status != REQUEST_FAILED_APPLICATION){
        COPS_SET_RC(COPS_RC_SIM_ERROR,
          "Failed to read imsi. Result=0x%x", request_status);
    }

    if (sim_iso_error_cause != SIM_ISO_NORMAL_COMPLETION) {
        COPS_SET_RC(COPS_RC_SIM_ERROR,
          "Failed to read imsi. sim_iso_error_cause=0x%x", sim_iso_error_cause);
    }

    /* Convert IMSI to COPS format */
    COPS_CHK_RC(cops_util_unpack_imsi(sim_imsi, sim_data.imsi));

    /* Check if CNL is included */
    COPS_CHK_RC(cops_tapp_sipc_get_bool(msg_in, &offset, &include_cnl));

    if (include_cnl) {
        /* Read CNL */
        COPS_CHK_RC(cops_util_get_cnl(&(sim_data.nbr_of_cnl_entries),
                                      &cnl_data_size,
                                      &cnl_data_p));
    }

    /* Create response message */
    COPS_CHK_RC(cops_sipc_alloc_message(ret_code,
                                     (sim_data_size
                                     + cnl_data_size),
                                     COPS_SIPC_READ_SIM_DATA_HRESP,
                                     COPS_SENDER_SECURE,
                                     msg_out));

    offset = 0;

    /* Set return code */
    sim_data.error_code = ret_code;

    /* Set payload data to message */
    COPS_CHK_RC(cops_tapp_sipc_set_opaque(*msg_out,
                                          &offset,
                                          (const void *)&sim_data,
                                          sim_data_size));

    /* Add CNL payload data to message */
    if (cnl_data_p != NULL) {
        COPS_CHK_RC(cops_tapp_sipc_set_opaque(*msg_out,
                                              &offset,
                                              (const void *)cnl_data_p,
                                              cnl_data_size));
    }

function_exit:
    if (NULL != cnl_data_p) {
        free(cnl_data_p);
    }
    return ret_code;
}

