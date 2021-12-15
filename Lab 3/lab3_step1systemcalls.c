/* Name: Adrian Ramirez
 * Date: 10/11/20
 * Title: Lab 3 - Part 1 system calls
 * Description: C program to copy binary and text using system calls
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
	
	char buff[100];

	//first file
	int source = open(argv[1], O_RDWR);
	if(source == -1){
		puts("Cannot open file");
		exit(0);
	}

	//create file if one doesn't exist or prepare to write to it
	int dest = open(argv[2], O_RDWR);
	if(dest == -1){
		perror("Destination file error");
		exit(0);
	}

	//write to the file until we reach the end of source file
	while(read(source, &buff, sizeof(buff))){
		printf("%s", buff);
		write(dest, &buff, strlen(buff));
	}
	preintf("\nContents are now successfully copied to new file.");
	close(source);
	clouse(dest);

	return 0;
}
