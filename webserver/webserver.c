/* webserver.c */

#include <cnaiapi.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/sendfile.h>

appnum app;
connection con;
void openTheFile(connection con) {

	char buf[1024] = { "" };
	char buf2[1024] = { "" };
	int len;
	char path[1024] = { "htdocs" };

	char headerOk[] = { "HTTP/1.0 200 OK\r\n\r\n\0" };
	char headerNotFound[] = { "HTTP\1.0 404 NOT FOUND\r\n\r\n\0" };
	char* token;

	int contains = 0;
	//end pakage
	char reqEnd[] = { "\r\n\r\n" };

	while (contains != 1) {
		//read the connection
		len = read(con, buf2, 1024);
		if (len == -1 || len == 0) {
			exit(1);
		}
		buf2[len] = 0;
		strcat(buf, buf2);
		if (strstr(buf, reqEnd) != NULL) {
			contains = 1;
		}
	}

	token = strtok(buf, " ");
	token = strtok(NULL, " ");
	if (token == NULL) {

		perror("error by read the path!");
		return;
	}
	//if the client just search host

	if (strcmp(token, "/") == 0) {
		//2 Versions either load automatically the index.html, or Page Not Found
		//token = "/index.html";
		write(con, headerNotFound, strlen(headerNotFound));
		end_contact(con);
		return;

	}

	strcat(path, token);

	//file descriptor
	int fd = open(path, O_RDONLY);

	//information about a file
	struct stat stats;
	int st;
	st = fstat(fd, &stats);

	if (st == -1) {
		perror("Error by File open!");
		write(con, headerNotFound, strlen(headerNotFound));
		end_contact(con);
		return;
	}

	//send Header text
	write(con, headerOk, strlen(headerOk));

	//send Data
	sendfile(con, fd, NULL, stats.st_size);

	//End the connection
	int end = end_contact(con);

	if (end != 0) {
		perror("Could not close child connection!");
	}
	return;

}

int main(int argc, char *argv[]) {

	signal(SIGCHLD, SIG_IGN);
	while (1) {
		if (argc == 2) {

			app = atoi(argv[1]);
		} else {
			perror("please give a port number");
			return 0;

		}

		con = await_contact(app);
		if (con < 0) {

			perror("CONECTION smaller than 0");
			return 0;

		}
		int pid = fork();
		if (pid == -1) {

			perror("ERROR by create child process!");
			return 0;
		}

		if (pid == 0) {

			openTheFile(con);
			return 0;
		} else {

			end_contact(con);

		}

	}

	return 0;

}
