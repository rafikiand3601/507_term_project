//**************************************************************************************
/** @file task_motor.h
 *    This file contains header stuff for a user interface task for a ME507/FreeRTOS
 *    test suite. 
 *
 *  Revisions:
 *    @li 11-29-2018 KM Header to 
 *
  */
//**************************************************************************************


// This define prevents this .h file from being included multiple times in a .cpp file
#ifndef _TASK_MOTOR_H_
#define _TASK_MOTOR_H_

#include <stdlib.h>                         // Prototype declarations for I/O functions


// FreeRTOS library includes
#include "FreeRTOS.h"                       // Primary header for FreeRTOS
#include "task.h"                           // Header for FreeRTOS task functions
#include "queue.h"                          // FreeRTOS inter-task communication queues


// ME 507 library includes
#include "rs232int.h"                       // ME405/507 library for serial comm.
#include "adc.h"                            // Header for A/D converter class driver
#include "time_stamp.h"                     // Class to implement a microsecond timer
#include "taskbase.h"                       // Header for ME405/507 base task class
#include "taskqueue.h"                      // Header of wrapper for FreeRTOS queues
#include "textqueue.h"                      // Header for a "<<" queue class
#include "taskshare.h"                      // Header for thread-safe shared data

#include "shares.h"                         // Global ('extern') queue declarations




class task_motor : public TaskBase
{
private:
	// No private variables or methods for this class

protected:
	// protected method which calculates pwm duty cycle from servo angle
	uint8_t calc_pwm (int8_t);

public:
	// This constructor creates a user interface task object
	task_motor (const char*, unsigned portBASE_TYPE, size_t);

	/** This method is called by the RTOS once to run the task loop for ever and ever.
	 */
	void run (void);
};

#endif // _TASK_MOTOR_H_
