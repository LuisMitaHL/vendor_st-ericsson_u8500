/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef SERVER_H
#define SERVER_H

#include "socket.h"
#include "tuningServer_log.h"


class Server
{


public:
    Server();
    ~Server();
    void startServer(int);
    //Method for creating a string used for testing purpose

private:
    Socket *serverSck;
    char *receive_message_from_client(Socket &srv, char *resText);
#ifdef X86_TARGET
    void read_content_from_file_to_string(string &str);
#endif

    /**
      * This method send text to client
      * @param  str the string we want to send to the client
      */
    void sendMessageToClient(const char *);
    // private method used for test purposes
    void saveContentToFile(char *filename, char *content);

    /**
     * This method initate the adm sockets needed for communication with adm sockets
     * which means that it initiate to a appropriate values and make a select on the socket
     */
    void initAdmSockets();

    /**
     * This value reads data from adm and send it to the client in this case the audio tuning tool
     *
     */
    void readDataFromAdm();




    // Server socket (internet sockets)
    Socket new_server_sock;

    // ste-adm socket, (inter process socket)
    int  adm_id;
    fd_set rfds;
};

#endif // SERVER_H
