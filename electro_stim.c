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
#include "electro_stim.h"

    
/***************************************
*        Global variables 
***************************************/
    
uint8 channel_selected = 0;
uint16 stimulation_time = 1000;  // milliseconds
uint8 current_setting = 0;  
uint8 polarity_setting = 'p';


/*******************************************************************************
* Function Name: isr_electro_stim_sto
********************************************************************************
*
* Summary:
*  The handler for the stopping the stimulator
*
*******************************************************************************/

CY_ISR(isr_electro_stim_stop) {
    IDAC_Electro_Stim_Sleep();
    AMux_Stim_DisconnectAll();
    Reg_Electro_Stim_Write(0);
    Timer_Electro_Stim_WriteCounter(stimulation_time);
    //LCD_Position(1,0);
    //LCD_PrintString("Done Stim");
}


/******************************************************************************
* Function Name: Electro_Stim_Start
*******************************************************************************
*
* Summary:
*  Start the hardware needed for the electric stimulator, the
*  IDAC and the AMux
*
*******************************************************************************/

void Electro_Stim_Start(void) {
    IDAC_Electro_Stim_Start();
    IDAC_Electro_Stim_Sleep();
    Timer_Electro_Stim_Start();
    AMux_Stim_Start();
}



/******************************************************************************
* Function Name: Electro_Stim_Set_Channel
*******************************************************************************
*
* Summary:
*  Set which pin the stimulator will push current through when it is started
*
* Parameters:
*  uint8 channel: channel (i.e. pin) the user wants the stimulator to stimulate
*
* Global variables:
*  channel_selected: channel to set AMux to when running the stimulator
*
*******************************************************************************/

void Electro_Stim_Set_Channel(uint8 channel) {
    channel_selected = channel;
    //LCD_Position(0,0);
    //sprintf(LCD_str, "channel: %i", channel_selected);
    //LCD_PrintString(LCD_str);
}

/******************************************************************************
* Function Name: Electro_Stim_Set_Period
*******************************************************************************
*
* Summary:
*  Set the amount of time the stimulator will be on
*
* Parameters:
*  uint16 time: time in milliseconds the stimulator will be on
*
* Global variables:
*  stimulation_time: stored value of time
*******************************************************************************/

void Electro_Stim_Set_Period(uint16 time) {
    Timer_Electro_Stim_WriteCounter(time);
    Timer_Electro_Stim_WritePeriod(time);
    
    //LCD_Position(1,0);
    //sprintf(LCD_str, "time2: %i|%i  ", Timer_Electro_Stim_ReadCounter(), time);
    //LCD_PrintString(LCD_str);
    stimulation_time = time;
}

/******************************************************************************
* Function Name: Electro_Stim_Set_Current
*******************************************************************************
*
* Summary:
*  Set the amount of current the stimulator will pass when turned on if the 
*  value is changed, keep the IDAC asleep if the value is the same
*
* Parameters:
*  uint8 current: current in microamperes the stimulator will pass
*
*******************************************************************************/

void Electro_Stim_Set_Current(uint8 current) {
    if(current != current_setting) {
        //LCD_Position(1,0);
        //sprintf(LCD_str, "current: %i", current);
        //LCD_PrintString(LCD_str);
        IDAC_Electro_Stim_Wakeup();
        IDAC_Electro_Stim_SetValue(current);
        IDAC_Electro_Stim_Sleep();
        current_setting = current;
    }
}

/******************************************************************************
* Function Name: Electro_Stim_Set_Polarity
*******************************************************************************
*
* Summary:
*  Set the direction the stimulator will pass current, positive (source) or 
*  negative (sink) if the value is changed, keep the IDAC asleep if the value 
*  is the same
*
* Parameters:
*  uint8 direction: 'p' for positive current, i.e. pushes out the current or 'n'
*  for negative current
*
*******************************************************************************/

void Electro_Stim_Set_Polarity(uint8 direction) {
    if (direction == polarity_setting)
        return
    IDAC_Electro_Stim_Wakeup();
    //LCD_Position(1,0);
    //sprintf(LCD_str, "direct: %c", direction);
    //LCD_PrintString(LCD_str);
    if(direction == 'p') {
        //LCD_Position(1,0);
        //LCD_PrintString("Setting Source");
        IDAC_Electro_Stim_SetPolarity(IDAC_Electro_Stim_SOURCE);
    }
    else if (direction == 'n') {
        //LCD_Position(1,0);
        //LCD_PrintString("Setting Sink");
        IDAC_Electro_Stim_SetPolarity(IDAC_Electro_Stim_SINK);
    } 
    IDAC_Electro_Stim_Sleep();
}

/******************************************************************************
* Function Name: Electro_Stim_Give_Stimulus
*******************************************************************************
*
* Summary:
*  Give the stimulus
*
*******************************************************************************/

void Electro_Stim_Give_Stimulus(void) {
    AMux_Stim_Select(channel_selected);
    Timer_Electro_Stim_Start();
    IDAC_Electro_Stim_Wakeup();
    //LCD_Position(1,0);
    //sprintf(LCD_str, "time: %i  ", Timer_Electro_Stim_ReadCounter());
    //LCD_PrintString(LCD_str);
    IDAC_Electro_Stim_SetValue(current_setting);
    Reg_Electro_Stim_Write(1);  // Enable the Timer
}



/* [] END OF FILE */
