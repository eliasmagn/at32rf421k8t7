/**
  **************************************************************************
  * @file     at32f421_board.h
  * @brief    Header file for board configuration adapted to the XY6020L.
  *           Provides minimal initialization for UART2 (Hello World test)
  *           and sets unused GPIOs to high impedance.
  **************************************************************************
  *                       Copyright notice & Disclaimer
  *
  * The software Board Support Package (BSP) provided by Artery is the copyrighted
  * work of Artery. It is provided "AS IS" without warranties.
  *
  **************************************************************************
  */

  #ifndef __AT32F421_BOARD_H
  #define __AT32F421_BOARD_H
  
  #ifdef __cplusplus
  extern "C" {
  #endif
  
  #include "stdio.h"
  #include "at32f421.h"
  
  /** @addtogroup AT32F421_board
    * @{
    */
  
  /** @defgroup BOARD_exported_functions
    * @{
    */
  
  /* Delay functions */
  void delay_init(void);
  void delay_us(uint32_t nus);
  void delay_ms(uint16_t nms);
  void delay_sec(uint16_t sec);
  
  /* UART initialization function (configured for USART2) */
  void uart_print_init(uint32_t baudrate);
  
  /* Board initialization: configures UART2 and sets unused GPIOs to high impedance */
  void at32_board_init(void);
  
  /* Function to configure unused GPIOs as high impedance */
  void board_unused_gpio_init(void);
  
  /**
    * @}
    */
  
  /** @} */
  
  #ifdef __cplusplus
  }
  #endif
  
  #endif
  