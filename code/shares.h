//*************************************************************************************
/** @file shares.h
 *    This file contains extern declarations for queues and other inter-task data
 *    communication objects for the 507 term project.
 *
 *  Revisions:
 *    @li 11-29-2018 KM file created to allow all tasks to access shared data.
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
//*************************************************************************************

// This define prevents this .h file from being included multiple times in a .cpp file
#ifndef _SHARES_H_
#define _SHARES_H_

//-------------------------------------------------------------------------------------
// Externs:  In this section, we declare variables and functions that are used in all
// (or at least two) of the files in the data acquisition project. Each of these items
// will also be declared exactly once, without the keyword 'extern', in one .cpp file
// as well as being declared extern here.

// This queue allows tasks to send characters to the user interface task for display.
extern TextQueue* p_print_ser_queue;

// Motor velocity setting variable
extern TaskShare<int8_t>* p_motor_vel;

// Servo position setting variable
extern TaskShare<int8_t>* p_servo_pos;

// Encoder feedback variable
extern TaskShare<int8_t>* p_enc_read;

// Radio ping flag
extern TaskShare<bool>* p_rf_ping;

// Drive state flag
extern TaskShare<uint8_t>* p_drive_state;

//Rising or falling edge flag for ultrasonic sensor
extern TaskShare<int8_t>* edge_1;

//Pulse width for USR1
extern TaskShare<uint16_t>* width_1;

#endif // _SHARES_H_
