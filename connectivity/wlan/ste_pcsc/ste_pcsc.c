/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * Author: Szymon Bigos <szymon.bigos@tieto.com> for ST-Ericsson
 *         Tomasz Hliwiak <tomasz.hliwiak@tieto.com> for ST-Ericsson
 *
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include "ste_pcsc.h"
#include "modem_utils.h"
#include "ste_pcsc_utils.h"
#include <string.h>

#define PCSC_BUF_SIZE 4096

// Application context. Currently only 1 supported.
long ctx = 0;
// modem connection handler. Currently only 1 supported.
int handler = 0;

//TODO: semaphore
int transaction_ongoing = 0;

/**
 * SCardEstablishContext - creates application context for PCSC.
 *
 * @scope: Scope of established context.
 * @reserved_1: Reserved value. Ignored.
 * @reserved_2: Reserved value. Ignored.
 * @context: Returned context.
 *
 * Returns: SCARD_S_SUCCESS on success or other value on failure
 */
long SCardEstablishContext(unsigned long scope, const void *reserved_1,
		const void *reserved_2, long *context)
{
	if (NULL == context) {
		return SCARD_E_INVALID_PARAMETER;
	}

	if ((SCARD_SCOPE_SYSTEM != scope) &&
		(SCARD_SCOPE_USER != scope) &&
		(SCARD_SCOPE_TERMINAL != scope) &&
		(SCARD_SCOPE_GLOBAL != scope)) {
		return SCARD_E_INVALID_VALUE;
	}

	// Check for free context.
	if (ctx) {
		*context = 0;
		return SCARD_E_NO_MEMORY;
	}
	else {
		//TODO: Random
		ctx = 1;
		*context = 1;
	}

	return SCARD_S_SUCCESS;
}

/**
 * SCardReleaseContext - destroys given application context.
 *
 * @context: Context to release.
 *
 * Returns: SCARD_S_SUCCESS on success or other value on failure
 */
long SCardReleaseContext(long context)
{
	if (ctx != context) {
		return SCARD_E_INVALID_HANDLE;
	}

	ctx = 0;

	// Disconnect from modem if required.
	if (handler)
	{
		modem_disconnect(handler);
		handler = 0;
	}

	return SCARD_S_SUCCESS;
}

/**
 * SCardListReaders - Lists available readers. If groups and readers are NULL,
 * readers_len contains length required to store readers names.
 *
 * @context: Application context.
 * @groups: Readers group. Ignored.
 * @readers: Returned reader names.
 * @readers_len: Length of returned readers names or max available readers buff.
 *
 * Returns: SCARD_S_SUCCESS on success or other value on failure
 */
long SCardListReaders (long context, const char *groups,
		char *readers, unsigned long *readers_len)
{
	// Check context.
	if (ctx != context) {
		return SCARD_E_INVALID_HANDLE;
	}

	if ((NULL == readers_len) && (readers))
	{
		return SCARD_E_NO_MEMORY;
	}
	if ((readers) && (readers_len))
	{
		if (*readers_len <  (strlen(ADAPTER_NAME)))
		{
			return SCARD_E_INSUFFICIENT_BUFFER;
		}
		else
		{
			strncpy(readers,ADAPTER_NAME,*readers_len);
		}
	}
	else if ((NULL == readers) && (NULL == groups) && (NULL != readers_len))
	{
		*readers_len = strlen(ADAPTER_NAME) + 1;
	}

	return SCARD_S_SUCCESS;
}

/**
 * SCardConnect - Establish connection to given reader.
 *
 * @context: Application context.
 * @reader: Reader name.
 * @mode: Reader share mode. Ignored.
 * @pref_proto: Preffered protocol to use. Ignored.
 * @card: Returned card handler.
 * @active_proto: Used protocol. Always T0.
 *
 * Returns: SCARD_S_SUCCESS on success or other value on failure
 */
long SCardConnect(long context, const char *reader, unsigned long mode,
		unsigned long pref_proto, long *card, unsigned long *active_proto)
{
	int ret;

	// Check context.
	if (ctx != context) {
		return SCARD_E_INVALID_HANDLE;
	}

	if ((NULL == card) || (NULL == reader) || (NULL == active_proto))
	{
		return SCARD_E_INVALID_PARAMETER;
	}

	if (strcmp(ADAPTER_NAME, reader))
	{
		return SCARD_E_UNKNOWN_READER;
	}

	ret = modem_connect();
	if (ret > 0)
	{
		handler = ret;
		*card = ret;
		*active_proto = SCARD_PROTOCOL_T0;
	}
	else
	{
		//connect error
		return SCARD_F_COMM_ERROR;
	}

	return SCARD_S_SUCCESS;
}

/**
 * SCardDisconnect - Terminate connection to given reader.
 *
 * @card: Card handler.
 * @disposition: Reader action. Ignored.
 *
 * Returns: SCARD_S_SUCCESS on success or other value on failure
 */
long SCardDisconnect(long card, unsigned long disposition)
{
	int ret;

	if ((handler) && (handler == card))
	{
		ret = modem_disconnect(card);
		handler = 0;
	}
	else
	{
		return SCARD_E_INVALID_HANDLE;
	}

	return SCARD_S_SUCCESS;
}

/**
 * SCardBeginTransaction - Begins card transaction.
 *
 * @card: Card handler.
 *
 * Returns: SCARD_S_SUCCESS on success or other value on failure
 */
long SCardBeginTransaction(long card)
{
	if (handler != card)
	{
		return SCARD_E_INVALID_HANDLE;
	}

	if (transaction_ongoing)
	{
		return SCARD_E_SHARING_VIOLATION;
	}
	else
	{
		transaction_ongoing = card;
	}
	return SCARD_S_SUCCESS;
}

/**
 * SCardEndTransaction - Ends card transaction.
 *
 * @card: Card handler.
 * @disposition: Reader action. Ignored.
 *
 * Returns: SCARD_S_SUCCESS on success or other value on failure
 */
long SCardEndTransaction(long card, unsigned long disposition)
{
	if (handler != card)
	{
		return SCARD_E_INVALID_HANDLE;
	}
	else
	{
		transaction_ongoing = 0;
	}

	return SCARD_S_SUCCESS;
}

/**
 * SCardTransmit - Transmits APDU frame to card.
 *
 * @card: Card handler.
 * @in_io_mode: Input IO mode. Ignored.
 * @in_buff: Frame to send.
 * @in_len: in_buff length.
 * @out_io_mode: Output IO mode. Ignored.
 * @out_buff: Buffer for response.
 * @out_len: Max out_buff length.
 *
 * Returns: SCARD_S_SUCCESS on success or other value on failure
 */
long SCardTransmit(long card, const SCARD_IO_REQUEST *in_io_mode,
		const unsigned char *in_buff, unsigned long in_len, SCARD_IO_REQUEST *out_io_mode,
		unsigned char *out_buff, unsigned long *out_len)
{
	unsigned int i;
	int len,ret;
	char buf[PCSC_BUF_SIZE];

	if (card && card != handler)
	{
		return SCARD_E_INVALID_HANDLE;
	}

	// Check if someone else didn't start transaction.
	if (transaction_ongoing && transaction_ongoing != card)
	{
		return SCARD_E_NOT_TRANSACTED;
	}

	if ((NULL == in_buff) || (NULL == out_buff) || (NULL == out_len))
	{
		return SCARD_E_INVALID_PARAMETER;
	}

	if (in_len <= 0)
	{
		return SCARD_E_INSUFFICIENT_BUFFER;
	}
	else
	{
		//2 bytes in string(hex) for each APDU byte + EOL
		len = (sizeof(char) * 2 * in_len);
	}

	for (i = 0; i < in_len; i++)
	{
		/* Write at most 2 bytes + EOL. Cast to unsigned char to prevent
		promoting to signed char and eventualy int. */
		snprintf(buf+(i*2), 3, "%02x", (unsigned char) in_buff[i]);
	}
	buf[len] = '\0';

	ret = write_APDU_req(handler, buf, len);
	if (ret <= 0)
	{
		return SCARD_E_NOT_TRANSACTED;
	}

	// Read response.
	ret = read_APDU_resp(handler, buf, PCSC_BUF_SIZE);
	if (ret > 0)
	{
		//Check if receive buffer is big enough. Byte string needs to be
		//converted to byte array, so it can be twice smaller
		if (*out_len < ((unsigned int) ret / 2))
		{
			return SCARD_E_INSUFFICIENT_BUFFER + 1;
		}

		for (i = 0; i < (unsigned int) ret; i = i + 2)
		{
			unsigned char a,b;
			a = strToHex(buf[i]);
			b = strToHex(buf[i + 1]);
			if ((a == NOT_HEX_VALUE) || (b == NOT_HEX_VALUE))
			{
				return SCARD_F_COMM_ERROR;
			}
			//Prepare most significant bits
			a = (a << 4) & 0xF0;
			//Prepare less significant bits
			b = b & 0x0F;
			out_buff[i / 2] = a | b;
		}
		*out_len = ret / 2;
	}
	else
	{
		return SCARD_E_NOT_TRANSACTED;
	}

	return SCARD_S_SUCCESS;
}
