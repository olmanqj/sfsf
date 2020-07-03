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
 * @brief	Application Task
 * @example app_task.c
 * Application Task Example

Application Task
================
This is an example of how to create an Application task.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// CSP Includes
#include <csp/csp.h>
#include <csp/arch/csp_thread.h>

// Framework Services
#include <sfsf.h>
#include <sfsf_debug.h>
#include <sfsf_log.h>
#include <sfsf_hk.h>
#include <sfsf_cmd.h>
#include <sfsf_param.h>
#include <sfsf_time.h>

// Mission config
#include <mission_config.h>



param_handle_t time_stamp_h;
void update_parameter_table(void)
{
	// At first call, populate handlers		// TODO get handle by Index
	if(time_stamp_h ==  NULL)
	{
		time_stamp_h = get_param_handle_by_name("time_stamp");
	}
	// Update timestamp
	get_timestamp_str((char*)time_stamp_h->value, time_stamp_h->size);
}



// CSP structs for app
csp_socket_t *  server_socket;
csp_conn_t *    new_conn;
csp_packet_t *  in_csp_packet;
cmd_exit_status_t cmd_exit_status;
// General purpose Buffer
char aux_message_buff[128];

/**
 * @brief	Application Task, execute the main application loop
 *
 * In this function user should write the endless loop that represents
 * the actual application routine. This routine usually resets the
 * watchdog timers, check for incoming commands and executes the routines
 * for the specific mode of operation for the mission.
*/
CSP_DEFINE_TASK(app_task);
CSP_DEFINE_TASK(app_task)
{
	// Bind port to socket
	server_socket = csp_socket(CSP_SO_NONE);
	csp_bind(server_socket, CSP_ANY);
	// Set socket to listen for incoming connections
	csp_listen(server_socket, 5);

	// Log Start Event
	log_print("System Start");

	// Print the parameter table, just for debug
	print_pram_table(); // TODO For debugging, remove for final build

	sync_timestamp(1829098090); // TODO remove this just for debug

	// Start Beacon Storage and Broadcast
	resume_hk_storage();
	resume_hk_broadcast();

	// Main Application Loop
    while(1)
    {
		// Clear Software and hardware Watchdog timers
		reset_sw_wdt();

		// Update parameter table
		update_parameter_table();


		print_debug("APP>\tWaiting for commands\n");

		// Check for new commands
        // Wait for a new connection
        if( (new_conn = csp_accept(server_socket, APP_CHECK_NEW_CMD_PERIOD)) == NULL ) continue;

        // Clear packet for incoming messages
		if(in_csp_packet!= NULL) csp_buffer_free(in_csp_packet);

		// Read new packets
        while( (in_csp_packet = csp_read(new_conn, 200)) != NULL )
        {
			// Select corresponding port and attend command. Note CSP reserved ports:
			//csp_reserved_ports_e {
			//CSP_CMP				= 0,
			//CSP_PING				= 1,
			//CSP_PS				= 2,
			//CSP_MEMFREE			= 3,
			//CSP_REBOOT			= 4,
			//CSP_BUF_FREE			= 5,
			//CSP_UPTIME			= 6,
			//CSP_ANY				= (CSP_MAX_BIND_PORT + 1),
			//CSP_PROMISC			= (CSP_MAX_BIND_PORT + 2)
            switch( csp_conn_dport(new_conn))
            {
				// If is a command, try to process it
                case CSP_OBC_PORT_CMD:
					// Log in Command Event
					sprintf(aux_message_buff, "In Cmd %x", in_csp_packet->data[0] );
					log_print(aux_message_buff);
					// Call Command Handler to process command
					cmd_exit_status = command_handler(new_conn, in_csp_packet);
					// Log Command Exit Status
					sprintf(aux_message_buff, "Command %x exit status: %d",in_csp_packet->data[0], cmd_exit_status );
					log_print(aux_message_buff);
                    break;
                default:
					// CSP Services Handler attends reserved Ports Services
                    csp_service_handler(new_conn, in_csp_packet);
                    break;
            }
        }
        // Close connection
        if (new_conn) csp_close(new_conn);
    }
	return CSP_TASK_RETURN;	// Should never reach here
}
