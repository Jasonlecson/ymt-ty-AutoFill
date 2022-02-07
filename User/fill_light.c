#define FILL_LIGHT_C_

#include "fill_light.h"
#include "sensor_control.h"
#include "FlashData.h"
extern sensor_control_struct sensor_control_struct_value;
void setled(unsigned char color, unsigned char on, unsigned long bright);
extern void TIM4_PWM_Init(u16 arr,u16 psc);
extern void Timer4_Config(void);
extern int light_white_delay;
extern int light_red_delay;
_Bool time_flag;
uint32_t timer_copy = 1000;
/**
* @brief  fill_light init
* @param  
* @param  None
* @param  None
* @retval None
* @example 
**/
void fill_light_init(void)
{
	Timer4_Config();
	TIM4_PWM_Init(999,71);
	sensor_control_struct_value.color_selection = 0; //0：白光 1：白光+红光 2：红光
	sensor_control_struct_value.color_selection_copy = 0;
	sensor_control_struct_value.brightness = 100; //亮度
	sensor_control_struct_value.brightness_copy = 100;
	sensor_control_struct_value.fill_light_on = 1; //0：关灯 1：开灯
	sensor_control_struct_value.fill_light_on_copy = 1;
/**************************************************************************/
	sensor_control_struct_value.control_mode = 1; //0：自动调光 1：手动调光
	sensor_control_struct_value.control_mode_copy = 1;
	sensor_control_struct_value.habit = 0; //0:喜阳 1：喜阴
	sensor_control_struct_value.habit_copy = 0;
	sensor_control_struct_value.openmode = 1; //0:自动 1：手动
	sensor_control_struct_value.openmode_copy = 1;
	sensor_control_struct_value.time_selection = 0; //0：常亮 1：8H 2：12H 3：16H
	sensor_control_struct_value.time_selection_copy = 0;
	sensor_control_struct_value.brightness = FlashGetFillLightBright();
	sensor_control_struct_value.control_mode = FlashGetFillLightCtrlMode();
	sensor_control_struct_value.openmode = FlashGetFillLightMode();
	sensor_control_struct_value.fill_light_on = FlashGetFillLightOn();
	sensor_control_struct_value.color_selection = FlashGetFillLightColor();
	sensor_control_struct_value.time_selection = FlashGetFillLightTimer();
	sensor_control_struct_value.habit = FlashGetFillLightHabit();
	sensor_control_struct_value.on = FlashGetSwitch();
}


/**
* @brief  fill_light_set
* @param  value 1:ON; 0:OFF
* @param  None
* @param  None
* @retval None
* @example 
**/
void fill_light_set(void)
{
	static int illumination = 0;
	static int bright = 0;
	illumination = sensor_control_struct_value.illumination_value;
																			/*手动调光*/
	if(sensor_control_struct_value.control_mode == 1)
	{
		setled(sensor_control_struct_value.color_selection,sensor_control_struct_value.fill_light_on,sensor_control_struct_value.brightness);
	}
																				/*自动调光*/
	if(sensor_control_struct_value.control_mode == 0)
	{
		if(sensor_control_struct_value.habit == 0)
		{
			bright = (5000-illumination)/50;
			if(bright <= 0)
			{
				bright = 0;
			}
			setled(sensor_control_struct_value.color_selection,sensor_control_struct_value.fill_light_on,bright);
			sensor_control_struct_value.brightness = bright;
		}
		else if(sensor_control_struct_value.habit == 1)
		{
			bright = (1000-illumination)/10;
			if(bright <= 0)
			{
				bright = 0;
			}
			setled(sensor_control_struct_value.color_selection,sensor_control_struct_value.fill_light_on,bright);
			sensor_control_struct_value.brightness = bright;
		}
	}
																			/*自动开关灯*/
	if(sensor_control_struct_value.openmode == 0)
	{
		if(illumination <= sensor_control_struct_value.safe_illumination[0])
		{
			sensor_control_struct_value.fill_light_on = 1;
		}
		else
		{
			sensor_control_struct_value.fill_light_on = 0;
		}
	}
	if(sensor_control_struct_value.time_selection == 1 && time_flag == 1)
	{
		timer_copy = timestamp+28800;
		time_flag = 0;
	}
	else if(sensor_control_struct_value.time_selection == 2 && time_flag == 1)
	{
		timer_copy = timestamp+43200;
		time_flag = 0;
	}
	else if(sensor_control_struct_value.time_selection == 3 && time_flag == 1)
	{
		timer_copy = timestamp+57600;
		time_flag = 0;
	}
	if(timer_copy == timestamp)
	{
		sensor_control_struct_value.fill_light_on = 0;
	}
}

void setled(unsigned char color, unsigned char on, unsigned long bright)
{
	static int bright_red_copy = 0;
	static int bright_red_set = 0;
	static int bright_white_copy = 0;
	static int bright_white_set = 0;

	if(color == 0)
	{
		bright_white_set = bright*on;
		bright_red_set = 0;
	}
	else if(color == 1)
	{
		bright_white_set = bright*on*0.5;
		bright_red_set = bright*on*0.5;
	}
	else if(color == 2)
	{
		bright_white_set = 0;
		bright_red_set = bright*on;
	}
		if(light_white_delay>5)
		{
			if(bright_white_set > bright_white_copy)
			{
			bright_white_copy++;
			}
			else if(bright_white_set < bright_white_copy)
			{
			bright_white_copy--;
			}
			light_white_delay = 0;
		}
		
		if(light_red_delay>5)
		{
			if(bright_red_set > bright_red_copy)
			{
			bright_red_copy++;
			}
			else if(bright_red_set < bright_red_copy)
			{
			bright_red_copy--;
			}
			light_red_delay = 0;
		}

		
		TIM_SetCompare3(TIM4,(int)(bright_white_copy*9.99));
		TIM_SetCompare4(TIM4,(int)(bright_red_copy*9.99));
}

