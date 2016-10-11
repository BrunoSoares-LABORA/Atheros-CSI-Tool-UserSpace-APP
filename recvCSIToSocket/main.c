#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include <math.h>

#include "csi_fun.h"
#include "socket.h"

#define BUFSIZE 4096

int quit;
unsigned char buf_addr[BUFSIZE];
unsigned char data_buf[1500];

void sig_handler(int signo) {
    if (signo == SIGINT) {
        quit = 1;
	}
}

int main(int argc, char* argv[]) {
	/* check usage */
	if (argc != 2){
		printf("/**************************************/\n");
		printf("/*   Usage: recv_csi <port>    */\n");
		printf("/**************************************/\n");
		return 0;
	}

	/* Open socket server */
	int         fd, cnt;
	u_int16_t   csi_buf_len;
	int port = atoi(argv[1]);
	int serverfd = 0, clientfd = 0;
	
	serverfd = create_server_sock(port);
	if(serverfd == -1) {
		printf("Could not start socket server, finishing application...\n");
		return 0;
	}
	printf("#Receiving data! Press Ctrl+C to quit!\n");

	csi_struct*   csi_status = (csi_struct*)malloc(sizeof(csi_struct));
	quit = 0;
	char command_buffer[50];
	
	/* Create device reading */
	fd = open_csi_device();
	if (fd < 0) {
		printf("Failed to open the CSI device...\n");
		return 0;
	}

	/* Client connection and command processing */
	while(1) {
		printf("Waiting for client connection...\n");
		clientfd = wai_for_client(serverfd);
		if(clientfd == -1) {
			printf("Could not proceed with client connection, closing it...\n");
			close(clientfd);
			continue;
		}
		printf("Connection received...\n");
		
		while(1) {
			printf("Waiting for client command...\n");
			memset(&command_buffer[0], 0, sizeof(command_buffer));
			recv(clientfd, command_buffer, 45, 0);
			printf("Command received: %s\n", command_buffer);

			if(strcmp("PACKET_REQUEST", command_buffer) == 0) {
				/* keep listening the kernel until csi report */
				do {
					cnt = read_csi_buf(buf_addr, fd, BUFSIZE);
				} while(!cnt);
				
				record_status(buf_addr, cnt, csi_status);

				printf("sending csi packet\n");
				csi_buf_len = csi_status->buf_len;
				send(clientfd, &csi_buf_len, sizeof(csi_buf_len), 0);
				send(clientfd, buf_addr, csi_buf_len, 0);
				printf("CSI packet sent\n");
			} else {
				printf("Unknown command received, closing connection...\n");
				break;
			}
		}

		printf("Connection with client closed, closing devices...\n");
		close(clientfd);

		if(quit == 1) {
			printf("Shutdown command received, finishing application...\n");
			break;
		}
	}

	close_csi_device(fd);
	free(csi_status);
	return 0;
}

