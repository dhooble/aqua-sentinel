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


#include "ina3221.h"

/**
*   Function : ina3221_ping send a ping the INA3221.
*
*   The function sends a ping to the INA3221 via I2C. The instruction sent is 
*   a request to read the Manufacturer ID register.
*   
*   @return 1 if a INA3221 is connected, 0 otherwise.
*/
uint8 ina3221_ping(void)
{
    uint8 target_reg = INA3221_REG_MANUFACTURER_ID;
    //uint8 addr_read = (INA3221_ADDR<<1)|1; //addr + read bit
    union buffer_t buffer;

    I2C_master_MasterWriteBuf(INA3221_ADDR/*addr_read*/,&target_reg,1,I2C_master_MODE_COMPLETE_XFER);
    CY_NOP;
    I2C_master_MasterReadBuf(INA3221_ADDR/*addr_read*/,buffer.tab,2,I2C_master_MODE_COMPLETE_XFER);
    /*
        Add the status check for I2C communication result
    */
    if(buffer.value != 0x3220)
    {
        return 0;
    }
    
    
    
    return 1;
}

/**
*   Function : ina3221_setConf sends the configuration parameters to the INA3221.
*
*   The function sends the configuration parameters to the INA3221 via I2C.
*   
*   @param  conf is an unsigned int (uint16) gathering all the parameters for the Configuration register.
*   @return 1 if the transfer succeeded, 0 otherwise.
*/
uint8 ina3221_setConf(uint16 conf)
{
    uint8 target_reg = INA3221_REG_CONF;
    union buffer_t buffer;
    buffer.value = conf;
    
    I2C_master_MasterWriteBuf(INA3221_ADDR/*addr_read*/,&target_reg,1,I2C_master_MODE_COMPLETE_XFER);
    I2C_master_MasterWriteBuf(INA3221_ADDR,buffer.tab,2,I2C_master_MODE_COMPLETE_XFER);
    /*
        Add the status check for I2C communication result
    */
    
    return 0;
}

/**
*   Function : ina3221_getConf retrieves the information from the Configuration register.
*
*   The function ina3221_getConf retrieves the information from the Configuration register.
*   
*   @return The value of the Configuration register.
*/
uint16 ina3221_getConf(void)
{
    uint8 target_reg = INA3221_REG_CONF;
    union buffer_t buffer;
    buffer.value = 0;
    
    I2C_master_MasterWriteBuf(INA3221_ADDR/*addr_read*/,&target_reg,1,I2C_master_MODE_COMPLETE_XFER);
    I2C_master_MasterReadBuf(INA3221_ADDR,buffer.tab,2,I2C_master_MODE_COMPLETE_XFER);
    /*
        Add the status check for I2C communication result
    */
    
    return buffer.value;
}

/**
*   Function : ina3221_getShuntVoltage retrieves the value of the Shunt Voltage from the specified channel.
*
*   The function retrieves the value of the Shunt Voltage from the channel specified in parameter.
*   
*   @param  channel is an unsigned char (uint8) that point to a specific channel (1-3).
*   @return the value of the Shunt Voltage of the specified channel.
*/
uint16 ina3221_getShuntVoltage(uint8 channel)
{
    uint8 target_reg;
    union buffer_t buffer ={.value=0};
    
    switch(channel)
    {
        case 1:
            target_reg = INA3221_REG_CH1_SHUNT;
            break;
        case 2:
            target_reg = INA3221_REG_CH2_SHUNT;
            break;
        case 3:
            target_reg = INA3221_REG_CH2_SHUNT;
            break;
        default:
            return 0;
    }
    
    I2C_master_MasterWriteBuf(INA3221_ADDR/*addr_read*/,&target_reg,1,I2C_master_MODE_COMPLETE_XFER);
    I2C_master_MasterReadBuf(INA3221_ADDR,buffer.tab,2,I2C_master_MODE_COMPLETE_XFER);
    /*
        Add the status check for I2C communication result
    */
    
    return buffer.value;
}


/**
*   Function : ina3221_getBusVoltage retrieves the value of the Shunt Voltage from the specified channel.
*
*   The function retrieves the value of the Bus Voltage from the channel specified in parameter.
*   
*   @param  channel is an unsigned char (uint8) that point to a specific channel (1-3).
*   @return the value of the Bus Voltage of the specified channel.
*/
uint16 ina3221_getBusVoltage(uint8 channel)
{
    uint8 target_reg;
    union buffer_t buffer ={.value=0};
    
    switch(channel)
    {
        case 1:
            target_reg = INA3221_REG_CH1_BUS;
            break;
        case 2:
            target_reg = INA3221_REG_CH2_BUS;
            break;
        case 3:
            target_reg = INA3221_REG_CH2_BUS;
            break;
        default:
            return 0;
    }
    
    I2C_master_MasterWriteBuf(INA3221_ADDR/*addr_read*/,&target_reg,1,I2C_master_MODE_COMPLETE_XFER);
    I2C_master_MasterReadBuf(INA3221_ADDR,buffer.tab,2,I2C_master_MODE_COMPLETE_XFER);
    /*
        Add the status check for I2C communication result
    */
    
    return buffer.value;
}


/* [] END OF FILE */
