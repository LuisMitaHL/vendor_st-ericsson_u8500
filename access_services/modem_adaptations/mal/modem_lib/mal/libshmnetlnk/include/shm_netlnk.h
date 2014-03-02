/**
 * \file shm_netlnk.h
 * \brief Includes interface headers for SHM netlink API.
 *
 * Copyright (C) ST-Ericsson 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \n This contains the description of the Netlink Library's Interface header file information
 * \n Author: Jayarami Reddy <jayarami.reddy@stericsson.com>
 * \n
 * \n Revision History:
 * \n
 * \n v1.0 Initial Version
 */

#ifndef _SHM_NETLNK_H
#define _SHM_NETLNK_H

#include <stdint.h>

/**
 * \enum shm_nl_mesg
 *  \brief Communication message enum
 *
 */
typedef enum shm_nl {
    MODEM_RESET_IND = 1,        /** SHM to clients */
    MODEM_QUERY_STATUS_REQ,         /** MCE MAL to SHM */
    MODEM_RESET_REQ,        /** ATCore (or other clients) to SHM */
    MODEM_STATUS_ONLINE,        /** SHM to MCE MAL */
    MODEM_STATUS_OFFLINE,       /** SHM to MCE MAL */
} shm_nl_mesg;

/**
 * \enum shm_netlnk_status
 *  \brief Netlink status enum
 *
 */
typedef enum netlnk_status {
    SHM_NETLNK_ERROR = -1,
    SHM_NETLNK_SUCCESS = 0,
} shm_netlnk_status;

/**
 * \fn int32_t netlnk_socket_create(int32_t* sock_fd)
 * \brief [API] Establishes a netlink socket I/F to communicate with shared memory.
 *
 * \n Creates a socket, binds the socket fd to the source address and subscribe for
 * \n receiving the broadcast messages from SHRM driver.
 *
 * \param [in] src netlink source address
 * \return In case of an error, returns a error(-1) else returns
 * \n       the netlink socket file descriptor.
 **/

int32_t netlnk_socket_create(int32_t *sock_fd);


/**
 * \fn int32_t netlnk_socket_close(int32_t netlnkfd)
 * \brief [API] close a netlink socket.
 *
 * \n Closes a socket.
 *
 * \param [in] netlnkfd netlink socket descriptor
 *
 * \return In case of an error, returns a error(-1) else returns zero.
 **/

int32_t netlnk_socket_close(int32_t netlnkfd);

/**
 * \fn int32_t netlnk_socket_send(int32_t netlnkfd, shm_nl_mesg mesg)
 * \brief [API] sends a unicast message to SHRM.
 * \n This API is non-blocking call.
 *
 * \n Send a unicast message to SHRM.
 *
 * \param [in] netlnkfd netlink socket descriptor
 * \n       mesg        Communication message
 *
 * \return In case of an error, returns a error(-1)  else returns zero.
 **/

int32_t netlnk_socket_send(int32_t netlnkfd, shm_nl_mesg mesg);

/**
 * \fn int32_t netlnk_socket_recv(int32_t netlnkfd, int *msg)
 * \brief [API] receives a unicast/multicast message from SHRM.
 * \n This API blocks the process, untill receives the message.
 *
 * \n Receives a message from SHRM.
 *
 * \param [in] netlnkfd netlink socket descriptor
 * \n       mesg     Received messge
 *
 * \return In case of an error, returns a error(-1) else returns zero.
 **/

int32_t netlnk_socket_recv(int32_t netlnkfd, int *msg);

/**
 * \fn int32_t netlnk_send_reset(void)
 * \brief [API] Send a MODEM_RESET_REQ message to SHRM.
 * \n This API is optional to use, It creates the socket,
 * \n send the message and close the socket.
 *
 * \n Send a MODEM_RESET_REQ message to SHRM.
 *
 * \param [in]
 *
 * \return In case of an error, returns a error(-1) else returns zero.
 **/

int32_t netlnk_send_reset(void);

#endif //__SHM_NETLINK_H

