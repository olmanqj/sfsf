/*
Copyright 2018 olmanqj
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */ 

/**
 * @brief	Command Table
 * @example cmd_table.h
 * Command Table Example

 
Command Table
=============
This is an example of how to create a Command Table.



*/

// Framework Services Includes
#include "sfsf.h"
#include "sfsf_cmd.h"


#ifndef CMD_TABLE_H_
#define CMD_TABLE_H_

//////////////////////////////////////////////
/////		COMMANDS CODES		//////////////
//////////////////////////////////////////////
/**
 * @name	Command Codes Definitions	
 * Define the command codes 	
*/
//@{
#define CMD_DUMMY							0x01
#define CMD_GET_PARAM						0x02
#define CMD_SET_PARAM						0x03
#define CMD_REBOOT_OBC						0x04
//@}

//////////////////////////////////////////////
/////	FUNCTION DEFINITIONS	//////////////
//////////////////////////////////////////////
DEFINE_CMD_ROUTINE(dummy);
DEFINE_CMD_ROUTINE(cmd_get_param);
DEFINE_CMD_ROUTINE(cmd_set_param);
DEFINE_CMD_ROUTINE(cmd_reboot_obc);


//////////////////////////////////////////////
/////		COMMAND TABLE		//////////////
//////////////////////////////////////////////
cmd_table_t mission_cmd_table = {
	//Code of command				Amount of args					Pointer to routine
	{.cmd_code = CMD_DUMMY,			.cmd_args_num = 0,				.cmd_routine_p = &dummy},
	{.cmd_code = CMD_GET_PARAM,		.cmd_args_num = ARGS_NUM_ANNY,	.cmd_routine_p = &cmd_get_param},
	{.cmd_code = CMD_SET_PARAM,		.cmd_args_num = ARGS_NUM_ANNY,	.cmd_routine_p = &cmd_set_param},
	{.cmd_code = CMD_REBOOT_OBC,	.cmd_args_num = 0,				.cmd_routine_p = &cmd_reboot_obc}
};


#endif /* CMD_TABLE_H_ */