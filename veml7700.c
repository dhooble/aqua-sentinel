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
        CyDelay(100);
        VEML7700success = I2C_master_MasterStatus();
        if(VEML7700success == I2C_master_MSTAT_WR_CMPLT)
        {
            I2C_master_MasterClearWriteBuf();
            I2C_master_MasterClearStatus();
            *detected_devices |= VEML7700_PRESENCE_CODE; 
            UART_PC_PutString("Capteur lum connecte\r\n");
            return 0;
        }
    }
    
    UART_PC_PutString("Capteur lum non connecte\r\n");
    return(VEML7700success);
}

void getStatus(void)
{
    uint8 VEML7700success = I2C_master_MasterStatus();
    UART_PC_PutString("\t[INFO](lumMeasure): first status : ");
    if(VEML7700success == I2C_master_MSTR_NO_ERROR) UART_PC_PutString("I2C_master_MSTR_NO_ERROR ");
    if(VEML7700success == I2C_master_MSTR_BUS_BUSY) UART_PC_PutString("I2C_master_MSTR_BUS_BUSY ");
    if(VEML7700success == I2C_master_MSTR_NOT_READY) UART_PC_PutString("I2C_master_MSTR_NOT_READY ");
    if(VEML7700success == I2C_master_MSTR_ERR_LB_NAK) UART_PC_PutString("I2C_master_MSTR_ERR_LB_NAK ");
    if(VEML7700success == I2C_master_MSTR_ERR_ARB_LOST) UART_PC_PutString("I2C_master_MSTR_ERR_ARB_LOST ");
    if(VEML7700success == I2C_master_MSTR_ERR_ABORT_START_GEN) UART_PC_PutString("I2C_master_MSTR_ERR_ABORT_START_GEN ");
    UART_PC_PutCRLF('\0');
    UART_PC_PutString("\t[INFO](lumMeasure): master status : ");
    if(VEML7700success & I2C_master_MSTAT_WR_CMPLT) UART_PC_PutString("I2C_master_MSTAT_WR_CMPLT ");
    if(VEML7700success & I2C_master_MSTAT_RD_CMPLT) UART_PC_PutString("I2C_master_MSTAT_RD_CMPLT ");
    if(VEML7700success & I2C_master_MSTAT_XFER_INP) UART_PC_PutString("I2C_master_MSTAT_XFER_INP ");
    if(VEML7700success & I2C_master_MSTAT_XFER_HALT) UART_PC_PutString("I2C_master_MSTAT_XFER_HALT ");
    if(VEML7700success & I2C_master_MSTAT_ERR_SHORT_XFER) UART_PC_PutString("I2C_master_MSTAT_ERR_SHORT_XFER ");
    if(VEML7700success & I2C_master_MSTAT_ERR_ADDR_NAK) UART_PC_PutString("I2C_master_MSTAT_ERR_ADDR_NAK ");
    if(VEML7700success & I2C_master_MSTAT_ERR_ARB_LOST) UART_PC_PutString("I2C_master_MSTAT_ERR_ARB_LOST ");
    if(VEML7700success & I2C_master_MSTAT_ERR_XFER) UART_PC_PutString("I2C_master_MSTAT_ERR_XFER ");
    UART_PC_PutCRLF('\0');
}

float lumMeasure(void)
{
    uint16 MyMeasure = 0;
    uint8 VEML7700success;
    uint8 read[3]={VEML7700_ALS_DATA,0u,0u};
    char text_val[13];
    
    VEML7700success = I2C_master_MasterSendStart(VEML7700_I2CADDR_DEFAULT,0);
    VEML7700success = I2C_master_MasterWriteByte(VEML7700_ALS_DATA);
    if(VEML7700success != I2C_master_MSTR_NO_ERROR)
    {
        UART_PC_PutString("[INFO](lumMeasure): master write error.\r\n");
        return -1;
    }
    I2C_master_MasterSendRestart(VEML7700_I2CADDR_DEFAULT,1);
    read[0] = I2C_master_MasterReadByte(I2C_master_ACK_DATA);
    read[1] = I2C_master_MasterReadByte(I2C_master_NAK_DATA);
    VEML7700success = I2C_master_MasterSendStop();
    if(VEML7700success != I2C_master_MSTR_NO_ERROR)
    {
        UART_PC_PutString("[INFO](lumMeasure): master read error.\r\n");
        getStatus();
        return -1;
    }
    I2C_master_MasterClearReadBuf();
    I2C_master_MasterClearWriteBuf();
    I2C_master_MasterClearStatus();
    CyDelay(1000);
    MyMeasure = read[1];
    MyMeasure = (MyMeasure << 8) | read[0];
    sprintf(text_val,"%f",4*MyMeasure* 0.0576);
    UART_PC_PutString("\tluminosite mesuree : ");
    UART_PC_PutString(text_val);
    UART_PC_PutString("lx");
    UART_PC_PutCRLF('\0');
    return 4*MyMeasure* 0.0576; // calcul extrait de la bibliothèque originale (4* => gain)
}
/* [] END OF FILE */
