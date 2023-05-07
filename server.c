#include "header.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
client_t *clients[MAX_CLIENTS];
int num_clients = 0;

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

void *handle_cl(void *arg) {    
    client_t *cli = (client_t *)arg;
    char message[BUFF_SIZE];
    char name[32];
    recv_msg(cli->sockfd, name, 32);
    strcpy(cli->name, name);
    printf("Client%d name: %s\n", cli->uid, cli->name);
    sprintf(message, "%s joined\n", cli->name);
    printf("%s", message);
    send_to_all(message, cli->uid);

    int leave_flag = 0;

    bzero(message, BUFF_SIZE);

    while (1) {
        if (leave_flag) {
            break;
        }
        recv_msg(cli->sockfd, message, BUFF_SIZE);
        make_nice(message, BUFF_SIZE);        
        if (strcmp(message, "exit") == 0) {
            sprintf(message, "%s left\n", cli->name);
            printf("%s", message);
            send_to_all(message, cli->uid);
            leave_flag = 1;
        } else {
            printf("%s: %s\n", cli->name, message);
            send_to_all(message, cli->uid);             
        }
        bzero(message, BUFF_SIZE);
    }
    close(cli->sockfd);
    remove_cl(cli->uid);
    free(cli);
    num_clients--;
    pthread_detach(pthread_self()); 
    return NULL;   
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
    int cl_fd; 
    pthread_t tid;
        
    
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
            printf("Client%d connected\n", num_clients);          
        }

        client_t *cli = (client_t *)malloc(sizeof(client_t));
        cli->address = client_add;
        cli->sockfd = cl_fd;
        cli->uid = num_clients++;

        add_cl(cli);
        pthread_create(&tid, NULL, &handle_cl, (void *)cli);                      
       
    } // while

    return 0;


} // server.c
