#include "client.h"

std::atomic<bool> running{true};
std::mutex mutex;

Client::Client(const std::string ipv4, const int port):
    ipv4_server(ipv4), ack_client(0), port(port)
{
    this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(this->socket_fd < 0) {
        std::cerr << "Erro ao criar socket . . .\n";
        close(this->socket_fd);
        exit(EXIT_FAILURE);
    }

    this->server_addr.sin_family = AF_INET;
    this->server_addr.sin_port = htons(port);

    // Converter endereço IP
    if (inet_pton(AF_INET, ipv4.c_str(), &this->server_addr.sin_addr) <= 0) {
        std::cerr << "Endereço inválido\n";
        close(this->socket_fd);
        exit(EXIT_FAILURE);
    }
}

Client::~Client() {
    close(this->socket_fd);
}

void Client::updateAddrServer(const std::string ipv4, const int port) {
    this->ipv4_server = ipv4;
    this->port = port;
    this->server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ipv4.c_str(), &this->server_addr.sin_addr) <= 0) {
        std::cerr << "Endereço inválido\n";
        // Alterar para não excluir, mas retorna uma flag que posso trocar os dados para endereço valido
        close(this->socket_fd);
        exit(EXIT_FAILURE);
    }
}

const bool Client::connectToServer() {
    // Conectar ao servidor
    if( connect(this->socket_fd, (sockaddr*)&this->server_addr, (socklen_t)sizeof(this->server_addr)) < 0) {
        std::cerr << "Conexão Falhou . . .\n";
        return false;
    }
    return true;
}

void Client::handleIncomingData() {
    char msg_failure[10] = "Failure\0";
    Packet receiving_p, shipping_p;
    ssize_t bytes_recv, bytes_send;
    FILE* file_write = NULL;
    while(running) {
        bytes_recv = recv(this->socket_fd, (Packet*)&receiving_p, sizeof(Packet), 0);
        if(bytes_recv < 0) {
            std::lock_guard<std::mutex> lock(mutex);
            std::cerr << "===========TIMEOUT===========" << std::endl;
        } else if (bytes_recv == 0) {
            std::cout << "Servidor está OFFLINE!\n";
            exit(EXIT_FAILURE);
            break;
        } else {
            switch(receiving_p.flag) {
                case DATA:
                    if(receiving_p.number == this->ack_client) {
                        if(receiving_p.number == 0) {
                            if(file_write != NULL) {
                                this->ack_client = 0;
                                fclose(file_write); 
                            }
                            file_write = fopen(this->file_name.c_str(), "wb");
                        }
                        fwrite(receiving_p.payload, 1, receiving_p.length, file_write);
                        ack_client += receiving_p.length;
                        fillPacket(&shipping_p, ack_client, ACK, 0, (byte*)"\0");
                    }
                    break;
                case ACK:
                    ack_client = 0;
                    //não envio pacotes resposta
                    std::cout << "SERVER SEND ACK: " << receiving_p.payload << std::endl;
                    receiving_p.flag = FIN;
                    break;
                case FIN_DATA:
                    this->ack_client = 0;
                    if(file_write != NULL) {
                        this->ack_client = 0;
                        fclose(file_write); 
                    }
                    //não envio pacotes resposta
                    if (strcmp(receiving_p.payload, get_file_sha256(file_name).c_str()) == 0) {
                        std::cout << "Arquivo Verificado com HASH SHA-256." << std::endl;
                    } else {
                        std::cout << "Falha na verificação SHA-256." << std::endl;
                    }
                    break;
                case MSG:
                    std::cout << std::endl << receiving_p.payload << std::endl;
                    fillPacket(&shipping_p, 0, ACK, strlen("Success\0"), (byte*)"Success\0");
                    break;
                case FIN:
                    
                    break;
                default:
                    std::cout << "Pacote Desconhecido!" << std::endl;
                    break;
            }
        }
        if(receiving_p.flag == DATA) {
            bytes_send = send(this->socket_fd, (Packet*)&shipping_p, sizeof(Packet), 0);
        }
    }
    if(file_write != NULL) {
        fclose(file_write);
    }
}

void Client::run() {
    Packet shipping_p;
    ssize_t bytes_send;
    std::string input, command;
    std::thread thread(&Client::handleIncomingData, this);
    thread.detach();
    std::cout << "*************************************" << std::endl;
    std::cout << "Comando Get:  GET/nome_do_arquivo.ext" << std::endl;
    std::cout << "Comando MSG:  MSG/mensagem!" << std::endl;
    std::cout << "\tA mensagem será enviada para todos " << std::endl; 
    std::cout << "\tos clientes do server!" << std::endl;
    std::cout << "Comando IDT:  IDT/nome_de_usuario    " << std::endl;
    std::cout << "Comando exit: EXIT!" << std::endl;
    std::cout << "*************************************" << std::endl;
    while(true) {
        std::getline(std::cin, input);
        if(input.length() > 4) {
            command = input.substr(0,4);
            if(command == "GET/" || command == "MSG/" || command == "IDT/") {
                //envio a requisição
                if(command == "GET/") {
                    this->file_name = input.substr(4);
                }
                fillPacket(&shipping_p, 0, REQ, input.length(), input);
                bytes_send = send(this->socket_fd, (Packet*)&shipping_p, sizeof(Packet), 0);
            }
        }
    }

}