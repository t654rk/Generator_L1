/* Includes ------------------------------------------------------------------*/
#include "stm32l162xe.h"
void SystemInit (void)
{     
        RCC->AHBENR     |= RCC_AHBENR_GPIOHEN;                     // enable clock for port H
        
        
        if ((RCC->CFGR & RCC_CFGR_SWS) == RCC_CFGR_SWS_PLL)     // #define  RCC_CFGR_SWS_PLL    ((uint32_t)0x00000008)   /*!< PLL used as system clock */
        //���� 3: 2 SWS [1: 0]: ��������� ������������� ��������� �����
        //00: ��������� HSI ������������ � �������� ��������� �����
        //01: ��������� HSE ������������ � �������� ��������� �����
        //10: PLL ������������ � �������� ��������� �����
        //11: ��������� HSI48, ������������ � �������� ��������� ����� (��� �������)
    	{ 
    	 RCC->CFGR &= (uint32_t) (~RCC_CFGR_SW);        // #define  RCC_CFGR_SW ((uint32_t)0x00000003)  /*!< SW[1:0] bits (System clock Switch) */
        //���� 1: 0 SW [1: 0]: ������������� ��������� �����
        // ������������ ���������� ������������ ��� ��������������� ������ HSI ��� ������
        // �� ������ ��������� � �������� ��� � ������ ���� ���������� HSE,
        // ������������� ����� ��� �������� � �������� ��������� ����� (���� �������� ������� ����������� ������������ �����).
        //00: HSI ������ � �������� ��������� �����
        //01: HSE ������ � �������� ��������� �����
        //10: PLL ������ � �������� ��������� �����
        //11: HSI48 ������ � �������� ��������� ����� (���� ��������)
    	 while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI) {}; // ���� �� ��������� HSI
    	}
        
    	RCC->CR |= ((uint32_t)RCC_CR_HSEON);
        // HSEON: ��������� ����� HSE
        // ������������ ��������� ��� ��������� ���������� HSE ��� �������� � ����� ��������� ��� ��������.
        // ���� ��� �� ����� ���� �������, ���� ��������� HSE ����� ��� �������� ������������ � �������� ��������� �����.
        //0: ��������� HSE ��������
        //1: ��������� HSE �������
    	while(!(RCC->CR & RCC_CR_HSERDY));
        //��� 17 HSERDY: ���� ���������� ����� HSE
        // ��������������� ���������, ����� �������, ��� ��������� HSE ��������. ���� ��� ������� 6 ������
        // ��������� ���������� HSE ��� ������ ����� ������ HSEON.
        //0: ��������� HSE �� �����
        //1: ��������� HSE �����
        
        FLASH->ACR = FLASH_ACR_ACC64;
        FLASH->ACR |= FLASH_ACR_PRFTEN | FLASH_ACR_LATENCY;
        //��� 4 PRFTBE: ��������� ������ ��������������� �������
        //0: ��������������� ������� ���������
        //1: ��������������� ������� ��������
        //���� 2: 0 LATENCY [2: 0]: ��������
        // ��� ���� ������������ ��������� ������� SYSCLK (��������� ����) � ������� ������� � Flash.
        //000: ������� ��������� ��������, ���� SYSCLK <= 24 ���
        //001: ���� ��������� ��������, ���� 24 ��� <SYSCLK <= 48 ���
        
        // https://community.st.com/s/question/0D50X00009XkYwWSAV/stm32l-clock-issue
        RCC->APB1ENR |= RCC_APB1ENR_PWREN;
        // ����� ������������ ���� �� �������, �������� ��������� ������������ ��������� ����� �� �������� ����������� ������������, � ������������ �������� ������ ����� 0x0.
        //PWREN: �������� ��������� ������� �������
        //���� ��� ��������������� � ������������ ����������� ������������.
        //0: ���� ���������� ������� ���������
        //1: ��������� ������������� ���������� �������
        RCC->APB1ENR |= RCC_APB1ENR_COMPEN;
        RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
        
        PWR->CR = PWR_CR_VOS_0;                 // __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
        while((PWR->CSR & PWR_CSR_VOSF)) {};    // #define PWR_REGULATOR_VOLTAGE_SCALE1       PWR_CR_VOS_0
        // 5.1.6 ������������ ������������� ��������������� ����������
        //  ��� ���������������� ���������� ���������� ���������� ��������� ��������� ������������������ ��������:
        //  1. ��������� VDD, ����� ����������, ����� ��������� ��������� (��. ������� 9: ������������������ STM32L1xxxx �� ��������� � ���������� VDD � VCORE).
        //  2. ����� VOSF ��� � PWR_CSR. ���������, ���� �� �� ����� ������� �� 0.
        //  3. ��������� �������� ��������������� ����������, ��������� ���� VOS [12:11] � �������� PWR_CR.
        //  4. �������� ��� VOSF � �������� PWR_CSR. ���������, ���� �� �� ����� ������� �� 0.
        //  ����������. �� ����� ��������� ��������������� ���������� ��������� ���� ��������������� �� ������������ ���������� (VOSF = 0).
        //  ��� ���������� ��������� ��� ���������� ����������, ���� ��������� ����������� ����� ������� �� ����������,
        //  � � ����������� �� ������������� ������������� ���������� (������, �����, ...).
        
        // ���� ��� � ����� ������ RCC_CFGR_HPRE_DIV1 = RCC_CFGR_PPRE1_DIV1 = RCC_CFGR_PPRE2_DIV1 = 0.
        RCC->CFGR |= RCC_CFGR_HPRE_DIV1;        // AHB = SYSCLK/1
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV1;       // APB1 = HCLK/1
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;       // APB2 = HCLK/1
        
        RCC->CR &= (uint32_t)(~RCC_CR_PLLON);
        //��� 24 PLLON: ��������� PLL
        //������������ ��������� ��� �������� � ����� ��������� ��� ��������. ���� ��� �� ����� ���� �������,
        //���� ���� PLL ������������ � �������� ��������� ����� ��� �������, ����� ����� ���������� ������.
        //0: PLL OFF
        //1: PLL ON
        
    	while(RCC->CR & RCC_CR_PLLRDY) {};
        //��� 25 PLLRDY: ���� ���������� ����� PLL
        // ���������� ���������, ����� �������, ��� PLL ������������.
        //0: PLL ��������������
        //1: PLL ������������
        
        //PLL input = HSE = 8 ���
    	RCC->CFGR |= RCC_CFGR_PLLSRC_HSE;
        RCC->CFGR |= RCC_CFGR_PLLDIV3 | RCC_CFGR_PLLMUL12;      // ����� �� ����� 48 ��� �� USB
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
        RCC->CR |= RCC_CR_PLLON;
        //Wait PLL to stabilize
        while (!(RCC->CR & RCC_CR_PLLRDY));

        
    	
    	
        //Set PLL as SYSCLK
    	RCC->CFGR |= RCC_CFGR_SW_PLL | RCC_CFGR_SWS_PLL;
        while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {};
        
        //Turn off MSI
//    	RCC->CR&=~RCC_CR_MSION; // Internal Multi Speed clock enable
    	
}

// 6.3.3 ������� ������������ ����� (RCC_CFGR)     (RM0038, pg. 141)
// �������� ������: 0x08
// �������� ��������: 0x0000 0000
// ������: 0 <= ��������� �������� <= 2, ������ � �����, ��������� � ����� 1 ��� 2 ��������� �������� �����������, ������ ���� ������ ���������� �� ����� ������������ ��������� �������������.

// ���� 31 ���������������, ������ ���� ��������� � �������� ������.

// ���� 30:28 MCOPRE [2:0]: ���������� ��������� ������ ����������������
// ��� ���� ��������������� � ��������� ����������� ������������.
// ������������ ������������� �������� ���� ��������� �� ����, ��� ����� ������� ����� MCO.
      //000: MCO ������� �� 1
      //001: MCO ������� �� 2
      //010: MCO ������� �� 4
      //011: MCO ������� �� 8
      //100: MCO ������� �� 16
      //������: �� �����������

// ���� 27 ���������������, ������ ���� ��������� ��� �������� ������.

// ���� 26:24 MCOSEL [2:0]: ����� ��������� ������ ����������������
// ��� ���� ��������������� � ��������� ����������� ������������.
      //000: ����� MCO ��������, ��� ����� �� MCO
      //001: ������� ���� SYSCLK
      //010: ������� �������� �������� HSI
      //011: ������� �������� �������� MSI
      //100: ������� ���� ���������� HSE
      //101: ������� ���� PLL
      //110: ������� �������� �������� LSI
      //111: ������� �������� �������� LSE
// ����������. ���� �������� ����� ����� ����� ��������� ��������� ������ ��� ������� ��� �� ����� ������������ ��������� ������������� MCO.

// ���� 23:22 PLLDIV [1:0]: ������� �������� �������� ����
// ��� ���� ��������������� � ��������� ����������� ������������ ��� ���������� �������� �������� ������� �������� �������� ���� �� �������� ������� ����.
// ��� ���� ����� ���� �������� ������ ����� PLL ��������.
      //00: �� �����������
      //01: �������� ������ PLL = PLLVCO / 2
      //10: �������� ������ PLL = PLLVCO / 3
      //11: �������� ������ PLL = PLLVCO / 4

// ���� 21:18 PLLMUL [3:0]: ����������� ��������� ����
// ��� ���� ������������ ����������� ������������ ��� ����������� ������������ ��������� ���� ��� ��������� ��������� ������� ���� ����.
// ��� ���� ����� ���� �������� ������ ����� PLL ��������.
      //0000: �������� ������ PLL = PLLVCO � 3
      //0001: �������� ������ PLL = PLLVCO x 4
      //0010: �������� ������ PLL = PLLVCO x 6
      //0011: �������� ������ PLL = PLLVCO x 8
      //0100: �������� ������ PLL = PLLVCO x 12
      //0101: �������� ������ PLL = PLLVCO x 16
      //0110: �������� ������ PLL = PLLVCO x 24
      //0111: �������� ������ PLL = PLLVCO x 32
      //1000: �������� ������ PLL = PLLVCO x 48
      //������: �� �����������
// ��������: �������� ������� PLL VCO �� ������ ��������� 96 ���, ����� ������� ��������� � ��������� 1,
// 48 ���, ����� ������� ��������� � ��������� 2, � 24 ���, ����� ������� ��������� � ��������� 3.

// ��� 17 ��������������, ���������� ��������� �������� ������.

// ��� 16 PLLSRC: �������� ������������� ����� PLL
// ���� ��� ��������������� � ��������� ����������� ������������ ��� ������ ��������� ������������� PLL. ���� ��� ����� ���� ������� ������ ����� PLL ��������.
      //0: �������� ��������� HSI ������ � �������� �������� ��������� ������� ����
      //1: �������� ��������� HSE ������ � �������� �������� ��������� ������� ����
// ����������. ����������� ������� �������� ������� ���� ���������� 2 ���.

// ���� 15:14 ���������������, ������ ���� ��������� �� ������ ������.

// ���� 13:11 PPRE2 [2:0]: ���������������� ��������� APB (APB2)
// ��� ���� ��������������� � ��������� ����������� ������������ ��� ���������� ������������� ������� ����������������� ��������� ������� APB (PCLK2).
      //0xx: HCLK �� �������
      //100: HCLK ������� �� 2
      //101: HCLK ������� �� 4
      //110: HCLK ������� �� 8
      //111: HCLK ������� �� 16
// ���� 10:8 PPRE1 [2:0]: ��������������� ��������� APB (APB1)
// ��� ���� ��������������� � ��������� ����������� ������������ ��� ���������� ������������� ������� ��������� ������� ������ �������� APB (PCLK1).
      //0xx: HCLK �� �������
      //100: HCLK ������� �� 2
      //101: HCLK ������� �� 4
      //110: HCLK ������� �� 8
      //111: HCLK ������� �� 16

// ���� 7:4 HPRE [3:0]: ��������� AHB
// ��� ���� ��������������� � ��������� ����������� ������������ ��� ���������� ������������� ������� ��������� ������� AHB.
// ��������: � ����������� �� ��������� ���������� ���������� ����������� ����������� ������ ��������� ���������� ��� ����,
// ����� ������� ������� �� ��������� ����������� ���������� ������� (����� ��������� ���������� ��. � ������� ����������� ������������ ���������������� ����������� � ����� PWR.)
// ����� �������� ������ � ��� ���� � ����� ����������� ��������� ���������� ���������� ��������� ���� �������, ����� ���������, ��� ����� �������� ���� ������.
      //0xxx: SYSCLK �� �������
      //1000: SYSCLK ������� �� 2
      //1001: SYSCLK ������� �� 4
      //1010: SYSCLK ������� �� 8
      //1011: SYSCLK ������� �� 16
      //1100: SYSCLK ������� �� 64
      //1101: SYSCLK ������� �� 128
      //1110: SYSCLK ������� �� 256
      //1111: SYSCLK ������� �� 512

// ���� 3:2 SWS [1:0]: ��������� ������������� ��������� �����
// ��� ���� ��������������� � ��������� ���������, ����� �������, ����� �������� ������������� ������������ � �������� ��������� �����.
      //00: MSI ��������� ������������ � �������� ��������� �����
      //01: HSI ��������� ������������ � �������� ��������� �����
      //10: HSE ���������, ������������ � �������� ��������� �����
      //11: PLL ������������ � �������� ��������� �����

// ���� 1:0 SW [1:0]: ������������� ��������� �����
// ��� ���� ��������������� � ��������� ����������� ������������ ��� ������ ��������� SYSCLK.
// ��������������� ��������� ��� ��������������� ������ MSI ��� ������ �� ������� ����� � ��������� ��� � ������ ������ ���������� HSE,
// ������������� ����� ��� �������� � �������� ��������� ����� (���� �������� ������� ����������� ������������ �����).
      //00: MSI ��������� ������������ � �������� ��������� �����
      //01: HSI ��������� ������������ � �������� ��������� �����
      //10: HSE ���������, ������������ � �������� ��������� �����
      //11: PLL ������������ � �������� ��������� �����

// 7.3.14 ������������� ������� OSC_IN / OSC_OUT � �������� ������� ����� GPIO PH0 / PH1
// ������ ���������� HSE OSC_IN / OSC_OUT ����� �������������� ��� ������������� ����� / ������ PH0 / PH1, ��������������, ����� ��������� HSE ��������.
// (����� ������ ��������� HSE ��������). ����� / ������ PH0 / PH1 ��������������� ��� ����������� HSE OSC_IN / OSC_OUT, ����� ��������� HSE �������.
// ��� �������� ����� ��������� ���� HSEON � �������� RCC_CR. HSE ����� ��������� ��� �������� GPIO.
    	