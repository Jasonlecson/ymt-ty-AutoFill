
/**
  ******************************************************************************
  * @file    timer.c
  * @author  yang feng wu 
  * @version V1.0.0
  * @date    2019/10/13
  * @brief   ��ʱ��
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
int timer2Value1 = 0;//��������ָʾ����ҫ
int timer2Value2 = 0;//ÿ��1S��ʱ���ת��Ϊʱ��
int timer2Value3 = 0;//ÿ��1S��ȡһ������״̬
int timer2_watering=0;//���ƽ�ˮ
int timer2_DelSmartConfigFlag=0;//���SmartConfig���
u32 timer2_auto_watering =0;//�����Զ���ˮ
u32 timer2_event_appear=0;//�¼��ϱ�

int timer2_water_stage=0;//���Ʋɼ�ˮλ������ʱ��

int timer2_appear1=0;//�����ϱ�Ƶ��
int timer2_appear2=0;//�����ϱ�Ƶ��
int timer2_appear3=0;//�����ϱ�Ƶ��

int TimerDataValue1=0;//��ʱ��ȡʱ���
int TimestampAddCount = 0;//ÿ��1S�ۼ�ʱ���

char water_lack_count = 0;//��һ���ϵ��ⲻִ��
int timer2_water_lack_cnt=0;//���ȱˮ
int timer2_water_full_cnt=0;//���ˮ��
int timer2_buzzer_cnt=0;//���Ʒ���������ʱ��
int timer2_water_clear_flag_cnt=0;//��ʱ���ȱˮ��ˮ����־
int light_white_delay = 0;//�ƹ⽥��
int light_red_delay = 0;//�ƹ⽥��
/**
* @brief  ��ʱ��2����
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
	//��ʱ��2
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_Init(&NVIC_InitStructure);
	
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	/* Resets the TIM2 */
  TIM_DeInit(TIM2);
	//������ʱ�ӷָ
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	// ѡ���˼�����ģʽ��
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	//��ֵ
	TIM_TimeBaseStructure.TIM_Period = 10;//��ʱʱ��1ms��һ��
	//������������Ϊ TIMx ʱ��Ƶ�ʳ�����Ԥ��Ƶֵ��72M / 7199+1 = 0.01M
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
* @brief  ��ʱ��4����
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
	//������ʱ�ӷָ
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	// ѡ���˼�����ģʽ��
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	//��ֵ
	TIM_TimeBaseStructure.TIM_Period = 999;
	//������������Ϊ TIMx ʱ��Ƶ�ʳ�����Ԥ��Ƶֵ��72M / 35+1 = 1M
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
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);  //ʹ��GPIO����ʱ��ʹ��
																																		

	//���ø�����Ϊ�����������,���TIM1 CH1��PWM���岨��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9; //TIM4_CH3,TIM4_CH4
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);


	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 80K
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  ����Ƶ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM������ȵ���ģʽ2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_Pulse = 0; //���ô�װ�벶��ȽϼĴ���������ֵ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //�������:TIM����Ƚϼ��Ը�
	
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;//�رջ������
	
	
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);  //����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);
	TIM_CtrlPWMOutputs(TIM4,ENABLE);	//MOE �����ʹ��	

	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);  //CH3Ԥװ��ʹ��	 
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);  //CH4Ԥװ��ʹ��	 
	TIM_ARRPreloadConfig(TIM4, ENABLE); //ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ��� 

	TIM_Cmd(TIM4, ENABLE);  //ʹ��TIM1
}

//ÿ��1S�ۼ�ʱ���
void TimestampAddCountFun(void){
	if(timestamp!=0){//�Ѿ���ȡ��ʱ���
		TimestampAddCount++;
		if(TimestampAddCount>=1000){
			TimestampAddCount=0;
			timestamp++;
		}
	}
}

//10S�Ժ����SmartConfig��־
void DeleteSmartConfigStartFlagFun(void){
	if(SmartConfigStartFlag){
		timer2_DelSmartConfigFlag++;
		if(timer2_DelSmartConfigFlag>10000){
			timer2_DelSmartConfigFlag=0;
			SmartConfigStartFlag=0;
		}
	}
}

//���Ʒ������ε�
void ControlBuzzer(void){
	//ȱˮ
	if(sensor_control_struct_value.water_stage_value == 0 && sensor_control_struct_value.water_lack_flag==0){
		timer2_water_lack_cnt++;
		if(timer2_water_lack_cnt>3000){
			timer2_water_lack_cnt=0;
			sensor_control_struct_value.water_full_flag=0;//ˮ����־
			sensor_control_struct_value.water_lack_flag=1;//ȱˮ��־
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
	
	//ˮ��
	if(sensor_control_struct_value.water_stage_value == 100 && sensor_control_struct_value.water_full_flag==0){
		timer2_water_full_cnt++;
		if(timer2_water_full_cnt>=3000){
			timer2_water_full_cnt=0;
			sensor_control_struct_value.water_full_flag=1;//ˮ����־
			sensor_control_struct_value.water_lack_flag=0;//ȱˮ��־
			timer2_buzzer_cnt=0;
			sensor_buzzer_set(0);
		}
	}
	else{
		timer2_water_full_cnt=0;
	}
	
	
	//ȱˮ,���Ʒ�������,������
	if(sensor_control_struct_value.water_lack_flag==1){
		if(timer2_buzzer_cnt<1500){
			timer2_buzzer_cnt++;
			if(timer2_buzzer_cnt>0 && timer2_buzzer_cnt<500 && water_lack_count==2){
				sensor_buzzer_set(1);//���Ʒ�����
			}
			else if(timer2_buzzer_cnt>500 && timer2_buzzer_cnt<1000 && water_lack_count==2){
				sensor_buzzer_set(0);//���Ʒ�����
			}
			else if(timer2_buzzer_cnt>1000 && timer2_buzzer_cnt<1500 && water_lack_count==2){
				sensor_buzzer_set(1);//���Ʒ�����
			}
		}
		else{
			sensor_buzzer_set(0);//���Ʒ�����
		}
	}
	
	
	//ˮ��,���Ʒ���������һ��ʱ��
	if(sensor_control_struct_value.water_full_flag==1){
		if(timer2_buzzer_cnt<3000){
			timer2_buzzer_cnt++;
			sensor_buzzer_set(1);//���Ʒ�����
		}
		else{
			sensor_buzzer_set(0);//���Ʒ�����
		}
	}
	
	
	//��⵽����ˮ�ֲ���ˮ��,����һ��ʱ��,���ȱˮ��ˮ����־
	if(sensor_control_struct_value.water_stage_value >0 && sensor_control_struct_value.water_stage_value<100){
		//�б�־
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
* @brief  ��ʱ��2�жϺ���
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
		timer2Value2++;//ÿ��1S��ʱ���ת��Ϊʱ��
		timer2Value3++;//ÿ��1S��ȡһ������״̬
		timer2_auto_watering++;//�����Զ���ˮ
		timer2_appear1++;
		timer2_appear2++;
		timer2_appear3++;
		timer2_event_appear++;//�¼��ϱ�
		timer2_watering++;
		timer2_water_stage++;//���Ʋɼ�ˮλ������ʱ��
		TimerDataValue1++;//��ʱ��ȡʱ���
		light_red_delay++;
		light_white_delay++;
		ConfigModuleNoBlockTimerCnt++;
		//ÿ��1S�ۼ�ʱ���
		TimestampAddCountFun();
		
		KeyWhile();
		KeyLoop();
		//�¼��ɼ�����(����1ms��ʱ��)
		event_loop_fun();
		
		TimerLoopFun(10);
		
		if(wifi_state == WIFI_CONN_CLOUD){
			//���Ʒ������ε�
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
		
		//10S�Ժ����SmartConfig��־
		DeleteSmartConfigStartFlagFun();
		
		sensor_control_struct_value.illumination_statistics_timer_delay++;
		
		sensor_control_struct_value.illumination_statistics_timer_delay1++;
	}
}


















