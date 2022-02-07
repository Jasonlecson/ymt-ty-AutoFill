
/**
  ******************************************************************************
  * @file    timer.c
  * @author  yang feng wu 
  * @version V1.0.0
  * @date    2019/10/13
  * @brief   定时器
  ******************************************************************************
  ******************************************************************************
  */


#define _TIME_C_

#include "timer.h"
#include "main.h"
#include "key.h"
#include "usart.h"
#include "ConfigModuleNoBlock.h"
#include "sensor_control.h"
#include "wifi.h"
void TIM4_PWM_Init(u16 arr,u16 psc);
void Timer4_Config(void);
int timer2Value=0;
int timer2Value1 = 0;//控制配网指示灯闪耀
int timer2Value2 = 0;//每隔1S把时间戳转换为时间
int timer2Value3 = 0;//每隔1S获取一次网络状态
int timer2_watering=0;//控制浇水
int timer2_DelSmartConfigFlag=0;//清除SmartConfig标记
u32 timer2_auto_watering =0;//控制自动浇水
u32 timer2_event_appear=0;//事件上报

int timer2_water_stage=0;//控制采集水位传感器时间

int timer2_appear1=0;//控制上报频率
int timer2_appear2=0;//控制上报频率
int timer2_appear3=0;//控制上报频率

int TimerDataValue1=0;//定时获取时间戳
int TimestampAddCount = 0;//每隔1S累加时间戳

char water_lack_count = 0;//第一次上电检测不执行
int timer2_water_lack_cnt=0;//检测缺水
int timer2_water_full_cnt=0;//检测水满
int timer2_buzzer_cnt=0;//控制蜂鸣器蜂鸣时间
int timer2_water_clear_flag_cnt=0;//超时清除缺水和水满标志
int light_white_delay = 0;//灯光渐变
int light_red_delay = 0;//灯光渐变
/**
* @brief  定时器2配置
* @param  None
* @param  None
* @param  None
* @retval None
* @example 
**/
void Timer2Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;	
	//定时器2
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_Init(&NVIC_InitStructure);
	
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	/* Resets the TIM2 */
  TIM_DeInit(TIM2);
	//设置了时钟分割。
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	// 选择了计数器模式。
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	//初值
	TIM_TimeBaseStructure.TIM_Period = 10;//定时时间1ms进一次
	//设置了用来作为 TIMx 时钟频率除数的预分频值。72M / 7199+1 = 0.01M
	TIM_TimeBaseStructure.TIM_Prescaler = 7199;
	
	//TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	/* Enables the TIM2 counter */
  TIM_Cmd(TIM2, ENABLE);
	
	/* Enables the TIM2 Capture Compare channel 1 Interrupt source */
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE );
}

/**
* @brief  定时器4配置
* @param  None
* @param  None
* @param  None
* @retval None
* @example 
**/

void Timer4_Config(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	/* Resets the TIM4 */
  TIM_DeInit(TIM4);
	//设置了时钟分割。
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	// 选择了计数器模式。
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	//初值
	TIM_TimeBaseStructure.TIM_Period = 999;
	//设置了用来作为 TIMx 时钟频率除数的预分频值。72M / 35+1 = 1M
	TIM_TimeBaseStructure.TIM_Prescaler = 71;
	
	//TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	/* Enables the TIM2 counter */
  TIM_Cmd(TIM4, ENABLE);

}

void TIM4_PWM_Init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);// 
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);  //使能GPIO外设时钟使能
																																		

	//设置该引脚为复用输出功能,输出TIM1 CH1的PWM脉冲波形
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9; //TIM4_CH3,TIM4_CH4
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);


	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 80K
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值  不分频
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_Pulse = 0; //设置待装入捕获比较寄存器的脉冲值
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //输出极性:TIM输出比较极性高
	
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;//关闭互补输出
	
	
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);
	TIM_CtrlPWMOutputs(TIM4,ENABLE);	//MOE 主输出使能	

	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);  //CH3预装载使能	 
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);  //CH4预装载使能	 
	TIM_ARRPreloadConfig(TIM4, ENABLE); //使能TIMx在ARR上的预装载寄存器 

	TIM_Cmd(TIM4, ENABLE);  //使能TIM1
}

//每隔1S累加时间戳
void TimestampAddCountFun(void){
	if(timestamp!=0){//已经获取过时间戳
		TimestampAddCount++;
		if(TimestampAddCount>=1000){
			TimestampAddCount=0;
			timestamp++;
		}
	}
}

//10S以后清除SmartConfig标志
void DeleteSmartConfigStartFlagFun(void){
	if(SmartConfigStartFlag){
		timer2_DelSmartConfigFlag++;
		if(timer2_DelSmartConfigFlag>10000){
			timer2_DelSmartConfigFlag=0;
			SmartConfigStartFlag=0;
		}
	}
}

//控制蜂鸣器滴滴
void ControlBuzzer(void){
	//缺水
	if(sensor_control_struct_value.water_stage_value == 0 && sensor_control_struct_value.water_lack_flag==0){
		timer2_water_lack_cnt++;
		if(timer2_water_lack_cnt>3000){
			timer2_water_lack_cnt=0;
			sensor_control_struct_value.water_full_flag=0;//水满标志
			sensor_control_struct_value.water_lack_flag=1;//缺水标志
			timer2_buzzer_cnt=0;
			sensor_buzzer_set(0);
			water_lack_count++;
			if(water_lack_count>=2){water_lack_count=2;}
		}
	}
	else
	{
		timer2_water_lack_cnt=0;
	}
	
	//水满
	if(sensor_control_struct_value.water_stage_value == 100 && sensor_control_struct_value.water_full_flag==0){
		timer2_water_full_cnt++;
		if(timer2_water_full_cnt>=3000){
			timer2_water_full_cnt=0;
			sensor_control_struct_value.water_full_flag=1;//水满标志
			sensor_control_struct_value.water_lack_flag=0;//缺水标志
			timer2_buzzer_cnt=0;
			sensor_buzzer_set(0);
		}
	}
	else{
		timer2_water_full_cnt=0;
	}
	
	
	//缺水,控制蜂鸣器滴,滴两声
	if(sensor_control_struct_value.water_lack_flag==1){
		if(timer2_buzzer_cnt<1500){
			timer2_buzzer_cnt++;
			if(timer2_buzzer_cnt>0 && timer2_buzzer_cnt<500 && water_lack_count==2){
				sensor_buzzer_set(1);//控制蜂鸣器
			}
			else if(timer2_buzzer_cnt>500 && timer2_buzzer_cnt<1000 && water_lack_count==2){
				sensor_buzzer_set(0);//控制蜂鸣器
			}
			else if(timer2_buzzer_cnt>1000 && timer2_buzzer_cnt<1500 && water_lack_count==2){
				sensor_buzzer_set(1);//控制蜂鸣器
			}
		}
		else{
			sensor_buzzer_set(0);//控制蜂鸣器
		}
	}
	
	
	//水满,控制蜂鸣器长鸣一段时间
	if(sensor_control_struct_value.water_full_flag==1){
		if(timer2_buzzer_cnt<3000){
			timer2_buzzer_cnt++;
			sensor_buzzer_set(1);//控制蜂鸣器
		}
		else{
			sensor_buzzer_set(0);//控制蜂鸣器
		}
	}
	
	
	//检测到即有水又不是水满,超过一定时间,清除缺水和水满标志
	if(sensor_control_struct_value.water_stage_value >0 && sensor_control_struct_value.water_stage_value<100){
		//有标志
		if(sensor_control_struct_value.water_lack_flag==1 || sensor_control_struct_value.water_full_flag==1){
			timer2_water_clear_flag_cnt++;
			if(timer2_water_clear_flag_cnt>TIMER_COMPARE_3MIN){
				timer2_water_clear_flag_cnt=0;
				sensor_control_struct_value.water_lack_flag = 0;
				sensor_control_struct_value.water_full_flag = 0;
			}
		}
	}
}

/**
* @brief  定时器2中断函数
* @param  None
* @param  None
* @param  None
* @retval None
* @example 
**/
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		
		timer2Value++;
		timer2Value1++;
		timer2Value2++;//每隔1S把时间戳转换为时间
		timer2Value3++;//每隔1S获取一次网络状态
		timer2_auto_watering++;//控制自动浇水
		timer2_appear1++;
		timer2_appear2++;
		timer2_appear3++;
		timer2_event_appear++;//事件上报
		timer2_watering++;
		timer2_water_stage++;//控制采集水位传感器时间
		TimerDataValue1++;//定时获取时间戳
		light_red_delay++;
		light_white_delay++;
		ConfigModuleNoBlockTimerCnt++;
		//每隔1S累加时间戳
		TimestampAddCountFun();
		
		KeyWhile();
		KeyLoop();
		//事件采集函数(放在1ms定时器)
		event_loop_fun();
		
		TimerLoopFun(10);
		
		if(wifi_state == WIFI_CONN_CLOUD){
			//控制蜂鸣器滴滴
			ControlBuzzer();
		}
		
		if(mcu_updata_flag){
			mcu_updata_flag_clear_cnt++;
			if(mcu_updata_flag_clear_cnt>TIMER_COMPARE_60SEC){
				mcu_updata_flag_clear_cnt=0;
				mcu_updata_flag=0;
			}
		}
		else{
			mcu_updata_flag_clear_cnt=0;
		}
		
		Usart2IdleLoop(50);
		
		//10S以后清除SmartConfig标志
		DeleteSmartConfigStartFlagFun();
		
		sensor_control_struct_value.illumination_statistics_timer_delay++;
		
		sensor_control_struct_value.illumination_statistics_timer_delay1++;
	}
}



















