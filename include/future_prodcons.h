# include <future.h>

syscall future_prod(future_t* fut, int value);
syscall future_cons(future_t* fut);

void future_prodcons(int nargs, char *args[]);

extern int val;
extern sid32 print_sem;
