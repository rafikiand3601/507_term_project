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
				//setup_rf();
				
				init_spi();
				*p_serial << get_reg(STATUS) << endl;
				
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
				// Send ping signal
				*p_serial << get_reg(STATUS) << endl;
				
				uint8_t val [5];
				val[0] = 0x01;
				// Enable data pipe 0
				//write_to_nrf (W, EN_RXADDR, val, 1);
				
				
				state = 1;
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


/*
void task_radio::setup_rf(void) {
    p_rf->ss.port = &PORTE;
    p_rf->ss.pin = PE2;
    p_rf->ce.port = &PORTE;
    p_rf->ce.pin = PE3;
    p_rf->sck.port = &PORTB;
    p_rf->sck.pin = PB1;
    p_rf->mosi.port = &PORTB;
    p_rf->mosi.pin = PB2;
    p_rf->miso.port = &PORTB;
    p_rf->miso.pin = PB3;
    nRF24L01_begin(p_rf);
}
*/




//----------------------------------------------------------------------------------

void task_radio::init_spi (void)
{
	// Set MOSI(PB2) and SCK(PB1) output, MISO(PB3) input
	DDRB |= (1 << PB2) | (1 << PB1);
	DDRB &= ~(1 << PB3);
	// Set CSN(PE2) as output
	DDRE |= (1 << PE2);
	// Enable SPI as master
	SPCR |= (1 << SPE) | (1 << MSTR);
	
	// CSN high to start with
	PORTE |= (1 << PE2);
	// CE low to start with
	PORTE |= (1 << PE3);
}


char task_radio::write_byte (char c_data)
{
	// Load byte to data register
	SPDR = c_data;
	
	// Wait for transmit to finish
	while(!(SPSR & (1 << SPIF)))
	{
	};
	
	return SPDR;
}

uint8_t task_radio::get_reg (uint8_t reg)
{
	_delay_us (10);
	// Set CSN low
	PORTE &= ~(1 << PE2);
	_delay_us (10);
	// Set nRF to reading mode
	write_byte (R_REGISTER + reg);
	_delay_us (10);
	reg = write_byte(NOP);
	_delay_us (10);
	// Set CSN high
	PORTE |= (1 << PE2);
	return reg;
}

void task_radio::write_nrf (uint8_t reg, uint8_t package)
{
	_delay_us(10);
	// Set CSN low
	PORTE &= ~(1 << PE2);
	_delay_us(10);
	// Set nRF to reading mode
	write_byte (W_REGISTER + reg);
	_delay_us(10);
	write_byte (package);
	_delay_us(10);
	// Set CSN high
	PORTE |= (1 << PE2);
}

uint8_t *task_radio::read_or_write (uint8_t ReadWrite, uint8_t reg, uint8_t *val, uint8_t antVal)
{
	if (ReadWrite == W)
	{
		reg = W_REGISTER + reg;
	}
	
	static uint8_t ret [32];
	
	_delay_us (10);
	// Set CSN low
	PORTE &= ~(1 << PE2);
	_delay_us (10);
	write_byte (reg);
	_delay_us (10);
	
	int i;
	for(i = 0; i < antVal; i++)
	{
		if (ReadWrite == R && reg != W_TX_PAYLOAD)
		{
			ret [i] = write_byte (NOP);
			_delay_us (10);
		}
		else
		{
			write_byte (val [i]);
			_delay_us (10);
		}
	}
	
	// Set CSN high
	PORTE |= (1 << PE2);
	return ret;
}
