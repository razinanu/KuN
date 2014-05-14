/* echoclient.c */

#include <cnaiapi.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
computer cmp;
appnum app;
char buf[1024];
connection con;

int main(int argc, char *argv[]) {

	if (argc == 1) {
		perror("please give a computer name and port number");
		return 0;
	}
	if (argc == 2) {
		perror("please give a port number");
		return 0;
	}
	if (argc >= 3) {
		cmp = cname_to_comp(argv[1]);
		app = atoi(argv[2]);
	}

	con = make_contact(cmp, app);
	if (con < 0) {

		perror("Fehler bei con ");
	}
	while (1) {

		read(0, buf, 1024);

		write(con, buf, 1024);

		int r = read(con, buf, 1024);

		if (r < 0) {

			perror("FEHLER BEI READ");
			return 0;
		}

		write(1, buf, 1024);
		memset(buf, 0, 1024);
	}

	return 0;
}
