/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * Author: Szymon Bigos <szymon.bigos@tieto.com> for ST-Ericsson
 *         Tomasz Hliwiak <tomasz.hliwiak@tieto.com> for ST-Ericsson
 *
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef MODEM_UTILS_H
#define MODEM_UTILS_H

/* Maximum modem response time in seconds. */
#define MODEM_RESP_TIMEOUT 5

/* Opens connection to modem.
 *
 * Returns:
 *  modem connection handler
 *  error status < 0 if failed */
int modem_connect();

/* Close connection to modem.
 *
 * In:
 *  handler - modem connection handler.
 *
 * Returns:
 *  modem connection handler
 *  error status < 0 if failed */
int modem_disconnect(int handler);

#ifdef TEST_AT
/* Sends AT command to modem. To get command response read_AT_resp()
 * needs to be used.
 *
 * In:
 *  handler - modem connection handler
 *  cmd - AT command to send
 *  len - AT command length
 *
 * Returns:
 *  length of writen data. */
int write_AT_req(int handler, char *cmd, size_t len);

/* Reads response for AT command. Waits for response for AT_RESP_TIMEOUT.
 * Response is writen into buf (it must be allocated), but no more then
 * len bytes.
 *
 * In:
 *  handler - modem connection handler
 *  buf - buf for AT command response
 *  len - buf length
 *
 * Returns:
 *  length of bytes writen into buf
 *  error status  < 0 if failed */
int read_AT_resp(int handler, char *buf, size_t len);
#endif /* TEST_AT */

/* Sends APDU command to modem. To get command response read_APDU_resp()
 * needs to be used.
 *
 * In:
 *  handler - modem connection handler
 *  cmd - APDU command to send
 *  len - APDU command length
 *
 * Returns:
 *  length of writen data. */
int write_APDU_req(int handler, char *cmd, size_t len);

/* Reads response for APDU command. Waits for response for MODEM_RESP_TIMEOUT.
 * Response is writen into buf (it must be allocated), but no more then
 * len bytes.
 *
 * In:
 *  handler - modem connection handler
 *  buf - buf for APDU command response
 *  len - buf length
 *
 * Returns:
 *  length of bytes writen into buf
 *  error status  < 0 if failed */
int read_APDU_resp(int handler, char *buf, size_t len);

#endif /* MODEM_UTILS_H */
