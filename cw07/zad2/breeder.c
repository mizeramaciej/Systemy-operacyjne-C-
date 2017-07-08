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
#include <sys/wait.h>
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
volatile int cutsCounter = 0;
sigset_t fullMask;
Fifo* fifo = NULL;

int validateclientsNumber(int k){
  if(k < 1 || k > 500){
    errorExit("Wrong number of Clients!");
    return -1;
  }
  else return k;
}
int validatecutsNumber(int k){
  if(k < 1 || k > 15){
    errorExit("Wrong number of Cuts!");
    return -1;
  }
  else return k;
}
void intHandler(int signo){
  exit(2);
}

void makeFifo();
void makeSemafors();
void makeMask();
void cleanAtExit(void);
int takePlace();
void getCut(int cutsNumber);

void rtminHandler(int signo){
  cutsCounter++;
}

int main(int argc, char** argv){
  if(argc != 3) {
  	printf("Example ./breeder.out <clientsNumber> <cutsNumber>\n");
  	exit(1);
  }
  if(atexit(cleanAtExit) == -1) errorExit("Breeder atexit error!");
  if(signal(SIGINT, intHandler) == SIG_ERR) errorExit("Breeder signal error!");
  if(signal(SIGRTMIN, rtminHandler) == SIG_ERR) errorExit("Breeder signal error!");

  int clientsNumber = validateclientsNumber(atoi(argv[1]));
  int cutsNumber = validatecutsNumber(atoi(argv[2]));

  makeFifo();
  makeSemafors();
  makeMask();

  sigset_t mask;
  if(sigemptyset(&mask) == -1) errorExit("Breeder emptyset error!");
  if(sigaddset(&mask, SIGRTMIN) == -1) errorExit("Breeder sigaddset error!");
  if(sigprocmask(SIG_BLOCK, &mask, NULL) == -1) errorExit("Breeder sigprocmask error!");
	
  int i;
  for(i=0; i<clientsNumber; i++){
    pid_t id = fork();
    if(id == -1) errorExit("Fork error!");
    if(id == 0){
      getCut(cutsNumber);
      return 0;
    }
  }

  
  while(1){
    wait(NULL);  // czekaj na dzieci
    if (errno == ECHILD) break;
  }
	printf("All clients has been bred!\n");
  return 0;
}

void getCut(int cutsNumber){
  while(cutsCounter < cutsNumber){
   
    if(sem_wait(CHECKER) == -1) errorExit("Client sem_wait error!");
    if(sem_wait(FIFO) == -1) errorExit("Client sem_wait error!");

    int res = takePlace();
    if(sem_post(FIFO) == -1) errorExit("Client sem_post error!");
	  
    if(sem_post(CHECKER) == -1) errorExit("Client sem_post error!");

    if(res != -1){
      sigsuspend(&fullMask);
      long timePrint = getTime();
      printf("Time: %ld, Client %d just got cut!\n", timePrint, getpid()); fflush(stdout);
    }
  }
}

int takePlace(){
  int barberStat;
  if(sem_getvalue(BARBER,&barberStat) == -1) errorExit("Client sem_getvalue error!");

  pid_t myPID = getpid();

  if(barberStat == 0){

    if(sem_post(BARBER) == -1) errorExit("Client sem_post error!"); //budzenie barber
    long timePrint = getTime();
    printf("Time: %ld, Client %d has awakened barber!\n", timePrint, myPID); fflush(stdout);
    if(sem_wait(SLOWER) == -1) errorExit("Client sem_wait error!");//czekanie na ustawnie barbera na 1

    fifo->chair = myPID;

    return 1;
  }else{
    int res =  pushFifo(fifo, myPID);
    if(res == -1){
      long timePrint = getTime();
      printf("Time: %ld, Client %d couldnt find free place!\n", timePrint, myPID); fflush(stdout);
      return -1;
    }else{
      long timePrint = getTime();
      printf("Time: %ld, Client %d took place in the queue!\n", timePrint, myPID); fflush(stdout);
      return 0;
    }
  }
}

void makeFifo(){
  int shmID = shm_open(shmPath,O_RDWR, 0666);
  if(shmID == -1) errorExit("Barber shmopen error!");

  void* tmp = mmap(NULL, sizeof(Fifo), PROT_READ | PROT_WRITE, MAP_SHARED, shmID, 0);
  if(tmp == (void*)(-1)) errorExit("Barber mmap error!");
  fifo = (Fifo*) tmp;
}

void makeSemafors(){
  SLOWER=sem_open(slowerPath,O_RDWR);
  if(SLOWER==SEM_FAILED) errorExit("Barber sem_open error!");
	
  FIFO=sem_open(fifoPath,O_RDWR);
  if(FIFO==SEM_FAILED) errorExit("Barber sem_open error!");
	
  BARBER=sem_open(barberPath,O_RDWR);
  if(BARBER==SEM_FAILED) errorExit("Barber sem_open error!");
	
  CHECKER=sem_open(checkerPath,O_RDWR);
  if(CHECKER==SEM_FAILED) errorExit("Barber sem_open error!");
}

void makeMask(){
  if(sigfillset(&fullMask) == -1) errorExit("Breeder sigfillset error!");
  if(sigdelset(&fullMask, SIGRTMIN) == -1) errorExit("Breeder sigdelset error!");
  if(sigdelset(&fullMask, SIGINT) == -1) errorExit("Breeder sigdelset error!");
}

void cleanAtExit(void){
  if(munmap(fifo,sizeof(fifo)) == -1) printf("Barber munmap error!");
  if(sem_close(BARBER) == -1) printf("Barber sem_close error!");
  if(sem_close(SLOWER) == -1) printf("Barber sem_close error!");
  if(sem_close(FIFO) == -1) printf("Barber sem_close error!");
  if(sem_close(CHECKER) == -1) printf("Barber sem_close error!");
}