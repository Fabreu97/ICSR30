/***********************************************************
 * Author:  Fernando P. G. de Abreu
 * Date:    08/06/2025
 ***********************************************************
 * 
 * Arquivo que define o compartamento do Cliente
 * 
 ***********************************************************
 */

#ifndef __INCLUDED_CLIENT_H__
#define __INCLUDED_CLIENT_H__

/***********************************************************
* INCLUDED
***********************************************************/

#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <arpa/inet.h>
#include "../utils/packet.h"

/***********************************************************
* DEFINES
***********************************************************/

class Client {
private:
    int socket_fd;
    sockaddr_in server_addr;
    std::string name;
    std::string ipv4_server;
    int port;

public:
    Client(const std::string ipv4 = "0.0.0.0", const int port = 12345);
    ~Client();

    void updateAddrServer(const std::string ipv4, const int port);
    const bool connectToServer();
    void run();
};

#endif // __INCLUDED_CLIENT_H__