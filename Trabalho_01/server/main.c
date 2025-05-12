#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "packet.h"

#define IP_SERVER "103.0.1.70"
#define PORT 12345
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(client_addr);

    // Criar socket UDP
    // AF_INET: IPv4
    // SOCK_DGRAM: Datagramas (UDP)
    // 0: Protocolo padrão (UDP)
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
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
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Servidor UDP esperando na porta %d...\n", PORT);

    // Esperar por dados
    while(1) {
        printf("Teste . . .");
        Packet packet;
        char ip_msg[20];
        int32_t port_msg;
        char file_name[100];
        addr_len = sizeof(client_addr.sin_addr);
        socklen_t bytes_received = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0,
                                        (struct sockaddr*)&client_addr, &addr_len);
        if (bytes_received < 0) {
            perror("recvfrom failed");
            close(sockfd);
            exit(EXIT_FAILURE);
        }

        buffer[bytes_received] = '\0'; // Null-terminar a string
        printf("Recebido do cliente: %s\n", buffer);
        addr_len = sizeof(client_addr);
        bytes_received = recvfrom(sockfd, &packet, sizeof(packet), 0,(struct sockaddr*)&client_addr, &addr_len);
        if (packet.flags == SYN) {
            printf("Recebido pacote SYN: %s\n", packet.data);
        }
        create_packet_SYN_ACK(&packet, IP_SERVER, PORT, 1);
        addr_len = sizeof(client_addr);
        ssize_t sent = sendto(sockfd, &packet, sizeof(packet), 0, (struct sockaddr*)&client_addr, addr_len);
        printf("\nEnviado o pacote SYN_ACK\n");
        if (sent < 0) {
            perror("Erro ao enviar resposta");
        }
        /*getNetworkInfo(packet.data, ip_msg, &port_msg, file_name);
        printf("\n\nIP: %s\n", ip_msg);
        printf("PORT: %d\n", port_msg);
        printf("FILE: %s\n", file_name);*/
    }
    close(sockfd);
    return 0;
}
