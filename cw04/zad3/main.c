#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

int L;
int Type;
int CPid=0;
int PPid;
sig_atomic_t PScounter=0;
sig_atomic_t CRcounter=0;
sig_atomic_t PRcounter=0;
//union sigval val;

void quit(){

	if(CPid!=0)
		kill(CPid,SIGKILL);	
	
	printf(" SIGINT -> EXIT\n");
	exit(0);
}

void handler(int signum,siginfo_t * info,void* nothing){//ucontext_t* nothing){

	
	if( signum == SIGUSR1 || signum == SIGRTMIN ){

		if(info->si_pid==PPid){
			CRcounter++;
			printf("Child received SIG1 from parent | num %d |\n",CRcounter);
			if(Type==3){
				kill(PPid,SIGRTMIN);				
			}else{
				kill(PPid,SIGUSR1);	
			}
		}else{
			PRcounter++;
			printf("Parent received SIG1 from child | num %d |\n",PRcounter);
		}
	}else if( signum == SIGUSR2 || signum == SIGRTMIN+1){
		printf("Child received SIG2 from parent\n");
		exit(0);
	}	

}


int main(int argc, char* argv[]){

	if(argc !=3 ){
		printf("2 arguments are needed\n");
		return 1;
	}
	
	int i;
	L=atoi(argv[1]);
	Type=atoi(argv[2]);
	
	printf("Start L=%d ,Type=%d\n",L,Type);
	
	signal(SIGINT,quit);
	
	struct sigaction act;
	act.sa_sigaction = handler; //zamiast sa_handler, bo w sa_flags=sa_siginfo
	sigemptyset(&act.sa_mask);
	act.sa_flags=SA_SIGINFO;//0; //dodatkowe informacje przekazywane w struct siginfo
	
	if(Type==3){
		sigaction(SIGRTMIN,&act,NULL);	
		sigaction(SIGRTMIN+1,&act,NULL);
	}else{
		sigaction(SIGUSR1,&act,NULL);
		sigaction(SIGUSR2,&act,NULL);
	}

	PPid=getpid();	
	printf("Parent PID %d\n",PPid);
	int pid=fork();
	if(pid==0){
		
		while(1);
		
	}else if (pid>0){
		
		union sigval val;
		//val.sival_int=0;
		printf("Child PID %d\n",pid);
		
		for(i=0;i<L;i++){
			if(Type==3){
				kill(pid,SIGRTMIN);
			}else
			if(Type==2){
				sigqueue(pid,SIGUSR1,val);			
			}else
			if(Type==1){
				kill(pid,SIGUSR1);	
			}
			PScounter++;
			printf("Parent sent SIG1 | PScounter= %d |\n",PScounter);
			sleep(1);
		}

		if(Type==3){
			kill(pid,SIGRTMIN+1);
		}else
		if(Type==2){
			sigqueue(pid,SIGUSR2,val);			
		}else
		if(Type==1){
			kill(pid,SIGUSR2);	
		}
		printf("Parent sent SIG2 to child\n");
		wait(NULL);
		printf("Parent sent %d SIG1, recieved %d SIG1\n",PScounter,PRcounter);
	}


	printf("end\n");
	return 0;
}







