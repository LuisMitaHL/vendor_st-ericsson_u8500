/*                               -*- Mode: C -*- 
 * Copyright (C) 2010, ST-Ericsson
 * 
 * File name       : event_stream.h
 * Description     : Event stream class.
 * 
 * Author          : Mats Bergstrom <mats.m.bergstrom@stericsson.com>
 * 
 */

#ifndef __event_stream_h__
#define __event_stream_h__ (1)

#ifdef __cplusplus
extern                  "C" {
#endif
#if 0
}
#endif
#include <stdlib.h>
#include <unistd.h>
/**
 * @brief Incomplete type representing an event stream.
 *
 * The event stream reads data from a file descriptor and places data in an
 * input buffer for consumption by a user-defined callback function.
 * The event stream (normally) maintains an input buffer.
 */ typedef struct ste_es_s ste_es_t;


/**
 * @brief The state of an event stream.
 */
typedef enum {
    STE_ES_STATE_OPEN = 0,              /**< The stream is open.  */
    STE_ES_STATE_ERROR,                 /**< A read error has occured.  */
    STE_ES_STATE_ATEOF,                 /**< The EOF has been reached.  */
    STE_ES_STATE_RESET,                 /**< Received RESET from Modem.  */
    STE_ES_STATE_MAX_STATE      /* Terminator */
} ste_es_state_t;


/**
 * @brief Callback function used by the event stream.
 * @param buf	Pointer to start of data in the input buffer.
 * @param buf_max	Pointer to one past end of the data in the input buffer.
 * @param id	User data.  This is the closure user_data.
 * @return	Number of character consumed in the input buffer.
 *
 * If both buf and buf_max are 0, the callback function is called when the
 * event stream object is destroyed using ste_es_delete().
 * For normal streams, buf and buf_max point into the input buffer.
 * For accept sockets, buf_max is always 0 and buf points to an int holding the
 * newly accepted socket.
 * For nonbuf streams, buf_max is always 0 and buf points to an int holding the
 * fd to read from.
 * 
 */
typedef ssize_t         ste_es_parser_func(char *buf, char *buf_max,
                                           void *ud);


/**
 * @brief Closure used by the event stream.
 */
typedef struct {
    ste_es_parser_func     *func;       /**< Parser callback function  */
    void                   *user_data;
} ste_es_parser_closure_t;


/**
 * @brief	Create a normal event stream.
 * @param fd	File descriptor to read from. Must be in non-blocking mode!
 * @param bsize	Input buffer size.
 * @param pc	Parser closure.
 * @return	Created event stream object or 0 on failure.
 */
ste_es_t               *ste_es_new_normal(int fd, size_t bsize,
                                          ste_es_parser_closure_t * pc);

/**
 * @brief	Create an accept event stream.
 * @param fd	Socket to accept() from.
 * @param pc	Parser closure.
 * @return	Created event stream object or 0 on failure.
 */
ste_es_t               *ste_es_new_accept(int fd,
                                          ste_es_parser_closure_t * pc);

/**
 * @brief	Create a non-buffered event stream.
 * @param fd	Fd passed to callback function.
 * @param pc	Parser closure.
 * @return	Created event stream object or 0 on failure.
 */
ste_es_t               *ste_es_new_nonbuf(int fd,
                                          ste_es_parser_closure_t * pc);

/**
 * @brief	Delete an event stream.
 * @param es	Event stream object to delete.
 *
 * The obejct is deleted.  For normal streams the input buffer is destroyed.
 * The callback function is called with both buf and buf_max pointers 0 to
 * indicate that the stream has died.
 */
void                    ste_es_delete(ste_es_t * es);

/**
 * @brief       Sets the user data in the closure connected to the event stream 
 * @param es    Event stream to update
 * @param user_data User data to set in the closure 
 * 
 * Updates the closure connected to the event stream with new user data.
 */
void                    ste_es_set_ud(ste_es_t * es, void * user_data);

/**
 * @brief	Fetch the file descriptor of the event stream.
 * @param es	Event stream to operate on.
 * @return		File descriptor of the event stream.
 * 
 * The intention is to fetch the file descriptor and use it in a subsequent
 * select().  If the select indicates there is input available from the fd
 * the user is expected to call ste_es_read() and then ste_es_parse().
 */
int                     ste_es_fd(ste_es_t * es);


/**
 * @brief	Get the state of the event stream.
 * @param es	Event stream to operate on.
 * @return		The state of the event stream.
 */
ste_es_state_t          ste_es_state(ste_es_t * es);


/**
 * @brief	Read data from the fd to the input buffer.
 * @param es	Event stream to operate on.
 * @return	No of bytes read, or -1 for error.
 * 
 * Data is consumed from the fd.  For normal event streams this means the data
 * is placed in the input buffer and the number of bytes in the input buffer
 * is returned.  For accept sockets, an accept() is performed and the new
 * file descriptor is cashed (for the subsequent ste_es_read()) and 1 is
 * returned.  For nonbuf streams the callback is called and is responsible for
 * reading from the file descriptor.
 * In all cases -1 is returned to inicate error.
 */
ssize_t                 ste_es_read(ste_es_t * es);

/**
 * @brief	Parse data.
 * @param es	Event stream to operate on.
 * @return	No of bytes parsed, or -1 for error.
 *
 * Data is consuned from the input buffer.
 * For normal event streams the callback function is called until there is
 * either no more data in the input buffer, or the callback function either
 * returns 0 (to indicate that more data is needed to parse input data), or a
 * negative number indicating an error.
 * For accept event streams one call to the callback function is done, where the
 * buf parameter points to an integer containign the new file descriptor.
 * For nonbuf streams, this call just returns 1 with no side effects.
 */
ssize_t                 ste_es_parse(ste_es_t * es);

#if 0
{
#endif
#ifdef __cplusplus
}
#endif
#endif
