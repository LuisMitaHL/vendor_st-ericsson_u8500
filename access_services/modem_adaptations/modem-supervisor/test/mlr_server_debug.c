/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <stdlib.h>
#include <pthread.h>
#include <sys/wait.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>


#include <semaphore.h>  /* Semaphore */

void mlr_error(char *msg)
{
	perror(msg);
	exit(1);
}


void Command_Handler(int fd)
{
	char buffer[256];
	int n;

	printf("Command_Handler is launched (fd=%d)\n",fd);

	while(1) {

		bzero(buffer,256);

		n = read(fd, buffer, 255);

		if (n < 0)
			mlr_error("ERROR reading from socket");

		printf("Command_Handler:: cmd received = %s\n",buffer);

		n = write(fd,"OK",strlen("OK"));

	}

}


void Launch_detatched_thread(void *proc, void *param)
{
	pthread_t thread;
	pthread_attr_t attribut;

	pthread_attr_init(&attribut);
	pthread_attr_setdetachstate(&attribut,PTHREAD_CREATE_DETACHED);

	if (pthread_create(&thread,&attribut,proc, param))
	{
		perror("pthread_create Launch_detatched_thread ");
		exit(EXIT_FAILURE);
	}
}



/**
 * \fn int Socket_Open_Server(int port)
 * \brief Open a server socket.
 * Open a server socket on a specific port and return the file descriptor
 * pctool can be detected on ethernet.
 *
 * \param port port to listen
 * \return file descriptor of sock if successfull else -1
 */

int Socket_Open_Server(int port, int nb_client)
{
	int sock, bind_error;
	struct sockaddr_in adresse_serveur;

	bzero(&adresse_serveur, sizeof(struct sockaddr));
	adresse_serveur.sin_family      = AF_INET;
	adresse_serveur.sin_addr.s_addr = INADDR_ANY;
	adresse_serveur.sin_port        = htons (port);

	sock = socket(AF_INET, SOCK_STREAM, 0);

	bind_error = bind(sock,(struct sockaddr *) &adresse_serveur, \
			sizeof(struct sockaddr));

	if ( bind_error == -1) {
		perror( "bind error:" );
		sock=-1;
	}

	if ( listen(sock, nb_client) == -1) {
		perror("listen error:");
		sock=-1;
	}
	return sock;
}



/**
 * \fn void sl3d_eth(char *num_port)
 * \brief This thread manages the tool detection of a ethernet port
 *  sl3d_eth opens a Socket server on specific port, wait client connection
 *
 * \param num_port is listen port in string format
 */
void Socket_Server_management(char * num_port)
{
	int sockfd, newsockfd, port;
	int end;

	port = atoi(num_port);

	printf("******* Server port=%d *******\n",port);
	sockfd = Socket_Open_Server(port, 2);

	end = 0;

	//	while(!end)
	{
		if ((newsockfd = accept(sockfd,NULL,0)) == -1)
		{
			perror("Accept");
		}
		else
		{
			Launch_detatched_thread(Command_Handler, (void *) newsockfd);
		}
	}
}



int mlr_server (int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}


	Socket_Server_management(argv[1]);

	printf("main...\n");

	while(1){
		sleep(0);
	}
	return 0;
}
