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

#include "rn2483.h"

/**
*   Function : n2483_is_plugged pings the UART Connection to find a RN2483
*
*   The function pings the the UART Connection to find a RN2483. (incomplete)
*
*   @return 0 (uint8).
*/
uint8 n2483_is_plugged(void) // verifier que le RN2483 est connecte au PSoC
{
    return 0;
}

/**
*   Function : rn2483_send_data sends the measured values through LoRaWAN
*
*   The function sends the measured values from connected sensors through LoRaWAN.
*   CAN/MUST BE IMPROVED
*
*   @param  temp is a float value collected from the temperature sensor.
*   @param  lum is a float value collected from the light sensor.
*   @param  ph is a float value collected from the ph sensor.
*   @param  cond is a float value collected from the conductivity sensor.
*   @param  oxy is a float value collected from the dissolved oxygen sensor.
*   @param  red is a float value collected from the redox sensor.
*   @return 0 if the process succeed and any positive value otherwise (uint8).
*/
uint8 rn2483_send_data(float temp, float lum, float ph, float cond, float oxy, float red)
{
    char message[128],message_hex[256];
    uint8 index_message = 0,index_message_hex = 0;
    sprintf(message,"{\"lc\":\"%s\",\"id\":%d",global_site_name,global_id);
    if(!(rn2483_config.mac_status & RN2483_MAC_STATUS_CONNECTED))
    {
        UART_PC_PutString("[rn2483_send_data]: since LoRaWAN not connected retry\r\n");
        rn2483_mac_join(RN2483_CONNECT_MODE_OTAA);
    }
    /*if(global_connected_devices & DS18B20_CONNECTED)*/ sprintf(message,"%s,\"tc\":%.2f",message,12.0);//0.0);
    /*if(global_connected_devices & VEML7700_CONNECTED)*/ sprintf(message,"%s,\"hc\":%.2f",message,12.0);//0.0);
    /*if(global_connected_devices & DS18B20_CONNECTED)*/ sprintf(message,"%s,\"tw\":%.2f",message,12.0);//temp);
    /*if(global_connected_devices & VEML7700_CONNECTED)*/ sprintf(message,"%s,\"l\":%.2f",message,lum);
    /*if(global_connected_devices & ATLAS_PH_CONNECTED)*/ sprintf(message,"%s,\"p\":%.2f",message,12.0);//ph);
    /*if(global_connected_devices & ATLAS_DO_CONNECTED)*/ sprintf(message,"%s,\"o\":%.2f",message,12.0);//oxy);
    /*if(global_connected_devices & ATLAS_ORP_CONNECTED)*/ sprintf(message,"%s,\"r\":%.2f",message,12.0);//red);
    /*if(global_connected_devices & ATLAS_EC_CONNECTED)*/ sprintf(message,"%s,\"v\":%.2f",message,12.0);//0.0);
    /*if(global_connected_devices & ATLAS_EC_CONNECTED)*/ sprintf(message,"%s,\"c\":%.2f",message,12.0);//cond);
    strcat(message,"}");
    UART_PC_PutString("[rn2483_send_data]: ");
    UART_PC_PutString(message);
    UART_PC_PutCRLF(0);
    while(message[index_message]!=0)
    {
        sprintf(&(message_hex[index_message_hex]),"%02X",message[index_message]);
        index_message++;
        index_message_hex+=2;
    }
    
    UART_PC_PutString("[rn2483_send_data]: message en hex :");
    UART_PC_PutString(message_hex);
    UART_PC_PutCRLF(0);
    
    rn2483_mac_tx(RN2483_MAC_TX_MODE_CNF,220,message_hex);
    return 1;
}

/**
*   Function : rn2483_init initialize the RN2483 with default parameters
*
*   The function initialize the RN2483 with default parameters such as the deveui, appeui
*   and the appkey. it also realize a join operation (by default in OTAA to confirm that
*   the configuration is correct. 
*
*   @return 0 if the process succeed and any positive value otherwise (uint8).
*/
uint8 rn2483_init()
{
    uint8 i = 0, join_result;
    rn2483_mac_set_deveui(rn2483_config.deveui);
    rn2483_mac_set_appeui(rn2483_config.appeui);
    UART_PC_PutString("[rn2483_init]: RN2483_STR(RN2483_DEFAULT_APPKEY) ");
    UART_PC_PutString(RN2483_STR(RN2483_DEFAULT_APPKEY));
    UART_PC_PutCRLF(0);
    UART_PC_PutString("[rn2483_init]: RN2483_DEFAULT_APPKEY_STR(RN2483_DEFAULT_APPKEY) ");
    UART_PC_PutString(RN2483_DEFAULT_APPKEY_STR(RN2483_DEFAULT_APPKEY));
    UART_PC_PutCRLF(0);
    UART_PC_PutString("[rn2483_init]: rn2483_config.appkey ");
    UART_PC_PutString(rn2483_config.appkey);
    UART_PC_PutCRLF(0);
    
    UART_PC_PutString("[rn2483_init]: rn2483_config.appkey with for ");
    for(i=0;i<RN2483_SIZE_APPKEY;i++) UART_PC_PutChar(rn2483_config.appkey[i]);
    UART_PC_PutCRLF(0);
    
    rn2483_mac_set_appkey(rn2483_config.appkey);
    
    UART_PC_PutString("[rn2483_init]: mac join otaa\r\n");
    join_result = rn2483_mac_join(RN2483_CONNECT_MODE_OTAA);
    
    rn2483_config.mac_status = 1;
    
    
    return 0;
}

/**
*   Function : rn_get_period returns the period value for the counter
*
*   The function returns the period value for the counter of the PSoC calculated from a value
*   in millisecond and taking the counter clock speed in account.
*
*   @param  time_ms is a char (uint8) used to calculate the period value.
*   @return the period value as a uint32.
*/
uint32 rn_get_period(uint16 time_ms)
{
    return time_ms*FREQ_COUNTER/1000;
}

/**
*   Function : rn_get_serial_message monitors the serial port for a given time to retrieve a message
*
*   The function monitors the serial port for a time given as a parameter in order to
*   retrieve a message. The monitoring time given as a parameter in millisecond is 
*   recomputed to fit the scale of clock used for the timer that counts the ellapsed time.
*   The message recieved is stored in an string from the char pointer given as an
*   argument.
*
*   @param  waiting_delay is a int32 value representing the monitoring time in milliseconds (ms).
*   @param  message is a char pointer addressed to where the recieved string will be stored.
*/
void rn_get_serial_message(int32 waiting_delay, char *message)
{  
    char read;
    uint8 idx_message = 0;
    Timer_timeout_WritePeriod(rn_get_period(waiting_delay));
    global_uart_loop = 1;
    timer_reset_reg_Write(0x01);
    while(global_uart_loop) //maybe replace status register by variable and interrupt
    {
        if(((read = uart_lora_GetChar()) !=0) && idx_message<127)
        {
            //UART_PC_PutString("[rn_get_serial_message]: uart_lora buffer has char\r\n");
            //UART_PC_PutChar(read);
            message[idx_message] = read;
            idx_message++;
        }
        
    }
    //uart_lora_ClearRxBuffer();
    message[idx_message] = 0;
    UART_PC_PutString("retour rn2483 : ");
    UART_PC_PutString(message);
    UART_PC_PutString("\r\n   lus\r\n");
}


//------------------------------------------- LoRaWAN -------------------------------------------
/**
*   Function : rn2483_mac_join realize a join operation
*
*   The function sends the command for a join operation using the mode given as a parameter.
*
*   @param  mode is a char (uint8) used to state the mode (OTAA or ABP). It can take 2 values:
*   RN2483_CONNECT_MODE_OTAA (0) or RN2483_CONNECT_MODE_ABP ([1;254]).
*   @return the mode chosen(uint8).
*/
uint8 rn2483_mac_join(uint8 mode) // connecter le module LoRaWAN RN2483 avec choix du mode (otaa, abp) (join)
{
    char message[128] = "";
    if(mode > 1) return 255;
    if(!mode)
    {
        uart_lora_PutString("mac join otaa\r\n");
    }
    else if (mode)
    {
        uart_lora_PutString("mac join abp\r\n");
    }
    rn_get_serial_message(9000,message);
    
    if(strstr(message,"accepted")) rn2483_config.mac_status = rn2483_config.mac_status | 0x02;
    else rn2483_config.mac_status = rn2483_config.mac_status & 0xFE;
    return mode;
}

/**
*   Function : rn2483_mac_tx sends data with LoRaWAN
*
*   The function sends data with LoRaWAN through a specified port. The message can request to
*   send back a confirmation or not according to the parameter 'mode'.
*
*   @param  mode is a char (uint8) used to request a confirmation that the message has been recieved (0:no confirmation; [1,254]:confirmation).
*   @param  port is a char (uint8) used to set the port.
*   @param  payload is a char (uint8) pointer to the payload (message).
*   @return 0 if the process succeed and any positive value otherwise (uint8).
*/
uint8 rn2483_mac_tx(uint8 mode, uint8 port, char *payload) // envoyer une chaine de donnee sur un port spécifique (tx)
{
    char cmd[128] = "mac tx";
    
    sprintf(cmd,"%s %s %u %s\r\n",cmd,(mode?"cnf":"uncnf"),port,payload);
    UART_PC_PutString("[rn2483_mac_tx]: full cmd ");
    UART_PC_PutString(cmd);
    UART_PC_PutCRLF(0);
    uart_lora_PutString(cmd);
    rn_get_serial_message(10000,cmd);
    return 0;
}

/**
*   Function : rn2483_mac_pause put the LoRaWAN stack on pause
*
*   The function put the LoRaWAN stack on pause for 'timeout'
*
*   @param  timeout is a char (uint8) used as the length of pause time.
*   @return 0 if the process succeed and any positive value otherwise (uint8).
*/
uint8 rn2483_mac_pause(uint16 timeout) // arreter/mettre en pause la stack LoRaWAN (to change radio configuration) (pause)
{
    char cmd[16] = "mac pause";
    if(timeout != 0)
    {
        sprintf(cmd,"%s %u\r\n",cmd,timeout);
    }
    uart_lora_PutString(cmd);
    return 0;
}

/**
*   Function : rn2483_mac_resume restarts the LoRaWAN stack
*
*   The function restarts the LoRaWAN stack
*
*   @return 0 if the process succeed and any positive value otherwise (uint8).
*/
uint8 rn2483_mac_resume(void) // reprendre le fonctionnement de la stack LoRaWAN (resume)
{
    uart_lora_PutString("mac resume\r\n");
    return 0;
}

/**
*   Function : rn2483_mac_save saves the configuration of the device in EEPROM
*
*   The function saves the configuration of the device in EEPROM (deveui,appeui,appkey,...)
*
*   @return 0 if the process succeed and any positive value otherwise (uint8).
*/
uint8 rn2483_mac_save(void) // sauvegarder en EEPROM les paramètres de classe A
{
    return 0;
}

/**
*   Function : rn2483_mac_set_deveui set the deveui on the device
*
*   The function set the deveui stored in the rn2483 configuration sctructure
*   to the device.
*
*   @param  deveui is a char (uint8) pointer to the deveui.
*   @return 0 if the process succeed and any positive value otherwise (uint8).
*/
uint8 rn2483_mac_set_deveui(char *deveui) // modifier le DevEUI (8 bytes)
{
    char cmd[32] = "mac set deveui";
    if(!strcmp(deveui,rn2483_config.deveui) && rn2483_config.mac_status) return 1;
    cymemcpy(rn2483_config.deveui,deveui,RN2483_SIZE_DEVEUI);
    sprintf(cmd,"%s %s\r\n",cmd,deveui);
    uart_lora_PutString(cmd);
    rn_get_serial_message(1000,cmd);
    if(!strcmp(cmd,"ok")) return 2;
    return 0;
}

/**
*   Function : rn2483_mac_get_deveui get the deveui from the device
*
*   The function get the deveui from the device and store it in the rn2483 configuration 
*   structure.
*
*   @return 0 if the process succeed and any positive value otherwise (uint8).
*/
uint8 rn2483_mac_get_deveui(void) // recuperer le DevEUI (8 bytes)
{
    char cmd[17] = "mac get deveui\r\n";
    
    uart_lora_PutString(cmd);
    rn_get_serial_message(1000,cmd);
    cymemcpy(rn2483_config.deveui,cmd,RN2483_SIZE_DEVEUI);
    return 0;
}

/**
*   Function : rn2483_mac_set_appeui set the appeui on the device
*
*   The function set the appeui stored in the rn2483 configuration sctructure
*   to the device.
*
*   @param  deveui is a char (uint8) pointer to the appeui.
*   @return 0 if the process succeed and any positive value otherwise (uint8).
*/
uint8 rn2483_mac_set_appeui(char *appeui) // recuperer l'AppEUI (8 bytes)
{
    char cmd[32] = "mac set appeui";
    if(!strcmp(appeui,rn2483_config.appeui) && rn2483_config.mac_status) return 1;
    cymemcpy(rn2483_config.appeui,appeui,RN2483_SIZE_APPEUI);
    sprintf(cmd,"%s %s\r\n",cmd,appeui);
    uart_lora_PutString(cmd);
    rn_get_serial_message(1000,cmd);
    if(!strcmp(cmd,"ok")) return 2;
    return 0;
}

/**
*   Function : rn2483_mac_get_appeui get the appeui from the device
*
*   The function get the appeui from the device and store it in the rn2483 configuration 
*   structure.
*
*   @return 0 if the process succeed and any positive value otherwise (uint8).
*/
uint8 rn2483_mac_get_appeui(void) // recuperer l'AppEUI (8 bytes)
{
    char cmd[17] = "mac get appeui\r\n";
    
    uart_lora_PutString(cmd);
    rn_get_serial_message(1000,cmd);
    cymemcpy(rn2483_config.appeui,cmd,RN2483_SIZE_APPEUI);
    return 0;
}

/**
*   Function : rn2483_mac_set_appkey set the appkey on the device
*
*   The function set the appkey stored in the rn2483 configuration sctructure
*   to the device.
*
*   @param  deveui is a char (uint8) pointer to the appkey.
*   @return 0 if the process succeed and any positive value otherwise (uint8).
*/
uint8 rn2483_mac_set_appkey(char *appkey) // modifier l'AppKey (16 bytes)
{
    char cmd[32] = "mac set appkey";
    if(!strcmp(appkey,rn2483_config.appkey) && rn2483_config.mac_status) return 1;
    cymemcpy(rn2483_config.appkey,appkey,RN2483_SIZE_APPKEY);
    sprintf(cmd,"%s %s\r\n",cmd,appkey);
    uart_lora_PutString(cmd);
    rn_get_serial_message(1000,cmd);
    if(!strcmp(cmd,"ok")) return 2;
    return 0;
}

/**
*   Function : rn2483_mac_get_appkey get the appkey from the device
*
*   The function get the appkey from the device and store it in the rn2483 configuration 
*   structure.
*
*   @return 0 if the process succeed and any positive value otherwise (uint8).
*/
uint8 rn2483_mac_get_appkey(char *appkey) // recuperer l'AppKey (16 bytes)
{
    char cmd[17] = "mac get appkey\r\n";
    
    uart_lora_PutString(cmd);
    rn_get_serial_message(1000,cmd);
    cymemcpy(rn2483_config.appkey,cmd,RN2483_SIZE_APPKEY);
    return 0;
}

/**
*   Function : rn2483_mac_set_devaddr set the devaddr on the device
*
*   The function set the devaddr stored in the rn2483 configuration sctructure
*   to the device.
*
*   @param  deveui is a char (uint8) pointer to the devaddr.
*   @return 0 if the process succeed and any positive value otherwise (uint8).
*/
uint8 rn2483_mac_set_devaddr(char *devaddr) // modifier le DevAddr (4 bytes)
{
    char cmd[24] = "mac set devaddr";
    if(!strcmp(devaddr,rn2483_config.devaddr) && rn2483_config.mac_status) return 1;
    cymemcpy(rn2483_config.devaddr,devaddr,RN2483_SIZE_DEVADDR);
    sprintf(cmd,"%s %s\r\n",cmd,devaddr);
    uart_lora_PutString(cmd);
    rn_get_serial_message(1000,cmd);
    if(!strcmp(cmd,"ok")) return 2;
    return 0;
}

/**
*   Function : rn2483_mac_get_devaddr get the devaddr from the device
*
*   The function get the devaddr from the device and store it in the rn2483 configuration 
*   structure.
*
*   @return 0 if the process succeed and any positive value otherwise (uint8).
*/
uint8 rn2483_mac_get_devaddr(char *devaddr) // recuperer le DevAddr (4 bytes)
{
    char cmd[17] = "mac get devaddr\r\n";
    
    uart_lora_PutString(cmd);
    rn_get_serial_message(1000,cmd);
    cymemcpy(rn2483_config.devaddr,cmd,RN2483_SIZE_DEVADDR);
    return 0;
}

/**
*   Function : rn2483_mac_set_appskey set the appskey on the device
*
*   The function set the appskey stored in the rn2483 configuration sctructure
*   to the device.
*
*   @param  deveui is a char (uint8) pointer to the appskey.
*   @return 0 if the process succeed and any positive value otherwise (uint8).
*/
uint8 rn2483_mac_set_appskey(char *appskey) // modifier l'AppSKey (16 bytes)
{
    char cmd[64] = "mac set appskey";
    if(!strcmp(appskey,rn2483_config.appskey) && rn2483_config.mac_status) return 1;
    cymemcpy(rn2483_config.appskey,appskey,RN2483_SIZE_APPSKEY);
    sprintf(cmd,"%s %s\r\n",cmd,appskey);
    uart_lora_PutString(cmd);
    rn_get_serial_message(1000,cmd);
    if(!strcmp(cmd,"ok")) return 2;
    return 0;
}


/**
*   Function : rn2483_mac_get_appskey get the appskey from the device
*
*   The function get the appskey from the device and store it in the rn2483 configuration 
*   structure.
*
*   @return 0 if the process succeed and any positive value otherwise (uint8).
*/
uint8 rn2483_mac_get_appskey(char *appskey) // recuperer l'AppSKey (16 bytes)
{
    char cmd[17] = "mac get appskey\r\n";
    
    uart_lora_PutString(cmd);
    rn_get_serial_message(1000,cmd);
    cymemcpy(rn2483_config.appskey,cmd,RN2483_SIZE_APPSKEY);
    return 0;
}

/**
*   Function : rn2483_mac_set_nwkskey set the nwkskey on the device
*
*   The function set the nwkskey stored in the rn2483 configuration sctructure
*   to the device.
*
*   @param  deveui is a char (uint8) pointer to the nwkskey.
*   @return 0 if the process succeed and any positive value otherwise (uint8).
*/
uint8 rn2483_mac_set_nwkskey(char *nwkskey) // modifier le NwkSKey (16 bytes)
{
    char cmd[64] = "mac set nwkskey";
    if(!strcmp(nwkskey,rn2483_config.nwkskey) && rn2483_config.mac_status) return 1;
    cymemcpy(rn2483_config.nwkskey,nwkskey,RN2483_SIZE_NWKSKEY);
    sprintf(cmd,"%s %s\r\n",cmd,nwkskey);
    uart_lora_PutString(cmd);
    rn_get_serial_message(1000,cmd);
    if(!strcmp(cmd,"ok")) return 2;
    return 0;
}

/**
*   Function : rn2483_mac_get_nwkskey get the nwkskey from the device
*
*   The function get the nwkskey from the device and store it in the rn2483 configuration 
*   structure.
*
*   @return 0 if the process succeed and any positive value otherwise (uint8).
*/
uint8 rn2483_mac_get_nwkskey(char *nwkskey) // recuperer le NwkSKey (16 bytes)
{
    char cmd[17] = "mac get nwkskey\r\n";
    
    uart_lora_PutString(cmd);
    rn_get_serial_message(1000,cmd);
    cymemcpy(rn2483_config.nwkskey,cmd,RN2483_SIZE_NWKSKEY);
    return 0;
}


//-------------------------------------------- LoRa ---------------------------------------------

/**
*   Function : rn2483_radio_tx sends data with LoRa
*
*   The function sends data with LoRa
*
*   @param  payload is a char (uint8) pointer to the payload to send.
*   @return 0 if the process succeed and any positive value otherwise (uint8).
*/
uint8 rn2483_radio_tx(char *payload) // envoyer des paquets en LoRa
{
    return 0;
}

/**
*   Function : rn2483_radio_rx sends data with LoRa
*
*   The function  data with LoRa
*
*   @param  payload is a char (uint8) pointer to space to store the recieved payload.
*   @return 0 if the process succeed and any positive value otherwise (uint8).
*/
uint8 rn2483_radio_rx(char *payload) // recevoir des paquets en LoRa
{
    return 0;
}

/**
*   Function : rn2483_radio_rxstop stops the continous message reception
*
*   The function stops the continous message reception
*
*   @return 0 if the process succeed and any positive value otherwise (uint8).
*/
uint8 rn2483_radio_rxstop(void) // arreter la reception continue (rxstop)
{
    uart_lora_PutString("radio rxstop\r\n");
    return 0;
}

/**
*   Function : rn2483_radio_set_freq sets the frequency of the carrier signal
*
*   The function set the frequency of the carrier signal from 433050000 to 434790000
*   or from 863000000 to 870000000, in Hz.
*
*   @param  freq is a int (uint32) used to set the frequency of the carrier signal.
*   @return 0 if the process succeed and any positive value otherwise (uint8).
*/
uint8 rn2483_radio_set_freq(uint32 freq) // modifier la fréquence de la porteuse
{
    char cmd[16] = "radio set freq";
    sprintf(cmd,"%s %lu\r\n",cmd,freq);
    uart_lora_PutString(cmd);
    return 0;
}

/**
*   Function : rn2483_radio_set_sf sets the Spreading Factor (SF)
*
*   The function set the Spreading Factor (SF)
*
*   @param  sf is a char (uint8) used set the spreading factor.
*   @return 0 if the process succeed and any positive value otherwise (uint8).
*/
uint8 rn2483_radio_set_sf(uint8 sf) // modifier le spreading factor (SF)
{
    char cmd[16] = "radio set deveui";
    sprintf(cmd,"%s sf%u\r\n",cmd,sf);
    uart_lora_PutString(cmd);
    return 0;
}

/**
*   Function : rn2483_radio_set_mod sets the modulation of the signal
*
*   The function sets the modulation of the signal with 2 possible modes: LoRa or FSK
*
*   @param  detected_devices is a char (uint8) pointer used as the list of connected devieces.
*   @return 0 if the process succeed and any positive value otherwise (uint8).
*/
uint8 rn2483_radio_set_mod(uint8 mod) // modifier la modulation (LoRa, FSK)
{
    return 0;
}


/* [] END OF FILE */
