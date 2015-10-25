#include "srf05.h"
#include "delay.h"
#include "rs485.h"

#define TRIGGER_PORT     GPIOC
#define TRIGGER_PIN      GPIO_PIN_6
#define ECHO_PORT       GPIOC
#define ECHO_PIN        GPIO_PIN_7

//__IO unsigned int counter, count_down;
__IO unsigned int capture_time;
__IO float srf_distance = 0.0;
__IO unsigned char auto_poll = 0, man_conv_flag = 0, trigger_flag = 0;

int SRF05_Init(void)
{
  GPIO_Init(TRIGGER_PORT, TRIGGER_PIN, GPIO_MODE_OUT_PP_HIGH_FAST);
  GPIO_WriteLow(TRIGGER_PORT, TRIGGER_PIN);
  
  /* TIM3 configuration:
   - TIM3CLK is set to 16 MHz, the TIM3 Prescaler is equal to 16 so the TIM1 counter
   clock used is 16 MHz / 16 = 1 000 000 Hz
  - With 1 000 000 Hz we can generate time base:
      max time base is 65.536 ms if TIM3_PERIOD = 65535 --> (65535 + 1) / 1000000 = 65.536 ms
      min time base is 0.002 ms if TIM3_PERIOD = 1   --> (  1 + 1) / 1000000 = 0.002 ms = 2us
  - In this example we need to generate a time base equal to 30 us
   so TIM3_PERIOD = (0.03 * 1000000 - 1) = 29999 */

  /* Time base configuration */
  TIM3_TimeBaseInit(TIM3_PRESCALER_16, 29999);
  /* Clear TIM3 update flag */
  TIM3_ClearFlag(TIM3_FLAG_UPDATE);
  /* Enable update interrupt */
  TIM3_ITConfig(TIM3_IT_UPDATE, ENABLE);
  
  /* Enable TIM3 */
  TIM3_Cmd(ENABLE);
  
	// config external gpio interrupt
  disableInterrupts();
  EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOC, EXTI_SENSITIVITY_RISE_FALL);
  while (EXTI_GetExtIntSensitivity(EXTI_PORT_GPIOC) != EXTI_SENSITIVITY_RISE_FALL);
  GPIO_Init(ECHO_PORT, ECHO_PIN, GPIO_MODE_IN_FL_IT);
  /* enable interrupts */
  enableInterrupts();
  
  capture_time = 0;
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
	if (auto_poll == 1)
	{
	}
	else
	{
		man_conv_flag = 1;
		// manual trigger
		GPIO_WriteHigh(TRIGGER_PORT, TRIGGER_PIN);
		_delay_us(100);
		GPIO_WriteLow(TRIGGER_PORT, TRIGGER_PIN);
		while(man_conv_flag == 1);
	}
  return srf_distance;
}

void SRF05_ProcessTrigger(void)
{
	if (trigger_flag == 1)
	{
  	GPIO_WriteHigh(TRIGGER_PORT, TRIGGER_PIN);
		_delay_us(100);
  	GPIO_WriteLow(TRIGGER_PORT, TRIGGER_PIN);
		trigger_flag = 0;
	}
}

/**
  * @brief Timer3 Update/Overflow/Break Interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(TIM3_UPD_OVF_BRK_IRQHandler, 15)
{
	if (capture_time == 0)
	{
		srf_distance = (float)-1;
	}
	else
	{
		srf_distance = (float)capture_time / (float)58;
	}
	if (auto_poll == 1)
	{
		// active trigger for new calculation
		trigger_flag = 1;
	}
	else
	{
		man_conv_flag = 0;
	}
	
  /* Cleat Interrupt Pending bit */
  TIM3_ClearITPendingBit(TIM3_IT_UPDATE);
}

INTERRUPT_HANDLER(EXTI_PORTD_IRQHandler, 6)
{
  if (GPIO_ReadInputPin(ECHO_PORT, ECHO_PIN) == SET)
  {
    // rising edge
    TIM3_SetCounter(0);
  }
  else if (GPIO_ReadInputPin(ECHO_PORT, ECHO_PIN) == RESET)
  {
    // falling edge
		capture_time = TIM3_GetCounter();
  }
}

/**
  * @brief  External Interrupt PORTC Interrupt routine
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI_PORTC_IRQHandler, 5)
{
  if (GPIO_ReadInputPin(ECHO_PORT, ECHO_PIN) == SET)
  {
    // rising edge
    TIM3_SetCounter(0);
  }
  else if (GPIO_ReadInputPin(ECHO_PORT, ECHO_PIN) == RESET)
  {
    // falling edge
		capture_time = TIM3_GetCounter();
  }
}