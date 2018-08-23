/*
Copyright 2018 olmanqj
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */ 
#include <stdlib.h>

// CSP Includes
#include <csp.h>
#include <csp_thread.h>
#include <csp_queue.h>

// Framework Includes
#include "sfsf.h"
#include "sfsf_debug.h"
#include "sfsf_cmd.h"

// The argument list size for command routines = size of CSP buffer - command header (2 bytes)
#define CONF_CMD_ARG_LIST_SIZE CONF_CSP_BUFF_SIZE-2

// Size of commands queue
uint16_t cmd_queue_size;
// Handler of the Command Queue
csp_queue_handle_t cmd_queue;
// Handler of the Command Response Queue
csp_queue_handle_t cmd_response_queue;
// Task handler
csp_thread_handle_t handle_cmd_service_task;
// Period to check Events
uint32_t event_chech_period;
// Pointer to command table
cmd_handle_t* cmd_table_p;
// Size of command Table
uint16_t cmd_table_size_v;



// Command Service Task
CSP_DEFINE_TASK( cmd_service_task )
{
	while(1)
	{
		csp_sleep_ms(event_chech_period);	

		#if CONF_CMD_DEBUG == ENABLE
		print_debug("CMD>\tChecking Cmd Queue\n");
		#endif
	}
	return CSP_TASK_RETURN;	
}


// Init HK Service for space segment
int init_cmd_queue()
{
	// Check if Comand Table exists, if not exit
	if(cmd_table_p == NULL || cmd_table_size_v < 1 ){
		#if CONF_CMD_DEBUG == ENABLE
		print_debug("CMD>\tInit Command Queue Fails! Set Comand Table befor init Cmd Queue!\n");
		#endif
	 	return EXIT_FAILURE;
	}
	// Set init params
	cmd_queue_size = CONF_CMD_QUEUE_SIZE;
	event_chech_period = CONF_CMD_EVENT_CHECK_PERIOD;
	//Create hk Service Task
    return csp_thread_create(cmd_service_task, "CMD_SERV_TASK", CONF_CMD_TASK_STACK_SIZE, NULL, CONF_CMD_TASK_PRIORITY, &handle_cmd_service_task);
}


// Set the Command Table
int set_cmd_table(cmd_table_t * cmd_table, uint16_t cmd_table_size)
{
	// Check args
	if(cmd_table == NULL || cmd_table_size < 1 ) return EXIT_FAILURE;
	// Set init params
	cmd_table_p = (cmd_handle_t*) cmd_table;
	cmd_table_size_v = cmd_table_size; 
	#if CONF_CMD_DEBUG == ENABLE
	print_debug("CMD>\tCommand Table OK!\n");
	#endif
	return EXIT_SUCCESS;
}

// Count amount of comma separated values (tokens) in a string 
int	count_csv(char * str_buff)
{
	int token_count = 0;	// To count how many tokens in string
	size_t token_len;	// Len of current token
	const char *token_p = str_buff;	// Aux pointer to string, points to the start of the current token
	do {
		// Find the next delimiter char in string
		token_len = strcspn(token_p, CONF_CMD_ARGS_DELIMITERS);
		// Place token pointer at new token begin position
		token_p += token_len;
		// If current token has at least one byte it is a valid token
		if(0 < token_len ) token_count++;
	// Repeat for the whole string, i.e. while next char is not end of str 
	} while (*token_p++);
	// Return the count
	return token_count;
}





//Store at out_buff the next Argument from cmd_packet Argument List buff
int get_next_arg(cmd_packet_t* cmd_packet, char * out_buff)
{
	size_t token_len = 0;	// Len of current token
	// Find the next delimiter char in string
	do {
		token_len = strcspn(cmd_packet->cmd_next_arg_p, CONF_CMD_ARGS_DELIMITERS);
		// If current token has at least one byte it is a valid argument
		if(0 < token_len ) break;
	// Repeat until find a token or is not end of str 
	} while (*cmd_packet->cmd_next_arg_p++);
	// Check out_buff is enough big
	if (token_len > strlen(out_buff)) EXIT_FAILURE;
	// Store argument in out_buff
	strncpy(out_buff , cmd_packet->cmd_next_arg_p, token_len);
	// Update argument pointer, place it at argument begin position
	cmd_packet->cmd_next_arg_p += token_len;
	// Return the count
	return token_len;
}




// Reset next argument pointer, in order to be able to retrieve first argument again
void rewind_arg_list(cmd_packet_t* cmd_packet)
{
	cmd_packet->cmd_next_arg_p = *cmd_packet->cmd_arg_list;
}



// Decodes a string from a CSP packet, and returns a cmd-pack struct with the command info  
int decode_cmd_message(csp_packet_t *  in_csp_packet, cmd_packet_t * out_cmd_packet)
{
	uint32_t str_len;
	// Truncate packet data to avoid read trash
	in_csp_packet->data[in_csp_packet->length] = '\0';
	// Command packet need at least 2 bytes, 1 byte for command code and 1 byte for event type
	if(in_csp_packet->length < 2) return EXIT_FAILURE;
	// First Byte of data is the command code
	out_cmd_packet->cmd_code = in_csp_packet->data[0];
	// Check if the command code correspond to a command table entry
	if(get_cmd_table_entry(out_cmd_packet->cmd_code) == NULL) return CMD_UNKNOWN;
	// Second Byte of data is the event trigger type
	out_cmd_packet->trigger_type = in_csp_packet->data[1];
	// Check if the trigger type is an actual trigger_type_t, i.e.: [1,TRIGGER_TYPE_COUNT-1]
	if(out_cmd_packet->trigger_type < 1 || TRIGGER_TYPE_COUNT <= out_cmd_packet->trigger_type ) return EXIT_FAILURE;
	// The remaining data are the arguments for the specific command
	// Calculate if there is remaining data
	str_len = in_csp_packet->length - 2;
	// Check if size of string with argument list is too big
	if(str_len > CONF_CMD_ARG_LIST_SIZE) return EXIT_FAILURE;
	// Copy string with arguments to cmd_packet argument list buffer
	if(str_len > 0 ) strcpy( out_cmd_packet->cmd_arg_list, in_csp_packet->data+2 );
	// Point the argument pointer to the first argument in list 
	out_cmd_packet->cmd_next_arg_p = &out_cmd_packet->cmd_arg_list;
	// If  cmd_args_num at corresponding table entry is different to ARGS_NUM_ANNY, then check that the amount of arguments in list match to cmd_args_num
	if(get_cmd_table_entry(out_cmd_packet->cmd_code)->cmd_args_num != ARGS_NUM_ANNY )
	{
		// Count how many csv arguments are in the argument list, check if match with corresponding cmd_args_num at table entry
		if( count_csv(out_cmd_packet->cmd_arg_list) != get_cmd_table_entry(out_cmd_packet->cmd_code)->cmd_args_num ) return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}



// Run the command routine
cmd_exit_status_t cmd_work(csp_conn_t *conn, cmd_packet_t * cmd_packet)
{
	cmd_handle_t * cmd_handle;
	csp_packet_t * response_packet;
	// Get command routine handle from command table by the command code
	cmd_handle = get_cmd_table_entry(cmd_packet->cmd_code);	
	// If cmd_packet invalid or command not found, response CMD_UNKNOWN
	if(cmd_packet == NULL || cmd_handle == NULL)
	{
		response_packet = csp_buffer_get(CONF_CMD_ARG_LIST_SIZE);
		if(send_message(conn, response_packet, "CMD_UNKNOWN")!= EXIT_SUCCESS) return CMD_SEND_FAIL;
		return CMD_UNKNOWN;
	}
	return cmd_handle->cmd_routine_p(conn, cmd_packet);
}



// Struct to encapsulate command info
cmd_packet_t cmd_packet;
// Process an incoming command, 
// This function is not reentrant, not thread-safe, i.g. process one command at the time
cmd_exit_status_t command_handler(csp_conn_t *conn, csp_packet_t *in_csp_packet)
{
	int decode_result;
	// Clear cmd_packet, erase data form past operations
	bzero(&cmd_packet, sizeof(cmd_packet));
	// Decode message and encapsulate in cmd struct
	decode_result = decode_cmd_message(in_csp_packet, &cmd_packet);
	// Free csp_packet, already used
	csp_buffer_free(in_csp_packet);
	// If decode Fails
	if(decode_result !=EXIT_SUCCESS)
	{		
		// Response with Fail message
		send_message(conn, in_csp_packet, "CMD_DECODE_FAIL");
		return CMD_DECODE_FAIL;
	}
	// If trigger_type is ON_REAL_TIME, execute immediately			// TODO block cmd_task to avoid a collision 
	if(cmd_packet.trigger_type==ON_REAL_TIME)
	{
		return cmd_work(conn, &cmd_packet);
	}
	// Else Enqueue for event detection
	send_message(conn, in_csp_packet, "CMD_ENQUEUED_FAIL");
	return CMD_ENQUEUED_FAIL;
	// TODO Implement Enqueue Commands
	send_message(conn, in_csp_packet, "CMD_ENQUEUED_OK");
	return CMD_ENQUEUED_OK;
}





// Get a command table entry by the given code
cmd_handle_t * get_cmd_table_entry(uint8_t cmd_code)
{
	int i;
	for(i = 0; i< cmd_table_size_v; i ++)
	{
		// If command code found, return pointer to it containing handle through dest_handle
		if(cmd_table_p[i].cmd_code ==  cmd_code ) 
		{
			// Check the handle has a pointer to a fn
			if(cmd_table_p[i].cmd_routine_p == NULL) return (cmd_handle_t *) NULL;
			// Return the table entry
			return &cmd_table_p[i];
		}
	}
	// If not found
	return (cmd_handle_t *) NULL;
}





// Return the service task handle
csp_thread_handle_t get_cmd_task_handle(void)
{
	return handle_cmd_service_task;
}




// TODO place elsewhere
int send_message( csp_conn_t * connection, csp_packet_t * csp_packet, const char *  message_buff)
{
	// If packet NULL, try to get new one
	if( csp_packet == NULL ) csp_packet = csp_buffer_get( strlen(message_buff) );
    // Exit Fail if no mem for new packet
	if( csp_packet == NULL ) return EXIT_FAILURE; 
	// Store message in packet
	strcpy( csp_packet->data, message_buff);  
    // Store message size
    csp_packet->length = strlen(message_buff);
    // Send message 
    if(csp_send(connection, csp_packet, 1000) == 0)	
    { 
    	csp_buffer_free(csp_packet);
		return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}


