#include <stdio.h>
#include <arpa/inet.h> // Para htons()

int main() {
    unsigned short port = 12345; // Porta no formato do host

    // Antes de enviar a porta para a rede, convertemos usando htons()
    unsigned short network_port = htons(port);

    printf("Porta no formato do host: %u\n", port);
    printf("Porta no formato da rede: %u\n", network_port);

    return 0;
}