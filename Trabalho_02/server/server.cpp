#include "server.h"

/*
 * AF_INET      =   IPv4 Internet protocols
 * SOCK_STREAM  =   Provides sequenced, reliable, two-way, connection-based
                    byte streams.  An out-of-band data transmission  mecha‐
                    nism may be supported.
    INADDR_ANY  =   Escuta em todas as interfaces eth0, loopback,...
 */

Server::Server(unsigned max_client, unsigned short int port):
    max_client(max_client),
    port(port)
{
    // criar o socker
    this->server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(this->server_fd < 0) {
        perror("Erro ao criar socket!\n");
        close(this->server_fd);
        exit(EXIT_FAILURE);
    }
    
    // Configurando o endereço do servidor
    this->server_addr.sin_family = AF_INET;
    this->server_addr.sin_addr.s_addr = INADDR_ANY;
    this->server_addr.sin_port = htons(port);

    // Vincular o socket ao endereço
    if(bind(this->server_fd, (sockaddr*)&this->server_addr, sizeof(server_addr)) < 0) {
        perror("Erro no bind!\n");
        close(this->server_fd);
        exit(EXIT_FAILURE);
    }

    if(listen(this->server_fd, (int)max_client) < 0) {
        perror("Erro ao liste!\n");
        close(this->server_fd);
        exit(EXIT_FAILURE);
    }
}

Server::~Server() {
    close(this->server_fd);
}

void Server::run() {
    while(true) {
        socklen_t addr_len;
        sockaddr_in client_addr;
        std::cout << "Esperando Novas Conexçoes . . .\n";
        int client_fd = accept(this->server_fd, (sockaddr*)&client_addr, &addr_len);
        if(client_fd < 0) {
            perror("Conexão com cliente não aceita!\n");
            continue;
        }
        
        ClientData client;
        client.fd = client_fd;
        client.addr = client_addr;
        client.name = "Unknown";
        client.acknowledge = false;

        /*Adiciona o endereço do cliente no vector*/
        this->client_datas.push_back(client);
        std::thread thread(&Server::client_service, this, client);
        thread.detach();
    }
}

void Server::client_service(struct ClientData client) {
    
}