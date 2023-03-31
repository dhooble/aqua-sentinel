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
#ifndef VEML7700_H
#define VEML7700_H
#include "project.h"
#include <stdio.h>
#include "globals_aq.h"

#define USBUART_BUFFER_SIZE (64u)
#define USBUART_DEVICE      (0u)
#define SIZE_MESSAGE_USB (64u)
#define VEML7700_I2CADDR_DEFAULT 0x10 ///< I2C address
#define VEML7700_ALS_CONFIG 0x00        ///< Light configuration register
#define VEML7700_ALS_THREHOLD_HIGH 0x01 ///< Light high threshold for irq
#define VEML7700_ALS_THREHOLD_LOW 0x02  ///< Light low threshold for irq
#define VEML7700_ALS_POWER_SAVE 0x03    ///< Power save regiester
#define VEML7700_ALS_DATA 0x04          ///< The light data output
#define VEML7700_WHITE_DATA 0x05        ///< The white light data output
#define VEML7700_INTERRUPTSTATUS 0x06   ///< What IRQ (if any)

#define VEML7700_INTERRUPT_HIGH 0x4000 ///< Interrupt status for high threshold
#define VEML7700_INTERRUPT_LOW 0x8000  ///< Interrupt status for low threshold

#define VEML7700_GAIN_1 0x00   ///< ALS gain 1x
#define VEML7700_GAIN_2 0x01   ///< ALS gain 2x
#define VEML7700_GAIN_1_8 0x02 ///< ALS gain 1/8x
#define VEML7700_GAIN_1_4 0x03 ///< ALS gain 1/4x

#define VEML7700_IT_100MS 0x00 ///< ALS intetgration time 100ms
#define VEML7700_IT_200MS 0x01 ///< ALS intetgration time 200ms
#define VEML7700_IT_400MS 0x02 ///< ALS intetgration time 400ms
#define VEML7700_IT_800MS 0x03 ///< ALS intetgration time 800ms
#define VEML7700_IT_50MS 0x08  ///< ALS intetgration time 50ms
#define VEML7700_IT_25MS 0x0C  ///< ALS intetgration time 25ms

#define VEML7700_PRESENCE_CODE 0x10

#define CST_LUX1 0.00000000000060135
#define CST_LUX2 0.000000093924
#define CST_LUX3 0.000081488
#define CST_LUX4 1.0023

    
uint8 vemlSearch(uint8 * detected_devices);
float lumMeasure(void);

uint8 set_gain(uint8 gain);
uint8 set_IT(uint8 it);
uint16 get_count(void);
uint8 set_SD(uint8 sd);
void increase_gain();
void increase_it();
void decrease_it();
float64 lux_calc(float32 count);
float get_resolution(void);
void reset_param(void);
float get_lum(void);

#endif
/* [] END OF FILE */
