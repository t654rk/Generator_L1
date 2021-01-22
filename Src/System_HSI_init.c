#include "stm32l162xe.h"
void SystemInit (void)

// http://we.easyelectronics.ru/_YS_/grabli-stm32l-taktirovanie.html
{
  //Turn ON HSI
  RCC->CR |= RCC_CR_HSION;
  
  //Wait until it's stable 
  while (!(RCC->CR & RCC_CR_HSIRDY));
  
  //Setting up flash for high speed
  FLASH->ACR = FLASH_ACR_ACC64;
  FLASH->ACR |= FLASH_ACR_LATENCY;
  FLASH->ACR |= FLASH_ACR_PRFTEN;
  
  // https://community.st.com/s/question/0D50X00009XkYwWSAV/stm32l-clock-issue
  RCC->APB1ENR |= RCC_APB1ENR_PWREN;              // ????????????
  PWR->CR = PWR_CR_VOS_0;
  while((PWR->CSR & PWR_CSR_VOSF)) {};
        // 5.1.6 Конфигурация динамического масштабирования напряжения
        //  Для программирования диапазонов регулятора напряжения требуется следующая последовательность действий:
        //  1. Проверьте VDD, чтобы определить, какие диапазоны разрешены (см. Рисунок 9: производительность STM32L1xxxx по сравнению с диапазоном VDD и VCORE).
        //  2. Опрос VOSF бит в PWR_CSR. Подождите, пока он не будет сброшен до 0.
        //  3. Настройте диапазон масштабирования напряжения, установив биты VOS [12:11] в регистре PWR_CR.
        //  4. Опросить бит VOSF в регистре PWR_CSR. Подождите, пока он не будет сброшен до 0.
        //  Примечание. Во время настройки масштабирования напряжения системные часы останавливаются до стабилизации регулятора (VOSF = 0).
        //  Это необходимо учитывать при разработке приложения, если требуется критическое время реакции на прерывание,
        //  и в зависимости от используемого периферийного устройства (таймер, связь, ...).
  
  //Switch to HSI as SYSCLK
  //PLL input = HSI
  //PLL division factor = 4
  //PLL multiplication factor = 8
//  RCC->CFGR |= RCC_CFGR_PLLDIV_0 | RCC_CFGR_PLLDIV_1 | RCC_CFGR_PLLMUL_0 | RCC_CFGR_PLLMUL_1;
  RCC->CFGR |= RCC_CFGR_PLLDIV4 | RCC_CFGR_PLLMUL8;
        //00: not allowed
        //01: PLL clock output = PLLVCO / 2
        //10: PLL clock output = PLLVCO / 3
        //11: PLL clock output = PLLVCO / 4
        
        //0000: PLLVCO = PLL clock entry x 3
        //0001: PLLVCO = PLL clock entry x 4
        //0010: PLLVCO = PLL clock entry x 6
        //0011: PLLVCO = PLL clock entry x 8
        //0100: PLLVCO = PLL clock entry x 12
        //0101: PLLVCO = PLL clock entry x 16
        //0110: PLLVCO = PLL clock entry x 24
        //0111: PLLVCO = PLL clock entry x 32
        //1000: PLLVCO = PLL clock entry x 48

  
  //Turn PLL on
  RCC->CR|=RCC_CR_PLLON;
  
  //Wait PLL to stabilize
  while (!(RCC->CR & RCC_CR_PLLRDY));
  
  
  
  //Set PLL as SYSCLK
  RCC->CFGR |= RCC_CFGR_SW_1 | RCC_CFGR_SW_0;
  
  //Turn off MSI
  RCC->CR &= ~RCC_CR_MSION;
}