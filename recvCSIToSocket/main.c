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
#include <arpa/inet.h>
#include <errno.h>
#include <time.h> 


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

    fd = open_csi_device();
    if (fd < 0){
        perror("Failed to open the device...");
        return errno;
    }
    
    /* Open socket server */
    int port = atoi(argv[1]);
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr;
    char sendBuff[BUFSIZE];

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port); 

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 
    listen(listenfd, 10); 
    
    printf("#Receiving data! Press Ctrl+C to quit!\n");

    quit = 0;
    total_msg_cnt = 0;
    
    while(1) {
		connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
		
		while(1) {
			if (1 == quit) {
				close(connfd);
				close(listenfd);
				close_csi_device(fd);
				return 0;
			}
			
			/* keep listening to the kernel and waiting for the csi report */
			cnt = read_csi_buf(buf_addr,fd,BUFSIZE);

			if (cnt) {
				total_msg_cnt += 1;

				/* fill the status struct with information about the rx packet */
				record_status(buf_addr, cnt, csi_status);

				/* 
				 * fill the payload buffer with the payload
				 * fill the CSI matrix with the extracted CSI value
				 */
				record_csi_payload(buf_addr, csi_status, data_buf, csi_matrix); 
				
				
				printf("Recv %dth msg with rate: 0x%02x | payload len: %d\n",total_msg_cnt,csi_status->rate,csi_status->payload_len);
				
				/* log the received data for off-line processing */
				buf_len = csi_status->buf_len;
				
				write(connfd, buf_addr, buf_len); 
			}
		}
	}
	close(connfd);
    close_csi_device(fd);
    free(csi_status);
    return 0;
}
