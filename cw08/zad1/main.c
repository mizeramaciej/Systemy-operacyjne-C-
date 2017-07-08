#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

int threads_no;
char* fileName;
int records_no;
char* word;
int file;
pthread_t *threads;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int BUFFSIZE = 1024;
int START=1;
#define VERSION 3  // 1-a, 2-b, 3-c

void* run(void*);

int main(int argc, char* argv[]) {
  fileName = calloc(30, sizeof(char));
  word = calloc(25, sizeof(char));

  if(argc != 5) {
    printf("Example ./main <thread_no>  <file_name> <records_no> <word> \n");
    exit(-1);
  }

  threads_no = atoi(argv[1]);
  fileName = argv[2];
  records_no = atoi(argv[3]);
  word = argv[4];


  if((file = open(fileName, O_RDONLY)) == -1) {
    printf("Error open %d: %s\n", errno, strerror(errno));
    exit(-1);
  }

  threads = calloc(threads_no, sizeof(pthread_t));
  int i;
  for(i = 0; i < threads_no; i++){
    if(pthread_create(&threads[i], NULL, run, NULL) != 0) {
      printf("Error pthread_create %d: %s\n", errno, strerror(errno));
      exit(-1);
    }
  }
  START = 0;
  for(i = 0; i < threads_no; i++) {
    pthread_join(threads[i], NULL);
  }

  free(threads);
  close(file);
  pthread_mutex_destroy(&mutex);
  return 0;
}

void* run(void *nothing) {
  int i;
  switch(VERSION) {
	  case 1:
		 pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
		 pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
		 break;
	  case 2:
		 pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
		 pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
		 break;
	  case 3:
		 pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
		 break;
  }
	
  char** readingBuffer = calloc(records_no, sizeof(char*));
  char* record_no = calloc(2, sizeof(char));
  int CharsReaded_no;
  int keepGoing = 1;

  for(i = 0; i < records_no; i++) {
    readingBuffer[i] = calloc(1024, sizeof(char));
  }

  while(START);
  while(keepGoing) {
    pthread_mutex_lock(&mutex);

    for(i = 0; i < records_no; i++){
      if((CharsReaded_no = read(file, readingBuffer[i], BUFFSIZE)) == -1) {
		pthread_mutex_unlock(&mutex);
        printf("Error read %d: %s\n", errno, strerror(errno));
        exit(-1);
      }
    }

	 if(VERSION==1){
		  if(CharsReaded_no == 0) {
		  int j;
		  for(j = 0; j < threads_no; j++) {
			if(threads[j] != pthread_self()) {
			  pthread_cancel(threads[j]);
			}
		  }
		  pthread_mutex_unlock(&mutex);
		  break;
    	  }	 
	 }
	  
    pthread_mutex_unlock(&mutex);

	if(VERSION == 2){
		usleep(1);
		pthread_testcancel(); // set cancel point, after unlocking mutex
	} 
	  
    for(i = 0; i < records_no; i++) {
      if(strstr(readingBuffer[i], word) != NULL) {
        strncpy(record_no, readingBuffer[i], 2);
        printf("%ld: found %s in record_no %d.\n", pthread_self(),word, atoi(record_no));

        if(VERSION==2 ||VERSION==1){ 
			int j;
			for(j = 0; j < threads_no; j++) {
			  if(threads[j] != pthread_self()) {
				pthread_cancel(threads[j]);
			  }
			}
			keepGoing = 0;
			i = records_no;
		}
      }
    }
	if(VERSION==3) return NULL;
  }

  return NULL;
}