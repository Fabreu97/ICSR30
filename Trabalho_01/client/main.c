#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "packet.h"
#include "utils.h"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 12345
#define PORT_CLIENT 12345

#define MAX_ATTEMPTS 3


int input_ip_and_port_file(char* ip, int* port, char* file_name) {
    char input[100];
    char trash;
    char* endptr;
    printf("\n******************************\n");
    printf("Digite o endereço IP do servidor: ");
    endptr = fgets(input, 100, stdin);
    if (strlen(input) <= 16 && endptr != NULL) {
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
    endptr = fgets(file_name, 100, stdin);
    if (endptr == NULL) {
        return 1; // Erro: Leitura do arquivo falhou
    }
    file_name[strlen(file_name) - 1] = '\0'; // Remove o '\n' do final

    return 0;
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
    int ack_client = 0;

    // configuração do socket
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in server_addr;
    socklen_t addr_len = sizeof(server_addr);

    while(1) {
        erro = 1;
        while(erro) {
            erro = input_ip_and_port_file(ip, &port, file_name);
        }
        // Configuração do endereço do servidor
        server_addr.sin_family = AF_INET; // IPv4
        server_addr.sin_port = htons(port);
        server_addr.sin_addr.s_addr = INADDR_ANY;
        if (inet_pton(AF_INET, ip, &server_addr.sin_addr) != 1) {
            printf("Erro ao converter o endereço IP.\n");
            erro = 1;
        }
        if(!erro) {
            // Enviar SYN
            attempts = 0;
            Packet recv_packet;
            Packet send_packet;
            create_packet_SYN(&send_packet, ip, port, file_name);
            setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
            while(attempts < MAX_ATTEMPTS && recv_packet.flags != SYN_ACK) {
                bytes_received = -1;
                gettimeofday(&start, NULL);
                sent = sendto(socket_fd, &send_packet, sizeof(send_packet), 0, (struct sockaddr*)&server_addr, (socklen_t)addr_len);
                printf("Mensagem SYN enviada para o servidor: %s\n", send_packet.data);
                printf("Enviado o pacote do tipo: ");
                print_type_packet(&send_packet);
                bytes_received = recvfrom(socket_fd, &recv_packet, sizeof(recv_packet), 0, NULL, NULL);
                gettimeofday(&end, NULL);
                if (bytes_received < 0) {
                    printf("Timeout! Tentativa %d de %d\n", attempts + 1, MAX_ATTEMPTS);
                    attempts++;
                    //timeout.tv_usec += 10000; // Aumenta o timeout em 10ms 
                    //printf("Novo timeout: %ld.%06ld segundos\n", timeout.tv_sec, timeout.tv_usec);
                } else {
                    printf("Recebido o pacote do tipo: ");
                    print_type_packet(&recv_packet);
                    // Verifico se SYN_ACK aceito a solicitação do arquivo
                    update_timeout(&timeout, start, end);
                    printf("Novo timeout: %ld.%06ld segundos\n", timeout.tv_sec, timeout.tv_usec);
                    setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)); // atualizo o timeout
                    if (recv_packet.flags == SYN_ACK && (recv_packet.data[0]) != 0) {
                        erro = 0;
                        printf(" o pacote SYN_ACK retorno com SUCESSO.\n");
                    } else {
                        erro = 1;
                        break;
                        printf(" o pacote SYN_ACK retorno com ERRO.\n");
                        printf("Arquivo não existe, tente novamente.\n");
                    }
                }
            }
            if(!erro) {
                // Envia ACK e recebe SND
                ack_client = 0;
                bool release = true;
                FILE* file = fopen(file_name, "wb");
                while(recv_packet.flags != FIN) {
                    // Enviar ACK
                    create_packet_ACK(&send_packet, ack_client);
                    if (release) {
                        gettimeofday(&start, NULL);
                    }
                    sent = sendto(socket_fd, &send_packet, sizeof(send_packet), 0, (struct sockaddr*)&server_addr, (socklen_t)addr_len);
                    if (sent < 0) {
                        perror("Erro ao enviar pacote ACK\n");
                        //fclose(file);
                        //close(socket_fd);
                        //exit(EXIT_FAILURE);
                    }
                    printf("Enviado o pacote do tipo: ");
                    print_type_packet(&send_packet);
                    printf("ACK = %d\n", send_packet.ack);
                    bytes_received = recvfrom(socket_fd, &recv_packet, sizeof(recv_packet), 0, NULL, NULL);
                    if (bytes_received < 0) {
                        release = false;
                        //timeout.tv_usec += 10000; // Aumenta o timeout em 10ms 
                        //printf("Novo timeout: %ld.%06ld segundos\n", timeout.tv_sec, timeout.tv_usec);
                    } else {
                        release = true;
                        gettimeofday(&end, NULL);
                        update_timeout(&timeout, start, end);
                        printf("Novo timeout: %ld.%06ld segundos\n", timeout.tv_sec, timeout.tv_usec);
                        setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)); // atualizo o timeout
                        printf("Recebido o pacote do tipo: ");
                        print_type_packet(&recv_packet);
                        if(recv_packet.seq_number == ack_client) {
                            ack_client++;
                            if (recv_packet.flags == SND && recv_packet.length > 0) { 
                                printf("SEQ_NUMBER(%d): %d\n", recv_packet.length, recv_packet.seq_number);
                                printf("ACK = %d\n", recv_packet.ack);
                                // Escreve no arquivo
                                if (file == NULL) {
                                    perror("Erro ao abrir o arquivo para escrita");
                                    exit(EXIT_FAILURE);
                                }
                                fwrite(recv_packet.data, 1, recv_packet.length, file);
                                //fclose(file);
                            }
                        } else {
                            printf("Pacote recebido fora de ordem. Esperando o pacote do tipo: SND do Servidor . . .\n");
                            print_packet(&recv_packet);
                        }
                    }
                }
                fclose(file);
                create_packet_FIN_ACK(&send_packet);
                printf("Enviado o pacote do tipo: ");
                print_type_packet(&send_packet);
                sent = sendto(socket_fd, &send_packet, sizeof(send_packet), 0, (struct sockaddr*)&server_addr, (socklen_t)addr_len);
            }
        }
    }
    close(socket_fd);
    return 0;
}
