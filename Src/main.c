#include "stm32l162xe.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


#include "main.h"

#define APBCLK   32000000UL
#define BAUDRATE 9600UL

//void MCO_out (void);
void GenSinus(uint32_t frequency);
void GenTriangle(uint32_t frequency);
void GPIO_Init (void);
void USART1_Init(void);
void USART1_Send (char chr);
void USART1_Send_String (char* str);

void vTaskLed1 (void *argument);
void vTaskLed2 (void *argument);



int main(void){				

//    MCO_out();
    GPIO_Init();
//    USART1_Init();
	
	GPIOC->ODR ^= GPIO_ODR_ODR_9;               // PC9 OUT LED INVERT
        
        GenSinus(167);
        GenTriangle(167);
        
//        USART1_Send_String ("STM32l162RE USART1 debugging CMSYS\r\n");
//        USART1->CR1 |= USART_CR1_RXNEIE;                // RXNE Int ON	
//	NVIC_EnableIRQ(USART1_IRQn);
	
    xTaskCreate(vTaskLed1, "LED1", 32, NULL, 1, NULL);
    xTaskCreate(vTaskLed2, "LED2", 32, NULL, 1, NULL);
	
    vTaskStartScheduler();
    
	while(1)
	{
		
	}
	
}

//______________________________________________________________________________
void GPIO_Init (void){
	
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    
	// Pin 8 Синенький светодиод Дискавери
	GPIOC->MODER         |= GPIO_MODER_MODER8_0;       // 01 (GPIO_MODER_MODER0_0) - выход
	GPIOC->PUPDR         |= GPIO_PUPDR_PUPDR8_1;       // подтяжка к "-"
	GPIOC->OSPEEDR       |= GPIO_OSPEEDER_OSPEEDR8;    // 50МГц
    
	// Pin 9 Зелененький светодиод Дискавери
	GPIOC->MODER         |= GPIO_MODER_MODER9_0;       // 01 (GPIO_MODER_MODER0_0) - выход
	GPIOC->PUPDR         |= GPIO_PUPDR_PUPDR9_1;       // подтяжка к "-"
	GPIOC->OSPEEDR       |= GPIO_OSPEEDER_OSPEEDR9;    // 50МГц
}
//______________________________________________________________________________
void USART1_Init(void){
	
        RCC->AHBENR |= RCC_AHBENR_GPIOBEN;              // GPIO port A clock enable
        RCC->AHBENR |= RCC_AHBLPENR_GPIOBLPEN;          // GPIO port A clock enabled in sleep mode
        
        GPIOB->OTYPER &= ~GPIO_OTYPER_OT_6;             // Output push-pull (reset state)
        GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR6;             // No pull-up, pull-down
        GPIOB->MODER |= GPIO_MODER_MODER6_1;            // Alternate function mode
        GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR6_1;     // 40 MHz High speed
        
        GPIOB->MODER |= GPIO_MODER_MODER7_1;           // Alternate function mode
        GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR7_1;    // 40 MHz High speed
        
//        GPIOB->AFR[0]   |= 0x7 << 24;                   // USART1_TX
//        GPIOB->AFR[0]   |= 0x7 << 28;                   // USART1_RX
        GPIOB->AFR[0]   |= 0x77000000;                       // И даже вот так! )))
        
        
        RCC->APB2ENR|= RCC_APB2ENR_USART1EN;
	
	USART1->BRR =(APBCLK+BAUDRATE/2)/BAUDRATE;      // Скорость usart
//        USART1->BRR = 0xD05;
	
	USART1->CR1 |= USART_CR1_TE;                    // USART1 ON, TX ON, RX ON
	USART1->CR1 |= USART_CR1_RE;
	USART1->CR1 |= USART_CR1_UE;
        
        // Все прерывания перенесены в Main
	
//	USART1->CR1 |= USART_CR1_RXNEIE;                // RXNE Int ON	
//	NVIC_EnableIRQ(USART1_IRQn);
	
}
//______________________________________________________________________________
void USART1_Send (char chr){
	
	while (!(USART1->SR & USART_CR1_TCIE));
	USART1->DR = chr;
	
}
//______________________________________________________________________________
void USART1_Send_String (char* str){
	
	uint8_t i = 0;
	
	while(str[i])
	USART1_Send (str[i++]);
	
}
//______________________________________________________________________________
void vTaskLed1 (void *argument){
	
	while(1)
	{
		// Pin 8 Синенький светодиод Дискавери
//		GPIOC->ODR ^= GPIO_ODR_ODR_8;
                GPIOC->BSRR |= GPIO_BSRR_BS_8;
                vTaskDelay(1000);
                GPIOC->BSRR |= GPIO_BSRR_BR_8;
                vTaskDelay(1000);
		
	}
	
}
//______________________________________________________________________________
void vTaskLed2 (void *argument){
	
	while(1)
	{
                // Pin 9 Зелененький светодиод Дискавери
		GPIOC->ODR ^= GPIO_ODR_ODR_9;
		vTaskDelay(1000);
				
	}
	
}


