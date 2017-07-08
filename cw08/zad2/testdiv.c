#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#define errorExit(X) if(X) {printf("%s\n", strerror(errno));exit(-1);}

void * run(void * args) {
	while (1);
	return NULL;
}

void * rundiv(void * args) {
    printf ("divbyzero = %d\n", 1/0);
    return NULL;
}

int main(int argc, char ** argv) {
	pthread_t thread;
	errorExit(pthread_create(&thread, NULL, &run, (void*) NULL) < 0)
	printf("Thread 1\n");

	pthread_t thread_bug;
	errorExit(pthread_create(&thread_bug, NULL, &rundiv, (void*) NULL) < 0)
	printf("Thread 2\n");

	usleep(1000);

	errorExit(pthread_join(thread, NULL) < 0)
	printf("Join 1\n");
    errorExit(pthread_join(thread_bug, NULL) < 0)
    printf("Join 4\n");

	return 0;
}