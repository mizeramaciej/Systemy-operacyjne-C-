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
#include "deff.h"

#define MAX_EPOLL_EVENTS 24

struct sockaddr_in inaddr;
struct sockaddr_un unaddr;
int un_sock=1,net_sock=1;
char path[UNIX_MAX_PATH];
int clients[MAXCLIENTS];
int epoll;
int taskCounter=0;
in_port_t port;
pthread_t servthread,commandthread,pingthread;
struct epoll_event events[MAX_EPOLL_EVENTS];

pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER;

int clientFds[MAXCLIENTS];
int clientsPong[MAXCLIENTS];
ssize_t clientFdsSize=-1;

void add_client(int);
void close_client(int);
int rand_client();
void init_sockets();
void atexitFun();
void sigexit(int);

void *serverTask(void * sth){
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	init_sockets();
	if(listen(net_sock,SOMAXCONN)!=0)
		exitError("net_listen");
	if(listen(un_sock,SOMAXCONN)!=0)
		exitError("un_listen");
	
	epoll=epoll_create1(0);
	if(epoll==-1)
		exitError("epoll_create1");
	
	struct epoll_event event;
	event.events=EPOLLIN|EPOLLRDHUP;
	event.data.fd=un_sock;
	if(epoll_ctl(epoll,EPOLL_CTL_ADD,un_sock,&event)!=0)
		exitError("epoll_ctl_un");
	
	event.data.fd=net_sock;
	if(epoll_ctl(epoll,EPOLL_CTL_ADD,net_sock,&event)!=0)
		exitError("epoll_ctl_net");
	
	while(1){
		int regievents=epoll_wait(epoll,events,MAX_EPOLL_EVENTS,-1);
		if(regievents==-1){
			printf("epoll_wait error\n");
			return NULL;
		}
		int i;
		struct sockaddr in_addr;
		for(i=0;i<regievents;i++){
			event = events[i];
			          
			if ((event.events & EPOLLERR) != 0 || (event.events & EPOLLHUP) != 0) {
                
                if (event.data.fd != net_sock && event.data.fd != un_sock) {
                    close_client(event.data.fd);
                }
            } else if ((event.events & EPOLLRDHUP) != 0) {
                if (event.data.fd != net_sock && event.data.fd != un_sock) {
                    close_client(event.data.fd);
                }
			} else if (event.data.fd == net_sock || event.data.fd == un_sock) {
                while(1) {
					
					
                    socklen_t in_len = sizeof(in_addr);
                    int fd = accept4(event.data.fd, &in_addr, &in_len,SOCK_NONBLOCK);
					//int fd = accept(event.data.fd, &in_addr, &in_len);
                    if (fd == -1) {
						
                        if (errno == EAGAIN || errno == EWOULDBLOCK) {
                            break;
                        } else {
                            perror("accept");
                            break;
                        }
                    }

                    struct epoll_event event2;
                    event2.events = EPOLLIN | EPOLLET; //edge triggerde
                    event2.data.fd = fd;
                    if (epoll_ctl(epoll, EPOLL_CTL_ADD, fd, &event2) == -1) {
                        exitError("epoll_ctl");
                    }

                    add_client(fd);
                }
			}else{
                while(1) {
                    struct message msg;
                    ssize_t count = recv(event.data.fd, &msg, sizeof(msg), MSG_WAITALL);
                    if (count == -1) {
                        if (errno != EAGAIN) {
                            perror("recv");
                            close_client(event.data.fd);
                        }
                        break;
                    } else if (count == 0) {
                        close_client(event.data.fd);
                        break;
                 //   } else if (count < sizeof(msg)) {
                 //       fprintf(stderr, "received partial message\n");
                 //       continue;
                    } else {
                        switch (msg.type) {
                            case MSG:
                                printf("Client please, just send me an answer not a message!\n");
								fflush(stdout);
                                break;
                            case ANSWER:
                                printf("Task no.%d = %f\n", msg.counter, msg.answer);
                                fflush(stdout);
                                break;
                            case PING:
								pthread_mutex_lock(&lock);
								for(i=0;i<=clientFdsSize;i++){
									if(clientFds[i]==event.data.fd){
										//printf("GOT");
										clientsPong[i]++;
										break;
									}
								}
								pthread_mutex_unlock(&lock);
                                break;
                            case EXIT:
                                close_client(event.data.fd);
                                break;
                            case HELLO:
                                printf("%s connected\n", msg.name);
								fflush(stdout);
                                break;
                        }
                    }
                }
			}
		}
	}
	return NULL;
};

void init_sockets(){
	un_sock=socket(AF_UNIX,SOCK_STREAM | SOCK_NONBLOCK,0);	
	if(un_sock == -1)
		exitError("un socket");
	
	struct sockaddr_un unaddr;
	memset(&unaddr,0,sizeof(unaddr));
	strcpy(unaddr.sun_path,path); //TU
	unaddr.sun_family=AF_UNIX;

	if( bind(un_sock,(const struct sockaddr*)&unaddr,sizeof(unaddr)) == -1)
		exitError("unbind");
	
	
	net_sock=socket(AF_INET,SOCK_STREAM | SOCK_NONBLOCK,0);	
	if(net_sock == -1)
		exitError("un socket");
	
	struct sockaddr_in inaddr;
	memset(&inaddr,0,sizeof(inaddr));
	inaddr.sin_port=htons(port);
	inaddr.sin_family=AF_INET;
	inaddr.sin_addr.s_addr=INADDR_ANY; //dowolny adres IP  | tylko lokalne to INADDR_LOOPBACK

	if( bind(net_sock,(const struct sockaddr*)&inaddr,sizeof(inaddr)) == -1)
		exitError("inbind");
	
}

void *commandline(void* sth){
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	char oper;
	float num1,num2;
	struct message msg;
	while(1){
		
		printf("%d: ",taskCounter); fflush(stdout);

		if(scanf("%s %f %f",&oper,&num1,&num2)==EOF)
			continue;
		
		
		if(strcmp(&oper,"+")==0){
			msg.oper=ADD;	
		}else if(strcmp(&oper,"-")==0){
			msg.oper=SUB;
		}else if(strcmp(&oper,"*")==0){
			msg.oper=MUL;
		}else if(strcmp(&oper,"/")==0){
			msg.oper=DIV;
		}else{
			continue;	
		}
			
		int client=rand_client();
		if(client==-1)
			fprintf(stderr,"No clients\n");
		else{
			msg.counter=taskCounter;
			msg.num1=num1;
			msg.num2=num2;
			//msg.oper=oper;
			msg.type=MSG;
		}
		
		if(write(client,&msg,sizeof(msg))==-1)
			exitError("write");
		taskCounter++;
		

	}
}

void* ping_task(void* sth){
	struct message msg;
	msg.type=PING;
	int i;
	for(;;){
		pthread_mutex_lock(&lock);
		for(i=0;i<=clientFdsSize;i++){
			clientsPong[i]=0;
			write(clientFds[i],&msg,sizeof(msg));
		//	fprintf(stderr,"serping");
		}
		pthread_mutex_unlock(&lock);
		sleep(1);
		pthread_mutex_lock(&lock);
		for(i=0;i<=clientFdsSize;i++){
			if(clientsPong[i]==0)
			//	printf("canceling pong");fflush(stdout);
			//	close_client(clientFds[i]);
				;
		}
		pthread_mutex_unlock(&lock);
	}
	
	
	return NULL;
}

int main(int argc, char *argv[]){
	printf("SERVER START \n");
	if(argc!=3){
		printf("Example : ./server <port> <path>\n");
		exit(1);
	}
	atexit(atexitFun);
	signal(SIGINT,sigexit);
	
	srand((unsigned int)getpid());
	
	port=(in_port_t)atoi(argv[1]);
	int i;
	for(i=0;i<UNIX_MAX_PATH;i++)
		path[i]='\0';
	strcpy(path,argv[2]);
	
	for(i=0;i<MAXCLIENTS;i++){
		clientFds[i]=-1;	
	}
	
	if(pthread_create(&servthread,NULL,serverTask,NULL) !=0 )
		exitError("pthread_create");
	if(pthread_create(&commandthread,NULL,commandline,NULL) !=0 )
		exitError("pthread_create");
	if(pthread_create(&pingthread,NULL,ping_task,NULL) !=0 )
		exitError("pthread_create");
	
	
	if(pthread_join(servthread,NULL) != 0)
		exitError("join");
	if(pthread_join(commandthread,NULL) != 0)
		exitError("join");
	if(pthread_join(pingthread,NULL) != 0);
		exitError("join");
	
	
	
	printf("END\n");
	return 0;
};

void add_client(int fd){
	pthread_mutex_lock(&lock);
	if(clientFdsSize >=MAXCLIENTS){
		exitError("Upper clients limit");	
	}
	clientFds[++clientFdsSize]=fd;
	pthread_mutex_unlock(&lock);
}

void close_client(int fd){
	pthread_mutex_lock(&lock);
	int i,j=0;
	for(i=0;i<=clientFdsSize;i++){
		if(clientFds[i]!=fd){
			clientFds[j++]=clientFds[i];
		}else{
			if(close(fd)==-1)
				exitError("close fd");
			clientFdsSize--;
		}
	}
	pthread_mutex_unlock(&lock);
}

int rand_client(){
//	if(clientFdsSize==0) return -1;
	//return clientFds[rand() % clientFdsSize];
	if(clientFdsSize==-1) return -1;
	if(clientFdsSize==0) return clientFds[0];
	int rn=(rand()% (clientFdsSize+1));
	return clientFds[rn];//rn
}

void atexitFun(){
	if(un_sock !=-1){
		if(shutdown(un_sock,SHUT_RDWR) ==-1)
			exitError("shutdown");
		if(close(un_sock)==-1)
			exitError("close");
		unlink(path);
	}
	if(net_sock !=-1){
		if(shutdown(net_sock,SHUT_RDWR) ==-1)
			exitError("shutdown");
		if(close(net_sock)==-1)
			exitError("close");
	}
	pthread_mutex_destroy(&lock);
};
void sigexit(int sig){
	atexitFun();
	exit(0);	
};