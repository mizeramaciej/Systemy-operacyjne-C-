#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<sys/wait.h>

#define MAX_CMDS 22
#define MAX_ARGS 6

const char* SEP = "|";

void exec_line(char*);
void exec_cmd(char*,int*,int,int);

int main(int argc, char *argv[]){
	printf("START\n");
	
	char* line=NULL;
	size_t size=0;
	getline(&line,&size,stdin);
	exec_line(line);
	
	return 0;
}	
	
void exec_line(char * line){
	
	char **commands= calloc(MAX_CMDS,sizeof(char*));
	int i;
	
	commands[0]=strtok(line,SEP);
	for(i=1;i<MAX_CMDS && commands[i-1]!=NULL;i++){
		commands[i]=strtok(NULL,SEP);	

	}
	
	int fd[2];
	pipe(fd);
	int last=STDIN_FILENO;
	for(i=0; i<MAX_CMDS && commands[i]!=NULL ;i++){
		pipe(fd);
		if( i==MAX_CMDS -1 || commands[i+1] ==NULL){
			exec_cmd(commands[i],fd,last,-1);	
		}else{
			exec_cmd(commands[i],fd,last,STDIN_FILENO);
		}
		last=fd[0];
		close(fd[1]);
		
	}
	close(fd[0]);
}
	
void exec_cmd(char* cmds, int *fd, int in,int out){
	int i;
	char **cmd=calloc(MAX_ARGS,sizeof(char*));
	cmd[0]=strtok(cmds," \n\t\r");
	for(i=1;i<MAX_ARGS && cmd[i-1]!=NULL;i++){
		cmd[i]=strtok(NULL," \n\t\r");
	}
	
	int pid=fork();
	if(pid==0){
		if(in!=-1) dup2(in,STDIN_FILENO);
		if(out!=-1) dup2(fd[1],STDOUT_FILENO);
		if(execvp(cmd[0],cmd)==-1){
			printf("exec error\n");
			exit(1);
		}
	}else if(pid>0){
		close(fd[1]);
		dup2(fd[0],STDIN_FILENO);
		wait(NULL);
		if(in != -1) close(in);
		
	}else{
		printf("FORK error\n");
		exit(1);
	}
	
}
	


							
				