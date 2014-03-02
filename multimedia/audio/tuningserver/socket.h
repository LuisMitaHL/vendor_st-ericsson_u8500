/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

// Definition of the Socket class

#ifndef SOCKET_H
#define SOCKET_H


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
//#include <string>
#include <arpa/inet.h>
#include "tuningServer_log.h"

//using namespace std;

#define max_nbr_of_connections  3
#define max_number_of_received_char 400

class Socket
{
public:
    Socket();
    Socket(int port);
    ~Socket();

    // Server initialization
    bool createSocket();
    //bind the socket
    bool bindSocket(const int port);
    //start the server to listen
    bool startListen() const;
    // Accept connection
    bool acceptConnection(Socket &) const;

    // connect the socket
    bool connectSocket(const char *host, const int port);

    /**
     * This functions sends data throw the sockets
     * @param string to send
     */
    bool sendData(const char *data) const;

    /**
     * This function receives data from the sockets
     */
    int readData(char *buf) const;

    void set_non_blocking_mode(const bool);

    bool is_socket_valid() const {
        return sock != -1;
    }


    // close the connection
    void closeConnection();

    void writeToClient(const char *);

    //  void readFromClient(string & str );
    /**
     * this method perform a select statemend on the socket
     */
    int doSelect();

    /*
     * This function return the gile descriptor for the socket
     */
    int getfileDescriptor() {
        return sock;
    }

    void setFd_Set(fd_set fd) {
        read_fd = fd;
    }

    fd_set get_fd_set() {
        return read_fd;
    }

private:
    int sock;
    //structure containing an internet address
    sockaddr_in internet_addr;
    fd_set read_fd;
};


#endif
