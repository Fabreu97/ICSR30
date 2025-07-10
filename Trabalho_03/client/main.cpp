#include "client.h"
#include <string>
#include <regex>
#include <cmath>

#define MAX_PORT (pow(2,16) - 1)

void getData(std::string* ip, int* port);
const bool validIPv4(std::string ip);

int main(void) {
    std::string ipv4;
    Client client = Client();
    int port;
    do {
        getData(&ipv4, &port);
        std::cout << "***********************************" << std::endl;
        std::cout << "IPv4: " << ipv4 << std::endl;
        std::cout << "Porta: " << port << std::endl;
        std::cout << "***********************************" << std::endl;

        client.updateAddrServer(ipv4, port);
    } while(!client.connectToServer());
    client.run();

    return 0;
}

void getData(std::string* ip, int* port) {
    bool error;
    do {
        error = false;
        std::cout << "Digite o endereço IPv4 do servidor: ";
        std::getline(std::cin, *ip);
        if (validIPv4(*ip)) {
            std::cout << "Endereço IPv4 válido: " << *ip << std::endl;
        } else {
            std::cout << "Endereço IPv4 inválido!" << std::endl;
            error = true;
        }
        std::cout << "Digite o valor da porta do servidor: ";
        std::cin >> *port;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if(*port > MAX_PORT) {
            std::cout << "Porta com valor muito maior que 16 bits." << std::endl;
            error = true;
        } else if(*port < 0) {
            std::cout << "Porta com valor negativo!\n" << std::endl;
            error = true;
        }
    } while(error);
    std::cout << "Endereço e Porta válidos!" << std::endl;
}

const bool validIPv4(std::string ip) {
    int i = 0,j, number;
    int pos[4];
    if (ip.length() > 16) {
        return false;
    }
    j=0;
    for(i = 0; i < ip.length(); i++) {
        if( ip[i] == '.') {
            pos[j] = i;
            j++;
        }
        if(j == 4) {
            return false;
        }
        if( (ip[i] < '0' || ip[i] > '9') && ip[i] != '.') {
            return false;
        }
    }
    if(j != 3) {
        return false;
    }
    pos[3] = ip.length();
    for(i = 0; i < 4; i++) {
        if(i == 0) {
            j=0;
        } else {
            j = pos[i-1] + 1;
        }
        number = 0;
        while(j < pos[i]) {
            number *= 10;
            number += ip[j] - '0';
            j++;
        }
        if(number > 255) {
            return false;
        }
    }
    return true;
}