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
 
#include <stdlib.h>
#include <time.h>

// CSP Includes
#include <csp/csp.h>
#include <csp/arch/csp_system.h>
#include <csp/arch/csp_thread.h>
#include <csp/arch/csp_queue.h>
#include <csp/arch/csp_time.h>

// Framework Includes
#include <sfsf.h>
#include <sfsf_debug.h>
#include <sfsf_time.h>

// For Time Service
uint32_t timestamp_offset_s;
uint32_t boot_timestamp_ms;
uint32_t boot_timestamp_s;


// For Software Watchdiog
uint32_t sw_wdt_last_reset_ms;
uint32_t sw_wdt_timout_ms;
// Task handle
csp_thread_handle_t sf_wt_service_task;


// Software Watchdog Task
CSP_DEFINE_TASK( sw_wdt_task )
{
	while(1)
	{
		// Sleep
		csp_sleep_ms(CONF_SW_WTD_CHECK_PERIOD);
		// if debug ENABLE, print info
		#if	CONF_TIME_DEBUG == ENABLE
		print_debug("TIME>\tChecking WDT\n");
		#endif
		// Check timeout, If Timeout elapsed, reboot OBC
		if( sw_wdt_last_reset_ms + sw_wdt_timout_ms < csp_get_ms()  ) csp_sys_reboot();
	}
	return CSP_TASK_RETURN;
}


// Init Software Watchdog Timer
int init_sw_wdt(void)
{
	sw_wdt_timout_ms = CONF_TIME_SW_WDT_TIMEOUT_MS;
    return csp_thread_create(sw_wdt_task, "SW_WDT_TASK", CONF_SW_WTD_TASK_STACK_SIZE, NULL, CONF_SW_WTD_TASK_PRIORITY, &sf_wt_service_task);
}



void reset_sw_wdt(void)
{
	sw_wdt_last_reset_ms =  csp_get_ms();
}


// init Time Service
void inti_time(void)
{
	boot_timestamp_ms = csp_get_ms();
	boot_timestamp_s  = csp_get_s();
}


//Return milli seconds science boot
uint32_t time_since_boot_ms(void)
{
	return csp_get_ms() - boot_timestamp_ms;
}


//Return seconds science boot
uint32_t time_since_boot_s(void)
{
	return csp_get_s() - boot_timestamp_s;
}


// Sync Timestamp
int sync_timestamp( uint32_t new_timestamp_s )			// TODO improve, if curr time stamp is bigger than new_time, sync dosn't work
{
	uint32_t curr_time = csp_get_s();
	if(curr_time >=  new_timestamp_s ) timestamp_offset_s = 0;
	else timestamp_offset_s = new_timestamp_s - curr_time;
	return EXIT_SUCCESS;
}


// Return the local timestamp in seconds
uint32_t get_timestamp_s(void)
{
	return timestamp_offset_s + csp_get_s();
}


// Stores the local timestamp as string into dest_buffer
size_t get_timestamp_str( char *dest_buffer, size_t buff_size )
{
	time_t raw_time;
	struct tm* curr_time;
	// Formt timestamp in seconds to struct with date
	raw_time = (time_t)get_timestamp_s();
	curr_time = localtime(&raw_time);
	// Print formated Date to buffer
	return strftime(dest_buffer, buff_size, CONF_TIME_TIMESTAMP_FORMT, curr_time);
}
