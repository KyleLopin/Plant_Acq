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
// standard libraries
#include "stdio.h"
#include "stdlib.h"
#include <stdbool.h>

// local files
#include "dma.h"
#include "electro_stim.h"
#include "globals.h"

#include "usb_protocols.h"


#define NOTHING_TO_SEND     8

int16 result;
int16 VIN;

/* make buffers for the USB ENDPOINTS */
uint8 OUT_Data_Buffer[16];

uint16 lut_index;
uint8 adc_buffer = 0;
uint8 adc_hold; 
uint8 num_channels = 1;
uint8 send_done_flag = NOTHING_TO_SEND;  // TODO:  Whast is this
bool running = false;

char LCD_str[16] = {'\0'};  // buffer for LCD screen, make it extra big to avoid overflow
char usb_str[64];  // buffer for string to send to the usb
//bool Input_Flag = false;
bool Input_Flag = false;

CY_ISR_PROTO(isr_electro_stim_stop);
void HardwareStart(void);
void HardwareSleep(void);
void HardwareWakeup(void);
//void Calibration(void);
uint16 Convert2Dec(uint8 array[], uint8 len);


CY_ISR(adc_buffer_full) {
    //LCD_Position(1,0);
    //sprintf(LCD_str, "buffer: %i", adc_buffer);
    //LCD_PrintString(LCD_str);
    if(! running ) {
        Timer_ADC_Sleep();
        isr_buffer_full_Disable();
        CyDmaClearPendingDrq(DMA_ADC_Chan);  // TODO: check if this is needed
        CyDmaChDisable(DMA_ADC_Chan);
        CyDmaClearPendingDrq(DMA_ADC_Chan);
        HardwareSleep();
        CyDelay(1);
                        
        CyDmaChSetInitialTd(DMA_ADC_Chan, DMA_ADC_TD[0]);
        adc_buffer = 0;
        LCD_Position(0,0);
        LCD_PrintString("stopping");
    }
    else {
        send_done_flag = adc_buffer;  // Set the flag so that the main thread will send the done signal for the proper adc buffer
        adc_buffer = (adc_buffer + 1) % ADC_BUFFERS;
        // No longer need to keep track of indexing because the DMA will
    }
}

CY_ISR(debug1) {
    LCD_Position(0,0);
    LCD_PrintString("debug1");
}

CY_ISR(debug2) {
    LCD_Position(0,0);
    LCD_PrintString("debug2");
}

int main()
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    HardwareStart();
    Electro_Stim_Start();
    HardwareSleep();
    
    //isr_soc_StartEx(debug1);
    //isr_timer_StartEx(debug2);
    //isr_adc_StartEx(adcInterrupt);
    //isr_adc_Disable(); 
    
    isr_buffer_full_StartEx(adc_buffer_full);
    isr_electro_stim_StartEx(isr_electro_stim_stop);
    // Put termination code in all buffers
    ADC_array[0].data[MAX_ADC_BUFFER_SIZE_UINT16-1] = 0xC000;
    ADC_array[1].data[MAX_ADC_BUFFER_SIZE_UINT16-1] = 0xC000;
    ADC_array[2].data[MAX_ADC_BUFFER_SIZE_UINT16-1] = 0xC000;
    ADC_array[3].data[MAX_ADC_BUFFER_SIZE_UINT16-1] = 0xC000;
      
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
            //LCD_Position(0,0);
            //LCD_PrintString(usb_str);
            
            USB_Export_Ctrl_Info((uint8*)usb_str, 5);  // use the 'F' command to retreive the data
            send_done_flag = NOTHING_TO_SEND;
        }
        
        if (Input_Flag == true) {
            switch (OUT_Data_Buffer[0]) { // check what command the user sent
                case EXPORT_ADC_CHANNEL: ; // User wants to export streaming data   
                    uint8 user_ch1 = OUT_Data_Buffer[1]-'0';
                    USB_Export_Data(&ADC_array[user_ch1].usb[0], ADC_BUFFER_SIZE_BYTES); 
                    break;
                
                case START_STREAM: ;  // user wants to read the voltage
                    if (!running) { // if adc is already running ignore this request
                        //LCD_ClearDisplay();
                        //CyDmaChSetInitialTd(DMA_ADC_Chan, DMA_ADC_TD[0]);  // re initialize the DMA at the end and start of a stream 
                        adc_buffer = 0; 
                        isr_buffer_full_Enable();
                        
                        HardwareWakeup();
                        CyDelay(50);
                        CyDmaChSetInitialTd(DMA_ADC_Chan, DMA_ADC_TD[0]);  // re initialize the DMA at the end and start of a stream 
                        CyDmaChEnable(DMA_ADC_Chan, 1);
                        ADC_Wakeup();
                        CyDelay(100);
                        Counter_buffer_reset_WriteCounter(0);
                            
                        Timer_ADC_Enable();
                        running = true;
                    }
                    break;
                case END_STREAM: ;  // user wants to end reading
                    if (running) {  // is adc is already stopped ignore this request
                        running = false; 
                    }
                    break;
                case SET_VIRTUAL_GROUND: ;
                    VDAC_vrt_gnd_Wakeup();
                    uint8 _voltage_ = Convert2Dec(&OUT_Data_Buffer[1], 4);
                    //sprintf(LCD_str, "DAC set: %d    ", _voltage_);
                    //LCD_Position(1,0);
                    //LCD_PrintString(LCD_str);
                    VDAC_vrt_gnd_SetValue(_voltage_);
                    VDAC_vrt_gnd_Sleep();
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
                    Counter_buffer_reset_Wakeup();
                    Counter_buffer_reset_WriteCompare(num_channels);
                    Counter_buffer_reset_Sleep();
                    break;
                    
                case SET_STIMULATOR: ; // The user wants to set the simulator in format "S|XXX|YYYYY|Z|A"
                    // where XXX is the value to put in the IDAC, YYYYY is put in the timer period, Z is polarity and A is channel
                    uint8 new_current = Convert2Dec(&OUT_Data_Buffer[2], 3);
                    uint16 new_time = Convert2Dec(&OUT_Data_Buffer[6], 5);
                    uint8 new_polarity = OUT_Data_Buffer[12];
                    uint8 new_channel = OUT_Data_Buffer[14]-'0';
                    Electro_Stim_Set_Current(new_current);
                    Electro_Stim_Set_Period(new_time-1);
                    Electro_Stim_Set_Channel(new_channel-1);
                    Electro_Stim_Set_Polarity(new_polarity);
                    break;
                case GIVE_STIMULUS: ;
                    Electro_Stim_Give_Stimulus();
                    //LCD_Position(1,0);
                    //LCD_PrintString("Give stim");
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

void HardwareStart(void){
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

void HardwareSleep(void) {
    Timer_ADC_Sleep();
    Opamp_vrt_grnd_Sleep();
    ADC_Sleep();
    VDAC_vrt_gnd_Sleep();
    PWM_delay_Sleep();
    Counter_buffer_reset_Sleep();
}

void HardwareWakeup(void) {
    //ADC_Wakeup();
    Opamp_vrt_grnd_Wakeup();
    
    VDAC_vrt_gnd_Wakeup();
    
    CyDelay(500);  // let the VDAC set the voltage settle before starting the timer that will start the conversions
    
    PWM_delay_Wakeup();
    Counter_buffer_reset_Wakeup();
    Timer_ADC_Wakeup();
    
}

uint16 Convert2Dec(uint8 array[], uint8 len){
    uint16 num = 0;
    for (int i = 0; i < len; i++){
        num = num * 10 + (array[i] - '0');
    }
    return num;
}

/* [] END OF FILE */
