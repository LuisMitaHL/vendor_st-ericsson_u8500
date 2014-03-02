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


int main()
{
  struct stec_ctrl *ctrl, *event_ctrl;
  struct sockaddr_un addr, event_addr, laddr;
  fd_set rfds;
  int event_fd;
  struct timeval timeout;
  const char *cmdPing = "PING";
  const char *respPong = "PONG";
  char buf[10];
  int res = 10;

  // First some error testing
  stec_close(NULL);
  (void)stec_get_fd(NULL);

  addr.sun_family = AF_UNIX;
  strcpy(addr.sun_path, "/tmp/stecom-test");

  event_addr.sun_family = AF_UNIX;
  strcpy(event_addr.sun_path, "/tmp/stecom-event-test");

  ctrl = stec_open((struct sockaddr *)&addr, sizeof(addr));
  if(!ctrl)
  {
    printf("Failed opening socket\n");
    goto THIS_FAILED;
  }

  event_ctrl = stec_open((struct sockaddr *)&event_addr, sizeof(event_addr));
  if(!event_ctrl)
  {
    printf("Failed opening event socket\n");
    goto THIS_FAILED;
  }

  event_fd = stec_get_fd(event_ctrl);

  // Test these
  (void)stec_get_local_addr(ctrl, (struct sockaddr *)&laddr, sizeof(laddr));
  (void)stec_get_last_recvfrom(ctrl, (struct sockaddr *)&laddr, sizeof(laddr));

  // Send PING and expect PONG in return
  printf("-> Sending PING\n");
  res = 10;
  res = stec_sendsync(ctrl, cmdPing, strlen(cmdPing)+1, buf, res, 3);
  buf[res] = 0;
  if(res == strlen(respPong)+1 && !strncmp(buf, respPong, res))
  {
    printf("<- Got PONG!\n");
  } else {
    printf("<- Yikes, got something else with len=%d: %s\n", res, buf);
  }

  // Subscribe
  printf("-> Subscribing to events\n");
  res = stec_subscribe(event_ctrl);
  if(res != 0)
  {
    printf("<- Subscribe failed!\n");

    goto THE_END;
  } else {
    printf("<- Subscribe OK\n");
  }

  // Wait for incoming event (max 1 min) on event socket (expecting PONG)
  FD_ZERO(&rfds);
  FD_SET(event_fd, &rfds);
  timeout.tv_sec = 60;
  timeout.tv_usec = 0;
  res = select(sizeof(rfds)*8, &rfds, NULL, NULL, &timeout);
  if(res >= 0)
  {
    if(FD_ISSET(event_fd, &rfds))
    {
      res = stec_recv(event_ctrl, buf, 10, -1);

      if(res == strlen(respPong)+1 && !strncmp(buf, respPong, res))
      {
        printf("<-- Received event PONG!\n");
      }
    }
    else if(res == 0)
    {
      printf("TIMEOUT waiting for event\n");
    }
  }

  // Trigger event
  printf("-> Trigger event via PING\n");
  res = 10;
  res = stec_sendsync(ctrl, cmdPing, strlen(cmdPing)+1, buf, res, 3);
  buf[res] = 0;
  if(res == strlen(respPong)+1 && !strncmp(buf, respPong, res))
  {
    printf("<- Got PONG!\n");
  } else {
    printf("<- Yikes, got something else with len=%d: %s\n", res, buf);
  }

  // Wait for incoming event (max 1 min) on event socket (expecting PONG)
  FD_ZERO(&rfds);
  FD_SET(event_fd, &rfds);
  timeout.tv_sec = 60;
  timeout.tv_usec = 0;
  res = select(sizeof(rfds)*8, &rfds, NULL, NULL, &timeout);
  if(res >= 0)
  {
    if(FD_ISSET(event_fd, &rfds))
    {
      res = stec_recv(event_ctrl, buf, 10, -1);

      if(res == strlen(respPong)+1 && !strncmp(buf, respPong, res))
      {
        printf("<-- Received event PONG!\n");
      }
    }
    else if(res == 0)
    {
      printf("TIMEOUT waiting for event\n");
    }
  }

  // Unsubscribe
  printf("-> Unsubscribing events\n");
  res = stec_unsubscribe(event_ctrl);
  if(res != 0)
  {
    printf("<- Unsubscribe failed!\n");

    goto THE_END;
  } else {
    printf("<- Unsubscribe OK\n");
  }

  // Send PING and expect PONG in return
  printf("-> Sending PING\n");
  res = 10;
  res = stec_sendsync(ctrl, cmdPing, strlen(cmdPing)+1, buf, res, 3);
  buf[res] = 0;
  if(res == strlen(respPong)+1 && !strncmp(buf, respPong, res))
  {
    printf("<- Got PONG!\n");
  } else {
    printf("<- Yikes, got something else with len=%d: %s\n", res, buf);
  }

  // Wait for incoming event (max 5 secs) on event socket (NOT expecting PONG)
  printf("Verifying that we don't receive an event PONG... (waiting max 5 secs)\n");
  FD_ZERO(&rfds);
  FD_SET(event_fd, &rfds);
  timeout.tv_sec = 5;
  timeout.tv_usec = 0;
  res = select(sizeof(rfds)*8, &rfds, NULL, NULL, &timeout);
  if(res >= 0)
  {
    if(FD_ISSET(event_fd, &rfds))
    {
      res = stec_recv(event_ctrl, buf, 10, -1);

      if(res == strlen(respPong)+1 && !strncmp(buf, respPong, res))
      {
        printf("<-- Received event PONG!\n");
      }
    }
  }

THE_END:
  stec_close(ctrl);
  stec_close(event_ctrl);

  printf("DONE\n");

  return 0;

THIS_FAILED:
  return -1;
}

