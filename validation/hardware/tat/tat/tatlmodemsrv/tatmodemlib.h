/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides services for power services module
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef TATMODEMLIB_H_
#define TATMODEMLIB_H_

#include <stdlib.h>
#include <stdint.h>
#include <errno.h>

#include "type_def_wm_isi_if.h"
#include "isi_common_lib.h"

#define TAT_MODEM_OWNER_LEN     63

/* possible value for operator of marshalling / unmarshalling operations */
typedef enum {
    ISI_MARSH,      /* marshaling */
    ISI_UNMARSH     /* unmarshaling */
} TAT_MODEM_ISI_MARSH;

static char *modem_modes[];

/* operating modes */
enum {
    /* nominal modes */
    TAT_MODEM_MODE_LOCAL,   /* test mode */
    TAT_MODEM_MODE_NORMAL,  /* normal mode */

    /* technical modes */
    TAT_MODEM_MODE_UNKNOWN, /* initial */
    TAT_MODEM_MODE_RESET,   /* after change mode request (need reboot) */
    TAT_MODEM_MODE_ERROR    /* problem with drivers */
};

/* general status */
enum {
#if defined(CONFIG_PHONET)
    TAT_MODEM_OFFLINE,  /* nominal: modem link closed */
    TAT_MODEM_ONLINE    /* nominal: modem link established */
#else
    TAT_MODEM_DISCONNECTED, /* nominal: modem link closed */
    TAT_MODEM_CONNECTED     /* nominal: modem link established */
#endif
};

typedef uint32_t    rsrcid_t;

/**
 * Set the library logging info level.
 * @param level [in] one of the syslog priority value (LOG_INFO, etc)
 */
void tat_modem_set_log(int level);

/* this function prints an ISI message in a FastTrace way */
void tat_modem_print_isi(uint8_t *bytes, size_t size, int prio);

/**
 * Encode or decode a byte from an ISI perspective.
 *
 * @param val    [in/out] non null pointer to a byte.
 * @param op     [in] ISI_MARSH to encode or ISI_UNMARSH to decode the byte.
 *
 * @return a pointer to the next byte after \a val.
 */
uint8_t *tat_modem_marsh8(uint8_t *val, TAT_MODEM_ISI_MARSH op);

/**
 * Encode or decode a word from an ISI perspective.
 *
 * @param val    [in/out] non null pointer to a word.
 * @param op     [in] ISI_MARSH to encode or ISI_UNMARSH to decode the word.
 *
 * @return a pointer to the next word after \a val.
 */
uint16_t *tat_modem_marsh16(uint16_t *val, TAT_MODEM_ISI_MARSH op);

/**
 * Encode or decode a dword from an ISI perspective.
 *
 * @param val    [in/out] non null pointer to a dword.
 * @param op     [in] ISI_MARSH to encode or ISI_UNMARSH to decode the dword.
 *
 * @return a pointer to the next dword after \a val.
 */
uint32_t *tat_modem_marsh32(uint32_t *val, TAT_MODEM_ISI_MARSH op);

/* macros for backwards compatibility */
#define IsiMarsh8       tat_modem_marsh8
#define IsiMarsh16      tat_modem_marsh16
#define IsiMarsh32      tat_modem_marsh32

/* return the current owner of the modem link, if defined.
 * the function does not tell if \a buf_size is not wide enough to receive the
 * entire owner name. owner max length is TAT_MODEM_OWNER_LEN.
 *
 * @param buf       [out] non null address to a string buffer
 * @param buf_size  [in] size in bytes of \a buf
 *
 * @return if no message has been sent or read after the modem link was opened,
 * meaning the owner is not set yet, then 0 is returned  and \a buf is set to
 * an empty string. If the owner is defined then \a buf contains it's name and
 * the function returns the number of bytes in buf.
 */
int tat_modem_get_owner(char *buf);

/* modem mode sources */
enum {
    TAT_MODEM_SRCE_LIB,
    TAT_MODEM_SRCE_MODEM,
    TAT_MODEM_SRCE_AUTO
};

/*
 * get modem operating mode from the program or the modem itself.
 *
 * @param srce    [in] a mode source, one of these:
 *                TAT_MODEM_SRCE_LIB: read the value from the library.
 *                TAT_MODEM_SRCE_MODEM: ask the modem its operating mode.
 *                The library internal state is also updated. The modem
 *                interface is activated if not done yet. Isi link is then
 *                opened.
 *				  TAT_MODEM_SRCE_AUTO: same as TAT_MODEM_SRCE_MODEM if the
 *                library modem mode is not set yet else TAT_MODEM_SRCE_LIB.
 *
 * @param mode    [out] non null pointer to int: returned operating mode
 * @return if the function succeeds, 0 is returned and mode is set. If the
 * function fails then it returns an errno code and mode is unchanged.
 */
int tat_modem_get_mode(int srce, int *mode);

/*
 * This function is reserved for TATLMODEM purpose.
 * change modem operating mode
 *
 * @param mode  [in] new nominal operating mode. Nominal modes are
 * TAT_MODEM_MODE_LOCAL and TAT_MODEM_MODE_NORMAL.
 *
 * @return 0 if the function succeeds
 * @return ECONNREFUSED if a driver error occurred
 * @return EBADRQC if mode is not valid
 * @return ENOEXEC if already in the specified mode
 */
int tat_modem_set_mode(int mode);

int tat_modem_get_state(void);

/*
 * This function is reserved for TATLMODEM purpose.
 * open modem connection if not already opened
 *
 * @return 0 if the function succeeds
 * @return ECONNREFUSED if an error occurred when opening the modem link
 * @return ECOMM if the library is not initialized
 * @return ENOEXEC if the modem connection is already established
 * @return EINVAL if a general error occurred
 */
int tat_modem_open(void);

/*
 * This function is reserved for TATLMODEM purpose.
 * close modem connection if not closed
 *
 * @return 0 if the function succeeds
 * @return ECONNREFUSED if a driver error occurred while closing the modem link
 * @return ECOMM if the library is not initialized
 * @return ENOEXEC if the modem connection is already closed
 */
int tat_modem_close(void);

/**
 * Send an ISI request to modem if allowed to do so, setting ownership to
 * feature if accurate. The function is blocking until the response is received
 * or an error occurs. If the feature is not allowed to do I/O access to modem,
 * the function fails.  The response is received if its resourceID is the same
 * as the request one.
 *
 * @param feature   [in] this string identifies the feature requesting I/O
 *                  operation for the modem.
 * @param msg       [in] address of a well-formated ISI request
 * @param msg_len   [in] length of msg in bytes
 * @param rsc_id    [out] filled with the request resource ID if success
 *
 * @retval 0 if the function succeeds
 * @retval ECOMM if fails to send the request or EPROTO if another ISI protocol
 * error occured.
 */
#if defined(CONFIG_PHONET)
int tat_modem_send(char *feature, void* msg, size_t msg_len, rsrcid_t *rsc_id);
#else
int tat_modem_send(char *feature, void* msg, size_t msg_len, uint32_t *rsc_id);
#endif


/**
 * Wait until an ISI response having reource ID of rsc_id is received and then
 * read it. The function is blocking until the response is received or an error
 * occurs. If the feature is not allowed to do I/O access to modem, the function
 * fails. Any ISI message received, which is not the response expected, is
 * processed by modem default handler
 *
 * @param feature   [in] this string identifies the feature requesting I/O
 *                  operation for the modem
 * @param rsc_id    [in] expected resource id. Should be the one returned by
 *                  tat_modem_send()
 * @param resp      [out] address a buffer to receive the ISI response or null
 * @param resp_len  [in/out] address of an int giving \a resp capacity in bytes.
 *                  The parameter is ignored if \a resp is null. The parameter is
 *                  set to the number of bytes read after the function call.
 *
 * @retval 0 if the function succeeds
 * @retval EOVERFLOW if \a resp is not long enough to hold the response. In that
 * case, \a resp_len contains the length required in bytes.
 * @retval EPROTO if an ISI protocol error occured.
 */
#if defined(CONFIG_PHONET)
int tat_modem_read(char *feature, rsrcid_t rsc_id, void* resp, size_t *resp_len);
#else
int tat_modem_read(char *feature, uint32_t rsc_id, void* resp, size_t *resp_len);
#endif

#endif /* TATL00MODEMSERVICES_H_ */
