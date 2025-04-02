/* add user code begin Header */
/**
  **************************************************************************
  * @file     wk_adc.h
  * @brief    Header file for ADC configuration with DMA (updated)
  **************************************************************************
  *                       Copyright notice & Disclaimer
  *
  * [Original Disclaimer Text]
  **************************************************************************
  */
/* add user code end Header */

#ifndef __WK_ADC_H
#define __WK_ADC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "at32f421_wk_config.h"
#include "wk_dma.h"  // Nutzung der zentralen DMA-Konfiguration aus dem Projekt

/* Exported functions ------------------------------------------------------- */
void wk_adc1_init(void);
int32_t process_adc_data(void);

#ifdef __cplusplus
}
#endif

#endif
