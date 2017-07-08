#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

#define SIGNAL SIGUSR1
#define VERSION 1
// 1 wysłanie sygnału do procesu, gdy żaden wątek nie ma zamaskowanego tego sygnału
// 2 wysłanie sygnału do procesu, gdy główny wątek programu ma zamaskowany ten sygnał, a wszystkie pozostałe wątki nie,
// 3 wysłanie sygnału do procesu, gdy wszystkie wątki mają zainstalowaną niestandardową procedurę obsługi przerwania, która wypisuje informację o nadejściu tego sygnału oraz PID i TID danego wątku
// 4 wysłanie sygnału do wątku z zamaskowanym tym sygnałem
// 5 wysłanie sygnału do wątku, w którym zmieniona jest procedura obsługi sygnału, jak przedstawiono w punkcie 3

#define errorExit(X) if(X) {printf("%s\n", strerror(errno));exit(-1);}


void handler(int signo) {
	printf("PID = %d TID = %d\n", getpid(), (int)pthread_self());
}

void * run(void * args) {
	if(VERSION == 3 || VERSION == 5) signal(SIGNAL, handler);
	if(VERSION == 4) {
		sigset_t set;
		sigemptyset(&set);
		sigaddset(&set, SIGNAL);
		errorExit(sigprocmask(SIG_SETMASK, &set, NULL) < 0)
	    printf("sigprocmask %d run\n", SIGNAL);
	}
	while (1) {}
	return NULL;
}

int main(int argc, char ** argv) {
	if(VERSION == 2) {
		sigset_t set;
		sigemptyset(&set);
		sigaddset(&set, SIGNAL);
		errorExit(sigprocmask(SIG_SETMASK, &set, NULL) < 0) 
		printf("sigprocmask %d main\n", SIGNAL);
	}

	pthread_t thread;
	errorExit(pthread_create(&thread, NULL, &run, (void*) NULL) < 0)
	printf("pthread_create\n");

	sleep(1);

	if(VERSION == 4 || VERSION == 5) {
	errorExit(pthread_kill(thread, SIGNAL) < 0) 
	printf("Send signal to the run.\n");
	} else {
		if(VERSION == 3) 
			signal(SIGNAL, handler);
		errorExit(raise(SIGNAL) < 0)
	    printf("Send signal to the main\n");
	}

	errorExit(pthread_join(thread, NULL) < 0)
    printf("end of program\n");

	return 0;
}