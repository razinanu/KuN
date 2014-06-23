#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <termios.h>
#include <signal.h>
#include <pthread.h>

#include "controller.h"

#define KEYCODE_RIGHT	0x43
#define KEYCODE_LEFT	0x44
#define KEYCODE_UP	0x41
#define KEYCODE_DOWN	0x42


static struct termios term_attr_orig;
static int pipefd[2];
static pthread_t pollThread;

static void cleanup(int sig)
{
	printf("cleaning up...\n");
	tcsetattr(STDIN_FILENO, TCSANOW, &term_attr_orig);
	exit(0);
}


static void * pollKeys(void *fd)
{
	int c;
	struct controllerInput ctrl;

	signal(SIGINT, cleanup);

	while ((c = getchar()) != EOF) {
		float l,r;
		switch (c) {
		    case KEYCODE_UP:
			l = r = 0.5;
			break;
		    case KEYCODE_DOWN:
			l = r = -0.5;
			break;
		    case KEYCODE_LEFT:
			l = -0.2;
			r = 0.2;
			break;
		    case KEYCODE_RIGHT:
			l = 0.2;
			r = -0.2;
			break;
		    case ' ':
			l = r = 0;
			break;
		    default:
			continue;
		}

		ctrl.speedLeft  = l;
		ctrl.speedRight = r;

		if (write(*(int*)fd, &ctrl, sizeof(ctrl)) <= 0) break;

	}

	cleanup(0);
	return NULL;
}


int initController()
{
	struct termios term_attr;

	if (pipe(pipefd) != 0) {
		perror("pipe() failed");
		return -1;
	}

	if (tcgetattr(STDIN_FILENO, &term_attr_orig) != 0) {
		perror("tcgetattr() failed");
		return -1;
	}
	memcpy(&term_attr, &term_attr_orig, sizeof(term_attr));
	term_attr.c_lflag &=~ (ICANON | ECHO);

	if (tcsetattr(STDIN_FILENO, TCSANOW, &term_attr) != 0) {
		perror("tcsetattr() failed");
		return -1;
	}

	if (pthread_create(&pollThread, NULL, pollKeys, &pipefd[1]))
	{
		perror("failed to create pthread");
		cleanup(0);
		return -1;
	}

	printf("Use arrow keys to control the robot and space to stop.\n");

	return pipefd[0];
}
