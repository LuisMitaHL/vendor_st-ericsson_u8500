/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file
* \brief   Asynch event test
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include "tatasync.h"

void
usage()
{
	fprintf(stderr, "asyncEvent_Test: -p <path> -t <type> -d\n"
		"\t-p\tPath where the event occurred.\n"
		"\t-t\toptional: Event type  (0 for permissions changed ,1 for value changed,2 for alert notification or 3 tree changed).\n"
		"\t-d\toptional: enable debug mode.\n"
		"\t-a\tif alert notification : alert message\n");
	exit(-1);
}

int main(int argc, char *argv[])
{

	int c;
	int debug = 0;
	char *path = NULL;
	char *msg_alert = NULL;
	int type = PERM_CHANGED;
	while ((c = getopt(argc, argv, "p:t:d:a:")) != -1) {
		switch (c) {
		case 'd':
			debug = 1;
			break;
		case 'p':
			path = optarg;
			break;
		case 't':
			type = atoi(optarg);
			break;
		case 'a':
			msg_alert = optarg;
			if( msg_alert != NULL)
			printf("\n ... optarg : %s",msg_alert);
			else
			printf("\n ... optarg : NULL");
			break;
		default:
			usage();
			break;
		}
	}

	if ((path!=NULL) && ((type == PERM_CHANGED) || (type == VALUE_CHANGED) || (type == TREE_CHANGED))) {
		/* asynch init */
		init_asynchLib();
		/* send asycn event */
		libtatasync_msg_send(type, path);
	} else if( (path!=NULL) && ((type == NOTIFICATION_ALERT))) {
		/* asynch init */
		init_asynchLib();
		/* send asycn event */
		libtatasync_msg_send_with_alert(type, path, msg_alert);
	}else
		usage();

	return 0;
}
