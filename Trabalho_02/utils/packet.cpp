#include "packet.h"

void fillPacket(Packet* packet, unsigned int number, byte flag, unsigned short int length, byte payload[LENGTH]) {
    packet->number = number;
    packet->flag = flag;
    packet->length = length;
    strcpy(packet->payload, payload);
    packet->payload[length] = '\0';
}

void fillPacket(Packet* packet, unsigned int number, byte flag, unsigned short int length, std::string payload) {
    packet->number = number;
    packet->flag = flag;
    packet->length = length;
    strcpy(packet->payload, payload.c_str());
}

void printPacket(Packet* packet) {
    std::cout << "==========PACKET==========" << std::endl;
    std::cout << "NUMBER: " << packet->number << std::endl;
    std::cout << "FLAG" << (packet->flag == REQ ? "REQ" : packet->flag == ACK ? "ACK" : packet->flag == MSG ? "MSG" : packet->flag == FIN ? "FIN" : packet->flag == FIN_DATA ? "FIN_DATA" : packet->flag == DATA ? "DATA" : "") << std::endl; 
    std::cout << "LENGTH: " << packet->length << std::endl;
}
