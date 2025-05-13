#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include "packet.h"

#define IP_SERVER "103.0.1.70"
#define PORT 12345
#define BUFFER_SIZE 1024

bool file_exists(char* file_name);

int main() {
    int socket_fd;
    struct sockaddr_in server_addr, client_addr;
    ssize_t bytes_reveived, sent;
    socklen_t addr_len = sizeof(client_addr);

    // Criar socket UDP
    // AF_INET: IPv4
    // SOCK_DGRAM: Datagramas (UDP)
    // 0: Protocolo padrão (UDP)
    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Configurar endereço do servidor
    memset(&server_addr, 0, sizeof(server_addr));   // Limpar estrutura
    server_addr.sin_family = AF_INET;               // IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY;       // Aceita de qualquer interface
    server_addr.sin_port = htons(PORT);             // Porta
    printf("teste: %X\n", server_addr.sin_port);
    // Vincular socket ao endereço
    if (bind(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed\n\n");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    printf("Servidor UDP esperando na porta %d...\n", PORT);

    // Esperar por dados
    while(1) {
        bool valid = false;
        Packet packet;
        packet.flags = -1;
        char ip_msg[20];
        int32_t port_msg;
        char file_name[100];

        addr_len = sizeof(client_addr);

        // Esperar receber um pacote SYN
        while(packet.flags != SYN && !valid) {
            bytes_reveived = recvfrom(socket_fd, &packet, sizeof(packet), 0, (struct sockaddr*)&client_addr, (socklen_t*)&addr_len);
            if (bytes_reveived < 0) {
                perror("recvfrom failed\n");
                close(socket_fd);
                exit(EXIT_FAILURE);
            }
            if(packet.flags == SYN) {
                valid = true;
                printf("Recebido pacote SYN: %s\n", packet.data);
                // Verificar se o pacote SYN realmente é valido
                getNetworkInfo(packet.data, ip_msg, &port_msg, file_name);
                printf("\n\nIP: %s\n", ip_msg);
                printf("PORT: %d\n", port_msg);
                printf("FILE: %s\n", file_name);
            }
        }
        // Pacote SYN enviado de forma correta
        // Enviando o pacote SYN_ACK
        bool permission = file_exists(file_name);
        if (permission) {
            printf("Arquivo existe, enviando SYN_ACK\n");
        } else {
            printf("Arquivo não existe, enviando SYN_ACK\n");
        }
        create_packet_SYN_ACK(&packet, IP_SERVER, PORT, permission);
        addr_len = sizeof(client_addr);
        sent = sendto(socket_fd, &packet, sizeof(packet), 0, (struct sockaddr*)&client_addr, addr_len);
        printf("\nEnviado o pacote SYN_ACK\n");
        if (sent < 0) {
            perror("Erro ao enviar resposta\n");
            close(socket_fd);
            exit(EXIT_FAILURE);
        }
    }
    close(socket_fd);
    return 0;
}

bool file_exists(char* file_name) {
    char caminho[1024];
    snprintf(caminho, sizeof(caminho), "shared_file/%s", file_name);
    // Verifique o arquivo
    if (access(caminho, F_OK) != -1) {
        printf("Arquivo encontrado!\n");
        return true;
    } else {
        perror("Erro ao acessar o arquivo");
        return false;
    }
}

        // Esperar receber eternamente um pacote SYN

        /*socklen_t bytes_received = recvfrom(socket_fd, buffer, BUFFER_SIZE - 1, 0,
                                        (struct sockaddr*)&client_addr, &addr_len);
        if (bytes_received < 0) {
            perror("recvfrom failed");
            close(socket_fd);
            exit(EXIT_FAILURE);
        }

        buffer[bytes_received] = '\0'; // Null-terminar a string
        printf("Recebido do cliente: %s\n", buffer);
        addr_len = sizeof(client_addr);
        bytes_received = recvfrom(socket_fd, &packet, sizeof(packet), 0,(struct sockaddr*)&client_addr, &addr_len);
        if (packet.flags == SYN) {
            printf("Recebido pacote SYN: %s\n", packet.data);
        }
        create_packet_SYN_ACK(&packet, IP_SERVER, PORT, 1);
        addr_len = sizeof(client_addr);
        ssize_t sent = sendto(socket_fd, &packet, sizeof(packet), 0, (struct sockaddr*)&client_addr, addr_len);
        printf("\nEnviado o pacote SYN_ACK\n");
        if (sent < 0) {
            perror("Erro ao enviar resposta");
        }
        getNetworkInfo(packet.data, ip_msg, &port_msg, file_name);
        printf("\n\nIP: %s\n", ip_msg);
        printf("PORT: %d\n", port_msg);
        printf("FILE: %s\n", file_name);*/
