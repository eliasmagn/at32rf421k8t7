/**
  **************************************************************************
  * @file     at32f421_board.c
  * @brief    Minimal board support package for the XY6020L.
  *           Provides delay functions, UART2 initialization and configures 
  *           all unused GPIOs to high impedance.
  **************************************************************************
  */

  #include "at32f421_board.h"

  /* delay macros */
  #define STEP_DELAY_MS    50
  
  /* Global delay variables */
  static __IO uint32_t fac_us;
  static __IO uint32_t fac_ms;
  
  /* Support for printf redirection */
  #if (__ARMCC_VERSION > 6000000)
    __asm (".global __use_no_semihosting\n\t");
    void _sys_exit(int x) { x = x; }
    void _ttywrch(int ch) { ch = ch; }
    FILE __stdout;
  #else
   #ifdef __CC_ARM
    #pragma import(__use_no_semihosting)
    struct __FILE { int handle; };
    FILE __stdout;
    void _sys_exit(int x) { x = x; }
    void _ttywrch(int ch) { ch = ch; }
   #endif
  #endif
  
  #if defined (__GNUC__) && !defined (__clang__)
    #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
  #else
    #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
  #endif
  
  PUTCHAR_PROTOTYPE
  {
    while(usart_flag_get(USART2, USART_TDBE_FLAG) == RESET);
    usart_data_transmit(USART2, (uint16_t)ch);
    while(usart_flag_get(USART2, USART_TDC_FLAG) == RESET);
    return ch;
  }
  
  #if (defined (__GNUC__) && !defined (__clang__)) || (defined (__ICCARM__))
  #if defined (__GNUC__) && !defined (__clang__)
  int _write(int fd, char *pbuffer, int size)
  #elif defined (__ICCARM__)
  #pragma module_name = "?__write"
  int __write(int fd, char *pbuffer, int size)
  #endif
  {
    for (int i = 0; i < size; i++)
    {
      while(usart_flag_get(USART2, USART_TDBE_FLAG) == RESET);
      usart_data_transmit(USART2, (uint16_t)(*pbuffer++));
      while(usart_flag_get(USART2, USART_TDC_FLAG) == RESET);
    }
    return size;
  }
  #endif
  
  /**
    * @brief  Initialize UART2 for printing (TX on PA2).
    * @param  baudrate: UART baudrate.
    * @retval none
    */
  void uart_print_init(uint32_t baudrate)
  {
    gpio_init_type gpio_init_struct;
  
  #if defined (__GNUC__) && !defined (__clang__)
    setvbuf(stdout, NULL, _IONBF, 0);
  #endif
  
    /* Enable clocks for USART2 and GPIOA */
    crm_periph_clock_enable(CRM_USART2_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
  
    gpio_default_para_init(&gpio_init_struct);
  
    /* Configure PA2 for USART2 TX */
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
    gpio_init_struct.gpio_pins = GPIO_PINS_2;   // PA2 als TX
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
    gpio_init(GPIOA, &gpio_init_struct);
  
    gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE2, GPIO_MUX_1);
  
    /* Configure USART2 */
    usart_init(USART2, baudrate, USART_DATA_8BITS, USART_STOP_1_BIT);
    usart_transmitter_enable(USART2, TRUE);
    usart_enable(USART2, TRUE);
  }
  
  /**
    * @brief  Configure unused GPIOs as high impedance to avoid interference.
    *         (Alle GPIOs, die nicht für UART2 genutzt werden, werden als Eingang
    *          ohne Pull-up/Pull-down konfiguriert.)
    * @retval none
    */
  void board_unused_gpio_init(void)
  {
    gpio_init_type gpio_init_struct;
    
    /* Grundkonfiguration für High-Z: Eingang, kein Pull */
    gpio_default_para_init(&gpio_init_struct);
    gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  
    /* Konfiguriere GPIOA: PA0, PA1, PA3, PA4, PA5, PA6, PA7 (PA2 wird für UART2 genutzt) */
    gpio_init_struct.gpio_pins = GPIO_PINS_0 | GPIO_PINS_1 | GPIO_PINS_3 |
                                 GPIO_PINS_4 | GPIO_PINS_5 | GPIO_PINS_6 | GPIO_PINS_7;
    gpio_init(GPIOA, &gpio_init_struct);
  
    /* Konfiguriere GPIOB: PB0 und PB1 */
    crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
    gpio_init_struct.gpio_pins = GPIO_PINS_0 | GPIO_PINS_1;
    gpio_init(GPIOB, &gpio_init_struct);
  
    /* Konfiguriere GPIOF: PF0 und PF1 */
    crm_periph_clock_enable(CRM_GPIOF_PERIPH_CLOCK, TRUE);
    gpio_init_struct.gpio_pins = GPIO_PINS_0 | GPIO_PINS_1;
    gpio_init(GPIOF, &gpio_init_struct);
  }
  
  /**
    * @brief  Initialize delay functions.
    * @retval none
    */
  void delay_init()
  {
    systick_clock_source_config(SYSTICK_CLOCK_SOURCE_AHBCLK_NODIV);
    fac_us = system_core_clock / 1000000U;
    fac_ms = fac_us * 1000U;
  }
  
  /**
    * @brief  Delay for a specified number of microseconds.
    * @param  nus: Delay in microseconds.
    * @retval none
    */
  void delay_us(uint32_t nus)
  {
    uint32_t temp;
    SysTick->LOAD = nus * fac_us;
    SysTick->VAL = 0;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    do {
      temp = SysTick->CTRL;
    } while ((temp & 0x01) && !(temp & (1 << 16)));
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    SysTick->VAL = 0;
  }
  
  /**
    * @brief  Delay for a specified number of milliseconds.
    * @param  nms: Delay in milliseconds.
    * @retval none
    */
  void delay_ms(uint16_t nms)
  {
    uint32_t temp;
    while(nms)
    {
      if(nms > STEP_DELAY_MS)
      {
        SysTick->LOAD = STEP_DELAY_MS * fac_ms;
        nms -= STEP_DELAY_MS;
      }
      else
      {
        SysTick->LOAD = nms * fac_ms;
        nms = 0;
      }
      SysTick->VAL = 0;
      SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
      do {
        temp = SysTick->CTRL;
      } while ((temp & 0x01) && !(temp & (1 << 16)));
      SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
      SysTick->VAL = 0;
    }
  }
  
  /**
    * @brief  Delay for a specified number of seconds.
    * @param  sec: Delay in seconds.
    * @retval none
    */
  void delay_sec(uint16_t sec)
  {
    for(uint16_t i = 0; i < sec; i++)
    {
      delay_ms(500);
      delay_ms(500);
    }
  }
  
  /**
    * @brief  Main board initialization.
    *         Initialisiert die Delay-Funktion, konfiguriert USART2 (TX auf PA2)
    *         und setzt alle nicht verwendeten GPIOs in den High-Impedanz-Zustand.
    * @retval none
    */
  void at32_board_init()
  {
    /* Delay initialisieren */
    delay_init();
  
    /* UART2 initialisieren (Hello World Test) */
    uart_print_init(115200);  // Baudrate anpassen falls gewünscht
  
    /* Unbenutzte GPIOs als High-Impedanz konfigurieren */
    board_unused_gpio_init();
  }
  