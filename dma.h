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


#if !defined(DMA_H)
#define DMA_H
    
#include <project.h>
    
/* Defines for DMA_ADC */
#define DMA_ADC_BYTES_PER_BURST 2
#define DMA_ADC_REQUEST_PER_BURST 1
#define DMA_ADC_SRC_BASE (CYDEV_PERIPH_BASE)
#define DMA_ADC_DST_BASE (CYDEV_SRAM_BASE)

/* Variable declarations for DMA_ADC */
/* Move these variable declarations to the top of the function */
uint8 DMA_ADC_Chan;
uint8 DMA_ADC_TD[4];


/***************************************
*        Function Prototypes
***************************************/  

void dma_config();
    
    

#endif

/* [] END OF FILE */
