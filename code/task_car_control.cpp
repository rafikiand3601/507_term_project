//**************************************************************************************
/** @file task_car_control.cpp
 *    This file contains code to drive the steering servo for the ME 507 term project.
 *
 *  Revisions:
 *    @li 11-29-2018 KM file created to test steering servo.
 *
 */
//**************************************************************************************



#include <avr/io.h>                         // Port I/O for SFR's
#include <avr/wdt.h>                        // Watchdog timer header

#include "task_car_control.h"                      // Header for this file




//-------------------------------------------------------------------------------------
/** This constructor creates a new data acquisition task. Its main job is to call the
 *  parent class's constructor which does most of the work.
 *  @param a_name A character string which will be the name of this task
 *  @param a_priority The priority at which this task will initially run (default: 0)
 *  @param a_stack_size The size of this task's stack in bytes
 *                      (default: configMINIMAL_STACK_SIZE)
 */

task_car_control::task_car_control (const char* a_name,
					  unsigned portBASE_TYPE a_priority,
					  size_t a_stack_size,
					  emstream* p_ser_dev
					 )
	: TaskBase (a_name, a_priority, a_stack_size, p_ser_dev)
{
	// Nothing is done in the body of this constructor. All the work is done in the
	// call to the frt_task constructor on the line just above this one
}


//-------------------------------------------------------------------------------------
/** This task handles the steering
 */

void task_car_control::run (void)
{

	// This is an infinite loop; it runs until the power is turned off. There is one
	// such loop inside the code for each task
	for (;;)
	{
		// Run the finite state machine. The variable 'state' is kept by parent class
		switch (state)
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			// State 0
			case (0):

				// Set motor and servo to initial positions
				p_motor_vel->put (0);
				p_servo_pos->put (0);

				state = 2;
				break; // End of state 0

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			// Run control actions
			case (1):
				if ((p_drive_state->get ()) == 0)
				{
					state = 2;
				}
				p_motor_vel->put (25);
				p_servo_pos->put (0);
				//*p_serial <<width_1->get () << endl;

				break; // End of state 1

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			// Stop car from running
			case (2):
				if ((p_drive_state->get ()) == 0x01)
				{
					state = 1;
				}
				else if ((p_drive_state->get ()) == 0x02)
				{
					state = 3;
				}

				p_motor_vel->put (0);
				p_servo_pos->put (90);

				break; // End of state 2

			case (3):
				if ((p_drive_state->get ()) == 0)
				{
					state = 2;
				}

				else if ((width_1->get ()) < 150)
				{
					p_motor_vel->put (0);
					p_servo_pos->put (90);
					//*p_serial <<width_1->get () << endl;

				}

				else if ((width_1->get ()) > 150)
				{
					p_motor_vel->put (0);
					p_servo_pos->put (0);
				//	*p_serial <<width_1->get () << endl;
				}
				//*p_serial <<'1'<< endl;
				break;
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			// We should never get to the default state. If we do, complain and restart
			default:
				*p_serial << PMS ("Illegal state! Resetting AVR") << endl;
				wdt_enable (WDTO_120MS);
				for (;;) ;
				break;

		} // End switch state

		runs++;                             // Increment counter for debugging

		// No matter the state, wait for approximately a millisecond before we
		// run the loop again. This gives lower priority tasks a chance to run
		delay_ms (1);
	}
}
