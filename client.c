/*********************************** client.c *********************************** 
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
* Files Included: client.c
* 
* Purpose:  Send data from multiple clients to server, perform some calculation, and return
* 	        calculation result to appropriate client through private FIFO
*
* Input: A given job_name, cpu_burst, and private_FIFO name
* 
* Preconditions: Server is launched before client to ensure proper connection on common_FIFO
* 
* Output: Calculation sent from server to client through private_FIFO and displayed on screen 
* 
* Postconditions: Server and Client close FIFO’s
* 
* Algorithm: 
*	Open Common_FIFO_to_Server
*	Open Private_FIFO_to_Client
*	Perform check whether the cpu_burst is valid (0 < cpu_burst <100)
*		If cpu_burst invalid:
*			Prompt user for valid input
*		If cpu_burst valid:
*			Send data to server through Common_FIFO
*	Wait for response
*	Receive response from server through Private_FIFO
*	Display results and completion_time from server
*	Close FIFO_to_Server
*	Close FIFO_to_Client
*
* 					Estimates  	Actuals
*   Design:			1 hour		1.5 hour 
*   Implementation:	1 hour		1.5 hour
*   Testing: 		1 hour		2 hour
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

int main (void) {
	
	struct Value {
	  char privateFIFO[14];
	  char jobName[14];
	  int cpuBurst;
	} input;

	int fda;  // CommonFIFO to write to server
	int fdb;  // PrivateFIFO to read from server 
	bool start = true;
	int clientID; // For privateFIFO
	int completionTime;	// Time to complete task - calculated by server

	printf("Enter a job name(no spaces): ");
	scanf("%s", &input.jobName);

	// Get the cpuBurst input from the user
	while(start){
		printf("Enter the cpuBurst: ");
		scanf("%d", &input.cpuBurst);
    
		if (input.cpuBurst > 0 && input.cpuBurst < 100) {
			start = false;
		} else {
			printf("Client - The cpuBurst entered is not >0 and <100.\n");
		}
	}//end while

	// Set PrivateFIFO name
	clientID = getpid();
	sprintf(input.privateFIFO, "FIFO_%d", clientID);

	// Open CommonFIFO to write to server
	if((fda=open("Common_FIFO", O_WRONLY))<0) {
		printf("Cannot open FIFO to write");
	}

	write(fda, &input, sizeof(input)); // Send the struct to the server 
	close(fda); // Close FIFO_to_server

	// Make private FIFO
	if ((mkfifo(input.privateFIFO, 0666)<0 && errno != EEXIST)) {
		perror("Cannot create Private_FIFO_to_server");
		exit(-1);
	}

	// Open private FIFO to read
	if((fdb=open(input.privateFIFO, O_RDONLY))<0) {
		printf("Cannot open FIFO to read");
	}
	
	read(fdb, &completionTime, sizeof(completionTime)); 
	printf("Client - the completion time is %d", completionTime);
	printf("\nAll done!\n");

	close(fdb); // Close private FIFO
	unlink(input.privateFIFO); // Unlink private FIFO
	
}//end main
