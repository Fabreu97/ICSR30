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

const bool Client::requestGET(const std::string payload) {
    Packet shipping_p, receiving_p;
    struct timeval timeout, start, end;
    timeout.tv_sec = INITIAL_TIMEOUT_SEC;
    timeout.tv_usec = INITIAL_TIMEOUT_USEC;

    // Enviar requisição do arquivo e verificar se foi recebido
    // ssize_t send(int sockfd, const void *buf, size_t len, int flags);
    // ssize_t recv(int sockfd, void *buf, size_t len, int flags);
    unsigned short int attempts = 0;
    receiving_p.flag = REQ;
    int recv_bytes, send_bytes;
    bool flag_timeout = false;
    fillPacket(&shipping_p, 0, REQ, payload.length(), payload.c_str());
    while(attempts < 3 && receiving_p.flag != ACK && receiving_p.flag != FIN && receiving_p.flag != DATA) {
        if (!flag_timeout) {
            gettimeofday(&start, NULL);
        }
        send_bytes = (int)send(this->socket_fd, (Packet*)&shipping_p, sizeof(Packet), 0);
        recv_bytes = (int)recv(this->socket_fd, (Packet*)&receiving_p, sizeof(Packet), 0);
        if (recv_bytes < 0) {
            flag_timeout = true;
            std::cerr << "---------TIMEOUT----------" << std::endl;
        } else {
            printPacket(&receiving_p);
            flag_timeout = false;
            gettimeofday(&end, NULL);
            update_timeout(&timeout, start, end);
            std::cout << "Novo timeout: " <<  timeout.tv_sec << "." << std::setfill('0') << std::setw(6) << timeout.tv_usec << " segundos" << std::endl;
            setsockopt(this->socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        }
        attempts++;
        std::cout << "Tentativa: " << attempts << std::endl;
    }
    if (attempts == MAX_ATEMPTS) {
        std::cout << "Número máximo de tentativas ocorridas!" << std::endl;
        return false;
    }
    std::string file_name = payload.substr(4);
    FILE* file = fopen(payload.substr(4).c_str(), "wb");
    flag_timeout = false;
    int ack_client = 0;
    fillPacket(&shipping_p, ack_client, ACK, 0, (byte*)"\0");
    do {
        //processar o pacote recebido
        if(receiving_p.flag == DATA) {
            if(receiving_p.number == ack_client) {
                fwrite(receiving_p.payload, 1, receiving_p.length, file);
                ack_client += receiving_p.length;
                // pacote a ser enviado
                fillPacket(&shipping_p, ack_client, ACK, 0, (byte*)"\0");
            }
            // TODO criar um buffer e processar pacotes fora de ordem

            // enviar o pacote resposta
        } else if(receiving_p.flag == MSG) {

        }

        send_bytes = (int)send(this->socket_fd, (Packet*)&shipping_p, sizeof(Packet), 0);
        recv_bytes = (int)recv(this->socket_fd, (Packet*)&receiving_p, sizeof(Packet), 0);
    }while(receiving_p.flag != ACK && receiving_p.flag != FIN && receiving_p.flag != FIN_DATA); // pacotes que recebo e tenho que processar apenas uma vez

    if(receiving_p.flag == ACK) {
        return false;
    } else if(receiving_p.flag == FIN_DATA) {
        if (receiving_p.payload == get_file_sha256(file_name)) {
            return true;
        } else {
            std::cout << "Falha na verificação SHA-256." << std::endl;
            return false;
        }
    } else if(receiving_p.flag == FIN) {
        fillPacket(&shipping_p, 0, FIN_ACK, 0, (byte*)"\0");
        send_bytes = (int)send(this->socket_fd, (Packet*)&shipping_p, sizeof(Packet), 0);
        Client::~Client();
    }
    return true;
}

void Client::handleIncomingData() {
    Packet receiving_p, shipping_p;
    ssize_t bytes_recv, bytes_send;
    FILE* file_write = NULL;
    while(running) {
        bytes_recv = recv(this->socket_fd, (Packet*)&receiving_p, sizeof(Packet), 0);
        if(bytes_recv < 0) {
            std::lock_guard<std::mutex> lock(mutex);
            std::cerr << "===========TIMEOUT===========" << std::endl;
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
                    //não envio pacotes resposta
                    if (receiving_p.payload == get_file_sha256(file_name)) {
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