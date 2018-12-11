//**************************************************************************************
/** @file task_USR1.cpp
 *    This file contains source code for an ultrasonic receiver task for a ME405/FreeRTOS
 *    test suite.
 *
 *  Revisions:
 *    @li 11-29-2018 AS created file for distance sensor operation
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

#include "task_USR1.h"                      // Header for this file
#include "shares.h"													//Shared variable header



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
 *  @param p_ser_dev Pointer to a serial device (port, radio, SD card, etc.) which can
 *                   be used by this task to communicate (default: NULL)
 */

task_USR1::task_USR1 (const char* a_name,
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
/** @brief This method is called to run the Ultrasonic distance sensor task.
 *  @details This function works within the FreeRTOS framework. Once it is called,
 *  it initializes necessary pins, and sets up input capture interrupts to measure
 *  the ECHO pulse from the sensor.It also cotinually toggles the trigger pin
 *  such that the sensor continually sends pulses for object detection.
 */

 // This is an infinite loop; it runs until the power is turned off. There is one
 // such loop inside the code for each task
void task_USR1::run (void)
{

	for (;;)
	{
		// Run the finite state machine. The variable 'state' is kept by parent class
		switch (state)
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			// In state 0, setup the output pin PC1, input capture interrupts on PE7
			case (0):
					//Variable for trigger pin, only needed for demo code
					ECHO = 0;
					//Input capture initialization for ECHO pin
					//Configure PE7 as input
					DDRE &= (1 << PE7);
					DDRE = 0x00;
					TCCR3B = 0x00;
					//Set to rising edge capture, 1024 prescaler set
					TCCR3B |= (1 << ICES3) | (1<<CS32);
					//Enable interrupts
					TIMSK3 |= (1 << ICIE3);
					//Clear input capture flag by writing a one
					TIFR3 = (1 << ICF3);

					edge_1->put (1);
					width_1->put (0);

					//Initialize Trigger pin
					//Configure as output
					DDRC |= (1 << PC1);
					//Set Trigger pin high
					PORTC |= (1 << PC1);
					//Go to state 1
					transition_to (1);
					break;
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			// In state 1, toggle distance sensor trigger pin to send pulses
			case (1):
//Test Code
//					if(width_1->get())
//					{
//						*p_serial <<width_1->get()<< endl; //Test code
//						PORTC &= ~(1 << PC1);		//Set PC1 low
//						width_1->put(0);			//Set width_1 to 0
//					}
					if (ECHO)
					{
						//Set PC1 low
						PORTC &= ~(1 << PC1);
						ECHO = 0;
					}
					else
					{
						//*p_serial <<width_1->get()<< endl;
						//Set PC1 high
						PORTC |= (1 << PC1);
						ECHO = 1;
					}

					break;
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			// We should never get to the default state. If we do, complain and restart
			default:
					*p_serial << PMS ("Illegal state! Resetting AVR") << endl;
					wdt_enable (WDTO_120MS);
					for (;;) ;
					break;
		};
		runs++;                             // Increment counter for debugging

		// No matter the state, wait for approximately 50 milliseconds before we
		// run the loop again. This gives lower priority tasks a chance to run
		delay_ms (50);
 	}
}
