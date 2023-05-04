#include "header.h"

void send_msg(int sock, char *msg) {
    if (send(sock, msg, sizeof(msg), 0) != sizeof(msg)) {
        perror("send failed");
        exit(-1);
    }
}

void recv_msg(int sock, char *msg) {
    if (recv(sock, msg, sizeof(msg), 0) < 0) {
        perror("recv failed");
        exit(-1);
    }
}


