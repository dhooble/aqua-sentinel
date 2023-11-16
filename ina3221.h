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



#ifndef INA3221_H
#define INA3221_H

#include "project.h"

#define INA3221_ADDR                    0x42
//************************ Register Address const ************************
#define INA3221_REG_CONF                0x00 //Configuration
#define INA3221_REG_CH1_SHUNT           0x01 //Channel-1 Shunt Voltage
#define INA3221_REG_CH1_BUS             0x02 //Channel-1 Bus Voltage
#define INA3221_REG_CH2_SHUNT           0x03 //Channel-2 Shunt Voltage
#define INA3221_REG_CH2_BUS             0x04 //Channel-2 Bus Voltage
#define INA3221_REG_CH3_SHUNT           0x05 //Channel-3 Shunt Voltage
#define INA3221_REG_CH3_BUS             0x06 //Channel-3 Bus Voltage
#define INA3221_REG_CH1_CRITICAL        0x07 //Channel-1 Critical-Alert Limit
#define INA3221_REG_CH1_WARNING         0x08 //Channel-1 Warning-Alert Limit
#define INA3221_REG_CH2_CRITICAL        0x09 //Channel-2 Critical-Alert Limit
#define INA3221_REG_CH2_WARNING         0x0A //Channel-2 Warning-Alert Limit
#define INA3221_REG_CH3_CRITICAL        0x0B //Channel-3 Critical-Alert Limit
#define INA3221_REG_CH3_WARNING         0x0C //Channel-3 Warning-Alert Limit
#define INA3221_REG_SHUNT_VOLT_SUM      0x0D //Shunt-Voltage Sum
#define INA3221_REG_SHUNT_VOLT_SUM_LIM  0x0E //Shunt-Voltage Sum Limit
#define INA3221_REG_MASK_EN             0x0F //Mask/Enable
#define INA3221_REG_POWER_VALID_UP_LIM  0x10 //Power-Valid Upper Limit
#define INA3221_REG_POWER_VALID_DN_LIM  0x11 //Power-Valid Lower Limit
#define INA3221_REG_MANUFACTURER_ID     0xFE //Manufacturer ID (0x1512(01010100010010))
#define INA3221_REG_DIE_ID              0xFF //Die ID (0x0C88(00110010001000))

//***************************** Config const *****************************
#define INA3221_CONF_RST    0x8000

#define INA3221_CONF_EN_CH1 0x4000
#define INA3221_CONF_EN_CH2 0x2000 // default : 0x7000
#define INA3221_CONF_EN_CH3 0x1000

#define INA3221_CONF_AVG_1      0x0000
#define INA3221_CONF_AVG_4      0x0200
#define INA3221_CONF_AVG_16     0x0400
#define INA3221_CONF_AVG_64     0x0600 // default : 0x0000
#define INA3221_CONF_AVG_128    0x0800
#define INA3221_CONF_AVG_256    0x0A00
#define INA3221_CONF_AVG_512    0x0C00
#define INA3221_CONF_AVG_1024   0x0E00

#define INA3221_CONF_CT_BUS_140US       0x0000
#define INA3221_CONF_CT_BUS_204US       0x0040
#define INA3221_CONF_CT_BUS_332US       0x0080
#define INA3221_CONF_CT_BUS_588US       0x00C0
#define INA3221_CONF_CT_BUS_1_1MS       0x0100 // default : 0x0100
#define INA3221_CONF_CT_BUS_2_116MS     0x0140
#define INA3221_CONF_CT_BUS_4_156MS     0x0180
#define INA3221_CONF_CT_BUS_8_244MS     0x01C0

#define INA3221_CONF_SH_BUS_140US       0x0000
#define INA3221_CONF_SH_BUS_204US       0x0008
#define INA3221_CONF_SH_BUS_332US       0x0010
#define INA3221_CONF_SH_BUS_588US       0x0018
#define INA3221_CONF_SH_BUS_1_1MS       0x0020 // default : 0x0020
#define INA3221_CONF_SH_BUS_2_116MS     0x0028
#define INA3221_CONF_SH_BUS_4_156MS     0x0030
#define INA3221_CONF_SH_BUS_8_244MS     0x0038

#define INA3221_CONF_MODE_POWER_DOWN            0x0
#define INA3221_CONF_MODE_SHUNT_SINGLE_SHOT     0x1
#define INA3221_CONF_MODE_BUS_SINGLE_SHOT       0x2
#define INA3221_CONF_MODE_SHUNT_BUS_SINGLE_SHOT 0x3
#define INA3221_CONF_MODE_POWER_DOWN2           0x4
#define INA3221_CONF_MODE_SHUNT_CONTINUOUS      0x5
#define INA3221_CONF_MODE_BUS_CONTINUOUS        0x6
#define INA3221_CONF_MODE_SHUNT_BUS_CONTINUOUS  0x7 // default : 0x0007

    
//****************************** Data Type *******************************
union buffer_t
{
    uint8 tab[2];
    uint16 value;
};

//****************************** Functions *******************************
uint8 ina3221_ping(void);
uint8 ina3221_setConf(uint16 conf);
uint16 ina3221_getConf(void);

uint16 ina3221_getShuntVoltage(uint8 channel);
uint16 ina3221_getBusVoltage(uint8 channel);



#endif




/* [] END OF FILE */
