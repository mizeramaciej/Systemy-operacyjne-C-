#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

//_Atomic int up=1;
sig_atomic_t up=1;

void doSIGINT(){
	printf("\nOdebrano sygnał SIGINT!\n");
	exit(1);
}

void doSIGTSTP(){
	//printf("\n!SIGTSTP!\n\n\n");
	if(up==1){
		up=-1;
	}else{
		up=1;
	}
}

int main(int argc, char* argv[]){

	signal(SIGINT,doSIGINT);

	//signal(SIGTSTP,doSIGTSTP);
	//2sposob 
	struct sigaction act;
	
	act.sa_handler=doSIGTSTP;	
	sigemptyset(&act.sa_mask);
	act.sa_flags=0;
	
	sigaction(SIGTSTP,&act,NULL);
	//
	
	// od 65 do 90
	int symbol = 65;

	while(1){
	
		fprintf(stderr,"%c ",(char)symbol);
		if(up==1){
			if(symbol>=90){
				symbol=64;
			}
		}
		if(up==-1){
			if(symbol<=65){
				symbol=91;
			}
		}
		symbol+=up;
		usleep(100000);
	}
	return 0;
}