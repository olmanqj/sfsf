/*
Copyright 2018 olmanqj
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
-*/
#include <stdlib.h>

// CSP Includes
#include <csp.h>
#include <csp_system.h>
#include <csp_thread.h>

//Framework Includes
#include "sfsf.h"
#include "sfsf_debug.h"
#include "sfsf_log.h"
#include "sfsf_hk.h"
#include "sfsf_cmd.h"
#include "sfsf_param.h"
#include "sfsf_time.h"


int main(int argc, char **argv)
{	
	// Init hardware, before any software service 
	extern void init_hardware(int argc, char **argv);	// Function prototype, user shall implement this function for specific hardware
	init_hardware(argc, argv);

	// Print greattings 
	print_debug(" ________  ________ ________  ________ \n"\
				"|\\   ____\\|\\  _____\\\\   ____\\|\\  _____\\\n"\
				"\\ \\  \\___|\\ \\  \\__/\\ \\  \\___|\\ \\  \\__/ \n"\
				" \\ \\_____  \\ \\   __\\\\ \\_____  \\ \\   __\\\n"\
				"  \\|____|\\  \\ \\  \\_| \\|____|\\  \\ \\  \\_|\n"\
				"    ____\\_\\  \\ \\__\\    ____\\_\\  \\ \\__\\ \n"\
				"   |\\_________\\|__|   |\\_________\\|__| ... \n"\
				"   \\|_________|       \\|_________|\n");
     
	 
	print_debug("\nStarting CSP...\n"); 
	// Init the Network service (CSP Router), before the Application Task
	extern void init_csp( int argc, char **argv );	// Function prototype, code is located in init_services.c
	init_csp(argc, argv);
	print_debug("CSP OK!\n");


	print_debug("\nStarting Services...\n"); 
	// Init all services required for Application
	
	extern void set_up_services( void );	// Function prototype, code is located in init_services.c
	set_up_services();
	
	extern void init_services( void );	// Function prototype, code is located in init_services.c
	init_services();
	print_debug("Services OK!\n");


	print_debug("\nStarting App...\n"); 
	// Start the application tasks, which handles all commands, updates the param table, clears the watch-dog, etc.
	CSP_DEFINE_TASK( app_task );		// Function prototype, code is located in app_task.c
	csp_thread_handle_t app_task_handle;
	if( csp_thread_create( app_task,  "APP_TASK",  CONF_APP_TASK_STACK_SIZE,  NULL, CONF_APP_TASK_PRIORITY, &app_task_handle )!=EXIT_SUCCESS)
	{
		print_debug("\n\nError at Init Application, NOMEM \n\n");
		csp_sleep_ms(1000);
		csp_sys_reboot();
	}
	print_debug("App OK!\n");


	// Some platforms ( OS and/or Hardware) require to run some routines to make effective previous function calls.
	extern void late_init_routine( void ); 
	late_init_routine();

	// Should never reach here
	while(1) csp_sleep_ms(1000000);
	return -1;
}





