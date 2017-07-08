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
#include "helpers.h"
#include "fifo.h"
#include "hairdresser.h"

key_t fifoKey;
int shmID = -1;
Fifo* fifo = NULL;
int SID = -1;

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
pid_t takeChair(struct sembuf*);



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
    struct sembuf sbuf;
    sbuf.sem_num = BARBER;
    sbuf.sem_op = -1;
    sbuf.sem_flg = 0;

    if(semop(SID, &sbuf, 1) == -1) errorExit("Barber error!"); // czekaj na obudzenie

    pid_t toCut = takeChair(&sbuf);
    cut(toCut);

    while(1){
      sbuf.sem_num = FIFO; // dla czytelnosci
      sbuf.sem_op = -1;
      if(semop(SID, &sbuf, 1) == -1) errorExit("Barber error!");
      toCut = popFifo(fifo); // zajmij FIFO i pobierz pierwszego z kolejki

      if(toCut != -1){ // jesli istnial, to zwolnij kolejke, ostrzyz i kontynuuj
        sbuf.sem_op = 1;
        if(semop(SID, &sbuf, 1) == -1) errorExit("Barber error!");
        cut(toCut);
      }else{ // jesli kolejka pusta, to ustaw, ze spisz, zwolnij kolejke i spij dalej (wyjdz z petli)
        long timePrint = getTime();
        printf("Time: %ld, Barber going to sleep...\n", timePrint);  fflush(stdout);
        sbuf.sem_num = BARBER;
        sbuf.sem_op = -1;
        if(semop(SID, &sbuf, 1) == -1) errorExit("Barber error!");

        sbuf.sem_num = FIFO;
        sbuf.sem_op = 1;
        if(semop(SID, &sbuf, 1) == -1) errorExit("Barber error!");
        break;
      }
    }
  }
}

pid_t takeChair(struct sembuf* sbuf){
  sbuf->sem_num = FIFO;
  sbuf->sem_op = -1;
  if(semop(SID, sbuf, 1) == -1) errorExit("Barber error!");

  pid_t toCut = fifo->chair;

  sbuf->sem_op = 1;
  if(semop(SID, sbuf, 1) == -1) errorExit("Barber error!");

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

  char* path = getenv(env);
  if(path == NULL) errorExit("Getting getenv error!");

  fifoKey = ftok(path, PROJECT_ID);
  if(fifoKey == -1) errorExit("Barber ftok error!");

  shmID = shmget(fifoKey, sizeof(Fifo), IPC_CREAT | IPC_EXCL | 0666);
  if(shmID == -1) errorExit("Barber shmget error!");

  void* tmp = shmat(shmID, NULL, 0);
  if(tmp == (void*)(-1)) errorExit("Barber shmat error!");
  fifo = (Fifo*) tmp;

  fifoInit(fifo, chairNumber);
}

void makeSemafors(){
  SID = semget(fifoKey, 4, IPC_CREAT | IPC_EXCL | 0666);
  if(SID == -1) errorExit("Barber semget error!");
  int i;
  for(i=1; i<3; i++){
    if(semctl(SID, i, SETVAL, 1) == -1) errorExit("Barber semctl error!");
  }
  if(semctl(SID, 0, SETVAL, 0) == -1) errorExit("Barber semctl error!");
}

void cleanAtExit(void){
  if(shmdt(fifo) == -1) printf("Barber shmdt error!\n");

  if(shmctl(shmID, IPC_RMID, NULL) == -1) printf("Barber shmctl error\n");

  if(semctl(SID, 0, IPC_RMID) == -1) printf("Barber semctl error!\n");

}