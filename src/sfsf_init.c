/*
 * sfsf_init.c
 *
 * Created: 7/20/2018 6:47:04 PM
 *  Author: cubesatteam
 */ 
// Services includes 
#include "sfsf.h"
#include "sfsf_debug.h"
#include "sfsf_log.h"
#include "sfsf_hk.h"
#include "sfsf_cmd.h"
#include "sfsf_param.h"
#include "sfsf_time.h"


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

