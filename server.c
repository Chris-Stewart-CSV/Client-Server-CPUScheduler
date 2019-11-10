/*********************************** server.c *********************************** 
* Programmer:  Christopher Stewart
* 
* Course: CSCI 4354.02 
* 
* Date: October 22, 2019
* 
* Assignment: Program #2: Client /  Server Programming Using Common & Private FIFO
* 
* Environment: Ubuntu Linux using the gcc compiler
* 
* Files Included: server.c
* 
* Purpose:  Receive data (job_name, cpu-burst, and private_FIFO name) from clients 
* 	        and perform the cpu_burst calculation then return to appropriate client through
*	        private_FIFO
*
* Input: No server explicit input. Input is received from multiple clients (job_name, cpu_burst, 
*	   and private_FIFO name)
* 
* Preconditions: Server is launched before client to ensure proper connection through common 
*		     and private_FIFO 
*
* Output: Perform calculation from client(s) input and print job_name and completion_time to
*	      screen 
* 
* Postconditions: Operations on floats are calculated with a range of -3.4E+38 to +3.4E+38 
*		      with an accuracy of about 7 decimal digits 
* 
* Algorithm: 
*         Create Common_FIFO_to_Server
*         Open Common_FIFO_to_Server
*         Listen for connection from client
*         Once Client connected:
*         		Receive input from Client(s)
*         		Perform calculation based on Client(s) input
*         		Send calculation results to Client(s) through private_FIFO
*         Close Common_FIFO_to_Server
*         Unlink Common_FIFO_to_Server
*
* 					Estimates  	Actuals
*   Design:		    1 hour		1 hour 
*   Implementation:	1.5 hour	2 hour
*   Testing: 		1.5 hour	2.5 hour
**********************************************************************/ 

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct Value {
	char privateFIFO[14];
	char jobName[14];
	int cpuBurst;
};//end struct

int main (void) {
	
	int fda; // CommonFIFO to read from client
	int fdb; // PrivateFIFO to write to client
	int i = 0; // Used to iterate some loops
	int howBig;
	int completionTime;
	int timesList[3]; // Tracks completion times for each of the clients
	float averageCompletionTime;
	struct Value input;
	struct Value inputs[3];

	// Make common FIFO
	if((mkfifo("Common_FIFO",0666)<0 && errno != EEXIST)){
		perror("\nCannot create Common_FIFO!");
		exit(-1);
	}//endif
	
	// Open common FIFO
        if((fda = open("Common_FIFO", O_RDONLY))<0){
	        printf("\nCannot open FIFO to write!");
	}//endif
 	
	// Read number of client requests made
	while(i < 3){
		howBig = read(fda, &inputs[i], sizeof(input));
		if(howBig > 0){
			read(fda, &inputs[i], sizeof(input));
			i++;
		}//endif
	}//end while
	close(fda);
	unlink("Common_FIFO");

	for(i = 0; i < 3; i++){
		printf("\nServer - Job name is %s", inputs[i].jobName);
		completionTime += inputs[i].cpuBurst;
		printf("\nServer - Completion time is %d\n", completionTime);
		timesList[i] = completionTime;
		
		if((fdb=open(inputs[i].privateFIFO, O_WRONLY))<0){
			printf("\nCannot open FIFO for read");
		}//endif

		write(fdb, &completionTime, sizeof(completionTime)); 
		close(fdb); // Close private FIFO
	}//end for
	
	for(i = 0; i < 3; i++){
		averageCompletionTime += timesList[i];
	}//end for
	averageCompletionTime /= 3;
	printf("\nServer - average completion time is %f", averageCompletionTime);
	printf("\nAll done!\n");	

}//end main

