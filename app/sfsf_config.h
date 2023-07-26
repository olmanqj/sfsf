#ifndef SFSF_CONFIG_H_
#define SFSF_CONFIG_H_
/**
 * @file	sfsf_config.h
 * @brief	SFSF Services Configurations


SFSF Services Configurations
============================
The SFSF services require a configuration file to work.
This is the only file from library that the user needs to modify.
The file contains configurations that modifies the behavior of
each service. Each configuration has its owns description.
*/

/** @name Enable and Disable Macros
 *  Some configurations may be enabled or disabled with this macros.
 */
//@{
#define  ENABLE		1						/**< Enable a configuration. */
#define  DISABLE	0						/**< Disable a configuration. */
//@}


//////////////////////////////////////////////
/////	OS CONFIGURATIOS		//////////////
//////////////////////////////////////////////
/** @name OS CONFIGURATIOS
 *  Some OS, like FreeRTOS, require to define the stack size for creating Tasks
 *  If your OS don't requires to define the stack size, comment the following lines
 */
//@{
#define	CONF_MINIMAL_STACK_SIZE				256
//@}

//////////////////////////////////////////////
/////		CSP					//////////////
//////////////////////////////////////////////
/** @name CSP Configurations
 *  It is necessary to know the size assigned to the CSP buffers.
 *  Set the same value used for csp_buffer_init().
 */
//@{
#define CONF_CSP_BUFF_SIZE                  300
//@}

//////////////////////////////////////////////
/////		DEBUGGING			//////////////
//////////////////////////////////////////////
/** @name Debug Configurations
 *	Uncomment to print debug info of each service.
 *  @note Debug functions should be implemented by user.
 *  @see  sfsf_port.h
 */
//@{
#define CONF_CMD_DEBUG						ENABLE			/**< Print cmd queue debug info to debug output. */
#define CONF_HK_DEBUG						ENABLE			/**< Print beacons to debug output.*/
#define CONF_LOG_DEBUG						ENABLE			/**< Print log messages to debug output. */
#define CONF_PARAM_DEBUG					ENABLE			/**< Print param debug info to debug output. */
#define CONF_TIME_DEBUG						ENABLE			/**< Print software watchdog timer debug info to debug output. */
//@}

//////////////////////////////////////////////
/////		PARAM SERVICE		//////////////
//////////////////////////////////////////////
/** @name Param Service Configurations
 *  Configurations for sfsf_param.h
 */
//@{
#define CONF_PARAM_PERSIST_ENABLE			ENABLE			/**< Enable or disable the Param task, which stores Params in file. */
#define CONF_PARAM_FILE_NAME				"params.txt"	/**< File name where params will be stored in persistent memory. */
#define CONF_PARAM_NAME_SIZE				16				/**< Max size of params names in bytes. */
#define CONF_PARAM_MAX_PARAM_SIZE			64				/**< Max size of Parameter of type STRING_PARAM. */
//@}

//////////////////////////////////////////////
/////		COMAND SERVICE		//////////////
//////////////////////////////////////////////
/** @name Command Service Configurations
 */
//@{
#define CONF_CMD_QUEUE_ENABLE				ENABLE			/**< Enable or Disable the Command Queue. */ // <=== Not Implemented!!!
#define CONF_CMD_QUEUE_SIZE					10				/**< Max amount of commands in queue waiting to be executed. */
#define CONF_CMD_ARGS_DELIMITERS			",;"			/**< Separators chars of Command Arguments within Command packet */
//@}

//////////////////////////////////////////////
/////		HK SERVICE			//////////////
//////////////////////////////////////////////
/** @name Housekeeping Service Configurations
 */
//@{
#define CONF_HK_ENABLE						ENABLE			/**< Enable or disable the Beacon transmission and storage. */
#define CONF_HK_BEACONS_FILE				"beacons.txt"	/**< Name of the file where beacons will be stored if CONF_HK_STORE_BEACONS is ENABLE. */
#define CONF_HK_SPORT						10				/**< CSP source port for Beacons. */
#define CONF_HK_DPORT						10				/**< CSP destination port for Beacons. */
#define CONF_HK_BEACON_PACKET_PRIORITY		2				/**< CSP Packet Priority for beacons. */
#define CONF_HK_BEACON_PERIOD_MS			20000			/**< Period between beacons in ms. */
//@}

//////////////////////////////////////////////
/////		LOG SERVICE			//////////////
//////////////////////////////////////////////
/** @name Log Service Configurations
 */
//@{
#define CONF_LOG_PERSIST_ENABLE				ENABLE			/**< Enable or disable the Log task, which stores log messages in file. */
#define CONF_LOG_FILE_NAME					"log.txt"		/**< Name of Log file. */
#define CONF_LOG_QUEUE_SIZE					10				/**< Max Log messages waiting to be stored. */
#define CONF_LOG_MESSAGE_SIZE				128				/**< Max size of a Log message. */
//@}

//////////////////////////////////////////////
/////		TIME SERVICE		//////////////
//////////////////////////////////////////////
/** @name Time Service Configurations
 */
//@{
#define CONF_TIME_SW_WDT_ENABLE				ENABLE			/**< Enable or disable the Software Watchdog Timer. */
#define CONF_TIME_SW_WDT_TIMEOUT_MS			10000			/**< Timeout of the software Watchdog timer. */
#define CONF_TIME_TIMESTAMP_FORMT			"%Y-%m-%d %T"	/**< Timestamp string format, see strftime() Posix function for more info. */
//@}



#endif /* SFSF_CONFIG_H_ */
