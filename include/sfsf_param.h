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
#ifndef SFSF_PARAM_H_
#define SFSF_PARAM_H_

#ifndef SFSF_H_
#error Include sfsf.h before sfsf_param.h!
#endif

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @file	sfsf_param.h
 * @brief	API for Parameter Service

Parameter Service
=================

To avoid confusion:
- Variable: variables from programming language, a storage location with an
 associated name.
- Argument: one of the pieces of data provided as input to a function.
- Parameter: a characteristic that models or describes a system.


Features Summary
-------------
- Set and get parameters.
- Supports any type of parameter.
- Definitions to create the parameter table.
- Automatically collects parameters with Telemetry option.
- Automatically stores parameters in persistent memory. (Still not implemented)
- Parameters protection with read only option.


Module Description
-----------------
The Parameter Service acts as a database for the spacecraft's parameters,
provides an easy way to set and get the value of any parameter. A parameter
can be described as a variable that helps model or describe a system, it can
be for example the pointing direction of the ADCS, or the output voltage of
the EPS, or even if the communication system is on or off.

Implementing the mission specific application using the Parameters Service
instead of built-in variables from the platform, facilitates the control and
monitoring of the spacecraft, because any parameter value can be modified or
retrieved at any time. Parameters can be accessed by a name or by an index,
this gives the ability to modify or retrieve the value of a parameter from
the ground.

The Parameter Table
-------------------
Parameters are stored in a table, the "Parameters Table". Each entry of the
table is of type param_t, which contains the name, type, size, options and
a pointer to the value of the parameter. In other words the Parameters Table
is an array of param_t.

There can only be one table and should be created by the user, with the type
param_table_t. It should be registered at init with the function
set_param_table(). The table isstatic and it is created at compilation, this
means new parameters can not be added at run time. Therefore, users should
include in the table, all required parameters during coding. There are two
 ways to add parameters to the table:

- When the parameter does not exist, a new memory space can be created to store
its value. This is done by adding a param_t struct to the table, as following:
@code
{.param_name = "example1", .type = UINT8_PARAM, .size = UINT8_SIZE,	.opts = TELEMETRY|PERSISTENT },
@endcode

- When the parameter is an existing variable, the variable can be "parameterized"
and added to the table. This is done with the macro parameterize(), by adding a
line as the following to the table:
@code
{.param_name = "example1", .type = UINT8_PARAM, .size = UINT8_SIZE,	.opts = TELEMETRY|PERSISTENT, parameterize(variable_name) },
@endcode

As shown in the two last examples, parameters require:
- A unique name, the max name size is determined by the configuration
 CONF_PARAM_NAME_SIZE.
- Type: all supported types are enlisted in enum param_type_t.
- Sze: size can be assigned with the macro sizeof(), or with the values of enum
 param_size_t. For a parameter of type STRING_PARAM the size can be any, it
 should be decided by the user.
- Options: there are some special options that can be assigned to parameters,
 these are enlisted in enum param_opts_t. Options are not mandatory,
 parameters can have no options. All parameters with option TELEMTRY can be
 collected into a string with the function collect_telemetry_params().
 All parameters with option PERSITENT will be stored in persistent memory. This
 is useful for restoring the configuration after a reboot from OBC. Note that
 init_param_persistence() should be called at init and Storage Service functions
 should be ported to make effective the PERSOSTENT option. The option READ_ONLY
 is only applicable for parameters created with the parameterize() macro, this
 inhibits the parameter value to be modified throughout the Parameter Service
 API. This option is useful when there are parameters that should not be
 modified from the ground, but retrieved, like a counter, or the output of a
 sensor.

An example of a Parameters Table:
@code
// Variables to be parameterized
uint32_t	variable_name;
float		variable2_name;
// Parameters Table
param_table_t mission_param_table = {
	//	NAME					TYPE				Size				Options							Variable
	{.name="example1",	.type=UINT8_PARAM,	.size=UINT8_SIZE,	.opts=TELEMETRY},
	{.name="example2",	.type=STRING_PARAM,	.size=20,			.opts=TELEMETRY|PERSISTENT|READ_ONLY},
	{.name="example3",	.type=UINT32_PARAM,	.size=UINT32_SIZE,}, // Params may have no options
	{.name="example4",	.type=UINT32_PARAM,	.size=UINT32_SIZE,	.opts=TELEMETRY|PERSISTENT,				.value=parameterize(variable_name)},
	{.name="example5",	.type=FLOAT_PARAM,	.size=FLOAT_SIZE,											.value=parameterize(variable2_name)}
};
@endcode

Then register the table during initialization, in init_services() function
at init_functions.c.
@code
set_param_table(&mission_param_table,  sizeof( mission_param_table)/sizeof(*mission_param_table));
@endcode

Handling Parameters
-------------------
Whit a Parameter Table populated and registered, we can retrieve
and modify the value of parameters. You may not access the param_t
structures from table directly. Use the a Parameter Handle instead,
the typedef param_handle_t, which is a pointer to a table entry. So
once you have a handle pointing to a param, the access to the value
is in immediate, there is no need to look-up through the table for
the parameter. There are two ways to obtain a handle, by the name
of the parameter, or by the index on the table.

The easiest, by the name, with the function get_param_handle_by_name().
This functions scans the table for a parameter with the name. If the
parameter is accessed very frequently, is not efficient.

@b Example:
@code
param_handle_t example1_h = get_param_handle_by_name("example1");
@endcode

The other way is by the index in table, using the function
get_param_handle_by_index(). This functions is way more efficient
that the previous one, because there is not need to scan the table,
the access is directly, but you need to keep track of the indexes
in table.

@b Example:
@code
#define EXAMPLE1_INDEX	1	// Recommended to keep indexes as macros
param_handle_t example1_h = get_param_handle_by_index(EXAMPLE1_INDEX);
@endcode

Once with the handle of a param, in other words a pointer to it, the
acces to its value can be done with two functions: get_param_val()
and set_param_val().

To retreive the value of a parameter use get_param_val().

@b Example:
@code
uint8_t aux_var;	// Variable where param value will be stored
param_handle_t example1_h = get_param_handle("example1");
get_param_val(example1_h, (void*)&aux_var );
@endcode

To modify the value of a parameter use set_param_val().

@b Example:
@code
uint8_t example1 = 4;	// Value to set to param
param_handle_t example1_h = get_param_handle("example1");
set_param_val(example1_h, (void*)&example1 );
@endcode

Also you can use the macros get_param() and set_param(),
which require less code.

@b Example:
@code
// Set the value, note for set_param() you can place the value as argument
set_param(example1_h, int8_t, -4);

// Get the value
uint8_t example1 = 4;
get_param( example1_h, example1 );
@endcode


The true advantage from the Param Service is the ability to
access values by a string, the name. This way the ground segment
can modify or retrieve variables in the spacecraft by knowing the
name. Incoming and outgoing messages from ground may contain
the value of parameters as strings, therefore you can also
use the functions str_to_param() and param_to_str().

To set the value of a parameter with a string with the value
use str_to_param().

@b Example:
@code
str_to_param(example_handle, "example string");
@endcode


To retrieve the value of a parameter as a string use
param_to_str().

@b Example:
@code
char buffer[20];
param_to_str(example_handle, buffer, 20);
@endcode

*/



/**
 * @enum	param_type_t
 * @brief	Parameter Types
 *
 * Supported types for parameters
*/
typedef enum
{
	UINT8_PARAM,
	INT8_PARAM,
	UINT16_PARAM,
	INT16_PARAM,
	UINT32_PARAM,
	INT32_PARAM,
	UINT64_PARAM,
	INT64_PARAM,
	FLOAT_PARAM,
	DOUBLE_PARAM,
	STRING_PARAM
} param_type_t;

/**
 * @enum	param_size_t
 * @brief	Parameter Sizes
 *
 * Macros for assigning the size in Parameters Table, note that STRING_PARAM
 * can have any size, is decision from user to assign the size.
*/
typedef enum
{
	UINT8_SIZE		= sizeof(uint8_t),
	INT8_SIZE		= sizeof(int8_t),
	UINT16_SIZE		= sizeof(uint16_t),
	INT16_SIZE		= sizeof(int16_t),
	UINT32_SIZE		= sizeof(uint32_t),
	INT32_SIZE		= sizeof(int32_t),
	UINT64_SIZE		= sizeof(uint64_t),
	INT64_SIZE		= sizeof(int64_t),
	FLOAT_SIZE		= sizeof(float),
	DOUBLE_SIZE		= sizeof(double)
	// String Size should be decided by user.
} param_size_t;

/**
 * @enum	param_opts_t
 * @brief	Parameter Options
 * @note	READ_ONLY is only applicable for parameterized variables.
 *
 * Macros for assigning the options in Parameters Table
*/
typedef enum
{
	TELEMETRY	= 0b00000001,		/**< Automatic collect this param for telemetry. */
	PERSISTENT	= 0b00000010,		/**< Persist the value on non volatile memory. */
	READ_ONLY	= 0b00000100		/**< Prohibited to write with param_service functions, only applicable for parameterized variables. */
} param_opts_t;

/**
 * @struct	param_t
 * @brief	Parameter struct
 *
 * Holds the data for a parameter in the table. The Parameter Table
 * is composed by a collection of this struct.
 * @see param_table_t
 *
 * Do not handle this struct directly, use instead param_handle_t type
 * and the API defined functions.
 * @see param_handle_t
*/
typedef struct
{
	const char name[CONF_PARAM_NAME_SIZE];	/**< Name of the param.	*/
	const param_type_t type;				/**< Param Type, from param_type_t. */
	const uint8_t size;						/**< Size in bytes, from param_size_t, or custom if STRING_PARAM.*/
	uint8_t opts;							/**< Special options, from param_opts_t.*/
	void* value;							/**< Pointer to param space where value is stored. */
}param_t;

/**
 * @typedef	param_table_t
 * @brief	Parameters Table Type
 *
 * Type to define the parameters table.
 * @note The table should be register during initialization with set_param_table()
 *
 * **Example**:
 * @code
 * // Variables to be parameterized
 * uint32_t	variable_name;
 * float		variable2_name;
 * // Parameters Table
 * param_table_t mission_param_table = {
 *	//	NAME					TYPE				Size				Options							Variable
 *	{.name="example1",	.type=UINT8_PARAM,	.size=UINT8_SIZE,	.opts=TELEMETRY},
 *	{.name="example2",	.type=STRING_PARAM,	.size=20,			.opts=TELEMETRY|PERSISTENT|READ_ONLY},
 *	{.name="example3",	.type=UINT32_PARAM,	.size=UINT32_SIZE,}, // Params may have no options
 *	{.name="example4",	.type=UINT32_PARAM,	.size=UINT32_SIZE,	.opts=TELEMETRY|PERSISTENT,				.value=parameterize(variable_name)},
 *	{.name="example5",	.type=FLOAT_PARAM,	.size=FLOAT_SIZE,											.value=parameterize(variable2_name)}
 * };
 * @endcode
*/
typedef param_t param_table_t[];

/**
 * @typedef	param_handle_t
 * @brief	Parameter Handle Type
 *
 * Pointer to a parameter struct, i.e: entry in parameters table.
 * Use this type and the API functions for handling parameters.
 * @see set_param_val(), get_param_val(), set_param(), get_param()
*/
typedef param_t* param_handle_t;

/**
 * @typedef	param_index_t
 * @brief	Index to a parameter Type
 *
 * Type to retrieve a parameter handler by the index in table.
*/
typedef int16_t param_index_t;



/**
 * @brief	Load parameters in a file to the Parameter Table
 *
 * When enabling the Parameter persistence Service with  init_param_persistence(), prameters
 * are stored in a file. After a rebbot form OBC, to restore the Parameter Table use this
 * function. The Param Table should be created befor, using  set_param_table().
 * @note 	Param Table should be created befor!
 * @see 	set_param_table()
 * @note	Storage Service functions should be ported. See simle_port.h.
 * @see		sfsf_port.h
 * @param 	file_name 		Name of file where parameters are stored in persistent memory
 * @return	-1 if error, 0 if OK
 */
int load_param_table(char* file_name);


/**
 * @brief	Init the task that stores parameters in persistent memory.
 * @note	Storage Service functions should be ported. See simle_port.h.
 * @see		sfsf_port.h
 * @return	-1 if error, 0 if OK
 */
int init_param_persistence(void);

/**
 * @brief	Set the parameter table
 *
 * Call this function during initialization, in init_services() function
 * at init_functions.c.
 * @param	param_table				Pointer to the param table
 * @param	param_table_size		Num of entries of param_table
 * @return	-1 if error, 0 if OK
 */
int set_param_table(param_table_t* param_table, uint16_t param_table_size);

/**
 * @def  parameterize
 * @brief Macro to parameterize a variable into the Parameters Table.
 *
 * Use this macro to create parameters into the Parameters Table using a variable.
 * Variable should be visible in the scope where the Table is declared, this can be
 * done with the "extern" keyword.
 *
 * **Example:**
 * @code
 * uint32_t example_var;	// Variable to parameterize
 * // The Parameter Table
 * param_table_t param_table= {
 *		{.name="example",	.type=UINT32_PARAM,	.size=UINT32_SIZE,	.opts=TELEMETRY	.value=parameterize(example_var),
 * }
 * @endcode
 * @param	variable_name			Name of the variable to parameterize
 */
#define parameterize(variable_name)  (void*)&variable_name

/**
 * @brief	Get the handle of a Parameter by the name
 * @param	name					Name of the parameter
 * @return	handle to param if OK, NULL if error or not found
 */
param_handle_t get_param_handle_by_name(const char * name);

/**
 * @brief	Get the index in table of a Parameter by the name
 * @param	name					Name of the parameter
 * @return	index to param if OK, -1 if error (not found)
 */
param_index_t get_param_index(const char * name);

/**
 * @brief	Get the handle of a Parameter by the index
 * @param	index					Index in table of the parameter
 * @return	handle to param if OK, NULL if error (not found)
 */
param_handle_t get_param_handle_by_index(param_index_t index);

/**
 * @brief	Set the value of a Parameter
 * @warning	Not thread-safe, user shall use mutex if sharing a param between many tasks, as with variables
 * @param	param_h				Handle of the param
 * @param	in_p 				Void pointer to memory where value is stored
 * @return	0 if OK, -1 if error ( Param no exists, or buffer too small)
 */
int set_param_val(param_handle_t param_h, void * in_p);

/**
 * @brief	Get the value of a Parameter
 * @param	param_h				Handle of the parameter
 * @param	out_p 				Void pointer to buffer to store param value, should be enough to store value
 * @return	0 if OK, -1 if error or Param no exists, or buffer too small
 */
int get_param_val(param_handle_t param_h, void * out_p);

/**
 * @brief	Macro for easing setting the value of a Parameters
 *
 * @code
 * //Example:
 * param_handle_t example_handle_uint16 = get_param_handle("example_uint16");	// get handle
 * set_param(example_handle_uint16, uint16, 100);								// Set value with macro
 * @endcode
 *
 * @param	handle				Handle of the parameter
 * @param	type				C build in type of the value, NOT a param_type_t, (i.e. uin8_t, int32_t, float, etc.)
 * @param	value				Value or variable with value to be assigned
 */
#define set_param(handle, type, value)	{ type aux_var = value; set_param_val(handle, (void*)&aux_var);}

/**
 * @brief	Macro for easing getting the value of a Parameters
 *
 * @code
 * Example:
 * param_handle_t example_handle_uint16 = get_param_handle("example_uint16");	// get handle
 * uint16_t other_var;
 * get_param( example_handle_uint16, other_var );				// get value with macro
 * @endcode
 *
 * @param	handle				Handle of the parameter
 * @param	dest_var			Destination variable where value will be stored
 */
#define get_param( handle, dest_var ) { get_param_val(handle, (void*)&dest_var);}

/**
 * @brief	Store the value of a param as string in a buffer
 * @param	param_handle		Handle of the param
 * @param	out_buff			Destination buffer
 * @param	buff_size 			Size of the destination buffer
 * @return	0 if OK, -1 if error
 */
int param_to_str(param_handle_t param_handle, char* out_buff, int buff_size);

/**
 * @brief	Set the value of a string to a param
 *
 * Convert the value of a string to the type of the param, and store it in the param value space
 * @param	param_handle		Handle of the param
 * @param	in_buff				Buffer with the value as string, to be assigned to the param
 * @return	0 if OK, -1 if error
 */
int str_to_param(param_handle_t param_handle, char* in_buff);

/**
 * @brief	Get amount of params in table
 * @return	Number of params in table
 */
uint16_t get_table_size(void);

/**
 * @brief	Collect all params with TEMELETRY
 *
 * Collects all params with TEMELETRY option and store the value with a tag
 * into dest_buff, if buff is not big enough, not all params will be collected.
 * Format: "TAG:value,TAG:value,TAG:value"
 * Example: "A:123,B:-3,C:0.001234"
 * Where "TAG" is a incremental alphabetical character (A,B,C,...,AA,AB,...)
 * and "value" is the value of the corresponding param.
 * For getting the reference between TAG and param see collect_telemtry_header()
 * @see collect_telemtry_header
 * @param	dest_buff			Buffer where telemetry data will be stored
 * @param	buff_size			Size of dest_buff
 */
void collect_telemetry_params(char * dest_buff, size_t buff_size);

/**
 * @brief	Collect the references between TAG and params with TELEMETRY
 *
 * Collect the references between TAG and params with TELEMETRY option into
 * dest_buff, if buff is not big enough, not all params will be collected.
 * Format: "TAG:param_name,TAG:param_name,TAG:param_name"
 * Example: "A:reset_cause,B:temperature,C:gps_lat"
 * Where "TAG" is a incremental alphabetical character (A,B,C,...,AA,AB,...)
 * and "param_name" is the name of the param assigned in the params table.
 *
 * @param	dest_buff			Buffer where telemetry data will be stored
 * @param	buff_size			Size of dest_buff
 * @return	-1 if error, size of string if OK
 */
int collect_telemtry_header(char * dest_buff, int buff_size);

/**
 * @brief	Print all params names and values in debug output.
 * @note	Debug Service function should be ported.
 * @see		sfsf_port.h
 */
void print_pram_table(void);

// Get Task Handle from parameters persistent task
csp_thread_handle_t get_param_task_handle(void);

#ifdef __cplusplus
}
#endif
#endif /* SFSF_PARAM_H_ */
