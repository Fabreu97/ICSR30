#include "server.h"

unsigned int Server::n_user = 0;

/*
 * AF_INET      =   IPv4 Internet protocols
 * SOCK_STREAM  =   Provides sequenced, reliable, two-way, connection-based
                    byte streams.  An out-of-band data transmission  mecha‐
                    nism may be supported.
 * INADDR_ANY   =   Escuta em todas as interfaces eth0, loopback,...
 */

Server::Server(unsigned max_client, unsigned short int port):
    max_client(max_client),
    port(port)
{   
    std::string path = "./shared_files";

    for(const auto& entry: std::filesystem::directory_iterator(path)) {
        if(entry.is_regular_file()) {
            std::string fname = entry.path().filename().string();
            std::cout << fname << std::endl;
            std::cout << entry.path().string() << std::endl;
            this->map_sha256.insert(std::make_pair(fname, get_file_sha256(entry.path().string())));
        }
    }

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

    // 
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
        client.name = std::string("Unknown ") + std::to_string(n_user);
        n_user++;
        client.acknowledge = false;

        /*Adiciona o endereço do cliente no vector*/
        this->client_datas.push_back(client);
        std::cout << "Usuarios onlines: " << client_datas.size() << std::endl;
        std::thread thread(&Server::client_service, this, client);
        thread.detach();
    }
}

void Server::client_service(struct ClientData client) {
    Packet received_p, shipping_p;
    ssize_t bytes_send, bytes_recv, bytes_read;
    FILE* file_read = NULL;
    std::string path, file_name;
    char buffer[LENGTH];
    std::vector<ClientData>::iterator it;
    do {
        //ssize_t send(int sockfd, const void *buf, size_t len, int flags);
        //ssize_t recv(int sockfd, void *buf, size_t len, int flags);
        bytes_recv = recv(client.fd, (Packet*)&received_p, sizeof(Packet), 0);

        if(bytes_recv < 0) {
            std::cerr << "Error" << std::endl;
        } else if(bytes_recv == 0) {
            break;
        } else {
            // servidor irá processar o pacote
            switch(received_p.flag) {
                case REQ:
                    if(received_p.payload != nullptr) {
                        if (strncmp(received_p.payload, "MSG/", 4) == 0) {
                            std::string msg = client.name + std::string(": ") + std::string(received_p.payload + 4);
                            fillPacket(&shipping_p, 0, MSG, msg.length(), msg);
                            for(it = client_datas.begin(); it != client_datas.end(); ++it) {
                                if(*it != client) {
                                    bytes_send = send(it->fd, (Packet*)&shipping_p, sizeof(Packet), 0);
                                }
                            }
                            fillPacket(&shipping_p, 0, ACK, strlen("Success\0"), (byte*)"Success\0");
                        } else if(strncmp(received_p.payload, "GET/", 4) == 0) {
                            path = std::string("./shared_files/") + std::string(received_p.payload + 4);
                            file_name = std::string(received_p.payload + 4);
                            if(file_exists(received_p.payload + 4)) {
                                if (file_read != NULL) {
                                    fclose(file_read);
                                    file_read = NULL;
                                }
                                file_read = fopen(path.c_str(), "rb");
                                fseek(file_read, received_p.number, SEEK_SET);
                                bytes_read = fread(buffer, sizeof(char), LENGTH, file_read);

                                // criando pacote DATA
                                shipping_p.number = received_p.number;
                                shipping_p.flag = DATA;
                                shipping_p.length = bytes_read;
                                memcpy(shipping_p.payload, buffer, bytes_read);
                            } else {
                                // enviar um ACK(FALHA)
                                fillPacket(&shipping_p, 0, ACK, strlen("Failure\0"), (byte*)"Failure\0");
                            }
                        } else if(strncmp(received_p.payload, "IDT/", 4) == 0) {
                            client.name = std::string(received_p.payload + 4);
                            fillPacket(&shipping_p, 0, ACK, strlen("Success\0"), (byte*)"Success\0");
                        }
                    }
                    break;
                case ACK:
                    bytes_read = 0;
                    if(strcmp(received_p.payload, "Success\0") != 0) { 
                        if(file_read != NULL) {
                            fseek(file_read, received_p.number, SEEK_SET);
                            bytes_read = fread(buffer, sizeof(char), LENGTH, file_read);
                        }
                        // criando pacote DATA
                        if(bytes_read > 0) {
                            shipping_p.number = received_p.number;
                            shipping_p.flag = DATA;
                            shipping_p.length = bytes_read;
                            memcpy(shipping_p.payload, buffer, bytes_read);
                        } else {
                            // FIN_DATA
                            if(this->map_sha256.find(file_name) == this->map_sha256.end()) {
                                map_sha256.insert(std::make_pair(file_name, get_file_sha256(path)));
                                fillPacket(&shipping_p, 0, FIN_DATA, this->map_sha256[file_name].size(), this->map_sha256[file_name]);
                            }
                            fillPacket(&shipping_p, 0, FIN_DATA, this->map_sha256[file_name].size(), this->map_sha256[file_name]);
                        }
                    }
                    break;
                default:
                    break;
            }
            if(shipping_p.flag == ACK || shipping_p.flag == DATA || shipping_p.flag == MSG || shipping_p.flag == FIN_DATA) {
                bytes_send = send(client.fd, (Packet*)&shipping_p, sizeof(Packet), 0);
            }
        }
    }while(received_p.flag != FIN && received_p.flag != FIN_ACK);
    client_datas.erase(std::remove(client_datas.begin(), client_datas.end(), client), client_datas.end());
    close(client.fd);
}