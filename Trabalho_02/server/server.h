/***********************************************************
 * Author:  Fernando P. G. de Abreu
 * Date:    07/06/2025
 ***********************************************************
 * 
 * Arquivo que define o compartamento do Servidor
 * 
 ***********************************************************
 */

#ifndef __INCLUDED_SERVER_H__
#define __INCLUDED_SERVER_H__

/***********************************************************
* INCLUDED
***********************************************************/

#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include <vector>
#include "clientdata.h"
#include <filesystem>
#include <map>
#include "../utils/utils.h"
#include "../utils/packet.h"

/***********************************************************
* DEFINES
***********************************************************/

#define MAX_CLIENT 3
#define SERVER_PORT 12345

class Server {
private:
    static unsigned int n_user;

    int server_fd;
    unsigned int max_client;
    unsigned short int port;
    unsigned int number_of_connections;
    sockaddr_in server_addr;
    std::vector<ClientData> client_datas;
    std::map<std::string, std::string> map_sha256;

public:
    Server(unsigned int max_client = MAX_CLIENT, unsigned short int port = SERVER_PORT);
    ~Server();

    void run();
    void client_service(struct ClientData client);
};

/***********************************************************
* END
***********************************************************/
#endif // __INCLUDED_SERVER_H__