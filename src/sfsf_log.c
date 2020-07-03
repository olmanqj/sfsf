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
#include <stdio.h>

// CSP Includes
#include <csp/csp.h>
#include <csp/arch/csp_thread.h>
#include <csp/arch/csp_queue.h>


// Framework Includes
#include <sfsf.h>
#include <sfsf_debug.h>
#include <sfsf_storage.h>
#include <sfsf_log.h>

// Task handle
csp_thread_handle_t handle_log_service_task;
// Handler of the Log Message Queue
csp_queue_handle_t log_queue;
// Sequence for message id
uint32_t messages_id_seq;
// Frequency for persist storage of log
uint32_t log_persist_frequency;
// Pointer to the function that generates the timestamp into a buffer,
// should be set at init by set_timestamp_generator()
timestamp_generator_t timestamp_generator_fun;
// Buffer to store the log messages before printing
char log_messages_buff[CONF_LOG_QUEUE_SIZE][CONF_LOG_MESSAGE_SIZE];


// Struct of Log Messages
typedef struct
{
	uint32_t message_id;	// TODO check if no necessary
	char*  message_data;	// pointer to position of log_messages_buff where the message is stored
} log_message_t;


// Log Task
// Wait for messages from the Log Queue and print them into the debugging console and Log Files
log_message_t new_message;  // TODO for other files, check big variables are at global context, to lower stack use
char timestamp_buff[20];	// Buff to tore Timestamp before printing
FILE *log_fd;				// log file Descriptor
CSP_DEFINE_TASK( log_service_task )
{
	while( 1 )
	{
		// Receive a message on the created queue.  Block for log_persist_frequency ticks if a message is not immediately available.
		if( csp_queue_dequeue( log_queue, (void*) &new_message , log_persist_frequency ) )
		{
			log_fd = fopen(CONF_LOG_FILE_NAME, "at"); // Try to Open
			if (!log_fd) log_fd = fopen(CONF_LOG_FILE_NAME, "wt"); // Create if not opened
			#if	CONF_LOG_DEBUG == ENABLE
			print_debug("LOG>\t");
			#endif
			// If Timestamp generator function is set, print the Timestamp before the Log message
			if(timestamp_generator_fun)
			{
				// Clear buffer
				bzero(timestamp_buff, sizeof(timestamp_buff));
				// Print timestamp into buffer
				timestamp_generator_fun(timestamp_buff, sizeof(timestamp_buff));
				// If File opened, Print in file
				if (log_fd)  fprintf(log_fd, "%s>",  timestamp_buff);
				// if debug enable, print timestamp in debug out
				#if	CONF_LOG_DEBUG == ENABLE
				print_debug(timestamp_buff);
				print_debug(">");
				#endif
			}
			// If file opened, print Log message into file
			if (log_fd)
			{
				fprintf(log_fd, "%s\n",  new_message.message_data );
				// Close File
				fclose(log_fd);
			}
			// if debug enable, print log message in debug out
			#if	CONF_LOG_DEBUG == ENABLE
			print_debug(new_message.message_data);
			print_debug("\n");
			#endif
		}
	}
	return CSP_TASK_RETURN;	//Never should reach here
}


// Create the log Messages Queue and the Log Task
int init_log_service()
{
	// Init sequencer
	messages_id_seq = 0;
	// Delay to store log
	log_persist_frequency = CONF_LOG_PERSIST_PERIOD;
	log_queue = csp_queue_create( CONF_LOG_QUEUE_SIZE, sizeof(log_message_t) );		 // TODO if message size is param, change this also
	// Queue was not created and must not be used.
	if( log_queue == NULL ) return EXIT_FAILURE;
	// Start Log Task (print into debugging console and log files)
	csp_thread_create( log_service_task,  "LOG_SERV_TASK",  CONF_LOG_TASK_STACK_SIZE,  NULL, CONF_LOG_TASK_PRIORITY,  &handle_log_service_task );
	// All Set
	return EXIT_SUCCESS;
}


// Set the timestamp generator function to print log messages with timestamp
void set_log_timestamp_generator( timestamp_generator_t timestamp_generator)
{
	timestamp_generator_fun = timestamp_generator;
}



// Return the next slot in the log_message_buff
char* get_next_buff_slot()
{
	return (char*) (log_messages_buff + (messages_id_seq % CONF_LOG_QUEUE_SIZE));
}



// Add a Log message into the Log Queue
int log_print(const char *str)
{
	// Handel for the new log message
	log_message_t new_message;
	// If queue don't exists, fail
	if(log_queue ==  NULL) return EXIT_FAILURE;
	// If message to long, fails
	if(strlen(str)>CONF_LOG_MESSAGE_SIZE) return EXIT_FAILURE;
	// If queue is full, fails
	if(csp_queue_size(log_queue) >= CONF_LOG_QUEUE_SIZE) return EXIT_FAILURE;
	// Assign pointer where message should be stored into the log_messga_buff
	new_message.message_data = get_next_buff_slot();
	// Copy message to log_message_buff
	strcpy(new_message.message_data, str);
	// Increment and set the message id
	new_message.message_id = ++messages_id_seq;
	// Enqueue message handle
	csp_queue_enqueue( log_queue, (void*) &new_message,  0 );
	return EXIT_SUCCESS;
}


// Add a Log message with format "key:value" into the Log Queue
int log_print_int(const char *name, int value)
{
	// Handel for the new log message
	log_message_t new_message;
	// If queue don't exists, fail
	if(log_queue ==  NULL) return EXIT_FAILURE;
	// If message to long, fails
	if(strlen(name)>CONF_LOG_MESSAGE_SIZE/2) return EXIT_FAILURE;
	// If queue is full, fails
	if(csp_queue_size(log_queue) >= CONF_LOG_QUEUE_SIZE) return EXIT_FAILURE;
	// Assign pointer where message should be stored into the log_messga_buff
	new_message.message_data = get_next_buff_slot();
	// Copy message
	sprintf(new_message.message_data, "%s:%d", name, value);
	// Increment and set the message id
	new_message.message_id = ++messages_id_seq;
	// Enqueue message handle
	csp_queue_enqueue( log_queue, (void*) &new_message,  0 );
	return EXIT_SUCCESS;
}



int log_print_float(const char *name, float value)
{
	// Handel for the new log message
	log_message_t new_message;
	// If queue don't exists, fail
	if(log_queue ==  NULL) return EXIT_FAILURE;
	// If message to long, fails
	if(strlen(name)>CONF_LOG_MESSAGE_SIZE/2) return EXIT_FAILURE;
	// If queue is full, fails
	if(csp_queue_size(log_queue) >= CONF_LOG_QUEUE_SIZE) return EXIT_FAILURE;
	// Assign pointer where message should be stored into the log_messga_buff
	new_message.message_data = get_next_buff_slot();
	// Copy message
	sprintf(new_message.message_data, "%s:%f", name, value);
	// Increment and set the message id
	new_message.message_id = ++messages_id_seq;
	// Enqueue message handle
	csp_queue_enqueue( log_queue, (void*) &new_message,  0 );
	return EXIT_SUCCESS;
}




csp_thread_handle_t get_log_task_handle(void)
{
	return handle_log_service_task;
}
