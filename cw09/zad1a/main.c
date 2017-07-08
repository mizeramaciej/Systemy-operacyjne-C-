// mutexy | wariant 2: faworyzujaca pisarzy

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>

#define tableSize 100
int iOption=0;
int wNo,rNo,readers=0,writers=0;
int writersInQueue=0;
int table[tableSize];
int *divTable;

pthread_t *readThreads;
pthread_t *writeThreads;
pthread_mutex_t mutex;
pthread_cond_t cond=PTHREAD_COND_INITIALIZER;

void* rFunc(void*);
void* wFunc(void*);
void prepareWritersAndReaders();
void randTable();
void exitError(const char*);
void clean();

int main(int argc, char** argv){
	printf("START\n");

	if(argc == 4 && (strcmp("-i",argv[3])==0) ){
		iOption=1;	
	}else if (argc !=3){
		printf("Example: ./main <writersNumber> <readersNumber> [-i]\n");	
		exit(1);
	}
	
	srand((unsigned int)getpid());
	
	wNo=atoi(argv[1]);
	rNo=atoi(argv[2]);
	
	randTable();
	prepareWritersAndReaders();
	
	int i;
	for(i=0;i<wNo;i++){
		if(pthread_join(writeThreads[i],NULL) !=0) exitError("write join");	
	}
	for(i=0;i<rNo;i++){
		if(pthread_join(readThreads[i],NULL) !=0) exitError("read join");	
	}
	
	
	printf("END\n");
	return 0;
}




void* rFunc(void* passdiv){
	int div=*(int*)passdiv;
	
	while(1){
		pthread_mutex_lock(&mutex);
		while(writersInQueue!=0){
			pthread_cond_wait(&cond,&mutex);
		}
		readers++;
		pthread_mutex_unlock(&mutex);
		
		
		int i,count=0;
		for(i=0;i<tableSize;i++){
			if(table[i] % div == 0){
				count++;
				if(iOption)
					printf("Reader %lu found number %d on position %d divisible by %d\n",pthread_self(),table[i],i,div);				
			}
		}
		printf("Reader %lu found %d numbers divisible by %d\n",pthread_self(),count,div);
		
		pthread_mutex_lock(&mutex);
		readers--;
		pthread_cond_broadcast(&cond);
		pthread_mutex_unlock(&mutex);
		
		sleep(1);
	}
	
	
	return NULL;
}

void* wFunc(void* nth){
	while(1){
		pthread_mutex_lock(&mutex);	
		writersInQueue++;
		while(readers || writers){
			pthread_cond_wait(&cond,&mutex);	
		}
		writers++;
		pthread_mutex_unlock(&mutex);
		
		int i,amount=((rand()%tableSize/10)+1);
		for(i=0;i<amount;i++){
			int k=rand()%tableSize;
			int random=rand()%999+1;
			table[k] = random;
			if(iOption) printf("Writer changed value on position %d to %d\n",k,random);
		}
		printf("Writer %lu changed table %d times\n",pthread_self(),amount);
		
		pthread_mutex_lock(&mutex);
		writers--;
		writersInQueue--;
		pthread_cond_broadcast(&cond);
		pthread_mutex_unlock(&mutex);
		
		sleep(1);
	}
	
	return NULL;
}

void prepareWritersAndReaders(){
	writeThreads=malloc(wNo* sizeof(pthread_t*));
	readThreads=malloc(rNo* sizeof(pthread_t*));
	divTable=malloc(rNo* sizeof(int));

	int i;
	for(i=0;i<rNo;i++){
		divTable[i]=(int)rand()% 100 + 1;	
	}
	
	for(i=0;i<wNo;i++){
		if(pthread_create(&writeThreads[i], NULL, wFunc,NULL) !=0 )
			exitError("writeThreads create");
	}
	for(i=0;i<rNo;i++){
		if(pthread_create(&readThreads[i], NULL, rFunc, divTable +i) !=0)
			exitError("readThreads create");
	}
}


void randTable(){
	int i;
	printf("Table: ");
	for(i=0;i<tableSize;i++){
		table[i]=rand()%999 + 1;
		printf("%d ",table[i]);
	}
	printf("\n");
}

void clean(){
	free(writeThreads);
	free(readThreads);
	free(divTable);
}

void exitError(const char* err){
  	printf("%s Errno %d, %s\n", err, errno, strerror(errno));
  	exit(1);
}