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
#ifndef ATLAS_SENSORS_H
#define ATLAS_SENSORS_H
#include "project.h"
#include "globals_aq.h"
#include <stdio.h>

#define PARAM_PH      (0x01)
#define PARAM_COND    (0x02)
#define PARAM_DO      (0x04)
#define PARAM_ORP     (0x08)

int8 atlasSearch(uint8 *detected_devices); // Checks for Atlas Scientific sensors connected through I2C
float atlasMeasure(const uint8 param); // Takes a measure from specified Atlas Scientific sensor

#endif
/* [] END OF FILE */
