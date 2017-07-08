#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "inf.h"

int clientQueueID;
int serverQueueID;
int clientID;

void exitHandler(void) {
    msgctl(clientQueueID, IPC_RMID, NULL);
}

void signalHandler(int signum) {
    exit(0);
}

void msg_snd(struct msgbuf msg) {
    msg.clientID = clientID;
    if (msgsnd(serverQueueID, &msg, MSG_SIZE, 0)) {
        printf("Could not connect with server. Stop.\n");
        exit(1);
    }
}

void msg_rcv(void) {
    struct msgbuf msg;
    msgrcv(clientQueueID, &msg, MSG_SIZE, 0 , 0); //0 najstarszy komunikat
    switch (msg.mtype) {
        case ServerAccept:
            clientID = msg.clientID;
            printf("Server has sent ServerAccept. My id=%d.\n", msg.clientID);
            break;
			
        case ServerDenies:
            printf("Server has sent ServerDenies. Stop.\n");
            exit(1);
			
        case ServerResponse:
            printf("Server has sent ServerResponse with message: %s\n", msg.mtext);
    }
}

int main(void) {

    struct msgbuf msg;

	serverQueueID = msgget(ftok(PATHNAME, PROJ_ID), 0);
	if (serverQueueID == -1) {
		printf("Could not open server queue.\n");
        return -1;
	}

	clientQueueID = msgget(IPC_PRIVATE, 0600); //creating queue
    
	atexit(exitHandler);
    signal(SIGINT, signalHandler);
	
    printf("Client is now running...\n");

    // Send request to server
    msg.mtype = ClientRequest;
    msg.clientPID = getpid();
    msg.clientQueueID = clientQueueID;
    msg_snd(msg);

    // Respond
    msg_rcv();

    printf("Available commands are:\n"
                   "echo <msg>\n"
                   "upper <msg>\n"
                   "time\n"
                   "term\n");
	
    char cmd[TEXTSIZE];
	
    while (1) {
        scanf("%s", cmd);
		
        if (strcmp(cmd, "echo")==0) {
            msg.mtype = ServiceEcho;
            scanf("%s", msg.mtext);
            msg_snd(msg);
        } else if (strcmp(cmd, "upper")==0) {
            msg.mtype = ServiceUpper;
            scanf("%s", msg.mtext);
            msg_snd(msg);
        } else if (strcmp(cmd, "time")==0) {
            msg.mtype = ServiceTime;
            msg_snd(msg);
        } else if (strcmp(cmd, "term")==0) {
            msg.mtype = ServiceTerm;
            msg_snd(msg);
            printf("Client has sent ServiceTerm. Stop.\n");
            exit(0);
        } else {
            printf("No such command: %s\n", cmd);
            continue;
        }

        // Respond
        msg_rcv();
    }
}