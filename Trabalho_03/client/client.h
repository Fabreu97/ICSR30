/***********************************************************
 * Author:  Fernando P. G. de Abreu
 * Date:    12/06/2025
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

// Standard
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <unistd.h>
#include <string>

// Socket
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>

// Threads
#include <thread>
#include <mutex>
#include <atomic>

// SHA-256 and Packets
#include "../utils/packet.h"
#include "../utils/utils.h"

/***********************************************************
* DEFINES
***********************************************************/

#define MAX_ATEMPTS 3

class Client {
private:
    int socket_fd;
    sockaddr_in server_addr;
    std::string name;
    std::string ipv4_server;
    int port;
    unsigned ack_client;
    std::string file_name;

public:
    Client(const std::string ipv4 = "0.0.0.0", const int port = 12345);
    ~Client();

    void updateAddrServer(const std::string ipv4, const int port);
    const bool connectToServer();
    const bool requestIDT(const std::string payload);
    void requestMSG(const std::string payload);
    void handleIncomingData();
    void run();
};

#endif // __INCLUDED_CLIENT_H__