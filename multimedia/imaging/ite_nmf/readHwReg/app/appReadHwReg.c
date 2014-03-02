/* appReadHwReg 
	- Application that help in debugging by read/write SIA hw reg over adb connection
	- 	
   
   AUTHOR: Chetan Nanda (chetan.nanda@stericsson.com)
*/

/* Header files inclusion
*/

/*Common header files*/
#include 	<stdio.h>
#include    <sys/types.h>       /* basic system data types */
#include    <sys/socket.h>      /* basic socket definitions */
#include    <sys/time.h>        /* timeval{} for select() */
#include    <time.h>            /* timespec{} for pselect() */
#include    <netinet/in.h>      /* sockaddr_in{} and other Internet defns */
#include    <arpa/inet.h>       /* inet(3) functions */
#include    <errno.h>
#include    <fcntl.h>           /* for nonblocking */
#include    <netdb.h>
#include    <string.h>
#include    <sys/stat.h>        /* for S_xxx file mode constants */
#include    <sys/uio.h>         /* for iovec{} and readv/writev */
#include    <unistd.h>
#include    <sys/wait.h>
#include    <sys/un.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <poll.h>

/*Handling Ctr-c*/
#include <stdlib.h>
#include <signal.h>

/*readHwReg headers*/
#include <readHwReg_ioctl.h>
#include "appReadHwReg.h"

/*Local defines used in the application*/

/*Debug trace*/
#define DEBUG_LOG printf

/*ReadHwReg driver file*/
#define READHWREG_DEV_FILE "readhwreg_camera"

/*Network port on which application accept connection*/
#define READHWREG_SERVER_PORT 38304

/*Driver file name*/
#define READHWREG_DRV_FILE_NAME 	"st_readHwReg.ko"

/*Maximum number of client at one time*/
#define READHWREG_MAX_CLIENT	5

// Global definitions 
int g_ServerSocketFd = -1;					//Server socket fd
int g_DevFileId = -1;							//Device file descriptor
struct pollfd client[READHWREG_MAX_CLIENT];	//Active clients connections

// Local function prototype declarations
int APPReadHwReg_DevInit(void);	//Initialize the driver connection
int APPReadHwReg_DevDeInit(void);	//DeInitialize the driver connection, freeup any resource acquired
int APPReadHwReg_ListenForClientConnection(void); // Listen on server socket for incoming connections from client
int APPReadHwReg_CreateServerConnection(void); // Create a server socket connection on which application accepting connection from the clienr
int APPReadHwReg_DestroyServerConnection(void);	// Destroy server connection 
int APPReadHwReg_HandleClientReq(int sockfd, sMsgBuf_t msg); //Handle request from client
void APPReadHwReg_Cleanup(int sig);// Signal handler for handling SIGINT - CTR+C

// Local function Definitions - 

/* APPReadHwReg_DevInit - 
** This will initialize the ReadHwReg driver
**  - Insert the kernel module
** 	- setup the fd for driver access
*/
int APPReadHwReg_DevInit(void)
{
	int error = 0;
	char cmdLine[100],devNodeName[64];
	
	//Insert the kernel module module
	//Create the cmdline for inseting the kernel module
	sprintf(cmdLine, "insmod /system/usr/share/camera/%s", READHWREG_DRV_FILE_NAME);
	error = system(cmdLine);
	if(error)
	{
		printf("%d %s: Failed to insert module .... %s !!!\n",__LINE__,__FUNCTION__,strerror(errno));
		error = errno;
		goto ERR_OUT;
	}

	//Create the device file
	{	
		char line[255];
		FILE* filefd = NULL;
		char dev_name[64];
		int dev_num = -1;
		int rc;
		dev_t dev;
		
		/*Open the /proc/devices file to read the Devicenumber for adetk thread*/	
		filefd = fopen("/proc/misc","r");
		if(filefd == NULL)
		{
			printf("%d %s: Fail to open the proc file: error - %s\n",__LINE__, __FUNCTION__,strerror(errno));
			error = errno;
			goto ERR_OUT_DEV;	
		}

		while(line == fgets(line, sizeof(line), filefd))
		{
			/*read the device number, and device name*/	
			rc = sscanf(line," %d %s",&dev_num, dev_name);

			//printf("%d %s: Device detected = %s, Device searched = %s, dev_num = %d\n",__LINE__,__FUNCTION__,dev_name, READHWREG_DEV_FILE, dev_num);

			if(0 == strcmp((const char*)dev_name,READHWREG_DEV_FILE))
				break;

			dev_num = -1;
		}

		//Check if dev found or not
		if(dev_num == -1)
		{
			printf("%d %s: ReadHwReg driver not registered properly ....",__LINE__, __FUNCTION__);
			error = -1;
			goto ERR_OUT_DEV;
		}

		//printf("%d %s: Creating the mknod on %d\n",__LINE__,__FUNCTION__,dev_num);

		//Create mknode device entry
		sprintf(devNodeName, "/dev/%s", dev_name);
		if(0 != mknod(devNodeName, S_IRUSR | S_IWUSR | S_IFCHR,makedev(10 /*10 -is fixed major number for misc devices*/, dev_num)))
		{
			if(errno == EEXIST)
			{
				//Node already exist ... do nothind
			}
			else
			{
				printf("%d %s: Failed to create device node... %s\n",__LINE__, __FUNCTION__,strerror(errno));
				error = errno;
				goto ERR_OUT_DEV;
			}
		}

		//printf("%d %s: mknod (%s) successful \n",__LINE__, __FUNCTION__, devNodeName);
		fclose(filefd);
		filefd = NULL;
	}

	//Open the device file for the communication with the driver
	g_DevFileId = open ((const char *)devNodeName, O_RDWR);
	if(g_DevFileId < 0)
	{
		printf("%d %s: Failed to open device file ... %s !!!\n",__LINE__,__FUNCTION__,strerror(errno));
		error = errno;
		goto ERR_OUT_DEV;
	}

	printf("%d %s: Driver ready for communication ....\n",__LINE__, __FUNCTION__);
	return error;
	
ERR_OUT_DEV:
	//Error in dev init, remove the module from the system
	{
		/*Removing the module from the system*/
		sprintf(cmdLine, "rmmod %s", READHWREG_DRV_FILE_NAME);
		system(cmdLine);
	}
ERR_OUT:
	return error;
	
}


/* APPReadHwReg_DevDeInit - 
** De init the device connection and freeup any resource acquired during the initialization
*/
int APPReadHwReg_DevDeInit(void)
{
	int error = 0;
	char cmdLine[100];
	
	//Remove the kernel module from the system
	sprintf(cmdLine, "rmmod %s", READHWREG_DRV_FILE_NAME);
	system(cmdLine);

	//Close the device file
	close(g_DevFileId);
	
	return error;
}
	
/* APPReadHwReg_ListenForClientConnection - 
** This API will listen for incoming connection from clients on server connection 
*/
int APPReadHwReg_ListenForClientConnection(void)
{
	int error = 0, n= 0;
	int connfd = -1, i = 0,maxi, nready,sockfd = -1;
	socklen_t clilen;
	struct sockaddr_in cliaddr;
	sMsgBuf_t msg;
	
	error = listen(g_ServerSocketFd, 5);
	if(error < 0)
	{
		printf("%d  %s: Listen command failed ... %s",__LINE__,__FUNCTION__, strerror(errno));
		error = errno;
		return error;
	}

	client[0].fd = g_ServerSocketFd,
	client[0].events = POLLRDNORM;
	for (i = 1; i < READHWREG_MAX_CLIENT; i++)
	     client[i].fd = -1;      /* -1 indicates available entry */
	maxi = 0;                   /* max index into client[] array */

#if TEST
	{	
		sMsgBuf_t msg2;
		memset((void *)&msg2, 0, sizeof(sMsgBuf_t));
		msg2.u32_address =  0xa025250c;
		msg2.u32_dataSize = 4;
		msg2.u8_access = READHWREG_READ;
		error = ioctl(g_DevFileId, READHWREG_CAM_READREG, (unsigned long)&msg2);
		printf("%d %s: Data = %x\n\n", __LINE__,__FUNCTION__, msg2.u32_data);
	}
#endif
	
	for ( ; ; ) 
	{
		nready = poll(client, maxi + 1, -1);
		//printf("%d %s: Number of FD ready %d\n",__LINE__,__FUNCTION__, nready);
		if(nready < 0)
		{
			printf("\n%d %s: Error in POLL system call ... %s\n",__LINE__,__FUNCTION__, strerror(errno));
			error = errno;
			return error;
		}
		
		if (client[0].revents & POLLRDNORM) 
		{  
			/* new client connection */
			clilen = sizeof(cliaddr);
			connfd = accept(g_ServerSocketFd, (struct sockaddr *) &cliaddr, &clilen);
			if(connfd < 0)
			{
				printf("%d %s: Accept system call failed ... %s",__LINE__, __FUNCTION__,strerror(errno));
				error = errno;
				return error;
			}
			printf("Connection recvd from: CliPort = %d \n",cliaddr.sin_port);
				
  			for (i = 1; i < READHWREG_MAX_CLIENT; i++)
                 	if (client[i].fd < 0) {
				printf("%d %s: Recv connection from client - %d, connfd = %d\n",__LINE__,__FUNCTION__,i,connfd);
                     	client[i].fd = connfd;  /* save descriptor */
                     	break;
                 	}
             		if (i == READHWREG_MAX_CLIENT)
	              	printf("%d %s: Maximum client connected ... ",__LINE__,__FUNCTION__);

			client[i].events = POLLRDNORM;
			if (i > maxi)
                 		maxi = i;       /* max index in client[] array */
             		if (--nready <= 0)
      			{
                 		continue;       /* no more readable descriptors */
     			}						
         	}

		for (i = 1; i <= maxi; i++) 
		{     
			sockfd =  client[i].fd;
			
			/* check all clients for data */
			if ( sockfd < 0)
                		continue;
			
            		if (client[i].revents & (POLLRDNORM | POLLERR)) 
			{	
				n = read(sockfd, (void *) &msg, sizeof(sMsgBuf_t));
				if (n < 0 )
				{
                    			if (errno == ECONNRESET) 
					{
                            		/* connection reset by client */
						printf("%d %s: Connection reset by client ...\n",__LINE__,__FUNCTION__);			
						close(sockfd);
						client[i].fd = -1;
					} 
					else
					{
						printf("\n%d %s Read error ... %s\n",__LINE__,__FUNCTION__, strerror(errno));
					}				
				} 
				else if (n == 0) 
				{
                     		/* connection closed by client */
					printf("%d %s: Connection closed by client ....\n",__LINE__,__FUNCTION__);
                    			close(sockfd);
                    			client[i].fd = -1;
                		} 
				else
				{
					/*Valid request from the client*/
					error = APPReadHwReg_HandleClientReq(sockfd, msg);
					if(error)
					{
						printf("%d %s: Failed to services client request ...",__LINE__,__FUNCTION__);
						return error;
					}

				}			
                		if (--nready <= 0)
                       		break;               /* no more readable descriptors */
            		}
        	}
    	}
	return error;
}

/* APPReadHwReg_HandleClientReq - 
** recv a valid request from client, process the read/write request and provide feedback to client
*/
int APPReadHwReg_HandleClientReq(int sockfd, sMsgBuf_t msg) //Handle request from client
{
	int error = 0;
	if(msg.u8_type == READHWREG_TYPE_HWREG)
	{
		if(READHWREG_READ==  msg.u8_access)
		{
			//Read request		
			//Service the client request, send the same request to driver
			//printf("%d %s: Request = %s, Address = 0x%8x, size = %d\n",__LINE__,__FUNCTION__, msg.u8_access?"WRITE":"READ", msg.u32_address, msg.u32_dataSize);
			error = ioctl(g_DevFileId, READHWREG_CAM_READREG, (unsigned long)&msg);
			if(error < 0 || msg.u8_status == READHWREG_STATUS_FAILED)
			{
				if(error < 0)
					printf("%d %s: ioctl system call failed ... %s\n",__LINE__,__FUNCTION__, strerror(errno));
				else
					printf("%d %s: driver failed to process the request",__LINE__,__FUNCTION__);
			}
			else
			{
				//printf("%d %s: regVal = 0x%x\n",__LINE__,__FUNCTION__,msg.u32_data);
				msg.u8_status = READHWREG_STATUS_OK;
			}
	
			//Send response back to client	
			write(sockfd, (char*)&msg, sizeof(sMsgBuf_t));
		}
		else if(READHWREG_WRITE ==  msg.u8_access)
		{
			//Write request		
			//Service the client request, send the same request to driver
			//printf("%d %s: Request = %s, Address = 0x%8x, size = %d, data = %x\n",__LINE__,__FUNCTION__, msg.u8_access?"WRITE":"READ", msg.u32_address, msg.u32_dataSize, msg.u32_data);
			error = ioctl(g_DevFileId, READHWREG_CAM_WRITEREG, (unsigned long)&msg);
			if(error < 0 || msg.u8_status == READHWREG_STATUS_FAILED)
			{
				if(error < 0)
					printf("%d %s: ioctl system call failed ... %s\n",__LINE__,__FUNCTION__, strerror(errno));
				else
					printf("%d %s: driver failed to process the request",__LINE__,__FUNCTION__);
			}
			else
			{	
				msg.u8_status = READHWREG_STATUS_OK;
			}
		
			//Send response back to client	
			write(sockfd, (char*)&msg, sizeof(sMsgBuf_t));
	
		}
	}	
	else
	{
		//Page element readwrite
		if(READHWREG_READ==  msg.u8_access)
		{
			//Read request		
			//Service the client request, send the same request to driver
			//printf("%d %s: = PE = Request = %s, Address = 0x%8x, size = %d\n",__LINE__,__FUNCTION__, msg.u8_access?"WRITE":"READ", msg.u32_address, msg.u32_dataSize);
			error = ioctl(g_DevFileId, READHWREG_CAM_READPE, (unsigned long)&msg);
			if(error < 0 || msg.u8_status == READHWREG_STATUS_FAILED)
			{
				if(error < 0)
					printf("%d %s: ioctl system call failed ... %s\n",__LINE__,__FUNCTION__, strerror(errno));
				else
					printf("%d %s: driver failed to process the request",__LINE__,__FUNCTION__);
			}
			else
			{
				//printf("%d %s: regVal = 0x%x\n",__LINE__,__FUNCTION__,msg.u32_data);
				msg.u8_status = READHWREG_STATUS_OK;
			}
	
			//Send response back to client	
			write(sockfd, (char*)&msg, sizeof(sMsgBuf_t));
		}
		else
		{
			//printf("%d %s: = PE = Request = %s, Address = 0x%8x, size = %d, data = %x\n",__LINE__,__FUNCTION__, msg.u8_access?"WRITE":"READ", msg.u32_address, msg.u32_dataSize, msg.u32_data);
			//Write request		
			//Service the client request, send the same request to driver
			//printf("%d %s: = PE = Request = %s, Address = 0x%8x, size = %d\n",__LINE__,__FUNCTION__, msg.u8_access?"WRITE":"READ", msg.u32_address, msg.u32_dataSize);
			error = ioctl(g_DevFileId, READHWREG_CAM_WRITEPE, (unsigned long)&msg);
			if(error < 0 || msg.u8_status == READHWREG_STATUS_FAILED)
			{
				if(error < 0)
					printf("%d %s: ioctl system call failed ... %s\n",__LINE__,__FUNCTION__, strerror(errno));
				else
					printf("%d %s: driver failed to process the request",__LINE__,__FUNCTION__);
			}
			else
			{
				//printf("%d %s: regVal = 0x%x\n",__LINE__,__FUNCTION__,msg.u32_data);
				msg.u8_status = READHWREG_STATUS_OK;
			}
	
			//Send response back to client	
			write(sockfd, (char*)&msg, sizeof(sMsgBuf_t));
		}
	}
	return error;
}

/* APPReadHwReg_CreateServerConnection - 
** This API create a server connection and bind it to port 'READHWREG_SERVER_PORT'
*/
int APPReadHwReg_CreateServerConnection()
{
	int error = 0;
    struct sockaddr_in servaddr;	//Server address of the connection
	
	//Create the server socket - a TCP socket
    if ( (g_ServerSocketFd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("%d %s: Server Socket Creation failed .... - %s!!!!\n",__LINE__,__FUNCTION__, strerror(errno));
        	return errno;
	}
	
	//Initialize the server socket address
    bzero(&servaddr, sizeof(servaddr));		//memset the server address 
    servaddr.sin_family = AF_INET;			// AF_INET family
    servaddr.sin_port = htons(READHWREG_SERVER_PORT);  //Server port - on which it listen for connections 
    if (inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) <= 0)	//Server IP address - local ip address
	{
		printf("%d %s: Inet_pton failed ... %s\n",__LINE__,__FUNCTION__,strerror(errno));
		return errno;	
	}

	//Bind server to a particular socket	
	if((bind(g_ServerSocketFd, (struct sockaddr *) &servaddr, sizeof(servaddr))) < 0 )
	{
		printf("%d %s: Server bind failed ... %s\n",__LINE__,__FUNCTION__,strerror(errno));
		return errno;	
	}

	printf("%d %s: Server socket creation/bind completed ... !!!\n",__LINE__,__FUNCTION__);
	return error;
}

/* APPReadHwReg_DestroyServerConnection - 
** This API will destroy the client/server connection, and finally close the server socket
*/
int APPReadHwReg_DestroyServerConnection(void)
{
	int error = 0;
	
	//Close the socket
	close(g_ServerSocketFd);
	g_ServerSocketFd = -1;
	
	return error;
}

/* APPReadHwReg_Cleanup - 
** Signal handler for allowing clean exit
*/ 
void APPReadHwReg_Cleanup(int sig)
{
	int i = 0;

	/*SIGINT recvd ... start cleanup*/
	printf("%d %s: Application Interrupted\n",__LINE__,__FUNCTION__);

	//Close the active client connection
	for(i = 1; i < READHWREG_MAX_CLIENT; i++)
	{
		if(client[i].fd < 0)
			continue;

		//Close the connection fd
		close(client[i].fd);
	}

	//close the driver connection
	APPReadHwReg_DevDeInit();


	//close the server connection
	APPReadHwReg_DestroyServerConnection();

	exit(0);
}


/*Main function*/
int main()
{
	int error = 0;
#if 0	
	struct sigaction sa;
	memset (&sa, 0, sizeof (sa));
	sa.sa_handler = &APPReadHwReg_Cleanup;
	sigaction (SIGINT, &sa, NULL);
	
	//Installing a signal handler for handling Ctr-C
	//(void) signal(SIGINT,APPReadHwReg_Cleanup);
#endif

	// Create the server connection, this will establish the connection between client running on PC 
	// and application runing on Android device
	error = APPReadHwReg_CreateServerConnection();
	if(error)
	{
		printf("\nServer connection failed ...!!!! \n");
		goto ERR_OUT;
	}
	// Initialize the application-driver connection
	error = APPReadHwReg_DevInit();
	if(error)
	{
		printf("\nDevice init failed ... !!!\n");
		goto ERR_OUT_DEVINIT_FAILED;
	}

	//Client and Device connection completed
	//Wait and connect to client
	error = APPReadHwReg_ListenForClientConnection();
	if(error)
	{
		printf("\nClient connection failed ... !!!\n");
		goto ERR_OUT_CLICON_FAILED;
	}
	
	return 0;
	
ERR_OUT_CLICON_FAILED:
	APPReadHwReg_DevDeInit();
	
ERR_OUT_DEVINIT_FAILED:
	APPReadHwReg_DestroyServerConnection();
	return error;	
	
ERR_OUT:
	return error;

}


