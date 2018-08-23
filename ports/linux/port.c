/*
Copyright 2018 olmanqj
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */ 

#include "sfsf_port.h"
 
///////////////////////////////////////////////
/////	SYSTEM FUNCTIONS		//////////////
//////////////////////////////////////////////
// Includes specific for AVR32_UC3C 
void cpu_reset(void)
{
	exit(0);
}


void cpu_shutdown(void)
{
	exit(0);
}



///////////////////////////////////////////////
/////	DEBUG FUNCTIONS			//////////////
//////////////////////////////////////////////
#include <stdio.h>

// Print a string of characters to debug output
void print_debug_port(const char *str)
{
	printf("%s", str);
}

// Print a character to debug output
void print_debug_char_port(char c)
{
	printf("%c", c);
}

// Print a hex to debug output
void print_debug_hex_port(char c)
{
	printf("%x", c);
}

// Prints an unsigned integer to debug output
void print_debug_uint_port(unsigned int n)
{
	printf("%d", n);
}


///////////////////////////////////////////////
/////			FILE SYSTEM				//////////////
//////////////////////////////////////////////
/*

FILE * fopen(const char *_name, const char *_type)
int fclose(FILE *)
char * fgets (char *,  int,  FILE *)
int fprintf(FILE *, const char *, ...)



*/

