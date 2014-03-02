/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/un.h>

#include "../libstecom.h"


int main(int argc, char **argv)
{
  struct stec_ctrl *ctrl, *event_ctrl;
  struct sockaddr_un addr, event_addr;
  fd_set rfds;
  int fd = -1, event_fd = -1, res = 0;
  struct timeval timeout;
  const char *cmdPing = "PING";
  const char *respPong = "PONG";
  char buf[21];
  size_t len = 20;
  int quit = 0;

  addr.sun_family = AF_UNIX;
  strcpy(addr.sun_path, "/tmp/stecom-test");
  unlink(addr.sun_path);

  event_addr.sun_family = AF_UNIX;
  strcpy(event_addr.sun_path, "/tmp/stecom-event-test");
  unlink(event_addr.sun_path);

  ctrl = stec_opensrv((struct sockaddr *)&addr, sizeof(addr));
  if(!ctrl)
  {
    printf("Failed opening socket\n");
    goto THIS_FAILED;
  }

  if(argc > 1 &&
     strlen(argv[1]) == strlen("event_only") &&
     strncmp(argv[1], "event_only", strlen(argv[1])) == 0)
  {
    event_ctrl = ctrl;
    ctrl = NULL;

    printf("Event only!\n");
  } else {
    event_ctrl = stec_opensrv((struct sockaddr *)&event_addr, sizeof(event_addr));

    if(!event_ctrl)
    {
      printf("Failed opening event socket\n");
      goto THIS_FAILED;
    }
  }

  if(ctrl)
  {
    fd = stec_get_fd(ctrl);
  }

  if(event_ctrl)
  {
    event_fd = stec_get_fd(event_ctrl);
  }

  printf("Entering wait loop. Timout after 10 seconds if no activity\n");
  while(!quit)
  {
    // Handle incoming cmds
    FD_ZERO(&rfds);
    if(event_fd > -1) FD_SET(event_fd, &rfds);
    if(fd > -1) FD_SET(fd, &rfds);

    timeout.tv_sec = 10;
    timeout.tv_usec = 0;

    res = select(sizeof(rfds)*8, &rfds, NULL, NULL, &timeout);
    if(res > 0)
    {
      if(fd > -1 && FD_ISSET(fd, &rfds))
      {
        printf("-> CTRL\n");
        res = stec_recv(ctrl, buf, len, -1);

        if(res == strlen(cmdPing)+1 && !strncmp(buf, cmdPing, res))
        {
          printf("<- Reply to PING\n");
          res = stec_send(ctrl, respPong, strlen(respPong)+1);

          // Also send pong as event
          printf("<-- Sending PONG as event\n");
          res = stec_sendevent(event_ctrl, respPong, strlen(respPong)+1);
        } else {
          buf[res] = 0;
          printf("<- Received unknown cmd (len=%d): '%s'\n", res, buf);
          res = stec_send(ctrl, "UNKNOWN", 7);
        }

        if(res < 0)
        {
          printf("Ctrl FAIL! %d\n", res);
        }
      }

      if(event_fd > -1 && FD_ISSET(event_fd, &rfds))
      {
        printf("-> EVENT subscribe/unsubscribe\n");
        res = stec_process_recv(event_ctrl);

        if(res < 0)
        {
          printf("Event process FAILED! %d\n", res);
        } else {
          printf("Event process OK\n");
          printf("<-- Sending PONG as event\n");
          res = stec_sendevent(event_ctrl, respPong, strlen(respPong)+1);
        }
      }
    }
    else
    {
      printf("Exiting on inactivity timer\n");
      quit = 1;
    } 
  }

  if(ctrl) stec_close(ctrl);
  if(event_ctrl) stec_close(event_ctrl);

  printf("DONE\n");

  return 0;

THIS_FAILED:
  printf("TEST FAILED\n");
  return -1;
}

