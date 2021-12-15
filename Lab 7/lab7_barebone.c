/* Name: Adrian Ramirez
 * Date: 11/14/20
 * Title: Lab 7 Link State Routing
 * Description: This program is the link state routing algorithm
*/

/*
 * Link State Routing
 * Lab 7
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <ctype.h>
#include <limits.h>
#include <string.h>


//defines
#define	N			4
#define	INFINITE	1000



// types
typedef struct machines
{
	char name[50];
	char ip[50];
	int port;
} MACHINES;


// global variables
MACHINES	machines[N];
int			costs[N][N];
int			distances[N];
int			myid, nodes;
int			sock;
struct sockaddr_in addr;
struct sockaddr_in otheraddr;
socklen_t addr_size = sizeof(struct sockaddr);
pthread_mutex_t	lock;



// functions
void *run_link_state (void *);
void *receive_info (void *);
void print_costs (void);
void *update_costs (void *);

/**********************
 * main
 **********************/
int
main (int argc, char *argv[])
{
	FILE	*fp;
	int		i, j;
	pthread_t	thr1, thr2, thr3;
	int		id, cost;
	int		packet[3];


	if (argc < 5)
	{
		printf("Arguement number erro");
		return 1;
	}

	myid = atoi(argv[1]);
	nodes = atoi(argv[2]);

	if (myid >= N)
	{
		printf ("wrong id\n");
		return 1;
	}	

	if (nodes != N)
	{
		printf ("wrong number of nodes\n");
		return 1;
	}	
	
	//get info on machines from the file using fopen and fscanf and populate the array of machines-struct
	//******STUDENT WORK******
	

	// get costs from the costs file and populate the matrix
	if ((fp = fopen (argv[3], "r")) == NULL)
	{
		printf("Was not able to read file");
		return 1;
	}
	
	for(i = 0; i < N; i++){
		for(j = 0; j < N; j++){
			fscanf(fp, "%d", &costs[i][j]);
		}
	}
	fclose(fp);

	fp = fopen(argv[4], "r");
	for(i = 0; i < N; i++){
		fscanf(fp, "%s %s %d", machines[i].name, machines[i].ip, &machines[i].port);
	}
	fclose(fp);


    // init address
    addr.sin_family = AF_INET;
	addr.sin_port = htons(myid);
	addr.sin_addr.s_addr = INADDR_ANY;

    // create socket
    if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
		perror("Failure to setup an endpoint socket");
		exit(1);
	}

    // bind
    if((bind(sock, (struct sockaddr *) &addr, sizeof(struct sockaddr))) < 0){
		perror("Failure to bind server address to the endpoint socket");
		exit(1);
	}

	// create threads and initiate mutex
	pthread_mutex_init (&lock, NULL);

	// read changes from the keyboard, update the costs matrix inform other nodes about the changes
 	// This section can be inside the main or can be a thread by itself
 		for (i = 0; i < 3; i++)
		{
			if (i == 0){
				pthread_create(&thr1, NULL, receive_info, NULL);
			} else if (i == 1){
				pthread_create (&thr2, NULL, update_costs, NULL);
			} else if (i == 2){
				pthread_create (&thr3, NULL, run_link_state, NULL);
			}
		}
	pthread_join(thr1, NULL);
	pthread_join(thr2, NULL);
	pthread_join(thr3, NULL);

	sleep (5);

}


/**********************
 * receive info
 **********************/
//If there is a cost update on other machine, they will let us know through a UDP packet. We need to receive that packet and update our costs table
void *receive_info (void *arg)
{
	int	new[3];
	int	nbytes;
	int	n0, n1, n2;
	printf("receive_info\n");

	while(true){
		n0 = new[0];
		n1 = new[1];
		n2 = new[2];

		printf("New update received, trying to update\n");

		pthread_mutex_lock(&lock);
		costs[n0][n1] = n2;
		costs[n1][n0] = n2;
		pthread_mutex_unlock(&lock);

		printf("Updated costs:\n");
		print_costs();
	}	
}

void *update_costs (void *arg)
{
	printf("update_cost\n");
	int neighborId;
	int newCost;

	while(1) {
		printf("Enter neighbor's id and new cost: \n");
		scanf("%d %d", &neighborId, &newCost);

		pthread_mutex_lock(&lock);
		costs[myid][neighborId] = newCost;
		costs[neighborId][myid] = newCost;
		pthread_mutex_unlock(&lock);

		printf("Updated table");
		print_costs();

		otheraddr.sin_family = AF_INET;

		int i;
		for(i = 0; i < N; i++){
			if(i == myid){
				printf("my router");
				continue;
			}

			otheraddr.sin_port = htons(machines[i].port);
			inet_pton(AF_INET, machines[i].ip, &otheraddr.sin_addr.s_addr);

			int info[3] ={
				myid,
				neighborId,
				newCost
			};
			printf("Updating %s \t %d \n", machines[i].ip, machines[i].port);
			sendto(sock, info, sizeof(info), 0, (struct sockaddr*) &otheraddr, addr_size);
			printf("sent\n");
		}
	}
}


/**********************
 * run_link_state
 **********************/
// This thread runs the Dijkastra's algorithm on the cost table and calculates thte shortest path
void *
run_link_state (void *arg)
{
	int	taken[N];
	int	min, spot;
	int	i, j;
	int	r;
	int minIndex, count;
	int distances[N];

	while(1)
	{
		for(i = 0; i < N; i++){
			for(j = 0; j < N; j++){
				taken[j] = 0;
				distances[j] = costs[i][j];
			}
			taken[j] = 1;
			count = N - 1;

			while(count > 0){
				min = 1;
				minIndex = -1;
				for(j = 0; j < N; j++){
					if(distances[N] < min && taken[j] == 0){
						min = distances[j];
						spot = j;
					}
				}
				taken[spot] = j;
				for(r = 0; r < N; r++){
					if(taken[r] == 0){
						if((distances[minIndex] + costs[minIndex][r] < distances[r])){
							distances[r] = distances[minIndex] + costs[minIndex][r];
						}
					}
				}
				count--;
			}
			printf ("new-shortest distances:\n");
			for (i = 0; i < N; i++)
			printf ("%d ", distances[i]);
			printf ("\n");

		}
		sleep(5);
	}
}


/**********************
 * print costs
 **********************/
void print_costs (void)
{
	int i, j;

	for (i = 0; i < N; i++)
	{
		for (j = 0; j < N; j++)
		{
			pthread_mutex_lock (&lock);
			printf ("%d ", costs[i][j]);
			pthread_mutex_unlock (&lock);
		}
		printf ("\n");
	}

}
