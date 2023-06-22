#include <xinu.h>
#include <prodcons.h>
#include <prodcons_bb.h>

int n;
int maxcons;
int arr_q[5];
int head;

void producer(int count) {
  // TODO: implement the following:
  // - Iterates from 0 to count (count including)
  //   - setting the value of the global variable 'n' each time
  //   - print produced value (new value of 'n'), e.g.: "produced : 8"
	for(int i = 0; i <= count; i++) {
		wait(cons);
		n = i;

		printf("produced : %d\n", i); // n??
		signal(prod);
	}
}


void producer_bb(int id, int count) {
	for(int i = 0; i < count; i++) {
		//wait(mutex);
		wait(mutex);

		arr_q[head] = i;
		printf("name : producer_%d, write : %d\n", id, arr_q[head]);
		head++;

		if(head == 5) {
			head = 0;
		}

		signal(mutex);
//		signal(mutex2);

	}

	return OK;
}

