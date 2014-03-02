/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

// Implementation of the Socket class.

#include <netinet/tcp.h>
#include "socket.h"
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
using namespace std;

Socket::Socket() :
    sock(-1)
{
    memset(&internet_addr,
           0,
           sizeof(internet_addr));

    if (!createSocket()) {
        ALOGI("Unable to create a socket \n");
        exit(EXIT_FAILURE);
    }
}

Socket::Socket(int port)
{
    if (! createSocket()) {
        ALOGI("Could not create server socket. \n");
        exit(EXIT_FAILURE);
    }

    if (! bindSocket(port)) {
        ALOGI("Could not bind to port %d \n", port);
        exit(EXIT_FAILURE);
    }

    if (! startListen()) {
        ALOGI("Could not listen to socket. \n");
        exit(EXIT_FAILURE);
    }
}


Socket::~Socket()
{
    if (is_socket_valid()) {
        ::close(sock);
    }
}

bool Socket::createSocket()
{
    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (! is_socket_valid()) {
        return false;
    }

    // TIME_WAIT - argh
    int on = 1;

    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char *) &on, sizeof(on)) == -1) {
        return false;
    }

    //    on = 1;
    //    if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char*) &on, sizeof(on)) == -1)
    //    {
    //        return false;
    //    }

    return true;
}


/*
 * bind the socket to the given number
 */
bool Socket::bindSocket(const int port)
{
    if (! is_socket_valid()) {
        return false;
    }

    internet_addr.sin_family      = AF_INET;
    internet_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    internet_addr.sin_port        = htons(port);
    /* call bind from #include <sys/socket.h> from h file
     */
    int bind_return = bind(sock,
                           (struct sockaddr *) &internet_addr,
                           sizeof(internet_addr));

    if (bind_return == -1) {
        ALOGI("Socket::bind failed. errno(%d)", errno);
        return false;
    }

    return true;
}

/*
 * let the socket listen , return ok if success false if failure
 */
bool Socket::startListen() const
{
    if (! is_socket_valid()) {
        return false;
    }

    /*
     * call listen from #include <sys/socket.h>
     */
    int listen_return = ::listen(sock, max_nbr_of_connections);


    if (listen_return == -1) {
        return false;
    }

    return true;
}

/*
 * accept a connection return ok if success false if failure
 */
bool Socket::acceptConnection(Socket &new_socket) const
{
    int addr_length = sizeof(internet_addr);
    new_socket.sock = accept(sock, (sockaddr *) &internet_addr, (socklen_t *) &addr_length);

    if (new_socket.sock <= 0) {
        return false;
    } else {
        return true;
    }
}

/*
 * send data throw the socket
 */
bool Socket::sendData(const char *buffer) const
{
    //sendData call send from #include <sys/socket.h>

    int status = ::send(sock, buffer, strlen(buffer), MSG_NOSIGNAL);

    if (status == -1) {
        ALOGI("Socket::send failed. errno(%d)", errno);
        return false;
    } else {
        return true;
    }
}



int Socket::readData(char *buf) const
{
    char buffer[max_number_of_received_char+1];
    int returnValue = 0;

    memset(buffer, 0, max_number_of_received_char + 1);
    // call the recv from <sys/socket.h>
    int status = ::recv(sock, buffer, max_number_of_received_char, 0);

    if (status == -1) {
        ALOGI("Socket::recv failed. errno(%d) \n", errno);
    } else if (status == 0) {
        // we get here when we disconnect from the other side, disconnect and exit
        ALOGI("Client disconnected. Closing connection fd=%d \n", sock);
        ::close(sock);
        //exit(EXIT_SUCCESS);
    } else {
        memcpy(buf, buffer, strlen(buffer));
    }

    return status;
}

bool Socket::connectSocket(const char *host, const int port)
{
    if (! is_socket_valid()) {
        return false;
    }

    internet_addr.sin_family = AF_INET;
    internet_addr.sin_port = htons(port);

    int status = inet_pton(AF_INET, host, &internet_addr.sin_addr);

    if (errno == EAFNOSUPPORT) {
        return false;
    }

    status = connect(sock, (sockaddr *) &internet_addr, sizeof(internet_addr));

    if (status == 0) {
        return true;
    } else {
        return false;
    }
}

void Socket::set_non_blocking_mode(const bool flag)
{
    int opts;

    opts = fcntl(sock, F_GETFL);

    if (opts < 0) {
        return;
    }

    if (flag) {
        opts = (opts | O_NONBLOCK);
    } else {
        opts = (opts & ~O_NONBLOCK);
    }

    fcntl(sock,  F_SETFL, opts);
}

void Socket::closeConnection()
{
    ::close(sock);
}

void Socket::writeToClient(const char *str)
{
    sendData(str);
}

int Socket::doSelect()
{
    FD_ZERO(&read_fd);
    FD_SET(sock, &read_fd);
    return select(sock + 1, &read_fd, NULL, NULL, NULL);
}

