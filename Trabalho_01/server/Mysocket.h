/*****************************************************************************
 * This file aims to abstract the processes of network configuration, sending
 * and receiving messages, ...
 *****************************************************************************
 * Author:  Fernando Abreu
 * Date:    2025-05-10
 *****************************************************************************
 */

#ifndef __INCLUDED_MY_SOCKET_H__
#define __INCLUDED_MY_SOCKET_H__

/*****************************************************************************
 * LIBRARY
 *****************************************************************************/
#include "packet.h"
#include <unistd.h> // close()
#include <sys/socket.h> //socket()
#include <netinet/in.h> // sockaddr_in

/*****************************************************************************
 * MACROS 
 *****************************************************************************/

typedef struct {
    int socket_fd; // Socket file descriptor
    char ip[16];
    int port;
    struct sockaddr_in server_addr; // Server address
} MySocket;

void create_MySocket(MySocket* socket, const char* ip, int port); // bind incluso
bool bind_MySocket(MySocket* socket, const char* ip, int port);
// TO DO



/*****************************************************************************/
 #endif // __INCLUDED_MY_SOCKET_H__