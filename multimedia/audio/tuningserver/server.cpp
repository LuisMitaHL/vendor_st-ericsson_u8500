/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "server.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "admRequest.h"
#include "ste_adm_client.h"
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#ifdef X86_TARGET
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;
#endif

Server::Server()
    : adm_id(0)
{
    ste_adm_res_t res = ste_adm_client_set_tuning_mode(1);
    if (res != STE_ADM_RES_OK) {
        ALOGE("Server constructor: ste_adm_client_set_tuning_mode failed res=%d", res);
    }
}



Server::~Server()
{
    if (serverSck != 0) {
        delete serverSck;
    }

    ste_adm_res_t res = ste_adm_client_set_tuning_mode(0);
    if (res != STE_ADM_RES_OK) {
        ALOGE("Server destructor: ste_adm_client_set_tuning_mode failed res=%d", res);
    }

    ste_adm_client_disconnect(adm_id);
    //close the adm socket
    close(adm_id);
}

/**
  * non gui method for starting the server by invoking this method from the object not user interface interaction
  */
void Server::startServer(int portNumber)
{
    int connected = 0;

    ALOGI("TuningServer started but communication not established yet....\n");

    Socket server(portNumber);

    while (true) {
        ALOGI("Waiting for new connection...\n");
        server.acceptConnection(new_server_sock);

        ALOGI("Connection established fd=%d\n", new_server_sock.getfileDescriptor());
        // new_server_sock.doSelect();
        connected = 1;

        AdmRequest admHndl;
        initAdmSockets();
        new_server_sock.set_non_blocking_mode(true);
        // both sockets have same fd_set
        new_server_sock.setFd_Set(rfds);

        // new_server_sock.doSelect();
        ste_adm_client_request_active_device_list(adm_id);

        while (connected) {
            FD_ZERO(&rfds);
            fd_set pc_socket = new_server_sock.get_fd_set();
            FD_SET(adm_id, &rfds);
            FD_SET(new_server_sock.getfileDescriptor(), &rfds);
            int max = (adm_id > new_server_sock.getfileDescriptor()) ? adm_id : new_server_sock.getfileDescriptor();
            int statusValue;
            static char *resText = NULL;
            ALOGD("Tuning_server: Entering select()\n");
            statusValue = select(max + 1, &rfds, NULL, NULL, NULL);
            ALOGD("Value of select %d \n", statusValue);

            if (-1 == statusValue) {
                connected = 0;
                continue;
            }

            char *bufchar = NULL;

            if (FD_ISSET(new_server_sock.getfileDescriptor(), &rfds)) {
                ALOGD("reading from pc-client  socket \n");
                bufchar = receive_message_from_client(new_server_sock, resText);

                if (!bufchar) {
                    connected = 0;
                    break;
                }

                admHndl.run_adm_update_db(bufchar);
                admHndl.run_adm_rescan_config_db(bufchar);
                admHndl.run_adm_rescan_param_db(bufchar);
                admHndl.run_adm_rescan_voice_chain(bufchar);

                char *respText = NULL;

                if (NULL != (respText = admHndl.run_scan_ping_request(bufchar))) {
                    char buf[200];
                    sprintf(buf, "PingResp %s\n", respText);
                    sendMessageToClient(buf);
                    free(respText);
                }

                int admRes = 0;

                if (0 != admHndl.run_scan_reopen_db(bufchar, &admRes)) {
                    char response[20];
                    sprintf(response, "ReopenDBResp %d\n", admRes);
                    sendMessageToClient(response);
                }

                // Check if there is uncomplete data after last endTag
                admHndl.checkForResidualText(bufchar, &resText);
            }

            else if (FD_ISSET(adm_id, &rfds)) {
                ALOGD("reading from adm socket \n");
                readDataFromAdm();
                ste_adm_client_request_active_device_list(adm_id);
            }

            if (bufchar != NULL) {
                free(bufchar);
            }

            bufchar = NULL;
        }
    }
}


char *Server::receive_message_from_client(Socket &sck, char *resText)
{
    // sck.set_non_blocking_mode(false);
    int totalsize = 0;
    int lengthOfBuffer = 0;
    int sizeOfReadedData = 0;
    int sizeOfResText = resText == NULL ? 0 : strlen(resText);

    // this guarantee us that we have a null terminated string
    char buffer_temp[max_number_of_received_char+1];

    memset(buffer_temp, 0, (max_number_of_received_char + 1));

    if (0 == sck.readData(buffer_temp)) {
        // Client disconnected
        return NULL;
    }

    char  *variableBuffer = (char *)malloc(max_number_of_received_char + sizeOfResText + 1);
    memset(variableBuffer, 0, (max_number_of_received_char + sizeOfResText + 1));
    sizeOfReadedData = strlen(buffer_temp) + 1;

    if (sizeOfResText > 0) {
        ALOGD("receive_message_from_client: Using residual text: '%s'\n", resText);
        memcpy(variableBuffer, resText, sizeOfResText);
    }

    memcpy(variableBuffer + sizeOfResText, buffer_temp, sizeOfReadedData);

    char *pointerForNextReading;

    memset(buffer_temp, 0, (max_number_of_received_char + 1));

    while ((lengthOfBuffer = sck.readData(buffer_temp)) > 0) {
        totalsize  = strlen(variableBuffer);
        sizeOfReadedData = strlen(buffer_temp);
        variableBuffer = (char *) realloc(variableBuffer, (totalsize + sizeOfReadedData + 1));

        if (variableBuffer == NULL) {
            ALOGE("realloc failed, the program will terminate \n");
            exit(EXIT_FAILURE);
        }

        // set all position after the previous text in variableBuffer to 0 so we can have a 0 terminated string
        pointerForNextReading = & variableBuffer[totalsize];
        memset(pointerForNextReading, 0, sizeOfReadedData + 1);

        strcat(variableBuffer, buffer_temp);
        totalsize +=  sizeOfReadedData;

        memset(buffer_temp, 0, max_number_of_received_char + 1);
    }

    // Remove new-line character at end if any
    if (variableBuffer[ strlen(variableBuffer) - 1 ] == '\x0a') {
        variableBuffer[ strlen(variableBuffer) - 1 ] = '\0';
    }

    if (variableBuffer[ strlen(variableBuffer) - 1 ] == '\x0a' ||  variableBuffer[ strlen(variableBuffer) - 1 ] == '\r') {
        variableBuffer[ strlen(variableBuffer) - 1 ] = '\0';
    }

    ALOGD("receive_message_from_client: Read %d(%d) bytes from socket.\n", sizeOfReadedData, totalsize);
    return variableBuffer;
}


void Server::sendMessageToClient(const char *buffer)
{
    ALOGD("Send message to client called \n");
    ALOGD("Length of text we send %d \n", strlen(buffer));
    new_server_sock.writeToClient(buffer);
}


void Server::initAdmSockets()
{
    if (adm_id != 0) {
        ste_adm_client_disconnect(adm_id);
    }

    adm_id = ste_adm_client_connect();

    if (adm_id < 0) {
        exit(0);
    }
    int x;

    ste_adm_res_t res;
    FD_ZERO(&rfds);
    FD_SET(adm_id, &rfds);
    x=fcntl(adm_id,F_GETFL,0);
    fcntl(adm_id,F_SETFL,x | O_NONBLOCK);  // set to non blocking mode
}

void Server::readDataFromAdm()
{
	ste_adm_res_t result_ste_adm;
    char str[4000];
    memset(str, 0, 4000);
    char beginTag[] = "BEGIN";
    char endTag[] = "END";
    active_device_t *devices_p = NULL;
    int size = 0;
    result_ste_adm = ste_adm_client_read_active_device_list(adm_id, &devices_p, &size);

    ALOGD("Value of return value from ste_adm_read_active.... %d \n", result_ste_adm);
    ALOGD("Value of size %d \n", size);
    //if we get STE_ADM_RES_ERR_MSG_IO than we have a broken pipe, no connection to admsrv
    if(result_ste_adm == STE_ADM_RES_ERR_MSG_IO)
    {
    	ALOGI("Broken connection between adm and tuning server");
    	sendMessageToClient("Broken connection between adm and tuning server");
    	free(devices_p);
    	//sleep some second(5 in this case) just to insure that the data is sent before exiting, than terminate
    	sleep(5);
    	exit(0);

    }

    int i;

    if (size == 0) {

    		strcat(str, beginTag);
    		strcat(str, endTag);

    } else {
        for (i = 0; i < size; i++) {
            strcat(str, beginTag);
            strcat(str, devices_p[i].dev_name);
            char app[10], voice[10], sampleRate[10];
            memset(app, 0, 10);
            memset(voice, 0, 10);
            memset(sampleRate, 0, 10);
            sprintf(app, "%d", devices_p[i].app_active);
            sprintf(voice, "%d", devices_p[i].voice_active);
            sprintf(sampleRate, "%d", devices_p[i].voicecall_samplerate);
            strcat(str, " ");
            strcat(str, app);
            strcat(str, " ");
            strcat(str, voice);
            strcat(str, " ");
            strcat(str, sampleRate);
            strcat(str, endTag);
        }
    }

    strcat(str, "\n");
    sendMessageToClient(str);
    free(devices_p);
    ALOGD("received string from adm-socket %s \n", str);




}



