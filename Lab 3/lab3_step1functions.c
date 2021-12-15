/*Name: Adrian Ramirez
 * Date: 10/11/20
 * Title: Lab 3 - Part 1
 * Description: C program to copy binary and text using functions
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
	FILE *source, *dest; //pointers of type FILE
	
	FILE *source = fopen(argv[1], "r");
	if(source == NULL){ //checks if file is empty
		puts("Cannot open file");
		exit(0);
	}

	//second file will be written to 
	dest = fopen(argv[2], "w");
	if (dest == NULL) {
		puts("Cannot open file");
		exit(0);
	}

	char buffer[1024];
	int read_bytes;



	while(!feof(source)){
		read_bytes = fread(&buffer, 1, sizeof(buffer), source);
		printf("%s", buffer);
		fwrite(&buffer, 1 ,read_bytes, dest);
	}

	fclose(source);
	fclouse(dest);
	return 0;

}
