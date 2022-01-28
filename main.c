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
int init_USBUART(void);
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


int init_USBUART(void)
{
    uint16 count;
    uint8 buffer[USBUART_BUFFER_SIZE];
    /* Start USBUART operation with 5-V operation. */
    USBUART_Start(USBUART_DEVICE, USBUART_5V_OPERATION);

    
    /* Host can send double SET_INTERFACE request. */
    if (0u != USBUART_IsConfigurationChanged())
    {
        /* Initialize IN endpoints when device is configured. */
        if (0u != USBUART_GetConfiguration())
        {
            /* Enumeration is done, enable OUT endpoint to receive data 
             * from host. */
            USBUART_CDC_Init();
        }
    }

    /* Service USB CDC when device is configured. */
    if (0u != USBUART_GetConfiguration())
    {
        /* Check for input data from host. */
        if (0u != USBUART_DataIsReady())
        {
            /* Read received data and re-enable OUT endpoint. */
            count = USBUART_GetAll(buffer);

            if (0u != count)
            {
                /* Wait until component is ready to send data to host. */
                while (0u == USBUART_CDCIsReady())
                {
                }

                /* Send data back to host. */
                USBUART_PutData(buffer, count);

                /* If the last sent packet is exactly the maximum packet 
                *  size, it is followed by a zero-length packet to assure
                *  that the end of the segment is properly identified by 
                *  the terminal.
                */
                if (USBUART_BUFFER_SIZE == count)
                {
                    /* Wait until component is ready to send data to PC. */
                    while (0u == USBUART_CDCIsReady())
                    {
                    }

                    /* Send zero-length packet to PC. */
                    USBUART_PutData(NULL, 0u);
                }
            }
        }
    }
    return 0;
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
