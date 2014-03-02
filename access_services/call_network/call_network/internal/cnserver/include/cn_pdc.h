/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __cn_predial_checker_h__
#define __cn_predial_checker_h__ (1)

#include "cn_data_types.h"
#include "cn_message_types.h"
#include "message_handler.h"

/**
 * \fn request_status_t cn_pdc_handle_request(cn_message_type_t type, void *data_p, request_record_t *record_p)
 * \brief Message handler function for pre-dial checking.
 *
 * Handler function for all messages related to pre-dial checking. This handler is used instead of the
 * traditional ones that haven't pre-dial checks. Full control is delegated to the pre-dial subsystem.
 *
 * \param [in]  data_p      Pointer to message data, see cn_client.h for more information.
 * \param [out] size_p      Pointer to request record.
 *
 * \return  \ref request_status_t   Request status
 *
 */
request_status_t cn_pdc_handle_request(void *data_p, request_record_t *record_p);


#endif /* __predial_checker_h__ */
