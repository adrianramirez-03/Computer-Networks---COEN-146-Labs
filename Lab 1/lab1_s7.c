/*  Adrian Ramirez
 *  9/28/20
 *  COEN 146L: Lab1 - Step 6/7
 *  Computation comparison of circuit switching and packet switching */
	
#include <stdio.h>
#include <math.h>
#include <stdlib.h>


double factorial(double n){
	if(n <= 1)
		return 1;
	else
		return n * factorial(n-1);
}


int main(int arg, char *argv[]){
	int i, linkBandwidth, userBandwidth, nCSusers, nPSusers;
	double pPSusers, tPSusers, pPSusersBusy, pPSusersNotBusy;

	//These are the command line values
	linkBandwidth = atoi(argv[1]);
	userBandwidth = atoi(argv[2]);
	tPSusers = atof(argv[3]);
	nPSusers = atoi(argv[4]);

	//6a: Circuit switching scenario
	nCSusers = linkBandwidth/userBandwidth;
	printf("6a: Circuit Switching: %d users\n", nCSusers);

	//PACKET SWITCHING SCENARIOS 
	
	//7a: probability that a given (specific) user is busy transmitting is computed as
	pPSusers = tPSusers;
	printf("7a: The probability that a given (specific user is busy transmitting is: %f\n", pPSusers);

	//7b: probability that one specific other user is not busy is computed as
	pPSusersNotBusy = 1 - pPSusers;
	printf("7b: The probability that one specific other user is not busy is: %f\n", pPSusersNotBusy);

	//7c: probability that all of the other specific other users are not busy is computed as
	double pPSnoOneBusy = pow((1-pPSusers), nPSusers-1);
	printf("7c: The probability that all of the other specific other users are not busy is: %e\n", pPSnoOneBusy);

	//7d: probability that one specific user is transmitting and the remaining users are not transmitting is computed as
	double pPSOneBusy = pow(pPSusers, 1) * pow(pPSusersNotBusy, (nPSusers-1));
	printf("7d: The probablity that one specific user is transmitting and the remaining users are not transmitting is: %e\n", pPSOneBusy);

	//7e
	double pPSonlyOnePS = nPSusers * (pow(pPSusers, 1) * pow(pPSusersNotBusy, nPSusers-1));
	printf("7e: The probability that exactly one (anyone) of the nPSusersusers is busy is pPSuserstimes times the probability that a given specific user is transmitting and the remaining users are not transmitting is: %le\n", pPSonlyOnePS);

	//7f: probability that 10 specific users of nPSusersare transmitting and the others are idle is computed as
	double pPStenBusy = pow(pPSusers, 10) * pow(pPSusersNotBusy, nPSusers-10);
   printf("7f: The probability that 10 specific users of nPSusersare transmitting and the others are idle is: %lg\n", pPStenBusy);

	//7g: probability that any 10 users of nPSusersare transmitting and the others are idle is computed as
	double pPSanyTen = (factorial(nPSusers)/(factorial(10)*factorial(nPSusers-10))) * pow(pPSusers,10) * pow(pPSusersNotBusy, nPSusers-10);
	printf("The probability that any 10 users of nPSusers are transmitting and the others are idle is: %lg\n", pPSanyTen);

	//7h: the probability that more than 10 users of nPSusers are transmitting and the others are idle is computed as
	double pPStenMorebusy = 0.0;
	for(i = 11; i <= nPSusers; i++){
		pPStenMorebusy += factorial(nPSusers)/(factorial(i)*factorial(nPSusers-i)) * pow(pPSusers, i) * pow(pPSusersNotBusy, nPSusers-i);
	}
	printf("The probability that more than 10 users of nPSusers are transmitting and the others are idle is: %lg\n", pPStenMorebusy); 

return 0;



}
