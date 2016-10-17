#define BUF_SIZE 2048

typedef struct {
	char send_buffer[BUF_SIZE];
	struct ether_header *eh;
	struct iphdr *iph;
	struct sockaddr_ll socket_address;
	int len;
} PACKET;

int create_raw_socket();
struct ifreq get_interface_index(int rawsock, char *if_name);
struct ifreq get_interface_mac_address(int rawsock, char *if_name);
PACKET create_packet(struct ifreq if_idx, struct ifreq if_mac, unsigned int dst_addr[6]);
void send_packet(int rawsock, PACKET packet_to_send, unsigned int quantity);
