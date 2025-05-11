/*****************************************************************************
 * This file is to structure the packets sent in the Client-Server architecture
 * Packet SIZE  = 1KB
 * HEADER SIZE  = 16B
 * DATA SIZE    = 1008B 
 *****************************************************************************
 * Author:  Fernando Abreu
 * Date:    2025-05-10
 *****************************************************************************
 */

/*****************************************************************************
 * LIBRARY
 *****************************************************************************/
#include <stdint.h>

/*****************************************************************************
 * MACROS 
 *****************************************************************************/
/*
 * Servidor Flags   : SYN_ACK, SND, FIN, FIN_ACK
 * Cliente Flags    : SYN, ACK, FIN, FIN_ACK
 */
#define SYN         1   // Cliente inicializa a conexão
#define SYN_ACK     2   // Servidor responde aceitando a conexão
#define SND         3   // Servidor envia os dados
#define ACK         4   // Cliente confirma a chegada dos dados
#define FIN         5   // Cliente ou Servidor finaliza a conexão
#define FIN_ACK     6   // Outra parte confirma o fechamento da conexão

#define LENGTH      1024 - sizeof(int32_t) * 4 // Length of the data in bytes 

typedef struct {
    int32_t seq_number;     // Sequence number = 4B
    int32_t ack;            // Acknowledgment number = 4B
    int32_t flags;          // Flags (SYN, SYN_ACK, ACK, ...) = 4B
    int32_t length;         // Length of the data = 4B
    char data[LENGTH];      // 1008B
}Packet;

void create_packet_SYN(Packet* packet, char* ip, int port, char* file_name);
void create_packet_SYN_ACK(Packet* packet, char* ip, int port, char permission);
void create_packet_SND(Packet* packet, char* ip, int port, char* data);
void create_packet_ACK(Packet* packet, char* ip, int port, char* ack);
void create_packet_FIN(Packet* packet, char* ip, int port);
void create_packet_FIN_ACK(Packet* packet, char* ip, int port);