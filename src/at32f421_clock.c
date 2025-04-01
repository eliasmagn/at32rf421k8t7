/**
  **************************************************************************
  * @file     at32f421_clock.c
  * @brief    system clock config program
  **************************************************************************
  *                       Copyright notice & Disclaimer
  *
  * The software Board Support Package (BSP) that is made available to
  * download from Artery official website is the copyrighted work of Artery.
  * Artery authorizes customers to use, copy, and distribute the BSP
  * software and its related documentation for the purpose of design and
  * development in conjunction with Artery microcontrollers. Use of the
  * software is governed by this copyright notice and the following disclaimer.
  *
  * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
  * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
  * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
  * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
  * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
  *
  **************************************************************************
  */

/* includes ------------------------------------------------------------------*/
#include "at32f421_clock.h"

/**
  * @brief  system clock config program
  * @note   the system clock is configured as follow:
  *         system clock (sclk)   = hext * pll_mult
  *         system clock source   = pll (hext)
  *         - hext                = HEXT_VALUE
  *         - sclk                = 120000000
  *         - ahbdiv              = 1
  *         - ahbclk              = 120000000
  *         - apb2div             = 1
  *         - apb2clk             = 120000000
  *         - apb1div             = 1
  *         - apb1clk             = 120000000
  *         - pll_mult            = 15
  *         - flash_wtcyc         = 3 cycle
  * @param  none
  * @retval none
  */
void system_clock_config(void)
{
  /* Reset des CRM */
  crm_reset();

  /* Konfiguriere Flash PSR Register */
  flash_psr_set(FLASH_WAIT_CYCLE_3);

  /* Aktiviere LICK */
  crm_clock_source_enable(CRM_CLOCK_SOURCE_LICK, TRUE);
  /* Warte bis LICK stabil ist */
  while(crm_flag_get(CRM_LICK_STABLE_FLAG) != SET)
  {
  }

  /* Aktiviere HICK */
  crm_clock_source_enable(CRM_CLOCK_SOURCE_HICK, TRUE);
  /* Warte bis HICK stabil ist */
  while(crm_flag_get(CRM_HICK_STABLE_FLAG) != SET)
  {
  }

  /* Konfiguriere PLL: Quelle = HICK, Multiplikator = 30 */
  crm_pll_config(CRM_PLL_SOURCE_HICK, CRM_PLL_MULT_30);

  /* Aktiviere PLL */
  crm_clock_source_enable(CRM_CLOCK_SOURCE_PLL, TRUE);
  /* Warte bis PLL stabil ist */
  while(crm_flag_get(CRM_PLL_STABLE_FLAG) != SET)
  {
  }

  /* Konfiguriere AHB-Clock (Divider = 1) */
  crm_ahb_div_set(CRM_AHB_DIV_1);

  /* Konfiguriere APB2-Clock (Divider = 1) */
  crm_apb2_div_set(CRM_APB2_DIV_1);

  /* Konfiguriere APB1-Clock (Divider = 1) */
  crm_apb1_div_set(CRM_APB1_DIV_1);

  /* Aktiviere Auto Step Mode */
  crm_auto_step_mode_enable(TRUE);

  /* Wähle PLL als Systemtaktquelle */
  crm_sysclk_switch(CRM_SCLK_PLL);
  /* Warte, bis PLL als Systemtaktquelle verwendet wird */
  while(crm_sysclk_switch_status_get() != CRM_SCLK_PLL)
  {
  }

  /* Deaktiviere Auto Step Mode */
  crm_auto_step_mode_enable(FALSE);

  /* Aktualisiere die globale Variable system_core_clock */
  system_core_clock_update();
}
