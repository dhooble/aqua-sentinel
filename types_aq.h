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


#ifndef TYPES_AQ_H
#define TYPES_AQ_H
    
#include "project.h"

#define STATUS_BUFFER_SIZE 15
   
union Salinity_compensation_val{ // union for salinity compensation for dissolved oxygen sensor
    uint32 val_uS;
    float val_ppt;
};
typedef union Salinity_compensation_val Salinity_compensation_val_t; // type for salinity compensation for dissolved oxygen sensor

typedef struct sensor_pH_struct {
    uint8 address; // I2C address (default 99 (0x63))
    float32 ph; // value of pH
    char status[15]; // ex : ?Status,P,5.038
    uint8 nb_cal_points; // from 0 to 3 (0 = none; 1 = midpoint; 2 = mid+lowpoint; 3 = mid+low+highpoint)
    float32 cal_points[3]; // in order : 0 = midpoint, 1 = lowpoint, 2 = highpoint (ex : {7.00,4.00,10.00})
    float32 temp_compensation; // value of temperature compensation in celcius degrees(ex : 19.5; default = 20)
     
}sensor_ph_t;

typedef struct sensor_do_struct {
    uint8 address; // I2C address (default 97 (0x61))
    float32 dissolved_oxygen; // value of dissolved oxygen (ex 7.82)
    char status[15]; // ex : ?Status,P,5.038
    uint8 nb_cal_points; // from 0 to 3 (0 = none; 1 = midpoint; 2 = mid+lowpoint; 3 = mid+low+highpoint)
    float32 cal_points[3]; // in order : 0 = midpoint, 1 = lowpoint, 2 = highpoint (ex : {7.00,4.00,10.00})
    float32 temp_compensation; // value of temperature compensation in celcius degrees(ex : 19.5; default = 20)
    Salinity_compensation_val_t salinity_compensation; // 0 - 50 000 (or more?) or 37.5
    uint8 output_parameters_mask; // XXXXXXAB : A = bit for %, B = bit for mg/L
    
    
}sensor_do_t;

typedef struct sensor_orp_struct {
    uint8 address; // I2C address (default 98 (0x62))
    float32 orp; // value of orp (ex : 209.6)
    char status[15]; // ex : ?Status,P,5.038
    uint8 nb_cal_points; // from 0 to 3 (0 = none; 1 = midpoint; 2 = mid+lowpoint; 3 = mid+low+highpoint)
    float32 cal_points[3]; // in order : 0 = midpoint, 1 = lowpoint, 2 = highpoint (ex : {7.00,4.00,10.00})
    
}sensor_orp_t;

typedef struct sensor_ec_struct {
    uint8 address; // I2C address (default 100 (0x64))
    float32 ec; // value of conductivity
    char status[15]; // ex : ?Status,P,5.038
    uint8 nb_cal_points; // from 0 to 3 (0 = none; 1 = midpoint; 2 = mid+lowpoint; 3 = mid+low+highpoint)
    float32 cal_points[3]; // in order : 0 = midpoint, 1 = lowpoint, 2 = highpoint (ex : {7.00,4.00,10.00})
    float32 temp_compensation; // value of temperature compensation in celcius degrees(ex : 19.5; default = 25)
    float32 TDS; // TDS conversion factor (range : [0.01;1.0]; default TDS = 0.54
    uint8 output_parameters_mask; // XXXXABCD : A = bit for Specific Gravity (SG), B = bit for Salinity (S), C = bit for Total Dissolved Solids(TDS), D = bit for conductivity (EC)
    float32 K; // type of probe (possible values {0.1; 1.0; 10.0}; default K = 1.0)
    
}sensor_ec_t;

typedef struct sensor_temp_ow_struct {
    float32 temperature;
    uint8 resolution; // valeurs possible : 9 bits (0b00), 10 bits (0b01), 11 bits (0b10), 12 bits (0b11)
}sensor_temp_ow_t;

typedef struct sensor_als_struct {
    float32 als; // value of Ambient Light
    union {
        uint8 conf_msb,conf_lsb;
        uint8 conf_div[2];
        uint16 conf; // configuration register
    }config;
    uint8 gain; // valeurs possible : x1 (0b00), x2 (0b01), x1/8 (0b10), x1/4 (0b11)
    uint16 integration_time; // valeurs possible : 25ms (0b1100), 50ms (0b1000), 100ms (0b0000), 200ms (0b0001), 400ms (0b0010), 800ms (0b0011)
    uint8 interrupt_power; // interrupt: on = 0xFX, off = 0x0X; power : on = 0xX0, off = 0xXF
}sensor_als_t;

#endif // TYPES_AQ_H


/* [] END OF FILE */
