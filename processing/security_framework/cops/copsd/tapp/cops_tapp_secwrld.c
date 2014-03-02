/*************************************************************************
 * Copyright ST-Ericsson 2010
 ************************************************************************/
#include <cops_tapp_common.h>
#include <cops_common.h>
#include <cops_debug.h>
#ifndef COPS_OSE_ENVIRONMENT
#ifndef COPS_ENABLE_ENV_MODE_ITP
#include <sys/wait.h>
#endif
#endif
#include <string.h>

cops_return_code_t
cops_tapp_invoke(struct cops_state *state, cops_tapp_io_t **tapp_io,
                 cops_taf_id_t taf_id)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    TEEC_Result teec_ret;
    TEEC_Operation teec_op;
#ifdef COPS_USE_GP_TEEV0_17
    TEEC_ErrorOrigin teec_err;
#else
    uint32_t teec_err;
#endif

    memset(&teec_op, 0, sizeof(TEEC_Operation));

    convertCopsIO2TeeOp_n2s(tapp_io, &teec_op);

    teec_ret = TEEC_InvokeCommand(&state->teec_sess,
                                  (uint32_t) taf_id,
                                  &teec_op,
                                  &teec_err);

    if (teec_ret == TEEC_SUCCESS) {
        convertTeeOp2CopsIO_n2s(&teec_op, *tapp_io);
        ret_code = COPS_RC_OK;
    } else {
        COPS_LOG(LOG_ERROR, "Invoke Command for taf_id=%u failed. "
                 "Returned 0x%x, teec_err=0x%x\n",
                 taf_id, (unsigned int) teec_ret, (unsigned int)teec_err);
        ret_code = COPS_RC_TEEC_ERROR;
    }

    cops_debug_print_error_stack(&(*tapp_io)->rvs);

    return ret_code;
}

