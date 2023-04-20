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
#include "veml7700.h"


float32 tab_resolution[4][6] = {{0.0576,0.0288,0.0144,0.0072,0.1152,0.2304},{0.0288,0.0144,0.0072,0.0036,0.0576,0.01152},{0.4608,0.2304,0.1152,0.0576,0.9216,1.8432},{0.2304,0.1152,0.0576,0.0288,0.4608,0.9216}};

/**
*   Function : vemlSearch pings the I2C bus to find a VEML7700
*
*   The function pings the I2C bus by sending the default config to the sensor
*
*   @param  detected_devices is a char (uint8) pointer used as the list of connected devieces.
*   @return 0 if the process failed and any positive value otherwise (uint8).
*/
uint8 vemlSearch(uint8 * detected_devices)
{
    uint8 VEML7700success;
    uint8 conf[3] = {VEML7700_ALS_CONFIG,sensor_als.config.conf_lsb,sensor_als.config.conf_msb}; // config code for gain at 1/8 and integration time at 100ms
    VEML7700success = I2C_master_MasterWriteBuf(VEML7700_I2CADDR_DEFAULT,conf,3,I2C_master_MODE_COMPLETE_XFER);
    if(VEML7700success == I2C_master_MSTR_NO_ERROR)
    {
        CyDelay(100);
        VEML7700success = I2C_master_MasterStatus();
        if(VEML7700success == I2C_master_MSTAT_WR_CMPLT)
        {
            I2C_master_MasterClearWriteBuf();
            I2C_master_MasterClearStatus();
            *detected_devices |= VEML7700_PRESENCE_CODE; 
            UART_PC_PutString("Capteur lum connecte\r\n");
            return 0;
        }
    }
    
    UART_PC_PutString("Capteur lum non connecte\r\n");
    return(VEML7700success);
}

/**
*   Function : getStatus sends the content of the status register
*
*   The function interprets the value recieved from the fonction I2C_master_MasterStatus()
*   and sends the interpretation of the status register to the maintenance serial 
*   connection. 
*
*   @param  status is a uint8 value recieved from the fonction I2C_master_MasterStatus().
*/
void getStatus(uint8 status)
{
    uint8 VEML7700success = I2C_master_MasterStatus();
    UART_PC_PutString("\t[INFO](lumMeasure): first status : ");
    if(status == I2C_master_MSTR_NO_ERROR) UART_PC_PutString("I2C_master_MSTR_NO_ERROR ");
    if(status == I2C_master_MSTR_BUS_BUSY) UART_PC_PutString("I2C_master_MSTR_BUS_BUSY ");
    if(status == I2C_master_MSTR_NOT_READY) UART_PC_PutString("I2C_master_MSTR_NOT_READY ");
    if(status == I2C_master_MSTR_ERR_LB_NAK) UART_PC_PutString("I2C_master_MSTR_ERR_LB_NAK ");
    if(status == I2C_master_MSTR_ERR_ARB_LOST) UART_PC_PutString("I2C_master_MSTR_ERR_ARB_LOST ");
    if(status == I2C_master_MSTR_ERR_ABORT_START_GEN) UART_PC_PutString("I2C_master_MSTR_ERR_ABORT_START_GEN ");
    UART_PC_PutCRLF('\0');
    UART_PC_PutString("\t[INFO](lumMeasure): master status : ");
    if(VEML7700success & I2C_master_MSTAT_WR_CMPLT) UART_PC_PutString("I2C_master_MSTAT_WR_CMPLT ");
    if(VEML7700success & I2C_master_MSTAT_RD_CMPLT) UART_PC_PutString("I2C_master_MSTAT_RD_CMPLT ");
    if(VEML7700success & I2C_master_MSTAT_XFER_INP) UART_PC_PutString("I2C_master_MSTAT_XFER_INP ");
    if(VEML7700success & I2C_master_MSTAT_XFER_HALT) UART_PC_PutString("I2C_master_MSTAT_XFER_HALT ");
    if(VEML7700success & I2C_master_MSTAT_ERR_SHORT_XFER) UART_PC_PutString("I2C_master_MSTAT_ERR_SHORT_XFER ");
    if(VEML7700success & I2C_master_MSTAT_ERR_ADDR_NAK) UART_PC_PutString("I2C_master_MSTAT_ERR_ADDR_NAK ");
    if(VEML7700success & I2C_master_MSTAT_ERR_ARB_LOST) UART_PC_PutString("I2C_master_MSTAT_ERR_ARB_LOST ");
    if(VEML7700success & I2C_master_MSTAT_ERR_XFER) UART_PC_PutString("I2C_master_MSTAT_ERR_XFER ");
    UART_PC_PutCRLF('\0');
}

/**
*   Function : [DEPRECATED] lumMeasure get the value from the VEML7700 
*
*   [DEPRECATED]
*   The function reads the value of ALS from the register of the VEML7700.
*   [DEPRECATED]
*
*   @return The raw value (float) from the sensor.
*/
float lumMeasure(void)
{
    uint16 MyMeasure = 0;
    uint8 VEML7700success;
    uint8 read[3]={VEML7700_ALS_DATA,0u,0u};
    char text_val[13];
    
    VEML7700success = I2C_master_MasterSendStart(VEML7700_I2CADDR_DEFAULT,0);
    VEML7700success = I2C_master_MasterWriteByte(VEML7700_ALS_DATA);
    if(VEML7700success != I2C_master_MSTR_NO_ERROR)
    {
        UART_PC_PutString("[INFO](lumMeasure): master write error.\r\n");
        return -1;
    }
    I2C_master_MasterSendRestart(VEML7700_I2CADDR_DEFAULT,1);
    read[0] = I2C_master_MasterReadByte(I2C_master_ACK_DATA);
    read[1] = I2C_master_MasterReadByte(I2C_master_NAK_DATA);
    VEML7700success = I2C_master_MasterSendStop();
    if(VEML7700success != I2C_master_MSTR_NO_ERROR)
    {
        UART_PC_PutString("[INFO](lumMeasure): master read error.\r\n");
        getStatus(VEML7700success);
        return -1;
    }
    I2C_master_MasterClearReadBuf();
    I2C_master_MasterClearWriteBuf();
    I2C_master_MasterClearStatus();
    CyDelay(1000);
    MyMeasure = read[1];
    MyMeasure = (MyMeasure << 8) | read[0];
    sprintf(text_val,"%f",4*MyMeasure* 0.0576);
    UART_PC_PutString("\tluminosite mesuree : ");
    UART_PC_PutString(text_val);
    UART_PC_PutString("lx");
    UART_PC_PutCRLF('\0');
    return 4*MyMeasure* 0.0576; // calcul extrait de la bibliothèque originale (4* => gain)
}

/**
*   Function : set_gain set the gain of the sensor
*
*   The function sends the new gain to the VEM7700.
*   VEML7700_GAIN_1   (00) = ALS gain x 1
*   VEML7700_GAIN_2   (01) = ALS gain x 2
*   VEML7700_GAIN_1_8 (10) = ALS gain x (1/8)
*   VEML7700_GAIN_1_4 (11) = ALS gain x (1/4)
*
*   @param  gain is a uint8.
*   @return 0 if gain is already set to wanted value and 1 otherwise.
*/
uint8 set_gain(uint8 gain)
{
    uint8 conf[3] = {VEML7700_ALS_CONFIG,sensor_als.config.conf_lsb,0};
    if(sensor_als.gain == gain) return 0;
    sensor_als.gain = gain;
    sensor_als.config.conf_msb = (sensor_als.config.conf_msb & 0xE7) | (gain<<3);
    conf[2] = sensor_als.config.conf_msb;
    I2C_master_MasterWriteBuf(VEML7700_I2CADDR_DEFAULT,conf,3,I2C_master_MODE_COMPLETE_XFER);
    I2C_master_MasterClearWriteBuf();
    I2C_master_MasterClearStatus();
    UART_PC_PutString("[set_gain : done, exiting\r\n");
    return 1;
}

/**
*   Function : set_IT set the integration time of the sensor
*
*   The function sends the integration time as a code to the VEML7700.
*   VEML7700_IT_100MS (0000) = 100ms
*   VEML7700_IT_200MS (0001) = 200ms
*   VEML7700_IT_400MS (0010) = 400ms
*   VEML7700_IT_800MS (0011) = 800ms
*   VEML7700_IT_50MS  (1000) = 50ms
*   VEML7700_IT_25MS  (1100) = 25ms
*
*   @param  it is a uint8. Values are constants.
*   @return 0 if integration time is already set to wanted value and 1 otherwise.
*/
uint8 set_IT(uint8 it)
{
    uint8 conf[3] = {VEML7700_ALS_CONFIG,0,0};
    if(sensor_als.integration_time == it) return 0;
    sensor_als.integration_time = it;
    sensor_als.config.conf_lsb = (sensor_als.config.conf_lsb & 0x3F) | (it<<6);
    sensor_als.config.conf_msb = (sensor_als.config.conf_msb & 0xFC) | (it>>2);
    conf[1] = sensor_als.config.conf_lsb;
    conf[2] = sensor_als.config.conf_msb;
    I2C_master_MasterWriteBuf(VEML7700_I2CADDR_DEFAULT,conf,3,I2C_master_MODE_COMPLETE_XFER);
    return 1;
}

/**
*   Function : get_count get the raw value of ALS from the sensor
*
*   The function get the raw value of ALS from the sensor. This function is yet 
*   unstable.
*
*   @return -1 if an incident occured, the value from the sensor otherwise.
*/
uint16 get_count(void)
{
    int16 MyMeasure = 0;
    uint8 VEML7700success;
    uint8 read[3]={VEML7700_ALS_DATA,0u,0u};
    char text_val[32];
    I2C_master_MasterClearReadBuf();
    I2C_master_MasterClearWriteBuf();
    I2C_master_MasterClearStatus();
    VEML7700success = I2C_master_MasterSendStart(VEML7700_I2CADDR_DEFAULT,0);
    if(VEML7700success != I2C_master_MSTR_NO_ERROR)
    {
        UART_PC_PutString("[INFO](get_count): master start error.\r\n");
        getStatus(VEML7700success);
        return -1;
    }
    VEML7700success = I2C_master_MasterWriteByte(VEML7700_ALS_DATA);
    if(VEML7700success != I2C_master_MSTR_NO_ERROR)
    {
        UART_PC_PutString("[INFO](gewt_count): master write error.\r\n");
        getStatus(VEML7700success);
        return -1;
    }
    VEML7700success = I2C_master_MasterSendRestart(VEML7700_I2CADDR_DEFAULT,1);
    if(VEML7700success != I2C_master_MSTR_NO_ERROR)
    {
        UART_PC_PutString("[INFO](get_count): master read error.\r\n");
        getStatus(VEML7700success);
        return -1;
    }
    read[0] = I2C_master_MasterReadByte(I2C_master_ACK_DATA);
    read[1] = I2C_master_MasterReadByte(I2C_master_NAK_DATA);
    I2C_master_MasterSendStop();
    MyMeasure = read[1];
    MyMeasure = (MyMeasure << 8) | read[0];
    I2C_master_MasterClearReadBuf();
    I2C_master_MasterClearWriteBuf();
    I2C_master_MasterClearStatus();
    sprintf(text_val,"%d\r\n",MyMeasure);
    UART_PC_PutString("[INFO](get_count): count mesured = ");
    UART_PC_PutString(text_val);
    return MyMeasure;
}

/**
*   Function : set_SD turns the sensor on or off
*
*   The function turns the sensor on or off
*
*   @param  is a uint8. 1 for on and 0 for off
*   @return 1 if operation was successful, -1 otherwise.
*/
uint8 set_SD(uint8 sd)
{
    uint8 conf[3] = {VEML7700_ALS_CONFIG,0,sensor_als.config.conf_msb};
    uint8 VEML7700success;
    sensor_als.config.conf_lsb = sd?(sensor_als.config.conf_lsb | 0x01):(sensor_als.config.conf_lsb & 0xFE);
    conf[1] = sensor_als.config.conf_lsb;
    I2C_master_MasterWriteBuf(VEML7700_I2CADDR_DEFAULT,conf,3,I2C_master_MODE_COMPLETE_XFER);
    if(VEML7700success != I2C_master_MSTR_NO_ERROR)
    {
        UART_PC_PutString("[INFO](set_SD): master write error.\r\n");
        getStatus(VEML7700success);
        return -1;
    }
    return 1;
}

/**
*   Function : increase_gain increases the gain of the sensor by one step
*
*   The function increases the gain of the sensor by one step until the maximum is reached.
*
*/
void increase_gain()
{
    
    UART_PC_PutString("increase gain\r\n");
    switch(sensor_als.gain)
    {
        case VEML7700_GAIN_1:
            UART_PC_PutString("   from 1 to 2\r\n");
            set_gain(VEML7700_GAIN_2);
            break;
        case VEML7700_GAIN_2:
            UART_PC_PutString("   stop increase at 2\r\n");
            set_gain(VEML7700_GAIN_1_8);
            break;
        case VEML7700_GAIN_1_4:
            UART_PC_PutString("   from 1/4 to 1\r\n");
            set_gain(VEML7700_GAIN_1);
            break;
        case VEML7700_GAIN_1_8:
            UART_PC_PutString("   from 1/8 to 1/4\r\n");
            set_gain(VEML7700_GAIN_1_4);
            break;
    }
}

/**
*   Function : increase_it increases the integration time of the sensor by one step
*
*   The function increases the integration time of the sensor by one step until the maximum is reached.
*
*/
void increase_it()
{
    
    UART_PC_PutString("increase IT\r\n");
        switch(sensor_als.integration_time)
    {
        case VEML7700_IT_25MS:
            set_IT(VEML7700_IT_50MS);
            break;
        case VEML7700_IT_50MS:
            set_IT(VEML7700_IT_100MS);
            break;
        case VEML7700_IT_100MS:
            set_IT(VEML7700_IT_200MS);
            break;
        case VEML7700_IT_200MS:
            set_IT(VEML7700_IT_400MS);
            break;
        case VEML7700_IT_400MS:
            set_IT(VEML7700_IT_800MS);
            break;
        case VEML7700_IT_800MS:
            set_IT(VEML7700_IT_25MS);
            break;
    }
}

/**
*   Function : decrease_it decreases the integration time of the sensor by one step
*
*   The function decreases the integration time of the sensor by one step until the minimum is reached.
*
*/
void decrease_it()
{
    UART_PC_PutString("decrease IT\r\n");
    switch(sensor_als.integration_time)
    {
        case VEML7700_IT_25MS:
            UART_PC_PutString("  no more decrease\r\n");
            //set_IT(VEML7700_IT_800MS);
            break;
        case VEML7700_IT_50MS:
            UART_PC_PutString("  from 50 to 25\r\n");
            set_IT(VEML7700_IT_25MS);
            break;
        case VEML7700_IT_100MS:
            UART_PC_PutString("  from 100 to 50\r\n");
            set_IT(VEML7700_IT_50MS);
            break;
        case VEML7700_IT_200MS:
            UART_PC_PutString("  from 200 to 100\r\n");
            set_IT(VEML7700_IT_100MS);
            break;
        case VEML7700_IT_400MS:
            UART_PC_PutString("  from 400 to 200\r\n");
            set_IT(VEML7700_IT_200MS);
            break;
        case VEML7700_IT_800MS:
            UART_PC_PutString("  from 800 to 400\r\n");
            set_IT(VEML7700_IT_400MS);
            break;
    }
}

/**
*   Function : get_resolution Returns the current resolution of the sensor
*
*   The function Returns the current resolution of the sensor depending on the value
*   of the gain and the integration time. The values are stored in a float32 array.
*
*   @return The current resolution as a float32.
*/
float32 get_resolution(void)
{
    uint8 index_gain = sensor_als.gain;
    uint8 index_it = sensor_als.integration_time;
    index_it = (index_it == VEML7700_IT_50MS)? 4:index_it;
    index_it = (index_it == VEML7700_IT_25MS)? 5:index_it;
    return tab_resolution[index_gain][index_it];
}

/**
*   Function : power compute the operation value to the power of exp
*
*   The function compute the operation value to the power of exp through a loop.
*
*   @param  value is a float32 value that will be raised to the power of exp.
*   @param  exp is a unsigned char (uint8) used as exponent.
*   @return The result as a float32.
*/
float32 power(float32 value, uint8 exp)
{
    float32 result = value;
    uint8 index_power;
    for(index_power = 1; index_power < exp; index_power++)
    {
        result *= value;
    }
    return result;
}

/**
*   Function : lux_calc compute the ALS with sensor correction
*
*   The function compute the Ambiant Light Sensor value with sensor correction. The
*   follow the instructions listed in the document "Designing the VEML7700 Onto an 
*   application".
*
*   @param  count is a char pointer addressed to where the resulting string will be stored.
*   @return The quality of the signal as a float.
*/
float64 lux_calc(float32 count)
{
    float32 x = count*get_resolution();
    char value[64];
 
    UART_PC_PutString("Lum calc\r\n");
    sprintf(value,"  valeur de x %f\r\n",x);
    UART_PC_PutString(value);
    sprintf(value,"  valeur de x4 %f\r\n",power(x,4));
    UART_PC_PutString(value);
    sprintf(value,"%f",CST_LUX1*power(x,4)-CST_LUX2*power(x,3)+CST_LUX3*power(x,2)+CST_LUX4*x);//6.0135*(1/power(10,13))*power(x,4)-9.3924*(1/power(10,9))*power(x,3)+8.1488*(1/power(10,5))*power(x,2)+1.0023*x);
    UART_PC_PutString("\tluminosite mesuree : ");
    UART_PC_PutString(value);
    UART_PC_PutString("lx");
    UART_PC_PutCRLF('\0');
    return CST_LUX1*power(x,4)-CST_LUX2*power(x,3)+CST_LUX3*power(x,2)+CST_LUX4*x;
}

/**
*   Function : reset_param set the sensor's parameters back to default
*
*   The function set the sensor's parameters back to default meaning the gain value
*   is 1/8 and integration time is 100ms.
*
*/
void reset_param(void)
{
    uint8 VEML7700success;
    uint8 conf[3]; // config code for gain at 1/8 and integration time at 100ms
    sensor_als.gain = VEML7700_GAIN_1_8;
    sensor_als.integration_time = VEML7700_IT_100MS;
    sensor_als.config.conf = 0x1000;
    conf[0] = VEML7700_ALS_CONFIG;
    conf[1] = sensor_als.config.conf_lsb;
    conf[2] = sensor_als.config.conf_msb;
    VEML7700success = I2C_master_MasterWriteBuf(VEML7700_I2CADDR_DEFAULT,conf,3,I2C_master_MODE_COMPLETE_XFER);
    if(VEML7700success == I2C_master_MSTR_NO_ERROR)
    {
        CyDelay(100);
        VEML7700success = I2C_master_MasterStatus();
        if(VEML7700success == I2C_master_MSTAT_WR_CMPLT)
        {
            I2C_master_MasterClearWriteBuf();
            I2C_master_MasterClearStatus();
            UART_PC_PutString("Capteur lum reset\r\n");
            return;
        }
    }
    UART_PC_PutString("Capteur lum : reset error\r\n");
}

/**
*   Function : get_lum get the corrected value of Ambient Light
*
*   The function get the corrected value of Ambient Light by applaying the algorithm
*   from the document Designing the VEML7700 Onto an application".
*
*   @return The value of ambient light as a float.
*/
float get_lum(void)
{
    uint16 count;
    uint8 iter_it_inc,iter_it_dec, iter_g;
    char value[64];
    if(sensor_als.als) reset_param();
    UART_PC_PutString("Getting lum measure\r\n");
    while(sensor_als.integration_time != VEML7700_IT_800MS)//for(iter_it_inc = 0; iter_it_inc<3; iter_it_inc++)
    {
        while(sensor_als.gain != VEML7700_GAIN_2)//for(iter_g = 0; iter_g<4; iter_g++)
        {
            count = get_count();
            sprintf(value,"%d",count);
            UART_PC_PutString("[get_lum]: count = ");
            UART_PC_PutString(value);
            UART_PC_PutCRLF(0);
            if(count<100)
            {
                set_SD(1);
                increase_gain();
                set_SD(0);
            }
            else
            {
                while(sensor_als.integration_time != VEML7700_IT_25MS)
                {
                    if(count > 10000)
                    {
                        decrease_it();
                    }
                    else
                    {
                       UART_PC_PutString("out of lum 1\r\n");
                       return lux_calc(count); //lux calc
                    }
                }
                
                UART_PC_PutString("out of lum 2\r\n");
                return lux_calc(count); //lux calc
            }
        }
        increase_it();
        set_gain(VEML7700_GAIN_1_8);
        set_SD(0);
    }
    count = get_count();
    UART_PC_PutString("out of lum 3\r\n");
    sprintf(value,"%f",count*get_resolution());
    UART_PC_PutString("\tluminosite mesuree : ");
    UART_PC_PutString(value);
    UART_PC_PutString("lx");
    UART_PC_PutCRLF('\0');
    return count*get_resolution(); // Lux_veml
            
}

/* [] END OF FILE */
