#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>

#define MAX_THREADS 4

void* thread_func(void* arg)
{
	int id = *((int*) arg);

	printf("[%d] Hello Thread!!! arg = %d\n", getpid(), id);
	printf("[%d] Going to sleep\n", getpid());
	sleep(1);
	printf("[%d] Sleep done, exiting\n", getpid());
	return 0;
}

int main(int argc, char** argv)
{
	pthread_t threads[MAX_THREADS];
	int i, ret, param[MAX_THREADS];

	printf("[%d] Main thread starts ...\n", getpid());

	for(i=0; i<MAX_THREADS; i++) {
		param[i] = i;
		ret = pthread_create(threads+i, NULL, thread_func, param+i);
		if (ret) {
			printf("[%d] Thread creation failed! error =  %d\n", getpid(), ret);
			return ret;
		} else printf("[%d] Created thread %d\n", getpid(), i);
	}


	printf("[%d] Going to sleep\n", getpid());
	/* Pierre: if we sleep 2 seconds here, we leave enough time for the other
	 * threads to exit so the pthread_join on the next lines will need no
	 * synchronization, i.e. no call to futex. Comment the sleep(2) to 
	 * trigger the calls to futex. */
	sleep(2);

	printf("[%d] Sleep done, trying to join\n", getpid());
	/* wait until all threads have terminated */
	for(i=0; i<MAX_THREADS; i++)
		pthread_join(threads[i], NULL);	

	printf("[%d] Joined\n", getpid());

	return 0;
}
