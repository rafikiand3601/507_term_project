ME 507 Term Project

Contributors:
-Kevin Marshall

How to use:
-Add/edit task objects that you are tasked with
-You must run the makefile from a linux machine OR CygWin.
	Clone this repository to your cygwin folder.
-Only modify the main project file to add tasks.
-You can use the servo task as a template for how to add new tasks.
-"make" in the project directory in a CygWin will compile your code.
-Add new .cpp files to the makefile under "SOURCES"
-"make install" will compile and put the code on the atmega(assuming
	AVRISP and UsbAsp are installed correctly).
-DO NOT BURN FUSES ON THE ATMEGA