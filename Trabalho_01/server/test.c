#include <stdio.h>
#include <stdlib.h> // exit() EXIT_FAILURE
#include <unistd.h> // close()
#include <sys/socket.h> //socket()
#include <netinet/in.h> // sockaddr_in
#include "packet.h" // Packet

#define PORT 12345
#define IP_SERVER "127.0.0.1"

int main() {
    int socket_fd;
    struct sockaddr_in server_addr;
    /*
     * Crinado Socket
     * socket()  creates  an endpoint for communication and returns a file de‐
     * scriptor that refers to that endpoint.
     */
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0); // domain(int), type(int), protocol(int)
    if (socket_fd == -1) {
        perror("Socket creation failed.\n");
        exit(EXIT_FAILURE);
    }
    /*
     *Internet domain sockets
       struct sockaddr_in {
           sa_family_t     sin_family;      AF_INET
           in_port_t       sin_port;        Port number 
           struct in_addr  sin_addr;        IPv4 address 
       };
     */
    // Configurando o endereço do servidor
    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_port = htons(PORT); // Port number
    server_addr.sin_addr.s_addr = INADDR_ANY; // Accepts from any interface
    inet_aton(IP_SERVER, &(server_addr.sin_addr));

    /*
     * bind() associates the socket with a local address and port number.
     * The socket is bound to the address specified by addr and addrlen.
     */

     // Vincular o socket ao endereço
     // retorno 0 se sucesso
     if ( bind(socket_fd, (struct sockaddr_in*)&server_addr, sizeof(server_addr)) != 0) {
        perror("Bind failed.\n");
        close(socket_fd);
        exit(EXIT_FAILURE);
     }

     //listen   -> usado em TCP
     //recvfrom -> usado em UDP

    return 0;
}