#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "packet.h"
#include "utils.h"

#define IP_SERVER "103.0.1.70"
#define PORT 12345
#define BUFFER_SIZE 1024
#define MAX_ATTEMPTS 3

bool file_exists(char* file_name);

int main() {
    unsigned int tamanho_bytes_totais_enviado = 0;
    int socket_fd;
    struct sockaddr_in server_addr, client_addr;
    ssize_t bytes_received, sent;
    socklen_t addr_len = sizeof(client_addr);
    struct timeval timeout, start, end;
    timeout.tv_sec = 1; // 1 segundo
    timeout.tv_usec = 0; // 0 microssegundos

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


    // Esperar por dados
    while(1) {
         printf("\n\nServidor UDP esperando na porta %d...\n", PORT);
        int ack_client = 0;
        bool valid = false;
        Packet packet;
        packet.flags = -1;
        char ip_msg[20];
        int32_t port_msg;
        char file_name[100];

        addr_len = sizeof(client_addr);

        // Retirar o timeout
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;
        setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));


        // Esperar receber um pacote SYN
        while(packet.flags != SYN && !valid) {
            bytes_received = recvfrom(socket_fd, &packet, sizeof(packet), 0, (struct sockaddr*)&client_addr, (socklen_t*)&addr_len);
            if (bytes_received < 0) {
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
        create_packet_SYN_ACK(&packet, IP_SERVER, PORT, permission);
        addr_len = sizeof(client_addr);
        sent = sendto(socket_fd, &packet, sizeof(packet), 0, (struct sockaddr*)&client_addr, addr_len);
        printf("\nEnviado o pacote SYN_ACK\n");
        if (sent < 0) {
            perror("Erro ao enviar resposta\n");
            close(socket_fd);
            exit(EXIT_FAILURE);
        }
        // Cada ACK recebido do cliente enviar um pacote SND com dados do arquivo
        if(permission) {
            char caminho[1024];
            char buffer[LENGTH];
            size_t bytes_read;

            snprintf(caminho, sizeof(caminho), "shared_file/%s", file_name);
            FILE* file = fopen(caminho, "rb");
            if (file == NULL) {
                perror("Erro ao abrir o arquivo");
                close(socket_fd);
                exit(EXIT_FAILURE);
            }
            ack_client = 0;
            timeout.tv_sec = INITIAL_TIMEOUT_SEC;
            timeout.tv_usec = 0;
            setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
            unsigned short int attempts = 0;
            packet.flags = -1;
            while(attempts < MAX_ATTEMPTS && packet.flags != ACK) {
                bytes_received = recvfrom(socket_fd, &packet, sizeof(packet), 0, (struct sockaddr*)&client_addr, (socklen_t*)&addr_len);
                if(bytes_received < 0) {
                    perror("recvfrom failed\n");
                }
                attempts++;
            }
            printf("Recebido pacote do tipo %s com valor de ack = %d\n", packet.flags == ACK ? "ACK" : "Nao_e_ACK", packet.ack);
            bool  release = true;
            bytes_read = 1;
            int largest_seq = 0;
            while(bytes_read > 0) {
                //Identifico a requisição de pacote do cliente
                ack_client = packet.ack;
                //Faço a montagem do pacote
                fseek(file, ack_client*LENGTH, SEEK_SET);
                bytes_read = fread(buffer, sizeof(char), LENGTH, file);
                // Enviar pacote SND
                if (bytes_read > 0) {
                    if(ack_client > largest_seq) {
                        largest_seq = ack_client;
                    }
                    packet.seq_number = ack_client;
                    packet.length = bytes_read;
                    packet.flags = SND;
                    memcpy(packet.data, buffer, bytes_read);
                    if (release) {
                        gettimeofday(&start, NULL);
                    }
                    tamanho_bytes_totais_enviado += bytes_read;
                    sent = sendto(socket_fd, &packet, sizeof(packet), 0, (struct sockaddr*)&client_addr, (socklen_t)addr_len);
                    printf("Enviado o pacote do tipo: ");
                    print_type_packet(&packet);
                    printf("SEQ_NUMBER: %d\n", packet.seq_number);
                    printf("Tamanho do Pacote: %ld\n", bytes_read);
                }
                if (sent < 0) {
                    perror("Erro ao enviar pacote SND\n");
                }
                bytes_received = recvfrom(socket_fd, &packet, sizeof(packet), 0, (struct sockaddr*)&client_addr, (socklen_t*)&addr_len);
                if (bytes_received < 0) {
                    release = false;
                    perror("Timeout de espera por mensagem. ACK\n");
                } else {
                    release = true;
                    gettimeofday(&end, NULL);
                    update_timeout(&timeout, start, end);
                    printf("Novo timeout: %ld.%06ld segundos\n", timeout.tv_sec, timeout.tv_usec);
                    setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)); // atualizo o timeout
                }
            }
            // Enviar pacote FIN
            create_packet_FIN(&packet, SUCCESS);
            sent = sendto(socket_fd, &packet, sizeof(packet), 0, (struct sockaddr*)&client_addr, (socklen_t)addr_len);
            while(packet.flags != FIN_ACK) {
                printf("Esperando o pacote do tipo: FIN_ACK do Cliente . . .\n");
                bytes_received = recvfrom(socket_fd, &packet, sizeof(packet), 0, (struct sockaddr*)&client_addr, (socklen_t*)&addr_len);
                if (bytes_received < 0) {
                    perror("Timeout de espera por mensagem. FIN_ACK\n");
                }
            }
            fclose(file);
            printf("Recebido o pacote do tipo: ");
            print_type_packet(&packet);
            printf("\n");
            printf("foi enviado %u bytes\n", tamanho_bytes_totais_enviado);
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
