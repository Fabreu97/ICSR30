#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "packet.h"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 12345
#define PORT_CLIENT 12345

int input_ip_and_port(char* ip, int* port, char* msg) {
    char input[100];
    char trash;
    printf("\n******************************\n");
    printf("Digite o endereço IP do servidor: ");
    fgets(input, 100, stdin);
    if (strlen(input) <= 16) {
        input[strlen(input) - 1] = '\0';
        strcpy(ip, input);
    } else {
        return 1; // Erro: IP muito longo
    }
    printf("Digite a porta do servidor: ");
    if(scanf("%d%c", port, &trash) == 0) {
        printf("\nErro ao ler a porta. Tente novamente.\n");
        return 1;
    }
    printf("\nDigite a mensagem: ");
    fgets(msg, 50, stdin);

    return 0;
}

int main() {
    int erro;
    char ip[16];
    int port;
    char message[50] = "Hello, World!";

    // configuração do socket
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in server_addr;

    while(1) {
        erro = 1;
        while(erro) {
            erro = input_ip_and_port(ip, &port, message);
        }
        // Configuração do endereço do servidor
        server_addr.sin_family = AF_INET; // IPv4
        server_addr.sin_port = htons(port);
        server_addr.sin_addr.s_addr = INADDR_ANY;
        if (!inet_aton(ip, &server_addr.sin_addr)) {
            printf("Erro ao converter o endereço IP.\n");
            erro = 1;
        }
        if(!erro) {
            Packet packet;
            create_packet_SYN(&packet, ip, PORT_CLIENT, "arquivo.txt");
            ssize_t sent = sendto(socket_fd, message, strlen(message), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
            printf("Mensagem enviada para o servidor: %s\n", message);
            sent = sendto(socket_fd, &packet, sizeof(packet), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
            printf("Enviado pacote SYN: %s\n", packet.data);
            ssize_t bytes_received = recvfrom(socket_fd, &packet, sizeof(packet), 0,NULL, NULL);
            printf("Recebido pacote SYN_ACK: %s\n", packet.data);
        }
    }
    close(socket_fd);
    return 0;
/*
    // Criar socket UDP
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Configurar endereço do servidor
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("inet_pton failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Enviar mensagem
    ssize_t sent = sendto(sockfd, message, strlen(message), 0,
                          (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (sent < 0) {
        perror("sendto failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Mensagem enviada para o servidor: %s\n", message);

    close(sockfd);
    */
}
