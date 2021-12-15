/* Adrian Ramirez
 * 9/25/20
 * COEN 146L : Lab 1 Q#5
 * Instead of using two processes, used two threads*/

#include <stdio.h> //printf, stderr
#include <stdlib.h> //atoi
#include <pthread.h> //pthread
#include <errno.h> //errno
#include <unistd.h> //fork

#define NTHREADS 2
pthread_t threads[NTHREADS];

void *threadRoutine(void *arg){
	int n, i;
	n = (int)arg;
	for(i = 0; i < 100; i++){
		printf("Thread with ID %d is printing iteration %d\n", pthread_self(), i);
		usleep(n);
	}
	return 0;
}

//main function
int main(int argc, char *argv[]){
	int n, i;
	n = atoi(argv[1]);
	printf("\nBefore forking threads..\n");
	for(i = 0; i < NTHREADS; i++){
		pthread_create(&threads[i], NULL, threadRoutine, (void *)(size_t)n);
		//thread creation
	}
	for(i = 0; i < NTHREADS; i++){
		pthread_join(threads[i], NULL);
	}
}
