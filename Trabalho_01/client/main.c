#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 12345

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    const char *message = "Hello, World!";

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
    return 0;
}
