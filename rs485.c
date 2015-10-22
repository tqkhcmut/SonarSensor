#include "rs485.h"

#define RS485_BUFF_SIZE 128
__IO unsigned char rs485_rx_buff[RS485_BUFF_SIZE];
__IO unsigned char rs485_rx_len; 

void RS485_Init(unsigned long baudrate)
{
  GPIO_Init(RS485_DIR_PORT, RS485_DIR_PIN, GPIO_MODE_OUT_PP_HIGH_FAST);
  RS485_DIR_INPUT;
  
  /* Deinitializes the UART3 peripheral */
  UART3_DeInit();
  
  
  /* Configure the UART3 */
  UART3_Init((uint32_t)baudrate, UART3_WORDLENGTH_8D, UART3_STOPBITS_1, UART3_PARITY_NO,
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
  UART3_ClearITPendingBit(UART3_IT_RXNE);
}


int RS485_Available(void)
{
  return rs485_rx_len;
}
int RS485_GetData(char * buffer)
{
//  memcpy((void *)buffer, (void const *)rs485_rx_buff, len);
//  memset((void *)rs485_rx_buff, 0, RS485_BUFF_SIZE);
  int i;
  for (i = 0; i < rs485_rx_len; i++)
  {
    buffer[i] = rs485_rx_buff[i];
  }
  rs485_rx_len = 0;
  return i;
}

int RS485_SendData(char * buffer, int len)
{
  int i;
  for (i = 0; i < len; i++)
  {
    UART3_SendData8(buffer[i]);
    while (UART3_GetFlagStatus(UART3_FLAG_TXE) == RESET);
  }
  return i;
}

void RS485_Flush(void)
{
	rs485_rx_len = 0;
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


