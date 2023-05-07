#include "header.h"

#define MAX_CLIENTS 7

typedef struct{
    char name[256];   
} client_s;

void init_client(client_s *cl) {
    cl->name[0] = '\0';
}

int main(int argc, char *argv[]) {
    int serv_sock; /* Socket descriptor for server */    
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
    int shmid;
    int *client_fds;
    client_s *clients = (client_s*) malloc(MAX_CLIENTS * sizeof(client_s));

    shmid = shmget(IPC_PRIVATE, MAX_CLIENTS * sizeof(int), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget failed");
        exit(EXIT_FAILURE);
    }

    client_fds = shmat(shmid, NULL, 0);
    if (client_fds == (int*) -1) {
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }

    while(status) { 

        //exit server
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
           init_client(&clients[num_clients]);
           num_clients++;           
           printf("Client%d connected\n", num_clients);          
        }
                

        client_fds[num_clients - 1] = cl_fd;
        

        pid_recv = fork();
        if (pid_recv < 0) {
            perror("fork failed");
            return -1;
        } else if (pid_recv == 0){  
            close(serv_sock);              
            while(1) {
                recv_msg(cl_fd, message);              
                if (clients[num_clients - 1].name[0] == '\0') {
                    strcpy(clients[num_clients - 1].name, message);
                    printf("Client%d name: %s\n", num_clients, clients[num_clients - 1].name);
                    continue;
                } 

                if (strcmp(message, "exit\n") == 0) {
                    printf("%s disconnected\n",clients[num_clients - 1].name);
                    num_clients--;    
                    close(cl_fd);
                    break;                    
                }    

                for (int i = 0; i < MAX_CLIENTS; i++) {                    
                    if (client_fds[i] != cl_fd && client_fds[i] != 0) {
                        printf("fd: %d\n", client_fds[i]);                    
                        char masg[256];
                        sprintf(masg, "%s: %s", clients[num_clients - 1].name, message);
                        send_msg(client_fds[i], masg);
                    }                                                            
                }
                                                
                printf("%s: %s\n",clients[num_clients - 1].name, message);
                // send message to other clients                    
                    

                
            }
            exit(0);                             
        }



    

    } // while

    for (int i = 0; i < num_clients; i++) {
        close(client_fds[i]); // close all client sockets
    }

    if (shmdt(client_fds) == -1) {
        perror("shmdt failed");
        exit(EXIT_FAILURE);
    }

    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl failed");
        exit(EXIT_FAILURE);
    
    }

    wait(NULL); // wait for child process to finish (recv)
    close(serv_sock);
    return 0;


} // server.c
