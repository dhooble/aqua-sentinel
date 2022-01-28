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
#ifndef USBUART_UTILS_H
#define USBUART_UTILS_H

#include "project.h"

#define USBUART_BUFFER_SIZE (64u)
    
int init_USBUART(void);
int8 USBUART_recieveString(uint8 *recived_message);
int8 USBUART_sendString(char* sent_message);

#endif

/* [] END OF FILE */
