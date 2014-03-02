#ifndef TEEC_HANDLER_H
#define TEEC_HANDLER_H
/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <bass_app.h>
#include <tee_client_api.h>

/*
 * @brief TEEC Invoke Secure World
 *
 * The following function handles the session towards the secure world.
 *
 * @param [in] operation    A pointer to the TEEC_Operation for the commad
                            to be executed
 * @param [in] command_id   The ID of the command to be executed.
 *
 * @return bass_return_code
 * @retval BASS_RC_FAILURE in case of an error
 * @retval BASS_RC_SUCCESS if successful .
 */
bass_return_code teec_invoke_secworld(TEEC_Operation *const operation,
                                      const uint32_t comamnd_id);

#endif
