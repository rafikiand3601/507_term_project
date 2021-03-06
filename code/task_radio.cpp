//**************************************************************************************
/** @file task_radio.cpp
 *    This file contains code to control the transciever for the ME 507 term project. 
 *
 *  Revisions:
 *    @li 12-1-2018 KM file created to operate the transciever.
 *    @li 12-9-2018 KM last planned edit.
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


#include "task_radio.h"                     // Header for this file






//-------------------------------------------------------------------------------------
/** This constructor creates a new data acquisition task. Its main job is to call the
 *  parent class's constructor which does most of the work.
 *  @param a_name A character string which will be the name of this task
 *  @param a_priority The priority at which this task will initially run (default: 0)
 *  @param a_stack_size The size of this task's stack in bytes 
 *                      (default: configMINIMAL_STACK_SIZE)
 *  @param p_ser_dev A pointer to the serial port which writes debugging info. 
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
				
				init_spi ();
				_delay_us (10);
				*p_serial << get_reg(STATUS) << endl;
				//write_byte (R_REGISTER + STATUS);
				
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
				
				uint8_t name [5];
				name[0] = 'n';
				name[1] = 'o';
				name[2] = 'd';
				name[3] = 'e';
				name[4] = '1';
				//int i;
				// Set up transmitter address
				read_or_write (W, TX_ADDR, name, 5);
				
				// Set as transmitter, and power on
				uint8_t val [2];
				val[0] = 0b00000010;
				read_or_write (W, CONFIG, val, 1);
				
				_delay_ms(2);
				
				// Send message
				uint8_t data[2];
				data[0] = 0x00;
				data[1] = 'a';
				transmit(data);
				
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



//----------------------------------------------------------------------------------



/** @brief A method to initialize the SPI interface to the RF module.
  * @details This method sets the correct SPI pins as inputs and outputs, and sets
  * their states so that the SPI interface can connect to the RF module.
  */
void task_radio::init_spi (void)
{
	// Set MOSI(PB2) and SCK(PB1) output, MISO(PB3) input, CSN(PB0) output
	DDRB |= (1 << PB2) | (1 << PB1) | (1 << PB0);
	DDRB &= ~(1 << PB3);
	// Set CE(PE3) as output
	DDRE |= (1 << PE3);

	// Enable SPI as master
	SPCR |= (1 << SPE) | (1 << MSTR) | (1 << SPR1);
	
	// CSN high to start with
	PORTB |= (1 << PB0);
	// CE low to start with
	PORTE &= ~(1 << PE3);
}


/** @brief A method to send a byte to the RF module.
  * @details This method uses the SPI data register on our microcontroller to
  * send a byte of information to the RF module.
  * @param c_data A char byte that is to be sent to the RF module.
  * @return SPDR The char data that was sent to the RF module.
  */
char task_radio::write_byte (char c_data)
{
	// Load byte to data register
	SPDR = c_data;
	
	// Wait for transmit to finish
	while(!(SPSR & (1 << SPIF))) ;
	
	return SPDR;
}

/** @brief A method to get the value of a register on the RF module.
  * @details This method uses the SPI data register on our microcontroller to
  * send a byte of information to the RF module.
  * @param reg A uint8_t value that corresonds to the desired register.
  * @return reg The uint8_t data that was contained in the desired register
  * RF module.
  */
uint8_t task_radio::get_reg (uint8_t reg)
{
	_delay_us (10);
	// Set CSN low
	PORTB &= ~(1 << PB0);
	_delay_us (10);
	// Set nRF to reading mode
	write_byte (R_REGISTER + reg);
	_delay_us (10);
	reg = write_byte (NOP);
	_delay_us (10);
	// Set CSN high
	PORTB |= (1 << PB0);
	return reg;
}


/** @brief A method to write data to the RF module.
  * @details This method uses the SPI data register on our microcontroller to
  * send a byte of information to the RF module.
  * @param ReadWrite A uint8_t character that corresponds to either a value
  * being written to or read from the RF module.
  * @param reg A uint8_t value that corresponds to the register of the RF
  * module that is being modified.
  * @param val A pointer to the values array that are to written to the RF
  * module.
  * @param antVal A uint8_t value that signifies the number of bytes to write
  * to the RF module.
  * @return ret The array of uint8_t* data that was sent to the RF module.
  */
uint8_t *task_radio::read_or_write (uint8_t ReadWrite, uint8_t reg, uint8_t *val, uint8_t antVal)
{
	if (ReadWrite == W)
	{
		reg = W_REGISTER + reg;
	}
	
	static uint8_t ret [32];
	
	_delay_us (10);
	// Set CSN low
	PORTB &= ~(1 << PB0);
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
	PORTB |= (1 << PB0);
	return ret;
}

/** @brief A method to transmit a character array with the RF module.
  * @details This method uses the read_or_write method to transmit an array
  * of characters using the RF module.
  * @param W_buff A uint8_t* pointer to an array which is to be sent to
  * the RF module.
  */
void task_radio::transmit (uint8_t *W_buff)
{
	read_or_write (R, FLUSH_TX, W_buff, 0);
	read_or_write (R, W_TX_PAYLOAD, W_buff, 2);
	
	_delay_ms(10);
	// Set CE high
	PORTE |= (1 << PE3);
	_delay_ms(10);
	// Set CE low
	PORTE &= ~(1 << PE3);
}
	