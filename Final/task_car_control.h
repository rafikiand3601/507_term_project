//**************************************************************************************
/** @file task_car_control.h
 *    This file contains header contents for the car control task.
 *
 *  Revisions:
 *    @li 11-29-2018 KM car control header created.
 *    @li 12-9-2018 KM last planned edit.
 *
 *  License:
 *	This code is based on Prof. JR Ridgely's FreeRTOS CPP example code. The FreeRTOS
 *	framework is used, but the tasks are a product of our 507 group. Since the original
 *	code used the LGPL, our code will also use the LGPL.
 *		THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *		AND	ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * 		IMPLIED 	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * 		ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * 		LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUEN-
 * 		TIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * 		OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * 		CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * 		OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * 		OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */
//**************************************************************************************


// This define prevents this .h file from being included multiple times in a .cpp file
#ifndef _TASK_CAR_CONTROL_H_
#define _TASK_CAR_CONTROL_H_

#include <stdlib.h>                         // Prototype declarations for I/O functions


// FreeRTOS library includes
#include "FreeRTOS.h"                       // Primary header for FreeRTOS
#include "task.h"                           // Header for FreeRTOS task functions
#include "queue.h"                          // FreeRTOS inter-task communication queues


// ME 507 library includes
#include "rs232int.h"                       // ME405/507 library for serial comm.
#include "time_stamp.h"                     // Class to implement a microsecond timer
#include "taskbase.h"                       // Header for ME405/507 base task class
#include "taskqueue.h"                      // Header of wrapper for FreeRTOS queues
#include "textqueue.h"                      // Header for a "<<" queue class
#include "taskshare.h"                      // Header for thread-safe shared data

#include "shares.h"                         // Global ('extern') queue declarations



/** @brief This task is used to control movement of the car.
 *  @details This task inherits the TaskBase class, and is used to run as a finite 
 *   state machine. It controls the actions of the motor and servo using shared
 *   variables.
 */
class task_car_control : public TaskBase
{
private:
	// No private variables or methods for this class

protected:
	// protected method which calculates pwm duty cycle from servo angle

public:
	// This constructor creates a user interface task object
	task_car_control (const char*, unsigned portBASE_TYPE, size_t, emstream*);

	/** This method is called by the RTOS once to run the task loop for ever and ever.
	 */
	void run (void);
};

#endif // _TASK_CAR_CONTROL_H_
