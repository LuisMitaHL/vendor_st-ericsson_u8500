/**
 * Copyright (C) ST-Ericsson 2009
 * Utility header file
 * Author: B Sampath Kumar <sampath.kumar@stericsson.com>
 */

/**
 * @file hdr/phonet_lib_misc.h
 * @brief Utility header file
 * @author B Sampath Kumar <sampath.kumar@stericsson.com>
 * @date 2009
 *
 * Utility header file for phonet library.
 */

#ifndef PHONET_LIB_MISC_H
#define PHONET_LIB_MISC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <linux/poll.h>
#include <linux/socket.h>
#include <linux/phonet.h>
#include <net/if.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <errno.h>
#include <math.h>
#include <string.h>
#include <stdint.h>

#ifdef BSP_PLUS
#include "phonet_types.h"
#endif

#define AF_PHONET   35
#define PF_PHONET   AF_PHONET

#define NL_BUFSIZE  8192
#define SOL_PNPIPE  275

#define SIZE        1024
#define MAX_RCV_LEN 4192

#define PN_COMMGR               0x10
#define PNS_PEP_CONNECT_REQ     0x40
#define PNS_PEP_DISCONNECT_RESP 0x43

#define PNS_PIPE_CREATED_IND    0x61
#define PNS_PIPE_RESET_IND      0x63
#define PNS_PIPE_ENABLED_IND    0x64
#define PNS_PIPE_REDIRECTED_IND 0x65
#define PNS_PIPE_DISABLED_IND   0x66

#define PN_MODEM_MCE_OBJ_ID     0x20
#define PN_PIPE_OBJ_ID          0x30

#define PTHREAD_SIGNAL()    dbg_printf(DBG_LEVEL4, "recvSeq Thread mutex lock!!\n"); \
                pthread_mutex_lock(&cMutex); \
                \
                dbg_printf(DBG_LEVEL4, "recvSeq Thread Signalling Unlock to Sender Thread!!\n"); \
                pthread_cond_signal(&cVar); \
                dbg_printf(DBG_LEVEL4, "recvSeq Thread Signalled Unlock to Sender Thread!!\n"); \
                \
                pthread_mutex_unlock(&cMutex); \
                dbg_printf(DBG_LEVEL4, "recvSeq Thread mutex unlock!!\n");

#define PTHREAD_WAIT()      dbg_printf(DBG_LEVEL4, "Sender Thread Waiting for SeqSocket Create!!\n"); \
                pthread_cond_wait(&cVar, &cMutex); \
                dbg_printf(DBG_LEVEL4, "Sender Thread got unlock signal: SeqSocket Created!!\n"); \
                \
                pthread_mutex_unlock(&cMutex);

#define SYNC_COND           (((char *)data)[3] != PN_COMMGR) && \
                (((char *)data)[9] != PNS_PIPE_CREATED_IND) && \
                (((char *)data)[9] != PNS_PIPE_RESET_IND) && \
                (((char *)data)[9] != PNS_PIPE_ENABLED_IND) && \
                (((char *)data)[9] != PNS_PIPE_DISABLED_IND) && \
                (((char *)data)[9] != PNS_PIPE_REDIRECTED_IND)

#define SYNC_LOCK()     if(SYNC_COND) { \
                    dbg_printf(DBG_LEVEL4, "Sender Thread mutex lock!!\n"); \
                    pthread_mutex_lock(&cMutex); \
                }

#define SYNC_WAIT()     if(SYNC_COND) { \
                    dbg_printf(DBG_LEVEL4, "Sender Thread Waiting for unlock signal!!\n"); \
                    pthread_cond_wait(&cVar, &cMutex); \
                    dbg_printf(DBG_LEVEL4, "Sender Thread got unlock signal!!\n"); \
                    pthread_mutex_unlock(&cMutex); \
                    dbg_printf(DBG_LEVEL4, "Sender Thread mutex unlock!!\n"); \
                }

#define SYNC_UNLOCK()    if(SYNC_COND) { \
                    pthread_mutex_unlock(&cMutex); \
                    dbg_printf(DBG_LEVEL4, "Sender Thread mutex unlock due to error!!\n"); \
                }
    enum {
        SET,
        ISSET
    };

    enum {
        PHONET_SUCCESS = 0,
        PHONET_FAILURE = -1
    };

    /** Netlink message structures */

    struct nlmsg {
        struct nlmsghdr nlMsg;
        struct ifaddrmsg addrMsg;
    };

    /** Parameters to receiver thread */
    struct rcv_thread_params {
        int32_t fd;
        struct sockaddr_pn dst;
    };

    int32_t create_pdp_list(void);
    int32_t send_isi_data(int32_t fd);
    int32_t create_gprs_interface(uint8_t pipe_handle);
    int32_t fill_pdp_list(uint8_t pipe_handle);
    int32_t create_seq_sock(uint8_t pipe_handle);
    int32_t get_seq_sock_by_pipe_hdl(uint8_t pipe_handle);

    void *recvAudio(void *addr);
    void set_thread_prio_create_seq_thread(uint8_t pipe_handle);
    void set_thread_attr(pthread_attr_t *attr, int32_t detachstate);
    void prepare_dst_msg(void *data, uint32_t len, struct sockaddr_pn *dst);
    void modify_thread_prio(pthread_attr_t *attr, int32_t prio, int32_t policy);

    /** Map resource Id to sequential index */
    int8_t get_res_by_index(uint8_t pn_res);
    /** Check if the response is solicited or unsolicited */
    char check_sol_unsol(unsigned char *buf, int32_t len);
    #if 0
    /** Send a netlink message */
    int32_t send_netlink_msg(int32_t fd);
    #endif
    /** Bind the socket to a particular address */
    int32_t bind_to_socket(int32_t fd, struct sockaddr_pn *src);
    /** Create socket */
    int32_t create_socket(int32_t *fd, int32_t sock_family, int32_t sock_type, int32_t protocol);
    /** Create receiver thread using POSIX library */
    int32_t create_recv_thread(struct sockaddr_pn src, pthread_attr_t attr);
    /** Log data to file in binary format */
    void log_data_to_file(FILE *fp, unsigned char *buf, uint32_t len);
    /** set or check if a particular fd is set from the bitmap */
    int32_t set_isset_fd_from_bitmap(int32_t flag);

    /**
     * Create/Receive data over a sequence socket I/F from a specified address (Today,
     * as we donot receive any data over sequence socket, it is used only to create
     * a Seq Socket I/F. In future (may be for CS Data Call - VT, if there is data
     * over this socket, then it needs to send a signal to the sender function
     * (the condition to occur).
     */
    void *recvOnSeqSocket(void *addr);

    /**
     * This API set the thread attributes to DETACHED and creates the receiver
     * thread which blocks on the socket fd until data is received.
     */
    int32_t createRecvThread(struct sockaddr_pn *src);

    /**
     * Receives data over a socket I/F from a specified address which
     * then sends a signal to the sender function after data is
     * received (the condition to occur). Once data is received, it
     * checks the res ID to invoke the respec. client callback handler.
     * The index of the callbacks is mapped to resource ID.
     * \return NONE
     */
    void *recvOnSocket(void *addr);

    /**
     * \typedef typedef int32_t (*clientCallback)(char *, char *, int32_t);
     * \brief [API] Client Callback handlers
     *
     * Call back handlers of each client needs to be registered at initialization.
     * Resource ID is the index for the registration of callback handlers.
     * Hence, each client will register its callback during init with resource ID
     * as the index for the array of function pointers.
     *
     * \param char *  Buffer which is filled with data received
     * \param char *  Filled with response type
     * \param int32_t Length of the data received
     *
     * \return Success or Failure
     *
     */
    typedef int32_t (*clientCallback)(char *, char *, int32_t);

    /**
     * Client libraries use this API to register the call back handlers
     * with phonet library. Once data is received, based on the resource
     * Id the respective callback handler is invoked.
     */
    uint8_t regClientCallbacks(uint8_t pn_res, clientCallback clientCb);

    /**
     * \fn  void set_fd_bitmap(uint32_t fdIdx)
     * \brief [API] Set the global bitmap index.
     *
     * When a socket is created, a global bitmap is set to identify the
     * index of the corresponding socket fd.
     *
     * \return None.
     */
    void set_fd_bitmap(uint32_t fdIdx);

#ifdef __cplusplus
}
#endif

#endif
