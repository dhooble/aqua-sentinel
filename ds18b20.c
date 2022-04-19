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

uint8 OW_Detect(void)
{
    

    CyPins_ClearPin(PinOW_0);
    CyDelayUs(480);

    CyPins_SetPin(PinOW_0);
    CyDelayUs(70);

    return CyPins_ReadPin(PinOW_0);
}

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

float tempMeasure(void)
{
    uint8 measure[2] = {0};
    OW_Detect();
    CyDelayUs(240);
    OW_WriteByte(0xCC);
    OW_WriteByte(0x44);
    while(!CyPins_ReadPin(PinOW_0))
    {
        CY_NOP;
    }
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
