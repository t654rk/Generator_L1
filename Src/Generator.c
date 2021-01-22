#include "stm32l162xe.h"
#include "Sin_DAC_3V.h"    // static volatile uint16_t Sin_DAC [24] = {};

#define APBCLK   32000000UL

// DACoutput = VDDA x DOR / 4096

void DAC_Init(void);
void DMA1_Init(void);
void TIM6_init(void);
void TIM7_init(void);

//______________________________________________________________________________
static uint32_t TIM6_prescalerRegister;
static uint32_t TIM6_autoReloadRegister;

static uint32_t TIM7_autoReloadRegister;
//______________________________________________________________________________
void GenSinus(uint32_t frequency) {
  
      if (frequency <= 50)
      {
	TIM6_prescalerRegister = 32 - 1;
	TIM6_autoReloadRegister =  APBCLK / 32 / frequency / 24;       // 24 - колличество точек синуса
      }
      
      if (frequency > 50)
      {
	TIM6_prescalerRegister = 0;
	TIM6_autoReloadRegister = APBCLK / frequency / 24;       // 24 - колличество точек синуса
      }

    DAC_Init();
    DMA1_Init();
    TIM6_init();
    NVIC_EnableIRQ(DAC_IRQn);
}

//______________________________________________________________________________
void GenTriangle(uint32_t frequency) {
// Внутренний треугольный счетчик увеличивается на три такта APB1 после каждого события запуска.
// Затем значение этого счетчика добавляется в регистр DAC_DHRx без переполнения,
// и сумма сохраняется в регистре DAC_DORx. Счетчик треугольника увеличивается до тех пор,
// пока он меньше максимальной амплитуды, определенной битами MAMPx [3: 0].
// По достижении заданной амплитуды счетчик уменьшается до 0, затем снова увеличивается и так далее.
    TIM7_autoReloadRegister = APBCLK / 4095 / frequency / 2 - 1;
    // Оно, вроде работает, в пределах точности целочисленного деления. Ну не 200 Гц, а 205...
    // Однако получается, что увеличивается на 1 такт APB1 после каждого события запуска.
    // И не меньше максимальной амплитуды (у нас после D1 3661), а значения определеннго битами MAMPx [3: 0]
    TIM7_init();
}
//______________________________________________________________________________
void DAC_Init(void)
{
  GPIOA->MODER |= GPIO_MODER_MODER4;       // 11: Analog mode Чтобы избежать паразитного потребления
  GPIOA->MODER |= GPIO_MODER_MODER5;
  
  RCC->APB1ENR |= RCC_APB1ENR_DACEN;
  
  DAC->CR |= DAC_CR_EN1 | DAC_CR_EN2;
  
  DAC->CR &= ~DAC_CR_BOFF1;
  //  DAC->CR |= DAC_CR_BOFF1;
  DAC->CR &= ~DAC_CR_BOFF2;
  //  DAC->CR |= DAC_CR_BOFF2;
  // Когда буфер отключен, выходное сопротивление канала может достигать 15 кОм.
  // Для получения точности преобразования 1%, сопротивление нагрузки (между DAC_OUT и VSS)
  // должно быть не меньше 1.5 МОм. При включённом буфере требования к нагрузке куда более мягкие,
  // сопротивление нагрузки должно быть от 5 кОм или выше, а ёмкость - не более 50 пФ.
  // С другой стороны, при отключённом буфере на выходе преобразователя можно получить напряжение
  // из полного диапазона 0..VREF+. Буфер же обеспечивает работу только с сигналами в диапазоне 0.2 В..VDDA-0.2 В.
  
    
//  DAC->CR |= DAC_CR_TSEL1;      // Вывод данных по програмному событию DAC->SWTRIGR |= DAC_SWTRIGR_SWTRIG1;    // программное события запуска

  DAC->CR |= DAC_CR_TEN1;       // DAC channel1 Trigger enable
  DAC->CR |= DAC_CR_TEN2;       // DAC channel2 Trigger enable
  
  DAC->CR |= DAC_CR_WAVE2_1;    // 1x: генерация треугольной волны включена
  DAC->CR |= DAC_CR_MAMP2_3 | DAC_CR_MAMP2_1 | DAC_CR_MAMP2_0;  // 1011 - амплитуда LFSR / треугольника, равна 4095
  
  DAC->CR &= ~DAC_CR_TSEL1;     // Timer6 TRGO event TSEL[2:0] = 000. Используется, только если бит TEN1 = 1 (триггер ЦАП 1 включен).
  DAC->CR |= DAC_CR_TSEL2_1;    // Timer7 TRGO event TSEL[2:0] = 010.
  
  DAC->CR |= DAC_CR_DMAEN1;     // DAC channel1 DMA enable
  DAC->CR |= DAC_CR_DMAUDRIE1;  // DAC channel1 DMA Underrun Interrupt enable Разрешение прерывания из-за недогрузки DMA

 }

//______________________________________________________________________________
void DMA1_Init(void)
{
RCC->AHBENR |= RCC_AHBENR_DMA1EN;       // Включили тактирование DMA1

        // Второй DAC_CR_DMAEN2 работает от TIM7 по каналу DMA1_Channel3

        DMA1_Channel2->CCR |= DMA_CCR_MINC;     // Memory increment mode                   Инкрементный режим адреса памяти
        DMA1_Channel2->CCR |= DMA_CCR_MSIZE_0; // будем передавать 16 бит данных
        DMA1_Channel2->CCR |= DMA_CCR_PSIZE_0;
        DMA1_Channel2->CCR |= DMA_CCR_CIRC;    // цыклическая передача  Configure increment, size (32-bits), interrupts, transfer from memory to peripheral and circular mode
        DMA1_Channel2->CCR |= DMA_CCR_DIR;     // чтение с памяти

        DMA1_Channel2->CNDTR = 24;                      // Счетчик массива Configure the number of DMA tranfer to be performs on channel 3
        DMA1_Channel2->CPAR = (uint32_t) &DAC->DHR12R1; // Configure the peripheral data register address
        DMA1_Channel2->CMAR = (uint32_t) Sin_DAC;       // Configure the memory address

        DMA1_Channel2->CCR |= DMA_CCR_EN;      // вкл. ПДП

}

//______________________________________________________________________________
void TIM6_init(void)
{
        RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;             // тактирование таймера

        TIM6->PSC = TIM6_prescalerRegister;             // предделитель
        TIM6->ARR = TIM6_autoReloadRegister;            // переполнение две секунды     

//        TIM6->DIER |= TIM_DIER_UIE;             // прерывание по переполнению все-таки наверно не нужно
        TIM6->DIER |= TIM_DIER_UDE;             // Update DMA request enable Здесь событие, а не прерывание.
        //  Поэтому обработчик не нужен. Само прерывание тоже не включаем
                
        TIM6->CR2 |= TIM_CR2_MMS_1;     // Событие обновления используется как выход триггера
        TIM6->CR1 |= TIM_CR1_CEN;               // запуск счета                 
}

//______________________________________________________________________________
void TIM7_init(void)
{
        RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;             // тактирование таймера

        TIM7->PSC = 0;             // предделитель
        TIM7->ARR = TIM7_autoReloadRegister;            // переполнение две секунды     

//        TIM6->DIER |= TIM_DIER_UIE;             // прерывание по переполнению все-таки наверно не нужно
        TIM7->DIER |= TIM_DIER_UDE;             // Update DMA request enable Здесь событие, а не прерывание.
        //  Поэтому обработчик не нужен. Само прерывание тоже не включаем
                
        TIM7->CR2 |= TIM_CR2_MMS_1;     // Событие обновления используется как выход триггера
        TIM7->CR1 |= TIM_CR1_CEN;               // запуск счета                 
}

//______________________________________________________________________________
void DAC_ConversionSpeedError (void)
{
    DAC->CR &= ~DAC_CR_EN1;             // Вырубает сигнал на выходе
    DMA1_Channel3->CCR &= ~DMA_CCR_EN;
    TIM6->CR1 &= ~TIM_CR1_CEN;
    NVIC_DisableIRQ(DAC_IRQn);
    GPIOC->BSRR |= GPIO_BSRR_BS_8;
}
