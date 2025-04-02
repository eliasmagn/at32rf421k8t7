/* add user code begin Header */
/**
  **************************************************************************
  * @file     wk_adc.c
  * @brief    Workbench-Konfigurationsprogramm mit DMA-basierter ADC-Scan-Messung
  *         zur Echtzeitkompensation (Feedback-Loop) für einen DC-DC-Wandler.
  *         Diese Version wurde angepasst, um zwei ADC-Kanäle (PB0 und PB1)
  *         nahezu simultan zu messen, mit maximaler Geschwindigkeit bei einem
  *         Systemtakt von 72 MHz.
  **************************************************************************
  *                       Copyright notice & Disclaimer
  * [Disclaimer-Text wie im Original]
  **************************************************************************
  */
/* add user code end Header */

/* Includes ------------------------------------------------------------------*/
#include "wk_adc.h"

/* add user code begin 0 */
// Zusätzliche Header für DMA (Anpassung an deine Umgebung erforderlich)
#include "dma.h"
/* add user code end 0 */

/**
  * @brief  Initialisiert ADC1 inklusive GPIOs, Scan-Modus und DMA.
  * @param  none
  * @retval none
  */
void wk_adc1_init(void)
{
  /* add user code begin adc1_init 0 */

  /* add user code end adc1_init 0 */

  gpio_init_type gpio_init_struct;
  adc_base_config_type adc_base_struct;

  gpio_default_para_init(&gpio_init_struct);

  /* add user code begin adc1_init 1 */
  // Wir konfigurieren nun beide ADC-Pins PB0 und PB1 als analoge Eingänge.
  /* add user code end adc1_init 1 */

  /* GPIO-Konfiguration ------------------------------------------------------*/
  /* Konfiguriere PB0 und PB1 als analoge Eingänge */
  gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
  gpio_init_struct.gpio_pins = GPIO_PINS_0 | GPIO_PINS_1;  // Annahme: Makros für PB0 und PB1
  gpio_init(GPIOB, &gpio_init_struct);

  /* ADC-Takt konfigurieren --------------------------------------------------*/
  // Bei einem Systemtakt von 72MHz wird hier z. B. ein Prescaler DIV_6 genutzt,
  // der dem ADC einen Takt von 12MHz liefert. Falls möglich, kann hier
  // ein geringerer Teiler gewählt werden, um die Geschwindigkeit zu maximieren.
  crm_adc_clock_div_set(CRM_ADC_DIV_6);

  /* ADC-Grundeinstellungen --------------------------------------------------*/
  adc_base_default_para_init(&adc_base_struct);
  adc_base_struct.sequence_mode = TRUE;                // Scan-/Mehrkanalmodus aktivieren
  adc_base_struct.repeat_mode = FALSE;
  adc_base_struct.data_align = ADC_RIGHT_ALIGNMENT;
  adc_base_struct.ordinary_channel_length = 2;          // Zwei Kanäle werden in der Sequenz abgefragt
  adc_base_config(ADC1, &adc_base_struct);

  /* ADC-Kanalkonfiguration --------------------------------------------------*/
  // Annahme: PB0 entspricht ADC_CHANNEL_14 (Rauschmessung)
  //          PB1 entspricht ADC_CHANNEL_15 (Hauptsignal)
  adc_ordinary_channel_set(ADC1, ADC_CHANNEL_14, 1, ADC_SAMPLETIME_1_5);
  adc_ordinary_channel_set(ADC1, ADC_CHANNEL_15, 2, ADC_SAMPLETIME_1_5);

  // Trigger: Software-Trigger für den Scanmodus. Die ADC-Konversion wird
  // per Software angestoßen und über DMA kontinuierlich übertragen.
  adc_ordinary_conversion_trigger_set(ADC1, ADC12_ORDINARY_TRIG_SOFTWARE, TRUE);
  adc_ordinary_part_mode_enable(ADC1, FALSE);

  /* add user code begin adc1_init 2 */
  // Aktivierung des DMA-Modus im ADC
  adc_dma_mode_enable(ADC1, TRUE);
  /* add user code end adc1_init 2 */

  // ADC aktivieren
  adc_enable(ADC1, TRUE);

  /* ADC-Kalibrierung --------------------------------------------------------*/
  adc_calibration_init(ADC1);
  while(adc_calibration_init_status_get(ADC1));
  adc_calibration_start(ADC1);
  while(adc_calibration_status_get(ADC1));

  /* add user code begin adc1_init 3 */
  // Hier kann optional die DMA-Konfiguration erfolgen, z.B.:
  // dma_adc_config();
  /* add user code end adc1_init 3 */
}

/* add user code begin 1 */
// DMA-Konfiguration und ADC-Datenverarbeitung für den Feedback-Loop

#define ADC_BUFFER_SIZE 2
volatile uint16_t adc_buffer[ADC_BUFFER_SIZE];

/**
  * @brief Konfiguriert den DMA-Controller für ADC1.
  */
void dma_adc_config(void)
{
    dma_init_type dma_init_struct;
    dma_default_para_init(&dma_init_struct);
    dma_init_struct.channel             = DMA_CHANNEL_ADC; // Passe den Kanal an deine Hardware an
    dma_init_struct.direction           = DMA_DIR_PERIPH_TO_MEMORY;
    dma_init_struct.peripheral_base_addr = (uint32_t)&ADC1->DATA; // Adresse des ADC-Datenregisters
    dma_init_struct.memory_base_addr     = (uint32_t)adc_buffer;
    dma_init_struct.buffer_size         = ADC_BUFFER_SIZE;
    dma_init_struct.peripheral_inc_enable = FALSE;
    dma_init_struct.memory_inc_enable     = TRUE;
    dma_init_struct.peripheral_data_width = DMA_PERIPH_DATA_WIDTH_HALFWORD;
    dma_init_struct.memory_data_width     = DMA_MEMORY_DATA_WIDTH_HALFWORD;
    dma_init_struct.mode                  = DMA_MODE_CIRCULAR;   // Zyklischer Modus für kontinuierliche Messungen
    dma_init_struct.priority              = DMA_PRIORITY_HIGH;
    
    dma_init(&dma_init_struct);
    dma_enable(DMA_CHANNEL_ADC, TRUE);
}

/**
  * @brief Verarbeitet die ADC-Daten und berechnet den kompensierten Wert.
  *        Es wird angenommen, dass adc_buffer[0] den Rauschwert (PB0) und
  *        adc_buffer[1] das Hauptsignal (PB1) enthält.
  * @retval Der berechnete Differenzwert (PB1 - PB0)
  */
int32_t process_adc_data(void)
{
    uint16_t noise_value = adc_buffer[0];  // Messung von PB0 (Rauschkomponente)
    uint16_t main_value  = adc_buffer[1];   // Messung von PB1 (Hauptsignal)
    int32_t compensated_value = (int32_t)main_value - (int32_t)noise_value;
    return compensated_value;
}

/* add user code end 1 */
