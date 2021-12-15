//Name: Adrian Ramirez
//Date: 10/31/20
//Title: Lab 6: Stop and wait for an unreliable channel with loss
//Description: This program is the client for a stop and wait reliable protocol built on top of UDP that provides a reliable transport service while also considering loss



#include <arpa/inet.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include <netdb.h>
#include "lab6.h"


int seq = 0;

int get Checksum(Packet packet){
	packet.header.cksum = 0;
int checksum = 0;
char *ptr = (char *)&packet;
char *end = ptr + sizeof(Header) + packet.header.len;
while (ptr < end) {
	checksum ^= *ptr++;
	}
	return checksum;
}


void logPacket(Packet packet) {
	printf("Packet{ header: { seq_ack: %d, len: %d, cksum: %d }, data: \"",
			packet.header.seq_ack,
			packet.header.len,
			packet.header.cksum);
fwrite(packet.data, (size_t)packet.header.len, 1, stdout);
printf("\" }\n");
}


void ClientSend(int sockfd, const struct sockaddr *address, socklen_t addrlen, Packet packet, unsigned retries) {
	

	int attempts = 0;
    while (attempts < retries) {
        // calculate the checksum
        //******STUDENT WORK******
        
        // log what is being sent
        printf("Created: ");
        logPacket(packet);

        // Simulate lost packet.
        if (rand() % PLOSTMSG == 0) {
            printf("Dropping packet\n");
        } else {
            sendto(sockfd, &packet, sizeof(packet), 0, address, addrlen);
			printf("Sent this packet: \n");
			logPacket(packet);
            }

        // wait until either a packet is received or timeout, i.e using the select statement
        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        fd_set readfds;
        //******STUDENT WORK******
    	fcntl(sockfd, F_SETFL, O_NONBLOCK);

		FD_ZERO(&readfds);
		FD_SET(sockfd, &readfds);

		int rv;
			rv = select(sockfd + 1, &readfds, NULL, NULL, &tv);

		if(rv == 0){
			printf("means timeout need to resend \n");
			attempts++;
		}
		else if(rv == 0){
			
		
        // receive an ACK from the server
        Packet recvpacket;
        //******STUDENT WORK******
		recvfrom(sockfd, &recvpacket, sizeof(packet), 0, NULL, NULL);
        // log what was received
        printf("Received ACK %d, checksum %d - ", recvpacket.header.seq_ack, recvpacket.header.cksum);

        // validate the ACK
        if (recvpacket.header.cksum != getChecksum(recvpacket)) {
            // bad checksum, resend packet
            printf("Bad checksum, expected %d\n", getChecksum(recvpacket));
			ClientSend(sockfd, address, addrlen, packet, retries);
        } else if (recvpacket.header.seq_ack != seq) {
            // incorrect sequence number, resend packet
            printf("Bad seqnum, expected %d\n", packet.header.seq_ack);
        	ClientSend(sockfd, address, addrlen, packet, retries);
		} else {
            // good ACK, we're done
            printf("Good ACK\n");
            seq = !seq;
			break;
        }
	}
}
    printf("\n");
}

int main(int argc, char *argv[]) {
    // check arguments
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <ip> <port> <infile>\n", argv[0]);
        return 1;
    }

    // seed the RNG
    srand((unsigned)time(NULL));

    // create and configure the socket
    //******STUDENT WORK******
	int sockfd;
	char sbuf[10];

	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("Failure to setup an endpoint socket");
		exit(1);
	}
    // initialize the server address structure
    //******STUDENT WORK******
	struct sockaddr_in servAddr;

	struct hostent *host;
		host = (struct hostent *)gethostbyname(argv[1]);

	char *p;
	int portNum = 0;
	long conv = strtol(argv[2], &p, 10);
		portNum = conv;

		servAddr.sin_family = AF_INET;
		servAddr.sin_port = htons(portNum);
		servAddr.sin_addr = *((struct in_addr *)host->h_addr);

    // open file
    FILE *file = fopen(argv[3], "rb");
    if (file == NULL) {
        perror("fopen");
        return 1;
    }

    // send file contents to server
    Packet packet;
    //******STUDENT WORK******
	int bytes_read;
	while ((bytes_read = read(file, packet.data, sizeof(packet.data))) != 0) {
		packet.header.seq_ack = seq;
		packet.header.len = bytes_read;
		packet.header.cksum = getChecksum(packet);
	
	ClientSend(sockfd, (struct sockaddr *)&servAddr, sizeof(servAddr), packet, 3);
	}

    // send zero-length packet to server to end connection according the description provided in the Lab manual
    //******STUDENT WORK******
	packet.header.len = 0;
	packet.header.seq_ack = seq;
	packet.header.cksum = getChecksum(packet);
    
	ClientSend(sockfd, (struct sockaddr *)&servAddr, sizeof(servAddr), packet, 3);
	// clean up
    //******STUDENT WORK******
    close(file);
	close(sockfd);
	return 0;
}
