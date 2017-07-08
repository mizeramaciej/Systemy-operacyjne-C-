#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <memory.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/epoll.h>
#include <time.h>
#include <arpa/inet.h> 
#include <string.h>
#include "deff.h"

int isUnix;
char name[MAXNAMELEN];
const char* ip;
in_port_t port;
char path[UNIX_MAX_PATH];
int sock=-1;

void init_unsock();
void init_insock();
void atexitFun();
void hello();
void respond(struct message msg);

void init_unsock(){
	sock=socket(AF_UNIX,SOCK_DGRAM,0);
	if(sock==-1)
		exitError("unsocket");
	struct sockaddr_un unaddr;
	memset(&unaddr,0,sizeof(unaddr));
	strcpy(unaddr.sun_path,path); //TU
	unaddr.sun_family=AF_UNIX;
	
	if(bind(sock,(const struct sockaddr*)&unaddr,sizeof(sa_family_t)) == -1)
		exitError("unixbind");
	
	if( connect(sock,(const struct sockaddr*)&unaddr,sizeof(unaddr)) == -1)
		exitError("unix_connect");
}

void init_insock(){
	sock=socket(AF_INET,SOCK_DGRAM,0);
	if(sock==-1)
		exitError("insocket");
	
	struct sockaddr_in inaddr;
	memset(&inaddr,0,sizeof(inaddr));
	inaddr.sin_port=htons(port);
	inaddr.sin_family=AF_INET;
	if(inet_aton(ip,&inaddr.sin_addr) == 0) // zwraa 0  z przypadku bledu
		exitError("inet_aton");

/*	int res;
    if((res = inet_pton(AF_INET, ip, &inaddr.sin_addr))==0) {
        printf("Wrong adress\n");
        exit(1);
    }
    else if(res == -1) {
        perror("inet_pton");
        exit(1);
    }
*/	
	if( connect(sock,(const struct sockaddr*)&inaddr,sizeof(inaddr)) == -1)
		exitError("inet_connect");
}

int main(int argc, char *argv[])
{	
	printf("START CLIENT\n");
	atexit(atexitFun);
	
	if(argc !=4 && argc !=5){
		printf("Example	./client <name> <unix> <path>\n\t ./client <name> <inet> <adress> <port>\n");
		exit(1);
	}
	int i;
	for(i=0;i<MAXNAMELEN;i++)
		name[i]='\0';
	strcpy(name,argv[1]);
	
	if(strcmp("unix",argv[2])==0)
		isUnix=1;
	else if(strcmp("inet",argv[2])==0)
		isUnix=0;
	else{
		printf("Example	./client <name> <unix> <path>\n\t ./client <name> <inet> <adress> <port>\n");
		exit(1);	
	}
	
	if(isUnix==1){
		for(i=0;i<UNIX_MAX_PATH;i++)
			path[i]='\0';
		strcpy(path,argv[3]);	
		init_unsock();
	}else{
		ip=argv[3];
		port=(in_port_t)atoi(argv[4]);
		init_insock();
	}
	
	hello();
	
	while(1){
		struct message msg;
	//	ssize_t resrec=recv(sock,&msg,sizeof(msg),MSG_WAITALL);W
	//	if(resrec<=0)
	//		return 0;
		
		if(read(sock,&msg,sizeof(msg))!=sizeof(msg))
			exitError("read");
		printf("got msg");fflush(stdout);
		switch(msg.type){
			case MSG:
				printf("Message receved");
				fflush(stdout);
				respond(msg);
				break;
			case PING:
				msg.type=PING;
				printf("ping");fflush(stdout);
				if(write(sock,&msg,sizeof(msg))==-1)
					exitError("PING write");	
				break;
			case EXIT:
				return(0);
			case HELLO:
				fprintf(stderr,"Server, please just send me a message!");
				break;
			case ANSWER:
				fprintf(stderr,"Server, please just send me a message!");
				break;
		}
		
	}
	
	printf("CLIENT END\n");
    return 0;

	
}

void respond(struct message msg){
	msg.type=ANSWER;
	msg.counter++;
	switch(msg.oper){
		case ADD:
			msg.answer=msg.num1+msg.num2;
			break;
		case SUB:
			msg.answer=msg.num1-msg.num2;
			break;
		case MUL:
			msg.answer=msg.num1*msg.num2;
			break;
		case DIV:
			msg.answer=msg.num1/msg.num2;
			break;	
	}
	if(write(sock,&msg,sizeof(msg))==-1)
		exitError("answer write");
}

void hello(){
	struct message msg;
	msg.type=HELLO;
	strcpy(msg.name,name);
	if( write(sock,&msg,sizeof(msg))==-1)
		exitError("hello send");
}

void atexitFun(){
	if(sock!=-1){
		shutdown(sock,SHUT_RDWR);
		close(sock);
	}
}




