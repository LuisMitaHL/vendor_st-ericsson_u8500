/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Description: Chache handling of PBR data.
 *
 * Author:  Dag Gullberg <dag.xd.gullberg@stericsson.com>
 */

#ifndef _INCLUSION_GUARD_SIMPBD_CACHE_H_
#define _INCLUSION_GUARD_SIMPBD_CACHE_H_

#include <stdlib.h>

#include "sim.h"
#include "simpb.h"
#include "simpbd_common.h"



#define SIMPB_MAX_EF_RECS 255
/*typedefs*/
typedef struct simpb_adn_file_s {
    uint8_t  tag;
    uint8_t  length;
    uint16_t fid;
    uint8_t  sfi;
    uint8_t  slice;
    uint8_t  instance;
    char    *file_path;
    uint16_t num_records;
    uint8_t  record_len;
    uint16_t index;
    uint16_t num_udpated;
    uint8_t adn_rec_occupied[SIMPB_MAX_EF_RECS];
    struct simpb_adn_file_s *next;
} simpb_adn_file_t;

typedef struct simpb_uid_file_s {
    uint8_t  tag;
    uint8_t  length;
    uint16_t fid;
    uint8_t  sfi;
    uint8_t  slice;
    uint8_t  instance;
    char    *file_path;
    uint16_t num_records;
    uint8_t  record_len;
    uint16_t index;
    uint16_t num_udpated;
    struct simpb_uid_file_s *next;
} simpb_uid_file_t;

typedef struct {
    simpb_adn_file_t *adn_file;
    simpb_uid_file_t *uid_file;
    uint8_t rec_num;
} ste_simpb_occupied_record;

/* cache functions */
ste_simpb_result_t simpbd_create_and_store_pbr_cache(ste_uicc_sim_file_read_record_response_t *data_p, uint8_t slice);
void simpbd_cache_flush_pbr_data();
ste_simpb_result_t simpbd_cache_adn_uid_file_info();
void simpbd_cache_flush_occupied_rec_info();

void simpbd_cache_flush_file_info();
ste_simpb_result_t simpdb_get_cached_occupied_adn_record();
ste_simpb_result_t simpbd_update_cached_adn_record(uint16_t fileid, uint8_t index, uint8_t value);
simpb_uid_file_t *simpbd_get_uid_file_info_for_adn_fid(uint16_t fid);
simpb_uid_file_t *simpb_get_uid_file_info(uint8_t slice_no, uint8_t instance);
simpb_adn_file_t *simpbd_get_adn_file_info(uint16_t fid);

simpb_uid_file_t *simpbd_get_uid_file_info_head();
simpb_adn_file_t *simpbd_get_adn_file_info_head();
ste_simpb_occupied_record *simpbd_get_occ_rec_holder();
simpb_file_ids_t *simpbd_get_adn_file_ids();


/* cache utilities and related functionality */
uint16_t simpbd_cache_get_fid_from_client_file_id(uint8_t file_id, uint8_t instance, uint8_t slice, ste_simpb_phonebook_type_t phonebook, uint8_t app_type);
char *simpbd_cache_get_path_on_selected_phonebook(ste_simpb_phonebook_type_t phonebook,
        uint8_t                    app_type,
        uint8_t                    card_type,
        ste_simpb_file_type_t      file_id);

void simpbd_cache_get_parameters(uint8_t file_id,
                                 uint8_t instance,
                                 uint8_t slice,
                                 uint8_t record_id,
                                 uint8_t p2,
                                 uint8_t selected_phonebook,
                                 uint8_t app_type,
                                 uint8_t card_type,
                                 uint16_t *fid_p,
                                 uint8_t *record_id_p, /* output record_id, current if present otherwise same as record_id */
                                 char **path_pp);

void simbpd_cache_set_current_record_id(uint8_t file_id, uint8_t record_id);


#endif /* _INCLUSION_GUARD_SIMPBD_CACHE_H_ */
