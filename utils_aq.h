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
#ifndef UTILS_AQ_H
#define UTILS_AQ_H

#include "project.h"
#include "types_aq.h"
#include "globals_aq.h"
#include <stdio.h>
  
#define BUFFER_SIZE (128u)

    
//functions
    
uint8 check_token(int8 *token, int16 len_token, int8 * str, int16 len_str);

uint8 cpy_mem(int8 * dest, int8 * src, uint16 size);

void reset_timer_timeout_period(uint32 period);

void reset_timer_measure_period(uint32 period);

uint8 is_str_alpha_num(int8 byte);

uint8 parse_command(int8 *in_buffer, int8 parsed_str[][32]);

void maintenance_utils_aq(void);

#endif //UTILS_AQ_H
/* [] END OF FILE */
