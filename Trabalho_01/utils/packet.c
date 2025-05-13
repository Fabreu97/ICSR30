#include "packet.h"

void create_packet_SYN(Packet* packet, char* ip, int32_t port, char* file_name) {
    char data[1008] = "";
    char ip_port[16];
    snprintf(ip_port, sizeof(ip_port), "%d", port);
    packet->seq_number = 0;
    packet->ack = 0;
    packet->flags = SYN;
    packet->length = strlen(ip) + 1 + strlen(ip_port) + 1 + strlen(file_name);
    // IP:PORTA/file_name
    strcat(data, ip);
    strcat(data, ":");
    strcat(data, ip_port);
    strcat(data, "/");
    strcat(data, file_name);
    strcpy(packet->data, data);
    packet->data[packet->length] = '\0';
    printf("PACOTE SYN: %s\n", packet->data);
}

void create_packet_SYN_ACK(Packet* packet, char* ip, int32_t port, bool permission) {
    char data[1008] = "";
    char ip_port[16];

    packet->seq_number = 0;
    packet->ack = 0;
    packet->flags = SYN_ACK;
    if (permission) {
        //IP:PORTA\0
        snprintf(ip_port, sizeof(ip_port), "%d", port);
        packet->length = strlen(ip) + 1 + strlen(ip_port) + 1;
        strcat(data, ip);
        strcat(data, ":");
        strcat(data, ip_port);
        strcpy(packet->data, data);
        packet->data[packet->length - 1] = '\0';
        //printf("PACOTE SYN_ACK: %s\n", packet->data);
    } else {
        packet->length = 0;
        packet->data[0] = 0;
        packet->data[1] = '\0';
    }
}

void create_packet_SND(Packet* packet, char* data, int32_t seq_number) {
    packet->seq_number = seq_number;
    packet->ack = ++seq_number;
    packet->flags = SND;
    packet->length = strlen(data);
    strcpy(packet->data, data);
}

void create_packet_ACK(Packet* packet, int32_t ack) {
    packet->seq_number = 0;
    packet->ack = ack;
    packet->flags = ACK;
    packet->length = 0;
    packet->data[0] = '\0';
}

void print_type_packet(Packet* packet) {
    switch(packet->flags) {
        case SYN:
            printf("SYN\n");
            break;
        case SYN_ACK:
            printf("SYN_ACK\n");
            break;
        case SND:
            printf("SND\n");
            break;
        case ACK:
            printf("ACK\n");
            break;
        case FIN:
            printf("FIN\n");
            break;
        case FIN_ACK:
            printf("FIN_ACK\n");
            break;
        default:
            printf("Unknown packet type\n");
    }
}

void getNetworkInfo(char* data, char* ip, int32_t* port, char* file_name) {
        unsigned int i = 0, j = 0;
        char ip_port[20];
        while(data[i] != ':') {
            ip[j++] = data[i++];
        }
        ip[j] = '\0';
        i++;
        j=0;
        while(data[i] != '/' && data[i] != '\0') {
            ip_port[j++] = data[i++];
        }
        i++;
        ip_port[j] = '\0';
        *port = atoi(ip_port);
        j=0;
        while(data[i] != '\0') {
            file_name[j++] = data[i++];
        }
        file_name[j] = '\0';
}