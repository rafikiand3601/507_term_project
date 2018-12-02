//**************************************************************************************
/** @file task_radio.h
 *    This file contains header stuff for a user interface task for a ME507/FreeRTOS
 *    test suite. 
 *
 *  Revisions:
 *    @li 11-29-2018 KM Header to 
 *
 */
//**************************************************************************************


// This define prevents this .h file from being included multiple times in a .cpp file
#ifndef _TASK_RADIO_H_
#define _TASK_RADIO_H_

#include <stdlib.h>                         // Prototype declarations for I/O functions


// FreeRTOS library includes
#include "FreeRTOS.h"                       // Primary header for FreeRTOS
#include "task.h"                           // Header for FreeRTOS task functions
#include "queue.h"                          // FreeRTOS inter-task communication queues


// ME 507 library includes
#include "rs232int.h"                       // ME405/507 library for serial comm.
#include "adc.h"                            // Header for A/D converter class driver
#include "time_stamp.h"                     // Class to implement a microsecond timer
#include "taskbase.h"                       // Header for ME405/507 base task class
#include "taskqueue.h"                      // Header of wrapper for FreeRTOS queues
#include "textqueue.h"                      // Header for a "<<" queue class
#include "taskshare.h"                      // Header for thread-safe shared data


// NRF24 Library
#include "nrf24l01.h"

#include "shares.h"                         // Global ('extern') queue declarations




class task_radio : public TaskBase
{
private:
	uint8_t to_address[2];
	nRF24L01Message msg;
	nRF24L01 *rf;

protected:
	void setup_rf (nRF24L01*);

public:
	// This constructor creates a user interface task object
	task_radio (const char*, unsigned portBASE_TYPE, size_t, emstream*);

	/** This method is called by the RTOS once to run the task loop for ever and ever.
	 */
	void run (void);
};

#endif // _TASK_RADIO_H_
