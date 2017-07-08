#ifndef inf_H
#define inf_H

#include <stdlib.h>
#define TEXTSIZE 100
#define MAX_CLIENTS 4
#define MSG_SIZE (sizeof(struct msgbuf)-sizeof(long))
#define PATHNAME getenv("HOME")
#define PROJ_ID 8

enum msgtype {
	ClientRequest = 1,
	ServerAccept,
	ServerDenies,
	ServerResponse,
	ServiceEcho,
	ServiceUpper,
	ServiceTime,
	ServiceTerm
};

struct msgbuf {
	long mtype;
	pid_t clientPID;
	int clientID;
	int clientQueueID;
	char mtext[TEXTSIZE];
};
#endif