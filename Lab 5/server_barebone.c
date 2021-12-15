//Name: Adrian Ramirez
//Date: 10/30/20
//Title: Lab 5: Stop and wait for an unreliable channel
//Description: This program is the server for a stop and wait reliable protocl built on top of UDP that provides a reliable transport service


#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include "lab5.h"

int seq = 0;

// NOTE: beware of sign extension if the checksum is stored in a char, then converted to an int!
int getChecksum(Packet packet) {
    packet.header.cksum = 0;

    int checksum = 0;
    char *ptr = (char *)&packet;
    char *end = ptr + sizeof(Header) + packet.header.len;

//Please find an error from the remaining part getChecksum() function 
//******STUDENT WORK****** 
    while (ptr < end) {
        checksum ^= ptr++;
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

//Sending a ACK to the client, i.e., letting the client know what was the status of the packet it sent.
void ServerSend(int sockfd, const struct sockaddr *address, socklen_t addrlen, int seqnum) {
    Packet packet;
	//-Prepare the packet headers
	//-send the packet
    	//******STUDENT WORK******
		packet.header.seq_ack = seqnum;
		packet.header.len = 0;
		packet.header.cksum = getChecksum(packet);

		sendto(sockfd, &packet, sizeof(packet), 0, address, addrlen);		

    printf("Sent ACK %d, checksum %d\n", packet.header.seq_ack, packet.header.cksum);
}

Packet ServerReceive(int sockfd, struct sockaddr *address, socklen_t *addrlen, int seqnum) {
    Packet packet;

    while (1) {
        //recv packets from the client
		//******STUDENT WORK******
		printf("recvfrom\n");
		recvfrom(sockfd, &packet, sizeof(packet), 0, address, addrlen);

        // log what was received
        printf("Received: ");
        logPacket(packet);

        if (packet.header.cksum != getChecksum(packet)) {
            printf("Bad checksum, expected checksum was: %d\n", getChecksum(packet));
            ServerSend(sockfd, address, *addrlen, !seqnum);
        } else if (packet.header.seq_ack != seqnum) {
            printf("Bad seqnum, expected sequence number was:%d\n", seqnum);
            	//******STUDENT WORK******
				ServerSend(sockfd, address, *addrlen, !seqnum);
        } else {
            printf("Good packet\n");
            	//******STUDENT WORK******
				ServerSend(sockfd, address, *addrlen, seqnum);
            break;
        }
    }

    printf("\n");
    return packet;
}


int main(int argc, char *argv[]) {
    // check arguments
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <port> <outfile>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // seed the RNG
    srand((unsigned)time(NULL));

    // create a socket
	//******STUDENT WORK******
	int sockfd;
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("Failure to setup an endpoint socket");
		exit(1);
	}

	struct hostent *host;
	host = (struct hostent *)gethostbyname("localhost");
    // initialize the server address structure using argv[1]
    struct sockaddr_in address;
	//******STUDENT WORK******
	socklen_t addrlen = sizeof(struct sockaddr);

	address.sin_family = AF_INET;
	address.sin_port = htons(atoi(argv[1]));
	address.sin_addr = *((struct in_addr *)host->h_addr);

    // bind the socket
	// [Question]: Do we need to bind the socket on the client side? Why?/Why not?
	//******STUDENT WORK******
	if ((bind(sockfd, (struct sockaddr *)&address, sizeof(struct sockaddr))) < 0) {
		perror("Failure");
		exit(1);
	}
    
	// open file using argv[2]
	//******STUDENT WORK******
	int fdest;
	fdest = open(argv[2], O_RDWR);

    // get file contents from client
    int seqnum = 0;
    Packet packet;
    struct sockaddr_in clientaddr;
    socklen_t clientaddr_len = sizeof(clientaddr);
    do {
        packet = ServerReceive(sockfd, (struct sockaddr*)&address, &addrlen, seq);
		write(fdest, packet.data, packet.header.len);
		seq = !seq;
    } while (packet.header.len != 0);

    fclose(fdest);
    close(sockfd);
    return 0;
}
