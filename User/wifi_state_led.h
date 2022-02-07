#ifndef WIFI_STATE_LED_Cc_
#define WIFI_STATE_LED_Cc_

#ifndef WIFI_STATE_LED_C_//如果没有定义
#define WIFI_STATE_LED_Cx_ extern
#else
#define WIFI_STATE_LED_Cx_
#endif

#include "stm32f10x.h"

/*config pin*/
#define WIFI_STATE_LED_PIN    GPIO_Pin_5
#define WIFI_STATE_LED_PORT   GPIOA
#define WIFI_STATE_LED_CLK    RCC_APB2Periph_GPIOA


void wifi_state_led_init(void);
void wifi_state_led_set(int value);
char wifi_state_led_get(void);

#endif
