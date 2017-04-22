#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <stdint.h>
#include <dirent.h>
#include <limits.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>

void addVariable(char *line){
	char *varname=strtok(line+1," \t\n\r"); //+1 , bo pierwszy znak to #, nie potrzebujemy go
	char *value=strtok(NULL,"\t\n\r"); // \t tab \n newline \r carriage return
	
	if(value==NULL){
		if(unsetenv(varname)!=0){
			printf("unsetenv error\n");
			return;
		}
	}else
	if(setenv(varname,value,1)!=0){ // '1' zadpisze wartosc
		printf("setenv error\n");
		return;
	}
		
}

void execCommand(char *commandLine,int lineNumber,rlim_t cpuLimit, rlim_t memLimit){
	
	short int maxArgs=10;
	int index=1;
	char* current=strtok(commandLine," \t\n\r");
	char** cmd=malloc(sizeof(char*));

	cmd[0]=malloc( (strlen(current)+1) * sizeof(char) );
	strcpy(cmd[0],current);
	
	while( (current=(char*)strtok(NULL," \n\t"))!=NULL){
		if(index>maxArgs){
			printf("Too many arguments\n");
			return;
		}
		
		index++;
		
		cmd=realloc(cmd,index*sizeof(char*)); //reallokowanie, aby dopisac nastepny agrument
		cmd[index-1]=malloc( ( strlen(current)+1) * sizeof(char) );
		strcpy(cmd[index-1],current);
		
	
	}
		  
	cmd=realloc(cmd,index*sizeof(char*));
	cmd[index]=NULL;		//powinno sie konczyc nullem;

	 
	int status;
	pid_t pid=fork();
	if(pid==0){
		
		struct rlimit cpuL;
		cpuL.rlim_max=cpuLimit;
		cpuL.rlim_cur=cpuLimit/10;
		
		if(setrlimit(RLIMIT_CPU,&cpuL)!=0){
			printf("setrlimit error\n");
			return;
		}
//		if(getrlimit(RLIMIT_CPU,&cpuL)==0){
//			printf( "cpuL_cur %d \n",(int)cpuL.rlim_cur);
//			printf( "cpuL_max %d \n",(int)cpuL.rlim_max);
//		}
	
		struct rlimit memL; 
		memL.rlim_max=memLimit;
		memL.rlim_cur=memLimit/4;
		
		if(setrlimit(RLIMIT_AS,&memL)!=0){
			printf("setrlimit error\n");
			return;
		}
//		if(getrlimit(RLIMIT_AS,&memL)==0){ 
//			printf( "memL_cur %d \n",(int)memL.rlim_cur);
//			printf( "memL_max %d \n",(int)memL.rlim_max);
//		} 
		
		if(execvp(cmd[0],cmd)==-1){
			printf("exec error\n");
			return;
		}		
	}else if(pid>0){
 		
		struct rusage usage;
		wait4(pid,&status,0,&usage);

		if(WIFEXITED(status)!=1){
			printf("error in line %d, command: %s\n",lineNumber,cmd[0]);
		
		}else{
			printf("User time %f \n",(double)(usage.ru_utime.tv_sec+(usage.ru_utime.tv_usec/(10e6))));
			printf("System time %f \n",(double)(usage.ru_stime.tv_sec+(usage.ru_stime.tv_usec/(10e6))));
		}
	}
	
	int i;
	for(i=1;i<=index;i++){
		free(cmd[i]);
	}
	

}

void evaluate(char *line,int lineNumber,rlim_t cpuLimit,rlim_t memLimit){

	if (line[0] == '#') {
		addVariable(line);
    } else {
		execCommand(line,lineNumber,cpuLimit,memLimit);

    }
	
}


int main(int argc, char* argv[]){
	
	
	if(argc!=4){
		printf("You need to give me path to file, cpu time in seconds, memory in MB\n");
		return 0;
	}
	
	char *filePath=argv[1];
	FILE * fp;
	if( (fp=fopen(filePath,"r"))==NULL){ //Opens a file to read. The file must exist.
		printf("fopen error\n");
		exit(1);
	}
	printf("%s\n",filePath);
	
	rlim_t cpuLimit=(rlim_t)atoi(argv[2]); //przyjmuje w MB i s
	rlim_t memLimit=((rlim_t)atoi(argv[3]))*10e6;
	
	
	size_t max_size=64;
	char *line=calloc(max_size,sizeof(char));
	int lineNumber=1;
	
	while(getline(&line,&max_size,fp)!=-1){
		printf("%s",line);
		evaluate(line,lineNumber,cpuLimit,memLimit);
		printf("\n");
		lineNumber++;
	}
	
	free(line);
	
	return 0;
}



