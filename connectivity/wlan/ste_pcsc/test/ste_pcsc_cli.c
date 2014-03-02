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
#include <modem_utils.h>
#include <string.h>
#include <stdlib.h>
#include "ste_pcsc.h"
#include "ste_pcsc_utils.h"

#define BUF_LEN 4096

int handler;

void usage(char *name)
{
	printf("Usage:\n");
	printf("%s [CMD] [ARGS]\n",name);
	printf("CMD = [--help, AT, APDU, PCSC]\n");
	printf("\t --help: print this message.\n");
	printf("\t at [ARG = cmd_string]: Sends AT command to modem and prints response.\n");
	printf("\t apdu [ARG = cmd_string]: Sends APDU command to modem and prints response.\n");
	printf("\t pcsc : Start interactive PCSC_lib CLI.\n");
	printf("\t PCSC cmd:\n");
	printf("\t\t EstablishContext\n");
	printf("\t\t ReleaseContext [context]\n");
	printf("\t\t ListReaders [context]\n");
	printf("\t\t Connect [context] [reader]\n");
	printf("\t\t Disconnect [card]\n");
	printf("\t\t BeginTransaction [card]\n");
	printf("\t\t EndTransaction [card]\n");
	printf("\t\t Transmit [card] [APDU message]\n");
	printf("\t\t quit\n");
	printf("\t\t help\n");
}

void local_connect()
{
	handler=modem_connect();

	if (handler < 0)
	{
		printf("Failed to open modem connection %d\n",handler);
		exit(0);
	}
	else
	{
		printf("Socket opened: %d\n",handler);
	}
}

void local_close()
{
	if (handler)
	{
		modem_disconnect(handler);
	}

	handler = 0;
}

void apdu(int argc, char *argv[])
{
	int i,ret;
	size_t len;
	char buf[BUF_LEN];
	buf[BUF_LEN-1] = '\n';

	local_connect();

	len = strlen(argv[2]);
	printf("APDU CMD: %s\n", argv[2]);
	ret = write_APDU_req(handler, argv[2], len);
	if (ret > 0)
	{
		printf("%d bytes sent to socket\n", ret);
	}
	else
	{
		printf("Failed to write data. Ret = %d\n", ret);
		local_close();
		exit(0);
	}

	ret = read_APDU_resp(handler, buf, BUF_LEN-1);
	if (ret > 0)
	{
		printf("APDU response received. Len = %d\n", ret);
		buf[ret]=0;
		for (i=0; i<ret; i++)
		{
			printf("%c",buf[i]);
		}
		printf("\n");
	}
	else
	{
		printf("Failed to read APDU response. Ret = %d\n",ret);
	}

	local_close();
	exit(0);
}

void at(int argc, char *argv[])
{
	int i,ret;
	size_t len;
	char buf[BUF_LEN];
	buf[BUF_LEN-1] = '\n';

	local_connect();

	len = strlen(argv[2]);
	printf("AT CMD: %s\n", argv[2]);
	ret = write_AT_req(handler, argv[2], len);
	if (ret > 0)
	{
		printf("%d bytes sent to socket\n", ret);
	}
	else
	{
		printf("Failed to write data. Ret = %d\n", ret);
		local_close();
		exit(0);
	}

	ret = read_AT_resp(handler, buf, BUF_LEN-1);
	if (ret > 0)
	{
		printf("At response received. Len = %d\n", ret);
		buf[ret]=0;
		for (i=0; i<ret; i++)
		{
			printf("%c",buf[i]);
		}
	}
	else
	{
		printf("Failed to read AT response. Ret = %d\n",ret);
	}

	local_close();
	exit(0);
}

void pcsc_establish_context()
{
	long context, ret;

	printf("PCSC CMD: SCardEstablishContext\n");

	ret = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &context);
	if (SCARD_S_SUCCESS == ret) {
		printf("Context = %ld\nOK\n", context);
	} else {
		printf("Error: %ld\n", ret);
	}
}

void pcsc_release_context()
{
	long context, ret;

	printf("PCSC CMD: SCardReleaseContext\n");

	//get context
	scanf("%ld", &context);
	ret = SCardReleaseContext(context);

	if (SCARD_S_SUCCESS == ret) {
		printf("OK\n");
	} else {
		printf("Error: %ld\n", ret);
	}
}

void pcsc_list_readers()
{
	long context, ret;
	char readers[4096];
	unsigned long readers_size = 4096;

	printf("PCSC CMD: SCardListReaders\n");

	//Get context
	scanf("%ld", &context);

	ret = SCardListReaders(context, NULL, readers, &readers_size);

	if (SCARD_S_SUCCESS == ret) {
		printf("Readers = %s\nOK\n", readers);
	} else {
		printf("Error: %ld\n", ret);
	}
}

void pcsc_connect()
{
	long context, card, ret;
	unsigned long protocol;
	char reader[4096];

	printf("PCSC CMD: SCardConnect\n");

	//Get context and reader name
	scanf("%ld", &context);
	scanf("%s", reader);

	ret = SCardConnect(context, reader, 0, SCARD_PROTOCOL_T0, &card,
			&protocol);

	if (SCARD_S_SUCCESS == ret) {
		printf("Card = %ld\nProtocol = %ld\nOK\n", card, protocol);
	} else {
		printf("Error: %ld\n", ret);
	}
}

void pcsc_disconnect()
{
	long card, ret;

	printf("PCSC CMD: SCardDisconnect\n");

	//Get card handler
	scanf("%ld", &card);

	ret = SCardDisconnect(card, 0);

	if (SCARD_S_SUCCESS == ret) {
		printf("OK\n");
	} else {
		printf("Error: %ld\n", ret);
	}
}

void pcsc_begin_transaction()
{
	long card, ret;

	printf("PCSC CMD: SCardBeginTransaction\n");

	//Get card handler
	scanf("%ld", &card);

	ret = SCardBeginTransaction(card);

	if (SCARD_S_SUCCESS == ret) {
		printf("OK\n");
	} else {
		printf("Error: %ld\n", ret);
	}
}

void pcsc_end_transaction()
{
	long card, ret;

	printf("PCSC CMD: SCardEndTransaction\n");

	//Get card handler
	scanf("%ld", &card);

	ret = SCardEndTransaction(card, 0);

	if (SCARD_S_SUCCESS == ret) {
		printf("OK\n");
	} else {
		printf("Error: %ld\n", ret);
	}
}

void pcsc_transimt()
{
	long card, ret;
	unsigned char send_buffer[4096];
	unsigned char rec_buffer[4096];
	unsigned long len = 4096;
	unsigned long len2;
	unsigned int i;
	unsigned char tmp[3];

	printf("PCSC CMD: SCardTransmit\n");

	scanf("%ld", &card);
	scanf("%s", send_buffer);

	len2 = strlen((char *) send_buffer);

	for(i = 0; i < len2 / 2; ++i) {
		unsigned char a,b;
		a = strToHex(send_buffer[i * 2]);
		b = strToHex(send_buffer[i * 2 + 1]);

		if ((a == NOT_HEX_VALUE) || (b == NOT_HEX_VALUE))
		{
			printf("Value not given in hex. Skip. [%s]\n", send_buffer);
			return;
		}
		send_buffer[i] = a << 4 | b;
	}

	ret = SCardTransmit(card, NULL,
			send_buffer, len2 / 2, NULL,
			rec_buffer, &len);

	if (0 > ret) {
		printf("SCardTransmit error %ld\n", ret);
		return;
	}

	rec_buffer[len] = '\0';

	printf("Received data (len=%ld):", len);

	for(i = 0; i < len; i++) {
		printf("%.2x", *(rec_buffer + i));
	}

	printf("\n");
}

void pcsc(int argc, char *argv[])
{
	char cmd[50];

	while (1) {
		scanf("%s", cmd);
		//SCardEstablishContext
		if (!strcmp(cmd, "EstablishContext")) {
			pcsc_establish_context();
		}
		//SCardReleaseContext
		else if (!strcmp(cmd, "ReleaseContext")) {
			pcsc_release_context();
		}
		//SCardListReaders
		else if (!strcmp(cmd, "ListReaders")) {
			pcsc_list_readers();
		}
		//SCardConnect
		else if (!strcmp(cmd, "Connect")) {
			pcsc_connect();
		}
		//SCardDisconnect
		else if (!strcmp(cmd, "Disconnect")) {
			pcsc_disconnect();
		}
		//SCardBeginTransaction
		else if (!strcmp(cmd, "BeginTransaction")) {
			pcsc_begin_transaction();
		}
		//SCardEndTransaction
		else if (!strcmp(cmd, "EndTransaction")) {
			pcsc_end_transaction();
		}
		//SCardTransmit
		else if (!strcmp(cmd, "Transmit")) {
			pcsc_transimt();
		}
		else if (!strcmp(cmd, "help")) {
			usage(argv[0]);
		}
		else if (!strcmp(cmd, "quit")) {
			local_close();
			return;
		}
		//Unknown command
		else {
			printf("Unknown PCSC command\n");
		}
	}
}

int main(int argc, char *argv[])
{
	if (argc > 1)
	{
		if (!strcmp(argv[1], "apdu"))
		{
			// sent APDU command
			apdu(argc, argv);
		}
		else if (!strcmp(argv[1], "at"))
		{
			at(argc, argv);
		}
		else if (!strcmp(argv[1], "pcsc"))
		{
			pcsc(argc, argv);
		}
		else if (!strcmp(argv[1], "--help"))
		{
			usage(argv[0]);
		}
		else
		{
			printf("Command unknown.\n");
			usage(argv[0]);
		}
	}
	else
	{
		usage(argv[0]);
	}

	return 0;
}
