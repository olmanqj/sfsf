/* 
Copyright 2018 olmanqj
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */ 
#ifndef SFSF_H_
#define SFSF_H_

// CSP Includes Needed by all SFSF Services
#include <csp.h>
#include <csp_thread.h>
// Include the Configurations for all SFSF Sercives // TODO Decouple
#include "sfsf_config.h"


/** 
 * @file	sfsf.h
 * @brief	SFSF Framework Private Configurations
 
 
Private Configurations
======================
This file contains configurations and definitions the the user
don't need to modify. However this file should be included in
any other where a SFSF Services will be used, hence all services
needs this configurations. 
*/
/// @cond DO_NOT_DOCUMENT


//////////////////////////////////////////////
/////	TASKS CONFIGURATIOS		//////////////
//////////////////////////////////////////////
// Configurations for Tasks
// 
// Make sure this Configuration is defined
#ifndef	CONF_MINIMAL_STACK_SIZE
#define CONF_MINIMAL_STACK_SIZE			256
#endif
// Priorities for FreeRTOS Tasks, the lower has less priority, Idle Priority is 1
#define TASK_PRIO_BACKGROUND				2
#define TASK_PRIO_MEDIUM					3
#define TASK_PRIO_HIGH						4
#define TASK_PRIO_SUPER						5
// App Task Configs
#define CONF_APP_TASK_PRIORITY				TASK_PRIO_HIGH
#define CONF_APP_TASK_STACK_SIZE			CONF_MINIMAL_STACK_SIZE*4
// Command Queue Task Configs
#define CONF_CMD_TASK_PRIORITY				TASK_PRIO_SUPER
#define CONF_CMD_TASK_STACK_SIZE			CONF_MINIMAL_STACK_SIZE
#define CONF_CMD_EVENT_CHECK_PERIOD			1000						// Period for checking if a event has occurred, to execute a command in queue.
// Param Task Configs
#define CONF_PARAM_TASK_PRIORITY			TASK_PRIO_BACKGROUND
#define CONF_PARAM_TASK_STACK_SIZE			CONF_MINIMAL_STACK_SIZE
#define CONF_PARAM_PERSIST_PERIOD			3000						// Period to store persistent parameters in persistent memory.
// HK Task Configs
#define CONF_HK_TASK_PRIORITY				TASK_PRIO_BACKGROUND
#define CONF_HK_TASK_STACK_SIZE				CONF_MINIMAL_STACK_SIZE*3
// Log Task Configs
#define CONF_LOG_TASK_PRIORITY				TASK_PRIO_BACKGROUND
#define CONF_LOG_TASK_STACK_SIZE			CONF_MINIMAL_STACK_SIZE*3
#define CONF_LOG_PERSIST_PERIOD				3000						// Period to store log messages in file.
// CSP Task Configs
#define CONF_CSP_TASK_PRIORITY				TASK_PRIO_MEDIUM
#define CONF_CSP_TASK_STACK_SIZE			CONF_MINIMAL_STACK_SIZE*4
// Software Watchdog Timer Task Configs
#define CONF_SW_WTD_TASK_PRIORITY			TASK_PRIO_BACKGROUND
#define CONF_SW_WTD_TASK_STACK_SIZE			CONF_MINIMAL_STACK_SIZE
#define CONF_SW_WTD_CHECK_PERIOD			1000


/// @endcond
#endif /* SFSF_FRAMEWORK_H_ */