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
#include <project.h>
#include "dma.h"
#include "globals.h"
#include "usb_protocols.h"
#include "stdio.h"
#include "stdlib.h"
#include <stdbool.h>

//#define false               0
//#define true                1

#define NOTHING_TO_SEND     8



int16 result;
int16 VIN;

/* make buffers for the USB ENDPOINTS */

//uint8 IN_Data_Buffer[MAX_NUM_BYTES];
uint8 OUT_Data_Buffer[16];

uint16 lut_index;
uint8 adc_buffer = 0;
uint8 adc_hold; 
uint8 num_channels = 1;
//uint16 buffer_size_data_pts = 2040;  // collect 2040 data points in an adc channel then export it
//uint16 buffer_size_bytes = 4082;  // 2042 data points is 4080 bytes plus 2 more bytes for termination code
int16 counter = 0;
uint8 send_done_flag = NOTHING_TO_SEND;  // TODO:  Whast is this
bool running = false;

char LCD_str[16] = {'\0'};  // buffer for LCD screen, make it extra big to avoid overflow
char usb_str[64];  // buffer for string to send to the usb
//bool Input_Flag = false;
bool Input_Flag = 1;

void HardwareSetup(void);
//void Calibration(void);
uint16 Convert2Dec(uint8 array[], uint8 len);

//CY_ISR(adcInterrupt) {
//    //ADC_array[adc_buffer].data[lut_index] = ADC_GetResult16();
//    //sprintf(LCD_str, "counter: %d", counter);
//    //LCD_Position(1, 0);
//    //LCD_PrintString(LCD_str);
//    
//   for (uint8 i = 0; i < num_channels; i++) {
//        ADC_array[adc_buffer].data[lut_index] = counter + 1000*i;
//        lut_index++;
//        //LCD_PrintString("check");
//    }
//    counter++;
//    
//    if (lut_index >= buffer_size_data_pts) {
//        ADC_array[adc_buffer].data[lut_index] = 0xC000;
//        lut_index = 0;
//        adc_hold = adc_buffer;
//        adc_buffer = (adc_buffer + 1) % ADC_BUFFERS;
//        send_done_flag = adc_hold;
//    }
//    if (counter > 10000) {
//        counter = -10000;
//    }
//}

CY_ISR(adc_buffer_full) {
    
    send_done_flag = adc_buffer;  // Set the flag so that the main thread will send the done signal for the proper adc buffer
    
    //ADC_array[adc_buffer].data[MAX_ADC_BUFFER_SIZE_UINT16-1] = 0xC000;
    adc_buffer = (adc_buffer + 1) % ADC_BUFFERS;
    
    // No longer need to keep track of indexing because the DMA will
    //LCD_Position(1,0);
    //LCD_PrintString("in isr");
    //LCD_Position(1,0);
    //LCD_PrintString("buffer full");
    
}

CY_ISR(debug1) {
    LCD_Position(1,0);
    LCD_PrintString("in isr");
    
}

CY_ISR(debug2) {
    counter++;
    LCD_Position(1,0);
    LCD_PrintNumber(counter);
    
}

CY_ISR(debug3) {
    LCD_Position(1,0);
    LCD_PrintString("debug3");
    isr_timer_Stop();
}

// TODO: User input changes adc channels by changing perio of counter_adc_channel



int main()
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    HardwareSetup();

    //isr_adc_StartEx(adcInterrupt);
    //isr_adc_Disable(); 
    //isr_eoc_StartEx(debug1);
    //isr_eoc_Stop();
    
    //isr_soc_StartEx(debug2);
    //isr_soc_Stop();
    
    //isr_timer_StartEx(debug3);
    
    isr_buffer_full_StartEx(adc_buffer_full);
    //isr_buffer_full_Disable();
    
    //isr_seq_reset_StartEx(debug2);
    
    // Put termination code in all buffers
    ADC_array[0].data[MAX_ADC_BUFFER_SIZE_UINT16-1] = 0xC000;
    ADC_array[1].data[MAX_ADC_BUFFER_SIZE_UINT16-1] = 0xC000;
    ADC_array[2].data[MAX_ADC_BUFFER_SIZE_UINT16-1] = 0xC000;
    ADC_array[3].data[MAX_ADC_BUFFER_SIZE_UINT16-1] = 0xC000;
    
//    
    USBFS_Start(0, USBFS_DWR_VDDD_OPERATION);  // initialize the USB
    while(!USBFS_bGetConfiguration());  //Wait till it the usb gets its configuration from the PC
    USBFS_EnableOutEP(OUT_ENDPOINT);
    
    dma_config();
    
    CyWdtStart(CYWDT_1024_TICKS, CYWDT_LPMODE_NOCHANGE);
    Opamp_vrt_grnd_Start();

    for(;;)
    {
        CyWdtClear();
        if(USBFS_IsConfigurationChanged()) {  // if the configuration is changed reenable the OUT ENDPOINT
            while(!USBFS_GetConfiguration()) {  // wait for the configuration with windows / controller is updated
            }
            USBFS_EnableOutEP(OUT_ENDPOINT);  // reenable OUT ENDPOINT
        }
        /* check if there is a response from the computer */
        if (Input_Flag == false) {  // make sure any input has already been dealt with
            Input_Flag = USB_CheckInput(OUT_Data_Buffer);  // check if there is a response from the computer
        }
        if (send_done_flag != NOTHING_TO_SEND) {
            sprintf(usb_str, "Done%d", send_done_flag);  // tell the user the data is ready to pick up and which channel its on
            USB_Export_Ctrl_Info((uint8*)usb_str, 5);  // use the 'F' command to retreive the data
            //LCD_Position(1,0);
            //LCD_PrintString(usb_str);
            //LCD_PrintNumber(counter);
            send_done_flag = NOTHING_TO_SEND;
        }
        //LCD_Position(1,0);
        //LCD_PrintInt16(ADC_array[0].data[0]);
        //LCD_PrintNumber(ADC_array[3].data[0]);
        
        if (Input_Flag == true) {
            //LCD_Position(0,0);
            //LCD_PrintString("got input");
            //LCD_Position(1,0);
            //LCD_PrintString(OUT_Data_Buffer);
            //sprintf(LCD_str, "buffer666%d: %d", counter, ADC_array[3].data[0]);  // tell the user the data is ready to pick up and which channel its on
            //LCD_Position(1,0);
            //LCD_PrintString(LCD_str);
            switch (OUT_Data_Buffer[0]) { // check what command the user sent
                case EXPORT_ADC_CHANNEL: ; // User wants to export streaming data   
                    //LCD_Position(0,0);
                    //LCD_PrintString("Export1");
                    uint8 user_ch1 = OUT_Data_Buffer[1]-'0';
                    USB_Export_Data(&ADC_array[user_ch1].usb[0], ADC_BUFFER_SIZE_BYTES); 
                    //LCD_Position(0,0);
                    //sprintf(LCD_str, "end export: %d", user_ch1);
                    //LCD_PrintString(LCD_str);
                    break;
                
                case START_STREAM: ;  // user wants to read the voltage
                    if (!running) { // if adc is already running ignore this request
                        LCD_Position(0,0);
                        LCD_PrintString("start1");
                        CyDmaChEnable(DMA_ADC_Chan, 1);
                        ADC_Wakeup();
                        Timer_ADC_Wakeup();
                        running = true;
                    }
                    break;
                case END_STREAM: ;  // user wants to end reading
                    if (running) {  // is adc is already stopped ignore this request
                        Timer_ADC_Sleep();
                        CyDelay(1);
                        CyDmaClearPendingDrq(DMA_ADC_Chan);
                        CyDmaChDisable(DMA_ADC_Chan);
                        CyDmaChSetInitialTd(DMA_ADC_Chan, DMA_ADC_TD[0]);
                        lut_index = 0;
                        adc_buffer = 0;
                        counter = 0;
                        //ADC_Sleep();
                        LCD_ClearDisplay();
                        LCD_Position(0,0);
                        LCD_PrintString("end");
                        running = false;
                    }
                    break;
                case SET_VIRTUAL_GROUND: ;
                    //VDAC_vrt_gnd_SetValue(OUT_Data_Buffer[1]);
                    uint8 _voltage_ = Convert2Dec(&OUT_Data_Buffer[1], 4);
                    sprintf(LCD_str, "DAC set: %d    ", _voltage_);
                    LCD_Position(1,0);
                    LCD_PrintString(LCD_str);
                    VDAC_vrt_gnd_SetValue(_voltage_);
                    break;
                case SET_ADC_BUFFER_GAIN: ;  // user can set the buffer gain of the adc
                    ADC_SetBufferGain(OUT_Data_Buffer[1]-'0');
                    break;
//                case CALIBRATE: ;  // calibrate 
//                    Calibration();
//                    break;
                case IDENTIFY: ; // Identify the device
                    USB_Export_Data((uint8*)"USB Test - Plant_Acq", 20);
                    break;
                case SET_NUMBER_OF_CHANNELS: ; // Set how many channels the adc should sample
                    num_channels = OUT_Data_Buffer[1]-'0';
                    sprintf(LCD_str, "Channels: %d", num_channels);
                    LCD_Position(0,0);
                    LCD_PrintString(LCD_str);
                    Counter_buffer_reset_WriteCompare(num_channels);
                    break;
                
            }
            OUT_Data_Buffer[0] = '0';  // clear data buffer cause it has been processed
            Input_Flag = false;  // turn off input flag because it has been processed
        }
        
    }
}
//
//void Calibration(void) {
//    VDAC_calibrate_Start();
//    AMux_calibrate_Start();
//    AMux_calibrate_Select(0);
//    CyDelay(200);
//    VDAC_calibrate_SetValue(0);
//    for (uint8 i = 0; i < 4; i++) {
//        CyDelay(500);
//        VDAC_calibrate_SetValue(20); // set VDAC to 20*4mV = 80 mV
//        CyDelay(500);
//        VDAC_calibrate_SetValue(0);
//    }
//    CyDelay(500);
//    VDAC_calibrate_Stop();
//    AMux_calibrate_DisconnectAll();
//}

void HardwareSetup(void){
    LCD_Start();
    LCD_Position(0,0);
    LCD_PrintString("Plant ACQ");
    ADC_Start();
    VDAC_vrt_gnd_Start();
    Timer_ADC_Start();
    Timer_ADC_Sleep();
    PWM_delay_Start();
    Counter_buffer_reset_Start();
}

uint16 Convert2Dec(uint8 array[], uint8 len){
    uint16 num = 0;
    for (int i = 0; i < len; i++){
        num = num * 10 + (array[i] - '0');
    }
    return num;
}

/* [] END OF FILE */
