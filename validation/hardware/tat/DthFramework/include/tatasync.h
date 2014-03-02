/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file
* \brief   This lib provide primitives to forward event to asynch deamon
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef TATASYNCH_H_
#define TATASYNCH_H_

/* IPC_KEY */
#define ASYNCH_IPC_KEY	0x0A
#define TYPE_DTH_ASYNCH 0xBA

/* value attibutes */

#define PERM_CHANGED	0
#define VALUE_CHANGED	1
#define NOTIFICATION_ALERT 2
#define TREE_CHANGED 3

#define MAX_MSG_LEN 256

typedef struct msg{
	int type;
	char message[MAX_MSG_LEN];
} msg_st;

int libtatasync_msg_send(unsigned char type, char *dth_path);
int libtatasync_msg_send_with_alert(unsigned char type, char *dth_path, char * alert_notification );
int init_asynchLib(void);

#endif
