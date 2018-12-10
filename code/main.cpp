//*************************************************************************************
/** \file main.cpp
 *    This file contains the main() code for a multi-task program run under the
 *    FreeRTOS framework. 
 *    
 *
 *  Revisions:
 *    \li 11-29-2018 KM file created to setup the task state-machine.
 *    \li 12-4-2018 KM added all tasks to state machine.
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

// Task includes
#include "task_user.h"                      // Header for user interface task
#include "task_steering.h"                  // Header for steering task
#include "task_motor.h"                     // Header for motor task
#include "task_car_control.h"               // Header for car control task
#include "task_radio.h"                     // Header for car control task
#include "task_USR1.h"		                // Header for ultra sonic receiver task





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
	new task_radio ("RF", task_priority (6), 200, p_ser_port);

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
