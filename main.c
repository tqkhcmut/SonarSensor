/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"

#include "rs485.h"
#include "delay.h"
#include "srf05.h"

#define LED_RUN_PORT    GPIOE
#define LED_RUN_PIN     GPIO_PIN_6
#define LED_RUN_TOGGLE  GPIO_WriteReverse(LED_RUN_PORT, LED_RUN_PIN)

#define PACKET_BUFFER_SIZE 50

void main(void)
{  
  int packet_len;
  char packet_buff[PACKET_BUFFER_SIZE];
  
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
  
  Delay_Init();
  RS485_Init();
  SRF05_Init();
  SRF05_AutoPoolEnable();
  
  // LED run
  GPIO_Init(LED_RUN_PORT, LED_RUN_PIN, GPIO_MODE_OUT_PP_HIGH_FAST);
  GPIO_WriteHigh(LED_RUN_PORT, LED_RUN_PIN);
    
  while (1)
  {
    LED_RUN_TOGGLE;
    packet_len = RS485_Available();
   if (packet_len > 0)
   {
     RS485_GetData(packet_buff, packet_len);
     
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
