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
#include "atlas_sensors.h"

/**
*   Function : atlasSearch find every Atlas sensors connected to the controller
*
*   The function ping the default address of the agreed Atlas sensors through the
*   the I2C bus. The command sent is asking for LED status and upon positive answer
*   the bit representing the sensor is added to the variable named detected_device.
*
*   @param  detected_sensor is an unsigned char (uint8) pointer acting as the list of connected sensors.
*   @return The code listing the detected (Atlas) sensors.
*/
int8 atlasSearch(uint8 *detected_devices)
{
    uint8 error = 0, address,ping_message[3] = {'L',',','?'},recieved_ping[6] = {0}; // modify to turn off LEDs
    uint8 device_presence_code = 0u;    
    char *sensor_name = "test";
    for(address = 97; address < 101; address++)
    {
        switch(address)
        {
            case 97:                                 
                sensor_name = "sonde Atlas EZO-DO";
                device_presence_code = PARAM_DO;
                break; 
            case 98:
                sensor_name = "sonde Atlas EZO-ORP";            
                device_presence_code = PARAM_ORP;
                break; 
            case 99:
                sensor_name = "sonde Atlas EZO-pH";
                device_presence_code = PARAM_PH;
                break; 
            case 100:
                sensor_name = "sonde Atlas EZO-EC";
                device_presence_code = PARAM_COND;
                break; 
        }
        //open i2c comm at address
        I2C_master_MasterWriteBuf(address,ping_message,3,I2C_master_MODE_COMPLETE_XFER);
        CyDelay(300);        
        I2C_master_MasterClearStatus();
        I2C_master_MasterReadBuf(address,recieved_ping,6,I2C_master_MODE_COMPLETE_XFER);
        CyDelay(100);
        error = I2C_master_MasterStatus();
        if(error == I2C_master_MSTAT_RD_CMPLT)
        {
            //draw_divider(80, true, '-');
            UART_PC_PutString("La ");
            UART_PC_PutString(sensor_name);
            UART_PC_PutString(" est presente");
            UART_PC_PutCRLF('\0');
            *detected_devices |= device_presence_code;
            
        }
        else
        {
            //draw_divider(80, true, '-');
            UART_PC_PutString("La ");
            UART_PC_PutString(sensor_name);
            UART_PC_PutString(" n'est pas connectee ou est defaillante !");
            UART_PC_PutCRLF('\0');
            *detected_devices &= ~device_presence_code;
        }
        I2C_master_MasterClearWriteBuf();
        I2C_master_MasterClearStatus();
        CyDelay(700);
    }
    return *detected_devices;
}

/**
*   Function : atlasMeasure take a measure from the specified sensor
*
*   The function take a measure from the specified Atlas sensors through the
*   the I2C bus. The command sent is a simple one value read and (for now) 
*   send the read value toward the PC throught UART and then return it as 
*   a float.
*
*   @param  param is an unsigned char (uint8) that represent the chosen sensor.
*   @return The read value from the chosen sensor.
*/
float atlasMeasure(const uint8 param)
{
    uint8 i = 0;                      //counter used for DO_data array.
    uint8 command_read[1] = {'R'}, buff_in[20]={0},command_sleep[5] = {'S','l','e','e','p'};
    uint16 answer_delay = 900;
    int code, address = -1;
    char *sensor_name = NULL, *param_name = NULL, *unit_name = NULL;  
    float measure;
    
    if(param == PARAM_DO)
    {
        address = sensor_do.address;
        param_name = "Teneur en oxygene dissous mesuree";
        sensor_name = "Sonde Atlas EZO-DO";
        unit_name = "mg/L";
        answer_delay = 600;
    }
    else if(param == PARAM_ORP)
    {
        address = sensor_orp.address;
        param_name = "Potentiel redox mesure";
        sensor_name = "Sonde Atlas EZO-ORP";
        unit_name = "mV";
    }
    else if(param == PARAM_PH)
    {
        address = sensor_ph.address;
        param_name = "pH mesure";
        sensor_name = "Sonde Atlas EZO-pH";
        unit_name = "";
    }
    else if(param == PARAM_COND)
    {
        address = sensor_ec.address;
        param_name = "Conductivite mesuree";
        sensor_name = "Sonde Atlas EZO-EC";
        unit_name = "muS/cm";
    }

/*    I2C_master_MasterClearReadBuf();
    I2C_master_MasterClearWriteBuf();
    I2C_master_MasterStatus();
    CyDelay(100);*/
    I2C_master_MasterWriteBuf(address,command_read,1,I2C_master_MODE_COMPLETE_XFER); // sends "read value" command at address through the serial port
    CyDelay(answer_delay);
    code = I2C_master_MasterStatus();
    I2C_master_MasterReadBuf(address,buff_in,12,I2C_master_MODE_COMPLETE_XFER); // read value from address through serial port (reading more than enough bytes)
    for(i = 0; i<20; i++)
    {
        if (buff_in[i] == 0)
        {                      //if we see that we have been sent a null command.
            buff_in[i] = '\0';                                 //set the last byte to '\0' for future sprintf (atof).
            break;                                 //exit the while loop.
        }
    }
    
    I2C_master_MasterWriteBuf(address,command_sleep,5,I2C_master_MODE_COMPLETE_XFER); // sends "sleep" command at address through the serial port
    UART_PC_PutString("\t");
    UART_PC_PutString(sensor_name);
    UART_PC_PutCRLF('\0');
    CyDelay(100);
    code = I2C_master_MasterStatus();
    if(code & I2C_master_MSTAT_WR_CMPLT)
    {
        
        I2C_master_MasterClearReadBuf();
        I2C_master_MasterClearWriteBuf();
        I2C_master_MasterClearStatus();
        sscanf((const char*)buff_in,"%f",&measure);
        UART_PC_PutString("\t\t ");
        UART_PC_PutString(param_name);
        UART_PC_PutString(" : ");
        UART_PC_PutString((char *)buff_in);
        UART_PC_PutString(" ");
        UART_PC_PutString(unit_name);
        UART_PC_PutCRLF('\0');
//        draw_divider(80, true, '*');
        return measure;
    }
    
    UART_PC_PutString("\n\t\t La sonde n'est pas connectee ou est defaillante ");
    UART_PC_PutCRLF('\0');
//        draw_divider(80, true, '*');
//        return -99.0;
    
    return 0;
}

/* [] END OF FILE */
