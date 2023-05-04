#include "header.h"

int main(int argc, char *argv[]) {
    char message[256];
    int sock;
    struct sockaddr_in serv_add;    

    // create socket
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        perror("socket failed");
        return -1;
    }

    // construct server address
    memset(&serv_add, 0, sizeof(serv_add)); // zero out structure
    serv_add.sin_family = AF_INET; // address family
    serv_add.sin_addr.s_addr = htonl(INADDR_ANY); // server ip
    serv_add.sin_port = htons(port_c); // server port


    // connect to server
    if (connect(sock, (struct sockaddr *) &serv_add, sizeof(serv_add)) < 0) {
        perror("connect failed");
        return -1;
    }

    pid_t pid;
    while (1) {

         // receive message from server
        printf("\n> ");
        pid = fork();
        if (pid < 0) {
            perror("fork failed");
            return -1;
        } else if (pid == 0) {
            while(1) {
                char mem[256];
                recv_msg(sock, mem);
                printf("Server: %s\n", mem);

            }            
            exit(0);
        }        
        fgets(message, sizeof(message), stdin);

        // send message to server    
        send_msg(sock, message);


        if (strcmp(message, "exit\n") == 0) {
            printf("Goodbye\n");
            break;
        }
    }
    
    wait(NULL);
    kill(pid, SIGTERM);
    close(sock);
    return 0;

} // client.c
