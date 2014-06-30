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
void imageCommand();
int send_banner();
void searchString();
void searchString(char buf[], double leftVel, double rightVel) {
	if (strstr(buf, "left")) {
		char* tmp = strtok(buf, "=");
		tmp = strtok(NULL, "\0");
		leftVel = atof(tmp);
		printf("leftvel is \"%s\"\n", leftVel);
	}
	if (strstr(buf, "right")) {
		char* tmp2 = strtok(buf, "=");
		tmp2 = strtok(NULL, "\0");
		rightVel = atof(tmp2);
		printf("rightVel is \"%f\"\n", rightVel);
	}

}
int send_banner(int s, int ok) {
	int bytes;
	char header[] = { "HTTP/1.0 200 OK\r\n\r\n" };
	char headerNotFound[] = { "HTTP/1.0 404 NOT FOUND\r\n\r\n" };

	if (ok) {
		bytes = send(s, header, strlen(header), 0);
		printf("ok!\n");
	} else {
		bytes = send(s, headerNotFound, strlen(headerNotFound), 0);
	}

	if (bytes == -1) {
		printf("send() in send_banner() failed\n");
		return 1;
	}

	return 0;

}

void driveCommand(char buf[]) {

	char bufCopy[BUF_SIZ];
	double leftVel = 0.0;
	double rightVel = 0.0;
	strcpy(bufCopy, buf);
	char* command;
	char* command_end;
	command = strtok_r(bufCopy, "& ", &command_end);

	float speedLeft = 0.0;
	float speedRight = 0.0;

	//ueber Paramter iterieren
	while (command != NULL) {
		char* value_end;
		char* value = strtok_r(command, "=", &value_end);

//	        puts(command);
		printf("command is  %s \n", command);

		if (strcmp(command, "left") == 0) {
			value = strtok_r(NULL, "=", &value_end);
			printf("ValueLeft %s \n", value);
			speedLeft = atof(value);
			printf("left: %f", speedLeft);
		} else if (strcmp(command, "right") == 0) {

			value = strtok_r(NULL, "=", &value_end);

			speedRight = atof(value);
			printf("right: %f \n", speedRight);
		}

		command = strtok_r(NULL, "& ", &command_end);
	}
//
	printf("left: %f, right: %f", speedLeft, speedRight);
	setRobSpeed(speedLeft, speedRight);
}
void imageCommand(int sock) {
	char* image;
	int size;

	getRobCamImage(&image, &size);
	send_banner(sock, 1);

	write(sock, image, size);
	printf("image \n");
}

int getTheCommand(int s) {

	char buf[BUF_SIZ];
	int bytes;
	int i;
	for (i = 0; i < BUF_SIZ - 1; i++) {
		buf[i] = '\0';
	}
//	     Empfang die Daten von einem Socket
	bytes = recv(s, buf, sizeof(buf) - 1, 0);
	if (bytes == -1) {
		perror("recv() in getTheCommand() failed");
		return 1;
	}

	buf[bytes] = '\0';

	printf("Banner is \"%s\"\n", buf);

//	printf("Banner is \"%s\"\n", strstr(buf, "drive"));
	if (strstr(buf, "drive")) {
		printf("Command Drive received!\n");
		driveCommand(buf);

	}
	if (strstr(buf, "image")) {

		printf("Command Image received!\n");
		imageCommand(s);
	} else {
		perror("invalid command!");
		//printf("buf is: %s\n", buf);

		//send_banner(sock, 1);
		return 0;
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
