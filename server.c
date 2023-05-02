#include "header.h"

int main(int argc, char *argv[]) {
    int serv_sock;
    int client_sock;
    struct sockaddr_in serv_add; /* Local address */
    struct sockaddr_in client_add; /* Client address */


    printf("==== Server started ====\n");

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

    unsigned int client_add_len = sizeof(client_add);
    if ((client_sock = accept(serv_sock, (struct sockaddr *) &client_add, &client_add_len)) < 0) {
        perror("accept failed");
        return -1;
    }

    int status = 1;
    while(status) {        
        char message[256];
        // wait for a client to connect

        if (client_sock < 0) {
            perror("accept failed");
            break;
        }

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork failed");
            return -1;
        } else if (pid == 0) {
            close(serv_sock);
            // recieve message            
            if (recv(client_sock, &message, sizeof(message), 0) < 0) {
                perror("recieve failed");
                return -1;
            }

            if (strcmp(message, "exit\n") == 0) {
                printf("Client said: %s", message);                
                status = 0;                
                exit(0);
            } else {
                printf("Client said: %s", message);                    
                exit(0);
            }
        }
        
        pid = waitpid(-1, NULL, WNOHANG);
        if (pid < 0) {
            perror("waitpid failed");
            return -1;
        } else if (pid == 0) {
            break;
        } 
        
    }
    close(serv_sock);
    return 0;


} // server.c
