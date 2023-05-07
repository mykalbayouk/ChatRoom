#include "header.h"

int sock = 0;
char name[32];
volatile sig_atomic_t flag = 0;

void catch_ctrl_c_and_exit(int sig) {
    flag = 1;
}

void std_out_c() {
  printf("%s", "> ");
  fflush(stdout);
}

void send_handle(void *arg) {
    char message[BUFF_SIZE];
    char buffer[BUFF_SIZE + 32];
    while (1) {
        std_out_c();
        fgets(message, BUFF_SIZE, stdin);
        make_nice(message, BUFF_SIZE);
        if (strcmp(message, "exit") == 0) {
            send_msg(sock, message, BUFF_SIZE);
            break;
        }
        sprintf(buffer, "%s: %s\n", name, message);
        send_msg(sock, buffer, BUFF_SIZE + 32);
        bzero(message, BUFF_SIZE);
        bzero(buffer, BUFF_SIZE + 32);
    }
    catch_ctrl_c_and_exit(2);
}

void recv_handle(void *arg) {
    char message[BUFF_SIZE];
    while(1) {
        recv_msg(sock, message, BUFF_SIZE);
        printf("%s", message);
        putchar('\n');
        memset(message, 0, sizeof(message));
    }
}

int main(int argc, char *argv[]) {
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

    // enter name
    printf("Enter your name: ");
    fgets(name, sizeof(name), stdin);

    if (strlen(name) > 32 || strlen(name) < 2) {
        printf("Name must be less than 30 and more than 2 characters.\n");
        return -1;
    }

    make_nice(name, 32);
    send_msg(sock, name, 32);

    printf("==== Client started ====\n");

    // create threads
    pthread_t send_thread;
    pthread_t recv_thread;

    // create send thread
    if (pthread_create(&send_thread, NULL, (void *) send_handle, NULL) != 0) {
        perror("pthread_create failed");
        return -1;
    }

    // create recv thread
    if (pthread_create(&recv_thread, NULL, (void *) recv_handle, NULL) != 0) {
        perror("pthread_create failed");
        return -1;
    }

    while (1){
	    if(flag){
		    printf("\nBye\n");
		    break;
        }
	}

    // close socket        
    close(sock);
    return 0;

} // client.c
