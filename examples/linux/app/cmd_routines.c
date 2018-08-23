/*
Copyright 2018 olmanqj
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */ 

/**
 * @brief	Command Routines
 * @example cmd_routines.c
 * Command Routines Example

 
Command Routines
================
This is an example of how to create a Command Routines.



*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// CSP Includes
#include <csp.h>
#include <csp_system.h>
#include <csp_thread.h>
#include <csp_queue.h>

// Framework Services Includes
#include "sfsf.h"
#include "sfsf_param.h"
#include "sfsf_cmd.h"
#include "sfsf_log.h"

// Mission config
#include "mission_config.h"


// DEFINE_CMD_ROUTINE receives (csp_conn_t *conn, cmd_packet_t * cmd_packet)
DEFINE_CMD_ROUTINE(dummy)
{
	csp_packet_t * response_packet;
	if((response_packet = csp_buffer_get(CSP_BUFFER_SIZE))==NULL) return CMD_FAIL;
	if(send_message(conn, response_packet, "OK")!= EXIT_SUCCESS) return CMD_SEND_FAIL;
	return CMD_OK;
}




DEFINE_CMD_ROUTINE(cmd_get_param)
{
	csp_packet_t * response_packet;
	param_handle_t param_h;
	char arg_value_buff[64]={0};
	char param_name[CONF_PARAM_NAME_SIZE]={0};
	int arg_size;
	// Get new Packet
	if((response_packet = csp_buffer_get(CSP_BUFFER_SIZE))==NULL) return CMD_FAIL;
	// Clear packet
	bzero(response_packet->data, CSP_BUFFER_SIZE);
	// Look for next command arguments until no args
	while( (arg_size=get_next_arg(cmd_packet, param_name))!=0 )
	{
		// look if exists requested param
		if((param_h =  get_param_handle_by_name(param_name))==NULL)
		{
			// If param not found, respond with error
			snprintf(arg_value_buff, sizeof(arg_value_buff), "not_found");
		}
		else
		{
			// If found, convert value into arg_value_buff
			param_to_str(param_h, arg_value_buff, sizeof(arg_value_buff));
		}
		// If no more space in csp response packet then finish
		if((strlen(response_packet->data)+arg_size+2) >= CSP_BUFFER_SIZE) break;
		// Separate by comma, except if first
		if(strlen(response_packet->data) != 0)strcat(response_packet->data,   "," );
		// Concatenate name of param into response packet
		strcat(response_packet->data,   param_name);
		strcat(response_packet->data,   ":" );
		// Concatenate value as str into response packet
		strcat(response_packet->data,  arg_value_buff );
		// Clear buffers for next arg
		bzero(arg_value_buff, sizeof(arg_value_buff));
		bzero(param_name, sizeof(param_name));
	}
	// Store message size
	response_packet->length = strlen(response_packet->data);
	// Send message
	if(csp_send(conn, response_packet, 1000) == 0)
	{
		csp_buffer_free(response_packet);
		return CMD_SEND_FAIL;
	}
	return CMD_OK;
}





DEFINE_CMD_ROUTINE(cmd_set_param)
{
	csp_packet_t * response_packet;
	param_handle_t param_h;
	char arg_value_buff[64]={0};
	char param_name[CONF_PARAM_NAME_SIZE]={0};
	int arg_size;
	// Get new Packet
	if((response_packet = csp_buffer_get(CSP_BUFFER_SIZE))==NULL) return EXIT_FAILURE;
	// Clear packet
	bzero(response_packet->data, CSP_BUFFER_SIZE);
	// Look for next command arguments until no args,
	while( (arg_size=get_next_arg(cmd_packet, param_name))!=0 )
	{
		// Get another Argument, set_command need two Arguments (param name and the value)
		if(get_next_arg(cmd_packet, &arg_value_buff)==0 )
		{
			// If not second argument, respond with error
			bzero(arg_value_buff, sizeof(arg_value_buff));
			snprintf(arg_value_buff, sizeof(arg_value_buff), "too_few_args");

		}
		// Look if exists requested param
		else if((param_h =  get_param_handle_by_name(param_name))==NULL)
		{
			// If param not found, respond with error
			bzero(arg_value_buff, sizeof(arg_value_buff));
			snprintf(arg_value_buff, sizeof(arg_value_buff), "not_found");
		}
		else
		{
			// If found, try set arg_value_buff to param
			if(str_to_param(param_h, arg_value_buff)==EXIT_SUCCESS)
			{
				// if OK, Response with OK
				bzero(arg_value_buff, sizeof(arg_value_buff));
				snprintf(arg_value_buff, sizeof(arg_value_buff), "ok");
			}
			else
			{
				// Else  Response with FAIL
				bzero(arg_value_buff, sizeof(arg_value_buff));
				snprintf(arg_value_buff, sizeof(arg_value_buff), "fail");
			}
		}
		// If no more space in csp response packet then finish
		if((strlen(response_packet->data)+strlen(arg_value_buff)+strlen(param_name)+2) >= CSP_BUFFER_SIZE) break;
		// Separate by comma, except if first
		if(strlen(response_packet->data) != 0)strcat(response_packet->data,   "," );
		// Concatenate name of param into response packet
		strcat(response_packet->data,   param_name);
		strcat(response_packet->data,   ":" );
		// Concatenate set_param exit status as str into response packet
		strcat(response_packet->data,  arg_value_buff );
		// Clear buffers for next arg
		bzero(arg_value_buff, sizeof(arg_value_buff));
		bzero(param_name, sizeof(param_name));
	}
	// Store message size
	response_packet->length = strlen(response_packet->data);
	// Send message
	if(csp_send(conn, response_packet, 1000) == 0)
	{
		csp_buffer_free(response_packet);
		return CMD_SEND_FAIL;
	}
	return CMD_OK;
}



// Reboots the OBC, requires 1 arg the password
DEFINE_CMD_ROUTINE(cmd_reboot_obc)
{
	log_print("Rebooting OBC by command request");
	// TODO check if first argument is the password
	
	// Call CSP reboot 
	csp_sys_reboot();
	// IF reboot should never reach here
	return CMD_SEND_FAIL;
}



