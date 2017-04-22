#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

int req=0;
int *CPids;
int *CReq;
int permAfter=0;
int N;

void quit(){
	int i;
	for(i=0;i<N;i++){
		kill(SIGKILL,CPids[i]);	
	}
	printf(" SIGINT -> EXIT\n");
	exit(0);
}

void handler(int signum,siginfo_t * info,void* nothing){//ucontext_t* nothing){

	if(signum==SIGUSR1){
		
		printf("Request SIGUSR1 from %d | %d |\n",info->si_pid,info->si_value.sival_int);
		if(permAfter==1){
			kill(info->si_pid,SIGUSR2);	
		}else{
			CReq[info->si_value.sival_int]=1;
		}
		req++;
	}else if(signum==SIGUSR2){
		
		printf("PID %d got permission SIGUSR2\n",getpid());
		
	}else{
		printf("Recived real time singal no. %d from PID %d\n",signum,info->si_pid);	
	}
}

int main(int argc, char* argv[]){

	if(argc !=3 ){
		printf("2 arguments are needed\n");
		return 1;
	}
	
	
	int i;
	N=atoi(argv[1]);
	int K=atoi(argv[2]);
	
	srand((unsigned int) N);
	
	printf("Start N=%d ,K=%d\n",N,K);
	
	CPids=malloc(N*sizeof(int));
	CReq=calloc(N,sizeof(int));
	
	signal(SIGINT,quit);
	
	struct sigaction act;
	act.sa_sigaction = handler; //zamiast sa_handler, bo w sa_flags=sa_siginfo
	sigemptyset(&act.sa_mask);
	act.sa_flags=SA_SIGINFO;//0; //dodatkowe informacje przekazywane w struct siginfo
	
	for(i=SIGRTMIN;i<SIGRTMAX;i++){
		sigaction(i,&act,NULL);	
	}
	
	sigaction(SIGUSR1,&act,NULL);
	sigaction(SIGUSR2,&act,NULL);
	
	int parentPid=getpid();
	
	for(i=0;i<N;i++){
		int pid=fork();
		if(pid==0){
			pid=getpid();
			printf("PID %d\n",pid);
			
			srand((unsigned int)pid);
			
			sleep(2+rand()%3);
			
			union sigval addOnInfo;
			addOnInfo.sival_int=i;
			sigqueue(parentPid,SIGUSR1,addOnInfo);
			
			int t=sleep(2+rand()%4);
			if(t==0){
				printf("PID %d hasn't recieved permission signal\n",pid);
				exit(0);
			}
			
			int randomSig=rand()%(SIGRTMAX-SIGRTMIN)+SIGRTMIN;
			kill(parentPid,randomSig);

			exit(t);
		}else{
			CPids[i]=pid;
			
		}
		
	}
	
	while(req<K);
	
	
    printf("\nGot K=%d requests\n", req);
	
	for(i=0;i<N;i++){
		if(CReq[i]==1)
				kill(CPids[i],SIGUSR2);
	}
	
	permAfter=1;
	
	for(i=0;i<N;i++){
		int status;
		waitpid(CPids[i],&status,0);
		status= WEXITSTATUS(status);
		printf("PID %d exited with %d\n",CPids[i],status);
	}
	printf("end\n");
	return 0;
}







