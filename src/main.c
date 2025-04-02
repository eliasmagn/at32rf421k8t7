/* add user code begin Header */
/**
  **************************************************************************
  * @file     main.c
  * @brief    Main program (updated for new DMA configuration)
  **************************************************************************
  *                       Copyright notice & Disclaimer
  *
  * [Original Disclaimer Text]
  **************************************************************************
  */
/* add user code end Header */

#include "at32f421_wk_config.h"
#include "wk_adc.h"
#include "wk_usart.h"
#include "wk_gpio.h"
#include "wk_system.h"
#include "wk_dma.h"  // Neu: DMA-Funktionen aus dem Projekt

int main(void)
{
  /* Systemkonfiguration */
  wk_system_clock_config();
  wk_periph_clock_config();
  wk_nvic_config();
  wk_timebase_init();

  /* USART2 initialisieren (unterstützt printf) */
  wk_usart2_init();

  /* ADC1 initialisieren (Scan-Modus, 2 Kanäle, DMA aktiviert) */
  wk_adc1_init();

  /* DMA für den ADC konfigurieren – diese Funktion ist in wk_dma.c implementiert */
  /* init dma1 channel1 */
  wk_dma1_channel1_init();
  /* config dma channel transfer parameter */
  /* user need to modify define values DMAx_CHANNELy_XXX_BASE_ADDR and DMAx_CHANNELy_BUFFER_SIZE in at32xxx_wk_config.h */
  wk_dma_channel_config(DMA1_CHANNEL1, 
                        (uint32_t)&ADC1->odt, 
                        DMA1_CHANNEL1_MEMORY_BASE_ADDR, 
                        DMA1_CHANNEL1_BUFFER_SIZE);
  dma_channel_enable(DMA1_CHANNEL1, TRUE);

  /* GPIOs initialisieren */
  wk_gpio_config();

  volatile uint32_t time_cnt = 0;
  printf("usart printf example: retarget the c library printf function to the usart\r\n");

  while(1)
  {
    printf("usart printf counter: %lu\r\n", time_cnt++);

    /* ADC-Konversion im Scan-Modus anstoßen – beide Kanäle werden in einem Zyklus abgefragt */
    adc_ordinary_software_trigger_enable(ADC1, TRUE);

    /* Warten, bis die ADC-Konversion abgeschlossen ist */
    while(adc_flag_get(ADC1, ADC_CCE_FLAG) == RESET);

    /* Verarbeite die DMA-Daten: Berechne die Differenz (PB1 - PB0) */
    int32_t compensated_value = process_adc_data();
    printf("ADC compensated value (PB1 - PB0): %ld\r\n", compensated_value);

    wk_delay_ms(1000);
  }
}
