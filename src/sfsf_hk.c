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

// Framework Includes
#include "sfsf.h"
#include "sfsf_debug.h"
#include "sfsf_storage.h"
#include "sfsf_hk.h"


// Frequency between beacons
uint32_t beacon_period;
// CSP Priority of Bacon packets
uint8_t beacon_packet_prio;
// CSP Destination Port of Bacon packets
uint8_t beacon_dport;
// CSP Source Port of Bacon packets
uint8_t beacon_sport;
// Packet to store Beacon
csp_packet_t *beacon_packet;
// Task handler
csp_thread_handle_t handle_hk_service_task;
// Beacon counter
uint32_t beacon_counter;
//For blocking Beacon Transmission
uint8_t beacon_broadcast_padlock;
//For blocking Beacon storage
uint8_t beacon_storage_padlock;
#define BEACON_BLOCKED	 0
#define BEACON_UNBLOCKED 1
// Pointer to the function that collects all telemetry data into a buffer,
// should be set at init set_telemetry_collector()
telemetry_collector_t telemetry_collector_fun;



// HK Service Task
FILE *beacon_fd;				// beacon file Descritor
CSP_DEFINE_TASK( hk_service_task )
{
	while(1)
	{	
		// Sleep for a while 
		csp_sleep_ms(beacon_period); 
		// Get a new packet
		beacon_packet = csp_buffer_get( CONF_CSP_BUFF_SIZE );
		if( beacon_packet == NULL )	continue;	
		// Clear packet
		bzero(beacon_packet->data,  CONF_CSP_BUFF_SIZE );
		// Collect telemetry data automatically with the telemetry_collector function, should be assigned at init
		if (telemetry_collector_fun != NULL) telemetry_collector_fun(beacon_packet->data,  CONF_CSP_BUFF_SIZE );
		beacon_packet->length = strlen( (char *) beacon_packet->data); 
		// Store Beacon in file if not blocked
		if(beacon_storage_padlock == BEACON_UNBLOCKED)
		{
			// If debug enabled, print storage action 
			#if	CONF_HK_DEBUG == ENABLE
			print_debug("HK>\tStoring Beacon\n");
			#endif
			beacon_fd = fopen(CONF_HK_BEACONS_FILE, "at"); // Try to Open
			if (!beacon_fd) beacon_fd = fopen(CONF_HK_BEACONS_FILE, "wt"); // Create if not opened
			// if file opened, write Bacon in file
			if (beacon_fd)
			{ 
				fprintf(beacon_fd, "%s\n",  beacon_packet->data );	// Write beacon in file
				fclose(beacon_fd);		// Close file
			}
		}
		// Broadcast Beacon if not blocked
		if( beacon_broadcast_padlock == BEACON_UNBLOCKED) 		// For blocking signals while Deploy
		{
			// If debug enabled, print action
			#if	CONF_HK_DEBUG == ENABLE
			print_debug("HK>\tBroadcasting Beacon:");
			print_debug(beacon_packet->data);
			print_debug("\n");
			#endif
			// Broadcast beacon
			send_beacon(beacon_packet);		
			// increment beacon counter
			beacon_counter++;
		}

		
		// If the packet was not used, should be freed manually 
		else csp_buffer_free(beacon_packet);
		
		

	} 
	return CSP_TASK_RETURN;	
}



// Init HK Service for space segment
// Note:  Beacons Transmission and Storage start blocked, App should resume it
int init_hk_service(void)
{
	// Set Options
	beacon_period = CONF_HK_BEACON_PERIOD_MS;
	beacon_packet_prio = CONF_HK_BEACON_PACKET_PRIORITY;
	beacon_dport = CONF_HK_DPORT;
	beacon_sport = CONF_HK_SPORT;
	//Create hk Service Task
    return csp_thread_create(hk_service_task, "HK_TASK", CONF_HK_TASK_STACK_SIZE, NULL, CONF_HK_TASK_PRIORITY, &handle_hk_service_task);
}




void set_telemetry_collector( telemetry_collector_t telemetry_collector)
{
	telemetry_collector_fun = telemetry_collector;
}




int send_beacon(csp_packet_t * beacon_packet)
{
	 // Send a packet without previously opening a connection				// TODO pass PRIO, and ports as params
	 // @return -1 if error (you must free packet), 0 if OK (you must discard pointer)
	if (csp_sendto( beacon_packet_prio,			// @param prio CSP_PRIO_x
	                CSP_BROADCAST_ADDR,         // @param dest destination node TODO Broadcast address
	                beacon_dport,				// @param dport destination port
	                beacon_sport,				// @param src_port source port
	                CSP_O_NONE,                 // @param opts CSP_O_x
	                beacon_packet,              // @param packet pointer to packet
	                200							// timeout timeout used by interfaces with blocking send
	                ) < 0) 
	{
		// TODO Handle error
		csp_buffer_free(beacon_packet);
	    return EXIT_FAILURE;
	}
	return EXIT_SUCCESS; 
}



void resume_hk_broadcast(void)
{
	// Resume Beacon Transmission
	beacon_broadcast_padlock = BEACON_UNBLOCKED;
}

void stop_hk_broadcast(void)
{
	//Stop beacons
	beacon_broadcast_padlock = BEACON_BLOCKED;
}

// Resume Beacons storage
void resume_hk_storage(void)
{
	beacon_storage_padlock = BEACON_UNBLOCKED;
}

// Stop Beacons storage
void stop_hk_storage(void)
{
	beacon_storage_padlock = BEACON_BLOCKED;
}


uint32_t get_beacon_count(void)
{
	return beacon_counter;
}


csp_thread_handle_t get_hk_task_handle(void)
{
	return handle_hk_service_task;
}
