/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"

#include "rs485.h"
#include "delay.h"
#include "srf05.h"

#include "packet.h"
#include "flash.h"
#include "uart.h"

#include <string.h>

#define LED_RUN_PORT    GPIOE
#define LED_RUN_PIN     GPIO_PIN_6
#define LED_RUN_TOGGLE  GPIO_WriteReverse(LED_RUN_PORT, LED_RUN_PIN)
#define LED_RUN_ON      GPIO_WriteHigh(LED_RUN_PORT, LED_RUN_PIN)
#define LED_RUN_OFF     GPIO_WriteLow(LED_RUN_PORT, LED_RUN_PIN)

#define PACKET_BUFFER_SIZE 50

struct flash_data my_data;
float tmp_distance = 0.0;

void main(void)
{  
  int packet_len;
  char packet_buff[PACKET_BUFFER_SIZE];
  struct Packet * packet;	
  unsigned int tmp_time = Millis();
  
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
  
  disableInterrupts();
  
  Delay_Init();
  RS485_Init();
  UART_Init(115200);
  SRF05_Init();
  SRF05_AutoPoolEnable();
  
  /* Enable general interrupts */
  enableInterrupts();  
  
  //flash_read_buffer((char *)&my_data, sizeof (struct flash_data));
  my_data.id = 0x21;
  UART_SendByte(0x21, HEX);
  //  RS485_SendStr("Hello world.\n");
  
  // LED run
  GPIO_Init(LED_RUN_PORT, LED_RUN_PIN, GPIO_MODE_OUT_PP_HIGH_FAST);
  GPIO_WriteHigh(LED_RUN_PORT, LED_RUN_PIN);
  
  while (1)
  {
    if (Millis() - tmp_time > 1000)
    {
      LED_RUN_TOGGLE;
      tmp_time = Millis();
    }
    
    packet_len = RS485_Available();
    if (packet_len > 2)
    {
      memset(packet_buff, 0, PACKET_BUFFER_SIZE);
      RS485_GetData(packet_buff, packet_len);
      packet = (struct Packet *)packet_buff;
      switch (packet->cmd)
      {
      case CMD_QUERY:
        if (packet->id == my_data.id)
        {
          LED_RUN_TOGGLE;
          
          tmp_distance = SRF05_GetDistance();
          packet->data_type = TYPE_FLOAT | BIG_ENDIAN_BYTE_ORDER;
          memcpy(packet->data, &tmp_distance, getTypeLength(packet->data_type));
          packet->data[getTypeLength(packet->data_type)] = checksum((char *)packet);
          RS485_SendChar(packet->id);
          RS485_SendChar(packet->cmd);
          RS485_SendChar(packet->data_type);
          RS485_SendChar(packet->data[0]);	// float data byte 1
          RS485_SendChar(packet->data[1]);	// float data byte 2
          RS485_SendChar(packet->data[2]);	// float data byte 3
          RS485_SendChar(packet->data[3]);	// float data byte 4
          RS485_SendChar(packet->data[4]);	// checksum byte
        }
        else if (IS_BROADCAST_ID(packet->id))
        {
          LED_RUN_ON;
          // this is for future mode, used to setting id
          // not implemetation jet
        }
        else
        {
          // not own id
        }
        break;
      case CMD_CONTROL:
        break;
      default:
        break;
      }
    }
  }
}


#ifdef USE_FULL_ASSERT

/**
* @brief  Reports the name of the source file and the source line number
*   where the assert_param error has occurred.
* @param file: pointer to the source file name
* @param line: assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  
  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
* @}
*/
