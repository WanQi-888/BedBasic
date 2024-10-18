#include "stm32f10x.h"                  // Device header

//extern uint16_t Num;//使用main.c里的变量

//第一步：打开时钟RCC
//第二步：选择时基单元的时钟源，内置or外置
//第三步：配置时基单元，PSC,CNT,ARR
//第四步：配置输出中断控制，允许更新中断输出到NVIC
//第五步：配置NVIC，分配优先级
//第六步：开启定时器
void Timer_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	TIM_InternalClockConfig(TIM2);//使用内部72mhz时钟
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;//滤波器的滤波频率，由内部时钟分频而来
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;//向上计数
	TIM_TimeBaseInitStruct.TIM_Period = 750 - 1;//ARR
	TIM_TimeBaseInitStruct.TIM_Prescaler = 7200 - 1;//PSC
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;//高级计数器才有的
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);//立即生成一个更新事件
	
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);//消除更新标志位
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);//更新中断，这样就可到NVIC了
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_Cmd(TIM2, ENABLE);
}

/*
void TIM2_IRQHandler(void)
{
	if( TIM_GetITStatus(TIM2, TIM_IT_Update) == SET )
	{
		
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}
*/
