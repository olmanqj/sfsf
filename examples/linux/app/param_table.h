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
 * @brief	Parameter Table
 * @example param_table.h
 * Param Table Example



Parameter Table
===============
This is an example of how to create a Param Table.


*/

#ifndef PARAM_TABLE_H_
#define PARAM_TABLE_H_

#include <sfsf.h>
#include <sfsf_param.h>
#include <sfsf_hk.h>

/**
 * @brief	Parameter Table
*/
param_table_t mission_param_table = {
//	NAME					TYPE				Size				Options									Variable
	{.name="example",		.type=UINT8_PARAM,	.size=UINT8_SIZE,	.opts=TELEMETRY|PERSISTENT},
	{.name="time_stamp",	.type=STRING_PARAM,	.size=20,			.opts=TELEMETRY|PERSISTENT},
	// Parameterized variables from HK Service, counts the amount of beacons
	{.name="beacon_count",	.type=UINT32_PARAM,	.size=UINT32_SIZE,	.opts=TELEMETRY|PERSISTENT|READ_ONLY,	.value=parameterize(beacon_counter)},
	// Parameterized variables from HK Service, means the period between each beacon transmission
	{.name="beacon_period",	.type=UINT32_PARAM,	.size=UINT32_SIZE,	.opts=PERSISTENT,						.value=parameterize(beacon_period)}
};


#endif /* PARAM_TABLE_H_ */
