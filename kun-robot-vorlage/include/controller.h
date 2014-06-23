/*
 * =====================================================================================
 *
 *       Filename:  controller.h
 *
 *    Description:  generic API for a manual robot controller
 *
 *         Author:  Timo Lindhorst <lindhors@ivs.cs.ovgu.de>
 *
 * =====================================================================================
 */

/**
 * @file controller.h 
 * 
 * @brief controller interface for client.c
 * 
 */

#ifndef __controller_h__
#define __controller_h__

/**
 * A structure to hold the steering commands issued by the controller.
 * 
 * After calling initController(), control commands obtained by the 
 * returned file descriptor will have this format.
 */
struct controllerInput {
	float speedLeft;	///< speed of left wheel in interval [-1,1]
	float speedRight;	///< speed of right wheel in interval [-1,1]
};

/**
 * @brief Initialize the controller.
 * 
 * The function initializes the keyboard control, such that keyboard commands
 * (arrow keys for steering, space bar for stopping) can be read from the 
 * returned file descriptor. @see controllerInput
 *
 * @return A file descriptor is returned from which the control commands can be
 * 	read or -1 in case of error.
 */
int initController();

#endif // __controller_h__
