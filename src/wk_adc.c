/* add user code begin Header */
/**
  **************************************************************************
  * @file     wk_adc.c
  * @brief    ADC configuration with DMA (updated for AT32 Workbench DMA config)
  **************************************************************************
  *                       Copyright notice & Disclaimer
  *
  * [Original Disclaimer Text]
  **************************************************************************
  */
/* add user code end Header */

#include "wk_adc.h"

volatile uint16_t adc_buffer[2];  // Puffer für zwei ADC-Werte (Scan-Modus)

void wk_adc1_init(void)
{
  gpio_init_type gpio_init_struct;
  adc_base_config_type adc_base_struct;

  gpio_default_para_init(&gpio_init_struct);

  /* Konfiguriere PB0 und PB1 als analoge Eingänge */
  gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
  gpio_init_struct.gpio_pins = GPIO_PINS_0 | GPIO_PINS_1;  // Annahme: diese Makros sind definiert
  gpio_init(GPIOB, &gpio_init_struct);

  /* ADC-Takt konfigurieren (z.B. CRM_ADC_DIV_6 bei 120 MHz Systemtakt) */
  crm_adc_clock_div_set(CRM_ADC_DIV_6);

  /* ADC-Grundeinstellungen initialisieren */
  adc_base_default_para_init(&adc_base_struct);
  adc_base_struct.sequence_mode = TRUE;                // Scan-/Mehrkanalmodus aktivieren
  adc_base_struct.repeat_mode = FALSE;
  adc_base_struct.data_align = ADC_RIGHT_ALIGNMENT;
  adc_base_struct.ordinary_channel_length = 2;          // Zwei Kanäle in der Konversionssequenz
  adc_base_config(ADC1, &adc_base_struct);

  /* ADC-Kanalkonfiguration:
     Annahme: PB0 -> ADC_CHANNEL_14 (Rauschmessung)
              PB1 -> ADC_CHANNEL_15 (Hauptsignal) */
  adc_ordinary_channel_set(ADC1, ADC_CHANNEL_8, 1, ADC_SAMPLETIME_1_5);
  adc_ordinary_channel_set(ADC1, ADC_CHANNEL_9, 2, ADC_SAMPLETIME_1_5);

  /* Software-Trigger für den ADC einstellen */
  adc_ordinary_conversion_trigger_set(ADC1, ADC12_ORDINARY_TRIG_SOFTWARE, TRUE);
  adc_ordinary_part_mode_enable(ADC1, FALSE);

  /* DMA-Modus für den ADC aktivieren */
  adc_dma_mode_enable(ADC1, TRUE);

  /* ADC aktivieren */
  adc_enable(ADC1, TRUE);

  /* ADC-Kalibrierung */
  adc_calibration_init(ADC1);
  while(adc_calibration_init_status_get(ADC1));
  adc_calibration_start(ADC1);
  while(adc_calibration_status_get(ADC1));
}

int32_t process_adc_data(void)
{
  /* adc_buffer[0] enthält den Rauschwert (PB0), adc_buffer[1] das Hauptsignal (PB1) */
  uint16_t noise_value = adc_buffer[0];
  uint16_t main_value  = adc_buffer[1];
  return (int32_t)main_value - (int32_t)noise_value;
}
