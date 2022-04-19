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

#ifndef DS18B20_H
#define DS18B20_H

#include "project.h"
#include <stdio.h>
    
uint8 OW_Detect(void);
void OW_WriteByte(uint8 u8);
uint8 OW_ReadByte(void);
float compute_measure(uint8 *measure);
float tempMeasure(void);

#endif

/* [] END OF FILE */
