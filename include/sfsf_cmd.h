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
#ifndef SFSF_CMD_H_
#define SFSF_CMD_H_

#ifndef SFSF_H_
#error Include sfsf.h before sfsf_cmd.h!
#endif

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @file	sfsf_cmd.h
 * @brief	API for Command Service


Command service
===============


Features Summary
-------------
- Handles incoming commands.
- Decodes commands contained in CSP packets.
- Definitions to create the command table.
- Executes command routines defined in the command table.
- Enqueue commands for event detection. (Still not implemented)


Module Description
-----------------
The Command Service is in charge of providing the ground segment with the
ability of controlling the spacecraft at any time. This module is able to
accept and commands coming from the ground in real-time.

Commands arrive encoded inside packages from the Network layer, therefore this
module is also capable of decoding an interpreting the content of a command
package.

Normally, ground segment expects a result for each command sent to the
spacecraft, therefore this service is capable of capturing the result from any
command routine and sending it back to the ground.

Depending on the mission requirements, storing commands is also necessary, this
provides the ability to execute commands, even during the section of the orbit
without a communication link with the ground. For this, the command packages
should also specify when to execute the command routine, this can be specified
with an eventual condition, for example, when the time reaches a certain value,
or when the GPS coordinates are between a certain range, or even when the
battery charge drops under a certain threshold. For simplicity, this framework
treats time-based conditions as other event condition, taking the time stamp as
the trigger parameter. See trigger_type_t.

The specific action performed by a command will be called command routine. The
amount of command routines needed and the actions performed by each command
routine are specify of the mission and shall be implemented by the user. This
is done by creating a command table with the cmd_handle_t type, and command
routines with the DEFINE_CMD_ROUTINE definition.


Command Format
--------------
The following table represents the format of the encoded command within an CSP
packet.

| Cmd Code      | Trigger Type  | Argument List (Comma Separated)  |
| ------------- | ------------- | -------------------------------- |
| 0			        | 1				      | 3 to 	CONF_CSP_BUFF_SIZE		     |

1. Cmd code:
is the specific code that identifies a command, the code
and the corresponding command routine should be specified in the command
table (see cmd_table_t type).

2. Trigger Type:
specifies when to execute the command, see trigger_type_t.
If Trigger Type is ON_REAL_TIME, the command will be executed immediately.
Otherwise will be enqueued until the trigger condition occurs.
If Trigger Type is different to ON_REAL_TIME, an on-board parameter name and a
value should be specified to trigger the event. In this case, the first
Argument from the Argument List is expected to be the on-board parameter name,
and the following Argument the value to compare the on-board parameter with. If
Trigger Type is ON_IN_BETWEEN, then an additional Argument should be specified.
The remaining Arguments on Arguments List will be passed to the specific
command execution.

3. Argument List:
command routines may require input arguments, for example changing the
satellite's pointing direction requires as arguments the new direction.
The Argument List contains the arguments for the execution of the command
routine. Arguments are contained in a string, each value speared by comma.
See get_next_arg() and  rewind_arg_list() to retrieve arguments contained in a
cmd_packet.


Command Table
-------------
The Service needs a way to know which action to execute, when arrives a
command. For this the user needs to create a Command Table with the typedef
cmd_table_t. The table contains the corresponding code for a command, a
trigger_type_t flag that indicate when to execute the command, and a pointer
to the corresponding routine which should attend the command, this routine
has to be defined with DEFINE_CMD_ROUTINE().

@b Example:

First define the routines that should attend the commands with
DEFINE_CMD_ROUTINE().
@code
DEFINE_CMD_ROUTINE(dummy)
{
	// Here the code you want the command to perform
	return CMD_OK;
}
@endcode

Then create the table with cmd_table_t and add the corresponding command
code and the pointer to the routine.
@code
cmd_table_t mission_cmd_table = {
	//Code of command	Amount of args			Pointer to routine
	{.cmd_code = 1,		.cmd_args_num = 0,		.cmd_routine_p = &dummy},
};
@endcode


Then register the table during initialization, in init_services() function
at init_functions.c.
@code
set_cmd_table(&mission_cmd_table, sizeof( mission_cmd_table)/sizeof(*mission_cmd_table));
@endcode



Handling Commands
-----------------
Once with the table and the routines ready. Incoming commands can be
executed usin the function command_handler(). This is the key function from
the Command Service. When a packet arrives with a command call this function
to handle it. This decodes the command from the CSP packet, look-up the
Command Table to find the corresponding routine to execute the activity.
If command is ON_REALTIME executes the command immediately , otherwise enqueue
the command for eventual execution.

The Application task should be in charged of checking for new packages with
commands, and when one arrives, to call command_handler().


Command Routines
----------------
As stated before command routines are defined by user with the macro
DEFINE_CMD_ROUTINE(), and are executed when a command with the correspondign
code arrives. Inside this functions the user is allowed to write the desired
code to attend the command. For example if the command is take a picture,
the routine code should communicate with a camera to take a picture.

Note that the command routines receive two arguments: the CSP connection,
and the CSP packet. With the connection the routine is able to send messages
back to ground, using the CSP API. And with the CSP packet, the routine is able
to retrieve the incoming arguments for performing the required action.
For example, if the command is to set a parameter, the packet will contain two
arguments: the name of the parameter and the value to set to the parameter.
This arguments can be retrieved with get_next_arg().

*/


/**
 * @enum	cmd_exit_status_t
 * @brief	Types of exist status of processing a command
 */
typedef enum
{
	CMD_ENQUEUED_OK   =  1,
	CMD_OK            =  0,
	CMD_UNKNOWN       = -1,
	CMD_FAIL          = -2,
	CMD_DECODE_FAIL   = -3,
	CMD_SEND_FAIL     = -4,
	CMD_ENQUEUED_FAIL = -5,
} cmd_exit_status_t;

/**
 * @enum	trigger_type_t
 * @brief	Contains the Type of Triggers for Event detection
 */
typedef enum
{
	ON_REAL_TIME          = 1,	 /**< Immediately */
	ON_EQUALS             = 2,	 /**< == */
	ON_NOT_EQUALS         = 3,	 /**< != */
	ON_LESS               = 4,	 /**<  < */
	ON_LESS_OR_EQUAL      = 5,	 /**< <= */
	ON_GREATER            = 6,	 /**< > */
	ON_GREATER_OR_EQUAL   = 7,	 /**< >= */
	ON_IN_BETWEEN         = 8,	 /**< < < */
	TRIGGER_TYPE_COUNT    /**< Keep this always at last position in enum. */
} trigger_type_t;

// TODO Implement
/* \struct event_checker_t
 *	\brief Struct with the handle of a Event checker
 *
 *	Contains the  trigger type for event detection,
 *  the amount of params to perform the relational comparasion,
 *  and a pointer to the routine which performs the comparasion.
 */
/*
typedef struct
{
	// Trigger type id
	trigger_type_t trigger_type;
	// Number params expected to perform the event detection
	uint8_t trigger_params_num;
	// Pointer to routine that performs the event check corresponding to the trigger type
	int (* event_check_routine_p)(void*, void*, param_type_t);
} event_checker_t;
*/

/**
 * @struct	cmd_packet_t
 * @brief	Struct with info of a incoming command
 *
 * Contains the command code, the trigger type for event detection,
 * a pointer to the t argument and a buffer with the arguments
 * separated by comma.
 */
typedef struct
{
	uint8_t cmd_code;				/**<	Code of Command, should be defined by the user. */
	trigger_type_t trigger_type;	/**< 	Trigger for event detection, see trigger_type_t */
	char * cmd_next_arg_p;			/**<	Pointer to retrieve arguments one by one, use get_next_arg() and rewind_arg_list().*/
	char cmd_arg_list[CONF_CSP_BUFF_SIZE-2]; /**< Buffer to store arguments for command, use get_next_arg() and rewind_arg_list(). */
} cmd_packet_t;




/**
 * @typedef		cmd_routine_t
 * @brief		Type of a command routine
 *
 * Use DEFINE_CMD_ROUTINE() to create command routines.
 */
typedef cmd_exit_status_t (*cmd_routine_t) (csp_conn_t *conn, cmd_packet_t * cmd_packet);	// TODO define cmd routine in cmd_handle_t with this type



/**
 * @struct	cmd_handle_t
 * @brief	Struct with the handle of a Command Routine
 *
 * Contains the command code corresponding to the routine,
 * the number of arguments expected and the pointer to the routine
 * with the command work. The command table is composed by a
 * collection of this struct.
 * @see cmd_table_t
 */
typedef struct
{
	const uint8_t cmd_code;		/**< Code of Command. */
	const uint8_t cmd_args_num;	/**< Expected amount of arguments to receive, if any amount expected use ARGS_NUM_ANNY. */
	const cmd_exit_status_t (* cmd_routine_p)(csp_conn_t*, cmd_packet_t*);	/**< Pointer to command routine, use DEFINE_CMD_ROUTINE to define the function. */
} const cmd_handle_t;

/**
 * @typedef		cmd_table_t
 * @brief		Type to define the command table
 * @note		The table should be registered at init with the function set_cmd_table()
 *
 * **Example**:
 * @code
 *	cmd_table_t mission_cmd_table = {
 *		//Code of command	Amount of args                     Pointer to routine
 *		{.cmd_code = 1,		.cmd_args_num = 0,                .cmd_routine_p = &dummy},
 *		{.cmd_code = 2,		.cmd_args_num = ARGS_NUM_ANNY,    .cmd_routine_p = &cmd_get_param},
 *		{.cmd_code = 3,		.cmd_args_num = ARGS_NUM_ANNY,    .cmd_routine_p = &cmd_set_param},
 *		{.cmd_code = 4,		.cmd_args_num = 0,                .cmd_routine_p = &cmd_reboot_obc}
 *	};
 * @endcode
*/
typedef const cmd_handle_t cmd_table_t[];

/**
 * @def		ARGS_NUM_ANNY
 * @brief	Define a table entry, of command table, that expect any amount of arguments
*/
#define ARGS_NUM_ANNY 0xff

/**
 * @def		DEFINE_CMD_ROUTINE
 * @brief	Define functions as Command Routines
 *
 * Define Command Routines with this macro, command routines should receive a CSP connection.
 * and a cmd_packet_t, and return a cmd_exit_status_t.
 * Functions defined with this macro are able to send response messages with the CSP connection
 * argument conn, and to retrieve the arguments in the CSP packet cmd_packet with  the function
 * get_next_arg().
*/
#define DEFINE_CMD_ROUTINE(cmd_routine_name) cmd_exit_status_t cmd_routine_name(csp_conn_t *conn, cmd_packet_t * cmd_packet)

/**
 *	@brief Start Command Queue task, for executing commands based on events.
 *	@return	-1 if error , 0 if OK
 */
int init_cmd_queue(void);

/**
 * @brief	Register the Command Table
 *
 * Call this function during initialization, in init_services() function
 * at init_functions.c.
 *
 * @param	cmd_table			Pointer to Command Table
 * @param	cmd_table_size 		Size of command Table
 * @return	-1 if error , 0 if OK
 */
int set_cmd_table(cmd_table_t * cmd_table, uint16_t cmd_table_size);

/**
 * @brief	Decodes a CSP packet with a command.
 *
 * Decodes a string from a CSP packet, and creates a cmd-pack struct with the command info
 * This means, extract the command code, the event trigger type and the argument list,
 * also check if this info (command code and amount of arguments) match with info in command table
 *
 * @param	in_csp_packet		CSP Packet with command info
 * @param	out_cmd_packet 		Pointer to a cmd_packet_t to store command info
 * @return	-1 if error , 0 if OK
 */
int decode_cmd_message(csp_packet_t *  in_csp_packet, cmd_packet_t * out_cmd_packet);

/**
 * @brief	Decode and Executes a command packet in a csp_packet_t
 *
 * This is the key function from the Command Service. When a packet arrives with a command
 * call this function to handle it. This decodes the command from the CSP packet, look-up
 * the Command Table to find the corresponding routine to execute the activity.
 * If command is ON_REALTIME executes the command immediately , otherwise enqueue the
 * command for eventual execution.
 *
 * @warning This function is not reentrant, not thread-safe, i.g. process one command at the time
 * @param	conn			Pointer to the new connection
 * @param	packet			Pointer to the packet, obtained by using csp_read()
 * @return	cmd_exit_status_t
 */
cmd_exit_status_t command_handler(csp_conn_t *conn, csp_packet_t *packet);

/**
 * @brief	Count amount of comma separated values
 *
 * Count amount of comma separated values (tokens) in a string, usefully
 * to count the amount of Arguments within a command routine.
 *
 * @param	str_buff		Pointer to the string
 * @return	amount of comma separated values, 0 if none
 */
int	count_csv(char * str_buff);

/**
 * @brief	Retrieves the next argument in Argument List
 *
 * Store at out_buff the next Argument from cmd_arg_list buff.
 * If all args already retrieved, use rewind_arg_list() to start from the first again.
 *
 * @param	cmd_packet	pointer to cmd_packet with the Argument List
 * @param	out_buff	Destination buffer to store next arg, should be big enough
 * @return	size of retrieved param, 0 if nothing retrieved or end of args
 */
int get_next_arg(cmd_packet_t* cmd_packet, char * out_buff);

/**
 * @brief	Rewind the argument list
 *
 * Reset Argument List pointer, in order to be able to retrieve first Argument again
 * with get_next_arg()
 *
 * @param	cmd_packet	Pointer to cmd_packet with Argument List pointer to reset
 * @return	void
 */
void rewind_arg_list(cmd_packet_t* cmd_packet);

/**
 * @brief	Get a command table entry by the given code
 * @param	cmd_code			Command code
 * @return	NULL if nor found, cmd_handle_t pointer if OK
 */
cmd_handle_t * get_cmd_table_entry(uint8_t cmd_code);

/**
 * @brief	Get Service Task Handle
 * @return	csp_thread_handle_t
 */
csp_thread_handle_t get_cmd_task_handle(void);

/**
 * @brief	Send message // TODO decouple
 * @param	connection		CSP Connection
 * @param	csp_packet		CSP Packet empty
 * @param	message_buff	Message string to send
 * @return	-1 if error , 0 if OK
 */
int send_message( csp_conn_t * connection, csp_packet_t * csp_packet, const char *  message_buff);



#ifdef __cplusplus
}
#endif
#endif /* SFSF_CMD_H_ */
