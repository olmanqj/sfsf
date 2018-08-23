/*
Copyright 2018 olmanqj
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */ 
#ifndef SFSF_PORT_H_
#define SFSF_PORT_H_

#include "sfsf.h"

/**
 * @file	sfsf_port.h
 * @brief	Function declarations to ported
 
SFSF Port
=================
Some features form Services require functions for interacting with hardware, 
for example rebooting the OBC, print messages to debug output, or writing a file
in a external memory. Each satellite is build with different hardware, therefore
the implementation of this functions may vary. It is expected that the developers
of each mission implement this functions for the specific hardware.

This file contains the definition of all the functions that need to be implemented.
This can be done in a source file ( file with ".c" extension), see the ports for 
AVR32_UC3C and Linux in folder services/src/ports .

@see services/src/ports

@note
Function prototype with "weak attribute"  means the function may be implemented
by user, but is not mandatory to do so.
*/



/**
 * @brief	Include the header of your port	
 *
 * Your port should be conformed by a header file (port.h)
 * and a source file (port.c).
 * @see src/ports
*/
#include <port.h>


//////////////////////////////////////////////
/////	SYSTEM FUNCTIONS		//////////////
//////////////////////////////////////////////
/** @name	System Functions
 *  @brief Port this functions for rebooting and shooting down the OBC.
 */
///@{
	
/**
 * @brief	Reboot the system. Use csp_sys_reboot()!
 * 
 * Implement this function to enable csp_sys_reboot().
 * @note Dont use this function! use csp_sys_reboot() instead.
*/
void cpu_reset(void);

/**
 * @brief	Shutdown the system.  Use csp_sys_shutdown()!
 * 
 * Implement this function to enable csp_sys_shutdown().
 * @note Dont use this function! use csp_sys_shutdown() instead.
*/
void cpu_shutdown(void);
///@}


//////////////////////////////////////////////
/////	DEBUG FUNCTIONS			//////////////
//////////////////////////////////////////////
/** @name	Debug Functions
 *  @brief	Port this functions to print debug info
 */
///@{

/** 
 * @brief	Print a string of characters on the debug output.
 * @param	str The string to print
 */
void print_debug_port(const char *str);


/** 
 * @brief	Print a character on the debug output
 * \param	c The character to print.
 */
void print_debug_char_port(char c);


/** 
 * @brief	Print a char as hex representation on the  debug output.
 * @param	c The hex character to print.
 */
void print_debug_hex_port(char c);


/** 
 * @brief Prints an unsigned integer on the debug output.
 * @param n The integer to print.
 */
void print_debug_uint_port(unsigned int n);

	
///@}

//////////////////////////////////////////////
/////	STORAGE FUNCTIONS		//////////////
//////////////////////////////////////////////
/** @name	Storage Functions
 *  @brief	Port this functions for File System Calls
 */
///@{
	

/**
 * @def		FILE_T
 * @brief	Type of File Descriptor for file system.
 *
 * Define FILE_T as the File Descriptor Type, in your port header file.
*/	
#ifndef FILE_T
#warning The File Descriptor type should be ported! Define FILE_T in yout port.h! See sfsf_port.h
#define FILE_T void
#endif

/**
 * @def		FILE_MODE_T
 * @brief	Type of File Open Modes
 *
 * Define FILE_MODE_T as the File Modes Type, in your port header file.
*/	
#ifndef FILE_MODE_T
#warning The File Mode type should be ported! Define FILE_MODE_T in yout port.h! See sfsf_port.h
#define FILE_MODE_T int
#endif


/**
 * @brief	Open or create a file
 * @param	fp			Pointer to a File descriptor to store file info
 * @param	path		Pathname of file to open or create
 * @param	mode		Mode to open or create file
 * @return	-1 if fails, 0 if OK
*/	
int  file_open_port(FILE_T * fp, const char *path, FILE_MODE_T mode);

/**
 * @brief	Close a file descriptor
 * @param	fp		File descriptor of open file
 * @return	-1 if fails, 0 if OK
*/
int file_close_port(FILE_T * fp);

/**
 * @brief	Read a string (until new-line or end-of-file) from a file descriptor
 * @param	fp		File descriptor of open file
 * @param	buff	Destination buffer to store read bytes
 * @param	len		Limit bytes to read
 * @return	-1 if fails, the number of bytes read if OK	
*/
char * file_read_port( FILE_T * fp, char * buff,  int len);

/**
 * @brief	Write a string to a file descriptor
 * @param	fp		File descriptor of open file
 * @param	str		Source buffer whit sting to write into file
 * @return	-1 if fails, the number of bytes written if OK
*/
int file_write_port( FILE_T* fp, const char* str );
	
/**
 * @brief	Remove a file
 * @param	path		Pathname of file to remove
 * @return	-1 if fails, 0 if OK
*/
int file_remove_port(const char *path);
	
///@}


#endif /* SFSF_PORT_H_ */