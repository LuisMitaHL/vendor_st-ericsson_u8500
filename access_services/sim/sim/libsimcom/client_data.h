/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : client_data.h
 * Description     : Client data definitions
 *
 * Author          : Steve Critchlow <steve.critchlow@stericsson.com>
 *
 */


#ifndef __client_data_h__
#define __client_data_h__ (1)


/**
 * @brief This structure contains data about each connected client
 *
 * As a minimum, this structure should contain the file descriptor of
 * the client, as this will be used as the key value for certain operations.
 */
typedef struct ste_sim_client_data_tag {
    int                     fd;         /**< The file descriptor for sending to a client */
} ste_sim_client_data_t;

/**
 * @brief A function pointer for performing actions on registered clients
 * @param p     A pointer to a client data instance
 * @param ud    A pointer to additional user data
 */
typedef void (*ste_sim_client_data_act_fn)( const ste_sim_client_data_t *p, void *ud );

/**
 * @brief This function initializes an instance of the client data manager.
 * @returns     0 if successful, -1 for error.
 *
 * Must only be called by the main thread before all other threads has started!
 */
int ste_sim_client_data_init(void);

/**
 * @brief This function finalizes an instance of the client data manager.
 *
 * Must only be called by the main thread after all other threads has ended!
 */
void ste_sim_client_data_fini(void);

/**
 * @brief This function registers a new client.
 *
 * @param cd    A pointer to the client data (this will be copied)
 * @returns     0 if successful
 */
int ste_sim_client_data_register( const ste_sim_client_data_t *cd );

/**
 * @brief This function removes a previously registered client.
 *
 * @param fd    The file descriptor of the client to be removed
 * @returns     0 if successful
 */
int ste_sim_client_data_deregister( int fd );

/**
 * @brief This function removes a previously registered client.
 *
 * This function iterates through all the registered clients.  For each one,
 * the func is called with a pointer to the client data, and the user data
 * pointer also passed to this function.
 *
 * @param func  A pointer to a function
 * @param ud    An additional user data pointer.
 *
 * Warning!  This is dangerous!  The func function must not call any function
 * that in turn causes changes in the client data as that will cause a
 * DEAD LOCK!
 */
void ste_sim_client_data_action( ste_sim_client_data_act_fn func, void *ud );

#endif
