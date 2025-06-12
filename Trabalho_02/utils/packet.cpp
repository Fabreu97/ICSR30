#include "packet.h"

void fillPacket(Packet* packet, unsigned int number, byte flag, unsigned short int length, byte payload[LENGTH]) {
    packet->number = number;
    packet->flag = flag;
    packet->length = length;
    strcpy(packet->payload, payload);
}

void fillPacket(Packet* packet, unsigned int number, byte flag, unsigned short int length, std::string payload) {
    packet->number = number;
    packet->flag = flag;
    packet->length = length;
    strcpy(packet->payload, payload.c_str());
}