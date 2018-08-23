/*
Copyright 2018 olmanqj
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */ 
#include <stdlib.h>
#include <stdio.h>

// CSP Includes
#include <csp.h>
#include <csp_thread.h>
#include <csp_queue.h>
#include <csp_semaphore.h>
#include <csp_malloc.h>


// Framework Includes
#include "sfsf.h"
#include "sfsf_debug.h"
#include "sfsf_storage.h"
#include "sfsf_param.h"


// Task handler
csp_thread_handle_t handle_param_service_task;
// Period to store persistent parameters in non-volatile memory
uint32_t param_persist_period;
// How may params in table
uint16_t param_table_size_v;
// List of parameters handles, i.e. Parameters Table
param_t * param_table_p;
// Memory space with parameters values, has the size of the sum of all params sizes
void * param_space_p;




// Parameters Service Task
int i;
FILE *param_fd;	// parameters file Descritor
char param_value_buff[CONF_PARAM_MAX_PARAM_SIZE]; // buffer for converting param values to str
CSP_DEFINE_TASK( param_service_task )
{
	while(1)
	{
		csp_sleep_ms(param_persist_period);
		// if debug enable print status
		#if CONF_PARAM_DEBUG == ENABLE
		print_debug("PARAM>\tStoring params\n");
		#endif		
		 // Try to Open Parameter storage file
		param_fd = fopen(CONF_PARAM_FILE_NAME, "w+");
		if (!param_fd) 
		{
			continue;	// TODO handle error  
		}
		// Get next param handle
		for(i = 0; i < param_table_size_v; i++ )
		{
			if(!(param_table_p[i].opts & PERSISTENT)) continue; // if param dos not has PERSISTENT option, continue
			// Clear buffer
			bzero(param_value_buff, sizeof(param_value_buff));
			// Convert param value to str and store it in buff
			param_to_str(&param_table_p[i], param_value_buff, sizeof(param_value_buff));
			// Print param to file
			fprintf(param_fd, "%s,%s\n", param_table_p[i].name, param_value_buff);
		}
		// Close file
		fclose(param_fd);		
	}
	return CSP_TASK_RETURN;
}


// Load parameters froma persistent file to the Parameter table, Table should exists
int load_param_table(char* file_name)
{
    char *val_p;	// pointer where starts iin line the value of the parameter
	param_handle_t param_h;
	// Bufer to store a line from file
	char line_buff[CONF_PARAM_NAME_SIZE+CONF_PARAM_MAX_PARAM_SIZE+3]; // Plus the comma separator, the new line char and terminating null-character

	// Check if table already exists, if not impossible to load data from a file 
    if(param_table_p ==NULL || param_table_size_v < 1 ) return EXIT_FAILURE;
	// Try to Open file
	param_fd = fopen(file_name, "r"); 
	if (!param_fd)  return EXIT_FAILURE;
	// Read line by line
	while( fgets( line_buff, sizeof(line_buff), param_fd )!= NULL) 
	{
		// Fin the comma delimiter in line
		val_p = line_buff;
		strsep(&val_p, ",");
		// If delimiter not found, continue
		if (val_p == NULL) continue;
        // Look if exists requested param
		if((param_h =  get_param_handle_by_name(line_buff))==NULL) continue;
        // Remove new line char '\n' from buffer (should be the last char)
        if(val_p[strlen(val_p)-1] == '\n') val_p[strlen(val_p)-1] = '\0';
        // Try set value in buff to param
		str_to_param(param_h, val_p);   
    }
    // Close file and fee pointer
    fclose(param_fd);
	return EXIT_SUCCESS;
}


int init_param_persistence(void)
{
	// Ckeck Param Table Exists
	if(param_table_p ==NULL || param_table_size_v < 1 )
	{
		#if CONF_PARAM_DEBUG == ENABLE
		print_debug("PARAM>\tInit Param Persistence Fails! Set Param Table befor init Param Persistence!\n");
		#endif
		return EXIT_FAILURE;
	}
	// Set Options	
	param_persist_period = CONF_PARAM_PERSIST_PERIOD;
	//Create param Service Task
	return csp_thread_create(param_service_task, "PARAM_TASK", CONF_PARAM_TASK_STACK_SIZE, NULL, CONF_PARAM_TASK_PRIORITY, &handle_param_service_task);
}



// Mount the Parameter Table
int set_param_table(param_table_t* param_table, uint16_t param_table_size)
{
	int i, param_space_size, offset;
	// Set Options
	param_table_size_v = param_table_size;
	param_table_p = (param_t*) param_table;
	// Check Options
	if(param_table_p ==NULL || param_table_size_v < 1 ) return EXIT_FAILURE;
	// Check entries of table and calculate param space ( memory space to store param values)
	param_space_size = 0;
	for(i = 0; i < param_table_size_v; i++ )
	{
		// Check that each entry has a name an a size > 0
		if(param_table_p[i].name[0] == '\0' || param_table_p[i].size < 1 ) return EXIT_FAILURE;
		// Param space is equals to the sum of all param sizes, except if is a existing variable parameterized
		// Check if is a variable parameterized
		if(param_table_p[i].value != NULL ) continue;
		// If not, the require memory shall be allocated, increment counter
		param_space_size+=param_table_p[i].size;
	}
	//	Alloc memory for param values
	if((param_space_p = csp_malloc(param_space_size))==NULL) return EXIT_FAILURE;
	// Populate value pointers in table entries
	offset = 0;
	for(i = 0; i < param_table_size_v; i++ )
	{
		// Check if is a parameterized variable
		// If is a parameterized variable the pointer shall not be changed, already contains the corresponding pointer
		if(param_table_p[i].value != NULL ) continue;
		// For each param in table, the value will be stored in param space + offset (size of previous params)
		param_table_p[i].value = param_space_p + offset;
		// Calculate offset for next param
		offset+=param_table_p[i].size;
	}
	#if CONF_PARAM_DEBUG == ENABLE
	print_debug("PARAM>\tParam Table OK!\n");
	#endif	
	return EXIT_SUCCESS;
}



// Search into the table to find param with the name
// Return the handle of a Parameter
param_handle_t get_param_handle_by_name( const char * name)	
{
	int i;
	for(i = 0; i < param_table_size_v; i++ )
	{
		// if same name, found =), return pointer
		if(strcmp( param_table_p[i].name, name) == 0 ) return &param_table_p[i];
	}
	
	// Returns NULL if not found
	return NULL;
}


// Search into the table to find param with the name
// Return the index of a Parameter
param_index_t get_param_index( const char * name)				// TODO Make indexes start at 1, otherwise Null index would be valid
{
	int16_t i;
	for(i = 0; i < param_table_size_v; i++ )
	{
		// if same name, found =), return pointer
		if(strcmp( param_table_p[i].name, name) == 0 ) return i;
	}
	
	// Returns NULL if not found
	return -1;
}




// Get the handle of a Parameter by the index
param_handle_t get_param_handle_by_index(param_index_t index)		// TODO Make indexes start at 1, otherwise Null index would be valid
{
	if(index < 0 || index >= param_table_size_v) return NULL;
	return &param_table_p[index];
}






// Set the value of a Parameter
int set_param_val(param_handle_t param_h, void * in_p)
{
	if (param_h==NULL || param_h->value==NULL || in_p==NULL ) return EXIT_FAILURE;
	// Check if param has READ_ONLY option
	if(param_h->opts & READ_ONLY) return EXIT_SUCCESS;
	// Write param value to mem position
	memcpy( param_h->value, in_p, param_h->size);
	return EXIT_SUCCESS;
}



// Get the value of a Parameter 
int get_param_val(param_handle_t param_h, void * out_p)
{
	if (param_h==NULL || param_h->value==NULL || out_p==NULL) return EXIT_FAILURE;
	memcpy(out_p, param_h->value, param_h->size);
	return EXIT_SUCCESS;
}



// Get amount of params stored id table
uint16_t get_table_size(void)
{
	return param_table_size_v;
}



// Store the value of a param as string in a buffer
int param_to_str(param_handle_t param_handle, char* out_buff, int buff_size)
{
	int exit_status;
	// Check param_handle exists
	if(param_handle == NULL || param_handle->value == NULL) return EXIT_FAILURE;
	// Select the type of the param
	switch(param_handle->type)
	{
		case UINT8_PARAM:
		{
			uint8_t param_value = 0;
			get_param_val(param_handle, (void*)&param_value);
			exit_status = snprintf(out_buff,  buff_size, "%"PRIu8"", param_value);
			break;
		}
		case INT8_PARAM:
		{
			int8_t param_value = 0;
			get_param_val(param_handle, (void*)&param_value);
			exit_status = snprintf(out_buff,  buff_size, "%"PRIi8"", param_value);
			break;
		}
		case UINT16_PARAM:
		{
			uint16_t param_value = 0;
			get_param_val(param_handle, (void*)&param_value);			
			exit_status = snprintf(out_buff,  buff_size, "%"PRIu16"", param_value);
			break;
		}
		case INT16_PARAM:
		{
			int16_t param_value = 0;
			get_param_val(param_handle, (void*)&param_value);
			exit_status = snprintf(out_buff,  buff_size, "%"PRIi16"", param_value);
			break;
		}
		case UINT32_PARAM:
		{
			uint32_t param_value = 0; 
			get_param_val(param_handle, (void*)&param_value);
			exit_status = snprintf(out_buff,  buff_size, "%"PRIu32"", param_value);
			break;
		}
		case INT32_PARAM:
		{
			int32_t param_value = 0;
			get_param_val(param_handle, (void*)&param_value);
			exit_status = snprintf(out_buff,  buff_size, "%"PRIi32"", param_value);
			break;
		}
		case UINT64_PARAM:
		{
			uint64_t param_value = 0;
			get_param_val(param_handle, (void*)&param_value);
			exit_status = snprintf(out_buff,  buff_size, "%"PRIu64"", param_value);
			break;
		}
		case INT64_PARAM:
		{
			int64_t param_value = 0;
			get_param_val(param_handle, (void*)&param_value);
			exit_status = snprintf(out_buff,  buff_size, "%"PRIi64"", param_value);
			break;
		}
		case FLOAT_PARAM:
		{
			float param_value = 0;
			get_param_val(param_handle, (void*)&param_value);
			exit_status = snprintf(out_buff,  buff_size, "%.3f", param_value);
			break;
		}
		case DOUBLE_PARAM:
		{
			double param_value = 0;
			get_param_val(param_handle, (void*)&param_value);
			exit_status = snprintf(out_buff,  buff_size, "%.3lf", param_value);				// TODO fix precision issue 
			break;
		}
		case STRING_PARAM:
		{
			exit_status = snprintf(out_buff,  buff_size, "%s", (char*)param_handle->value);
			break;
		}
		default:
			snprintf(out_buff, buff_size, "unknown_type");
			return EXIT_FAILURE;
	}
	// snprintf return > 1 if success
	if(exit_status)	return EXIT_SUCCESS;
	return EXIT_FAILURE;
}




// Convert a string to the type of the param, and store it in the param value space
int str_to_param(param_handle_t param_handle, char* in_buff)
{
	// Check param_handle exists
	if(param_handle == NULL || param_handle->value == NULL) return EXIT_FAILURE;
	// Select the type of the param
	switch(param_handle->type)
	{
		case UINT8_PARAM:
		{
			// Auxiliary var where the value of the param will be temporary stored
			uint8_t param_value;
			// Scan the source buffer for a uint, store the converted value in param_value
			param_value = (uint8_t) atoi( in_buff );
			// Set  param_value to the desired parameter 
			set_param_val(param_handle, (void*)&param_value);	
			break;
		}
		case INT8_PARAM:
		{
			int8_t param_value;
			param_value = (int8_t) atoi( in_buff );
			set_param_val(param_handle, (void*)&param_value);
			break;
		}
		case UINT16_PARAM:
		{
			uint16_t param_value;
			param_value = (uint16_t) atoi( in_buff );
			set_param_val(param_handle, (void*)&param_value);			
			break;
		}
		case INT16_PARAM:
		{
			int16_t param_value;
			param_value = (int16_t) atoi( in_buff );
			set_param_val(param_handle, (void*)&param_value);
			break;
		}
		case UINT32_PARAM:
		{
			uint32_t param_value;
			param_value = (uint32_t) atoi( in_buff );
			set_param_val(param_handle, (void*)&param_value);
			break;
		}
		case INT32_PARAM:
		{
			int32_t param_value;
			param_value = (int32_t) atoi( in_buff );
			set_param_val(param_handle, (void*)&param_value);
			break;
		}
		case UINT64_PARAM:
		{
			uint64_t param_value;
			param_value = (uint64_t) atol( in_buff );
			set_param_val(param_handle, (void*)&param_value);			
			break;
		}
		case INT64_PARAM:
		{
			int64_t param_value;
			param_value = (int64_t) atol( in_buff );
			set_param_val(param_handle, (void*)&param_value);			
			break;
		}
		case FLOAT_PARAM:
		{
			float param_value;
			param_value = (float) atof( in_buff );
			set_param_val(param_handle, (void*)&param_value);			
			break;
		}
		case DOUBLE_PARAM:
		{
			double param_value;
			param_value = (double) atof( in_buff );
			set_param_val(param_handle, (void*)&param_value);			
			break;
		}
		case STRING_PARAM:
		{
			snprintf((char*)param_handle->value, param_handle->size, "%s", in_buff);
			break;
		}
		default:
			return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}




// Take an id and calculates the corresponding TAG, store the TAG in dest_buff
// e.g.: 0 = A, 1 = B, 25 = AA, 26 = AB, ...
void get_tag(int id, char* dest_buff, int buff_size)								// TODO fix issue, gives correct answers until 'YZ'
{
	#define ALPHA_DELTA  26 // Letters in alphabet
	int buff_index,i;
	char aux_buffer[buff_size];
	buff_index = 0;
	// While digits form id left, and buff not full
	while(buff_index < buff_size)
	{
		// First char of TAG is ( id MOD 26 ) + 'A'
		aux_buffer[buff_index] = ((id) % ALPHA_DELTA)+('A'- buff_index ) ;
		// Increment buffer index
		buff_index++;
		// For the following id = id/26
		id = id / ALPHA_DELTA;
		// Repeat until id != 0
		if(id == 0 ) break;
	}
	// Invert aux_buff an thats the TAG
	for(i= 0; 0 < buff_index; i++)
	{
		buff_index--;
		// Invert position on index
		dest_buff[i] = aux_buffer[buff_index];
	}
	
	return;
}

//Collect all params with TEMELETRY option and store the value with a tag into dest_buff
void collect_telemetry_params(char * dest_buff, size_t buff_size)
{
	int i, tag_id;
	// buff to store the tag, start by A, max YZ
	char tag_buff[3];
	// buff to store param value as str
	char param_buff[CONF_PARAM_MAX_PARAM_SIZE];
	tag_id=0;
	// get next param handle
	for(i = 0; i < param_table_size_v; i++ )
	{
		// Check if param has TELEMTRY option
		if(!(param_table_p[i].opts & TELEMETRY)) continue;
		// Clear buffers
		bzero(tag_buff, sizeof(tag_buff));
		bzero(param_buff, sizeof(param_buff));
		// Generate TAG
		get_tag(tag_id, tag_buff, sizeof(tag_buff));
		// Store the value from param as str
		param_to_str(&param_table_p[i], param_buff, sizeof(param_buff));
		// Check if enough space in dest_buff (enough for tag + param + ':' + ',') 
		if(strlen(dest_buff)+strlen(tag_buff)+strlen(param_buff)+2 >= buff_size) break;
		// Separate by comma, except if first
		if(strlen(dest_buff) != 0) strcat(dest_buff,   "," );
		// Concatenate  TAG
		strcat(dest_buff,  tag_buff );
		// Concatenate ':' separator
		strcat(dest_buff, ":");
		// Concatenate param value
		strcat(dest_buff, param_buff);
		tag_id++;
	}
}






// Collect the references between TAG and params with TELEMETRY option into dest_buff
int collect_telemtry_header(char * dest_buff, int buff_size)
{
	int i, tag_id;
	// buff to store the tag, start by A, max YZ
	char tag_buff[3];
	// buff to store param value as str
	char name_buff[CONF_PARAM_NAME_SIZE];
	tag_id=0;
	// get next param handle
	for(i = 0; i < param_table_size_v; i++ )
	{
		// Check if param has TELEMTRY option
		if(!(param_table_p[i].opts & TELEMETRY)) continue;
		// Clear buffers
		bzero(tag_buff, sizeof(tag_buff));
		bzero(name_buff, sizeof(name_buff));
		// Generate TAG
		get_tag(tag_id, tag_buff, sizeof(tag_buff));
		// Store the value from param as str
		param_to_str(&param_table_p[i], name_buff, sizeof(name_buff));
		// Check if enough space in dest_buff (enough for tag + param + ':' + ',') 
		if(strlen(dest_buff)+strlen(tag_buff)+strlen(name_buff)+2 >= buff_size) break;
		// Separate by comma, except if first
		if(strlen(dest_buff) != 0) strcat(dest_buff,   "," );
		// Concatenate  TAG
		strcat(dest_buff,  tag_buff );
		// Concatenate ':' separator
		strcat(dest_buff, ":");
		// Concatenate param value
		strcat(dest_buff, name_buff);
		tag_id++;
	}
	return EXIT_FAILURE;
}




// Print the param table, for debug
void print_pram_table(void)
{
	int i;
	char param_list_buff[CONF_PARAM_MAX_PARAM_SIZE];
	print_debug("\n-----------------------------------------\n");
	print_debug(" Param Name\t\t| Param Value\n");
	print_debug("-----------------------------------------\n");
	// get next param handle
	for(i = 0; i < param_table_size_v; i++ )
	{
		// Clear buffer
		bzero(param_list_buff, sizeof(param_list_buff));
		// Print name
		print_debug(param_table_p[i].name);
		print_debug("\t\t| ");
		// Convert param value to str and store it in buff
		param_to_str(&param_table_p[i], param_list_buff, sizeof(param_list_buff));
		// Print param value
		print_debug(param_list_buff);
		print_debug("\n");
	}
	print_debug("-----------------------------------------\n");

}


csp_thread_handle_t get_param_task_handle(void)
{
	return handle_param_service_task;
}
