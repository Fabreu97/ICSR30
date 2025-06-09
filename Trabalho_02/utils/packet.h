/*****************************************************************************
 * This file is to structure the packets sent in the Client-Server architecture
 * Packet SIZE  = 1KB
 * HEADER SIZE  = 7B
 * DATA SIZE    = 1017B 
 *****************************************************************************
 * Author:  Fernando P. G. de Abreu
 * Date:    2025-05-10
 * version: 2.0
 * Changes from previous version:
 *      - Simplified the header
 *      - ...
 *****************************************************************************
 */

#ifndef __INCLUDED_PACKET_H__
#define __INCLUDED_PACKET_H__

/*****************************************************************************
 * INCLUDED
 *****************************************************************************/

#include <string.h>

/*****************************************************************************
 * DEFINES 
 *****************************************************************************/

#define REQ     0 // Requisição de recurso GET or MSG
#define ACK     1 
#define IDN     2
#define FIN     3
#define FIN_ACK 4
#define LENGTH  1017

using byte = char;

typedef struct {
    unsigned int number;
    byte flag;
    unsigned short int length;
    byte payload[LENGTH];
}Packet;

void fillPacket(Packet* packet, unsigned int number, byte flag, unsigned short int length, byte payload[LENGTH]);
// TODO void printHeader(Packet* packet);

#endif // __INCLUDED_PACKET_H__