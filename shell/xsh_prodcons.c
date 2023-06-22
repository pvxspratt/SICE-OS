#include <xinu.h>
#include <prodcons.h>
#include <shprototypes.h>

int n;                 // Definition for global variable 'n'
/* Now global variable n is accessible by all the processes i.e. consume and produce */

shellcmd xsh_prodcons(int nargs, char *args[]) {
  // Argument verifications and validations
	int count = 200;    // local varible to hold count
	//sid32 prod;
	//sid32 cons;
	cons = semcreate(1);
	prod = semcreate(0);
	
	prosfish = semcreate(0);

  // TODO: check args[1], if present assign value to count
	if(nargs == 2) {
		count = atoi(args[1]);
	} else if(nargs > 2) {
		printf("Syntax: run prodcons [counter]\n");
		signal(spawnrun);
		return(0);
	}
  // create the process producer and consumer and put them in ready queue.
  // Look at the definations of function create and resume in the system folder for reference.
  	resume(create(producer, 1024, 20, "producer", 1, count));
	resume(create(consumer, 1024, 20, "consumer", 1, count));
				
	wait(prosfish);
	signal(spawnrun);
	return 1;		
  // TODO: call the 'resume' and 'create' functions to start two processes that execute
  //       your 'producer' and 'consumer' code.
}
