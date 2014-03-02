/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Description: Cache handling of PBR data.
 *
 * Author: Dag Gullberg <dag.xd.gullberg@stericsson.com>
 */

#include <string.h>

#include "simpbd_cache.h"
#include "simpbd.h"
#include "simpbd_p.h"
#include "log.h"

/*-----  private type definitions  ------*/

typedef struct simpbd_cache_pbr_s {
    uint8_t  link_type;
    uint8_t  tag;
    uint8_t  length;
    uint16_t fid;
    uint8_t  sfi;
    uint8_t  slice;
    uint8_t  instance;
    uint8_t  current_sim_record_id;
    struct simpbd_cache_pbr_s *next;
} simpbd_cache_pbr_t;

typedef enum {
    ADN_FILE_TAG = 0xC0,
    UID_FILE_TAG = 0xC9
} simpbd_file_tags;

/* Path definitions */
typedef char simpbd_uicc_path_string_t[20];
typedef uint8_t simpbd_uicc_path_uint_t[10];

typedef struct {
    int                       id;
    simpbd_uicc_path_string_t path_usim;
    simpbd_uicc_path_string_t path_usim_parent;
    simpbd_uicc_path_string_t path_gsm;
} simpbd_uicc_path_t;
/*-----  global variables  ------*/




#define SIMBPD_CLIENT_FID_TO_DF_PHONEBOOK_TAG_MAX 16 /* note that also fixed fid EFs need to be included to keep track of current record in next/previous mode */
static const uint16_t  simpbd_client_fid_to_df_phonebook_tag[] = {0xff, 0xC0, 0xC2, 0xCB, 0xC7, 0xC8, 0xC9, 0xC1, 0xC6, 0xC4, 0xCA, 0xC3, 0xC5, 0x00, 0x00, 0x00};
static uint8_t simpbd_cache_instances_for_tags[SIMBPD_CLIENT_FID_TO_DF_PHONEBOOK_TAG_MAX];

static uint8_t simpbd_current_record_id_list[STE_SIMPB_FILE_TYPE_MAX];

static const simpbd_uicc_path_t simpbd_uicc_file_paths_local  = {STE_SIMPB_PHONEBOOK_TYPE_LOCAL,    "3F007FFF5F3A", "3F007FFF", ""};
static const simpbd_uicc_path_t simpbd_uicc_file_paths_global = {STE_SIMPB_PHONEBOOK_TYPE_GLOBAL,   "3F007F105F3A", "3F007F10", "3F007F10"};

static simpbd_cache_pbr_t *simpbd_cache_pbr_p = NULL;
simpb_adn_file_t *simpbd_cache_adn_file_info_head = NULL;
simpb_adn_file_t *simpbd_cache_adn_file_info_tail = NULL;
simpb_uid_file_t *simpbd_cache_uid_file_info_head = NULL;
simpb_uid_file_t *simpbd_cache_uid_file_info_tail = NULL;
simpb_file_ids_t *simpbd_cache_adn_file_ids_g = NULL;
ste_simpb_occupied_record *simpbd_cache_occupied_record_g = NULL;
uint16_t simpb_puid_value = 0xffff;
uint16_t simpb_cc_value = 0xffff;
uint16_t simpb_p_cc_value = 0xffff;
uint32_t simpb_psc_value = 0xffffffff;

/*------- prototypes local functions ----------*/

static void simpbd_cache_get_file_id_and_sfi(uint8_t tag, uint16_t *file_id_p, uint8_t *sfi_p, uint8_t instance, uint8_t slice);

static void simpbd_cache_get_fid_and_current_record_id_from_client_file_id(uint8_t file_id,
        uint8_t instance,
        uint8_t slice,
        ste_simpb_phonebook_type_t phonebook,
        uint8_t app_type,
        uint16_t *fid_p,
        uint8_t *current_record_id_p);
static uint16_t simpbd_get_fixed_fid(uint8_t file_id);

static void simpbd_cache_set_current_record_id_on_file_id(uint8_t tag, uint8_t record_id);

static void simpbd_reset_instances_all(void);
static uint8_t simpbd_get_instance_on_tag(uint8_t tag);
static uint8_t *simpbd_get_instance_ptr_on_tag(uint8_t tag);

/*------- local functions ----------*/
static void simpbd_cache_get_file_id_and_sfi(uint8_t tag, uint16_t *file_id_p, uint8_t *sfi_p, uint8_t instance , uint8_t slice)
{
    simpbd_cache_pbr_t *cache_pbr_p = simpbd_cache_pbr_p;
    uint8_t count = 0;
    *sfi_p = 0xff;

    SIMPB_LOG_D("entered");

    while (NULL != cache_pbr_p) {
        if (tag == cache_pbr_p->tag) {
            if ((slice == cache_pbr_p->slice) && (instance == cache_pbr_p->instance)) {
                *file_id_p = cache_pbr_p->fid;
                *sfi_p = cache_pbr_p->sfi;
                break;
            }
        }

        cache_pbr_p = cache_pbr_p->next;
    }
}

static void simpbd_cache_set_current_record_id_on_file_id(uint8_t tag, uint8_t record_id)
{
    simpbd_cache_pbr_t *cache_pbr_p = simpbd_cache_pbr_p;

    SIMPB_LOG_D("entered");

    while (NULL != cache_pbr_p) {
        if (tag == cache_pbr_p->tag) {
            cache_pbr_p->current_sim_record_id = record_id;
        }

        cache_pbr_p = cache_pbr_p->next;
    }
}

static uint16_t simpbd_get_fixed_fid(uint8_t file_id)
{
    uint16_t result = 0;

    switch (file_id) {
    case STE_SIMPB_FILE_TYPE_PBR:
        result = 0x4F30;
        break;

    case STE_SIMPB_FILE_TYPE_ADN:
        result = 0x6F3A;
        break;

    case STE_SIMPB_FILE_TYPE_EXT1:
        result = 0x6F4A;
        break;

        /* The following is only for USIM and is in folder above DF_Phonebook */

    case STE_SIMPB_FILE_TYPE_FDN:
        result = 0x6F3B;
        break;

    case STE_SIMPB_FILE_TYPE_EXT2:
        result = 0x6F4B;
        break;

    case STE_SIMPB_FILE_TYPE_CCP2:
        result = 0x6F4F;
        break;

        /* Fall through intentional. */
    case STE_SIMPB_FILE_TYPE_CCP1:
    case STE_SIMPB_FILE_TYPE_AAS:
    case STE_SIMPB_FILE_TYPE_GAS:
    case STE_SIMPB_FILE_TYPE_IAP:
    case STE_SIMPB_FILE_TYPE_GRP:
    case STE_SIMPB_FILE_TYPE_ANR:
    case STE_SIMPB_FILE_TYPE_EMAIL:
    case STE_SIMPB_FILE_TYPE_SNE:
    case STE_SIMPB_FILE_TYPE_PBC:
    default:
        result = 0;
        break;
    }

    return result;
}

static void simpbd_cache_get_fid_and_current_record_id_from_client_file_id(uint8_t file_id,
        uint8_t instance,
        uint8_t slice,
        ste_simpb_phonebook_type_t phonebook,
        uint8_t app_type,
        uint16_t *fid_p, /* output */
        uint8_t *current_record_id_p) /* output */
{
    uint8_t tag = 0;
    uint8_t sfi = 0; /* not intended for use here but in API */

    SIMPB_LOG_D("entered");

    (void)app_type;

    if (SIMBPD_CLIENT_FID_TO_DF_PHONEBOOK_TAG_MAX > file_id) {
        *fid_p = simpbd_get_fixed_fid(file_id);
        *current_record_id_p = simpbd_current_record_id_list[file_id];

        tag = simpbd_client_fid_to_df_phonebook_tag[file_id];
        simpbd_cache_get_file_id_and_sfi(tag, fid_p, &sfi, instance, slice);
    }
}

static void simpbd_reset_instances_all(void)
{
    memset(simpbd_cache_instances_for_tags, 0, SIMBPD_CLIENT_FID_TO_DF_PHONEBOOK_TAG_MAX);
}

static uint8_t simpbd_get_instance_on_tag(uint8_t tag)
{
    uint8_t i = 0;
    uint8_t result = SIMBPD_CLIENT_FID_TO_DF_PHONEBOOK_TAG_MAX;

    for (i = 0; i < SIMBPD_CLIENT_FID_TO_DF_PHONEBOOK_TAG_MAX; i++) {
        if (tag == simpbd_client_fid_to_df_phonebook_tag[i]) {
            result = simpbd_cache_instances_for_tags[i];
            break;
        }
    }

    return result;
}

static uint8_t *simpbd_get_instance_ptr_on_tag(uint8_t tag)
{
    uint8_t i = 0;
    uint8_t *result_p = NULL;

    for (i = 0; i < SIMBPD_CLIENT_FID_TO_DF_PHONEBOOK_TAG_MAX; i++) {
        if (tag == simpbd_client_fid_to_df_phonebook_tag[i]) {
            result_p = &(simpbd_cache_instances_for_tags[i]);
            break;
        }
    }

    return result_p;
}

/*------- public functions ----------*/
void simpbd_cache_flush_pbr_data()
{
    simpbd_cache_pbr_t *pbr_cache_p = simpbd_cache_pbr_p;
    simpbd_cache_pbr_t *free_this_p = NULL;
    SIMPB_LOG_D("entered");

    while (NULL != pbr_cache_p) {
        free_this_p = pbr_cache_p;
        pbr_cache_p = pbr_cache_p->next;
        free(free_this_p);
    }

    simpbd_cache_pbr_p = NULL;
    memset(simpbd_current_record_id_list, 1, STE_SIMPB_FILE_TYPE_MAX);
}

/*
* Flush most the globals used.
*
*/
void simpbd_cache_flush_file_info()
{
    simpb_puid_value = 0xffff;
    simpb_cc_value = 0xffff;
    simpb_psc_value = 0xffffffff;
    simpb_p_cc_value = 0xffff;

    simpb_adn_file_t *adn_file_info_cache_p = simpbd_cache_adn_file_info_head;
    simpb_uid_file_t *uid_file_info_cache_p = simpbd_cache_uid_file_info_head;

    simpb_adn_file_t *free_this_adn_p = NULL;
    simpb_uid_file_t *free_this_uid_p = NULL;
    SIMPB_LOG_D("entered");

    while (NULL != adn_file_info_cache_p) {
        free_this_adn_p = adn_file_info_cache_p;
        adn_file_info_cache_p = adn_file_info_cache_p->next;
        free(free_this_adn_p);
    }

    simpbd_cache_adn_file_info_head = NULL;
    simpbd_cache_adn_file_info_tail = NULL;
    SIMPB_LOG_D("Cleared ADN info");

    while (NULL != uid_file_info_cache_p) {
        free_this_uid_p = uid_file_info_cache_p;
        uid_file_info_cache_p = uid_file_info_cache_p->next;
        free(free_this_uid_p);
    }

    simpbd_cache_uid_file_info_head = NULL;
    simpbd_cache_uid_file_info_tail = NULL;
    SIMPB_LOG_D("Cleared UID info");

    if (simpbd_cache_adn_file_ids_g != NULL) {
        free(simpbd_cache_adn_file_ids_g);
        simpbd_cache_adn_file_ids_g = NULL;
    }

    SIMPB_LOG_D("Cleared File id info");

    simpbd_cache_flush_occupied_rec_info();

}
/*
* Flush the simpbd_cache_occupied_record_g
* This is needed seperately since it will be called during uid update.
*/
void simpbd_cache_flush_occupied_rec_info()
{
    if (simpbd_cache_occupied_record_g != NULL) {
        free(simpbd_cache_occupied_record_g);
    }

    simpbd_cache_occupied_record_g = NULL;
    SIMPB_LOG_D("Cleared occupied record info");
}

/*
* Iterate thorugh PBR cache and identify ADN and UID files
*
*/
ste_simpb_result_t simpbd_cache_adn_uid_file_info()
{
    simpbd_cache_pbr_t *cache_pbr_p = simpbd_cache_pbr_p;
    simpb_adn_file_t *current_adn_file = NULL;
    simpb_uid_file_t *current_uid_file = NULL;
    uint8_t count = 0;
    uint8_t count_adn = 0;
    uint8_t count_uid = 0;
    SIMPB_LOG_D("entered");

    if (NULL == cache_pbr_p) {
        SIMPB_LOG_D("simpbd_cache_pbr_p is NULL");
        goto error;
    }

    while (NULL != cache_pbr_p) {

        if (ADN_FILE_TAG == cache_pbr_p->tag) { /*ADN file tag*/
            current_adn_file = (simpb_adn_file_t *)calloc(1, sizeof(simpb_adn_file_t));

            if (current_adn_file == NULL) {
                SIMPB_LOG_E("calloc failed");
                goto error;
            }

            /*To send to AT*/
            if (simpbd_cache_adn_file_ids_g == NULL) {
                simpbd_cache_adn_file_ids_g = (simpb_file_ids_t *)calloc(1, sizeof(simpb_file_ids_t));

                if (simpbd_cache_adn_file_ids_g == NULL) {
                    SIMPB_LOG_E("calloc failed");
                    if ( current_adn_file ) {
                        free(current_adn_file);
                    }
                    goto error;
                }
            }

            current_adn_file->slice = cache_pbr_p->slice;
            current_adn_file->instance = cache_pbr_p->instance;
            current_adn_file->fid = cache_pbr_p->fid;
            current_adn_file->sfi = cache_pbr_p->sfi;
            current_adn_file->next = NULL;

            if (simpbd_cache_adn_file_info_head == NULL) {
                simpbd_cache_adn_file_info_head = current_adn_file;
            } else {
                simpbd_cache_adn_file_info_tail->next = current_adn_file;
            }

            memset(current_adn_file->adn_rec_occupied, FALSE, SIMPB_MAX_EF_RECS);
            simpbd_cache_adn_file_info_tail = current_adn_file;

            /*Note down ADN file ids to send to AT*/
            simpbd_cache_adn_file_ids_g->pb_file_ids[count_adn] = cache_pbr_p->fid;
            simpbd_cache_adn_file_ids_g->no_of_pb_files = count_adn + 1;
            count_adn++;
        }

        if (UID_FILE_TAG == cache_pbr_p->tag) { /*UID File tag*/
            current_uid_file = (simpb_uid_file_t *)calloc(1, sizeof(simpb_uid_file_t));

            if (current_uid_file == NULL) {
                SIMPB_LOG_E("calloc failed");
                goto error;
            }

            current_uid_file->slice = cache_pbr_p->slice;
            current_uid_file->instance = cache_pbr_p->instance;
            current_uid_file->fid = cache_pbr_p->fid;
            current_uid_file->sfi = cache_pbr_p->sfi;
            current_uid_file->next = NULL;

            if (simpbd_cache_uid_file_info_head == NULL) {
                simpbd_cache_uid_file_info_head = current_uid_file;
            } else {
                simpbd_cache_uid_file_info_tail->next = current_uid_file;
            }

            simpbd_cache_uid_file_info_tail = current_uid_file;
            count_uid++;
        }

        count++;
        cache_pbr_p = cache_pbr_p->next;
    }

    if (count_adn && count_uid && (count_adn == count_uid)) {
        SIMPB_LOG_D(" No of ADNs(%d) and UIDs(%d)", count_adn, count_uid);
        return STE_SIMPB_SUCCESS;
    }

error:
    SIMPB_LOG_E("No of ADNs(%d) and UIDs(%d) dont match", count_adn, count_uid);
    return STE_SIMPB_FAILURE;

}

/*
* Find and return the UID file info, based on the slice and instance
*
*/
simpb_uid_file_t *simpb_get_uid_file_info(uint8_t slice_no, uint8_t instance)
{
    simpb_uid_file_t *current_file_info = simpbd_cache_uid_file_info_head;

    while (NULL != current_file_info) {
        if (current_file_info->slice == slice_no && current_file_info->instance == instance) {
            return (current_file_info);
        }

        current_file_info = current_file_info->next;
    }

    SIMPB_LOG_E("Corresponding UID file not found");
    current_file_info = NULL;
    return current_file_info;
}

/*
* Find and Update a specific record in ADN cache
*
*/
ste_simpb_result_t simpbd_update_cached_adn_record(uint16_t fileid, uint8_t index, uint8_t value)
{
    simpb_adn_file_t *current_file_info = simpbd_get_adn_file_info(fileid);

    if (NULL != current_file_info) {
        current_file_info->adn_rec_occupied[index] = value;
        return STE_SIMPB_SUCCESS;
    }

    return STE_SIMPB_FAILURE;
}

/*
* return ADN file info based on file id provided
*
*/
simpb_adn_file_t *simpbd_get_adn_file_info(uint16_t fid)
{
    simpb_adn_file_t *current_file_info = simpbd_cache_adn_file_info_head;
    SIMPB_LOG_D("ADN File id to match: %d", fid);

    while (NULL != current_file_info) {
        SIMPB_LOG_D("ADN File id in cache: %d", current_file_info->fid);

        if (current_file_info->fid == fid) {
            return current_file_info;
        }

        current_file_info = current_file_info->next;
    }

    return NULL;
}

/*
* Find and return UID file info corresponding to a ADN File id provided
*
*/
simpb_uid_file_t *simpbd_get_uid_file_info_for_adn_fid(uint16_t fid)
{
    simpb_adn_file_t *current_file_info = simpbd_cache_adn_file_info_head;

    while (NULL != current_file_info) {
        if (current_file_info->fid == fid) {
            return ((simpb_uid_file_t *)simpb_get_uid_file_info(current_file_info->slice,
                    current_file_info->instance));

        }

        current_file_info = current_file_info->next;
    }

    SIMPB_LOG_E("Corresponding UID file not found in Cache");
    return NULL;
}

/*
* Find and update the global structure to contain the next occupied record.
*
*/
ste_simpb_result_t simpdb_get_cached_occupied_adn_record()
{
    /*First call*/
    if (simpbd_cache_occupied_record_g == NULL) {
        simpbd_cache_occupied_record_g = (ste_simpb_occupied_record *)calloc(1, sizeof(ste_simpb_occupied_record));
        simpbd_cache_occupied_record_g->adn_file = simpbd_cache_adn_file_info_head;
        simpbd_cache_occupied_record_g->uid_file = (simpb_uid_file_t *)simpb_get_uid_file_info(simpbd_cache_occupied_record_g->adn_file->slice,
                simpbd_cache_occupied_record_g->adn_file->instance);
        simpbd_cache_occupied_record_g->rec_num = 0;
    }

    uint8_t i = 0;
    simpbd_cache_occupied_record_g->rec_num = simpbd_cache_occupied_record_g->rec_num + 1;

    while (1) {
        simpb_adn_file_t *current_file = simpbd_cache_occupied_record_g->adn_file;

        for (i = simpbd_cache_occupied_record_g->rec_num;  i <= current_file->num_records; i++) {
            if (current_file->adn_rec_occupied[i - 1] == TRUE) {
                SIMPB_LOG_D("Cached ADN file %d, record (%d) state:%d", current_file->fid, i, current_file->adn_rec_occupied[i - 1]);
                simpbd_cache_occupied_record_g->rec_num = i;
                return (TRUE);
            }
        }

        /*end of file records, no empty records found, go to next file*/
        if (current_file->next != NULL) {
            simpbd_cache_occupied_record_g->adn_file = current_file->next;
            simpbd_cache_occupied_record_g->uid_file = simpb_get_uid_file_info(simpbd_cache_occupied_record_g->adn_file->slice,
                    simpbd_cache_occupied_record_g->adn_file->instance);
            simpbd_cache_occupied_record_g->rec_num = 1;
        } else {
            return(FALSE);
        }
    }
}

/*
* Below functions will return the pointers to global variables used.
*
*/
simpb_adn_file_t *simpbd_get_adn_file_info_head()
{
    return simpbd_cache_adn_file_info_head;
}

simpb_uid_file_t *simpbd_get_uid_file_info_head()
{
    return simpbd_cache_uid_file_info_head;
}

ste_simpb_occupied_record *simpbd_get_occ_rec_holder()
{
    return simpbd_cache_occupied_record_g;
}

simpb_file_ids_t *simpbd_get_adn_file_ids()
{
    return simpbd_cache_adn_file_ids_g;
}


ste_simpb_result_t simpbd_create_and_store_pbr_cache(ste_uicc_sim_file_read_record_response_t *data_p, uint8_t slice)
{
    uint8_t *pbr_data_p = NULL;
    uint8_t pbr_length = 0;
    uint8_t index = 0;
    uint8_t link_type = 0xff;

    simpbd_cache_pbr_t *pbr_cache_p;

    SIMPB_LOG_E("entered");

    if (NULL == data_p) {
        SIMPB_LOG_E("data_p is NULL!");
        goto error;
    }

    SIMPB_LOG_D(" STATUS BYTES sw1=0x%02X    sw2=0x%02X ", data_p->status_word.sw1, data_p->status_word.sw2);
    SIMPB_LOG_D(" DATA         length=0x%02X data=0x%08X ", data_p->length, data_p->data);
    SIMPB_LOG_D(" RESULT CODES status=%d     iso=%d ", data_p->uicc_status_code, data_p->uicc_status_code_fail_details);

    if (NULL == data_p->data) {
        SIMPB_LOG_E("data_p->data is NULL!");
        goto error;
    }

    if (0 == data_p->length) {
        SIMPB_LOG_E("data_p->length is 0!");
        goto error;
    }

    pbr_data_p = data_p->data;
    pbr_length = (uint8_t)(data_p->length & 0x00ff);

    memset(simpbd_current_record_id_list, 1, STE_SIMPB_FILE_TYPE_MAX);

    while (index < pbr_length) {
        uint8_t length = 0;
        uint8_t k = 0;

        simpbd_reset_instances_all();

        link_type = pbr_data_p[index++];

        if (0xa8 != link_type && 0xa9 != link_type &&  0xaa != link_type) {
            if (0xff == link_type) {
                return STE_SIMPB_SUCCESS;  /* 0xff means that there are no new entries. */
            } else {
                int in = 0;
                char str[64];
                char sd[4];
                str[0] = '\0';

                SIMPB_LOG_E("LinkType = %d not a valid EF_PBR entry. Bad data in EF_PBR?", link_type);

                for (in = 0; in < pbr_length; in++) {
                    sprintf(sd, "%02X ", (uint8_t)pbr_data_p[in]);
                    strcat(str, sd);

                    if ((in + 1) % 16 == 0) {
                        SIMPB_LOG_E("RECORD HEXDUMP: %02d->%02d   %s", in - 15, in, str);
                        str[0] = '\0';
                    }
                }

                if ('\0' != str[0]) {
                    SIMPB_LOG_E("RECORD HEXDUMP: %02d->%02d   %s", in - 15, in, str);
                }

                goto error;
            }
        }

        SIMPB_LOG_D(" iterating through link types link_type=0x%02X pbr_length=%d index=%d ", link_type, pbr_length, index);

        length = pbr_data_p[index++];

        while (k < length) {
            uint8_t tag = 0xff;
            uint8_t prim_length = 0;
            uint16_t fid = 0xffff;
            uint8_t sfi = 0xff;
            uint8_t *instance_p = NULL;

            tag = pbr_data_p[index++];
            k++;
            prim_length = pbr_data_p[index++];
            k++;
            fid = ((uint16_t)pbr_data_p[index++]) << 8;
            fid += (uint16_t)(pbr_data_p[index++] & 0x00ff);
            k += 2;

            if (3 == prim_length) {
                sfi = pbr_data_p[index++];
                k++;
            }

            pbr_cache_p = simpbd_cache_pbr_p;

            if (NULL == pbr_cache_p) {
                simpbd_cache_pbr_p = (simpbd_cache_pbr_t *)calloc(1, sizeof(simpbd_cache_pbr_t));
                pbr_cache_p = simpbd_cache_pbr_p;
            } else {
                while (NULL != pbr_cache_p->next) {
                    pbr_cache_p = pbr_cache_p->next;
                }

                pbr_cache_p->next = (simpbd_cache_pbr_t *)calloc(1, sizeof(simpbd_cache_pbr_t));
                pbr_cache_p = pbr_cache_p->next;
                /* no NULL setting on next needed as we use calloc */
            }

            pbr_cache_p->link_type = link_type;
            pbr_cache_p->tag = tag;
            pbr_cache_p->length = prim_length;
            pbr_cache_p->fid = fid;
            pbr_cache_p->sfi = sfi;
            pbr_cache_p->slice = slice;

            instance_p = simpbd_get_instance_ptr_on_tag(tag);

            if (NULL != instance_p) {
                pbr_cache_p->instance = *instance_p;
                (*instance_p)++;
            } else {
                SIMPB_LOG_E("Instance undefined for tag=0x%02X. EF file not supported or bad PBR data.", tag);
            }

            SIMPB_LOG_D("pbr_cache_p=0x%08X index=0x%04X link_type=0x%02X tag=0x%02X prim_length=0x%02X fid=0x%04X sfi=0x%02X instance=%d slice=%d",
                        pbr_cache_p, index, link_type, tag, prim_length, fid, sfi, pbr_cache_p->instance, slice);
        }
    }

    SIMPB_LOG_E("Exit");

    return STE_SIMPB_SUCCESS;
error:
    return STE_SIMPB_FAILURE;
}

/*------- public utility functions -------*/

uint16_t simpbd_cache_get_fid_from_client_file_id(uint8_t file_id, uint8_t instance, uint8_t slice, ste_simpb_phonebook_type_t phonebook, uint8_t app_type)
{
    uint16_t fid = simpbd_get_fixed_fid(file_id);
    uint8_t tag = 0;
    uint8_t sfi = 0; /* not intended for use here but in API */

    if (SIMBPD_CLIENT_FID_TO_DF_PHONEBOOK_TAG_MAX > file_id) {
        tag = simpbd_client_fid_to_df_phonebook_tag[file_id];

        simpbd_cache_get_file_id_and_sfi(tag, &fid, &sfi, instance, slice);
        SIMPB_LOG_D("simpbd_cache_get_fid_from_client_file_id: tag=0x%02X fid=0x%04X sfi=0x%02X instance=%d slice=%d",
                    tag, fid, sfi, instance, slice);
    }

    return fid;
}

char *simpbd_cache_get_path_on_selected_phonebook(ste_simpb_phonebook_type_t phonebook, uint8_t app_type, uint8_t card_type, ste_simpb_file_type_t file_id)
{
    char *result = NULL;

    if (SIM_CARD_TYPE_ICC == card_type || SIM_APP_GSM == app_type) {
        result = (char *)(simpbd_uicc_file_paths_global.path_gsm);
    } else {
        uint8_t use_parent = STE_SIMPB_FILE_TYPE_FDN == file_id ||
                             STE_SIMPB_FILE_TYPE_EXT2 == file_id ||
                             STE_SIMPB_FILE_TYPE_CCP2 == file_id;

        if (STE_SIMPB_PHONEBOOK_TYPE_LOCAL == phonebook) {
            if (use_parent) {
                result = (char *)(simpbd_uicc_file_paths_local.path_usim_parent);
            } else {
                result = (char *)(simpbd_uicc_file_paths_local.path_usim);
            }
        } else {
            if (use_parent) {
                result = (char *)(simpbd_uicc_file_paths_global.path_usim_parent);
            } else {
                result = (char *)(simpbd_uicc_file_paths_global.path_usim);
            }
        }
    }

    return result;
}

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
                                 char **path_pp)
{
    uint8_t current_record_id = 0;

    *path_pp = simpbd_cache_get_path_on_selected_phonebook(selected_phonebook, app_type, card_type, file_id);
    simpbd_cache_get_fid_and_current_record_id_from_client_file_id(file_id, instance, slice, selected_phonebook, app_type, fid_p, &current_record_id);
    SIMPB_LOG_D("           ** file_id=0x%04X pb=%d app=%d p2=0x%02X  path_p=%s current_record_id=%d",
                *fid_p, selected_phonebook, app_type, p2, *path_pp, current_record_id);

    if (0 == p2) {
        *record_id_p = 0;
    } else if (4 == (p2 & 0x04)) {
        /* we use absolute address on record_id. If > 0 use record_id. If == 0 use current record id */
        if (0 == record_id) {
            *record_id_p = current_record_id;
        } else {
            *record_id_p = record_id;
        }
    } else if (2 == (p2 & 0x03)) { /* use next record */
        if (255 == current_record_id) {
            *record_id_p = 0;
        } else {
            *record_id_p = current_record_id + 1;
        }
    } else if (3 == (p2 & 0x03)) { /* use previous record */
        if (1 == record_id) {
            *record_id_p = 0;
        } else {
            *record_id_p = current_record_id - 1;
        }
    }

    SIMPB_LOG_D("           **  *record_id_p = %d", *record_id_p);
}

void simbpd_cache_set_current_record_id(uint8_t file_id, uint8_t record_id)
{
    simpbd_current_record_id_list[file_id] = record_id;
}
