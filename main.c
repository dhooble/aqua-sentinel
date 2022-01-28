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
#include "project.h"
#include "atlas_sensors.h"
#include "USBUART_utils.h"

//Constant
#define version_num "v1.4"
#define USBUART_DEVICE    (0u)

/* The buffer size is equal to the maximum packet size of the IN and OUT bulk
* endpoints.
*/
#define USBUART_BUFFER_SIZE (64u)

//Global variable
char* site_name = "francon";

//functions protoypes
int init_setup(void);

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    
    init_setup(); // init function (add error handling)
    
    for(;;)
    {
        /* Place your application code here. */
        //start watchdog 160s isr priority 0 Cant USE WATCHDOG USE TIMER + ISR
        //prise de mesure si 3 minutes se sont écoulées depuis la dernière mesure
        //envoi de la donnée avec gestion d'erreur et reemission du message (3 fois)
        //ajout de deep sleep mode ?
        //USBUART_sendString("test\n");
        
    }
}

/** Function : init_setup initalize UART, OneWire and IoT
* 
*   Initialize the UART, OneWire and GPRS (later LoRaWAN) connections. At the
*   same time it record the plugged-in sensors. All the operations are done
*   under Watchdog Timer.
*
*   @return 0 if every operations went well. 1..255 if there was an error
*/
int init_setup(void)
{
    //initialising UART for USB and GSM
    init_USBUART();
    UART_GSM_Start();
    
    USBUART_sendString("Aquacosme - version ");
    USBUART_sendString(version_num);
    USBUART_sendString("\n");
    CyDelay(100);
    USBUART_sendString("Projet ECONECT");
    CyDelay(100);
    USBUART_PutCRLF();
    USBUART_sendString("Programmation : J.L. Druilhe et A. Elger");
    CyDelay(100);
    USBUART_PutCRLF();
    USBUART_sendString("Site : ");
    USBUART_sendString(site_name);
    //start watchdog timer 30s isr priority 0 Cant USE WATCHDOG USE TIMER + ISR
    //init I2C for every Atlas sensor
    I2C_master_Start();
    //init OneWire comm (Dallas Temperature)
    //init VEML7700
    //disable watchdog timer
    return 0;
}

/* [] END OF FILE */
