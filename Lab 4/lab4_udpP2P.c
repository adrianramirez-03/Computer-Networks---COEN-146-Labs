//Name: Adrian Ramirez
//Date: 10/18/20
//Title: Lab 4 Step 2
//Description: Multiple Clients with UDP/IP for P2P file distribution peer 1



#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define SIZE 100

//Declare socket file descriptor
int sockfd;

//Declare sending and recieving buffer of size 1k bytes
char sbuf[1024], rbuf[1024];

//Declare server address to which to bind for receiving messages and client address to fill in sending address
struct sockaddr_in servAddr1, servAddr2;
socklen_t addrLen = sizeof(struct sockaddr);

struct hostent *host;
char host_name[20], fsrc[20], fdest[20];

void openSocket(){
	printf("Opening up the socket\n");
	host = (struct hostent *)gethostbyname(host_name);

	if(host == NULL){
		perror("Failure to resolve hostname");
		exit(1);
	}
	
	//Set the server address to send using socket addressing structure
	servAddr1.sin_family = AF_INET;
	servAddr1.sin_port = htons(5001);
	servAddr1.sin_addr = *((struct in_addr *)host->h_addr);

	//Setup server address to bind using socket addressing structure
	servAddr2.sin_family = AF_INET;
	servAddr2.sin_port = htons(5002); //port 5000 is assigned
	servAddr2.sin_addr = *((struct in_addr *)host->h_addr); //local IP address of any interface is assigned

	//Open a socket, if successfut it returns 0
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
		perror("Failure to setup an endpoint socket");
		exit(1);
	}
}

void server(){
	printf("server function\n");
	gethostname(host_name, 20);
	printf("Server hostname: %s\n", host_name);
	openSocket();

	//Set address/port of server endpoint for socket descriptor
	if((bind(sockfd, (struct sockaddr *)&servAddr1, sizeof(struct sockaddr))) < 0){
		perror("Failure to bind server address to the endpoint socket");
		exit(1);
	}

	int rb;
	while((rb = recvfrom(sockfd, rbuf, 1024, 0, (struct sockaddr *)&servAddr2, &addrLen)) > 0){
		rbuf[rb] = '\0';
		printf("Message from client: %s\n", rbuf);
		if(strcmp(rbuf, "REQUEST") == 0){
			break;
		}
	}

	//server open the source file
	int fp = open(fsrc, O_RDWR);
	if (fp < 0){
		perror("Cannot open src \n");
		sendto(sockfd, "ERROR", strlen("ERROR"), 0, (struct sockaddr *)&servAddr2, sizeof(struct sockaddr));
		exit(1);
	}

	sendto(sockfd, "SENDING", strlen("SENDING"), 0, (struct sockaddr *)&servAddr2, sizeof(struct sockaddr));

	//Server read the rouce file and send it in chunks of size sbuf size to the client
	int bytes_read;
	while (0 < (bytes_read = read(fp, sbuf, sizeof(sbuf)))){
		sendto(sockfd, sbuf, bytes_read, 0, (struct sockaddr *)&servAddr2, sizeof(struct sockaddr));
	}

	sendto(sockfd, "EOF", sizeof("EOF"), 0, (struct sockaddr *)&servAddr2, sizeof(struct sockaddr));

	//close file
	close(fp);
	printf("Done with file!\n");

	//close socket
	close(sockfd);
}

void client(){
	printf("client function\n");
	printf("Enter server hostname to receive from: ");
	scanf("%s", host_name);
	openSocket();

	//client send its file request
	sendto(sockfd, "REQUEST", strlen("REQUEST"), 0, (struct sockaddr *)&servAddr2, sizeof(struct sockaddr));
	printf("Sent file request to server\n");

	//waiting response
	int rb;
	while((rb = recvfrom(sockfd, rbuf, 1024, 0, (struct sockaddr *)&servAddr2, &addrLen)) > 0){
		rbuf[rb] = '\0';
		printf("Message sent to server: %s\n", rbuf);
		if(strcmp(rbuf, "SENDING") == 0){
			break;
		}
		else if(strcmp(rbuf, "ERROR") == 0){
			perror("Cannot open src in servAddr2\n");
			exit(1);
		}
	}


	int fp = open(fdest, O_RDWR | O_CREAT, 0777);
	if(fp < 0){
		perror("Cannot open srs \n");
		exit(1);
	}

	while((rb = recvfrom(sockfd, rbuf, 1024, 0, (struct sockaddr *)&servAddr2, &addrLen)) > 0){
		rbuf[rb] = '\0';
		if(strcmp(rbuf, "EOF") == 0){
			printf ("End of file\n");
			break;
		}
		else{
			write(fp , rbuf, strlen(rbuf));
		}
	}

	//close file
	close(fp);
	printf("Done with file..\n");

	//close socket
	close(sockfd);
}

int main(){
	while(1){
		char input;
		printf("Peer 1: would you like to send (S), receive (R), or quit (Q)? \n");
		scanf("%c", &input);
		if(input == 'S' || input == 's'){
			printf("Peer 1 is sending!\n");
			printf("Enter filename: ");
			scanf("%s", fsrc);
			server();
		}
		else if(input == 'R' || input == 'r'){
			printf("Peer 1 is receiving\n");
			printf("Enter filename: ");
			scanf("%s", fdest);
			client();
		}
		else if(input == 'Q' || input == 'q'){
			exit(1);
		}
	}
	return 0;
}
