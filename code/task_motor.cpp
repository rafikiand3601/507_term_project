//**************************************************************************************
/** @file task_motor.cpp
 *    This file contains code to drive the ESC for the ME 507 term project. 
 *
 *  Revisions:
 *    @li 11-29-2018 KM file created to test ESC.
 * 
 */ 
//**************************************************************************************



#include <avr/io.h>                         // Port I/O for SFR's
#include <avr/wdt.h>                        // Watchdog timer header

#include "task_motor.h"                     // Header for this file


/** This constant sets how many RTOS ticks the task delays if the user's not talking.
 *  The duration is calculated to be about 5 ms.
 */
const TickType_t ticks_to_delay = ((configTICK_RATE_HZ / 1000) * 5);


//-------------------------------------------------------------------------------------
/** This constructor creates a new data acquisition task. Its main job is to call the
 *  parent class's constructor which does most of the work.
 *  @param a_name A character string which will be the name of this task
 *  @param a_priority The priority at which this task will initially run (default: 0)
 *  @param a_stack_size The size of this task's stack in bytes 
 *                      (default: configMINIMAL_STACK_SIZE)
 */

task_motor::task_motor (const char* a_name, 
					  unsigned portBASE_TYPE a_priority, 
					  size_t a_stack_size
					 )
	: TaskBase (a_name, a_priority, a_stack_size)
{
	// Nothing is done in the body of this constructor. All the work is done in the
	// call to the frt_task constructor on the line just above this one
}


//-------------------------------------------------------------------------------------
/** This task handles the motor driving
 */

void task_motor::run (void)
{



	// This is an infinite loop; it runs until the power is turned off. There is one 
	// such loop inside the code for each task
	for (;;)
	{
		// Run the finite state machine. The variable 'state' is kept by parent class
		switch (state)
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			// In state 0, setup the output pin for the servo on PB4(OC2A)
			case (0):
				// Need a wave with period ~ 20 ms and pulse length of 1.0-2.0 ms
				/** f = f_clk / N*(1 + TOP)
				  * Want f = 50 [hz]
				  * TOP = 0xFF = 255, f_clk = 16 [Mhz]
				  * N = 1024 ---> f = 61.3 [hz] close enough
				*/
				
				// Set PB4 as output pin
				DDRB |= (1 << PB4);
				// Setup timer register for fast pwm, non-inverting
				// WGM: fast pwm     COM: non-inverting output
				TCCR2A |= (1 << WGM20) | (1 << WGM21) | (1 << COM2A1);
				// WGM: fast pwm     CS: prescaler = 1024
				TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);

				// Setup of OCR
				// 1.5 [ms] ----> 667 [hz] ---> 24
				// Should run 61.3 [hz] period with 1.5 [ms] pulses
				OCR2A = 24;
				state = 1;
				break; // End of state 0

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			// In state 1, control the esc power
			case (1):
				// Vary OCR to change pulse length.
				// Pulses are between 1.0 and 2.0 [ms]
				OCR2A = calc_pwm(0);
				break; // End of state 1

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



uint8_t task_motor::calc_pwm (int8_t pwm)
{
	// Make sure input is between -90 and 90 degrees
	if (pwm < -90)
	{
		pwm = -90;
	} else if (pwm > 90) {
		pwm = 90;
	}
	
	// Convert degrees to pulse length (16-31)
	return (uint8_t) (24 + (pwm * 0.077));
}