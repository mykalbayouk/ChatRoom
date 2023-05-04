#include "header.h"

#define MAX_CLIENTS 7

int main(int argc, char *argv[]) {
    int serv_sock; /* Socket descriptor for server */    
    int clients[MAX_CLIENTS];
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
    serv_add.sin_port = htons(port_c);      /* Local port */

    // bind to local addreess
    if (bind(serv_sock, (struct sockaddr *) &serv_add, sizeof(serv_add)) < 0) {
        perror("bind failed");
        return -1;
    }

    // mark socket so it will listen for in coming connections
    if (listen(serv_sock, 10) < 0) {
        perror("listen failed");
        return -1;
    }

    int status = 1;

    unsigned int client_add_len;
    char message[256];
    int num_clients = 0;
    int cl_fd; 
    pid_t pid_recv;

    while(status) {        
        // wait for a client to connect

                
        client_add_len = sizeof(client_add);
        if ((cl_fd = accept(serv_sock, (struct sockaddr *) &client_add, &client_add_len)) < 0) {
            perror("accept failed");
            return -1;
        } else if (num_clients == MAX_CLIENTS) {
            printf("Server is full\n");
            close(cl_fd);
            continue;
        } else {
           num_clients++;
           printf("Client%d connected\n", num_clients);          
        }
        
        
        
        pid_recv = fork();
        if (pid_recv < 0) {
            perror("fork failed");
            return -1;
        } else if (pid_recv == 0){  
            close(serv_sock);              
            while(1) {
                if (recv(cl_fd, &message, sizeof(message), 0) < 0) {
                    perror("recv failed");
                    return -1;
                }
                if (strcmp(message, "exit\n") == 0) {
                    printf("Client%d disconnected\n", num_clients);
                    num_clients--;    
                    if (num_clients == 0) {                            
                        printf("Server is empty\n");
                    }
                    close(cl_fd);
                    break;
                } 
                printf("Client%d: %s\n", num_clients, message);
                
            }                             
        }      

    } // while

    close(serv_sock);
    return 0;


} // server.c
