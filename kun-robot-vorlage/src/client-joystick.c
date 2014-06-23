#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <linux/joystick.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <pthread.h>

#include "controller.h"

#define FWD_SPEED 3.0
#define TURN_SPEED 0.5

static int joy_fd;
static int pipefd[2];
static pthread_t pollThread;
static int *axis=NULL;
static char *button=NULL;

static void * pollInput(void *fd)
{
	struct js_event js;
	struct controllerInput ctrl;

	while( 1 ) 	/* infinite loop */
	{
		/* read the joystick state */
		read(joy_fd, &js, sizeof(struct js_event));

			/* see what to do with the event */
		switch (js.type & ~JS_EVENT_INIT)
		{
			case JS_EVENT_AXIS:
				axis   [ js.number ] = js.value;
				break;
			case JS_EVENT_BUTTON:
				button [ js.number ] = js.value;
				break;
		}

		float fwd  = -(float)axis[1]/32768.f;
		float turn = +(float)axis[3]/32768.f;
		fwd *= FWD_SPEED;
		turn *= TURN_SPEED;
		float l = fwd + fwd*turn;
		float r = fwd - fwd*turn;

		ctrl.speedLeft  = l;
		ctrl.speedRight = r;

		if (write(*(int*)fd, &ctrl, sizeof(ctrl)) <= 0) break;
	}

	close( joy_fd );	/* too bad we never get here */
	return 0;
}


int initController()
{
	int num_of_axis=0, num_of_buttons=0, i;
	char  name_of_joystick[80];
	char path[20];

	if (pipe(pipefd) != 0) {
		perror("pipe() failed");
		return -1;
	}

	for (i=0; i < 10; i++) {
		sprintf(path, "/dev/input/js%u", i);
		if ((joy_fd = open(path, O_RDONLY)) != -1)
			break;
	}

	if ( joy_fd  == -1 )
	{
		printf( "Couldn't open joystick\n" );
		return -1;
	}

	ioctl( joy_fd, JSIOCGAXES, &num_of_axis );
	ioctl( joy_fd, JSIOCGBUTTONS, &num_of_buttons );
	ioctl( joy_fd, JSIOCGNAME(80), &name_of_joystick );

	axis = (int *) calloc( num_of_axis, sizeof( int ) );
	button = (char *) calloc( num_of_buttons, sizeof( char ) );

	if (pthread_create(&pollThread, NULL, pollInput, &pipefd[1]))
	{
		perror("failed to create pthread");
		return -1;
	}

	printf("Joystick detected: %s\n\t%d axis\n\t%d buttons\n\n"
		, name_of_joystick
		, num_of_axis
		, num_of_buttons );

	//fcntl( joy_fd, F_SETFL, O_NONBLOCK );	/* use non-blocking mode */

	return pipefd[0];
}
