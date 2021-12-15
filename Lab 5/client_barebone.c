//Name: Adrian Ramirez
//Date: 10/30/20
//Title: Lab 5: Stop and Wait for an unreliable channel
//Description: Clien for a stop and wait reliable protocl built on top of UDP that provides a reliable transport service



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
#include <netinet/in.h>
#include <netdb.>
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



void ClientSend(int sockfd, const struct sockaddr *address, socklen_t addrlen, Packet packet) {
    while (1) {

        // send the packet
        printf("Sending packet\n");
		//******STUDENT WORK******
			logPacket(packet);
			sendto(sockfd, &packet, sizeof(packet), 0, address, addrlen);
		printf("sendto\n");

        // receive an ACK from the server
        Packet recvpacket;
		//******STUDENT WORK******
		printf("recvfrom\n");
			recvfrom(sockfd, &recvpacket, 1024, 0, address, &addrlen);
        // log what was received
        printf("Received ACK %d, checksum %d - ", recvpacket.header.seq_ack, recvpacket.header.cksum);

        // validate the ACK
        if (recvpacket.header.cksum != getChecksum(recvpacket)) {
            // bad checksum, resend packet
            printf("Bad checksum, expected checksum was: %d\n", getChecksum(recvpacket));
        } else if (recvpacket.header.seq_ack != seq) {
            // incorrect sequence number, resend packet
            printf("Bad seqnum, expected sequence number was: %d\n", seq);
        } else {
            // good ACK, we're done
            printf("Good ACK\n");
            seq = !seq;
			break;
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

    // create and configure the socket [name the socket sockfd] 
		//******STUDENT WORK******	    
		int sockfd;
	if ((sockfd == socket(AF_INET, SOCK_DGRAM, 0)) < 0){
		perror("Failure to set up an endpoint socket");
		exit(1);
	}

	struct hostent *host;
	host = (struct hostent *)gethostbyname(argv[1]);

    // initialize the server address structure using argv[2] and argv[1]
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
	//******STUDENT WORK******
	socklen_t addrlen = sizeof(struct sockaddr);

	address.sin_family = AF_INET;
	address.sin_port = htons(atoi(argv[2]));
	address.sin_addr = *((struct in_addr *)host->h_addr);
    // open file using argv[3]
	//******STUDENT WORK******
		int fsrc;
		fsrc = open(argv[3], O_RDWR);
   	 // send file contents to server
		//-create a packet
		//-set the seq_ack field 
		//-set the len field
		//-calculate the checksum
		//-send the packet
		//******STUDENT WORK******
		Packet p1;
		int bytes_read;
		while ((bytes_read = read(fsrc, p1.data, sizeof(p1.data))) != 0) {
			p1.header.seq_ack = seq;
			p1.header.len = bytes_read;
			int randomNumber = rand()%(10) + 1;
			printf("randomNumber: %d", randomNumber);
			if (randomNumber <= 8) { // 8 out of 10 time
				p1.header.cksum = getChecksum(p1);
			}
			else {	
				p1.header.cksum = 100;
			}
			ClientSend(sockfd, &address, sizeof(struct sockaddr), p1);
		}	

    // send zero-length packet to server to end connection
		//******STUDENT WORK******
		p1.header.len = 0;
		p1.header.seq_ack = seq;
		p1.header.cksum = getChecksum(p1);
	ClientSend(sockfd, &address, sizeof(struct sockaddr), p1);
    // clean up
	//-close the sockets and file
    	//******STUDENT WORK******
		close(fsrc);
		close(sockfd);

    return 0;
}
