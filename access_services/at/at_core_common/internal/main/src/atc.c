/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */



#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <unistd.h>
#include <termios.h>
#ifdef AT_SERVICE
#include <time.h>
#include <cutils/sockets.h>
#endif /* AT_SERVICE */

#include "atc_anchor.h"
#include "atc_config.h"
#include "atc_connection.h"
#include "atc_debug.h"
#include "atc_exe_glue.h"
#include "atc.h"
#include "atc_log.h"
#include "atc_parser.h"
#include "atc_selector.h"
#include "atc_sockets.h"
#include "c_type.h"
#include "exe.h"

#include "util_security.h"

#ifdef ATC_IN_ANDROID
#define LOG_TAG "AT"
/* In Android use their logging*/
#include "utils/Log.h"
#endif

#define AT_SOCKET "at"
#define MAX(A, B) (A)>(B)?(A):(B)
/* environment buffer, the kernel's size in lib/kobject_uevent.c should fit in */
#define HOTPLUG_BUFFER_SIZE     1024
#define HOTPLUG_NUM_ENVP        32
#define OBJECT_SIZE         512
#define USB_STATE_FILE          "/sys/devices/virtual/android_usb/android0/state"
#define USB_STATE_LEN           20
typedef struct {
    int fd;
    char name[SERIAL_DEVICE_MAX_LENGTH + 1];
    long baud;
} serial_device_t;



/******************************************************************************
 * Static Data
 *******************************************************************************
 */
static serial_device_t serial_device[SERIAL_DEVICE_MAX_NUMBER];
static int serial_device_config_num     = 0;        /* Number of configured serial devices */

static bool stop                        = false;    /* Set to true to stop daemon */

#ifdef AT_SERVICE
static time_t at_service_start          = 0;
#endif
static int is_usb_connected             = false;
static int32_t netlink_fd               = -1;
static int32_t ttyGS0_fd                = -1;
static atc_context_t *ttyGS0_ctx        = NULL;
/******************************************************************************
 * Static Functions
 *******************************************************************************
 */
static bool prepare_string_for_parser(atc_context_t *context,
                                      unsigned char *end);
static bool handle_data_from_connection(int fd, void *data_p);

static bool atc_callback_for_anchor_connection(int fd, void *data_p);
static bool atc_callback_for_debug_connection(int fd, void *data_p);

#ifdef AT_SERVICE
static bool atc_callback_for_at_service_connection(int fd, void *data_p);
static int handle_data_from_at_service(atc_context_t *context_p);
static int read_from_at_service(atc_context_t *context_p, unsigned char *buf_p,
                                size_t buf_size);
#endif /* AT_SERVICE */

static int atc_create_netlink_socket();
static bool atc_kernel_uevent_handler(int fd, void *data_p);
static int atc_check_usb_status(void);


/******************************************************************************
 *******************************************************************************
 * AT MODULE - AT PROCESS MAIN LOOP
 *******************************************************************************
 *******************************************************************************
 */

bool atc_stop()
{
    bool old_stop = stop;

    /*
     *  Set stop flag to true. This will make the main
     * function drop out of its loop calling select_loop().
     */
    stop = true;

    return old_stop;
}

/* --------------------------------------------------------------
 * Writes data to a context's anchor connection. Returns the number
 * of bytes in managed to write or -1 for no bytes.
 * --------------------------------------------------------------*/
int atc_write_to_client(atc_context_t *context, unsigned char *data_p,
                        unsigned int length)
{
    size_t write_total = 0;
    int written;
    int fd;

    if (!context) {
        goto error;
    }

    if (1 > length) {
        goto error;
    }

    if (context->at_connection_type == ATC_CONNECTION_TYPE_INTERNAL) {
        fd = context->sockfd;
    } else {
        if (context->conn) {
            fd = context->conn->fdw;
        } else {
            goto error;
        }
    }

    do {
        written = write(fd, data_p, (length - write_total));

        if (written > 0) {
            write_total += written;
            data_p += written;

        } else if (written < 0) {
            int errnum = errno;

            switch (errno) {
            case EINTR:
                /* Interrupted syscall, try again */
                break;

            default:
                ATC_LOG_E("Error %d (%s) on fd %d",
                          errnum, strerror(errnum), fd);
                goto error;
            }

        } else { /* write_length == 0 */
            /* Should never happen as zero size writes are not allowed */
            goto error;
        }

    } while (write_total < length);

    /* Print the output for the external channel. */
    if (context->at_connection_type == ATC_CONNECTION_TYPE_EXTERNAL) {
        ATC_LOG_I("AT(%s)< %s",
                  context->conn != NULL ? context->conn->device_name : NULL,
                  data_p - write_total + 2);
    }

    return write_total;

error:
    return -1;
}


#ifdef AT_SERVICE
/*--------------------------------------------------------------
 * Open a socket for communication with the AtService daemon.
 * -------------------------------------------------------------*/
bool atc_connect_to_at_service(atc_context_t *context_p)
{
    static bool displayed_msg = false;
    int fd = -1;

    if (!context_p) {
        goto error;
    }

    fd = context_p->sockfd;

    if (0 > fd) {
        /* Try to launch AtService service. But only once per minute. */
        int result;
        time_t time_now = time(NULL);

        if (60 < (time_now - at_service_start) || time_now < at_service_start) {
            ATC_LOG_I("%s: Trying to launch AtService", __FUNCTION__);
            result = system("am startservice -n com.stericsson.atservice/.AtService --ez thread_start true");

            if (0 > result) {
                int errnum = errno;
                ATC_LOG_E("%s: Launch of AtService failed, error(%d)=\"%s\"!",
                          __FUNCTION__, fd, errnum, strerror(errnum));
            }

            at_service_start = time_now;
        }
    } else {
        /* Disconnect socket if already opened */
        ATC_LOG_I("%s: AtService communication: Closing socket(%d)!", __FUNCTION__, fd);

        selector_deregister_callback_for_fd(fd);
        close(fd);
        fd = -1;

        context_p->is_waiting = false;
        context_p->sockfd = fd;
        displayed_msg = false;
    }

    /* Try to connect to AT_SERVICE */
    fd = socket_local_client(AT_SOCKET, ANDROID_SOCKET_NAMESPACE_ABSTRACT, SOCK_STREAM);

    if (0 > fd) {
        if (!displayed_msg) {
            /* Don't be obnoxious, display this message once only */
            displayed_msg = true;
            ATC_LOG_I("%s: AtService Communication: Not possible to setup a socket at this time, try again later.",
                      __FUNCTION__);
        }
    } else {
        /* Set socket to non-blocking */
        int flags = fcntl(fd, F_GETFL, 0);

        if (flags < 0 || fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
            int errnum = errno;
            ATC_LOG_E("%s: AtService Communication error: Unable to set socket(%d) non-blocking, error(%d)=\"%s\"!",
                      __FUNCTION__, fd, errnum, strerror(errnum));
        }

        /* Register socket with selector monitor */
        selector_register_callback_for_fd(fd, atc_callback_for_at_service_connection, context_p);

        ATC_LOG_I("%s: AtService Communication: Opened socket(%d).", __FUNCTION__, fd);
    }

    context_p->sockfd = fd;

    return !(0 > fd);

error:
    return false;
}


/* --------------------------------------------------------------
 * Writes to the AtService with the socket belonging to context
 * with the given ID. If successful the number of bytes written
 * will be returned otherwise -1.
 * --------------------------------------------------------------*/
int atc_write_to_at_service(unsigned char context_id, unsigned char *cmd_p,
                            size_t size)
{
    atc_context_t *context_p = atc_context_get_by_context_id(context_id);
    size_t write_total = 0;
    int written;
    char cmd_to_send[size + 2];

    if (!context_p) {
        ATC_LOG_E("%s: AtService Communication error: Invalid parameter context_id(%d)!",
                  __FUNCTION__, context_id);
        goto error;
    }

    if (1 > size) {
        ATC_LOG_E("%s: AtService Communication error: Invalid parameter size(%d)!",
                  __FUNCTION__, size);
        goto error;
    }

    if (0 > context_p->sockfd) {
        atc_connect_to_at_service(context_p);
    }

    if (0 > context_p->sockfd) {
        ATC_LOG_E("%s: AtService Communication error: No socket connection!",
                  __FUNCTION__);
        goto error;
    }

    /* Prepare command for sending to AtService */
    strncpy(cmd_to_send, (const char *) cmd_p, size);
    cmd_to_send[size] = '\0';
    ATC_LOG_D("Command (%s)", cmd_to_send);

    /* AtService expects a line-feed at the end of the command */
    cmd_to_send[size++] = '\n';
    cmd_to_send[size] = '\0';

    do {
        written = write(context_p->sockfd, &cmd_to_send[write_total], (size - write_total));

        if (written > 0) {
            write_total += written;

            /* set the context to waiting for a response */
            context_p->is_waiting = true;

        } else if (written < 0) {
            int errnum = errno;

            switch (errnum) {
            case EINTR:
                /* Interrupted syscall, try again */
                break;

            default:
                ATC_LOG_E("%s: Error %d (%s) on fd %d", __FUNCTION__,
                          errnum, strerror(errnum), context_p->sockfd);
                goto error;
            }

        } else { /* written == 0 */
            /* Should never happen as zero size writes are not allowed */
            goto error;
        }

    } while (write_total < size);

    return write_total;

error:
    return -1;
}
#endif /* AT_SERVICE */


/*--------------------------------------------------------------
 * Connects a serial connection from the anchor with an at context.
 * -------------------------------------------------------------*/
bool atc_connect_external(anchor_connection_t *conn_p)
{
    atc_context_t *context_p = atc_context_get_free();

    if (!context_p) {
        ATC_LOG_E("Not enough at contexts in system!!!");
        return false;
    }

    if (!atc_context_connect_to_parser(context_p)) {
        ATC_LOG_E("Not enough parser states in system!!!");
        return false;
    }

    if (!atc_configure_serial(conn_p,
                              context_p->parser_state->Echo,
                              Parser_GetS3(context_p->parser_state),
                              parser_get_s5(context_p->parser_state))) {
        ATC_LOG_E("Failed configuring serial port!!!");
        return false;
    }

    context_p->conn = conn_p;
    context_p->at_connection_type = ATC_CONNECTION_TYPE_EXTERNAL;
    Parser_SetConnectionType(context_p->parser_state, context_p->at_connection_type);

    selector_register_callback_for_fd(conn_p->fdr, atc_callback_for_anchor_connection, context_p);

    /* Save fd and context_p in global variables for usage in kernel uevent handler */
    ttyGS0_ctx = context_p;
    ttyGS0_fd = conn_p->fdr;

#ifdef AT_SERVICE
    atc_connect_to_at_service(context_p);
#endif /* AT_SERVICE */

    return true;
}


/*--------------------------------------------------------------
 * Prepare a string for parsing. Checks that it starts with AT
 * and change letters inside quotes to upper case.
 * -------------------------------------------------------------*/
static bool prepare_string_for_parser(atc_context_t *context,
                                      unsigned char *end)
{
    bool result = false;
    unsigned char *current = context->input_p;
    bool inside_quotes = false;
    /*TODO: This function should handle the translation caused by AT+CSCS.
     *      Need a redesign to handle HEX or UTF-8 since they will use more
     *      than one byte per character.
     *                                                                      */

    if ((*end = Parser_GetS3(context->parser_state))) {
        /* The parser expects all characters to be upper case except inside strings.*/
        while (current < end) {
            if (*current == '\"') {
                inside_quotes = !inside_quotes;
            }

            if (!inside_quotes) {
                *current = to_upper(*current);
            }

            current++;
        }

        /* The string should start with AT.*/
        if (*context->input_p == 'A' && *(context->input_p + 1) == 'T') {
            result = true;
        }
    }

    return result;
}


#ifdef AT_SERVICE
/**********************************************************************************
 * Name: read_from_at_service
 *
 * Parameters:  atc_context *context    context
 *              char* buf_p             a pointer to a char buffer
 *              size_t to_read          how many bytes to read from socket
 *
 * Returns:    if successful it returns the number of bytes it read otherwise -1
 *
 * Description: This function reads from the file associated with the open
 *         file descriptor, sockfd, into the buffer pointed to by buf_p.
 *
 *         The message read could be a response with a result code or one
 *         without. In the first case no other response can be expected for
 *         the AT command. In the second, at least on more response can be
 *         expected.
 *
 *         The AT command in turn can be a part of a chained AT command in
 *         which case the result codes from all the parts must be analyzed to
 *         get the result code for the chained AT command.
 *
 *********************************************************************************/
static int read_from_at_service(atc_context_t *context_p, unsigned char *buf_p,
                                size_t to_read)
{
    size_t total_read = 0;
    int nbr = 0;
    int count = 0;
    struct timespec req = {0, 0};

    do {
        nbr = read(context_p->sockfd, buf_p, (to_read - total_read));
        count++;

        if (0 < nbr) {
            total_read += nbr;
            buf_p += nbr;
            break;

        } else if (0 > nbr) {
            int errnum = errno;

            switch (errno) {
            case EINTR:
                /* Interrupted syscall, try again */
                break;

            case EAGAIN:
                /* Try again */
                req.tv_sec = 0;
                req.tv_nsec = 10000000;
                nanosleep(&req, NULL);
                break;

            default:
                ATC_LOG_E("%s: Error %d (%s) on fd %d", __FUNCTION__,
                          errnum, strerror(errnum), context_p->sockfd);
                goto error;
            }

        } else { /* 0 == nbr */
            /* Socket has been closed, return error */
            goto error;
        }

    } while (total_read < to_read && count < 100);

    if (total_read == to_read) {
        return total_read;
    }

error:
    return -1;
}


/*--------------------------------------------------------------
 * Reads data from a file descriptor representing a socket. The
 * data is written back to an anchor connection. Note that this
 * method will write the entire response it gets from the socket
 * to the anchor connection.
 * -------------------------------------------------------------*/
static int handle_data_from_at_service(atc_context_t *context_p)
{
    unsigned char header[HEADER_SIZE + 1];
    unsigned char buffer[BUFFER_SIZE];
    int size;
    char type;
    int nbr;

    memset(header, 0, HEADER_SIZE + 1);
    memset(buffer, 0, BUFFER_SIZE);

    /* read header from socket */
    size = sizeof(header) - 1;
    nbr = read_from_at_service(context_p, header, size);

    if (size != nbr) {
        if (0 == nbr) {
            ATC_LOG_I("%s: AtService communication: Peer socket closed.", __FUNCTION__);
        } else {
            ATC_LOG_E("%s: AtService communication error: Incomplete header!", __FUNCTION__);
        }

        goto sockclose;
    }

    /* decode header - Format: TYPEiSIZExyzCMEabc */
    type = header[4];
    header[HEADER_SIZE] = '\0';
    size = atoi((const char *) &header[9]);

    if (0 > size || (int)sizeof(buffer) < size) {
        ATC_LOG_E("%s: AtService communication error: Invalid payload size(%d)!", __FUNCTION__, size);
        goto sockclose;

    } else if (0 < size) {
        /* read payload */
        nbr = read_from_at_service(context_p, buffer, size);

        if (size != nbr) {
            ATC_LOG_E("%s: AtService communication error: Incomplete payload, expected(%d), got(%d)!", __FUNCTION__, size, nbr);
            goto sockclose;
        }

    } else {
        /* size in header is 0, no message will follow */
        nbr = 0;
    }

    /* managed to read something, there is no end sign in buf_p */
    if (type == AT_RESULT_CODE_OK) { /* Result code OK from java*/
        context_p->is_waiting = false;
        context_p->result_code = AT_OK;
    } else if (type == AT_RESULT_CODE_ERROR) { /* Result code ERROR from java*/
        context_p->is_waiting = false;
        context_p->result_code = AT_ERROR;
    } else if (type == AT_RESULT_CODE_CME_ERROR) { /* Result code CME_ERROR from java*/
        context_p->is_waiting = false;
        context_p->result_code = AT_CME_ERROR;
        context_p->cme_error_code = (exe_cmee_error_t)atoi((const char *) &header[15]);
    }

    if (0 < size) {
        /* write to anchor connection */
        /* only want to write response text - no result code - */
        /* result code is taken care of from Parser_ParseCommandLine */
        nbr = atc_write_to_client(context_p, buffer, size);
    }

    return nbr;

sockclose:
    /* Close and try to re-open AtService socket */
    atc_connect_to_at_service(context_p);

    return -1;
}
#endif /* AT_SERVICE */


/*--------------------------------------------------------------
 * Default callback for the selector.
 * This function is called when select returns, but no file
 * descriptor can be found. The variable fd is -1.
 * -------------------------------------------------------------*/
static bool atc_default_callback(int fd, void *data_p)
{
    return false;
}

/*--------------------------------------------------------------
 *
 *
 * -------------------------------------------------------------*/
static bool atc_callback_for_anchor_connection(int fd, void *data_p)
{
    atc_context_t *context_p = (atc_context_t *) data_p;
    handle_data_from_connection(context_p->conn->fdr, context_p);
    return true;
}


#ifdef AT_SERVICE
/*--------------------------------------------------------------
 *
 *
 * -------------------------------------------------------------*/
static bool atc_callback_for_at_service_connection(int fd, void *data_p)
{
    atc_context_t *context_p = (atc_context_t *) data_p;
    handle_data_from_at_service(context_p);
    Parser_ParseCommandLine(context_p->parser_state, NULL);
    return true;
}
#endif /* AT_SERVICE */


/*--------------------------------------------------------------
 *
 *
 * -------------------------------------------------------------*/
static bool atc_callback_for_debug_connection(int fd, void *data_p)
{
    char buffer[20];
    int nbr_of_bytes;
    int i;
    anchor_connection_t *conn;
    nbr_of_bytes = read(fd, buffer, 20);

    /* TODO: assume connect for now...*/
    if (nbr_of_bytes > 0) {
        test_connect(buffer);
        conn = anc_get_connections();

        for (i = 0; i <= MAX_NBR_CONNECTIONS; i++) {
            at_debug_print_conn(&conn[i]);
        }
    } else if (nbr_of_bytes == 0) {
        ATC_LOG_E("reader_loop: Closing debug connection\n");
        close(fd);
    } else {
        ATC_LOG_E(
            "reader_loop: WARNING debug connection not working :( \n");
        /* TODO: Handle this in a nice way... quit is probably good when debugging */
        exit(1);
    }

    return true;
}


/*--------------------------------------------------------------
 * Handles disconnect of a connection.
 * Deregisters from selector and closes the file-descriptor(s).
 * -------------------------------------------------------------*/
static bool atc_disconnect(atc_context_t *context_p)
{
    bool result = false;

    /* Disconnect from anchor connection (if present) */
    if (NULL != context_p->conn) {
        ATC_LOG_I("disconnect anchor connection");
        selector_deregister_callback_for_fd(context_p->conn->fdr);

        if (!anc_disconnect(context_p->conn)) {
            ATC_LOG_E("Anchor disconnect FAILED (%d)\n",
                      context_p->context_id);
        }
    }

    /* Disconnect from socket */
    if (!(0 > context_p->sockfd)) {
        ATC_LOG_I("disconnect socket (%d)", context_p->sockfd);
        selector_deregister_callback_for_fd(context_p->sockfd);
        close(context_p->sockfd);
    }

    /* Disconnect from parser context */
    if (!atc_context_disconnect_from_parser(context_p)) {
        ATC_LOG_E("Parser disconnect FAILED (%d)",
                  context_p->context_id);
        goto exit;
    }

    result = true;

exit:
    return result;
}


/*--------------------------------------------------------------
 * Reads data from a file descriptor (in the context) into the
 * end of a buffer. The function will add data to the buffer each
 * time it is called until a termination character is found (Set by ATS3).
 * It will then call the parser and after the line has been parsed move the remaining
 * data to the beginning of the buffer.
 *
 * -------------------------------------------------------------*/
static bool handle_data_from_connection(int fd, void *data_p)
{
    atc_context_t *context_p = (atc_context_t *)data_p;
    int len = 0;
    int nbr_read = 0;
    char S3_char = Parser_GetS3(context_p->parser_state);
    uint8_t *end_of_line;
    uint8_t *current_pos;
    int unread;

    /* Find out how many bytes are queued to be read on the descriptor */
    if (ioctl(fd, SIOCINQ, &unread) < 0) {
        /* Error handling */
        int errnum = errno;
        ATC_LOG_E("ioctl(%d) returns error(%d)=\"%s\".", fd, errnum, strerror(errnum));
        goto error;
    }

    /* Always try to read at least 1 byte */
    unread = (unread > 0) ? unread : 1;

    /* Make sure we have enough buffer to hold it all */
    if ((context_p->input_pos + unread) > context_p->input_size) {
        uint8_t *buf_p;
        size_t buf_size;

        /* Keep buffer size within an allowed maximum size */
        if (AT_COMMAND_BUFFER_MAX_SIZE > context_p->input_size) {
            buf_size = context_p->input_pos + unread;

            if (AT_COMMAND_BUFFER_MAX_SIZE < buf_size) {
                ATC_LOG_W("Context %d: Maximum command buffer size of %d bytes reached!",
                          context_p->context_id, AT_COMMAND_BUFFER_MAX_SIZE);
                buf_size = AT_COMMAND_BUFFER_MAX_SIZE;
                unread = buf_size - context_p->input_pos;
            }

            buf_p = realloc(context_p->input_p, buf_size + 1);

            if (buf_p) {
                ATC_LOG_D("Context %d: Command buffer increased from %d to %d bytes, at %p.",
                          context_p->context_id, context_p->input_size, buf_size, buf_p);
                context_p->input_p = buf_p;
                context_p->input_size = buf_size;
            } else {
                ATC_LOG_E("Context %d: Command buffer (re-)allocation failure!",
                          context_p->context_id);
                goto error;
            }
        } else {
            ATC_LOG_E("Context %d: Required command buffer size exceeds allowed limit of %d bytes!",
                      context_p->context_id, AT_COMMAND_BUFFER_MAX_SIZE);
            goto error;
        }
    }

    /* Read the data queued on the descriptor */
    nbr_read = read(fd, context_p->input_p + context_p->input_pos, unread);

    if (nbr_read > 0) {
        current_pos = context_p->input_p + context_p->input_pos;
        context_p->input_pos += nbr_read;
        end_of_line = context_p->input_p + context_p->input_pos;
        *end_of_line = '\0';

        /* +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
         * | a | t | + | c | k | p | d | ? | S3|   |   |   |   |   |   |   |   |
         * +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
         * ^                                   ^
         * |                                   |
         * context->input_p                    context->input_p + context->input_pos
         * |                                   |
         * current_pos                         end_of_line
         *
         * */

        while (current_pos < end_of_line) {

            switch (context_p->input_mode) {
                /* Wait for CTRL+Z to end transparent mode to exit and wait for CR/LF */
            case ATC_INPUT_MODE_TRANSPARENT:

                if (AT_CTRL_Z == *current_pos) {
                    len = current_pos - context_p->input_p;
                    struct AT_ResponseMessage_s AT_response_message;
                    AT_response_message.data_p = context_p->input_p;
                    AT_response_message.exe_result_code = EXE_SUCCESS;
                    AT_response_message.len = len;
                    Parser_ParseCommandLine(context_p->parser_state, &AT_response_message);
                    /* Change input_mode to wait for CR/LF */
                    context_p->input_mode = ATC_INPUT_MODE_WAIT_FOR_NEW_LINE;
                    *current_pos = S3_char;
                    continue;
                }

                break;

                /* Wait for CR/LF to end the AT command, process AT command and return to wait for new-line */
            case ATC_INPUT_MODE_AT_COMMAND:

                if (S3_char == *current_pos) {
                    len = current_pos - context_p->input_p;
                    Parser_AT(context_p->input_p + 2, len - 2, AT_DATA_COMMAND, context_p->parser_state);
                    /* Change input_mode to wait for CR/LF */
                    context_p->input_mode = ATC_INPUT_MODE_WAIT_FOR_NEW_LINE;
                    /* Exit to enclosing while-loop whithout advancing current_pos */
                    continue;
                }

                break;

                /* Wait for the beginning of an AT command */
            case ATC_INPUT_MODE_NEW_LINE:

                if (current_pos == context_p->input_p) {
                    /* First character must be 'a' or 'A' */
                    if ('A' == to_upper(*current_pos)) {
                        break;
                    }
                } else {
                    /* Second character must be 't' or 'T' */
                    if ('T' == to_upper(*current_pos)) {
                        /* This looks promising, wait for terminating CR/LF */
                        context_p->input_mode = ATC_INPUT_MODE_AT_COMMAND;
                        break;
                    }
                }

                /* Not an AT-command. Call the AT-parser to check if there
                 * is a command currently executing that can be aborted. */
                Parser_AT(NULL, 0, AT_DATA_ABORT, context_p->parser_state);

                /* Change input_mode to wait for CR/LF */
                context_p->input_mode = ATC_INPUT_MODE_WAIT_FOR_NEW_LINE;
                /* Intentional drop-through to next case label */

                /* Wait for a new line (CR/LF) */
            case ATC_INPUT_MODE_WAIT_FOR_NEW_LINE:
            default:

                if (S3_char == *current_pos) {
                    len = current_pos + 1 - context_p->input_p;
                    memmove(context_p->input_p, current_pos + 1, context_p->input_pos - len);
                    context_p->input_pos -= len;
                    end_of_line -= len;

                    /* The next_input_mode is currently only used by the AT+CMGS handler to get into
                     * transparent_mode after the input buffer has been emptied from the AT+CMGS=<n>
                     * itself.
                     */
                    context_p->input_mode = context_p->next_input_mode;
                    context_p->next_input_mode = ATC_INPUT_MODE_NEW_LINE;
                    current_pos -= len - 1;
                    /* Exit to enclosing while-loop whithout advancing current_pos */
                    continue;
                }

                break;
            }

            current_pos++;

        } /* while () */

        /* Empty the input buffer if still looking for beginning of new line */
        if (ATC_INPUT_MODE_WAIT_FOR_NEW_LINE == context_p->input_mode &&
                context_p->input_pos > 0) {
            context_p->input_pos = 0;
        }

    } else if (nbr_read < 0) {
        int errnum = errno;

        switch (errno) {
        case EINTR:
            /* Interrupted syscall, try again */
            break;

        case EAGAIN:
            /* Try again */
            break;

        default:
            ATC_LOG_E("Error %d (%s) on fd %d", errnum, strerror(errnum), fd);
            goto error;
        }

        /* We rely on being called immediately again if any data reamins to be read on the socket */

    } else {
        if (ATC_CONNECTION_TYPE_INTERNAL == context_p->at_connection_type) {
            ATC_LOG_W("Internal AT command socket (%d) closed.", fd);
        } else {
            ATC_LOG_W("External AT command channel (%d) closed.", fd);
        }

        goto error;
    }

    return true;

error:

    if (ATC_CONNECTION_TYPE_INTERNAL == context_p->at_connection_type) {
        /* Disconnect AT command socket */
        if (!atc_disconnect(context_p)) {
            ATC_LOG_E("Internal AT command socket (%d) disconnect ERROR", fd);
        }
    } else if (ATC_CONNECTION_TYPE_EXTERNAL == context_p->at_connection_type) {
        /* Disconnect AT command channel */
        bool type_serial = (NULL != context_p->conn && ANC_TYPE_SERIAL == context_p->conn->type);

        if (!atc_disconnect(context_p)) {
            ATC_LOG_E("External AT command channel (%d) disconnect ERROR", fd);
        }

        ttyGS0_ctx = NULL;
        ttyGS0_fd = -1;
        /* Reconnect channel if it is a serial device channel and usb is still connected */
        if (type_serial && is_usb_connected && !atc_check_usb_status()) {
            int i;

            /* Use the fd to identify the serial device that needs to be reconnected */
            for (i = 0; i < serial_device_config_num; i++) {
                if (fd == serial_device[i].fd) {
                    serial_device[i].fd = anc_connect(serial_device[i].name, serial_device[i].baud);
                    break;
                }
            }
        }
    }

    return true;
}


/*--------------------------------------------------------------
 * Accept incoming socket connection requests
 *
 *-------------------------------------------------------------*/
static bool atc_connect_internal(int socket_fd, void *data_p)
{
    int connection_fd = atc_socket_accept(socket_fd);

    if (connection_fd < 0) {
        ATC_LOG_E("Accept failed for fd=%d.", socket_fd);
    } else {
        atc_context_t *context_p = atc_context_get_free();

        if (context_p) {
            if (atc_context_connect_to_parser(context_p)) {
                context_p->conn = NULL;
                context_p->at_connection_type = ATC_CONNECTION_TYPE_INTERNAL;
                Parser_SetConnectionType(context_p->parser_state, ATC_CONNECTION_TYPE_INTERNAL);
                context_p->sockfd = connection_fd;
                selector_register_callback_for_fd(connection_fd, handle_data_from_connection, context_p);
                ATC_LOG_I("Accepted incoming socket connection, fd=%d.", connection_fd);
            } else {
                ATC_LOG_E("Not enough parser states in system!!!");
            }
        } else {
            ATC_LOG_E("Not enough at contexts in system!!!");
        }
    }

    return true;
}

/*--------------------------------------------------------------
 *
 *
 * -------------------------------------------------------------*/
static bool socket_init()
{
    bool res = false;
    int fd;

    fd = atc_socket_create(ATC_SOCKET_PATH);

    if (fd >= 0) {
        res = selector_register_callback_for_fd(fd, atc_connect_internal, NULL);
    }

    return res;
}

enum {
    atc_startup_mode_normal,
    atc_startup_mode_debug,
    atc_startup_mode_internal
};


/*--------------------------------------------------------------
 *
 *
 * -------------------------------------------------------------*/
int atc_init_serial_device(const char *devname)
{
    int num = -1;
    size_t length;

    /* Check device name for min and max length, and "tty" prefix */
    length = strlen(devname);

    if (4 > length || SERIAL_DEVICE_MAX_LENGTH < length || 0 != strncmp("tty", devname, 3)) {
        ATC_LOG_E("Malformed device name: \"%s\"!", devname);

    } else {
        /* Check if the device is already in the list, if so no new entry is needed */
        for (num = 0; num < serial_device_config_num; num++) {
            if (0 == strcmp(serial_device[num].name, devname)) {
                ATC_LOG_W("Serial device \"%s\" is already configured!",
                          devname);
                return num;
            }
        }

        /* Check if there is room for one more device */
        if (SERIAL_DEVICE_MAX_NUMBER > serial_device_config_num) {
            num = serial_device_config_num++;
            serial_device[num].fd = -1;
            strncpy(serial_device[num].name, devname, sizeof(serial_device[num].name));
            serial_device[num].baud = B115200;
        } else {
            ATC_LOG_E("Serial device table is exhausted! Support limited to %d serial ports.",
                      SERIAL_DEVICE_MAX_NUMBER);
        }
    }

    return num;
}

bool atc_set_serial_device_baud(const int num, const long baudrate)
{
    long baud;

    if (0 > num || !(serial_device_config_num > num)) {
        ATC_LOG_E("Invalid serial device table slot: %d!", num);
        return false;
    }

    switch (baudrate) {
    case 96:
    case 9600:
        baud = B9600;
        break;
    case 192:
    case 19200:
        baud = B19200;
        break;
    case 384:
    case 38400:
        baud = B38400;
        break;
    case 576:
    case 57600:
        baud = B57600;
        break;
    case 1152:
    case 115200:
        baud = B115200;
        break;
    case 15000:
    case 1500000:
        baud = B1500000;
        break;
    case 20000:
    case 2000000:
        baud = B2000000;
        break;
    case 25000:
    case 2500000:
        baud = B2500000;
        break;
    default:
        ATC_LOG_E("Serial device \"%s\" invalid baud rate: %ld!",
                  serial_device[num].name, baudrate);
        return false;
    }

    serial_device[num].baud = baud;
    ATC_LOG_I("Serial device \"%s\" using baud rate : %ld",
              serial_device[num].name, baudrate);

    return true;
}


/*--------------------------------------------------------------
 * Configure serial port
 * Set speed, input and output processing.
 *
 * Configurable options:
 * echo - Turn character echo off or on
 * veol - End-of-line character, normally CR
 * verase - Backspace character
 * -------------------------------------------------------------*/
bool atc_configure_serial(anchor_connection_t *conn_p, bool echo, char veol, char verase)
{
    struct termios tio;

    ATC_LOG_I("port=\"%s\", echo=%d, veol=%d, verase=%d",
              conn_p->device_name, echo, veol, verase);

    if (0 > conn_p->fdr) {
        ATC_LOG_E("port \"%s\" is closed, fd=%d!",
                  conn_p->device_name, conn_p->fdr);
        return false;
    }

    memset(&tio, 0, sizeof(tio));

    /*
    * CS8, 8bit,no parity,1 stopbit
    * CREAD, receiver enabled
    * CLOCAL, don't change the port's owner
    * HUPCL, change DTR on close, perform a disconnect
    */
    tio.c_cflag = conn_p->baud | CS8 | CREAD | CLOCAL | HUPCL;

    /*
    * ICANON, canonical input
    * IEXTEN, enable extended functions
    */
    tio.c_lflag = ICANON | IEXTEN;

    /*
    * ECHO, enable echo
    * ECHOE, echo ERASE as a destructive backspace
    * ECHOK, echo KILL by erasing the current line
    */
    if (echo) {
        tio.c_lflag |= ECHO | ECHOE | ECHOK;
    }

    /*
    * OPOST, processed output
    */
    tio.c_oflag = OPOST;

    /*
    * ICRNL, map CR to NL
    * IXON, enable software flow control (outgoing)
    */
    tio.c_iflag = IXON;

    /*
    * VERASE, Erase character. Requires ICANON.
    * Default: Ctrl+H a.k.a. Backspace
    */
    tio.c_cc[VERASE] = verase;

    /*
    * VEOL, End-of-line character. Requires ICANON.
    * Default: Ctrl+M a.k.a Carriage Return or CR
    */
    tio.c_cc[VEOL] = veol;

    /*
    * VEOL2, End-of-line character. Requires ICANON and IEXTEN.
    * Default: Ctrl+Z
    */
    tio.c_cc[VEOL2] = 26;

    /*
    * VKILL, Kill character, erase input. Requires ICANON.
    * Default: Ctrl+U
    */
    tio.c_cc[VKILL] = 21;

    /*
    * VSTART, X-ON character, restart output. Requires IXON.
    * Default: Ctrl+Q
    */
    tio.c_cc[VSTART] = 17;

    /*
    * VSTOP, X-OFF character, stop output. Requires IXON.
    * Default: Ctrl+S
    */
    tio.c_cc[VSTOP] = 19;

    if (-1 == tcsetattr(conn_p->fdr, TCSANOW, &tio)) {
        int errnum = errno;
        ATC_LOG_E("tcsetattr failed for \"%s\", (%s)",
                  conn_p->device_name, strerror(errnum));
        return false;
    }

    return true;
}

/*--------------------------------------------------------------
 *  Parse start arguments
 *
 * -------------------------------------------------------------*/
static int parse_options(int argc, char **argv)
{
    int i;
    int serial_device_num = -1;
    int mode = atc_startup_mode_normal;     /* Default startup mode */

    for (i = 1; i < argc ;) {
        if (0 == strcmp(argv[i], "-mode") && (argc - i > 1)) {
            if (0 == strcmp(argv[i + 1], "debug")) {
#ifdef CFG_ENABLE_DEBUG_MODE /* debug mode not supported by default for security reasons. */
                mode = atc_startup_mode_debug;
                ATC_LOG_I("Using startup mode debug");
#else
                ATC_LOG_W("Debug mode not supported");
#endif
            } else if (0 == strcmp(argv[i + 1], "internal")) {
                mode = atc_startup_mode_internal;
                ATC_LOG_I("Using startup mode internal");
            } else {
                ATC_LOG_I("Using startup mode normal");
            }

            i += 2;
        } else if (0 == strcmp(argv[i], "-serial") && (argc - i > 1)) {
            serial_device_num = atc_init_serial_device(argv[i + 1]);
            i += 2;
        } else if (0 == strcmp(argv[i], "-baud") && (argc - i > 1)) {
            if (0 > serial_device_num) {
                ATC_LOG_E("No serial port to apply the baud setting on!");
            } else {
                long baudrate = atol(argv[i + 1]);
                atc_set_serial_device_baud(serial_device_num, baudrate);
            }

            i += 2;
        } else if (0 == strcmp(argv[i], "normal")) {
            ATC_LOG_I("Using normal *** Comply to old versions use -mode normal or default");
            i++;
        } else {
            ATC_LOG_E("Usage: %s [-mode modeopt] [-serial devicename] [-baud rate] [-log level]\n"
                      "\tmodeopt : normal | debug | internal, default is normal\n"
                      "\tdefault no serial device\n"
                      "\tdefault rate 115200\n"
                      "\tlevel : error | info | debug, default is info\n"
                      "\told normal argument is ignored\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    return mode;
}

static int atc_create_netlink_socket()
{
    struct sockaddr_nl src_addr;

    netlink_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_KOBJECT_UEVENT);

    if (netlink_fd < 0) {
        ATC_LOG_I("Failed to create Netlink socket!");
        goto error;
    }

    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid();
    src_addr.nl_groups = 1;

    if (bind(netlink_fd, (struct sockaddr *)&src_addr, sizeof(src_addr)) < 0) {
        ATC_LOG_I("Failed to bind netlink socket\n");
        goto error;
    }

    selector_register_callback_for_fd(netlink_fd, atc_kernel_uevent_handler, NULL);

    ATC_LOG_I("Created netlink socket: %d and added to selector \n", netlink_fd);

    return netlink_fd;

error:
    return -1;

}

static bool atc_kernel_uevent_handler(int netlink_fd, void *data_p)
{
    static char buffer[HOTPLUG_BUFFER_SIZE + OBJECT_SIZE];
    static char object[OBJECT_SIZE];
    const char *devpath;
    const char *state;
    const char *action;
    const char *envp[HOTPLUG_NUM_ENVP];
    int i;
    char *pos;
    size_t bufpos;
    ssize_t buflen;

    buflen = recv(netlink_fd, &buffer, sizeof(buffer), 0);

    if (buflen <  0) {
        ATC_LOG_E("error receiving uevent message\n");
        goto exit;
    }

    if ((size_t)buflen > sizeof(buffer) - 1) {
        buflen = sizeof(buffer) - 1;
    }

    buffer[buflen] = '\0';

    /* save start of payload */
    bufpos = strlen(buffer) + 1;

    /* action string */
    action = buffer;
    pos = strchr(buffer, '@');

    if (!pos) {
        goto exit;
    }

    pos[0] = '\0';

    /* sysfs path */
    devpath = &pos[1];

    /* check if the uevent is for USB */
    if (strcmp(devpath, "/devices/virtual/android_usb/android0")) {
        ATC_LOG_I("uevent not related to USB");
        goto exit;
    }

    /* hotplug events have the environment attached - reconstruct envp[] */
    for (i = 0; (bufpos < (size_t)buflen) && (i < HOTPLUG_NUM_ENVP - 1); i++) {
        int keylen;
        char *key;

        key = &buffer[bufpos];
        keylen = strlen(key);
        envp[i] = key;
        bufpos += keylen + 1;
    }

    envp[i] = NULL;

    /* print payload environment */
    for (i = 0; envp[i] != NULL; i++) {
        if (!strncmp(envp[i], "USB_STATE", 9)) {
            pos = strchr(envp[i], '=');

            if (!pos) {
                goto exit;
            }

            pos[0] = '\0';
            state = &pos[1];

            if (!strcmp(state, "DISCONNECTED")) {
                is_usb_connected = FALSE;

                if (NULL != ttyGS0_ctx) {
                    if (!atc_disconnect(ttyGS0_ctx)) {
                        ATC_LOG_E("External AT command channel (%d) disconnect ERROR", ttyGS0_fd);
                    }
                }
            } else if (!strcmp(state, "CONFIGURED")) {
                is_usb_connected = TRUE;

                /* Use the name to identify the USB serial device that needs to be reconnected */
                for (i = 0; i < serial_device_config_num; i++) {
                    if (!strcmp(serial_device[i].name, "ttyGS0")) {
                        serial_device[i].fd = anc_connect(serial_device[i].name, serial_device[i].baud);
                        break;
                    }
                }
            }

            is_usb_connected ? ATC_LOG_I("USB is Configured") : ATC_LOG_I("USB is Disconnected");
        }
    }

exit:
    return true;
}

/*--------------------------------------------------------------
 *
 *
 * -------------------------------------------------------------*/
static int atc_check_usb_status(void)
{
  FILE *fp = NULL;
  char state[USB_STATE_LEN];
  int usb_status = 0;
  fp = fopen(USB_STATE_FILE, "r");
  if (NULL != fp) {
    memset(state,0,USB_STATE_LEN);
    fgets(state, USB_STATE_LEN, fp);
    ATC_LOG_I("USB State read from file: %s",state);
    if (NULL != strstr(state,"CONFIGURED")) {
      usb_status = 0;
    } else {
      usb_status = -1;
    }
    fclose(fp);
  } else {
    ATC_LOG_E("Failed to open usb state file");
    usb_status = -1;
  }
  return usb_status;
}


/*--------------------------------------------------------------
 *
 *
 * -------------------------------------------------------------*/
int main(int argc, char **argv)
{
    int i;
    exe_t *exe_p;
    int startup_mode;

    ATC_LOG_I("%s started", argv[0]);

    /*Ingore SIGPIPE and avoid crash */
    if (signal((int)SIGPIPE, SIG_IGN) == SIG_ERR) {
        ATC_LOG_E("AT Core: SIG_ERR when ignoring SIGPIPE\n");
        abort();
    }

    /* Set umask before anything else */
    util_set_restricted_umask();

#ifdef AT_SERVICE
    at_service_start = time(NULL);
#endif
    startup_mode = parse_options(argc, argv);
    selector_init();
    atc_log_init();
    anc_init();

    if (atc_startup_mode_debug == startup_mode) {
        int debug_fd = at_debug_init();

        if (debug_fd < 0) {
            ATC_LOG_E("FATAL ERROR IN CALL TO at_debug_init!");
            goto error;
        }

        selector_register_callback_for_fd(debug_fd, atc_callback_for_debug_connection, NULL);
    }

    atc_context_init_all_contexts();
    init_parser_states();

    /* Create and initialize the executor (return when all needed services
     * are available or return error after timeout). */
    exe_p = exe_create();

    if (!exe_p) {
        ATC_LOG_E("FATAL ERROR IN CALL TO exe_create!");
        goto error;
    }

    atc_setup_exe_glue(exe_p);

    /* Connect all serial ports if USB is connected/configured */
    if ((!atc_check_usb_status()) && (atc_startup_mode_normal == startup_mode)) {
      for (i = 0; i < serial_device_config_num; i++) {
         serial_device[i].fd = anc_connect(serial_device[i].name, serial_device[i].baud);
      }
    }


    /* Create a netlink socket */
    if (atc_create_netlink_socket() < 0) {
        ATC_LOG_I("Failed to create Netlink socket!");
        goto error;
    }

    if (!socket_init()) {
        ATC_LOG_E("FATAL ERROR IN CALL TO socket_init!");
        goto error;
    }

    /* The current process privilege is only required for AT initialization.
     * Throw away privilege and become a normal user (the current group
     * privilege stays as it is). */
    util_continue_as_non_privileged_user();

    /* Loop here until signal to stop is received */
    do {
        selector_loop(atc_default_callback, NULL);
    } while (!stop);

    /* Destroy the executer */
    exe_destroy(exe_p);

    atc_log_close();
    return EXIT_SUCCESS;

error:
    atc_log_close();
    return EXIT_FAILURE;
}
