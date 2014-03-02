#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#define DEFAULT_PORT 1981

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, device_port,local_port, n,ret;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char server_arg[255];
    char cmd [1024];

    FILE * fp;
    char buffer[1024];

    sprintf(cmd,"adb wait-for-device");
    fprintf(stderr,"Waiting for device...");
    ret = system(cmd);
    if (ret) {
        fprintf(stderr,"\nSystem command >%s< returned an error ! Exiting...\n",cmd);
        exit(1);
    }
    else {
        fprintf(stderr,"Device connected\n");
    }
    int is_trace_enabled=0;


    char socket_trace_enabled[16];
    // Let's see if trace was enabled
    sprintf(cmd,"adb shell getprop ste.syslog.socket");
    //fprintf(stderr,"Issuing system command %s\n",cmd);
    fp = popen(cmd, "r");

    if (fp == NULL) {
        fprintf(stderr,"Failed to run command\n" );
        exit;
    }
    while (fgets(socket_trace_enabled, sizeof(socket_trace_enabled)-1, fp) != NULL) {
        //fprintf(stderr,"Retrieve socket enable flag : %s\n", socket_trace_enabled);
    }
    fclose(fp);
    is_trace_enabled =  atoi(socket_trace_enabled);


    if (!is_trace_enabled) {
        fprintf(stderr,"Redirection of traces to socket does not seem to be active, please set property ste.syslog.socket=1 in local.prop and reboot device\n");
        exit(1);
    }


    if (argc < 2) {

        char port[16];
        // Lets retrieve port

        sprintf(cmd,"adb shell getprop ste.syslog.socket.port");
        //fprintf(stderr,"Issuing system command %s\n",cmd);
        fp = popen(cmd, "r");

        if (fp == NULL) {
            fprintf(stderr,"Failed to run command\n" );
            exit;
        }
        while (fgets(port, sizeof(port)-1, fp) != NULL) {
            fprintf(stderr,"Retrieved remote port : %s\n", port);
        }
        fclose(fp);


        device_port = local_port = atoi(port);
        if (device_port == 0) {
            device_port = local_port = DEFAULT_PORT;

            fprintf(stderr,"Could not retrieve communication port from device through property ste.syslog.socket.port. Using default : %d\n",DEFAULT_PORT);
        }

        //fprintf(stderr,"Assuming local port is the same as remote port : %d\n",local_port);


    } else {

        //fprintf(stderr,"usage %s device_port [local_port]\n", argv[0]);
        //exit(0);
        device_port = atoi(argv[1]);
        if (argc>=3) {
            local_port = atoi(argv[2]);
        }
        else {
            local_port = device_port;
            //fprintf(stderr,"Assuming local and device port are the same : %d\n",local_port);

        }
    }

    server = gethostbyname("localhost");
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }


    sprintf(cmd,"adb forward tcp:%d tcp:%d",device_port,local_port);

    fprintf(stderr,"Forwarding device port...");
    ret = system(cmd);
    if (ret != 0) {
        fprintf(stderr,"System command >%s< returned an error ! Continuing anyway\n",cmd);
    }
    else {
        fprintf(stderr,"Success\n");
    }



    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");


    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
            (char *)&serv_addr.sin_addr.s_addr,
            server->h_length);
    serv_addr.sin_port = htons(local_port);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    fd_set rfds;


    char waitingchar=0;   sprintf(cmd,"adb wait-for-device");

    const char waitingloop[]="|/-\\|/-";
    int is_a_tty = isatty(fileno(stdin));
    int COUNT = strlen(waitingloop);
    int need_newline=0;
    while(1) {

        FD_ZERO(&rfds);
        FD_SET(sockfd, &rfds);
        struct timeval tv;

        tv.tv_sec = 0;
        tv.tv_usec = 2000000;

        int retval = select(sockfd+1, &rfds, NULL, NULL, &tv);
        /* Don't rely on the value of tv now! */
        if (retval == -1)
            perror("select()");
        else if (retval)
        {
            if (is_a_tty && need_newline) {
                fprintf(stderr,"\n");
                need_newline=0;
            }
        }
        else {
            if (is_a_tty) {
                fprintf(stderr,"Waiting for data %c...\r",waitingloop[(waitingchar++)%COUNT]);
                need_newline=1;
            }
            continue;
        }



        bzero(buffer,1024);
        n = read(sockfd,buffer,1023);
        if (n < 0) {
            error("ERROR reading from socket");
            exit(1);
        }
        else if (n==0) {
            fprintf(stderr,"Connection lost, exiting...\n");
            shutdown(sockfd,SHUT_RDWR);
            close(sockfd);
            exit(0);
        }
        printf("%s",buffer);
    }
    close(sockfd);
    return 0;
}

