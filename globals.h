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

#if !defined(GLOBALS_H)
#define GLOBALS_H


#include <project.h>

/**************************************
*        Global Variables
**************************************/ 
    
    
    

    
#define EXPORT_ADC_CHANNEL          'F'
#define START_STREAM                'R'
#define END_STREAM                  'E'
#define SET_VIRTUAL_GROUND          'V'
#define SET_ADC_BUFFER_GAIN         'B'
#define CALIBRATE                   'C'
#define IDENTIFY                    'I'
#define SET_NUMBER_OF_CHANNELS      'S'
    
/**************************************
*        USB INPUT OPTIONS
**************************************/ 

#define ADC_BUFFERS        4

#define MAX_ADC_BUFFER_SIZE_UINT16  2041  // can hold 1, 2, 3, or 4 adc channels being read with no modulo
#define ADC_BUFFER_SIZE_BYTES       4082  // 2 bytes per data point //+2 for terminal signal

union data_usb_union {
    uint8 usb[2*MAX_ADC_BUFFER_SIZE_UINT16];
    int16 data[MAX_ADC_BUFFER_SIZE_UINT16];
};
union data_usb_union ADC_array[ADC_BUFFERS];
    

#endif

/* [] END OF FILE */
