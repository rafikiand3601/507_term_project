//**************************************************************************************
/** @file task_radio.cpp
 *    This file contains code to control the transciever for the ME 507 term project. 
 *
 *  Revisions:
 *    @li 12-1-2018 KM file created to operate the transciever.
 *  
 */ 
//**************************************************************************************



#include <avr/io.h>                         // Port I/O for SFR's
#include <avr/wdt.h>                        // Watchdog timer header

#include "task_radio.h"                     // Header for this file






//-------------------------------------------------------------------------------------
/** This constructor creates a new data acquisition task. Its main job is to call the
 *  parent class's constructor which does most of the work.
 *  @param a_name A character string which will be the name of this task
 *  @param a_priority The priority at which this task will initially run (default: 0)
 *  @param a_stack_size The size of this task's stack in bytes 
 *                      (default: configMINIMAL_STACK_SIZE)
 */

task_radio::task_radio (const char* a_name, 
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
/** This task handles sending messages over the transciever to the transponder
 */

void task_radio::run (void)
{

	// This is an infinite loop; it runs until the power is turned off. There is one 
	// such loop inside the code for each task
	for (;;)
	{
		// Run the finite state machine. The variable 'state' is kept by parent class
		switch (state)
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			// State 0, setup radio transciever
			case (0):
				to_address[0] = 0x00;
				to_address[1] = 0x01;
				rf = nRF24L01_init();
				setup_rf(rf);
				
				state = 1;
				break; // End of state 0

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			// Run control actions
			case (1):
				// Wait for ping flag to be set
				if (p_rf_ping->get () == 1)
				{
					p_rf_ping->put (0);
					state = 2;
				}
				break;
				
			case (2):
				memcpy(msg.data, "a", 2);
				nRF24L01_transmit(rf, to_address, &msg);
				state = 1;
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





void task_radio::setup_rf(nRF24L01 *p_rf) {
    p_rf->ss.port = &PORTE;
    p_rf->ss.pin = PE0;
    p_rf->ce.port = &PORTE;
    p_rf->ce.pin = PE1;
    p_rf->sck.port = &PORTB;
    p_rf->sck.pin = PB1;
    p_rf->mosi.port = &PORTB;
    p_rf->mosi.pin = PB2;
    p_rf->miso.port = &PORTB;
    p_rf->miso.pin = PB3;
    nRF24L01_begin(p_rf);
}

