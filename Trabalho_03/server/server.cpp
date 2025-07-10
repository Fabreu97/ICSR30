#include "server.h"

/*
 * AF_INET      =   IPv4 Internet protocols
 * SOCK_STREAM  =   Provides sequenced, reliable, two-way, connection-based
                    byte streams.  An out-of-band data transmission  mecha‐
                    nism may be supported.
 * INADDR_ANY   =   Escuta em todas as interfaces eth0, loopback,...
 */

 Server::Server() {
    this->sock_fd = socket(AF_INET, SOCK_STREAM, 0);
 }

 Server::~Server