/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/inotify.h>
#include <stdbool.h>
#include <dbus/dbus.h>
#include "mad/mad.h"
#include "mad_log.h"
#include <util_mainloop.h>
#include "mad_log.h"
#include "backend/fd_handler.h"
#include "backend/at_handler.h"
#include <cutils/properties.h>


#define DISPLAY_WAKE_LOCK_PATH "/sys/power/wait_for_fb_sleep"
static bool special_dormancy = false;
static int tethering = false;
static int screenState = true;
static int mad_screen_fd;
static int mad_screen_wd;
static long int mask_flag = 0;

void mad_fd_handler_init()
{
     char special_fast_dormancy[PROPERTY_VALUE_MAX]={0};

    property_get("ste.special_fast_dormancy", special_fast_dormancy, "false");

    MAD_LOG_D("special_fast_dormancy: %c  special_dormancy: %d", special_fast_dormancy[0],special_dormancy);

    /* If fast_dormancy is true  then send AT*EHSTACT=2 to AT Core Once and
      let modem take decision on Fast dormancy.if fast_dormancy is false
      then based on screen and Tether states trigger AT commands */
    if(!strcmp(special_fast_dormancy,"true"))
      {
        special_dormancy = true;
        mad_at_handler_send_FD(FD_ENABLE);
      }
    else
     {
        special_dormancy = false;
        mad_screenstate_monitor_init();
      }
   return;
}

/* Dbus handler for Receiveing USB Tethering events from netd */
bool tether_dbus_msg_handler(DBusConnection *conn, DBusMessage *msg)
{
    DBusMessage *msg_reply;
    DBusMessageIter args;
    char *state_string = NULL;

    if (!msg) {
        MAD_LOG_E("msg was null");
        return false;
    }

    if (dbus_message_is_method_call(msg, STE_MAD_DBUS_TETHER_NAME, STE_MAD_USB_TETHER)) {
        MAD_LOG_D("USB Tether event Received %s ", STE_MAD_USB_TETHER);
    } else {
       MAD_LOG_W("Unknown message received");
        goto not_handled;
    }

    if (!dbus_message_iter_init(msg, &args)) {
        MAD_LOG_W("Message has no arguments!");
        goto not_handled;
    }

    if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args)) {
        MAD_LOG_W("Argument is not string!");
        goto not_handled;
    }

    dbus_message_iter_get_basic(&args, &state_string);

    if (!strncmp(state_string, STE_MAD_FEATURE_ON, strlen(STE_MAD_FEATURE_ON))) {
          tethering = true;
    } else if (!strncmp(state_string, STE_MAD_FEATURE_OFF, strlen(STE_MAD_FEATURE_OFF))) {
          tethering = false;
    } else {
        MAD_LOG_W("Message contained unknown state value:");
        goto not_handled;
    }

    /* Send an empty reply back */
    msg_reply = dbus_message_new_method_return(msg);

    if (!msg_reply) {
        MAD_LOG_E("Failed to create a reply message");
        goto not_handled;
    }

    if (!dbus_connection_send(conn, msg_reply, NULL)) {
        MAD_LOG_E("Unable to reply to received msg");
    }

    dbus_message_unref(msg_reply);

    /* Call mad_screen_tether_event for sending AT*EHSTACT Fast Dormancy Command */
    if (!special_dormancy)
       mad_screen_tether_event();

    return true;

not_handled:
    return false;
}

/*Call Back function for Screen state handling */
int screen_display_process_callback(const int fd, const void *data_p)
{

    struct inotify_event *inotify_data_p = NULL;
    char tmp_data[20];
    int res = -1;
    (void)data_p;

    res = read(fd, &tmp_data, sizeof(tmp_data));

    if (res < 0) {
        MAD_LOG_E("Error %d (%s) when reading inotify ",
                 errno, strerror(errno));
        goto exit;
    }

    inotify_data_p = (struct inotify_event *)tmp_data;

    if (mask_flag == inotify_data_p->mask){
        MAD_LOG_E("Screen state same as the last sent, not sending it again");
        goto exit;
    }
      if(inotify_data_p->mask & IN_CLOSE) {
        /* Wake-lock has been released (Display is off) */
        screenState = 0;
        mask_flag = inotify_data_p->mask;
        mad_screen_tether_event();
      } else if (inotify_data_p->mask & IN_OPEN) {
        /* Wake-lock has been set (Display is on) */
        screenState = 1;
        mask_flag = inotify_data_p->mask;
        mad_screen_tether_event();
    } else {
       MAD_LOG_E("invalid screen state");
    }

exit:
   return true;
}

  /* monitoring the file using Util Mainloop */
  void mad_screenstate_monitor_init()
 {

   /*creating the INOTIFY instance*/
    mad_screen_fd = inotify_init();

    if (mad_screen_fd== -1) {
        goto error;
    }
     /* Creating the Watch descriptor */
    mad_screen_wd = inotify_add_watch(mad_screen_fd, DISPLAY_WAKE_LOCK_PATH, IN_CLOSE | IN_OPEN);

    if (mad_screen_wd == -1) {
        goto error;
    }

    MAD_LOG_D("mad_screen_wd is: %d ,mad_screen_fd is: %d", mad_screen_wd, mad_screen_fd);

    util_mainloop_add_watch(EVENT_READ, mad_screen_fd,
                             (void *)NULL, screen_display_process_callback, NULL);

 error:
    return;
}

/* Handling screen and Usb Tethering Events */
void mad_screen_tether_event()
{
    MAD_LOG_D("inside Screen_tether Screensate: %d  Tether State: %d", screenState, tethering);

    if ((!tethering) && (!screenState))
      mad_at_handler_send_FD(FD_ENABLE); /* FD_ENABLE 1 */
    else
      mad_at_handler_send_FD(FD_DISABLE); /* FD_DISABLE 2 */

    return;
}


