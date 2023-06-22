#include <xinu.h>
#include <shprototypes.h>
#include <prodcons.h>
#include <future_prodcons.h> 
#include <future.h>
#include <futest.h>

typedef void (*cmd) (int, char*[]);

void sem_wrapper(cmd, int, char *[]);

sid32 finished;

shellcmd xsh_run(int nargs, char *args[]) {
	spawnrun = semcreate(0);

	finished = semcreate(0);
	
	if((nargs == 1) || (strncmp(args[1], "list", 4) == 0)) {
		printf("futest\n");
		printf("hello\n");
		printf("list\n");
		printf("prodcons\n");
		printf("prodcons_bb\n");
		return OK;
	}
	
	args++;
	nargs--;

//	if(strncmp(args[0], "my_function_1", 13) == 0) {
		/* create a process with the function as an entry point. */
//		resume(create((void *) my_function_1, 4096, 20, "my_func_1", 2, nargs - 1, &(args[1])));
//	}

	if(strncmp(args[0], "hello", 5) == 0) {
		/* creating process with the fucntion as an entry point. */
		resume(create((void *) xsh_hello, 1024, 20, "hello", 2, nargs, args));
		wait(spawnrun);
		return OK;
	}

	if(strncmp(args[0], "prodcons_bb", 11) == 0) {
		/* create process with the function as entry point */
		resume(create((void *) sem_wrapper, 4096, 20, "prodcons_bb", 3, xsh_prodcons_bb, nargs, args));
		wait(finished);
		return 0;
	}

	if(strncmp(args[0], "prodcons", 8) == 0) {
		/* creating process with the function as an entry point. */
		resume(create((void *) xsh_prodcons, 4096, 20, "prodcons", 2, nargs, args));
		wait(spawnrun);
		return OK;
	}

	if(strncmp(args[0], "futest", 6) == 0) {


		if(nargs <= 3) {
			printf("Syntax: run futest [g ...] [VALUE ...]\n");
			return 0;
		} else {
			resume(create((void *) xsh_futest, 4096, 20, "futest", 2, nargs, args));
			wait(spawnrun);
			return OK;
		}

		if(strncmp(args[1], "-f", 3) == 0) {
			if(nargs < 3) {
				printf("Syntax: run futest ([g ...] [VALUE ...]) | (-f NUMBER) | (--free)\n");
				return 1;
			}

			future_fib(nargs, args);
			return 0;
		}

		if(strncmp(args[1], "--free", 6) == 0) {
			if(nargs >= 3) {
				printf("Syntax: run futest ([g ...] [VALUE ...]) | (-f NUMBER) | (--free)\n");
				return 1;
			}

			future_fib(nargs, args);
			return 0;		
		}
	}

	if(strncmp(args[0], "fstest", 6) == 0) {
		resume(create((void *) xsh_fstest, 4096, 20, "fstest", 2, nargs, args));
		wait(spawnrun);
		return OK;
	}

}


void sem_wrapper(cmd command, int nargs, char* args[]) {
	command(nargs, args);
	signal(finished);
}
