/**
  ******************************************************************************
  * @file    at32f421_clock.h
  * @brief   Prototypen und Definitionen für die System-Clock-Konfiguration.
  *
  *          Die hier definierten Clock-Werte orientieren sich an der Reset-Konfiguration:
  *          Interner 48 MHz Oszillator, geteilt durch 6 → 8 MHz.
  *          Der externe Oszillator (HEXT) wird als 8 MHz angenommen, kommt aber beim xy6020l nicht zum Einsatz.
  ******************************************************************************
  */

  #ifndef __AT32F421_CLOCK_H
  #define __AT32F421_CLOCK_H
  
  #ifdef __cplusplus
   extern "C" {
  #endif
  
  #include "at32f421.h"
  
  /* Clock-Werte (anpassbar an euer Board):
     - HICK_VALUE: Intern 48 MHz geteilt durch 6 → 8 MHz nach Reset
     - HEXT_VALUE: Angenommener externer Oszillatorwert (hier 8 MHz; typischer Bereich 4–25 MHz)
  */
  #define HICK_VALUE ((uint32_t)8000000)
  #define HEXT_VALUE ((uint32_t)8000000)
  
  /**
    * @brief  Konfiguriert die Systemuhr.
    *
    *         Aktiviert den internen Oszillator (HICK) und aktualisiert die SystemCoreClock-Variable.
    *         Da beim xy6020l kein externer Takt genutzt wird, erfolgt hier keine HEXT-Konfiguration.
    */
  void system_clock_config(void);
  
  #ifdef __cplusplus
  }
  #endif
  
  #endif /* __AT32F421_CLOCK_H */
  