#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>

#define MAX_THREADS 4

__thread int thr_data = 4;
__thread int thr_bss;

void* thread_func(void* arg)
{
	int id = *((int*) arg);

	thr_data = id;
	thr_bss = id;

	printf("[%d] Hello Thread!!! arg = %d\n", getpid(), id);
	printf("[%d] tdata = %d, tbss = %d\n", getpid(), thr_data, thr_bss);
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
	sleep(2);

	printf("[%d] Sleep done, trying to join\n", getpid());
	/* wait until all threads have terminated */
	for(i=0; i<MAX_THREADS; i++)
		pthread_join(threads[i], NULL);	

	printf("[%d] Joined\n", getpid());

	return 0;
}
