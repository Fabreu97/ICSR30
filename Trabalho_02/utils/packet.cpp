#include "packet.h"

void fillPacket(Packet* packet, unsigned int number, byte flag, unsigned short int length, byte payload[LENGTH]) {
    int i;
    packet->number = number;
    packet->flag = flag;
    packet->length = length;
    strcpy(packet->payload, payload);
}