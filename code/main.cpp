//*************************************************************************************
/** \file main.cpp
 *    This file contains the main() code for a program which runs the ME405 board for
 *    ME405 lab 1. This program currently uses obfuscated code to use an A/D converter
 *    to convert an analog signal into an LED brightness via pulse width modulation.
 *    \b This \b comment \b is \b quite \b messed \b up \b and \b the \b student
 *    \b should \b fix \b it \b so \b that \b it \b accurately \b reflects \b the
 *    \b function \b of \b the \b program \b which \b is \b handed \b in \b for \b this
 *    \b assignment.
 *
 *  Revisions:
 *    \li 09-30-2012 JRR Original file was a one-file demonstration with two tasks
 *    \li 10-05-2012 JRR Split into multiple files, one for each task plus a main one
 *    \li 10-30-2012 JRR A hopefully somewhat stable version with global queue
 *                       pointers and the new operator used for most memory allocation
 *    \li 11-04-2012 JRR FreeRTOS Swoop demo program changed to a sweet test suite
 *    \li 01-05-2012 JRR Program reconfigured as ME405 Lab 1 starting point
 *    \li 03-28-2014 JRR Pointers to shared variables and queues changed to references
 *    @li 01-04-2015 JRR Names of share & queue classes changed; allocated with new now
 *
 *  License:
 *		This file is copyright 2015 by JR Ridgely and released under the Lesser GNU
 *		Public License, version 2. It intended for educational use only, but its use
 *		is not limited thereto. */
/*		THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *		AND	ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * 		IMPLIED 	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * 		ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * 		LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUEN-
 * 		TIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * 		OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * 		CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * 		OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * 		OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */
//*************************************************************************************


// Basic C/AVR library includes
#include <stdlib.h>                         // Prototype declarations for I/O functions
#include <avr/io.h>                         // Port I/O for SFR's
#include <avr/wdt.h>                        // Watchdog timer header
#include <string.h>                         // Functions for C string handling
#include <avr/interrupt.h>

// FreeRTOS includes
#include "FreeRTOS.h"                       // Primary header for FreeRTOS
#include "task.h"                           // Header for FreeRTOS task functions
#include "queue.h"                          // FreeRTOS inter-task communication queues
#include "croutine.h"                       // Header for co-routines and such

// ME 507 library includes
#include "rs232int.h"                       // ME405/507 library for serial comm.
#include "time_stamp.h"                     // Class to implement a microsecond timer
#include "taskbase.h"                       // Header of wrapper for FreeRTOS tasks
#include "textqueue.h"                      // Wrapper for FreeRTOS character queues
#include "taskqueue.h"                      // Header of wrapper for FreeRTOS queues
#include "taskshare.h"                      // Header for thread-safe shared data
#include "shares.h"                         // Global ('extern') queue declarations

// task includes
#include "task_user.h"                      // Header for user interface task
#include "task_steering.h"                  // Header for steering task
#include "task_motor.h"                     // Header for motor task
#include "task_car_control.h"               // Header for car control task
#include "task_radio.h"                     // Header for car control task
#include "task_USR1.h"		                // Header for ultra sonic receiver task


// Declare the queues which are used by tasks to communicate with each other here.
// Each queue must also be declared 'extern' in a header file which will be read
// by every task that needs to use that queue. The format for all queues except
// the serial text printing queue is 'frt_queue<type> name (size)', where 'type'
// is the type of data in the queue and 'size' is the number of items (not neces-
// sarily bytes) which the queue can hold

/** This is a print queue, descended from \c emstream so that things can be printed
 *  into the queue using the "<<" operator and they'll come out the other end as a
 *  stream of characters. It's used by tasks that send things to the user interface
 *  task to be printed.
 */
TextQueue* p_print_ser_queue;
TaskShare<int8_t>* p_servo_pos;
TaskShare<int8_t>* p_motor_vel;
TaskShare<int8_t>* p_enc_read;
TaskShare<bool>* p_rf_ping;
TaskShare<uint8_t>* p_drive_state;
TaskShare<int8_t>* edge_1;
TaskShare<uint16_t>* width_1;
//=====================================================================================
/** The main function sets up the RTOS.  Some test tasks are created. Then the
 *  scheduler is started up; the scheduler runs until power is turned off or there's a
 *  reset.
 *  @return This is a real-time microcontroller program which doesn't return. Ever.
 */

int main (void)
{
	// Disable the watchdog timer unless it's needed later. This is important because
	// sometimes the watchdog timer may have been left on...and it tends to stay on
	MCUSR = 0;
	wdt_disable ();

	// Configure a serial port which can be used by a task to print debugging infor-
	// mation, or to allow user interaction, or for whatever use is appropriate.  The
	// serial port will be used by the user interface task after setup is complete and
	// the task scheduler has been started by the function vTaskStartScheduler()
	rs232* p_ser_port = new rs232 (9600, 1);
	*p_ser_port << clrscr << PMS ("ME405 Lab 1 Starting Program") << endl;

	// Create the queues and other shared data items here
	p_print_ser_queue = new TextQueue (32, "Print", p_ser_port, 10);

	// Create the shared servo position object (-90 degrees to 90 degrees)
	p_servo_pos = new TaskShare<int8_t> ("Servo_Pos");

	// Create the shared servo position object (-100 to 100)
	p_motor_vel = new TaskShare<int8_t> ("Motor_Vel");

	// Create the shared encoder reading variable
	p_enc_read = new TaskShare<int8_t> ("Encoder_read");

	// Create the shared ping flag variable
	p_rf_ping = new TaskShare<bool> ("Ping_Flag");

	// Create the shared drive flag variable
	p_drive_state = new TaskShare<uint8_t> ("Drive_State");
	p_drive_state->put (0);

	//Create shared edge1 flag
	edge_1 = new TaskShare<int8_t> ("Edge1");

	//Create shared USR1 pulse width
	width_1 = new TaskShare<uint16_t> ("Width1");

	// The user interface is at low priority; it could have been run in the idle task
	// but it is desired to exercise the RTOS more thoroughly in this test program
	new task_user ("UserInt", task_priority (1), 260, p_ser_port);

	// Create a Task to control the steering of the car
	new task_steering ("Steering", task_priority (5), 200, p_ser_port);

	// Create a Task to control the motor
	new task_motor ("Motor", task_priority (8), 200, p_ser_port);

	// Create a Task to control the RF transceiver
	//new task_radio ("RF", task_priority (6), 200, p_ser_port);

	//Create a Task to coordinate the other tasks
	new task_car_control ("CarControl",task_priority (2), 200, p_ser_port);

	//Create a Task to read ultrasonic receiver 1
	new task_USR1 ("USR1",task_priority (7), 200, p_ser_port);
	//Create a Task to read ultrasonic receiver 2
	//new task_USR2 ("USR2",task_priority (7), 200, p_ser_port);

	//Create a Task to read ultrasonic distance sensor
	//new task_USD ("USD",task_priority (3), 200, p_ser_port);

	//Create a Task to read motor hall efect sensor
	//new task_HallEffect ("HallEffect",task_priority (9), 200, p_ser_port);

	// Here's where the RTOS scheduler is started up. It should never exit as long as
	// power is on and the microcontroller isn't rebooted
	sei(); // interrupts on
	vTaskStartScheduler ();

	return 1;
}

ISR(TIMER3_CAPT_vect)
{
    uint16_t count1 = TCNT3;
		//width_1->ISR_put(1);	//store value of pulse width
    if (edge_1->ISR_get ())	// rising edge
    {
	// set Edge Sense set to falling edge, clear counter
        TCCR3B &= ~(1 << ICES3);
        TCNT3 = 0;
				edge_1->ISR_put(0);		//Toggle edge_1 to 0
    }
    else        // falling edge
    {
        // set Edge Sense set to rising edge
        TCCR3B |= (1 << ICES3);
	    	width_1->ISR_put(count1);	//store value of pulse width
				edge_1->ISR_put(1);		//Toggle edge_1 to 1
    }
}
