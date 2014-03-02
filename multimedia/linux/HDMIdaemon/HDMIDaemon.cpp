/*
 * Copyright (C) 2007 The Android Open Source Project
 * Copyright (c) 2010, Code Aurora Forum. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "HDMIDaemon"

#include <ctype.h>
#include <stdint.h>
#include <sys/types.h>
#include <math.h>
#include <fcntl.h>
#include <utils/misc.h>
#include <signal.h>

#include <binder/IPCThreadState.h>
#include <utils/threads.h>
#include <utils/Atomic.h>
#include <utils/Errors.h>
#include <utils/Log.h>
#include <androidfw/AssetManager.h>


#include <linux/fb.h>
#include <sys/ioctl.h>

#include <cutils/properties.h>
#include <hdmi_service_api.h>

#include "HDMIDaemon.h"


namespace android {

// ---------------------------------------------------------------------------
#define HDMI_SOCKET_NAME        "hdmid"

#define HDMI_EVT_CONNECTED      "hdmi_connected"
#define HDMI_EVT_DISCONNECTED   "hdmi_disconnected"


#define HDMI_CMD_SET_RESOLUTION     "hdmi_preferred_resolution"
#define HDMI_CMD_DEFAULT_RESOLUTION "hdmi_default_resolution"

HDMIDaemon::HDMIDaemon() : Thread(false),
           mFrameworkSock(-1), mAcceptedConnection(-1), mUeventSock(-1),
           mHDMIUeventQueueHead(NULL), fd1(-1), mResolutionListHead(NULL)
{
}

HDMIDaemon::~HDMIDaemon() {
    ResolutionList* rl = mResolutionListHead, *rtmp;
    HDMIUeventQueue* tmp = mHDMIUeventQueueHead, *tmp1;

    while (tmp != NULL) {
        tmp1 = tmp;
        tmp = tmp->next;
        delete tmp1;
    }
    mHDMIUeventQueueHead = NULL;
    if (fd1 > 0)
        close(fd1);

    hdmi_disable();
    hdmi_exit();

    while (rl != NULL) {
        rtmp = rl;
        rl = rl->next;
        delete rtmp;
    }
    mResolutionListHead = NULL;
}

void HDMIDaemon::onFirstRef() {
    run("HDMIDaemon", PRIORITY_AUDIO);
}

sp<SurfaceComposerClient> HDMIDaemon::session() const {
    return mSession;
}


void HDMIDaemon::binderDied(const wp<IBinder>& who)
{
    requestExit();
}

status_t HDMIDaemon::readyToRun() {
    if ((mFrameworkSock = android_get_control_socket(HDMI_SOCKET_NAME)) < 0) {
        ALOGE("Obtaining file descriptor socket '%s' failed: %s",
             HDMI_SOCKET_NAME, strerror(errno));
        return -1;
    }

    if (listen(mFrameworkSock, 4) < 0) {
        ALOGE("Unable to listen on fd '%d' for socket '%s': %s",
             mFrameworkSock, HDMI_SOCKET_NAME, strerror(errno));
        return -1;
    }

    mUeventSock = hdmi_init(0);
    hdmi_enable();

    ALOGD("readyToRun: success");

    return NO_ERROR;
}

bool HDMIDaemon::threadLoop()
{
    int max = -1;
    fd_set read_fds;
    FD_ZERO(&read_fds);

    FD_SET(mFrameworkSock, &read_fds);
    if (max < mFrameworkSock)
        max = mFrameworkSock;
    FD_SET(mUeventSock, &read_fds);
    if (max < mUeventSock)
        max = mUeventSock;

    if (mAcceptedConnection != -1) {
        FD_SET(mAcceptedConnection, &read_fds);
        if (max < mAcceptedConnection)
            max = mAcceptedConnection;
    }

    struct timeval to;
    to.tv_sec = (60 * 60);
    to.tv_usec = 0;

    int ret;
    if ((ret = select(max + 1, &read_fds, NULL, NULL, &to)) < 0) {
        ALOGE("select() failed (%s)", strerror(errno));
        sleep(1);
        return true;
    }

    if (!ret) {
        return true;
    }

    if (mAcceptedConnection != -1 && FD_ISSET(mAcceptedConnection, &read_fds)) {
        if (processFrameworkCommand() == -1)
            mAcceptedConnection = -1;
    }

    if (FD_ISSET(mFrameworkSock, &read_fds)) {
        struct sockaddr addr;
        socklen_t alen;
        alen = sizeof(addr);

        if (mAcceptedConnection != -1) {
            close(mAcceptedConnection);
            mAcceptedConnection = accept(mFrameworkSock, &addr, &alen);
            return true;
        }

        if ((mAcceptedConnection = accept(mFrameworkSock, &addr, &alen)) < 0) {
            ALOGE("Unable to accept framework connection (%s)",
                strerror(errno));
        }
        else {
            mSession = new SurfaceComposerClient();
            processUeventQueue();

            if (!mDriverOnline) {
                ALOGE("threadLoop: driver not online; use state-file");
                sendCommandToFramework(false);
            }
        }

        ALOGD("threadLoop: Accepted connection from framework");
    }

    if (FD_ISSET(mUeventSock, &read_fds)) {
        if (mAcceptedConnection == -1)
            queueUevent();
        else
            processUevent();
    }

    return true;
}

bool HDMIDaemon::processUeventMessage(uevent& event)
{
    char buffer[64 * 1024];
    int count;
    char *s = buffer;
    int cmd;
    int cmd_len;
    int remaining;
    int processed = false;
    status_t err;

#define CMD_OFFSET              0
#define CMD_ID_OFFSET           4
#define CMD_LEN_OFFSET          8
#define CMD_DATA_OFFSET         12

    if ((count = recv(mUeventSock, buffer, sizeof(buffer), 0)) < 0) {
        ALOGE("Error receiving uevent (%s)", strerror(errno));
        return false;
    }

    ALOGD("uevent recvd %d bytes", count);

    remaining = count;
    if (remaining >= CMD_DATA_OFFSET)
        memcpy(&cmd_len, s + CMD_LEN_OFFSET, 4);
    else
        cmd_len = 0;
    while (remaining >= (CMD_DATA_OFFSET + cmd_len)) {
        /* Convert cmd to string */
        memcpy(&cmd, s + CMD_OFFSET, 4);
        switch(cmd) {
        case HDMI_PLUGGED_EV:
            ALOGD("HDMI_PLUGGED_EV");
            event.action = action_online;
            parseResolutionList(s + CMD_LEN_OFFSET, cmd_len + 4);
            SurfaceComposerClient::setHDMIOutputMode(HDMIOUT_ENABLE);
            ALOGI("%s HDMI plugged", __func__);
            break;
        case HDMI_UNPLUGGED_EV:
            ALOGD("HDMI_UNPLUGGED_EV");
            event.action = action_online;
            SurfaceComposerClient::setHDMIOutputMode(HDMIOUT_DISABLE);
            /*
             * Wait for 200 ms to make sure that all clients to the fb1 has performed
             * a close before the framebuffer is destroyed
             */
            usleep(200000);
            hdmi_fb_release();
            ALOGI("%s HDMI unplugged", __func__);
            break;
        case HDMI_ILLSTATE_POWERED:
            ALOGD("HDMI_ILLSTATE_POWERED_EV");
            break;
        case HDMI_ILLSTATE_UNPOWERED:
            ALOGD("HDMI_ILLSTATE_UNPOWERED_EV");
            break;
        case HDMI_ILLSTATE_UNPLUGGED:
            ALOGD("HDMI_ILLSTATE_UNPLUGGED_EV");
            break;
        case HDMI_ILLSTATE_PWRON_PLUGGED:
            ALOGD("HDMI_ILLSTATE_PWRON_PLUGGED_EV");
            break;
        default:
            break;
        }

        processed = true;

        s += CMD_DATA_OFFSET + cmd_len;
        remaining -= (CMD_DATA_OFFSET + cmd_len);
        if (remaining >= CMD_DATA_OFFSET)
            memcpy(&cmd_len, s + CMD_LEN_OFFSET, 4);
        else
            cmd_len = 0;
    }

    if (remaining)
        ALOGD("uevent not handled bytes:%d", remaining);

    return processed;
}

void HDMIDaemon::queueUevent()
{
    HDMIUeventQueue* tmp = mHDMIUeventQueueHead, *tmp1;
    while (tmp != NULL && tmp->next != NULL)
        tmp = tmp->next;
    if (!tmp) {
        tmp = new HDMIUeventQueue();
        tmp->next = NULL;
        if(!processUeventMessage(tmp->mEvent))
            delete tmp;
        else
            mHDMIUeventQueueHead = tmp;
    } else {
        tmp1 = new HDMIUeventQueue();
        tmp1->next = NULL;
        if(!processUeventMessage(tmp1->mEvent))
            delete tmp1;
        else
            tmp->next = tmp1;
    }
}

void HDMIDaemon::processUeventQueue()
{
    HDMIUeventQueue* tmp = mHDMIUeventQueueHead, *tmp1;
    while (tmp != NULL) {
        tmp1 = tmp;
        if (tmp->mEvent.action == action_offline) {
            ALOGD("processUeventQueue: event.action == offline");
            mDriverOnline = true;
            sendCommandToFramework(false);
        } else if (tmp->mEvent.action == action_online) {
            ALOGD("processUeventQueue: event.action == online");
            mDriverOnline = true;
            sendCommandToFramework(true);
        }
        tmp = tmp->next;
        delete tmp1;
    }
    mHDMIUeventQueueHead = NULL;
}

void HDMIDaemon::processUevent()
{
    uevent event;
    if(processUeventMessage(event)) {
        if (event.action == action_offline) {
            ALOGD("processUevent: event.action == offline");
            mDriverOnline = true;
            sendCommandToFramework(false);
        } else if (event.action == action_online) {
            ALOGD("processUevent: event.action == online");
            mDriverOnline = true;
            sendCommandToFramework(true);
        }
    }
}

int HDMIDaemon::parseResolutionList(const char * const buf,
        const int buflen)
{
    int payloadlen = 0;
    int entries;
    char *pbuf = (char *)buf;
    int remaining = (int) buflen;
    struct ResolutionList* rl = mResolutionListHead, *rtmp, *rprev;
    while (rl != NULL) {
        rtmp = rl;
        rl = rl->next;
        delete rtmp;
    }
    mResolutionListHead = NULL;

    if (buflen < 5) {
        ALOGE("Resolution list invalid");
        return -1;
    }

    memcpy(&payloadlen, pbuf, 4);
    pbuf += 4;
    remaining -= 4;

    if (payloadlen != remaining) {
        ALOGE("Payload length does not match received buffer size"
                "(payloadlen: %d, buflen: %d)",
                payloadlen, remaining);
        return -1;
    }

    /* Skip audio_support */
    pbuf++;
    remaining--;

    entries = (int) *pbuf++;
    remaining--;
    if ((entries <= 0) || ((entries * 2) != remaining)) {
        ALOGE("Number of entries in resolution list: %d, remaining bytes: %d",
                entries, remaining);
        return -1;
    }

    /*
     * Parse the [cea, vesaceanr] list
     *
     * NOTE: The list head will contain the default resolution
     */
    rprev = NULL;
    for (int i = 0; i < entries; i++) {
        rl = new ResolutionList();
        rl->next = NULL;
        rl->mEntry.cea = pbuf[2 * i];
        rl->mEntry.vesaceanr = pbuf[2 * i + 1];
        if (mResolutionListHead == NULL) {
            mResolutionListHead = rl;
            rprev = rl;
        } else {
            rprev->next = rl;
            rprev = rl;
        }
    }

    /*
     * Fill in the resolutions and refresh
     * rates for the parsed list
     */
    rl = mResolutionListHead;
    while (rl != NULL) {
        if (rl->mEntry.cea == 0) {
            switch (rl->mEntry.vesaceanr) {
            case 4:
                rl->mEntry.width = 640;
                rl->mEntry.height = 480;
                rl->mEntry.refreshrate = 60;
                break;
            case 9:
                rl->mEntry.width = 800;
                rl->mEntry.height = 600;
                rl->mEntry.refreshrate = 60;
                break;
            case 14:
                rl->mEntry.width = 848;
                rl->mEntry.height = 480;
                rl->mEntry.refreshrate = 60;
                break;
            case 16:
                rl->mEntry.width = 1024;
                rl->mEntry.height = 768;
                rl->mEntry.refreshrate = 60;
                break;
            case 22:
                rl->mEntry.width = 1280;
                rl->mEntry.height = 768;
                rl->mEntry.refreshrate = 60;
                break;
            case 23:
                rl->mEntry.width = 1280;
                rl->mEntry.height = 768;
                rl->mEntry.refreshrate = 60;
                break;
            case 27:
                rl->mEntry.width = 1280;
                rl->mEntry.height = 800;
                rl->mEntry.refreshrate = 60;
                break;
            case 28:
                rl->mEntry.width = 1280;
                rl->mEntry.height = 800;
                rl->mEntry.refreshrate = 60;
                break;
            case 39:
                rl->mEntry.width = 1360;
                rl->mEntry.height = 768;
                rl->mEntry.refreshrate = 60;
                break;
            case 81:
                rl->mEntry.width = 1366;
                rl->mEntry.height = 768;
                rl->mEntry.refreshrate = 60;
                break;
            default:
                rl->mEntry.width = 0;
                rl->mEntry.height = 0;
                rl->mEntry.refreshrate = 0;
                break;
            }
        } else if (rl->mEntry.cea == 1) {
            switch (rl->mEntry.vesaceanr) {
            case 1:
                rl->mEntry.width = 640;
                rl->mEntry.height = 480;
                rl->mEntry.refreshrate = 60;
                break;
            case 2:
            case 3:
            case 6:
            case 7:
                rl->mEntry.width = 720;
                rl->mEntry.height = 480;
                rl->mEntry.refreshrate = 60;
                break;
            case 4:
                rl->mEntry.width = 1280;
                rl->mEntry.height = 720;
                rl->mEntry.refreshrate = 60;
                break;
            case 5:
            case 16:
                rl->mEntry.width = 1920;
                rl->mEntry.height = 1080;
                rl->mEntry.refreshrate = 60;
                break;
            case 14:
            case 15:
                rl->mEntry.width = 1440;
                rl->mEntry.height = 480;
                rl->mEntry.refreshrate = 60;
                break;
            case 17:
            case 18:
            case 21:
            case 22:
                rl->mEntry.width = 720;
                rl->mEntry.height = 576;
                rl->mEntry.refreshrate = 50;
                break;
            case 19:
                rl->mEntry.width = 1280;
                rl->mEntry.height = 720;
                rl->mEntry.refreshrate = 50;
                break;
            case 20:
            case 31:
                rl->mEntry.width = 1920;
                rl->mEntry.height = 1080;
                rl->mEntry.refreshrate = 50;
                break;
            case 29:
            case 30:
                rl->mEntry.width = 1440;
                rl->mEntry.height = 576;
                rl->mEntry.refreshrate = 50;
                break;
            case 32:
                rl->mEntry.width = 1920;
                rl->mEntry.height = 1080;
                rl->mEntry.refreshrate = 24;
                break;
            case 33:
                rl->mEntry.width = 1920;
                rl->mEntry.height = 1080;
                rl->mEntry.refreshrate = 25;
                break;
            case 34:
                rl->mEntry.width = 1920;
                rl->mEntry.height = 1080;
                rl->mEntry.refreshrate = 30;
                break;
            case 60:
                rl->mEntry.width = 1280;
                rl->mEntry.height = 720;
                rl->mEntry.refreshrate = 24;
                break;
            case 61:
                rl->mEntry.width = 1280;
                rl->mEntry.height = 720;
                rl->mEntry.refreshrate = 25;
                break;
            case 62:
                rl->mEntry.width = 1280;
                rl->mEntry.height = 720;
                rl->mEntry.refreshrate = 30;
                break;
            default:
                rl->mEntry.width = 0;
                rl->mEntry.height = 0;
                rl->mEntry.refreshrate = 0;
                break;
            }
        }
        else {
            ALOGE("Invalid CEA parsed from hdmi service");
            rl->mEntry.width = 0;
            rl->mEntry.height = 0;
            rl->mEntry.refreshrate = 0;
        }

        ALOGD("Entry contains w:%d, h:%d, rr:%d, cea:%d, vesaceanr:%d",
                rl->mEntry.width, rl->mEntry.height, rl->mEntry.refreshrate,
                rl->mEntry.cea, rl->mEntry.vesaceanr);

        rl = rl->next;
    }

    return 0;
}

int HDMIDaemon::parseResolution(
        const char * const buf,
        const ssize_t datalen,
        __u16 *width,
        __u16 *height)
{
    __u16 w;
    __u16 h;

    if (datalen != 4)
        return -1;

    w = (__u16) buf[0];
    w |= ((__u16) buf[1]) << 8;
    h = (__u16) buf[2];
    h |= ((__u16) buf[3]) << 8;

    *width = w;
    *height = h;

    return 0;
}

int HDMIDaemon::getResolutionFromList(__u16 width,__u16 height, __u16 rr,
        int *cea, int *vesaceanr)
{
    struct ResolutionList *rl;
    int ret = -1;

    if (cea == NULL || vesaceanr == NULL)
        return -1;

    if (mResolutionListHead == NULL)
        return -1;

    rl = mResolutionListHead;
    while (rl != NULL) {
        if (rl->mEntry.width == width &&
                rl->mEntry.height == height &&
                (rr == 0 || rl->mEntry.refreshrate == rr))
        {
            *cea = rl->mEntry.cea;
            *vesaceanr = rl->mEntry.vesaceanr;
            ret = 0;
            break;
        }
        rl = rl->next;
    }

    return ret;
}

int HDMIDaemon::processFrameworkCommand()
{
    char buffer[128];
    int ret;
    ssize_t totlen;
    ssize_t commandlen;
    ssize_t datalen;

    if ((ret = read(mAcceptedConnection, buffer, sizeof(buffer) -1)) < 0) {
        ALOGE("Unable to read framework command (%s)", strerror(errno));
        return -1;
    } else if (!ret)
        return -1;

    totlen = (ssize_t)ret;

    ALOGE("Read framework command (%ld bytes)", totlen);

    /* Null terminate after read bytes */
    buffer[totlen] = 0;

    if (!strncmp(buffer, HDMI_CMD_SET_RESOLUTION,
            strlen(HDMI_CMD_SET_RESOLUTION))) {
        __u16 width;
        __u16 height;
        __u16 rr = 0;
        int cea, vesaceanr;

        ALOGD(HDMI_CMD_SET_RESOLUTION);
        commandlen = (ssize_t)(strlen(HDMI_CMD_SET_RESOLUTION) + 1);
        /* Skip the null terminator */
        datalen = totlen - commandlen - 1;

        ALOGE("Command length: %ld bytes, data length: %ld bytes",
                commandlen, datalen);

        if (datalen > 0) {
            if (parseResolution(&buffer[commandlen], datalen,
                    &width, &height) < 0) {
                ALOGE("Failed to parse resolution from input buffer (%ld bytes)",
                        datalen);
                return 0;
            }

            if (getResolutionFromList(width, height, rr, &cea, &vesaceanr) < 0) {
                ALOGE("Failed to get cea and vesaceanr from list, w:%d h:%d rr:%d"
                        ", selecting default resolution", width, height, rr);
                cea = mResolutionListHead->mEntry.cea;
                vesaceanr = mResolutionListHead->mEntry.vesaceanr;
                width = mResolutionListHead->mEntry.width;
                height = mResolutionListHead->mEntry.height;
                rr = mResolutionListHead->mEntry.refreshrate;
            }

            ALOGD("Setting resolution w:%d, h:%d, prr:%d, cea:%d, vesaceanr:%d",
                    width, height, rr, cea, vesaceanr);
            if (hdmi_resolution_set(cea, vesaceanr) < 0) {
                ALOGE("Failed to set resolution with cea:%d vesaceanr:%d",
                        cea, vesaceanr);
                return 0;
            }
        } else {
            ALOGE("Data missing from package, no preferred "
                    "resolution will be set");
        }
    } else if (!strcmp(buffer, HDMI_CMD_DEFAULT_RESOLUTION)) {
        ALOGD(HDMI_CMD_DEFAULT_RESOLUTION);
        int cea, vesaceanr;

        if (mResolutionListHead == NULL) {
            cea = 0;
            vesaceanr = 0;
        } else {
            cea = mResolutionListHead->mEntry.cea;
            vesaceanr = mResolutionListHead->mEntry.vesaceanr;
        }

        ALOGD("Setting resolution cea:%d, vesaceanr:%d",
                    cea, vesaceanr);
        if (hdmi_resolution_set(cea, vesaceanr) < 0) {
            ALOGE("Failed to set resolution with cea:%d vesaceanr:%d",
                    cea, vesaceanr);
            return 0;
        }
    }

    return 0;
}

bool HDMIDaemon::sendCommandToFramework(bool connected)
{
#ifdef HDMI_EVT_OVER_SOCKET
    char message[512];

    if (connected)
        strncpy(message, HDMI_EVT_CONNECTED, sizeof(message));
    else
        strncpy(message, HDMI_EVT_DISCONNECTED, sizeof(message));

    int result = write(mAcceptedConnection, message, strlen(message) + 1);
    ALOGD("sendCommandToFramework: '%s' %s", message, result >= 0 ?
            "successful" : "failed");
    return result >= 0;
#else
    return true;
#endif
}

// ---------------------------------------------------------------------------

}; // namespace android
