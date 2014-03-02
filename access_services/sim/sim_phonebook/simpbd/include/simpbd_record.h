/*                               -*- Mode: C -*-
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Description: simpbd record handling functions
 *
 * Author: Mats Bengtsson <mats.m.bengtson@stericsson.com>
 *
 */

#ifndef __simpbd_record_h__
#define __simpbd_record_h__ (1)

/* common functions used. record handling */
void simpbd_record_init(void);

simpbd_record_t *simpbd_record_create(simpbd_request_t  request,
                                      simpbd_client_t  *simpbd_client_p,
                                      uint8_t           request_id,
                                      void             *request_data_p,
                                      void             *caller_data_p);

uintptr_t simpbd_record_get_sim_tag(const simpbd_record_t *record_p);

simpbd_record_t *simpbd_record_from_sim_tag(const uintptr_t sim_tag);

void simpbd_record_free(simpbd_record_t *simpbd_record_p);

void simpbd_record_free_all(void);

simpbd_record_t *simpbd_record_get_first_non_free(void);

ste_simpb_result_t simpbd_execute(simpbd_client_t  *simpbd_client_p,
                                  uint8_t              request_id,
                                  void                *request_data_p,
                                  void                *caller_data_p);

simpbd_request_t simpbd_get_request(uint8_t request_id);

#endif /* __simpbd_record_h__ */
