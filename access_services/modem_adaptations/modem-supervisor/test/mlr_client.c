/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <stdlib.h>
#include <pthread.h>
#include <sys/wait.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "./mlr_api.h"


void error(char *msg)
{
	perror(msg);
	exit(0);
}

int Socket_Client_Open(char *hostname, char *sportno)
{
	int sockfd, portno;
	struct sockaddr_in serv_addr;
	struct hostent *server;

/*|+    printf("Hostname %s sportno %s \n",hostname, sportno);+|*/
/*|+    portno = atoi(sportno);+|*/

/*    sockfd = socket(AF_INET, SOCK_STREAM, 0);*/
/*    printf("Create Socket \n");*/

/*    if (sockfd < 0) {*/
/*        error("ERROR opening socket");*/
/*        printf("Error Create Socket \n");*/
/*    }*/

/*    printf("Create Server with hostname %s \n", hostname);*/
/*    server = gethostbyname(hostname);*/
/*    printf("After Create Server with hostname %s \n", hostname);*/

/*    if (server == NULL) {*/
/*        fprintf(stderr,"ERROR, no such host\n");*/
/*        printf("No host found  \n");*/

/*        exit(0);*/
/*    }*/

/*    bzero((char *) &serv_addr, sizeof(serv_addr));*/

/*    serv_addr.sin_family = AF_INET;*/

/*    bcopy((char *)server->h_addr,\*/
/*            (char *)&serv_addr.sin_addr.s_addr,*/
/*            server->h_length);*/

/*    serv_addr.sin_port = htons(portno);*/

/*    printf("connect Socketfd \n");*/
/*    if (connect(sockfd,(const struct sockaddr *) &serv_addr,\*/
/*                sizeof(serv_addr)) < 0)*/
/*        error("ERROR connecting");*/

/*    return sockfd;*/
	return 0;

}
