/*
 * Name: Adrian Ramirez
 * Date: 10/17/20
 * Title: Lab 4 - File distribution: Client server vs P2P
 * Description: TCP server for multiple file trsanfer
 */


// UDP Server
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#define N 1024



int fd:
pthread_t threader[N]; //N = number of client the thread can hold

void *readWriteClients (void *tracker){
	int bytes_received;
	char recv_data[1024];
	int i = (int)tracker;
	while ((bytes_received = recv(i, recv_data, 1024, 0))){
		write(fd, recv_data, bytes_received);
		printf("Writing %d bytes", bytes_received);
		if (bytes_received == 0){
			break;
		}
	}
}

int main(){
 //Declare socket file descriptor. All  Unix  I/O  streams  are  referenced  by  descriptors
 int sockfd, connfd, rb, sin_size;;
 
 //Declare sending and recieving buffer of size 1k bytes
 char rbuf[1024], sbuf[1024];;

 //Declare server address to which to bind for receiving messages and client address to fill in sending address
 struct sockaddr_in servAddr, clienAddr;


 //Open a TCP socket, if successful, returns  a descriptor  associated  with  an endpoint 
 if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Failure to setup an endpoint socket");
    exit(1);
 }
 
 //Setup the server address to bind using socket addressing structure
 servAddr.sin_family = AF_INET;
 servAddr.sin_port = htons(5000); //Port 5000 is assigned
 servAddr.sin_addr.s_addr = INADDR_ANY; //Local IP address of any interface is assigned (generally one interface IP address)

 //Set address/port of server endpoint for socket socket descriptor
 if ((bind(sockfd, (struct sockaddr *)&servAddr, sizeof(struct sockaddr))) < 0){
	perror("Failure to bind server address to the endpoint socket");
    exit(1);
 }

 printf("Server waiting for client at port 5000\n");
 //server is listening to socket endpoint and can queue 5 client requests
 listen(sockfd, 5);

 sin_size = sizeof(struct sockaddr_in);
 int read_bytes;
 fd = open("dest.dat", O_WRONLY, 0);
 int j = 0;
 while(1){
 	//Server aceepting to establish a connection with a connecting client, if it is successful it will return a connection descriptor
	connfd = accept(sockfd, (struct sockaddr *) &clienAddr, (socklen_t * ) & sin_size);
	printf("connection accepted");
	pthread_create(&threader[j], NULL, readWriteClients, (void *)(size_t)connfd);
	j++;
 }

 printf("Connection Established with client: IP %s and Port %d\n", inet_ntoa(clienAddr.sin_addr), ntohs(clienAddr.sin_port));
 close(connfd);
 close(fd);
 close(sockfd);
 
 /*
 char filename[1024];
 printf("Server: Type a filename to save the messages sent by the client\n");
 scanf("%s", filename);


//Sever continuously waits for messages from client, then prints incoming messages.
 while (1){
    printf("Server waiting for messages from client: \n");
	int nr = recvfrom(sockfd, rbuf, 1024, 0, (struct sockaddr *)&clienAddr, &addrLen);
	rbuf[nr] = '\0';
	printf("Client with IP: %s and Port: %d sent message: %s\n", inet_ntoa(clienAddr.sin_addr),ntohs(clienAddr.sin_port), rbuf);
 	FILE *fdest = fopen(filename, "a");
	fwrite(rbuf, 1, sterlen(rbuf), fdest);
	close(fdest);
 
 
 }
 */
 return 0;
}
