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
#include <pthread.h>

#include "socket.h"
#include "sender.h"
#include "csi_collector.h"

int quit;
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
	u_int16_t   csi_buf_len;
	int port = atoi(argv[1]);
	int serverfd = 0, clientfd = 0;
	
	serverfd = create_server_sock(port);
	if(serverfd == -1) {
		printf("Could not start socket server, finishing application...\n");
		return 0;
	}
	printf("#Receiving data! Press Ctrl+C to quit!\n");

	quit = 0;
	char command_buffer[50];
	
	/* Send vars */
	char ifName[IFNAMSIZ];
	char input_buffer[100];
	unsigned int DstAddr[6];
	int i, quantity_of_packets, until_seconds;
	strcpy(ifName, argv[2]);

	/* Create thread what will control the packet storage into file */
	pthread_t tid;
	pthread_create(&tid, NULL, collect_csi_data, NULL);

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

			if(strcmp("SEND_PACKET", command_buffer) == 0) {
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

	return 0;
}

