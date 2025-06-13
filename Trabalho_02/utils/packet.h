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
#include <iostream>
#include <string.h>
#include <string>
/*****************************************************************************
 * DEFINES 
 *****************************************************************************/

#define REQ         0 
#define DATA        1 
#define MSG         2
#define ACK         3
#define FIN_DATA    4
#define FIN         5
#define FIN_ACK     6

#define LENGTH      1017

using byte = char;

typedef struct {
    unsigned int number;
    byte flag;
    unsigned short int length;
    byte payload[LENGTH];
}Packet;

void fillPacket(Packet* packet, unsigned int number, byte flag, unsigned short int length, byte payload[LENGTH]);
void fillPacket(Packet* packet, unsigned int number, byte flag, unsigned short int length, std::string payload);
void printPacket(Packet* packet);
// TODO void printHeader(Packet* packet);

#endif // __INCLUDED_PACKET_H__