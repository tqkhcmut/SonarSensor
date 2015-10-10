#include "rs485.h"
#include <string.h>

#define RS485_BUFF_SIZE 50
unsigned char rs485_rx_buff[RS485_BUFF_SIZE];
unsigned char rs485_rx_len; 

void RS485_Init(void)
{
  /* Deinitializes the UART3 peripheral */
  UART3_DeInit();
  
//  GPIO_Init(GPIOD, GPIO_PIN_6, GPIO_MODE_OUT_PP_HIGH_FAST);
//  GPIO_Init(GPIOD, GPIO_PIN_6, GPIO_MODE_IN_FL_NO_IT);
  
  /* Configure the UART3 */
  UART3_Init((uint32_t)115200, UART3_WORDLENGTH_8D, UART3_STOPBITS_1, UART3_PARITY_NO,
             UART3_MODE_TXRX_ENABLE);
  
  /* Enable UART3 Receive interrupt */
  UART3_ITConfig(UART3_IT_RXNE_OR, ENABLE);
  
  /* Enable general interrupts */
  enableInterrupts();    
  
}

INTERRUPT_HANDLER(UART3_RX_IRQHandler, 21)
{
  /* Read one byte from the receive data register */
  rs485_rx_buff[rs485_rx_len++] = UART3_ReceiveData8();
  
  
  if (rs485_rx_len >= RS485_BUFF_SIZE)
  {
    rs485_rx_len = 0;
  }
}


int RS485_Available(void)
{
  return rs485_rx_len;
}
int RS485_GetData(char * buffer, int len)
{
  memcpy(buffer, rs485_rx_buff, len);
  memset(rs485_rx_buff, 0, RS485_BUFF_SIZE);
  rs485_rx_len = 0;
  return len;
}

void RS485_SendChar(char c)
{
  UART3_SendData8(c);
  while (UART3_GetFlagStatus(UART3_FLAG_TXE) == RESET);
}

void RS485_SendStr(char Str[])
{
  while(*Str)
  {
    UART3_SendData8(*Str++);
    while (UART3_GetFlagStatus(UART3_FLAG_TXE) == RESET);
  }
}

void RS485_SendNum(int num)
{
  unsigned long tmp = 10000000;
  if (num == 0)
  {
    RS485_SendChar('0');
    return;
  }
  if (num < 0)
  {
    RS485_SendChar('-');
    num = -num;
  }
  while (tmp > 0)
  {
    if (tmp <= num)
    {
      RS485_SendChar((num/tmp)%10 + '0');
    }
    tmp = tmp / 10;
  }
}

void RS485_SendFloat(float num)
{
  int __int = (int) num;
  RS485_SendNum(__int);
  RS485_SendChar('.');
  __int = (int)((num-__int)*100);
  if (__int < 0)
    __int = 0;
  RS485_SendNum(__int);
}

void RS485_SendByte(uint8_t b, BYTE_FORMAT f)
{
  uint8_t bitMask = 1;
  uint8_t i;
  switch (f)
  {
  case BIN:
    for (i = 8; i > 0; i--)
    {
      RS485_SendChar((b&(bitMask << i)) ? '1' : '0');
    }
    break;
  case OCT:
    break;
  case DEC:
    RS485_SendNum(b);
    break;
  case HEX:
    if(b/16 < 10){
      RS485_SendChar(0x30 + b/16);
    }else{
      RS485_SendChar(0x37 + b/16);
    }
    
    if(b%16 < 10){
      RS485_SendChar(0x30 + b%16);
    }else{
      RS485_SendChar(0x37 + b%16);
    }
    break;
  default:
    break;
  }
}


