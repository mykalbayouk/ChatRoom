#include "header.h"


/**
 * Gets the port from the command line
*/
void get_port(int argc, char *argv[], int *port) {
    if (argc < 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }
    *port = atoi(argv[1]);
}
   

/**
 * Sends a message to single clients
 */
void send_msg(int soc, char* buffer, int size)
{
   if (send(soc, buffer, size, 0) < 0)
   {
       perror("send");
       exit(1);
   }
}

/**
 * Recieve a message from single clients
 */
void recv_msg(int soc, char* buffer, int size)
{
    if (recv(soc, buffer, size, 0) < 0)
    {
        perror("recv");
        exit(1);
    }
}

/**
 * Removes any new line chars from the message
 */ 
void make_nice(char *msg, int size) {
    int i;
    for (i = 0; i < size; i++) {
        if (msg[i] == '\n') {
            msg[i] = '\0';
            break;
        }
    }
}



