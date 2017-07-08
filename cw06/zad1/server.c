#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <string.h>
#include "inf.h"

int serverQueueID;
int clientsQueueIDS[MAX_CLIENTS];
int clientsCounter = 0;

void exitHandler(void) {
    msgctl(serverQueueID, IPC_RMID, NULL);
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

void msg_rcv(void) {
    struct msgbuf msg;
    msgrcv(serverQueueID, &msg, MSG_SIZE, 0, 0);
    switch (msg.mtype) {
        case ClientRequest:
            printf("Client with PID=%d has sent ClientRequest.\n", msg.clientPID);
            if (clientsCounter < MAX_CLIENTS) {
                clientsQueueIDS[clientsCounter] = msg.clientQueueID;
                msg.mtype = ServerAccept;
                msg.clientID = clientsCounter;
                msgsnd(msg.clientQueueID, &msg, MSG_SIZE, 0);
                printf("Client with PID=%d has been accepted and got id=%d.\n", msg.clientPID, msg.clientID);
                clientsCounter++;
            } else {
                msg.mtype = ServerDenies;
                msgsnd(msg.clientQueueID, &msg, MSG_SIZE, 0);
                printf("Client with PID=%d has been declined because of maximum of clients reached.\n", msg.clientPID);
            }
            break;
			
        case ServiceEcho:
            printf("Client with id=%d wants ServiceEcho.\n", msg.clientID);
            msg.mtype = ServerResponse;
            msgsnd(clientsQueueIDS[msg.clientID], &msg, MSG_SIZE, 0);
            break;
			
        case ServiceUpper:
            printf("Client with id=%d wants ServiceUpper.\n", msg.clientID);
            msg.mtype = ServerResponse;
            upperCaseF(msg.mtext);
            msgsnd(clientsQueueIDS[msg.clientID], &msg, MSG_SIZE, 0);
            break;
			
        case ServiceTime:
            printf("Client with id=%d wants ServiceTime.\n", msg.clientID);
            msg.mtype = ServerResponse;
            getTimeF(msg.mtext);
            msgsnd(clientsQueueIDS[msg.clientID], &msg, MSG_SIZE, 0);
            break;
			
        case ServiceTerm:
            printf("Client with id=%d wants ServiceTerm. Stop.\n", msg.clientID);
            exit(0);
    }
}

int main(void) {
  
	serverQueueID = msgget(ftok(PATHNAME, PROJ_ID), IPC_CREAT | 0600);
	atexit(exitHandler);
    signal(SIGINT, signalHandler);
    printf("Server is now running...\n");
 
	while(1)
        msg_rcv();
}