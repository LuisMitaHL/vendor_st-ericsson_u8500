/*                               -*- Mode: C -*-
 * Copyright (C) ST-Ericsson SA 2010
 * $Id$
 *
 * File name       : sync.c
 * Description     : Handling of synchronous calls.
 *
 * Author          : Mats Bergstrom <mats.m.bergstrom@stericsson.com>
 *
 */



#include "sim.h"
#include "sim_unused.h"
#include "sim_internal.h"
#include "uicc_internal.h"

#include <stdlib.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/un.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

static int
wait_for_select( ste_sim_t* sim )
{
    fd_set                  rset;
    int fd;
    int i;

    fd = ste_sim_fd(sim);

    for(;;) {

        FD_ZERO(&rset);
        FD_SET( fd, &rset);

        i = select( fd+1, &rset, 0 , 0 , 0 );
        if ( i == -1 ) // Error
            break;
        if ( i > 0 ) { // OK, there are bytes to read.
            i = 0;
            break;
        }

        // If i == 0, we try again.
    }

    return i;
}



static
int ste_sim_sync_handling( ste_sim_t* sim,
                           uintptr_t  client_tag,
                           uint16_t   expected_response_id,
                           void*      data )
{
    int rv = -1;


    // Set up for the CB function to know what to do
    sim->sync_data.expected_response_id = expected_response_id;
    sim->sync_data.client_tag = client_tag;
    sim->sync_data.vdata = data;
    sim->sync_data.is_sync = 1;
    sim->sync_data.done = 0;
    sim->sync_data.rv   = -1;

    do {
        ssize_t n;
        ste_es_t* es;

        es = sim->es;

        for(;;) {

            // Simple select on the FD
            rv = wait_for_select(sim);
            if ( rv ) {
                // Something failed
                rv = -1;
                break;
            }


            n = ste_es_read(es);
            if ( n > 0 ) {
                n = ste_es_parse(es);
            }

            if ( ste_es_state(es) != STE_ES_STATE_OPEN ) {
                // We fail
                rv = -1;
                break;
            }

            if ( sim->sync_data.done ) {
                rv = sim->sync_data.rv;
                break;
            }


        }

    } while(0);

    sim->sync_data.expected_response_id = 0;
    sim->sync_data.vdata = 0;
    sim->sync_data.is_sync = 0;
    sim->sync_data.client_tag = 0;

    return rv;
}

int  ste_uicc_sim_get_state_sync( ste_sim_t* sim,
                                  uintptr_t  client_tag,
                                  ste_uicc_get_sim_state_response_t * sim_state)
{
    int rv;
    const uint16_t expected_response_id = STE_UICC_RSP_GET_SIM_STATE;

    // MUST run in st mode!
    if ( !sim || !sim->is_st )
        return -1;

    // First send the message to simd
    rv = ste_uicc_sim_get_state( sim, client_tag );
    if ( rv ) {
    // We failed.
        return rv;
    }

    rv = ste_sim_sync_handling( sim,
                                client_tag,
                                expected_response_id,
                                sim_state );
    return rv;
}


int ste_sim_ping_sync( ste_sim_t* sim,
                       uintptr_t  client_tag )
{
    int rv;
    const uint16_t expected_response_id = STE_SIM_RSP_PING;

    // MUST run in st mode!
    if ( !sim || !sim->is_st )
        return -1;

    // First send the message to simd
    rv = ste_sim_ping( sim, client_tag );
    if ( rv ) {
        // We failed.
        return rv;
    }


    rv = ste_sim_sync_handling( sim, client_tag,
                                expected_response_id,
                                0 );            /* No output data */

    return rv;
}
int ste_uicc_sim_file_read_record_sync( ste_sim_t * sim,
                                        uintptr_t client_tag,
                                        int file_id,
                                        int record_id,
                                        int length,
                                        const char * file_path,
                                        ste_uicc_sim_file_read_record_response_t * read_result )
{
    int rv;
    const uint16_t expected_response_id = STE_UICC_RSP_READ_SIM_FILE_RECORD;

    // MUST run in st mode!
    if ( !sim || !sim->is_st )
        return -1;

    // First send the message to simd
    rv = ste_uicc_sim_file_read_record( sim, client_tag, file_id, record_id, length, file_path );
    if ( rv ) {
        // We failed.
        return rv;
    }

    rv = ste_sim_sync_handling( sim, client_tag,
                                expected_response_id, read_result );
    return rv;
}

int ste_uicc_sim_file_read_binary_sync( ste_sim_t * sim,
                                        uintptr_t client_tag,
                                        int file_id,
                                        int offset,
                                        int length,
                                        const char * file_path,
                                        ste_uicc_sim_file_read_binary_response_t * read_result )
{
    int rv;
    const uint16_t expected_response_id = STE_UICC_RSP_READ_SIM_FILE_BINARY;

    // MUST run in st mode!
    if ( !sim || !sim->is_st )
        return -1;

    // First send the message to simd
    rv = ste_uicc_sim_file_read_binary( sim, client_tag, file_id, offset, length, file_path );
    if ( rv ) {
        // We failed.
        return rv;
    }

    rv = ste_sim_sync_handling( sim, client_tag,
                                expected_response_id, read_result );
    return rv;
}
int ste_uicc_sim_file_update_record_sync(ste_sim_t * sim,
                                         uintptr_t client_tag,
                                         int file_id,
                                         int record_id,
                                         int length,
                                         const char * file_path,
                                         const uint8_t * data,
                                         ste_uicc_update_sim_file_record_response_t * update_result )
{
    int rv;
    const uint16_t expected_response_id = STE_UICC_RSP_UPDATE_SIM_FILE_RECORD;

    // MUST run in st mode!
    if ( !sim || !sim->is_st )
        return -1;

    // First send the message to simd
    rv = ste_uicc_sim_file_update_record( sim, client_tag, file_id, record_id, length, file_path, data );
    if ( rv ) {
        // We failed.
        return rv;
    }

    rv = ste_sim_sync_handling( sim, client_tag,
                                expected_response_id, update_result );
    return rv;
}

int ste_uicc_sim_file_update_binary_sync(ste_sim_t * sim,
                                         uintptr_t client_tag,
                                         int file_id,
                                         int offset,
                                         int length,
                                         const char * file_path,
                                         const uint8_t * data,
                                         ste_uicc_update_sim_file_binary_response_t * update_result )
{
    int rv;
    const uint16_t expected_response_id = STE_UICC_RSP_UPDATE_SIM_FILE_BINARY;

    // MUST run in st mode!
    if ( !sim || !sim->is_st )
        return -1;

    // First send the message to simd
    rv = ste_uicc_sim_file_update_binary( sim, client_tag, file_id, offset, length, file_path, data );
    if ( rv ) {
        // We failed.
        return rv;
    }

    rv = ste_sim_sync_handling( sim, client_tag,
                                expected_response_id, update_result );
    return rv;
}

int ste_uicc_sim_file_get_format_sync(ste_sim_t * sim,
                                      uintptr_t client_tag,
                                      int file_id,
                                      const char *file_path,
                                      ste_uicc_sim_file_get_format_response_t * file_format )
{
    int rv;
    const uint16_t expected_response_id = STE_UICC_RSP_SIM_FILE_GET_FORMAT;

    // MUST run in st mode!
    if ( !sim || !sim->is_st )
        return -1;

    // First send the message to simd
    rv = ste_uicc_sim_file_get_format( sim, client_tag, file_id, file_path );
    if ( rv ) {
        // We failed.
        return rv;
    }

    rv = ste_sim_sync_handling( sim, client_tag,
                                expected_response_id, file_format );
    return rv;
}

int ste_uicc_sim_get_file_information_sync(ste_sim_t * sim,
                                           uintptr_t client_tag,
                                           int file_id,
                                           const char *path,
                                           ste_uicc_sim_get_file_info_type_t type,
                                           ste_uicc_get_file_information_response_t * file_info )

{
    int rv;
    const uint16_t expected_response_id = STE_UICC_RSP_GET_FILE_INFORMATION;

    // MUST run in st mode!
    if ( !sim || !sim->is_st )
        return -1;

    // First send the message to simd
    rv = ste_uicc_sim_get_file_information( sim, client_tag, file_id, path, type );
    if ( rv ) {
        // We failed.
        return rv;
    }

    rv = ste_sim_sync_handling( sim, client_tag,
                                expected_response_id, file_info );
    return rv;
}




int ste_uicc_sim_file_read_specific_imsi_sync ( ste_sim_t * sim,
                                                uintptr_t client_tag,
                                                ste_uicc_sim_file_read_imsi_response_t * read_result )
{
    const struct {
        const char *usim_file_path;
        const char *gsm_file_path;
    } paths = { "3F007FFF", "3F007F20" };
    int                                         file_id = SIM_EF_IMSI;
    int                                         offset = 0;
    int                                         length = SIM_EF_IMSI_LEN;
    const char *                                file_path = paths.usim_file_path;
    ste_uicc_sim_file_read_binary_response_t    retData;
    int                                         rv;

    rv = ste_uicc_sim_file_read_binary_sync( sim, client_tag, file_id, offset, length, file_path, &retData );
    if ( rv == 0 && retData.length == length && retData.data != NULL ) {
        sim_create_imsi_response(read_result, retData.data, retData.length);
        read_result->uicc_status_code = retData.uicc_status_code;
        read_result->uicc_status_code_fail_details = retData.uicc_status_code_fail_details;
        read_result->status_word.sw1 = retData.status_word.sw1;
        read_result->status_word.sw2 = retData.status_word.sw2;
        free( retData.data );
    } else {
        read_result->uicc_status_code = STE_UICC_STATUS_CODE_FAIL;
        read_result->uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR;
    }

    return rv;
}


int ste_uicc_sim_smsc_get_active_sync ( ste_sim_t * sim,
                                        uintptr_t client_tag,
                                        ste_uicc_sim_smsc_get_active_response_t * get_result )
{
    int rv;
    const uint16_t expected_response_id = STE_UICC_RSP_READ_SMSC;

    // MUST run in st mode!
    if ( !sim || !sim->is_st )
        return -1;

    // First send the message to simd
    rv = ste_uicc_sim_smsc_get_active(sim, client_tag);
    if ( rv ) {
        // We failed.
        return rv;
    }

    rv = ste_sim_sync_handling( sim, client_tag,
                                expected_response_id, get_result );
    return rv;
}

int ste_uicc_sim_smsc_set_active_sync ( ste_sim_t * sim,
                                        uintptr_t client_tag,
                                        ste_sim_call_number_t * smsc,
                                        ste_uicc_sim_smsc_set_active_response_t * set_result )
{
    int rv;
    const uint16_t expected_response_id = STE_UICC_RSP_UPDATE_SMSC;

    // MUST run in st mode!
    if ( !sim || !sim->is_st )
        return -1;

    // First send the message to simd
    rv = ste_uicc_sim_smsc_set_active(sim, client_tag, smsc);
    if ( rv ) {
        // We failed.
        return rv;
    }

    rv = ste_sim_sync_handling( sim, client_tag,
                                expected_response_id, set_result );
    return rv;
}


int ste_uicc_get_service_table_sync ( ste_sim_t * sim,
                                      uintptr_t client_tag,
                                      sim_service_type_t service_type,
                                      ste_uicc_get_service_table_response_t * read_response)
{
    int rv;
    const uint16_t expected_response_id = STE_UICC_RSP_GET_SERVICE_TABLE;

    // MUST run in st mode!
    if ( !sim || !sim->is_st )
        return -1;

    // First send the message to simd
    rv = ste_uicc_get_service_table( sim, client_tag, service_type );
    if ( rv ) {
        // We failed.
        return rv;
    }

    rv = ste_sim_sync_handling( sim, client_tag,
                                expected_response_id, read_response );
    return rv;
}

int ste_uicc_get_service_availability_sync ( ste_sim_t * sim,
                                      uintptr_t client_tag,
                                      sim_service_type_t service_type,
                                      ste_uicc_get_service_availability_response_t * read_response)
{
    int rv;
    const uint16_t expected_response_id = STE_UICC_RSP_GET_SERVICE_AVAILABILITY;

    // MUST run in st mode!
    if ( !sim || !sim->is_st )
        return -1;

    // First send the message to simd
    rv = ste_uicc_get_service_availability( sim, client_tag, service_type );
    if ( rv ) {
        // We failed.
        return rv;
    }

    rv = ste_sim_sync_handling( sim, client_tag,
                                expected_response_id, read_response );
    return rv;
}

int ste_uicc_sim_file_read_fdn_sync(ste_sim_t *uicc,
                                    uintptr_t client_tag,
                                    ste_uicc_sim_fdn_response_t *result)
{
  int rv;

  if (!uicc || !uicc->is_st) return -1;
  rv = ste_uicc_sim_file_read_fdn(uicc, client_tag);
  if (rv) return rv;
  rv = ste_sim_sync_handling(uicc, client_tag,
                             (uint16_t)STE_UICC_RSP_READ_FDN, result);
  return rv;
}

int ste_uicc_sim_file_read_ecc_sync(ste_sim_t *uicc,
                                    uintptr_t client_tag,
                                    ste_uicc_sim_ecc_response_t *result)
{
  int rv;

  if (!uicc || !uicc->is_st) return -1;
  rv = ste_uicc_sim_file_read_ecc(uicc, client_tag);
  if (rv) return rv;
  rv = ste_sim_sync_handling(uicc, client_tag,
                             (uint16_t)STE_UICC_RSP_READ_ECC, result);
  return rv;
}
