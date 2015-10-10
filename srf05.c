#include "srf05.h"
#include "delay.h"
#include "rs485.h"

#define TRIGER_PORT     GPIOC
#define TRIGER_PIN      GPIO_PIN_6
#define ECHO_PORT       GPIOD
#define ECHO_PIN        GPIO_PIN_0

__IO unsigned long counter, count_down;
__IO unsigned long capture_start, capture_stop;
__IO float distance = 0.0;
__IO unsigned char auto_poll = 0;

float DistanceCalculation(unsigned long duration)
{
  // We have 10us per tick
  float time = duration * 10; // in microseconds
  return time / 58; // result in centimet
}

int SRF05_Init(void)
{
  GPIO_Init(TRIGER_PORT, TRIGER_PIN, GPIO_MODE_OUT_PP_HIGH_FAST);
  GPIO_WriteLow(TRIGER_PORT, TRIGER_PIN);
  GPIO_Init(GPIOC, GPIO_PIN_7, GPIO_MODE_IN_FL_NO_IT);
  GPIO_Init(ECHO_PORT, ECHO_PIN, GPIO_MODE_IN_FL_NO_IT);
  
  /* TIM3 configuration:
   - TIM3CLK is set to 16 MHz, the TIM3 Prescaler is equal to 128 so the TIM1 counter
   clock used is 16 MHz / 8 = 2 000 000 Hz
  - With 2 000 000 Hz we can generate time base:
      max time base is 2.048 ms if TIM3_PERIOD = 65535 --> (65535 + 1) / 2000000 = 32.768 ms
      min time base is 0.016 ms if TIM3_PERIOD = 1   --> (  1 + 1) / 2000000 = 0.001 ms = 1us
  - In this example we need to generate a time base equal to 10 us
   so TIM3_PERIOD = (0.00001 * 2000000 - 1) = 19 */

  /* Time base configuration */
  TIM3_TimeBaseInit(TIM3_PRESCALER_8, 19);
  /* Clear TIM3 update flag */
  TIM3_ClearFlag(TIM3_FLAG_UPDATE);
  /* Enable update interrupt */
  TIM3_ITConfig(TIM3_IT_UPDATE, ENABLE);
  
  /* enable interrupts */
  enableInterrupts();

  /* Enable TIM3 */
  TIM3_Cmd(ENABLE);
  
  EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOD, EXTI_SENSITIVITY_RISE_FALL);
  
  capture_start = 0;
  capture_stop = 0;
  return 0;
}
int SRF05_AutoPoolEnable(void)
{
  auto_poll = 1;
  return 0;
}
int SRF05_AutoPoolDisable(void)
{
  auto_poll = 0;
  return 1;
}
float SRF05_GetDistance(void)
{  
  return distance;
}

/**
  * @brief Timer3 Update/Overflow/Break Interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(TIM3_UPD_OVF_BRK_IRQHandler, 15)
{
  counter++;
  if (auto_poll)
  {
    if (count_down == 32)
    {
      GPIO_WriteHigh(TRIGER_PORT, TRIGER_PIN);
    }
    else if (count_down == 30)
    {
      GPIO_WriteLow(TRIGER_PORT, TRIGER_PIN);
    } 
    else if (count_down == 0)
    {
      count_down = 33;
    }
    count_down--;
  }
  /* Cleat Interrupt Pending bit */
  TIM3_ClearITPendingBit(TIM3_IT_UPDATE);
}

INTERRUPT_HANDLER(EXTI_PORTD_IRQHandler, 6)
{
  if (GPIO_ReadInputPin(ECHO_PORT, ECHO_PIN) == SET)
  {
    // rising edge
    capture_start = counter;
  }
  else
  {
    // falling edge
    capture_stop = counter;
    distance = (capture_stop - capture_start) / 5.8;
  }
}