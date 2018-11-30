#ME 507 Term Project

Contributors:
-Kevin Marshall

*How to use:
 *-Add/edit task objects that you are tasked with.
 *-You must run the makefile from a linux machine OR CygWin.
 *-Clone this repository to your cygwin folder.
 *-Code is stored in the "code" folder.
 *-Only modify the main project file to add tasks.
 *-You can use the servo task as a template for how to add new tasks.
 *-"make" in the code directory in a CygWin will compile your code.
 *-Add new .cpp files to the makefile under "SOURCES"
 *-"make install" will compile and put the code on the atmega(assuming
	AVRISP and UsbAsp are installed correctly).
 *-Use PuTTy or Tera Term to communicate with the board via serial.
	baud = 9600, COMX, you will need to look in device manager for your
	COM number to connect to.
 *-**DO NOT BURN FUSES ON THE ATMEGA**