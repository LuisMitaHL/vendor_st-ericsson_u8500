/**
 * \file hdr/phonet_lib_api.h
 * \brief Application interface library for Phonet Library
 *
 * Copyright (C) ST-Ericsson 2009
 * \n Application interface library for Phonet Library
 * \n \author: B Sampath Kumar <sampath.kumar@stericsson.com>
 *
 * Application interface file for Phonet Library
 *
 * \n Revision History:
 * \n
 * \n 1.0 Updated with doxygen sytle
 * \n 0.9 Updated the phonet library de-init API.
 */

/** \defgroup  MAL_PHONET_LIB PHONET_LIB API
 *  \par
 *      This part describes the interface of Phonet library.
 */

#ifndef PHONET_LIB_API_H
#define PHONET_LIB_API_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef BSP_PLUS
#include "phonet_types.h"
#endif


#define MAX_CLIENTS     25  /** Each Client is identified by its resource ID */

    /** Resource Id's */
#define PN_SMS                          0x02  /** Short Message Services              */
#define PN_SS                           0x06  /** Supplementary Services              */
#define PN_COMMGR                       0x10  /** Indication subscription messages    */
#define PN_CSD                          0x16  /** Circuit Switched Data               */
#define PN_GPDS                         0x31  /** General Packet Radio System server  */
#define PN_GSS                          0x32  /** GSM Stack server                    */
#define PN_UICC                         0x8C  /** Universal Integrated Circuit Card   */
#define PN_AT_MODEM                     0x8E  /** Modem AT server                     */
#define PN_MODEM_LCS                    0x90  /** Modem LCS server                    */
#define PN_MODEM_TEST                   0x91  /** Modem Test server                   */
#define PN_MODEM_NVD                    0x92  /** Modem Non Volatile Data server      */
#define PN_MODEM_PERM                   0x93  /** Modem PERManent Data server         */
#define PN_RF_HAL                       0xB7  /** RF control and tuning server        */
#define PN_MODEM_MCE                    0xC2  /** Modem MCE server                    */
#define PN_MODEM_MON                    0xC3  /** Modem Monitor server                */
#define PN_MODEM_INFO                   0xC5  /** Modem Info Server                   */
#define PN_MODEM_NETWORK                0xC8  /** Modem NET server                    */
#define PN_MODEM_CALL                   0xC9  /** Modem CALL server                   */
#define PN_PIPE                         0xD9  /** Pipe messages                       */
#define PN_COMMON_RF_TEST               0xEC  /** Common RF test server               */
#define PN_WCDMA_RF_TEST                0xEE  /** WCDMA RF test server                */
#define PN_GSM_RF_TEST                  0xF1  /** GSM test server                     */

#define PN_MODEM                        0x60
#define PN_DEV_HOST                     0x00

    /**
     * \enum Response type of the ISI message received.
     *
     * \brief Response type of the ISI message indicating whether
     *        it is solicited/unsolicited.
     *
     */
    enum
    {
        SOLICITED_MSG,
        UNSOLICITED_MSG
    };

    /**
     * \enum Resource Id mapping
     *
     * \brief Resource Id's of clients are mapped to enum values.
     *
     */
    enum resIdIndex {
        SMS,
        SS,
        COMMGR,
        CSD,
        GPDS,
        GSS,
        UICC,
        AT_MODEM,
        MODEM_LCS,
        MODEM_TEST,
        MODEM_NVD,
        MODEM_PERM,
        RF_HAL,
        MODEM_MCE,
        MODEM_MON,
        MODEM_INFO,
        MODEM_NETWORK,
        MODEM_CALL,
        PIPE,
        COMMON_RF_TEST,
        WCDMA_RF_TEST,
        GSM_RF_TEST,
        RES_ID_NOT_SUPPORTED
    };

    /**
     * \enum Phonet lib operation mode
     *
     * \brief Configure Phonet in test mode to send/receive data from/to user space
     *        (instead of via TCP/IP network stack)
     *
     *       */
    enum {
        PHONET_LIB_NORMAL_MODE,
        PHONET_LIB_TEST_MODE
    };

    /**
     * \struct phonet_hdr
     * \brief Phonet/ISI Header
     *
     * Phonet/ISI header containing media type, receiver device id,
     * sender device id, resource id, length of data, receiver object id,
     * sender object id. All the message from or to modem contain this
     * header.
     */

    struct phonet_hdr {
        uint8_t media;
        uint8_t rdev;
        uint8_t sdev;
        uint8_t res;
        uint16_t length;
        uint8_t robj;
        uint8_t sobj;
    };

    /**
     * \fn int32_t recvFromSocket(int32_t fd, unsigned char *buf , struct sockaddr_pn *dst, int32_t maxLen)
     * \brief [API] Receives data over a phonet socket I/F.
     *
     * Receives data over the phonet socket from a specified destination address.
     * It is an asynchronous call.
     *
     * \param fd      Socket file descriptor, over which the data is supposed
     *            to be read.
     * \param buf     Buffer to be filled with the data received.
     * \param dst     Destination address from which the data is to be received
     * \param maxLen  Maxse length of the data that is to be read.
     *
     * \return       In case of an error, returns a error number else
     *           returns the length of the data received.
     */
    int32_t recvFromSocket(int32_t fd, unsigned char *buf , struct sockaddr_pn *dst, int32_t maxLen);

    /**
     * \fn int32_t get_phonet_ifname(char *ifname, uint8_t pipe_handle)
     * \brief [API] Get the Phonet & TCP/IP Stack I/F name.
     *
     * Invokes a standard socket operations call, to fetch the phonet &
     * TCP/IP Stack Interface name.
     *
     * \param ifname   Buffer to be filled with the I/F name.
     * \param pipe_handle Pipe Handle is associated with the interface name.
     *
     * \return         Returns failure or success.
     */
    int32_t get_phonet_ifname(char *ifname, uint8_t pipe_handle);

    /**
     * \fn int32_t sendOnSocket(int32_t fd, void *data, uint32_t len)
     * \brief [API] Sends data over a phonet socket I/F.
     *
     * Sends the data over the socket to a specified destination address.
     * This function supports synchronous & asynchronous implementation
     * using a macro. In case of synchronous implementation, it blocks
     * on a condition until it receives a signal from receiver thread
     * indicating the occurance of a condition. Note that the data field
     * also includes the phonet header of 8 bytes.
     *
     * \param fd   Socket file descriptor, over which the data is supposed
     *         to be sent.
     * \param data The data/packet that is to be sent over the socket IF.
     * \param len  The length of the data that is to be sent.
     * \param dst           The destination address need to be filled by the respective
     *              MAL clients.
     *
     * \return      In case of an error, returns a error number else
     *      returns the length of the data that is sent.
     */
    int32_t sendOnSocket(int32_t fd, void *data, uint32_t len, struct sockaddr_pn *dst);

    /**
     * \fn int32_t estSockConn(struct sockaddr_pn *src)
     * \brief [API] Establishes a socket I/F to communicate with phonet stack.
     *
     * Creates a socket, binds the socket fd to the source address.
     *
     * \param src source address
     *
     * \return In case of an error, returns a error number else returns
     *     the DGRAM socket file descriptor.
     */
    int32_t estSockConn(struct sockaddr_pn *src);

    /**
     * \fn int32_t sockClose(int32_t fd)
     * \brief [API] Close a specified socket fd.
     *
     * Closes a phonet socket fd specified by the users
     *
     * \param fd   Socket fd to be closed
     *
     * \return Success or failure.
     */
    int32_t sockClose(int32_t fd);

    /**
     * \fn int32_t SeqSockClose(uint32_t pipe_handle)
     * \brief [API] Close a specified Sequence socket fd.
     *
     * Closes a phonet seq socket fd specified by the users
     * and also deletes the particular pdp node from the
     * available pdp list.
     *
     * \param pipe_handle   pipe handle associated with the
     * sequence socket fd to be closed.
     *
     * \return Success or failure.
     */
    int32_t SeqSockClose(uint8_t pipe_handle);

    /**
     * \fn int32_t deInitPhonetLib(int8_t flag)
     * \brief [API] Deinitialize the phonet library.
     *
     * Destroy the socket I/F and wait for the completion of threads
     *
     * \return None.
     */
    int32_t deInitPhonetLib(int8_t flag);

    /**
     * \fn int32_t get_phonet_pipefd(uint8_t pipe_handle)
     * \brief [API] Get the Phonet Pipe file Descriptor.
     *
     * Invokes a standard socket operations call, to fetch the phonet
     * stack pipe handle
     *
     * \param pipe_handle Pipe Handle is associated with the interface name
     *
     * \return         Returns Pipe file descriptor
     **/

    int32_t get_phonet_pipefd(uint8_t pipe_handle);

    /**
     * \fn void  set_phonet_opr_mode(uint32_t mode)
     * \brief [API]   Set Phonet lib operation flag to Normal or Test mode
     *
     * Confgure phonetlib in test mode to send/receive data from/to user space
     * (instead of via TCP/IP network stack)
     *
     * \param mode The operation mode to set PHONET_OPERATION_TEST_MODE or
     *              PHONET_OPERATION_NORMAL_MODE
     *
     * \return NONE
     *           * */
    void set_phonet_opr_mode(uint32_t mode);

    /**
     * \fn int32_t create_src_pep(uint32_t pipe_handle)
     * \brief [API] Create a source Pipe end Point.
     *
     * Create source pep by creating a sequence socket
     * whose object id is same as pipe handle.
     *
     * \param pipe_handle   pipe handle associated with
     * the PDP context.
     *
     * \return Success or failure.
     */
    int32_t create_src_pep(uint8_t pipe_handle);

    /**
     * \fn int32_t connect_pep(uint32_t pipe_handle, struct sockaddr_pn dst)
     * \brief [API] Connect source PEP and destination PEP.
     *
     * Connect source PEP and destination PEP and wait
     * in a timed loop until connect succeeds or timesout.
     *
     * \param pipe_handle   pipe handle associated with
     * the PDP context.
     *
     * \return Success or failure.
     */
    int32_t connect_pep(uint8_t pipe_handle, struct sockaddr_pn dst);

    /**
     * \fn int32_t enable_pipe(uint32_t pipe_handle)
     * \brief [API] Enable Pipe.
     *
     * Enable the pipe and wait in a timed loop until
     * enable succeeds or timesout.
     *
     * \param pipe_handle   pipe handle associated with
     * the PDP context.
     *
     * \return Success or failure.
     */
    int32_t enable_pipe(uint8_t pipe_handle);

#ifdef __cplusplus
}
#endif

#endif
