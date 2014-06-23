/*
 * robot-vrep.c
 *
 *  Created on: 12.03.2014
 *      Author: fengelha
 */

#include "robot.h"
#include "bitmap.h"

#include "extApi.h"
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

#define handle_error_en(en, msg) \
		do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#define handle_error(msg) \
		do { perror(msg); exit(EXIT_FAILURE); } while (0)


// simRob interface
//-----------------------------------------------------------------------------------------------

const int VREP_PORT=20496;
const char* VREP_LEFT_MOTOR_NAME="leftMotor#";
const char* VREP_RIGHT_MOTOR_NAME="rightMotor#";
const char* VREP_SENSING_NOSE_NAME="sensingNose#";
const char* VREP_VISION_SENSOR_NAME="Vision_sensor#";

typedef struct simRobot_ {
	int leftMotorHandle;
	int rightMotorHandle;
	int sensorHandle;
	int visionSensorHandle;
	int clientID;
	int running;
	clock_t lastHeartbeat;
	pthread_t threadid;
} simRobot;

simRobot rob = { .clientID = -1 };

void* watchdogThread(void* arg) {
	while(rob.running) {
		clock_t now = clock();
		double diff = (double)(now - rob.lastHeartbeat)/CLOCKS_PER_SEC;
		if(diff > 0.5) setRobSpeed(0.0f, 0.0f);
	}
	return NULL;
}

int initializeRob() {
	int ret;
	rob.clientID=simxStart("127.0.0.1",VREP_PORT,1,1,2000,5);
	if (rob.clientID!=-1)
	{
		// initialize struct
		rob.leftMotorHandle=0;
		rob.rightMotorHandle=0;
		rob.sensorHandle=0;
		rob.visionSensorHandle=0;
		
		// get sensor/actuator handles
		simxGetObjectHandle(rob.clientID, VREP_LEFT_MOTOR_NAME, &rob.leftMotorHandle, simx_opmode_oneshot_wait);
		simxGetObjectHandle(rob.clientID, VREP_RIGHT_MOTOR_NAME, &rob.rightMotorHandle, simx_opmode_oneshot_wait);
		simxGetObjectHandle(rob.clientID, VREP_SENSING_NOSE_NAME, &rob.sensorHandle, simx_opmode_oneshot_wait);
		
		ret = pthread_create(&rob.threadid, NULL, watchdogThread, NULL);
		if(ret < 0) handle_error_en(ret, "cannot initialize watchdog thread (pthread_create())");
		rob.running = 1;

		return 0;
	}
	else {
		fprintf(stderr,"Could not connect to V-REP simulation. Is there a simulation running?\n");
	}
	return -1;
}

int getRobCamImage(char** buf, int* size) {
	static char* img;
	simxInt resolution[2];
	simxUChar* data;
	int cnt;

	if(isRobRunning() == -1) {
		fprintf(stderr, "You should initialize the bot first");
		return -1;
	}

	simxGetObjectHandle(rob.clientID, VREP_VISION_SENSOR_NAME, &rob.visionSensorHandle, simx_opmode_oneshot_wait);

	if(simxGetVisionSensorImage(rob.clientID, rob.visionSensorHandle, resolution, &data, 0, simx_opmode_oneshot_wait)
		!= simx_error_noerror) {
		rob.running=0;
		fprintf(stderr, "Error when fetching image from robot (simxGetVisionSensorImage())\n");
		return -1;
	}

	bitmapheader header;
	generateBMPHeader(&header, resolution[0], resolution[1]);

	cnt = resolution[0]*resolution[1]*3 + sizeof(bitmapheader);
	img = realloc(img, cnt);
	if(img == NULL) {
		fprintf(stderr, "Out of memory\n");
		return -1;
	}

	memcpy(img, &header, sizeof(bitmapheader));
	memcpy(img + sizeof(bitmapheader), data, resolution[0]*resolution[1]*3);

	*buf = img;
	*size = cnt;
	return 0;
}

int shutdownRob() {
	simxFinish(rob.clientID);
	return 0;
}

int setRobSpeed(float speedLeft, float speedRight) {
	simxSetJointTargetVelocity(rob.clientID,rob.leftMotorHandle,speedLeft,simx_opmode_oneshot);
	simxSetJointTargetVelocity(rob.clientID,rob.rightMotorHandle,speedRight,simx_opmode_oneshot);
	sendRobHeartbeat();
	return 0;
}

int isRobRunning() {
	simxGetConnectionId(rob.clientID);
	return rob.clientID;
}

void sendRobHeartbeat() {
	rob.lastHeartbeat = clock();
}
