#ifndef infposix_H
#define infposix_H

#include <stdlib.h>
#include <mqueue.h>
#define TEXTSIZE 100
#define MAX_CLIENTS 4
#define MAX_QUEUESIZE 10
#define MSG_SIZE sizeof(struct msgbuf)

enum msgtype {
	ClientRequest = 1,
	ServerAccept,
	ServerDenies,
	ServerResponse,
	ServiceEcho,
	ServiceUpper,
	ServiceTime,
	ServiceTerm,
    ClientLogout
};

struct msgbuf {
	long mtype;
	pid_t clientPID;
	int clientID;
	char mtext[TEXTSIZE];
};

#endif