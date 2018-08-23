/*
Copyright 2018 olmanqj
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */ 

/**
 * @brief	Mission Configurations Example
 * @example mission_config.h
 * Command	Mission Configurations
 
Mission Configurations
======================
Recommendation example of how to keep constants and configurations 
for mission application (user code).
	
*/

#ifndef MISSION_CONFIG_H_
#define MISSION_CONFIG_H_


//////////////////////////////////////////////
/////		CSP					//////////////
//////////////////////////////////////////////
#define CSP_BUFFER_POOL_SIZE					20
#define CSP_BUFFER_SIZE							300

// CSP Config for Space Segment
#define CSP_OBC_ADDRESS							1				// OBC Default address
#define	CSP_OBC_PORT_CMD						11				// OBC Command Port

// CSP Config for Ground Segment
#define CSP_GROUND_STATION_ADDRESS				9				// GROUND Station address
#define	CSP_REMOTE_STATION_ADDRESS				10				// Remote station segment address 


//////////////////////////////////////////////
/////		APP					//////////////
//////////////////////////////////////////////
#define APP_CHECK_NEW_CMD_PERIOD			1000




#endif /* MISSION_CONFIG_H_ */
