//**************************************************************************************
/** @file task_motor.cpp
 *    This file contains code to drive the ESC for the ME 507 term project.
 *
 *  Revisions:
 *    @li 11-29-2018 KM file created to test ESC.
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



#include <avr/io.h>                         // Port I/O for SFR's
#include <avr/wdt.h>                        // Watchdog timer header

#include "task_motor.h"                     // Header for this file




//-------------------------------------------------------------------------------------
/** This constructor creates a task to control the motor velocity. It is used to
 *  
 *  parent task task base class handles the RTOS functionality.
 *  @param a_name A character string which will be the name of this task
 *  @param a_priority The priority at which this task will initially run (default: 0)
 *  @param a_stack_size The size of this task's stack in bytes
 *                      (default: configMINIMAL_STACK_SIZE)
 */

task_motor::task_motor (const char* a_name,
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
/** This task handles the motor driving
 */

void task_motor::run (void)
{

	offset = -2;

	// This is an infinite loop; it runs until the power is turned off. There is one
	// such loop inside the code for each task
	for (;;)
	{
		// Run the finite state machine. The variable 'state' is kept by parent class
		switch (state)
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			// In state 0, setup the output pin for the motor on PB5(OC1A)
			case (0):
				// Need a wave with period ~ 20 ms and pulse length of 1.0-2.0 ms
				/** f = f_clk / N*(1 + TOP)
				  * Want f = 50 [hz]
				  * TOP = 0xFF = 255, f_clk = 16 [Mhz]
				  * N = 1024 ---> f = 61.3 [hz] close enough
				*/

				// Set PB5 as output pin
				DDRB |= (1 << PB5);
				// Setup register for fast pwm, non-inverting
				// WGM: fast pwm 0x03FF/1023    COM1A1: non-inverting output
				TCCR1A |= (1 << WGM11) | (1 << COM1A1);
				// WGM: fast pwm 0x03FF     CS: prescaler = 256
				TCCR1B |= (1 << WGM12) | (1 << WGM13) | (1 << CS12);
				// TCCR1C unused

				// ICR = 0xFFFF counts to this value
				ICR1H = 0x04;
				ICR1L = 0xE1;

				// Setup of OCR
				// 1.5 [ms] ----> 667 [hz] ---> 94
				// Should run 50.0 [hz] period with 1.5 [ms] pulses
				OCR1AH = 0x00;
				OCR1AL = 0x5E;

				// Move to control state
				state = 1;
				break; // End of state 0

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			// In state 1, control the esc power
			case (1):
				// Vary OCR to change pulse length.
				// Pulses are between 1.0 and 2.0 [ms]

				OCR1AL = calc_pwm(p_motor_vel->get ());

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


//-------------------------------------------------------------------------------------
/** This method takes a single input of the desired motor velocity. From this value,
 *  the timer pulse length that corresponds to this velocity is calculated and returned.
 *  @param pwm An int8_t type variable that represents the desired motor speed.
 *  This value can be between 100 and -100.
 *  @return pulse_length A uint8_t type variable that represents the counter value to 
 *  be written to timer register so that the correct motor speed will be achieved.
 */

uint8_t task_motor::calc_pwm (int8_t pwm)
{
	// Make sure input is between -90 and 90 degrees
	if (pwm < -100)
	{
		pwm = -100;
	} else if (pwm > 100) {
		pwm = 100;
	}

	// Convert degrees to pulse length (64-124)
	return (uint8_t) (offset + 94 + (pwm * 0.3));
}
