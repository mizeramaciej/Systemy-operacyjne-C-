#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <mqueue.h>
#include "infposix.h"

int clientQueueID;
int serverQueueID;
int clientID;
char queueName[TEXTSIZE];



void signalHandler(int signum) {
    exit(0);
}

mqd_t createQueue(void){ 
	struct mq_attr at;
	at.mq_maxmsg=MAX_QUEUESIZE;
	at.mq_msgsize=MSG_SIZE;
	sprintf(queueName,"/queue%d",getpid());
	mqd_t result= mq_open(queueName,O_CREAT|O_RDONLY,0600,&at);
	if(result==-1){
		printf("Couldn't open client queue.\n");
		exit(1);
	}
	return result;
}

void msg_snd(struct msgbuf msg) {
	msg.clientID = clientID;
    if (mq_send(serverQueueID, (char*)&msg, MSG_SIZE, 0)) {
        printf("Could not connect with server. Stop.\n");
        exit(1);
    }
}

void exitHandler(void) {
    struct msgbuf msg;
    msg.mtype = ClientLogout;
    msg_snd(msg);
	mq_close(clientQueueID);
    mq_close(serverQueueID);
    mq_unlink(queueName);
}

void msg_rcv(void) {
    struct msgbuf msg;
 
	mq_receive(clientQueueID,(char*)&msg,MSG_SIZE,NULL);
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

	serverQueueID = mq_open("/q_server",O_WRONLY);
	if (serverQueueID == -1) {
		printf("Could not open server queue.\n");
        return -1;
	}

	clientQueueID = createQueue(); //creating queue
	
    atexit(exitHandler);
    signal(SIGINT, signalHandler);
	
    printf("Client is now running...\n");

  
    msg.mtype = ClientRequest;
    msg.clientPID = getpid();
	strcpy(msg.mtext,queueName);
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

        //Respond waiting
        msg_rcv();
    }
}