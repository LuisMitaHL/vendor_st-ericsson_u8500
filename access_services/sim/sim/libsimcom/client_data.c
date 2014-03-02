/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : client_data.c
 * Description     : Client data implementation file
 *
 * Author          : Steve Critchlow <steve.critchlow@stericsson.com>
 *
 */


#include "client_data.h"

#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <string.h>


/**
 * @brief The maximum number of clients that can connect to either uiccd or catd.
 */
#define MIN_ARRAY_INC     (10)


// This is the dynamic array of the clients.
typedef struct {
    ste_sim_client_data_t  *clients;            /* Points to the array */
    size_t                  numClients;         /* No of used entires */
    size_t                  sizeofClients;      /* No of allocated */
} ste_sim_client_data_array_t;

static ste_sim_client_data_array_t* client_data_array = 0;
static pthread_mutex_t         client_data_array_mtx; /* Lock for the array */



static ste_sim_client_data_array_t* ste_sim_client_data_array_new(size_t n)
{
    ste_sim_client_data_array_t* p;
    p = malloc( sizeof(*p) );
    if ( p ) {
        size_t size_of_array;
        memset( p, 0, sizeof(*p) );
        if ( n < MIN_ARRAY_INC )
            n = MIN_ARRAY_INC;
        size_of_array = n * sizeof(ste_sim_client_data_t);
        p->numClients = 0;
        p->sizeofClients = n;
        p->clients = malloc( size_of_array );
        if ( !p->clients ) {
            free(p);
            p = 0;
        }
        else {
            memset(p->clients,0, size_of_array );
        }
    }
    return p;
}


static void ste_sim_client_data_array_delete( ste_sim_client_data_array_t* cda)
{
    if ( cda ) {
        if ( cda->clients )
            free( cda->clients );
        free( cda );
    }
}



static int ste_sim_client_data_array_register(ste_sim_client_data_array_t* cda,
                                              const ste_sim_client_data_t *cd )

{
    int result = -1;
    if ( cda ) {
        if ( !(cda->numClients < cda->sizeofClients) ) {
            ste_sim_client_data_t* new_array;
            size_t size_of_array;
            size_of_array = ( (cda->sizeofClients + MIN_ARRAY_INC) *
                              sizeof(ste_sim_client_data_t) );
            new_array = malloc( size_of_array );
            if ( new_array ) {
                size_t i;
                for ( i = 0; i < cda->numClients; ++i ) {
                    new_array[i] = cda->clients[i];
                }
                free( cda->clients );
                cda->clients = new_array;
                cda->sizeofClients += MIN_ARRAY_INC;
            }
            else {
                return -1;
            }
        }

        cda->clients[ cda->numClients ] = *cd;
        ++(cda->numClients);

        result = 0;
    }
    return result;
}




static int ste_sim_client_data_array_deregister(ste_sim_client_data_array_t*cda,
                                                int fd )
{
    int result = -1;
    if ( cda ) {
        size_t i;
        for ( i = 0; i < cda->numClients ; ++i ) {
            if ( cda->clients[i].fd == fd ) {
                // copy the last entry
                cda->clients[i] = cda->clients[cda->numClients-1];
                cda->clients[cda->numClients-1].fd = -1; // mark as free
                --(cda->numClients);                // one less client
                result = 0;
            }
        }
    }
    return result;
}



static void ste_sim_client_data_array_action(ste_sim_client_data_array_t*cda,
                                             ste_sim_client_data_act_fn func,
                                              void *ud )
{
    if ( cda ) {
        size_t i;
        for ( i = 0 ; i < cda->numClients ; i++ ) {
            func( &(cda->clients[i]), ud );
        }
    }
}




int ste_sim_client_data_init(void)
{
    int result = 0;

    pthread_mutex_init(&client_data_array_mtx, 0);       /* 0=default args */

    assert( client_data_array == 0 );

    client_data_array = ste_sim_client_data_array_new(MIN_ARRAY_INC);
    if ( !client_data_array )
        result = -1;

    return result;
}


void ste_sim_client_data_fini(void)
{
    pthread_mutex_lock( &client_data_array_mtx );
    do {
        ste_sim_client_data_array_delete(client_data_array);
        client_data_array = 0;
    }while(0);
    pthread_mutex_unlock( &client_data_array_mtx );
    pthread_mutex_destroy( &client_data_array_mtx );
}



int ste_sim_client_data_register( const ste_sim_client_data_t *cd )
{
    int rv;
    pthread_mutex_lock( &client_data_array_mtx );
    do {
        rv = ste_sim_client_data_array_register(client_data_array,cd);
    }while(0);
    pthread_mutex_unlock( &client_data_array_mtx );
    return rv;
}



int ste_sim_client_data_deregister( int fd )
{
    int rv;
    pthread_mutex_lock( &client_data_array_mtx );
    do {
        rv = ste_sim_client_data_array_deregister(client_data_array,fd);
    }while(0);
    pthread_mutex_unlock( &client_data_array_mtx );
    return rv;
}



void ste_sim_client_data_action( ste_sim_client_data_act_fn func, void *ud )
{
    pthread_mutex_lock( &client_data_array_mtx );
    do {
        ste_sim_client_data_array_action(client_data_array,func,ud);
    }while(0);
    pthread_mutex_unlock( &client_data_array_mtx );
}
