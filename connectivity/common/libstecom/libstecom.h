/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * ST-Ericsson Inter Process Communication library
 *
 */

#ifndef _LIBSTECOM_H_
#define _LIBSTECOM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/socket.h>


/**
 * stec_open - create a control interface
 * @addr: address to connect to
 * @addr_len: size of addr struct
 * Returns: Pointer to abstract control interface data or %NULL on failure
 *
 * This function is used to create a control interface 
 */
struct stec_ctrl *stec_open(const struct sockaddr *addr, socklen_t addr_len);

/**
 * stec_opensrv - create a control interface for server side
 * @addr: address to connect to
 * @addr_len: size of addr struct
 * Returns: Pointer to abstract control interface data or %NULL on failure
 *
 * This function is used to create a control interface 
 */
struct stec_ctrl *stec_opensrv(const struct sockaddr *addr, socklen_t addr_len);

/**
 * stec_close - destroy a control interface to 
 * @ctrl: Control interface data from stec_open()
 *
 * This function is used to destroy a control interface.
 */
void stec_close(struct stec_ctrl *ctrl);

/**
 * stec_get_fd - fetch fd for control interface
 * @ctrl: Control interface data from stec_open()
 * Returns: Socket fd for control interface or -1
 *          if no socket.
 *
 * This function provides the socket file descriptor
 * for the control interface which may be used in
 * select or similar.
 */
int stec_get_fd(struct stec_ctrl *ctrl);

/**
 * stec_get_local_addr - Fetch local addr
 * @ctrl:     Control interface data from stec_open()
 * @addr:     pointer to buffer where from addr struct will be put
 * @addr_len: size of addr struct buffer
 * Returns:   size of addr buffer on success, -1 on failure
 *
 * This functions is used to fetch the local socket address.
 * Useful when telling server our own address.
 */
int stec_get_local_addr(struct stec_ctrl *ctrl, struct sockaddr *addr, socklen_t addr_len);


/**
 * stec_send - Send a message
 * @ctrl:     Control interface data from stec_open()
 * @buf:      Buffer for the message data
 * @buf_len:  Length of the reply buffer
 * Returns: 0 on success, -1 on error
 *
 * This function is used to send messages
 */
int stec_send(struct stec_ctrl *ctrl, const char *buf, size_t buf_len);


/**
 * stec_sendto - Send a message to a specified client
 * @ctrl:     Control interface data from stec_open()
 * @buf:      Buffer for the message data
 * @buf_len:  Length of the reply buffer
 * @addr:     receiver of the event
 * @addr_len: size of addr
 * Returns: 0 on success, -1 on error
 *
 * This function is used to send messages
 */
int stec_sendto(struct stec_ctrl *ctrl, const char *buf, size_t buf_len, const struct sockaddr *addr, socklen_t addr_len);

/**
 * stec_recv - Receive a pending message
 * @ctrl:        Control interface data from stec_open()
 * @buf:         Buffer for the message data
 * @buf_len:     Length of the reply buffer
 * @sec_timeout: Timeout in seconds for receive. Set to -1 for blocking
 * Returns:      Length of received data on success, -1 on failure
 *
 * This function will receive a pending message. This function will block
 * if no messages are available. The received message will be written
 * to buf and the actual length of the message received returned.
 *
 * stec_recv() may be used for receiving event messages.
 */
int stec_recv(struct stec_ctrl *ctrl, char *buf, size_t buf_len, int sec_timeout);

/**
 * stec_sendsync - Send a request to server process
 * @ctrl:        Control interface data from stec_open()
 * @req:         request
 * @req_len:     Length of the request in bytes
 * @resp:        response
 * @resp_len:    Length of the response in bytes
 * @sec_timeout: Timeout in seconds for receive. Set to -1 for blocking
 * Returns: size of received buffer on success, -1 on error (send or receive failed)
 *
 * This function is used to send requests to .. . 
 *
 * stec_sendsync() blocks until the server process has responded
 * the given request.
 */
int stec_sendsync(struct stec_ctrl *ctrl, const char *req, size_t req_len, char *resp, size_t resp_len, int sec_timeout);

/**
 * stec_get_last_recvfrom - Fetch from addr for last received packet
 * @ctrl:     Control interface data from stec_open()
 * @addr:     pointer to buffer where from addr struct will be put
 * @addr_len: size of addr struct buffer
 * Returns:   size of addr struct on success, -1 on failure
 *
 * This functions is used to fetch the sender address for a
 * received packet and will only work on a server socket.
 */
int stec_get_last_recvfrom(struct stec_ctrl *ctrl, struct sockaddr *addr, socklen_t addr_len);



/**
 * stec_subscribe - Register as an event monitor for the control interface
 * @ctrl: Control interface data from stec_open()
 * Returns: 0 on success, -1 on failure
 *
 * This function registers the control interface connection as a monitor for
 * events. After a success stec_subscribe() call, the control interface connection
 * starts receiving event messages that can be read with stec_recv().
 * programs can register two control interface connections and use one of them for
 * requests and the other one for receiving event messages, in other words, call
 * stec_subscribe() only for the control interface connection that will be used
 * for event messages.
 */
int stec_subscribe(struct stec_ctrl *ctrl);

/**
 * stec_unsubscribe - Unregister event monitor from the control interface
 * @ctrl: Control interface data from stec_open()
 * Returns: 0 on success, -1 on failure
 *
 * This function unregisters the control interface connection as a monitor for
 * events, i.e., cancels the registration done with stec_subscribe().
 */
int stec_unsubscribe(struct stec_ctrl *ctrl);

/**
 * stec_process_recv - Process received packet automatically
 * @ctrl: Control interface data from stec_open()
 * Returns: 0 on success, -1 on failure
 *
 * This function will automatically process an incoming packet,
 * checking it for subscribe or unsubscribe and maintain the
 * event subscriber list. This is only useful for a server-side
 * event socket.
 */
int stec_process_recv(struct stec_ctrl *ctrl);

/**
 * stec_sendevent - broadcast event to process(es)
 * @ctrl:     Control interface data from stec_open()
 * @buf:      the message to be sent
 * @buf_len:  Length of the message in bytes
 * Returns: 0 on success, -1 on error (send or receive failed)
 *
 * This function is used to broadcast messages to processes that has registered
 * by calling stec_subscribe(). 
 */
int stec_sendevent(struct stec_ctrl *ctrl, const char *buf, size_t buf_len);

/**
 * stec_sendeventto - Send a event to a client process
 * @ctrl:     Control interface data from stec_open()
 * @buf:      the message to be sent
 * @buf_len:  Length of the message in bytes
 * @addr:     receiver of the event
 * @addr_len: size of addr
 * Returns: 0 on success, -1 on error (send or receive failed)
 *
 * This function is used to send an event to a specific event subscriber.
 * Higher layers must provide destination address.
 */
int stec_sendeventto(struct stec_ctrl *ctrl, const char *buf, size_t buf_len, const struct sockaddr *addr, socklen_t addr_len);

#ifdef __cplusplus
}
#endif

#endif

