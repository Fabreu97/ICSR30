#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>

#include "packet.h"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 12345
#define PORT_CLIENT 12345

#define MAX_ATTEMPTS 3
#define INITIAL_TIMEOUT_SEC 1 // 1 second
#define ALPHA 0.125



int input_ip_and_port_file(char* ip, int* port, char* file_name) {
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
    printf("\nDigite o Arquivo para Download: ");
    fgets(file_name, 100, stdin);

    return 0;
}

// RTT: Round Trip Time = mede o tempo total entre o envio de um pacote até o recebimento da resposta.
void update_timeout(struct timeval* timeout, struct timeval start, struct timeval end) {
    if (end.tv_usec < start.tv_usec) {
        // "empresta" 1 segundo (1.000.000 microssegundos)
        end.tv_sec -= 1;
        end.tv_usec += 1000000;
    }
    timeout->tv_sec = (long)((end.tv_sec - start.tv_sec) * ALPHA + timeout->tv_sec * (1 - ALPHA));
    timeout->tv_usec = (long)((end.tv_usec - start.tv_usec) * ALPHA) + (long)(timeout->tv_usec * (float)(1 - ALPHA));
}

int main() {
    int erro;
    char ip[16];
    int port;
    char file_name[100] = "Hello, World!";
    unsigned short int attempts;
    ssize_t sent;
    ssize_t bytes_received;
    struct timeval timeout, start, end;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    // configuração do socket
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in server_addr;

    while(1) {
        erro = 1;
        while(erro) {
            erro = input_ip_and_port_file(ip, &port, file_name);
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
            // Enviar SYN
            attempts = 0;
            Packet packet;
            create_packet_SYN(&packet, ip, port, file_name);
            setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
            while(attempts < MAX_ATTEMPTS && packet.flags != SYN_ACK) {
                bytes_received = -1;
                gettimeofday(&start, NULL);
                sent = sendto(socket_fd, &packet, sizeof(packet), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
                printf("Mensagem SYN enviada para o servidor: %s\n", packet.data);
                printf("Enviado o pacote do tipo: ");
                print_type_packet(&packet);
                bytes_received = recvfrom(socket_fd, &packet, sizeof(packet), 0, NULL, NULL);
                gettimeofday(&end, NULL);
                if (bytes_received < 0) {
                    printf("Timeout! Tentativa %d de %d\n", attempts + 1, MAX_ATTEMPTS);
                    attempts++;
                    //timeout.tv_usec += 10000; // Aumenta o timeout em 10ms 
                    //printf("Novo timeout: %ld.%06ld segundos\n", timeout.tv_sec, timeout.tv_usec);
                } else {
                    printf("Recebido o pacote do tipo: ");
                    print_type_packet(&packet);
                    // Verifico se SYN_ACK aceito a solicitação do arquivo
                    update_timeout(&timeout, start, end);
                    printf("Novo timeout: %ld.%06ld segundos\n", timeout.tv_sec, timeout.tv_usec);
                    setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)); // atualizo o timeout
                    if (packet.flags == SYN_ACK && !(packet.data[0])) {
                        erro = 1;
                        printf(" o pacote SYN_ACK retorno com ERRO.\n");
                    } else {
                        printf(" o pacote SYN_ACK retorno com SUCESSO.\n");
                    }
                }
            }
        }
            /*
            Packet packet;
            create_packet_SYN(&packet, ip, PORT_CLIENT, "arquivo.txt");
            ssize_t sent = sendto(socket_fd, message, strlen(message), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
            printf("Mensagem enviada para o servidor: %s\n", message);
            sent = sendto(socket_fd, &packet, sizeof(packet), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
            printf("Enviado pacote SYN: %s\n", packet.data);
            ssize_t bytes_received = recvfrom(socket_fd, &packet, sizeof(packet), 0,NULL, NULL);
            printf("Recebido pacote SYN_ACK: %s\n", packet.data);
            */
    }
    close(socket_fd);
    return 0;
}
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
