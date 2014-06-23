#include "robot.h"

#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <poll.h>
#include <time.h>
#include <assert.h>
#include <sys/time.h>
static int sock;
static char* port;

#define BUF_SIZ	4096
;
int getTheCommand();
void driveCommmand();

void driveCommand(char buf[]) {

	char bufCopy[BUF_SIZ];
	printf("buf is \"%s\"\n", buf);
	strcpy(bufCopy, buf);
	char* leftPart = strtok(bufCopy, "&");
//	printf("firstPart is \"%s\"\n", leftPart);
	char* rightPart =  strtok(NULL, "\n");
//	printf("firstPart is \"%s\"\n", rightPart);
	char* tmp = strtok(leftPart, "=");
	tmp = strtok(NULL, "\0");
	double leftVel, rightVel;
	printf("leftPArt is \"%s\"\n", tmp);
	leftVel = atof(tmp);
	printf("leftVel is \"%f\"\n", leftVel);
	tmp = strtok(rightPart, "=");
	printf("firstPart is \"%s\"\n", tmp);
	tmp = strtok(NULL, "\0");
	rightVel = atof(tmp);
	printf("leftVel is \"%d\"\n", rightVel);

}

int getTheCommand(int s) {

	char buf[BUF_SIZ];
	int bytes;
//	     Empfang die Daten von einem Socket
	bytes = recv(s, buf, sizeof(buf) - 1, 0);
	if (bytes == -1) {
		perror("recv() in getTheCommand() failed");
		return 1;
	}

	buf[bytes] = '\0';

	printf("Banner is \"%s\"\n", buf);
	if (strstr(buf, "drive")) {
		printf("Command Drive recieved!\n");
		driveCommand(buf);

	}

	return 0;

}

int main(int argc, char* argv[]) {
	signal(SIGPIPE, SIG_IGN);
	if (-1 == initializeRob()) {

		exit(1);
	}
	printf("initializeRob()\n");
	while (1) {
		if (argc < 2) {
			printf("usage: %s <port>\n", argv[0]);
			exit(2);
		}
		port = atoi(argv[1]);
		printf("port[%d]", port);

		//first parameter: domain, PF for Protocol family
		//Second parameter: type of Sockets :SOCK_STREAM TCP, Network-Programm with TCP/IP
		//Third parameter:protocol 0
		sock = socket(PF_INET, SOCK_STREAM, 0);
		if (sock == -1) {
			perror("socket() failed");
			return 1;
		} else {
			printf("socket built. \n");

		}
		//int sockopt = 1;
//		setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof(&sockopt));
		struct sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.s_addr = INADDR_ANY;
//		printf("port[%s]",port);
		//client
		//inet_aton("127.0.0.1", &addr.sin_addr);
		//Server

//		printf("IP[%s]", addr.sin_addr.s_addr);
		//verbindet man einen Socket mit einem Ziel
//		int conn = connect(sock, (struct sockaddr*) &addr, sizeof(addr));
//
//		if (conn == -1) {
//			perror("connect() failed");
//			return 2;
//		}

		int bin = bind(sock, (struct sockaddr*) &addr, sizeof(addr));
		if (bin == -1) {
			perror("bind() failed");
			return 2;
		} else {
			printf("bind done!. \n");

		}
		int lis = listen(sock, 4);
		if (lis == -1) {
			perror("listen () failed");
			return 3;
		} else {
			printf("listen done!. \n");

		}
		while (1) {
			int s = accept(sock, NULL, NULL);
			if (s == -1) {
				perror("accept() failed");
				sendRobHeartbeat();
				continue;
			}
			getTheCommand(s);
			close(s);
		}

	}

	// TODO: code goes here ...

	// shutdown
	sleep(1);	// required to perform last robot command
	printf("Simulation finished\n");
	shutdownRob();
	close(sock);

	return (0);
}

