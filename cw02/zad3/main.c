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

//domyslnie advisory
//aby ustawic mandatory to nalezy miec dostem do root i wykonac kroki ;
// 'mount -oremount,mand /'         zmiana opcji w systemie plikow
// 'chmod g+s,g-x mandatory.txt' 
//
// PROGRAM DZIAŁA DLA TRYBU MANDATORY ( dla rygli wymuszanych )
//
// aby wlaczyc tryb advisory nalezy uzyc opcji 'nomand'


int lockRequest(int fp, int cmd,int type, int offset){
	
	struct flock lock;
	
	lock.l_type=type;
	lock.l_start=offset;
	lock.l_whence=SEEK_SET;
	lock.l_len=1;
	
	if (fcntl(fp,cmd,&lock)==-1){
		printf("fcntl error\n");
		return -1;
	}
	
	//return offset;
	return (int)lock.l_start;
}


void readf(int fp,int offset){
	
	if((lseek(fp, offset, SEEK_SET))==-1){
		printf("lseek error\n");
		return;
	}
	char * symbol=malloc(sizeof(char));
	if( (read(fp,symbol,sizeof(char)))!=sizeof(char)){
		printf("read error\n");
		return;
	}
	
	printf("Read symbol from position %d is '%s'",offset,symbol);	
	
	free(symbol);
}

int overwrite(int fp,int offset,char symbol){
	
	if((lseek(fp, offset, SEEK_SET))==-1){
		printf("lseek error\n");
		return -1;
	}
	
	if( (write(fp,&symbol,sizeof(char)))!=sizeof(char)){
		printf("write error\n");
		return -1;
	}
	
	printf("Overwrited position %d with '%s'",offset,&symbol);
	return 0;
}

int printLocks(int fp){
	int b;
	struct flock lock;
	
	int len=(int)lseek(fp,0,SEEK_END);
	
	lseek(fp, 0, SEEK_SET); //back to start
	
	for( b=0;b<len;b++){
		lock.l_type=F_WRLCK;
		lock.l_len=1;
		lock.l_whence=SEEK_SET;
		lock.l_start=b;
		
		if( fcntl(fp,F_GETLK,&lock)==-1){
			printf("fcntl error\n");
			return -1;
		}
		if(lock.l_type ==F_UNLCK){
			continue;
		}
		if(lock.l_type ==F_RDLCK){
			printf("Read lock on position %d | PID: %d\n",b,lock.l_pid);
		}
		if(lock.l_type ==F_WRLCK){
			printf("Write lock on position %d | PID: %d\n",b,lock.l_pid);	
		}
		
	}
	return 0;
}

int main(int argc, char* argv[]){
	
	if(argc!=2){
		printf("You need to give me path\n");
		return 0;
	}
	char *command=calloc(32,sizeof(char));
	char *filePath=argv[1];
	
	int fp;
	if( (fp=open(filePath,O_RDWR))==-1){
		printf("open error");
		exit(1);
	}
	
	printf("%s",filePath);
	
	
	char readOrWrite;
	char singleOrWait;
	int offset;
	char symbol;
	int result;
	
	while( strcmp(command,"quit")!=0 && strcmp(command,"q")!=0 ){
		
		printf("\nCommand : ");
		scanf("%s",command);
		
		if(strcmp(command,"help")==0){
			printf("Available commands :\nquit | q\nread <which byte>\noverwrite <which byte> <new byte>");
		}else
		
		if(strcmp(command,"read")==0 || strcmp(command,"r")==0 ){
			scanf("%d",&offset);
			readf(fp,offset);
		}else
		
		if(strcmp(command,"overwrite")==0 || strcmp(command,"ow")==0 ){
			int tmp=0;
			scanf("%i",&tmp);
			int offset1=tmp; 
			scanf("%s",&symbol);
			overwrite(fp,offset1,symbol);
		}else
		
		if(strcmp(command,"lock")==0){

			scanf("%s",&readOrWrite);
			result=-1;
			if(strcmp(&readOrWrite,"read")==0){
				scanf("%s",&singleOrWait);
				
				if(strcmp(&singleOrWait,"single")==0){
					scanf("%i",&offset);
					result=lockRequest(fp,F_SETLK,F_RDLCK,offset); 			
				}else
				if(strcmp(&singleOrWait,"wait")==0){
					scanf("%i",&offset);
					result=lockRequest(fp,F_SETLKW,F_RDLCK,offset);
				}else{
					printf("Wrong command\n");
				}	
				
				if(result>=0){
					printf("Read lock on position %d",result);	
				}else{
					printf("UNABLE\n");
				}
			}else
			
			if(strcmp(&readOrWrite,"write")==0){
				scanf("%s",&singleOrWait);
				result=-1;
				if(strcmp(&singleOrWait,"single")==0){
					scanf("%i",&offset);					
					result=lockRequest(fp,F_SETLK,F_WRLCK,offset);
				}else
				if(strcmp(&singleOrWait,"wait")==0){
					scanf("%i",&offset);
					result=lockRequest(fp,F_SETLKW,F_WRLCK,offset);
				}else{
					printf("Wrong command\n");
				}
				
				if(result>=0){
					printf("Write lock on position %d",result);	
				}else{
					printf("UNABLE\n");
				}
			}else{
				printf("Wrong command\n");
			}
			
		}else
		
		if(strcmp(command,"unlock")==0){
			scanf("%i",&offset);
			result=-1;
			result=lockRequest(fp,F_SETLK,F_UNLCK,offset);
			if(result>=0){
				printf("Unlocked position %d",result);	
			}else{
				printf("UNABLE\n");
			}
		}else
			
		if(strcmp(command,"printlocks")==0 || strcmp(command,"pl")==0){
			printLocks(fp);
		}else
			printf("UNKNOWN COMMAND. Type 'help' for help\n");
	}
	
	free(command);
	return 0;
}




