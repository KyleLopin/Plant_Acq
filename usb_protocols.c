/*********************************************************************************
* File Name: usb_protocols.c
*
* Description:
*  Source code for the protocols used by the USB.
NOTE: WAITING ON USB VENDOR AND PRODUCT ID
*
*
**********************************************************************************
 * Copyright Naresuan University, Phitsanulok Thailand
 * Released under Creative Commons Attribution-ShareAlike  3.0 (CC BY-SA 3.0 US)
*********************************************************************************/

#include <project.h>
#include "usb_protocols.h"


/******************************************************************************
* Function Name: USB_CheckInput
*******************************************************************************
*
* Summary:
*  Check if any incoming USB data and store it to the input buffer
*
* Parameters:
*  uint8 buffer: array of where the data is stored
*
* Return:
*  true (1) if data has been inputed or false (0) if no data
*
* Global variables:
*  OUT_ENDPOINT:  number that is the endpoint coming out of the computer
*
*******************************************************************************/

uint8 USB_CheckInput(uint8 buffer[]) {
    
    if(USBFS_GetEPState(OUT_ENDPOINT) == USBFS_OUT_BUFFER_FULL) {
        
        /* There is data coming in, get the number of bytes*/
        uint8 OUT_COUNT = USBFS_GetEPCount(OUT_ENDPOINT);
        /* Read the OUT endpoint and store data in OUT_Data_buffer */
        USBFS_ReadOutEP(OUT_ENDPOINT, buffer, OUT_COUNT);
        /* Re-enable OUT endpoint */
        USBFS_EnableOutEP(OUT_ENDPOINT);
        //LCD_Position(0,0);
        //LCD_PrintString((char*)buffer);
        
        return true;
    }
    
    return false;
}

/******************************************************************************
* Function Name: USB_Export_Data
*************************************************************************************
*
* Summary:
*  Take a buffer as input and export it, the size of bytes to send is also inputted
*
* Parameters:
*  uint8 array array: array of data to export
*  uint16 size: the number of bytes to send in the array
*
* Return:
*  None
*
* Global variables:
*  MAX_BUFFER_SIZE:  the number of bytes the UBS device can hold
*
*******************************************************************************************/

void USB_Export_Data(uint8 array[], uint16 size) {
    //LCD_Position(1,0);
    //LCD_PrintString("Export_in1");
    for (int i=0; i < size; i=i+MAX_IN_ENDPOINT_BUFFER_SIZE) {
        //LCD_Position(1,0);
        //LCD_PrintString("Export_in2");
        while(USBFS_GetEPState(STREAMING_ENDPOINT) != USBFS_IN_BUFFER_EMPTY)
        {
        }
        //LCD_Position(1,0);
        //LCD_PrintString("Export_in3");
        uint16 size_to_send = size - i;
        if (size_to_send > MAX_IN_ENDPOINT_BUFFER_SIZE) {
            size_to_send = MAX_IN_ENDPOINT_BUFFER_SIZE;
        }
        if(USBFS_GetEPState(STREAMING_ENDPOINT) == USBFS_IN_BUFFER_EMPTY){
            USBFS_LoadInEP(STREAMING_ENDPOINT, &array[i], size_to_send);  // TODO: Fix this
            USBFS_EnableOutEP(OUT_ENDPOINT);
        }
        //LCD_Position(1,0);
        //LCD_PrintString("Export_in4");
    }
}

void USB_Export_Ctrl_Info(uint8* message, uint16 size) {
    //LCD_Position(1,0);
    //LCD_PrintString("Export_inI1");
    while(USBFS_GetEPState(INFO_IN_ENDPOINT) != USBFS_IN_BUFFER_EMPTY)
    {
    }
    //LCD_Position(1,0);
    //LCD_PrintString("Export_inI2");
    if(USBFS_GetEPState(INFO_IN_ENDPOINT) == USBFS_IN_BUFFER_EMPTY){
        USBFS_LoadInEP(INFO_IN_ENDPOINT, message, size);
        USBFS_EnableOutEP(OUT_ENDPOINT);
    }
    //LCD_Position(1,0);
    //LCD_PrintString("Export_inI3");
}

/* [] END OF FILE */