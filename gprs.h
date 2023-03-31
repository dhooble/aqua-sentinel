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

#ifndef GPRS_H
#define GPRS_H

#include "project.h"
#include <string.h>
#include <stdio.h>

#define FREQ_COUNTER    24000000u
#define REQUEST_SIZE    256u
#define ANSWER_SIZE     16u
#define IP_ADDRESS_LEN  16u
    
 
void init_gprs(void);

float get_network_strength(char *chaine_result);

char* get_IP(void);

int8 is_gprs_connected(void);

int8 send_request(float temp, float lum, float ph, float cond, float oxy, float red);

void sim_reset(void);

void updateSerial(void);

void get_serial_message(int32 waiting_delay, char *message);

uint16 get_period(uint16 time_ms);

void float_to_string(const float value, char *result);

#endif

/* [] END OF FILE */
