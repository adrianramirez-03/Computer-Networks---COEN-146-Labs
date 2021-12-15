//UDP Client
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>


#define SIZE 1024;

int main(){
 //Declare socket file descriptor. All  Unix  I/O  streams  are  referenced  by  descriptors
 int sockfd; 

 //Declare sending buffer of size 1k bytes
 char sbuf[1024]; 
 
 //Declare server address
 struct sockaddr_in servAddr; 

 //Converts  domain  names   into  numerical  IP  addresses  via  DNS
 struct hostent *host;
 host = (struct hostent *)gethostbyname("localhost"); //Local host runs the server. You may use "127.0.0.1": loopback IP address
 
 //Open a socket, if successful, returns  a descriptor  associated  with  an endpoint 
 if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("Failure to setup an endpoint socket");
    exit(1);
 }

 //Set the server address to send using socket addressing structure
 servAddr.sin_family = AF_INET;
 servAddr.sin_port = htons(5000);
 servAddr.sin_addr = *((struct in_addr *)host->h_addr);


 int read_bytes;

 //Client to send messages to the server continuously using UDP socket 
 while(1){
  printf("Client: Type a message to send to Server\n");
  scanf("%s", sbuf);
  
  FILE *source = fopen(sbuf, "r");
  char buffer[SIZE];
  
  if(source == NULL){
  	printf("Cannot open source\n");
	exit(0);
  }
  while(!feof(source)){
  	read_bytes = fread(&buffer, 1, 5, source);
	sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *) &servAddr, sizeof(struct sockaddr));
  }

  fclose(source);
  printf("Finished!\n");

 }
 return 0;
}
