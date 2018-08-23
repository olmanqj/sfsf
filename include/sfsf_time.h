/*
Copyright 2018 olmanqj
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/ 
#ifndef SFSF_TIME_H_
#define SFSF_TIME_H_

#ifndef SFSF_H_
#error Include sfsf.h before sfsf_time.h!
#endif

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @file	sfsf_time.h
 * @brief	API for Time Service
 
Time Service
=================

Features Summary
-------------
- Retrieve the time since boot
- Synchronize time with the ground
- Retrieve the time from the ground
- Enable and reset a software Watchdog Timer


Module Description
-----------------	
The Time Service is a small module which provides functions for
collecting the current timestamp and the time since boot of the system.
Also provides functions for enabling and managing a software Watchdog timer.
 */


/** @name Parameterizable Variables
 *
 * Use the parateerize() Macro to parameterize this variables into the Parameters Table,
 * this will simplify the control of Time Service, by providing a way to change the behavior .
 * @see sfsf_param.h.
 */
///@{
extern uint32_t sw_wdt_timout_ms;		/**< Timeout for software watchdog timer. */
///@}


/**
 * @brief	Init Software Watchdog Timer 
 * @return	EXIT_FAILURE if error , EXIT_SUCCESS if OK
 */
int init_sw_wdt(void);

/**
 * @brief	Reset Software Watchdog Timer 
 */
void reset_sw_wdt(void);


/**
 * @brief	Init Time Service   
 * 
 * Init Time Service for gathering time since boot, run at init.
 */
void inti_time(void);


/**
 * @brief	Return milliseconds science boot    
 * @return	Milliseconds science boot    
 */
uint32_t time_since_boot_ms(void);

/**
 * @brief	Return seconds science boot    
 * @return  Seconds science boot 
 */
uint32_t time_since_boot_s(void);

/**
 * @brief	Sync Timestamp with ground
 * @param	new_timestamp_s   New timestamp to be stored in s    
 * @return	EXIT_FAILURE if error , EXIT_SUCCESS if OK 
 */
int sync_timestamp( uint32_t new_timestamp_s );

/**
 * @brief	Return the local timestamp in seconds
 * @return	Timestamp in milliseconds
 */
uint32_t get_timestamp_s(void);

/**
 * @brief	Stores the local timestamp as string into dest_buffer
 * @param	dest_buffer  Destination Buffer where timestamp will be stored
 * @param	buff_size	Size of dest_buffer      
 * @return	length of the resulting C string
 */
size_t get_timestamp_str( char *dest_buffer, size_t buff_size );

#ifdef __cplusplus
}
#endif
#endif /* SFSF_TIME_H_ */
