/* echoserver.c */

#include <cnaiapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

appnum app;
connection con;
char buffer[1024];
int r;

int main(int argc, char *argv[])

{

	if (argc == 2) {

		app = atoi(argv[1]);
	} else {
		perror("please give a port number");
		return 0;

	}
	con = await_contact(app);

	while (con > 0) {
		r = read(con, buffer, 1024);

		if (r <= 0) {
			perror("fehler bei read");
			return 0;

		}

		r = write(con, buffer, 1024);

	}
	if (con < 0) {
		perror("CONECTION kleiner als 0");
		return 0;
	}

	return 0;
}
