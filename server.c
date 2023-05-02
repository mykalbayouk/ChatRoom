#include "header.h"

int main(int argc, char *argv[]) {
    int serv_sock;
    int client_sock;
    struct sockaddr_in serv_add; /* Local address */
    struct sockaddr_in client_add; /* Client address */

    if((serv_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        perror("socket failed");
        return -1;
    }

     /* Construct local address structure */
    memset(&serv_add, 0, sizeof(serv_add));   /* Zero out structure */
    serv_add.sin_family = AF_INET;                /* Internet address family */
    serv_add.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    serv_add.sin_port = htons(5002);      /* Local port */

    // bind to local addreess
    if (bind(serv_sock, (struct sockaddr *) &serv_add, sizeof(serv_add)) < 0) {
        perror("bind failed");
        return -1;
    }

    // mark socket so it will listen for in coming connections
    if (listen(serv_sock, 5) < 0) {
        perror("listen failed");
        return -1;
    }

    while(1) {
        // wait for a client to connect
        if ((client_sock = accept(serv_sock, NULL, NULL)) < 0) {
            perror("accept failed");
            return -1;
        }

        char message[50];

        // recieve message from client
        if (recv(client_sock, &message, sizeof(message), 0) < 0) {
            perror("recieve failed");
            return -1;
        }

        if (strcmp(message, "exit\n") == 0) {
            printf("client said: %s", message);
            break;
        }

        printf("client said: %s", message);


    }
    close(serv_sock); 
    return 1;


} // server.c
