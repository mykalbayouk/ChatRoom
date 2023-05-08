#ifndef _HEADER_H_
#define _HEADER_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>

#define BUFF_SIZE 2048
#define MAX_CLIENTS 10

typedef struct{
	struct sockaddr_in address;
	int sockfd;
	int uid;
	char name[32];
} client_t;

void send_msg(int sock, char *msg, int size);
void recv_msg(int sock, char *msg, int size);
void send_to_all(char *msg, int uid);
void make_nice(char *msg, int size);
void get_port(int argc, char *argv[], int *port);

#endif /*HEADER.h*/
