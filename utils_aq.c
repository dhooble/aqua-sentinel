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

#include "utils_aq.h"

#define ATLAS_PH_CONNECTED 0x01
#define ATLAS_EC_CONNECTED 0x02
#define ATLAS_DO_CONNECTED 0x04
#define ATLAS_ORP_CONNECTED 0x08
#define VEML7700_CONNECTED 0x10
#define DS18B20_CONNECTED 0x20
#define NB_SONDES 6

#define UART_DELIMITER ';'
#define UART_DEFAULT_RECIEVED_BYTE 27

#define TIMER_MEASURE_FREQ_CONST 100000

uint8 check_token(int8 *token, int16 len_token, int8 * str, int16 len_str)
{
    int16 idx_token = 0, idx_str = 0;
    if((len_token <= 0) || (len_str <=0)) return -1;
    while((idx_token < len_token) && (idx_str < len_str))
    {
        if(token[idx_token]==str[idx_str])
        {
            idx_token++;
        }
        else
        {
            idx_token = 0;
        }
        if(idx_token > len_token)
        {
            return 1;
        }
        idx_str++;
    }
    return 0;
}


uint8 cpy_mem(int8 * dest, int8 * src, uint16 size)
{
    uint16 iter = 0;
    if(size == 0) return -1;
    while(iter < size)
    {
        dest[iter] = src[iter];
        iter++;
    }
    return 1;
}


void reset_timer_timeout_period(uint32 period)
{
    //write a new period from parameter into the register and reset the timer to load the new period.
    Timer_timeout_WritePeriod(period);
    Timer_timeout_ReadStatusRegister();
    timer_reset_reg_Write(0x01);
    if((Timer_timeout_ReadStatusRegister())&(Timer_timeout_STATUS_TC)) UART_PC_PutString("\r\n\t[ERROR]: le flag est toujours en hisse\r\n");
    CY_NOP;
    return;
}

void reset_timer_measure_period(uint32 period)
{
    //write a new period from parameter into the register and reset the timer to load the new period.
    Timer_measure_WritePeriod(period);
    Timer_measure_ReadStatusRegister();
    timer_measure_reset_reg_Write(0x01);
    if((Timer_timeout_ReadStatusRegister())&(Timer_timeout_STATUS_TC)) UART_PC_PutString("\r\n\t[ERROR]: le flag est toujours en hisse\r\n");
    CY_NOP;
    return;
}

uint8 is_str_alpha_num(int8 byte)
{
    if((byte >= '0') && (byte <= '9')) return 1; // if byte is the ASCII for a number (0-9)
    if((byte >= 'A') && (byte <= 'Z')) return 2; // if byte is the ASCII for a upper case (A-Z)
    if((byte >= 'a') && (byte <= 'z')) return 3; // if byte is the ASCII for a lower case (a-z)
    if(byte == '.') return 4; // if byte is the ASCII for a period (.)
    if(byte == ',') return 5; // if byte is the ASCII for a comma (,)
    if(byte == '_') return 5; // if byte is the ASCII for an underscore (_)
    if(byte == '?') return 6; // if byte is the ASCII for an question mark (?)
    return 0;
}

uint8 parse_command(int8 *in_buffer, int8 parsed_str[][32])
{
    char iter_buffer = 0, iter_parsed_str_command = 0, iter_parsed_str_char = 0;
    while((in_buffer[iter_buffer] != '\0') && (iter_parsed_str_command < 4) && (iter_parsed_str_char < 32))
    {
        printf("orig : %d, param %d, char : %d, = %c\n",iter_buffer,iter_parsed_str_command, iter_parsed_str_char, in_buffer[iter_buffer]);
        parsed_str[iter_parsed_str_command][iter_parsed_str_char] = in_buffer[iter_buffer];
        iter_buffer++;
        iter_parsed_str_char++;
        if(in_buffer[iter_buffer] == ',')
        {
            if(iter_parsed_str_char<31) parsed_str[iter_parsed_str_command][iter_parsed_str_char] = 0;
            iter_parsed_str_char = 0;
            iter_parsed_str_command++;
            iter_buffer++;
        }
        if((in_buffer[iter_buffer] == '\0') && (iter_parsed_str_char<31)) parsed_str[iter_parsed_str_command][iter_parsed_str_char] = 0;
        
    }
    if(iter_parsed_str_command < 1) parsed_str[1][0] = 0;
    return 1; 
}

void maintenance_utils_aq(void)
{
    //TEMPORARY
    uint8 i = 0, j = 0;
    //TEMPORARY
    uint16 iter_buffer = 0;
    uint8 sensor_buffer[BUFFER_SIZE] = "l,0", recieved_byte = UART_DEFAULT_RECIEVED_BYTE;
    int8 command[4][32] = {0};
    uint8 device_for_measure = 0, iter = 0;
    //cymemset(global_uart_buffer,0,BUFFER_SIZE);
    UART_PC_PutString("\r\n");
    LCD_ClearDisplay();
    LCD_Position(0u,0u);
    UART_PC_ClearRxBuffer();
    while(strcmp(global_uart_buffer,"quit"))
    {
        while(recieved_byte != UART_DELIMITER)
        {
            if(UART_PC_GetRxBufferSize())
            {
                recieved_byte = UART_PC_GetChar();
                if(recieved_byte == UART_DELIMITER)
                {
                    global_uart_buffer[iter_buffer] = 0;
                    iter_buffer = 0;
                    UART_PC_ClearRxBuffer();
                }
                else
                {
                    if(is_str_alpha_num(recieved_byte))
                    {
                        global_uart_buffer[iter_buffer] = recieved_byte;
                        iter_buffer++;
                        if(iter_buffer >= BUFFER_SIZE)
                        {
                            global_uart_buffer[BUFFER_SIZE-1] = 0;
                            iter_buffer = 0;
                            recieved_byte = UART_DELIMITER;
                            UART_PC_ClearRxBuffer();
                            break;
                        }
                    }
                }
            }
        }
        recieved_byte = UART_DEFAULT_RECIEVED_BYTE;
        UART_PC_PutString(global_uart_buffer);
        UART_PC_PutString("\r\n");
        parse_command(global_uart_buffer,command);
        
        UART_PC_PutString("Commande recue :\r\n");
        UART_PC_PutString(command[0]);
        UART_PC_PutString("\r\n");
        UART_PC_PutString(command[1]);
        UART_PC_PutString("\r\n");
        
        if(!strcmp(global_uart_buffer,"help")) // display every options
        {
            UART_PC_PutString("Help :\r\n");
            UART_PC_PutString("\tlist : list connected devices\r\n");
            UART_PC_PutString("\tdo   : read or modify parameters for dissolved oxygen sensor\r\n");
            UART_PC_PutString("\tec   : read or modify parameters for conductivity sensor\r\n");
            UART_PC_PutString("\torp  : read or modify parameters for oxydo-reduction potential sensor\r\n");
            UART_PC_PutString("\tph   : read or modify parameters for pH sensor\r\n");
            UART_PC_PutString("\tds   : read or modify parameters for temperature sensor\r\n");
            UART_PC_PutString("\tveml : read or modify parameters for ambient light sensor\r\n");
            UART_PC_PutString("\tsample_freq : list connected devices\r\n");
            UART_PC_PutString("\r\n");
        }
        else if(!strcmp(command[0],"list")) // display every connected sensors
        {
            UART_PC_PutString("liste des sondes connectees : \r\n");
                for (iter = 0; iter < NB_SONDES; iter++)
                {
                    device_for_measure = (global_connected_devices & (0x01<<iter));
                    switch (device_for_measure)
                    {
                        case ATLAS_PH_CONNECTED:
                            UART_PC_PutString("\tSonde Atlas pH (ph)\r\n");
                            break;
                        case ATLAS_EC_CONNECTED:
                            UART_PC_PutString("\tSonde Atlas Conductivite (ec)\r\n");
                            break;
                        case ATLAS_DO_CONNECTED:
                            UART_PC_PutString("\tSonde Atlas Oxygene Dissous (do)\r\n");
                            break;
                        case ATLAS_ORP_CONNECTED:
                            UART_PC_PutString("\tSonde Atlas Potentiel d Oxydo-Reduction (orp)\r\n");
                            break;
                        case VEML7700_CONNECTED:
                            UART_PC_PutString("\tSonde luminosite VEML7700 (veml)\r\n");
                            break;
                        case DS18B20_CONNECTED:
                            UART_PC_PutString("\tSonde de temperature DS18B20 (ds)\r\n");
                            break;            
                        default:
                            break;
                    }
                } 
            UART_PC_PutString("\r\n");
        }
        else if(!strcmp(command[0],"do") && (global_connected_devices & ATLAS_DO_CONNECTED)) // enter maintenance for Dissolved Oxygen sensor
        {
            UART_PC_PutString("\tinto do\r\n");
            if(!strcmp(command[1],"cal")) // calibration (routine or info)
            {
                I2C_master_MasterWriteBuf(sensor_do.address,"status",6,I2C_master_MODE_COMPLETE_XFER); // sends "status" command at address through the serial port
                CyDelay(300);
                I2C_master_MasterReadBuf(sensor_do.address,sensor_buffer,17,I2C_master_MODE_COMPLETE_XFER); // read value from address through serial port (reading more than enough bytes)
            }
            /*else if(!strcmp(command[1],"export"))
            {
                
                I2C_master_MasterWriteBuf(99,"status",6,I2C_master_MODE_COMPLETE_XFER); // sends command at address through the serial port
                CyDelay(300);
                I2C_master_MasterReadBuf(99,sensor_buffer,17,I2C_master_MODE_COMPLETE_XFER); // read value from address through serial port (reading more than enough bytes)
            }*/
            else if(!strcmp(command[1],"i2c")) // modify I2C address 
            {
                sprintf(sensor_buffer,"%s,%s",command[1],command[2]);
                I2C_master_MasterWriteBuf(sensor_do.address,sensor_buffer,8,I2C_master_MODE_COMPLETE_XFER); // sends command at address through the serial port
                CyDelay(300);
                I2C_master_MasterReadBuf(sensor_do.address,sensor_buffer,17,I2C_master_MODE_COMPLETE_XFER); // read value from address through serial port (reading more than enough bytes)
                UART_PC_PutString("\tsensor answer ?: ");
                UART_PC_PutString(sensor_buffer);
                UART_PC_PutString("\r\n");
                sscanf(sensor_buffer,"%d",&(sensor_do.address));
            }
            else if(!strcmp(command[1],"status")) // read status from device
            {
                I2C_master_MasterWriteBuf(sensor_do.address,"status",6,I2C_master_MODE_COMPLETE_XFER); // sends command at address through the serial port
                CyDelay(300);
                I2C_master_MasterReadBuf(sensor_do.address,sensor_buffer,17,I2C_master_MODE_COMPLETE_XFER); // read value from address through serial port (reading more than enough bytes)
                UART_PC_PutString("\tsensro status: ");
                UART_PC_PutString(sensor_buffer);
                UART_PC_PutString("\r\n");
                cymemcpy(sensor_do.status,sensor_buffer,STATUS_BUFFER_SIZE);
            }
            else if(!strcmp(command[1],"o")) // removing parameters
            {
                if(command[2][0] == '?') 
                {
                    sprintf(sensor_buffer, "%s,%c",command[1], command[2][0]);
                }
                else
                {
                    sprintf(sensor_buffer, "%s,%s,%s",command[1], command[2], command[3]);
                }
                I2C_master_MasterWriteBuf(sensor_do.address,sensor_buffer,6,I2C_master_MODE_COMPLETE_XFER); // sends "read value" command at address through the serial port
                CyDelay(300);
                I2C_master_MasterReadBuf(sensor_do.address,sensor_buffer,8,I2C_master_MODE_COMPLETE_XFER); // read value from address through serial port (reading more than enough bytes)
                UART_PC_PutString("\ttemperature compensation: ");
                UART_PC_PutString(sensor_buffer);
                UART_PC_PutString("\r\n");
            }
            else if(!strcmp(command[1],"p")) // pressure compensation
            {
                if(command[2][0] == '?') 
                {
                    sprintf(sensor_buffer, "%s,%c",command[1], command[2][0]);
                }
                else
                {
                    sprintf(sensor_buffer, "%s,%s",command[1], command[2]);
                }
                I2C_master_MasterWriteBuf(sensor_do.address,sensor_buffer,6,I2C_master_MODE_COMPLETE_XFER); // sends "read value" command at address through the serial port
                CyDelay(300);
                I2C_master_MasterReadBuf(sensor_do.address,sensor_buffer,8,I2C_master_MODE_COMPLETE_XFER); // read value from address through serial port (reading more than enough bytes)
                UART_PC_PutString("\ttemperature compensation: ");
                UART_PC_PutString(sensor_buffer);
                UART_PC_PutString("\r\n");
            }
            else if(!strcmp(command[1],"s")) // salinity compensation
            {
                if(command[2][0] == '?') 
                {
                    sprintf(sensor_buffer, "%s,%c",command[1], command[2][0]);
                }
                else
                {
                    sprintf(sensor_buffer, "%s,%s",command[1], command[2]);
                }
                I2C_master_MasterWriteBuf(sensor_do.address,sensor_buffer,6,I2C_master_MODE_COMPLETE_XFER); // sends "read value" command at address through the serial port
                CyDelay(300);
                I2C_master_MasterReadBuf(sensor_do.address,sensor_buffer,8,I2C_master_MODE_COMPLETE_XFER); // read value from address through serial port (reading more than enough bytes)
                UART_PC_PutString("\ttemperature compensation: ");
                UART_PC_PutString(sensor_buffer);
                UART_PC_PutString("\r\n");
            }
            else if(!strcmp(command[1],"t")) // manage temperature compensation
            {
                if(command[2][0] == '?') 
                {
                    sprintf(sensor_buffer, "%s,%c",command[1], command[2][0]);
                }
                else
                {
                    sprintf(sensor_buffer, "%s,%s",command[1], command[2]);
                }
                I2C_master_MasterWriteBuf(sensor_do.address,sensor_buffer,6,I2C_master_MODE_COMPLETE_XFER); // sends "read value" command at address through the serial port
                CyDelay(300);
                I2C_master_MasterReadBuf(sensor_do.address,sensor_buffer,8,I2C_master_MODE_COMPLETE_XFER); // read value from address through serial port (reading more than enough bytes)
                UART_PC_PutString("\ttemperature compensation: ");
                UART_PC_PutString(sensor_buffer);
                UART_PC_PutString("\r\n");
            }
            if(strstr(command[1],"l")) // manage device led
            {
                UART_PC_PutString("\tlight control\r\n");
                if((command[2][0] == '0') || (command[2][0] == '1')) // command led state
                {
                    sprintf(sensor_buffer,"%s,%s", command[1], command[2]);
                    UART_PC_PutString(sensor_buffer);
                    UART_PC_PutString("\r\n");
                    I2C_master_MasterWriteBuf(sensor_do.address,sensor_buffer,3,I2C_master_MODE_COMPLETE_XFER); // sends "read value" command at address through the serial port
                    CyDelay(300);
                    I2C_master_MasterClearStatus();
                    I2C_master_MasterReadBuf(sensor_do.address,sensor_buffer,6,I2C_master_MODE_COMPLETE_XFER);
                }
                else if(command[2][0] == '?') // read led state
                {
                    sprintf(sensor_buffer,"%s,%s", command[1], command[2]);
                    I2C_master_MasterWriteBuf(sensor_do.address,sensor_buffer,3,I2C_master_MODE_COMPLETE_XFER); // sends "read value" command at address through the serial port
                    CyDelay(300);
                    I2C_master_MasterReadBuf(sensor_do.address,sensor_buffer,2,I2C_master_MODE_COMPLETE_XFER); // read value from address through serial port (reading more than enough bytes)
                }
                UART_PC_PutString("\tlight control answer\r\n\t");
                UART_PC_PutString(sensor_buffer);
                UART_PC_PutString("\r\n");
            }
        }
        else if(!strcmp(command[0],"ec") && (global_connected_devices & ATLAS_EC_CONNECTED)) // enter maintenance for Conductivity sensor
        {
            UART_PC_PutString("\tinto ec\r\n");
            if(!strcmp(command[1],"cal")) // calibration (routine or info)
            {
                I2C_master_MasterWriteBuf(sensor_ec.address,"status",6,I2C_master_MODE_COMPLETE_XFER); // sends "read value" command at address through the serial port
                CyDelay(300);
                I2C_master_MasterReadBuf(sensor_ec.address,sensor_buffer,17,I2C_master_MODE_COMPLETE_XFER); // read value from address through serial port (reading more than enough bytes)
            }
            /*else if(!strcmp(command[1],"export"))
            {
                
                I2C_master_MasterWriteBuf(99,"status",6,I2C_master_MODE_COMPLETE_XFER); // sends "read value" command at address through the serial port
                CyDelay(300);
                I2C_master_MasterReadBuf(99,sensor_buffer,17,I2C_master_MODE_COMPLETE_XFER); // read value from address through serial port (reading more than enough bytes)
            }*/
            else if(!strcmp(command[1],"i2c")) // modify I2C address 
            {
                sprintf(sensor_buffer,"%s,%s",command[1],command[2]);
                I2C_master_MasterWriteBuf(sensor_ec.address,sensor_buffer,8,I2C_master_MODE_COMPLETE_XFER); // sends command at address through the serial port
                CyDelay(300);
                I2C_master_MasterReadBuf(sensor_ec.address,sensor_buffer,17,I2C_master_MODE_COMPLETE_XFER); // read value from address through serial port (reading more than enough bytes)
                UART_PC_PutString("\tsensor answer ?: ");
                UART_PC_PutString(sensor_buffer);
                UART_PC_PutString("\r\n");
                sscanf(sensor_buffer,"%d",&(sensor_do.address));
            }
            else if(!strcmp(command[1],"status")) // read status from device
            {
                I2C_master_MasterWriteBuf(sensor_ec.address,"status",6,I2C_master_MODE_COMPLETE_XFER); // sends command at address through the serial port
                CyDelay(300);
                I2C_master_MasterReadBuf(sensor_ec.address,sensor_buffer,17,I2C_master_MODE_COMPLETE_XFER); // read value from address through serial port (reading more than enough bytes)
                UART_PC_PutString("\tsensro status: ");
                UART_PC_PutString(sensor_buffer);
                UART_PC_PutString("\r\n");
                cymemcpy(sensor_do.status,sensor_buffer,STATUS_BUFFER_SIZE);
            }
            else if(!strcmp(command[1],"o")) // removing parameters
            {
                if(command[2][0] == '?') 
                {
                    sprintf(sensor_buffer, "%s,%c",command[1], command[2][0]);
                }
                else
                {
                    sprintf(sensor_buffer, "%s,%s,%s",command[1], command[2], command[3]);
                }
                I2C_master_MasterWriteBuf(sensor_ec.address,sensor_buffer,6,I2C_master_MODE_COMPLETE_XFER); // sends command at address through the serial port
                CyDelay(300);
                I2C_master_MasterReadBuf(sensor_ec.address,sensor_buffer,8,I2C_master_MODE_COMPLETE_XFER); // read value from address through serial port (reading more than enough bytes)
                UART_PC_PutString("\ttemperature compensation: ");
                UART_PC_PutString(sensor_buffer);
                UART_PC_PutString("\r\n");
            }
            else if(!strcmp(command[1],"k")) // probe type
            {
                if(command[2][0] == '?') 
                {
                    sprintf(sensor_buffer, "%s,%c",command[1], command[2][0]);
                }
                else
                {
                    sprintf(sensor_buffer, "%s,%s",command[1], command[2]);
                }
                I2C_master_MasterWriteBuf(sensor_ec.address,sensor_buffer,6,I2C_master_MODE_COMPLETE_XFER); // sends command at address through the serial port
                CyDelay(300);
                I2C_master_MasterReadBuf(sensor_ec.address,sensor_buffer,8,I2C_master_MODE_COMPLETE_XFER); // read value from address through serial port (reading more than enough bytes)
                UART_PC_PutString("\ttemperature compensation: ");
                UART_PC_PutString(sensor_buffer);
                UART_PC_PutString("\r\n");
            }
            else if(!strcmp(command[1],"s")) // salinity compensation
            {
                if(command[2][0] == '?') 
                {
                    sprintf(sensor_buffer, "%s,%c",command[1], command[2][0]);
                }
                else
                {
                    sprintf(sensor_buffer, "%s,%s",command[1], command[2]);
                }
                I2C_master_MasterWriteBuf(sensor_ec.address,sensor_buffer,6,I2C_master_MODE_COMPLETE_XFER); // sends command at address through the serial port
                CyDelay(300);
                I2C_master_MasterReadBuf(sensor_ec.address,sensor_buffer,8,I2C_master_MODE_COMPLETE_XFER); // read value from address through serial port (reading more than enough bytes)
                UART_PC_PutString("\ttemperature compensation: ");
                UART_PC_PutString(sensor_buffer);
                UART_PC_PutString("\r\n");
            }else if(!strcmp(command[1],"tds")) // tds
            {
                if(command[2][0] == '?') 
                {
                    sprintf(sensor_buffer, "%s,%c",command[1], command[2][0]);
                }
                else
                {
                    sprintf(sensor_buffer, "%s,%s",command[1], command[2]);
                }
                I2C_master_MasterWriteBuf(sensor_ec.address,sensor_buffer,6,I2C_master_MODE_COMPLETE_XFER); // sends command at address through the serial port
                CyDelay(300);
                I2C_master_MasterReadBuf(sensor_ec.address,sensor_buffer,8,I2C_master_MODE_COMPLETE_XFER); // read value from address through serial port (reading more than enough bytes)
                UART_PC_PutString("\ttemperature compensation: ");
                UART_PC_PutString(sensor_buffer);
                UART_PC_PutString("\r\n");
            }
            else if(!strcmp(command[1],"t")) // manage temperature compensation
            {
                if(command[2][0] == '?') 
                {
                    sprintf(sensor_buffer, "%s,%c",command[1], command[2][0]);
                }
                else
                {
                    sprintf(sensor_buffer, "%s,%s",command[1], command[2]);
                }
                I2C_master_MasterWriteBuf(sensor_ec.address,sensor_buffer,6,I2C_master_MODE_COMPLETE_XFER); // sends command at address through the serial port
                CyDelay(300);
                I2C_master_MasterReadBuf(sensor_ec.address,sensor_buffer,8,I2C_master_MODE_COMPLETE_XFER); // read value from address through serial port (reading more than enough bytes)
                UART_PC_PutString("\ttemperature compensation: ");
                UART_PC_PutString(sensor_buffer);
                UART_PC_PutString("\r\n");
            }
            if(strstr(command[1],"l")) // manage device led
            {
                UART_PC_PutString("\tlight control\r\n");
                if((command[2][0] == '0') || (command[2][0] == '1')) // command led state
                {
                    sprintf(sensor_buffer,"%s,%s", command[1], command[2]);
                    UART_PC_PutString(sensor_buffer);
                    UART_PC_PutString("\r\n");
                    I2C_master_MasterWriteBuf(sensor_ec.address,sensor_buffer,3,I2C_master_MODE_COMPLETE_XFER); // sends command at address through the serial port
                    CyDelay(300);
                    I2C_master_MasterClearStatus();
                    I2C_master_MasterReadBuf(sensor_ec.address,sensor_buffer,6,I2C_master_MODE_COMPLETE_XFER);
                }
                else if(command[2][0] == '?') // read led state
                {
                    sprintf(sensor_buffer,"%s,%s", command[1], command[2]);
                    I2C_master_MasterWriteBuf(sensor_ec.address,sensor_buffer,3,I2C_master_MODE_COMPLETE_XFER); // sends command at address through the serial port
                    CyDelay(300);
                    I2C_master_MasterReadBuf(sensor_ec.address,sensor_buffer,2,I2C_master_MODE_COMPLETE_XFER); // read value from address through serial port (reading more than enough bytes)
                }
                UART_PC_PutString("\tlight control answer\r\n\t");
                UART_PC_PutString(sensor_buffer);
                UART_PC_PutString("\r\n");
            }
        }
        else if(!strcmp(command[0],"orp") && (global_connected_devices & ATLAS_ORP_CONNECTED)) // enter maintenance for OxydoReduction Potential sensor
        {
            UART_PC_PutString("\tinto ph\r\n");
            if(!strcmp(command[1],"cal")) // calibration (routine or info)
            {
                I2C_master_MasterWriteBuf(sensor_orp.address,"status",6,I2C_master_MODE_COMPLETE_XFER); // sends command at address through the serial port
                CyDelay(300);
                I2C_master_MasterReadBuf(sensor_orp.address,sensor_buffer,17,I2C_master_MODE_COMPLETE_XFER); // read value from address through serial port (reading more than enough bytes)
            }
            /*else if(!strcmp(command[1],"export"))
            {
                
                I2C_master_MasterWriteBuf(99,"status",6,I2C_master_MODE_COMPLETE_XFER); // sends command at address through the serial port
                CyDelay(300);
                I2C_master_MasterReadBuf(99,sensor_buffer,17,I2C_master_MODE_COMPLETE_XFER); // read value from address through serial port (reading more than enough bytes)
            }*/
            else if(!strcmp(command[1],"i2c")) // modify I2C address 
            {
                sprintf(sensor_buffer,"%s,%s",command[1],command[2]);
                I2C_master_MasterWriteBuf(sensor_orp.address,sensor_buffer,8,I2C_master_MODE_COMPLETE_XFER); // sends command at address through the serial port
                CyDelay(300);
                I2C_master_MasterReadBuf(sensor_orp.address,sensor_buffer,17,I2C_master_MODE_COMPLETE_XFER); // read value from address through serial port (reading more than enough bytes)
                UART_PC_PutString("\tsensor answer ?: ");
                UART_PC_PutString(sensor_buffer);
                UART_PC_PutString("\r\n");
                sscanf(sensor_buffer,"%d",&(sensor_do.address));
            }
            else if(!strcmp(command[1],"status")) // read status from device
            {
                I2C_master_MasterWriteBuf(sensor_orp.address,"status",6,I2C_master_MODE_COMPLETE_XFER); // sends command at address through the serial port
                CyDelay(300);
                I2C_master_MasterReadBuf(sensor_orp.address,sensor_buffer,17,I2C_master_MODE_COMPLETE_XFER); // read value from address through serial port (reading more than enough bytes)
                UART_PC_PutString("\tsensor status: ");
                UART_PC_PutString(sensor_buffer);
                UART_PC_PutString("\r\n");
                cymemcpy(sensor_ph.status,sensor_buffer,STATUS_BUFFER_SIZE);
            }
            if(strstr(command[1],"l")) // manage device led
            {
                UART_PC_PutString("\tlight control\r\n");
                if((command[2][0] == '0') || (command[2][0] == '1')) // command led state
                {
                    sprintf(sensor_buffer,"%s,%s", command[1], command[2]);
                    UART_PC_PutString(sensor_buffer);
                    UART_PC_PutString("\r\n");
                    I2C_master_MasterWriteBuf(sensor_orp.address,sensor_buffer,3,I2C_master_MODE_COMPLETE_XFER); // sends command at address through the serial port
                    CyDelay(300);
                    I2C_master_MasterClearStatus();
                    I2C_master_MasterReadBuf(sensor_orp.address,sensor_buffer,6,I2C_master_MODE_COMPLETE_XFER);
                }
                else if(command[2][0] == '?') // return led state
                {
                    sprintf(sensor_buffer,"%s,%s", command[1], command[2]);
                    I2C_master_MasterWriteBuf(sensor_orp.address,sensor_buffer,3,I2C_master_MODE_COMPLETE_XFER); // sends command at address through the serial port
                    CyDelay(300);
                    I2C_master_MasterReadBuf(sensor_orp.address,sensor_buffer,2,I2C_master_MODE_COMPLETE_XFER); // read value from address through serial port (reading more than enough bytes)
                }
                UART_PC_PutString("\tlight control answer\r\n\t");
                UART_PC_PutString(sensor_buffer);
                UART_PC_PutString("\r\n");
            }
        } 
        else if(!strcmp(command[0],"ph") && (global_connected_devices & ATLAS_PH_CONNECTED)) // enter maintenance for pH sensor
        {
            UART_PC_PutString("\tinto ph\r\n");
            if(!strcmp(command[1],"cal")) // calibration (routine or info)
            {
                I2C_master_MasterWriteBuf(sensor_ph.address,"status",6,I2C_master_MODE_COMPLETE_XFER); // sends command at address through the serial port
                CyDelay(300);
                I2C_master_MasterReadBuf(sensor_ph.address,sensor_buffer,17,I2C_master_MODE_COMPLETE_XFER); // read value from address through serial port (reading more than enough bytes)
            }
            /*else if(!strcmp(command[1],"export"))
            {
                
                I2C_master_MasterWriteBuf(99,"status",6,I2C_master_MODE_COMPLETE_XFER); // sends command at address through the serial port
                CyDelay(300);
                I2C_master_MasterReadBuf(99,sensor_buffer,17,I2C_master_MODE_COMPLETE_XFER); // read value from address through serial port (reading more than enough bytes)
            }*/
            else if(!strcmp(command[1],"i2c")) // modify I2C address 
            {
                sprintf(sensor_buffer,"%s,%s",command[1],command[2]);
                I2C_master_MasterWriteBuf(sensor_ph.address,sensor_buffer,8,I2C_master_MODE_COMPLETE_XFER); // sends command at address through the serial port
                CyDelay(300);
                I2C_master_MasterReadBuf(sensor_ph.address,sensor_buffer,17,I2C_master_MODE_COMPLETE_XFER); // read value from address through serial port (reading more than enough bytes)
                UART_PC_PutString("\tsensor answer ?: ");
                UART_PC_PutString(sensor_buffer);
                UART_PC_PutString("\r\n");
                sscanf(sensor_buffer,"%d",&(sensor_do.address));
            }
            else if(!strcmp(command[1],"status")) // read status from device
            {
                I2C_master_MasterWriteBuf(sensor_ph.address,"status",6,I2C_master_MODE_COMPLETE_XFER); // sends command at address through the serial port
                CyDelay(300);
                I2C_master_MasterReadBuf(sensor_ph.address,sensor_buffer,17,I2C_master_MODE_COMPLETE_XFER); // read value from address through serial port (reading more than enough bytes)
                UART_PC_PutString("\tsensor status: ");
                UART_PC_PutString(sensor_buffer);
                UART_PC_PutString("\r\n");
                cymemcpy(sensor_ph.status,sensor_buffer,STATUS_BUFFER_SIZE);
            }
            else if(!strcmp(command[1],"t")) // manage temperature compensation
            {
                if(command[2][0] == '?') 
                {
                    sprintf(sensor_buffer, "%s,%c",command[1], command[2][0]);
                }
                else
                {
                    sprintf(sensor_buffer, "%s,%s",command[1], command[2]);
                }
                I2C_master_MasterWriteBuf(sensor_ph.address,sensor_buffer,6,I2C_master_MODE_COMPLETE_XFER); // sends command at address through the serial port
                CyDelay(300);
                I2C_master_MasterReadBuf(sensor_ph.address,sensor_buffer,8,I2C_master_MODE_COMPLETE_XFER); // read value from address through serial port (reading more than enough bytes)
                UART_PC_PutString("\ttemperature compensation: ");
                UART_PC_PutString(sensor_buffer);
                UART_PC_PutString("\r\n");
            }
            if(strstr(command[1],"l")) // manage device led
            {
                UART_PC_PutString("\tlight control\r\n");
                if((command[2][0] == '0') || (command[2][0] == '1')) // command led state
                {
                    sprintf(sensor_buffer,"%s,%s", command[1], command[2]);
                    UART_PC_PutString(sensor_buffer);
                    UART_PC_PutString("\r\n");
                    I2C_master_MasterWriteBuf(sensor_ph.address,sensor_buffer,3,I2C_master_MODE_COMPLETE_XFER); // sends command at address through the serial port
                    CyDelay(300);
                    I2C_master_MasterClearStatus();
                    I2C_master_MasterReadBuf(sensor_ph.address,sensor_buffer,6,I2C_master_MODE_COMPLETE_XFER);
                }
                else if(command[2][0] == '?') // return led state
                {
                    sprintf(sensor_buffer,"%s,%s", command[1], command[2]);
                    I2C_master_MasterWriteBuf(sensor_ph.address,sensor_buffer,3,I2C_master_MODE_COMPLETE_XFER); // sends command at address through the serial port
                    CyDelay(300);
                    I2C_master_MasterReadBuf(sensor_ph.address,sensor_buffer,2,I2C_master_MODE_COMPLETE_XFER); // read value from address through serial port (reading more than enough bytes)
                }
                UART_PC_PutString("\tlight control answer\r\n\t");
                UART_PC_PutString(sensor_buffer);
                UART_PC_PutString("\r\n");
            }
        }
        else if(!strcmp(command[0],"ds") && (global_connected_devices & DS18B20_CONNECTED)) // enter maintenance for Temperature sensor
        {
            UART_PC_PutString("Capteur de temperature DS18B20\r\n");
            if(!strcmp(command[1],"res")) // resolution of measure
            {
                UART_PC_PutString("Resolution\r\n");
                continue;
            }
            UART_PC_PutString("\r\n");
        }
        else if(!strcmp(command[0],"veml") && (global_connected_devices & VEML7700_CONNECTED)) // enter maintenance for Luminosity sensor
        {
            UART_PC_PutString("Capteur de luminosite VEML7700\r\n");
            if(!strcmp(command[0],"rconf"))
            {
                I2C_master_MasterSendStart(VEML7700_I2CADDR_DEFAULT,0);
                I2C_master_MasterWriteByte(VEML7700_ALS_CONFIG);
                
                I2C_master_MasterSendRestart(VEML7700_I2CADDR_DEFAULT,1);
                sensor_als.config.conf_div[0] = I2C_master_MasterReadByte(I2C_master_ACK_DATA);
                sensor_als.config.conf_div[1] = I2C_master_MasterReadByte(I2C_master_NAK_DATA);
                I2C_master_MasterSendStop();
            }
            else if(!strcmp(command[0],"wconf"))
            {
                I2C_master_MasterSendStart(VEML7700_I2CADDR_DEFAULT,0);
                I2C_master_MasterWriteByte(VEML7700_ALS_CONFIG);
                I2C_master_MasterWriteByte(sensor_als.config.conf_div[0]);
                I2C_master_MasterWriteByte(sensor_als.config.conf_div[1]);
                I2C_master_MasterSendStop();
            }
            UART_PC_PutString("\r\n");
        }
        else if(!strcmp(command[0],"sample_freq"))
        {
            UART_PC_PutString("Frequence de prise de mesure actuelle : ");
            sprintf(sensor_buffer, "%ld", Timer_measure_ReadPeriod()/TIMER_MEASURE_FREQ_CONST);
            UART_PC_PutString(sensor_buffer);
            UART_PC_PutString("s\r\n");
            //reset_timer_measure_period(TIMER_MEASURE_FREQ_CONST); // multiply by atoi of new period
            
        }
        else
        {
            UART_PC_PutString("La commande n'existe pas.\r\n");
        }
    }
    UART_PC_PutString("quitting Maintenance mode\r\n");
}


/* [] END OF FILE */
