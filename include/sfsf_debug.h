/*
Copyright 2018 olmanqj
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */
#ifndef SFSF_DEBUG_H_
#define SFSF_DEBUG_H_

#ifndef SFSF_H_
#error Include sfsf.h before sfsf_debug.h!
#endif

#include <sfsf_port.h>

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @file	sfsf_debug.h
 * @brief	API for Debug Service

Debug Service
=================

Features Summary
-------------
- Interface for printing information on the debug output.


Module Description
-----------------
The debug Service is a small interface. It provides functions
for printing information on the debug output. It is important to note
that the actual implementation for this functions may differ between
platforms, therefore is responsibility of the user to port this functions.
All the functions should be ported to make functional the debug
configurations options from sfsf_config.h
See sfsf_port.h.
*/

/**
 * @brief	Print a string of characters on the debug output.
 * @note This function should be ported, see sfsf_port.h
 * @param	str The string to print
 */
void print_debug(const char *str);

/**
 * @brief	Print a character on the debug output
  * @note This function should be ported, see sfsf_port.h
 * \param	c The character to print.
 */
void print_debug_char(char c);

/**
 * @brief	Print a char as hex representation on the  debug output.
 * @note This function should be ported, see sfsf_port.h
 * @param	c The hex character to print.
 */
void print_debug_hex(char c);

/**
 * @brief Prints an unsigned integer on the debug output.
 * @note This function should be ported, see sfsf_port.h
 * @param n The integer to print.
 */
void print_debug_uint(unsigned int n);



#ifdef __cplusplus
}
#endif
#endif /* SFSF_DEBUG_H_ */
