/* add user code begin Header */
/**
  **************************************************************************
  * @file     main.c
  * @brief    main program (angepasst für DMA-basierte ADC-Scan-Messung)
  **************************************************************************
  *                       Copyright notice & Disclaimer
  *
  * [Original Disclaimer-Text]
  *
  **************************************************************************
  */
/* add user code end Header */

/* Includes ------------------------------------------------------------------*/
#include "at32f421_wk_config.h"
#include "wk_adc.h"
#include "wk_usart.h"
#include "wk_gpio.h"
#include "wk_system.h"

/* private includes ----------------------------------------------------------*/
/* add user code begin private includes */

/* add user code end private includes */

/* private typedef -----------------------------------------------------------*/
/* add user code begin private typedef */

/* add user code end private typedef */

/* private define ------------------------------------------------------------*/
/* add user code begin private define */

/* add user code end private define */

/* private macro -------------------------------------------------------------*/
/* add user code begin private macro */

/* add user code end private macro */

/* private variables ---------------------------------------------------------*/
/* add user code begin private variables */

/* add user code end private variables */

/* private function prototypes --------------------------------------------*/
/* add user code begin function prototypes */

/* add user code end function prototypes */

/* private user code ---------------------------------------------------------*/
/* add user code begin 0 */

/* add user code end 0 */

/**
  * @brief main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  /* add user code begin 1 */
  /* add user code end 1 */

  /* Systemkonfiguration */
  wk_system_clock_config();
  wk_periph_clock_config();
  wk_nvic_config();
  wk_timebase_init();

  /* USART2 initialisieren (unterstützt printf) */
  wk_usart2_init();

  /* ADC1 initialisieren (Scan-Modus, 2 Kanäle, DMA aktiviert) */
  wk_adc1_init();

  /* DMA für den ADC konfigurieren */
  dma_adc_config();

  /* GPIOs initialisieren */
  wk_gpio_config();

  /* add user code begin 2 */
  __IO uint32_t time_cnt = 0;
  /* add user code end 2 */
  
  printf("usart printf example: retarget the c library printf function to the usart\r\n");

  while(1)
  {
    /* add user code begin 3 */
    printf("usart printf counter: %lu\r\n", time_cnt++);
    
    /* ADC-Konversion anstoßen (Scan-Modus: beide Kanäle in einem Zyklus) */
    adc_ordinary_software_trigger_enable(ADC1, TRUE);
    
    /* Warten, bis die ADC-Konversion abgeschlossen ist */
    while(adc_flag_get(ADC1, ADC_CCE_FLAG) == RESET);

    /* DMA-Daten verarbeiten: Differenz zwischen PB1 und PB0 berechnen */
    int32_t compensated_value = process_adc_data();
    printf("ADC compensated value (PB1 - PB0): %ld\r\n", compensated_value);
    /* add user code end 3 */

    wk_delay_ms(1000);
  }
}

/* add user code begin 4 */

/* add user code end 4 */
