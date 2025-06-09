/***********************************************************
 * Author: Fernando P. G. de Abreu
 * Date: 07/06/2025
 ***********************************************************
 * 
 * Arquivo que define os dados pertinentes do cliente para o servidor
 * 
 ***********************************************************
 */

#ifndef __INCLUDED_CLIENT_DATA_H__
#define __INCLUDED_CLIENT_DATA_H__

#include <netinet/in.h>
#include <string>

struct ClientData {
    int fd;
    sockaddr_in addr;
    std::string name;
    bool acknowledge;
};

#endif // __INCLUDED_CLIENT_DATA_H__