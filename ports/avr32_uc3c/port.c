/*
Copyright 2018 olmanqj
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/ 

#include "sfsf_port.h"

 
//////////////////////////////////////////////
/////	SYSTEM FUNCTIONS		//////////////
//////////////////////////////////////////////
// Includes specific for AVR32_UC3C 
#include <avr32_reset_cause.h>
#include <FreeRTOS.h>
#include <task.h>


void cpu_reset()
{
	reset_do_soft_reset();
}


void cpu_shutdown()
{
	vTaskSuspendAll();
}



//////////////////////////////////////////////
/////	DEBUG FUNCTIONS			//////////////
//////////////////////////////////////////////
// Includes specific for AVR32_UC3C with FreeRTOS
#include <print_funcs.h>
#include <stdio.h>

// Print a string of characters to debug output

void print_debug_port(const char *str)
{
	taskENTER_CRITICAL();
	print_dbg(str);
	printf("%s", str);
	taskEXIT_CRITICAL();
}

// Print a character to debug output
void print_debug_char_port(char c)
{
	taskENTER_CRITICAL();
	print_dbg_char((int)c);
	printf("%c", c);
	taskEXIT_CRITICAL();
}

// Print a hex to debug output
void print_debug_hex_port(char c)
{
	taskENTER_CRITICAL();
	print_dbg_char_hex((unsigned char) c);
	printf("%x", c);
	taskEXIT_CRITICAL();
}

// Prints an unsigned integer to debug output
void print_debug_uint_port(unsigned int n)
{
	taskENTER_CRITICAL();
	print_dbg_ulong((unsigned long) n);
	printf("%d", n);
	taskEXIT_CRITICAL();
}



//////////////////////////////////////////////
/////		File System			//////////////
//////////////////////////////////////////////
#include <ff.h>
#include "sfsf.h"
#include "sfsf_debug.h"

int file_open_port(FILE_T * fp, const char *path, FILE_MODE_T mode)
{
	return f_open(fp,  path,  mode );
}


int file_close_port(FILE_T * fp)
{
	return f_close ( fp );
}


char * file_read_port( FILE_T * fp, char * buff,  int len)
{
	return (char*)f_gets (buff, len, fp );
}


int file_write_port( FILE_T* fp, const char* str )
{
	return f_puts (str, fp);
}


int file_remove_port(const char *path)
{
	return f_unlink ( path );
}
