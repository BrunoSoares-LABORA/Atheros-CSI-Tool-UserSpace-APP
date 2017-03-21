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
#include <net/if.h>
#include <linux/if_packet.h>

#include "csi_fun.h"
#include "socket.h"
#include "sender.h"

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
	if (argc != 3) {
		printf("/*******************************************/\n");
		printf("/*   Usage: recv_csi <port> <interface>    */\n");
		printf("/*******************************************/\n");
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
	
	/* Send vars */
	char ifName[IFNAMSIZ];
	char input_buffer[100];
	unsigned int DstAddr[6];
	int i, quantity_of_packets, until_seconds;
	strcpy(ifName, argv[2]);
	
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

				csi_buf_len = csi_status->buf_len;
				printf("sending csi packet len: %d\n", csi_buf_len);
				send(clientfd, &csi_buf_len, sizeof(csi_buf_len), 0);
				memset(&command_buffer[0], 0, sizeof(command_buffer));
	                        recv(clientfd, command_buffer, 45, 0);
				if(strcmp("RECEIVER_READY", command_buffer) == 0) {
					send(clientfd, buf_addr, csi_buf_len, 0);
					printf("CSI packet sent\n");
				} else {
					printf("Unknown command received '%s', closing connection...\n", command_buffer);
					break;
				}
			} else if(strcmp("SEND_PACKET", command_buffer) == 0) {
				// Receive destination mac address
				memset(&input_buffer[0], 0, sizeof(input_buffer));
				recv(clientfd, input_buffer, 17, 0);
				sscanf(input_buffer,"%x:%x:%x:%x:%x:%x",&DstAddr[0],&DstAddr[1],&DstAddr[2],&DstAddr[3],&DstAddr[4],&DstAddr[5]);
				
				// Receive how much packets will be sent and loop size
				memset(&input_buffer[0], 0, sizeof(input_buffer));
				recv(clientfd, input_buffer, 95, 0);
				quantity_of_packets = atoi(strtok(input_buffer, "/"));
				until_seconds = atoi(strtok(NULL, "/"));
				
				// send packets...
				int rawsock = create_raw_socket();
				struct ifreq if_idx = get_interface_index(rawsock, ifName);
				struct ifreq if_mac = get_interface_mac_address(rawsock, ifName);
				PACKET packet_to_send = create_packet(if_idx, if_mac, DstAddr);
				
				printf("Sending packets %d for %d seconds...\n", quantity_of_packets, until_seconds);
				for(i = 0; i < until_seconds; i++) {
					send_packet(rawsock, packet_to_send, quantity_of_packets);
				}
				printf("Packets sent\n");
				close(rawsock);
			} else {
				printf("Unknown command received, closing connection...\n");
				break;
			}
		}

		printf("Connection with client closed...\n");
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

