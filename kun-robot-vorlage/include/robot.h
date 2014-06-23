/**
 * @file robot.h 
 * @brief
 * Robot interface functions for usage in the server.c file.
 * 
 * Use these functions to operate the robot. The robot can either be
 * a 'real' robot (VolksBot Indoor), or a simulated BubbleRob.
 *
 *  Created on: 12.03.2014
 *      Author: fengelha, lindhors
 */

#ifndef ROBOT_H_
#define ROBOT_H_


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the robot.
 *
 * Call this function before you do anything else with the robot.
 *
 * @return 0 on success
 */
int initializeRob();

/**
 * @brief Shutdown the robot.
 *
 * @return 0 on success
 */
int shutdownRob();

/**
 * @brief Set speed for left and right wheel in meter per seconds. Max speed is 1 m/s.
 *
 * @return 0 on success
 */
int setRobSpeed(float speedLeft, float speedRight);

/**
 * @brief Test if robot was initialized.
 *
 * @return 1 if running, 0 if not
 */
int isRobRunning();

/**
 * @brief Read a single image from the robot's camera.
 *
 * @param buf Pointer to a char*, which will return the pointer to a buffer containing the image data
 * as windows bmp image. You may write this data immediately to a file. The pointer to the buffer is
 * valid until the next call to getRobCamImage().
 * @param size Pointer to an integer, where the actual size in bytes of the image is returned.
 * @return 0 on success
 */
int getRobCamImage(char** buf, int* size);

/**
 * @brief Send an alive-signal to the robot.
 * 
 * The robot will stop, if it does not receive either a drive command or a heartbeat every second.
 * Assure to call either this function or setRobSpeed() regularly.
 */
void sendRobHeartbeat();

#ifdef __cplusplus
}
#endif

#endif /* ROBOT_H_ */
