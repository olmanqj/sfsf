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

/**
 * @brief	Initialization Functions for hardware and software.
 * @example init_functions.c
 * Initialization Functions Example


 Initialization Functions
 ========================
 This is an example of how to create the Initialization Functions for hardware and software.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// CSP Includes
#include <csp/csp.h>
#include <csp/arch/csp_system.h>
#include <csp/arch/csp_thread.h>
#include <csp/drivers/usart.h>
#include <csp/interfaces/csp_if_kiss.h>
#include <csp/interfaces/csp_if_lo.h>

// Services includes
#include <sfsf.h>
#include <sfsf_debug.h>
#include <sfsf_log.h>
#include <sfsf_hk.h>
#include <sfsf_cmd.h>
#include <sfsf_param.h>
#include <sfsf_time.h>

// Mission Includes
#include <mission_config.h>
#include <param_table.h>
#include <cmd_table.h>




/**
 * @brief	Init hardware calls and configurations
 *
 * This is the first function to be executed when the software starts.
 * In this function user should write all the code for initializing
 * the hardware.
*/
void init_hardware(int argc, char **argv)
{
	// For Linux on pc you dont need to init your hardware
}



/**
 * @fn		void init_csp(void)
 * @brief	Init CubeSat Space Protocol
 *
 * This function is the second to be called by the main.
 * User should write the initialization code required for CSP.
 * @see 	CSP examples: https://github.com/libcsp/libcsp/tree/master/examples
 * @note 	In examples interface definition, handle and callback are defined
 *	within the main function, because this will never ends so the definitions remains.
 *  For init_csp() interface definition, handle and callback need to be global, so
 *  after the function returns, the definitions still prevails.
 * @see Application Example
*/


// Global CSP interface definitions
csp_iface_t * csp_if_kiss;
void init_csp(int argc, char **argv)
{
	// Check that user assigned serial port
    if(argc != 2)
    {
        printf("Missing Serial Port!\n");
        printf("Usage: ./sfsf_example SERIAL_PORT_NAME\n\n");
        exit(1);
    }

	csp_log_info("Initialising CSP");

  // Init CSP
	// Get default configurations
  csp_conf_t csp_conf;
  csp_conf_get_defaults(&csp_conf);
	// Set custon configurations
  csp_conf.address = CSP_OBC_ADDRESS;
	csp_conf.buffers = CSP_BUFFER_POOL_SIZE;
	csp_conf.buffer_data_size = CSP_BUFFER_SIZE;
	// Try init
  int error = csp_init(&csp_conf);
  if (error != CSP_ERR_NONE) {
      csp_log_error("csp_init() failed, error: %d", error);
      exit(1);
  }

	// Set up communications interface
  csp_usart_conf_t conf = {
      .device = argv[1],
      .baudrate = 57600,
      .databits = 8,
      .stopbits = 1,
      .paritysetting = 0,
      .checkparity = 0};
  error = csp_usart_open_and_add_kiss_interface(&conf, CSP_IF_KISS_DEFAULT_NAME,  &csp_if_kiss);
  if (error != CSP_ERR_NONE) {
      csp_log_error("failed to add KISS interface [%s], error: %d", argv[1], error);
      exit(1);
  }


	// Static Routing
	// Setup default routing table based on CSP address
	// Ground Segment Addresses 8 to 15
	// Space Segment Addresses	0 to 7
	// Reserved Addresses 0 = net base, 31 = broadcast add
	csp_route_set(CSP_GROUND_STATION_ADDRESS, csp_if_kiss, CSP_NODE_MAC);		// Ground Station:	9	=> kiss
	csp_route_set(CSP_BROADCAST_ADDR, csp_if_kiss, CSP_NODE_MAC);			// Broadcast:		32	=> kiss
	// Start the router task
	csp_route_start_task(CONF_CSP_TASK_STACK_SIZE, CONF_CSP_TASK_PRIORITY);
 	printf("Route table\r\n");
	csp_route_print_table();
	printf("Interfaces\r\n");
	csp_route_print_interfaces();
}



/**
 * @brief Set Up the SFSF services
 *
 * This is the third function called by the main.
 * In this function the user should set the SFSF Services
 * dependencies. For example the Command Table, Parameter Table,
 * or change the Telemetry collector or Log timestamp generator function.
*/
void set_up_services(void)
{
	// Set Parameter Table
	set_param_table(&mission_param_table,  sizeof( mission_param_table)/sizeof(*mission_param_table));

	// Set the Command Table, see cmd_table.c for the Command Table
	set_cmd_table(&mission_cmd_table, sizeof( mission_cmd_table)/sizeof(*mission_cmd_table));
}


/**
 * @brief	 Last init calls
 *
 * Some platforms ( OS and/or Hardware) require to run some routines
 * to make effective previous function calls. This function is optional.
*/
void late_init_routine(void)
{
	// You dont need this for linux
}
