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
#ifndef _ELECTRO_STIM_H
#define _ELECTRO_STIM_H
    
#include <project.h>
    
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
    
extern char LCD_str[16];
  

/***************************************
*        Function Prototypes
***************************************/  
CY_ISR_PROTO(isr_electro_stim_stop);   
void Electro_Stim_Start(void);
void Electro_Stim_Set_Channel(uint8 channel);
void Electro_Stim_Set_Period(uint16 time);
void Electro_Stim_Set_Current(uint8 current);
void Electro_Stim_Set_Polarity(uint8 direction);
void Electro_Stim_Give_Stimulus(void);
    
    
#endif

/* [] END OF FILE */
