/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"

#include "rs485.h"
#include "delay.h"
#include "srf05.h"

#define LED_RUN_PORT    GPIOE
#define LED_RUN_PIN     GPIO_PIN_6
#define LED_RUN_TOGGLE  GPIO_WriteReverse(LED_RUN_PORT, LED_RUN_PIN)

//void delay(unsigned int time)
//{
//  unsigned int i = 0xffff;
//  while(time--)
//  {
//    i = 0xffff;
//    while(i--);
//  }
//}

void main(void)
{
  float distance_cm;
  
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
  
  Delay_Init();
  RS485_Init();
  SRF05_Init();
  
  // LED run
  GPIO_Init(LED_RUN_PORT, LED_RUN_PIN, GPIO_MODE_OUT_PP_HIGH_FAST);
  GPIO_WriteHigh(LED_RUN_PORT, LED_RUN_PIN);
  
  while (1)
  {
//    RS485_SendStr("Hello world.\n");
    LED_RUN_TOGGLE;
    
    distance_cm = SRF05_GetDistance();
    
//    RS485_SendStr("\n");
    RS485_SendChar((char)distance_cm/1 * 10);
//    RS485_SendStr("cm.\n");
    
    Delay(50);
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
