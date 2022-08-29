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
#include "ds18b20.h"

/**
*   Function : OW_Detect verify the presence of an probe on the OneWire bus.
*
*   The function ping the OneWire bus and return a positive numer if a probe is connected and zero in the other case.
*   
*   @return 1 if a probe is connected, 0 otherwise.
*/
uint8 OW_Detect(void)
{
    

    CyPins_ClearPin(PinOW_0);
    CyDelayUs(482);

    CyPins_SetPin(PinOW_0);
    CyDelayUs(70);

    return !CyPins_ReadPin(PinOW_0);
}

/**
*   Function : OW_WriteByte writes a byte/command on the OneWire bus.
*
*   The function write a byte bit by bit on the OneWire bus while respecting the timing set for OneWire communication. 
*   
*   @param  u8 is an unsigned char (uint8) the byte that need to be sent.
*   @return None.
*/
void OW_WriteByte(uint8 u8)
{
    uint8 i=0;
    for(i=0; i<8; i++)
    {
        if(u8 & 0x01)
        {
            CyPins_ClearPin(PinOW_0);
            CyDelayUs(6);
            CyPins_SetPin(PinOW_0);
            CyDelayUs(64);
        }
        else
        {
            CyPins_ClearPin(PinOW_0);
            CyDelayUs(60);
            CyPins_SetPin(PinOW_0);
            CyDelayUs(10); 
        }

        u8>>=1;
    }
}

/**
*   Function : OW_ReadByte reads a byte/result on the OneWire bus.
*
*   The function reads a byte bit by bit on the OneWire bus while respecting the timing set for OneWire communication.
*   
*   @return The read byte as a uint8.
*/
uint8 OW_ReadByte(void)
{
    unsigned char b=0;
    int i=0;

    while(i<8)
    {
        b >>= 1;
        CyPins_ClearPin(PinOW_0);
        CyDelayUs(6);
        CyPins_SetPin(PinOW_0);
        CyDelayUs(9);

        b |= (CyPins_ReadPin(PinOW_0)>0) ? 0x80 : 0x00;
        CyDelayUs(55);

        i++;
    }

    return b;
}

/**
*   Function : compute_measure formats the result bytes recieved.
*
*   The function formats the result bytes recieved from the temperature probe by recalculating the result from binaries 
*   values
*
*   @param  measure is an unsigned char (uint8) pointer form the result bytes read from the probe.
*   @return The calculated value as a float.
*/
float compute_measure(uint8 *measure)
{
    float result = 0.0;
    char coeff = -4;
    unsigned char index = 0, mask_shift = 0;
    char text_val [13],hex[4];

    for (int i = 0; i < 11; i++)
    {
        if(i == 8)
        {
            index = 1;
            mask_shift = 0;
        }
        if(measure[index] & (0x01<<mask_shift))
        {
            if(coeff<0)
            {
                result += 1.0/(1<<(-coeff));
            }
            else
            {
                result += (1<<coeff);
            }
        }
        coeff++;
        mask_shift++;
    }
    if((measure[1] & 0x80)) result = - result;  
    sprintf(text_val,"%f",result);
    UART_PC_PutString("\ttemperature mesuree : ");
    UART_PC_PutString(text_val);
    UART_PC_PutString("C"); 
    UART_PC_PutCRLF('\0');
    return result;
}

/**
*   Function : tempMeasure read the value from a probe on the OneWire bus.
*
*   The function request and read the value from a probe on the OneWire bus, compute it with compute_measure and returns it as
*   a float
*
*   @return The temperature value as a float.
*/
float tempMeasure(void)
{
    uint8 measure[2] = {0};
    reset_timer_timeout_period(2000000);
    global_run_parse_loop = 1;
    OW_Detect();
    CyDelayUs(240);
    OW_WriteByte(0xCC);
    OW_WriteByte(0x44);
    Timer_timeout_Start();
    while((!CyPins_ReadPin(PinOW_0)) && (global_run_parse_loop))
    {
        CY_NOP;
    }
    Timer_timeout_Stop();
    //blink_led(LED_4);
    OW_Detect();
    CyDelayUs(240);
    OW_WriteByte(0xCC);
    OW_WriteByte(0xBE);
    measure[0] = OW_ReadByte();
    measure[1] = OW_ReadByte();
    return compute_measure(measure);
}
/* [] END OF FILE */
