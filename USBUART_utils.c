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
#include "USBUART_utils.h"

int8 USBUART_sendString(char* sent_message)
{
    /* Service USB CDC when device is configured. */
    if (0u != USBUART_GetConfiguration())
    {
        /* Wait until component is ready to send data to host. */
        while (0u == USBUART_CDCIsReady())
        {
        }
        USBUART_PutString(sent_message);
            
    }
    else 
    {
        return 1;
    }
    return 0;
}

int8 USBUART_recieveString(uint8 *recived_message)
{
    uint8 bytes_read = 0;
    /* Service USB CDC when device is configured. */
    if (0u != USBUART_GetConfiguration())
    {
        /* Check for input data from host. */
        if (0u != USBUART_DataIsReady())
        {
            bytes_read = USBUART_GetData(recived_message,USBUART_BUFFER_SIZE); 
            return bytes_read;
        }
    }
    return 0;
}

/* [] END OF FILE */
