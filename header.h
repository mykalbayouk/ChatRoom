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

#define port_c 3333

void send_msg(int sock, char *msg);
void recv_msg(int sock, char *msg);

#endif /*HEADER.h*/
