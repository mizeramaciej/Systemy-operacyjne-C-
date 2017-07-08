#define UNIX_MAX_PATH 108
#define MAXCLIENTS 16
#define MAXNAMELEN 16


enum type{
	MSG, ANSWER, HELLO,PING, EXIT
};
enum operation{
	ADD = 0, SUB, MUL, DIV
};

struct message{
	enum type type;
	float answer;
	float num1;
	float num2;
	int counter;
	enum operation oper;
	char name[MAXNAMELEN];	
}message;

void exitError(const char* err){
  	printf("%s Errno %d, %s\n", err, errno, strerror(errno));
  	exit(1);
}