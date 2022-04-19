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
//#include "USBUART_utils.h"
//#include "gprs.h"
#include "veml7700.h"
#include "ds18b20.h"

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

#define IDLE_STATE          0
#define INIT_STATE          1
#define MEASURE_STATE       2
#define COMMUNICATE_STATE   3

/* The buffer size is equal to the maximum packet size of the IN and OUT bulk
* endpoints.
*/
#define USBUART_BUFFER_SIZE (64u)

//Global variable
char* site_name = "francon\r\n"; // used in URL for data upload
uint8 connected_devices = 0u; // code representing connected sensors to poll for measure
float temp_value, lum_value, pH_value, cond_value, DO_value, ORP_value; // variable to store measures
uint8 state = INIT_STATE;

//Interrupt handlers prototypes
CY_ISR_PROTO(ISR_timer_1_handler);

//Functions protoypes
int init_setup(void);
void launch_measure();

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    isr_timer_measure_StartEx(ISR_timer_1_handler); /* intialize interrupt and set handler. */
    
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    
    init_setup(); // init function (add error handling)
    Timer_1_Start(); /* launch the timer for measure interval (180s). */
    for(;;)
    {
        /* Place your application code here. */
        //start watchdog 160s isr priority 0 Cant USE WATCHDOG USE TIMER + ISR
        //prise de mesure si 3 minutes se sont écoulées depuis la dernière mesure
        //envoi de la donnée avec gestion d'erreur et reemission du message (3 fois)
        //ajout de deep sleep mode ?
        switch(state)
        {
            case MEASURE_STATE:
                launch_measure();
                state = COMMUNICATE_STATE;
                break;
            case COMMUNICATE_STATE:
                state = IDLE_STATE;
                break;
            default:
                continue;
        }
        
    }
}

/**
*   Interruption Handler : ISR_timer_1_handler launch a measure on every connected sensors
* 
*   Clear the interrupt, change state to set a round of measure and relaunch the timer
*
*/
CY_ISR(ISR_timer_1_handler)
{
    //clear timer interrupt
    Timer_1_ReadStatusRegister();
    timer_measure_reset_reg_Write(0x01);
    CyDelay(10);
    state = MEASURE_STATE;
    Timer_1_Start();
}

/**
*   Function : init_setup initalize UART, OneWire, I2C and IoT
*
*   Initialize the UART, OneWire, I2C and GPRS (later LoRaWAN) connections. At the
*   same time it record the plugged-in sensors. All the operations are done
*   under Watchdog Timer.
*
*   @return 0 if every operations went well. 1..255 if there was an error
*/
int init_setup(void)
{
    // initialising UART for PC
    UART_PC_Start();
    
    UART_PC_PutString("Aquacosme - version ");
    UART_PC_PutString(version_num);
    UART_PC_PutString("\r\n");
    CyDelay(100);
    UART_PC_PutString("Projet ECONECT");
    CyDelay(100);
    UART_PC_PutCRLF('\0');
    UART_PC_PutString("Programmation : J.L. Druilhe et A. Elger");
    CyDelay(100);
    UART_PC_PutCRLF('\0');
    UART_PC_PutString("Site : ");
    UART_PC_PutString(site_name);
    
    //initialising UART for GSM
    UART_GSM_Start();
    //init_gprs();
    
    //start watchdog timer 30s isr priority 0 Cant USE WATCHDOG USE TIMER + ISR
    //init I2C for every Atlas sensor
    I2C_master_Start();
    atlasSearch(&connected_devices);
    //init OneWire comm (Dallas Temperature)
    if(!OW_Detect())
    {
        connected_devices |= DS18B20_CONNECTED;
        UART_PC_PutString("Capteur temperature connecte\r\n");
    }
    else
    {
        
        UART_PC_PutString("Capteur temperature non connecte\r\n");
    }
    //init VEML7700
    vemlSearch(&connected_devices);
    //disable watchdog timer
    state = IDLE_STATE;
    return 0;
}

/**
*   Function : launch_measure set a round of sampling
*
*   Initiate sampling for every plugged in sensors referenced in the status variable
*   connected_devices.
*
*   @return None
*/
void launch_measure()
{
    uint8 device_for_measure = 0u;
    //effectuer les mesures
    for (uint8 i = 0; i < NB_SONDES; i++)
    {
        device_for_measure = (connected_devices & (0x01<<i));
        switch (device_for_measure)
        {
            case ATLAS_PH_CONNECTED:
                pH_value = atlasMeasure(PARAM_PH);
                break;
            case ATLAS_EC_CONNECTED:
                cond_value = atlasMeasure(PARAM_COND);
                break;
            case ATLAS_DO_CONNECTED:
                DO_value = atlasMeasure(PARAM_DO);
                break;
            case ATLAS_ORP_CONNECTED:
                ORP_value = atlasMeasure(PARAM_ORP);
                break;
            case VEML7700_CONNECTED:
                lum_value = lumMeasure();
                break;
            case DS18B20_CONNECTED:
                temp_value = tempMeasure();
                break;            
            default:
                break;
        }
    }
}
/* [] END OF FILE */
