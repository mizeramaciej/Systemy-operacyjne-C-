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
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include "helpers.h"
#include "fifo.h"
#include "hairdresser.h"

key_t fifoKey;
int shmID = -1;
Fifo* fifo = NULL;
int SID = -1;
volatile int cutsCounter = 0;
sigset_t fullMask;

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
    struct sembuf sbuf;
    sbuf.sem_num = CHECK;
    sbuf.sem_op = -1;
    sbuf.sem_flg = 0;
    if(semop(SID, &sbuf, 1) == -1) errorExit("Client semop error!");

    sbuf.sem_num = FIFO;
    if(semop(SID, &sbuf, 1) == -1) errorExit("Client semop error!");

    int res = takePlace();

    sbuf.sem_op = 1;
    if(semop(SID, &sbuf, 1) == -1) errorExit("Client semop error!");

    sbuf.sem_num = CHECK;
    if(semop(SID, &sbuf, 1) == -1) errorExit("Client semop error!");

    if(res != -1){
      sigsuspend(&fullMask);
      long timePrint = getTime();
      printf("Time: %ld, Client %d just got cut!\n", timePrint, getpid()); fflush(stdout);
    }
  }
}

int takePlace(){
  int barberStat = semctl(SID, 0, GETVAL);
  if(barberStat == -1) errorExit("Client semctl error!");

  pid_t myPID = getpid();

  if(barberStat == 0){
    struct sembuf sbuf;
    sbuf.sem_num = BARBER;
    sbuf.sem_op = 1;
    sbuf.sem_flg = 0;

    if(semop(SID, &sbuf, 1) == -1) errorExit("Client semop error!");
    long timePrint = getTime();
    printf("Time: %ld, Client %d has awakened barber!\n", timePrint, myPID); fflush(stdout);
    if(semop(SID, &sbuf, 1) == -1) errorExit("Client semop error!");

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
  char* path = getenv(env);
  if(path == NULL) errorExit("Breeder getenv error!");

  fifoKey = ftok(path, PROJECT_ID);
  if(fifoKey == -1) errorExit("Breeder ftok error!");

  shmID = shmget(fifoKey, 0, 0);
  if(shmID == -1) errorExit("Breeder shmget error!");

  void* tmp = shmat(shmID, NULL, 0);
  if(tmp == (void*)(-1)) errorExit("Breeder shmat error!");
  fifo = (Fifo*) tmp;
}

void makeSemafors(){
  SID = semget(fifoKey, 0, 0);
  if(SID == -1) errorExit("Breeder semget error!");
}

void makeMask(){
  if(sigfillset(&fullMask) == -1) errorExit("Breeder sigfillset error!");
  if(sigdelset(&fullMask, SIGRTMIN) == -1) errorExit("Breeder sigdelset error!");
  if(sigdelset(&fullMask, SIGINT) == -1) errorExit("Breeder sigdelset error!");
}

void cleanAtExit(void){
  if(shmdt(fifo) == -1) printf("Breeder Error detaching fifo sm!\n");
}