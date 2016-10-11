/*
 * =====================================================================================
 *       Filename:  main.c
 *
 *    Description:  Here is an example for receiving CSI matrix
 *                  Basic CSi procesing fucntion is also implemented and called
 *                  Check csi_fun.c for detail of the processing function
 *        Version:  1.0
 *
 *         Author:  Yaxiong Xie
 *         Email :  <xieyaxiongfly@gmail.com>
 *   Organization:  WANDS group @ Nanyang Technological University
 *
 *   Copyright (c)  WANDS group @ Nanyang Technological University
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <errno.h>
#include <time.h>
#include <math.h>

#include "csi_fun.h"

#define BUFSIZE 4096

int quit;
unsigned char buf_addr[BUFSIZE];
unsigned char data_buf[1500];

COMPLEX csi_matrix[3][3][114];
csi_struct*   csi_status;

void sig_handler(int signo)
{
    if (signo == SIGINT)
        quit = 1;
}

int enable_keepalive(int sock) {
    setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &(int){ 1 }, sizeof(int));
    setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &(int){ 1 }, sizeof(int));
    setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &(int){ 1 }, sizeof(int));
    setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &(int){ 10 }, sizeof(int));
}

int main(int argc, char* argv[])
{
    /* check usage */
    if (argc != 2){
        printf("/**************************************/\n");
        printf("/*   Usage: recv_csi <port>    */\n");
        printf("/**************************************/\n");
        return 0;
    }

    int         fd;
    int         i;
    int         total_msg_cnt,cnt;
    int         log_flag;
    u_int16_t   buf_len;
    csi_status = (csi_struct*)malloc(sizeof(csi_struct));

    /* Open socket server */
    int port = atoi(argv[1]);
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr;
    char sendBuff[BUFSIZE];

    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);

    listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(listenfd == -1) {
	printf("Could not create socket\n");
        return 0;
    }

    // Set socket reusable
    if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) == -1) {
	printf("Could not set socket reusable\n");
	return 0;
    }

    if(bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
	printf("Could not bind socket\n");
	return 0;
    }
    if(listen(listenfd, 1) == -1) {
	printf("Could not listen socket\n");
	return 0;
    }

    printf("#Receiving data! Press Ctrl+C to quit!\n");

    quit = 0;
    total_msg_cnt = 0;
    char buff_size[50];

    while(1) {
		printf("Waiting for client connection...\n");
		connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
		enable_keepalive(connfd);
		printf("Connection received...\n");

		/* Connection with client oppened, create device reading */
		fd = open_csi_device();
		if (fd < 0) {
			printf("Failed to open the CSI device...\n");
			close(connfd);
			continue;
		}

		while(1) {
			printf("Waiting for client command...\n");
			memset(&buff_size[0], 0, sizeof(buff_size));
			recv(connfd, buff_size, 45, 0);
			printf("Command received: %s\n", buff_size);

			if(strcmp("PACKET_REQUEST", buff_size) == 0) {
				/* keep listening to the kernel and waiting for the csi report */
				do {
					cnt = read_csi_buf(buf_addr, fd, BUFSIZE);
				} while(!cnt);

				total_msg_cnt += 1;
				record_status(buf_addr, cnt, csi_status);

				printf("sending csi packet\n");
				u_int16_t csi_buf_len = csi_status->buf_len;
				send(connfd, &csi_buf_len, sizeof(csi_buf_len), 0);
				send(connfd, buf_addr, csi_buf_len, 0);
				printf("CSI packet sent\n");
			} else {
				printf("Unknown command received, closing connection...\n");
				break;
			}
		}

		printf("Connection with client closed, closing devices...\n");
		close(connfd);
		close_csi_device(fd);

		if(quit == 1) {
			printf("Shutdown command received, finishing...\n");
			break;
		}
    }

    free(csi_status);
    return 0;
}

