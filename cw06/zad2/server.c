#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <string.h>
#include <mqueue.h>
#include <unistd.h>
#include "infposix.h"

int serverQueueID;
int clientsQueueIDS[MAX_CLIENTS];
int clientsCounter = 0;
const char queueName[]="/q_server";


void exitHandler(void) {
	mq_close(serverQueueID);
    int i;
	for(i=0;i<clientsCounter;i++)
		mq_close(clientsQueueIDS[i]);
	mq_unlink(queueName);
}

void signalHandler(int signum) {
    exit(0);
}

void upperCaseF(char *string) {
    while (*string) {
        *string = (char)toupper(*string);
        string++;
    }
}

void getTimeF(char *string) {
	struct tm * tinfo;
    time_t t;  
    time(&t);
    tinfo = localtime(&t);
    strcpy(string, asctime(tinfo));
}

mqd_t createQueue(void){
	struct mq_attr at;
	at.mq_msgsize=MSG_SIZE;
	at.mq_maxmsg=MAX_QUEUESIZE;
	mqd_t result= mq_open(queueName,O_CREAT|O_RDONLY,0600,&at);
	if(result==-1){
		printf("Couldn't open client queue.\n");
		exit(1);
	}
	return result;
}

void msg_rcv(void) {
    struct msgbuf msg;

	mq_receive(serverQueueID,(char*)&msg,MSG_SIZE,NULL);
    switch (msg.mtype) {
        case ClientRequest:
            printf("Client with PID=%d has sent ClientRequest.\n", msg.clientPID);
            if (clientsCounter < MAX_CLIENTS) {
                clientsQueueIDS[clientsCounter] = mq_open(msg.mtext, O_WRONLY);
                msg.mtype = ServerAccept;
                msg.clientID = clientsCounter;
                mq_send(clientsQueueIDS[clientsCounter], (char*)&msg, MSG_SIZE, 0);
                printf("Client with PID=%d has been accepted and got id=%d.\n", msg.clientPID, msg.clientID);
                clientsCounter++;
            } else {
                msg.mtype = ServerDenies;
                int clientQueueID = mq_open(msg.mtext, O_WRONLY);
                mq_send(clientQueueID, (char*)&msg, MSG_SIZE, 0);
                mq_close(clientQueueID);
                printf("Client with PID=%d has been declined because of maximum of clients reached.\n", msg.clientPID);
            }
            break;
			
        case ServiceEcho:
            printf("Client with id=%d wants ServiceEcho.\n", msg.clientID);
            msg.mtype = ServerResponse;
            mq_send(clientsQueueIDS[msg.clientID], (char*)&msg, MSG_SIZE, 0);
            break;
			
        case ServiceUpper:
            printf("Client with id=%d wants ServiceUpper.\n", msg.clientID);
            msg.mtype = ServerResponse;
            upperCaseF(msg.mtext);
            mq_send(clientsQueueIDS[msg.clientID], (char*)&msg, MSG_SIZE, 0);
            break;
			
        case ServiceTime:
            printf("Client with id=%d wants ServiceTime.\n", msg.clientID);
            msg.mtype = ServerResponse;
            getTimeF(msg.mtext);
            mq_send(clientsQueueIDS[msg.clientID], (char*)&msg, MSG_SIZE, 0);
            break;
			
        case ServiceTerm:
            printf("Client with id=%d wants ServiceTerm. Stop.\n", msg.clientID);
            exit(0);
			
		case ClientLogout:
			printf("Client with id=%d wants ClientLogout.\n", msg.clientID);
            mq_close(clientsQueueIDS[clientsCounter]);
            break;
    }
}

int main(void) {

	serverQueueID = createQueue();
	atexit(exitHandler);
    signal(SIGINT, signalHandler);
    printf("Server is now running...\n");

	while(1)
        msg_rcv();
}