#include "utils.h"

// RTT: Round Trip Time = mede o tempo total entre o envio de um pacote até o recebimento da resposta.
void update_timeout(struct timeval* timeout, struct timeval start, struct timeval end) {
    if (end.tv_usec < start.tv_usec) {
        // "empresta" 1 segundo (1.000.000 microssegundos)
        end.tv_sec -= 1;
        end.tv_usec += 1000000;
    }
    timeout->tv_sec = (long)((end.tv_sec - start.tv_sec) * ALPHA + timeout->tv_sec * (1 - ALPHA));
    timeout->tv_usec = (long)((end.tv_usec - start.tv_usec) * ALPHA) + (long)(timeout->tv_usec * (float)(1 - ALPHA));
}