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

uint8 n2483_is_plugged(void) // verifier que le RN2483 est connecte au PSoC
{
    return 0;
}

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
    return 0;
}

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

uint32 rn_get_period(uint16 time_ms)
{
    return time_ms*FREQ_COUNTER/1000;
}

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

uint8 rn2483_mac_tx(uint8 mode, uint8 port, char *payload) // envoyer une chaine de donnee sur un port spécifique (tx)
{
    char cmd[128] = "mac tx";
    
    sprintf(cmd,"%s %s %u %s\r\n",cmd,(mode?"cnf":"uncnf"),port,payload);
    UART_PC_PutString("[rn2483_mac_tx]: full cmd ");
    UART_PC_PutString(cmd);
    UART_PC_PutCRLF(0);
    uart_lora_PutString(cmd);
    rn_get_serial_message(10000,cmd);
}

uint8 rn2483_mac_pause(uint16 timeout) // arreter/mettre en pause la stack LoRaWAN (to change radio configuration) (pause)
{
    char cmd[16] = "mac pause";
    if(timeout != 0)
    {
        sprintf(cmd,"%s %u\r\n",cmd,timeout);
    }
    uart_lora_PutString(cmd);
}

uint8 rn2483_mac_resume(void) // reprendre le fonctionnement de la stack LoRaWAN (resume)
{
    uart_lora_PutString("mac resume\r\n");
}

uint8 rn2483_mac_save(void) // sauvegarder en EEPROM les paramètres de classe A
{
}

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

uint8 rn2483_mac_get_deveui(void) // recuperer le DevEUI (8 bytes)
{
    char cmd[17] = "mac get deveui\r\n";
    
    uart_lora_PutString(cmd);
    rn_get_serial_message(1000,cmd);
    cymemcpy(rn2483_config.deveui,cmd,RN2483_SIZE_DEVEUI);
    return 0;
}

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

uint8 rn2483_mac_get_appeui(char *appeui) // recuperer l'AppEUI (8 bytes)
{
    char cmd[17] = "mac get appeui\r\n";
    
    uart_lora_PutString(cmd);
    rn_get_serial_message(1000,cmd);
    cymemcpy(rn2483_config.appeui,cmd,RN2483_SIZE_APPEUI);
    return 0;
}

uint8 rn2483_mac_set_appkey(char *appkey) // modifier l'AppKey (16 bytes)
{
    char cmd[32] = "mac set appkey";
    if(!strcmp(appkey,rn2483_config.appkey) && rn2483_config.mac_status) return 1;
    cymemcpy(rn2483_config.appkey,appkey,RN2483_SIZE_APPKEY);
    sprintf(cmd,"%s %s\r\n",cmd,appkey);
    uart_lora_PutString(cmd);
    rn_get_serial_message(1000,cmd);
    if(!strcmp(cmd,"ok")) return 2;
}

uint8 rn2483_mac_get_appkey(char *appkey) // recuperer l'AppKey (16 bytes)
{
    char cmd[17] = "mac get appkey\r\n";
    
    uart_lora_PutString(cmd);
    rn_get_serial_message(1000,cmd);
    cymemcpy(rn2483_config.appkey,cmd,RN2483_SIZE_APPKEY);
    return 0;
}

uint8 rn2483_mac_set_devaddr(char *devaddr) // modifier le DevAddr (4 bytes)
{
    char cmd[24] = "mac set devaddr";
    if(!strcmp(devaddr,rn2483_config.devaddr) && rn2483_config.mac_status) return 1;
    cymemcpy(rn2483_config.devaddr,devaddr,RN2483_SIZE_DEVADDR);
    sprintf(cmd,"%s %s\r\n",cmd,devaddr);
    uart_lora_PutString(cmd);
    rn_get_serial_message(1000,cmd);
    if(!strcmp(cmd,"ok")) return 2;
}

uint8 rn2483_mac_get_devaddr(char *devaddr) // recuperer le DevAddr (4 bytes)
{
    char cmd[17] = "mac get devaddr\r\n";
    
    uart_lora_PutString(cmd);
    rn_get_serial_message(1000,cmd);
    cymemcpy(rn2483_config.devaddr,cmd,RN2483_SIZE_DEVADDR);
    return 0;
}
uint8 rn2483_mac_set_appskey(char *appskey) // modifier l'AppSKey (16 bytes)
{
    char cmd[64] = "mac set appskey";
    if(!strcmp(appskey,rn2483_config.appskey) && rn2483_config.mac_status) return 1;
    cymemcpy(rn2483_config.appskey,appskey,RN2483_SIZE_APPSKEY);
    sprintf(cmd,"%s %s\r\n",cmd,appskey);
    uart_lora_PutString(cmd);
    rn_get_serial_message(1000,cmd);
    if(!strcmp(cmd,"ok")) return 2;
}

uint8 rn2483_mac_get_appskey(char *appskey) // recuperer l'AppSKey (16 bytes)
{
    char cmd[17] = "mac get appskey\r\n";
    
    uart_lora_PutString(cmd);
    rn_get_serial_message(1000,cmd);
    cymemcpy(rn2483_config.appskey,cmd,RN2483_SIZE_APPSKEY);
    return 0;
}

uint8 rn2483_mac_set_nwkskey(char *nwkskey) // modifier le NwkSKey (16 bytes)
{
    char cmd[64] = "mac set nwkskey";
    if(!strcmp(nwkskey,rn2483_config.nwkskey) && rn2483_config.mac_status) return 1;
    cymemcpy(rn2483_config.nwkskey,nwkskey,RN2483_SIZE_NWKSKEY);
    sprintf(cmd,"%s %s\r\n",cmd,nwkskey);
    uart_lora_PutString(cmd);
    rn_get_serial_message(1000,cmd);
    if(!strcmp(cmd,"ok")) return 2;
}

uint8 rn2483_mac_get_nwkskey(char *nwkskey) // recuperer le NwkSKey (16 bytes)
{
    char cmd[17] = "mac get nwkskey\r\n";
    
    uart_lora_PutString(cmd);
    rn_get_serial_message(1000,cmd);
    cymemcpy(rn2483_config.nwkskey,cmd,RN2483_SIZE_NWKSKEY);
    return 0;
}


//-------------------------------------------- LoRa ---------------------------------------------

uint8 rn2483_radio_tx(char *payload) // envoyer des paquets en LoRa
{
}

uint8 rn2483_radio_rx(char *payload) // recevoir des paquets en LoRa
{ 
}

uint8 rn2483_radio_rxstop(void) // arreter la reception continue (rxstop)
{
    uart_lora_PutString("radio rxstop\r\n");
}

uint8 rn2483_radio_set_freq(uint32 freq) // modifier la fréquence de la porteuse
{
    char cmd[16] = "radio set freq";
    sprintf(cmd,"%s %u\r\n",cmd,freq);
    uart_lora_PutString(cmd);
}

uint8 rn2483_radio_set_sf(uint8 sf) // modifier le spreading factor (SF)
{
    char cmd[16] = "radio set deveui";
    sprintf(cmd,"%s sf%u\r\n",cmd,sf);
    uart_lora_PutString(cmd);
}

uint8 rn2483_radio_set_mod(uint8 mod) // modifier la modulation (LoRa, FSK)
{
}


/* [] END OF FILE */
