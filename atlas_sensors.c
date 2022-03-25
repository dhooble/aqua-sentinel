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

int8 atlasSearch(uint8 *detected_devices)
{
    uint8 error = 0, address,ping_message[3] = {'L',',','?'},recieved_ping[6] = {0}; // modify to turn off LEDs
    uint8 device_presence_code = 0u;    
    char *sensor_name = "test";
    for(address = 97; address < 101; address++ )
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
        I2C_master_MasterClearStatus();
        CyDelay(300);
        I2C_master_MasterReadBuf(address,recieved_ping,6,I2C_master_MODE_COMPLETE_XFER);
        error = I2C_master_MasterStatus();
        if(error == I2C_master_MSTAT_RD_CMPLT)
        {
            //draw_divider(80, true, '-');
            USBUART_sendString("La ");
            USBUART_sendString(sensor_name);
            USBUART_sendString(" est presente");
            USBUART_PutCRLF();
            *detected_devices |= device_presence_code;
            
        }
        else
        {
            //draw_divider(80, true, '-');
            USBUART_sendString("La ");
            USBUART_sendString(sensor_name);
            USBUART_sendString(" n'est pas connectee ou est defaillante !");
            USBUART_PutCRLF();
        }
        I2C_master_MasterClearWriteBuf();
        I2C_master_MasterClearStatus();
        CyDelay(700);
    }
    return *detected_devices;
}


float atlasMeasure(const uint8 param)
{
    uint8 i = 0;                      //counter used for DO_data array.
    uint8 command_read[1] = {'R'}, buff_in[20]={0},command_sleep[5] = {'S','l','e','e','p'};
    int code, address = -1;
    char *sensor_name = NULL, *param_name = NULL, *unit_name = NULL;  
    float measure;
    if(param == PARAM_DO)
    {
        address = 97;
        param_name = "Teneur en oxygene dissous mesuree";
        sensor_name = "Sonde Atlas EZO-DO";
        unit_name = "mg/L";
    }
    else if(param == PARAM_ORP)
    {
        address = 98;
        param_name = "Potentiel redox mesure";
        sensor_name = "Sonde Atlas EZO-ORP";
        unit_name = "mV";
    }
    else if(param == PARAM_PH)
    {
        address = 99;
        param_name = "pH mesure";
        sensor_name = "Sonde Atlas EZ0O-pH";
        unit_name = "";
    }
    else if(param == PARAM_COND)
    {
        address = 100;
        param_name = "Conductivite mesuree";
        sensor_name = "Sonde Atlas EZO-EC";
        unit_name = "muS/cm";
    }
    I2C_master_MasterWriteBuf(address,command_read,1,I2C_master_MODE_COMPLETE_XFER); // sends "read value" command at address through the serial port
    CyDelay(900);
    I2C_master_MasterReadBuf(address,buff_in,20,I2C_master_MODE_COMPLETE_XFER); // read value from address through serial port (reading more than enough bytes)
    for(i = 0; i<20; i++)
    {
        if (buff_in[i] == 0)
        {                      //if we see that we have been sent a null command.
            buff_in[i] = '\0';                                 //set the last byte to '\0' for future sprintf (atof).
            break;                                 //exit the while loop.
        }
    }
    
    I2C_master_MasterWriteBuf(address,command_sleep,5,I2C_master_MODE_COMPLETE_XFER); // sends "sleep" command at address through the serial port
    USBUART_sendString("\t\uFFED ");
    USBUART_sendString(sensor_name);
    code = I2C_master_MasterStatus();
    if(code == I2C_master_MSTAT_WR_CMPLT)
    {
        sscanf((const char*)buff_in,"%f",&measure);
        USBUART_sendString("\n\t\t\u2714 ");
        USBUART_sendString(param_name);
        USBUART_sendString(" : ");
        USBUART_PutString((char *)buff_in);
        //USBUART_(MyMeasure);
        USBUART_sendString(" ");
        USBUART_sendString(unit_name);
        USBUART_PutCRLF();
//        draw_divider(80, true, '*');
        return measure;
    }
    else
//    {
        USBUART_sendString("\n\t\t\u2714 La sonde n'est pas connectee ou est defaillante ");
        USBUART_PutCRLF();
//        draw_divider(80, true, '*');
//        return -99.0;
//    }

    return 0;
}

/* [] END OF FILE */
