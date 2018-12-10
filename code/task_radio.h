//**************************************************************************************
/** @file task_radio.h
 *    This file contains header contents for the RF transciever task.
 *
 *  Revisions:
 *    @li 11-29-2018 KM header for RF transciever task.
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


// This define prevents this .h file from being included multiple times in a .cpp file
#ifndef _TASK_RADIO_H_
#define _TASK_RADIO_H_

#include <stdlib.h>                         // Prototype declarations for I/O functions
#include <util/delay.h>

// FreeRTOS library includes
#include "FreeRTOS.h"                       // Primary header for FreeRTOS
#include "task.h"                           // Header for FreeRTOS task functions
#include "queue.h"                          // FreeRTOS inter-task communication queues


// ME 507 library includes
#include "rs232int.h"                       // ME405/507 library for serial comm.
#include "time_stamp.h"                     // Class to implement a microsecond timer
#include "taskbase.h"                       // Header for ME405/507 base task class
#include "taskqueue.h"                      // Header of wrapper for FreeRTOS queues
#include "textqueue.h"                      // Header for a "<<" queue class
#include "taskshare.h"                      // Header for thread-safe shared data



#include "nrf_names.h"

#include "shares.h"                         // Global ('extern') queue declarations



/** @brief This task is used to control the RF transciever.
 *  @details This task inherits the TaskBase class, and is used to run as a finite 
 *  state machine. It controls the actions of the RF transciever using an SPI interface.
 *  While this task can communicate with the RF transciever, we have not gotten the
 *  transciever to communicate with another RF module.
 */
class task_radio : public TaskBase
{
private:
	uint8_t to_address[2];

protected:
	void init_spi (void);
	char write_byte (char);
	uint8_t get_reg (uint8_t);
	uint8_t *read_or_write (uint8_t, uint8_t, uint8_t*, uint8_t);
	void transmit (uint8_t*);

public:
	// This constructor creates a user interface task object
	task_radio (const char*, unsigned portBASE_TYPE, size_t, emstream*);

	/** This method is called by the RTOS once to run the task loop for ever and ever.
	 */
	void run (void);
};

#endif // _TASK_RADIO_H_
