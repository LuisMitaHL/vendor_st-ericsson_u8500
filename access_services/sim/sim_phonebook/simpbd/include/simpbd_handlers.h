/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Description: internal API for handlers in simpbd
 *
 * Author:  Dag Gullberg <dag.xd.gullberg@stericsson.com>
 *          Mats Bengtsson <mats.m.bengtsson@stericsson.com>
 */

#ifndef _INCLUSION_GUARD_SIMPBD_HANDLERS_H_
#define _INCLUSION_GUARD_SIMPBD_HANDLERS_H_

#include "simpbd_common.h"

/* Handler function prototypes */
ste_simpb_result_t simpbd_handler_startup(simpbd_record_t *simpbd_record_p);
ste_simpb_result_t simpbd_handler_shutdown(simpbd_record_t *simpbd_record_p);

ste_simpb_result_t simpbd_handler_cache_pbr(simpbd_record_t *simpbd_record_p);
ste_simpb_result_t simpbd_handler_adn_uid_update(simpbd_record_t *simpbd_record_p);

ste_simpb_result_t simpbd_handler_cache_adn_info(simpbd_record_t *simpbd_record_p);
ste_simpb_result_t simpbd_handler_cache_uid_info(simpbd_record_t *simpbd_record_p);
ste_simpb_result_t simpbd_handler_update_psc(simpbd_record_t *simpbd_record_p);

ste_simpb_result_t simpbd_handler_read(simpbd_record_t *simpbd_record_p);
ste_simpb_result_t simpbd_handler_update(simpbd_record_t *simpbd_record_p);
ste_simpb_result_t simpbd_handler_status_get(simpbd_record_t *simpbd_record_p);
ste_simpb_result_t simpbd_handler_file_information_get(simpbd_record_t *simpbd_record_p);
ste_simpb_result_t simpbd_handler_phonebook_select(simpbd_record_t *simpbd_record_p);
ste_simpb_result_t simpbd_handler_selected_phonebook_get(simpbd_record_t *simpbd_record_p);
ste_simpb_result_t simpbd_handler_supported_phonebooks_get(simpbd_record_t *simpbd_record_p);
ste_simpb_result_t simpbd_handler_sync_ef_pbc(simpbd_record_t *simpbd_record_p);
ste_simpb_result_t simpbd_internal_handler_get_phonebooks(simpbd_record_t *simpbd_record_p);

#endif /* _INCLUSION_GUARD_SIMPBD_HANDLERS_H_ */
