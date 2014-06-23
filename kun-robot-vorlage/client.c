#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <poll.h>
#include <time.h>
#include <assert.h>
#include <sys/time.h>

#ifdef USE_OPENCV
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/core/types_c.h>
#endif

#include "bitmap.h"
#include "controller.h"

#define time_diff_ms(now, last) (now.tv_sec*1000+now.tv_usec/1000-last.tv_sec*1000-last.tv_usec/1000)

static int sock;
static char* hostname;
static char* port;

// display an image in a separate window (uses opencv)
#ifdef USE_OPENCV
static void showImageInWindow(char* img, int len) {
	static int windowOpen = 0;
	if(!windowOpen)
		cvNamedWindow( "Robot Camera", CV_WINDOW_AUTOSIZE );
	windowOpen = 1;
	
	CvMat* buf = cvCreateMat(1, len, CV_8UC1);
	buf->data.ptr = (unsigned char*) img;
	
	IplImage* mat = cvDecodeImage(buf, CV_LOAD_IMAGE_COLOR);
	
	cvShowImage("Robot Camera", mat);
	cvWaitKey(1);
	cvReleaseMat(&buf);
}
#else
static void showImageInWindow() {
	// do nothing
}
#endif

// connect to robot and return a socket representing the connection
static int client_connect(char *hostname, char *service)
{
	int sock = 0;
	
	// TODO: connect to hostname and port
	
	return sock;
}

// send drive command to robot
static void set_speed(float l, float r)
{
	int sock = client_connect(hostname, port);
	if(sock <= 0) {
		fprintf(stderr, "could not open socket\n");
		exit(1);
	}

	// TODO: send request
	
	close(sock);
}

static void get_image() {
	int size = 0;
	char* image = malloc(1024); // likely to small, has to be realloc'ed according to actual size

	// TODO: request image and extract it from the server's response
	// the image will be in bmp format, including the file header
	
	// display image
	showImageInWindow(image, size);
	
	close(sock);
	free(image);
}

int main(int argc, char** argv)
{
	int ctrl_fd;
	struct controllerInput input;
	struct pollfd pfd[1] = {{0}};
	struct timeval lastImg, now;
	float speedLeft = 0.0f, speedRight = 0.0f;

	if(argc < 3) {
		printf("usage: %s <target host> <port>\n", argv[0]);
		exit(2);
	}

	hostname = argv[1];
	port = argv[2];

	if((ctrl_fd = initController()) == -1) {
		fprintf(stderr, "Could not initialize controller\n");
		exit(1);

	}
	pfd[0].events = POLLIN;
	pfd[0].fd = ctrl_fd;

	while(1) {
		// read controller input and display images
		gettimeofday(&now, 0);
		int ret = poll(pfd, 1, 100);
		if (ret < 0) {
			perror("errror with poll()");
			break;
		}
		if(ret > 0) {
			// we got controller input
			ret = read(ctrl_fd, &input, sizeof(input));
			if (ret <= 0) {
				perror("errror while reading from keyboard");
				break;
			}
			speedLeft = input.speedLeft;
			speedRight = input.speedRight;
		}

		// request and display image each 1000 ms
		if(time_diff_ms(now, lastImg) > 1000) {
			get_image();
			gettimeofday(&lastImg, 0);
		}

		set_speed(speedLeft, speedRight);
	}

	return 0;
}
