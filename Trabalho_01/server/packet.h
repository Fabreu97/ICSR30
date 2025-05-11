/*****************************************************************************
 * This file is to structure the packets sent in the Client-Server architecture
 * Packet SIZE  = 1KB
 * HEADER SIZE  = 16B
 * DATA SIZE    = 1008B 
 *****************************************************************************
 * Author: Fernando Abreu
 * Date: 2025-05-10
 *****************************************************************************
 */

/*****************************************************************************
 * LIBRARY
 *****************************************************************************/
#include <stdint.h>

/*****************************************************************************
 * MACROS 
 *****************************************************************************/
#define SYN         1   // Cliente inicializa a conexão
#define SYN_ACK     2   // Servidor responde aceitando a conexão
#define ACK         3   // Cliente confirma a conexão
#define FIN         4   // Cliente ou Servidor finaliza a conexão
#define FIN_ACK     5   // Outra parte confirma o fechamento da conexão

#define LENGTH      1024 - sizeof(int32_t) * 4 // Length of the data in bytes 

typedef struct {
    int32_t seq_number;     // Sequence number = 4B
    int32_t ack;            // Acknowledgment number = 4B
    int32_t flags;          // Flags (SYN, SYN_ACK, ACK, ...) = 4B
    int32_t length;         // Length of the data = 4B
    char data[LENGTH];      // 1008B
}Packet;