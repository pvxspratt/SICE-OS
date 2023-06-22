#include <xinu.h>
#include <prodcons.h>
#include <prodcons_bb.h>

int n;
int maxcons;
int arr_q[5];
int tail;

void consumer(int count) {
  // TODO: implement the following:
  // - Iterates from 0 to count (count including)
  
  for(int i = 0; i <= count; i++) {
  //   - reading the value of the global variable 'n' each time
  //   - print consumed value (the value of 'n'), e.g. "consumed : 8"
      wait(prod);
      printf("consumed : %d\n", n);
      signal(cons);

      if(i == count) {
	      signal(prosfish);
      }
  }

}

void consumer_bb(int id, int count) {
	int32 i;
	for(i = 0; i < count; i++) {
		//wait(mutex);
		wait(mutex);

		printf("name : consumer_%d, read : %d\n", id, arr_q[tail]);

		tail++;

		maxcons--;

		if(tail == 5) {
			tail = 0;
		}
		//mutex
		signal(mutex);
	}

	if(maxcons = 0) {
		signal(done);
	}
	
	return OK;
}
