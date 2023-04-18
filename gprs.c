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

#include "gprs.h"

char* site_name_gprs = "francon";

/**
*   Function : init_gprs find every Atlas sensors connected to the controller
*
*   The function ping the default address of the agreed Atlas sensors through the
*   the I2C bus. The command sent is asking for LED status and upon positive answer
*   the bit representing the sensor is added to the variable named detected_device.
*
*   @param  detected_sensor is an unsigned char (uint8) pointer acting as the list of connected sensors.
*   @return The code listing the detected (Atlas) sensors.
*/
void init_gprs(void)
{
    char chaine_network_strength[6];
    UART_PC_PutString("Initialisation du gprs...");
    UART_GSM_PutString("AT\r\n");
    UART_GSM_PutString("ATZ\r\n");
    CyDelay(500);
    UART_GSM_PutString("AT+CFUN=0\r\n");
    CyDelay(1000);
    UART_GSM_PutString("AT+CFUN=1\r\n");
    CyDelay(1000);
    UART_GSM_PutString("AT+CPIN?\r\n");
    CyDelay(500);
    UART_GSM_PutString("AT+CPIN=7719\r\n"); // Carte SIM n°5 - forfait illimité
    CyDelay(500);
    UART_GSM_PutString("AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r\n");
    CyDelay(500);
    UART_GSM_PutString("AT+SAPBR=3,1,\"APN\",\"box\"\r\n");
    CyDelay(500);
    UART_GSM_PutString("AT+SAPBR=3,1,\"USER\",\"\"\r\n");
    CyDelay(500);
    UART_GSM_PutString("AT+SAPBR=3,1,\"PWD\",\"\"\r\n");
    CyDelay(500);
    //updateSerial();
    UART_PC_PutString("Etat du reseau : ");
    get_network_strength(chaine_network_strength);
    UART_PC_PutString(chaine_network_strength);
    //draw_divider(80, true, '-');
    UART_PC_PutString("\r\n");
}

/**
*   Function : get_network_strength get the quality of the signal
*
*   The function sends the AT+CSQ command to the SIM800L to recieve the quality of
*   the signal. The result is stored in an string from the char pointer given as an
*   argument and returned as a float at the end of the function.
*
*   @param  chaine_result is a char pointer addressed to where the resulting string will be stored.
*   @return The quality of the signal as a float.
*/
float get_network_strength(char *chaine_result)
{
    char answer_message[ANSWER_SIZE] = "", chaine_network_strength[6];
    char *chaine;
    float result;
    uint8 iter = 0;
    UART_GSM_PutString("AT+CSQ\r\n");
    get_serial_message(200, answer_message);
    chaine = strstr(answer_message, "+SAPBR:");
    while(chaine[iter+6] != ',')
    {
        chaine_network_strength[iter] = chaine[iter+6];
        iter++;
    }
    if (chaine_result != NULL) strcpy(chaine_result,chaine_network_strength);
    sscanf(chaine_network_strength,"%f", &result);
    return result;
}

/**
*   Function : get_IP return the IP address of the modem
*
*   The function sends the AT+SAPBR=2,1 command to the SIM800L to recieve the IP
*   address of the modem. The result is currently returned as a char pointer.
*   MIGHT NEED REWORK!!
*
*   @return The quality of the signal as a float.
*/
char* get_IP(void)
{
    char answer_message[ANSWER_SIZE] = "";//, ip_address[IP_ADDRESS_LEN];
    //char *debut_chaine;//, *fin_chaine;
    UART_GSM_PutString("AT+SAPBR=2,1\r\n");
    get_serial_message(50,answer_message);
    if (strstr(answer_message,"ERROR")!= NULL || strstr(answer_message,"\"0.0.0.0\"") != NULL)
    {
        return "Pas d'adresse IP";
    }
    //int index = answer_message.indexOf("+SAPBR:");
    //answer_message = answer_message.substring(answer_message.indexOf("\"")+1,answer_message.indexOf("OK")-5);
    //debut_chaine = strstr(answer_message, "\"");
    //fin_chaine = 
    UART_PC_PutString(answer_message);
    UART_PC_PutCRLF('\0');
    return NULL;//answer_message;
}

/**
*   Function : is_gprs_connected informs if the modem is connected to the internet (has an IP address)
*
*   The function sends the AT+SAPBR=2,1 command to the SIM800L to recieve the IP
*   address of the modem and thus confirm that it is indeed connected to the internet.
*   The result is currently returned as a char (int8).
*
*   @return 0 if the modem is disconnected and any other value otherwise as a char (int8).
*/
int8 is_gprs_connected(void)
{
    char answer_message[ANSWER_SIZE] = "";
    UART_GSM_PutString("AT+SAPBR=2,1\r\n");
    get_serial_message(50, answer_message);
    if (strstr(answer_message,"ERROR")!= NULL || strstr(answer_message,"\"0.0.0.0\"") != NULL)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

/**
*   Function : send_request get the quality of the signal
*
*   The function sends the AT+CSQ command to the SIM800L to recieve the quality of
*   the signal. The result is stored in an string from the char pointer given as an
*   argument and returned as a float at the end of the function.
*
*   @param  temp is a float value collected from the temperature sensor.
*   @param  lum is a float value collected from the light sensor.
*   @param  ph is a float value collected from the ph sensor.
*   @param  cond is a float value collected from the conductivity sensor.
*   @param  oxy is a float value collected from the dissolved oxygen sensor.
*   @param  red is a float value collected from the redox sensor.
*   @return 1 if the operation was successful and 0 otherwise.
*/
int8 send_request(float temp, float lum, float ph, float cond, float oxy, float red)
{
    char request_string[REQUEST_SIZE], server_response[ANSWER_SIZE],chaine_temporaire[6];
    char net_strength[6];
    UART_GSM_PutString("AT+SAPBR=1,1\r\n");
    CyDelay(500);
    updateSerial();
    get_network_strength(net_strength);
    UART_PC_PutString("Etat du reseau : ");
    UART_PC_PutString(net_strength);
    UART_PC_PutString("Adresse IP : ");
    UART_PC_PutString(get_IP());
    UART_PC_PutCRLF('\0');
    if(is_gprs_connected())
    {
        UART_GSM_PutString("AT+HTTPINIT\r\n");
        CyDelay(500);
        UART_GSM_PutString("AT+HTTPPARA=\"CID\",1\r\n");
        CyDelay(500);
        strcpy(request_string,"AT+HTTPPARA=\"URL\",\"http://145.239.199.14/cgi-bin/econect/receive_data.py?location=");
        strcat(request_string,site_name_gprs);
        if(temp != 85 && temp != -99 && temp != 0)
        {
            float_to_string(temp,chaine_temporaire);
            strcat(request_string,"&temp_eau=");
            strcat(request_string,chaine_temporaire);
        }
        if(lum < 100000 && lum != -99)
        {
            float_to_string(lum,chaine_temporaire);
            strcat(request_string,"&int_lumineuse=");
            strcat(request_string,chaine_temporaire);
        }
        if(ph != -99 && ph != 0)
        {
            float_to_string(ph,chaine_temporaire);
            strcat(request_string,"&ph=");
            strcat(request_string,chaine_temporaire);
        }
        if(oxy != -99 && oxy != 0)
        {
            float_to_string(oxy,chaine_temporaire);
            strcat(request_string,"&oxygene=" );
            strcat(request_string,chaine_temporaire);
        }
        if(red != -99 && red != 0)
        {
            float_to_string(red,chaine_temporaire);
            strcat(request_string,"&redox=");
            strcat(request_string,chaine_temporaire);
        }
        if(cond != -99)
        {
            float_to_string(cond,chaine_temporaire);
            strcat(request_string,"&conductivite=");
            strcat(request_string,chaine_temporaire);
        }
        strcat(strcat(request_string,"&signal_gsm="),net_strength);
        strcat(request_string,"\"\r");
        UART_GSM_PutString(request_string);
        UART_PC_PutString(request_string);
        UART_PC_PutString("Reponse du serveur: ");
        updateSerial();
        CyDelay(500);
        UART_GSM_PutString("AT+HTTPACTION=0\r\n");
        get_serial_message(4000,server_response);
        strcpy(server_response,strstr(server_response,"+HTTPACTION:"));
        strcpy(chaine_temporaire,"");
        strncat(chaine_temporaire,&(server_response[15]),4);
        strcpy(server_response,chaine_temporaire);
        UART_GSM_PutString("AT+HTTPREAD=0,100\r\n");
        CyDelay(500);
        UART_GSM_PutString("AT+HTTPTERM\r\n");
        CyDelay(500);
        UART_GSM_PutString("AT+SAPBR=0,1\r\n");     //Fermeture de la liaison GPRS
        if(!strcmp(server_response,"200"))
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}

/**
*   Function : sim_reset resset the SIM800L modem
*
*   The function execute a hard reset of the SIM800L.
*/
void sim_reset(void)
{
    Timer_timeout_WritePeriod(get_period(10000));
    Timer_timeout_Start();
    UART_PC_PutString("Hard reset du SIM800 !\r\n");
    UART_GSM_PutString("AT+CFUN=0\r\n"); 
    CyDelay(1000);
    UART_GSM_PutString("AT+CFUN=1\r\n"); 
    CyDelay(1000);
    do
    {
        updateSerial();
        UART_PC_PutString("Waiting for network...");
        CyDelay(500);
    } while(get_network_strength(NULL)==0.0 && (Timer_timeout_ReadStatusRegister() & 0x01)); //maybe replace status register by variable and interrupt
}

/**
*   Function : updateSerial read everythings that may be left in the serial buffer
*
*   The function reads everything that is left in the serial buffer and sends it 
*   to the maintenance serial channel.
*/
void updateSerial(void)
{
    //uint8 message[64];
    while(UART_GSM_GetRxBufferSize() != 0u)
    {
        UART_PC_PutChar(UART_GSM_GetChar());//Forward what Serial received to Software Serial Port
    }
    if(UART_PC_GetRxBufferSize())
    {
        UART_PC_ClearRxBuffer();
    }
}

/**
*   Function : get_serial_message monitors the serial port for a given time to retrieve a message
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
void get_serial_message(int32 waiting_delay, char *message)
{  
    char read;
    Timer_timeout_WritePeriod(get_period(waiting_delay));
    Timer_timeout_Start();
    while(Timer_timeout_ReadStatusRegister() & 0x01) //maybe replace status register by variable and interrupt
    {
        while(UART_GSM_GetRxBufferSize() != 0)
        {
            read = UART_GSM_ReadRxData();
            strcat(message, &read);
        }
    }
}

/**
*   Function : get_period return the period for the timer based on the milliseconds input
*
*   The function returns the value of a period for a timer connected to a clock
*   of 24MHz from a value in milliseconds (ms).
*
*   @param  time_ms is a unsigned int (uint16) value of the timer period in milliseconds (ms).
*   @return The period for the timer (a uint16) corresponding a time in milliseconds (ms).
*/
uint16 get_period(uint16 time_ms)
{
    return time_ms*FREQ_COUNTER/1000;
}

/**
*   Function : float_to_string  get the quality of the signal
*
*   The function sends the value of a float to the char pointer given as parameter.
*
*   @param  chaine_result is a char pointer addressed to where the resulting string will be stored.it
*/
void float_to_string(const float value, char *result)
{
    
    sprintf(result,"%f",value);
}

/* [] END OF FILE */
