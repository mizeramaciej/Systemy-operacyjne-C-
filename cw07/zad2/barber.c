#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include "helpers.h"
#include "fifo.h"
#include "hairdresser.h"

sem_t* BARBER;
sem_t* FIFO;
sem_t* CHECKER;
sem_t* SLOWER;
Fifo* fifo = NULL;

void validatechairNumber(int k){
  if(k < 2 || k > 100) errorExit("No. of chairs should be between 2 and 100");
}
void intHandler(int signo){
  exit(2);
}

void cleanAtExit(void);
void makeFifo(int chairNumber);
void makeSemafors();
void startWork();
void cut(pid_t pid);
pid_t takeChair();


int main(int argc, char** argv){
  if(argc != 2) {
  	printf("Example ./barber.out <chairNumber>\n");
  	exit(1);
  }
  if(atexit(cleanAtExit) == -1) errorExit("Barber atexit error!");
  if(signal(SIGINT, intHandler) == SIG_ERR) errorExit("Barber signal error!");

  makeFifo(atoi(argv[1]));
  makeSemafors();
  printf("Barber is starting work!\n");	
  startWork();

  return 0;
}

void startWork(){
  while(1){

    if(sem_wait(BARBER) == -1) errorExit("Barber semwait error!"); // czekaj na obudzenie
	if(sem_post(BARBER) == -1) errorExit("Barber senpost error!");
    if(sem_post(SLOWER) == -1) errorExit("Barber sempost error!");
	  
    pid_t toCut = takeChair();
    cut(toCut);

    while(1){
      if(sem_wait(FIFO) == -1) errorExit("Barber sempost error!"); //+1
      toCut = popFifo(fifo); // zajmij FIFO i pobierz pierwszego z kolejki

      if(toCut != -1){ // jesli istnial, to zwolnij kolejke, ostrzyz i kontynuuj
        if(sem_post(FIFO) == -1) errorExit("Barber error!");
        cut(toCut);
      }else{ // jesli kolejka pusta, to ustaw, ze spisz, zwolnij kolejke i spij dalej (wyjdz z petli)
        long timePrint = getTime();
        printf("Time: %ld, Barber going to sleep...\n", timePrint);  fflush(stdout);
        
        if(sem_wait(BARBER) == -1) errorExit("Barber error!");
        if(sem_post(FIFO) == -1) errorExit("Barber error!");
        break;
      }
    }
  }
}

pid_t takeChair(){

  if(sem_wait(FIFO) == -1) errorExit("Barber error!");
  pid_t toCut = fifo->chair;
  if(sem_post(FIFO) == -1) errorExit("Barber error!");

  return toCut;
}

void cut(pid_t pid){
  long timePrint = getTime();
  printf("Time: %ld, Barber preparing to cut %d\n", timePrint, pid); fflush(stdout);

  kill(pid, SIGRTMIN);

  timePrint = getTime();
  printf("Time: %ld, Barber finished cutting %d\n", timePrint, pid); fflush(stdout);
}

void makeFifo(int chairNumber){
  validatechairNumber(chairNumber);
	
  int shmID = shm_open(shmPath, O_CREAT | O_EXCL | O_RDWR, 0666);
  if(shmID == -1) errorExit("Barber shmopen create error!");

  if(ftruncate(shmID, sizeof(Fifo)) == -1) errorExit("Barber ftruncate error!");

  void* tmp = mmap(NULL, sizeof(Fifo), PROT_READ | PROT_WRITE, MAP_SHARED, shmID, 0);
  if(tmp == (void*)(-1)) errorExit("Barber mmap error!");
  fifo = (Fifo*) tmp;

  fifoInit(fifo, chairNumber);
}

void makeSemafors(){

	BARBER=sem_open(barberPath,O_CREAT|O_EXCL|O_RDWR,0666,0);
	if(BARBER==SEM_FAILED) errorExit("Barber sem_open error!");
	
	SLOWER = sem_open(slowerPath, O_CREAT | O_EXCL | O_RDWR, 0666, 0);
	if(SLOWER==SEM_FAILED) errorExit("Barber sem_opens error!");
	
	FIFO=sem_open(fifoPath,O_CREAT|O_EXCL|O_RDWR,0666,1);
	if(FIFO==SEM_FAILED) errorExit("Barber sem_open error!");
	
	CHECKER=sem_open(checkerPath,O_CREAT|O_EXCL|O_RDWR,0666,1);
	if(CHECKER==SEM_FAILED) errorExit("Barber sem_open error!");
}

void cleanAtExit(void){
  if(munmap(fifo,sizeof(fifo)) == -1) printf("Barber munmap error!\n");
  if(shm_unlink(shmPath) == -1) printf("Barber shm_unlink error!\n");
	
  if(sem_close(BARBER) == -1) printf("Barber sem_close error!\n");
  if(sem_unlink(barberPath) ==-1) printf("Barber sem_unlink error!\n");
	 
  if(sem_close(SLOWER) == -1) printf("Barber sem_close error!\n");
  if(sem_unlink(slowerPath) ==-1) printf("Barber sem_unlink error!\n");
	 
  if(sem_close(FIFO) == -1) printf("Barber sem_close error!\n");
  if(sem_unlink(fifoPath) ==-1) printf("Barber sem_unlink error!\n");
	 
  if(sem_close(CHECKER) == -1) printf("Barber sem_close error!\n");
  if(sem_unlink(checkerPath) ==-1) printf("Barber sem_unlink error!\n");
}