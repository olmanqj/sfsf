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
#ifndef SFSF_HK_H_
#define SFSF_HK_H_

#ifndef SFSF_H_
#error Include sfsf.h before sfsf_hk.h!
#endif

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @file	sfsf_hk.h
 * @brief	API for Housekeeping Service


Housekeeping Service
====================

Features Summary
-------------
- Transmits telemetry data periodically.
- Stores telemetry data periodically.

Module Description
------------------
The Housekeeping (HK) Service is in charge of providing the ground segment with
telemetry data about the state and health of the spacecraft.This service is able
to automatically collect, store and transmit telemetry data. Storing telemetry
data may be of interest for the mission, if it is required to know the state of
the spacecraft even during the section of the orbit without a communication link
with ground.

*/




/** @name Parameterizable Variables
 *
 * Use the parateerize() Macro to parameterize this variables into the Parameters Table,
 * this will simplify the control of HK Service, by providing a way to change the behavior .
 * See sfsf_param.h.
 *
 * Example:
 * @code
 * param_t param_table[]= {
 *	 ...
 *	 parameterize(  "beacon_count",		UINT32_PARAM,	UINT32_SIZE,	TELEMETRY|PERSISTENT|READ_ONLY, beacon_counter ),
 *	 parameterize(  "beacon_period",	UINT32_PARAM,	UINT32_SIZE,	PERSISTENT,						beacon_period ),
 *	 ...
 * }
 * @endcode
 *
 */
///@{
extern uint32_t beacon_counter;		/**< Counts the amount of beacons sent. */
extern uint32_t beacon_period;		/**< The period between each beacon transmission. */
extern uint8_t beacon_packet_prio;	/**< CSP Priority of Bacon packets. */
extern uint8_t beacon_dport;		/**< CSP Destination Port of Bacon packets. */
extern uint8_t beacon_sport;		/**< CSP Source Port of Bacon packets. */
extern uint8_t beacon_broadcast_padlock;	/**< For pausing a resuming Beacon Transmission. Paused if 0, Resumed if 1. */
extern uint8_t beacon_storage_padlock;		/**< For pausing a resuming Beacon Storage. Paused if 0, Resumed if 1. */
///@}



/**
 * @typedef	telemetry_collector_t
 * @brief	Typedef of a Telemetry Data Collector function
 *
 * If set, this function will be called automatically by the service to collect telemetry data.
 * It should be set during initialization with set_telemetry_collector().
*/
typedef void (*telemetry_collector_t) (char * dest_buf, size_t buf_len);

/**
 * @brief Init HK task, collects, stores and broadcasts telemetry data periodically.
 *
 * @note When the Flight Software starts Beacons wont be stored neither broadcasted, this for
 * avoid radio transmissions during deployment of the satellite.  App should start Beacons
 * transmission and storage with resume_hk_broadcast() and resume_hk_storage().
 * @note For the HK service to collect Telemetry Data automatically a collector function
 * should by set during initialization with set_telemetry_collector().
 *
 * @return	-1 if error , 0 if OK
 */
int init_hk_service(void);

/**
 * @brief	Broadcast a CSP packet as a Beacon
 * @param	beacon_packet			CSP packet with telemetry data to broadcast
 * @return	-1 if error , 0 if OK exit status
 */
int send_beacon(csp_packet_t * beacon_packet);

/**
 * @brief	Set the Telemetry Collector function
 * @note	collect_telemetry_params() fomr Param Service is situable for this.
 * @see		sfsf_param.h
 * @param	telemetry_collector_p		Function that collects telemetry data into dest_buf
 */
void set_telemetry_collector( telemetry_collector_t telemetry_collector_p);

/**
 * @brief Stop Beacons broadcasting
 */
void stop_hk_broadcast(void);

/**
 * @brief Resume Beacons broadcasting
 */
void resume_hk_broadcast(void);

/**
 * @brief Resume Beacons storage
 */
void resume_hk_storage(void);

/**
 * @brief Stop Beacons storage
 */
void stop_hk_storage(void);

/**
 * @brief	Returns the count of Beacons sent
 * @return	Count of Beacons sent
 */
uint32_t get_beacon_count(void);

/**
 * @brief	Get HK Handle
 * @return	csp_thread_handle_t
 */
csp_thread_handle_t get_hk_task_handle(void);

#ifdef __cplusplus
}
#endif
#endif /* SFSF_HK_H_ */
