#include "stm32l162xe.h"
void USART1_Send_String (char* str);
void DAC_ConversionSpeedError (void);

//______________________________________________________________________________
void  DAC_IRQHandler (void)    // Прерывание от DAC
{           

    if(DAC->SR & DAC_SR_DMAUDR1)   {    // Это флаг опустошения DMA. Когда DAC не успел завершить преобразование, а следующий запрос уже пришел
            DAC->CR &= ~DAC_CR_DMAEN1;  // Выключить DMA, понизить частоту и по новой все инициализировать
            DAC->SR |= DAC_SR_DMAUDR1;  // DAC channel1 DMA underrun flag в мануале сказано очистить написав «1»
            // Но не сказано когда. Не чистится до отключения DMA
            DAC_ConversionSpeedError ();
  }
}

//______________________________________________________________________________
void USART1_IRQHandler (void){
	
	if (USART1->SR & USART_SR_RXNE)
        {
		
		if (USART1->DR == '0'){
			
			USART1_Send_String ("OFF\r\n");
			GPIOC->BSRR |= GPIO_BSRR_BR_8;
			
		}
		
		if (USART1->DR == '1'){
			
			USART1_Send_String ("ON\r\n");
			GPIOC->BSRR |= GPIO_BSRR_BS_8;
			
		}
		
	}
	
}