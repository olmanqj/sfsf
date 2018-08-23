/*
Copyright 2018 olmanqj
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */ 
#ifndef SFSF_LOG_H_
#define SFSF_LOG_H_

#ifndef SFSF_H_
#error Include sfsf.h before sfsf_log.h!
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file	sfsf_log.h
 * @brief	API for Log Service
 
Log Service
=================

Features Summary
-------------
- Store data about the behavior of the spacecraft 
- Store data with timestamp.

Module Description
-----------------
Provides an easy way store data about the behavior of the spacecraft into a file. 
It provides functions for storing a string message and variable values. 
Some data that may be valuable to store in the Log file is for example occurrence 
of events or errors, the value of a variable at a given time, incoming commands 
and the result. The Log Service can print every message with the timestamp, but a
function which provides the timestamp as string should be set with the function 
set_log_timestamp_generator(), the function get_timestamp_str() from the 
Time Service can be assigned. If desired to print the Log messages also to the 
debug output, enable the CONF_LOG_DEBUG.
*/

/**
 * @brief	Init tasks which stores Log messages.
 *
 * Init Log Service, which provides persistence for the messages.
 *
 * @note	Storage Service functions should be ported, see sfsf_port.h.
 * @return	-1 if error , 0 if OK 
 */
int init_log_service(void);

/**
 * @typedef timestamp_generator_t
 * @brief	Typedef of a Timestamp generator function.
 * 
 * The function should receive the destination buffer where the timestamp will
 * be stored, and the size of the buffer. It should return the size of the
 * string if success, zero if fails. Set with set_log_timestamp_generator() 
 * during initialization.  
 *
 * @note	The function get_timestamp_str() from Time Service meets this requirements. 
*/
typedef size_t (*timestamp_generator_t) ( char *dest_buffer, size_t buff_size);

/** 
 * @brief	Set the Timestamp generator function.
 * 
 * If set, log messages will be printed with the Timestamp.
 * 
 * @note	The function get_timestamp_str() from Time Service is situable.
 * @param	timestamp_generator		Function that generates the timestamp into dest_buf
 */
void set_log_timestamp_generator( timestamp_generator_t timestamp_generator);

/**
 * @brief	Pint a string on the Log File
 * @param	str			String to be printed on Log file
 * @return	-1 if error , 0 if OK  
 */
int log_print(const char *str);

/**
 * @brief	Pint a int value with format "key:value" on the Log File 
 * @param	name		Name of the value, "key"
 * @param	value		Corresponding value 
 * @return	-1 if error , 0 if OK  
 */
int log_print_int(const char *name, int value);

/**
 * @brief	Print a float value with format "key:value" on the Log File  
 * @param	name		Name of the value, "key"
 * @param	value		Corresponding value 
 * @return	-1 if error , 0 if OK  
 */
int log_print_float(const char *name, float value);

/**
 * @brief	Get Task Handle
 * @return	csp_thread_handle_t 
 */
csp_thread_handle_t get_log_task_handle(void);

#ifdef __cplusplus
}
#endif
#endif /* SFSF_LOG_H_ */
