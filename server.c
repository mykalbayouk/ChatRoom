#include "header.h"

// global variablesfor threads
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
client_t *clients[MAX_CLIENTS];
int num_clients = 0;

/**
 * Sends messages to all clients except the sender
*/
void send_to_all(char *msg, int uid) {
    int i;
    pthread_mutex_lock(&mutex);
    for (i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i]) {
            if (clients[i]->uid != uid) {
                send_msg(clients[i]->sockfd, msg, BUFF_SIZE);
            }
        }
    }
    pthread_mutex_unlock(&mutex);
}

/**
 * Removes client from clients queue
*/
void remove_cl(int uid) {
    int i;
    pthread_mutex_lock(&mutex);
    for (i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i]) {
            if (clients[i]->uid == uid) {
                clients[i] = NULL;
                break;
            }
        }
    }
    pthread_mutex_unlock(&mutex);
}

/**
 * Adds client to clients queue
*/
void add_cl(client_t *cl) {
    int i;
    pthread_mutex_lock(&mutex);
    for (i = 0; i < MAX_CLIENTS; i++) {
        if (!clients[i]) {
            clients[i] = cl;
            break;
        }
    }
    pthread_mutex_unlock(&mutex);
}

/**
 * Handles communication with clients
*/
void *handle_cl(void *arg) {    
    // create client variable
    client_t *cli = (client_t *)arg;
    // create message and name variables
    char message[BUFF_SIZE];
    char name[32];

    // handle recieving name from client
    recv_msg(cli->sockfd, name, 32);
    strcpy(cli->name, name);
    sprintf(message, "%s joined\n", cli->name);
    send_to_all(message, cli->uid);

    // handle sending and recieving messages
    int leave_flag = 0;

    bzero(message, BUFF_SIZE);
    // while the leave flag is not set, send and recieve messages
    while (1) {
        if (leave_flag) {
            break;
        }
        // recieve message
        recv_msg(cli->sockfd, message, BUFF_SIZE);
        make_nice(message, BUFF_SIZE);        
        if (strcmp(message, "exit") == 0) {
            sprintf(message, "%s left\n", cli->name);
            send_to_all(message, cli->uid);
            leave_flag = 1;
        } else {
            // send message
            send_to_all(message, cli->uid);             
        }
        bzero(message, BUFF_SIZE);
    }
    // cleanup
    close(cli->sockfd);
    remove_cl(cli->uid);
    free(cli);
    num_clients--;
    pthread_detach(pthread_self()); 
    return NULL;   
}                             


/**
 * Main function
*/
int main(int argc, char *argv[]) {
    int serv_sock; /* Socket descriptor for server */    
    int cl_fd; /* Socket descriptor for client */   
    int port_c; /* Server port */
    struct sockaddr_in serv_add; /* Local address */
    struct sockaddr_in client_add; /* Client address */
    unsigned int client_add_len;

    printf("==== Server started ====\n");

    if((serv_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        perror("socket failed");
        return -1;
    }

    get_port(argc, argv, &port_c);

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

    // create thread id and status variables
    pthread_t tid;
    int status = 1;    
    while(status) { 
        
        // wait for a client to connect                        
        client_add_len = sizeof(client_add);
        if ((cl_fd = accept(serv_sock, (struct sockaddr *) &client_add, &client_add_len)) < 0) {
            perror("accept failed");
            return -1;            
        } else if (num_clients == MAX_CLIENTS) {
            // if max clients reached, close connection
            close(cl_fd);  
            continue;
        } 


        // create client and thread
        client_t *cli = (client_t *)malloc(sizeof(client_t));
        cli->address = client_add;
        cli->sockfd = cl_fd;
        cli->uid = num_clients++;

        // add client to clients queue and create thread
        add_cl(cli);
        pthread_create(&tid, NULL, &handle_cl, (void *)cli);                      
       
    } // while

    return 0;


} // server.c
