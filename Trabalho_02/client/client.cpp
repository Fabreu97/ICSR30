#include "client.h"

Client::Client(const std::string ipv4, const int port):
    ipv4_server(ipv4), port(port)
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
const bool Client::requestIDT(const std::string payload) { return false; }

void Client::requestMSG(const std::string payload) {
    ssize_t bytes_send, bytes_recv;
    Packet received_p, shipping_p;
    fillPacket(&shipping_p, 0, REQ, sizeof(payload.c_str()), payload);
    bytes_send = send(this->socket_fd, &shipping_p, sizeof(Packet), 0);
    bytes_recv = recv(this->socket_fd, &received_p, sizeof(Packet), 0);
    // ssize_t recv(int sockfd, void *buf, size_t len, int flags);
}

void Client::run() {
    std::string input, command;
    while(true) {

        std::cout << "*************************************" << std::endl;
        std::cout << "Comando Get:  GET/nome_do_arquivo.ext" << std::endl;
        std::cout << "Comando MSG:  MSG/mensagem!" << std::endl;
        std::cout << "\tA mensagem será enviada para todos " << std::endl; 
        std::cout << "\tos clientes do server!" << std::endl;
        std::cout << "Comando IDT:  IDT/nome_de_usuario    " << std::endl;
        std::cout << "Comando exit: EXIT!" << std::endl;
        std::cout << "*************************************" << std::endl;
        std::getline(std::cin, input);
        
        if(input.length() > 4) {
            command = input.substr(0,4);
            if(command == "GET/") {
                if(this->requestGET(input)) {
                    std::cout << "Arquivo recebido com sucesso!\n";
                }
            } else if(command == "MSG/") {
                requestMSG(input);
            } else if(command == "IDT/") {
                requestIDT(input);
            }

        }
    }

}