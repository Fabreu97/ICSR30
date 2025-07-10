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
    bool isOn;

    bool operator==(const ClientData& other) const {
        if(fd == other.fd && name == other.name && acknowledge == other.acknowledge) {
            return true;
        }
        return false;
    }

    bool operator!=(const ClientData& other) const {
        if(fd == other.fd && name == other.name && acknowledge == other.acknowledge) {
            return false;
        }
        return true;
    }
};


#endif // __INCLUDED_CLIENT_DATA_H__