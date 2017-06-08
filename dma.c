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

void dma_config() {
//    /* DMA Configuration for DMA_ADC */
//    DMA_ADC_Chan = DMA_ADC_DmaInitialize(DMA_ADC_BYTES_PER_BURST, DMA_ADC_REQUEST_PER_BURST, 
//        HI16(DMA_ADC_SRC_BASE), HI16(DMA_ADC_DST_BASE));
//    DMA_ADC_TD[0] = CyDmaTdAllocate();
//    DMA_ADC_TD[1] = CyDmaTdAllocate();
//    DMA_ADC_TD[2] = CyDmaTdAllocate();
//    DMA_ADC_TD[3] = CyDmaTdAllocate();
//    CyDmaTdSetConfiguration(DMA_ADC_TD[0], 4080, DMA_ADC_TD[1], DMA_ADC__TD_TERMOUT_EN | CY_DMA_TD_INC_DST_ADR);
//    CyDmaTdSetConfiguration(DMA_ADC_TD[1], 4080, DMA_ADC_TD[2], DMA_ADC__TD_TERMOUT_EN | CY_DMA_TD_INC_DST_ADR);
//    CyDmaTdSetConfiguration(DMA_ADC_TD[2], 4080, DMA_ADC_TD[3], DMA_ADC__TD_TERMOUT_EN | CY_DMA_TD_INC_DST_ADR);
//    CyDmaTdSetConfiguration(DMA_ADC_TD[3], 4080, DMA_ADC_TD[0], DMA_ADC__TD_TERMOUT_EN | CY_DMA_TD_INC_DST_ADR);
//    //CyDmaTdSetConfiguration(DMA_ADC_TD[3], 4080, DMA_ADC_TD[0], CY_DMA_TD_TERMIN_EN);
//    CyDmaTdSetAddress(DMA_ADC_TD[0], LO16((uint32)ADC_DEC_SAMP_PTR), LO16((uint32)&ADC_array[0]));
//    CyDmaTdSetAddress(DMA_ADC_TD[1], LO16((uint32)ADC_DEC_SAMP_PTR), LO16((uint32)&ADC_array[1]));
//    CyDmaTdSetAddress(DMA_ADC_TD[2], LO16((uint32)ADC_DEC_SAMP_PTR), LO16((uint32)&ADC_array[2]));
//    CyDmaTdSetAddress(DMA_ADC_TD[3], LO16((uint32)ADC_DEC_SAMP_PTR), LO16((uint32)&ADC_array[3]));
//    CyDmaChSetInitialTd(DMA_ADC_Chan, DMA_ADC_TD[0]);
//    CyDmaChEnable(DMA_ADC_Chan, 1);
    
    
    /* DMA Configuration for DMA_ADC */
    DMA_ADC_Chan = DMA_ADC_DmaInitialize(DMA_ADC_BYTES_PER_BURST, DMA_ADC_REQUEST_PER_BURST, 
        HI16(DMA_ADC_SRC_BASE), HI16(DMA_ADC_DST_BASE));
    DMA_ADC_TD[0] = CyDmaTdAllocate();
    DMA_ADC_TD[1] = CyDmaTdAllocate();
    DMA_ADC_TD[2] = CyDmaTdAllocate();
    DMA_ADC_TD[3] = CyDmaTdAllocate();
    //CyDmaTdSetConfiguration(DMA_ADC_TD[0], 4080, DMA_ADC_TD[1], DMA_ADC__TD_TERMOUT_EN | CY_DMA_TD_INC_DST_ADR | CY_DMA_TD_AUTO_EXEC_NEXT);
    //CyDmaTdSetConfiguration(DMA_ADC_TD[1], 4080, DMA_ADC_TD[2], DMA_ADC__TD_TERMOUT_EN | CY_DMA_TD_INC_DST_ADR | CY_DMA_TD_AUTO_EXEC_NEXT);
    //CyDmaTdSetConfiguration(DMA_ADC_TD[2], 4080, DMA_ADC_TD[3], DMA_ADC__TD_TERMOUT_EN | CY_DMA_TD_INC_DST_ADR | CY_DMA_TD_AUTO_EXEC_NEXT);
    //CyDmaTdSetConfiguration(DMA_ADC_TD[3], 4080, DMA_ADC_TD[0], DMA_ADC__TD_TERMOUT_EN | CY_DMA_TD_INC_DST_ADR | CY_DMA_TD_AUTO_EXEC_NEXT);
    CyDmaTdSetConfiguration(DMA_ADC_TD[0], 4080, DMA_ADC_TD[1], DMA_ADC__TD_TERMOUT_EN | CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_TD[1], 4080, DMA_ADC_TD[2], DMA_ADC__TD_TERMOUT_EN | CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_TD[2], 4080, DMA_ADC_TD[3], DMA_ADC__TD_TERMOUT_EN | CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_TD[3], 4080, DMA_ADC_TD[0], DMA_ADC__TD_TERMOUT_EN | CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetAddress(DMA_ADC_TD[0], LO16((uint32)ADC_DEC_SAMP_PTR), LO16((uint32)&ADC_array[0]));
    CyDmaTdSetAddress(DMA_ADC_TD[1], LO16((uint32)ADC_DEC_SAMP_PTR), LO16((uint32)&ADC_array[1]));
    CyDmaTdSetAddress(DMA_ADC_TD[2], LO16((uint32)ADC_DEC_SAMP_PTR), LO16((uint32)&ADC_array[2]));
    CyDmaTdSetAddress(DMA_ADC_TD[3], LO16((uint32)ADC_DEC_SAMP_PTR), LO16((uint32)&ADC_array[3]));
    CyDmaChSetInitialTd(DMA_ADC_Chan, DMA_ADC_TD[0]);
    //CyDmaChEnable(DMA_ADC_Chan, 1);
}

//void dma_config() {



/* [] END OF FILE */
