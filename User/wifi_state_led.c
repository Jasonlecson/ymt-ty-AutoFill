#define WIFI_STATE_LED_C_

#include "wifi_state_led.h"


/**
* @brief  wifi_state_led init
* @param  
* @param  None
* @param  None
* @retval None
* @example 
**/
void wifi_state_led_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(WIFI_STATE_LED_CLK, ENABLE);	 //Clock
	
	GPIO_InitStructure.GPIO_Pin = WIFI_STATE_LED_PIN;				 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(WIFI_STATE_LED_PORT, &GPIO_InitStructure);
}


/**
* @brief  wifi_state_led_set
* @param  value 1:ON; 0:OFF
* @param  None
* @param  None
* @retval None
* @example 
**/
void wifi_state_led_set(int value)
{
	switch(value)
	{
		case 1:
			GPIO_SetBits(WIFI_STATE_LED_PORT,WIFI_STATE_LED_PIN);
		break;
		case 0:
			GPIO_ResetBits(WIFI_STATE_LED_PORT,WIFI_STATE_LED_PIN);
		break;
		case -1:
			if(GPIO_ReadInputDataBit(WIFI_STATE_LED_PORT,WIFI_STATE_LED_PIN))
				GPIO_ResetBits(WIFI_STATE_LED_PORT,WIFI_STATE_LED_PIN);
			else
				GPIO_SetBits(WIFI_STATE_LED_PORT,WIFI_STATE_LED_PIN);
		break;
	}
}


/**
* @brief  wifi_state_led_get
* @param  None
* @param  None
* @param  None
* @retval value 1:ON; 0:OFF
* @example 
**/
char wifi_state_led_get(void)
{
	return GPIO_ReadInputDataBit(WIFI_STATE_LED_PORT,WIFI_STATE_LED_PIN);
}


