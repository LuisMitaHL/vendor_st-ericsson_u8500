/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include "../api/t_mm_probe.h"

#ifdef X86
  #define SOCK_PATH "/tmp/mmprobed_socket"
#else
  #define SOCK_PATH "/data/tmp/mmprobed_socket"
#endif

int main(void)
{
    int s, t, len;
    struct sockaddr_un remote;
    char str[100];

    // Create socket
    if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    printf("Trying to connect...\n");

    // Connect to remote socket
    remote.sun_family = AF_UNIX;
    strcpy(remote.sun_path, SOCK_PATH);
    len = strlen(remote.sun_path) + sizeof(remote.sun_family);
    if (connect(s, (struct sockaddr *)&remote, len) == -1) {
        perror("connect");
        exit(1);
    }

    printf("Connected\n");

    //Let mmprobe know which id this proxy listens to
    MM_ProbeSubscriberID_t subscriber_id = MM_PROBE_SUBSCRIBER_TEST;
    send(s, &subscriber_id, sizeof(int), 0);

    // Wait for data on socket and print received data
    while(1) {
        if ((t=recv(s, str, 100, 0)) > 0) {
            str[t] = '\0';
            printf("%s\n", str);
        } else {
            if (t < 0) perror("recv");
            else printf("Server closed connection\n");
            exit(1);
        }
    }
    //Close socket
    close(s);

    return 0;
}
