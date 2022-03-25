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
//#include "atlas_sensors.h"
#include "USBUART_utils.h"
//#include "gprs.h"
//#include "veml7700.h"

//Constant
#define version_num "v1.4"
#define USBUART_DEVICE    (0u)
#define ATLAS_PH_CONNECTED 0x01
#define ATLAS_EC_CONNECTED 0x02
#define ATLAS_DO_CONNECTED 0x04
#define ATLAS_ORP_CONNECTED 0x08
#define VEML7700_CONNECTED 0x10
#define DS18B20_CONNECTED 0x20
#define NB_SONDES (6u)

/* The buffer size is equal to the maximum packet size of the IN and OUT bulk
* endpoints.
*/
#define USBUART_BUFFER_SIZE (64u)

//Global variable
char* site_name = "francon";
uint8 connected_devices = 0u;
float temp_value, lum_value, pH_value, cond_value, DO_value, ORP_value;

//Interrupt handlers prototypes
CY_ISR_PROTO(ISR_timer_1_handler);

//Functions protoypes
int init_setup(void);

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    isr_timer_measure_StartEx(ISR_timer_1_handler);
    
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    
    init_setup(); // init function (add error handling)
    Timer_1_Start();
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

CY_ISR(ISR_timer_1_handler)
{
    uint8 device_for_measure = 0u;
    //effectuer les mesures
    for (uint8 i = 0; i < NB_SONDES; i++)
    {
        device_for_measure = (connected_devices & (0x01<<i));
        switch (device_for_measure)
        {
            case ATLAS_PH_CONNECTED:
                //pH_value = atlasMeasure(PARAM_PH);
                break;
            case ATLAS_EC_CONNECTED:
                //cond_value = atlasMeasure(PARAM_COND);
                break;
            case ATLAS_DO_CONNECTED:
                //DO_value = atlasMeasure(PARAM_DO);
                break;
            case ATLAS_ORP_CONNECTED:
               // ORP_value = atlasMeasure(PARAM_ORP);
                break;
            case VEML7700_CONNECTED:
                //lum_value = lumMeasure();
                break;
            case DS18B20_CONNECTED:
                temp_value = 0;
                break;            
            default:
                break;
        }
    }
    //clear timer interrupt
    Timer_1_ReadStatusRegister();
    Timer_1_Start();
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
    CyDelay(2000);
    init_USBUART();
    CyDelay(2000);
    UART_GSM_Start();
    CyDelay(2000);
    //init_gprs();
    
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
    //atlasSearch(&connected_devices);
    //init OneWire comm (Dallas Temperature)
    //init VEML7700
    //vemlSearch(&connected_devices);
    //disable watchdog timer
    return 0;
}

/* [] END OF FILE */
