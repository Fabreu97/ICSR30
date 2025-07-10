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
#include <sys/socket.h>

#include <vector>
/***********************************************************
* DEFINE
***********************************************************/

#define SERVER_IP   "103.0.1.70"
#define MAX_USER    3

class Server {
private:
    int sock_fd;
    sockaddr address;
    std::vector<struct ClientData> client_list;

public:
    Server();
    ~Server();
    void run();
    void clientRequestHandler(struct ClientData client);

};


/***********************************************************
* END
***********************************************************/
#endif // __INCLUDED_SERVER_H__