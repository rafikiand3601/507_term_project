//**************************************************************************************
/** @file task_steering.cpp
 *    This file contains code to drive the steering servo for our ME 507 term project.
 * 
 *
 *  Revisions:
 *    @li 11-29-2018 KM file created to test steering servo.
 *    @li 12-4-2018 KM last planned edit.
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

#include "task_steering.h"                      // Header for this file




//-------------------------------------------------------------------------------------
/** This constructor creates a new data acquisition task. Its main job is to call the
 *  parent class's constructor which does most of the work.
 *  @param a_name A character string which will be the name of this task
 *  @param a_priority The priority at which this task will initially run (default: 0)
 *  @param a_stack_size The size of this task's stack in bytes 
 *                      (default: configMINIMAL_STACK_SIZE)
 */

task_steering::task_steering (const char* a_name, 
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

void task_steering::run (void)
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
			// In state 1, control the servo position
			case (1):
				// Vary OCR to change pulse length.
				// Pulses are between 1.0 and 2.0 [ms]
				OCR2A = calc_pwm(p_servo_pos->get ());
				
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
/** This method takes a single input of the desired servo angle. From this value,
 *  the timer pulse length that corresponds to this angle is calculated and returned.
 *  @param pwm An int8_t type variable that represents the desired servo angle.
 *  This value can be between 90 and -90.
 *  @return pulse_length A uint8_t type variable that represents the counter value to 
 *  be written to timer register so that the correct motor speed will be achieved.
 */

uint8_t task_steering::calc_pwm (int8_t pwm)
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