// Phoebe Spratt
// OS assignment 01 
// Jan12/23

/* xsh_hello.c - xsh_echo */

#include <xinu.h>
#include <stdio.h>
#include <prodcons.h>

shellcmd xsh_hello(int nargs, char *args[]) {
	if (nargs > 2) {
		fprintf(stderr, "%s: too many arguments\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
			args[0]);
		return 1;
	}

	
	printf("Hello %s, Welcome to the world of Xinu! \n", args[1]);
	signal(spawnrun);	

	return 0;
}
