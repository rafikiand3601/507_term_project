//**************************************************************************************
/** @file task_USR1.cpp
 *    This file contains source code for an ultrasonic receiver task for a ME405/FreeRTOS
 *    test suite.
 *
 *  Revisions:
 *    @li 09-30-2012 JRR Original file was a one-file demonstration with two tasks
 *    @li 10-05-2012 JRR Split into multiple files, one for each task
 *    @li 10-25-2012 JRR Changed to a more fully C++ version with class task_user
 *    @li 11-04-2012 JRR Modified from the data acquisition example to the test suite
 *    @li 01-04-2014 JRR Changed base class names to TaskBase, TaskShare, etc.
 *
 *  License:
 *    This file is copyright 2012 by JR Ridgely and released under the Lesser GNU
 *    Public License, version 2. It intended for educational use only, but its use
 *    is not limited thereto. */
/*    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 *    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUEN-
 *    TIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 *    OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */
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
/** This task interacts with the user for force him/her to do what he/she is told. It
 *  is just following the modern government model of "This is the land of the free...
 *  free to do exactly what you're told."
 */

void task_USR1::run (void)
{

	for (;;)
	{
		switch (state)
		{
			//State 0
			case (0):
					//DDRG |= (1 << PG5);				//Configure port pin as output
					//TCCR0 |= ((1 << WGM02) | (1 << CS02) | (1 << CS01) | (1 << CS00))
					//DDRC &= (1 << PC0);				//Configure port pin as input
					ECHO = 0;			//Test Code
					//Input capture initialization for ECHO pin
					DDRE &= (1 << PE7);					//Configure as input
					DDRE = 0x00;	//test code
					TCCR3B = 0x00; //test code
					TCCR3B |= (1 << ICES3) | (1<<CS32);	//Set to edge capture, 1024 prescaler set
					TIMSK3 |= (1 << ICIE3);
					TIFR3 = (1 << ICF3);

					edge_1->put (1);			//Initialize
					width_1->put (0);			//Initialize

					//Initialize Trigger pin
					DDRC |= (1 << PC1);		//Configure as output
					PORTC |= (1 << PC1);	//Set Trigger pin high
					transition_to (1);		//Go to state 1
					break;
			case (1):
//Do nothing
//Test Code
//					if(width_1->get())
//					{
//						*p_serial <<width_1->get()<< endl; //Test code
//						PORTC &= ~(1 << PC1);		//Set PC1 low (test code)
//						width_1->put(0);			//Set width_1 to 0 (test code)
//					}
					if (ECHO)
					{
						PORTC &= ~(1 << PC1);		//Set PC1 low (test code)
						ECHO = 0;
					}
					else
					{
						*p_serial <<width_1->get()<< endl;
						PORTC |= (1 << PC1);		//Set PC1 high (test code)
						ECHO = 1;
						//delay_ms(50);
					}

					break;

			default:
					*p_serial << PMS ("Illegal state! Resetting AVR") << endl;
					wdt_enable (WDTO_120MS);
					for (;;) ;
					break;
		};
		runs++;                             // Increment counter for debugging

		// No matter the state, wait for approximately a millisecond before we
		// run the loop again. This gives lower priority tasks a chance to run
		delay_ms (50);
 	}
}
