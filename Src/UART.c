#include "stm32f051x8.h"
#include "USART.h"
#include <string.h>


extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

extern __IO uint8_t USART1_RX_Flag;
extern __IO uint8_t USART2_RX_Flag;

__IO uint8_t win_in_usart_symbol;
__IO uint8_t gsm_in_usart_symbol;

__IO usart_ring usart_ring_win;
__IO usart_ring usart_ring_gsm;



//------------------------------------------------------------------------------
void RING_Clear(usart_ring __IO * buf)
{
    buf->idxIN = 0;
    buf->idxOUT = 0;
}
//------------------------------------------------------------------------------
uint8_t RING_GetCount(usart_ring __IO * buf)
{
    return (buf->idxIN - buf->idxOUT)&BUF_MASK;
}
//------------------------------------------------------------------------------
void RING_Put(__IO uint8_t value, usart_ring __IO * buf)
{
    buf->cycleBuf[buf->idxIN++] = value;
    buf->idxIN &= BUF_MASK;
}
//------------------------------------------------------------------------------
uint8_t RING_Get(usart_ring __IO * buf)
{
    uint8_t value = buf->cycleBuf[buf->idxOUT++];
    buf->idxOUT &= BUF_MASK;
    return value;
}
//------------------------------------------------------------------------------
void RING_Read(usart_ring __IO * buf, char *data, uint8_t len)
{
    for(uint8_t i = 0; i < len; i++)
    {
        data[i] = buf->cycleBuf[buf->idxOUT++];
        buf->idxOUT &= BUF_MASK;
    }
}
//------------------------------------------------------------------------------
void RING_Write(char *data, usart_ring __IO * buf, uint8_t len)
{
    for(uint8_t i = 0; i < len; i++)
    {
        buf->cycleBuf[buf->idxIN++] = data[i];
        buf->idxOUT &= BUF_MASK;
    }
}
//------------------------------------------------------------------------------
void WIN_RxCallBack(void)
{
//  RING_Put(win_in_usart_symbol, &usart_ring_win);
  usart_ring_win.cycleBuf[usart_ring_win.idxIN++] = win_in_usart_symbol;
  usart_ring_win.idxIN &= BUF_MASK;
  if(win_in_usart_symbol == '\r')
	{
//                RING_Put('\n', &usart_ring_win);
                usart_ring_win.cycleBuf[usart_ring_win.idxIN++] = '\n';
                usart_ring_win.idxIN &= BUF_MASK;
                USART1_RX_Flag = 1;
	}
  HAL_UART_Receive_IT(&huart1, (uint8_t*)&win_in_usart_symbol, 1);  
}

//------------------------------------------------------------------------------
void GSM_RxCallBack(void)
{
//  RING_Put(gsm_in_usart_symbol, &usart_ring_gsm);
  usart_ring_gsm.cycleBuf[usart_ring_gsm.idxIN++] = gsm_in_usart_symbol;
  usart_ring_gsm.idxIN &= BUF_MASK;
  if(gsm_in_usart_symbol == '\n')
	{
                USART2_RX_Flag = 1;
	}
  HAL_UART_Receive_IT(&huart2, (uint8_t*)&gsm_in_usart_symbol, 1);  
}

/*
//////////////// проверка и установка скорости 19200, нужна один раз /////////////////
void chek_speed(void)
{
  for(uint8_t i = 0; i < 7; i++)
  {
	  uint32_t sp = 0;

	  if(i == 0) sp = 2400;
	  else if(i == 1) sp = 4800;
	  else if(i == 2) sp = 9600;
	  else if(i == 3) sp = 19200;
	  else if(i == 4) sp = 38400;
	  else if(i == 5) sp = 57600;
	  else if(i == 6) sp = 115200;

	  huart2.Instance = USART2;
	  huart2.Init.BaudRate = sp;
	  huart2.Init.WordLength = UART_WORDLENGTH_8B;
	  huart2.Init.StopBits = UART_STOPBITS_1;
	  huart2.Init.Parity = UART_PARITY_NONE;
	  huart2.Init.Mode = UART_MODE_TX_RX;
	  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	  huart2.Init.OverSampling = UART_OVERSAMPLING_16;

	  if (HAL_UART_Init(&huart2) != HAL_OK)
	  {
		  Error_Handler();
	  }

	  char str[16] = {0,};
	  HAL_UART_Transmit(&huart2, (uint8_t*)"AT\r\n", strlen("AT\r\n"), 1000);
	  HAL_Delay(300);

	  if(gsm_available()) //если модуль что-то прислал
	  {
		  uint16_t i = 0;

		  while(gsm_available())
		  {
			  str[i++] = gsm_read();
			  if(i > 15) break;
			  HAL_Delay(1);
		  }

		  if(strstr(str, "OK") != NULL)
		  {
			  char buf[64] = {0,};
			  snprintf(buf, 64, "Uart modem was %lu, switched to 57600\n", huart1.Init.BaudRate);
			  HAL_UART_Transmit(&huart1, (uint8_t*)win10.usart_buf, strlen(win10.usart_bufz), 100);
			  HAL_UART_Transmit(&huart2, (uint8_t*)"AT+IPR=57600\r\n", strlen("AT+IPR=57600\r\n"), 1000);
			  HAL_Delay(250);
			  MX_USART2_UART_Init();
			  break;
		  }
	  }
  }
}*/