#include "stm32l162xe.h"
void USART1_Send_String (char* str);
void DAC_ConversionSpeedError (void);

//______________________________________________________________________________
void  DAC_IRQHandler (void)    // ���������� �� DAC
{           

    if(DAC->SR & DAC_SR_DMAUDR1)   {    // ��� ���� ����������� DMA. ����� DAC �� ����� ��������� ��������������, � ��������� ������ ��� ������
            DAC->CR &= ~DAC_CR_DMAEN1;  // ��������� DMA, �������� ������� � �� ����� ��� ����������������
            DAC->SR |= DAC_SR_DMAUDR1;  // DAC channel1 DMA underrun flag � ������� ������� �������� ������� �1�
            // �� �� ������� �����. �� �������� �� ���������� DMA
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