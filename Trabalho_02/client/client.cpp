#include "client.h"

Client::Client(const std::string ipv4, const int port):
    ipv4_server(ipv4), port(port)
{
    this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(this->socket_fd < 0) {
        perror("Erro ao criar socket . . .\n");
        close(this->socket_fd);
        exit(EXIT_FAILURE);
    }

    this->server_addr.sin_family = AF_INET;
    this->server_addr.sin_port = htons(port);

    // Converter endereço IP
    if (inet_pton(AF_INET, ipv4.c_str(), &this->server_addr.sin_addr) <= 0) {
        perror("Endereço inválido\n");
        close(this->socket_fd);
        exit(EXIT_FAILURE);
    }
}

void Client::updateAddrServer(const std::string ipv4, const int port) {
    this->ipv4_server = ipv4;
    this->port = port;
    this->server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ipv4.c_str(), &this->server_addr.sin_addr) <= 0) {
        perror("Endereço inválido\n");
        // Alterar para não excluir, mas retorna uma flag que posso trocar os dados para endereço valido
        close(this->socket_fd);
        exit(EXIT_FAILURE);
    }
}
const bool Client::connectToServer() {
    // Conectar ao servidor
    if( connect(this->socket_fd, (sockaddr*)&this->server_addr, (socklen_t)sizeof(this->server_addr)) < 0) {
        perror("Conexão Falhou . . .\n");
        return false;
    }
    return true;
}

Client::~Client() {
    close(this->socket_fd);
}

void Client::run() {
    std::string input;
    while(true) {
        std::cout << "*************************************" << std::endl;
        std::cout << "Comando Get:  GET/nome_do_arquivo.ext" << std::endl;
        std::cout << "Comando msg:  MSG/mensagem!" << std::endl;
        std::cout << "\tA mensagem será enviada para todos " << std::endl << "\tos clientes do server!" << std::endl;
        std::cout << "Comando exit: EXIT!" << std::endl;
        std::cout << "*************************************" << std::endl;
        std::getline(std::cin, input);
        
        

    }

}