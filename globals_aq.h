/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/


#ifndef GLOBALS_AQ_H
#define GLOBALS_AQ_H
//#include <stdio.h>
#include "project.h"
#include "types_aq.h"

#define DO_ATLAS_SENSOR_DEFAULT_ADDRESS 97
#define ORP_ATLAS_SENSOR_DEFAULT_ADDRESS 98
#define PH_ATLAS_SENSOR_DEFAULT_ADDRESS 99
#define EC_ATLAS_SENSOR_DEFAULT_ADDRESS 100

extern char* global_site_name; // used in URL for data upload
extern uint8 global_id;
extern uint8 global_run_parse_loop;
extern uint8 global_uart_loop;
extern char global_uart_buffer[128]; // #define BUFFER_SIZE (128u)
extern uint8 global_connected_devices;

extern sensor_ph_t sensor_ph;
extern sensor_do_t sensor_do;
extern sensor_orp_t sensor_orp;
extern sensor_ec_t sensor_ec;
extern sensor_temp_ow_t sensor_temp_ow;
extern sensor_als_t sensor_als;


    
#endif // GLOBALS_AQ_H


/* [] END OF FILE */
