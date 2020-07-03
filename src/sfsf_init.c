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
 
#include <sfsf.h>
#include <sfsf_debug.h>
#include <sfsf_log.h>
#include <sfsf_hk.h>
#include <sfsf_cmd.h>
#include <sfsf_param.h>
#include <sfsf_time.h>


/*
 * Initialize the SFSF Services with the configs
 * from the configuration file.
 */
void init_services(void)
{
	// Init Time Service Features
	// Init time service this will register time since reboot
	inti_time();
	// Init Software Watchdog Timer
	#if CONF_TIME_SW_WDT_ENABLE == ENABLE
	init_sw_wdt();
	#endif

	// Init Log Service Features
	// Set Timestamp generator for Log Service, this way Log messages include Timestamp
	set_log_timestamp_generator(get_timestamp_str);
	// Start the Log Service
	#if CONF_LOG_PERSIST_ENABLE == ENABLE
	init_log_service();
	#endif

	// Init Param Service Features
	// Enable Task that stores params in persistent memory, storage functions should be ported
	#if CONF_PARAM_PERSIST_ENABLE ==  ENABLE
	init_param_persistence();
	#endif

	// Init HK Service Features
	// Set the telemetry collector function, to automatically collect and send telemetry
	set_telemetry_collector(collect_telemetry_params);
	// Init Housekeeping Service, broadcast and store beacons
	#if CONF_HK_ENABLE == ENABLE
	init_hk_service();
	#endif

	// Init Command Service Features
	// Enable commands queue, for execute based on events
	#if CONF_CMD_QUEUE_ENABLE ==  ENABLE
	init_cmd_queue();	// Not implemented
	#endif
}
