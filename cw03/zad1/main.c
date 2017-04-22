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

void execCommand(char *commandLine,int lineNumber){
	
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
		if(execvp(cmd[0],cmd)==-1){
			printf("exec error\n");
			return;
		}
	}else if(pid>0){
		waitpid(pid,&status,0);
		if(WIFEXITED(status)!=1){
			printf("error in line %d, command: %s\n",lineNumber,cmd[0]);
		
		}
	}
	int i;
	for(i=1;i<=index;i++){
		free(cmd[i]); 
	}
	
}

void evaluate(char *line,int lineNumber){

	if (line[0] == '#') {
		addVariable(line);
    } else {
		execCommand(line,lineNumber);
    }
}


int main(int argc, char* argv[]){
	
	if(argc!=2){
		printf("You need to give me path\n");
		return 0;
	}
	
	char *filePath=argv[1];
	FILE * fp;
	if( (fp=fopen(filePath,"r"))==NULL){ //Opens a file to read. The file must exist.
		printf("fopen error\n");
		exit(1);
	}
	printf("%s\n",filePath);
	
	size_t max_size=64;
	char *line=calloc(max_size,sizeof(char));
	int lineNumber=1;
	
	while(getline(&line,&max_size,fp)!=-1){
		printf("%s",line);
		evaluate(line,lineNumber);
		lineNumber++;
	}
	
	return 0;
}



