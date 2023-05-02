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
    serv_add.sin_port = htons(5002); // server port


    // connect to server
    if (connect(sock, (struct sockaddr *) &serv_add, sizeof(serv_add)) < 0) {
            perror("connect failed");
            return -1;
        }
    while (1) {
        
        printf("Enter message: ");
        fgets(message, sizeof(message), stdin);

        // send message to server    
        if (send(sock, message, sizeof(message), 0) != sizeof(message)) {
            perror("send failed");
            return -1;
        }

        if (strcmp(message, "exit\n") == 0) {
            printf("Goodbye\n");
            break;
        }
    }    
    close(sock);
    return 0;

} // client.c
