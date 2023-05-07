#include "header.h"

void send_msg(int soc, char* buffer, int size)
{
   if (send(soc, buffer, size, 0) < 0)
   {
       perror("send");
       exit(1);
   }
}

void recv_msg(int soc, char* buffer, int size)
{
    if (recv(soc, buffer, size, 0) < 0)
    {
        perror("recv");
        exit(1);
    }
}

void make_nice(char *msg, int size) {
    int i;
    for (i = 0; i < size; i++) {
        if (msg[i] == '\n') {
            msg[i] = '\0';
            break;
        }
    }
}



