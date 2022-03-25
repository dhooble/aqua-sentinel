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
#include "veml7700.h"

uint8  messageInUSB[SIZE_MESSAGE_USB], messageOutUSB[SIZE_MESSAGE_USB];

uint8 vemlSearch(uint8 * detected_devices)
{
    uint8 VEML7700success;
    uint8 conf[3] = {VEML7700_ALS_CONFIG,0x02,0x1B}; // config code for gain at 1/4 and integration time at 25ms
    VEML7700success = I2C_master_MasterWriteBuf(VEML7700_I2CADDR_DEFAULT,conf,3,I2C_master_MODE_COMPLETE_XFER);
    if(VEML7700success == I2C_master_MSTR_NO_ERROR)
    {
        VEML7700success = I2C_master_MasterStatus();
        if(VEML7700success == I2C_master_MSTAT_WR_CMPLT)
        {
            I2C_master_MasterClearWriteBuf();
            I2C_master_MasterClearStatus();
            *detected_devices |= VEML7700_PRESENCE_CODE; 
            return 0;
        }
    }
    return(VEML7700success);
}

float lumMeasure(void)
{
    uint16 MyMeasure = 0;
    uint8 VEML7700success;
    uint8 read[3]={VEML7700_ALS_DATA,0u,0u};
    VEML7700success = I2C_master_MasterReadBuf(VEML7700_I2CADDR_DEFAULT,read,1,I2C_master_MODE_NO_STOP);
    if(VEML7700success != I2C_master_MSTR_NO_ERROR)
    {
        return -1;
    }
    VEML7700success = I2C_master_MasterReadBuf(VEML7700_I2CADDR_DEFAULT,read,2,I2C_master_MODE_COMPLETE_XFER);
    if(VEML7700success != I2C_master_MSTR_NO_ERROR)
    {
        return -1;
    }
    CyDelay(1000);
    MyMeasure = read[1];
    MyMeasure = (MyMeasure << 8) | read[0];
    return 4*MyMeasure* 0.0576; // calcul extrait de la bibliothèque originale (4* => gain)
}
/* [] END OF FILE */
