/*
 * robot-vrep.c
 *
 *  Created on: 12.03.2014
 *      Author: fengelha,lindhors
 */

#include <stdio.h>
#include "CVmc.h"
#include <opencv2/opencv.hpp>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <vector>
#include "bitmap.h"

#include "robot.h"

#define handle_error_en(en, msg) \
		do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#define handle_error(msg) \
		do { perror(msg); exit(EXIT_FAILURE); } while (0)

using namespace VMC;
using namespace cv;

static CVmc *vmc;
static pthread_t camera;
static pthread_t watchdog;
static VideoCapture* cap;
static int running = 0;
static clock_t lastHeartbeat;
static pthread_mutex_t cammutex = PTHREAD_MUTEX_INITIALIZER;
static Mat frame;

void* watchdogThread(void* arg) {
	while(running) {
		clock_t now = clock();
		double diff = (double)(now - lastHeartbeat)/CLOCKS_PER_SEC;
		if(diff > 1.0) setRobSpeed(0.0f, 0.0f);
	}
	return NULL;
}

void* cameraThread(void* arg) {
	if(!cap->isOpened()) {
		printf("could not open video capture device\n");
		return NULL;
	}
	
	// get image from sensor regularly and put it onto a pipe
	while(running) {
		pthread_mutex_lock(&cammutex);
		if(cap->read(frame)) {
			resize(frame, frame, Size(640, 480));
			//cvtColor(frame, frame, CV_YCrCb2RGB, 3);
			flip(frame, frame, 0);
		}
		else {
			// black image
			frame.create(640, 480, CV_8UC3);
			frame.setTo(Scalar(0,0,0));
		}
		pthread_mutex_unlock(&cammutex);
		waitKey(10);
	}

	return NULL;
}

int initializeRob() {
	vmc = new CVmc("/dev/ttyUSB-VMC");

	if(!vmc->isConnected()) {
		fprintf(stderr, "Couldn't connect to motor controller!\n");
		return -1;
	}
	
	int deviceNum = 0;
	while(!cap && deviceNum < 20) {
		cap = new VideoCapture(deviceNum);
		if(!cap->isOpened()) {
			delete cap;
			cap = NULL;
			deviceNum += 1;
		}
	}
	
	if(deviceNum == 20) {
		printf("could not open video capture device\n");
		exit(1);
	}
	
	frame.create(640, 480, CV_8UC3);
	frame.setTo(Scalar(0,0,0));
	
	cap->set(CV_CAP_PROP_FRAME_WIDTH, 640);
	cap->set(CV_CAP_PROP_FRAME_HEIGHT, 480);
	
	running = 1;
	int ret = pthread_create(&camera, NULL, cameraThread, NULL);
	if(ret < 0) handle_error_en(ret, "cannot initialize camera thread (pthread_create())");
	ret = pthread_create(&watchdog, NULL, watchdogThread, NULL);
	if(ret < 0) handle_error_en(ret, "cannot initialize watchdog thread (pthread_create())");

	return 0;
}

int shutdownRob() {
	delete vmc;
	return 0;
}

int setRobSpeed(float speedLeft, float speedRight) {
	if (!vmc) {
		fprintf(stderr, "You should initialize the robot first\n");
		return -1;
	}
	vmc->setMotors(speedLeft * 100, speedRight * 100);
	sendRobHeartbeat();
	return 0;
}

int isRobRunning() {
	if (!vmc)
		return 0;

	return vmc->isConnected();
}

int getRobCamImage(char** buf, int* size) {
	if(isRobRunning() == -1) {
		fprintf(stderr, "You should initialize the bot first");
		return -1;
	}
	
	static vector<unsigned char> vec;
	
	vec.resize(640*480*3 + sizeof(bitmapheader));
	bitmapheader header;
	generateBMPHeader(&header, 640, 480);
	pthread_mutex_lock(&cammutex);
	copy((unsigned char*)&header, (unsigned char*)&header + sizeof(bitmapheader), vec.begin());
	copy(frame.data, frame.data + 640*480*3, vec.begin() + sizeof(bitmapheader));
	pthread_mutex_unlock(&cammutex);
	
	*size = (int) vec.size();
	*buf = (char*) &vec[0];

	return 0;
}

void sendRobHeartbeat() {
	lastHeartbeat = clock();
}
