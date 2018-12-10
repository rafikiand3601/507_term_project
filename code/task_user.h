//**************************************************************************************
/** @file task_user.h
 *    This file contains header contents for the user control task.
 *
 *  Revisions:
 *    @li 11-29-2018 KM header for user control task created.
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
#ifndef _TASK_USER_H_
#define _TASK_USER_H_

#include <stdlib.h>                         // Prototype declarations for I/O functions

#include "FreeRTOS.h"                       // Primary header for FreeRTOS
#include "task.h"                           // Header for FreeRTOS task functions
#include "queue.h"                          // FreeRTOS inter-task communication queues

#include "rs232int.h"                       // ME405/507 library for serial comm.
#include "adc.h"                            // Header for A/D converter class driver
#include "time_stamp.h"                     // Class to implement a microsecond timer
#include "taskbase.h"                       // Header for ME405/507 base task class
#include "taskqueue.h"                      // Header of wrapper for FreeRTOS queues
#include "textqueue.h"                      // Header for a "<<" queue class
#include "taskshare.h"                      // Header for thread-safe shared data

#include "shares.h"                         // Global ('extern') queue declarations


/// This macro defines a string that identifies the name and version of this program. 
#define PROGRAM_VERSION		PMS ("ME507 Term Project Steering V0.01")


//-------------------------------------------------------------------------------------


class task_user : public TaskBase
{
private:
	// No private variables or methods for this class

protected:
	// This method displays a simple help message telling the user what to do. It's
	// protected so that only methods of this class or possibly descendents can use it
	void print_help_message (void);

	// This method displays information about the status of the system
	void show_status (void);

public:
	// This constructor creates a user interface task object
	task_user (const char*, unsigned portBASE_TYPE, size_t, emstream*);

	/** This method is called by the RTOS once to run the task loop for ever and ever.
	 */
	void run (void);
};

#endif // _TASK_USER_H_
