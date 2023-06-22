// for assignment 04

extern int arr_q[5];
extern int maxcons;
extern int head;
extern int tail;

//sid32 mutex, mutex1, mutex2, done, finished;
sid32 mutex2, mutex, done, finished;

void consumer_bb(int id, int count);
void producer_bb(int id, int count);
//void consumer_bb(int id, int count);
