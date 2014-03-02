/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file
* \brief   This lib provide asynchronous event primitves for DTH framework
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <stdio.h>
#include <errno.h>

#include "tatasync.h"

#define NB_OF_SEPARATOR 3

static int file_id = -1;

int init_asynchLib(void)
{
	/* BAL init */
	file_id = msgget(ASYNCH_IPC_KEY, 0);
	if (file_id == -1)
{
		fprintf(stderr,
		"Asyncd probably not running. Can't create msg file (%s)\n"
				,strerror(errno));
        perror("asyncd probably not running");
}
	else
		printf("Create msg file (%i)\n", file_id);

	return file_id;
}

int libtatasync_msg_send(unsigned char type, char *dth_path)
{
	int err = 0;
	msg_st msg;

	if (file_id == -1)
		goto no_asyncd;

	msg.type = TYPE_DTH_ASYNCH;

	if (strlen(dth_path) - 1 > MAX_MSG_LEN)
		return -1;

	sprintf(msg.message, "%s %d", dth_path, type);

	err = msgsnd(file_id, &msg, sizeof(msg_st), 0);
	if(err == -1)
		printf("error can't send msg (%s)\n",strerror(errno));

	return err;

no_asyncd:
	fprintf(stderr, "libasync not initialized\n");
	return -1;
}

int libtatasync_msg_send_with_alert(unsigned char type, char *dth_path, char * alert_notification )
{
	int err = 0;
	msg_st msg;

	if (file_id == -1)
		goto no_asyncd;

	msg.type = TYPE_DTH_ASYNCH;

	if (strlen(dth_path) - 1 > MAX_MSG_LEN)
		return -1;

	sprintf(msg.message, "%s %d %s", dth_path, type, alert_notification);

	err = msgsnd(file_id, &msg, sizeof(msg_st), 0);
	if(err == -1)
		printf("error can't send msg (%s)\n",strerror(errno));

	return err;

no_asyncd:
	fprintf(stderr, "libasync not initialized\n");
	return -1;
}
